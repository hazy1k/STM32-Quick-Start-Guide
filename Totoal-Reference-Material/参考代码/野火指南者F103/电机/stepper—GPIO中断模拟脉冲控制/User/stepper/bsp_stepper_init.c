#include "./stepper/bsp_stepper_init.h"
#include "./led/bsp_led.h"   
#include "stm32f1xx.h"


TIM_HandleTypeDef TIM_TimeBaseStructure;
/**
  * @brief  通用定时器 TIMx,x[6,7]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
  /* 外设中断配置 */
  HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode       TIMx,x[6,7]没有，其他都有（通用定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(通用定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{

  MOTOR_PUL_CLK_ENABLE();

  TIM_TimeBaseStructure.Instance = MOTOR_PUL_TIM;
  /* 累计 TIM_Period个后产生一个更新或者中断*/    
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.Init.Period = 300-1;  
  // 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
  // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.Init.Prescaler = 84-1;
  // 计数方式
  TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
  // 采样时钟分频
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  // 初始化定时器TIMx, x[2,5] [9,14]
  HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

  // 开启定时器更新中断
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);  
}

/**
  * @brief  初始化通用定时器定时
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{
  TIMx_NVIC_Configuration();  
  
  TIM_Mode_Config();
}

/**
  * @brief  引脚初始化
  * @retval 无
  */
void stepper_Init()
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*开启Motor相关的GPIO外设时钟*/
  MOTOR_DIR_GPIO_CLK_ENABLE();
  MOTOR_PUL_GPIO_CLK_ENABLE();
  MOTOR_EN_GPIO_CLK_ENABLE();

  /*选择要控制的GPIO引脚*/                                 
  GPIO_InitStruct.Pin = MOTOR_DIR_PIN;  

  /*设置引脚的输出类型为推挽输出*/
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

  GPIO_InitStruct.Pull =GPIO_PULLUP;

  /*设置引脚速率为高速 */   
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  /*Motor 方向引脚 初始化*/
  HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct); 

  /*Motor 脉冲引脚 初始化*/
  GPIO_InitStruct.Pin = MOTOR_PUL_PIN;  
  HAL_GPIO_Init(MOTOR_PUL_GPIO_PORT, &GPIO_InitStruct); 

  /*Motor 使能引脚 初始化*/
  GPIO_InitStruct.Pin = MOTOR_EN_PIN; 
  HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);  

  /*关掉使能*/
  MOTOR_EN(LOW);
  /*初始化定时器*/
  TIMx_Configuration();
        
}

/**
  * @brief  定时器中断函数
  * @note   无
  * @retval 无
  */
void MOTOR_PUL_IRQHandler (void)
{
  HAL_TIM_IRQHandler(&TIM_TimeBaseStructure);   
}
/**
  * @brief  回调函数
  * @note   无
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==(&TIM_TimeBaseStructure))
  {
    MOTOR_PUL_T();//翻转IO口达到脉冲的效果
  }
}
















