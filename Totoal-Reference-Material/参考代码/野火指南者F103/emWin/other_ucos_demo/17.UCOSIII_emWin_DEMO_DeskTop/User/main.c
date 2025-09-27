/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "includes.h"

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/
static  OS_TCB	 AppTaskStartTCB;		     //定义任务控制块
static  OS_TCB   AppTaskUpdateTCB;
static  OS_TCB   AppTaskShotTCB;
static  OS_TCB   AppTaskUserappTCB;
/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/
static	CPU_STK	 AppTaskStartStk[APP_TASK_START_STK_SIZE];	   //定义任务堆栈
static  CPU_STK  AppTaskUpdateStk[APP_TASK_UPDATE_STK_SIZE];
static  CPU_STK  AppTaskCOMStk[APP_TASK_SHOT_STK_SIZE];
__align(8) static  CPU_STK  AppTaskUserappStk[APP_TASK_USERAPP_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void   AppTaskStart(void *p_arg);
static  void   AppTaskCreate(void);
static  void   AppObjCreate (void);
static  void   AppTaskUpdate(void *p_arg);
static  void   AppTaskShot(void *p_arg);
static  void   AppTaskUserapp(void *p_arg);

/*********************************************************************
*
*       data
*
**********************************************************************
*/
/*---------- kernel objects ----------*/
static  OS_SEM   SEM_SHOT;

/*---------- FatFS data ----------*/
FIL 		file;				/* file objects */
FRESULT result; 
UINT 		bw;  			/* File R/W count */

static uint8_t Pic_Name=0;
static KEY Key1,Key2;
uint8_t key_flag=1;//1：允许按键截图，0：禁用按键截图
uint8_t IsCal =0;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	OS_ERR err;	
	
	/* 初始化"uC/OS-III"内核 */  
	OSInit(&err);	
	
	/*创建任务*/
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,              // 任务控制块指针          
               (CPU_CHAR   *)"App Task Start",		          // 任务名称
               (OS_TASK_PTR )AppTaskStart, 	                  // 任务代码指针
               (void       *)0,			                      // 传递给任务的参数parg
               (OS_PRIO     )APP_TASK_START_PRIO,			  // 任务优先级
               (CPU_STK    *)&AppTaskStartStk[0],	          // 任务堆栈基地址
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE/10,	  // 堆栈剩余警戒线
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,		  // 堆栈大小
               (OS_MSG_QTY  )1u,			                      // 可接收的最大消息队列数
               (OS_TICK     )0u,			                      // 时间片轮转时间
               (void       *)0,			                      // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	      // 任务选项
               (OS_ERR     *)&err);		                    // 返回值
	  
  /* 启动多任务系统，控制权交给uC/OS-III */
  OSStart(&err);  
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static void  AppTaskStart(void *p_arg)
{
    OS_ERR      err;

   (void)p_arg;
	
	/* 板级初始化 */	
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();
		BSP_Tick_Init();

//    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif
    
    OS_INFO("Creating Application Tasks...\n\r");
    AppTaskCreate();                                            /* Create Application Tasks                             */
    OS_INFO("Creating Application Events...\n\r");
    AppObjCreate();                                             /* Create Application Objects  */
	
	/*Delete task*/
//	OSTaskDel(&AppTaskStartTCB,&err);	
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        GUI_TOUCH_Exec();
				Key_RefreshState(&Key2);//刷新按键状态
				if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0 && key_flag)//按键被按下过
				{
					OSSemPost ((OS_SEM  *)&SEM_SHOT,
							 (OS_OPT   )OS_OPT_POST_ALL,
							 (OS_ERR  *)&err);
					Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
				}
        OSTimeDlyHMSM(0, 0, 0, 15,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskGUIUpdate
*	功能说明: 
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/
static void AppTaskUpdate(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
	
	while(TPAD_Init())
	{
		bsp_DelayUS(5*1000);
	}	
	
	while(1)
	{	
		if(TPAD_Scan(0))
		{
			//LED1_TOGGLE;
			macBEEP_ON();
		}
		OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
		macBEEP_OFF();		
		//printf("cpuusage:%d\n",OSStatTaskCPUUsage);
	}   
}

/*
*********************************************************************************************************
*	函 数 名: _WriteByte2File()
*	功能说明: 
*	形    参：  	
*	返 回 值: 
*********************************************************************************************************
*/
static void _WriteByte2File(uint8_t Data, void * p) 
{
	result = f_write(p, &Data, 1, &bw);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/
static void AppTaskShot(void *p_arg)
{	
	OS_ERR  err;
	CPU_TS 	Ts;
	uint8_t i;
	char 	buf[20];
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
		//阻塞任务直到按键被按下！
		OSSemPend((OS_SEM   *)&SEM_SHOT,
						 (OS_TICK   )0,                     //如果这个参数设置为0就一直等待下去
						 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果没有信号量可用就等待
						 (CPU_TS   *)&Ts,             //这个参数是指向存放信号量被提交、强制解除等待状态、或者信号量被删除的时间戳        
						 (OS_ERR   *)&err);
		i=0;
		while(i<0xff)
		{
			sprintf((char *)buf,"0:/picture%d.bmp",i);
			result=f_open(&file,(const TCHAR*)buf,FA_READ);
			if(result==FR_NO_FILE)break;	
			else
				f_close(&file);
			i++;
		}
		OSSchedLock(&err);
		printf("start shot picture\n\r");
		/* 创建截图 */
		result = f_open(&file,buf, FA_WRITE|FA_CREATE_ALWAYS);
		/* 向SD卡绘制BMP图片 */
		GUI_BMP_Serialize(_WriteByte2File,&file);
		
		/* 创建完成后关闭file */
		result = f_close(&file);
		printf("shot picture success\n\r");
		Pic_Name++;
		OSSchedUnlock(&err);	
	}  
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: LED4闪烁
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
static void AppTaskUserapp(void *p_arg)
{
	(void)p_arg;		/* 避免编译器告警 */
	if(IsCal!=0x55)
	{
		Touch_MainTask();
	}
	while (1) 
	{   		
		UserAPP();
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate(void)
{
	OS_ERR      err;
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskUpdateTCB,             
                 (CPU_CHAR     *)"App Task Update",
                 (OS_TASK_PTR   )AppTaskUpdate, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_UPDATE_PRIO,
                 (CPU_STK      *)&AppTaskUpdateStk[0],
                 (CPU_STK_SIZE  )APP_TASK_UPDATE_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_UPDATE_STK_SIZE,
                 (OS_MSG_QTY    )1,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskShotTCB,            
                 (CPU_CHAR     *)"App Task SHOT",
                 (OS_TASK_PTR   )AppTaskShot, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_SHOT_PRIO,
                 (CPU_STK      *)&AppTaskCOMStk[0],
                 (CPU_STK_SIZE  )APP_TASK_SHOT_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_SHOT_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskUserappTCB,             
                 (CPU_CHAR     *)"App Task Userapp",
                 (OS_TASK_PTR   )AppTaskUserapp, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_USERAPP_PRIO,
                 (CPU_STK      *)&AppTaskUserappStk[0],
                 (CPU_STK_SIZE  )APP_TASK_USERAPP_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_USERAPP_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);				 
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
	OS_ERR      err;
	
  //创建一个按键，按键的具体细节如无兴趣可不管
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	//创建一个信号量，按键按下的时候post一个信号量给其他任务
	OSSemCreate ((OS_SEM *)&SEM_SHOT,     //指向信号量变量的指针
			   (CPU_CHAR    *)"SEM_SHOT",    //信号量的名字
			   (OS_SEM_CTR   )0,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
			   (OS_ERR      *)&err);         //错误类型
}

/*********************************************END OF FILE**********************/
