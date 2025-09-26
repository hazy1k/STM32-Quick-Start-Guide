#ifndef CPU_H
#define CPU_H

/*
*********************************************************************************************************
*                                          包含的头文件
*********************************************************************************************************
*/
#include  <cpu_def.h>
#include  <cpu_cfg.h>

/*
*********************************************************************************************************
*                                        标准数据类型配置
*********************************************************************************************************
*/
typedef  unsigned  short       CPU_INT16U;
typedef  unsigned  int         CPU_INT32U;
typedef  unsigned  char        CPU_INT08U;
typedef  volatile  CPU_INT32U  CPU_REG32;

/*
*********************************************************************************************************
*                                        CPU 字长配置
*********************************************************************************************************
*/
#define  CPU_CFG_ADDR_SIZE              CPU_WORD_SIZE_32
#define  CPU_CFG_DATA_SIZE              CPU_WORD_SIZE_32
#define  CPU_CFG_DATA_SIZE_MAX          CPU_WORD_SIZE_64
typedef  CPU_INT32U                     CPU_DATA;

/*
*********************************************************************************************************
*                                 配置 CPU 地址 & 数据 的类型
*********************************************************************************************************
*/
typedef  CPU_INT32U  CPU_ADDR;

/*
*********************************************************************************************************
*                                          CPU 堆栈配置
*********************************************************************************************************
*/
typedef  CPU_INT32U             CPU_STK;
typedef  CPU_ADDR               CPU_STK_SIZE;

/*
*********************************************************************************************************
*                                          临界段配置
*********************************************************************************************************
*/

                                                               
#define  CPU_CFG_CRITICAL_METHOD    CPU_CRITICAL_METHOD_STATUS_LOCAL

typedef  CPU_INT32U                 CPU_SR;                    

                                                                
#if     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
/* 定义一个局部变量，用于保存CPU中断状态 */
#define  CPU_SR_ALLOC()             CPU_SR  cpu_sr = (CPU_SR)0
#else
#define  CPU_SR_ALLOC()
#endif


/* Save    CPU 状态 & 失能中断 */
#define  CPU_INT_DIS()         do { cpu_sr = CPU_SR_Save(); } while (0)
/* Restore CPU 状态  */
#define  CPU_INT_EN()          do { CPU_SR_Restore(cpu_sr); } while (0)

/* 失能中断 */
#define  CPU_CRITICAL_ENTER()  do { CPU_INT_DIS(); } while (0)
/* 重新使能中断 */
#define  CPU_CRITICAL_EXIT()   do { CPU_INT_EN();  } while (0) 

/*
*********************************************************************************************************
*                                             CPU 前/后 导0配置
*********************************************************************************************************
*/

#define  CPU_CFG_LEAD_ZEROS_ASM_PRESENT                        

                                                                
#define  CPU_CFG_TRAIL_ZEROS_ASM_PRESENT



/*
*********************************************************************************************************
*                                             函数声明
*********************************************************************************************************
*/

void        CPU_IntDis       (void); 	           /* 在cpu_a.asm定义 */
void        CPU_IntEn        (void);	           /* 在cpu_a.asm定义 */



CPU_DATA    CPU_CntLeadZeros (CPU_DATA    val);    
CPU_DATA    CPU_CntTrailZeros(CPU_DATA  val);

/* 在cpu_a.asm定义 */
CPU_SR      CPU_SR_Save      (void);
void        CPU_SR_Restore   (CPU_SR      cpu_sr);

#endif /* CPU_H */

