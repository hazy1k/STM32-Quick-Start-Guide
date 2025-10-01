/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
 * @date        2020-03-25
 * @brief       低功耗模式 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200325
 * 第一次发布
 * V1.1 20200325
 * 支持进入睡眠模式及唤醒
 * 新增pwr_wkup_key_init和pwr_enter_sleep函数
 * V1.2 20200325
 * 支持进入停止模式及唤醒
 * 新增pwr_enter_stop函数
 * V1.3 20200326
 * 支持进入待机模式及唤醒
 * 新增pwr_enter_standby函数
 *
 ****************************************************************************************************
 */

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/RTC/rtc.h"


/**
 * @brief       初始化PVD电压监视器
 * @param       pls: 电压等级
 *   @arg       PWR_PVDLEVEL_0,1.95V;  PWR_PVDLEVEL_1,2.1V
 *   @arg       PWR_PVDLEVEL_2,2.25V;  PWR_PVDLEVEL_3,2.4V;
 *   @arg       PWR_PVDLEVEL_4,2.55V;  PWR_PVDLEVEL_5,2.7V;
 *   @arg       PWR_PVDLEVEL_6,2.85V;  PWR_PVDLEVEL_7,使用PVD_IN脚上的电压(与Vrefint比较)
 * @retval      无
 */
void pwr_pvd_init(uint32_t pls)
{
    PWR_PVDTypeDef pvd_handle = {0};

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriority(PVD_AVD_IRQn, 3 ,3);
    HAL_NVIC_EnableIRQ(PVD_AVD_IRQn);
    
    pvd_handle.PVDLevel = pls;
    pvd_handle.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
    HAL_PWR_ConfigPVD(&pvd_handle);

    HAL_PWR_EnablePVD();
}

/**
 * @brief       PVD/AVD中断服务函数
 * @param       无
 * @retval      无
 */
void PVD_AVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

/**
 * @brief       PVD/AVD中断服务回调函数
 * @param       无
 * @retval      无
 */
void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO))   /* 电压比PLS所选电压还低 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED); /* LCD显示电压低 */
        LED1(0);    /* 点亮绿灯, 表明电压低了 */
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);/* LCD显示电压正常 */
        LED1(1);    /* 灭掉绿灯 */
    }
}

/***********************************睡眠模式实验程序*******************************************/

/**
 * @brief       低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式/待机模式)
 * @param       无
 * @retval      无
 */
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    PWR_WKUP_GPIO_CLK_ENABLE();     /* WKUP时钟使能 */

    gpio_init_struct.Pin = PWR_WKUP_GPIO_PIN;                 /* KEY0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;              /* 中断,上升沿 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                    /* 下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;       /* 高速 */
    HAL_GPIO_Init(PWR_WKUP_GPIO_PORT, &gpio_init_struct);     /* KEY0引脚初始化 */

    HAL_NVIC_SetPriority(PWR_WKUP_INT_IRQn,2,2); /* 抢占优先级2，子优先级2 */
    HAL_NVIC_EnableIRQ(PWR_WKUP_INT_IRQn); 
}

/**
 * @brief       WK_UP按键 外部中断服务程序
 * @param       无
 * @retval      无
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PWR_WKUP_GPIO_PIN);
}

/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
    HAL_SuspendTick();  /* 暂停滴答时钟，防止通过滴答时钟中断唤醒 */

    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); /* 进入睡眠模式 */
}

/**
 * @brief       外部中断回调函数
 * @param       GPIO_Pin:中断线引脚
 * @note        此函数会被PWR_WKUP_INT_IRQHandler()调用
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PWR_WKUP_GPIO_PIN)
    {
        /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
    }
}

/***********************************停止模式实验程序*******************************************/

/**
 * @brief       进入停止模式
 * @param       无
 * @retval      无
 */
void pwr_enter_stop(void)
{
    sys_stm32_clock_init(200, 2, 2, 4);/* 设置时钟,400Mhz，降频 */
    HAL_SuspendTick();                 /*设置停止模式时，FLASH进入掉电状态*/
    HAL_PWREx_EnableFlashPowerDown();
    
    /* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 */
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/***********************************停止模式实验程序*******************************************/

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void pwr_enter_standby(void)
{
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); /* 禁止唤醒 */

    __HAL_RCC_BACKUPRESET_FORCE();      /* 复位备份区域 */
    HAL_PWR_EnableBkUpAccess();         /* 后备区域访问使能 */

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&g_rtc_handle);/* 关闭RTC写保护 */

    /* 关闭RTC相关中断，可能在RTC实验打开了 */
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&g_rtc_handle, RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&g_rtc_handle, RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&g_rtc_handle, RTC_IT_ALRA | RTC_IT_ALRB);

    /* 清除RTC相关中断标志位 */
    __HAL_RTC_ALARM_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_ALRAF | RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_TSF); 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_WUTF);

    __HAL_RCC_BACKUPRESET_RELEASE();                   /* 备份区域复位结束 */
    __HAL_RTC_WRITEPROTECTION_ENABLE(&g_rtc_handle);   /* 使能RTC写保护 */
    
   
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_HIGH);  /* 使能KEY_UP引脚的唤醒功能 */

    HAL_SuspendTick();             /* 暂停滴答时钟，防止通过滴答时钟中断唤醒 */
  
    HAL_PWR_EnterSTANDBYMode();    /* 进入待机模式 */
}






















