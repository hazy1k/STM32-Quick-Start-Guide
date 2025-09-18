#include "key.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_KEY_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /*Configure GPIO pin : WK_UP_Pin */
  GPIO_InitStruct.Pin = WK_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(WK_UP_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pins : KEY0_Pin KEY1_Pin */
  GPIO_InitStruct.Pin = KEY0_Pin|KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief 按键扫描函数
  * @note 该函数有响应优先级(同时按下多个按键): WK_UP > KEY1 > KEY0!!
  * @param mode:0 / 1, 具体含义如下:
  * @arg 0, 不支持连续按(当按键按下不放时, 只有第一次调用会返回键值,
  * 必须松开以后, 再次按下才会返回其他键值)
  * @arg 1, 支持连续按(当按键按下不放时, 每次调用该函数都会返回键值)
  * @retval 键值, 定义如下:
  * KEY0_PRES, 1, KEY0 按下
  * KEY1_PRES, 2, KEY1 按下
  * WKUP_PRES, 3, WKUP 按下
 */
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1; /* 按键按松开标志 */
    uint8_t keyval = 0;
    if (mode) key_up = 1; /* 支持连按 */
    // 获取按键的实际电平状态
    GPIO_PinState key0_state = HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
    GPIO_PinState key1_state = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
    GPIO_PinState wkup_state = HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin);
    /* 按键松开标志为 1, 且有任意一个按键按下了 */
    // 注意：KEY0和KEY1是上拉，按下为低电平；WK_UP是下拉，按下为高电平。
    if (key_up && (key0_state == GPIO_PIN_RESET || key1_state == GPIO_PIN_RESET || wkup_state == GPIO_PIN_SET))
    {
        HAL_Delay(10); /* 去抖动 */ // 短暂延时进行消抖
        // 再次读取以确认按键状态，防止抖动
        key0_state = HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
        key1_state = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
        wkup_state = HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin);
        key_up = 0; // 设置按键已按下标志
        // 根据按键状态判断是哪个按键被按下
        // 优先级：WK_UP > KEY1 > KEY0
        if (wkup_state == GPIO_PIN_SET)
        {
            keyval = WKUP_PRES;
        }
        else if (key1_state == GPIO_PIN_RESET) // KEY1是上拉，按下为低电平
        {
            keyval = KEY1_PRES;
        }
        else if (key0_state == GPIO_PIN_RESET) // KEY0是上拉，按下为低电平
        {
            keyval = KEY0_PRES;
        }
    }
    // 没有任何按键按下, 标记按键松开
    // 此时所有按键都处于非按下状态 (KEY0/KEY1 高电平，WK_UP 低电平)
    else if (key0_state == GPIO_PIN_SET && key1_state == GPIO_PIN_SET && wkup_state == GPIO_PIN_RESET)
    {
        key_up = 1;
    }
    return keyval; /* 返回键值 */
}
