# 第十五章 DMA介绍及应用

## 1. DMA简介

DMA，全称为： Direct Memory Access，即直接存储器访问。 DMA 传输方式无需 CPU 直接控制传输，也没有中断处理方式那样保留现场和恢复现场的过程，通过硬件为 RAM 与 I/O 设备开辟一条直接传送数据的通路，能使 CPU 的效率大为提高。

STM32F407 最多有 2 个 DMA 控制器(DMA1 和 DMA2)， 两个 DMA 控制器总共有 16 个数据流。 每个通道专门用来管理来自于一个或多个外设对存储器访问的请求。还有一个仲裁器来协调各个 DMA 请求的优先权。

STM32F407 的 DMA 有以下一些特性：

① 双 AHB 主总线架构，一个用于存储器访问，另一个用于外设访问。

② 仅支持 32 位访问的 AHB 从编程接口。

③ 每个 DMA 控制器有 8 个数据流，每个数据流有多达 8 个通道（请求） 。

④ 每个数据流有单独的四级 32 位先进先出存储器缓冲区(FIFO)，可用于 FIFO 模式或直接模式。

⑤ 通过硬件可以将每个数据流配置为：

1，支持外设到存储器、存储器到外设和存储器到存储器传输的常规通道。

2，支持在存储器方双缓冲的双缓冲区通道。

⑥ 8 个数据流中的每一个都连接到专用硬件 DMA 通道（请求） 。

⑦ DMA 数据流请求之间的优先级可用软件编程（4 个级别：非常高、高、中、低），在软件优先级相同的情况下可以通过硬件决定优先级（例如，请求 0 的优先级高于请求 1）

⑧ 每个数据流也支持通过软件触发存储器到存储器的传输（仅限 DMA2 控制器）

⑨可供每个数据流选择的通道请求数多达 8 个。此选择可由软件配置，允许多个外设启动DMA 请求。

⑩ 要传输的数据项的数目可以由 DMA 控制器或外设管理：

1， DMA 流控制器：要传输的数据项的数目是 1 到 65535，可用软件编程。

2，外设流控制器：要传输的数据项的数目未知并由源或目标外设控制，这些外设通过硬件发出传输结束的信号。

⑪ 独立的源和目标传输宽度（字节、半字、字）：源和目标的数据宽度不相等时， DMA自动封装/解封必要的传输数据来优化带宽。这个特性仅在 FIFO 模式下可用。

⑫ 对源和目标的增量或非增量寻址。

⑬ 支持 4 个、 8 个和 16 个节拍的增量突发传输。突发增量的大小可由软件配置，通常等于外设 FIFO 大小的一半。

⑭ 每个数据流都支持循环缓冲区管理。

⑮ 5 个事件标志（DMA 半传输、 DMA 传输完成、 DMA 传输错误、 DMA FIFO 错误、直接模式错误），进行逻辑或运算，从而产生每个数据流的单个中断请求。

## 2. DMA基础使用示例

我们将利用 DMA 来实现串口数据传送，并在 LCD模块上显示当前的传送进度。

### 2.1 DMA配置

```c
#include "dma.h"
#include "usart.h"

extern UART_HandleTypeDef g_uart1_handle;
DMA_HandleTypeDef dma_handle;

// 串口dma传输初始化（存储器->外设）
void dam_uart_init(DMA_Stream_TypeDef *dma_stream_handle, uint32_t ch)
{
    if((uint32_t)dma_stream_handle > (uint32_t)DMA2)
    {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();
    }
    __HAL_LINKDMA(&g_uart1_handle, hdmatx, dma_handle); // 将DMA和USART1绑定
    /*TX DMA configuration*/
    dma_handle.Instance = dma_stream_handle; // 数据流选择
    dma_handle.Init.Channel = ch; // 通道选择
    dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 传输方向：存储器到外设
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增
    dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 存储器地址增
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_handle.Init.Mode = DMA_NORMAL; // 正常模式
    dma_handle.Init.Priority = DMA_PRIORITY_LOW; // 优先级低
    dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO模式
    dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL; // 传输阈值
    dma_handle.Init.MemBurst = DMA_MBURST_SINGLE; // 单次传输
    dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE; // 单次传输
    HAL_DMA_DeInit(&dma_handle); // 复位DMA
    HAL_DMA_Init(&dma_handle);
}
```

### 2.2 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "dma.h"

const uint8_t STRING_TO_SEND[] = "This STM32F407 DMA Transfer Test"; 
#define SEND_BUFF_SIZE ((sizeof(STRING_TO_SEND)+2)*200)
uint8_t send_buff[SEND_BUFF_SIZE];

int main(void)
{
    uint8_t key = 0;
    uint16_t i = 0, j = 0, len, remain;
    uint8_t mask = 0;
    float pb = 0;
    bsp_init();
    LCD_ShowString(30,50,200,16,16,"STM32 DMA Transfer Test");

    // 只初始化一次
    dam_uart_init(DMA2_Stream7, DMA_CHANNEL_4);

    // 填充数据
    len = sizeof(STRING_TO_SEND) - 1; //实际内容长度，不包含\0
    j = 0;
    for(i = 0; i < SEND_BUFF_SIZE; i++)
    {
        if(j >= len)
        {
            if(mask == 0)
            {
                send_buff[i] = 0x0d;
                mask++;
            }
            else
            {
                send_buff[i] = 0x0a;
                mask = 0;
                j = 0;
            }
        }
        else
        {
            send_buff[i] = STRING_TO_SEND[j];
            j++;
            mask = 0;
        }
    }

    while(1)
    {
        key = key_scan(0);
        if(key == KEY0_Press)
        {
            printf("\r\nDMA DATA:\r\n");
            LCD_ShowString(30,130,200,16,16, "Start DMA Transfer...");
            LCD_ShowString(30,150,200,16,16,"   %");

            // 启动DMA传输
            HAL_UART_Transmit_DMA(&g_uart1_handle, send_buff, SEND_BUFF_SIZE);

            // 等待传输完成
            while(__HAL_DMA_GET_COUNTER(&dma_handle) != 0) // DMA还未完成
            {
                remain = __HAL_DMA_GET_COUNTER(&dma_handle);
                pb = 1.0f - ((float)remain / SEND_BUFF_SIZE);
                LCD_ShowNum(30,150,(int)(pb*100),3,16);
            }

            // DMA完成，清标志
            __HAL_DMA_CLEAR_FLAG(&dma_handle, DMA_FLAG_TCIF3_7);
            LCD_ShowNum(30,150,100,3,16);
            LCD_ShowString(30,130,200,16,16, "DMA Transfer Complete!");
            HAL_UART_DMAStop(&g_uart1_handle); // 释放DMA
        }

        // 可控制LED闪烁等
        i++;
        if(i % 10 == 0)
        {
          LED_TOGGLE(LED0_GPIO_Pin);
        }
        delay_ms(10);
    }
}
```

## 3. DMA常见函数（HAL库）

### 3.1 DMA 初始化与配置

#### 3.1.1 `HAL_DMA_Init()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
```

**参数**:

- `hdma`: DMA 句柄指针

**配置结构体**:

```c
typedef struct {
 DMA_Stream_TypeDef *Instance; // DMA 数据流 (如 DMA1_Stream0)
 DMA_InitTypeDef Init; // DMA 初始化参数
 HAL_LockTypeDef Lock; // 锁定对象
 __IO HAL_DMA_StateTypeDef State; // DMA 状态
 void *Parent; // 父对象指针
 void (* XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);
 void (* XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);
 void (* XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);
 __IO uint32_t ErrorCode; // 错误代码
} DMA_HandleTypeDef;

typedef struct {
 uint32_t Channel; // 通道选择 (DMA_CHANNEL_0 到 7)
 uint32_t Direction; // 传输方向:
 // DMA_MEMORY_TO_MEMORY
 // DMA_MEMORY_TO_PERIPH
 // DMA_PERIPH_TO_MEMORY
 uint32_t PeriphInc; // 外设地址增量: DMA_PINC_ENABLE/DISABLE
 uint32_t MemInc; // 内存地址增量: DMA_MINC_ENABLE/DISABLE
 uint32_t PeriphDataAlignment; // 外设数据宽度:
 // DMA_PDATAALIGN_BYTE
 // DMA_PDATAALIGN_HALFWORD
 // DMA_PDATAALIGN_WORD
 uint32_t MemDataAlignment; // 内存数据宽度:
 // DMA_MDATAALIGN_BYTE
 // DMA_MDATAALIGN_HALFWORD
 // DMA_MDATAALIGN_WORD
 uint32_t Mode; // 传输模式:
 // DMA_NORMAL (单次)
 // DMA_CIRCULAR (循环)
 uint32_t Priority; // 优先级:
 // DMA_PRIORITY_LOW
 // DMA_PRIORITY_MEDIUM
 // DMA_PRIORITY_HIGH
 // DMA_PRIORITY_VERY_HIGH
 uint32_t FIFOMode; // FIFO 模式: DMA_FIFOMODE_ENABLE/DISABLE
 uint32_t FIFOThreshold; // FIFO 阈值:
 // DMA_FIFO_THRESHOLD_1QUARTERFULL
 // DMA_FIFO_THRESHOLD_HALFFULL
 // DMA_FIFO_THRESHOLD_3QUARTERSFULL
 // DMA_FIFO_THRESHOLD_FULL
 uint32_t MemBurst; // 内存突发传输:
 // DMA_MBURST_SINGLE
 // DMA_MBURST_INCR4
 // DMA_MBURST_INCR8
 // DMA_MBURST_INCR16
 uint32_t PeriphBurst; // 外设突发传输:
 // DMA_PBURST_SINGLE
 // DMA_PBURST_INCR4
 // DMA_PBURST_INCR8
 // DMA_PBURST_INCR16
} DMA_InitTypeDef;
```

**功能**: 初始化 DMA 数据流  
**示例配置**:

```c
DMA_HandleTypeDef hdma_adc1;

void DMA_ADC1_Init(void) {
    __HAL_RCC_DMA2_CLK_ENABLE();
    
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_adc1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_adc1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_adc1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
        Error_Handler();
    }
    
    // 关联到ADC1
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
}
```

### 3.2 DMA 传输控制

#### 3.2.1 启动 DMA 传输

```c
// 启动 DMA 传输 (无中断)
HAL_StatusTypeDef HAL_DMA_Start(
 DMA_HandleTypeDef *hdma,
 uint32_t SrcAddress,
 uint32_t DstAddress,
 uint32_t DataLength)

// 启动 DMA 传输 (带中断)
HAL_StatusTypeDef HAL_DMA_Start_IT(
 DMA_HandleTypeDef *hdma,
 uint32_t SrcAddress,
 uint32_t DstAddress,
 uint32_t DataLength)
```

**示例** (ADC 连续转换):

```c
#define ADC_BUF_SIZE 256
uint16_t adc_buffer[ADC_BUF_SIZE];

// 启动ADC DMA传输
HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_SIZE);
```

#### 3.2.2 停止 DMA 传输

```c
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma)
```

### 3.3 DMA 中断处理

#### 3.3.1 中断服务函数

```c
// DMA数据流中断服务函数
void DMA2_Stream0_IRQHandler(void) {
 HAL_DMA_IRQHandler(&hdma_adc1);
}
```

#### 3.3.2 回调函数 (用户实现)

```c
// 传输完成回调
__weak void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)

// 半传输完成回调
__weak void HAL_DMA_XferHalfCpltCallback(DMA_HandleTypeDef *hdma)

// 传输错误回调
__weak void HAL_DMA_XferErrorCallback(DMA_HandleTypeDef *hdma)
```

**示例实现**:

```c
void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma) {
 if(hdma == &hdma_adc1) {
 // 处理完整的ADC数据
 Process_ADC_Data();
 }
}

void HAL_DMA_XferHalfCpltCallback(DMA_HandleTypeDef *hdma) {
 if(hdma == &hdma_adc1) {
 // 处理前一半ADC数据
 Process_Half_ADC_Data();
 }
}
```

### 3.4 DMA 状态管理

#### 3.4.1 状态检查函数

```c
// 获取DMA状态
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma)

// 获取错误代码
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma)
```

#### 3.4.2 状态枚举

```c
HAL_DMA_STATE_RESET = 0x00U, // 未初始化
HAL_DMA_STATE_READY = 0x01U, // 就绪
HAL_DMA_STATE_BUSY = 0x02U, // 忙
HAL_DMA_STATE_TIMEOUT = 0x03U, // 超时
HAL_DMA_STATE_ERROR = 0x04U // 错误
```

### 3.5 外设 DMA 集成函数

#### 3.5.1 UART DMA 传输

```c
HAL_DMA_STATE_RESET = 0x00U, // 未初始化
HAL_DMA_STATE_READY = 0x01U, // 就绪
HAL_DMA_STATE_BUSY = 0x02U, // 忙
HAL_DMA_STATE_TIMEOUT = 0x03U, // 超时
HAL_DMA_STATE_ERROR = 0x04U // 错误
```

#### 3.5.2 ADC DMA 传输

```c
// 启动ADC DMA转换
HAL_StatusTypeDef HAL_ADC_Start_DMA(
 ADC_HandleTypeDef *hadc,
 uint32_t *pData,
 uint32_t Length)
```

#### 3.5.3 SPI DMA 传输

```c
// 启动SPI DMA接收
HAL_StatusTypeDef HAL_SPI_Receive_DMA(
 SPI_HandleTypeDef *hspi,
 uint8_t *pData,
 uint16_t Size)

// 启动SPI DMA发送
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(
 SPI_HandleTypeDef *hspi,
 uint8_t *pData,
 uint16_t Size)
```

### 3.6 内存到内存传输

#### 3.6.1 `HAL_DMA_Start()`

**功能**: 内存到内存传输  
**示例**:

```c
#define DATA_SIZE 1024
uint32_t src_buffer[DATA_SIZE];
uint32_t dst_buffer[DATA_SIZE];

// 填充源数据
Fill_Source_Buffer(src_buffer, DATA_SIZE);

// 配置DMA
DMA_HandleTypeDef hdma_mem2mem;
hdma_mem2mem.Instance = DMA2_Stream0;
hdma_mem2mem.Init.Channel = DMA_CHANNEL_0;
hdma_mem2mem.Init.Direction = DMA_MEMORY_TO_MEMORY;
// ... 其他配置

HAL_DMA_Init(&hdma_mem2mem);

// 启动内存到内存传输
HAL_DMA_Start(&hdma_mem2mem, 
(uint32_t)src_buffer, 
(uint32_t)dst_buffer, 
DATA_SIZE);

// 等待传输完成
HAL_DMA_PollForTransfer(&hdma_mem2mem, HAL_DMA_FULL_TRANSFER, 100);
```

### 3.7 DMA 轮询模式

#### 3.7.1 `HAL_DMA_PollForTransfer()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_DMA_PollForTransfer(
 DMA_HandleTypeDef *hdma,
 uint32_t CompleteLevel,
 uint32_t Timeout)
```

**参数**:

- `CompleteLevel`: 传输完成级别  
  `HAL_DMA_FULL_TRANSFER` - 完整传输  
  `HAL_DMA_HALF_TRANSFER` - 半传输

- `Timeout`: 超时时间 (ms)

**示例**:

```c
HAL_DMA_Start(&hdma_mem2mem, src, dst, length);
if(HAL_DMA_PollForTransfer(&hdma_mem2mem, HAL_DMA_FULL_TRANSFER, 100) == HAL_OK) {
 // 传输成功
} else {
 // 传输超时或出错
}
```

### 3.8 DMA 中断配置

#### 3.8.1 中断使能与配置

```c
// 配置DMA流中断
void DMA_Config_Interrupt(DMA_HandleTypeDef *hdma) {
    // 使能传输完成中断
    __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TC);
    
    // 使能半传输中断
    __HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);
    
    // 使能传输错误中断
    __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TE);
    
    // 配置NVIC
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}
```

### 3.9 双缓冲模式 (Double Buffer)

#### 3.9.1 `HAL_DMAEx_MultiBufferStart()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(
 DMA_HandleTypeDef *hdma,
 uint32_t SrcAddress,
 uint32_t DstAddress,
 uint32_t SecondMemAddress,
 uint32_t DataLength)
```

**功能**: 启动双缓冲 DMA 传输  
**示例** (ADC 双缓冲):

```c
#define ADC_BUF_SIZE 256
uint16_t adc_buffer0[ADC_BUF_SIZE];
uint16_t adc_buffer1[ADC_BUF_SIZE];

// 启动双缓冲ADC DMA
HAL_ADCEx_MultiModeStart_DMA(&hadc1, 
(uint32_t*)adc_buffer0, 
(uint32_t*)adc_buffer1, 
ADC_BUF_SIZE);
```

#### 3.9.2 双缓冲回调函数

```c
// 内存0传输完成回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
 // 处理buffer0数据
}

// 内存1传输完成回调
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
 // 处理buffer1数据
}
```

---


