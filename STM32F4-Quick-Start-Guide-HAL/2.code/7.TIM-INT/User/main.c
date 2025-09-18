#include <bsp_init.h>

int main(void)
{
    bsp_init();
    printf("Hello World!\n");
    while(1)
    {
        LED_TOGGLE(LED0_GPIO_Pin);
        printf("LED0_GPIO_Pin 1s\r\n");
        delay_ms(1000);
    }
}
