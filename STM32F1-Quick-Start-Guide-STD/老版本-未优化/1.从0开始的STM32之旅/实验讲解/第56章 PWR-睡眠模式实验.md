# 第五十六章 PWR-睡眠模式实验

## 1. 硬件设计

实验中的硬件主要使用到了按键、LED彩灯以及使用串口输出调试信息， 这些硬件都与前面相应实验中的一致，涉及到硬件设计的可参考原理图或前面章节中的内容。

## 2. 软件设计

### 2.1 编程大纲

1. EXTI相关参数宏定义

2. EXTI配置

3. 中断服务函数

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 EXTI相关参数宏定义

```c
/* KEY1 */
#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_PIN GPIO_Pin_0
#define KEY1_GPIO_CLK (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define KEY1_EXTI_PORTSOURCE GPIO_PortSourceGPIOA
#define KEY1_EXTI_PINSOURCE GPIO_PinSource0
#define KEY1_EXTI_LINE EXTI_Line0
#define KEY1_EXTI_IRQn EXTI0_IRQn
#define KEY1_IRQHandler EXTI0_IRQHandler
/* KEY2 */
#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_PIN GPIO_Pin_13
#define KEY2_GPIO_CLK (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
#define KEY2_EXTI_PORTSOURCE GPIO_PortSourceGPIOC
#define KEY2_EXTI_PINSOURCE GPIO_PinSource13
#define KEY2_EXTI_LINE EXTI_Line13
#define KEY2_EXTI_IRQn EXTI15_10_IRQn
#define KEY2_IRQHandler EXTI15_10_IRQHandler
```

#### 2.2.2 中断服务函数

```c
void KEY1_IRQHandler(void)
{
    if(EXTI_GetITStatus(KEY1_EXTI_LINE) != RESET) 
    {
        LED_BLUE();        
        printf("\r\n KEY1 按键中断唤醒 \r\n");    
        EXTI_ClearITPendingBit(KEY1_EXTI_LINE);     
    }  
}

void KEY2_IRQHandler(void)
{
    if(EXTI_GetITStatus(KEY2_EXTI_LINE) != RESET) 
    {
        LED_BLUE();
        printf("\r\n KEY2 按键中断唤醒 \r\n");   
        EXTI_ClearITPendingBit(KEY2_EXTI_LINE);     
    }  
}
```

#### 2.2.3 EXTI配置

```c
#include "key.h"

void Key_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}
void Key_GPIOandEXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
    /* KEY1 引脚配置 */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 输入浮空模式
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_EXTILineConfig(KEY1_EXTI_PORTSOURCE, KEY1_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* KEY2 引脚配置 */
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_EXTILineConfig(KEY2_EXTI_PORTSOURCE, KEY2_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void KEY_EXTI_Init(void)
{
    Key_NVIC_Init();
    Key_GPIOandEXTI_Init();
}
```

#### 2.2.4 主函数

```c
#include "led.h"
#include "usart.h"
#include "key.h"

static void Delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

int main(void)
{
    KEY_EXTI_Init();
    LED_Init();
    USART_Config();
    printf("PWR-Sleep Mode Test\r\n");
    while(1)
    {
        printf("绿灯亮，stm32正常运行\r\n");
        LED_GREEN();
        Delay(0xFFFFFF);
        printf("红灯亮，stm32进入睡眠模式\r\n");
        LED_RED();
        __WFI(); // 进入睡眠模式
        Delay(0xFFFFFF);
        printf("已退出睡眠模式\r\n");
    }
}
```

这个main函数的执行流程见图：

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/Power006.png" title="" alt="" width="501">

1. 程序中首先初始化了LED灯及串口以便用于指示芯片的运行状态， 并且把实验板上的两个按键都初始化成了中断模式，以便当系统进入睡眠模式的时候可以通过按键来唤醒。这些硬件的初始化过程都跟前面章节中的一模一样。

2. 初始化完成后使用LED及串口表示运行状态，在本实验中， LED彩灯为绿色时表示正常运行，红灯时表示睡眠状态，蓝灯时表示刚从睡眠状态中被唤醒。

3. 程序执行一段时间后，直接使用WFI指令进入睡眠模式， 由于WFI睡眠模式可以使用任意中断唤醒，所以我们可以使用按键中断唤醒。在实际应用中，也可以把它改成串口中断、定时器中断等。

4. 当系统进入停止状态后，我们按下实验板上的KEY1或KEY2按键， 即可使系统回到正常运行的状态，当执行完中断服务函数后，会继续执行WFI指令后的代码。

## 3. 小结

### 3.1 实验目标

- 理解STM32的睡眠模式及其工作原理。
- 通过LED指示灯和按键来观察和验证睡眠模式的效果。

### 3.2 实验步骤

#### 1. 硬件连接

- 将LED连接到一个GPIO引脚（例如PA5）。
- 将按键连接到另一个GPIO引脚（例如PC13），并确保其配置为外部中断。
- 使用适当的电阻连接LED，以控制电流。

#### 2. 软件配置

1. **创建新项目**：在STM32CubeIDE中创建一个新项目，选择对应的STM32芯片。
2. **配置GPIO**：
   - 将PA5设置为输出模式，用于控制LED。
   - 将PC13设置为输入模式，并配置为外部中断（EXTI）触发，选择下降沿触发（按键按下）。
3. **配置系统时钟**：确保系统时钟设置为合适的频率。
4. **启用中断**：在NVIC中使能PC13的中断。

#### 3.3 编写代码

```c
#include "stm32f10x.h"

// 声明LED和按键的引脚
#define LED_PIN GPIO_PIN_5
#define LED_PORT GPIOA
#define BUTTON_PIN GPIO_PIN_13
#define BUTTON_PORT GPIOC

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    while (1)
    {
        // 点亮LED，指示系统即将进入睡眠模式
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        HAL_Delay(1000); // 保持1秒

        // 关闭LED，进入睡眠模式
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        // 进入睡眠模式
        __WFI(); // 等待中断（Wait For Interrupt）

        // 唤醒后点亮LED，指示系统已唤醒
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        HAL_Delay(1000); // 保持1秒
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BUTTON_PIN)
    {
        // 按键按下时的处理代码（可选）
    }
}

static void MX_GPIO_Init(void)
{
    // GPIO初始化代码
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LED引脚配置
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // 按键引脚配置
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 下降沿触发
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

    // 配置并使能中断
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// 系统时钟配置函数
void SystemClock_Config(void)
{
    // 系统时钟配置代码（生成的代码）
}
```

---

2024.9.25 第一次修订，后期不再维护

2025.1.29 优化代码
