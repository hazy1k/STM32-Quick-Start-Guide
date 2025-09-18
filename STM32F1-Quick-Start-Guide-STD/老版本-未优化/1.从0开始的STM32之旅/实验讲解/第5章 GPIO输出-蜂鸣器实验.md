# 第五章 蜂鸣器实验

## 1. 硬件设计

我们开发板上面使用的有源蜂鸣器，图中BEEP1：

![屏幕截图 2024 07 21 180628](https://img.picgo.net/2024/07/21/-2024-07-21-18062893891a234e02235d.png)

蜂鸣器分无源和有源两种，从电路上来看，这两种蜂鸣器在这都可以工作，区别是单片机程序不一样。

当节点Beep为高电平时，三极管Q4截止，蜂鸣器无电流，不响。

当节点Beep为低电平时，三极管Q4导通，蜂鸣器有电流，会响。

所以在Beep节点输入PWM脉冲时，蜂鸣器就会发出响声。

更多内容参考：[蜂鸣器原理与实现方法 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/513107262)

[有源和无源蜂鸣器两者有什么不一样？如何进行区分？ - 21ic电子网](https://www.21ic.com/a/957252.html)

## 2. 软件设计

### 2.1 蜂鸣器相关参数宏定义

```c
#ifndef __BEEP_H
#define __BEEP_H
#include "stm32f10x.h"
// 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的蜂鸣器引脚 */
#define BEEP_GPIO_PORT       GPIOA                 // 蜂鸣器使用的GPIO端口
#define BEEP_GPIO_CLK        RCC_APB2Periph_GPIOA // 开启GPIOA端口时钟
#define BEEP_GPIO_PIN        GPIO_Pin_8             // 连接到蜂鸣器的GPIO的引脚
// 高电平时，蜂鸣器响 */
#define ON  1
#define OFF 0
// 带参宏，可以像内联函数一样使用 */
#define BEEP(a)    if (a)    \
                    GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);\
                    else        \
                    GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void BEEP_GPIO_Config(void);
#endif 
```

上面使用了一个带参宏，可能C语言基础薄弱的看不懂，我们这里来复习一下：

其实就是宏定义了一个函数，以上面的BEEP(a)为例，当函数传入一个非零即真的时候执行if(a)下面的语句，当为假就执行else下面的语句，理解了带参宏，再来认识一下GPIO_SetBits()和GPIO_ResetBits()，简单来说就是控制GPIO引脚的电平状态，下面是具体的介绍：

1. `GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);`  
   这行代码将 `BEEP_GPIO_PIN` 引脚在 `BEEP_GPIO_PORT` 端口设置为高电平。常用于使 GPIO 引脚输出高电压信号（例如 3.3V 或 5V）。

2. `GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);`  
   这行代码将 `BEEP_GPIO_PIN` 引脚在 `BEEP_GPIO_PORT` 端口设置为低电平。常用于使 GPIO 引脚输出低电压信号（例如 0V）。

再对照我们的宏定义，是不是豁然开朗了呢:smile:

### 2.2 蜂鸣器初始化函数

```c
#include "bsp_beep.h"   

// 蜂鸣器初始化函数
void BEEP_GPIO_Config(void)
{        
        // 定义一个GPIO_InitTypeDef类型的结构体
        GPIO_InitTypeDef GPIO_InitStructure;

        // 开启控制蜂鸣器的GPIO的端口时钟
        RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE); 

        // 选择要控制蜂鸣器的GPIO                                                               
        GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;    

        // 设置GPIO模式为通用推挽输出
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

        // 设置GPIO速率为50MHz  
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

        // 调用库函数，初始化控制蜂鸣器的GPIO
        GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);             

        // 关闭蜂鸣器
        GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);     
}
```

### 2.3 主函数

```c
#include "stm32f10x.h"
#include "./beep/bsp_beep.h"

// 函数声明
void Delay(__IO u32 nCount); 

int main(void)
{    
    // 蜂鸣器初始化
    BEEP_GPIO_Config();    

    while(1)
    {        
        BEEP(ON); // 打开蜂鸣器     
        Delay(0x0FFFFF); // 延迟    

        BEEP( OFF );        
        Delay(0x0FFFFF);
    }
}

// 延时函数
void Delay(__IO uint32_t nCount) 
{
    for(; nCount != 0; nCount--);
}
```

## 3. 小结

这个实验还是比较简单，主要就是配置蜂鸣器初始化函数了，但是作为初学者我们还是得好好分析一下子，从beep头文件开始

- 首先就是要定义端口、时钟、引脚了

```c
#define BEEP_GPIO_PORT       GPIOA                // 蜂鸣器使用的GPIO端口
#define BEEP_GPIO_CLK        RCC_APB2Periph_GPIOA // 开启GPIOA端口时钟
#define BEEP_GPIO_PIN        GPIO_Pin_8           // 连接到蜂鸣器的GPIO的引脚
```

这个我们了解过工程模板和参考以前的知识就知道了，我们要利用单片机驱动某个对象，首先确立我们要使用的端口，确立了端口就打开对应的时钟呗，之后规定我们要连接的引脚

- 我们知道，有源蜂鸣器，高电平1响，0就关，为了代码可读性，我们使用宏定义

```c
#define ON  1
#define OFF 0
```

随后我们分析一下beep.c，其实这没什么可分析的，都是使用的库函数，我们直接填几个参数就行了

```c
void BEEP_GPIO_Config(void)
{        
        /*定义一个GPIO_InitTypeDef类型的结构体*/
        GPIO_InitTypeDef GPIO_InitStructure;

        /*开启控制蜂鸣器的GPIO的端口时钟*/
        RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE); 

        /*选择要控制蜂鸣器的GPIO*/                                                               
        GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;    

        /*设置GPIO模式为通用推挽输出*/
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

        /*设置GPIO速率为50MHz */   
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

        /*调用库函数，初始化控制蜂鸣器的GPIO*/
        GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);             

        // 关闭蜂鸣器
        GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);     
}
```

但是初次使用库函数，我们怎么能不分析，下面一一道来：

- 在嵌入式编程中，特别是在使用 STM32 等微控制器时，结构体常用于配置硬件外设。我们在 `BEEP_GPIO_Config` 函数中定义的 `GPIO_InitTypeDef` 结构体是用来配置 GPIO 引脚的属性的。也可以看到，后面的配置都需要这个结构体

这个结构体包含了用于配置 GPIO 引脚的各种设置，例如引脚号、模式、速率等：

```c
typedef struct
{
  uint16_t GPIO_Pin;       /*!< 指定要配置的 GPIO 引脚。
                               这个参数可以是 GPIO_Pin_x 的任意组合，其中 x 可以是 (0..15) */
  GPIOMode_TypeDef GPIO_Mode;    /*!< 指定所选引脚的操作模式。
                               这个参数可以是 @ref GPIOMode_TypeDef 的一个值 */
  GPIOOType_TypeDef GPIO_OType;  /*!< 指定所选引脚的输出类型。
                               这个参数可以是 @ref GPIO_OType_TypeDef 的一个值 */
  GPIOPuPd_TypeDef GPIO_PuPd;    /*!< 指定所选引脚的上拉或下拉激活状态。
                               这个参数可以是 @ref GPIOPuPd_TypeDef 的一个值 */
  GPIOSpeed_TypeDef GPIO_Speed;  /*!< 指定所选引脚的速度。
                               这个参数可以是 @ref GPIOSpeed_TypeDef 的一个值 */
} GPIO_InitTypeDef;
```

- `RCC_APB2PeriphClockCmd` 是一个 STM32 微控制器库函数，用于配置和控制 APB2 总线上的外设时钟。具体来说，它用来启用或禁用 APB2 总线上的外设时钟，这样可以控制外设的时钟源，以确保外设可以正常工作。

在 STM32 的标准外设库中，`RCC_APB2PeriphClockCmd` 的函数原型如下：

```c
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); //  函数参数：端口时钟，使能与否
```

- GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;：通过我们的结构体设置GPIO要控制的引脚

- GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;：通过我们的结构体设置GPIO的模式-这里我们设置为推挽输出模式

- GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;：通过我们的结构体设置GPIO的引脚速率-我们这里设置50Mhz

- `GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);` 是 STM32 微控制器库函数中的一个重要函数，用于初始化和配置指定的 GPIO（通用输入/输出）端口及其引脚。我们可以看一下函数原型

```c
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
```

重点是理解函数参数：

1. **`GPIOx`**:
   
   - 类型：`GPIO_TypeDef*`
   - 描述：指定要初始化的 GPIO 端口。`GPIOx` 是一个指向 GPIO 端口寄存器的指针，例如 `GPIOA`, `GPIOB`, `GPIOC` 等。这个参数决定了你要配置哪个端口的引脚。
   
   例如，如果你传入 `GPIOA`，那么函数会配置 GPIOA 端口的引脚。

2. **`GPIO_InitStruct`**:
   
   - 类型：`GPIO_InitTypeDef*`
   - 描述：指向一个 `GPIO_InitTypeDef` 结构体的指针，这个结构体包含了配置 GPIO 引脚所需的所有设置，例如引脚号、模式、输出类型、上拉/下拉电阻以及速度等。

你需要先设置好这个结构体，然后将其地址传递给 `GPIO_Init` 函数。

- GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);：最后这个我们已经学习过了，就是给GPIO低电平，我们这里使用的有源蜂鸣器，低电平关闭

---

以后的整体结构差不多都是这样

1. 定义一个结构体(通常为GPIO_InitTypeDef GPIO_InitStructure;)

2. 开启GPIO的时钟(参数：XXX_GPIO_PIN，ENABLE)

3. 选择要控制引脚（选择XXX_GPIO_PIN)

4. 选择输出模式（可选OD\PP)

5. 设置速率（可选5，10，50）MHz

6. 调用库函数，进行初始化（参数：端口，引用结构体）

7. 调用函数关闭对象（参数：端口，引脚）

那主函数更不必说了

```c
#include "stm32f10x.h"
#include "./beep/bsp_beep.h"

void Delay(__IO u32 nCount); 

int main(void)
{    
    // 初始化蜂鸣器GPIO
    BEEP_GPIO_Config();    

    while(1)
    {        
        BEEP( ON ); // 开启蜂鸣器    
        Delay(0x0FFFFF);    

        BEEP( OFF );        
        Delay(0x0FFFFF);
    }
}

// 延时函数
void Delay(__IO uint32_t nCount) 
{
    for(; nCount != 0; nCount--);
}
```

首先就是初始化beep了，之后想让蜂鸣器响，就是不断发送脉冲咯，就是不断的开->延时->关->延时

---

2024.7.21 第一次修订

2024.8.19 第二次修订，后期不再维护

2024.12.9 修补部分内容，优化结构
