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
#include "los_membox.h"
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


/******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
/* 相关宏定义 */
#define  MEM_BOXSIZE   128		//内存池大小
#define  MEM_BLKSIZE   16			//内存块大小


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
 static UINT32 BoxMem[MEM_BOXSIZE*MEM_BLKSIZE];
 
  UINT32 *p_Num = NULL;			//指向读写内存池地址的指针
  UINT32 *p_Initial = NULL;		//保存初始指针
	
	
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
	
	/* 板载相关初始化 */
  BSP_Init();
	
	printf("这是一个[野火]-STM32全系列开发板-LiteOS内存管理实验！\n");
	printf("按下KEY1申请内存，按下KEY2释放内存！\n\n");
	/* LiteOS 内核初始化 */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS 核心初始化失败！失败代码0x%X\n",uwRet);
		return LOS_NOK;
  }
	
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
	// 定义一个事件接收变量
	UINT32 uwRet;			
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
    LED2_TOGGLE;
    LOS_TaskDelay(1000);/* 延时1000个tick */
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
	
	printf("正在初始化静态内存池....................\n");
	/* 初始化内存池 */
	uwRet = LOS_MemboxInit(	&BoxMem[0],			/* 内存池地址 */
													MEM_BOXSIZE,		/* 内存池大小 */
													MEM_BLKSIZE);		/* 内存块大小 */
	if (uwRet != LOS_OK)
		printf("内存池初始化失败\n\n");
	else
		printf("内存池初始化成功!\n\n");
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		/* KEY1 被按下 */
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       
		{
      if(NULL == p_Num)
      {
				printf("正在向内存池申请内存....................\n");  
					
				/* 向已经初始化的内存池申请内存 */
				p_Num = (UINT32*)LOS_MemboxAlloc(BoxMem);
					
				if (NULL == p_Num)
					printf("申请内存失败!\n");
				else
				{
					printf("申请内存成功!地址为0x%X \n",(uint32_t)p_Num);
					
					//向Test_Ptr中写入当数据:当前系统时间
					sprintf((char*)p_Num,"当前系统TickCount = %d",(UINT32)LOS_TickCountGet());
					printf("写入的数据是 %s \n\n",(char*)p_Num);
				}
			}
			else
				printf("请先按下KEY2释放内存再申请\n");
		}

    /* KEY2 被按下 */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )      
		{
      if(NULL != p_Num)
      {
				printf("清除前内存信息是 %s ,地址为0x%X \n", (char*)p_Num,(uint32_t)p_Num);
				printf("正在清除p_Num的内容......................\n");
				LOS_MemboxClr(BoxMem, p_Num);		/* 清除在p_Num地址的内容 */
				printf("清除后内存信息是 %s ,地址为0x%X \n\n", (char*)p_Num,(uint32_t)p_Num);
				
				printf("正在释放内存.............................\n");
				uwRet = LOS_MemboxFree(BoxMem, p_Num);
				if (LOS_OK == uwRet)
				{
					printf("内存释放成功!\n");//内存释放成功！
					p_Num = NULL;
				}
				else
				{
					printf("内存释放失败!\n");//内存释放失败！
				}
			}
			else
        printf("请先按下KEY1申请内存再释放\n");
		}
		
		LOS_TaskDelay(20);     //每20ms扫描一次		
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

	/* 串口初始化	*/
	USART_Config();

  /* 按键初始化 */
	Key_GPIO_Config();
}


/*********************************************END OF FILE**********************/

