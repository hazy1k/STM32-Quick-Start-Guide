#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "key.h"
#include "beep.h"
//ALIENTEK 探索者STM32F407开发板 实验56
//UCOSII实验1-2 任务创建，删除，挂起，恢复  -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//创建任务堆栈空间	
__align(8) OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);	
 			   
//LED任务
//设置任务优先级
#define LED_TASK_PRIO       			7 
//设置任务堆栈大小
#define LED_STK_SIZE  		    		64
//创建任务堆栈空间	
__align(8) OS_STK LED_TASK_STK[LED_STK_SIZE];
//任务函数接口
void led_task(void *pdata);


//蜂鸣器任务
//设置任务优先级
#define BEEP_TASK_PRIO       			5 
//设置任务堆栈大小
#define BEEP_STK_SIZE  					64
//创建任务堆栈空间	
__align(8) OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//任务函数接口
void beep_task(void *pdata);


//按键扫描任务
//设置任务优先级
#define KEY_TASK_PRIO       			3 
//设置任务堆栈大小
#define KEY_STK_SIZE  					64
//创建任务堆栈空间	
__align(8) OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数接口
void key_task(void *pdata);

int main(void)
{ 
 
	delay_init(168);		  //初始化延时函数 
	uart_init(115200);
	LED_Init();		  		//初始化与LED连接的硬件接口
 	BEEP_Init();			//蜂鸣器初始化	
	KEY_Init();				//按键初始化
	OSInit();  	 			//初始化UCOSII		 			  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	
}

//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led_task,(void *)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);						    				   
 	OSTaskCreate(beep_task,(void *)0,(OS_STK*)&BEEP_TASK_STK[BEEP_STK_SIZE-1],BEEP_TASK_PRIO);	 				   				   
 	OSTaskCreate(key_task,(void *)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);	 				   
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
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
			OSTaskDel(OS_PRIO_SELF);						   //删除任务本身TaskLed
		}
		BEEP=1;
		delay_ms(60);
		BEEP=0;
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
		  OSTaskSuspend(LED_TASK_PRIO);//挂起LED任务，LED停止闪烁
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
		 OSTaskCreate(beep_task,(void *)0,(OS_STK*)&BEEP_TASK_STK[BEEP_STK_SIZE-1],BEEP_TASK_PRIO);//重新创建任务beep	 				   				   
		}   
 		delay_ms(10);
	}
}



