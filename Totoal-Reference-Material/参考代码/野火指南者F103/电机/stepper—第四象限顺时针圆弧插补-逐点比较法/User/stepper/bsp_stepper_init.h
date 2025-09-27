#ifndef __BSP_STEP_MOTOR_INIT_H
#define	__BSP_STEP_MOTOR_INIT_H

#include "stm32f1xx_hal.h"

/* 步进电机结构体 */
typedef struct{
  uint16_t pul_pin;
  uint16_t dir_pin;
  uint16_t en_pin;
  uint32_t pul_channel;
  GPIO_TypeDef *pul_port;
  GPIO_TypeDef *dir_port;
  GPIO_TypeDef *en_port;
  uint16_t oc_pulse_num;
}Stepper_TypeDef;




/*宏定义*/
/*******************************************************/
#define MOTOR_PUL_TIM                        TIM8
#define MOTOR_PUL_IRQn                       TIM8_UP_TIM13_IRQn
#define MOTOR_PUL_IRQHandler                 TIM8_UP_TIM13_IRQHandler
#define MOTOR_PUL_CLK_ENABLE()               __HAL_RCC_TIM8_CLK_ENABLE()

/*********************X轴电机引脚定义*******************/
//Motor 方向
#define X_MOTOR_DIR_PIN                      GPIO_PIN_14
#define X_MOTOR_DIR_GPIO_PORT                GPIOB
#define X_MOTOR_DIR_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor 使能
#define X_MOTOR_EN_PIN                       GPIO_PIN_12
#define X_MOTOR_EN_GPIO_PORT                 GPIOB
#define X_MOTOR_EN_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor 脉冲
#define X_MOTOR_PUL_PORT                     GPIOC
#define X_MOTOR_PUL_PIN                      GPIO_PIN_6
#define X_MOTOR_PUL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

//定时器通道
#define X_MOTOR_PUL_CHANNEL                  TIM_CHANNEL_1

/*********************Y轴电机引脚定义*******************/
//Motor 方向
#define Y_MOTOR_DIR_PIN                      GPIO_PIN_15
#define Y_MOTOR_DIR_GPIO_PORT                GPIOB        
#define Y_MOTOR_DIR_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor 使能
#define Y_MOTOR_EN_PIN                       GPIO_PIN_13
#define Y_MOTOR_EN_GPIO_PORT                 GPIOB                       
#define Y_MOTOR_EN_GPIO_CLK_ENABLE()      	 __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor 脉冲
#define Y_MOTOR_PUL_PORT       			         GPIOC
#define Y_MOTOR_PUL_PIN             		     GPIO_PIN_7
#define Y_MOTOR_PUL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

//定时器通道
#define Y_MOTOR_PUL_CHANNEL                  TIM_CHANNEL_2


//控制使能引脚
/* 带参宏，可以像内联函数一样使用 */
#define MOTOR_PUL(port, pin, x)              HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_DIR(port, pin, x)              HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_OFFLINE(port, pin, x)          HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_START(tim, channel, status)    TIM_CCxChannelCmd(tim, channel, status)
#define MOTOR_STOP(tim, channel, status)     TIM_CCxChannelCmd(tim, channel, status)

/*频率相关参数*/
//定时器实际时钟频率为：168MHz/TIM_PRESCALER
//其中 高级定时器的 频率为168MHz,其他定时器为84MHz
//168/TIM_PRESCALER=28MHz
//具体需要的频率可以自己计算
#define TIM_PRESCALER                2
// 定义定时器周期，输出比较模式周期设置为0xFFFF
#define TIM_PERIOD                   0xFFFF

/************************************************************/
#define HIGH GPIO_PIN_SET	      //高电平
#define LOW  GPIO_PIN_RESET	    //低电平

#define ON  HIGH	              //开
#define OFF LOW	                //关

#define CW 	LOW		              //顺时针
#define CCW HIGH      	        //逆时针


//控制使能引脚
#define MOTOR_EN(x)					HAL_GPIO_WritePin(X_MOTOR_EN_GPIO_PORT,X_MOTOR_EN_PIN,x);\
                            HAL_GPIO_WritePin(Y_MOTOR_EN_GPIO_PORT,Y_MOTOR_EN_PIN,x)

extern TIM_HandleTypeDef TIM_StepperHandle;
extern Stepper_TypeDef step_motor[2];

void stepper_Init(void);

#endif /* __STEP_MOTOR_INIT_H */
