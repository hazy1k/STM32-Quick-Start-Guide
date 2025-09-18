# 第十九章 内部FLASH介绍及应用

## 1. 内部 Flash 简介

STM32H750VBT6 的**内部 Flash** 是嵌入在芯片中的非易失性存储器，用于存储 **程序代码（固件）、常量数据、配置参数** 等，即使断电也能保留内容。它是系统启动和运行的核心，支持 **XIP（eXecute In Place）** ——即 CPU 直接从 Flash 执行指令，无需加载到 RAM。

> 🔍 **核心定位**：
> 
> - **内部 Flash ≠ 外部存储**，而是**代码运行的“主仓库”**
> - 与 **QSPI Flash** 协同实现灵活存储架构
> - 支持 **读取、编程（写入）、擦除** 操作
> - 是 **Bootloader、固件升级、参数存储** 的基础

---

### 1.1 内部 Flash 核心特性（STM32H750VBT6）

| **特性**   | **参数**                       | **说明**                       | **应用场景**            |
| -------- | ---------------------------- | ---------------------------- | ------------------- |
| **容量**   | **128 KB**                   | 小容量高性能芯片                     | 适合轻量级应用             |
| **架构**   | 基于 **QSPI 接口 + 外部引脚复用**      | 实际为“虚拟内部Flash”               | 节省封装成本              |
| **物理存储** | 外部 QSPI NOR Flash 封装在芯片内     | 通过 QSPI 接口访问                 | 启动时映射到 `0x08000000` |
| **执行速度** | 支持 **0 等待状态**（WS=0）@ 480 MHz | 配合 ART Accelerator（自适应实时加速器） | 高性能运行               |
| **擦除单位** | 扇区（Sector）                   | 最小擦除单位                       | 编程前必须擦除             |
| **编程单位** | 32-bit 字或 64-bit 双字          | 最小写入单位                       | 不支持字节写入             |
| **耐久性**  | ≥ 100,000 次擦写                | 可靠用于参数存储                     | 频繁更新需注意             |
| **数据保持** | ≥ 20 年                       | 断电后长期保存                      | 工业级可靠性              |

📌 **STM32H750VBT6 特殊架构说明**：

- **H750 是“Flashless”型号的变种**，其 128 KB “内部 Flash” 实际是一个 **封装内 QSPI NOR Flash**（Micron MT25TL01G），通过芯片内部连接到 QSPI 控制器。
- 启动时自动映射到地址 `0x08000000`，表现为“内部 Flash”。
- **因此，Flash 操作实际是 QSPI 操作**，但 STM32 HAL 提供了统一接口（`HAL_FLASH_*`），简化使用。

---

### 1.2 存储结构与地址映射

#### 1.2.1 地址空间布局

| **区域**           | **起始地址**           | **大小** | **用途**           |
| ---------------- | ------------------ | ------ | ---------------- |
| **主 Flash**      | `0x0800 0000`      | 128 KB | 用户程序、常量          |
| **系统存储区**        | `0x1FF0 9800`      | 30 KB  | ST 出厂 Bootloader |
| **Option Bytes** | `0x0800 0000` + OB | 512 B  | 用户配置（读保护、BOR等）   |
| **OTP 区域**       | `0x1FF0 F400`      | 512 B  | 一次性编程（UID、密钥）    |

- **Flash 分为 8 个扇区**（Sector），每扇区 16 KB：
  
  ```c
  Sector 0: 0x08000000 – 0x08003FFF (16 KB)
  Sector 1: 0x08004000 – 0x08007FFF (16 KB)
  ...
  Sector 7: 0x0801C000 – 0x0801FFFF (16 KB)
  ```

#### 1.2.2 启动模式与 Flash 选择

| **BOOT0/1** | **启动区域**              | **说明**                         |
| ----------- | --------------------- | ------------------------------ |
| `BOOT0=0`   | 主 Flash（`0x08000000`） | 正常运行用户程序                       |
| `BOOT0=1`   | 系统存储区（`0x1FF09800`）   | 进入 ST Bootloader（UART/USB DFU） |

---

### 1.3 关键操作：擦除与编程

#### 1.3.1 Flash 操作流程

1. **解锁 Flash**（写 `KEY` 到 `KEYR`）
2. **擦除扇区**（必须先擦再写）
3. **编程数据**（写入字/双字）
4. **锁定 Flash**（防止误操作）

#### 1.3.2 寄存器级操作（以擦除扇区 0 为例）

```c
// 1. 使能 Flash 时钟
__HAL_RCC_FLASH_CLK_ENABLE();

// 2. 解锁 Flash
FLASH->KEYR = 0x45670123;
FLASH->KEYR = 0xCDEF89AB;

// 3. 等待操作完成
while (FLASH->SR & FLASH_SR_BSY);

// 4. 擦除扇区 0
FLASH->CR &= ~FLASH_CR_PSIZE;
FLASH->CR |= FLASH_CR_PSIZE_1; // 编程大小 = 32-bit
FLASH->CR |= FLASH_CR_SER;     // 扇区擦除
FLASH->CR |= (0 << 3);         // 扇区编号 = 0
FLASH->CR |= FLASH_CR_STRT;    // 开始擦除

// 5. 等待完成
while (FLASH->SR & FLASH_SR_BSY);

// 6. 清除完成标志
FLASH->SR |= FLASH_SR_EOP;

// 7. 重新锁定
FLASH->CR |= FLASH_CR_LOCK;
```

#### 1.3.3 HAL 库简化操作

```c
FLASH_EraseInitTypeDef EraseInitStruct = {0};
uint32_t SectorError = 0;

// 擦除扇区 0
EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
EraseInitStruct.Sector = FLASH_SECTOR_0;
EraseInitStruct.NbSectors = 1;
EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
    Error_Handler();
}

// 写入数据（地址必须为 32-bit 对齐）
uint32_t address = 0x08000000;
uint32_t data = 0x12345678;

if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data) != HAL_OK) {
    Error_Handler();
}

// 锁定
HAL_FLASH_Lock();
```

### 1.4 内部 Flash vs 外部 QSPI Flash 对比

| **特性**     | **内部 Flash (H750)** | **外部 QSPI Flash**    | **FMC PSRAM** |
| ---------- | ------------------- | -------------------- | ------------- |
| **容量**     | 128 KB              | 8 MB – 128 MB        | 2 MB – 64 MB  |
| **速度**     | 480 MHz (0 WS)      | 80–133 MHz           | 100–168 MHz   |
| **XIP 支持** | ✅（默认）               | ✅（需配置）               | ✅（代码可运行）      |
| **写入方式**   | 扇区擦除 + 字编程          | 命令 + 间接模式            | 直接写入          |
| **成本**     | 已包含                 | 外加 Flash 芯片          | 高（PSRAM）      |
| **引脚**     | 0（内部连接）             | 6–8（IO0–3, CLK, NCS） | 30+           |
| **典型用途**   | Bootloader、小固件      | 大固件、音频、文件            | 图形缓冲、高速缓存     |

> 💡 **选型建议**：
> 
> - **小固件、快速启动** → 内部 Flash
> - **大固件、资源存储** → 外部 QSPI Flash
> - **运行代码 + 大数据** → QSPI XIP + FMC PSRAM

## 2. 内部FLASH使用示例-STM32IDE

### 2.1 STM32Cube配置

![屏幕截图 2025-09-11 230030.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/12-14-35-18-屏幕截图%202025-09-11%20230030.png)

### 2.2 用户代码

```c
#include "stmflash.h"

/**
 * @brief       得到FLASH的错误状态
 * @param       无
 * @retval      错误代码
 *   @arg       0   , 无错误
 *   @arg       其他, 错误编号
 */
static uint8_t stmflash_get_error_status(void)
{
    uint32_t res = 0;
    res = FLASH->SR1;

    if (res & (1 << 17)) return 1;  /* WRPERR=1,写保护错误 */
    if (res & (1 << 18)) return 2;  /* PGSERR=1,编程序列错误 */
    if (res & (1 << 19)) return 3;  /* STRBERR=1,复写错误 */
    if (res & (1 << 21)) return 4;  /* INCERR=1,数据一致性错误 */
    if (res & (1 << 22)) return 5;  /* OPERR=1,写/擦除错误 */
    if (res & (1 << 23)) return 6;  /* RDPERR=1,读保护错误 */
    if (res & (1 << 24)) return 7;  /* RDSERR=1,非法访问加密区错误 */
    if (res & (1 << 25)) return 8;  /* SNECCERR=1,1bit ecc校正错误 */
    if (res & (1 << 26)) return 9;  /* DBECCERR=1,2bit ecc错误 */

    return 0;   /* 没有任何状态/操作完成. */
}

/**
 * @brief       等待操作完成
 * @param       time : 要延时的长短
 * @retval      错误代码
 *   @arg       0   : 已完成
 *   @arg       1~9 : 错误代码
 *   @arg       0XFF: 超时
 */
static uint8_t stmflash_wait_done(uint32_t time)
{
    uint8_t res = 0;
    uint32_t tempreg = 0;

    while (1)
    {
        tempreg = FLASH->SR1;

        if ((tempreg & 0X07) == 0)
        {
            break;  /* BSY=0,WBNE=0,QW=0,则操作完成 */
        }

        time--;

        if (time == 0)return 0XFF;
    }

    res = stmflash_get_error_status();

    if (res)
    {
        FLASH->CCR1 = 0X07EE0000;   /* 清所有错误标志 */
    }

    return res;
}

/**
 * @brief       擦除扇区0
 * @param       无
 * @retval      错误代码
 *   @arg       0   : 已完成
 *   @arg       1~9 : 错误代码
 *   @arg       0XFF: 超时
 */
static uint8_t stmflash_erase_sector0(void)
{
    uint8_t res = 0;
    res = stmflash_wait_done(0XFFFFFFFF);   /* 等待上次操作结束 */

    if (res == 0)
    {
        FLASH->CR1 &= ~(7 << 8);    /* SNB1[2:0]=0,清除原来的设置 */
        FLASH->CR1 &= ~(3 << 4);    /* PSIZE1[1:0]=0,清除原来的设置 */
        FLASH->CR1 |= (uint32_t)0 << 8; /* 设置要擦除的扇区编号,0 */
        FLASH->CR1 |= 2 << 4;       /* 设置为32bit宽 */
        FLASH->CR1 |= 1 << 2;       /* SER1=1,扇区擦除 */
        FLASH->CR1 |= 1 << 7;       /* START1=1,开始擦除 */
        res = stmflash_wait_done(0XFFFFFFFF);   /* 等待操作结束 */
        FLASH->CR1 &= ~(1 << 2);    /* SER1=0,清除扇区擦除标志 */
    }

    return res;
}

/**
 * @brief       在FLASH指定地址写8个字,即256bit
 *   @note      必须以256bit为单位(32字节)编程!!
 * @param       faddr : 指定地址(此地址必须为4的倍数!!)
 * @param       pdata : 要写入的数据
 * @retval      错误代码
 *   @arg       0   : 写入成功
 *   @arg       其他: 错误代码
 */
static uint8_t stmflash_write_8word(uint32_t faddr, uint32_t *pdata)
{
    volatile uint8_t nword = 8; /* 每次写8个字,256bit */
    uint8_t res;
    res = stmflash_wait_done(0XFFFF);

    if (res == 0)       /* OK */
    {
        FLASH->CR1 &= ~(3 << 4);/* PSIZE1[1:0]=0,清除原来的设置 */
        FLASH->CR1 |= 2 << 4;   /* 设置为32bit宽 */
        FLASH->CR1 |= 1 << 1;   /* PG1=1,编程使能 */

        __DSB();

        while (nword)
        {
            *(volatile uint32_t *)faddr = *pdata;   /* 写入数据 */
            faddr += 4;         /* 写地址+4 */
            pdata++;            /* 偏移到下一个数据首地址 */
            nword--;
        }

        __DSB();                /* 写操作完成后,屏蔽数据同步,使CPU重新执行指令序列 */
        res = stmflash_wait_done(0XFFFF);   /* 等待操作完成,一个字编程,最多100us. */
        FLASH->CR1 &= ~(1 << 1);/* PG1=0,清除扇区擦除标志 */
    }

    return res;
}

/**
 * @brief       读取指定地址的一个字(32位数据)
 * @param       faddr : 要读取的地址
 * @retval      读取到的数据
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

/**
 * @brief       从指定地址开始写入指定长度的数据
 *   @note      特别注意：因为STM32H750只有一个扇区（128K）,因此我们规定：前16K留作BootLoader用
 *              后112K用作APP用，我们要做写入测试，尽量使用16K以后的地址，否则容易出问题
 *              另外，由于写数据时，必须是0XFF才可以写入数据，因此不可避免的需要擦除扇区
 *              所以在擦除时需要先对前16K数据做备份保存（读取到RAM），然后再写入，以保证
 *              前16K数据的完整性。且执行写入操作的时候，不能发生任何中断（凡是在写入时执
 *              行内部FLASH代码，必将导致hardfault）。
 * @param       waddr : 起始地址(此地址必须为32的倍数!!,否则写入出错!)
 * @param       pbuf  : 数据指针
 * @param       length: 字(32位)数(就是要写入的32位数据的个数,一次至少写入32字节,即8个字)
 * @retval      无
 */

/* FLASH 写入数据缓存 */
uint32_t g_flashbuf[BOOT_FLASH_SIZE / 4];

void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
    uint8_t status = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    uint16_t wbfcyc = BOOT_FLASH_SIZE/32;    /* 写bootflashbuf时,需要执行的循环数 */
    uint32_t *wbfptr;
    uint32_t wbfaddr;

    if (waddr < (STM32_FLASH_BASE + BOOT_FLASH_SIZE))return;    /* 写入地址小于STM32_FLASH_BASE+BOOT_FLASH_SIZE,非法. */

    if (waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE))return;   /* 写入地址大于STM32总FLASH地址范围,非法. */

    if (waddr % 32)return;          /* 写入地址不是32字节倍数,非法. */

    HAL_FLASH_Unlock();             /* 解锁 */
    addrx = waddr;                  /* 写入的起始地址 */
    endaddr = waddr + length * 4;   /* 写入的结束地址 */

    while (addrx < endaddr)         /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
    {
        if (stmflash_read_word(addrx) != 0XFFFFFFFF) /* 有非0XFFFFFFFF的地方,要擦除这个扇区 */
        {
            stmflash_read(STM32_FLASH_BASE, g_flashbuf, BOOT_FLASH_SIZE / 4);  /* 读出BOOT_FLASH_SIZE大小数据 */
            sys_intx_disable(); /* 禁止所有中断 */

            status = stmflash_erase_sector0();

            if (status) /* 发生错误了 */
            {
                sys_intx_enable();    /* 允许中断 */
                break;                /* 发生错误了 */
            }

            SCB_CleanInvalidateDCache();    /* 清除无效的D-Cache */
            wbfptr = g_flashbuf;            /* 指向g_flashbuf首地址 */
            wbfaddr = STM32_FLASH_BASE;     /* 指向STM32 FLASH首地址 */

            while (wbfcyc)                  /* 写数据 */
            {
                if (stmflash_write_8word(wbfaddr, wbfptr))  /* 写入数据 */
                {
                    break;  /* 写入异常 */
                }
                wbfaddr += 32;
                wbfptr += 8;
                wbfcyc--;
            }
            sys_intx_enable();  /* 允许中断 */
        }
        else
        {
            addrx += 4;         /* 偏移到下一个位置 */
        }
    }

    if (status == HAL_OK)
    {
        while (waddr < endaddr) /* 写数据 */
        {
            if (stmflash_write_8word(waddr, pbuf))  /* 写入数据 */
            {
                break;  /* 写入异常 */
            }

            waddr += 32;
            pbuf += 8;
        }
    }

    HAL_FLASH_Lock();           /* 上锁 */
}

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
        pbuf[i] = stmflash_read_word(raddr);    /* 读取4个字节. */
        raddr += 4; /* 偏移4个字节. */
    }
}

/******************************************************************************************/
/* 测试用代码 */

/**
 * @brief       测试写数据(写1个字)
 * @param       waddr : 起始地址
 * @param       wdata : 要写入的数据
 * @retval      读取到的数据
 */
void test_write(uint32_t waddr, uint32_t wdata)
{
    stmflash_write(waddr, &wdata, 1);   /* 写入一个字 */
}

```

```c
#include "main.h"
#include "bsp_init.h"
#include "stmflash.h"
#include <stdio.h>

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
//static void MPU_Config(void);

// 要写入到FLASH的字符串
const uint8_t test_str[] = "Hello Flash!";
#define text_size sizeof(test_str)
/*SIZE表示字长(4字节), 大小必须是4的整数倍, 如果不是的话, 强制对齐到4的整数倍 */
#define SIZE  text_size / 4 + ((text_size % 4) ? 1 : 0)
/* 设置FLASH 保存地址(必须大于用户代码区地址范围,且为4的倍数) */
#define FLASH_SAVE_ADDR     0X08004000

int main(void)
{
  uint8_t temp_data[SIZE];
  //MPU_Config();
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  bsp_init();
  while (1)
  {
    printf("Write to Flash\r\n");
    HAL_Delay(1000);
    stmflash_write(FLASH_SAVE_ADDR, (uint32_t*)test_str, SIZE);
    HAL_Delay(3000);
    printf("Write Done\r\n");
    HAL_Delay(1000);

	printf("Read from Flash\r\n");
	HAL_Delay(1000);
	stmflash_read(FLASH_SAVE_ADDR, (uint32_t*)temp_data, SIZE);
	HAL_Delay(3000);
	printf("Read Done\r\n");
	printf("Data: %s\r\n", temp_data);
    HAL_Delay(3000);
  }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

```

## 3. 内部Flash相关函数总结（HAL库）

### 3.1 初始化与配置

- **核心操作流程**（四步关键操作）：
  
  1. **解锁Flash**（解除写保护）
  2. **擦除扇区**（必须先擦除才能写入）
  3. **写入数据**（按编程单位写入）
  4. **锁定Flash**（恢复写保护）

- `HAL_FLASH_Unlock()`  
  解除Flash写保护，**必须在所有写/擦除操作前调用**：
  
  ```c
  // 解锁Flash控制器
  HAL_FLASH_Unlock();
  
  // 之后可进行擦除/编程操作
  // ...
  
  // 操作完成后必须锁定
  HAL_FLASH_Lock();
  ```

- **`FLASH_OBProgramInitTypeDef` 选项字节配置**：
  
  | **成员**            | **说明** | **关键选项**                            | **H750特殊说明** |
  | ----------------- | ------ | ----------------------------------- | ------------ |
  | `OptionType`      | 配置类型   | `OPTIONBYTE_WRP`, `RDP`, `USER`     | 可组合          |
  | `WRPState`        | 写保护状态  | `OB_WRPSTATE_ENABLE`, `DISABLE`     |              |
  | `WRPStartAddress` | 保护起始地址 | `0x08000000`                        | 扇区对齐         |
  | `WRPEndAddress`   | 保护结束地址 | `0x080FFFFF`                        |              |
  | `RDPLevel`        | 读保护等级  | `OB_RDP_LEVEL_0`, `1`, `2`          | Level 2不可逆   |
  | `USERConfig`      | 用户选项   | `OB_USER_nRST_STOP`, `nRST_STANDBY` |              |

- **Flash扇区划分**（H750为例）：
  
  | **扇区**   | **地址范围**              | **大小** | **用途建议** |
  | -------- | --------------------- | ------ | -------- |
  | Sector 0 | 0x08000000-0x08007FFF | 32KB   | 引导加载程序   |
  | Sector 1 | 0x08008000-0x0800FFFF | 32KB   |          |
  | Sector 2 | 0x08010000-0x0801FFFF | 64KB   | 应用程序     |
  | Sector 3 | 0x08020000-0x0803FFFF | 128KB  |          |
  | Sector 4 | 0x08040000-0x0807FFFF | 256KB  |          |
  | Sector 5 | 0x08080000-0x080FFFFF | 512KB  |          |
  | Sector 6 | 0x08100000-0x081FFFFF | 1MB    |          |
  | Sector 7 | 0x08200000-0x083FFFFF | 2MB    | 数据存储     |

- **编程单位**（必须遵守）：
  
  | **数据宽度**   | **编程单位** | **H750说明** |
  | ---------- | -------- | ---------- |
  | Byte       | 1字节      | 推荐使用       |
  | HalfWord   | 2字节      | 对齐地址       |
  | Word       | 4字节      | 对齐地址       |
  | DoubleWord | 8字节      | 对齐地址       |

### 3.2 Flash操作核心函数

- **基础擦除操作**：
  
  | **函数**                   | **原型**                         | **特点** | **应用场景** |
  | ------------------------ | ------------------------------ | ------ | -------- |
  | `HAL_FLASHEx_Erase()`    | `(pEraseInit, SectorError)`    | 扇区擦除   | 清除数据     |
  | `HAL_FLASHEx_Erase_IT()` | `(pEraseInit)`                 | 中断擦除   | 非阻塞      |
  | `HAL_FLASH_Program()`    | `(TypeProgram, Address, Data)` | 编程写入   | 写数据      |
  | `HAL_FLASH_Program_IT()` | `(TypeProgram, Address, Data)` | 中断编程   |          |

- **扇区擦除配置**：
  
  ```c
  // 配置扇区擦除参数
  FLASH_EraseInitTypeDef erase_init = {0};
  erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;        // 擦除类型
  erase_init.Sector = FLASH_SECTOR_7;                   // 扇区7
  erase_init.NbSectors = 1;                             // 擦除1个扇区
  erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;      // 电压范围
  
  uint32_t sector_error = 0;
  HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
  
  if (status != HAL_OK) {
      // 处理擦除错误
      if (sector_error != 0xFFFFFFFF) {
          // 指定扇区擦除失败
      }
  }
  ```

- **数据写入操作**：
  
  ```c
  // 写入32位数据
  HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                                              0x08100000, 
                                              0x12345678);
  
  // 写入64位数据
  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                            0x08100000, 
                            0x1122334455667788ULL);
  
  // 写入数组（循环写入）
  uint32_t data[] = {0x11111111, 0x22222222, 0x33333333};
  for (int i = 0; i < 3; i++) {
      status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                                0x08100000 + i*4, 
                                data[i]);
      if (status != HAL_OK) break;
  }
  ```

- **选项字节操作**：
  
  ```c
  // 读取选项字节
  FLASH_OBProgramInitTypeDef ob_init = {0};
  HAL_FLASHEx_OBGetConfig(&ob_init);
  
  // 配置选项字节
  ob_init.OptionType = OPTIONBYTE_WRP;
  ob_init.WRPState = OB_WRPSTATE_ENABLE;
  ob_init.WRPStartAddress = FLASH_SECTOR_6;
  ob_init.WRPEndAddress = FLASH_SECTOR_7;
  
  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();
  HAL_FLASHEx_OBProgram(&ob_init);
  HAL_FLASH_OB_Launch();  // 应用选项字节
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();
  ```

### 3.3 高级功能与特性

- **双区Flash操作**（Bank Swap）：
  
  ```c
  // H750支持双Bank Flash
  erase_init.TypeErase = FLASH_TYPEERASE_MASSERASE;
  erase_init.Banks = FLASH_BANK_1;  // 或 FLASH_BANK_2
  
  // 扇区擦除指定Bank
  erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
  erase_init.Banks = FLASH_BANK_1;
  erase_init.Sector = FLASH_SECTOR_3;
  ```

- **PCROP保护**（私有代码读出保护）：
  
  ```c
  // 配置PCROP保护
  FLASH_PCROPConfigTypeDef pcrop_config = {0};
  pcrop_config.Sector = FLASH_PCROP_ZONE_A;
  pcrop_config.StartAddress = 0x08020000;
  pcrop_config.EndAddress = 0x0803FFFF;
  pcrop_config.Banks = FLASH_BANK_1;
  
  HAL_FLASHEx_EnablePCROP(&pcrop_config);
  ```

- **Flash中断处理**：
  
  ```c
  // 擦除完成中断回调
  void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
  {
      if (ReturnValue == HAL_OK) {
          flash_operation_complete = 1;
      }
  }
  
  // 错误中断回调
  void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
  {
      flash_error_occurred = 1;
      // 清除错误标志
      __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);
  }
  
  // 中断服务函数
  void FLASH_IRQHandler(void)
  {
      HAL_FLASH_IRQHandler();
  }
  ```

- **读出保护等级**：
  
  | **等级**  | **功能** | **可逆性**    | **H750说明** |
  | ------- | ------ | ---------- | ---------- |
  | Level 0 | 无保护    | 可设置Level 1 |            |
  | Level 1 | 防止调试访问 | 可降级        | 推荐使用       |
  | Level 2 | 完全锁定   | **不可逆**    | 慎用         |

### 3.4 使用示例（完整流程）

#### 3.4.1 示例1：Flash数据存储

```c
#define DATA_SECTOR    FLASH_SECTOR_7
#define DATA_ADDRESS   0x08200000
#define DATA_SIZE      1024

// 1. 擦除扇区
HAL_StatusTypeDef Erase_Flash_Sector(uint32_t sector)
{
    FLASH_EraseInitTypeDef erase_init = {0};
    uint32_t sector_error = 0;
    HAL_StatusTypeDef status;

    // 解锁Flash
    HAL_FLASH_Unlock();

    // 配置擦除参数
    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.Sector = sector;
    erase_init.NbSectors = 1;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    // 执行擦除
    status = HAL_FLASHEx_Erase(&erase_init, &sector_error);

    // 锁定Flash
    HAL_FLASH_Lock();

    return status;
}

// 2. 写入数据到Flash
HAL_StatusTypeDef Write_Data_To_Flash(uint32_t *data, uint32_t size)
{
    HAL_StatusTypeDef status = HAL_OK;

    // 解锁Flash
    HAL_FLASH_Unlock();

    // 按字写入
    for (uint32_t i = 0; i < size; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                                  DATA_ADDRESS + i*4, 
                                  data[i]);
        if (status != HAL_OK) break;
    }

    // 锁定Flash
    HAL_FLASH_Lock();

    return status;
}

// 3. 读取数据（直接访问）
uint32_t Read_Data_From_Flash(uint32_t offset)
{
    return *(uint32_t*)(DATA_ADDRESS + offset);
}

// 4. 使用示例
void Flash_Data_Example(void)
{
    uint32_t test_data[256];

    // 初始化测试数据
    for (int i = 0; i < 256; i++) {
        test_data[i] = i * 1000;
    }

    // 1. 擦除目标扇区
    if (Erase_Flash_Sector(DATA_SECTOR) != HAL_OK) {
        error_handler();
    }

    // 2. 写入数据
    if (Write_Data_To_Flash(test_data, 256) != HAL_OK) {
        error_handler();
    }

    // 3. 验证读取
    for (int i = 0; i < 256; i++) {
        if (Read_Data_From_Flash(i*4) != test_data[i]) {
            error_handler();
        }
    }
}
```

#### 3.4.2 示例2：固件升级（Bootloader）

```c
#define APP_START_SECTOR   FLASH_SECTOR_2
#define APP_START_ADDRESS  0x08010000
#define APP_SIZE           0x20000  // 128KB

// 1. 擦除应用程序区域
HAL_StatusTypeDef Erase_Application_Area(void)
{
    FLASH_EraseInitTypeDef erase_init = {0};
    uint32_t sector_error = 0;

    HAL_FLASH_Unlock();

    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.Sector = APP_START_SECTOR;
    erase_init.NbSectors = 3;  // 扇区2,3,4 (32+64+128=224KB)
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &sector_error);

    HAL_FLASH_Lock();
    return status;
}

// 2. 写入新固件
HAL_StatusTypeDef Program_Application(uint8_t *firmware, uint32_t size)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t address = APP_START_ADDRESS;

    HAL_FLASH_Unlock();

    // 按32位对齐写入
    for (uint32_t i = 0; i < size; i += 4) {
        uint32_t word = 0;
        for (int j = 0; j < 4; j++) {
            if (i+j < size) {
                word |= ((uint32_t)firmware[i+j]) << (j*8);
            }
        }
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, word);
        if (status != HAL_OK) break;
    }

    HAL_FLASH_Lock();
    return status;
}

// 3. 验证固件完整性
uint32_t Calculate_CRC32(uint8_t *data, uint32_t size)
{
    // CRC32计算函数
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < size; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

// 4. 启动应用程序
void Jump_To_Application(void)
{
    // 验证应用程序向量表
    if (((*(volatile uint32_t*)APP_START_ADDRESS) & 0x2FFE0000) == 0x20000000) {
        // 设置主堆栈指针
        uint32_t msp_value = *(volatile uint32_t*)APP_START_ADDRESS;
        __set_MSPLIM((uint32_t)SRAM1_BASE);
        __set_MSP(msp_value);

        // 跳转到复位处理程序
        uint32_t reset_handler = *(volatile uint32_t*)(APP_START_ADDRESS + 4);
        void (*app_reset)(void) = (void (*)(void))reset_handler;

        // 禁用所有中断
        __disable_irq();

        // 跳转
        app_reset();
    }
}
```

## 4. 关键注意事项

1. **擦除写入顺序**：
   
   - **必须先擦除**，再写入
   - **擦除后所有位为1**（0xFF）
   - **写入只能将1变为0**，不能将0变为1

2. **地址对齐要求**：
   
   - **Word编程**：地址必须4字节对齐
     
     ```c
     // 错误！未对齐
     HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x08100002, data);
     ```
   
   - **DoubleWord编程**：地址必须8字节对齐
   
   - **正确做法**：
     
     ```c
     // 确保地址对齐
     address = (address + 3) & ~0x3;  // 4字节对齐
     ```

3. **电压范围配置**：
   
   - **电压范围**必须匹配VDD：
     
     | **电压**   | **配置**    | **H750推荐** |
     | -------- | --------- | ---------- |
     | 1.7-2.1V | `RANGE_1` |            |
     | 2.1-2.7V | `RANGE_2` |            |
     | 2.7-3.6V | `RANGE_3` | ✅ 推荐       |
     | 2.7-3.0V | `RANGE_4` |            |

4. **Flash寿命**：
   
   - **擦除次数**：≥ 100,000次/扇区
   
   - **数据保持**：≥ 20年
   
   - **寿命管理技巧**：
     
     ```c
     // 避免频繁擦写同一扇区
     // 使用磨损均衡算法
     static uint8_t current_sector = 0;
     uint32_t sector_address = 0x08200000 + (current_sector * SECTOR_SIZE);
     current_sector = (current_sector + 1) % 8;  // 轮转使用
     ```

5. **中断与实时性**：
   
   - **擦除操作耗时**：
     
     - 扇区擦除：100-300ms
     - 全片擦除：数秒
   
   - **中断处理**：
     
     ```c
     // 在擦除期间，中断可能被阻塞
     // 使用中断模式避免阻塞
     HAL_FLASHEx_Erase_IT(&erase_init);
     ```

---

### 4.1 H750特有优化技巧

| **场景**      | **解决方案**    | **优势** | **实现要点**                    |
| ----------- | ----------- | ------ | --------------------------- |
| **双Bank操作** | Bank1+Bank2 | 可在线升级  | `FLASH_BANK_1/2`            |
| **高速写入**    | 64位编程       | 速度×2   | `DOUBLEWORD`模式              |
| **固件保护**    | PCROP       | 防止读出   | `HAL_FLASHEx_EnablePCROP()` |
| **数据完整性**   | 页缓冲         | 写入优化   | 先存RAM再写Flash                |

> **避坑指南**：
> 
> 1. **RDP Level 2陷阱**：
>    
>    - 一旦设置Level 2读保护，**无法解除**
>    - 只能通过**芯片清除**（Mass Erase）恢复
> 
> 2. **Cache一致性**：
>    
>    ```c
>    // 写Flash后必须清除指令Cache
>    SCB_InvalidateICache();
>    // 若从写入区域执行代码，必须清除指令Cache
>    ```
> 
> 3. **H750双区Flash**：
>    
>    - 支持同时擦除两个Bank
>    - 可配置Bank Swap功能
> 
> 4. **写保护配置**：
>    
>    - 选项字节中的写保护**优先级高于**软件写保护
>    - 仔细核对保护范围

---

### 4.2 Flash操作时间参考

| **操作**     | **典型时间**  | **影响因素** | **H750说明** |
| ---------- | --------- | -------- | ---------- |
| Word编程     | 12.5ns    | 电压/温度    | 80MHz系统时钟  |
| Sector擦除   | 100-300ms | 扇区大小     | 大小不同       |
| Mass擦除     | 2-5s      | 总容量      |            |
| Option字节编程 | 5-10ms    | 配置项      |            |

> **重要提示**：
> 
> - Flash操作**必须解锁**才能执行
> - **擦除是写入的前提**，不能跳过
> - 高级别读保护（Level 2）**不可逆**，慎用
> - 写入后建议验证数据完整性
> - 考虑Flash寿命，避免频繁擦写

---


