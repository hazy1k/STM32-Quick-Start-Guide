#include "usbplay.h"
#include "usb_app.h" 
#include "sdio_sdcard.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-USB连接 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

extern u8*const nes_remindmsg_tbl[5][GUI_LANGUAGE_NUM];//在nesplay.c里面定义
//USB连接  				 
u8 usb_play(void)
{			  
	u8 rval=0;			//返回值	  
 	u8 offline_cnt=0;
	u8 tct=0;
 	u8 USB_STA=0; 
	u8 busycnt=0;		//USB忙计数器
	u8 errcnt=0;		//USB错误计数器
	if(gui_phy.memdevflag&(1<<2))
	{
		window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)nes_remindmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		while(gui_phy.memdevflag&(1<<2))
		{
			delay_ms(5);//死循环等待U盘被拔出
		}
	}
	POINT_COLOR=BLUE;  
  	BACK_COLOR=LGRAY; 
	LCD_Clear(BACK_COLOR);
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[12][gui_phy.language],0X05);//显示标题 
	usbapp_mode_set(USBD_MSC_MODE);//DEVICE MSC 
	LCD_ShowString(60+(lcddev.width-240)/2,130+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB DisConnected ");//首先提示USB无连接
 	while(rval==0)
	{
 		delay_ms(1000/OS_TICKS_PER_SEC);//延时一个时钟节拍
  		if(system_task_return)//TPAD返回
		{	  
			if(busycnt)//USB正在读写
			{
				POINT_COLOR=RED;  					    
				LCD_ShowString(60+(lcddev.width-240)/2,110+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB BUSY!!!");//提示SD卡已经准备了
 				POINT_COLOR=BLUE;  
				system_task_return=0;//取消   
			}else break;//USB空闲,则退出USB	 		
		} 
		tct++; 
		if(tct==40)//每200ms进入一次
		{
			tct=0;
			if(busycnt)busycnt--;
			else gui_fill_rectangle(60+(lcddev.width-240)/2,110+(lcddev.height-320)/2,100,16,BACK_COLOR);//清除显示
			if(errcnt)errcnt--;
			else gui_fill_rectangle(60+(lcddev.width-240)/2,170+(lcddev.height-320)/2,128,16,BACK_COLOR);//清除显示
			if(usbx.bDeviceState&0x10)//有轮询操作
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				usbx.bDeviceState|=0X80;	//标记USB连接正常
				usbx.bDeviceState&=~(1<<4);	//清除轮询标志位
			}else
			{
				offline_cnt++;  
				if(offline_cnt>10)usbx.bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			} 
		} 
		if(USB_STA!=usbx.bDeviceState)//状态改变了 
		{	 	
			gui_fill_rectangle(60+(lcddev.width-240)/2,150+(lcddev.height-320)/2,120,16,BACK_COLOR);//清除显示 
			if(usbx.bDeviceState&0x01)//正在写		  
			{
				if(busycnt<5)busycnt++;
				LCD_ShowString(60+(lcddev.width-240)/2,150+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Writing...");//提示USB正在写入数据
			}
			if(usbx.bDeviceState&0x02)//正在读
			{					
				if(busycnt<5)busycnt++;
				LCD_ShowString(60+(lcddev.width-240)/2,150+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Reading...");//提示USB正在读出数据  
			}	
			POINT_COLOR=RED; 
			if(usbx.bDeviceState&0x04)
			{
				if(errcnt<5)errcnt++;
				LCD_ShowString(60+(lcddev.width-240)/2,170+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Write Error");//提示写入错误
 			}
			if(usbx.bDeviceState&0x08)
			{
				if(errcnt<5)errcnt++;
				LCD_ShowString(60+(lcddev.width-240)/2,170+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Read  Error");//提示读出错误
			}
			POINT_COLOR=BLUE;  
			if(usbx.bDeviceState&0X80)LCD_ShowString(60+(lcddev.width-240)/2,130+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Connected   ");//提示USB连接已经建立
 			else LCD_ShowString(60+(lcddev.width-240)/2,130+(lcddev.height-320)/2,lcddev.width,320,16,"USB DisConnected");//提示USB被拔出了
 			usbx.bDeviceState&=0X90;	//清除除连接状态&轮询标志外的其他所有位.
			USB_STA=usbx.bDeviceState;	//记录最后的状态
		}
	}	
	BACK_COLOR=BLACK;
	usbapp_mode_set(USBH_MSC_MODE);//HOST MSC 
	return rval; 
}







