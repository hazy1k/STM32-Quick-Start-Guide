# 第九章 DMA介绍及应用

## 1. DMA简介

DMA(Direct Memory Access)—直接存储器存取，是单片机的一个外设，它的主要功能是用来搬数据，但是不需要占用 CPU，即在传输数据的时候， CPU 可以干其他的事情，好像是多线程一样。数据传输支持从外设到存储器或者存储器到存储器，这里的存储器可以是 SRAM 或者是 FLASH。DMA 控制器包含了 DMA1 和 DMA2，其中 DMA1 有 7 个通道， DMA2 有 5 个通道，这里的通道可以理解为传输数据的一种管道。

如果外设要想通过 DMA 来传输数据，必须先给 DMA 控制器发送 DMA 请求， DMA 收到请求信号之后，控制器会给外设一个应答信号，当外设应答后且 DMA 控制器收到应答信号之后，就会启动 DMA 的传输，直到传输完毕。

## 2. DMA初始化结构体

标准库函数对每个外设都建立了一个初始化结构体 xxx_InitTypeDef(xxx 为外设名称)，结构体成员用于设置外设工作参数，并由标准库函数 xxx_Init() 调用这些设定参数进入设置外设相应的寄存器，达到配置外设工作环境的目的。

结构体 xxx_InitTypeDef 和库函数 xxx_Init 配合使用是标准库精髓所在，理解了结构体xxx_InitTypeDef 每个成员意义基本上就可以对该外设运用自如。结构体 xxx_InitTypeDef 定义在stm32f10x_xxx.h(后面 xxx 为外设名称) 文件中，库函数 xxx_Init 定义在 stm32f10x_xxx.c 文件中，编程时我们可以结合这两个文件内注释使用。

DMA_ InitTypeDef 初始化结构体

```c
typedef struct
{    
    uint32_t DMA_PeripheralBaseAddr; // 外设地址
    uint32_t DMA_MemoryBaseAddr; // 存储器地址
    uint32_t DMA_DIR; // 传输方向
    uint32_t DMA_BufferSize; // 传输数目
    uint32_t DMA_PeripheralInc; // 外设地址增量模式
    uint32_t DMA_MemoryInc; // 存储器地址增量模式
    uint32_t DMA_PeripheralDataSize; // 外设数据宽度
    uint32_t DMA_MemoryDataSize; // 存储器数据宽度
    uint32_t DMA_Mode; // 模式选择
    uint32_t DMA_Priority; // 通道优先级
    uint32_t DMA_M2M; // 存储器到存储器模式
} DMA_InitTypeDef;
```

结构体成员说明：

1. **`DMA_PeripheralBaseAddr`**
   
   - 外设的基地址，DMA 传输的目标外设地址。

2. **`DMA_MemoryBaseAddr`**
   
   - 存储器的基地址，DMA 传输的源或目的存储器地址。

3. **`DMA_DIR`**
   
   - 传输方向，选择外设到存储器（`DMA_DIR_PeripheralToMemory`）还是存储器到外设（`DMA_DIR_MemoryToPeripheral`），或存储器到存储器（`DMA_DIR_MemoryToMemory`）。

4. **`DMA_BufferSize`**
   
   - 传输的数据量，指定 DMA 每次传输的字节数。

5. **`DMA_PeripheralInc`**
   
   - 外设地址增量模式。配置是否在传输过程中增加外设地址，通常情况下，外设地址是固定的，但某些外设可能需要逐次增加地址（例如 SPI 外设的寄存器）。

6. **`DMA_MemoryInc`**
   
   - 存储器地址增量模式。配置是否在传输过程中增加存储器地址。`DMA_MemoryInc_Enable` 会使存储器地址自增，`DMA_MemoryInc_Disable` 则禁止自增。

7. **`DMA_PeripheralDataSize`**
   
   - 外设数据宽度。通常配置为 8 位、16 位或 32 位，取决于外设的传输宽度。

8. **`DMA_MemoryDataSize`**
   
   - 存储器数据宽度。指定存储器中数据的宽度，通常为 8 位、16 位或 32 位。

9. **`DMA_Mode`**
   
   - 配置 DMA 的工作模式。常见模式有：
     - `DMA_Mode_Normal`：正常模式。
     - `DMA_Mode_Circular`：环形模式，传输完成后会自动重新加载 DMA 设置，适用于连续的数据流应用。

10. **`DMA_Priority`**
    
    - DMA 通道的优先级配置。常见优先级有：
    - `DMA_Priority_Low`：低优先级。
    - `DMA_Priority_Medium`：中等优先级。
    - `DMA_Priority_High`：高优先级。
    - `DMA_Priority_VeryHigh`：非常高优先级。

11. **`DMA_M2M`**
    
    - 配置是否为存储器到存储器模式。
    - `DMA_M2M_Disable`：不启用存储器到存储器传输。
    - `DMA_M2M_Enable`：启用存储器到存储器传输模式（这通常用于从一个存储区域传输数据到另一个存储区域）。

## 3. DMA使用示例

### 3.1 DMA存储器到存储器

#### 3.1.1DMA相关参数宏定义

```c
#ifndef __DMA_H__
#define __DMA_H__ 

#include "stm32f10x.h" 

#define DMA_CHx DMA1_Channel6
#define DMA_CLK RCC_AHBPeriph_DMA1
#define DMA_FLAG_TC DMA1_FLAG_TC6 // 传输完成标志
#define DMA_BUFFER_SIZE 32 // DMA缓存大小

extern uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE];
extern uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];

void BSP_DMA_Init(void);

#endif /* __DMA_H__ */
```

#### 3.1.2 DMA配置

```c
#include "dma.h"

void BSP_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
    // 选择源数据地址和目标数据地址，本工程为存储器到存储器
    // 数据源为FLASH，目标SRAM
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)Send_DMA_BUFFER;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Rece_DMA_BUFFER;
    // 设置数据的传输方向和大小
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 源数据地址为存储器
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE; 
    // 设置外设地址和内存地址递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 设置数据单位和传输模式以及优先级
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    // 初始化
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable; // 存储器到存储器
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_ClearFlag(DMA_FLAG_TC);
    DMA_Cmd(DMA_CHx, ENABLE);
}
```

#### 3.1.3 主函数测试

```c
#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "SysTick.h"
#include "usart.h"
#include "dma.h"
#include <stdio.h>

// DMA要传输的数据-存储在内部的FLASH中
uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE] = {0x11223344, 0x55667788, 0x99AABBCC, 0xDDCCEEFF};
// DMA接收到的数据-存储在内部的SRAM中
uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];

// 判断两个数据是否相同
uint8_t Buffer_Compare(uint32_t *pBuffer1, uint32_t *pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return 0;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return 1;
}

int main(void) 
{
  uint8_t Buffer_Result;
  SysTick_Init();
  BSP_LED_Init();
  BSP_Beep_Init();
  USARTx_Init();
  Delay_ms(2000);
  BSP_DMA_Init();

  while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET); // 等待DMA传输完成

  Buffer_Result = Buffer_Compare(Send_DMA_BUFFER, Rece_DMA_BUFFER, DMA_BUFFER_SIZE);

  if(Buffer_Result == 1)
  {
    LED_ON(BLUE_LED_Pin);
    BEEP_OFF();
    printf("DMA_Transfer_Complete\r\n");

    // 打印详细数据
    printf("Sent Buffer Address: %p\r\n", Send_DMA_BUFFER);
    printf("Sent Data:\t\t");
    for(int i = 0; i < DMA_BUFFER_SIZE; i++) {
        printf("%08X ", Send_DMA_BUFFER[i]);
    }

    printf("\r\nReceived Buffer Address: %p\r\n", Rece_DMA_BUFFER);
    printf("Received Data:\t");
    for(int i = 0; i < DMA_BUFFER_SIZE; i++) {
        printf("%08X ", Rece_DMA_BUFFER[i]);
    }
    printf("\r\n");
  }
  else
  {
    LED_ON(RED_LED_Pin);
    BEEP_ON();
    printf("DMA_Transfer_Error\r\n");
  }

  while(1);
}
```

### 3.2 DMA存储器到外设

#### 3.2.1 DMA相关参数宏定义

```c
#ifndef __DMA_H__
#define __DMA_H__ 

#include "stm32f10x.h" 

#define USART_DMA_CHx      DMA1_Channel4
#define DMA_CLK            RCC_AHBPeriph_DMA1
#define USART_DR_ADDRESS   (USART1_BASE + 0x04) // USART1 数据寄存器地址
#define DMA_TC_FLAG        DMA1_FLAG_TC4        // 通道4传输完成标志
#define DMA_BUFFER_SIZE    32                   // DMA缓存大小

extern uint8_t Send_DMA_BUFFER[DMA_BUFFER_SIZE]; // 发送缓冲区

void USART_DMA_Init(void);
void DMA_StartTransfer(void);
uint8_t DMA_IsTransferComplete(void);

#endif /* __DMA_H__ */

```

#### 3.2.2 DMA配置

```c
void USART_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    // 1. 使能DMA时钟
    RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
    
    // 2. 配置DMA参数
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Send_DMA_BUFFER;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // 内存->外设
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;         // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;       // 普通模式(单次传输)
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;        // 禁用内存到内存
    
    DMA_Init(USART_DMA_CHx, &DMA_InitStructure);
    
    // 3. 清除传输完成标志
    DMA_ClearFlag(DMA_TC_FLAG);
}
```

#### 3.2.3 启动DMA传输和检查是否完成

```c
// 启动DMA传输
void DMA_StartTransfer(void)
{
    DMA_Cmd(USART_DMA_CHx, DISABLE);   // 先禁用通道
    DMA_SetCurrDataCounter(USART_DMA_CHx, DMA_BUFFER_SIZE); // 重置数据计数器
    DMA_Cmd(USART_DMA_CHx, ENABLE);    // 启用通道
}

// 检查传输是否完成
uint8_t DMA_IsTransferComplete(void)
{
    return (DMA_GetFlagStatus(DMA_TC_FLAG) == SET);
}
```

#### 3.2.4 主函数测试

```c
#include "stm32f10x.h"
#include "led.h"
#include "SysTick.h"
#include "usart.h"
#include "dma.h"
#include <stdio.h>
#include <string.h>

uint8_t Send_DMA_BUFFER[DMA_BUFFER_SIZE];

int main(void) 
{
    SysTick_Init();
    BSP_LED_Init();
    USARTx_Init();  // 初始化USART1
    
    // 1. 准备要发送的数据
    const char *message = "Hello DMA! Memory to USART transfer.\r\n";
    uint8_t msg_len = strlen(message);
    
    // 确保不超过缓冲区大小
    uint8_t copy_len = (msg_len < DMA_BUFFER_SIZE) ? msg_len : DMA_BUFFER_SIZE;
    memcpy(Send_DMA_BUFFER, message, copy_len);
    
    // 2. 初始化DMA
    USART_DMA_Init();
    
    // 3. 启用USART的DMA发送请求
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    
    // 4. 启动DMA传输
    DMA_StartTransfer();
    
    LED_ON(BLUE_LED_Pin);  // 开始传输，蓝灯亮
    
    // 5. 等待传输完成
    while(!DMA_IsTransferComplete()); 
    
    LED_OFF(BLUE_LED_Pin); // 传输完成，蓝灯灭
    LED_ON(GREEN_LED_Pin); // 绿灯亮表示成功
    
    // 6. 通过串口打印状态信息
    printf("DMA Transfer Complete!\r\n");
    printf("Sent %d bytes via DMA\r\n", copy_len);
    
    while(1) {
        // 可添加其他操作
    }
}
```

1. **数据准备阶段**：
   
   - 在内存缓冲区(`Send_DMA_BUFFER`)中准备要发送的数据
   
   - 使用标准库函数填充数据，确保数据已就绪

2. **DMA初始化阶段**：
   
   - 配置DMA通道为内存到外设模式
   
   - 设置USART数据寄存器为目标地址
   
   - 配置传输参数（数据大小、地址增量等）

3. **传输启动阶段**：
   
   - 启用USART的DMA发送请求
   
   - 重置DMA数据计数器
   
   - 使能DMA通道开始传输

4. **传输监控阶段**：
   
   - 使用LED指示传输状态（蓝灯=传输中）
   
   - 轮询DMA传输完成标志
   
   - 传输完成后切换LED状态（绿灯=完成）

5. **结果验证**：
   
   - 通过串口打印传输完成信息
   
   - 在终端查看实际发送的数据

## 4. DMA常见函数（STD库）

### 4.1 DMA 配置步骤

#### 4.1.1 使能时钟

```c
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能DMA1时钟
// 或
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);  // 使能DMA2时钟
```

#### 4.1.2 初始化DMA通道

```c
DMA_InitTypeDef DMA_InitStruct;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;  // 外设地址
DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buffer;          // 内存地址
DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;                // 传输方向
DMA_InitStruct.DMA_BufferSize = BUFFER_SIZE;                   // 传输数据量
DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;           // 内存地址递增
DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度
DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 内存数据宽度
DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;                     // 传输模式
DMA_InitStruct.DMA_Priority = DMA_Priority_High;               // 通道优先级
DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;                      // 非内存到内存模式

DMA_Init(DMA1_Channel4, &DMA_InitStruct);  // 初始化通道
```

#### 4.1.3 使能DMA通道

```c
DMA_Cmd(DMA1_Channel4, ENABLE);  // 使能DMA通道
```

#### 4.1.4 配置外设DMA请求

```c
// 使能USART1的DMA发送请求
USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
```

### 4.2 核心DMA操作函数

#### 4.2.1 初始化函数

```c
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
```

#### 4.2.2 通道控制

```c
// 使能/禁用通道
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);

// 设置传输数据量
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber);

// 获取剩余传输数据量
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
```

#### 4.2.3 中断配置

```c
// 使能DMA中断
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);

// 检查中断标志
ITStatus DMA_GetITStatus(uint32_t DMA_IT);

// 清除中断标志
void DMA_ClearITPendingBit(uint32_t DMA_IT);
```

| **中断标志**    | **描述**     |
| ----------- | ---------- |
| `DMA_IT_TC` | 传输完成中断     |
| `DMA_IT_HT` | 半传输完成中断    |
| `DMA_IT_TE` | 传输错误中断     |
| `DMA_IT_GL` | 全局中断（所有事件） |

### 4.3 高级功能配置

#### 4.3.1 双缓冲模式

```c
// 配置两个内存地址
DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)buffer1;
DMA_InitStruct.DMA_Memory1BaseAddr = (uint32_t)buffer2;
DMA_InitStruct.DMA_BufferSize = BUFFER_SIZE;

// 使能双缓冲模式
DMA_DoubleBufferModeConfig(DMA1_Channel1, (uint32_t)buffer1, DMA_Memory_0);
DMA_DoubleBufferModeCmd(DMA1_Channel1, ENABLE);

// 切换缓冲区
DMA_MemoryTargetConfig(DMA1_Channel1, (uint32_t)next_buffer, DMA_Memory_1);
```

#### 4.3.2 循环模式

```c
DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;  // 循环模式

// 应用配置后，DMA会自动重载传输
```

#### 4.3.3 内存到内存传输

```c
DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)src_buffer;
DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;      // 使能内存到内存
```

#### 4.3.4 状态监控函数

```c
// 获取传输状态
DMA_GetFlagStatus(DMA1_FLAG_TC4);  // 检查传输完成标志

// 清除状态标志
DMA_ClearFlag(DMA1_FLAG_TC4);

// 检查通道使能状态
FunctionalState status = DMA_GetCmdStatus(DMA1_Channel4);
```
