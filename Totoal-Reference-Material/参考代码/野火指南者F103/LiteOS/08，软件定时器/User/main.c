/***************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   STM32全系列开发板-LiteOS！
  **************************************************************
  * @attention
  *
  * 实验平台:野火 F103-全系列 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ***************************************************************
  */ 
 /* LiteOS 头文件 */
#include "los_sys.h"
#include "los_task.ph"
#include "los_swtmr.h"
/* 板级外设头文件 */
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h" 

/********************************** 任务句柄 *************************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */

/* 定义定时器句柄（ID） */
UINT16 Timer1_Handle;
UINT16 Timer2_Handle;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
static UINT32 TmrCb_Count1 = 0;
static UINT32 TmrCb_Count2 = 0;


/* 函数声明 */
static UINT32 AppTaskCreate(void);
static void Timer1_Callback(UINT32 arg);
static void Timer2_Callback(UINT32 arg);

static void LED_Task(void);
static void Key_Task(void);
static void BSP_Init(void);


/***************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
			第二步：创建APP应用任务
			第三步：启动LiteOS，开始多任务调度，启动失败则输出错误信息
  **************************************************************/
int main(void)
{	
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;  
	
	/* 板载相关初始化 */
  BSP_Init();
	
	printf("这是一个[野火]-STM32全系列开发板-LiteOS软件定时器实验！\n\n");
	printf("Timer1_Callback只执行一次就被销毁\n");
	printf("Timer2_Callback则循环执行\n");
	
	/* LiteOS 内核初始化 */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS 核心初始化失败！失败代码0x%X\n",uwRet);
		return LOS_NOK;
  }
	
	/* 创建APP应用任务，所有的应用任务都可以放在这个函数里面 */
	uwRet = AppTaskCreate();
	if (uwRet != LOS_OK)
  {
		printf("AppTaskCreate创建任务失败！失败代码0x%X\n",uwRet);
		return LOS_NOK;
  }

  /* 开启LiteOS任务调度 */
  LOS_Start();
	
	//正常情况下不会执行到这里
	while(1);
}


/*********************************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 任务创建，为了方便管理，所有的任务创建函数都可以放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
static UINT32 AppTaskCreate(void)
{
	/* 定义一个返回类型变量，初始化为LOS_OK */
	UINT32 uwRet = LOS_OK;
	
	/* 创建一个软件定时器定时器*/
	uwRet = LOS_SwtmrCreate(5000, 					/* 软件定时器的定时时间*/	
													LOS_SWTMR_MODE_ONCE, 	/* 软件定时器模式 一次模式 */
													(SWTMR_PROC_FUNC)Timer1_Callback,		/* 软件定时器的回调函数 */
													&Timer1_Handle,					/* 软件定时器的id */
													0);		
	if (uwRet != LOS_OK)
    {
		printf("软件定时器Timer1创建失败！\n");
    }
	uwRet = LOS_SwtmrCreate(1000, 					/* 软件定时器的定时时间（ms）*/	
													LOS_SWTMR_MODE_PERIOD, 	/* 软件定时器模式 周期模式 */
													(SWTMR_PROC_FUNC)Timer2_Callback,		/* 软件定时器的回调函数 */
													&Timer2_Handle,			/* 软件定时器的id */
													0);		
	if (uwRet != LOS_OK) 
  {
		printf("软件定时器Timer2创建失败！\n");
		return uwRet;
  }
	
	/* 启动一个软件定时器定时器*/
	uwRet = LOS_SwtmrStart(Timer1_Handle);
  if (LOS_OK != uwRet)
  {
		printf("start Timer1 failed\n");
		return uwRet;
  }
  else
  {
		printf("start Timer1 sucess\n");
  }
	/* 启动一个软件定时器定时器*/
	uwRet = LOS_SwtmrStart(Timer2_Handle);
  if (LOS_OK != uwRet)
  {
		printf("start Timer2 failed\n");
		return uwRet;
  }
	else
	{
		printf("start Timer2 sucess\n");
	}
	
	return LOS_OK;
}

/*********************************************************************************
  * @ 函数名  ： Timer1_Callback
  * @ 功能说明： 软件定时器回调函数1
  * @ 参数    ： 传入1个参数，但未使用  
  * @ 返回值  ： 无
  ********************************************************************************/
static void Timer1_Callback(UINT32 arg)
{
    UINT32 tick_num1;

    TmrCb_Count1++;						/* 每回调一次加一 */
		LED1_TOGGLE;
    tick_num1 = (UINT32)LOS_TickCountGet();	/* 获取滴答定时器的计数值 */
	
    printf("Timer_CallBack_Count1=%d\n", TmrCb_Count1);
    printf("tick_num1=%d\n", tick_num1);
}
/*********************************************************************************
  * @ 函数名  ： Timer2_Callback
  * @ 功能说明： 软件定时器回调函数2
  * @ 参数    ： 传入1个参数，但未使用  
  * @ 返回值  ： 无
  ********************************************************************************/
static void Timer2_Callback(UINT32 arg)
{
    UINT32 tick_num2;
	
    TmrCb_Count2++;				/* 每回调一次加一 */
		LED2_TOGGLE;
    tick_num2 = (UINT32)LOS_TickCountGet();	/* 获取滴答定时器的计数值 */
	
    printf("Timer_CallBack_Count2=%d\n", TmrCb_Count2);
	
    printf("tick_num2=%d\n", tick_num2);
    
}

/*******************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 串口初始化	*/
	USART_Config();

  /* 按键初始化 */
	Key_GPIO_Config();
}


/*********************************************END OF FILE**********************/

