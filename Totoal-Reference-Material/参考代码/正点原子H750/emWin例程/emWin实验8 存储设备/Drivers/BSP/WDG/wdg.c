/**
 ****************************************************************************************************
 * @file        wdg.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-03-13
 * @brief       看门狗 驱动代码
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
 * V1.0 20200312
 * 第一次发布
 * V1.1 20200312
 * 新增wwdg_init函数
 *
 ****************************************************************************************************
 */

#include "./BSP/WDG/wdg.h"
#include "./BSP/LED/led.h"


IWDG_HandleTypeDef g_iwdg_handle; /* 独立看门狗句柄 */

/**
 * @brief       初始化独立看门狗 
 * @param       prer: 分频数:0~7(只有低3位有效!)
 *   @arg       分频因子 = 4 * 2^prer. 但最大值只能是256!
 * @param       rlr: 自动重装载值,0~0XFFF. 
 * @note        时间计算(大概):Tout=((4 * 2^prer) * rlr) / 32 (ms). 
 * @retval      无
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    g_iwdg_handle.Instance = IWDG1;
    g_iwdg_handle.Init.Prescaler = prer;               /* 设置IWDG分频系数 */
    g_iwdg_handle.Init.Reload = rlr;                   /* 重装载 */
    g_iwdg_handle.Init.Window = IWDG_WINDOW_DISABLE;   /* 关闭窗口功能 */
    HAL_IWDG_Init(&g_iwdg_handle);    
}
    
/**
 * @brief       喂独立看门狗
 * @param       无
 * @retval      无
 */
void iwdg_feed(void)
{   
    HAL_IWDG_Refresh(&g_iwdg_handle); /* 重装载 */
}


/************************以下是窗口看门狗代码********************************/

WWDG_HandleTypeDef g_wwdg_handle;     //窗口看门狗句柄

/**
 * @brief       初始化窗口看门狗
 * @param       tr: T[6:0],计数器值
 * @param       tw: W[6:0],窗口值
 * @note        fprer:分频系数（WDGTB）,范围:0~7,表示2^WDGTB分频
 *              Fwwdg=PCLK3/(4096*2^fprer). 一般PCLK3=120Mhz
 *              溢出时间=(4096*2^fprer)*(tr-0X3F)/PCLK3
 *              假设fprer=4,tr=7f,PCLK3=120Mhz
 *              则溢出时间=4096*16*64/120Mhz=34.95ms
 * @retval      无
 */
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer)
{
    g_wwdg_handle.Instance = WWDG1;
    g_wwdg_handle.Init.Prescaler = fprer;  /* 设置分频系数 */
    g_wwdg_handle.Init.Window = wr;        /* 设置窗口值 */
    g_wwdg_handle.Init.Counter = tr;       /* 设置计数器值 */
    g_wwdg_handle.Init.EWIMode = WWDG_EWI_ENABLE;/* 使能窗口看门狗提前唤醒中断 */
    HAL_WWDG_Init(&g_wwdg_handle);        /* 初始化WWDG */
}

/**
 * @brief       WWDG底层驱动
 * @param       hwwdg:窗口看门狗句柄
 * @note        此函数会被HAL_WWDG_Init()调用
 * @retval      无
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    __HAL_RCC_WWDG1_CLK_ENABLE();            /* 使能窗口看门狗时钟 */
     
    HAL_NVIC_SetPriority(WWDG_IRQn,2,3);     /* 抢占优先级2，子优先级为3 */
    HAL_NVIC_EnableIRQ(WWDG_IRQn);           /* 使能窗口看门狗中断 */
}

/**
 * @brief       窗口看门狗中断服务程序
 * @param       无
 * @retval      无
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&g_wwdg_handle);
}

/**
 * @brief       窗口看门狗中断服务回调函数
 * @param       无
 * @note        此函数会被HAL_WWDG_IRQHandler()调用
 * @retval      无
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
    HAL_WWDG_Refresh(&g_wwdg_handle);/* 更新窗口看门狗值 */
    LED1_TOGGLE();                   /* 绿灯闪烁 */
}



















