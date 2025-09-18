#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 首先就是开启时钟啦
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    // 接下来配置GPIO
    GPIO_InitStructure.Pin = KEY_UP_PIN;            // key_up引脚
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;      // 输入模式
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;        // 下拉
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     // 高速
    HAL_GPIO_Init(KEY_UP_GPIO, &GPIO_InitStructure);// 初始化GPIO
    GPIO_InitStructure.Pin = KEY0_PIN|KEY1_PIN|KEY2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;           // 上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

// 按键扫描函数说明
/*
    功能：扫描按键，返回按键值
    函数参数mode：0表示只扫描一次，1表示一直扫描
    返回值：0，没有任何按键按下；1，KEY_UP按下
    响应优先级：KEY0 > KEY1 > KEY2 > KEY_UP
*/
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_flag = 1; // 按键松开标志位
    if(mode == 1)
       key_flag = 1;
    // 判断是否有按键按下
    if(key_flag && (KEY0_GET == 0 || KEY1_GET == 0 || KEY2_GET == 0 || KEY_UP_GET == 1)) // 不要忘记了KEY_UP是高电平有效   
    {
        delay_ms(10);
        key_flag = 0; // 按键按下标志位
        if(KEY0_GET == 0) return KEY0_PRES;
        else if(KEY1_GET == 0) return KEY1_PRES;
        else if(KEY2_GET == 0) return KEY2_PRES;
        else if(KEY_UP_GET == 1) return KEY_UP_PRES;
    }
    else if(KEY0_GET == 1 && KEY1_GET == 1 && KEY2_GET == 1 && KEY_UP_GET == 0)
    {
        key_flag = 1; // 按键松开
        return 0;
    }
    return 0;
}       
