# 第二十一章 DAC实验

## 1. 硬件设计

本章使用的硬件资源有：

- 指示灯DS0

- KEY_UP和KEY1按键

- 串口

- TFTLCD模块

- ADC

- DAC

本章，我们使用 DAC 通道 1 输出模拟电压，然后通过 ADC1 的通道 1 对该输出电压进行读取，并显示在 LCD 模块上面， DAC 的输出电压，我们通过按键（或 USMART）进行设置。

我们需要用到 ADC 采集 DAC 的输出电压，所以需要在硬件上把他们短接起来。 ADC和 DAC 的连接原理图如图：

![屏幕截图 2024-10-16 090156.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-09-02-00-屏幕截图%202024-10-16%20090156.png)

P12 是多功能端口，我们只需要通过跳线帽短接 P14 的 ADC 和 DAC，就可以开始做本章实验了。

![屏幕截图 2024-10-16 090214.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-09-02-17-屏幕截图%202024-10-16%20090214.png)

## 2. 软件设计

### 2.1 初始化DAC

```c
DAC_HandleTypeDef DAC1_Handler; // DAC句柄
// 初始化DAC
void DAC1_Init(void)
{
    DAC_ChannelConfTypeDef DACCH1_Config; 
    DAC1_Handler.Instance = DAC;
    HAL_DAC_Init(&DAC1_Handler); // 初始化DAC
    DACCH1_Config.DAC_Trigger = DAC_TRIGGER_NONE;             // 不使用触发功能
    DACCH1_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;// DAC1输出缓冲关闭
    HAL_DAC_ConfigChannel(&DAC1_Handler,&DACCH1_Config,DAC_CHANNEL_1);// DAC通道1配置
    HAL_DAC_Start(&DAC1_Handler,DAC_CHANNEL_1);  // 开启DAC通道1
}
```

### 2.2 DAC GPIO配置

```c
// DAC底层驱动，时钟配置，引脚 配置
// 此函数会被HAL_DAC_Init()调用
// hdac:DAC句柄
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_DAC_CLK_ENABLE();          // 使能DAC时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();		 // 开启GPIOA时钟	
    GPIO_Initure.Pin = GPIO_PIN_4;       // PA4
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;// 模拟
    GPIO_Initure.Pull = GPIO_NOPULL;     // 不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
```

### 2.3 设置DAC输出电压

```c
// 设置通道1输出电压
// vol:0~3300,代表0~3.3V
void DAC1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
    HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);//12位右对齐数据格式设置DAC值
}
```

### 2.4 主函数

```c
int main(void)
{
    u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 dacval=0;
	u8 key;	
	
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	usmart_dev.init(84); 		 // 初始化USMART
	LED_Init();				 	 // 初始化LED	
	KEY_Init();				     // 初始化KEY
 	LCD_Init();           		 // 初始化LCD
	MY_ADC_Init();               // 初始化ADC1
    DAC1_Init();                 // 初始化DAC1
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"DAC TEST");		 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	  
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V"); 	
    HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0); // 初始值为0 	
    while(1)
	{
		t++;
		key = KEY_Scan(0);			  
		if(key == WKUP_PRES) // WKUP按下了
		{		 
			if(dacval<4000)dacval+=200; // 增加200
            HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dacval);//设置DAC值
		}else if(key==2) // KEY1按下了	
		{
			if(dacval>200)dacval-=200; // 减少200
			else dacval=0;
            HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dacval);//设置DAC值
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) // WKUP/KEY1按下了,或者定时时间到了
		{	  
            adcx=HAL_DAC_GetValue(&DAC1_Handler,DAC_CHANNEL_1); // 读取前面设置DAC的值
			LCD_ShowxNum(94,150,adcx,4,16,0);     	            // 显示DAC寄存器值
			temp=(float)adcx*(3.3/4096);			            // 得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	            // 显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0X80); 	            // 显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC_CHANNEL_5,10);             // 得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			            // 得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	   			// 显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0X80); 	    		// 显示电压值的小数部分
			LED0=!LED0;	   
			t=0;
		}	    
		delay_ms(10);	
	}
}
```

此部分代码，我们先对需要用到的模块进行初始化，然后显示一些提示信息，本章我们通过 KEY_UP（WKUP 按键）和 KEY1（也就是上下键）来实现对 DAC 输出的幅值控制。按下 KEY_UP 增加，按 KEY1 减小。同时在 LCD 上面显示 DHR12R1 寄存器的值、 DAC设计输出电压以及 ADC 采集到的 DAC 输出电压。

## 3. 小结

DAC配置比ADC简单许多，下面我们来回顾一下：

### 硬件连接

1. **DAC 引脚**：将 DAC 输出引脚（如 `PA4`）连接到示波器或电压表，以监测输出电压。
2. **按键**：连接一个按键到 GPIO 引脚（如 `PC13`），并在按键和地之间连接一个上拉电阻（通常为 10kΩ）。
3. **电源**：确保 STM32 微控制器供电（通常为 3.3V）。

### 代码示例

```c
#include "main.h"  // STM32 的主头文件

// DAC 句柄声明
DAC_HandleTypeDef hdac;
// 定义常量
#define DAC_MAX_VALUE 4095  // 12位DAC的最大值
#define NUM_VOLTAGES 5      // 可输出电压的数量
// 输出电压数组
uint32_t voltages[NUM_VOLTAGES] = {
    0,          // 0V
    DAC_MAX_VALUE / 4,  // 0.825V
    DAC_MAX_VALUE / 2,  // 1.65V
    3 * DAC_MAX_VALUE / 4,  // 2.475V
    DAC_MAX_VALUE // 3.3V
};
uint8_t currentVoltageIndex = 0; // 当前电压索引
// 函数原型声明
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DAC_Init(void);

int main(void) {
    // 初始化 HAL 库
    HAL_Init();
    // 配置系统时钟
    SystemClock_Config();
    // 初始化 GPIO 和 DAC
    MX_GPIO_Init();
    MX_DAC_Init();
    // 初始化 DAC 输出
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1); // 启动 DAC 通道1
    // 设置初始输出电压
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, voltages[currentVoltageIndex]);
    while (1) {
        // 检测按键是否按下
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) { // 检测到按键按下
            HAL_Delay(200); // 消抖延时
            // 检查按键状态
            if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
                // 更新电压索引
                currentVoltageIndex++;
                if (currentVoltageIndex >= NUM_VOLTAGES) {
                    currentVoltageIndex = 0; // 回到第一个电压
                }
                // 设置新的输出电压
                HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, voltages[currentVoltageIndex]);
            }
        }
        HAL_Delay(50); // 小延时，避免读取过于频繁
    }
}

// DAC 初始化函数
static void MX_DAC_Init(void) {
    __HAL_RCC_DAC12_CLK_ENABLE(); // 启用 DAC12 时钟
    // 配置 DAC 通道
    DAC_ChannelConfTypeDef sConfig = {0};
    hdac.Instance = DAC; // 选择 DAC 实例
    HAL_DAC_Init(&hdac); // 初始化 DAC
    // 配置 DAC 通道
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE; // 设置为无触发模式
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // 启用输出缓冲区
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1); // 配置 DAC 通道1
}

// GPIO 初始化函数
static void MX_GPIO_Init(void) {
    // 启用 GPIOC 时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // 配置按键引脚为输入模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13; // 按键引脚
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // 设置为输入模式
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 启用上拉电阻
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // 初始化按键引脚
}

// 系统时钟配置函数
void SystemClock_Config(void) {
    // 配置系统时钟，具体配置根据你的开发板和时钟要求
}


```


