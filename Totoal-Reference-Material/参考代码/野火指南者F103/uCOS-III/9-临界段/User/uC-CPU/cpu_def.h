#ifndef  CPU_DEF_MODULE_PRESENT
#define  CPU_DEF_MODULE_PRESENT

/*
*********************************************************************************************************
*                                          CPU 字配置
*********************************************************************************************************
*/                                                        
#define  CPU_WORD_SIZE_08                          1u
#define  CPU_WORD_SIZE_16                          2u
#define  CPU_WORD_SIZE_32                          4u
#define  CPU_WORD_SIZE_64                          8u

/*
*********************************************************************************************************
*                                          临界段配置
*********************************************************************************************************
*/  
#define  CPU_CRITICAL_METHOD_NONE                  0u  
#define  CPU_CRITICAL_METHOD_INT_DIS_EN            1u   /* DIS/EN       中断*/
#define  CPU_CRITICAL_METHOD_STATUS_STK            2u   /* Push/Pop     中断状态到任务堆栈 */
#define  CPU_CRITICAL_METHOD_STATUS_LOCAL          3u   /* Save/Restore 中断状态到局部变量*/

#endif /* CPU_DEF_MODULE_PRESENT */


