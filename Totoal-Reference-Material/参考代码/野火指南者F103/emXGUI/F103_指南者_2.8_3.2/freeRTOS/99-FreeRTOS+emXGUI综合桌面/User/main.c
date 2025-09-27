/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 任务管理
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"   
#include "./usart/bsp_usart.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */


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


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void GUI_Thread_Entry(void* pvParameters);/* Test_Task任务实现 */
static void DEBUG_Thread_Entry(void* parameter);

static void BSP_Init(void);
  
/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 开发板硬件初始化 */
  BSP_Init();  
  
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* 任务入口函数 */
                        (const char*    )"gui",/* 任务名字 */
                        (uint16_t       )1024/2,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )10, /* 任务的优先级 */
                        (TaskHandle_t*  )NULL);/* 任务控制块指针 */ 
//  xReturn = xTaskCreate((TaskFunction_t )DEBUG_Thread_Entry,  /* 任务入口函数 */
//                        (const char*    )"debug",/* 任务名字 */
//                        (uint16_t       )256,  /* 任务栈大小 */
//                        (void*          )NULL,/* 任务入口函数参数 */
//                        (UBaseType_t    )3, /* 任务的优先级 */
//                        (TaskHandle_t*  )NULL);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}

extern void GUI_Startup(void);

/**********************************************************************
  * @ 函数名  ： gui_thread_entry
  * @ 功能说明： gui_thread_entry任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void GUI_Thread_Entry(void* parameter)
{	
  printf("野火emXGUI演示例程\n\n");

  /* 执行本函数不会返回 */
	GUI_Startup();
  
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* 延时500个tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* 延时500个tick */
  }
}

void FSMC_Init(void);
void FSMC_InitLCD(void);

/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/* 硬件BSP初始化统统放在这里，比如LED，串口，LCD等 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	
	/* usart 端口初始化 */
  Debug_USART_Config();
  
  FSMC_Init();
	FSMC_InitLCD();

  /* KEY 端口初始化 */
  Key_GPIO_Config();
}
 
static void DEBUG_Thread_Entry(void* parameter)
{
/* 任务DEBUG */
	char tasks_buf[360] = {0};
	uint8_t tog=0;
	
	while(1)
{
	if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN))
	{
		memset(tasks_buf, 0, 360);
	#if 0
		strcat((char *)tasks_buf, "任务名称\t运行计数\t使用率\r\n" );

		strcat((char *)tasks_buf, "---------------------------------------------\r\n");

		/* displays the amount of time each task has spent in the Running state

		* in both absolute and percentage terms. */

		vTaskGetRunTimeStats((char *)(tasks_buf + strlen(tasks_buf)));

		strcat((char *)tasks_buf, "\r\n");
		printf("%s\r\n",tasks_buf);

		memset(tasks_buf, 0, 512);
	#endif
		strcat((char *)tasks_buf, "\n任务名称\t运行状态\t优先级\t剩余堆栈\t任务序号\r\n" );

		strcat((char *)tasks_buf, "---------------------------------------------\r\n");

		vTaskList((char *)(tasks_buf + strlen(tasks_buf)));

		strcat((char *)tasks_buf, "\r\n---------------------------------------------\r\n");

		strcat((char *)tasks_buf, "B : 阻塞,\t R : 就绪,\t D : 删除,\t S : 暂停\r\n");
		printf("%s\r\n",tasks_buf);
		
		printf("剩余总的堆空间为:%d Byte\n",xPortGetFreeHeapSize());
	//	unsigned portBASE_TYPE uxHighWaterMark;
	//	uxHighWaterMark=uxTaskGetStackHighWaterMark( pvCreatedTask_ToggleLed4 );
	//　printf("剩余栈空间是多少：%ld.\r\n",uxHighWaterMark);
		vTaskDelay(1000);
	}else
	{
	vTaskDelay(1);
	}
}
}
/*********************************************END OF FILE**********************/

