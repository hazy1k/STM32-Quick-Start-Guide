# 第四章 IWGD介绍及应用

## 1. IWDG简介

独立看门狗本质上是一个定时器，这个定时器有一个输出端，可以输出复位信号。该定时器是一个 12 位的递减计数器，当计数器的值减到 0 的时候，就会产生一个复位信号。如果在计数没减到 0 之前，重置计数器的值的话，那么就不会产生复位信号，这个动作我们称为喂狗。看门狗功能由 VDD 电压域供电，在停止模式和待机模式下仍然可以工作。

![屏幕截图 2025-06-01 155705.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/01-15-57-12-屏幕截图%202025-06-01%20155705.png)

从 IWDG 框图整体认知就是， IWDG 有一个输入（时钟 LSI），经过一个 8 位的可编程预分频器提供时钟给一个 12 位递减计数器，满足条件就会输出一个复位信号（iwdg1_out_rst）。

## 2. IWDG使用示例

独立看门狗实验的核心是在 STM32F407 内部进行，并不需要外部电路。但是考虑到指示当前状态和喂狗等操作，我们需要 2 个 IO 口，一个用来触发喂狗信号，另外一个用来指示程序是否重启。喂狗我们采用板上的 KEY_UP 键来操作，而程序重启，则是通过 LED0 来指示的。

### 2.1 iwgd配置及喂狗函数

```c
#include "iwdg.h"

IWDG_HandleTypeDef IWDG_Handler;
// time = ((4*2^pre) * reload) / 40 (ms)
void bsp_iwdg_init(uint8_t prescaler, uint16_t reload)
{
    IWDG_Handler.Instance = IWDG;           // 选择IWDG外设
    IWDG_Handler.Init.Prescaler = prescaler;// 设置分频系数
    IWDG_Handler.Init.Reload = reload;      // 设置自动重装载值
    HAL_IWDG_Init(&IWDG_Handler);           // 初始化IWDG
}

void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&IWDG_Handler);        // 喂狗
}
```

### 2.2 主函数测试

```c
#include "bsp_init.h"

int main(void)
{
    // iwdg预分频器4（64倍分频），重载值500
    // time = ((4*2^4)*500)/40 = 800ms
    bsp_init();
    LED_ON(LED0_GPIO_Pin); // 默认开启LED0，iwdg会复位
    printf("iwdg reset !!!\r\n");
    while(1)
    {
        if(key_scan(0) == WKUP_Press)
        {
            printf("key feed !!!\r\n");
            iwdg_feed();
            LED_ON(LED1_GPIO_Pin);
        }
    }
}
```

## 3. IWDG常见函数（HAL库）

### 3.1 初始化函数

**初始化函数：`HAL_IWDG_Init()`**

- **功能**：配置预分频系数（Prescaler）和重装载值（Reload），启动IWDG。

- **参数**：`IWDG_HandleTypeDef *hiwdg`（看门狗句柄指针）。

- **关键结构体**：

```c
typedef struct {
  IWDG_TypeDef  *Instance;  // 看门狗寄存器基址（固定为IWDG）
  IWDG_InitTypeDef Init;    // 初始化参数
} IWDG_HandleTypeDef;

typedef struct {
  uint32_t Prescaler; // 预分频系数（IWDG_PRESCALER_4~256）
  uint32_t Reload;    // 重装载值（0-0xFFF，12位）
} IWDG_InitTypeDef;
```

- **超时时间计算**：
  
  Tout=LSI_FREQ4×2PRESK×RLR​
  
  - `LSI_FREQ` ≈ 32 kHz（实际范围17-47 kHz，设计时需留余量）158。
  
  - **示例**：预分频64（`IWDG_PRESCALER_64`），重载值625，超时 ≈ 1秒：
    
    Tout=320004×26×625​/32000=1秒

### 3.2 喂狗函数

**喂狗函数：`HAL_IWDG_Refresh()`**

- **功能**：将重装载值（RLR）重新载入计数器，防止复位。

- **调用要求**：需在超时前周期性调用（如主循环或定时中断中）


