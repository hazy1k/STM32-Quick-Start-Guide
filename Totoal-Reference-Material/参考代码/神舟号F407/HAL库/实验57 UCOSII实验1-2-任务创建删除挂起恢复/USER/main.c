#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "includes.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验57
 UCOSII实验1-2 任务创建，删除，挂起，恢复 -任务调度-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//START 任务
//设置任务优先级
#define START_TASK_PRIO			10  //开始任务的优先级为最低
//设置任务堆栈大小
#define START_STK_SIZE			128
//任务任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);

//LED任务
//设置任务优先级
#define LED_TASK_PRIO			7
//设置任务堆栈大小
#define LED_STK_SIZE			128
//任务堆栈
OS_STK LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *pdata);

//蜂鸣器任务
//设置任务优先级
#define BEEP_TASK_PRIO          5 
//设置任务堆栈大小
#define BEEP_STK_SIZE  	        128
//创建任务堆栈空间	
OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//任务函数接口
void beep_task(void *pdata);

//按键扫描任务
//设置任务优先级
#define KEY_TASK_PRIO       	3
//设置任务堆栈大小
#define KEY_STK_SIZE  	        128
//创建任务堆栈空间	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数接口
void key_task(void *pdata);

int main(void)
{ 
    HAL_Init();                    	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);              //初始化USART
	LED_Init();						//初始化LED	
	KEY_Init();                    	//初始化按键
	BEEP_Init();					//初始化蜂鸣器
	
	OSInit();                       //UCOS初始化
    OSTaskCreateExt((void(*)(void*) )start_task,                //任务函数
                    (void*          )0,                         //传递给任务函数的参数
                    (OS_STK*        )&START_TASK_STK[START_STK_SIZE-1],//任务堆栈栈顶
                    (INT8U          )START_TASK_PRIO,           //任务优先级
                    (INT16U         )START_TASK_PRIO,           //任务ID，这里设置为和优先级一样
                    (OS_STK*        )&START_TASK_STK[0],        //任务堆栈栈底
                    (INT32U         )START_STK_SIZE,            //任务堆栈大小
                    (void*          )0,                         //用户补充的存储区
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//任务选项,为了保险起见，所有任务都保存浮点寄存器的值
	OSStart(); //开始任务
}

//开始任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
	
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
    //LED任务
    OSTaskCreateExt((void(*)(void*) )led_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED_TASK_STK[LED_STK_SIZE-1],
                    (INT8U          )LED_TASK_PRIO,            
                    (INT16U         )LED_TASK_PRIO,            
                    (OS_STK*        )&LED_TASK_STK[0],         
                    (INT32U         )LED_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//BEEP任务
    OSTaskCreateExt((void(*)(void*) )beep_task,                 
                    (void*          )0,
                    (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                    (INT8U          )BEEP_TASK_PRIO,            
                    (INT16U         )BEEP_TASK_PRIO,            
                    (OS_STK*        )&BEEP_TASK_STK[0],         
                    (INT32U         )BEEP_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//按键任务
    OSTaskCreateExt((void(*)(void*) )key_task,                 
                    (void*          )0,
                    (OS_STK*        )&KEY_TASK_STK[KEY_STK_SIZE-1],
                    (INT8U          )KEY_TASK_PRIO,          
                    (INT16U         )KEY_TASK_PRIO,            
                    (OS_STK*        )&KEY_TASK_STK[0],         
                    (INT32U         )KEY_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    OS_EXIT_CRITICAL();             //退出临界区(开中断)
	OSTaskSuspend(START_TASK_PRIO); //挂起开始任务
}
 

//LED任务
void led_task(void *pdata)
{   
	while(1)
	{  
        LED0=!LED0;
        LED1=!LED1;
        delay_ms(500);
	}									 
}	   

//蜂鸣器任务
void beep_task(void *pdata)
{
	while(1)
	{  	
        if(OSTaskDelReq(OS_PRIO_SELF)==OS_ERR_TASK_DEL_REQ) //判断是否有删除请求  
        {
            OSTaskDel(OS_PRIO_SELF);			            //删除任务本身TaskLed
        }
		BEEP=1;												//打开蜂鸣器
        delay_ms(60);
		BEEP=0;												//关闭蜂鸣器                       
        delay_ms(940);
	}									 
}

//按键扫描任务
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
            OSTaskSuspend(LED_TASK_PRIO);   //挂起LED任务，LED停止闪烁
		}
		else if (key==KEY2_PRES)
		{
            OSTaskResume(LED_TASK_PRIO);	//恢复LED任务，LED恢复闪烁
		}
		else if (key==WKUP_PRES)
		{
            OSTaskDelReq(BEEP_TASK_PRIO);	//发送删除BEEP任务请求，任务睡眠，无法恢复
		}
		else if(key==KEY1_PRES)
		{				   				   
            //重新创建任务beep	 
            OSTaskCreateExt((void(*)(void*) )beep_task,                 
                            (void*          )0,
                            (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                            (INT8U          )BEEP_TASK_PRIO,            
                            (INT16U         )BEEP_TASK_PRIO,            
                            (OS_STK*        )&BEEP_TASK_STK[0],         
                            (INT32U         )BEEP_STK_SIZE,             
                            (void*          )0,                         
                            (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
        }   
 		delay_ms(10);
	}
}
