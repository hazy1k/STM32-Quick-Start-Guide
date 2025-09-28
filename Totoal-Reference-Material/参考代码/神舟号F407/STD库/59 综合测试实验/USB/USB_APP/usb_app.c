#include "usb_app.h" 
#include "delay.h"   
#include "malloc.h"    
#include "exfuns.h"      
#include "includes.h"      
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//USB-APP 代码	   
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

//USB HOST
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core_dev; 
_usb_app usbx;//USB APP控制器 


//USB OTG 中断服务函数
//处理所有USB中断
void OTG_FS_IRQHandler(void)
{ 
	OSIntEnter();    		    
	if (USB_OTG_IsHostMode(&USB_OTG_Core_dev)) //确定是不是USB主机模式?
	{  
		USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);//USB主机中断
	}
	else
	{
		USBD_OTG_ISR_Handler(&USB_OTG_Core_dev);//USB丛机中断
	}
	OSIntExit();  											 
}  
//初始化USB
void usbapp_init(void)
{
	usbx.bDeviceState=0;
	usbx.hdevclass=0;
	usbx.mode=0XFF; 	//设置为一个非法的模式,必须先调用usbapp_mode_set设置模式,才能正常工作
} 
extern HID_Machine_TypeDef HID_Machine;	
//USB轮询函数,必须周期性的被调用.
void usbapp_pulling(void)
{
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			if(HCD_IsDeviceConnected(&USB_OTG_Core_dev)==0||usbx.hdevclass!=1)//U盘连接失败了,需要重新处理USB事务
			{
				USBH_Process(&USB_OTG_Core_dev, &USB_Host);//处理USB事务
				if(USBH_Check_EnumeDead(&USB_Host))	//检测USB HOST 是否死机了?死机了,则重新初始化 
				{ 	    
					usbapp_mode_set(USBH_MSC_MODE);//重连
				}	
			}			
			break;
		case USBH_HID_MODE: 
			USBH_Process(&USB_OTG_Core_dev, &USB_Host); 
			if(usbx.bDeviceState&0X80)//连接建立了
			{ 
				if(USBH_Check_HIDCommDead(&USB_OTG_Core_dev,&HID_Machine))//检测USB HID通信,是否还正常? 
				{ 	    
					usbapp_mode_set(USBH_HID_MODE);//重连
				}				
				
			}else	//连接未建立的时候,检测
			{
				if(USBH_Check_EnumeDead(&USB_Host))	//检测USB HOST 是否死机了?死机了,则重新初始化 
				{ 	    
					usbapp_mode_set(USBH_HID_MODE);//重连
				}			
			}		
			break;		
		case USBD_MSC_MODE:
			break;	  
	}	
}
//USB结束当前工作模式
void usbapp_mode_stop(void)
{
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			myfree(SRAMIN,MSC_Machine.buff);	//释放内存
			myfree(SRAMIN,USBH_DataInBuffer);	//释放内存
			myfree(SRAMIN,USBH_DataOutBuffer);	//释放内存
		case USBH_HID_MODE: 
			USBH_DeInit(&USB_OTG_Core_dev,&USB_Host);	//复位USB HOST
			USB_OTG_StopHost(&USB_OTG_Core_dev);		//停止USBhost
			if(USB_Host.usr_cb->DeviceDisconnected)		//存在,才禁止
			{
				USB_Host.usr_cb->DeviceDisconnected(); 	//关闭USB连接
				USBH_DeInit(&USB_OTG_Core_dev, &USB_Host);
				USB_Host.usr_cb->DeInit();
				USB_Host.class_cb->DeInit(&USB_OTG_Core_dev,&USB_Host.device_prop);
			}
			USB_OTG_DisableGlobalInt(&USB_OTG_Core_dev);//关闭所有中断
			myfree(SRAMIN,USBH_CfgDesc);	//释放内存
			break;		
		case USBD_MSC_MODE:
			DCD_DevDisconnect(&USB_OTG_Core_dev);
			USB_OTG_StopDevice(&USB_OTG_Core_dev); 
			myfree(SRAMIN,MSC_BOT_Data);	//释放内存
			break;	 
	} 
	RCC->AHB2RSTR|=1<<7;	//USB OTG FS 复位
	delay_ms(5);
	RCC->AHB2RSTR&=~(1<<7);	//复位结束   
	memset(&USB_OTG_Core_dev,0,sizeof(USB_OTG_CORE_HANDLE));
	memset(&USB_Host,0,sizeof(USB_Host));
	usbx.mode=0XFF;
	usbx.bDeviceState=0;
	usbx.hdevclass=0; 
}
//设置USB工作模式
//mode:0,USB HOST MSC模式(默认模式,接U盘)
//	   1,USB HOST HID模式(驱动鼠标键盘等)
//	   2,USB Device MSC模式(USB读卡器) 
void usbapp_mode_set(u8 mode)
{
	usbapp_mode_stop();//先停止当前USB工作模式
	usbx.mode=mode;
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			MSC_Machine.buff=mymalloc(SRAMIN,USBH_MSC_MPS_SIZE);
			USBH_DataInBuffer=mymalloc(SRAMIN,512);
			USBH_DataOutBuffer=mymalloc(SRAMIN,512);
		
			USBH_CfgDesc=mymalloc(SRAMIN,512); 
			USBH_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
			break;
		case USBH_HID_MODE: 
			USBH_CfgDesc=mymalloc(SRAMIN,512);
			USBH_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USB_Host,&HID_cb,&USR_Callbacks);  
			break;		
		case USBD_MSC_MODE:
			MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);
			USBD_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
			break;	  
	}
}







