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
#include "los_queue.h"
#include "los_sem.h"
/* 板级外设头文件 */
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h" 
#include "bsp_exti.h"

/* 标准库头文件 */
#include <string.h>

/********************************** 任务句柄 *************************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */

/* 定义任务句柄 */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

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
 /* 定义消息队列的句柄 */
UINT32 Test_Queue_Handle;
/* 定义二值信号量的句柄 */
UINT32 BinarySem_Handle;

/******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
/* 定义消息队列长度 */
#define  TEST_QUEUE_LEN  	10
#define  TEST_QUEUE_SIZE  10

/* 函数声明 */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_LED_Task(void);
static UINT32 Creat_Key_Task(void);

static void LED_Task(void);
static void Key_Task(void);
static void BSP_Init(void);

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
 
extern char Usart_Rx_Buf[USART_RBUFF_SIZE];
	
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
	UINT32 uwRet = LOS_OK;  //定义一个任务创建的返回值，默认为创建成功
	
	/* 板级初始化，所有的跟开发板硬件相关的初始化都可以放在这个函数里面 */
  BSP_Init();
	
	printf("这是一个[野火]-STM32全系列开发板-LiteOS中断管理实验！\n");
	printf("按下KEY1或者KEY2发送队列消息\n");
	printf("Receive_Task任务接收到信息在串口回显\n\n");
	
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
	
	/* 创建一个测试队列*/
	uwRet = LOS_QueueCreate("Test_Queue",				/* 队列的名称 */
													TEST_QUEUE_LEN,			/* 队列的长度 */
													&Test_Queue_Handle,	/* 队列的ID(句柄) */
													0,						/* 队列模式，官方暂时未使用 */
													TEST_QUEUE_SIZE);	/* 节点大小，单位为字节 */
	if (uwRet != LOS_OK)
  {
		printf("Test_Queue队列创建失败！失败代码0x%X\n",uwRet);
		return uwRet;
  }
	
	/* 创建一个二值信号量*/
	uwRet = LOS_BinarySemCreate(0,&BinarySem_Handle);
	if (uwRet != LOS_OK)
	{
		printf("BinarySem创建失败！失败代码0x%X\n",uwRet);
	}
	
	uwRet = Creat_LED_Task();
  if (uwRet != LOS_OK)
  {
		printf("LED_Task任务创建失败！失败代码0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Key_Task();
  if (uwRet != LOS_OK)
  {
		printf("Key_Task任务创建失败！失败代码0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ 函数名  ： Creat_LED_Task
  * @ 功能说明： 创建LED_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_LED_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "LED_Task";/* 任务名 */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}
/*******************************************************************
  * @ 函数名  ： Creat_Key_Task
  * @ 功能说明： 创建Key_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Key_Task()
{
	// 定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Key_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Key_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&Key_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}

/******************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void LED_Task(void)
{
	/* 定义一个返回类型变量，初始化为LOS_OK */
	UINT32 uwRet = LOS_OK;
	UINT32 *r_queue;
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		/* 队列读取（接收），等待时间为一直等待 */
		uwRet = LOS_QueueRead(Test_Queue_Handle,	/* 读取（接收）队列的ID(句柄) */
													&r_queue,			/* 读取（接收）的数据保存位置 */
													TEST_QUEUE_SIZE,		/* 读取（接收）的数据的长度 */
													LOS_WAIT_FOREVER); 	/* 等待时间：一直等 */
		if(LOS_OK == uwRet)
		{
			printf("触发中断的是 KEY%d !\n",*r_queue);
		}
		else
		{
			printf("数据接收出错,错误代码0x%X\n",uwRet);
		}
	}
}
/******************************************************************
  * @ 函数名  ： Key_Task
  * @ 功能说明： Key_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Key_Task(void)
{			
	// 定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			

	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
    //获取二值信号量 xSemaphore,没获取到则一直等待
		uwRet = LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); 
    if(LOS_OK == uwRet)
    {
      printf("收到数据:%s\n",Usart_Rx_Buf);
      memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* 清零 */
      LED2_TOGGLE;
    }
		
	}
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

	/* DMA初始化	*/
	USARTx_DMA_Config();
	
	/* 串口初始化	*/
	USART_Config();
  
  /* 按键初始化	*/
  Key_GPIO_Config();
	
	/* 按键初始化	*/
	EXTI_Key_Config();
}


/*********************************************END OF FILE**********************/

