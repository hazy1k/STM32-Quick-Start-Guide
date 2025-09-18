# 第十八章 ADC实验

## 1. 硬件设计

本实验用到的硬件资源有：

- 指示灯DS0

- TFTLCD模块

- ADC

- 杜邦线

前面 2 个均已介绍过，而 ADC 属于 STM32F4 内部资源，实际上我们只需要软件设置就可以正常工作，不过我们需要在外部连接其端口到被测电压上面。本章，我们通过 ADC1 的通道5（PA5）来读取外部电压值， 探索者 STM32F4 开发板没有设计参考电压源在上面，但是板上有几个可以提供测试的地方： 1. 3.3V 电源。 2. GND。 3. 后备电池。注意：这里不能接到板上 5V 电源上去测试，这可能会烧坏 ADC!。

因为要连接到其他地方测试电压，所以我们需要 1 根杜邦线，或者自备的连接线也可以，一头插在多功能端口 P12 的 ADC 插针上（与 PA5 连接），另外一头就接你要测试的电压点（确保该电压不大于 3.3V 即可）。

## 2. 软件设计

### 2.1 ADC初始化

```c
// 初始化ADC
// ch: ADC_channels 
// 通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
    ADC1_Handler.Instance = ADC1; // 选取ADC1
    ADC1_Handler.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;// 4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
    ADC1_Handler.Init.Resolution = ADC_RESOLUTION_12B;          // 12位模式
    ADC1_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;          // 右对齐
    ADC1_Handler.Init.ScanConvMode = DISABLE;                   // 非扫描模式
    ADC1_Handler.Init.EOCSelection = DISABLE;                   // 关闭EOC中断
    ADC1_Handler.Init.ContinuousConvMode = DISABLE;             // 关闭连续转换
    ADC1_Handler.Init.NbrOfConversion = 1;                      // 1个转换在规则序列中 也就是只转换规则序列1 
    ADC1_Handler.Init.DiscontinuousConvMode = DISABLE;          // 禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion = 0;                  // 不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;    // 软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;// 使用软件触发
    ADC1_Handler.Init.DMAContinuousRequests = DISABLE;          // 关闭DMA请求
    HAL_ADC_Init(&ADC1_Handler);                                // ADC初始化 
}

```

### 2.2 ADC GPIO配置

```c
// ADC底层驱动，引脚配置，时钟使能
// 此函数会被HAL_ADC_Init()调用
// hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();        // 使能ADC1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();	    // 开启GPIOA时钟
    GPIO_Initure.Pin=GPIO_PIN_5;        // PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG; // 模拟输入模式
    GPIO_Initure.Pull=GPIO_NOPULL;      // 不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
```

### 2.3 获取ADC值

```c
// 获得ADC值
// ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
// 返回值:转换结果
u16 Get_Adc(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf; 
    ADC1_ChanConf.Channel=ch;                           // 通道
    ADC1_ChanConf.Rank=1;                               // 第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;// 采样时间
    ADC1_ChanConf.Offset=0;                             // 偏移量为0
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);// 通道配置
    HAL_ADC_Start(&ADC1_Handler);                       // 开启ADC
    HAL_ADC_PollForConversion(&ADC1_Handler,10);        // 轮询转换
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	    // 返回最近一次ADC1规则组的转换结果
}
```

### 2.4 多次采集取平均值

```c
// 获取指定通道的转换值，取times次,然后平均 
// times:获取次数
// 返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u32 ch,u8 times)
{
	u32 temp_val = 0; // 临时变量
	u8 t;
	for(t = 0; t < times; t++) // 循环获取转换值
	{
		temp_val+=Get_Adc(ch); // 获取一次转换值
		delay_ms(5);
	}
	return temp_val/times; // 计算平均值并返回
} 
```

### 2.5 主函数

```c
int main(void)
{
    u16 adcx;
	float temp;
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	usmart_dev.init(84); 		 // 初始化USMART
	LED_Init();				 	 // 初始化LED	
	KEY_Init();					 // 初始化KEY
 	LCD_Init();           		 // 初始化LCD
    MY_ADC_Init();               // 初始化ADC1通道5
	POINT_COLOR = RED; // 设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:"); // 显示提示信息	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	// 先在固定位置显示小数点  	
    while(1)
	{
        adcx = Get_Adc_Average(ADC_CHANNEL_5,20); // 获取通道5的转换值，20次取平均
		LCD_ShowxNum(134,130,adcx,4,16,0);        // 显示ADCC采样后的原始值
		temp=(float)adcx*(3.3/4096);              // 获取计算后的带小数的实际电压值，比如3.1111
		adcx=temp;                                // 赋值整数部分给adcx变量，因为adcx为u16整形
		LCD_ShowxNum(134,150,adcx,1,16,0);        // 显示电压值的整数部分，3.1111的话，这里就是显示3
		temp-=adcx;                               // 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
		temp*=1000;                               // 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
		LCD_ShowxNum(150,150,temp,3,16,0X80);     // 显示小数部分（前面转换为了整形显示），这里显示的就是111.
		LED0 = !LED0;
		delay_ms(250);	
	} 
}
```

## 3. 小结

本章主要就是一个配置的事，然后还要对原始数据进行处理，下面我们来回顾一下：

### 实验目的

1. 理解 STM32F4 的 ADC 工作原理和配置。
2. 学会使用 ADC 采集模拟电压信号。
3. 通过 LED 或串口输出转换结果，观察 ADC 的工作。

### 硬件连接

1. **ADC 输入**：
   
   - 将可调电源或电位器的输出连接到 STM32F4 开发板的某个 ADC 输入引脚，例如 PA0。

2. **LED 输出**（可选）：
   
   - 将 LED 的长脚（阳极）连接到 STM32F4 的一个 GPIO 引脚（如 PC13），短脚（阴极）连接到地（GND）。
   - 在 LED 和 VCC 之间串联一个限流电阻（通常为 220Ω）。

### 主要代码示例：

```c
#include "main.h"

// 定义 ADC 句柄，负责管理 ADC 外设
ADC_HandleTypeDef hadc1;
// 定义 USART 句柄，用于串口通信
UART_HandleTypeDef huart1;
// 函数声明
void SystemClock_Config(void);   // 系统时钟配置
static void MX_GPIO_Init(void);  // GPIO 初始化
static void MX_ADC1_Init(void);  // ADC1 初始化
static void MX_USART1_UART_Init(void);  // USART1 初始化
int main(void) {
    // 初始化 HAL 库
    HAL_Init();
    // 配置系统时钟
    SystemClock_Config();
    // 初始化 GPIO、ADC 和 USART 外设
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    // 启动 ADC 转换
    HAL_ADC_Start(&hadc1);
    while (1) {
        // 启动 ADC 转换，并等待转换完成
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        // 读取 ADC 转换值
        uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
        // 将 ADC 值转换为电压值
        // 假设参考电压 Vref = 3.3V，12 位分辨率（4095）
        float voltage = adcValue * (3.3f / 4095.0f);  // 转换公式
        // 将 ADC 值和电压值发送到串口，准备输出
        char buffer[50];  // 缓冲区，用于存储输出字符串
        snprintf(buffer, sizeof(buffer), "ADC Value: %lu, Voltage: %.2f V\r\n", adcValue, voltage);
        // 通过串口发送数据
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        // 延时 1 秒，控制输出频率
        HAL_Delay(1000);  // 每秒读取一次
    }
}

// 系统时钟配置函数
void SystemClock_Config(void) {
    // 这里可以根据具体应用需要配置系统时钟
    // 例如，可以配置 PLL 时钟、AHB、APB 时钟等
}

// GPIO 初始化函数
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // GPIO 初始化结构体
    // 启用 GPIOC 时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // LED 引脚配置
    GPIO_InitStruct.Pin = GPIO_PIN_13;  // 选择 PC13 引脚（通常是开发板上的板载 LED）
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 设置为推挽输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // 不使用上下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // 设置输出速度
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  // 初始化 GPIO
}

// ADC1 初始化函数
static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};  // ADC 通道配置结构体
    // 启用 ADC1 时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    // 配置 ADC 外设
    hadc1.Instance = ADC1;  // 选择 ADC1 实例
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;  // ADC 时钟预分频
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;  // 设置分辨率为 12 位
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;  // 禁用扫描模式（单通道）
    hadc1.Init.ContinuousConvMode = ENABLE;  // 启用连续转换模式
    hadc1.Init.DiscontinuousConvMode = DISABLE;  // 禁用不连续转换模式
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;  // 软件触发转换
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;  // 数据右对齐
    hadc1.Init.NbrOfConversion = 1;  // 只进行一次转换
    HAL_ADC_Init(&hadc1);  // 初始化 ADC
    // 配置 ADC 通道
    sConfig.Channel = ADC_CHANNEL_0;  // 选择 ADC_CHANNEL_0（PA0）
    sConfig.Rank = ADC_REGULAR_RANK_1;  // 设定通道在转换序列中的顺序
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;  // 设定采样时间
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);  // 配置 ADC 通道
}

// USART1 初始化函数
static void MX_USART1_UART_Init(void) {
    // USART 配置（波特率 115200，8位数据位，无校验位，1位停止位）
    huart1.Instance = USART1;  // 选择 USART1 实例
    huart1.Init.BaudRate = 115200;  // 设置波特率
    huart1.Init.WordLength = UART_WORDLENGTH_8B;  // 8 位数据位
    huart1.Init.StopBits = UART_STOPBITS_1;  // 1 位停止位
    huart1.Init.Parity = UART_PARITY_NONE;  // 无校验位
    huart1.Init.Mode = UART_MODE_TX_RX;  // 发送和接收模式
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // 不使用硬件流控制
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;  // 16 倍过采样
    HAL_UART_Init(&huart1);  // 初始化 USART
}
```

代码注释说明

- **函数声明部分**：包括系统时钟、GPIO、ADC 和 USART 的初始化函数声明。
- **`main` 函数**：主循环中启动 ADC，定期读取 ADC 值并将其通过串口输出。
  - 使用 `HAL_ADC_PollForConversion` 函数等待 ADC 转换完成，然后读取转换结果。
  - 通过公式将 ADC 读取的值转换为电压值。
  - 使用 `snprintf` 函数将 ADC 值和电压值格式化为字符串，发送到串口。
- **`SystemClock_Config` 函数**：可以根据需要配置系统时钟，以确保 STM32F4 以预期的速度运行。
- **`MX_GPIO_Init` 函数**：设置 GPIO 引脚的工作模式和属性，准备好 LED 引脚。
- **`MX_ADC1_Init` 函数**：配置 ADC 的工作参数，设定分辨率、触发方式和通道配置。
- **`MX_USART1_UART_Init` 函数**：配置串口通信的基本参数，包括波特率、数据位、停止位等。






