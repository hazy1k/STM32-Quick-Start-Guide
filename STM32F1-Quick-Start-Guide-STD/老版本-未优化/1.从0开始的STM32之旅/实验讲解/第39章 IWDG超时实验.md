# 第三十九章 IWDG超时实验

## 1. 硬件设计

1. IWDG一个

2. 按键一个

3. LED三个

IWDG属于单片机内部资源，不需要外部电路，需要一个外部的按键和LED，在 1s 的时间内通过按键来不断的喂狗，如果喂狗失败，红灯闪烁。如果一直喂狗成功，则绿灯常亮。

## 2. 软件设计

### 2.1 编程大纲

1. iwdg初始化函数

2. 喂狗函数

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 IWDG配置函数

```c
// 前置知识，iwdg超时时间计算
/*
    T = prv/40*rlv (s)
    prv为预分频器值，prv取值可以是[4,8,16,32,64,128,256]
    rlv为重载寄存器的值，取值范围为：0~0xFFF
    举例：T = 64/40*625 = 1s
*/
void IWDG_Config(uint8_t prv, uint16_t rlv)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 使能寄存器可写
    IWDG_SetPrescaler(prv); // 设置预分频器
    IWDG_SetReload(rlv); // 设置重载寄存器
    IWDG_Enable(); // 使能IWDG
}
```

其中一些库函数，我们需要介绍一下：

1. **IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable)**:
   
   - **功能**: 使能写访问权限，以允许对预分频寄存器（PR）和重装载寄存器（RLR）进行配置。某些微控制器在未开启写访问时，无法修改这些寄存器的值。
   - **参数**: `IWDG_WriteAccess_Enable`，使能写权限。

2. **IWDG_SetPrescaler(uint32_t prescaler)**:
   
   - **功能**: 设置IWDG的预分频器值。预分频器决定了看门狗计时器的计数速率。
   - **参数**: `prescaler`，通常是一个枚举值或常量，表示预分频器的设置，决定IWDG计数器的更新频率。

3. **IWDG_SetReload(uint32_t reload)**:
   
   - **功能**: 设置重装载寄存器的值。重装载寄存器决定了看门狗计时器的超时值，即如果在这个时间段内未重装载计数器，IWDG将触发复位。
   - **参数**: `reload`，一个整数值，表示计数器达到该值后会超时。

4. **IWDG_ReloadCounter()**:
   
   - **功能**: 将重装载寄存器的值写入计数器中，重置计数器。这是防止IWDG触发复位的关键步骤，通常在主程序循环中定期调用。
   - **无参数**: 该函数没有输入参数，直接执行重装载操作。

5. **IWDG_Enable()**:
   
   - **功能**: 启动IWDG，使其开始计数。如果未在设定的超时之前重装载计数器，IWDG将复位微控制器。
   - **无参数**: 该函数没有输入参数，直接使能看门狗。

---

IWDG配置函数有两个形参，prv用来设置预分频的值，取值可以是：

```c
/*
*     @arg IWDG_Prescaler_4:    IWDG prescaler set to 4
*     @arg IWDG_Prescaler_8:    IWDG prescaler set to 8
*     @arg IWDG_Prescaler_16:   IWDG prescaler set to 16
*     @arg IWDG_Prescaler_32:   IWDG prescaler set to 32
*     @arg IWDG_Prescaler_64:   IWDG prescaler set to 64
*     @arg IWDG_Prescaler_128:  IWDG prescaler set to 128
*     @arg IWDG_Prescaler_256:  IWDG prescaler set to 256
*/
```

这些宏在stm32f10x_iwdg.h中定义，宏展开是8位的16进制数，具体作用是配置配置预分频寄存器IWDG_PR，获得各种分频系数。 形参rlv用来设置重装载寄存器IWDG_RLR的值，取值范围为0~0XFFF。溢出时间Tout = prv/40 * rlv(s)， prv可以是[4,8,16,32,64,128,256]。如果我们需要设置1s的超时溢出，prv可以取IWDG_Prescaler_64，rlv取625， 即调用:IWDG_Config(IWDG_Prescaler_64 ,625)。Tout=64/40*625=1s。

#### 2.2.2 喂狗函数

```c
void IWDG_Feed(void)
{
    IWDG_ReloadCounter(); // 重载计数器
}
```

#### 2.2.3 主函数测试

```c
#include "key.h"
#include "led.h"
#include "iwdg.h"

int main(void)
{
    LED_Init();
    Key_Init();
    IWDG_Config(IWDG_Prescaler_64 , 625); // IWDG 1s timeout
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) // Check if the reset was caused by IWDG reset
    {
        LED_RED();
        RCC_ClearFlag(); 
    }
    else
    {
        LED_BLUE();
    }
    while(1)
    {
        if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PING) == KEY_ON)
        {
            IWDG_Feed();
            LED_GREEN();
        }
    }
}
```

主函数中我们初始化好LED和按键相关的配置，设置IWDG 1s超时溢出之后，进入while死循环，通过按键来喂狗，如果喂狗成功， 则亮绿灯，如果喂狗失败的话，系统重启，程序重新执行，当执行到RCC_GetFlagStatus函数的时候，则会检测到是IWDG复位， 然后让红灯亮。如果喂狗一直失败的话，则会一直产生系统复位，加上前面延时的效果，则会看到红灯一直闪烁。

我们这里是通过按键来模拟一个喂狗程序，真正的项目中则不是这样使用。while部分是我们在项目中具体需要写的代码， 这部分的程序可以用独立看门狗来监控，如果我们知道这部分代码的执行时间，比如是500ms，那么我们可以设置独立看门狗的溢出时间是510ms， 比500ms多一点，如果要被监控的程序没有跑飞正常执行的话，那么执行完毕之后就会执行喂狗的程序，如果程序跑飞了那程序就会超时， 到达不了喂狗的程序，此时就会产生系统复位，但是也不排除程序跑飞了又跑回来了，刚好喂狗了，歪打正着。所以要想更精确的监控程序， 可以使用窗口看门狗，窗口看门狗规定必须在规定的窗口时间内喂狗，早了不行，晚了也不行。

## 3. 小结

看门狗可以理解为一个监控器，我们监控一个地方，如果程序正常不做反应，如出现错误则看门狗复位，我们可以简单回顾一下：

### 3.1 实验目的

1. 理解独立看门狗的工作原理。
2. 实现一个基本的看门狗定时器程序，确保程序在正常运行时不复位，异常情况下能够复位微控制器。

### 3.2 实验环境

- 开发板：STM32F103
- 开发工具：Keil uVision 或 STM32CubeIDE
- 硬件连接：无特别连接，只需开发板和电脑。

### 3.3 实验步骤

1. **初始化IWDG**: 配置IWDG的预分频器和重装载值。
2. **主程序逻辑**: 在主循环中，定期重装载IWDG。
3. **故障模拟**: 人为制造故障（如进入死循环），观察微控制器复位。

### 3.4 代码示例

```c
#include "stm32f10x.h" // 包含STM32F10x库头文件

void IWDG_Config(void) {
    // 使能写访问
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // 设置预分频器为64
    IWDG_SetPrescaler(IWDG_Prescaler_64);

    // 设置重装载值
    IWDG_SetReload(0xFFF); // 设置为最大值，约12秒超时（根据时钟频率）

    // 重装载计数器
    IWDG_ReloadCounter();

    // 启动IWDG
    IWDG_Enable();
}

int main(void) {
    // 初始化IWDG
    IWDG_Config();

    // 主循环
    while (1) {
        // 模拟正常工作
        // 在这里加入你的主程序逻辑

        // 定期重装载看门狗
        IWDG_ReloadCounter();

        // 为了测试故障，模拟一次故障
        // 可以注释掉下面这一行以进行正常运行测试
        // if (/* some condition to simulate fault */) {
        //     while (1); // 进入死循环，观察IWDG复位
        // }
    }
}
```

### 3.5 说明

1. **IWDG_Config() 函数**:
   
   - 启用写访问，允许配置看门狗。
   - 设置预分频器和重装载值。这里设置为最大值，以实现较长的超时。
   - 调用`IWDG_ReloadCounter()`以初始化计数器。
   - 启动看门狗定时器。

2. **主循环**:
   
   - 在正常情况下，定期调用`IWDG_ReloadCounter()`以防止复位。
   - 你可以通过条件控制模拟一个故障，例如进入一个死循环。

### 3.6 故障测试

- 运行程序，观察在正常情况下微控制器不会复位。
- 注释掉`IWDG_ReloadCounter()`的调用或强制进入死循环，观察微控制器是否复位。

---

2024.9.19 第一次修订，后期不再维护

2025.1.29 修订代码
