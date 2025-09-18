#include "bsp_init.h"

void bsp_init(void)
{
	MX_LED_GPIO_Init();
	MX_BEEP_GPIO_Init();
	// MX_KEY_GPIO_Init();
	MX_EXTI_GPIO_Init();
}
