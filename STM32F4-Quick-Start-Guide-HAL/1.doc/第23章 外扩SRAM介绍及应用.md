# 第二十三章  外扩SRAM介绍及应用

## 1. IS62WV51216 芯片介绍

**存储特性：**

- **存储容量与组织：** 8Mbit (兆比特) 静态RAM，组织形式为 512K 字 × 16 位。
- **高速存取时间：** 提供 45ns 和 55ns 两种高速存取时间。
- **全静态操作：** 无需时钟或刷新操作，简化了设计。

**功耗特性：**

- **CMOS 低功耗操作：** 具有低功耗特性，典型工作电流为 36mW，典型 CMOS 待机电流为 12µW。 新版本的IS62/65WV51216HALL/BLL型号，其典型CMOS待机电流可低至 3.2 µA (25°C)。
- **待机模式：** 当芯片未被选中时（例如，CS1 为高或 CS2 为低，或 CS1 为低、CS2 为高且 LB 和 UB 都为高），器件进入待机模式，此时功耗显著降低。

**电压和接口：**

- **单电源供电：**
  - IS62WV51216ALL 版本：1.65V 至 2.2V VDD。
  - IS62WV51216BLL 版本：2.5V 至 3.6V VDD。
  - 新版本的 IS62/65WV51216HALL 支持 1.65V-2.2V VDD，IS62/65WV51216HBLL 支持 2.2V-3.6V VDD。
- **TTL 兼容接口电平：** 输入/输出接口与 TTL 标准兼容。

**控制和封装：**

- **三态输出：** 具有三态输出功能。
- **字节控制：** 支持上下字节 (UB 和 LB) 的数据控制，允许对单个字节进行读写操作。
- **易于存储器扩展：** 通过使用片选 (Chip Enable) 和输出使能 (Output Enable) 输入实现存储器扩展。
- **封装类型：** 通常采用 JEDEC 标准 48 引脚 mini BGA (7.2mm x 8.7mm) 和 44 引脚 TSOP (TYPE II) 封装。

## 2. 外扩SRAM应用示例

### 2.1 相关宏定义

```c
#ifndef __SRAM_H
#define __SRAM_H

#include "sys.h"

extern SRAM_HandleTypeDef sram_hanle;

/* FSMC相关参数 定义 
 * 注意: 我们默认是通过FSMC块3来连接SRAM, 块1有4个片选: FSMC_NE1~4
 *
 * 修改SRAM_FSMC_NEX, 对应的SRAM_CS_GPIO相关设置也得改
 */
#define SRAM_FSMC_NEX           3         /* 使用FSMC_NE3接SRAM_CS,取值范围只能是: 1~4 */
#define SRAM_FSMC_BCRX          FSMC_Bank1->BTCR[(SRAM_FSMC_NEX - 1) * 2]       /* BCR寄存器,根据SRAM_FSMC_NEX自动计算 */
#define SRAM_FSMC_BTRX          FSMC_Bank1->BTCR[(SRAM_FSMC_NEX - 1) * 2 + 1]   /* BTR寄存器,根据SRAM_FSMC_NEX自动计算 */
#define SRAM_FSMC_BWTRX         FSMC_Bank1E->BWTR[(SRAM_FSMC_NEX - 1) * 2]      /* BWTR寄存器,根据SRAM_FSMC_NEX自动计算 */

/* SRAM基地址, 根据 SRAM_FSMC_NEX 的设置来决定基址地址
 * 我们一般使用FSMC的块1(BANK1)来驱动SRAM, 块1地址范围总大小为256MB,均分成4块:
 * 存储块1(FSMC_NE1)地址范围: 0X6000 0000 ~ 0X63FF FFFF
 * 存储块2(FSMC_NE2)地址范围: 0X6400 0000 ~ 0X67FF FFFF
 * 存储块3(FSMC_NE3)地址范围: 0X6800 0000 ~ 0X6BFF FFFF
 * 存储块4(FSMC_NE4)地址范围: 0X6C00 0000 ~ 0X6FFF FFFF
 */
#define SRAM_BASE_ADDR         (0X60000000 + (0X4000000 * (SRAM_FSMC_NEX - 1)))

void sram_init(void);
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen);
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen);
void sram_test_write(uint32_t addr, uint8_t data);
uint8_t sram_test_read(uint32_t addr);

#endif /* __SRAM_H__ */
```

### 2.2 FSMC-SRAM初始化

```c
void sram_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAM_TimingTypeDef fsmc_handle;
    __HAL_RCC_FSMC_CLK_ENABLE(); 
    __HAL_RCC_GPIOD_CLK_ENABLE(); 
    __HAL_RCC_GPIOE_CLK_ENABLE(); 
    __HAL_RCC_GPIOF_CLK_ENABLE(); 
    __HAL_RCC_GPIOG_CLK_ENABLE();
    // WR-PD5 RD-PD4 CS-PG10
    GPIO_InitStructure.Pin = GPIO_PIN_10;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_5|GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    // PD0,1,4,5,8~15
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    // PE0,1,7~15
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    // PF0~5,12~15
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
    // PG0~5,10
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    // FSMC Configuration
    sram_handle.Instance = FSMC_NORSRAM_DEVICE;
    sram_handle.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    sram_handle.Init.NSBank = (SRAM_FSMC_NEX == 1) ? FSMC_NORSRAM_BANK1 : \
                              (SRAM_FSMC_NEX == 2) ? FSMC_NORSRAM_BANK2 : \
                              (SRAM_FSMC_NEX == 3) ? FSMC_NORSRAM_BANK3 : FSMC_NORSRAM_BANK4; /* 根据配置选择FSMC_NE1~4 */
    sram_handle.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; // 地址/数据线不复用
    sram_handle.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM; // SRAM
    sram_handle.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; // 16位数据宽度
    sram_handle.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; // 突发访问模式关闭
    sram_handle.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; // 等待信号极性低
    sram_handle.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS; // 等待信号在前
    sram_handle.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE; // 写使能
    sram_handle.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE; // 等待信号关闭
    sram_handle.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE; // 扩展模式关闭
    sram_handle.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; // 异步等待关闭
    sram_handle.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE; // 写突发关闭
    /* FSMC读时序控制寄存器 */
    fsmc_handle.AddressSetupTime = 0x02; // 地址设置时间
    fsmc_handle.AddressHoldTime = 0x00; // 地址保持时间
    fsmc_handle.DataSetupTime = 0x08; // 数据设置时间
    fsmc_handle.BusTurnAroundDuration = 0x00; // 总线转向持续时间
    fsmc_handle.AccessMode = FSMC_ACCESS_MODE_A; // 访问模式A
    HAL_SRAM_Init(&sram_handle, &fsmc_handle, &fsmc_handle); // 初始化SRAM
}
```

### 2.3 SRAM读写

```c
/**
 * @brief       往SRAM指定地址写入指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大32bit)
 * @retval      无
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}

/**
 * @brief       从SRAM指定地址读取指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大32bit)
 * @retval      无
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *pbuf++ = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        addr++;
    }
}
```

### 2.4 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "SerialInvoker.h"
#include "sram.h"

// 测试数据
// uint32_t test_data[250000] __attribute__((at(SRAM_BASE_ADDR))); // AC5写法
uint32_t test_data[250000] __attribute__((section(".ARM.__at_0x68000000"))); // AC6写法

// 外扩SRAM测试
void sram_test(uint16_t x, uint16_t y)
{
  uint32_t i = 0;
  uint8_t temp = 0;
  uint16_t addr = 0; // 在地址0读到的数据
  LCD_ShowString(x,y,230,y+16,16,"Ex Memory Test:   0KB");
  // 每隔4k字节写入一个数据，总共写入256次，即写入1MB数据
  for(i=0;i<1024*1024;i+=4096)
  {
    sram_write(&temp,i,1);
    temp++;
  }
  // 读取数据进行校验
  for(i=0;i<1024*1024;i+=4096)
  {
    sram_read(&temp,i,1);
    if(i == 0)
    {
      addr = temp; // 记录地址0读到的数据
    }
    else if(temp <= addr)
    {
      break; // 校验失败，退出循环
    }
    LCD_ShowxNum(x+15*8,y,(uint16_t)(temp-addr+1)*4,4,16,0);
  }
}

int main(void)
{
  uint8_t key_value = 0;
  uint8_t i = 0;
  uint32_t data = 0;
  bsp_init();
  sram_init();
    serial_invoker_init(84);
  LCD_ShowString(30,110,200,16,16,"KEY0:Test Sram");
  LCD_ShowString(30,130,200,16,16,"KEY1:TEST Data");
  for(data=0;data<250000;data++)
  {
    test_data[data] = data; // 初始化测试数据
  }
  while(1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      sram_test(30,150); // 测试外扩SRAM
    }
    else if(key_value == KEY1_Press)
    {
      for(data=0;data<250000;data++)
      {
        LCD_ShowxNum(30,170,test_data[data],6,16,0); // 显示测试数据
      }
    }
    else
    {
      delay_ms(10);
    }
    i++;
    if(i == 10)
    {
      i = 0;
      LED_TOGGLE(LED0_GPIO_Pin);
    }
  }
}
```

## 3. FSMC外扩SRAM相关函数（HAL库）

### 3.1 FSMC 初始化与配置

#### 3.1.1 SRAM 句柄与初始化结构体

```c
SRAM_HandleTypeDef hsram;
FSMC_NORSRAM_TimingTypeDef Timing;
```

#### 3.1.2 配置时序参数

```c
/* 地址建立时间、数据建立时间等 (单位: HCLK周期) */
Timing.AddressSetupTime = 2; // 地址建立时间
Timing.AddressHoldTime = 1; // 地址保持时间 (模式A/D)
Timing.DataSetupTime = 2; // 数据建立时间 (关键参数)
Timing.BusTurnAroundDuration = 0; // 总线周转时间
Timing.CLKDivision = 0; // 时钟分频
Timing.DataLatency = 0; // 数据延迟
Timing.AccessMode = FSMC_ACCESS_MODE_A; // 访问模式
```

#### 3.1.3 配置 SRAM 初始化结构体

```c
hsram.Instance = FSMC_NORSRAM_DEVICE;
hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
hsram.Init.NSBank = FSMC_NORSRAM_BANK1; // 使用BANK1
hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; // 16位总线
hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
```

#### 3.1.4 初始化 FSMC

```c
// 初始化FSMC并关联时序
HAL_SRAM_Init(&hsram, &Timing, &Timing);

// 启用FSMC时钟
__HAL_RCC_FSMC_CLK_ENABLE();
```

### 3.2 SRAM 读写操作函数

#### 3.2.1 阻塞式读写

```c
// 16位写操作
HAL_StatusTypeDef HAL_SRAM_Write_16b(
 SRAM_HandleTypeDef *hsram,
 uint32_t *pAddress, // SRAM地址 (0x6xxxxxxx)
 uint16_t *pSrcBuffer, // 数据缓冲区
 uint32_t BufferSize // 数据大小 (单位: 16位字)
);

// 16位读操作
HAL_StatusTypeDef HAL_SRAM_Read_16b(
 SRAM_HandleTypeDef *hsram,
 uint32_t *pAddress,
 uint16_t *pDstBuffer,
 uint32_t BufferSize
);
```

#### 3.2.2 其他数据宽度

```c
// 8位读写
HAL_SRAM_Write_8b();
HAL_SRAM_Read_8b();

// 32位读写
HAL_SRAM_Write_32b();
HAL_SRAM_Read_32b();
```

#### 3.2.3 中断模式读写

```c
// 中断写
HAL_StatusTypeDef HAL_SRAM_Write_16b_IT(
 SRAM_HandleTypeDef *hsram,
 uint32_t *pAddress,
 uint16_t *pSrcBuffer,
 uint32_t BufferSize
);

// 中断读
HAL_StatusTypeDef HAL_SRAM_Read_16b_IT(...);
```

#### 3.2.4 DMA 模式读写

```c
// DMA写
HAL_StatusTypeDef HAL_SRAM_Write_16b_DMA(
 SRAM_HandleTypeDef *hsram,
 uint32_t *pAddress,
 uint16_t *pSrcBuffer,
 uint32_t BufferSize
);

// DMA读
HAL_StatusTypeDef HAL_SRAM_Read_16b_DMA(...);
```

### 3.3 回调函数 (中断/DMA模式)

```c
// 写完成回调
void HAL_SRAM_TxCpltCallback(SRAM_HandleTypeDef *hsram);

// 读完成回调
void HAL_SRAM_RxCpltCallback(SRAM_HandleTypeDef *hsram);

// DMA错误回调
void HAL_SRAM_DMAError(DMA_HandleTypeDef *hdma);
```

### 3.4 SRAM 控制函数

```c
// 启用/禁用写操作
void HAL_SRAM_WriteOperation_Enable(SRAM_HandleTypeDef *hsram);
void HAL_SRAM_WriteOperation_Disable(SRAM_HandleTypeDef *hsram);
```

---


