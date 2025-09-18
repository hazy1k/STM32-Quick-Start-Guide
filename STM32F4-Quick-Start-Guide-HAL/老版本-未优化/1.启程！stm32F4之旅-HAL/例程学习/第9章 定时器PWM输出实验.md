# 第九章 定时器PWM输出实验

## 1. 硬件设计

本实验用到的硬件资源有：

- 指示灯DS0

- 定时器TIM14

这两个我们前面都已经介绍了，因为 TIM14_CH1 可以通过 PF9 输出 PWM，而 DS0 就是直接在 PF9 上面的，所以电路上并没有任何变化。

## 2. 软件设计

### 2.1 编程大纲

1. 定时器TIM14初始化

2. 配置TIM14_CH1为PWM1模式

3. 编写占空比函数

4. 主函数测试（通过改变占空比实现呼吸灯）

### 2.2 代码分析

#### 2.2.1 TIM14_CH1配置PWM模式

```c
TIM_HandleTypeDef TIM14_Handler; // 定时器TIM14句柄
TIM_OC_InitTypeDef TIM14_CH1Handler; // 定时器TIM14通道1句柄

// TIM14 PWM初始化 
void TIM14_PWM_Init(u16 arr, u16 psc)
{  
    TIM14_Handler.Instance = TIM14;          	   
    TIM14_Handler.Init.Prescaler = psc;            
    TIM14_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handler.Init.Period = arr;               
    TIM14_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM14_Handler); 
    TIM14_CH1Handler.OCMode=TIM_OCMODE_PWM1; // 模式选择PWM1
    TIM14_CH1Handler.Pulse = arr/2;          // 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM14_CH1Handler.OCPolarity = TIM_OCPOLARITY_LOW; // 输出比较极性为低 
    HAL_TIM_PWM_ConfigChannel(&TIM14_Handler,&TIM14_CH1Handler,TIM_CHANNEL_1); // 配置TIM14通道1
    HAL_TIM_PWM_Start(&TIM14_Handler,TIM_CHANNEL_1); // 开启PWM通道1
}

// 定时器底层驱动，时钟使能，引脚配置
// 此函数会被HAL_TIM_PWM_Init()调用
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM14_CLK_ENABLE();			
	__HAL_RCC_GPIOF_CLK_ENABLE();			
	GPIO_Initure.Pin = GPIO_PIN_9;          
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;  	
	GPIO_Initure.Pull = GPIO_PULLUP;        
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;   
	GPIO_Initure.Alternate = GPIO_AF9_TIM14; // PF9复用为TIM14_CH1
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}
```

#### 2.2.2 设置占空比函数

```c
// 设置TIM通道4的占空比
void TIM_SetTIM14Compare1(u32 compare)
{
	TIM14->CCR1 = compare; // 设置占空比 
}
```

#### 2.2.3 主函数测试

```c
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"

int main(void)
{
	uint8_t dir = 1; // 1: 正序 0: 逆序
	uint16_t led0_duty = 0; // LED0 PWM输出占空比 
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	// Time = (500*84)/84MHz = 500ms
	// 计数频率为84MHz/psc(84) = 1MHz
	// PWM频率为1MHz/500 = 2KHz
	TIM14_PWM_Init(500-1, 84-1);
	while(1)
	{
		delay_ms(10);
		if(dir)
		{
			led0_duty++; // 正序的时候，增加占空比
		}
		else
		{
			led0_duty--; // 逆序的时候，减少占空比
		}
		if(led0_duty > 300) // 占空比达到峰值，改变方向
		{
			dir = 0;
		}
		if(led0_duty == 0) // 占空比达到谷值，改变方向
		{
			dir = 1;
		}
		TIM_SetTIM14Compare1(led0_duty);
	}
}

```

第一次接触PWM可能对这个主函数不熟悉，下面我们来解释一下：

```c
u8 dir = 1; // 1:递增 0:递减
u16 led0pwmval = 0; // LED0 PWM输出占空比值
```

- `dir`：一个8位无符号整数，用于控制LED的亮度变化方向。1表示亮度逐渐增加，0表示亮度逐渐减少。
- `led0pwmval`：一个16位无符号整数，表示PWM（脉宽调制）信号的占空比，也就是LED的亮度值，初始为0。

```c
while(1)
{
    delay_ms(10);         
    if(dir)
        led0pwmval++; // dir==1 led0pwmval递增
    else 
        led0pwmval--; // dir==0 led0pwmval递减 
    if(led0pwmval > 300)
        dir=0;          // led0pwmval到达300后,方向改为递减
    if(led0pwmval == 0)
        dir=1;          // led0pwmval递减到0后，方向改为递增
    TIM_SetTIM14Compare1(led0pwmval); // 修改比较值，修改占空比
}
```

- **`while(1)`**：进入无限循环，不断执行内部的代码。
- **`delay_ms(10)`**：每次循环延时10毫秒，控制LED亮度更新的频率。
- **`if(dir)`**：检查`dir`的值。如果为1，则表示当前是递增状态，`led0pwmval`增加1；如果为0，则表示当前是递减状态，`led0pwmval`减少1。
- **`if(led0pwmval > 300)`**：当`led0pwmval`大于300时，切换方向为递减（将`dir`设为0）。
- **`if(led0pwmval == 0)`**：当`led0pwmval`减小到0时，切换方向为递增（将`dir`设为1）。
- **`TIM_SetTIM14Compare1(led0pwmval)`**：根据当前的`led0pwmval`值更新PWM信号的占空比，从而调整LED的亮度。

## 3. 小结

实际上我们就是用定时器输出PWM来做了一个呼吸灯嘛，没什么难度，简单回顾一下吧：

```c
#include "stm32f4xx_hal.h" // 根据你的STM32型号选择合适的头文件

TIM_HandleTypeDef htim3; // 定时器句柄

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);

int main(void)
{
    HAL_Init(); // 初始化HAL库
    SystemClock_Config(); // 配置系统时钟
    MX_GPIO_Init(); // 初始化GPIO
    MX_TIM3_Init(); // 初始化定时器

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // 启动PWM输出

    uint16_t pwmValue = 0; // PWM占空比
    uint8_t direction = 1; // 方向：1递增，0递减

    while (1)
    {
        // 调整PWM占空比
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwmValue); 
        // 更新占空比
        if (direction)
        {
            pwmValue += 5; // 增加亮度
            if (pwmValue >= 1000) // 假设最大值为1000
            {
                direction = 0; // 转向减小
            }
        }
        else
        {
            pwmValue -= 5; // 减少亮度
            if (pwmValue == 0)
            {
                direction = 1; // 转向增加
            }
        }
        HAL_Delay(50); // 延时以控制呼吸速度
    }
}

static void MX_TIM3_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 8399; // 预分频，从84MHz变为10kHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000; // 最大计数值
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0; // 初始占空比为0
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE(); // 根据实际连接的引脚使能时钟
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0; // 假设PWM输出连接在PB0
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void SystemClock_Config(void)
{
    // 这里配置系统时钟，省略具体细节
}
```

1. **定时器初始化**：
   
   - `MX_TIM3_Init()`：配置TIM3为PWM模式，设置预分频和计数周期。

2. **PWM输出**：
   
   - `HAL_TIM_PWM_Start()`：启动PWM输出。
   - `__HAL_TIM_SET_COMPARE()`：根据当前的`pwmValue`设置PWM的占空比。

3. **循环控制**：
   
   - 使用`direction`变量控制`pwmValue`的递增和递减，以实现呼吸灯的效果。

---

2024.10.3 第一次修订，后期不再维护

2025.1.14 简化内容，优化代码，去除原本的TIM3中断
