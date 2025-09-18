#include "pwr.h"
#include "key.h"
#include "usart.h"

// 低功耗模式下按键初始化，用于唤醒
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    WKUP_GPIO_CLK_ENABLE();
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; // 上升沿触发
    GPIO_InitStructure.Pull = GPIO_PULLDOWN; // 下拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_GPIO_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == WKUP_GPIO_Pin)
    {
        printf("WKUP Key pressed\r\n");
    }
    /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
}

// 进入睡眠模式
void pwr_enter_sleep(void)
{
    HAL_SuspendTick(); // 关闭Tick中断
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // 进入低功耗模式
}

// 进入停止模式
void pwr_enter_stop(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_SuspendTick(); // 关闭Tick中断
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

// 进入待机模式
void pwr_enter_standby(void)
{
    __HAL_RCC_AHB1_FORCE_RESET(); // 强制复位所有AHB1总线
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BACKUPRESET_FORCE();
    HAL_PWR_EnableBkUpAccess();
    /* STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置 */
    /* RTC中断,再进入待机模式才可以正常唤醒,否则会有问题. */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    // __HAL_RTC_WRITEPROTECTION_DISABLE(&g_rtc_handle);   /* 关闭RTC写保护 */
    /* 关闭RTC相关中断，可能在RTC实验打开了 */
    // __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&g_rtc_handle, RTC_IT_WUT);
    // __HAL_RTC_TIMESTAMP_DISABLE_IT(&g_rtc_handle, RTC_IT_TS);
    // __HAL_RTC_ALARM_DISABLE_IT(&g_rtc_handle, RTC_IT_ALRA|RTC_IT_ALRB);

    /* 清除RTC相关中断标志位 */
    // __HAL_RTC_ALARM_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    // __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_TSF); 
    // __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_WUTF);

    __HAL_RCC_BACKUPRESET_RELEASE();                    /* 备份区域复位结束 */
    // __HAL_RTC_WRITEPROTECTION_ENABLE(&g_rtc_handle); /* 使能RTC写保护 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  /* 清除Wake_UP标志 */

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           /* 设置WKUP用于唤醒 */
    HAL_PWR_EnterSTANDBYMode();                         /* 进入待机模式 */
}

