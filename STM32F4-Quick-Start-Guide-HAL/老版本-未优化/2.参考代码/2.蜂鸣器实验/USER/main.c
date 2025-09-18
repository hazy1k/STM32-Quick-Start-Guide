#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"

int main(void)
{
    HAL_Init();
    LED_Init();
    BEEP_Init();

	HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_RESET); // ≤‚ ‘DS1
    while(1)
    {
        BEEP_ON();
        delay_ms(5000);
        BEEP_OFF();
        delay_ms(5000);
    }
}
