# 第二十二章 内部flash介绍及应用

## 1. STM32F407ZGT6  FLASH 简介

**1. Flash 存储器容量**  
STM32F407ZGT6 拥有高达 **1MB（兆字节）的 Flash 存储器**。 这为复杂的嵌入式应用程序提供了充足的代码和数据存储空间。

**2. Flash 存储器类型**  
STM32F4 系列的微控制器，包括 STM32F407ZGT6，使用的是 **基于扇区 (Sector-based) 的 Flash 存储器**。 这与一些低端 STM32 系列（如 STM32F1/M0/M3）使用的基于页 (Page-based) 的 Flash 存储器有所不同。

**3. Flash 存储器组织结构**  
STM32F407ZGT6 的 Flash 存储器通常分为以下几个部分：

- **主存储器 (Main Memory)：** 这是我们通常用来存储用户程序代码和数据常量 (如 `const` 类型数据) 的区域。 STM32F407ZGT6 的主存储器被划分为 **12 个扇区 (Sectors)**。
  
  - 前 4 个扇区（扇区 0 到扇区 3）的大小通常为 **16KB**。
  - 扇区 4 的大小为 **64KB**。
  - 扇区 5 到扇区 11 的大小通常为 **128KB**。
  - 程序的起始地址通常从 `0x08000000` 开始。

- **系统存储器 (System Memory)：** 这个区域固化了 ST 公司的 Bootloader 代码。 这个 Bootloader 在出厂时烧录，主要用于通过串口 (UART) 或 USB 等接口下载用户程序到主存储器中。 当特定的启动模式引脚 (BOOT0 和 BOOT1) 配置为从系统存储器启动时，MCU 将进入 Bootloader 模式。

- **OTP (One-Time Programmable) 区域：** 这是一个一次性可编程区域，总共有 528 字节。
  
  - 其中的 512 字节（分为 16 个块，每块 32 字节）可以用来存储用户数据，但这些数据一旦写入就无法擦除。
  - 剩下的 16 字节用于锁定对应的块。

- **Option Bytes (选项字节)：** 这部分存储器用于配置 MCU 的一些关键特性，例如读保护 (Read Protection)、BOR (Brown-Out Reset) 级别、看门狗定时器 (Watchdog Timers) 等。

**4. Flash 访问与编程**

- **读访问：** Flash 存储器可以直接像普通内存一样通过指针进行读取。
- **写访问和擦除：** 对 Flash 进行写入或擦除操作需要特定的步骤和协议，以确保数据的完整性和安全性。
  - 通常需要先解锁 Flash 控制寄存器。
  - 然后进行扇区或页的擦除操作（STM32F4 系列是基于扇区的）。
  - 最后才能写入新的数据。
  - 完成操作后需要再次锁定 Flash。
  - ST 提供了 HAL 库函数来简化这些 Flash 编程操作。

**5. ART 加速器 (ART Accelerator™)**  
STM32F407ZGT6 内置了 ART 加速器， 它可以实现对 Flash 存储器的零等待状态 (0-wait state) 执行，即使在 CPU 工作在最高频率 168 MHz 时也能保持高效的指令执行，从而提升了整体性能。

## 2. 内部flash应用示例

### 2.1 flash相关宏定义

```c
#ifndef __STMFLASH_H
#define __STMFLASH_H

#include "sys.h"

/* FLASH起始地址 */
#define STM32_FLASH_SIZE 0x100000  // STM32 FLASH 总大小
#define STM32_FLASH_BASE 0X08000000 // STM32 FLASH 起始地址
#define FLASH_WATTETIME 5000 // FLASH等待超时时间 

/* FLASH 扇区的起始地址 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t )0x08000000)     /* 扇区0起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t )0x08004000)     /* 扇区1起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t )0x08008000)     /* 扇区2起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t )0x0800C000)     /* 扇区3起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t )0x08010000)     /* 扇区4起始地址, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t )0x08020000)     /* 扇区5起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t )0x08040000)     /* 扇区6起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t )0x08060000)     /* 扇区7起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t )0x08080000)     /* 扇区8起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t )0x080A0000)     /* 扇区9起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t )0x080C0000)     /* 扇区10起始地址,128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t )0x080E0000)     /* 扇区11起始地址,128 Kbytes */

uint32_t stmflash_read_word(uint32_t faddr);
uint8_t  stmflash_get_flash_sector(uint32_t addr);
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length);
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length);
void stmflash_erase_sector(uint8_t sector);

#endif /* __STMFLASH_H__ */
```

### 2.2 从指定地址读取一个字

```c
/**
 * @brief       从指定地址读取一个字 (32位数据)
 * @param       faddr   : 读取地址 (此地址必须为4倍数!!)
 * @retval      读取到的数据 (32位)
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}
```

### 2.3 获取某个地址所在的flash扇区

```c
/**
 * @brief       获取某个地址所在的flash扇区
 * @param       addr    : flash地址
 * @retval      0~11,即addr所在的扇区
 */
uint8_t  stmflash_get_flash_sector(uint32_t addr)
{
    if (addr < ADDR_FLASH_SECTOR_1) return FLASH_SECTOR_0;
    else if (addr < ADDR_FLASH_SECTOR_2) return FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_3) return FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_4) return FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_5) return FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_6) return FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_7) return FLASH_SECTOR_6;
    else if (addr < ADDR_FLASH_SECTOR_8) return FLASH_SECTOR_7;
    else if (addr < ADDR_FLASH_SECTOR_9) return FLASH_SECTOR_8;
    else if (addr < ADDR_FLASH_SECTOR_10) return FLASH_SECTOR_9;
    else if (addr < ADDR_FLASH_SECTOR_11) return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}
```

### 2.4 获取扇区大小

```c
/**
 * @brief       获取扇区大小
 * @param       sector  : 扇区号(0-11)
 * @retval      扇区大小(字节)
 */
static uint32_t get_sector_size(uint8_t sector)
{
    if (sector < 4) return 16 * 1024;      // 扇区0-3: 16KB
    else if (sector == 4) return 64 * 1024; // 扇区4: 64KB
    else return 128 * 1024;                // 扇区5-11: 128KB
}
```

### 2.5 在FLASH 指定位置, 写入指定长度的数据

```c
/**
 * @brief       在FLASH 指定位置, 写入指定长度的数据(自动擦除)
 *   @note      因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数写地址如果非0XFF
 *              ,那么会先擦除整个扇区且不保存扇区数据.所以写非0XFF的地址,将导致整个扇区数据丢失.
 *              建议写之前确保扇区里没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
 *              该函数对OTP区域也有效!可以用来写OTP区!
 *              OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(注意：最后16字节，用于OTP数据块锁定，别乱写！！)
 * @param       waddr   : 起始地址 (此地址必须为4的倍数!!,否则写入出错!)
 * @param       pbuf    : 数据指针
 * @param       length  : 要写入的 字(32位)数(就是要写入的32位数据的个数)
 * @retval      无
 */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t addrx = waddr;
    uint32_t endaddr = waddr + length * 4;
    uint32_t sectorerror = 0;

    /* 写入地址小于 STM32_FLASH_BASE, 或不是4的整数倍, 非法. */
    /* 写入地址大于 STM32_FLASH_BASE + STM32_FLASH_SIZE, 非法. */
    if(waddr < STM32_FLASH_BASE || waddr % 4 || waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE)){
        return;
    }

    HAL_FLASH_Unlock(); // 解锁FLASH
    FLASH->ACR &= ~(1<<10); // 禁止指令缓存

    if(addrx < 0x1FFF0000) // 只有主存储区,才需要执行擦除操作!!
    {
        while(addrx < endaddr) // 扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
        {
            // 修复: 检查当前地址的值是否为0xFFFFFFFF
            if(stmflash_read_word(addrx) != 0xFFFFFFFF)
            {
                uint8_t sector = stmflash_get_flash_sector(addrx);
                uint32_t sector_size = get_sector_size(sector);

                EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS; // 擦除类型：扇区擦除
                EraseInitStruct.Sector = sector; // 要擦除的扇区号
                EraseInitStruct.NbSectors = 1; // 一次只擦除一个扇区
                EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // VCC=2.7~3.6V之间

                // 等待上次操作完成
                FLASH_WaitForLastOperation(FLASH_WATTETIME);

                if(HAL_FLASHEx_Erase(&EraseInitStruct, &sectorerror) != HAL_OK) // 擦除失败,退出循环
                {
                    break;
                }

                // 重要: 擦除后跳过整个扇区
                addrx = ADDR_FLASH_SECTOR_0 + (sector + 1) * sector_size;
            }
            else
            {
                addrx += 4; // 地址已经是0xFFFFFFFF, 跳过
            }
        }
    }

    FlashStatus = FLASH_WaitForLastOperation(FLASH_WATTETIME); // 等待上次操作完成
    if(FlashStatus == HAL_OK)
    {
        while(waddr < endaddr) // 写入数据
        {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, waddr, *pbuf) != HAL_OK)
            {
                break;
            }
            waddr += 4;
            pbuf++;
        }
    }

    FLASH->ACR |= (1<<10); // 开启指令缓存
    HAL_FLASH_Lock(); // 锁定FLASH
}
```

### 2.6 从指定地址开始读出指定长度的数据

```c
/**
 * @brief       从指定地址开始读出指定长度的数据
 * @param       raddr : 起始地址
 * @param       pbuf  : 数据指针
 * @param       length: 要读取的字(32)数,即4个字节的整数倍
 * @retval      无
 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_word(raddr);   
        raddr += 4; 
    }
}
```

### 2.7 擦除指定扇区

```c
/**
* @brief       擦除指定扇区
* @param       sector  : 要擦除的扇区号(0~11)
* @retval      无
*/
void stmflash_erase_sector(uint8_t sector)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t sector_error = 0; // 用于存储擦除过程中出错的扇区地址
    // 检查扇区号是否有效
    if (sector > 11) {
        return;
    }
    HAL_FLASH_Unlock(); // 解锁FLASH，允许编程和擦除操作
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS; // 设置擦除类型为扇区擦除
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 设置工作电压范围，对于STM32F407，通常为VCC=2.7~3.6V
    EraseInitStruct.Sector = sector; // 指定要擦除的扇区
    EraseInitStruct.NbSectors = 1; // 指定要擦除的扇区数量，这里只擦除一个扇区
    // 等待上一次FLASH操作完成，确保FLASH空闲
    // FLASH_WaitForLastOperation(FLASH_WATTETIME); // HAL_FLASHEx_Erase 内部会等待
    // 执行扇区擦除操作
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &sector_error) != HAL_OK)
    {
        // 擦除操作失败
    }
    else
    {
        // 擦除操作成功
    }
    HAL_FLASH_Lock(); // 锁定FLASH，禁止编程和擦除操作
}
```

### 2.8 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "stmflash.h"
#include "string.h"

const uint8_t text_buf[] = {"STM32 FLASH TEST"};
#define TEXT_LEN sizeof(text_buf)

/* 计算需要写入的字数(32位) */
#define SIZE ((TEXT_LEN + 3) / 4)  // 标准4字节对齐计算

/* 设置FLASH 保存地址(必须为4的整数倍，且其值要大于本代码所占用FLASH的大小 + 0X08000000) */
#define FLASH_SAVE_ADDR 0x08010000 

/* 获取保存地址所在的扇区 */
#define FLASH_SAVE_SECTOR stmflash_get_flash_sector(FLASH_SAVE_ADDR)

int main(void)
{
    uint8_t key_value = 0;
    uint16_t i = 0;
    uint8_t databuf[SIZE * 4];  // 确保足够大的缓冲区

    bsp_init();
    LCD_ShowString(30,110,200,16,16,"KEY0:Write KEY1:Read");
    LCD_ShowString(30,130,200,16,16,"KEY2:Erase Sector");

    // 初始化数据缓冲区
    memset(databuf, 0, sizeof(databuf)); // 清空缓冲区
    memcpy(databuf, text_buf, TEXT_LEN); // 复制实际数据

    while(1)
    {
        key_value = key_scan(0);

        if(key_value == KEY0_Press) // 按下KEY0，写入数据
        {
            LCD_Fill(30,150,240,190,WHITE); // 清除显示区域
            LCD_ShowString(30,150,200,16,16,"Start Write FLASH....");
            stmflash_write(FLASH_SAVE_ADDR, (uint32_t*)databuf, SIZE);
            LCD_ShowString(30,150,200,16,16,"Write FLASH Success !");
        }
        else if(key_value == KEY1_Press) // 按下KEY1，读取数据
        {
            LCD_Fill(30,150,240,190,WHITE); // 清除显示区域
            LCD_ShowString(30,150,200,16,16,"Start Read FLASH....");

            // 清空缓冲区确保显示正确
            memset(databuf, 0, sizeof(databuf));
            stmflash_read(FLASH_SAVE_ADDR, (uint32_t*)databuf, SIZE);

            // 确保字符串以null结尾
            databuf[sizeof(databuf) - 1] = '\0';

            LCD_ShowString(30,150,200,16,16,"The Data Readed Is:  ");

            // 检查是否为擦除状态 (全0xFF)
            uint8_t is_erased = 1;
            for(int j = 0; j < sizeof(databuf); j++) {
                if(databuf[j] != 0xFF) {
                    is_erased = 0;
                    break;
                }
            }

            if(is_erased) {
                LCD_ShowString(30,170,200,16,16,"[SECTOR ERASED]");
            } else {
                LCD_ShowString(30,170,200,16,16,(char*)databuf);
            }
        }
        else if(key_value == KEY2_Press) // 按下KEY2，擦除扇区
        {
            LCD_Fill(30,150,240,190,WHITE); // 清除显示区域
            LCD_ShowString(30,150,200,16,16,"Erasing Sector...");

            // 擦除保存地址所在的扇区
            stmflash_erase_sector(FLASH_SAVE_SECTOR);

            LCD_ShowString(30,150,200,16,16,"Erase Complete!   ");
            LCD_ShowString(30,170,200,16,16,"Sector Cleared to 0xFF");
        }

        i++;
        delay_ms(10);
        if(i == 20)
        {
            i = 0;
            LED_TOGGLE(LED0_GPIO_Pin);
        }
    }
}
```

## 3. 内部flash相关函数（HAL库）

### 3.1 Flash 初始化和控制

#### 3.1.1 解锁/锁定 Flash

```c
// 解锁 Flash 控制寄存器
HAL_StatusTypeDef HAL_FLASH_Unlock(void);

// 锁定 Flash 控制寄存器
HAL_StatusTypeDef HAL_FLASH_Lock(void);

// 解锁选项字节寄存器
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);

// 锁定选项字节寄存器
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
```

**使用示例**：

```c
HAL_FLASH_Unlock(); // 开始操作前必须解锁
// 执行Flash操作...
HAL_FLASH_Lock(); // 操作完成后锁定
```

### 3.2 Flash 擦除操作

#### 3.2.1 扇区擦除

```c
HAL_StatusTypeDef HAL_FLASHEx_Erase(
 FLASH_EraseInitTypeDef *pEraseInit, 
uint32_t *SectorError
);
```

**擦除配置结构体**：

```c
typedef struct {
 uint32_t TypeErase; // 擦除类型: FLASH_TYPEERASE_SECTORS
 uint32_t Banks; // 选择Bank: FLASH_BANK_1 或 FLASH_BANK_BOTH
 uint32_t Sector; // 起始扇区号 (FLASH_SECTOR_x)
 uint32_t NbSectors; // 要擦除的扇区数量
 uint32_t VoltageRange; // 电压范围: FLASH_VOLTAGE_RANGE_3
} FLASH_EraseInitTypeDef;
```

**擦除示例**：

```c
FLASH_EraseInitTypeDef EraseInit;
uint32_t SectorError = 0;

EraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
EraseInit.Banks = FLASH_BANK_1;
EraseInit.Sector = FLASH_SECTOR_5;
EraseInit.NbSectors = 2; // 擦除扇区5和6
EraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;

HAL_FLASH_Unlock();
if (HAL_FLASHEx_Erase(&EraseInit, &SectorError) != HAL_OK) {
 // 错误处理，SectorError包含失败扇区号
}
HAL_FLASH_Lock();
```

### 3.3 Flash 编程操作

#### 3.3.1 按数据类型编程

```c
HAL_StatusTypeDef HAL_FLASH_Program(
 uint32_t TypeProgram, // 数据类型:
 // FLASH_TYPEPROGRAM_BYTE
 // FLASH_TYPEPROGRAM_HALFWORD (16位)
 // FLASH_TYPEPROGRAM_WORD (32位)
 // FLASH_TYPEPROGRAM_DOUBLEWORD (64位)
 uint32_t Address, // 目标地址 (必须对齐)
 uint64_t Data // 要写入的数据
);
```

#### 3.3.2 连续编程（推荐）

```c
HAL_StatusTypeDef HAL_FLASH_Program_IT(
 uint32_t TypeProgram,
 uint32_t Address,
 uint64_t Data
);
```

**编程示例**：

```c
#define FLASH_ADDR 0x08080000 // 扇区5起始地址

uint32_t data[] = {0x12345678, 0x9ABCDEF0, 0x0F0F0F0F};

HAL_FLASH_Unlock();

// 写入多个32位数据
for (int i = 0; i < 3; i++) {
 if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
FLASH_ADDR + (i * 4), 
data[i]) != HAL_OK) {
 // 错误处理
 break;
 }
}

HAL_FLASH_Lock();
```

### 3.4 选项字节操作

#### 3.4.1 编程选项字节

```c
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
```

**选项字节配置结构体**：

```c
typedef struct {
 uint32_t OptionType; // 选项类型:
 // OPTIONBYTE_WRP (写保护)
 // OPTIONBYTE_RDP (读保护)
 // OPTIONBYTE_USER (用户配置)
 // OPTIONBYTE_BOR (BOR级别)

uint32_t WRPState; // 写保护状态: OB_WRPSTATE_DISABLE/ENABLE
 uint32_t WRPSector; // 写保护扇区
 uint32_t WRPArea; // 写保护区: OB_WRPAREA_BANK1/2

uint32_t RDPLevel; // 读保护级别: 
// OB_RDP_LEVEL_0 (无保护)
 // OB_RDP_LEVEL_1 (启用保护)
 // OB_RDP_LEVEL_2 (永久保护)

uint32_t USERConfig; // 用户配置 (组合以下标志):
 // OB_USER_nRST_STOP
 // OB_USER_nRST_STDBY
 // OB_USER_IWDG_SW
 // OB_USER_WWDG_SW
 // OB_USER_BFB2

uint32_t BORLevel; // BOR级别: OB_BOR_LEVEL0~3
} FLASH_OBProgramInitTypeDef;
```

#### 3.4.2 应用选项字节

```c
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void); // 应用选项字节并复位
```

**选项字节设置示例**：

```c
FLASH_OBProgramInitTypeDef OBConfig;

// 设置读保护级别1
OBConfig.OptionType = OPTIONBYTE_RDP;
OBConfig.RDPLevel = OB_RDP_LEVEL_1;

// 设置硬件看门狗
OBConfig.OptionType |= OPTIONBYTE_USER;
OBConfig.USERConfig = OB_IWDG_SW; // 硬件独立看门狗

HAL_FLASH_Unlock();
HAL_FLASH_OB_Unlock();

if (HAL_FLASHEx_OBProgram(&OBConfig) == HAL_OK) {
 // 应用选项字节并复位
 HAL_FLASH_OB_Launch();
}

HAL_FLASH_OB_Lock();
HAL_FLASH_Lock();
```

### 3.5 Flash 中断处理

#### 3.5.1 中断服务函数

```c
void FLASH_IRQHandler(void); // Flash中断处理函数
```

#### 3.5.2 中断回调函数

```c
// 编程完成回调
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);

// 操作错误回调
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
```

### 3.6 Flash 状态和错误处理

#### 3.6.1 获取错误状态

```c
uint32_t HAL_FLASH_GetError(void);
```

**错误代码**：

```c
HAL_FLASH_ERROR_NONE // 无错误
HAL_FLASH_ERROR_PGA // 编程对齐错误
HAL_FLASH_ERROR_WRP // 写保护错误
HAL_FLASH_ERROR_OPTV // 选项字节错误
HAL_FLASH_ERROR_SIZE // 大小错误
HAL_FLASH_ERROR_RD // 读保护错误
HAL_FLASH_ERROR_OPERA // 操作错误
```

#### 3.6.2 获取操作状态

```c
FLASH_ProcedureTypeDef FLASH_GetProcedure(void);
```

**返回值**：

```c
FLASH_PROC_NONE // 无操作
FLASH_PROC_PAGEERASE // 页擦除中
FLASH_PROC_MASSERASE // 整片擦除中
FLASH_PROC_PROGRAM // 编程中
```

### 3.7 高级技巧

#### 3.7.1 磨损均衡

```c
// 在多个扇区间轮换存储位置
static uint32_t current_sector = FLASH_SECTOR_5;

void Write_With_Wear_Leveling(uint32_t data) {
  if (current_offset >= SECTOR_SIZE) {
    Erase_Sector(current_sector);
    current_sector = (current_sector == FLASH_SECTOR_5) ? 
                     FLASH_SECTOR_6 : FLASH_SECTOR_5;
    current_offset = 0;
  }
  Program_Word(SECTOR_ADDR(current_sector) + current_offset, data);
  current_offset += 4;
}
```

#### 3.7.2 掉电保护

```c
// 检测电压下降提前终止操作
if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) {
  // 立即终止Flash操作
  __disable_irq();
  FLASH->CR &= ~FLASH_CR_PER;  // 禁用擦除
  FLASH->CR &= ~FLASH_CR_PG;   // 禁用编程
  __enable_irq();
}
```

#### 3.7.3 CRC校验

```c
// 写入数据时存储CRC
uint32_t crc = HAL_CRC_Calculate(&hcrc, data, data_size/4);
Program_Word(address + data_size, crc);

// 读取时验证CRC
uint32_t stored_crc = *(__IO uint32_t*)(address + data_size);
if (HAL_CRC_Calculate(&hcrc, data, data_size/4) != stored_crc) {
  // 数据损坏处理
}
```

#### 3.7.4 内存加速读取

```c
// 启用Flash加速（预取和ART加速器）
__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_5); // 根据时钟频率设置
```

使用内部Flash存储数据时，务必注意：

1. 避免频繁擦写同一扇区（Flash有擦写次数限制，通常10K次）

2. 预留足够的存储空间（最小擦除单位是扇区）

3. 关键数据应有备份机制

4. 考虑意外掉电情况下的数据完整性

---
