# 第七章 IWDG介绍及应用

## 1. IWDG简介

STM32 有两个看门狗，一个是独立看门狗另外一个是窗口看门狗，独立看门狗号称宠物狗，窗口看门狗号称警犬，本章我们主要分析独立看门狗的功能框图和它的应用。独立看门狗用通俗一点的话来解释就是一个 12 位的递减计数器，当计数器的值从某个值一直减到 0 的时候，系统就会产生一个复位信号，即 IWDG_RESET。如果在计数没减到 0 之前，刷新了计数器的值的话，那么就不会产生复位信号，这个动作就是我们经常说的喂狗。看门狗功能由 VDD 电压域供电，在停止模式和待机模式下仍能工作。

独立看门狗一般用来检测和解决由程序引起的故障，比如一个程序正常运行的时间是 50ms，在运行完这个段程序之后紧接着进行喂狗，我们设置独立看门狗的定时溢出时间为 60ms，比我们需要监控的程序 50ms 多一点，如果超过 60ms 还没有喂狗，那就说明我们监控的程序出故障了，跑飞了，那么就会产生系统复位，让程序重新运行。

## 2. IWDG应用示例

### 2.1 IWDG初始化

```c
void IWDG_Init(uint8_t prv, uint16_t rld)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 允许写IWDG_PR和IWDG_RLD寄存器
    IWDG_SetPrescaler(prv); // 设置预分频器值
    IWDG_SetReload(rld); // 设置重装载值
    IWDG_ReloadCounter(); // 重新装载
    IWDG_Enable();
}
```

### 2.2 IWDG喂狗函数

```c
void IWDG_Feed(void)
{
    IWDG_ReloadCounter(); // 重新装载
}
```

### 2.3 主函数测试

```c
#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"
#include "usart.h"
#include "stdio.h"

int main(void)
{
  BSP_LED_Init();
  BSP_KEY_Init();
  USARTx_Init();
  IWDG_Init(IWDG_Prescaler_64, 625); // 大约1s
  if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    printf("IWDG Reset\r\n");
    // 独立看门狗复位
    LED_ON(RED_LED_Pin);
    RCC_ClearFlag();
  }
  else
  {
    printf("Other Reset\r\n");
    // 其他复位
    LED_ON(BLUE_LED_Pin);
  }
  while(1)
  {
    if(KEY_Scan(KEY1_GPIO_Port, KEY1_GPIO_Pin) == KEY_ON)
    {
      printf("IWDG Feed\r\n");
      IWDG_Feed();
      LED_ON(GREEN_LED_Pin);
    }
  }
}
```

## 3. IWDG常见函数（STD库）

### 3.1 初始化函数

```c
void IWDG_Init(uint8_t prer, uint16_t rlr) {
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  // 解锁寄存器
    IWDG_SetPrescaler(prer);                      // 设置预分频系数
    IWDG_SetReload(rlr);                          // 设置重装载值
    IWDG_ReloadCounter();                         // 首次喂狗
    IWDG_Enable();                                // 启动看门狗
}
```

### 3.2 喂狗函数

```c
void IWDG_FeedDog(void) {
    IWDG_ReloadCounter();  // 重载计数器（写入0xAAAA

}
```

### 3.3 状态检查函数

```c
if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) {
    // 系统复位源为IWDG
    RCC_ClearFlag();  // 清除复位标志
}
```

| 预分频系数 | 重装载值 | 超时时间    | 适用场景   |
| ----- | ---- | ------- | ------ |
| `4`   | 500  | 50 ms   | 高速任务监控 |
| `64`  | 625  | 1000 ms | 常规任务   |
| `128` | 1000 | 5120 ms | 低功耗模式  |

> **公式推导**：
> 
> - 分频后时钟 = 40 kHz / (4 × 2<sup>prv</sup>)
> 
> - 计数周期 = 1 / (分频后时钟)
> 
> - 超时时间 = (rlr + 1) × 计数周期
