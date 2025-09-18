#include "bsp_init.h"
#include "wwdg.h"

// 上限时间：Twwdg=4096×8×(0x7F-0x5F)/42MHz=24.98ms
// 下限时间：Twwdg=4096×8×(0x7F-0x3F)/42MHz=49.97ms
// 即喂狗的窗口区间为 24.98~49.97ms。 
// 我们在程序的其它地方没有喂狗，所以程序会在 49.97ms 左右进入中断喂狗一次，并翻转LED0。
int main(void)
{
    bsp_init();
    LED_ON(LED1_GPIO_Pin);
    delay_ms(1000);
    // 先点亮LED1，延时1s后，初始化窗口看门狗，进入死循环，关闭LED1。
    bsp_wwdg_init(0x7F, 0x5F, WWDG_PRESCALER_8); //  WWDG初始化
    while(1)
    {
        LED_OFF(LED1_GPIO_Pin);
    }
}
