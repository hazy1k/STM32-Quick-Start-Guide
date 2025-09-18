# 第十七章 TIM-基本定时器

## 1. 定时器分类

STM32F1 系列中，共有 8个定时器，分为基本定时器，通用定时器和高级定时器。 

基本定时器 TIM6 和 TIM7 是一个 16位的只能向上计数的定时器，只能定时，没有外部 IO。 

通用定时器 TIM2/3/4/5是一个 16位的可以向上/下计数的定时器，可以定时，可以输出比较，可以输入捕捉， 每个定时器有四个外部IO。

高级定时器 TIM1/8是一个 16位的可以向上/下计数的定时器，可以定时，可以输出比较， 可以输入捕捉，还可以有三相电机互补输出信号，每个定时器有8 个外部 IO。更加具体的分类详情见：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMbas002.png)

## 2. 基本定时器功能框图讲解

基本定时器的核心是时基，不仅基本定时器有，通用定时器和高级定时器也有。学习定时器时，我们先从简单的基本定时器学起， 到了后面的通用和高级定时器的学习中，我们直接跳过时基部分的讲解即可。 基本定时器的功能框图见图：

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMbas003.png" title="" alt="" width="773">

### 2.1 时钟源

定时器时钟 TIMxCLK，即内部时钟 CK_INT，经 APB1预分频器后分频提供，如果APB1 预分频系数等于 1，则频率不变， 否则频率乘以 2，库函数中 APB1预分频的系数是2，即 PCLK1=36M，所以定时器时钟 TIMxCLK = 36*2 = 72M。

### 2.2 计时器时钟

定时器时钟经过 PSC 预分频器之后，即 CK_CNT，用来驱动计数器计数。PSC是一个16 位的预分频器， 可以对定时器时钟 TIMxCLK 进行 1~65536之间的任何一个数进行分频。具体计算方式为：CK_CNT = TIM x CLK/(PSC+1)。

### 2.3 计数器

计数器 CNT 是一个 16 位的计数器，只能往上计数，最大计数值为65535。当计数达到自动重装载寄存器的时候产生更新事件，并清零从头开始计数。

### 2.4 自动重装载寄存器

自动重装载寄存器 ARR 是一个 16位的寄存器，这里面装着计数器能计数的最大数值。当计数到这个值的时候，如果使能了中断的话，定时器就产生溢出中断。

### 2.5 定时时间的计算

#### 2.5.1 定时器的时钟频率

定时器计数器的时钟频率通常是由定时器时钟源（`TIMxCLK`）和预分频器（`PSC`）共同决定的。具体计算公式为：

f<sub>CNT</sub>​ = f<sub>TIMxCLK</sub>​​ / PSC+1

#### 2.5.2 定时器每次计数所需时间

每次计数（即计数器增加 1）所需的时间是：

T<sub>count</sub>​ = 1 / f<sub>CNT</sub>​​ = PSC+1​ / f​<sub>TIMxCLK</sub>

#### 2.5.3 产生一次中断的时间（计数器溢出）

定时器计数器从 0 计数到 `ARR` 时，会发生溢出并产生一个中断。这个过程的时间是：

T<sub>interrupt</sub> ​= ARR+1 / f<sub>CNT</sub> = ​(ARR+1)×(PSC+1)​ / f<sub>TIMxCLK</sub>

#### 2.5.4 在中断服务程序中的次数计数

如果在中断服务程序中使用一个变量 `time` 来记录中断发生的次数，那么在发生 `time` 次中断后，定时器总的定时时间就是：

T<sub>total</sub> ​= T<sub>interrupt</sub> ​× time = ​(ARR+1)×(PSC+1)​×time / f<sub>TIMxCLK</sub>

## 3. 定时器初始化结构体详解

在标准库函数头文件stm32f10x_tim.h中对定时器外设建立了四个初始化结构体，基本定时器只用到其中一个即TIM_TimeBaseInitTypeDef， 其他三个我们在高级定时器章节讲解。

```c
typedef struct {
    uint16_t TIM_Prescaler;       // 预分频器
    uint16_t TIM_CounterMode;     // 计数模式
    uint32_t TIM_Period;          // 定时器周期
    uint16_t TIM_ClockDivision;   // 时钟分频
    uint8_t TIM_RepetitionCounter;// 重复计算器
} TIM_TimeBaseInitTypeDef;
```

- TIM_Prescaler： 定时器预分频器设置，时钟源经该预分频器才是定时器时钟，它设定TIMx_PSC寄存器的值。可设置范围为0至65535，实现1至65536分频。

- TIM_CounterMode： 定时器计数方式，可是在为向上计数、向下计数以及三种中心对齐模式。基本定时器只能是向上计数，即TIMx_CNT只能从0开始递增，并且无需初始化。

- TIM_Period： 定时器周期，实际就是设定自动重载寄存器的值，在事件生成时更新到影子寄存器。可设置范围为0至65535。

- TIM_ClockDivision： 时钟分频，设置定时器时钟CK_INT频率与数字滤波器采样时钟频率分频比，基本定时器没有此功能，不用设置。

- TIM_RepetitionCounter： 重复计数器，属于高级控制寄存器专用寄存器位，利用它可以非常容易控制输出PWM的个数。这里不用设置。

虽然定时器基本初始化结构体有5个成员，但对于基本定时器只需设置其中两个就可以，想想使用基本定时器就是简单。

---

当然，我们还是要详细解释一下的：

1. **`uint16_t TIM_Prescaler`**:
   
   - **描述**: 预分频器，用于分频定时器时钟。定时器时钟频率被这个值分频后，用于实际的计数。
   - **数据类型**: 16 位无符号整数。
   - **作用**: 控制定时器计数频率。较大的值将降低定时器的计数速度。

2. **`uint16_t TIM_CounterMode`**:
   
   - **描述**: 计数模式，控制计数器的计数方式（如向上计数、向下计数或向上/向下计数）。
   - **数据类型**: 16 位无符号整数。
   - **作用**: 定义计数器的运行模式，例如 `TIM_COUNTERMODE_UP` 表示向上计数。

3. **`uint32_t TIM_Period`**:
   
   - **描述**: 定时器周期。定时器计数到这个值后，会触发中断或者事件。
   - **数据类型**: 32 位无符号整数。
   - **作用**: 决定定时器溢出的时间间隔（以计数器的时钟频率为基准）。

4. **`uint16_t TIM_ClockDivision`**:
   
   - **描述**: 时钟分频，用于进一步分频定时器时钟。
   - **数据类型**: 16 位无符号整数。
   - **作用**: 控制定时器时钟的精度，例如 `TIM_CLOCKDIVISION_DIV1` 表示不分频。

5. **`uint8_t TIM_RepetitionCounter`**:
   
   - **描述**: 重复计算器，用于设置定时器的重复次数。
   - **数据类型**: 8 位无符号整数。
   - **作用**: 主要用于高级定时器的功能，如定时器重复计数的设置。

## 4. 简单举例

假设我们有一个定时器配置的需求，我们希望定时器每 1 秒触发一次中断。假设系统时钟频率为 72 MHz，我们可以使用以下参数进行配置：

- **`TIM_Prescaler`**: 71
  - **解释**: 将 72 MHz 的时钟频率分频到 1 MHz（`72 MHz / (71 + 1) = 1 MHz`）。
- **`TIM_CounterMode`**: `TIM_COUNTERMODE_UP`
  - **解释**: 计数器向上计数。
- **`TIM_Period`**: 999999
  - **解释**: 计数器周期设置为 999999，即每 1 微秒产生一次溢出。周期为 1 秒。
- **`TIM_ClockDivision`**: `TIM_CLOCKDIVISION_DIV1`
  - **解释**: 不对时钟进行额外的分频。
- **`TIM_RepetitionCounter`**: 0
  - **解释**: 在普通定时器中通常设置为 0，表示没有重复计数器的使用。

```c
TIM_TimeBaseInitTypeDef TIM_InitStruct;

TIM_InitStruct.TIM_Prescaler = 71;           // 预分频器
TIM_InitStruct.TIM_CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
TIM_InitStruct.TIM_Period = 999999;         // 周期设置为 999999
TIM_InitStruct.TIM_ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频
TIM_InitStruct.TIM_RepetitionCounter = 0;   // 重复计数器设置为 0

// 假设有一个定时器的初始化函数
TIM_TimeBaseInit(TIM1, &TIM_InitStruct);
```

---

下面我们加大难度，配置 TIM2 和 GPIO 进行 LED 闪烁，即定时器产生一次中断，led翻转状态

### 4.1 硬件设置

- **LED**: 连接到 GPIOA 的 PIN 5。
- **定时器**: 使用 TIM2。

### 4.2 示例代码

1. **初始化 GPIO（LED）**

```c
#include "stm32f4xx.h"  // 替换为适合你的 STM32 系列的头文件
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 GPIOA 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 配置 PA5 为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
```

2. **初始化定时器（TIM2）**

```c
#include "stm32f4xx_tim.h"
#include "stm32f4xx_nvic.h"

void TIM2_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    // 使能 TIM2 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // 配置 TIM2
    TIM_InitStructure.TIM_Period = 999;            // 自动重载寄存器值
    TIM_InitStructure.TIM_Prescaler = 7199;         // 预分频器
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
    // 使能更新中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
    // 配置并使能中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

3. **定时器中断处理函数**

```c
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除中断标志
        GPIO_ToggleBits(GPIOA, GPIO_Pin_5);          // 切换 PA5 状态（LED）
    }
}
```

4. **主函数**

```c
int main(void) {
    SystemInit();   // 初始化系统时钟（假设已配置）
    GPIO_Init();    // 初始化 GPIO（LED）
    TIM2_Init();    // 初始化定时器

    while (1) {
        // 主循环可以处理其他任务
        // 这里不需要处理定时器，因为定时器中断会自动处理 LED 状态翻转
    }
}
```

### 说明

- **GPIO 初始化**: 使用标准外设库函数 `GPIO_Init` 来配置 GPIO。
- **TIM2 初始化**: 使用 `TIM_TimeBaseInit` 配置 TIM2 的计时参数，并使能更新中断。
- **中断处理**: `TIM2_IRQHandler` 处理定时器中断并切换 LED 状态。
- **主循环**: 同样依赖于定时器中断来控制 LED 闪烁。

---

2024.9.10 第一次修订，后期不再维护

2025.1.23 优化结构
