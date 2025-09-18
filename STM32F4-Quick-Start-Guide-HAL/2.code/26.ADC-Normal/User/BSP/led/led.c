#include <led.h>

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    LED_GPIO_CLK_ENABLE(); // 使能GPIO时钟
    GPIO_InitStructure.Pin = LED0_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出 
    GPIO_InitStructure.Pull = GPIO_PULLUP; // 上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; // 高速
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = LED1_GPIO_Pin;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    LED_OFF(LED0_GPIO_Pin|LED1_GPIO_Pin); // 初始关闭LED
}
