# 第六章 GPIO输出-使用标准库控制LED

## 1. 硬件设计

    在开发板上面与LED的连接见图，这是一个RGB灯，里面由红蓝绿三个小灯构成， 使用PWM控制时可以混合成256不同的颜色。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIOou002.png)

## 2. 软件设计

### 2.1 编程大纲

1. 使能GPIO端口时钟；

2. 初始化GPIO目标引脚为推挽输出模式；

3. 编写简单测试程序，控制GPIO引脚输出高、低电平。

### 2.2 代码分析

#### 2.2.1 LED灯脚宏定义

```c
// LED引脚宏定义
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_PIN GPIO_Pin_5
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_PIN GPIO_Pin_0
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_PIN GPIO_Pin_1
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB
```

    以上代码分别把控制LED灯的GPIO端口、GPIO引脚号以及GPIO端口时钟封装起来了。在实际控制的时候我们就直接用这些宏，以达到应用代码硬件无关的效果。

    其中的GPIO时钟宏“RCC_APB2Periph_GPIOB”是STM32标准库定义的GPIO端口时钟相关的宏，它的作用与“GPIO_Pin_x”这类宏类似，是用于指示寄存器位的， 方便库函数使用，下面初始化GPIO时钟的时候可以看到它的用法。

#### 2.2.2 控制LED灯亮灭状态的宏定义

      为了方便控制LED灯，我们把LED灯常用的亮、灭及状态反转的控制也直接定义成宏：

```c
// 控制LED宏定义
#define LED1_ON() GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_OFF() GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED2_ON() GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_OFF() GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED3_ON() GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_OFF() GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)

// 基本混色
#define LED_RED()   {LED1_ON(); LED2_OFF(); LED3_OFF();}
#define LED_GREEN() {LED1_OFF(); LED2_ON(); LED3_OFF();}
#define LED_BLUE()  {LED1_OFF(); LED2_OFF(); LED3_ON();}
#define LED_YELLOW() {LED1_ON(); LED2_ON(); LED3_OFF();}
#define LED_PURPLE() {LED1_ON(); LED2_OFF(); LED3_ON();}
#define LED_CYAN()  {LED1_OFF(); LED2_ON(); LED3_ON();}
#define LED_WHITE() {LED1_ON(); LED2_ON(); LED3_ON();}
#define LED_RGBOFF() {LED1_OFF(); LED2_OFF(); LED3_OFF();}
```

### 2.2.3 LED GPIO初始化函数

    利用上面的宏，编写LED灯的初始化函数：

```c
#include "led.h"

// LED初始化函数
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);

    // 初始化LED1
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 设置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置GPIO速度
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

    // 初始化LED2
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

    // 初始化LED3
    GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

    // 初始化完成后，先关闭所有LED
    LED_RGBOFF();
}
```

函数执行流程如下：

1. 使用GPIO_InitTypeDef定义GPIO初始化结构体变量，以便下面用于存储GPIO配置。

2. 调用库函数RCC_APB2PeriphClockCmd来使能LED灯的GPIO端口时钟，在前面的章节中我们是直接向RCC寄存器赋值来使能时钟的， 不如这样直观。该函数有两个输入参数，第一个参数用于指示要配置的时钟，如本例中的“RCC_APB2Periph_GPIOB”， 应用时我们使用“|”操作同时配置3个LED灯的时钟；函数的第二个参数用于设置状态，可输入“Disable”关闭或“Enable”使能时钟。

3. 向GPIO初始化结构体赋值，把引脚初始化成推挽输出模式，其中的GPIO_Pin使用宏“LEDx_GPIO_PIN”来赋值，使函数的实现方便移植。

4. 使用以上初始化结构体的配置，调用GPIO_Init函数向寄存器写入参数，完成GPIO的初始化， 这里的GPIO端口使用“LEDx_GPIO_PORT”宏来赋值，也是为了程序移植方便。

5. 使用同样的初始化结构体，只修改控制的引脚和端口，初始化其它LED灯使用的GPIO引脚。

6. 使用宏控制RGB灯默认关闭。

#### 2.2.4 主函数

    编写完LED灯的控制函数后，就可以在main函数中测试了

```c
#include "stm32f10x.h"
#include "led.h"

// 延时函数
void Delay(uint32_t time)
{
    while (time--);
}

int main(void)
{
    // 初始化LED
    LED_Init();

    while (1)
    {
        LED_RED();      // 红色
        Delay(1000000); // 延时
        LED_GREEN();    // 绿色
        Delay(1000000); // 延时
        LED_BLUE();     // 蓝色
        Delay(1000000); // 延时
        LED_YELLOW();   // 黄色
        Delay(1000000); // 延时
        LED_PURPLE();   // 紫色
        Delay(1000000); // 延时
        LED_CYAN();     // 青色
        Delay(1000000); // 延时
        LED_WHITE();    // 白色
        Delay(1000000); // 延时
        LED_RGBOFF();   // 关闭所有LED
        Delay(1000000); // 延时
    }
}
```

## 3. 小结

    前期写功能实现的代码倒是不难，关键是如何配置和初始化才是难点，虽然每个配置我们都已经讲了，但是既然是小结，在此重新总结一下：

- 首先就是写LED的头文件了，在这个头文件里写什么呢：主要就是定义各种宏-方便后面调试

- 首先就是定义LED1~3的宏，例如：

```c
// LED1对应颜色为红色
#define LED1_GPIO_PORT        GPIOB                 // 用LED1_GPIO_PORT代替GPIOB，输出输入端口     
#define LED1_GPIO_CLK         RCC_APB2Periph_GPIOB // 用LED1_GPIO_CLK代替RCC_APB2Periph_GPIOB，端口时钟
#define LED1_GPIO_PIN         GPIO_Pin_5             // 用LED1_GPIO_PIN代表GPIO_Pin_5，接的哪个引脚    
```

- 接着比较重要的就是定义方法控制IO口实现输出高低电平：

```c
// 控制LED宏定义
#define LED1_ON() GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_OFF() GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED2_ON() GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_OFF() GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED3_ON() GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_OFF() GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)
```

- 后面就是多种LED混合了，可有可无

- 开始写LCD.c文件，这个主要就是定义一个结构体，把LED1~3都封装到里面，还配置了推挽输出模式和50MHz速度，就拿LED1举例吧：

```c
    // 初始化LED1
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 设置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置GPIO速度
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
```

- 主函数很简单，没什么可说的，至此工程完成

---

2024.7.22 第一次修订

2024.8.80 第二次修订，后期不再维护

2024.12.16 简化内容，优化代码
