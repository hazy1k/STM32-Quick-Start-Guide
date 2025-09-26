#ifndef __EXTI_H
#define	__EXTI_H


#include "stm32f10x.h"


/*外部中断EXIT相关宏定义*/
#define             macEXTI_GPIO_CLK                        (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)     
#define             macEXTI_GPIO_PORT                       GPIOA   
#define             macEXTI_GPIO_PIN                        GPIO_Pin_0
#define             macEXTI_SOURCE_PORT                     GPIO_PortSourceGPIOA
#define             macEXTI_SOURCE_PIN                      GPIO_PinSource0
#define             macEXTI_LINE                            EXTI_Line0
#define             macEXTI_IRQ                             EXTI0_IRQn
#define             macEXTI_INT_FUNCTION                    EXTI0_IRQHandler


void EXTI_Pxy_Config(void);


#endif /* __EXTI_H */
