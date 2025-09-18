# 第三十七章 TIM-全彩呼吸灯

## 1. 硬件设计

与上一章一样

## 2. 软件设计

### 2.1 编程大纲

1. TIM相关参数宏定义

2. 在单色呼吸灯的基础上，增加PWM输出通道，三个通道分别控制红绿蓝颜色

3. 编写中断服务函数，增加对拟合波形幅值的控制

4. 计算获取新的PWM数据表

5. 主函数测试

### 2.2 代码分析

#### 2.2.1 TIM及通道相关参数宏定义

```c
// 电压幅值等级数
#define AMPLITUDE_CLASS 256
/* TIM相关参数定义 */
#define TIMx TIM3
#define TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define TIM_CLK           RCC_APB1Periph_TIM3
#define TIM_GPIO_CLK      (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
#define GPIO_REMAP_FUN()  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);     
/* 中断相关配置 */
#define TIMx_IRQn       TIM3_IRQn             
#define TIMx_IRQHandler TIM3_IRQHandler

/* 三色灯通道定义 */
#define RED_TIM_LED_PORT GPIOB
#define RED_TIM_LED_PIN  GPIO_Pin_5
#define RED_TIM_OCxInit          TIM_OC2Init          
#define RED_TIM_OCxPreloadConfig TIM_OC2PreloadConfig 
#define RED_CCRx CCR2        
#define GREEN_TIM_LED_PORT GPIOB
#define GREEN_TIM_LED_PIN  GPIO_Pin_0
#define GREEN_TIM_OCxInit          TIM_OC3Init          
#define GREEN_TIM_OCxPreloadConfig TIM_OC3PreloadConfig 
#define GREEN_CCRx CCR3
#define BLUE_TIM_LED_PORT GPIOB
#define BLUE_TIM_LED_PIN  GPIO_Pin_1
#define BLUE_TIM_OCxInit           TIM_OC4Init           
#define BLUE_TIM_OCxPreloadConfig  TIM_OC4PreloadConfig  
#define BLUE_CCRx CCR4
```

#### 2.2.2 定义PWM表

```c
// 控制输出波形的频率
__IO uint16_t period_class = 1;
// PWM表
const uint16_t indexWave[] = {
1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 
    2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
    5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
    11, 12, 12, 13, 14, 15, 17, 18,
    19, 20, 22, 23, 25, 27, 29, 31, 
    33, 36, 38, 41, 44, 47, 51, 54, 
    58, 63, 67, 72, 77, 83, 89, 95, 
    102, 110, 117, 126, 135, 145, 156, 
    167, 179, 192, 206, 221, 237, 254,
    272, 292, 313, 336, 361, 387, 415,
    445, 477, 512, 512, 477, 445, 415, 
    387, 361, 336, 313, 292, 272, 254, 
    237, 221, 206, 192, 179, 167, 156, 
    145, 135, 126, 117, 110, 102, 95, 
    89, 83, 77, 72, 67, 63, 58, 54, 51, 
    47, 44, 41, 38, 36, 33, 31, 29, 27,
    25, 23, 22, 20, 19, 18, 17, 15, 14,
    13, 12, 12, 11, 10, 9, 9, 8, 8, 7, 7,
    6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 
    3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1
};

uint16_t PWM_Num = sizeof(indexWave)/sizeof(indexWave[0]); // PWM元素个数 
```

代码中列出的PWM表内元素的最大值均为512，元素个数均为128， 把两个表绘制成成曲线如图：呼吸曲线和正弦曲线

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPWM010.jpeg)

#### 2.2.3 呼吸灯初始化

```c
static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
    NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM_GPIO_Init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(TIM_GPIO_CLK, ENABLE);
    GPIO_REMAP_FUN();
    GPIO_InitStructure.GPIO_Pin = RED_TIM_LED_PIN ;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RED_TIM_LED_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GREEN_TIM_LED_PIN ;
    GPIO_Init(GREEN_TIM_LED_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = BLUE_TIM_LED_PIN ;
    GPIO_Init(BLUE_TIM_LED_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Init(void)
{
    TIM_APBxClock_FUN(TIM_CLK, ENABLE); 
    /* 时基结构初始化 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = (512-1);                
    TIM_TimeBaseStructure.TIM_Prescaler = (10-1);                 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;          
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
    /* PWM输出通道初始化 */
    TIM_OCInitTypeDef TIM_OCInitStructure;    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
    TIM_OCInitStructure.TIM_Pulse = 0;                             
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      
    /* RGB三个通道输出初始化 */
    RED_TIM_OCxInit(TIMx, &TIM_OCInitStructure );    
    RED_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); 
    GREEN_TIM_OCxInit(TIMx, &TIM_OCInitStructure);    
    GREEN_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); 
    BLUE_TIM_OCxInit(TIMx, &TIM_OCInitStructure);    
      BLUE_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); 
    TIM_ARRPreloadConfig(TIMx, ENABLE);    
    TIM_Cmd(TIMx, ENABLE);     
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);        
}

void breathing_Init(void)
{
    TIM_NVIC_Init();
    TIM_GPIO_Init();
    TIM_Mode_Init();    
}
```

本配置中同时使能三个PWM通道，而定时器初始化中的TIM_Period成员被配置为(512-1)，即PWM表元素的极大值，TIM_Prescaler被配置为(10-1)， 相对于单色呼吸灯实验，提高了定时器的时钟频率。

#### 2.2.4 定时器中断服务函数

```c
void TIMx_IRQHandler(void)
{	
	static uint16_t pwm_index = 0;		// 用于PWM查表
	static uint16_t period_cnt = 0;		// 用于计算周期数
	static uint16_t amplitude_cnt = 0;// 用于计算幅值等级
	if(TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET)
 	{		
		amplitude_cnt++;	
		// 每个PWM表中的每个元素有AMPLITUDE_CLASS个等级，
		// 每增加一级多输出一次脉冲，即PWM表中的元素多使用一次
		// 使用256次，根据RGB颜色分量设置通道输出
		if(amplitude_cnt > (AMPLITUDE_CLASS-1))		 						
		{		
			period_cnt++;
			//每个PWM表中的每个元素使用period_class次
			if(period_cnt > period_class)
			{				
				pwm_index++;							
				if(pwm_index >= PWM_Num)			
				{
					pwm_index=0;								
				}	
				period_cnt = 0;											
			}	
			amplitude_cnt=0;											
		}
		else
		{	
      //根据RGB颜色分量值，设置各个通道是否输出当前的PWM表元素表示的亮度
		  //红
			if(((rgb_color&0xFF0000)>>16) >= amplitude_cnt)				
			  TIMx->RED_CCRx = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
			else
				TIMx->RED_CCRx = 0; // 比较寄存器值为0，通道输出高电平，该通道LED灯灭
			if(((rgb_color&0x00FF00)>>8) >= amplitude_cnt)				
				TIMx->GREEN_CCRx = indexWave[pwm_index];	
			else
				TIMx->GREEN_CCRx = 0;	
			if((rgb_color&0x0000FF) >= amplitude_cnt)				
				TIMx->BLUE_CCRx = indexWave[pwm_index];	
			else
				TIMx->BLUE_CCRx = 0;		
			}					
	TIM_ClearITPendingBit(TIMx, TIM_IT_Update);	
	}
}
```

#### 2.2.5 主函数

```c
#include "stm32f10x.h"
#include "breathing.h"

// 定义RGB灯的颜色变量
__IO uint32_t rgb_color = 0x00FFFF;

int main(void)
{			
	breathing_Init();
	while(1)
	{
	}		
}

```

## 3. 小结

本章与上面一章单色呼吸灯大同小异，就是多加两个通道，用来进行混色，下面我们来复盘一下：

### 3.1 实验步骤：

1. **硬件连接**：
   
   - 连接LED的红、绿、蓝引脚到STM32F103的PWM输出引脚（如TIM1、TIM2等）。
   - 通过适当的电阻限流。

2. **配置时钟**：
   
   - 配置系统时钟，确保TIM的时钟频率正确。

3. **配置定时器（TIM）**：
   
   - 设置TIM为PWM模式。
   - 配置TIM的预分频器和自动重装载寄存器（ARR）以获得所需的PWM频率。
   - 配置通道1、2和3来控制红、绿、蓝LED。

4. **编写代码**：
   
   - 初始化TIM。
   - 设置PWM输出。
   - 实现呼吸灯效果的逻辑，通过调整PWM占空比来实现渐变效果。

### 3.2 代码详解：

```c
#include "stm32f10x.h"

// 定义周期和预分频器的值
#define PERIOD  1000
#define PRESCALER  72

// 初始化TIM
void TIM_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    TIM_TimeBaseInitStruct.TIM_Prescaler = PRESCALER - 1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = PERIOD - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = PERIOD / 2; // 初始占空比50%
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM1, &TIM_OCInitStruct);
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);
    TIM_OC3Init(TIM1, &TIM_OCInitStruct);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

// 设置PWM占空比
void SetPWM_Pulse(uint32_t channel, uint32_t pulse) {
    if (channel == TIM_Channel_1) TIM1->CCR1 = pulse;
    if (channel == TIM_Channel_2) TIM1->CCR2 = pulse;
    if (channel == TIM_Channel_3) TIM1->CCR3 = pulse;
}

// 呼吸灯效果
void BreathingLight(void) {
    uint32_t pulse;
    while (1) {
        for (pulse = 0; pulse < PERIOD; pulse++) {
            SetPWM_Pulse(TIM_Channel_1, pulse);
            SetPWM_Pulse(TIM_Channel_2, PERIOD - pulse);
            SetPWM_Pulse(TIM_Channel_3, (pulse + PERIOD / 2) % PERIOD); // 简单变化的蓝色
            Delay(10); // 延时函数，根据需要调整
        }
    }
}

// 简单延时函数
void Delay(uint32_t time) {
    for (volatile uint32_t i = 0; i < time * 1000; i++);
}

int main(void) {
    SystemInit();
    TIM_Config();
    BreathingLight();
    while (1);
}
```

### 3.3 代码说明：

- **`TIM_Config`**: 配置定时器TIM1为PWM模式，设置预分频器和周期。`TIM_OCInitStruct` 用于配置PWM的占空比和极性。
- **`SetPWM_Pulse`**: 调节指定通道的PWM占空比。
- **`BreathingLight`**: 实现呼吸灯效果，通过调整PWM占空比来使LED颜色变化。
- **`Delay`**: 一个简单的延时函数，用于调整亮度变化的速度。

---

2024.9.18 第一次修订，后期不再维护

2025.1.28 优化代码
