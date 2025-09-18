# 第十五章 TFTLCD显示

## 1. TFTLCD简介

TFT-LCD 即薄膜晶体管液晶显示器。其英文全称为： Thin Film Transistor-Liquid Crystal Display。 TFT-LCD 与无源 TN-LCD、 STN-LCD 的简单矩阵不同，它在液晶显示屏的每一个象素上都设置有一个薄膜晶体管（TFT），可有效地克服非选通时的串扰，使显示液晶屏的静态特性与扫描线数无关，因此大大提高了图像质量。 TFT-LCD 也被叫做真彩液晶显示器。上一章介绍了 OLED 模块，本章，我们给大家介绍 ALIENTEK TFTLCD 模块、

实物图如下：

![屏幕截图 2024 09 20 103223](https://img.picgo.net/2024/09/20/-2024-09-20-103223fc8e00103b646258.png)

模块原理图如下：

![屏幕截图 2024 09 20 103434](https://img.picgo.net/2024/09/20/-2024-09-20-1034349e086ad126eebf26.png)

接口定义图：

![屏幕截图 2024 09 20 103512](https://img.picgo.net/2024/09/20/-2024-09-20-10351255c76f1adad54177.png)

ALIENTEK TFTLCD 模块采用 16 位的并方式与外部连接， 之所以不采用 8 位的方式，是因为彩屏的数据量比较大，尤其在显示图片的时候，如果用 8 位数据线，就会比 16 位方式慢一倍以上，我们当然希望速度越快越好，所以我们选择 16 位的接口。图还列出了触摸屏芯片的接口，关于触摸屏本章我们不多介绍，后面的章节会有详细的介绍。 该模块的 8080 并口有如下一些信号线：

- CS： TFTLCD 片选信号。

- WR：向 TFTLCD 写入数据。

- RD：从 TFTLCD 读取数据。

- D[15： 0]： 16 位双向数据线。

- RST：硬复位 TFTLCD。

- RS：命令/数据标志（0，读写命令； 1，读写数据）。

## 2. 一般TFTLCD模块使用流程

![屏幕截图 2024 09 20 104053](https://img.picgo.net/2024/09/20/-2024-09-20-1040539447a75ca0b5125e.png)

任何 LCD，使用流程都可以简单的用以上流程图表示。其中硬复位和初始化序列，只需要执行一次即可。而画点流程就是：设置坐标→写 GRAM 指令→写入颜色数据，然后在 LCD 上面，我们就可以看到对应的点显示我们写入的颜色了。读点流程为：设置坐标→读 GRAM 指令→读取颜色数据，这样就可以获取到对应点的颜色数据了。

以上只是最简单的操作，也是最常用的操作，有了这些操作，一般就可以正常使用 TFTLCD了。接下来我们将该模块用来来显示字符和数字， 通过以上介绍，我们可以得出 TFTLCD 显示需要的相关设置步骤如下：

### 2.1 设置STM32F4与TFTLCD模块相连接的IO

这一步，先将我们与 TFTLCD 模块相连的 IO 口进行初始化，以便驱动 LCD。这里我们用到的是 FSMC

### 2.2 初始化TFTLCD模块

初始化序列，就是向 LCD 控制器写入一系列的设置值（比如伽马校准），这些初始化序列一般 LCD 供应商会提供给客户，我们直接使用这些序列即可，不需要深入研究。在初始化之后， LCD 才可以正常使用。

### 2.3 通过函数将字符和数字显示到TFTLCD模块上

设置坐标→写 GRAM 指令→写 GRAM 来实现，但是这个步骤，只是一个点的处理，我们要显示字符/数字，就必须要多次使用这个步骤，从而达到显示字符/数字的目的，所以需要设计一个函数来实现数字/字符的显示，之后调用该函数，就可以实现数字/字符的显示了。

## 3. FSMC简介

STM32F407 或 STM32F417 系列芯片都带有 FSMC 接口， ALIENTEK 探索者 STM32F4 开发板的主芯片为 STM32F407ZGT6，是带有 FSMC 接口的。

FSMC，即灵活的静态存储控制器，能够与同步或异步存储器和 16 位 PC 存储器卡连接， STM32F4 的 FSMC 接口支持包括 SRAM、 NAND FLASH、 NOR FLASH 和 PSRAM 等存储器。FSMC 的框图如图：

![屏幕截图 2024 09 20 104703](https://img.picgo.net/2024/09/20/-2024-09-20-104703e6bf60a5b009d723.png)

从上图我们可以看出， STM32F4 的 FSMC 将外部设备分为 2 类： NOR/PSRAM 设备、NAND/PC 卡设备。他们共用地址数据总线等信号，他们具有不同的 CS 以区分不同的设备，比如本章我们用到的 TFTLCD 就是用的 FSMC_NE4 做片选，其实就是将 TFTLCD 当成 SRAM 来控制。

这里我们介绍下为什么可以把 TFTLCD 当成 SRAM 设备用：首先我们了解下外部 SRAM的连接，外部 SRAM 的控制一般有：地址线（如 A0~A18）、数据线（如 D0~D15）、写信号（WE）、读信号（OE）、片选信号（CS），如果 SRAM 支持字节控制，那么还有 UB/LB 信号。而 TFTLCD的信号我们有介绍，包括： RS、 D0~D15、 WR、 RD、 CS、 RST 和 BL 等，其中真正在操作 LCD 的时候需要用到的就只有： RS、 D0~D15、 WR、 RD 和 CS。其操作时序和 SRAM的控制完全类似，唯一不同就是 TFTLCD 有 RS 信号，但是没有地址信号。

TFTLCD 通过 RS 信号来决定传送的数据是数据还是命令，本质上可以理解为一个地址信号，比如我们把 RS 接在 A0 上面，那么当 FSMC 控制器写地址 0 的时候，会使得 A0 变为 0，对 TFTLCD 来说，就是写命令。而 FSMC 写地址 1 的时候， A0 将会变为 1，对 TFTLCD 来说，就是写数据了。这样，就把数据和命令区分开了，他们其实就是对应 SRAM 操作的两个连续地址。当然 RS 也可以接在其他地址线上，探索者 STM32F4 开发板是把 RS 连接在 A6 上面的。

STM32F4 的 FSMC 支持 8/16/32 位数据宽度，我们这里用到的 LCD 是 16 位宽度的，所以在设置的时候，选择 16 位宽就 OK 了。我们再来看看 FSMC 的外部设备地址映像， STM32F4的 FSMC 将外部存储器划分为固定大小为 256M 字节的四个存储块，如图所示：

![屏幕截图 2024 09 20 104754](https://img.picgo.net/2024/09/20/-2024-09-20-104754d51fb9bfecb65195.png)

## 4. FSMC初始化函数

在 HAL 库中提供了 FSMC 初始化函数为

```c
HAL_SRAM_Init ();
```

下面我们看看函数定义：

```c
HAL_StatusTypeDef HAL_SRAM_Init(SRAM_HandleTypeDef *hsram,
FMC_NORSRAM_TimingTypeDef *Timing, FMC_NORSRAM_TimingTypeDef *ExtTiming)
```

这个函数有三个入口参数，SRAM_HandleTypeDef 类 型 指 针 变 量 、FMC_NORSRAM_TimingTypeDef 类型指针变量、 FMC_NORSRAM_TimingTypeDef 类型指针变量。

FMC_NORSRAM_TimingTypeDef 指针类型的成员变量。前面我们讲到， FSMC 有读时序和写时序之分，所以这里就是用来设置读时序和写时序的参数了， 也就是说，这两个参数是用来配置寄存器 FSMC_BTRx 和 FSMC_BWTRx，后面我们会讲解到。下面我们主要来看看模式A 下的相关配置参数：

- 参数 NSBank 用来设置使用到的存储块标号和区号，前面讲过，我们是使用的存储块 1 区号 4，所以选择值为 FSMC_NORSRAM_BANK4。

- 参数 MemoryType 用来设置存储器类型，我们这里是 SRAM，所以选择值为FSMC_MEMORY_TYPE_SRAM。

- 参数 MemoryDataWidth 用来设置数据宽度，可选 8 位还是 16 位，这里我们是 16 位数据宽度，所以选择值为 FSMC_NORSRAM_MEM_BUS_WIDTH_16。

- 参数 WriteOperation 用来设置写使能，毫无疑问，我们前面讲解过我们要向 TFT 写数据，所以要写使能， 这里我们选择 FSMC_WRITE_OPERATION_ENABLE。

- 参数 ExtendedMode 是设置扩展模式使能位，也就是是否允许读写不同的时序，这里我们采取的读写不同时序，所以设置值为 FSMC_EXTENDED_MODE_ENABLE。

上面的这些参数是与模式 A 相关的，下面我们也来稍微了解一下其他几个参数的意义吧：参数 DataAddressMux 用来设置地址/数据复用使能，若设置为使能，那么地址的低 16 位和数据将共用数据总线，仅对 NOR 和 PSRAM 有效，所以我们设置为默认值不复用，值FSMC_DATA_ADDRESS_MUX_DISABLE。

其他参数在成组模式同步模式才需要设置，大家可以参考中文参考手册了解相关参数的意思。

接下来我们看看设置读写时序参数的两个变量 FSMC_ReadWriteTim 和 FSMC_WriteTim，他们都是 FSMC_NORSRAM_TimingTypeDef 结构体指针类型，这两个参数在初始化的时候分别用来初始化片选控制寄存器 FSMC_BTRx 和写操作时序控制寄存器 FSMC_BWTRx。 下面我们看看 FSMC_NORSRAMTimingInitTypeDef 类型的定义：

```c
typedef struct
{
    uint32_t AddressSetupTime;
    uint32_t AddressHoldTime;
    uint32_t DataSetupTime;
    uint32_t BusTurnAroundDuration;
    uint32_t CLKDivision;
    uint32_t DataLatency;
    uint32_t AccessMode;
}FSMC_NORSRAM_TimingTypeDef
```

这个结构体有 7 个参数用来设置 FSMC 读写时序。其实这些参数的意思我们前面在讲解 FSMC的时序的时候有提到，主要是设计地址建立保持时间，数据建立时间等等配置，对于我们的实验中，读写时序不一样，读写速度要求不一样，所以对于参数 FSMC_DataSetupTime 设置了不同的值，大家可以对照理解一下。

## 5. FSMC 初始化和TFT LCD 驱动代码示例

```c
#include "stm32f4xx_hal.h"

// 定义FSMC句柄
SRAM_HandleTypeDef hsram;

// 该函数用于初始化FSMC
void FSMC_Init(void)
{
    FSMC_NORSRAM_TimingTypeDef Timing;

    // 使能FSMC外设时钟
    __HAL_RCC_FSMC_CLK_ENABLE();

    // 配置NORSRAM控制器参数
    hsram.Instance = FSMC_NORSRAM_DEVICE; // 使用NORSRAM设备
    hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE; // 不使用扩展模式
    hsram.Init.NSBank = FSMC_NORSRAM_BANK1; // 选择银行1
    hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; // 禁用地址数据复用
    hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM; // 设置内存类型为SRAM
    hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; // 数据宽度为16位
    hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; // 禁用突发访问模式
    hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; // 等待信号极性为低
    hsram.Init.WrapMode = FSMC_WRAP_MODE_DISABLE; // 禁用包装模式
    hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS; // 等待信号在写入之前激活
    hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE; // 启用写操作
    hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE; // 禁用等待信号
    hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE; // 禁用扩展模式
    hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; // 禁用异步等待
    hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE; // 禁用写突发

    // 配置时序参数
    Timing.AddressSetupTime = 0; // 地址建立时间
    Timing.AddressHoldTime = 0; // 地址保持时间
    Timing.DataSetupTime = 15; // 数据建立时间
    Timing.BusTurnAroundDuration = 0; // 总线转向时间
    Timing.ClockDivision = 0; // 时钟分频
    Timing.DataLatency = 0; // 数据延迟
    Timing.AccessMode = FSMC_ACCESS_MODE_A; // 访问模式A

    // 初始化FSMC NORSRAM
    HAL_FSMC_NORSRAM_Init(&hsram, &Timing);
}

// 该函数用于初始化LCD控制器
void LCD_Init(void)
{
    // 发送LCD初始化命令
    LCD_Write_Command(0x01); // 软件复位
    HAL_Delay(150); // 等待150毫秒
    LCD_Write_Command(0x28); // 关闭显示
    // 更多初始化命令...
    LCD_Write_Command(0x29); // 打开显示
}

// 发送命令到LCD
void LCD_Write_Command(uint8_t cmd)
{
    // 选择命令模式，RS引脚设置为低电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // RS = 0
    FSMC_NORSRAM->BTR = cmd; // 将命令写入FSMC寄存器
    // 根据需要添加其他必要的设置，例如延时等
}

// 发送数据到LCD
void LCD_Write_Data(uint8_t data)
{
    // 选择数据模式，RS引脚设置为高电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // RS = 1
    FSMC_NORSRAM->BTR = data; // 将数据写入FSMC寄存器
    // 根据需要添加其他必要的设置，例如延时等
}

// 主函数
int main(void)
{
    HAL_Init(); // 初始化HAL库
    SystemClock_Config(); // 配置系统时钟
    FSMC_Init(); // 初始化FSMC
    LCD_Init(); // 初始化LCD

    // 绘制示例
    LCD_Write_Data(0xFF); // 设置为白色（根据实际情况设置颜色值）
    // 其他绘制操作...

    while (1) // 主循环
    {
        // 主程序逻辑
    }
}
```

---

2024.10.7 第一次修订，后期不再维护
