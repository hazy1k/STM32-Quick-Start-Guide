# 第二十三章 FatFs文件系统移植实验

## 1. FatFs程序结构图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF006.png)

用户应用程序需要由用户编写，想实现什么功能就编写什么的程序，一般我们只用到f_mount()、f_open()、f_write()、f_read()就可以实现文件的读写操作。

FatFs组件是FatFs的主体，文件都在源码src文件夹中，其中ff.c、ff.h、integer.h以及diskio.h四个文件我们不需要改动，只需要修改ffconf.h和diskio.c两个文件。

底层设备输入输出要求实现存储设备的读写操作函数、存储设备信息获取函数等等。我们使用SPI Flash芯片作为物理设备， 在上一章节已经编写好了SPI Flash芯片的驱动程序，这里我们就直接使用。

## 2. 硬件设计

FatFs属于软件组件，不需要附带其他硬件电路。我们使用SPI Flash芯片作为物理存储设备，其硬件电路在上一章已经做了分析，这里就直接使用。

## 3. FatFs移植步骤

上一章我们已经实现了SPI Flash芯片驱动程序，并实现了读写测试， 为移植FatFs方便，我们直接拷贝一份工程， 我们在工程基础上添加FatFs组件，并修改main函数的用户程序即可。

1) 先拷贝一份SPI Flash芯片测试的工程文件(整个文件夹)，并修改文件夹名为“SPI-FatFs文件系统”。 将FatFs源码中的src文件夹整个文件夹拷贝一份至“SPI—FatFs文件系统USER”文件夹下并修改名为“FATFS”， 见图：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF007.png)

2) 使用KEIL软件打开工程文件(..SPI—-atFs文件系统ProjectRVMDK(uv5)\ BH-F103.uvprojx)， 并将FatFs组件文件添加到工程中， 需要添加有ff.c、diskio.c和cc936.c三个文件

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF008.jpg)

3) 添加FATFS文件夹到工程的include选项中。打开工程选项对话框，选择“C/C++”选项下的“Include Paths”项目， 在弹出路径设置对话框中选择添加“FATFS”文件夹

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF009.jpg)

4) 如果现在编译工程，可以发现有两个错误，一个是来自diskio.c文件，提示有一些头文件没找到， diskio.c文件内容是与底层设备输入输出接口函数文件，不同硬件设计驱动就不同，需要的文件也不同； 另外一个错误来自cc936.c文件，提示该文件不是工程所必需的，这是因为FatFs默认使用日语， 我们想要支持简体中文需要修改FatFs的配置，即修改ffconf.h文件。至此，将FatFs添加到工程的框架已经操作完成， 接下来要做的就是修改diskio.c文件和ffconf.h文件。

## 4. FatFs底层设备驱动函数

FatFs文件系统与底层介质的驱动分离开来，对底层介质的操作都要交给用户去实现，它仅仅是提供了一个函数接口而已。 表为FatFs移植时用户必须支持的函数。 通过表我们可以清晰知道很多函数是在一定条件下才需要添加的， 只有前三个函数是必须添加的。我们完全可以根据实际需求选择实现用到的函数。

前三个函数是实现读文件最基本需求。接下来三个函数是实现创建文件、修改文件需要的。为实现格式化功能， 需要在disk_ioctl添加两个获取物理设备信息选项。我们一般只要实现前面六个函数就可以了，已经足够满足大部分功能。

为支持简体中文长文件名称需要添加ff_convert和ff_wtoupper函数，实际这两个已经在cc936.c文件中实现，我们只要直接把cc936.c文件添加到工程中就可以。

后面六个函数一般都不用。如真有需要可以参考syscall.c文件(srcoption文件夹内)。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF01.png)

底层设备驱动函数是存放在diskio.c文件，我们的目的就是把diskio.c中的函数接口与SPI Flash芯片驱动连接起来。 总共有五个函数，分别为设备状态获取(disk_status)、设备初始化(disk_initialize)、扇区读取(disk_read)、 扇区写入(disk_write)、其他控制(disk_ioctl)。

### 4.1 设备宏定义

```c
// 为每个设备定义一个物理编号
#define ATA         0     // 预留SD卡使用
#define SPI_FLASH   1     // 外部SPI Flash
```

这两个宏定义在FatFs中非常重要，FatFs是支持多物理设备的，必须为每个物理设备定义一个不同的编号。

SD卡是预留接口，在讲解SDIO接口相关章节后会用到，可以实现使用读写SD卡内文件。

### 4.2 设备状态获取

```c
// 获取当前设备状态，函数参数：pdrv：驱动器号
DSTATUS disk_status(BYTE pdrv)
{
	DSTATUS status = STA_NOINIT; // 默认未初始化
	switch (pdrv) {
		case ATA:
			// 预留SD卡使用
			break;
		case SPI_FLASH:
			if(sFLASH_ID == SPI_FLASH_ReadID()) // 读取到FLASH
			{
				status &= ~STA_NOINIT; // 初始化成功
			}
			else
			{
				status = STA_NOINIT; // 未初始化
			}
			break;
		default:
			status = STA_NOINIT;				
	}
	return status;
}
```

- **`DSTATUS status = STA_NOINIT;`**: 初始化状态为 `STA_NOINIT`，表示设备尚未初始化。
- **`switch (pdrv)`**: 根据传入的物理编号 `pdrv`，选择不同的设备类型进行处理。
  - **`case ATA:`**: 处理 SD 卡（具体实现尚未完成）。
  - **`case SPI_FLASH:`**: 处理 SPI Flash，读取其设备 ID 进行状态检测。
  - **`default:`**: 对于不认识的设备类型，返回 `STA_NOINIT`。

### 4.3 设备初始化

```c
// 设备初始化，函数参数：pdrv：驱动器号
DSTATUS disk_initialize(BYTE pdrv)
{
	uint16_t i;
	DSTATUS status = STA_NOINIT; // 默认未初始化
	switch (pdrv) {
		case ATA:
			break;
		case SPI_FLASH:
			SPI_FLASH_Init(); // 初始化SPI Flash
			i = 500;
			while(--i);
			SPI_FLASH_WakeUp(); // 唤醒Flash
			status = disk_status(pdrv); // 获取当前设备状态
			break;
		default:
			status = STA_NOINIT;
	}
	return status;
}
```

disk_initialize函数也是有一个参数pdrv，用来指定设备物理编号。 对于SPI Flash芯片我们调用SPI_FLASH_Init()函数实现对SPI Flash芯片引脚GPIO初始化配置以及SPI通信参数配置。 SPI_Flash_WAKEUP()函数唤醒SPI Flash芯片，当SPIFlash芯片处于睡眠模式时需要唤醒芯片才可以进行读写操作。

最后调用disk_status函数获取SPI Flash芯片状态，并返回状态值。

### 4.4 读取扇区

```c
// 读扇区
// 函数参数：pdrv：驱动器号；buff：读出数据缓冲区；sector：扇区号；count：扇区个数
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
	DRESULT status = RES_PARERR; // 默认参数错误
	switch (pdrv) {
		case ATA:
			break;
		case SPI_FLASH:
			/* 扇区偏移2MB，外部FLASH文件系统存储在后6MB空间 */
			sector += 512; // 扇区偏移
			SPI_FLASH_BufferRead(buff, sector<<12, count<<12); // 读出扇区数据
			status = RES_OK; // 读取成功
		default:
			status = RES_PARERR;
		}
	return status;	
}
```

disk_read函数有四个形参。pdrv为设备物理编号。buff是一个BYTE类型指针变量，buff指向用来存放读取到数据的存储区首地址。 sector是一个DWORD类型变量，指定要读取数据的扇区首地址。count是一个UINT类型变量，指定扇区数量。

BYTE类型实际是unsigned char类型，DWORD类型实际是unsigned long类型， UINT类型实际是 unsigned int类型，类型定义在integer.h文件中。

开发板使用的SPI Flash芯片型号为W25Q64FV，每个扇区大小为4096个字节(4KB)，总共有8M字节空间， 为兼容后面实验程序，我们只将后部分6MB空间分配给FatFs使用，前部分2MB空间用于其他实验需要，即FatFs是从2MB空间开始， 为实现这个效果需要将所有的读写地址都偏移512个扇区空间。

对于SPI Flash芯片，主要是使用SPI_FLASH_BufferRead()实现在指定地址读取指定长度的数据，它接收三个参数， 第一个参数为指定数据存放地址指针。第二个参数为指定数据读取地址，这里使用左移运算符，左移12位实际是乘以4096， 这与每个扇区大小是息息相关的。第三个参数为读取数据个数，也是需要使用左移运算符。

### 4.5 扇区写入

```c
// 写扇区
// 函数参数：pdrv：驱动器号；buff：写入数据缓冲区；sector：扇区号；count：扇区个数
#if _USE_WRITE
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	uint32_t write_addr;
	DRESULT status = RES_PARERR;
	if(!count)
	{
		return RES_PARERR;
	}
	switch (pdrv) {
		case ATA:
			break;
		case SPI_FLASH:
			/* 扇区偏移2MB，外部FLASH文件系统存储在后6MB空间 */
			sector += 512; // 扇区偏移
			write_addr = sector<<12; // 计算写入地址
			SPI_FLASH_SectorErase(write_addr); // 扇区擦除
			SPI_FLASH_BufferWrite((uint8_t*)buff, write_addr, count<<12); // 写入扇区数据
			status = RES_OK; // 写入成功
			break;
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif
```

disk_write函数有四个形参，pdrv为设备物理编号。buff指向待写入扇区数据的首地址。sector，指定要写入数据的扇区首地址。 count指定扇区数量。对于SPI Flash芯片，在写入数据之前需要先擦除，所以用到扇区擦除函数(SPI_FLASH_SectorErase)。 然后就是在调用数据写入函数(SPI_FLASH_BufferWrite)把数据写入到指定位置内。

### 4.6 其他控制

```c
// 其他控制
#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:
			break;
		case SPI_FLASH:
			switch(cmd) // 处理IOCTL命令
			{
				case GET_SECTOR_COUNT: // 获取总扇区数
				/* 扇区数量：1536*4096/1024/1024= 6MB*/
					*(DWORD*)buff = 1536;
					break;
				case GET_SECTOR_SIZE: // 获取扇区大小
					*(WORD*)buff = 4096;
					break;
				case GET_BLOCK_SIZE: // 同时擦除扇区数
					*(WORD*)buff = 1;
					break;
			}
			status = RES_OK;
			break;
		default:
			status = RES_PARERR;
	}
	return status;				
}
#endif
```

disk_ioctl函数有三个形参，pdrv为设备物理编号，cmd为控制指令，包括发出同步信号、获取扇区数目、获取扇区大小、 获取擦除块数量等等指令，buff为指令对应的数据指针。

对于SPI Flash芯片，为支持FatFs格式化功能， 需要用到获取扇区数量(GET_SECTOR_COUNT)指令和获取擦除块数量(GET_BLOCK_SIZE)指令。 另外，SD卡扇区大小为512字节，SPI Flash芯片一般设置扇区大小为4096字节，所以需要用到获取扇区大小(GET_SECTOR_SIZE)指令。

### 4.7 时间戳获取

```c
__weak DWORD get_fattime(void) {
    return ((DWORD)(2015-1980)<<25) // 年 (2015)
           | ((DWORD)1<<21)         // 月 (1 月)
           | ((DWORD)1<<16)         // 日 (1 日)
           | ((DWORD)0<<11)         // 时 (0 时)
           | ((DWORD)0<<5)          // 分 (0 分)
           | ((DWORD)0>>1);    		// 秒 (0 秒)
}
```

get_fattime函数用于获取当前时间戳，在ff.c文件中被调用。FatFs在文件创建、被修改时会记录时间， 这里我们直接使用赋值方法设定时间戳。为更好的记录时间，可以使用控制器RTC功能，具体要求返回值格式为：

- bit31:25 ——从1980至今是多少年，范围是 (0..127) ；

- bit24:21 ——月份，范围为 (1..12) ；

- bit20:16 ——该月份中的第几日，范围为(1..31) ；

- bit15:11——时，范围为 (0..23)；

- bit10:5 ——分，范围为 (0..59)；

- bit4:0 ——秒/ 2，范围为 (0..29) 。

## 5. FatFs功能配置

ffconf.h文件是FatFs功能配置文件，我们可以对文件内容进行修改，使得FatFs更符合我们的要求。 ffconf.h对每个配置选项都做了详细的使用情况说明。下面只列出修改的配置，其他配置采用默认即可。

```c
#define _USE_MKFS   1
#define _CODE_PAGE  936
#define _USE_LFN    2
#define _VOLUMES    2
#define _MIN_SS     512
#define _MAX_SS     4096
```

1) _USE_MKFS： 格式化功能选择，为使用FatFs格式化功能，需要把它设置为1。

2) _CODE_PAGE： 语言功能选择，并要求把相关语言文件添加到工程宏。为支持简体中文文件名需要使用“936”的操作，我们已经把cc936.c文件添加到工程中。

3) _USE_LFN： 长文件名支持，默认不支持长文件名，这里配置为2，支持长文件名，并指定使用栈空间为缓冲区。

4) _VOLUMES： 指定物理设备数量，这里设置为2，包括预留SD卡和SPI Flash芯片。

5) _MIN_SS 、_MAX_SS： 指定扇区大小的最小值和最大值。SD卡扇区大小一般都为512字节，SPI Flash芯片扇区大小一般设置为4096字节，所以需要把_MAX_SS改为4096。

## 6. FatFs功能测试

移植操作到此，就已经把FatFs全部添加到我们的工程了，这时我们编译功能，顺利编译通过，没有错误。接下来， 我们就可以使用编写图中用户应用程序了。

主要的测试包括格式化测试、文件写入测试和文件读取测试三个部分，主要程序都在main.c文件中实现。

### 6.1 变量定义

```c
FATFS fs;				   // FatFs文件系统对象
FIL fnew;				   // 文件对象
FRESULT res_flash;         // 文件操作结果
UINT fnum;            	   // 文件成功读写数量 
BYTE ReadBuffer[1024] = {0}; // 读缓冲区
BYTE WriteBuffer[] =  "Demo File System Test\r\n"; // 写缓冲区
```

FATFS是在ff.h文件定义的一个结构体类型，针对的对象是物理设备，包含了物理设备的物理编号、扇区大小等等信息， 一般我们都需要为每个物理设备定义一个FATFS变量。

FIL也是在ff.h文件定义的一个结构体类型，针对的对象是文件系统内具体的文件，包含了文件很多基本属性，比如文件大小、 路径、当前读写地址等等。如果需要在同一时间打开多个文件进行读写，才需要定义多个FIL变量，不然一般定义一个FIL变量即可。

FRESULT是也在ff.h文件定义的一个枚举类型，作为FatFs函数的返回值类型，主要管理FatFs运行中出现的错误。总共有19种错误类型， 包括物理设备读写错误、找不到文件、没有挂载工作空间等等错误。这在实际编程中非常重要，当有错误出现时我们要停止文件读写， 通过返回值我们可以快速定位到错误发生的可能地点。如果运行没有错误才返回FR_OK。

fnum是个32位无符号整形变量，用来记录实际读取或者写入数据的数组。

buffer和textFileBuffer分别对应读取和写入数据缓存区，都是8位无符号整形数组。

### 6.2 主函数

```c

int main(void)
{
	LED_Init();	
	LED_BLUE();
	USART_Config();	
  	printf("外设初始化正常");
	/*
		在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
		初始化函数调用流程如下
		f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	*/
	res_flash = f_mount(&fs, "1:", 1); // 挂载文件系统，函数参数：文件系统对象、挂载点、格式化标志	
	// 如果没有文件系统就格式化创建创建文件系统
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("FLASH还没有文件系统，即将进行格式化...\r\n");
    	// 下面进行格式化操作
		res_flash = f_mkfs("1:", 0, 0); // 函数参数：挂载点、分区大小、工作区大小							
		if(res_flash == FR_OK) // 格式化成功
		{
			printf("FLASH已成功格式化文件系统。\r\n");
      		// 格式化后，先取消挂载
			res_flash = f_mount(NULL,"1:",1); // 函数参数：文件系统对象、挂载点、格式化标志		
      		// 重新挂载			
			res_flash = f_mount(&fs,"1:",1); // 函数参数：文件系统对象、挂载点、格式化标志
		}
		else
		{
			LED_RED();
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  	else if(res_flash != FR_OK) // 其他错误
    {
    	printf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
		while(1);
  	}
  	else
  	{
    	printf("文件系统挂载成功\r\n");
  	}
  
	// 文件系统测试：写测试
	// 打开文件，每次都以新建的形式打开，属性为可写 */
	printf("\r\n即将进行文件写入测试... \r\n");	
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE ); // f_open()函数打开文件，参数：文件对象、文件名、属性
	if (res_flash == FR_OK)
	{
		printf("打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    	// 将指定存储区内容写入到文件内
		res_flash = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum); // f_write()函数写入文件，参数：文件对象、写入数据、写入字节数、实际写入字节数
    	if(res_flash == FR_OK)
    	{
      		printf("文件写入成功，写入字节数据：%d\n",fnum);
      		printf("向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    	}
    	else
    	{
      		printf("文件写入失败：(%d)\n",res_flash);
    	}    
    	f_close(&fnew); // f_close()函数关闭文件，参数：文件对象
	}
	else
	{	
		LED_RED();
		printf("打开/创建文件失败。\r\n");
	}
	// 文件系统测试：读测试
	printf("即将进行文件读取测试...\r\n");
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_OPEN_EXISTING | FA_READ); // f_open()函数打开文件，参数：文件对象、文件名、属性
	if(res_flash == FR_OK)
	{
		LED_GREEN();
		printf("打开文件成功。\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); // f_read()函数读取文件，参数：文件对象、读入数据、读入字节数、实际读入字节数
    	if(res_flash == FR_OK)
    	{
      		printf("文件读取成功,读到字节数据：%d\r\n",fnum);
      		printf("读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    	}
    	else
    	{
      		printf("文件读取失败：(%d)\n",res_flash);
    	}			
	}
	else
	{
		LED_RED();
		printf("打开文件失败。\r\n");
	}
	// 测试完毕，关闭文件
	f_close(&fnew);	
	// 不再使用文件系统，取消挂载文件系统
	f_mount(NULL,"1:",1);
	while(1)
	{
	}
}
```

首先，初始化RGB彩灯和调试串口，用来指示程序进程。

FatFs的第一步工作就是使用f_mount函数挂载工作区。f_mount函数有三个形参，第一个参数是指向FATFS变量指针，如果赋值为NULL可以取消物理设备挂载。 第二个参数为逻辑设备编号，使用设备根路径表示，与物理设备编号挂钩，我们定义SPI Flash芯片物理编号为1，所以这里使用“1：”。 第三个参数可选0或1，1表示立即挂载，0表示不立即挂载，延迟挂载。 f_mount函数会返回一个FRESULT类型值，指示运行情况。

如果f_mount函数返回值为FR_NO_FILESYSTEM，说明没有FAT文件系统，比如新出厂的SPI Flash芯片就没有FAT文件系统。 我们就必须对物理设备进行格式化处理。使用f_mkfs函数可以实现格式化操作。f_mkfs函数有三个形参， 第一个参数为逻辑设备编号；第二参数可选0或者1，0表示设备为一般硬盘，1表示设备为软盘。第三个参数指定扇区大小，如果为0， 表示通过disk_ioctl函数获取。格式化成功后需要先取消挂载原来设备，再重新挂载设备。

在设备正常挂载后，就可以进行文件读写操作了。使用文件之前，必须使用f_open函数打开文件，不再使用文件必须使用f_close函数关闭文件， 这个跟电脑端操作文件步骤类似。f_open函数有三个形参，第一个参数为文件对象指针。第二参数为目标文件，包含绝对路径的文件名称和后缀名。 第三个参数为访问文件模式选择，可以是打开已经存在的文件模式、读模式、写模式、新建模式、总是新建模式等的或运行结果。比如对于写测试， 使用FA_CREATE_ALWAYS和FA_WRITE组合模式，就是总是新建文件并进行写模式。

f_close函数用于不再对文件进行读写操作关闭文件，f_close函数只要一个形参，为文件对象指针。f_close函数运行可以确保缓冲区完全写入到文件内。

成功打开文件之后就可以使用f_write函数和f_read函数对文件进行写操作和读操作。这两个函数用到的参数是一致的，只不过一个是数据写入， 一个是数据读取。f_write函数第一个形参为文件对象指针，使用与f_open函数一致即可。第二个参数为待写入数据的首地址， 对于f_read函数就是用来存放读出数据的首地址。第三个参数为写入数据的字节数，对于f_read函数就是欲读取数据的字节数。 第四个参数为32位无符号整形指针，这里使用fnum变量地址赋值给它，在运行读写操作函数后，fnum变量指示成功读取或者写入的字节个数。

最后，不再使用文件系统时，使用f_mount函数取消挂载。

保证开发板相关硬件连接正确，用USB线连接开发板“USB TOUART”接口跟电脑，在电脑端打开串口调试助手，把编译好的程序下载到开发板。 程序开始运行后，RGB彩灯为蓝色，在串口调试助手可看到格式化测试、 写文件检测和读文件检测三个过程；最后如果所有读写操作都正常，RGB彩灯会指示为绿色，如果在运行中FatFs出现错误RGB彩灯指示为红色。

虽然我们通过RGB彩灯指示和串口调试助手信息打印方法来说明FatFs移植成功，并顺利通过测试，但心底总是很踏实，所谓眼见为实， 虽然我们创建了“FatFs读写测试文件.txt”这个文件，却完全看不到实体。这个确实是个问题，因为我们这里使用SPI Flash芯片作为物理设备， 并不像SD卡那么方便直接用读卡器就可以在电脑端打开验证。另外一个问题，就目前来说， 在SPI Flash芯片上挂载FatFs好像没有实际意义，无法发挥文件系统功能。

实际上，这里归根到底就是我们目前没办法在电脑端查看SPI Flash芯片内FatFs的内容，没办法非常方便拷贝、删除文件。我们当然不会做无用功， STM32控制器还有一个硬件资源可以解决上面的问题，就是USB！我们可以通过编程把整个开发板变成一个U盘，而U盘存储空间就是SPI Flash芯片的空间。 这样非常方便实现文件读写。至于USB内容将在USB相关章节讲解。

## 6. 流程小结

### 6.1 环境准备

- **下载 FatFs 文件系统**: 从 [FatFs 官方网站](http://elm-chan.org/fsw/ff/00index_e.html) 下载 FatFs 的源代码。
- **STM32F103 标准外设库**: 从 [STMicroelectronics 的官网](https://www.st.com/en/development-tools/stm32cubeide.html) 下载 STM32F1 标准外设库。

### 6.2 添加 FatFs 源代码

将 FatFs 的源文件 (`ff.c`, `ff.h`, `diskio.c`, `diskio.h`) 添加到你的项目中。

### 6.3 配置 STM32F103 的 SPI 或 SD 接口

假设我们使用 SPI 接口连接 SD 卡，这里是如何初始化 SPI 接口的基本步骤：

#### 6.3.1 SPI 初始化代码

```c
#include "stm32f10x.h"

// SPI1 初始化函数
void SPI1_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 启用 SPI1 和 GPIOA 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // SPI1 SCK 和 MOSI 引脚配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; // SCK 和 MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SPI1 MISO 引脚配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SPI1 配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 10;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 启用 SPI1
    SPI_Cmd(SPI1, ENABLE);
}
```

### 6.4 实现 `diskio.c`

`diskio.c` 是 FatFs 的接口文件，它需要与硬件驱动进行绑定。这里是一个简化的 SPI 接口实现的示例：

```c
#include "diskio.h"
#include "stm32f10x.h"

// SPI 数据发送和接收函数
static uint8_t SPI_SendReceive(uint8_t data)
{
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

// SPI 初始化和 SD 卡初始化代码省略
DSTATUS disk_initialize(BYTE pdrv)
{
    // 省略初始化代码
    return RES_OK;
}

DSTATUS disk_status(BYTE pdrv)
{
    // 省略状态检查代码
    return RES_OK;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    // 省略读取代码
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    // 省略写入代码
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    // 省略控制命令代码
    return RES_OK;
}
```

### 6.5 配置 FatFs

在你的 `ffconf.h` 配置文件中，确保正确设置以下参数以符合你的系统需求：

```c
#define FF_MAX_SS  4096
#define FF_MIN_SS  512
#define FF_FS_RPATH 0
#define FF_FS_TINY 0
#define FF_FS_EXFAT 0
#define FF_FS_NORTC 1
#define FF_FS_LOCK 0
#define FF_FS_REENTRANT 0
#define FF_FS_TIMEOUT 1000
#define FF_FS_NOFSINFO 0
```

### 6.6 集成 FatFs

在你的主程序中，初始化 FatFs 并挂载文件系统：

```c
#include "ff.h"

FATFS FatFs;
FIL fil;
FRESULT fr;

int main(void)
{
    // 系统初始化代码
    SPI1_Init(); // 初始化 SPI

    // 挂载文件系统
    fr = f_mount(&FatFs, "", 1);
    if (fr != FR_OK) {
        // 处理错误
    }

    // 打开文件
    fr = f_open(&fil, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fr == FR_OK) {
        // 写入文件
        f_write(&fil, "Hello World!", 12, NULL);
        f_close(&fil);
    } else {
        // 处理错误
    }

    while (1) {
        // 主循环
    }
}
```

---

2024.9.6 第一次修订，后期不再维护

2025.3.3 简化内容，优化代码
