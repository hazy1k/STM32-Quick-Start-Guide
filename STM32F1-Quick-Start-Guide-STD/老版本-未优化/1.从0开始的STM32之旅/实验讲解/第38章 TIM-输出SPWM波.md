# 第三十八章 TIM-输出SPWM波

## 1. 硬件设计

与全彩呼吸灯一样，只是使用的PWM表不同

## 2. 软件设计

### 2.1 编程大纲

1. TIM及通道相关参数宏定义

2. PWM表定义

3. TIM模式配置

4. 定时器中断服务函数

5. 主函数测试

### 2.2 代码分析

#### 2.2.1 TIM及通道相关参数宏定义

```c
#define AMPLITUDE_CLASS 256 // 电压幅值等级数

/* TIM相关参数宏定义 */
#define TIMx TIM3
#define TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define TIM_CLK           RCC_APB1Periph_TIM3
#define TIM_GPIO_CLK      (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
#define GPIO_REMAP_FUN()  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                 

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
#define BLUE_TIM_OCxInit          TIM_OC4Init            
#define BLUE_TIM_OCxPreloadConfig TIM_OC4PreloadConfig  
#define BLUE_CCRx CCR4
```

#### 2.2.2 SPWM表定义

```c
// SPWM表,正弦曲线，此表使用工程目录下的python脚本sin_wave.py生成
const uint16_t indexWave[] = {
0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 89, 98,
    107, 116, 125, 133, 142, 151, 159, 168, 176,
    184, 193, 201, 209, 218, 226, 234, 242, 249,
    257, 265, 273, 280, 288, 295, 302, 310, 317, 
    324, 331, 337, 344, 351, 357, 364, 370, 376, 
    382, 388, 394, 399, 405, 410, 416, 421, 426, 
    431, 436, 440, 445, 449, 454, 458, 462, 465, 
    469, 473, 476, 479, 482, 485, 488, 491, 493, 
    496, 498, 500, 502, 503, 505, 506, 508, 509, 
    510, 510, 511, 512, 512, 512, 512, 512, 512,
    511, 510, 510, 509, 508, 506, 505, 503, 502,
    500, 498, 496, 493, 491, 488, 485, 482, 479,
    476, 473, 469, 465, 462, 458, 454, 449, 445, 
    440, 436, 431, 426, 421, 416, 410, 405, 399, 
    394, 388, 382, 376, 370, 364, 357, 351, 344, 
    337, 331, 324,     317, 310, 302, 295, 288, 280, 
    273, 265, 257, 249, 242, 234, 226, 218, 209, 
    201, 193, 184, 176, 168, 159, 151, 142, 133, 
125, 116, 107, 98, 89, 81, 72, 63, 54, 45, 36,
27, 18, 9, 0
};
```

#### 2.2.3 TIM模式配置

```c
static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
      NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQ;
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
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
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
    // 时基结构初始化
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = (512-1);                                    
    TIM_TimeBaseStructure.TIM_Prescaler = (10-1);                                    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
    // PWM模式初始化
      TIM_OCInitTypeDef TIM_OCInitStructure;                                                                                
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                        
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
    TIM_OCInitStructure.TIM_Pulse = 0;                                                           
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        
      RED_TIM_OCxInit(TIMx, &TIM_OCInitStructure );    
    RED_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable);    
      GREEN_TIM_OCxInit(TIMx, &TIM_OCInitStructure);    
      GREEN_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable);    
      BLUE_TIM_OCxInit(TIMx, &TIM_OCInitStructure);    
      BLUE_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable);                            
      TIM_ARRPreloadConfig(TIMx, ENABLE); // 使能TIM重载寄存器ARR
    TIM_Cmd(TIMx, ENABLE);                                                           
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);                                        
}

void SPWM_Init(void)
{
    TIM_NVIC_Init();
    TIM_GPIO_Init();
    TIM_Mode_Init();
}
```

#### 2.2.4 定时器中断服务函数

```c
void TIMx_IRQHandler(void)
{    
    static uint16_t pwm_index = 0;      // 用于PWM查表
    static uint16_t period_cnt = 0;        // 用于计算周期数
    static uint16_t amplitude_cnt = 0;// 用于计算幅值等级
    if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET)
     {        
        amplitude_cnt++;
        if(amplitude_cnt > (AMPLITUDE_CLASS-1))                                 
        {        
            period_cnt++;
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
      if(((rgb_color&0xFF0000)>>16) >= amplitude_cnt)        
      {        
              TIMx->RED_CCRx = indexWave[pwm_index]; // 根据PWM表修改定时器的比较寄存器值
      }
            else
      {
                TIMx->RED_CCRx = 0;    // 比较寄存器值为0，通道输出高电平，该通道LED灯灭
      }
            if(((rgb_color&0x00FF00)>>8) >= amplitude_cnt)
      {                
                TIMx->GREEN_CCRx = indexWave[pwm_index];    
      }
            else
      {
                TIMx->GREEN_CCRx = 0;
      }    
            if((rgb_color&0x0000FF) >= amplitude_cnt)
      {                
                TIMx->BLUE_CCRx = indexWave[pwm_index];
      }    
            else
      {
                TIMx->BLUE_CCRx = 0;    
      }    
        }                    
    TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
    }
}
```

#### 2.2.5 主函数测试

```c
#include "stm32f10x.h"
#include "spwm.h"

__IO uint32_t rgb_color = 0x008080;

int main(void)
{            
    SPWM_Init();
    while(1)
    {

    }        
}
```

## 3. 小结

照例，我们简单回顾一下：

### 3.1 实验步骤：

1. **硬件连接**：
   
   - 连接红、绿、蓝LED到PWM输出引脚。

2. **生成SPWM信号**：
   
   - 为每个颜色生成对应的正弦波PWM信号。

3. **循环更新占空比**：
   
   - 在主循环中，根据时间改变LED的占空比，混合不同颜色。

### 3.2 代码示例：

```c
#include "stm32f10x.h"
#include <math.h>

#define PERIOD 1000
#define SINE_RESOLUTION 360

uint32_t sine_wave[SINE_RESOLUTION];

void Generate_Sine_Wave(void) {
    for (int i = 0; i < SINE_RESOLUTION; i++) {
        sine_wave[i] = (uint32_t)((sin(i * 3.14159 / 180) + 1) * (PERIOD / 2));
    }
}

void TIM_Config(void) {
    // 配置定时器代码，与之前示例类似
}

void SetPWM_Pulse(TIM_TypeDef* TIMx, uint32_t pulse) {
    if (TIMx == TIM1) {
        TIM1->CCR1 = pulse; // 红色
    } else if (TIMx == TIM2) {
        TIM2->CCR1 = pulse; // 绿色
    } else if (TIMx == TIM3) {
        TIM3->CCR1 = pulse; // 蓝色
    }
}

int main(void) {
    SystemInit();
    Generate_Sine_Wave();
    TIM_Config();

    while (1) {
        for (int i = 0; i < SINE_RESOLUTION; i++) {
            SetPWM_Pulse(TIM1, sine_wave[i]); // 红色
            SetPWM_Pulse(TIM2, sine_wave[(i + 120) % SINE_RESOLUTION]); // 绿色
            SetPWM_Pulse(TIM3, sine_wave[(i + 240) % SINE_RESOLUTION]); // 蓝色
            for (volatile uint32_t j = 0; j < 1000; j++);
        }
    }
}
```

### 3.3 代码说明：

1. **硬件配置**：确保每个LED连接到不同的PWM通道。
2. **波形生成**：通过不同的偏移量（如120度、240度）生成彩色混合效果。
3. **循环控制**：在主循环中依次更新三个LED的占空比，以形成彩灯效果。

---

2024.9.18 第一次修订，后期不再维护

2025.1.29 优化代码
