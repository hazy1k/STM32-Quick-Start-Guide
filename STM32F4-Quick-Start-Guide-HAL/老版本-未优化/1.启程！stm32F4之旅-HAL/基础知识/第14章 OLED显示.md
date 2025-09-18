# 第十四章 OLED显示

## 1. 基本介绍

OLED，即有机发光二极管（Organic Light-Emitting Diode），又称为有机电激光显示（Organic Electroluminesence Display， OELD）。 OLED 由于同时具备自发光，不需背光源、对比度高、厚度薄、视角广、反应速度快、可用于挠曲性面板、使用温度范围广、构造及制程较简单等优异之特性，被认为是下一代的平面显示器新兴应用技术。

LCD 都需要背光，而 OLED 不需要，因为它是自发光的。这样同样的显示， OLED 效果要来得好一些。 以目前的技术， OLED 的尺寸还难以大型化，但是分辨率确可以做到很高。在本章中，我们使用的是 ALINETEK 的 OLED 显示模块，该模块有以下特点：

1. 模块有单色和双色两种可选，单色为纯蓝色，而双色则为黄蓝双色。

2. 尺寸小，显示尺寸为 0.96 寸，而模块的尺寸仅为 27mm*26mm 大小。

3. 高分辨率，该模块的分辨率为 128*64。

4. 多种接口方式，该模块提供了总共 4 种接口包括： 6800、 8080 两种并行接口方式、 4线 SPI 接口方式以及 IIC 接口方式

5. 不需要高压，直接接 3.3V 就可以工作了。

这里要提醒大家的是，该模块不和 5.0V 接口兼容，所以请大家在使用的时候一定要小心，别直接接到 5V 的系统上去，否则可能烧坏模块。以上 4 种模式通过模块的 BS1 和 BS2 设置， BS1 和 BS2 的设置与模块接口模式的关系如表所示：

![屏幕截图 2024 09 19 104354](https://img.picgo.net/2024/09/19/-2024-09-19-10435455969228e1f7073c.png)

其中1代表VCC，0代表的是GND

该模块的外观图所示：

![屏幕截图 2024 09 19 104517](https://img.picgo.net/2024/09/19/-2024-09-19-10451710f055dcaf4581b3.png)

ALIENTEK OLED 模块默认设置是： BS1 和 BS2 接 VCC ，即使用 8080 并口方式，如果你想要设置为其他模式，则需要在 OLED 的背面，用烙铁修改 BS1 和 BS2 的设置。

<img src="https://img.picgo.net/2024/09/19/-2024-09-19-10475145e3b82b3546e2c8.png" title="" alt="屏幕截图 2024 09 19 104751" width="697">

该模块采用 8*2 的 2.54 排针与外部连接，总共有 16 个管脚，在 16 条线中，我们只用了 15条，有一个是悬空的。 15 条线中，电源和地线占了 2 条，还剩下 13 条信号线。在不同模式下，我们需要的信号线数量是不同的，在 8080 模式下，需要全部 13 条，而在 IIC 模式下，仅需要2 条线就够了！这其中有一条是共同的，那就是复位线 RST（RES）， RST 上的低电平，将导致OLED 复位，在每次初始化之前，都应该复位一下 OLED 模块。

ALIENTEK OLED 模块的控制器是 SSD1306， 本章，我们将学习如何通过 STM32F4 来控制该模块显示字符和数字，本章的实例代码将可以支持两种方式与 OLED 模块连接，一种是8080 的并口方式，另外一种是 4 线 SPI 方式。

## 2. 8080并行接口

首先我们介绍一下模块的 8080 并行接口， 8080 并行接口的发明者是 INTEL，该总线也被广泛应用于各类液晶显示器， ALIENTEK OLED 模块也提供了这种接口，使得 MCU 可以快速的访问 OLED。 ALIENTEK OLED 模块的 8080 接口方式需要如下一些信号线：

- CS： OLED 片选信号。

- WR：向 OLED 写入数据。

- RD：从 OLED 读取数据。

- D[7:0]： 8 位双向数据线。

- RST(RES)：硬复位 OLED。

- DC：命令/数据标志（0，读写命令； 1，读写数据）。

模块的 8080 并口读/写的过程为：先根据要写入/读取的数据的类型，设置 DC 为高（数据） /低（命令），然后拉低片选，选中 SSD1306，接着我们根据是读数据，还是要写数据置 RD/WR为低，然后：

在 RD 的上升沿， 使数据锁存到数据线（D[7:0]）上；

在 WR 的上升沿，使数据写入到 SSD1306 里面；

SSD1306 的 8080 接口方式下，控制脚的信号状态所对应的功能如表：

![屏幕截图 2024 09 19 105148](https://img.picgo.net/2024/09/19/-2024-09-19-105148d5777ede0ecd034d.png)

在 8080 方式下读数据操作的时候，我们有时候（例如读显存的时候）需要一个假读命（Dummy Read），以使得微控制器的操作频率和显存的操作频率相匹配。在读取真正的数据之前，由一个的假读的过程。这里的假读，其实就是第一个读到的字节丢弃不要，从第二个开始，才是我们真正要读的数据。

### 2.1 使用举例

#### 硬件连接

1. **OLED显示模块**：
   
   - 使用适合8080并行接口的OLED显示屏（例如SSD1306或类似）。
   - 确保OLED模块支持8080并行接口，并查阅其数据手册了解引脚配置。

2. **STM32引脚连接**：
   
   - 将OLED的控制信号连接到STM32的GPIO端口。
   - 典型连接如下（具体引脚可根据实际情况调整）：
     - **D0-D7**（数据线）: PA0-PA7
     - **DC**（数据/命令选择）: PB0
     - **RST**（复位）: PB1
     - **WR**（写使能）: PB2
     - **CS**（片选）: PB3

#### 硬件连接示意图

```cs
STM32      OLED
--------------------
PA0  <--> D0
PA1  <--> D1
PA2  <--> D2
PA3  <--> D3
PA4  <--> D4
PA5  <--> D5
PA6  <--> D6
PA7  <--> D7
PB0  <--> DC
PB1  <--> RST
PB2  <--> WR
PB3  <--> CS
```

#### 编写代码

以下是一个简单的代码示例，展示如何通过8080并行接口驱动OLED显示器：

```c
#include "main.h"

// GPIO初始化
void GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 初始化数据引脚（PA0-PA7）
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // 初始化控制引脚
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3; // PB0, PB1, PB2, PB3
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// 发送数据到OLED
void OLED_Write(uint8_t data, uint8_t isData) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, isData); // 设置DC引脚
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); // 激活片选

    // 发送数据到OLED
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 << i, (data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 设置数据线
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); // 发送WR脉冲
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);   // 结束WR脉冲
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);   // 取消片选
}

// OLED初始化
void OLED_Init(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // 复位OLED
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // 复位结束

    // OLED初始化指令
    OLED_Write(0xAE, 0); // 设置显示关闭
    OLED_Write(0xD5, 0); // 设置时钟分频
    OLED_Write(0x80, 0); // 指定时钟
    OLED_Write(0xA8, 0); // 设置多路复用
    OLED_Write(0x3F, 0); // 64
    OLED_Write(0xD3, 0); // 设置显示偏移
    OLED_Write(0x00, 0); // 无偏移
    OLED_Write(0x40, 0); // 设置起始行
    OLED_Write(0xAD, 0); // 设置充电泵
    OLED_Write(0x8B, 0); // 开启充电泵
    OLED_Write(0xA1, 0); // 设置段重映射
    OLED_Write(0xC8, 0); // 设置COM扫描方向
    OLED_Write(0xDA, 0); // 设置COM硬件配置
    OLED_Write(0x12, 0); // 设置COM硬件配置
    OLED_Write(0x81, 0); // 设置对比度控制
    OLED_Write(0x7F, 0); // 对比度值
    OLED_Write(0xA4, 0); // 输出RAM内容
    OLED_Write(0xA6, 0); // 设置正常显示
    OLED_Write(0xAF, 0); // 开启显示
}

// 主函数
int main(void) {
    HAL_Init();
    SystemClock_Config(); // 时钟配置
    GPIO_Init();
    OLED_Init();

    // 清屏
    for (uint8_t page = 0; page < 8; page++) {
        OLED_Write(0xB0 + page, 0); // 设置页地址
        OLED_Write(0x00, 0); // 设置列低地址
        OLED_Write(0x10, 0); // 设置列高地址
        for (uint8_t col = 0; col < 128; col++) {
            OLED_Write(0x00, 1); // 写入0清屏
        }
    }
    // 示例：绘制一个简单的图形
    // ... 可以在此添加你的绘图代码
    while (1) {
        // 主循环
    }
}
```

## 3. SPI接口

我们接下来介绍一下 4 线串行（SPI）方式， 4 先串口模式使用的信号线有如下几条：

- CS： OLED 片选信号。

- RST(RES)：硬复位 OLED。

- DC：命令/数据标志（0，读写命令； 1，读写数据）。

- SCLK：串行时钟线。在 4 线串行模式下， D0 信号线作为串行时钟线 SCLK。

- SDIN：串行数据线。在 4 线串行模式下， D1 信号线作为串行数据线 SDIN。

模块的 D2 需要悬空，其他引脚可以接到 GND。在 4 线串行模式下，只能往模块写数据而不能读数据。

在 4 线 SPI 模式下，每个数据长度均为 8 位，在 SCLK 的上升沿，数据从 SDIN 移入到SSD1306，并且是高位在前的。 DC 线还是用作命令/数据的标志线。

### 3.1 使用举例

#### 连接方式

将OLED显示模块与STM32的引脚连接如下（示例连接）：

![屏幕截图 2024 10 05 103408](https://img.picgo.net/2024/10/05/-2024-10-05-103408369a63beb8b19662.png)

#### 示例代码

以下是主要的代码部分，包括初始化SSD1306和发送数据：

```c
#include "stm32f1xx_hal.h"  // 包含STM32 HAL库
#include "ssd1306.h"        // 包含SSD1306 OLED显示器的驱动库

// SPI句柄，用于管理SPI配置和状态
SPI_HandleTypeDef hspi1;

// SPI初始化函数
void SPI_Init(void) {
    // 配置SPI1设置
    hspi1.Instance = SPI1;  // 使用SPI1
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // 设置波特率分频
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;  // 双线通信
    hspi1.Init.Mode = SPI_MODE_MASTER;             // 设置为主模式
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;      // 数据大小为8位
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;     // 时钟极性为低电平
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;         // 时钟相位为第一个边沿
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;        // 数据从高位开始传输
    HAL_SPI_Init(&hspi1);  // 初始化SPI
}

// OLED显示初始化函数
void OLED_Init(void) {
    // 先将复位引脚拉低，等待一段时间
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // RESET引脚拉低
    HAL_Delay(100);  // 延迟100毫秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);   // 将RESET引脚拉高
    // 调用SSD1306的初始化函数
    ssd1306_Init(); // 初始化OLED显示屏
}

// 主函数入口
int main(void) {
    HAL_Init();  // 初始化HAL库
    SystemClock_Config(); // 配置系统时钟
    SPI_Init();  // 初始化SPI接口
    // OLED初始化
    OLED_Init();
    // 清空显示，设置为黑色背景
    ssd1306_Fill(Black);
    // 设置光标位置
    ssd1306_SetCursor(10, 10); // 在坐标(10,10)处设置光标
    // 显示字符串，使用指定字体和颜色
    ssd1306_WriteString("Hello, World!", Font_11x18, White); // 显示"Hello, World!"字串
    // 刷新OLED显示，更新屏幕内容
    ssd1306_UpdateScreen();
    // 主循环
    while (1) {
        // 可以在此处添加其他功能，例如定时更新显示内容
    }
}
```

## 4. 配置OLED基本步骤

### 4.1 设置STM32F4与OLED模块相连接的IO

这一步，先将我们与 OLED 模块相连的 IO 口设置为输出，具体使用哪些 IO 口，这里需要根据连接电路以及 OLED 模块所设置的通讯模式来确定。这些将在硬件设计部分向大家介绍。

### 4.2 初始化OLED模块

![屏幕截图 2024 09 19 105540](https://img.picgo.net/2024/09/19/-2024-09-19-105540ce2e8f3431d69b14.png)

其实这里就是上面的初始化框图的内容，通过对 OLED 相关寄存器的初始化，来启动 OLED的显示。为后续显示字符和数字做准备。

### 4.3 通过函数将字符和数字显示到OLED模块上

这里就是通过我们设计的程序，将要显示的字符送到 OLED 模块就可以了，这些函数将在软件设计部分向大家介绍。

通过以上三步，我们就可以使用 ALIENTEK OLED 模块来显示字符和数字了，在后面我们还将会给大家介绍显示汉字的方法。这一部分就先介绍到这里。

---

2024.10.5 第一次修订，后期不再维护
