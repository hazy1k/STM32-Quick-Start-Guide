#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "usmart.h"
#include "timer.h"
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "sram.h"
#include "malloc.h"
#include "beep.h"
#include "lwip_comm.h"
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "udp_demo.h"
#include "httpd.h" 
//ALIENTEK 探索者STM32F407开发板 实验55
//网络通信实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 
  
//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//得到网速
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		LCD_ShowString(30,170,200,16,16,"KEY0:TCP Server Test");
		LCD_ShowString(30,190,200,16,16,"KEY1:TCP Client Test");
		LCD_ShowString(30,210,200,16,16,"KEY2:UDP Test");
	}
}

int main(void)
{ 
	u8 t;
	u8 key;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
	usmart_dev.init(84);		//初始化USMART
	LED_Init();  				//LED初始化
	KEY_Init();  				//按键初始化
	LCD_Init(); 				//LCD初始化
	BEEP_Init();				//蜂鸣器初始化
	RTC_Init();  				//RTC初始化
	Adc_Init();  				//ADC初始化 
	TIM3_Int_Init(100-1,8400-1);//10khz的频率,计数100为10ms
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	POINT_COLOR=RED; 			//红色字体
	lwip_test_ui(1);			//加载前半部分UI
	//先初始化lwIP(包括LAN8720初始化),此时必须插上网线,否则初始化会失败!! 
	LCD_ShowString(30,110,200,16,16,"lwIP Initing...");
	while(lwip_comm_init()!=0)
	{
		LCD_ShowString(30,110,200,16,16,"lwIP Init failed!");
		delay_ms(1200);
		LCD_Fill(30,110,230,110+16,WHITE);//清除显示
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
	//等待DHCP获取 
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();
	}
	lwip_test_ui(2);//加载后半部分UI 
	httpd_init();	//HTTP初始化(默认开启websever)
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://TCP Server模式
				tcp_server_test();
 				lwip_test_ui(3);//重新加载UI
				break;
			case KEY1_PRES://TCP Client模式
				tcp_client_test();
				lwip_test_ui(3);//重新加载UI
				break; 
			case KEY2_PRES://UDP模式
				udp_demo_test();
				lwip_test_ui(3);//重新加载UI
				break; 
		}
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if(t==100)LCD_ShowString(30,230,200,16,16,"Please choose a mode!");
		if(t==200)
		{ 
			t=0;
			LCD_Fill(30,230,230,230+16,WHITE);//清除显示
			LED0=!LED0;
		} 
	} 
}


















