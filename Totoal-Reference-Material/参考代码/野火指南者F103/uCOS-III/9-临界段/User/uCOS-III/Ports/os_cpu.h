#ifndef  OS_CPU_H
#define  OS_CPU_H

/*
*********************************************************************************************************
*                                               宏定义
*********************************************************************************************************
*/

#ifndef  NVIC_INT_CTRL
#define  NVIC_INT_CTRL                      *((CPU_REG32 *)0xE000ED04)   /* 中断控制及状态寄存器 SCB_ICSR */
#endif

#ifndef  NVIC_PENDSVSET
#define  NVIC_PENDSVSET                                    0x10000000    /* 触发PendSV异常的值 Bit28：PENDSVSET */
#endif

#define  OS_TASK_SW()               NVIC_INT_CTRL = NVIC_PENDSVSET
#define  OSIntCtxSw()               NVIC_INT_CTRL = NVIC_PENDSVSET

/*
*********************************************************************************************************
*                                               时间戳配置
*********************************************************************************************************
*/

#if      OS_CFG_TS_EN == 1u
#define  OS_TS_GET()               (CPU_TS)CPU_TS_TmrRd()
#else
#define  OS_TS_GET()               (CPU_TS)0u
#endif


/*
*********************************************************************************************************
*                                              函数声明
*********************************************************************************************************
*/
void OSStartHighRdy(void);
void PendSV_Handler(void);

void  OS_CPU_SysTickInit (CPU_INT32U  ms);
void SysTick_Handler(void);


#endif   /* OS_CPU_H */

