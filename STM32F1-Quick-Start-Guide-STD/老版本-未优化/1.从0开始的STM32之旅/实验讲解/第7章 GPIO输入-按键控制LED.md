# 第七章 GPIO输入-按键控制LED

## 1. 硬件设计

    本章我们用到的硬件是-机械按键开关。在51单片机里我们已经学过了。此出再强调一下消抖。

    按键机械触点断开、闭合时，由于触点的弹性作用，按键开关不会马上稳定接通或一下子断开， 使用按键时会产生带波纹信号，需要用软件消抖处理滤波，不方便输入检测。本实验板连接的按键带硬件消抖功能， 见图 ，它利用电容充放电的延时，消除了波纹，从而简化软件的处理，软件只需要直接检测引脚的电平即可。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIOin002.jpg)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIOin003.png)

    从按键的原理可知，在按键没有被按下的时候，GPIO引脚的输入状态为低电平（按键所在的电路不通，引脚接地）当按键按下时，GPIO引脚的输入状态为高电平（按键电路导通，引脚接到电源）所为我们只需要检测引脚的输入电平，就可以判断按键是否按下。（按下1，没有按下0）

    关于软件消抖与硬件消抖：[硬件消抖和软件消抖的比较 - 面包板社区](https://mbb.eet-china.com/tech/t5/24494.html)

## 2. 软件设计

### 2.1 编程大纲

1. 使能GPIO端口时钟；

2. 初始化GPIO目标引脚为输入模式(浮空输入)；

3. 编写简单测试程序，检测按键的状态，实现按键控制LED灯。

### 2.2 代码分析

#### 2.2.1 Key宏定义

    在编写按键驱动时，我们同样需要功能模块化。我们把按键检测引脚相关的宏定义到“key.h”文件中。

```c
#define KEY_OFF 0
#define KEY_ON 1

// key port and pin define
#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA
#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_PIN GPIO_Pin_0
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOC
#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_PIN GPIO_Pin_13
```

#### 2.2.2 Key初始化和扫描

    下面利用我们刚刚定义的宏，编写按键的初始化函数与扫描函数：

```c
#include "key.h"

void KEY_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}

uint8_t KEY_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)  // 读取按键电平
    {
        while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON);  // 等待按键松开
        return KEY_ON;
    }
    else
    {
        return KEY_OFF;
    }
}

```

    在这里我们定义了一个Key_Scan函数用于扫描按键状态。GPIO引脚的输入电平可通过读取IDR寄存器对应的数据位来感知， 而STM32标准库提供了库函数GPIO_ReadInputDataBit来获取位状态，该函数输入GPIO端口及引脚号，函数返回该引脚的电平状态，高电平返回1， 低电平返回0。

    Key_Scan函数中以GPIO_ReadInputDataBit的返回值与自定义的宏“KEY_ON”对比，若检测到按键按下，则使用while循环持续检测按键状态， 直到按键释放，按键释放后Key_Scan函数返回一个“KEY_ON”值；若没有检测到按键按下，则函数直接返回“KEY_OFF”。 

    若按键的硬件没有做消抖处理，需要在这个Key_Scan函数中做软件滤波，防止波纹抖动引起误触发。（我们的硬件已经做了处理所以不需要，软件处理就是添加一个延时函数，可以参考51的按键实验）

#### 2.2.3 主函数

```c
#include "led.h"
#include "key.h"
int main(void)
{
    // 外设初始化
    LED_Init();
    KEY_GPIO_Init();
    while(1)
    {
        if(KEY_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            LED1_TOGGLE();
        }
        if(KEY_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            LED2_TOGGLE();
        }
    }
}

```

    led初始化和配置是上一章的内容，在此不再赘述，主函数就是简单地测试一下我们的按键功能。

## 3. 小结

    写入程序后，初始LED关，按下key1，LED亮（红）再按下关闭，key2--LED（绿），有趣的是你可以把两个灯同时打开就能得到混合颜色，至于为什么显示这种颜色，上一章已经说过或者参考led函数的头文件。

    代码不难而且注释也写得很详细了，主要就是一个配置问题和一个扫描函数，led可以照搬

    扫描按键函数中我们使用了一个新的库函数-GPIO_ReadInputDataBit，下面我们详细解释一下（以后一旦我们遇到新函数，都尽量解释一下吧，慢慢见多识广了）：

在 STM32 标准外设库中，`GPIO_ReadInputDataBit` 函数的原型通常如下：

```c
BitAction GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, GPIO_PinState GPIO_Pin);
```

参数说明：

1. **`GPIOx`**:
   
   - 类型：`GPIO_TypeDef*`
   - 描述：指定要读取的 GPIO 端口。`GPIOx` 是一个指向 GPIO 端口寄存器的指针，如 `GPIOA`, `GPIOB`, `GPIOC` 等。
   
   例如，如果你传入 `GPIOA`，函数将读取 GPIOA 端口的引脚状态。

2. **`GPIO_Pin`**:
   
   - 类型：`GPIO_PinState`
   - 描述：指定要读取的 GPIO 引脚。这个参数是一个掩码值，用于选择特定的引脚。例如，`GPIO_Pin_0` 表示第 0 号引脚，`GPIO_Pin_1` 表示第 1 号引脚，依此类推。

返回值：

- **`Bit_SET`**：表示指定的 GPIO 引脚为高电平（逻辑 1）。
- **`Bit_RESET`**：表示指定的 GPIO 引脚为低电平（逻辑 0）。

函数作用：

`GPIO_ReadInputDataBit` 函数的主要作用是读取一个 GPIO 引脚的电平状态，这在需要根据引脚的电平状态来做决策时非常有用。例如，读取一个按钮的状态或传感器的输出。

假设你要读取 GPIOA 端口的第 5 号引脚的电平状态，你可以使用 `GPIO_ReadInputDataBit` 函数来完成。下面是一个简单的示例代码：

```c
#include "stm32f10x.h"

// 假设你已经配置好了 GPIOA 的第 5 号引脚为输入模式

int main(void)
{
    // 初始化 GPIOA，第 5 号引脚为输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    while (1)
    {
        // 读取 GPIOA 的第 5 号引脚的电平状态
        BitAction pinState = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);

        if (pinState == Bit_SET)
        {
            // 引脚为高电平，执行相关操作
        }
        else
        {
            // 引脚为低电平，执行相关操作
        }

        // 延迟或其他操作
    }
}
```

在这个示例中：

1. **GPIOA** 的第 5 号引脚被配置为浮空输入模式。
2. `GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)` 用于读取该引脚的电平状态。
3. 根据返回的电平状态（`Bit_SET` 或 `Bit_RESET`），程序可以执行不同的操作。

---

2024.7.22 第一次修订

2024.8.20 第二次修订，后期不再维护

2024.12.17 简化内容，优化代码
