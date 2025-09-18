#include "led.h"
#include "key.h"
int main(void)
{
    // 外设初始化
    LED_Init();
    KEY_GPIO_Init();
    while(1)
    {
        if(KEY_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            LED1_TOGGLE();
        }
        if(KEY_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            LED2_TOGGLE();
        }
    }
}
