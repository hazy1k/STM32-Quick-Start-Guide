#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"    
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 
//ALIENTEK 探索者STM32F407开发板 实验51
//USB读卡器(Slave)实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司


USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况
 
int main(void)
{         
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;

	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128  
  
 	POINT_COLOR=RED;//设置字体为红色	   
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"USB Card Reader TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/7/21");	
	if(SD_Init())LCD_ShowString(30,130,200,16,16,"SD Card Error!");	//检测SD卡错误
	else //SD 卡正常
	{   															  
		LCD_ShowString(30,130,200,16,16,"SD Card Size:     MB"); 
 		LCD_ShowNum(134,130,SDCardInfo.CardCapacity>>20,5,16);	//显示SD卡容量
 	}
//	if(W25QXX_ReadID()!=W25Q128)LCD_ShowString(30,130,200,16,16,"W25Q128 Error!");	//检测W25Q128错误
	if(W25QXX_ReadID()!=W25Q128 && W25QXX_ReadID()!=NM25Q128 && W25QXX_ReadID()!=BY25Q128)  /* 读取QSPI FLASH ID */
		LCD_ShowString(30,130,200,16,16,"W25Q128 Error!");	//检测W25Q128错误
	else //SPI FLASH 正常
	{   														 
		LCD_ShowString(30,150,200,16,16,"SPI FLASH Size:12MB");	 
	}  
 	LCD_ShowString(30,170,200,16,16,"USB Connecting...");//提示正在建立连接 	    
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	delay_ms(1800);	
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   
			LCD_Fill(30,190,240,190+16,WHITE);//清除显示			  	   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LED1=0;
				LCD_ShowString(30,190,200,16,16,"USB Writing...");//提示USB正在写入数据	 
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LED1=0;
				LCD_ShowString(30,190,200,16,16,"USB Reading...");//提示USB正在读出数据  		 
			}	 										  
			if(USB_STATUS_REG&0x04)LCD_ShowString(30,210,200,16,16,"USB Write Err ");//提示写入错误
			else LCD_Fill(30,210,240,210+16,WHITE);//清除显示	  
			if(USB_STATUS_REG&0x08)LCD_ShowString(30,230,200,16,16,"USB Read  Err ");//提示读出错误
			else LCD_Fill(30,230,240,230+16,WHITE);//清除显示    
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)LCD_ShowString(30,170,200,16,16,"USB Connected    ");//提示USB连接已经建立
			else LCD_ShowString(30,170,200,16,16,"USB DisConnected ");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED1=1;
			LED0=!LED0;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	};  
}
















