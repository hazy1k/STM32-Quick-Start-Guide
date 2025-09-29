/**
 ****************************************************************************************************
 * @file        usmart_port.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V3.4
 * @date        2020-03-24
 * @brief       USMART 移植文件
 *
 *              通过修改改文件,可以方便的将USMART移植到其他工程 
 *
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明 
 * 
 * V3.4之前版本详细修改说明见USMART文件夹下的:readme.txt
 * 
 * V3.4 20200324
 * 1, 新增usmart_port.c和usmart_port.h,用于管理USMART的移植,方便修改
 * 2, 修改变量命名方式为: uint8_t, uint16_t, uint32_t
 * 3, 修改usmart_reset_runtime为usmart_timx_reset_time
 * 4, 修改usmart_get_runtime为usmart_timx_get_time
 * 5, 修改usmart_scan函数实现方式,改成由usmart_get_input_string获取数据流
 * 6, 修改printf函数为USMART_PRINTF宏定义
 * 7, 修改定时扫描相关函数,改用宏定义方式,方便移植
 *
 ****************************************************************************************************
 */
 
#ifndef __USMART_PORT_H
#define __USMART_PORT_H

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"




/******************************************************************************************/
/* 用户配置参数 */


#define MAX_FNAME_LEN           30      /* 函数名最大长度，应该设置为不小于最长函数名的长度。 */
#define MAX_PARM                10      /* 最大为10个参数 ,修改此参数,必须修改usmart_exe与之对应. */
#define PARM_LEN                200     /* 所有参数之和的长度不超过PARM_LEN个字节,注意串口接收部分要与之对应(不小于PARM_LEN) */


#define USMART_ENTIMX_SCAN      1       /* 使用TIM的定时中断来扫描SCAN函数,如果设置为0,需要自己实现隔一段时间扫描一次scan函数.
                                         * 注意:如果要用runtime统计功能,必须设置USMART_ENTIMX_SCAN为1!!!!
                                         */

#define USMART_USE_HELP         1       /* 使用帮助，该值设为0，可以节省近700个字节，但是将导致无法显示帮助信息。 */
#define USMART_USE_WRFUNS       1       /* 使用读写函数,使能这里,可以读取任何地址的值,还可以写寄存器的值. */

#define USMART_PRINTF           printf  /* 定义printf输出 */

/******************************************************************************************/
/* USMART定时器 定义 */

# if USMART_ENTIMX_SCAN == 1    /* 开启了使能定时器扫描,则需要如下定义 */

/* TIMX 中断定义 
 * 用于定时调用usmart.scan函数扫描串口数据,并执行相关操作
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM17任意一个定时器.
 */
#define USMART_TIMX                     TIM4
#define USMART_TIMX_IRQn                TIM4_IRQn
#define USMART_TIMX_IRQHandler          TIM4_IRQHandler
#define USMART_TIMX_CLK_ENABLE()        do{ RCC->APB1LENR |= 1 << 2; }while(0)  /* TIMX 时钟使能 */

#endif

/******************************************************************************************/


/* 如果没有定义uint32_t,则定义 */
#ifndef uint32_t
typedef unsigned           char uint8_t;
typedef unsigned short     int  uint16_t;
typedef unsigned           int  uint32_t;
#endif



char * usmart_get_input_string(void);   /* 获取输入数据流 */
void usmart_timx_reset_time(void);      /* 复位运行时间 */
uint32_t usmart_timx_get_time(void);    /* 获取运行时间 */
void usmart_timx_init(uint16_t tclk);   /* 初始化定时器 */

#endif



























