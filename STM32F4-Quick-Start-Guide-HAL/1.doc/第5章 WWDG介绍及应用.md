# 第五章 WWDG介绍及应用

## 1. WWDG简介

窗口看门狗（WWDG）通常被用来监测由外部干扰或不可预见的逻辑条件造成的应用程序背离正常的运行序列而产生的软件故障。窗口看门狗跟独立看门狗一样，也是一个递减计数器，不同的是它们的复位条件不一样。窗口看门狗产生复位信号有两个条件：

1. 当递减计数器的数值从 0x40 减到 0x3F 时（T6 位跳变到 0）。

2. 当喂狗的时候如果计数器的值大于 W[6:0]时，此数值在 WWDG_CFR 寄存器定义。

上述的两个条件详细解释是，当计数器的值减到 0x40 时还不喂狗的话，到下一个计数就会产生复位，这个值称为窗口的下限值，是固定的值，不能改变。这个跟独立看门狗类似，不同的是窗口看门狗的计数器的值在减到某一个数之前喂狗的话也会产生复位，这个值叫窗口的上限，上限值 W[6:0]由用户设置。窗口看门狗计数器的上限值和下限值就是窗口的含义，喂狗也必须在窗口之内，否则就会复位。

## 2. WWDG超时公式

知道了窗口看门狗的工作原理，下面学习如何计算窗口看门狗的超时公式：

```c
TWWDG = TPCLK1× 4096× 2^WDGTB× (T[5:0] + 1)
```

其中：

1. TWWDG： WWDG 超时时间(单位为 ms）

2. TPCLK1： APB1 以 ms 为单位的时钟间隔（即单位为 1 /KHz） 

3. T[5:0]：窗口看门狗的计数器低 6 位(T6 位固定 1，范围 0x7F~0x40，对应值 0x3F~0x00)

根据以上公式，我们来计算一下对应当重装值为 0x40 时， 分频系数 WDGTB=3 时对应，假 设 PCLK1 = 42MHz ， 下 一 个 计 数 将 发 生 复 位 ，到达下个复位的时间是：1/(42MHz)*4096*2^3*(0+1) =780.19us ，类似地，可以得到在 42MHz 时钟下不同分频值那么可以得到最小-最大超时时间表

![屏幕截图 2025-06-02 135059.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/02-13-51-06-屏幕截图%202025-06-02%20135059.png)

## 3. WWDG应用示例

### 3.1 WWGD初始化

```c
WWDG_HandleTypeDef WWDG_Handler;
/*
    tr：T[6:0]，计数器值
    tw：W[6:0]，窗口值
    fprer：分频系数，Fwwdg=PLCK1/(4096*2^fprer)
*/
void bsp_wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer)
{
    /* WWDG初始化 */
    WWDG_Handler.Instance = WWDG;
    WWDG_Handler.Init.Prescaler = fprer;
    WWDG_Handler.Init.Window = wr;
    WWDG_Handler.Init.Counter = tr;
    WWDG_Handler.Init.EWIMode = WWDG_EWI_ENABLE; // 使能WWDG提前唤醒中断
    HAL_WWDG_Init(&WWDG_Handler);
}

// 此函数会被WWDG_Init()调用
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    __HAL_RCC_WWDG_CLK_ENABLE(); // 使能看门狗时钟
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 3); // 设置看门狗中断优先级
    HAL_NVIC_EnableIRQ(WWDG_IRQn); // 使能看门狗中断
}
```

### 3.2 WWDG中断及回调

```c
// 看门狗中断服务函数
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&WWDG_Handler); // 调用WWDG共用中断处理函数
}

// 此函数会被HAL_WWDG_IRQHandler调用
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    LED_TOGGLE(LED0_GPIO_Pin);
    HAL_WWDG_Refresh(hwwdg); // 刷新看门狗计数器
}
```

### 3.3 主函数测试

```c
#include "bsp_init.h"
#include "wwdg.h"

// 上限时间：Twwdg=4096×8×(0x7F-0x5F)/42MHz=24.98ms
// 下限时间：Twwdg=4096×8×(0x7F-0x3F)/42MHz=49.97ms
// 即喂狗的窗口区间为 24.98~49.97ms。 
// 我们在程序的其它地方没有喂狗，所以程序会在 49.97ms 左右进入中断喂狗一次，并翻转LED0。
int main(void)
{
    bsp_init();
    LED_ON(LED1_GPIO_Pin);
    delay_ms(1000);
    // 先点亮LED1，延时1s后，初始化窗口看门狗，进入死循环，关闭LED1。
    bsp_wwdg_init(0x7F, 0x5F, WWDG_PRESCALER_8); //  WWDG初始化
    while(1)
    {
        LED_OFF(LED1_GPIO_Pin);
    }
}
```

## 4. WWDG常见函数（HAL库）

### 4.1 **初始化函数**

**`HAL_WWDG_Init()`**

- **功能**：配置 WWDG 的分频系数、窗口值、计数器初始值，并启动看门狗。

- **参数**：  
  `WWDG_HandleTypeDef *hwwdg`（指向 WWDG 句柄的指针）。

- **句柄结构体关键成员**：

```c
typedef struct {
  WWDG_TypeDef      *Instance;   // WWDG 寄存器基地址（固定为 WWDG）
  WWDG_InitTypeDef  Init;        // 初始化配置结构体
} WWDG_HandleTypeDef;

typedef struct {
  uint32_t Prescaler;  // 分频系数（WWDG_PRESCALER_1~8）
  uint32_t Window;     // 窗口上限值（0x40 ~ 0x7F）
  uint32_t Counter;    // 计数器初始值（必须 > Window 且 ≤ 0x7F）
  uint32_t EWIMode;    // 早期唤醒中断使能（WWDG_EWI_ENABLE/DISABLE）
} WWDG_InitTypeDef;
```

### 4.2 **MSP 初始化回调函数**

**`HAL_WWDG_MspInit()`**

- **功能**：由 `HAL_WWDG_Init()` 自动调用，用于启用外设时钟和配置 NVIC 中断。

- **典型实现**：

```c
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg) {
  __HAL_RCC_WWDG_CLK_ENABLE();             // 使能 WWDG 时钟
  HAL_NVIC_SetPriority(WWDG_IRQn, 2, 0);   // 设置中断优先级
  HAL_NVIC_EnableIRQ(WWDG_IRQn);           // 使能中断通道
}
```

### 4.3 **喂狗函数**

 **`HAL_WWDG_Refresh()`**

- **功能**：重载计数器值（喂狗），防止复位。

- **参数**：  
  `WWDG_HandleTypeDef *hwwdg`（句柄指针）。

- **注意事项**：
  
  - 必须在 **窗口期内**（计数器值在 `Window` 和 0x3F 之间）调用，否则触发复位。
  
  - 典型用法：在 `HAL_WWDG_EarlyWakeupCallback` 中调用，确保安全喂狗。

### 4.4 **中断服务函数**

**`WWDG_IRQHandler()`**

- **功能**：WWDG 全局中断入口，需在 `stm32fXxx_it.c` 中实现。

- **实现**：

```c
void WWDG_IRQHandler(void) {
  HAL_WWDG_IRQHandler(&hwwdg);  // 调用 HAL 库中断处理
}
```

### 4.5 **HAL 中断处理函数**

 **`HAL_WWDG_IRQHandler()`**

- **功能**：检测中断标志并调用回调函数。

- **内部逻辑**：
  
  1. 检查是否使能 EWI 中断。
  
  2. 检测标志位 `WWDG_FLAG_EWIF`。
  
  3. 清除标志并触发 `HAL_WWDG_EarlyWakeupCallback()`

### 4.6 **早期唤醒回调函数**

 **`HAL_WWDG_EarlyWakeupCallback()`**

- **功能**：**用户必须重写此函数**，在计数器减至 0x40 时执行喂狗或紧急操作。

- **示例**：

```c
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg) {
  HAL_WWDG_Refresh(hwwdg);        // 喂狗
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // 示例：翻转 LED
}
```

- **关键点**：
  
  - 此处是 **唯一安全的喂狗时机**（计数器=0x40），错过将导致复位。
  
  - 避免执行耗时操作（如 `HAL_Delay`），否则可能无法及时喂狗。
