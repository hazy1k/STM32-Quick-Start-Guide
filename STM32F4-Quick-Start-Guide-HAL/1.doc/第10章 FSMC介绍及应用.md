# 第十章 FSMC介绍及应用

## **1. FSMC (Flexible Static Memory Controller) 的作用**

FSMC，即**灵活静态存储控制器**，是 STM32 微控制器中一个非常强大的外设。它的主要作用是**提供与外部静态存储器（如 SRAM、NOR Flash、NAND Flash）和外部并行 LCD 显示器的接口**。

简单来说，FSMC 的作用就是：

- **将外部存储器或设备映射到 STM32 的内存地址空间。** 这样，CPU 就可以像访问内部 RAM 一样，直接通过内存地址读写外部设备，而无需通过复杂的 GPIO 控制和时序操作。这大大简化了驱动代码，提高了访问效率。
- **生成外部设备所需的各种控制信号。** 比如片选 (CS)、读使能 (OE)、写使能 (WE)、地址线 (A)、数据线 (D) 等。FSMC 可以配置这些信号的时序，以匹配不同外部设备的严格要求。

**核心优势：**

- **速度快：** 硬件实现时序控制，比软件模拟（GPIO 翻转）快得多，能达到更高的总线速度。
- **简化编程：** 应用程序只需读写内存地址，而无需关心复杂的时序细节。
- **资源节约：** 内部集成控制器，无需大量 CPU 周期用于位操作。

## **2. FSMC 如何模拟 8080 时序驱动 LCD 显示**

8080 时序（也称为 Intel 80 并行总线时序）是一种常见的并行 LCD 接口标准。它主要依赖以下信号：

- **数据线 (D[x:0])：** 用于传输数据（像素数据、指令参数等）。在你的项目中是 16 位数据线。
- **片选 (CS, Chip Select)：** 通常是低电平有效，用于选择 LCD 控制器。
- **读使能 (RD, Read Enable) / 输出使能 (OE, Output Enable)：** 通常是低电平有效，当要从 LCD 读取数据时有效。
- **写使能 (WR, Write Enable) / 写使能 (WE, Write Enable)：** 通常是低电平有效，当要向 LCD 写入数据时有效。
- **数据/命令选择 (D/C, Data/Command)：** 高电平表示传输数据，低电平表示传输命令。
- **复位 (RES, Reset)：** 用于复位 LCD 控制器。

**FSMC 模拟 8080 时序的原理：**

FSMC 没有一个专门的 "8080 模式"，但它通过其灵活的 NOR/SRAM 接口来**通用地模拟** 8080 时序。具体方法是将 8080 时序中的关键信号映射到 FSMC 的相应引脚和功能上。

在这个工程中，FSMC 模拟 8080 时序的关键映射和配置如下：

### 2.1 硬件连接与信号映射

1. **数据线 (D0-D15)：** LCD 的 16 位数据线直接连接到 STM32 FSMC 的数据线（`PD0, PD1, PE7-PE15, PD14, PD15`）。FSMC 被配置为 16 位数据宽度 (`FSMC_NORSRAM_MEM_BUS_WIDTH_16`)。
2. **片选 (CS)：** LCD 的片选线连接到 STM32 FSMC 的某个片选引脚，例如 `FSMC_NE4` (对应 `PG12`)。在 FSMC 配置中，`NSBank = FSMC_NORSRAM_BANK4` 就是选择这个片选线。
3. **读使能 (RD/OE)：** LCD 的读使能线连接到 STM32 FSMC 的读使能引脚 `FSMC_NOE` (对应 `PD4`)。
4. **写使能 (WR/WE)：** LCD 的写使能线连接到 STM32 FSMC 的写使能引脚 `FSMC_NWE` (对应 `PD5`)。
5. **数据/命令选择 (D/C)：** 这是最巧妙的部分。FSMC 没有一个专门的 "D/C" 引脚。为了模拟 D/C，我们利用 FSMC 的**地址线**。
   - LCD 的 D/C 线连接到 STM32 FSMC 的某根地址线，例如 `FSMC_A16` (对应 `PF12`)。
   - 我们将 LCD 控制器映射到 STM32 的内存地址空间：
     - 当 CPU 访问映射区域的基地址时 (例如 `0x6C000000`)，`FSMC_A16` 是低电平 (0)。此时，写入 LCD 的是**命令**。
     - 当 CPU 访问基地址加上 `(1 << 16)` 的偏移地址时 (例如 `0x6C000000 + 0x00010000`)，`FSMC_A16` 是高电平 (1)。此时，写入 LCD 的是**数据**。
     - 你的代码中 `#define LCD_BASE ((u32)(0x6C000000 | 0x0000007E))` 实际上是利用了这种地址位模拟 D/C 的技巧。`0x0000007E` 的实际意义是 `0x00000040 * 2 - 2`，对应到 FSMC_A6 位 (因为地址在 STM32 内部会右移一位，所以 FSMC_A6 对应的是地址的第 6 位)。因此，`0x6C000000` 访问寄存器，`0x6C000000 + (1 << 7)` 访问数据。
     - `LCD_REG` 对应 `0x6C000000` (D/C 为低，写入命令)
     - `LCD_RAM` 对应 `0x6C000000 | (1 << A_D_OFFSET)` (D/C 为高，写入数据)
     - **在你提供的代码中，D/C 线实际上是连接到 FSMC_A6 (PF12)。** 那么地址的第 6 位（A6）就作为 D/C 控制线。当 A6 为 0 时，访问命令寄存器；当 A6 为 1 时，访问数据寄存器。这就是 `#define LCD_BASE ((u32)(0x6C000000 | 0x0000007E))` 的奥秘。`0x0000007E` 这个偏移量确保了 `LCD->LCD_REG` 和 `LCD->LCD_RAM` 在访问时，FSMC_A6 会有不同的电平。
       - `LCD->LCD_REG` 访问 `0x6C000000` 区域，此时 `FSMC_A6` 为 0 (D/C 低，命令模式)。
       - `LCD->LCD_RAM` 访问 `0x6C000000 + (1 << (A_D_OFFSET))` 区域，此时 `FSMC_A6` 为 1 (D/C 高，数据模式)。
       - **需要注意：** 你的 `#define LCD_BASE ((u32)(0x6C000000 | 0x0000007E))` 似乎有些误导性。更典型的用法是：  
         `#define LCD_CMD_ADDR (0x6C000000)`  
         `#define LCD_DATA_ADDR (0x6C000000 | (1 << 16))` // 如果D/C接FSMC_A16  
         然后 `LCD->LCD_REG` 就是 `*(volatile u16*)LCD_CMD_ADDR`，`LCD->LCD_RAM` 就是 `*(volatile u16*)LCD_DATA_ADDR`。  
         **在你给出的实际代码中，LCD_BASE的定义以及`LCD->LCD_REG`和`LCD->LCD_RAM`的访问方式，暗示D/C线接到了FSMC_A6，并且在`FSMC_NORSRAM_BANK4`的配置下，`0x6C000000`映射到了Bank4，然后通过地址偏移来控制A6。** 这是一个非常常见的ALIENTEK等开发板的接线方式。

### 2.2 时序配置

8080 时序的关键是读写周期中的地址建立时间、数据建立时间、数据保持时间等。FSMC 的时序参数（`AddressSetupTime`, `DataSetupTime`, `AddressHoldTime`, `BusTurnAroundDuration`）就是用来配置这些的：

- **`FSMC_ReadWriteTim.AddressSetupTime = 0x0F;` (16 HCLKs)**：地址建立时间。当 FSMC_NE (CS) 信号变为有效时，地址线 (包括 D/C) 需要保持稳定一段时间后，数据线才能开始传输。
- **`FSMC_ReadWriteTim.DataSetupTime = 60;` (61 HCLKs)**：数据建立时间。在 `FSMC_NWE` 或 `FSMC_NOE` 有效期间，数据需要保持稳定的时间。这个参数非常关键，它决定了数据在读写信号有效期间的稳定时长，必须满足 LCD 芯片对数据保持时间的要求。
- **`FSMC_WriteTim.AddressSetupTime = 9;` (10 HCLKs)**：写地址建立时间。
- **`FSMC_WriteTim.DataSetupTime = 17;` (18 HCLKs)**：写数据建立时间。
- **`FSMC_ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;`**：使能扩展模式，允许读写使用不同的时序配置，这在 LCD 驱动中很常见，因为读写时序要求可能不同。
- **`FSMC_AccessMode = FSMC_ACCESS_MODE_A;`**：FSMC 访问模式。模式 A 通常是异步模式，适用于大部分 LCD。

通过精确配置这些时间参数，FSMC 能够生成符合 ILI9486 等 LCD 控制器 8080 并行总线时序要求的波形。

### 2.3 软件层面的接口

一旦 FSMC 配置完成，CPU 就可以通过直接访问内存地址来操作 LCD：

**写入命令：**

```c
LCD->LCD_REG = command_value; // 访问映射到命令地址的寄存器，FSMC_A6 (D/C) 自动拉低
```

当 CPU 写入 `LCD->LCD_REG` 时，FSMC 会：

1. 拉低 `FSMC_NE4` (CS)。
2. 根据基地址，拉低 `FSMC_A6` (模拟 D/C)。
3. 将 `command_value` 放到 FSMC 数据线上。
4. 拉低 `FSMC_NWE` (WR) 一段时间（由 `FSMC_WriteTim` 控制）。
5. 拉高 `FSMC_NWE`，拉高 `FSMC_NE4`。

**写入数据：**

```c
LCD->LCD_RAM = data_value; // 访问映射到数据地址的寄存器，FSMC_A6 (D/C) 自动拉高
```

- 当 CPU 写入 `LCD->LCD_RAM` 时，FSMC 会：
  1. 拉低 `FSMC_NE4` (CS)。
  2. 根据偏移地址，拉高 `FSMC_A6` (模拟 D/C)。
  3. 将 `data_value` 放到 FSMC 数据线上。
  4. 拉低 `FSMC_NWE` (WR) 一段时间。
  5. 拉高 `FSMC_NWE`，拉高 `FSMC_NE4`。
- **读取数据：** 类似写入，FSMC 会拉低 `FSMC_NOE` (RD) 来读取数据。

## 3. 软件设计

### 3.1 lcd.h

```c
#ifndef __LCD_H
#define __LCD_H

#include "sys.h"
#include "stdlib.h"

// SRAM句柄，用于控制LCD
extern SRAM_HandleTypeDef TFTSRAM_Handler;

// LCD重要参数结构体
typedef struct  
{
    u16 width;     // LCD 宽度
    u16 height;    // LCD 高度
    u16 id;        // LCD ID (固定为9486)
    u8  dir;       // 横屏还是竖屏控制：0，竖屏；1，横屏。
    u16 wramcmd;   // 开始写GRAM指令
    u16 setxcmd;   // 设置x坐标指令
    u16 setycmd;   // 设置y坐标指令
}_lcd_dev;   

// LCD参数
extern _lcd_dev lcddev; 

// LCD的画笔颜色和背景色    
extern u32  POINT_COLOR; // 画笔颜色，默认红色    
extern u32  BACK_COLOR;  // 背景颜色，默认为白色

// LCD背光引脚定义
#define LCD_LED PBout(15) // LCD背光 PB15     

// LCD地址结构体
typedef struct
{
    vu16 LCD_REG;
    vu16 LCD_RAM;
} LCD_TypeDef;

// 使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A6作为数据命令区分线
// 注意设置时STM32内部会右移一位对其! 111 1110=0X7E     
#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

// 扫描方向定义
#define L2R_U2D  0 // 从左到右,从上到下
#define L2R_D2U  1 // 从左到右,从下到上
#define R2L_U2D  2 // 从右到左,从上到下
#define R2L_D2U  3 // 从右到左,从下到上
#define U2D_L2R  4 // 从上到下,从左到右
#define U2D_R2L  5 // 从上到下,从右到左
#define D2U_L2R  6 // 从下到上,从左到右
#define D2U_R2L  7 // 从下到上,从右到左
#define DFT_SCAN_DIR  L2R_U2D  // 默认的扫描方向

// 画笔颜色
#define WHITE          0xFFFF
#define BLACK          0x0000   
#define BLUE           0x001F  
#define BRED           0XF81F
#define GRED           0XFFE0
#define GBLUE          0X07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define BROWN          0XBC40 // 棕色
#define BRRED          0XFC07 // 棕红色
#define GRAY           0X8430 // 灰色

// GUI颜色
#define DARKBLUE       0X01CF // 深蓝色
#define LIGHTBLUE      0X7D7C // 浅蓝色  
#define GRAYBLUE       0X5458 // 灰蓝色
#define LIGHTGREEN     0X841F // 浅绿色
#define LGRAY          0XC618 // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE      0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE         0X2B12 // 浅棕蓝色(选择条目的反色)

// LCD操作函数声明
void LCD_Init(void);                                  // 初始化LCD
void LCD_DisplayOn(void);                             // 开启显示
void LCD_DisplayOff(void);                            // 关闭显示
void LCD_Clear(u32 Color);                            // 清屏
void LCD_SetCursor(u16 Xpos, u16 Ypos);               // 设置光标
void LCD_DrawPoint(u16 x,u16 y);                      // 画点
void LCD_Fast_DrawPoint(u16 x,u16 y,u32 color);       // 快速画点
u32  LCD_ReadPoint(u16 x,u16 y);                      // 读点
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);             // 画圆
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);    // 画线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2); // 画矩形
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color); // 填充单色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color); // 填充指定颜色
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode); // 显示一个字符
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);   // 显示一个数字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode); // 显示数字
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p); // 显示一个字符串
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);     // 写寄存器
u16 LCD_ReadReg(u16 LCD_Reg);                         // 读寄存器
void LCD_WriteRAM_Prepare(void);                      // 准备写入GRAM
void LCD_WriteRAM(u16 RGB_Code);                      // 写入GRAM
void LCD_Scan_Dir(u8 dir);                            // 设置屏扫描方向
void LCD_Display_Dir(u8 dir);                         // 设置屏幕显示方向
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height); // 设置窗口        

// LCD分辨率设置 (适用于9486常见分辨率，根据实际屏幕调整)
#define LCD_WIDTH_PX  320 // LCD默认竖屏宽度 (例如 320x480)
#define LCD_HEIGHT_PX 480 // LCD默认竖屏高度

#endif 

```

### 3.2 lcd.c

```c
#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h" 
#include "delay.h" 

// SRAM句柄，用于控制LCD
SRAM_HandleTypeDef TFTSRAM_Handler;    

// LCD的画笔颜色和背景色    
u32 POINT_COLOR = 0xFF000000; // 画笔颜色，默认黑色
u32 BACK_COLOR  = 0xFFFFFFFF; // 背景色，默认为白色

// 管理LCD重要参数，默认为竖屏
_lcd_dev lcddev;

// 写LCD寄存器指令
// regval: 寄存器地址
void LCD_WR_REG(vu16 regval)
{   
    regval = regval; // 防止编译器优化
    LCD->LCD_REG = regval; // 写入要写的寄存器序号
}

// 写LCD数据
// data: 要写入的值
void LCD_WR_DATA(vu16 data)
{   
    data = data; // 防止编译器优化
    LCD->LCD_RAM = data;
}

// 读LCD数据
// 返回值: 读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram; // 防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}    

// 写寄存器
// LCD_Reg: 寄存器地址
// LCD_RegValue: 要写入的数据
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;     // 写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue; // 写入数据     
}    

// 读寄存器
// LCD_Reg: 寄存器地址
// 返回值: 读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{    
    LCD_WR_REG(LCD_Reg); // 写入要读的寄存器序号
    delay_us(5);         // 延时等待数据稳定
    return LCD_RD_DATA(); // 返回读到的值
}   

// 准备写入GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;   
}

// LCD写入GRAM
// RGB_Code: 颜色值
void LCD_WriteRAM(u16 RGB_Code)
{     
    LCD->LCD_RAM = RGB_Code; // 写16位GRAM数据
}

// BGR格式颜色转换为RGB格式
// c: BGR格式的颜色值
// 返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
    u16  r,g,b,rgb;   
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return(rgb);
}

// 编译器优化时的延时函数
void opt_delay(u8 i)
{
    while(i--);
}

// 读取指定点的颜色值
// x,y: 坐标
// 返回值: 此点的颜色
u32 LCD_ReadPoint(u16 x,u16 y)
{
    u16 r = 0,g = 0,b = 0;
    if(x >= lcddev.width || y >= lcddev.height) return 0; // 超过范围，直接返回    
    
    LCD_SetCursor(x,y);     
    LCD_WR_REG(0X2E); // 9486发送读GRAM指令
    
    r = LCD_RD_DATA(); // dummy Read    
    opt_delay(2);   
    r = LCD_RD_DATA(); // 实际坐标颜色
    opt_delay(2);   
    b = LCD_RD_DATA();
    g = r & 0XFF; 
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); 
}

// 开启LCD显示
void LCD_DisplayOn(void)
{    
    LCD_WR_REG(0X29); // 开启显示指令
    printf("LCD Display ON (0x29).\r\n"); 
}

// 关闭LCD显示
void LCD_DisplayOff(void)
{    
    LCD_WR_REG(0X28); // 关闭显示指令
    printf("LCD Display OFF (0x28).\r\n"); 
}   

// 设置光标位置 (对于GRAM写入操作的起始点)
// Xpos: 横坐标
// Ypos: 纵坐标
// 注意：这个函数设置的是从 (Xpos, Ypos) 到 (lcddev.width-1, lcddev.height-1) 的写入窗口
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    // 参数校验：确保设置的坐标在当前显示尺寸内
    if(Xpos >= lcddev.width || Ypos >= lcddev.height) return;

    // SetColumnAddress (0x2A) 设置X起始和结束地址
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8); 
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_DATA((lcddev.width - 1) >> 8); // 结束X坐标为当前逻辑宽度最大值
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    
    // SetPageAddress (0x2B) 设置Y起始和结束地址
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8); 
    LCD_WR_DATA(Ypos & 0XFF);
    LCD_WR_DATA((lcddev.height - 1) >> 8); // 结束Y坐标为当前逻辑高度最大值
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

// 设置LCD的自动扫描方向
// dir: 0~7, 代表8个方向 (具体定义见lcd.h)
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;  

    switch(dir)
    {
        case L2R_U2D: // 从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U: // 从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D: // 从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U: // 从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R: // 从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L: // 从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R: // 从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L: // 从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }
    dirreg = 0X36; 
    regval |= 0X08; // 9486通常需要BGR设置 (这是你原始代码的设置，保留)
    LCD_WriteReg(dirreg,regval);
    
    if(regval & 0X20) // 如果设置了行列交换 (MV位为1)
    {
        if(lcddev.width < lcddev.height) 
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else // 未设置行列交换 (MV位为0)
    {
        if(lcddev.width > lcddev.height) 
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
      
    // 重新设置显示区域的X和Y范围 (很重要，这里使用更新后的 lcddev.width/height)
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);  

    printf("Scan_Dir: 0x36 set to 0x%X (dir=%d), Current W:H = %d:%d.\r\n", regval, dir, lcddev.width, lcddev.height); 
}     

// 画点
// x,y: 坐标
// POINT_COLOR: 此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
    // 参数校验
    if(x >= lcddev.width || y >= lcddev.height) return;

    LCD_SetCursor(x,y);     // 设置光标位置
    LCD_WriteRAM_Prepare(); // 准备写入GRAM
    LCD->LCD_RAM = POINT_COLOR;  
}

// 快速画点 (关键修改在这里！)
// x,y: 坐标
// color: 颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u32 color)
{    
    // 参数校验
    if(x >= lcddev.width || y >= lcddev.height) return;

    // 针对单个像素点写入，设置精确的1x1像素窗口，而不是从当前点到右下角
    LCD_WR_REG(lcddev.setxcmd); // Column Address Set (0x2A)
    LCD_WR_DATA(x >> 8); 
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_DATA(x >> 8); // 结束X坐标与起始X相同，形成1像素宽的窗口
    LCD_WR_DATA(x & 0XFF);
    
    LCD_WR_REG(lcddev.setycmd); // Page Address Set (0x2B)
    LCD_WR_DATA(y >> 8); 
    LCD_WR_DATA(y & 0XFF);
    LCD_WR_DATA(y >> 8); // 结束Y坐标与起始Y相同，形成1像素高的窗口
    LCD_WR_DATA(y & 0XFF);

    LCD->LCD_REG = lcddev.wramcmd; // 准备写入GRAM (Write Memory Start)
    LCD->LCD_RAM = color; // 写入颜色

    // !!! 每次单点写入后，立即恢复全屏窗口 !!!
    // 这是为了确保后续的字符绘制（它们会调用此函数）不会因为之前设置的1x1窗口而受限。
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); 
}

// 设置LCD显示方向
// dir: 0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    printf("LCD_Display_Dir: Changing to direction %d.\r\n", dir); 

    if(dir == 0) // 竖屏
    {
        lcddev.dir = 0; // 竖屏模式
        lcddev.width = LCD_WIDTH_PX; 
        lcddev.height = LCD_HEIGHT_PX;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;  
        LCD_Scan_Dir(DFT_SCAN_DIR); // 应用默认扫描方向 (L2R_U2D)
    }
    else // 横屏
    {   
        lcddev.dir = 1; // 横屏模式
        lcddev.width = LCD_HEIGHT_PX; // 宽度变为竖屏时的高度
        lcddev.height = LCD_WIDTH_PX; // 高度变为竖屏时的宽度
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;  
        LCD_Scan_Dir(U2D_L2R); // 横屏通常需要行列交换
    }
    // 每次切换方向后，都确保窗口被重置为全屏
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
    printf("LCD_Display_Dir completed, current W:H = %d:%d.\r\n", lcddev.width, lcddev.height); 
}
// 设置窗口
// sx,sy: 窗口起始坐标(左上角)
// width,height: 窗口宽度和高度, 必须大于0!!
// 窗体大小: width * height
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{     
    u16 twidth = sx + width - 1;
    u16 theight = sy + height - 1;

    // 参数校验，确保窗口范围不超出屏幕逻辑尺寸
    if(sx >= lcddev.width || sy >= lcddev.height) return;
    if(twidth >= lcddev.width) twidth = lcddev.width - 1;
    if(theight >= lcddev.height) theight = lcddev.height - 1;
    if(width == 0 || height == 0 || twidth < sx || theight < sy) return; // 避免无效窗口
    
    // 9486窗口设置 (Column Address Set)
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);  
    
    // 9486窗口设置 (Page Address Set)
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);

    // printf("Set_Window: (%d,%d) to (%d,%d) for W:%d H:%d.\r\n", sx, sy, twidth, theight, lcddev.width, lcddev.height); 
}

// SRAM底层驱动，时钟使能，引脚分配
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_FSMC_CLK_ENABLE(); 
    __HAL_RCC_GPIOD_CLK_ENABLE(); 
    __HAL_RCC_GPIOE_CLK_ENABLE(); 
    __HAL_RCC_GPIOF_CLK_ENABLE(); 
    __HAL_RCC_GPIOG_CLK_ENABLE(); 
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | \
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;   
    GPIO_Initure.Pull = GPIO_PULLUP;       
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;  
    GPIO_Initure.Alternate = GPIO_AF12_FSMC; 
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                       GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    GPIO_Initure.Pin = GPIO_PIN_12; 
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    GPIO_Initure.Pin = GPIO_PIN_12; 
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    printf("HAL_SRAM_MspInit: FSMC GPIOs Configured.\r\n"); 
}
// 初始化LCD 
void LCD_Init(void)
{   
    GPIO_InitTypeDef GPIO_Initure;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
    FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
    
    __HAL_RCC_GPIOB_CLK_ENABLE(); 
    GPIO_Initure.Pin = GPIO_PIN_15;           
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    LCD_LED = 0; 
    printf("LCD_Init: Starting LCD Initialization...\r\n"); 

    TFTSRAM_Handler.Instance = FSMC_NORSRAM_DEVICE;                
    TFTSRAM_Handler.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;    
    TFTSRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK4;                  
    TFTSRAM_Handler.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; 
    TFTSRAM_Handler.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;          
    TFTSRAM_Handler.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; 
    TFTSRAM_Handler.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; 
    TFTSRAM_Handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; 
    TFTSRAM_Handler.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;   
    TFTSRAM_Handler.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;    
    TFTSRAM_Handler.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;           
    TFTSRAM_Handler.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;        
    TFTSRAM_Handler.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; 
    TFTSRAM_Handler.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;           
    TFTSRAM_Handler.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC; 
    
    FSMC_ReadWriteTim.AddressSetupTime = 0x0F;        
    FSMC_ReadWriteTim.AddressHoldTime = 0;
    FSMC_ReadWriteTim.DataSetupTime = 60;             
    FSMC_ReadWriteTim.AccessMode = FSMC_ACCESS_MODE_A; 
    
    FSMC_WriteTim.BusTurnAroundDuration = 0;          
    FSMC_WriteTim.AddressSetupTime = 9;               
    FSMC_WriteTim.AddressHoldTime = 0;
    FSMC_WriteTim.DataSetupTime = 17;                 
    FSMC_WriteTim.AccessMode = FSMC_ACCESS_MODE_A;    
    
    if (HAL_SRAM_Init(&TFTSRAM_Handler,&FSMC_ReadWriteTim,&FSMC_WriteTim) != HAL_OK) 
    {
        printf("HAL_SRAM_Init FAILED! Program halted.\r\n");
        while(1); 
    }
    delay_ms(50); 
    
    LCD_WR_REG(0XD3);    
    LCD_RD_DATA(); 
    LCD_RD_DATA(); 
    u16 temp_id_high = LCD_RD_DATA();    
    u16 temp_id_low = LCD_RD_DATA();    
    lcddev.id = (temp_id_high << 8) | temp_id_low; 
    lcddev.id = 0x9486; 
    
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); 
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); 
    FSMC_Bank1E->BWTR[6] |= 3 << 0;      
    FSMC_Bank1E->BWTR[6] |= 2 << 8;      
    printf("LCD ID:%x\r\n",lcddev.id); 
    
    printf("Starting 9486 Initialization Sequence...\r\n"); 
    LCD_WR_REG(0XF2);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0xA3);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0XB2);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0x00);
    
    LCD_WR_REG(0XF8); 
    LCD_WR_DATA(0x21); 
    LCD_WR_DATA(0x04);
    
    LCD_WR_REG(0XF9); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x08);
    
    LCD_WR_REG(0X36); 
    LCD_WR_DATA(0X08); 
    printf("Sent Memory Access Control (0x36) to 0x08.\r\n"); 
    
    LCD_WR_REG(0X3A); 
    LCD_WR_DATA(0x05); 
    printf("Sent Pixel Format Set (0x3A) to 0x05 (RGB565).\r\n"); 
    
    LCD_WR_REG(0XB4); 
    LCD_WR_DATA(0x01); 
    
    LCD_WR_REG(0XB6); 
    LCD_WR_DATA(0X02); 
    LCD_WR_DATA(0x22); 
    
    LCD_WR_REG(0XC1); 
    LCD_WR_DATA(0X41); 
    
    LCD_WR_REG(0XC5); 
    LCD_WR_DATA(0X00); 
    LCD_WR_DATA(0x07); 
    
    LCD_WR_REG(0XE0); 
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x1F);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0X0F);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0X98);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0X0D);
    LCD_WR_DATA(0x00);
    
    LCD_WR_REG(0XE1); 
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x2E);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0X0D);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x47);
    LCD_WR_DATA(0X75);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0X20);
    LCD_WR_DATA(0x00);
    printf("Sent Gamma Correction commands.\r\n"); 
    
    LCD_WR_REG(0x11); // Exit Sleep mode
    delay_ms(120);    
    printf("Sent Sleep Out (0x11).\r\n"); 
    
    LCD_WR_REG(0x29); // Turn on Display
    printf("Sent Display On (0x29).\r\n"); 
    
    LCD_Display_Dir(0); 
    printf("LCD_Display_Dir(0) called for initial setup.\r\n"); 

    LCD_LED = 1;        
    printf("LCD_LED set to HIGH (Backlight ON).\r\n"); 

    LCD_Clear(WHITE);   
    printf("LCD Clear to WHITE after init.\r\n"); 
    printf("LCD_Init: Initialization sequence completed.\r\n"); 
}  

// 清屏函数
void LCD_Clear(u32 color)
{
    u32 totalpoint = (u32)lcddev.width * lcddev.height; 
    // 在清屏前设置全屏窗口，确保覆盖整个屏幕
    LCD_Set_Window(0,0,lcddev.width,lcddev.height); // <-- 确保这里设置了全屏窗口
    LCD_WriteRAM_Prepare();     
    for(u32 index = 0; index < totalpoint; index++) 
    {
        LCD->LCD_RAM = color; 
    }
    // 清屏后，窗口仍然保持全屏状态，无需额外恢复
    printf("Screen cleared to color 0x%X (W:%d, H:%d).\r\n", color, lcddev.width, lcddev.height); 
}  

// 在指定区域内填充单个颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{          
    if(sx >= lcddev.width || sy >= lcddev.height || sx > ex || sy > ey) return;
    if(ex >= lcddev.width) ex = lcddev.width - 1;
    if(ey >= lcddev.height) ey = lcddev.height - 1;

    u32 total_pixels = (u32)(ex - sx + 1) * (ey - sy + 1);
    if(total_pixels == 0) return;

    LCD_Set_Window(sx,sy,(ex-sx+1),(ey-sy+1)); // 设置填充区域的窗口
    LCD_WriteRAM_Prepare();    
    for(u32 i = 0; i < total_pixels; i++) 
    {
        LCD->LCD_RAM = color;     
    }
    // 填充后恢复全屏窗口
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // <-- 关键修复：填充后恢复全屏窗口
    printf("Filled rectangle (%d,%d) to (%d,%d) with color 0x%X.\r\n", sx, sy, ex, ey, color); 
}  

// 在指定区域内填充指定颜色块
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
    if(sx >= lcddev.width || sy >= lcddev.height || sx > ex || sy > ey) return;
    if(ex >= lcddev.width) ex = lcddev.width - 1;
    if(ey >= lcddev.height) ey = lcddev.height - 1;

    u32 total_pixels = (u32)(ex - sx + 1) * (ey - sy + 1);
    if(total_pixels == 0) return;

    LCD_Set_Window(sx,sy,(ex-sx+1),(ey-sy+1));    
    LCD_WriteRAM_Prepare();      
    for(u32 k = 0; k < total_pixels; k++) 
    {
        LCD->LCD_RAM = color[k]; 
    }
    // 填充后恢复全屏窗口
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // <-- 关键修复：填充后恢复全屏窗口
    printf("Filled color block (%d,%d) to (%d,%d).\r\n", sx, sy, ex, ey); 
}  

// 画线
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; 
    delta_y = y2 - y1; 
    uRow = x1;
    uCol = y1;
    if(delta_x > 0) incx = 1;        
    else if(delta_x == 0) incx = 0;  
    else { incx = -1; delta_x = -delta_x; }
    if(delta_y > 0) incy = 1;        
    else if(delta_y == 0) incy = 0;  
    else { incy = -1; delta_y = -delta_y; }
    if(delta_x > delta_y) distance = delta_x; 
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++) 
    {  
        LCD_DrawPoint(uRow,uCol); 
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }  
}    
// 画矩形   
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1,y1,x2,y1); 
    LCD_DrawLine(x1,y1,x1,y2); 
    LCD_DrawLine(x1,y2,x2,y2); 
    LCD_DrawLine(x2,y1,x2,y2); 
}
// 在指定位置画一个指定大小的圆
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
    int a,b;
    int di;
    a=0;b=r;   
    di=3-(r<<1); 
    while(a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);             
        LCD_DrawPoint(x0 + b, y0 - a);             
        LCD_DrawPoint(x0 + b, y0 + a);             
        LCD_DrawPoint(x0 + a, y0 + b);             
        LCD_DrawPoint(x0 - a, y0 + b);             
        LCD_DrawPoint(x0 - b, y0 + a);             
        LCD_DrawPoint(x0 - a, y0 - b);             
        LCD_DrawPoint(x0 - b, y0 - a);             
        
        a++;
        if(di < 0) di += 4*a + 6;   
        else
        {
            di += 10 + 4*(a - b);   
            b--;
        }     
    }
}   
// 在指定位置显示一个字符
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{     
    u8 temp,t1; // 删除了未使用的 't'
    u16 y0 = y;
    u16 char_width_pixels = size / 2; // 字符的像素宽度，通常是高度的一半
    
    if(num < ' ' || num > '~') return; 
    // 字符起始位置和整体是否在屏幕范围内
    if(x >= lcddev.width || y >= lcddev.height || (x + char_width_pixels) > lcddev.width || (y + size) > lcddev.height) return;

    u16 csize = ((size / 8) + ((size % 8) ? 1 : 0)) * char_width_pixels; 
    num = num - ' '; 

    // 字符点阵是按列存储，每8行一个字节。
    for(u16 t_byte_offset = 0; t_byte_offset < csize; t_byte_offset++) // 遍历字模的每个字节
    {   
        if(size == 12) temp = asc2_1206[num][t_byte_offset]; 
        else if(size == 16) temp = asc2_1608[num][t_byte_offset]; 
        else if(size == 24) temp = asc2_2412[num][t_byte_offset]; 
        else if(size == 32) temp = asc2_3216[num][t_byte_offset]; 
        else return; 

        for(t1 = 0; t1 < 8; t1++) // 遍历每个字节的8个位（像素）
        {     
            // 计算当前像素的绝对X坐标 (基于当前字节属于哪一列)
            u16 current_pixel_x = x + (t_byte_offset / (size / 8 + ((size % 8) ? 1 : 0))); // 计算当前字节属于的列
            
            // 计算当前像素的绝对Y坐标 (基于当前字节是哪一行，以及字节内的位)
            u16 current_pixel_y = y0 + (t_byte_offset % (size / 8 + ((size % 8) ? 1 : 0))) * 8 + t1;
            
            // 如果计算出来的像素超出屏幕范围，则不绘制并返回
            if(current_pixel_x >= lcddev.width || current_pixel_y >= lcddev.height) return;
            
            if(temp & 0x80) LCD_Fast_DrawPoint(current_pixel_x,current_pixel_y,POINT_COLOR);
            else if(mode == 0) LCD_Fast_DrawPoint(current_pixel_x,current_pixel_y,BACK_COLOR);
            
            temp <<= 1; 
        }  
    }              
}   
// m的n次方函数
u32 LCD_Pow(u8 m,u8 n)
{
    u32 result = 1;
    while(n--) result *= m;    
    return result;
}
// 显示数字,高位为0,则不显示
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         
    u8 t,temp;
    u8 enshow = 0;    
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}
// 显示数字,高位为0,仍显示
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
    u8 t,temp;
    u8 enshow = 0;    
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode & 0X80) LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);  
                else LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);  
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
    }
}
// 显示字符串
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
    u16 x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' ')) // 判断是不是合法ASCII字符
    {       
        if(x >= width) { x = x0; y += size; } // 超出区域宽度，换行
        if(y >= height) break;                // 超出区域高度，退出
        
        LCD_ShowChar(x, y, *p, size, 0); // 显示当前字符
        x += size / 2;                   // 移动到下一个字符的X坐标
        p++;                             // 指向下一个字符
    }  
}

```

### 3.3 main.c

```c
#include "bsp_init.h" 
#include "lcd.h"      
#include "stdio.h"    
#include "delay.h"    

// 定义一些测试用的颜色
#define TEST_RED      0xF800
#define TEST_GREEN    0x07E0
#define TEST_BLUE     0x001F
#define TEST_YELLOW   0xFFE0
#define TEST_CYAN     0x7FFF
#define TEST_MAGENTA  0xF81F
#define TEST_WHITE    0xFFFF
#define TEST_BLACK    0x0000

// Main函数
int main(void)
{
    u8 test_stage = 0; 
    char lcd_id_str[16];   
    char resolution_str[30]; 
    
    bsp_init(); 
    LCD_Init();           			
    sprintf(lcd_id_str, "LCD ID:%04X", lcddev.id);

    while(1) 
    {		 
        switch(test_stage % 4) 
        {
            case 0: BACK_COLOR = WHITE;   break;
            case 1: BACK_COLOR = GREEN;   break;
            case 2: BACK_COLOR = BLUE;    break;
            case 3: BACK_COLOR = RED;     break;
        }
        LCD_Clear(BACK_COLOR); 
        
        POINT_COLOR = BLACK; 
        
        sprintf(resolution_str, "Current W:H = %d:%d", lcddev.width, lcddev.height);
        LCD_ShowString(10, 10, lcddev.width - 20, 24, 24, (u8*)"Explorer STM32F4");	
        LCD_ShowString(10, 40, lcddev.width - 20, 16, 16, (u8*)resolution_str); 
        LCD_ShowString(10, 60, lcddev.width - 20, 16, 16, (u8*)"HAZY1K");
        LCD_ShowString(10, 80, lcddev.width - 20, 16, 16, (u8*)lcd_id_str); 	      					 
        LCD_ShowString(10, 100, lcddev.width - 20, 12, 12, (u8*)"2025/7/30 (Updated)"); 

        switch(test_stage)
        {
            case 0: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 0: Basic Info Display");
                break;
            case 1: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 1: Draw Points Test");
                POINT_COLOR = TEST_BLUE;
                for (u16 i = 0; i < 50; i++)
                {
                    if((i + 10) < lcddev.width && (i + 150) < lcddev.height) 
                        LCD_DrawPoint(i + 10, i + 150);
                    if((i + 10) < lcddev.width && (200 - i) < lcddev.height) 
                        LCD_Fast_DrawPoint(i + 10, 200 - i, TEST_RED);
                }
                break;
           case 2: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 2: Draw Lines Test");
                POINT_COLOR = TEST_GREEN;
                LCD_DrawLine(10, 150, 150, 200);
                LCD_DrawLine(10, 200, 150, 150);
                LCD_DrawLine(80, 150, 80, 250);
                LCD_DrawLine(10, 220, 200, 220);
                break;
            case 3: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 3: Draw Rects Test");
                POINT_COLOR = TEST_MAGENTA;
                LCD_DrawRectangle(20, 150, 120, 250); 
                
                POINT_COLOR = TEST_CYAN;
                LCD_Fill(150, 160, 250, 210, TEST_YELLOW); 
                break; 
            case 4: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 4: Draw Circles Test");
                POINT_COLOR = TEST_BLUE;
                LCD_Draw_Circle(80, 200, 40); 
                POINT_COLOR = TEST_GREEN;
                LCD_Draw_Circle(200, 180, 20); 
                break;
            case 5: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 5: Chars & Nums Test");
                POINT_COLOR = TEST_RED;
                LCD_ShowChar(10, 160, 'G', 32, 0);
                LCD_ShowChar(40, 160, 'A', 32, 0);
                LCD_ShowChar(70, 160, 'M', 32, 0);
                LCD_ShowChar(100, 160, 'E', 32, 0);
                LCD_ShowNum(10, 200, 12345, 5, 24);
                LCD_ShowxNum(10, 230, 007, 3, 24, 0X80);
                LCD_ShowxNum(10, 260, 987654321, 9, 16, 0);
                break;
            case 6: // 字符串显示和屏幕旋转测试
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 6: String & Rotate Test");
                POINT_COLOR = BLACK;
                LCD_ShowString(10, 300, lcddev.width - 20, 50, 16, (u8*)"Testing string wrapping here with a long sentence.");
                delay_ms(1000); 
                
                // 屏幕旋转到横屏
                // 修改这一行，尝试不同的扫描方向来解决镜像问题
                // 原始: LCD_Display_Dir(1); // 内部调用 U2D_L2R
                // 尝试新的方向
                lcddev.dir = 1; // 横屏模式
                lcddev.wramcmd = 0X2C;
                lcddev.setxcmd = 0X2A;
                lcddev.setycmd = 0X2B;  
                // 这里是关键的改变：
                LCD_Scan_Dir(U2D_R2L); // 尝试使用 U2D_R2L (从上到下，从右到左，行列交换)
                // 或者你也可以尝试 D2U_L2R (从下到上，从左到右，行列交换)
                // LCD_Scan_Dir(D2U_L2R); 
                
                LCD_Clear(MAGENTA); 
                POINT_COLOR = WHITE;
                sprintf(resolution_str, "H_Mode W:H = %d:%d", lcddev.width, lcddev.height); 
                LCD_ShowString(10, 10, lcddev.width - 20, 50, 24, (u8*)"Horizontal Mode!");
                LCD_ShowString(10, 40, lcddev.width - 20, 16, 16, (u8*)resolution_str);
                LCD_ShowString(10, 60, lcddev.width - 20, 16, 16, (u8*)lcd_id_str);
                delay_ms(1000); 
                
                // 切换回竖屏
                LCD_Display_Dir(0); 
                break; 
            default: 
                test_stage = 0;
                continue; 
        }
        test_stage++; 
        delay_ms(1000);
    } 
}

```

## 4. LCD API函数总结

### 4.1 LCD 基本信息和状态变量

- `_lcd_dev lcddev;`：LCD 参数结构体，包含：
  - `lcddev.width`：当前 LCD 逻辑宽度（会随横竖屏切换而改变）。
  - `lcddev.height`：当前 LCD 逻辑高度（会随横竖屏切换而改变）。
  - `lcddev.id`：LCD ID (强制为 9486)。
  - `lcddev.dir`：当前屏幕方向（0: 竖屏，1: 横屏）。
  - `lcddev.wramcmd`：写入 GRAM 指令。
  - `lcddev.setxcmd`：设置 X 坐标指令。
  - `lcddev.setycmd`：设置 Y 坐标指令。
- `u32 POINT_COLOR;`：全局画笔颜色变量，在绘图前设置即可改变绘图颜色。
- `u32 BACK_COLOR;`：全局背景颜色变量，在清屏或非叠加模式显示时使用。
- `#define LCD_LED PBout(15)`：LCD 背光控制宏（PB15 引脚）

### 4.2 初始化和控制

- `void LCD_Init(void);`
  
  - **作用：** 初始化 LCD 控制器（ILI9486），配置 FSMC，设置时序，执行必要的软件复位和初始化序列，并默认将屏幕设置为竖屏模式并清屏为白色。
  
  - **使用示例：**
  
  ```c
  // 在main函数开始时调用一次
  bsp_init(); // 假设你的BSP初始化包含了延时和串口，FSMC的MspInit会被HAL_SRAM_Init调用
  LCD_Init(); 
  ```
  
  - **效果：** LCD 屏幕点亮，显示初始化信息到串口，屏幕清屏为白色（或你设置的 `BACK_COLOR`）。

- `void LCD_DisplayOn(void);`
  
  - **作用：** 开启 LCD 显示。
  
  - **使用示例：** 在 `LCD_Init` 后通常是默认开启的，如果之前调用过 `LCD_DisplayOff`，需要再次开启。
  
  ```c
  LCD_DisplayOn();
  ```
  
  - **效果：** 屏幕显示内容。

- `void LCD_DisplayOff(void);`
  
  - **作用：** 关闭 LCD 显示（进入待机模式）。
  
  - **使用示例：**
  
  ```c
  LCD_DisplayOff();
  ```
  
  - **效果：** 屏幕变黑（不显示内容），但背光可能仍然亮着（取决于 `LCD_LED` 状态）。

- `void LCD_Clear(u32 Color);`
  
  - **作用：** 使用指定的颜色填充整个屏幕。**此函数内部会设置全屏窗口并恢复。**
  
  - **使用示例：**
  
  ```c
  LCD_Clear(WHITE);  // 清屏为白色
  LCD_Clear(BLACK);  // 清屏为黑色
  ```
  
  - **效果：** 整个 LCD 屏幕瞬间变为指定颜色。

- `void LCD_SetCursor(u16 Xpos, u16 Ypos);`
  
  - **作用：** 设置 GRAM (Graphics RAM) 写入的起始坐标。此函数会将写入区域设置为从 `(Xpos, Ypos)` 到当前逻辑屏幕的右下角。主要用于**流式写入**。
  
  - **使用示例：** 通常在需要连续写入像素数据前调用，例如 `LCD_ReadPoint` 或如果手动实现图像绘制。
  
  ```c
  LCD_SetCursor(50, 50); // 设置写入从 (50,50) 开始
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(RED); // 写入一个像素
  // ... 继续写入会自动递增地址
  ```
  
  - **效果：** 改变 GRAM 内部地址指针，影响后续 `LCD_WriteRAM` 的写入位置。

- `void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);`
  
  - **作用：** 设置一个矩形区域作为 GRAM 的写入窗口。所有后续的 `LCD_WriteRAM` 操作都会被限制在这个窗口内。**在局部绘制完成后，通常需要调用 `LCD_Set_Window(0, 0, lcddev.width, lcddev.height);` 恢复全屏窗口。**
  
  - **使用示例：** 主要由填充函数（`LCD_Fill`, `LCD_Color_Fill`）和字符显示函数（`LCD_ShowChar`）内部调用。
  
  ```c
  LCD_Set_Window(50, 50, 100, 100); // 设置一个100x100的窗口，左上角(50,50)
  LCD_WriteRAM_Prepare();
  for(int i=0; i<100*100; i++) {
      LCD_WriteRAM(BLUE); // 在窗口内填充蓝色
  }
  LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // 恢复全屏窗口
  ```
  
  - **效果：** 限制像素写入的区域。

- `void LCD_Scan_Dir(u8 dir);`
  
  - **作用：** 设置 LCD 硬件扫描方向（通过修改 `0x36` 寄存器）。这个函数会根据传入的方向调整 `lcddev.width` 和 `lcddev.height` 的值，并重新设置 GRAM 寻址范围。
  - **使用示例：** 由 `LCD_Display_Dir` 内部调用。通常不需要直接调用。

- `void LCD_Display_Dir(u8 dir);`
  
  - **作用：** 设置屏幕的显示方向（0: 竖屏，1: 横屏）。此函数会相应地更新 `lcddev.dir` 和 `lcddev.width/height`，并调用 `LCD_Scan_Dir` 来应用硬件设置。**此函数内部会设置并恢复全屏窗口。**
  
  - **使用示例：**
  
  ```c
  LCD_Display_Dir(0); // 切换到竖屏模式
  // ... 绘制竖屏内容
  delay_ms(1000);
  LCD_Display_Dir(1); // 切换到横屏模式
  // ... 绘制横屏内容
  ```
  
  - **效果：** 屏幕显示方向切换，`lcddev.width` 和 `lcddev.height` 会自动更新以反映当前逻辑分辨率。

### 4.3 绘图函数

- `void LCD_DrawPoint(u16 x,u16 y);`
  
  - **作用：** 在指定 `(x, y)` 坐标处绘制一个点，颜色由全局 `POINT_COLOR` 决定。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = BLUE;
  LCD_DrawPoint(100, 100); 
  ```
  
  - **效果：** 在屏幕上绘制一个指定颜色的像素点。

- `void LCD_Fast_DrawPoint(u16 x,u16 y,u32 color);`
  
  - **作用：** 快速在指定 `(x, y)` 坐标处绘制一个点，使用传入的 `color`。**此函数内部会设置 1x1 窗口并立即恢复。**
  
  - **使用示例：**
  
  ```c
  LCD_Fast_DrawPoint(150, 150, RED);
  ```
  
  - **效果：** 绘制一个指定颜色的像素点。此函数通常比 `LCD_DrawPoint` 略快，因为它避免了全局颜色变量的访问，但更重要的优化是其内部窗口管理。

- `u32 LCD_ReadPoint(u16 x,u16 y);`
  
  - **作用：** 读取指定 `(x, y)` 坐标处的像素颜色值。
  
  - **使用示例：**
  
  ```c
  u32 pixel_color = LCD_ReadPoint(100, 100);
  if (pixel_color == RED) {
      // ...
  }
  ```
  
  - **效果：** 返回指定点的 RGB565 颜色值。

- `void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);`
  
  - **作用：** 在 `(x1, y1)` 和 `(x2, y2)` 之间绘制一条直线，颜色由全局 `POINT_COLOR` 决定。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = GREEN;
  LCD_DrawLine(10, 10, 200, 200);
  ```
  
  - **效果：** 绘制一条直线。

- `void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);`
  
  - **作用：** 绘制一个由 `(x1, y1)` 和 `(x2, y2)` 定义的空心矩形，颜色由全局 `POINT_COLOR` 决定。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = BLUE;
  LCD_DrawRectangle(50, 50, 150, 100);
  ```
  
  - **效果：** 绘制一个空心矩形。

- `void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);`
  
  - **作用：** 在 `(x0, y0)` 处绘制一个半径为 `r` 的空心圆，颜色由全局 `POINT_COLOR` 决定。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = YELLOW;
  LCD_Draw_Circle(160, 240, 50); // 在屏幕中心绘制半径50的圆
  ```
  
  - **效果：** 绘制一个空心圆。

- `void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color);`
  
  - **作用：** 使用指定的 `color` 填充从 `(sx, sy)` 到 `(ex, ey)` 的矩形区域。**此函数内部会设置局部窗口并恢复。**
  
  - **使用示例：**
  
  ```c
  LCD_Fill(10, 10, 50, 50, RED); // 填充一个红色小方块
  ```
  
  - **效果：** 填充指定矩形区域为纯色。

- `void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);`
  
  - **作用：** 使用 `color` 数组中的像素数据填充从 `(sx, sy)` 到 `(ex, ey)` 的矩形区域。`color` 数组的尺寸必须与填充区域的大小匹配。**此函数内部会设置局部窗口并恢复。**
  
  - **使用示例：** 用于显示图片数据。
  
  ```c
  // 假设有一个 10x10 的蓝色像素数据数组
  u16 image_data[100]; 
  // ... 填充 image_data
  LCD_Color_Fill(50, 50, 59, 59, image_data); 
  ```
  
  - **效果：** 在指定区域绘制一个像素图像。

### 4.4 文本显示

- `void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);`
  
  - **作用：** 在 `(x, y)` 坐标处显示一个字符。
  
  - `num`：要显示的 ASCII 字符（' ' 到 '~'）。
  
  - `size`：字体高度（例如 12, 16, 24, 32）。宽度为 `size / 2`。
  
  - `mode`：显示模式。`0` 表示非叠加模式（字符背景填充 `BACK_COLOR`）；`1` 表示叠加模式（字符背景透明）。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = BLACK;
  BACK_COLOR = WHITE;
  LCD_ShowChar(10, 10, 'A', 24, 0); // 在 (10,10) 显示一个白色背景的 'A'
  ```
  
  - **效果：** 在指定位置显示一个字符。

- `void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);`
  
  - **作用：** 在 `(x, y)` 处显示一个数字，高位为 0 时不显示。
  
  - `num`：要显示的数字。
  
  - `len`：数字的位数（决定显示多少位）。
  
  - `size`：字体大小。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = BLUE;
  LCD_ShowNum(10, 50, 123, 5, 16); // 显示 "  123" (前面两个空格)
  ```
  
  - **效果：** 显示数字，前面0不会被显示为实际的0，而是空格。

- `void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);`
  
  - **作用：** 在 `(x, y)` 处显示一个数字，高位为 0 时根据 `mode` 决定是否填充 '0'。
  
  - `num`：要显示的数字。
  
  - `len`：数字的位数。
  
  - `size`：字体大小。
  
  - `mode`：`[7]`: 0, 不填充0; 1, 填充0. `[0]`: 0, 非叠加; 1, 叠加。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = RED;
  LCD_ShowxNum(10, 70, 7, 3, 16, 0X80); // 显示 "007" (前面填充0)
  ```
  
  - **效果：** 显示数字，可控制是否用0填充高位。

- `void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);`
  
  - **作用：** 在指定区域内显示一个字符串，支持自动换行。
  
  - `x,y`：起始坐标。
  
  - `width,height`：字符串可以占据的区域宽度和高度。
  
  - `size`：字体大小。
  
  - `*p`：指向字符串的指针。
  
  - **使用示例：**
  
  ```c
  POINT_COLOR = MAGENTA;
  LCD_ShowString(10, 100, lcddev.width - 20, 50, 16, (u8*)"Hello World! This is a long sentence to test wrapping.");
  ```
  
  - **效果：** 在指定区域内显示字符串，超出区域宽度会自动换行，超出区域高度会截断。

---


