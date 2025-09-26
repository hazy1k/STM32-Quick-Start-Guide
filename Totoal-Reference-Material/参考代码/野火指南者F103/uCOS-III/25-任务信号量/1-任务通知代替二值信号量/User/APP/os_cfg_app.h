/*
************************************************************************************************************************
*                                                     uC/OS-III
*                                                The Real-Time Kernel
*
*                                  (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                          All rights reserved.  Protected by international copyright laws.
*
*                                       OS CONFIGURATION (APPLICATION SPECIFICS)
*
* File    : OS_CFG_APP.H
* By      : JJL
* Version : V3.01.2
*
* LICENSING TERMS:
* ---------------
*               uC/OS-III is provided in source form to registered licensees ONLY.  It is 
*               illegal to distribute this source code to any third party unless you receive 
*               written permission by an authorized Micrium representative.  Knowledge of 
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
************************************************************************************************************************
*/

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/

                                                            /* --------------------- MISCELLANEOUS ------------------ */
#define  OS_CFG_MSG_POOL_SIZE            100u               //消息的最大数目/* Maximum number of messages                             */
#define  OS_CFG_ISR_STK_SIZE             128u               /* Stack size of ISR stack (number of CPU_STK elements)   */
#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY  10u               /* Stack limit position in percentage to empty            */


                                                            /* ---------------------- IDLE TASK --------------------- */
#define  OS_CFG_IDLE_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */


                                                            /* ------------------ ISR HANDLER TASK ------------------ */
#define  OS_CFG_INT_Q_SIZE                10u               //中断处理任务队列的大小/* Size of ISR handler task queue                         */
#define  OS_CFG_INT_Q_TASK_STK_SIZE      128u               //中断处理任务的栈大小（单位：CPU_STK）/* Stack size (number of CPU_STK elements)                */


                                                            /* ------------------- STATISTIC TASK ------------------- */
#define  OS_CFG_STAT_TASK_PRIO            11u               /* Priority                                               */
#define  OS_CFG_STAT_TASK_RATE_HZ         10u               /* Rate of execution (10 Hz Typ.)                         */
#define  OS_CFG_STAT_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */


                                                            /* ------------------------ TICKS ----------------------- */
#define  OS_CFG_TICK_RATE_HZ            1000u               // 时钟节拍频率 (10 to 1000 Hz)                    
#define  OS_CFG_TICK_TASK_PRIO            10u               // 时钟节拍任务 OS_TickTask() 的优先级
#define  OS_CFG_TICK_TASK_STK_SIZE       128u               // 时钟节拍任务 OS_TickTask() 的栈空间大小（单位：CPU_STK）
#define  OS_CFG_TICK_WHEEL_SIZE           17u               // OSCfg_TickWheel 数组的大小，推荐使用任务总数/4，且为质数


                                                            /* ----------------------- TIMERS ----------------------- */
#define  OS_CFG_TMR_TASK_PRIO             11u               //定时器任务的优先级
#define  OS_CFG_TMR_TASK_RATE_HZ          10u               //定时器的时基 (一般不能大于 OS_CFG_TICK_RATE_HZ )  
#define  OS_CFG_TMR_TASK_STK_SIZE        128u               //定时器任务的栈空间大小（单位：CPU_STK）
#define  OS_CFG_TMR_WHEEL_SIZE            17u               // OSCfg_TmrWheel 数组的大小，推荐使用任务总数/4，且为质数

#endif









