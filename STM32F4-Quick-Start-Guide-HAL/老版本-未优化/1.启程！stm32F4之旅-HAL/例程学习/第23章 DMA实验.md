# 第二十三章 DMA实验

## 1. 硬件设计

本章用到的硬件资源有：

- 指示灯DS0

- KEY0按键

- 串口

- TFTLCD模块

- DMA

本章我们将利用外部按键 KEY0 来控制 DMA 的传送，每按一次 KEY0， DMA 就传送一次数据到USART1，然后在 TFTLCD 模块上显示进度等信息。 DS0 还是用来做为程序运行的指示灯。

## 2. 软件设计

### 2.1 DMA初始化及配置

```c
DMA_HandleTypeDef  UART1TxDMA_Handler; // DMA句柄

// DMAx的各通道配置
// 这里的传输形式是固定的,这点要根据不同的情况来修改
// 从存储器->外设模式/8位数据宽度/存储器增量模式
// DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
// chx:DMA通道选择,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
    if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
    {
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2时钟使能    
    }else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
    }

    __HAL_LINKDMA(&UART1_Handler,hdmatx,UART1TxDMA_Handler);    //将DMA与USART1联系起来(发送DMA)

    //Tx DMA配置
    UART1TxDMA_Handler.Instance=DMA_Streamx;                        // 数据流选择
    UART1TxDMA_Handler.Init.Channel=chx;                            // 通道选择
    UART1TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;         // 存储器到外设
    UART1TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;             // 外设非增量模式
    UART1TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                 // 存储器增量模式
    UART1TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;// 外设数据长度:8位
    UART1TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;   // 存储器数据长度:8位
    UART1TxDMA_Handler.Init.Mode=DMA_NORMAL;                        // 外设普通模式
    UART1TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;           // 中等优先级
    UART1TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    UART1TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    UART1TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;             // 存储器突发单次传输
    UART1TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;          // 外设突发单次传输
    HAL_DMA_DeInit(&UART1TxDMA_Handler);   
    HAL_DMA_Init(&UART1TxDMA_Handler);
} 
```

### 2.2 DMA串口传输

```c
// 开启一次DMA传输
// huart:串口句柄
// pData：传输的数据指针
// Size:传输的数据量
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) // 函数参数：串口句柄，数据指针，数据长度
{
    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size); // 开启DMA传输 
    huart->Instance->CR3 |= USART_CR3_DMAT;//使能串口DMA发送
}
```

### 2.3 主函数

```c
#define SEND_BUF_SIZE 8200    //发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.

u8 SendBuff[SEND_BUF_SIZE]; //发送数据缓冲区
const u8 TEXT_TO_SEND[]={"ALIENTEK Explorer STM32F4 DMA 串口实验"}; 

int main(void)
{
    u16 i;
    u8 t=0;
    u8 j,mask=0;
    float pro=0; 

    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
    delay_init(168);            // 初始化延时函数
    uart_init(115200);          // 初始化USART
    usmart_dev.init(84);         // 初始化USMART
    LED_Init();                    // 初始化LED    
    KEY_Init();                    // 初始化KEY
     LCD_Init();                   // 初始化LCD
    MYDMA_Config(DMA2_Stream7,DMA_CHANNEL_4); // 初始化DMA，使用DMA2_Stream7通道4
    POINT_COLOR=RED; 
    LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");    
    LCD_ShowString(30,70,200,16,16,"DMA TEST");     
    LCD_ShowString(30,130,200,16,16,"KEY0:Start"); 
    POINT_COLOR=BLUE;//设置字体为蓝色 
    //显示提示信息    
    j=sizeof(TEXT_TO_SEND);       
    for(i=0;i<SEND_BUF_SIZE;i++)  // 填充ASCII字符集数据
    {
        if(t>=j)//加入换行符
        {
            if(mask)
            {
                SendBuff[i]=0x0a;
                t=0;
            }else 
            {
                SendBuff[i]=0x0d;
                mask++;
            }    
        }else//复制TEXT_TO_SEND语句
        {
            mask=0;
            SendBuff[i]=TEXT_TO_SEND[t];
            t++;
        }          
    }   
    POINT_COLOR=BLUE;//设置字体为蓝色      
    i=0;
       while(1)
    {
        t=KEY_Scan(0);
        if(t==KEY0_PRES)  //KEY0按下
        {
            printf("\r\nDMA DATA:\r\n");         
            LCD_ShowString(30,150,200,16,16,"Start Transimit....");
            LCD_ShowString(30,170,200,16,16,"   %") ;     //显示百分号      
            HAL_UART_Transmit_DMA(&UART1_Handler,SendBuff,SEND_BUF_SIZE);//启动传输
            //使能串口1的DMA发送 //等待DMA传输完成，此时我们来做另外一些事，点灯
            //实际应用中，传输数据期间，可以执行另外的任务
            while(1)
            {
                if(__HAL_DMA_GET_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
                {
                    __HAL_DMA_CLEAR_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
                    HAL_UART_DMAStop(&UART1_Handler);      //传输完成以后关闭串口DMA
                    break; 
                }
                pro=__HAL_DMA_GET_COUNTER(&UART1TxDMA_Handler);//得到当前还剩余多少个数据
                pro=1-pro/SEND_BUF_SIZE;    //得到百分比      
                pro*=100;                      //扩大100倍
                LCD_ShowNum(30,170,pro,3,16);        
            }
            LCD_ShowNum(30,170,100,3,16);//显示100%      
            LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//提示传送完成
        }
        i++;
        delay_ms(10);
        if(i==20)
        {
            LED0=!LED0;//提示系统正在运行    
            i=0;
        }           
    }
}
```

main 函数的流程大致是：先初始化内存 SendBuff 的值，然后通过 KEY0 开启串口 DMA 发送，在发送过程中，通过__HAL_DMA_GET_COUNTER()函数获取当前还剩余的数据量来计算传输百分比，最后在传输结束之后清除相应标志位，提示已经传输完成。这里还有一点要注意，因为是使用的串口 1 DMA 发送，所以代码中使用 HAL_UART_Transmit_DMA 函数开启串口的 DMA 发送。

## 3. 小结

DMA不光可以用在串口上，比如下面我们可以利用DMA来采集ADC数据

### 实验目标

- 理解 DMA 的工作原理
- 学习如何配置 DMA 进行 ADC 数据采集
- 验证 DMA 数据传输的正确性

### 代码实现

```c
#include "main.h"

#define BUFFER_SIZE 100 // 定义 ADC 数据缓冲区大小
uint32_t adc_buffer[BUFFER_SIZE]; // ADC 数据缓冲区
UART_HandleTypeDef huart2; // UART 句柄

// ADC 和 DMA 相关句柄
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

// 初始化函数声明
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);

// 主函数
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    SystemClock_Config(); // 配置系统时钟
    MX_GPIO_Init(); // 初始化 GPIO
    MX_DMA_Init(); // 初始化 DMA
    MX_ADC1_Init(); // 初始化 ADC
    MX_USART2_UART_Init(); // 初始化 UART

    // 启动 ADC DMA
    HAL_ADC_Start_DMA(&hadc1, adc_buffer, BUFFER_SIZE);

    while (1) {
        // 计算 ADC 数据的平均值
        uint32_t sum = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            sum += adc_buffer[i];
        }
        uint32_t average = sum / BUFFER_SIZE;

        // 将平均值发送到串口
        char buffer[50];
        int len = snprintf(buffer, sizeof(buffer), "Average ADC Value: %lu\r\n", average);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, HAL_MAX_DELAY); // 通过 UART 发送数据

        HAL_Delay(1000); // 每秒发送一次
    }
}

// ADC 初始化
static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // 配置 ADC
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 设置 ADC 时钟
    hadc1.Init.Resolution = ADC_RESOLUTION12b; // 设置分辨率
    hadc1.Init.ScanConvMode = DISABLE; // 禁用扫描模式
    hadc1.Init.ContinuousConvMode = ENABLE; // 启用连续转换模式
    hadc1.Init.DiscontinuousConvMode = DISABLE; // 禁用不连续转换模式
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软件触发
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据对齐
    hadc1.Init.NbrOfConversion = 1; // 只进行一次转换
    HAL_ADC_Init(&hadc1); // 初始化 ADC

    // 配置 ADC 通道
    sConfig.Channel = ADC_CHANNEL_0; // 使用通道 0（PA0）
    sConfig.Rank = 1; // 优先级
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES; // 采样时间
    HAL_ADC_ConfigChannel(&hadc1, &sConfig); // 配置 ADC 通道
}

// DMA 初始化
static void MX_DMA_Init(void) {
    __HAL_RCC_DMA1_CLK_ENABLE(); // 启用 DMA1 时钟

    // 配置 DMA
    hdma_adc1.Instance = DMA1_Stream1; // DMA 流
    hdma_adc1.Init.Channel = DMA_CHANNEL_0; // DMA 通道
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY; // 从外设到内存
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE; // 内存地址增
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // 外设数据对齐
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; // 内存数据对齐
    hdma_adc1.Init.Mode = DMA_CIRCULAR; // 循环模式
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW; // 优先级
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用 FIFO
    HAL_DMA_Init(&hdma_adc1); // 初始化 DMA

    // 连接 ADC 和 DMA
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
}

// UART 初始化
static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2; // UART 实例
    huart2.Init.BaudRate = 115200; // 波特率
    huart2.Init.WordLength = UART_WORDLENGTH_8B; // 字长
    huart2.Init.StopBits = UART_STOPBITS_1; // 停止位
    huart2.Init.Parity = UART_PARITY_NONE; // 奇偶校验
    huart2.Init.Mode = UART_MODE_TX_RX; // 模式
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; // 硬件流控制
    huart2.Init.OverSampling = UART_OVERSAMPLING_16; // 超采样
    HAL_UART_Init(&huart2); // 初始化 UART
}

// 系统时钟配置
void SystemClock_Config(void) {
    // 配置系统时钟（这里使用默认设置）
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // 配置振荡器
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    // 配置时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

// GPIO 初始化
static void MX_GPIO_Init(void) {
    // 此处可添加 GPIO 初始化代码（如 LED 等）
}

// DMA 完成回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    // 可以在此处添加数据处理逻辑
}
```
