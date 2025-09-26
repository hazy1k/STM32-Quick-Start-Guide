#ifndef __KEY_H
#define	__KEY_H



#include "stm32f10x.h"



/************************************ ≈‰÷√ KEY1 *********************************/
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOA
#define               macKEY1_GPIO_PORT    	                GPIOA			   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_0
#define               macKEY1_GPIO_Mode		                  GPIO_Mode_IPD



/************************************ ≈‰÷√ KEY2 *********************************/
#define               macKEY2_GPIO_CLK                      RCC_APB2Periph_GPIOC
#define               macKEY2_GPIO_PORT    	                GPIOC
#define               macKEY2_GPIO_PIN		                  GPIO_Pin_13
#define               macKEY2_GPIO_Mode		                  GPIO_Mode_IPU



void    Key_Initial    ( void );
uint8_t Key_Scan       ( GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin, uint8_t ucPushState, uint8_t * pKeyPress );
uint8_t Key_ReadStatus ( GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin, uint8_t ucPushState );


#endif /* __KEY_H */

