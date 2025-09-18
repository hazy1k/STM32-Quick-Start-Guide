#ifndef _LED_H
#define _LED_H

#include "sys.h"

#define LED0_GPIO GPIOF
#define LED0_PIN GPIO_PIN_9
#define LED1_GPIO GPIOF
#define LED1_PIN GPIO_PIN_10

void LED_Init(void);

#endif
