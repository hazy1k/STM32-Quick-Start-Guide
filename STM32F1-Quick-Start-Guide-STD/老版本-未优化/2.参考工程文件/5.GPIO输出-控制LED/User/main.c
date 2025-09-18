#include "stm32f10x.h"
#include "led.h"

// 延时函数
void Delay(uint32_t time)
{
    while (time--);
}

int main(void)
{
    // 初始化LED
    LED_Init();

    while (1)
    {
        LED_RED();      // 红色
        Delay(1000000); // 延时
        LED_GREEN();    // 绿色
        Delay(1000000); // 延时
        LED_BLUE();     // 蓝色
        Delay(1000000); // 延时
        LED_YELLOW();   // 黄色
        Delay(1000000); // 延时
        LED_PURPLE();   // 紫色
        Delay(1000000); // 延时
        LED_CYAN();     // 青色
        Delay(1000000); // 延时
        LED_WHITE();    // 白色
        Delay(1000000); // 延时
        LED_RGBOFF();   // 关闭所有LED
        Delay(1000000); // 延时
    }
}
