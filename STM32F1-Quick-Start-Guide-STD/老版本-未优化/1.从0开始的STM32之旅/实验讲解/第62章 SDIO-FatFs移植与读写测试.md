# 第六十二章 SDIO-FatFs移植与读写测试

## 1. FatFs移植步骤

上一章我们已经完成了SD卡驱动程序以及进行了简单的读写测试。该工程有很多东西是现在可以使用的，所以我们先把上一章的工程文件完整的拷贝一份， 并修改文件夹名为“SDIO-FatFs移植与读写测试”，如果此时使用KEIL软件打开该工程，应该是编译无错误并实现上一章的测试功能。

接下来，我们到串行Flash文件系统移植工程文件的“SPI-FatFs移植与读写测试User”文件夹下拷贝“FATFS” 整个文件夹到现在工程文件的“SDIO-FatFs移植与读写测试User”文件夹下，如图 拷贝FatFs文件夹。 该文件夹是FatFs文件系统的所有代码文件，在串行Flash移植FatFs文件系统时我们对部分文件做了修改，这里主要是想要保留之前的配置， 而不是使用FatFs官方源码还需要重新配置。

现在就可以使用KEIL软件打开“SDIO-FatFs移植与读写测试”工程文件，并把FatFs相关文件添加到工程内， 同时把sdio_test.c文件移除。

![屏幕截图 2025-03-10 183429.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/03/10-18-34-49-屏幕截图%202025-03-10%20183429.png)

## 2. FatFs接口函数

FatFs文件系统与存储设备的连接函数在diskio.c文件中，主要有5个函数需要我们编写的

### 2.1 宏定义和存储设备状态获取函数

```c
//宏定义
#define ATA        0     // SD卡
#define SPI_FLASH  1     // 预留外部SPI Flash使用
// SD卡块大小
#define SD_BLOCKSIZE     512

//存储设备状态获取
DSTATUS disk_status (
    BYTE pdrv   /* 物理编号 */
)
{
    DSTATUS status = STA_NOINIT;
    switch (pdrv) {
    case ATA: /* SD CARD */
        status &= ~STA_NOINIT;
        break;

    case SPI_FLASH:        /* SPI Flash */
        break;

    default:
        status = STA_NOINIT;
    }
    return status;
}
```

FatFs支持同时挂载多个存储设备，通过定义为不同编号以区别。SD卡一般定义为编号0，编号1预留给串行Flash芯片使用。使用宏定义方式给出SD卡块大小， 方便修改。实际上，SD卡块大小一般都是设置为512字节的，不管是标准SD卡还是高容量SD卡。

disk_status函数要求返回存储设备的当前状态，对于SD卡一般返回SD卡插入状态，这里直接返回正常状态。

### 2.2 存储设备初始化函数

```c
DSTATUS disk_initialize (
    BYTE pdrv       /* 物理编号 */
)
{
    DSTATUS status = STA_NOINIT;
    switch (pdrv) {
    case ATA:          /* SD CARD */
        if (SD_Init()==SD_OK) {
            status &= ~STA_NOINIT;
        } else {
            status = STA_NOINIT;
        }

        break;

    case SPI_FLASH:    /* SPI Flash */
        break;

    default:
        status = STA_NOINIT;
    }
    return status;
}
```

该函数用于初始化存储设备，一般包括相关GPIO初始化、外设环境初始化、中断配置等等。对于SD卡，直接调用SD_Init函数实现对SD卡初始化， 如果函数返回SD_OK说明SD卡正确插入，并且控制器可以与之正常通信。

### 2.3 存储设备数据读取函数

```c
DRESULT disk_read (
    BYTE pdrv,    /* 设备物理编号(0..) */
    BYTE *buff,   /* 数据缓存区 */
    DWORD sector, /* 扇区首地址 */
    UINT count    /* 扇区个数(1..128) */
)
{
    DRESULT status = RES_PARERR;
    SD_Error SD_state = SD_OK;

    switch (pdrv) {
    case ATA: /* SD CARD */
        if ((DWORD)buff&3) {
            DRESULT res = RES_OK;
            DWORD scratch[SD_BLOCKSIZE / 4];
            while (count--) {
                res = disk_read(ATA,(void *)scratch, sector++, 1);
                if (res != RES_OK) {
                    break;
                }
                memcpy(buff, scratch, SD_BLOCKSIZE);
                buff += SD_BLOCKSIZE;
            }
            return res;
        }

        SD_state=SD_ReadMultiBlocks(buff,sector *SD_BLOCKSIZE,SD_BLOCKSIZE,count);
        if (SD_state==SD_OK) {
            /* Check if the Transfer is finished */
            SD_state=SD_WaitReadOperation();
            while (SD_GetStatus() != SD_TRANSFER_OK);
        }
        if (SD_state!=SD_OK)
            status = RES_PARERR;
        else
            status = RES_OK;
        break;

    case SPI_FLASH:
        break;

    default:
        status = RES_PARERR;
    }
    return status;
}
```

disk_read函数用于从存储设备指定地址开始读取一定的数量的数据到指定存储区内。对于SD卡，最重要是使用SD_ReadMultiBlocks函数读取多块数据到存储区。 这里需要注意的地方是SD卡数据操作是使用DMA传输的，并设置数据尺寸为32位大小，为实现数据正确传输，要求存储区是4字节对齐。在某些情况下， FatFs提供的buff地址不是4字节对齐，这会导致DMA数据传输失败，所以为保证数据传输正确，可以先判断存储区地址是否是4字节对齐， 如果存储区地址已经是4字节对齐，无需其他处理，直接使用SD_ReadMultiBlocks函数执行多块读取即可。如果判断得到地址不是4字节对齐， 则先申请一个4字节对齐的临时缓冲区，即局部数组变量scratch，通过定义为DWORD类型可以使得其自动4字节对齐，scratch所占的总存储空间也是一个块大小， 这样把一个块数据读取到scratch内，然后把scratch存储器内容拷贝到buff地址空间上就可以了。

SD_ReadMultiBlocks函数用于从SD卡内读取多个块数据，它有四个形参，分别为存储区地址指针、起始块地址、块大小以及块数量。为保证数据传输完整， 还需要调用SD_WaitReadOperation函数和SD_GetStatus函数检测和保证传输完成。

### 2.4 存储设备数据写入函数

```c
#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,        /* 设备物理编号(0..) */
    const BYTE *buff, /* 欲写入数据的缓存区 */
    DWORD sector,     /* 扇区首地址 */
    UINT count        /* 扇区个数(1..128) */
)
{
    DRESULT status = RES_PARERR;
    SD_Error SD_state = SD_OK;

    if (!count) {
        return RES_PARERR;    /* Check parameter */
    }

    switch (pdrv) {
    case ATA: /* SD CARD */
        if ((DWORD)buff&3) {
            DRESULT res = RES_OK;
            DWORD scratch[SD_BLOCKSIZE / 4];

            while (count--) {
                memcpy( scratch,buff,SD_BLOCKSIZE);
                res = disk_write(ATA,(void *)scratch, sector++, 1);
                if (res != RES_OK) {
                    break;
                }
                buff += SD_BLOCKSIZE;
            }
            return res;
        }

        SD_state=SD_WriteMultiBlocks((uint8_t *)buff,sector*SD_BLOCKSIZE,
                                                SD_BLOCKSIZE,count);
        if (SD_state==SD_OK) {
            /* Check if the Transfer is finished */
            SD_state=SD_WaitReadOperation();

            /* Wait until end of DMA transfer */
            while (SD_GetStatus() != SD_TRANSFER_OK);
        }
        if (SD_state!=SD_OK)
            status = RES_PARERR;
        else
            status = RES_OK;
        break;

    case SPI_FLASH:
        break;

    default:
        status = RES_PARERR;
    }
    return status;
}
#endif
```

disk_write函数用于向存储设备指定地址写入指定数量的数据。对于SD卡，执行过程与disk_read函数是非常相似，也必须先检测存储区地址是否是4字节对齐， 如果是4字节对齐则直接调用SD_WriteMultiBlocks函数完成多块数据写入操作。如果不是4字节对齐，申请一个4字节对齐的临时缓冲区， 先把待写入的数据拷贝到该临时缓冲区内，然后才写入到SD卡。

SD_WriteMultiBlocks函数是向SD卡写入多个块数据，它有四个形参，分别为存储区地址指针、起始块地址、块大小以及块数量， 它与SD_ReadMultiBlocks函数执行相互过程。最后也是需要使用相关函数保存数据写入完整才退出disk_write函数。

### 2.5 其他控制函数

```c
#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,    /* 物理编号 */
    BYTE cmd,     /* 控制指令 */
    void *buff    /* 写入或者读取数据地址指针 */
)
{
    DRESULT status = RES_PARERR;
    switch (pdrv) {
    case ATA: /* SD CARD */
        switch (cmd) {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE :
            *(WORD * )buff = SD_BLOCKSIZE;
            break;
        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE :
            *(DWORD * )buff = SDCardInfo.CardBlockSize;
            break;

        case GET_SECTOR_COUNT:
    *(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
            break;
        case CTRL_SYNC :
            break;
        }
        status = RES_OK;
        break;

    case SPI_FLASH:
        break;

    default:
        status = RES_PARERR;
    }
    return status;
}
#endif
```

disk_ioctl函数有三个形参，pdrv为设备物理编号，cmd为控制指令，包括发出同步信号、获取扇区数目、获取扇区大小、获取擦除块数量等等指令， buff为指令对应的数据指针。

对于SD卡，为支持格式化功能，需要用到获取扇区数量(GET_SECTOR_COUNT)指令和获取块尺寸(GET_BLOCK_SIZE)。另外，SD卡扇区大小为512字节， 串行Flash芯片一般设置扇区大小为4096字节，所以需要用到获取扇区大小(GET_SECTOR_SIZE)指令。

至此，基于SD卡的FatFs文件系统移植就已经完成了，最重要就是diskio.c文件中5个函数的编写。 接下来就编写FatFs基本的文件操作检测移植代码是否可以正确执行。

## 3. FatFs功能测试

主要的测试包括格式化测试、文件写入测试和文件读取测试三个部分，主要程序都在main.c文件中实现。

### 3.1 变量定义

```c
FATFS fs;                    /* FatFs文件系统对象 */
FIL fnew;                    /* 文件对象 */
FRESULT res_sd;             /* 文件操作结果 */
UINT fnum;                    /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};  /* 读缓冲区 */
BYTE WriteBuffer[] =        /* 写缓冲区*/
"SDIO-SD移植FatFs读写测试文件，新建文件系统测试文件\r\n";  
extern SD_CardInfo SDCardInfo; // SD卡信息结构体
```

FATFS是在ff.h文件定义的一个结构体类型，针对的对象是物理设备，包含了物理设备的物理编号、扇区大小等等信息， 一般我们都需要为每个物理设备定义一个FATFS变量。

FIL也是在ff.h文件定义的一个结构体类型，针对的对象是文件系统内具体的文件，包含了文件很多基本属性，比如文件大小、 路径、当前读写地址等等。如果需要在同一时间打开多个文件进行读写，才需要定义多个FIL变量，不然一般定义一个FIL变量即可。

FRESULT是也在ff.h文件定义的一个枚举类型，作为FatFs函数的返回值类型，主要管理FatFs运行中出现的错误。 总共有19种错误类型，包括物理设备读写错误、找不到文件、没有挂载工作空间等等错误。这在实际编程中非常重要， 当有错误出现是我们要停止文件读写，通过返回值我们可以快速定位到错误发生的可能地点。如果运行没有错误才返回FR_OK。

fnum是个32位无符号整形变量，用来记录实际读取或者写入数据的数组。

buffer和textFileBuffer分别对应读取和写入数据缓存区，都是8位无符号整形数组。

### 3.2 主函数

```c
#include "stm32f10x.h"
#include "sdio_sdcard.h"
#include "usart.h"    
#include "led.h"
#include "ff.h"

FATFS fs;                    /* FatFs文件系统对象 */
FIL fnew;                    /* 文件对象 */
FRESULT res_sd;             /* 文件操作结果 */
UINT fnum;                    /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};  /* 读缓冲区 */
BYTE WriteBuffer[] =        /* 写缓冲区*/
"SDIO-SD移植FatFs读写测试文件，新建文件系统测试文件\r\n";  
extern SD_CardInfo SDCardInfo; // SD卡信息结构体

int main(void)
{
    LED_Init();    
    LED_BLUE();    
    USART_Config();    
    // 在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    res_sd = f_mount(&fs,"0:",1);
    printf("SD卡初始化容量=%lld",SDCardInfo.CardCapacity/1024/1024);

/*----------------------- 格式化测试 ---------------------------*/  
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if(res_sd == FR_NO_FILESYSTEM)
    {
        printf("检测到SD卡还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        res_sd = f_mkfs("0:",0,0);                            
        if(res_sd == FR_OK)
        {
            printf("SD卡已成功格式化文件系统。\r\n");
              /* 格式化后，先取消挂载 */
            res_sd = f_mount(NULL,"0:",1);            
              /* 重新挂载    */            
            res_sd = f_mount(&fs,"0:",1);
        }
        else
        {
            LED_RED();
            printf("《《格式化失败。》》\r\n");
            while(1);
        }
    }
  else if(res_sd!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
    while(1);
  }
  else
  {
    printf("文件系统挂载成功，可以进行读写测试\r\n");
  }

/*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("\r\n****** 即将进行文件写入测试... ******\r\n");    
    res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
    if ( res_sd == FR_OK )
    {
        printf("打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
        if(res_sd==FR_OK)
        {
              printf("文件写入成功，写入字节数据：%d\n",fnum);
              printf("向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res_sd);
    }    
        /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {    
        LED_RED();
        printf("！！打开/创建文件失败。\r\n");
    }

/*------------------- 文件系统测试：读测试 ------------------------------------*/
    printf("****** 即将进行文件读取测试... ******\r\n");
    res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ);      
    if(res_sd == FR_OK)
    {
        LED_GREEN();
        printf("》打开文件成功。\r\n");
        res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);    
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",res_sd);
    }        
    }
    else
    {
        LED_RED();
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&fnew);    
    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL,"0:",1);
      /* 操作完成，停机 */
    while(1)
    {
    }
}
```
