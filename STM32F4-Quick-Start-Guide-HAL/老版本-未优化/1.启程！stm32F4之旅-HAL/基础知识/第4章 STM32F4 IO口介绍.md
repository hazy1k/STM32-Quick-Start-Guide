# 第四章 STM32F4 IO口介绍

## 1. 导入

STM32F4 每组通用 I/O 端口包括 4 个 32 位配置寄存器（MODER、 OTYPER、 OSPEEDR和 PUPDR）、 2 个 32 位数据寄存器（IDR 和 ODR）、 1 个 32 位置位/复位寄存器 (BSRR)、1 个 32 位锁定寄存器 (LCKR) 和 2 个 32 位复用功能选择寄存器（AFRH 和 AFRL）等。

这样， STM32F4 每组 IO 有 10 个 32 位寄存器控制，其中常用的有 4 个配置寄存器 + 2 个数据寄存器 + 2 个复用功能选择寄存器，共 8 个，如果在使用的时候，每次都直接操作寄存器配置IO， 代码会比较多，也不容易记住，所以我们在讲解寄存器的同时会讲解使用 HAL 库函数配置 IO 的方法。

同 STM32F1 一样， STM32F4 的 IO 可以由软件配置成如下 8 种模式中的任何一种：

- 输入浮空

- 输入上拉

- 输入下拉

- 模拟输出

- 开漏输出

- 推挽输出

- 推挽式复用功能

- 开漏式复用功能

关于这8种模式的更多介绍：[明解STM32—GPIO理论基础知识篇之八种工作模式 - 知乎](https://zhuanlan.zhihu.com/p/612432573)

## 2. 相关寄存器介绍

接下来我们详细介绍 IO 配置常用的 8 个寄存器： MODER、 OTYPER、 OSPEEDR、 PUPDR、 ODR、 IDR 、 AFRH 和 AFRL。同时讲解对应的库函数配置方法。

### 2.1 MODER寄存器

该寄存器是 GPIO 端口模式控制寄存器，用于控制 GPIOx （STM32F4 最多有 9 组 IO，分别用大写字母表示，即 x=A/B/C/D/E/F/G/H/I，下同）的工作模式。

![屏幕截图 2024 09 26 214534](https://img.picgo.net/2024/09/26/-2024-09-26-214534800bd8ba061de22c.png)

该寄存器各位在复位后，一般都是 0（个别不是 0，比如 JTAG 占用的几个 IO 口），也就是默认条件下一般是输入状态的。每组 IO 下有 16 个 IO 口，该寄存器共 32 位，每 2 个位控制 1个 IO，

假设我们要将GPIOA的第0引脚配置为输出模式，第1引脚配置为复用模式，可以按以下步骤操作：

```c
#include "stm32f4xx_hal.h"

void GPIO_Init(void) {
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // 配置MODER寄存器
    // 设置PA0为输出模式（01），PA1为复用模式（10）
    GPIOA->MODER &= ~(0x00000003); // 清零PA0
    GPIOA->MODER |= (0x00000001);  // PA0设置为输出模式
    GPIOA->MODER &= ~(0x0000000C); // 清零PA1
    GPIOA->MODER |= (0x00000008);  // PA1设置为复用模式
}
```

### 2.2 OSPEEDR寄存器

该寄存器用于控制 GPIOx 的输出速度，该寄存器各位描述见表

![屏幕截图 2024 09 26 215229](https://img.picgo.net/2024/09/26/-2024-09-26-215229abf609a635c8437b.png)

该寄存器也仅用于输出模式，寄存器每 2 个位控制一个 IO 口，复位后，该寄存器值一般为 0。

假设我们要将GPIOA的第0引脚配置为高速，第1引脚配置为超高速，可以按以下步骤操作：

```c
#include "stm32f4xx_hal.h"

void GPIO_Init(void) {
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置OSPEEDR寄存器
    // 设置PA0为高速（10），PA1为超高速（11）
    GPIOA->OSPEEDR &= ~(0x00000003); // 清零PA0
    GPIOA->OSPEEDR |= (0x00000002);  // PA0设置为高速

    GPIOA->OSPEEDR &= ~(0x0000000C); // 清零PA1
    GPIOA->OSPEEDR |= (0x0000000C);  // PA1设置为超高速
}
```

### 2.3 PUPDR寄存器

该寄存器用于控制 GPIOx 的上拉/下拉，该寄存器各位描述见表：

![屏幕截图 2024 09 26 215534](https://img.picgo.net/2024/09/26/-2024-09-26-2155343e4534332590a972.png)

假设要将GPIOA的第0引脚配置为上拉，第1引脚配置为下拉，可以按以下步骤操作：

```c
#include "stm32f4xx_hal.h"

void GPIO_Init(void) {
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // 配置PUPDR寄存器
    // 设置PA0为上拉（01），PA1为下拉（10）
    GPIOA->PUPDR &= ~(0x00000003); // 清零PA0
    GPIOA->PUPDR |= (0x00000001);  // PA0设置为上拉
    GPIOA->PUPDR &= ~(0x0000000C); // 清零PA1
    GPIOA->PUPDR |= (0x00000002);  // PA1设置为下拉
}
```

该寄存器每 2 个位控制一个 IO 口，用于设置上下拉，这里提醒大家， STM32F1 是通过 ODR寄存器控制上下拉的，而 STM32F4则由单独的寄存器 PUPDR控制上下拉，使用起来更加灵活。复位后，该寄存器值一般为 0。

### 2.4 OTYPER寄存器

该寄存器用于控制 GPIOx 的输出类型，该寄存器各位描述见表：

![屏幕截图 2024 09 26 215929](https://img.picgo.net/2024/09/26/-2024-09-26-215929e29e69bd57fa0384.png)

假设要将GPIOA的第0引脚配置为推挽输出，第1引脚配置为开漏输出，可以按以下步骤操作：

```c
#include "stm32f4xx_hal.h"

void GPIO_Init(void) {
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置OTYPER寄存器
    GPIOA->OTYPER &= ~(0x00000001); // 清零PA0，配置为推挽输出
    GPIOA->OTYPER |= (0x00000002);  // 设置PA1为开漏输出
}
```

### 2.5 GPIO初始化函数

在 HAL 库开发中， 操作上面介绍四个配置寄存器初始化 GPIO 是通过 GPIO 初始化函数完成：

```c
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
```

这个函数有两个参数，第一个参数是用来指定需要初始化的 GPIO 对应的 GPIO 组，取值范围为 GPIOA~GPIOK。第二个参数为初始化参数结构体指针，结构体类型为 GPIO_InitTypeDef。下面我们看看这个结构体的定义。

```c
typedef struct
{
    uint32_t Pin; // 引脚
    uint32_t Mode;// 模式
    uint32_t Pull;// 上/下拉
    uint32_t Speed;// 速度
    uint32_t Alternate;
}GPIO_InitTypeDef;
```

下面来举一个例子：

```c
GPIO_Initure.Pin = GPIO_PIN_9|GPIO_PIN_10;// PF9,10
GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
GPIO_Initure.Pull = GPIO_PULLUP;          // 上拉
GPIO_Initure.Speed = GPIO_SPEED_HIGH;     // 高速
HAL_GPIO_Init(GPIOF,&GPIO_Initure); // 初始化GPIO
```

上面代码的意思是设置 GPIOF 的第 9 和 10 端口为推挽输出模式，同时速度为 100M，上拉。

看完了 GPIO 的参数配置寄存器，接下来我们看看 GPIO 输入输出电平控制相关的寄存器。

### 2.6 ODR寄存器

该寄存器用于控制 GPIOx 的输出，该寄存器各位描述见表：

![屏幕截图 2024 09 26 220404](https://img.picgo.net/2024/09/26/-2024-09-26-2204041754f0b5f5521398.png)

该寄存器用于设置某个 IO 输出低电平(ODRy=0)还是高电平(ODRy=1)，该寄存器也仅在输出模式下有效。

假设要将GPIOA的第0引脚设置为高电平，第1引脚设置为低电平，可以按以下步骤操作：

```c
#include "stm32f4xx_hal.h"

void GPIO_Init(void) {
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置GPIOA引脚模式等（省略具体配置代码）
}

void Set_GPIO_State(void) {
    // 设置PA0为高电平
    GPIOA->ODR |= (1 << 0);  // PA0 = 1
    // 设置PA1为低电平
    GPIOA->ODR &= ~(1 << 1); // PA1 = 0
}
```

在HAL 库中设置 ODR 寄存器的值来控制 IO 口的输出状态是通过函数 HAL_GPIO_WritePin 来实现的：

```c
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
```

- **GPIOx**: 指向要操作的 GPIO 端口的指针，例如 `GPIOA`, `GPIOB`, 等等。
- **GPIO_Pin**: 要设置的 GPIO 引脚，可以是单个引脚或多个引脚的组合，使用按位或（`|`）操作符来结合多个引脚。例如，可以使用 `GPIO_PIN_0 | GPIO_PIN_1` 来同时设置引脚 0 和引脚 1。
- **PinState**: 指定引脚的状态，类型为 `GPIO_PinState`，通常有两个取值：
  - `GPIO_PIN_SET`: 将引脚设置为高电平。
  - `GPIO_PIN_RESET`: 将引脚设置为低电平。

### 2.7 IDR寄存器

该寄存器用于读取 GPIOx 的输入电平状态，该寄存器各位描述见表：

![屏幕截图 2024 09 26 220719](https://img.picgo.net/2024/09/26/-2024-09-26-2207190bbb788e2be4753d.png)

该寄存器用于读取某个 IO 的电平，如果对应的位为 0(IDRy=0)，则说明该 IO 输入的是低电平，如果是 1(IDRy=1)，则表示输入的是高电平。 库函数相关函数为：

```c
GPIO_PinState = HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
```

函数是用来读取一组 IO 口的一个输入电平。 比如我们要读取 GPIOF.5 的输入电平，方法为

```c
HAL_GPIO_ReadPin (GPIOF, GPIO_Pin_5);
```

假设要读取 GPIOA 的第0引脚和第1引脚的输入状态，可以按以下步骤操作：

```c
#include "stm32f4xx_hal.h"

// 假设已经初始化了系统时钟和 GPIO

void GPIO_Init(void) {
    // 使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 配置 PA0 和 PA1 为输入模式
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; // 选择引脚 PA0 和 PA1
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // 输入模式
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉或下拉电阻
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // 初始化 GPIO
}

void Read_GPIO_State(void) {
    // 读取 PA0 和 PA1 的输入状态
    GPIO_PinState state_PA0 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    GPIO_PinState state_PA1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);

    if (state_PA0 == GPIO_PIN_SET) {
        // PA0 为高电平
    } else {
        // PA0 为低电平
    }

    if (state_PA1 == GPIO_PIN_SET) {
        // PA1 为高电平
    } else {
        // PA1 为低电平
    }
}

int main(void) {
    HAL_Init(); // 初始化 HAL 库
    GPIO_Init(); // 初始化 GPIO

    while (1) {
        Read_GPIO_State(); // 读取 GPIO 输入状态
        HAL_Delay(100); // 延迟
    }
}
```

### 2.8 BSRR寄存器

这个寄存器是用来置位或者复位 IO 口， 该寄存器和 ODR 寄存器具有类似的作用，都可以用来设置 GPIO 端口的输出位是1 还是 0。寄存器描述如下：

![屏幕截图 2024 09 26 221034](https://img.picgo.net/2024/09/26/-2024-09-26-221034e474c09d5f734cc4.png)

对于低 16 位（0-15），我们往相应的位写 1，那么对应的 IO 口会输出高电平，往相应的位写 0，对 IO 口没有任何影响。高 16 位（16-31）作用刚好相反，对相应的位写 1 会输出低电平，写 0没有任何影响。

- **设置引脚**：将指定引脚设置为高电平
- **复位引脚**：将指定引脚设置为低电平

![屏幕截图 2024 09 26 221350](https://img.picgo.net/2024/09/26/-2024-09-26-22135069d0b5c58234958a.png)

假设要将 GPIOA 的引脚 0 设置为高电平，并将引脚 1 复位为低电平，可以按以下方式操作：

```c
#include "stm32f4xx_hal.h"
// 假设已经初始化了系统时钟和 GPIO
void GPIO_Init(void) {
    // 使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 配置 PA0 和 PA1 为输出模式
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; // 选择引脚 PA0 和 PA1
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉或下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 设置速度
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // 初始化 GPIO
}
void Control_GPIO(void) {
    // 设置 PA0 为高电平
    GPIOA->BSRR = GPIO_PIN_0; // 设置引脚 PA0
    // 将 PA1 复位为低电平
    GPIOA->BSRR = (GPIO_PIN_1 << 16); // 重置引脚 PA1
}
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    GPIO_Init(); // 初始化 GPIO
    while (1) {
        Control_GPIO(); // 控制 GPIO 输出状态
        HAL_Delay(1000); // 每秒更新一次
    }
}
```

### 2.9 AFRL与AFRH寄存器

32 位寄存器 GPIOx_AFRL GPIOx_AFRH每四个位控制一个 IO 口，所以每个寄存器控制32/4=8 个 IO 口。寄存器对应四位的值配置决定这个 IO 映射到哪个复用功能 AF。

这两个寄存器允许你将特定的 GPIO 引脚配置为不同的功能，比如 UART、SPI、I2C 等。它通过设置不同的比特位来选择引脚的替代功能。

![屏幕截图 2024 09 26 221629](https://img.picgo.net/2024/09/26/-2024-09-26-2216299000dc2be4210555.png)

<img src="https://img.picgo.net/2024/09/26/-2024-09-26-222118107f9adadd7ac41b.png" title="" alt="屏幕截图 2024 09 26 222118" width="259">

假设要将 GPIOA 的引脚 9 和引脚 10 配置为其替代功能（例如，USART），可以按以下方式操作：

```c
#include "stm32f4xx_hal.h"
// 假设已经初始化了系统时钟和 GPIO
void GPIO_Init(void) {
    // 使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // 配置 PA9 和 PA10 为替代功能
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 推挽替代功能
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉或下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 设置速度
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1; // 选择 USART1 的替代功能
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // 初始化 GPIO
}
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    GPIO_Init(); // 初始化 GPIO

    while (1) {
        // 主循环
    }
}
```

## 3. HAL库配置IO

### 3.1 输出模式（Push-Pull 或 Open-Drain）

**推挽输出**：通常用于控制 LED、继电器等输出设备。这是最常用的输出模式。

```c
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出模式
GPIO_InitStruct.Pull = GPIO_NOPULL;          // 不使用上拉/下拉电阻
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**开漏输出**：用于 I2C 等需要开漏通信的场合。

```c
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出模式
GPIO_InitStruct.Pull = GPIO_PULLUP;          // 使用上拉电阻
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

### 3.2 输入模式（Analog, Digital, or Interrupt）

**普通输入模式**：用于读取外部信号。

```c
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      // 输入模式
GPIO_InitStruct.Pull = GPIO_PULLDOWN;        // 使用下拉电阻
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**中断输入模式**：用于 GPIO 引脚的中断触发。

```c
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  // 上升沿中断
GPIO_InitStruct.Pull = GPIO_NOPULL;          // 不使用上拉/下拉电阻
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

### 3.3 模拟模式

模拟模式一般用于 ADC 和 DAC 的引脚。此模式下，GPIO 会进入低功耗模式，不会进行数字化操作。

```c
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;     // 模拟模式
GPIO_InitStruct.Pull = GPIO_NOPULL;          // 不使用上拉/下拉电阻
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

### 3.4 复用功能模式

STM32F4 的 GPIO 支持复用功能，用于外设接口如 UART、SPI、I2C 等。通过设置 `GPIO_MODE_AF_PP` 或 `GPIO_MODE_AF_OD`，并指定具体的复用功能编号，GPIO 可以用作这些外设的信号接口。

例如，配置一个 UART 的 TX 引脚：

```c
GPIO_InitStruct.Pin = GPIO_PIN_9;               // 选择 TX 引脚
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;         // 复用推挽输出
GPIO_InitStruct.Pull = GPIO_PULLUP;             // 上拉电阻
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;   // 高速
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);         // 初始化 GPIOA
```

## 4. 小结

本章我们就主要介绍了STM32F4 GPIO相关的寄存器，但是我们在开发的时候往往不直接配置寄存器，比如我们可以使用HAL库

---

2024.9.26 第一次修订，后期不再维护

2024.12.20 更新HAL库配置
