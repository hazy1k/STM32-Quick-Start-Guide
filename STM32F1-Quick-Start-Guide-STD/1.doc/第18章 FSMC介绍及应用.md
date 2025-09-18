# 第十八章 FSMC介绍及应用

## 1. FSMC简介

STM32F1 系列芯片使用 FSMC 外设来管理扩展的存储器， FSMC 是 Flexible Static Memory Controller的缩写，译为灵活的静态存储控制器。它可以用于驱动包括 SRAM、 NOR FLASH 以及 NAND FLSAH 类型的存储器，不能驱动如 SDRAM 这种动态的存储器而在 STM32F429 系列的控制器中，它具有 FMC 外设，支持控制 SDRAM 存储器。

由于 FSMC 外设可以用于控制扩展的外部存储器，而 MCU 对液晶屏的操作实际上就是把显示数据写入到显存中，与控制存储器非常类似，且 8080 接口的通讯时序完全可以使用 FSMC 外设产生，因而非常适合使用 FSMC 控制液晶屏。

![屏幕截图 2025-07-14 140846.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/14-14-09-02-屏幕截图%202025-07-14%20140846.png)

## 2. NOR FLASH时序结构体

```c
typedef struct
{
    uint32_t FSMC_AddressSetupTime; 
    uint32_t FSMC_AddressHoldTime;
    uint32_t FSMC_DataSetupTime; 
    uint32_t FSMC_BusTurnAroundDuration;
    uint32_t FSMC_CLKDivision;
    uint32_t FSMC_DataLatency; 
    uint32_t FSMC_AccessMode;
}FSMC_NORSRAMTimingInitTypeDef;
```

这个结构体与 SRAM 中的时序结构体完全一样，以下仅列出控制 NOR FLASH 时使用模式 B 用到的结构体成员说明：

(1) FSMC_AddressSetupTime

本成员设置地址建立时间，即 FSMC 读写时序图 FSMC 写 NOR_FLASH 的时序图 中的ADDSET 值，它可以被设置为 0-0xF 个 HCLK 周期数，按 STM32 标准库的默认配置， HCLK 的时钟频率为 72MHz，即一个 HCLK 周期为 1/72 微秒。

(2) FSMC_DataSetupTime

本成员设置数据建立时间，即 FSMC 读写时序图 FSMC 写 NOR_FLASH 的时序图 中的DATAST 值，它可以被设置为 0-0xF 个 HCLK 周期数。

(3) FSMC_DataSetupTime

本成员设置数据建立时间，即 FSMC 读写时序图 FSMC 写 NOR_FLASH 的时序图 中的DATAST 值，它可以被设置为 0-0xF 个 HCLK 周期数。

(4) FSMC_BusTurnAroundDuration

本成员设置总线转换周期，在 NOR FLASH 存储器中，地址线与数据线可以分时复用，总线转换周期就是指总线在这两种状态间切换需要的延时，防止冲突。控制其它存储器时这个参数无效，配置为 0 即可。

(5) FSMC_CLKDivision

本成员用于设置时钟分频，它以 HCLK 时钟作为输入，经过 FSMC_CLKDivision 分频后输出到 FSMC_CLK 引脚作为通讯使用的同步时钟。控制其它异步通讯的存储器时这个参数无效，配置为 0 即可。

(6) FSMC_DataLatency

本成员设置数据保持时间，它表示在读取第一个数据之前要等待的周期数，该周期指同步时钟的周期，本参数仅用于同步 NOR FLASH 类型的存储器，控制其它类型的存储器时，本参数无效。

(7) FSMC_AccessMode

本成员设置存储器访问模式，不同的模式下 FSMC 访问存储器地址时引脚输出的时序不一样，可选 FSMC_AccessMode_A/B/C/D 模式。控制异步 NOR FLASH 时使用 B模式。

## 3. FSMC初始化结构体

```c
typedef struct
{
    uint32_t FSMC_Bank; 
    uint32_t FSMC_DataAddressMux; 
    uint32_t FSMC_MemoryType; 
    uint32_t FSMC_MemoryDataWidth; 
    uint32_t FSMC_BurstAccessMode;
    uint32_t FSMC_AsynchronousWait;
    uint32_t FSMC_WaitSignalPolarity; 
    uint32_t FSMC_WrapMode;
    uint32_t FSMC_WaitSignalActive;
    uint32_t FSMC_WriteOperation;
    uint32_t FSMC_WaitSignal; 
    uint32_t FSMC_ExtendedMode; 
    uint32_t FSMC_WriteBurst;
    /* 当不使用扩展模式时，本参数用于配置读写时序，否则用于配置读时序 */
    FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct;
    /* 当使用扩展模式时，本参数用于配置写时序 */
    FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;
}FSMC_NORSRAMInitTypeDef;
```

(1) FSMC_Bank

本成员用于选择 FSMC 映射的存储区域，它的可选参数以及相应的内核地址映射范围见表

![屏幕截图 2025-07-14 142925.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/14-14-29-33-屏幕截图%202025-07-14%20142925.png)

(2) FSMC_DataAddressMux

本成员用于设置地址总线与数据总线是否复用 (FSMC_DataAddressMux_Enable /Disable)，在控制 NOR FLASH 时，可以地址总线与数据总线可以分时复用，以减少使用STM32 信号线的数量。

(3) FSMC_MemoryType

本成员用于设置要控制的存储器类型，它支持控制的存储器类型为 SRAM、 PSRAM以及 NOR FLASH(FSMC_MemoryType_SRAM/PSRAM/NOR)。

(4) FSMC_MemoryDataWidth

本成员用于设置要控制的存储器的数据宽度，可选择设置成 8 或 16 位(FSMC_MemoryDataWidth_8b /16b)。

(5) FSMC_BurstAccessMode

本成员用于设置是否使用突发访问模式 (FSMC_BurstAccessMode_Enable/Disable)，突发访问模式是指发送一个地址后连续访问多个数据，非突发模式下每访问一个数据都需要输入一个地址，仅在控制同步类型的存储器时才能使用突发模式。

(6) FSMC_AsynchronousWait

本成员用于设置是否使能在同步传输时使用的等待信号(FSMC_AsynchronousWait_Enable/Disable)，在控制同步类型的 NOR 或 PSRAM时，存储器可以使用 FSMC_NWAIT 引脚通知 STM32 需要等待。

(7) FSMC_WaitSignalPolarity

本成员用于设置等待信号的有效极性，即要求等待时，使用高电平还是低电平(FSMC_WaitSignalPolarity_High/Low)。

(8) FSMC_WrapMode

本成员用于设置是否支持把非对齐的 AHB 突发操作分割成 2 次线性操作(FSMC_WrapMode_Enable/Disable)，该配置仅在突发模式下有效。

(9) FSMC_WaitSignalActive

本成员用于配置在突发传输模式时，决定存储器是在等待状态之前的一个数据周期有效还是在等待状态期间有效(FSMC_WaitSignalActive_BeforeWaitState/DuringWaitState)。

(10) FSMC_WriteOperation

这个成员用于设置是否写使能 (FSMC_WriteOperation_ Enable /Disable)，禁止写使能的话 FSMC 只能从存储器中读取数据，不能写入。

(11) FSMC_WaitSignal

本成员用于设置当存储器处于突发传输模式时，是否允许通过 NWAIT 信号插入等待状态 (FSMC_WaitSignal_Enable/Disable)。

(12) FSMC_ExtendedMode

本成员用于设置是否使用扩展模式 (FSMC_ExtendedMode_Enable/Disable)，在非扩展模式下，对存储器读写的时序都只使用 FSMC_BCR 寄存器中的配置，即下面的FSMC_ReadWriteTimingStruct 结构体成员；在扩展模式下，对存储器的读写时序可以分开配置，读时序使用 FSMC_BCR 寄存器，写时序使用 FSMC_BWTR 寄存器的配置，即后面的 FSMC_WriteTimingStruct 结构体成员。

(13) FSMC_ReadWriteTimingStruct

本成员是一个指针，赋值时使用上一小节中讲解的时序结构体FSMC_NORSRAMInitTypeDef 设置，当不使用扩展模式时，读写时序都使用本成员的参数配置。

(14) FSMC_WriteTimingStruct

同样地，本成员也是一个时序结构体的指针，只有当使用扩展模式时，本配置才有效，它是写操作使用的时序。

对本结构体赋值完成后，调用 FSMC_NORSRAMInit 库函数即可把配置参数写入到 FSMC_BCR及 FSMC_BTR/BWTR 寄存器中。

## 4. FSMC-LCD显示使用示例

### 4.1 LCD.h

```c
#ifndef __LCD_H
#define    __LCD_H

#include "stm32f10x.h"
#include "fonts.h"

// 根据液晶扫描方向而变化的XY像素宽度
// 调用ILI9341_GramScan函数设置方向时会自动更改
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 
// 液晶屏扫描模式
// 参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

/*选择BANK1-BORSRAM1 连接 TFT，地址范围为0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X60000000
RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000*/

// FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define FSMC_Addr_ST7789V_CMD  ((uint32_t)0x60000000)
// FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define FSMC_Addr_ST7789V_DATA ((uint32_t)0x60020000)
// 由片选引脚决定的NOR/SRAM块
#define FSMC_Bank1_NORSRAMx FSMC_Bank1_NORSRAM1
/* 控制引脚 */
// 片选，选择NOR/SRAM块
#define ST7789V_CS_CLK  RCC_APB2Periph_GPIOD // CS->PD7
#define ST7789V_CS_PORT GPIOD
#define ST7789V_CS_PIN  GPIO_Pin_7
// DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
#define ST7789V_DC_CLK  RCC_APB2Periph_GPIOD // DC->PD11
#define ST7789V_DC_PORT GPIOD
#define ST7789V_DC_PIN  GPIO_Pin_11
// 写使能
#define ST7789V_WR_CLK  RCC_APB2Periph_GPIOD // WR->PD5  
#define ST7789V_WR_PORT GPIOD
#define ST7789V_WR_PIN  GPIO_Pin_5
// 读使能
#define ST7789V_RD_CLK  RCC_APB2Periph_GPIOD // RD->PD4     
#define ST7789V_RD_PORT GPIOD
#define ST7789V_RD_PIN  GPIO_Pin_4
// 复位引脚
#define ST7789V_RST_CLK  RCC_APB2Periph_GPIOE // RST->PE1  
#define ST7789V_RST_PORT GPIOE
#define ST7789V_RST_PIN  GPIO_Pin_1
// 背光引脚
#define ST7789V_BK_CLK  RCC_APB2Periph_GPIOD // BK->PD12   
#define ST7789V_BK_PORT GPIOD
#define ST7789V_BK_PIN  GPIO_Pin_12
/* 数据线 */
#define ST7789V_D0_CLK  RCC_APB2Periph_GPIOD // D0->PD14   
#define ST7789V_D0_PORT GPIOD
#define ST7789V_D0_PIN  GPIO_Pin_14
#define ST7789V_D1_CLK  RCC_APB2Periph_GPIOD // D1->PD15   
#define ST7789V_D1_PORT GPIOD
#define ST7789V_D1_PIN  GPIO_Pin_15
#define ST7789V_D2_CLK  RCC_APB2Periph_GPIOD // D2->PD0   
#define ST7789V_D2_PORT GPIOD
#define ST7789V_D2_PIN  GPIO_Pin_0
#define ST7789V_D3_CLK  RCC_APB2Periph_GPIOD // D3->PD1   
#define ST7789V_D3_PORT GPIOD
#define ST7789V_D3_PIN  GPIO_Pin_1
#define ST7789V_D4_CLK  RCC_APB2Periph_GPIOE // D4->PE7   
#define ST7789V_D4_PORT GPIOE
#define ST7789V_D4_PIN  GPIO_Pin_7
#define ST7789V_D5_CLK  RCC_APB2Periph_GPIOE // D5->PE8   
#define ST7789V_D5_PORT GPIOE
#define ST7789V_D5_PIN  GPIO_Pin_8
#define ST7789V_D6_CLK  RCC_APB2Periph_GPIOE // D6->PE9   
#define ST7789V_D6_PORT GPIOE
#define ST7789V_D6_PIN  GPIO_Pin_9
#define ST7789V_D7_CLK  RCC_APB2Periph_GPIOE // D7->PE10  
#define ST7789V_D7_PORT GPIOE
#define ST7789V_D7_PIN  GPIO_Pin_10
#define ST7789V_D8_CLK  RCC_APB2Periph_GPIOE // D8->PE11   
#define ST7789V_D8_PORT GPIOE
#define ST7789V_D8_PIN  GPIO_Pin_11
#define ST7789V_D9_CLK  RCC_APB2Periph_GPIOE // D9->PE12   
#define ST7789V_D9_PORT GPIOE
#define ST7789V_D9_PIN  GPIO_Pin_12
#define ST7789V_D10_CLK  RCC_APB2Periph_GPIOE // D10->PE13  
#define ST7789V_D10_PORT GPIOE
#define ST7789V_D10_PIN  GPIO_Pin_13
#define ST7789V_D11_CLK  RCC_APB2Periph_GPIOE // D11->PE14   
#define ST7789V_D11_PORT GPIOE
#define ST7789V_D11_PIN  GPIO_Pin_14
#define ST7789V_D12_CLK  RCC_APB2Periph_GPIOE // D12->PE15   
#define ST7789V_D12_PORT GPIOE
#define ST7789V_D12_PIN  GPIO_Pin_15
#define ST7789V_D13_CLK  RCC_APB2Periph_GPIOD // D13->PD8   
#define ST7789V_D13_PORT GPIOD
#define ST7789V_D13_PIN  GPIO_Pin_8
#define ST7789V_D14_CLK  RCC_APB2Periph_GPIOD // D14->D9   
#define ST7789V_D14_PORT GPIOD
#define ST7789V_D14_PIN  GPIO_Pin_9
#define ST7789V_D15_CLK  RCC_APB2Periph_GPIOD // D15->PD10   
#define ST7789V_D15_PORT GPIOD
#define ST7789V_D15_PIN  GPIO_Pin_10
/* LCD显示区域 */
#define ST7789V_X_Star   0 // 起始点的X坐标
#define ST7789V_Y_Star   0 // 起始点的Y坐标
#define ST7789V_LESS_PIXEL    240 // 屏较短方向的像素宽度
#define ST7789V_MORE_PIXEL    320 // 屏较长方向的像素宽度
/* 颜色定义 */
#define WHITE   0xFFFF       
#define BLACK   0x0000       
#define GREY    0xF7DE       
#define BLUE    0x001F       
#define BLUE2   0x051F        
#define RED     0xF800        
#define MAGENTA 0xF81F        
#define GREEN   0x07E0        
#define CYAN    0x7FFF       
#define YELLOW  0xFFE0        
#define BRED    0xF81F
#define GRED    0xFFE0
#define GBLUE   0x07FF
/* ST7789V命令 */
#define CMD_SetCoordinateX 0x2A    // 设置X坐标
#define CMD_SetCoordinateY 0x2B    // 设置Y坐标
#define CMD_SetPixel       0x2C    // 填充像素
/* 定义 LCD 驱动芯片 ID */
#define LCDID_ST7789V 0x9341

/* 函数声明 */
void     ST7789V_Init(void);
uint16_t ST7789V_ReadID(void);
void     ST7789V_Rst(void);
void     ST7789V_BackLed_Control(FunctionalState enumState);
void     ST7789V_GramScan(uint8_t ucOtion);
void     ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void     ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void     ST7789V_SetPointPixel(uint16_t usX, uint16_t usY);
uint16_t ST7789V_GetPointPixel(uint16_t usX , uint16_t usY);
void     ST7789V_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);
void     ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight,uint8_t ucFilled);
void     ST7789V_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled);
void     ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar);
void     ST7789V_DispStringLine_EN(uint16_t line, char* pStr);
void     ST7789V_DispString_EN(uint16_t usX, uint16_t usY, char* pStr);
void      ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY ,  char* pStr);
void      LCD_SetFont(sFONT *fonts);
sFONT   *LCD_GetFont(void);
void      LCD_ClearLine(uint16_t Line);
void      LCD_SetBackColor(uint16_t Color);
void     LCD_SetTextColor(uint16_t Color)    ;
void      LCD_SetColors(uint16_t TextColor, uint16_t BackColor);
void      LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor);

#endif /* __LCD_H */
```

### 4.2 LCD.c

```c
#include "lcd.h"
#include "fonts.h"    

// 根据液晶扫描方向而变化的XY像素宽度
uint16_t LCD_X_LENGTH = ST7789V_LESS_PIXEL; // 240
uint16_t LCD_Y_LENGTH = ST7789V_MORE_PIXEL; // 320
uint8_t LCD_SCAN_MODE = 6; // 液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
uint16_t lcdid = LCDID_ST7789V; // 保存液晶屏驱动ic的 ID
static sFONT *LCD_Currentfonts = &Font8x16; // 字体
static uint16_t CurrentTextColor = BLACK; // 字体颜色-黑
static uint16_t CurrentBackColor = WHITE; // 背景颜色-白
/* 函数声明 */
static void     ST7789V_Write_Cmd(uint16_t usCmd);
static void     ST7789V_Write_Data(uint16_t usData);
static uint16_t ST7789V_Read_Data(void);
static void     ST7789V_Delay( __IO uint32_t nCount);
static void     ST7789V_GPIO_Config(void);
static void     ST7789V_FSMC_Config(void);
static void     ST7789V_REG_Config(void);
static void ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
static uint16_t ST7789V_Read_PixelData(void);
// 向ST7789V写入命令
static void ST7789V_Write_Cmd(uint16_t usCmd)
{
    *(__IO uint16_t*)(FSMC_Addr_ST7789V_CMD) = usCmd;    
}
// 向ST7789V写入数据
static void ST7789V_Write_Data(uint16_t usData)
{
    *(__IO uint16_t*)(FSMC_Addr_ST7789V_DATA) = usData;
}
// 从ST7789V读取数据    
static uint16_t ST7789V_Read_Data(void)
{
    return(*( __IO uint16_t *)(FSMC_Addr_ST7789V_DATA));

}
// 简单延时函数    
static void ST7789V_Delay(__IO uint32_t nCount)
{
  for(;nCount != 0; nCount--);

}
// 初始化ST7789V的GPIO
static void ST7789V_GPIO_Config ( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( // 控制信号
    ST7789V_CS_CLK|ST7789V_DC_CLK|ST7789V_WR_CLK|ST7789V_RD_CLK    |ST7789V_BK_CLK|ST7789V_RST_CLK|
    // 数据信号
    ST7789V_D0_CLK|ST7789V_D1_CLK|ST7789V_D2_CLK|ST7789V_D3_CLK|ST7789V_D4_CLK|ST7789V_D5_CLK|
    ST7789V_D6_CLK|ST7789V_D7_CLK|ST7789V_D8_CLK|ST7789V_D9_CLK|ST7789V_D10_CLK|ST7789V_D11_CLK|
    ST7789V_D12_CLK|ST7789V_D13_CLK|ST7789V_D14_CLK|ST7789V_D15_CLK, ENABLE);
    // 配置FSMC相对应的数据线    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = ST7789V_D0_PIN;
    GPIO_Init(ST7789V_D0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D1_PIN;
    GPIO_Init(ST7789V_D1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D2_PIN;
    GPIO_Init(ST7789V_D2_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D3_PIN;
    GPIO_Init(ST7789V_D3_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D4_PIN;
    GPIO_Init(ST7789V_D4_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D5_PIN;
    GPIO_Init(ST7789V_D5_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D6_PIN;
    GPIO_Init(ST7789V_D6_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D7_PIN;
    GPIO_Init(ST7789V_D7_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D8_PIN;
    GPIO_Init(ST7789V_D8_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D9_PIN;
    GPIO_Init(ST7789V_D9_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D10_PIN;
    GPIO_Init(ST7789V_D10_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D11_PIN;
    GPIO_Init(ST7789V_D11_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D12_PIN;
    GPIO_Init(ST7789V_D12_PORT, &GPIO_InitStructure);    
    GPIO_InitStructure.GPIO_Pin = ST7789V_D13_PIN;
    GPIO_Init(ST7789V_D13_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D14_PIN;
    GPIO_Init(ST7789V_D14_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D15_PIN;
    GPIO_Init(ST7789V_D15_PORT, &GPIO_InitStructure);
    /* 配置FSMC相对应的控制线
    FSMC_NOE   :LCD-RD
    FSMC_NWE   :LCD-WR
    FSMC_NE1   :LCD-CS
    FSMC_A16   :LCD-DC */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = ST7789V_RD_PIN; 
    GPIO_Init(ST7789V_RD_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_WR_PIN; 
    GPIO_Init(ST7789V_WR_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_CS_PIN; 
    GPIO_Init(ST7789V_CS_PORT, &GPIO_InitStructure);  
    GPIO_InitStructure.GPIO_Pin = ST7789V_DC_PIN; 
    GPIO_Init(ST7789V_DC_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = ST7789V_RST_PIN; 
    GPIO_Init(ST7789V_RST_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Pin = ST7789V_BK_PIN; 
    GPIO_Init(ST7789V_BK_PORT, &GPIO_InitStructure);
}
// LCD FSMC 模式配置
static void ST7789V_FSMC_Config ( void )
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure = {0};
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    readWriteTiming.FSMC_AddressSetupTime = 0x01;// 地址建立时间
    readWriteTiming.FSMC_DataSetupTime = 0x04;     // 数据保持时间
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B; // 模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配    
    /* 以下配置与模式B无关 */
    readWriteTiming.FSMC_AddressHoldTime = 0x00; 
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;    
    /* FSMC配置 */
    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx; // 使用NOR FLASH的BANK1
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable; // 禁止地址复用
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR; // 使用NOR FLASH
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b; // 数据宽度为16位
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable; // 禁止突发访问模式
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low; // 时钟低电平有效
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable; // 禁止WRAP模式
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState; // 等待状态前有效
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable; // 使能写操作
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable; // 禁止等待信号
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable; // 禁止扩展模式
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; // 禁止写突发模式
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; // 设置时序参数
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming; // 设置时序参数
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  
}
// 初始化ST7789V的寄存器
static void ST7789V_REG_Config ( void )
{
    lcdid = ST7789V_ReadID(); // 读取LCD ID
    if(lcdid == LCDID_ST7789V)
    {
        ST7789V_Write_Cmd(0xCF);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x81);
        ST7789V_Write_Data(0x30);
        /* Power on sequence control (EDh) */
        ST7789V_Write_Cmd(0xED);
        ST7789V_Write_Data(0x64);
        ST7789V_Write_Data(0x03);
        ST7789V_Write_Data(0x12);
        ST7789V_Write_Data(0x81);
        /* Driver timing control A (E8h) */
        ST7789V_Write_Cmd(0xE8);
        ST7789V_Write_Data(0x85);
        ST7789V_Write_Data(0x10);
        ST7789V_Write_Data(0x78);
        /* Power control A (CBh) */
        ST7789V_Write_Cmd(0xCB);
        ST7789V_Write_Data(0x39);
        ST7789V_Write_Data(0x2C);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x34);
        ST7789V_Write_Data(0x06); // 改为0x06可防止液晶显示白屏时有条纹的情况
        /* Pump ratio control (F7h) */
        ST7789V_Write_Cmd(0xF7);
        ST7789V_Write_Data(0x20);
        /* Driver timing control B */
        ST7789V_Write_Cmd(0xEA);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
        ST7789V_Write_Cmd(0xB1);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x1B);
        /* Display Function Control (B6h) */
        ST7789V_Write_Cmd(0xB6);
        ST7789V_Write_Data(0x0A);
        ST7789V_Write_Data(0xA2);
        /* Power Control 1 (C0h) */
        ST7789V_Write_Cmd(0xC0);
        ST7789V_Write_Data(0x35);
        /* Power Control 2 (C1h) */
        ST7789V_Write_Cmd(0xC1);
        ST7789V_Write_Data(0x11);
        /* VCOM Control 1 (C5h) */
        ST7789V_Write_Cmd(0xC5);
        ST7789V_Write_Data(0x45);
        ST7789V_Write_Data(0x45);
        /* VCOM Control 2 (C7h) */
        ST7789V_Write_Cmd(0xC7);
        ST7789V_Write_Data(0xA2);
        /* Enable 3G (F2h) */
        ST7789V_Write_Cmd(0xF2);
        ST7789V_Write_Data(0x00);
        /* Gamma Set (26h) */
        ST7789V_Write_Cmd(0x26);
        ST7789V_Write_Data(0x01);
        /* Positive Gamma Correction */
        ST7789V_Write_Cmd(0xE0); // Set Gamma
        ST7789V_Write_Data(0x0F);
        ST7789V_Write_Data(0x26);
        ST7789V_Write_Data(0x24);
        ST7789V_Write_Data(0x0B);
        ST7789V_Write_Data(0x0E);
        ST7789V_Write_Data(0x09);
        ST7789V_Write_Data(0x54);
        ST7789V_Write_Data(0xA8);
        ST7789V_Write_Data(0x46);
        ST7789V_Write_Data(0x0C);
        ST7789V_Write_Data(0x17);
        ST7789V_Write_Data(0x09);
        ST7789V_Write_Data(0x0F);
        ST7789V_Write_Data(0x07);
        ST7789V_Write_Data(0x00);
        /* Negative Gamma Correction (E1h) */
        ST7789V_Write_Cmd(0xE1); // Set Gamma
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x19);
        ST7789V_Write_Data(0x1B);
        ST7789V_Write_Data(0x04);
        ST7789V_Write_Data(0x10);
        ST7789V_Write_Data(0x07);
        ST7789V_Write_Data(0x2A);
        ST7789V_Write_Data(0x47);
        ST7789V_Write_Data(0x39);
        ST7789V_Write_Data(0x03);
        ST7789V_Write_Data(0x06);
        ST7789V_Write_Data(0x06);
        ST7789V_Write_Data(0x30);
        ST7789V_Write_Data(0x38);
        ST7789V_Write_Data(0x0F);
        /* Memory access control set */
        ST7789V_Write_Cmd(0x36); 
        ST7789V_Write_Data(0xC8);    /* 竖屏，左上角到右下角扫描方式 */
        /* Column address control set */
        ST7789V_Write_Cmd(CMD_SetCoordinateX); 
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0xEF);
        /* Page address control set */
        ST7789V_Write_Cmd(CMD_SetCoordinateY); 
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x01);
        ST7789V_Write_Data(0x3F);
        /* Pixel Format Set (3Ah) */
        ST7789V_Write_Cmd(0x3a); 
        ST7789V_Write_Data(0x55);
        /* Sleep Out (11h) */
        ST7789V_Write_Cmd(0x11);
        ST7789V_Delay(0xAFFf<<2);
        /* Display ON (29h) */
        ST7789V_Write_Cmd(0x29);
    }
}
// ST7789V初始化函数
void ST7789V_Init ( void )
{
    ST7789V_GPIO_Config (); 
    ST7789V_FSMC_Config (); 
    ST7789V_BackLed_Control(ENABLE);
    ST7789V_Rst(); 
    ST7789V_REG_Config();
    // 设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向  
    ST7789V_GramScan(LCD_SCAN_MODE);
}
// ST7789V背光控制函数
void ST7789V_BackLed_Control(FunctionalState enumState )
{
    if(enumState) // 点亮背光灯
        GPIO_ResetBits(ST7789V_BK_PORT, ST7789V_BK_PIN); // 低电平点亮
    else
        GPIO_SetBits(ST7789V_BK_PORT, ST7789V_BK_PIN);
}
// 读取LCD驱动芯片ID函数
uint16_t ST7789V_ReadID(void)
{
    uint16_t id = 0;    
    ST7789V_Write_Cmd(0xD3); // 写入命令
    ST7789V_Read_Data();
    ST7789V_Read_Data();
    id = ST7789V_Read_Data();
    id <<= 8;
    id |= ST7789V_Read_Data();
    if(id == LCDID_ST7789V)
    {
        return id; // 成功读取到ST7789V ID
      }
    return 0; // 读取失败
}
// ST7789V软件复位
void ST7789V_Rst(void)
{
    GPIO_ResetBits(ST7789V_RST_PORT, ST7789V_RST_PIN); // 低电平复位
    ST7789V_Delay(0xAFF);                        
    GPIO_SetBits (ST7789V_RST_PORT, ST7789V_RST_PIN);              
    ST7789V_Delay(0xAFF); 
}
// 设置ST7789V的GRAM的扫描方向 
void ST7789V_GramScan(uint8_t ucOption)
{    
    // 参数检查，只可输入0-7
    if(ucOption > 7)
    {
        return;
    }
    LCD_SCAN_MODE = ucOption; // 根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
    if(ucOption%2 == 0)    
    {
        // 0 2 4 6模式下X方向像素宽度为240，Y方向为320
        LCD_X_LENGTH = ST7789V_LESS_PIXEL;
        LCD_Y_LENGTH = ST7789V_MORE_PIXEL;
    }
    else                
    {
        // 1 3 5 7模式下X方向像素宽度为320，Y方向为240
        LCD_X_LENGTH = ST7789V_MORE_PIXEL;
        LCD_Y_LENGTH = ST7789V_LESS_PIXEL; 
    }
    ST7789V_Write_Cmd(0x36);    // 0x36命令参数的高3位可用于设置GRAM扫描方向    
    if(lcdid == LCDID_ST7789V)
      {
        ST7789V_Write_Data(0x00 |(ucOption<<5)); // 根据ucOption的值设置LCD参数，共0-7种模式
      }
    ST7789V_Write_Cmd(CMD_SetCoordinateX); 
    ST7789V_Write_Data(0x00);        
    ST7789V_Write_Data(0x00);    
    ST7789V_Write_Data(((LCD_X_LENGTH-1)>>8)&0xFF); 
    ST7789V_Write_Data((LCD_X_LENGTH-1)&0xFF);                
    ST7789V_Write_Cmd(CMD_SetCoordinateY); 
    ST7789V_Write_Data(0x00);        
    ST7789V_Write_Data(0x00);        
    ST7789V_Write_Data(((LCD_Y_LENGTH-1)>>8)&0xFF); 
    ST7789V_Write_Data((LCD_Y_LENGTH-1)&0xFF);            
    ST7789V_Write_Cmd(CMD_SetPixel);    
}
// ST7789V显示器上开辟一个窗口
void ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{    
    ST7789V_Write_Cmd(CMD_SetCoordinateX);                 
    ST7789V_Write_Data(usX>>8);     
    ST7789V_Write_Data(usX&0xff);     
    ST7789V_Write_Data((usX+usWidth-1)>>8);
    ST7789V_Write_Data((usX+usWidth-1)&0xff);
    ST7789V_Write_Cmd(CMD_SetCoordinateY);               
    ST7789V_Write_Data(usY>>8);
    ST7789V_Write_Data(usY&0xff);
    ST7789V_Write_Data((usY+usHeight-1)>>8);
    ST7789V_Write_Data((usY+usHeight-1)&0xff);
}
// 设置ST7789V光标坐标
static void ST7789V_SetCursor(uint16_t usX, uint16_t usY)    
{
    ST7789V_OpenWindow(usX, usY, 1, 1 );
}
// 填充颜色
static void ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
    uint32_t i = 0;
    ST7789V_Write_Cmd(CMD_SetPixel);    
    for(i = 0; i < ulAmout_Point; i++)
    {
        ST7789V_Write_Data(usColor);
    }
}
// 清屏
void ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
    ST7789V_OpenWindow(usX, usY, usWidth, usHeight);
    ST7789V_FillColor(usWidth * usHeight, CurrentBackColor) ;        
}
// 对LCD的某一点以某种颜色进行填充
void ST7789V_SetPointPixel(uint16_t usX, uint16_t usY)    
{    
    if((usX < LCD_X_LENGTH)&&(usY < LCD_Y_LENGTH))
      {
        ST7789V_SetCursor(usX, usY);    
        ST7789V_FillColor(1, CurrentTextColor);
    }
}
// 读取 GRAM 的一个像素数据
static uint16_t ST7789V_Read_PixelData(void)    
{    
    uint16_t usRG=0, usB=0;
    ST7789V_Write_Cmd(0x2E);
    ST7789V_Read_Data (); // 去掉前一次读取结果
    // 获取红色通道与绿色通道的值
    usRG = ST7789V_Read_Data();  
    usB = ST7789V_Read_Data();      
      return((usRG&0xF800)|((usRG<<3)&0x7E0)|(usB>>11));
}
// 获取LCD的某一点的颜色值
uint16_t ST7789V_GetPointPixel(uint16_t usX, uint16_t usY)
{ 
    uint16_t usPixelData;
    ST7789V_SetCursor(usX, usY);
    usPixelData = ST7789V_Read_PixelData();
    return usPixelData;
}
// 在LCD使用 Bresenham 算法画线段 
void ST7789V_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
    uint16_t us; 
    uint16_t usX_Current, usY_Current;
    int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
    int32_t lIncrease_X, lIncrease_Y;     
    lDelta_X = usX2 - usX1; // 计算坐标增量 
    lDelta_Y = usY2 - usY1; 
    usX_Current = usX1; 
    usY_Current = usY1; 
    if ( lDelta_X > 0 ) 
    {
        lIncrease_X = 1; // 设置单步方向
    } 
    else if(lDelta_X == 0) 
    {
        lIncrease_X = 0; // 垂直线
    } 
    else 
      { 
        lIncrease_X = -1;
        lDelta_X = - lDelta_X;
      } 
    if(lDelta_Y > 0)
    {
        lIncrease_Y = 1; 
    }
    else if(lDelta_Y == 0)
    {
        lIncrease_Y = 0; // 水平线 
    }
    else 
      {
        lIncrease_Y = -1;
        lDelta_Y = - lDelta_Y;
      } 
    if(lDelta_X > lDelta_Y)
    {
        lDistance = lDelta_X; // 选取基本增量坐标轴 
    }
    else 
    {
        lDistance = lDelta_Y; 
    }
    for(us = 0; us <= lDistance + 1; us++ ) // 画线输出 
    {  
        ST7789V_SetPointPixel(usX_Current, usY_Current ); // 画点 
        lError_X += lDelta_X ; 
        lError_Y += lDelta_Y ;         
        if(lError_X > lDistance) 
        { 
            lError_X -= lDistance; 
            usX_Current += lIncrease_X; 
        }  
        if(lError_Y > lDistance) 
        { 
            lError_Y -= lDistance; 
            usY_Current += lIncrease_Y; 
        } 
    }  
}   
// 在LCD上画一个矩形
void ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
    if(ucFilled)
    {
        ST7789V_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
        ST7789V_FillColor(usWidth * usHeight ,CurrentTextColor);    
    }
    else
    {
        ST7789V_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
        ST7789V_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
        ST7789V_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
        ST7789V_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);        
    }
}
// 在LCD上使用 Bresenham 算法画圆
void ST7789V_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled)
{
    int16_t sCurrentX, sCurrentY;
    int16_t sError;
    sCurrentX = 0; sCurrentY = usRadius;      
    sError = 3 - ( usRadius << 1 ); // 判断下个点位置的标志
    while(sCurrentX <= sCurrentY)
    {
        int16_t sCountY;
        if(ucFilled)
        {            
            for(sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++) 
            {                      
                ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCountY);          
                ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCountY);                  
                ST7789V_SetPointPixel(usX_Center - sCountY,   usY_Center + sCurrentX);         
                ST7789V_SetPointPixel(usX_Center - sCountY,   usY_Center - sCurrentX);        
                ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCountY);               
                ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCountY);           
                ST7789V_SetPointPixel(usX_Center + sCountY,   usY_Center - sCurrentX);              
                ST7789V_SetPointPixel(usX_Center + sCountY,   usY_Center + sCurrentX);                        
            }
        }
        else
        {          
            ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCurrentY);            
            ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCurrentY);              
            ST7789V_SetPointPixel(usX_Center - sCurrentY, usY_Center + sCurrentX);            
            ST7789V_SetPointPixel(usX_Center - sCurrentY, usY_Center - sCurrentX);            
            ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCurrentY);             
            ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCurrentY);            
            ST7789V_SetPointPixel(usX_Center + sCurrentY, usY_Center - sCurrentX);             
            ST7789V_SetPointPixel(usX_Center + sCurrentY, usY_Center + sCurrentX);           
        }            
        sCurrentX ++;
        if(sError < 0 )
        { 
            sError += 4 * sCurrentX + 6;
        }      
        else
        {
            sError += 10 + 4 * (sCurrentX - sCurrentY);   
            sCurrentY--;
        }     
    }
}
// 在LCD上显示一个英文字符
void ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
    uint8_t  byteCount, bitCount,fontLength;    
    uint16_t ucRelativePositon;
    uint8_t *Pfont;
    // 对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
    ucRelativePositon = cChar - ' ';
    // 每个字模的字节数
    fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
    //字模首地址
    /*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
    Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
    // 设置显示窗口
    ST7789V_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);    
    ST7789V_Write_Cmd(CMD_SetPixel);            
    // 按字节读取字模数据
    for(byteCount = 0; byteCount < fontLength; byteCount++)
    {
        // 一位一位处理要显示的颜色
        for(bitCount = 0; bitCount < 8; bitCount++ )
        {
            if(Pfont[byteCount] & (0x80>>bitCount))
                ST7789V_Write_Data(CurrentTextColor);            
            else
                ST7789V_Write_Data(CurrentBackColor);
        }    
    }    
}
// 在LCD上显示英文字符串
void ST7789V_DispStringLine_EN(uint16_t line,  char* pStr)
{
    uint16_t usX = 0;
    while(*pStr != '\0')
    {
        if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            line += LCD_Currentfonts->Height;
        }
        if ( ( line - ST7789V_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
        {
            usX = ST7789V_X_Star;
            line = ST7789V_Y_Star;
        }
        ST7789V_DispChar_EN(usX, line, * pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;    
    }
}
// 在LCD上显示英文字符串
void ST7789V_DispString_EN(uint16_t usX ,uint16_t usY,  char* pStr)
{
    while(*pStr != '\0')
    {
        if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY += LCD_Currentfonts->Height;
        }        
        if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY = ST7789V_Y_Star;
        }    
        ST7789V_DispChar_EN(usX, usY, * pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;    
    }
}
// 在LCD上显示英文字符串(沿Y轴方向)
void ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY, char* pStr)
{    
    while(* pStr != '\0')
    {
        if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height)>LCD_Y_LENGTH)
        {
            usY = ST7789V_Y_Star;
            usX += LCD_Currentfonts->Width;
        }
        if((usX - ST7789V_X_Star + LCD_Currentfonts->Width)> LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY = ST7789V_Y_Star;
        }
        ST7789V_DispChar_EN(usX, usY, *pStr);
        pStr ++;
        usY += LCD_Currentfonts->Height;        
    }    
}
// 设置英文字体类型
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}
// 获取当前字体类型
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
// 设置LCD的前景(字体)及背景颜色,RGB565
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}
// 获取LCD的前景(字体)及背景颜色,RGB565
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}
// 设置LCD的前景(字体)颜色,RGB565
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}
// 设置LCD的背景颜色,RGB565
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}
// 清除某行文字
void LCD_ClearLine(uint16_t Line)
{
  ST7789V_Clear(0,Line,LCD_X_LENGTH,((sFONT *)LCD_GetFont())->Height);
}
```

### 4.3 main.c

```c
#include "stm32f10x.h"
#include "lcd.h"
#include <stdio.h>

static void LCD_Test(void);    
static void Delay (__IO uint32_t nCount);

int main(void)
{
    ST7789V_Init (); 
  ST7789V_GramScan(6); 
    while ( 1 )
    {
        LCD_Test(); // 测试显示函数
    }

}
extern uint16_t lcdid;
// 用于测试各种液晶的函数
void LCD_Test(void)
{
    static uint8_t testCNT = 0;    
    char dispBuff[100];
    testCNT++;
    LCD_SetFont(&Font8x16); 
    LCD_SetColors(RED, BLACK); 
  ST7789V_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
  /* 显示字符串测试 */
  if(lcdid == LCDID_ST7789V)
  {
    ST7789V_DispStringLine_EN(LINE(1),"ST7789V LCD driver");
  }
    /* 不同字体测试 */
    LCD_SetFont(&Font16x24);
    LCD_SetTextColor(GREEN);
    sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(2));    
    ST7789V_DispStringLine_EN(LINE(2),dispBuff);
    /* 显示图形测试 */
    LCD_SetFont(&Font24x32);
  /* 画直线 */
  LCD_ClearLine(LINE(3));
    LCD_SetTextColor(BLUE);
  ST7789V_DispStringLine_EN(LINE(3),"Draw line:");
    LCD_SetTextColor(RED);
  ST7789V_DrawLine(50,170,210,230);  
  ST7789V_DrawLine(50,200,210,240);
    LCD_SetTextColor(GREEN);
  ST7789V_DrawLine(100,170,200,230);  
  ST7789V_DrawLine(200,200,220,240);
    LCD_SetTextColor(BLUE);
  ST7789V_DrawLine(110,170,110,230);  
  ST7789V_DrawLine(130,200,220,240);
  Delay(0xFFFFFF); 
  ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
  /*画矩形*/
  LCD_ClearLine(LINE(3));    /* 清除单行文字 */
    LCD_SetTextColor(BLUE);
  ST7789V_DispStringLine_EN(LINE(3),"Draw Rect:");
    LCD_SetTextColor(RED);
  ST7789V_DrawRectangle(50,200,100,30,1);
    LCD_SetTextColor(GREEN);
  ST7789V_DrawRectangle(160,200,20,40,0);
    LCD_SetTextColor(BLUE);
  ST7789V_DrawRectangle(170,200,50,20,1);
  Delay(0xFFFFFF);
    ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);    
  /* 画圆 */
  LCD_ClearLine(LINE(3));    
    LCD_SetTextColor(BLUE);
  ST7789V_DispStringLine_EN(LINE(3),"Draw Cir:");
    LCD_SetTextColor(RED);
  ST7789V_DrawCircle(100,200,20,0);
    LCD_SetTextColor(GREEN);
  ST7789V_DrawCircle(100,200,10,1);
    LCD_SetTextColor(BLUE);
    ST7789V_DrawCircle(140,200,20,0);
  Delay(0xFFFFFF);  
  ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
}

static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );

}
```

## 5. FSMC常用函数（STD库）

### 5.1 SRAM/NOR Flash 配置步骤

#### 5.1.1 时钟与GPIO初始化

```c
// 使能FSMC时钟
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

// 配置地址/数据/控制线（以Bank1为例）
GPIO_InitTypeDef GPIO_InitStruct;
// 数据线D0-D15（PD14, PD15, PD0, PD1, PE7-PE15）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | ...; 
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOD, &GPIO_InitStruct);
GPIO_Init(GPIOE, &GPIO_InitStruct);

// 地址线A0-A18（PF0-PF5, PF12-PF15, PG0-PG5）
GPIO_InitStruct.GPIO_Pin = ...; 
GPIO_Init(GPIOF, &GPIO_InitStruct);
GPIO_Init(GPIOG, &GPIO_InitStruct);

// 控制线：NE1（片选）、NOE（读）、NWE（写）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;  // NE1@PD7
GPIO_Init(GPIOD, &GPIO_InitStruct);
```

#### 5.1.2 SRAM 时序配置

```c
FSMC_NORSRAMTimingInitTypeDef Timing;
Timing.FSMC_AddressSetupTime = 1;      // 地址建立时间（1个HCLK周期）
Timing.FSMC_AddressHoldTime = 0;       // 地址保持时间
Timing.FSMC_DataSetupTime = 2;         // 数据建立时间（2个HCLK）
Timing.FSMC_BusTurnAroundDuration = 0; // 总线转换时间
Timing.FSMC_CLKDivision = 0;           // 时钟分频（禁用）
Timing.FSMC_DataLatency = 0;           // 数据延迟
Timing.FSMC_AccessMode = FSMC_AccessMode_A; // 模式A

FSMC_NORSRAMInitTypeDef FSMC_InitStruct;
FSMC_InitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM1; // Bank1-SRAM1
FSMC_InitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
FSMC_InitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM; // SRAM类型
FSMC_InitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; // 16位总线
FSMC_InitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
FSMC_InitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
FSMC_InitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
FSMC_InitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
FSMC_InitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
FSMC_InitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable; // 使能写操作
FSMC_InitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
FSMC_InitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // 单一时序
FSMC_InitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
FSMC_InitStruct.FSMC_ReadWriteTimingStruct = &Timing; // 读写共用时序
FSMC_InitStruct.FSMC_WriteTimingStruct = &Timing;     // 写时序（扩展模式时分开）

FSMC_NORSRAMInit(&FSMC_InitStruct);  // 应用配置
FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); // 使能Bank
```

### 5.2 LCD接口配置（ILI9341为例）

#### 5.2.1 基本配置

```c
// 使用Bank1, NE4片选（PG12）
FSMC_NORSRAMInitTypeDef FSMC_InitStruct;
FSMC_InitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM4; 
FSMC_InitStruct.FSMC_MemoryType = FSMC_MemoryType_NOR; 
FSMC_InitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
FSMC_InitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;

// 时序配置（快速模式）
FSMC_NORSRAMTimingInitTypeDef Timing;
Timing.FSMC_AddressSetupTime = 1;  // 1个HCLK
Timing.FSMC_DataSetupTime = 2;     // 2个HCLK（72MHz下约27ns）
FSMC_InitStruct.FSMC_ReadWriteTimingStruct = &Timing;

FSMC_NORSRAMInit(&FSMC_InitStruct);
FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
```

#### 5.2.2 地址映射

```c
// 命令/数据地址定义
#define LCD_CMD_ADDR  (*(__IO uint16_t*)0x6C000000) // RS=0
#define LCD_DATA_ADDR (*(__IO uint16_t*)0x6C000800) // RS=1（A10=1）

// 发送命令
void LCD_WriteCmd(uint16_t cmd) {
    LCD_CMD_ADDR = cmd;
}

// 发送数据
void LCD_WriteData(uint16_t data) {
    LCD_DATA_ADDR = data;
}
```

### 5.3 核心操作函数

#### 5.3.1 存储器读写

```c
// 直接地址访问（SRAM示例）
#define SRAM_BASE_ADDR ((uint16_t*)0x60000000)

// 写入数据
void SRAM_Write(uint32_t addr, uint16_t data) {
    *(volatile uint16_t*)(SRAM_BASE_ADDR + addr) = data;
}

// 读取数据
uint16_t SRAM_Read(uint32_t addr) {
    return *(volatile uint16_t*)(SRAM_BASE_ADDR + addr);
}
```

#### 5.3.2 块数据传输

```c
// 使用DMA传输数据到FSMC
void FSMC_DMA_Write(uint32_t fsmc_addr, uint16_t* data, uint32_t size) {
    DMA_InitTypeDef DMA_InitStruct;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)fsmc_addr;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)data;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST; // 内存->外设
    DMA_InitStruct.DMA_BufferSize = size;
    // ...其他DMA参数
    DMA_Init(DMA1_Channel6, &DMA_InitStruct); // FSMC使用Channel6
    DMA_Cmd(DMA1_Channel6, ENABLE);
}
```

| **Bank**           | **片选**  | **地址范围**                  | **典型用途**   |
| ------------------ | ------- | ------------------------- | ---------- |
| Bank1-NORSRAM1     | NE1     | 0x6000 0000-63FF FFFF     | SRAM1      |
| Bank1-NORSRAM2     | NE2     | 0x6400 0000-67FF FFFF     | SRAM2      |
| Bank1-NORSRAM3     | NE3     | 0x6800 0000-6BFF FFFF     | NOR Flash  |
| **Bank1-NORSRAM4** | **NE4** | **0x6C00 0000-6FFF FFFF** | **LCD接口**  |
| Bank2-NAND         | NCE2    | 0x7000 0000-73FF FFFF     | NAND Flash |
| Bank3-NAND         | NCE3    | 0x8000 0000-83FF FFFF     | NAND Flash |
