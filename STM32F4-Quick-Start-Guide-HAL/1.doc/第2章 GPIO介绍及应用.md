# 第二章 GPIO介绍及应用

## 1. STM32F407 GPIO 简介

PIO 是控制或者采集外部器件的信息的外设， 即负责输入输出。它按组分配存在，每组最多 16 个 IO 口，组数视芯片而定。 比如： STM32F407ZGT6 芯片是 144 脚的芯片， 分为 7 组，分别是： GPIOA、 GPIOB、 GPIOC、 GPIOD、 GPIOE、 GPIOF 和 GPIOG， 其中共有 112 个 IO口可供我们编程使用。这里重点说一下 STM32F407 的 IO 电平兼容性问题， STM32F407 的绝大部分 IO 口，都兼容 5V，至于到底哪些是兼容 5V 的，请看 STM32F407ZG 的数据手册（注意是数据手册，不是中文参考手册），见表 5 大容量 STM32F40xxx 引脚定义，凡是有 FT 标志的，都是兼容 5V 电平的 IO 口，可以直接接 5V 的外设（注意：如果引脚设置的是模拟输入模式，则不能接 5V！ )，凡是不带 FT 标志的，就建议大家不要接 5V 了，可能烧坏 MCU。

![屏幕截图 2025-05-28 164103.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/28-16-42-06-屏幕截图%202025-05-28%20164103.png)

## 2. GPIO使用示例

### 2.1 GPIO输出-LED闪烁

![屏幕截图 2025-05-28 165659.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/28-16-57-06-屏幕截图%202025-05-28%20165659.png)

#### 2.1.1 LED相关参数宏定义

```c
#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

#define LED_GPIO_Port GPIOF
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED0_GPIO_Pin GPIO_PIN_9
#define LED1_GPIO_Pin GPIO_PIN_10

// 低电平点亮
#define LED_ON(x)  HAL_GPIO_WritePin(LED_GPIO_Port, x, GPIO_PIN_RESET)
#define LED_OFF(x) HAL_GPIO_WritePin(LED_GPIO_Port, x, GPIO_PIN_SET)

void bsp_led_init(void);

#endif /* __LED_H__ */
```

#### 2.1.2 LED GPIO初始化

```c
#include "led.h"

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    LED_GPIO_CLK_ENABLE(); // 使能GPIO时钟
    GPIO_InitStructure.Pin = LED0_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出 
    GPIO_InitStructure.Pull = GPIO_PULLUP; // 上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; // 高速
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = LED1_GPIO_Pin;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    LED_OFF(LED0_GPIO_Pin|LED1_GPIO_Pin); // 初始关闭LED
}
```

#### 2.1.3 主函数测试

```c
#include "bsp_init.h"

int main(void)
{
    bsp_init();
    while(1)
    {
        LED_ON(LED0_GPIO_Pin);
        delay_ms(500);
        LED_OFF(LED0_GPIO_Pin);
        delay_ms(500);
        LED_ON(LED1_GPIO_Pin);
        delay_ms(500);
        LED_OFF(LED1_GPIO_Pin);
        delay_ms(500);
    }
}
```

### 2.2 GPIO输出-蜂鸣器

#### 2.2.1 蜂鸣器相关参数宏定义

```c
#ifndef __BEEP_H__
#define __BEEP_H__

#include "sys.h"

#define BEEP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define BEEP_GPIO_Port GPIOF
#define BEEP_GPIO_Pin GPIO_PIN_8

#define BEEP_ON() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_GPIO_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_GPIO_Pin, GPIO_PIN_RESET)
#define BEEP_TOGGLE() HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_GPIO_Pin)

void bsp_beep_init(void);

#endif /* __BEEP_H__ */
```

#### 2.2.2 蜂鸣器GPIO初始化

```c
#include "beep.h"

void bsp_beep_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BEEP_GPIO_CLK_ENABLE();
    GPIO_InitStructure.Pin = BEEP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStructure);
    BEEP_OFF();
}
```

#### 2.2.3 主函数测试

```c
#include "bsp_init.h"

int main(void)
{
    bsp_init();
    while(1)
    {
        LED_ON(LED0_GPIO_Pin);
        BEEP_ON();
        delay_ms(500);
        BEEP_OFF();
        LED_OFF(LED0_GPIO_Pin);
        delay_ms(500);
    }
}
```

### 2.3 GPIO输入-按键控制

![屏幕截图 2025-05-29 162500.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/29-16-25-50-屏幕截图%202025-05-29%20162500.png)

这里需要注意的是： KEY0、 KEY1 和 KEY2 设计为采样到按键另一端的低电平为有效电平，而 KEY_UP 则需要采样到高电平才为按键有效，并且按键外部没有上下拉电阻，所以需要在 STM32F407 内部设置上下拉。

#### 2.3.1 按键相关函数宏定义

```c
#ifndef __KEY_H__
#define __KEY_H__

#include "sys.h"
#include "delay.h"

/* 普通按键 */
#define KEY_GPIO_Port GPIOE
#define KEY_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define KEY0_GPIO_Pin GPIO_PIN_4 // KEY0 - PE4
#define KEY1_GPIO_Pin GPIO_PIN_3 // KEY1 - PE3
#define KEY2_GPIO_Pin GPIO_PIN_2 // KEY2 - PE2
/* UP按键 */
#define WKUP_GPIO_Port GPIOA
#define WKUP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define WKUP_GPIO_Pin GPIO_PIN_0

/* 读取按键状态 */
#define KEY0_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY0_GPIO_Pin)
#define KEY1_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY1_GPIO_Pin)
#define KEY2_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY2_GPIO_Pin)
#define WKUP_GET HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_GPIO_Pin)

#define KEY0_Press 1 // KEY0 按键按下
#define KEY1_Press 2 // KEY1 按键按下
#define KEY2_Press 3 // KEY2 按键按下
#define WKUP_Press 4 // WKUP 按键按下

void bsp_key_init(void);
uint8_t key_scan(uint8_t mode);

#endif /* __KEY_H__ */
```

#### 2.3.2 按键GPIO初始化

```c
void bsp_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    KEY_GPIO_CLK_ENABLE();
    WKUP_GPIO_CLK_ENABLE();
    /* KEY0-PE4 */
    GPIO_InitStructure.Pin = KEY0_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY1-PE3 */
    GPIO_InitStructure.Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY2-PE2 */
    GPIO_InitStructure.Pin = KEY2_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* WKUP-PA0 */
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);
}
```

#### 2.3.3 按键扫描函数

```c
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1; // 按键松开标志
    uint8_t key_val = 0;
    if(mode)
        key_up = 1; // 支持连按
    // 判断按键是否按下
    if(key_up && (KEY0_GET == 0 || KEY1_GET == 0 || KEY2_GET == 0 || WKUP_GET == 1))
    {
        delay_ms(10);
        key_up = 0; // 代表按键按下
        if(KEY0_GET == 0)
            return KEY0_Press;
        if(KEY1_GET == 0)
            return KEY1_Press;
        if(KEY2_GET == 0)
            return KEY2_Press;
        if(WKUP_GET == 1)
            return WKUP_Press;
    }
    else if(KEY0_GET == 1 && KEY1_GET == 1 && KEY2_GET == 1 && WKUP_GET == 0) // 释放按键
    {
        key_up = 1; // 代表按键释放
        return 0;
    }
    return 0;
}
```

#### 2.3.4 主函数测试

```c
#include "bsp_init.h"

int main(void)
{
    uint8_t key_value;
    bsp_init();
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case WKUP_Press:
                LED_ON(LED0_GPIO_Pin);
                break;
            case KEY0_Press:
                LED_OFF(LED0_GPIO_Pin);
                break;
            case KEY1_Press:
                LED_ON(LED1_GPIO_Pin);
                break;
            case KEY2_Press:
                LED_OFF(LED1_GPIO_Pin);
                break;
        }
    }
}
```

## 3. GPIO常见函数（HAL库）

### 3.1 初始化与配置

- **`HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)`**
  
  - **功能：** 这是配置 GPIO 引脚**最核心**的函数。它根据 `GPIO_InitTypeDef` 结构体中的参数，初始化指定 GPIO 端口 (GPIOx) 中的一个或多个引脚。
  
  - **参数：**
    
    - `GPIOx`: 指定 GPIO 端口 (A, B, C, ... K)。例如 `GPIOA`, `GPIOB`。
    
    - `GPIO_Init`: 指向 `GPIO_InitTypeDef` 结构体的指针，该结构体包含了具体的配置信息。
  
  - **`GPIO_InitTypeDef` 结构体成员详解 (配置的关键)：**
    
    - `Pin`: 指定要配置的引脚。可以是单个引脚 (如 `GPIO_PIN_0`, `GPIO_PIN_1`)，也可以是多个引脚的组合 (如 `GPIO_PIN_0 | GPIO_PIN_1`)。使用 `|` 运算符组合。
    
    - `Mode`: 设置引脚的工作模式。这是**最重要**的配置项！常用值：
      
      - `GPIO_MODE_INPUT`: 输入模式 (浮空输入 - 最常见)
      
      - `GPIO_MODE_OUTPUT_PP`: 推挽输出模式
      
      - `GPIO_MODE_OUTPUT_OD`: 开漏输出模式
      
      - `GPIO_MODE_AF_PP`: 复用功能推挽输出 (用于 UART, SPI, I2C, TIM 等外设)
      
      - `GPIO_MODE_AF_OD`: 复用功能开漏输出
      
      - `GPIO_MODE_ANALOG`: 模拟模式 (用于 ADC, DAC)
      
      - `GPIO_MODE_IT_RISING`: 外部中断模式，上升沿触发
      
      - `GPIO_MODE_IT_FALLING`: 外部中断模式，下降沿触发
      
      - `GPIO_MODE_IT_RISING_FALLING`: 外部中断模式，上升沿和下降沿都触发
      
      - `GPIO_MODE_EVT_RISING`: 外部事件模式，上升沿触发 (用于唤醒睡眠模式，不进中断)
      
      - `GPIO_MODE_EVT_FALLING`: 外部事件模式，下降沿触发
      
      - `GPIO_MODE_EVT_RISING_FALLING`: 外部事件模式，上升沿和下降沿都触发
    
    - `Pull`: 设置上拉或下拉电阻。
      
      - `GPIO_NOPULL`: 无上拉无下拉 (浮空)
      
      - `GPIO_PULLUP`: 内部上拉电阻使能
      
      - `GPIO_PULLDOWN`: 内部下拉电阻使能
    
    - `Speed`: 设置引脚的输出速度 (仅对输出模式有效)。影响翻转速度和功耗/EMI。
      
      - `GPIO_SPEED_FREQ_LOW`: 低速 (约 2MHz)
      
      - `GPIO_SPEED_FREQ_MEDIUM`: 中速 (约 10-25MHz)
      
      - `GPIO_SPEED_FREQ_HIGH`: 高速 (约 50-100MHz)
      
      - `GPIO_SPEED_FREQ_VERY_HIGH`: 超高速 (F4系列最高速)
    
    - `Alternate`: **仅当 `Mode` 设置为 `GPIO_MODE_AF_PP` 或 `GPIO_MODE_AF_OD` 时才需要配置。** 指定该引脚使用的复用功能编号 (AF0 - AF15)。具体哪个外设对应哪个 AF 号，需要查阅 STM32F407 的芯片手册 (Datasheet) 或参考手册 (Reference Manual) 中的 "Alternate function mapping" 表格。例如，UART1_TX 在 PA9 上是 AF7。

- **`HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)`**
  
  - **功能：** 将指定 GPIO 端口的一个或多个引脚恢复到默认状态 (模拟输入模式)，通常用于释放引脚资源或重置配置。在 CubeMX 生成的 `HAL_GPIO_DeInit` 函数中，它还会禁用该 GPIO 端口的时钟 (如果该端口所有引脚都被反初始化了)。
  
  - **参数：**
    
    - `GPIOx`: 指定 GPIO 端口。
    
    - `GPIO_Pin`: 指定要反初始化的引脚 (或引脚组合)。

### 3.2 输出操作

- **`void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)`**
  
  - **功能：** 设置指定 GPIO 引脚 (或引脚组合) 的输出电平。
  
  - **参数：**
    
    - `GPIOx`: 指定 GPIO 端口。
    
    - `GPIO_Pin`: 指定要写入的引脚 (或引脚组合)。
    
    - `PinState`: 要设置的电平状态。
      
      - `GPIO_PIN_RESET`: 低电平 (0)
      
      - `GPIO_PIN_SET`: 高电平 (1)
  
  - **示例：** `HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);` // 将 PD12 置高
  
  - **注意：** 如果 `GPIO_Pin` 指定了多个引脚，它们会被设置为**相同**的电平 (`PinState`)。

- **`void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`**
  
  - **功能：** 翻转指定 GPIO 引脚 (或引脚组合) 的当前输出电平 (高变低，低变高)。非常方便用于控制 LED 闪烁或生成简单方波。
  
  - **参数：**
    
    - `GPIOx`: 指定 GPIO 端口。
    
    - `GPIO_Pin`: 指定要翻转的引脚 (或引脚组合)。
  
  - **示例：** `HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);` // 翻转 PD14 的电平

### 3.3 输入操作

- **`GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`**
  
  - **功能：** 读取指定 GPIO 引脚的当前输入电平。
  
  - **参数：**
    
    - `GPIOx`: 指定 GPIO 端口。
    
    - `GPIO_Pin`: 指定要读取的引脚 (注意：通常一次只读取一个引脚的值有意义，虽然参数允许组合，但返回的是组合中**最低位**引脚的状态，这通常不是你想要的。建议一次只读一个引脚)。
  
  - **返回值：** 返回该引脚的电平状态 (`GPIO_PIN_RESET` 或 `GPIO_PIN_SET`)。
  
  - **示例：**

```c
GPIO_PinState buttonState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
if (buttonState == GPIO_PIN_RESET) {
    // PA0 被按下 (假设低电平有效)
} else {
    // PA0 被释放
}
```

### 3.4 中断处理（当配置为中断模式时）

- **`void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)`**
  
  - **功能：** 这是外部中断 (EXTI) 的**通用中断服务子程序 (ISR)**。它需要被放在对应 EXTI 线的中断向量服务函数中。它的作用是检查哪个引脚触发了中断标志位，并清除该标志位，然后调用对应的回调函数 `HAL_GPIO_EXTI_Callback`。
  
  - **参数：**
    
    - `GPIO_Pin`: 中断服务函数对应的 EXTI 线所连接的引脚号。例如，PA0、PB0、PC0... 都连接到 EXTI 线 0，所以 EXTI0 的中断服务函数里调用 `HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);`。
  
  - **位置：** 通常在 CubeMX 自动生成的 `stm32f4xx_it.c` 文件中，对应的外部中断函数 (如 `EXTI0_IRQHandler`, `EXTI1_IRQHandler`, ..., `EXTI15_10_IRQHandler`) 里调用此函数。
  
  - **示例 (在 `stm32f4xx_it.c` 中):**

```c
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); // 处理连接到 EXTI0 的引脚 (如 PA0) 的中断
}
```

- **`__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`**
  
  - **功能：** 这是一个**弱定义 (weak)** 的回调函数。`HAL_GPIO_EXTI_IRQHandler` 在确认中断源并清除标志位后，会调用这个函数。**你需要在自己的代码中 (通常是 `main.c` 或专门的源文件中) 重新实现 (Override) 这个函数**，并在里面编写处理该中断事件的实际逻辑 (比如翻转 LED、发送信号量、设置标志位等)。这是你处理中断具体任务的地方。
  
  - **参数：**
    
    - `GPIO_Pin`: 触发中断的引脚号。这允许同一个回调函数处理来自不同引脚的中断 (通过检查 `GPIO_Pin` 参数)。
  
  - **非常重要：**
    
    - 在这个回调函数中执行的代码应该尽量**简短快速**，避免长时间阻塞。不要调用可能导致阻塞的 HAL 延时函数 (如 `HAL_Delay()`)。
    
    - 通常在此设置一个软件标志位 (`volatile` 全局变量)，然后在 `main` 循环中检查并处理耗时任务，是更好的实践。

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == USER_Btn_Pin) { // USER_Btn_Pin 是你定义的按键引脚宏
        // 按键按下触发了中断
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // 翻转 LED
    }
    // 可以添加其他引脚的中断处理...
}
```

### 3.5 锁定配置

- **`HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`**
  
  - **功能：** 锁定指定 GPIO 引脚的配置 (模式、速度、上下拉、复用功能)。一旦锁定，除非复位，否则无法通过软件修改这些配置。主要用于防止关键配置被意外更改。
  
  - **参数：**
    
    - `GPIOx`: 指定 GPIO 端口。
    
    - `GPIO_Pin`: 指定要锁定的引脚 (或引脚组合)。
  
  - **返回值：** `HAL_OK` (成功), `HAL_ERROR` (失败 - 通常是配置序列写错了)。
  
  - **使用流程 (必须严格遵循):**
    
    1. 正常调用 `HAL_GPIO_Init` 配置好引脚。
    
    2. 调用 `HAL_GPIO_LockPin`。
    
    3. `HAL_GPIO_LockPin` 内部会执行一个特定的写序列 (`LCKK` 位操作) 来锁定。
  
  - **注意：** 锁定后，`HAL_GPIO_Init` 对该引脚的配置将失效。只有输出电平 (`ODR`/`BSRR`) 和输入读取 (`IDR`) 仍然可以操作。
