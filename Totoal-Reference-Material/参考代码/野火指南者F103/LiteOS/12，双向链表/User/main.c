/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   野火STM32全系列开发板 + LiteOS
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
/* LiteOS 头文件 */
#include "los_sys.h"
#include "los_task.ph"
#include "los_memory.h"
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
UINT32 Test_Task_Handle;


/* 函数声明 */
extern LITE_OS_SEC_BSS UINT8* m_aucSysMem0;

static void AppTaskCreate(void);
static UINT32 Creat_Test_Task(void);
static void Test_Task(void);
static void BSP_Init(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
			第二步：创建APP应用任务
			第三步：启动LiteOS，开始多任务调度，启动不成功则输出错误信息
  */
int main(void)
{	
	UINT32 uwRet = LOS_OK;
	/* 板级初始化，所有的跟开发板硬件相关的初始化都可以放在这个函数里面 */
	BSP_Init();
	/* 发送一个字符串 */
	printf("这是一个[野火]-STM32全系列开发板-LiteOS的双向链表实验！\n");
	/* LiteOS 核心初始化 */
	uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
		printf("LiteOS 核心初始化失败！\n");
		return LOS_NOK;
    }
	/* 创建APP应用任务，所有的应用任务都可以放在这个函数里面 */
	AppTaskCreate();
	
	/* 开启LiteOS任务调度 */
	LOS_Start();
}
static void AppTaskCreate(void)
{
	UINT32 uwRet = LOS_OK;
   	/* 创建Test_Task任务 */
	uwRet = Creat_Test_Task();
    if (uwRet != LOS_OK)
    {
		printf("Test_Task任务创建失败！\n");
    }

}

/* 创建Test_Task任务*/
static UINT32 Creat_Test_Task(void)
{
    UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;				/* 优先级，数值越小，优先级越高 */
    task_init_param.pcName = "Test_Task";						/* 任务名 */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Test_Task;	/* 任务函数名 */
    task_init_param.uwStackSize = 1024;						/* 堆栈大小 */

    uwRet = LOS_TaskCreate(&Test_Task_Handle, &task_init_param);/* 创建任务 */
    return uwRet;
}



/*********************************************************************************
  * @ 函数名  ： Clear_Task
  * @ 功能说明： 写入已经初始化成功的内存池地址数据
  * @ 参数    ： void
  * @ 返回值  ： 无
  ********************************************************************************/
static void Test_Task(void)
{
	UINT32 uwRet = LOS_OK;			/* 定义一个初始化的返回类型，初始化为成功的返回值 */
	printf("\n双向链表初始化中......\n");
	
	LOS_DL_LIST *head;				/* 定义一个双向链表的头节点 */
	head = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* 动态申请头结点的内存 */

	LOS_ListInit(head);				/* 初始化双向链表 */
	if (!LOS_ListEmpty(head))		/* 判断是否初始化成功 */
	{
			printf("双向链表初始化失败!\n\n");
	}
	else
	{
		printf("双向链表初始化成功!\n\n");
	}
	
	printf("添加节点和尾节点添加......\n");			/* 插入节点：顺序插入与从末尾插入 */

    LOS_DL_LIST *node1 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* 动态申请第一个结点的内存 */
    LOS_DL_LIST *node2 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* 动态申请第二个结点的内存 */
    LOS_DL_LIST *tail = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* 动态申请尾结点的内存 */

	printf("添加第一个节点与第二个节点.....\n");
    LOS_ListAdd(head,node1);					/* 添加第一个节点，连接在头结点上 */
    LOS_ListAdd(node1,node2);					/* 添加第二个节点，连接在一个节点上 */
    if ((node1->pstPrev == head) || (node2->pstPrev == node1))	/* 判断是否插入成功 */
    {
        printf("添加节点成功!\n\n");
    }
	else
	{
		printf("添加节点失败!\n\n");
	}
	printf("将尾节点插入双向链表的末尾.....\n");
	LOS_ListTailInsert(head, tail);				/* 将尾节点插入双向链表的末尾 */
    if (tail->pstPrev == node2)					/* 判断是否插入成功 */		
    {
        printf("链表尾节点添加成功!\n\n");
    }
	else
	{
		printf("链表尾节点添加失败!\n\n");
	}
	
    printf("删除节点......\n");				/* 删除已有节点 */		
    LOS_ListDelete(node1);					/* 删除第一个节点 */	
    LOS_MemFree(m_aucSysMem0, node1);		/* 释放第一个节点的内存，保证后面的程序回收内存空间 */	
    if (head->pstNext == node2)				/* 判断是否删除成功 */
    {
        printf("删除节点成功\n\n");
    }
    else
    {
        printf("删除节点失败\n\n");
        
    }
	
	while(1)
	{
		LED2_TOGGLE;			//LED2翻转
		printf("任务运行中!\n");
		LOS_TaskDelay (2000);
	}
}


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

