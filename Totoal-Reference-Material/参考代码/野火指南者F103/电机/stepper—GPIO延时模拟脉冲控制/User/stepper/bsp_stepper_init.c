#include "./stepper/bsp_stepper_init.h"
#include "./led/bsp_led.h"   
#include "./delay/core_delay.h"   
#include "stm32f1xx.h"

/**
  * @brief  步进电机旋转
  * @param  tim         方波周期 单位MS 周期越短频率越高，转速越快 细分为1时最少10ms
  * @param  angle       需要转动的角度值
  * @param  dir       选择正反转(取值范围：0,1) 
  * @param  subdivide   细分值
  * @note   无
  * @retval 无
  */
void stepper_turn(int tim,float angle,float subdivide,uint8_t dir)  
{
  int n,i;
  /*根据细分数求得步距角被分成多少个方波*/
  n=(int)(angle/(1.8/subdivide));
  if(dir==CW) //顺时针
  {
    MOTOR_DIR(CW);
  }
  else if(dir==CCW)//逆时针
  {
    MOTOR_DIR(CCW);
  }
  /*开使能*/
  MOTOR_EN(HIGH);
  /*模拟方波*/
  for(i=0;i<n;i++)
  {   
    MOTOR_PLU(HIGH);
    delay_us(tim/2);
    MOTOR_PLU(LOW);
    delay_us(tim/2);
  }
  /*关使能*/
  MOTOR_EN(LOW);
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

}

