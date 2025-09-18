#include <bsp_init.h>
#include <gtim.h>

int main(void)
{
    bsp_init();
    uint8_t dir = 1; // 1：正序，0：倒序
    uint16_t led_duty = 0; // LED PWM输出占空比
    // time = (500*84)/84 = 500ms
    // 84/84 = 1Mhz
    // PWM频率1Mhz/500 = 2KHz
    TIM14_PWM_Init(500-1, 84-1);
    while(1)
    {
        if(dir)
            led_duty++;
        else
            led_duty--;
        if(led_duty >= 500)
        {    
            dir = 0;
        }
        if(led_duty == 0)
        {    
            dir = 1;           
        }
        TIM_SetCompare(led_duty);
    }
}
