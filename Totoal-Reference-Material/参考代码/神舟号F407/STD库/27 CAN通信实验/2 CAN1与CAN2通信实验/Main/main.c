#include "led.h"
#include "lcd.h"
#include "key.h"
#include "can1.h"
#include "can2.h"
#include "usart1.h"

/*********************************************************************************
*********************启明欣欣 STM32F407应用开发板(高配版)*************************
**********************************************************************************
* 文件名称: 例程13 CAN通信实验                                                   *
* 文件简述：can通信                                                              *
* 创建日期：2017.08.30                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：                                                                     *
* 淘宝店铺：https://shop125046348.taobao.com                                     *
* 声    明：本例程代码仅用于学习参考                                             *
**********************************************************************************
*********************************************************************************/

int main(void)
{ 
	u8 CAN1_mode=0; //CAN工作模式;0,普通模式;1,环回模式
	u8 CAN2_mode=0; //CAN工作模式;0,普通模式;1,环回模式
	
	u8 can1_sendbuf[8]="CAN1SEND";
	u8 can2_sendbuf[8]="CAN2SEND";
	
	u8 can1_RECbuf[8]={0};
	u8 can2_RECbuf[8]={0};
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();    //初始化延时函数
	LED_Init();			 //初始化LED 
 	LCD_Init();			 //LCD初始化 
	KEY_Init(); 		 //按键初始化  
	uart1_init(9600);
	
	CAN1_Mode_Init(CAN1_mode);//CAN初始化普通模式,波特率500Kbps   两个can通信
  CAN2_Mode_Init(CAN2_mode);//CAN初始化普通模式,波特率500Kbps 
  //则波特率为:42M/((1+6+7)*6)=500Kbps	
 	
 	BRUSH_COLOR=RED;//设置字体为红色 
	LCD_DisplayString(10,10,24,"Illuminati STM32F4");	
  LCD_DisplayString(10,40,16,"Author:Clever");
	LCD_DisplayString(30,60,24,"13.CAN TEST");

    CAN1_Send_Msg(can1_sendbuf,8);//发送8个字节   
	  delay_ms(10);
	  CAN2_Receive_Msg(can2_RECbuf); 
	  LCD_DisplayString(30,120,24,can2_RECbuf);   //液晶屏显示can2接收到的数值
	  uart1SendChars(can2_RECbuf,8);  //串口发出can2接收到的数值
  
/**************************************************************************************/

    CAN2_Send_Msg(can2_sendbuf,8);//发送8个字节   
    delay_ms(10);	
	  CAN1_Receive_Msg(can1_RECbuf); 
		LCD_DisplayString(30,180,24,can1_RECbuf); //液晶屏显示can1接收到的数值
	  uart1SendChars(can1_RECbuf,8);     //串口发出can1接收到的数值
 									  
while(1)
	{
		delay_ms(100);
		LED0=!LED0;//提示系统正在运行	   
	} 
}

