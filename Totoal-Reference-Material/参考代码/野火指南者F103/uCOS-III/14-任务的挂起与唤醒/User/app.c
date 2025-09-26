/*
************************************************************************************************************************
*                                                 包含的头文件
************************************************************************************************************************
*/
#include "os.h"
#include "ARMCM3.h"

/*
************************************************************************************************************************
*                                                   宏定义
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*                                                  全局变量
************************************************************************************************************************
*/

uint32_t flag1;
uint32_t flag2;
uint32_t flag3;

/*
************************************************************************************************************************
*                                                  TCB & STACK & 任务声明
************************************************************************************************************************
*/
#define  TASK1_STK_SIZE       128
#define  TASK2_STK_SIZE       128
#define  TASK3_STK_SIZE       128


static   OS_TCB    Task1TCB;
static   OS_TCB    Task2TCB;
static   OS_TCB    Task3TCB;


static   CPU_STK   Task1Stk[TASK1_STK_SIZE];
static   CPU_STK   Task2Stk[TASK2_STK_SIZE];
static   CPU_STK   Task3Stk[TASK2_STK_SIZE];


void     Task1( void *p_arg );
void     Task2( void *p_arg );
void     Task3( void *p_arg );


/*
************************************************************************************************************************
*                                                  函数声明
************************************************************************************************************************
*/
void delay(uint32_t count);

/*
************************************************************************************************************************
*                                                    main函数
************************************************************************************************************************
*/
/*
* 注意事项：1、该工程使用软件仿真，debug需选择 Ude Simulator
*           2、在Target选项卡里面把晶振Xtal(Mhz)的值改为25，默认是12，
*              改成25是为了跟system_ARMCM3.c中定义的__SYSTEM_CLOCK相同，确保仿真的时候时钟一致
*/
int main(void)
{	
	OS_ERR err;	
	
	
	/* CPU初始化：1、初始化时间戳 */
	CPU_Init();	
	
	/* 关闭中断 */
	CPU_IntDis();
	
	/* 配置SysTick 10ms 中断一次 */
	OS_CPU_SysTickInit (10);
	
	/* 初始化相关的全局变量 */
	OSInit(&err);
	
	/* 创建任务 */
	OSTaskCreate( (OS_TCB       *)&Task1TCB, 
	              (OS_TASK_PTR   )Task1, 
                  (void         *)0,
				  (OS_PRIO       )1,
				  (CPU_STK      *)&Task1Stk[0],
				  (CPU_STK_SIZE  )TASK1_STK_SIZE,
				  (OS_TICK       )0,
				  (OS_ERR       *)&err );
				  
	OSTaskCreate( (OS_TCB       *)&Task2TCB, 
	              (OS_TASK_PTR   )Task2, 
                  (void         *)0,
				  (OS_PRIO       )2,
				  (CPU_STK      *)&Task2Stk[0],
				  (CPU_STK_SIZE  )TASK2_STK_SIZE,
				  (OS_TICK       )0,
				  (OS_ERR       *)&err );	
				  
	OSTaskCreate( (OS_TCB       *)&Task3TCB, 
	              (OS_TASK_PTR   )Task3, 
                  (void         *)0,
				  (OS_PRIO       )3,
				  (CPU_STK      *)&Task3Stk[0],
				  (CPU_STK_SIZE  )TASK3_STK_SIZE,
				  (OS_TICK       )0,
				  (OS_ERR       *)&err ); 
				  
	/* 启动OS，将不再返回 */				
	OSStart(&err);
}

/*
************************************************************************************************************************
*                                                    函数实现
************************************************************************************************************************
*/
/* 软件延时 */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}



void Task1( void *p_arg )
{	
	OS_ERR err;
	
	for( ;; )
    {
		flag1 = 1;
		OSTaskSuspend(&Task1TCB,&err);		
		flag1 = 0;
		OSTaskSuspend(&Task1TCB,&err);		
	}
}

void Task2( void *p_arg )
{
	OS_ERR err;
	
	for( ;; )
    {
		flag2 = 1;
		OSTimeDly(1);
		//OSTaskResume(&Task1TCB,&err);		
		flag2 = 0;
		OSTimeDly(1);;
		OSTaskResume(&Task1TCB,&err);
	}
}

void Task3( void *p_arg )
{
	for( ;; )
    {
		flag3 = 1;
		OSTimeDly(1);		
		flag3 = 0;
		OSTimeDly(1);
	}
}




