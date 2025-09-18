#include <key.h>

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
