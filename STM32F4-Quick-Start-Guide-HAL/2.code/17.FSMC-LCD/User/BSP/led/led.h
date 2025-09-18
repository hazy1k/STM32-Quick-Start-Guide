#ifndef __LED_H__
#define __LED_H__

#include <sys.h>

#define LED_GPIO_Port GPIOF
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED0_GPIO_Pin GPIO_PIN_9
#define LED1_GPIO_Pin GPIO_PIN_10

// 低电平点亮
#define LED_ON(x)  HAL_GPIO_WritePin(LED_GPIO_Port, x, GPIO_PIN_RESET)
#define LED_OFF(x) HAL_GPIO_WritePin(LED_GPIO_Port, x, GPIO_PIN_SET)
#define LED_TOGGLE(x) HAL_GPIO_TogglePin(LED_GPIO_Port, x)

void bsp_led_init(void);

#endif /* __LED_H__ */
