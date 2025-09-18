#include "key.h"
#include "led.h"
#include "beep.h"

void bsp_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    KEY_GPIO_CLK_ENABLE();
    WKUP_GPIO_CLK_ENABLE();
    /* KEY0-PE4 */
    GPIO_InitStructure.Pin = KEY0_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY1-PE3 */
    GPIO_InitStructure.Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY2-PE2 */
    GPIO_InitStructure.Pin = KEY2_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* WKUP-PA0 */
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);
}

uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1; // 按键松开标志
    uint8_t key_val = 0;
    if(mode)
        key_up = 1; // 支持连按
    // 判断按键是否按下
    if(key_up && (KEY0_GET == 0 || KEY1_GET == 0 || KEY2_GET == 0 || WKUP_GET == 1))
    {
        delay_ms(10);
        key_up = 0; // 代表按键按下
        if(KEY0_GET == 0)
            return KEY0_Press;
        if(KEY1_GET == 0)
            return KEY1_Press;
        if(KEY2_GET == 0)
            return KEY2_Press;
        if(WKUP_GET == 1)
            return WKUP_Press;
    }
    else if(KEY0_GET == 1 && KEY1_GET == 1 && KEY2_GET == 1 && WKUP_GET == 0) // 释放按键
    {
        key_up = 1; // 代表按键释放
        return 0;
    }
    return 0;
}

void bsp_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    bsp_key_init();
    /* KEY0 - PE4 - 下降沿触发中断 */
    GPIO_InitStructure.Pin = KEY0_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING; //  下降沿触发
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY1 - PE3 - 下降沿触发中断 */
    GPIO_InitStructure.Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY2 - PE2 - 下降沿触发中断 */
    GPIO_InitStructure.Pin = KEY2_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* WKUP - PA0 - 上升沿触发中断 */
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);   
    /* NVIC配置 */
    HAL_NVIC_SetPriority(KEY0_INT_IRQn, 0, 2); // 抢占0，子优先级2
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn); // 使能EXTI_Line4
    HAL_NVIC_SetPriority(KEY1_INT_IRQn, 1, 2); // 抢占1，子优先级2 
    HAL_NVIC_EnableIRQ(KEY1_INT_IRQn); // 使能EXTI_Line3
    HAL_NVIC_SetPriority(KEY2_INT_IRQn, 2, 2); // 抢占2，子优先级2
    HAL_NVIC_EnableIRQ(KEY2_INT_IRQn); //  使能EXTI_Line2
    HAL_NVIC_SetPriority(WKUP_INT_IRQn, 3, 2); //  抢占3，子优先级2
    HAL_NVIC_EnableIRQ(WKUP_INT_IRQn); //  使能EXTI_Line0
}

// 中断服务函数
void KEY0_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_GPIO_Pin); // 清中断标志位
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_GPIO_Pin);
}
void KEY1_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY1_GPIO_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_GPIO_Pin);
}
void KEY2_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY2_GPIO_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(KEY2_GPIO_Pin);
}
void WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_GPIO_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(WKUP_GPIO_Pin);
}

// 中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case KEY0_GPIO_Pin:
            LED_TOGGLE(LED0_GPIO_Pin); 
            break;
        case KEY1_GPIO_Pin:
            LED_TOGGLE(LED1_GPIO_Pin);
            break;
        case KEY2_GPIO_Pin:
            BEEP_ON();            
            break;
        case WKUP_GPIO_Pin:
                BEEP_OFF();
            break;
        default:    
            break;
    } 
}
