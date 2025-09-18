# 第六十九章 ChipID-芯片ID

## 1. 硬件设计

无硬件设计

## 2. 软件设计

### 2.1 编程大纲

1. 获取芯片ID函数

2. 主函数测试

### 2.2 代码分析

#### 2.2.1 获取芯片ID函数

```c
#include "chipid.h"

uint32_t Unique_ID[3];

void Get_Unique_ID(void)
{
    Unique_ID[0] = *(__IO uint32_t*)(0x1FFFF7F0);
    Unique_ID[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    Unique_ID[2] = *(__IO uint32_t*)(0x1FFFF7E8);
}
```

#### 2.2.2 主函数测试

```c
#include "stm32f10x.h"
#include "chipid.h"
#include "usart.h"

extern uint32_t Unique_ID[3];

int main(void)
{
  USART_Config();
  Get_Unique_ID();
  printf("Unique ID: %08X-%08X-%08X\r\n", Unique_ID[0],Unique_ID[1],Unique_ID[2]);
  printf("芯片flash大小: %d KB\r\n", *(__IO uint16_t*)(0x1FFFF7E0));
  while(1);
}
```
