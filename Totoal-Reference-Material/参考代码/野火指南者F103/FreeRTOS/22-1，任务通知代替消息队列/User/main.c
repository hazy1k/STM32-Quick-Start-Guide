/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 任务通知代替消息队列
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 STM32全系列开发板 
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
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "limits.h"
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t Receive1_Task_Handle = NULL;/* Receive1_Task任务句柄 */
static TaskHandle_t Receive2_Task_Handle = NULL;/* Receive2_Task任务句柄 */
static TaskHandle_t Send_Task_Handle = NULL;/* Send_Task任务句柄 */

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


/******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
#define  USE_CHAR  0  /* 测试字符串的时候配置为 1 ，测试变量配置为 0  */

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void Receive1_Task(void* pvParameters);/* Receive1_Task任务实现 */
static void Receive2_Task(void* pvParameters);/* Receive2_Task任务实现 */

static void Send_Task(void* pvParameters);/* Send_Task任务实现 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */

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
	printf("这是一个[野火]-STM32全系列开发板-FreeRTOS任务通知代替消息队列实验！\n");
  printf("按下KEY1或者KEY2进行任务消息通知发送 \n");
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区

  /* 创建Receive1_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Receive1_Task, /* 任务入口函数 */
                        (const char*    )"Receive1_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Receive1_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建Receive1_Task任务成功!\r\n");
  
  /* 创建Receive2_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Receive2_Task, /* 任务入口函数 */
                        (const char*    )"Receive2_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )3,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Receive2_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建Receive2_Task任务成功!\r\n");
  
  /* 创建Send_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Send_Task,  /* 任务入口函数 */
                        (const char*    )"Send_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )4, /* 任务的优先级 */
                        (TaskHandle_t*  )&Send_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建Send_Task任务成功!\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}



/**********************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Receive1_Task(void* parameter)
{	
  BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
#if USE_CHAR
  char *r_char;
#else
  uint32_t r_num;
#endif
  while (1)
  {
    /* BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                                  uint32_t ulBitsToClearOnExit, 
                                  uint32_t *pulNotificationValue, 
                                  TickType_t xTicksToWait ); 
     * ulBitsToClearOnEntry：当没有接收到任务通知的时候将任务通知值与此参数的取
       反值进行按位与运算，当此参数为Oxfffff或者ULONG_MAX的时候就会将任务通知值清零。
     * ulBits ToClearOnExit：如果接收到了任务通知，在做完相应的处理退出函数之前将
       任务通知值与此参数的取反值进行按位与运算，当此参数为0xfffff或者ULONG MAX的时候
       就会将任务通知值清零。
     * pulNotification Value：此参数用来保存任务通知值。
     * xTick ToWait：阻塞时间。
     *
     * 返回值：pdTRUE：获取到了任务通知。pdFALSE：任务通知获取失败。
     */
    //获取任务通知 ,没获取到则一直等待
		xReturn=xTaskNotifyWait(0x0,			//进入函数的时候不清除任务bit
                            ULONG_MAX,	  //退出函数的时候清除所有的bit
#if USE_CHAR
                            (uint32_t *)&r_char,		  //保存任务通知值
#else
                            &r_num,		  //保存任务通知值
#endif                        
                            portMAX_DELAY);	//阻塞时间
    if( pdTRUE == xReturn )
#if USE_CHAR
      printf("Receive1_Task 任务通知消息为 %s \n",r_char);                      
#else
      printf("Receive1_Task 任务通知消息为 %d \n",r_num);                      
#endif  
     
    
		LED1_TOGGLE;
  }
}

/**********************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Receive2_Task(void* parameter)
{	
  BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
#if USE_CHAR
  char *r_char;
#else
  uint32_t r_num;
#endif
  while (1)
  {
    /* BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                                  uint32_t ulBitsToClearOnExit, 
                                  uint32_t *pulNotificationValue, 
                                  TickType_t xTicksToWait ); 
     * ulBitsToClearOnEntry：当没有接收到任务通知的时候将任务通知值与此参数的取
       反值进行按位与运算，当此参数为Oxfffff或者ULONG_MAX的时候就会将任务通知值清零。
     * ulBits ToClearOnExit：如果接收到了任务通知，在做完相应的处理退出函数之前将
       任务通知值与此参数的取反值进行按位与运算，当此参数为0xfffff或者ULONG MAX的时候
       就会将任务通知值清零。
     * pulNotification Value：此参数用来保存任务通知值。
     * xTick ToWait：阻塞时间。
     *
     * 返回值：pdTRUE：获取到了任务通知。pdFALSE：任务通知获取失败。
     */
    //获取任务通知 ,没获取到则一直等待
		xReturn=xTaskNotifyWait(0x0,			//进入函数的时候不清除任务bit
                            ULONG_MAX,	  //退出函数的时候清除所有的bit
#if USE_CHAR
                            (uint32_t *)&r_char,		  //保存任务通知值
#else
                            &r_num,		  //保存任务通知值
#endif                        
                            portMAX_DELAY);	//阻塞时间
    if( pdTRUE == xReturn )
#if USE_CHAR
      printf("Receive2_Task 任务通知消息为 %s \n",r_char);                      
#else
      printf("Receive2_Task 任务通知消息为 %d \n",r_num);                      
#endif  
		LED2_TOGGLE;
  }
}

/**********************************************************************
  * @ 函数名  ： Send_Task
  * @ 功能说明： Send_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Send_Task(void* parameter)
{	 
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
#if USE_CHAR
  char test_str1[] = "this is a mail test 1";/* 邮箱消息test1 */
  char test_str2[] = "this is a mail test 2";/* 邮箱消息test2 */
#else
  uint32_t send1 = 1;
  uint32_t send2 = 2;
#endif
  

  
  while (1)
  {
    /* KEY1 被按下 */
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {
      /* 原型:BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify, 
                                      uint32_t ulValue, 
                                      eNotifyAction eAction ); 
       * eNoAction = 0，通知任务而不更新其通知值。
       * eSetBits，     设置任务通知值中的位。
       * eIncrement，   增加任务的通知值。
       * eSetvaluewithoverwrite，覆盖当前通知
       * eSetValueWithoutoverwrite 不覆盖当前通知
       * 
       * pdFAIL：当参数eAction设置为eSetValueWithoutOverwrite的时候，
       * 如果任务通知值没有更新成功就返回pdFAIL。
       * pdPASS: eAction 设置为其他选项的时候统一返回pdPASS。
      */
      xReturn = xTaskNotify( Receive1_Task_Handle, /*任务句柄*/
#if USE_CHAR 
                             (uint32_t)&test_str1, /* 发送的数据，最大为4字节 */
#else
                              send1, /* 发送的数据，最大为4字节 */
#endif
                             eSetValueWithOverwrite );/*覆盖当前通知*/
      
      if( xReturn == pdPASS )
        printf("Receive1_Task_Handle 任务通知消息发送成功!\r\n");
    } 
    /* KEY2 被按下 */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )
    {
      xReturn = xTaskNotify( Receive2_Task_Handle, /*任务句柄*/
#if USE_CHAR 
                             (uint32_t)&test_str2, /* 发送的数据，最大为4字节 */
#else
                              send2, /* 发送的数据，最大为4字节 */
#endif
                             eSetValueWithOverwrite );/*覆盖当前通知*/
      /* 此函数只会返回pdPASS */
      if( xReturn == pdPASS )
        printf("Receive2_Task_Handle 任务通知消息发送成功!\r\n");
    }
    vTaskDelay(20);
  }
}
/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
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
  
  /* 按键初始化	*/
  Key_GPIO_Config();

}

/********************************END OF FILE****************************/
