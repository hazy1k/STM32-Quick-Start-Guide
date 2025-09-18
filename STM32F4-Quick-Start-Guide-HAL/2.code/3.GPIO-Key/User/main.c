#include "bsp_init.h"

int main(void)
{
    uint8_t key_value;
    bsp_init();
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case WKUP_Press:
                LED_ON(LED0_GPIO_Pin);
                break;
            case KEY0_Press:
                LED_OFF(LED0_GPIO_Pin);
                break;
            case KEY1_Press:
                LED_ON(LED1_GPIO_Pin);
                break;
            case KEY2_Press:
                LED_OFF(LED1_GPIO_Pin);
                break;
        }
    }
}
