/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                          All rights reserved.  Protected by international copyright laws.
*
*                                                  CONFIGURATION  FILE
*
* File    : OS_CFG.H
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

#ifndef OS_CFG_H
#define OS_CFG_H


                                             /* ---------------------------- MISCELLANEOUS -------------------------- */
#define OS_CFG_APP_HOOKS_EN             1u   //使能/禁用钩子函数
#define OS_CFG_ARG_CHK_EN               1u   //使能/禁用参数检测
#define OS_CFG_CALLED_FROM_ISR_CHK_EN   1u   //使能/禁用检测中断中非法调用功能
#define OS_CFG_DBG_EN                   0u   //使能/禁用调试代码和变量                                     
#define OS_CFG_ISR_POST_DEFERRED_EN     1u   //使能/禁用中断延迟发布
#define OS_CFG_OBJ_TYPE_CHK_EN          1u   //使能/禁用对象类型检测            
#define OS_CFG_TS_EN                    1u   //使能/禁用时间戳

#define OS_CFG_PEND_MULTI_EN            1u   /* Enable (1) or Disable (0) code generation for multi-pend feature      */

#define OS_CFG_PRIO_MAX                32u   /* Defines the maximum number of task priorities (see OS_PRIO data type) */

#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN  1u   /* Include code to measure scheduler lock time                           */
#define OS_CFG_SCHED_ROUND_ROBIN_EN     1u   //使能/禁用时间片轮转调度
#define OS_CFG_STK_SIZE_MIN            64u   /* Minimum allowable task stack size                                     */


                                             /* ----------------------------- EVENT FLAGS --------------------------- */
#define OS_CFG_FLAG_EN                  1u   //使能/禁用事件标志
#define OS_CFG_FLAG_DEL_EN              1u   /*     Include code for OSFlagDel()                                      */
#define OS_CFG_FLAG_MODE_CLR_EN         1u   /*     Include code for Wait on Clear EVENT FLAGS                        */
#define OS_CFG_FLAG_PEND_ABORT_EN       1u   /*     Include code for OSFlagPendAbort()                                */


                                             /* -------------------------- MEMORY MANAGEMENT ------------------------ */
#define OS_CFG_MEM_EN                   1u   /* Enable (1) or Disable (0) code generation for MEMORY MANAGER          */


                                             /* --------------------- MUTUAL EXCLUSION SEMAPHORES ------------------- */
#define OS_CFG_MUTEX_EN                 1u   /* Enable (1) or Disable (0) code generation for MUTEX                   */
#define OS_CFG_MUTEX_DEL_EN             1u   /*     Include code for OSMutexDel()                                     */
#define OS_CFG_MUTEX_PEND_ABORT_EN      1u   /*     Include code for OSMutexPendAbort()                               */


                                             /* --------------------------- MESSAGE QUEUES -------------------------- */
#define OS_CFG_Q_EN                     1u   //使能/禁用消息队列
#define OS_CFG_Q_DEL_EN                 1u   /*     Include code for OSQDel()                                         */
#define OS_CFG_Q_FLUSH_EN               1u   /*     Include code for OSQFlush()                                       */
#define OS_CFG_Q_PEND_ABORT_EN          1u   /*     Include code for OSQPendAbort()                                   */


                                             /* ----------------------------- SEMAPHORES ---------------------------- */
#define OS_CFG_SEM_EN                   1u   //使能或禁用多值信号量
#define OS_CFG_SEM_DEL_EN               1u   //使能或禁用 OSSemDel() 函数 
#define OS_CFG_SEM_PEND_ABORT_EN        1u   //使能或禁用 OSSemPendAbort() 函数
#define OS_CFG_SEM_SET_EN               1u   //使能或禁用 OSSemSet() 函数


                                             /* -------------------------- TASK MANAGEMENT -------------------------- */
#define OS_CFG_STAT_TASK_EN             1u   /* Enable (1) or Disable(0) the statistics task                          */
#define OS_CFG_STAT_TASK_STK_CHK_EN     1u   /* Check task stacks from statistic task                                 */

#define OS_CFG_TASK_CHANGE_PRIO_EN      1u   /* Include code for OSTaskChangePrio()                                   */
#define OS_CFG_TASK_DEL_EN              1u   /* Include code for OSTaskDel()                                          */
#define OS_CFG_TASK_Q_EN                1u   /* Include code for OSTaskQXXXX()                                        */
#define OS_CFG_TASK_Q_PEND_ABORT_EN     1u   /* Include code for OSTaskQPendAbort()                                   */
#define OS_CFG_TASK_PROFILE_EN          1u   /* Include variables in OS_TCB for profiling                             */
#define OS_CFG_TASK_REG_TBL_SIZE        1u   /* Number of task specific registers                                     */
#define OS_CFG_TASK_SEM_PEND_ABORT_EN   1u   /* Include code for OSTaskSemPendAbort()                                 */
#define OS_CFG_TASK_SUSPEND_EN          1u   //使能/禁用函数 OSTaskSuspend() 和 OSTaskResume()                  


                                             /* -------------------------- TIME MANAGEMENT -------------------------- */
#define OS_CFG_TIME_DLY_HMSM_EN         1u   //使能/禁用 OSTimeDlyHMSM() 函数
#define OS_CFG_TIME_DLY_RESUME_EN       1u   //使能/禁用 OSTimeDlyResume() 函数

                                             /* ------------------------- TIMER MANAGEMENT -------------------------- */
#define OS_CFG_TMR_EN                   1u   //使能/禁用软件定时器
#define OS_CFG_TMR_DEL_EN               1u   //使能/禁用 OSTmrDel() 函数

#endif
