# 第二十二章 PWM DAC实验

## 1. 硬件设计

本章用到的硬件资源有：

- 指示灯DS0

- KEY_UP和KEY1按键

- 串口

- TFTLCD模块

- ADC

- PWM DAC

本章，我们使用 STM32F4 的 TIM9_CH2(PA3)输出 PWM，经过二阶 RC 滤波后，转换为直流输出，实现 PWM DAC。同上一章一样，我们通过 ADC1 的通道 5（PA5）读取 PWM DAC 的输出，并在 LCD 模块上显示相关数值，通过按键和 USMART 控制 PWM DAC 的输出值。我们需要用到 ADC 采集 DAC 的输出电压，所以需要在硬件上将 PWM DAC 和 ADC短接起来， PWM DAC 部分原理图如图：

![屏幕截图 2024-10-16 220544.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-22-05-55-屏幕截图%202024-10-16%20220544.png)

## 2. 软件设计

### 2.1 初始化ADC

```c
ADC_HandleTypeDef ADC1_Handler; // ADC句柄

// 初始化ADC
// ch: ADC_channels 
// 通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_12B;             //12位模式
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    ADC1_Handler.Init.ScanConvMode=DISABLE;                      //非扫描模式
    ADC1_Handler.Init.EOCSelection=DISABLE;                      //关闭EOC中断
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion=1;                         //1个转换在规则序列中 也就是只转换规则序列1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
    ADC1_Handler.Init.DMAContinuousRequests=DISABLE;             //关闭DMA请求
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化 
}
// ADC底层驱动，引脚配置，时钟使能
// 此函数会被HAL_ADC_Init()调用
// hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();            //使能ADC1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();            //开启GPIOA时钟

    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
```

### 2.2 获取ADC值

```c
// 获得ADC值
// ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
// 返回值:转换结果
u16 Get_Adc(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;

    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        //采样时间
    ADC1_ChanConf.Offset=0;                 
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //通道配置

    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC

    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //轮询转换

    return (u16)HAL_ADC_GetValue(&ADC1_Handler);            //返回最近一次ADC1规则组的转换结果
}
// 获取指定通道的转换值，取times次,然后平均 
// times:获取次数
// 返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u32 ch,u8 times)
{
    u32 temp_val=0;
    u8 t;
    for(t=0;t<times;t++)
    {
        temp_val+=Get_Adc(ch);
        delay_ms(5);
    }
    return temp_val/times;
} 
```

### 2.3 TIM初始化

```c
// PWM DAC初始化(也就是TIM9通道2初始化)
// PWM输出初始化
// arr：自动重装值
// psc：时钟预分频数
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{ 
    TIM9_Handler.Instance = TIM9;                      // 定时器9
    TIM9_Handler.Init.Prescaler = psc;                 // 定时器分频
    TIM9_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;// 向上计数模式
    TIM9_Handler.Init.Period = arr;                    // 自动重装载值
    TIM9_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频因子
    HAL_TIM_PWM_Init(&TIM9_Handler);                   // 初始化PWM
    TIM9_CH2Handler.OCMode = TIM_OCMODE_PWM1; // 模式选择PWM1
    TIM9_CH2Handler.Pulse = arr/2;            // 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM9_CH2Handler.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出比较极性为高
    HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH2Handler,TIM_CHANNEL_2); // 配置TIM9通道2
    HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);//开启PWM通道2
}

//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM9_CLK_ENABLE();          // 使能定时器9
    __HAL_RCC_GPIOA_CLK_ENABLE();          // 开启GPIOA时钟
    GPIO_Initure.Pin=GPIO_PIN_3;          // PA3
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;    // 复用推完输出
    GPIO_Initure.Pull=GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;   // 高速
    GPIO_Initure.Alternate= GPIO_AF3_TIM9;// PA3复用为TIM9_CH2
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
```

### 2.4 设置占空比

```c
// 设置TIM通道2的占空比
// TIM_TypeDef:定时器
// compare:比较值
void TIM_SetTIM9Compare2(u32 compare)
{
    TIM9->CCR2 = compare;
}
```

### 2.5 主函数

```c
// 设置输出电压
// vol:0~330,代表0~3.3V
void PWM_DAC_Set(u16 vol)
{
    double temp=vol;
    temp/=100;
    temp=temp*256/3.3;
    TIM_SetTIM9Compare2(temp);       
}

int main(void)
{
    u16 adcx;
    float temp;
    u8 t=0;     
    u16 pwmval=0;
    u8 key;

    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
    delay_init(168);            // 初始化延时函数
    uart_init(115200);          // 初始化USART
    usmart_dev.init(84);         // 初始化USMART
    LED_Init();                     // 初始化LED    
    KEY_Init();                    // 初始化KEY
     LCD_Init();                   // 初始化LCD
    MY_ADC_Init();              // 初始化ADC1
    TIM9_CH2_PWM_Init(255,1);   // TIM9 PWM初始化, Fpwm=84M/256=328.125Khz.
    POINT_COLOR=RED; 
    LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");    
    LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");     
    LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");      
    POINT_COLOR=BLUE;//设置字体为蓝色           
    LCD_ShowString(30,150,200,16,16,"DAC VAL:");          
    LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");          
    LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");     
    TIM_SetTIM9Compare2(pwmval);    // 初始值为0     
    while(1)
    {
        t++;
        key=KEY_Scan(0);              
        if(key==WKUP_PRES) // WKUP按下
        {         
            if(pwmval<250)pwmval+=10;   // 增加10
            TIM_SetTIM9Compare2(pwmval);// 输出        
        }else if(key==KEY1_PRES) // KEY1按下
        {
            if(pwmval>10)pwmval-=10; // 减少10
            else pwmval=0;
            TIM_SetTIM9Compare2(pwmval); // 输出    
        }     
        if(t==10||key==KEY1_PRES||key==WKUP_PRES) // WKUP/KEY1按下了,或者定时时间到了
        {      
            adcx=HAL_TIM_ReadCapturedValue(&TIM9_Handler,TIM_CHANNEL_2);
            LCD_ShowxNum(94,150,adcx,3,16,0);  // 显示DAC寄存器值
            temp=(float)adcx*(3.3/256);;       // 得到DAC电压值
            adcx=temp;
            LCD_ShowxNum(94,170,temp,1,16,0);  // 显示电压值整数部分
            temp-=adcx;
            temp*=1000;
            LCD_ShowxNum(110,170,temp,3,16,0x80);     // 显示电压值的小数部分
            adcx=Get_Adc_Average(ADC_CHANNEL_5,20); // 得到ADC转换值      
            temp=(float)adcx*(3.3/4096);            // 得到ADC电压值
            adcx=temp;
            LCD_ShowxNum(94,190,temp,1,16,0);         // 显示电压值整数部分
            temp-=adcx;
            temp*=1000;
            LCD_ShowxNum(110,190,temp,3,16,0x80);     // 显示电压值的小数部分
            t=0;
            LED0=!LED0;       
         }        
        delay_ms(10); 
    }
}
```

此部分代码， 同上一章的基本一样，先对需要用到的模块进行初始化，然后显示一些提示信息，本章我们通过 KEY_UP 和 KEY1（也就是上下键）来实现对 PWM 脉宽的控制，经过 RC 滤波，最终实现对 DAC 输出幅值的控制。按下 KEY_UP 增加，按 KEY1 减小。同时在 LCD 上面显示 TIM4_CCR1 寄存器的值、 PWM DAC 设计输出电压以及 ADC 采集到的实际输出电压。同时 DS0 闪烁，提示程序运行状况。

同时伴随 DS0 的不停闪烁，提示程序在运行。 此时，我们通过按 KEY_UP 按键，可以看到输出电压增大，按 KEY1 则变小。

## 3. 小结

### 硬件连接

1. **PWM 输出**：将 PWM 输出引脚连接到负载（如 LED 或电压表）。假设 PWM 使用 `PA0` 引脚。
2. **ADC 输入**：将 ADC 输入引脚连接到负载的相同点（即负载的正极），假设使用 `PA1` 引脚。
3. **电源**：确保 STM32 微控制器供电（通常为 3.3V）。

### 代码示例

以下代码演示如何设置 TIM 输出 PWM 信号并通过 ADC 采集电压：

```c
#include "main.h"

// 定义常量
#define PWM_FREQUENCY 1000 // PWM 频率为 1kHz
#define ADC_CHANNEL ADC_CHANNEL_1 // ADC 通道

TIM_HandleTypeDef htim2; // TIM 句柄
ADC_HandleTypeDef hadc1; // ADC 句柄

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);

int main(void) {
    // 初始化 HAL 库
    HAL_Init();
    // 配置系统时钟
    SystemClock_Config();
    // 初始化 GPIO、TIM 和 ADC
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_ADC1_Init();
    // 启动 PWM 输出
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    // 启动 ADC
    HAL_ADC_Start(&hadc1);
    uint32_t adcValue;
    uint32_t pwmValue = 0; // PWM 初始占空比
    while (1) {
        // 设置 PWM 占空比 (0-1000)
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwmValue);
        // 读取 ADC 值
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); // 等待 ADC 转换完成
        adcValue = HAL_ADC_GetValue(&hadc1); // 获取 ADC 值
        // 处理 ADC 值
        // 这里可以添加代码来处理 adcValue，例如显示或存储
        // 简单的 PWM 调整，增加占空比
        pwmValue += 10; // 每次增加10，调整步长可自行设置
        if (pwmValue > 1000) {
            pwmValue = 0; // 重置占空比
        }
        HAL_Delay(100); // 100毫秒的延时
    }
}

// TIM2 初始化函数
static void MX_TIM2_Init(void) {
    __HAL_RCC_TIM2_CLK_ENABLE(); // 启用 TIM2 时钟

    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83; // 84MHz / 84 = 1MHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1; // 1kHz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2); // 初始化基础 TIM

    // PWM 配置
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0; // 初始占空比为0
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1); // 配置通道
}

// ADC 初始化函数
static void MX_ADC1_Init(void) {
    __HAL_RCC_ADC1_CLK_ENABLE(); // 启用 ADC1 时钟

    ADC_ChannelConfTypeDef sConfig = {0};
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; // 选择 ADC 时钟
    hadc1.Init.Resolution = ADC_RESOLUTION_12B; // 设置分辨率
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE; // 单通道模式
    hadc1.Init.ContinuousConvMode = ENABLE; // 连续转换模式
    hadc1.Init.DiscontinuousConvMode = DISABLE; // 禁用不连续模式
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // 禁用外部触发
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软件启动
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据对齐
    hadc1.Init.NbrOfConversion = 1; // 单通道
    HAL_ADC_Init(&hadc1); // 初始化 ADC

    // 配置 ADC 通道
    sConfig.Channel = ADC_CHANNEL; // 设置通道
    sConfig.Rank = ADC_REGULAR_RANK_1; // 常规通道
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; // 采样时间
    HAL_ADC_ConfigChannel(&hadc1, &sConfig); // 配置通道
}

// GPIO 初始化函数
static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 启用 GPIOA 时钟

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0; // PWM 输出引脚
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉或下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 低速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // 初始化 PWM 引脚

    GPIO_InitStruct.Pin = GPIO_PIN_1; // ADC 输入引脚
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; // 模拟模式
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉或下拉电阻
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // 初始化 ADC 引脚
}

// 系统时钟配置函数
void SystemClock_Config(void) {
    // 配置系统时钟，具体配置根据你的开发板和时钟要求
}
```
