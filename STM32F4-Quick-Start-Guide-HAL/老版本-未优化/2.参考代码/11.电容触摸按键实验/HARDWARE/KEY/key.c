#include "key.h"
#include "delay.h"

// 按键初始化函数
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 开启GPIOA时钟
    __HAL_RCC_GPIOE_CLK_ENABLE(); // 开启GPIOE时钟

    GPIO_Initure.Pin = GPIO_PIN_0;        // PA0
    GPIO_Initure.Mode = GPIO_MODE_INPUT;  // 输入
    GPIO_Initure.Pull = GPIO_PULLDOWN;    // 下拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH; // 高速
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    
	GPIO_Initure.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4; // PE2,3,4
    GPIO_Initure.Mode = GPIO_MODE_INPUT;  // 输入
    GPIO_Initure.Pull = GPIO_PULLUP;      // 上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH; // 高速
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);
}

// 按键处理函数
// 返回按键值
// mode:0,不支持连续按;1,支持连续按;
// 0，没有任何按键按下
// 1，WKUP按下 WK_UP
// 注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1; // 按键松开标志
    if(mode == 1)
        key_up = 1; // 支持连按
    if(key_up && (KEY0==0||KEY1==0||KEY2==0||WK_UP==1)) // 按键按下
    {
        delay_ms(10);
        key_up = 0; // 按键按下标志
        if(KEY0 == 0)     return KEY0_PRES; // KEY0按下
        else if(KEY1==0)  return KEY1_PRES; // KEY1按下
        else if(KEY2==0)  return KEY2_PRES; // KEY2按下
        else if(WK_UP==1) return WKUP_PRES; // WKUP按下
    }
    else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0) // 按键松开
    {
        key_up = 1; // 按键松开
        return 0;   // 无按键按下
    }
    return 0; // 无按键按下
}
