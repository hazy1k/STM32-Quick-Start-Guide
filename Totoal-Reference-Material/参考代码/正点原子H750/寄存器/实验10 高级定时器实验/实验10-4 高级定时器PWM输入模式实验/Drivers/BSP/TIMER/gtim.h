/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2020-03-14
 * @brief       通用定时器 驱动代码
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
 * V1.0 20200314
 * 第一次发布
 * V1.1 20200317
 * 新增gtim_timx_pwm_chy_init函数
 * V1.2 20200318
 * 1,新增gtim_timx_cap_chy_init函数
 * V1.3 20200329
 * 1,支持外部脉冲计数功能
 * 2,新增gtim_timx_cnt_chy_init,gtim_timx_cnt_chy_get_count和gtim_timx_cnt_chy_restart三个函数
 * V1.4 20200329
 * 1,新增gtim_timx_npwm_chy_init函数
 * 2,新增gtim_timx_npwm_chy_set函数
 *
 ****************************************************************************************************
 */

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 通用定时器 定义 */

/* TIMX 中断定义
 * 默认是针对TIM2~TIM5, TIM12~TIM17.
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM17任意一个定时器.
 */
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ RCC->APB1LENR |= 1 << 1; }while(0)      /* TIM3 时钟使能 */

/* TIMX PWM输出定义
 * 这里输出的PWM控制LED0(RED)的亮度
 * 默认是针对TIM2~TIM5, TIM12~TIM17.
 * 注意: 通过修改这8个宏定义,可以支持TIM1~TIM17任意一个定时器,任意一个IO口输出PWM
 */
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN14
#define GTIM_TIMX_PWM_CHY_GPIO_AF           2                           /* AF功能选择 */
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 1; }while(0)       /* PB口时钟使能 */

#define GTIM_TIMX_PWM                       TIM12
#define GTIM_TIMX_PWM_CHY                   1                           /* 通道Y,  1<= Y <=4 */
#define GTIM_TIMX_PWM_CHY_CCRX              TIM12->CCR1                  /* 通道Y的输出比较寄存器 */
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB1LENR |= 1 << 6; }while(0)      /* TIM12 时钟使能 */

///* 这里输出的PWM控制LED0(RED)的亮度 */
//#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
//#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN4
//#define GTIM_TIMX_PWM_CHY_GPIO_AF           2                           /* AF功能选择 */
//#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 1; }while(0)       /* PB口时钟使能 */

//#define GTIM_TIMX_PWM                       TIM3
//#define GTIM_TIMX_PWM_CHY                   1                           /* 通道Y,  1<= Y <=4 */
//#define GTIM_TIMX_PWM_CHY_CCRX              TIM3->CCR1                  /* 通道Y的输出比较寄存器 */
//#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB1LENR |= 1 << 1; }while(0)      /* TIM3 时钟使能 */

///* 这里输出的PWM控制LED1(GREEN)的亮度 */
//#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOE
//#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN6
//#define GTIM_TIMX_PWM_CHY_GPIO_AF           4                           /* AF功能选择 */
//#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 4; }while(0)     /* PE口时钟使能 */

//#define GTIM_TIMX_PWM                       TIM15
//#define GTIM_TIMX_PWM_CHY                   2                           /* 通道Y,  1<= Y <=4 */
//#define GTIM_TIMX_PWM_CHY_CCRX              TIM15->CCR2                 /* 通道Y的输出比较寄存器 */
//#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB2ENR |= 1 << 16; }while(0)    /* TIM15 时钟使能 */


///* 这里输出的PWM控制LED2(BLUE)的亮度 */
//#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOE
//#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN5
//#define GTIM_TIMX_PWM_CHY_GPIO_AF           4                           /* AF功能选择 */
//#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 4; }while(0)     /* PE口时钟使能 */

//#define GTIM_TIMX_PWM                       TIM15
//#define GTIM_TIMX_PWM_CHY                   1                           /* 通道Y,  1<= Y <=4 */
//#define GTIM_TIMX_PWM_CHY_CCRX              TIM15->CCR1                 /* 通道Y的输出比较寄存器 */
//#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB2ENR |= 1 << 16; }while(0)    /* TIM15 时钟使能 */


/* TIMX 输出指定个数PWM 定义
* 这里输出的PWM控制LED2(BLUE)的亮灭,亮灭一次表示一个PWM波
* 默认是针对TIM2~TIM5, TIM12~TIM17.
* 注意: 通过修改这9个宏定义,可以支持TIM1~TIM17任意一个定时器,任意一个IO口输出PWM
*/
#define GTIM_TIMX_NPWM_CHY_GPIO_PORT            GPIOE
#define GTIM_TIMX_NPWM_CHY_GPIO_PIN             SYS_GPIO_PIN5
#define GTIM_TIMX_NPWM_CHY_GPIO_AF              4                          /* AF功能选择 */
#define GTIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define GTIM_TIMX_NPWM                          TIM15
#define GTIM_TIMX_NPWM_IRQn                     TIM15_IRQn
#define GTIM_TIMX_NPWM_IRQHandler               TIM15_IRQHandler
#define GTIM_TIMX_NPWM_CHY                      1                           /* 通道Y,  1<= Y <=4 */
#define GTIM_TIMX_NPWM_CHY_CCRX                 TIM15->CCR1                 /* 通道Y的输出比较寄存器 */
#define GTIM_TIMX_NPWM_CHY_CLK_ENABLE()         do{ RCC->APB2ENR |= 1 << 16; }while(0)  /* TIMX 时钟使能 */

/* TIMX 输入捕获定义
* 这里的输入捕获使用定时器TIM5_CH1,捕获WK_UP按键的输入
* 默认是针对TIM2~TIM5, TIM12~TIM17.
* 注意: 通过修改这10个宏定义,可以支持TIM1~TIM17任意一个定时器,任意一个IO口做输入捕获
*       特别要注意:默认用的PA0,设置的是下拉输入!如果改其他IO,对应的上下拉方式/AF功能等也得改!
*/
#define GTIM_TIMX_CAP_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN             SYS_GPIO_PIN0
#define GTIM_TIMX_CAP_CHY_GPIO_AF              2                          /* AF功能选择 */
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define GTIM_TIMX_CAP                          TIM5
#define GTIM_TIMX_CAP_IRQn                     TIM5_IRQn
#define GTIM_TIMX_CAP_IRQHandler               TIM5_IRQHandler
#define GTIM_TIMX_CAP_CHY                      1                            /* 通道Y,  1<= Y <=4 */
#define GTIM_TIMX_CAP_CHY_CCRX                 TIM5->CCR1                   /* 通道Y的输出比较寄存器 */
#define GTIM_TIMX_CAP_CHY_CLK_ENABLE()         do{ RCC->APB1LENR |= 1 << 3; }while(0)   /* TIMX 时钟使能 */

/* TIMX 输入计数定义
* 这里的输入计数使用定时器TIM2_CH1,捕获WK_UP按键的输入
* 默认是针对TIM2~TIM5, TIM12~TIM17.  只有CH1和CH2通道可以用做输入计数, CH3/CH4不支持!
* 注意: 通过修改这9个宏定义,可以支持TIM1~TIM17任意一个定时器,CH1/CH2对应IO口做输入计数
*       特别要注意:默认用的PA0,设置的是下拉输入!如果改其他IO,对应的上下拉方式/AF功能等也得改!
*/
#define GTIM_TIMX_CNT_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CNT_CHY_GPIO_PIN             SYS_GPIO_PIN0
#define GTIM_TIMX_CNT_CHY_GPIO_AF              1                          /* AF功能选择 */
#define GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define GTIM_TIMX_CNT                          TIM2
#define GTIM_TIMX_CNT_IRQn                     TIM2_IRQn
#define GTIM_TIMX_CNT_IRQHandler               TIM2_IRQHandler
#define GTIM_TIMX_CNT_CHY                      1                            /* 通道Y,  1<= Y <=2 */
#define GTIM_TIMX_CNT_CHY_CLK_ENABLE()         do{ RCC->APB1LENR |= 1 << 0; }while(0)   /* TIMX 时钟使能 */

/******************************************************************************************/

void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* 通用定时器 定时中断初始化函数 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* 通用定时器 PWM初始化函数 */
void gtim_timx_npwm_chy_init(uint16_t arr, uint16_t psc);   /* 通用定时器 输出指定个数PWM初始化函数 */
void gtim_timx_npwm_chy_set(uint32_t npwm);                 /* 通用定时器 设置输出PWM的个数 */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc);    /* 通用定时器 输入捕获初始化函数 */
void gtim_timx_cnt_chy_init(uint16_t psc);                  /* 通用定时器 脉冲计数初始化函数 */
uint32_t gtim_timx_cnt_chy_get_count(void);                 /* 通用定时器 获取脉冲计数 */
void gtim_timx_cnt_chy_restart(void);                       /* 通用定时器 重启计数器 */
#endif

















