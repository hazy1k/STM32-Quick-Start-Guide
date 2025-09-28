#include "usbh_usr.h"
#include "usb_hcd_int.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"  
#include "usbh_hid_gamepad.h"  
#include "lcd.h"  
#include "string.h"  
#include "delay.h"  
#include "usb_app.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//USBH-USR 代码	   
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

static u8 AppState;

////USB OTG 中断服务函数
////处理所有USB中断
//void OTG_FS_IRQHandler(void)
//{ 
//	USBH_OTG_ISR_Handler(&USB_OTG_Core_dev_dev);
//}  

//USB HOST 用户回调函数.
USBH_Usr_cb_TypeDef USR_Callbacks =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};
 
//USB HOST 初始化 
void USBH_USR_Init(void)
{
	USB_OTG_GINTMSK_TypeDef intmsk;
	intmsk.d32=USB_OTG_READ_REG32(&USB_OTG_Core_dev.regs.GREGS->GINTMSK);
	intmsk.b.usbsuspend=0;	//关闭挂起中断 
	USB_OTG_WRITE_REG32(&USB_OTG_Core_dev.regs.GREGS->GINTMSK,intmsk.d32); 
	printf("USB OTG FS MSC Host\r\n");
	printf("> USB Host library started.\r\n");
	printf("  USB Host Library v2.1.0\r\n\r\n");
}
//检测到U盘插入
void USBH_USR_DeviceAttached(void)//U盘插入
{ 
	usbx.bDeviceState|=1<<6;//检测到USB插入
	printf("检测到USB设备插入!\r\n");
}
//检测到U盘拔出
void USBH_USR_DeviceDisconnected (void)//U盘移除
{ 
	printf("USB设备拔出!\r\n");
	usbx.bDeviceState=0;//连接失效
	usbx.hdevclass=0;	//设备标记清零 
}  
//复位从机
void USBH_USR_ResetDevice(void)
{
	printf("复位设备...\r\n");
}
//检测到从机速度
//DeviceSpeed:从机速度(0,1,2 / 其他)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("高速(HS)USB设备!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("全速(FS)USB设备!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("低速(LS)USB设备!\r\n");  
	}
	else
	{
		printf("设备错误!\r\n");  
	}
}
//检测到从机的描述符
//DeviceDesc:设备描述符指针
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}
//从机地址分配成功
void USBH_USR_DeviceAddressAssigned(void)
{
	printf("从机地址分配成功!\r\n");   
}
//配置描述符获有效
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		printf("可移动存储器设备!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		printf("HID 设备!\r\n"); 
	}    
}
//获取到设备Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}
//获取到设备Product String 
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString);  
}
//获取到设备SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 
//设备USB枚举完成
void USBH_USR_EnumerationDone(void)
{ 
	printf("设备枚举完成!\r\n\r\n");    
} 
//无法识别的USB设备
void USBH_USR_DeviceNotSupported(void)
{  
	printf("无法识别的USB设备!\r\n\r\n");    
}  
//等待用户输入按键,执行下一步操作
USBH_USR_Status USBH_USR_UserInput(void)
{ 
	printf("跳过用户确认步骤!\r\n");
	usbx.bDeviceState|=0X80;		//标记已连接
	return USBH_USR_RESP_OK;
} 
//USB接口电流过载
void USBH_USR_OverCurrentDetected (void)
{
	printf("端口电流过大!!!\r\n");
}  
//重新初始化
void USBH_USR_DeInit(void)
{
  	AppState=USH_USR_FS_INIT;
	printf("重新初始化!!!\r\n");
}
//无法恢复的错误!!  
void USBH_USR_UnrecoveredError (void)
{
	printf("无法恢复的错误!!!\r\n\r\n");	
}  
int USBH_USR_MSC_Application(void)
{
	u8 res=0;
  	switch(AppState)
  	{
    	case USH_USR_FS_INIT://初始化文件系统 
			printf("开始执行用户程序!!!\r\n");
			AppState=USH_USR_FS_TEST;
      		break;
    	case USH_USR_FS_TEST:	//执行USB OTG 测试主程序
			//res=USH_User_App(); //用户主程序
			if(HCD_IsDeviceConnected(&USB_OTG_Core_dev))//设备连接成功
			{
				usbx.hdevclass=1;	//检测到U盘插入
				//do nothing
			}else 
			{ 
				AppState=USH_USR_FS_INIT;
      		}
			break;
    	default:break;
  	} 
	return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
//下面两个函数,为ALIENTEK添加,以防止USB死机

//USB枚举状态死机检测,防止USB枚举失败导致的死机
//phost:USB_HOST结构体指针
//返回值:0,没有死机
//       1,死机了,外部必须重新启动USB连接.
u8 USBH_Check_EnumeDead(USBH_HOST *phost)
{
	static u16 errcnt=0;
	//这个状态,如果持续存在,则说明USB死机了.
	if(phost->gState==HOST_CTRL_XFER&&(phost->EnumState==ENUM_IDLE||phost->EnumState==ENUM_GET_FULL_DEV_DESC))
	{
		errcnt++;
		if(errcnt>2000)//死机了
		{ 
			errcnt=0;
			RCC->AHB2RSTR|=1<<7;	//USB OTG FS 复位
			delay_ms(5);
			RCC->AHB2RSTR&=~(1<<7);	//复位结束  
			return 1;
		} 
	}else errcnt=0;
	return 0;
} 
//USB HID通信死机检测,防止USB通信死机(暂时仅针对:DTERR,即Data toggle error)
//pcore:USB_OTG_Core_dev_HANDLE结构体指针
//phidm:HID_Machine_TypeDef结构体指针 
//返回值:0,没有死机
//       1,死机了,外部必须重新启动USB连接.
u8 USBH_Check_HIDCommDead(USB_OTG_CORE_HANDLE *pcore,HID_Machine_TypeDef *phidm)
{
 	if(pcore->host.HC_Status[phidm->hc_num_in]==HC_DATATGLERR)//检测到DTERR错误,直接重启USB.
	{  
		return 1;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
//USB鼠标/键盘/游戏手柄相关函数
//鼠标初始化
void USR_MOUSE_Init(void)
{
	usbx.hdevclass=2;		//检测到鼠标插入 
}
//键盘初始化
void  USR_KEYBRD_Init(void)
{ 
	usbx.hdevclass=3;		//检测到键盘插入 
}
//游戏手柄初始化
void USR_GAMEPAD_Init(void)
{
	usbx.hdevclass=4;		//检测到手柄 
}

//零时数组,用于存放鼠标坐标/键盘输入内容(4.3屏,最大可以输入2016字节)
//__align(4) u8 tbuf[2017]; 

//USB鼠标数据处理
//data:USB鼠标数据结构体指针
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{  
//	static signed short x,y,z; 
//	if(USB_FIRST_PLUGIN_FLAG)//第一次插入,将数据清零
//	{
//		USB_FIRST_PLUGIN_FLAG=0;
//		x=y=z=0;
//	}
//	x+=(signed char)data->x;
//	if(x>9999)x=9999;
//	if(x<-9999)x=-9999;
//	y+=(signed char)data->y;
//	if(y>9999)y=9999;
//	if(y<-9999)y=-9999;
//	z+=(signed char)data->z;
//	if(z>9999)z=9999;
//	if(z<-9999)z=-9999;
//	POINT_COLOR=BLUE;
//	sprintf((char*)tbuf,"BUTTON:");
//	if(data->button&0X01)strcat((char*)tbuf,"LEFT");
//	if((data->button&0X03)==0X02)strcat((char*)tbuf,"RIGHT");
//	else if((data->button&0X03)==0X03)strcat((char*)tbuf,"+RIGHT");
//	if((data->button&0X07)==0X04)strcat((char*)tbuf,"MID");
//	else if((data->button&0X07)>0X04)strcat((char*)tbuf,"+MID");  
// 	LCD_Fill(30+56,180,lcddev.width,180+16,WHITE);	
//	LCD_ShowString(30,180,210,16,16,tbuf);	
//	sprintf((char*)tbuf,"X POS:%05d",x); 
//	LCD_ShowString(30,200,200,16,16,tbuf);	
//	sprintf((char*)tbuf,"Y POS:%05d",y);
//	LCD_ShowString(30,220,200,16,16,tbuf);	
//	sprintf((char*)tbuf,"Z POS:%05d",z);
//	LCD_ShowString(30,240,200,16,16,tbuf);	 
//	printf("btn,X,Y,Z:0x%x,%d,%d,%d\r\n",data->button,(signed char)data->x,(signed char)data->y,(signed char)data->z);   
} 
//USB键盘数据处理
//data:USB鼠标数据结构体指针
void  USR_KEYBRD_ProcessData (uint8_t data)
{ 
//	static u16 pos; 
//	static u16 endx,endy;
//	static u16 maxinputchar;
//	
//	u8 buf[4];
//	if(USB_FIRST_PLUGIN_FLAG)//第一次插入,将数据清零
//	{
//		USB_FIRST_PLUGIN_FLAG=0;
//		endx=((lcddev.width-30)/8)*8+30;		//得到endx值
//		endy=((lcddev.height-220)/16)*16+220;	//得到endy值
//		maxinputchar=((lcddev.width-30)/8);
//		maxinputchar*=(lcddev.height-220)/16;	//当前LCD最大可以显示的字符数.
// 		pos=0; 
//	}
//	POINT_COLOR=BLUE;
//	sprintf((char*)buf,"%02X",data);
//	LCD_ShowString(30+56,180,200,16,16,buf);//显示键值	 
//	if(data>=' '&&data<='~')
//	{
//		tbuf[pos++]=data;
//		tbuf[pos]=0;		//添加结束符. 
//		if(pos>maxinputchar)pos=maxinputchar;//最大输入这么多
//	}else if(data==0X0D)	//退格键
//	{
//		if(pos)pos--;
//		tbuf[pos]=0;		//添加结束符.  
//	}
//	if(pos<=maxinputchar)	//没有超过显示区
//	{
//		LCD_Fill(30,220,endx,endy,WHITE);
//		LCD_ShowString(30,220,endx-30,endy-220,16,tbuf);
//	}		
//	printf("KEY Board Value:%02X\r\n",data);
//	printf("KEY Board Char:%c\r\n",data); 
}
u8* const JOYPAD_SYMBOL_TBL[8]=
{"Right","Left","Down","Up","Start","Select","A","B"};//手柄按键符号定义
//USB键盘数据处理
//data:USB鼠标数据结构体指针
void  USR_GAMEPAD_ProcessData(uint8_t data)
{
//	u8 i;
//	if(data)
//	{
//		POINT_COLOR=BLUE;
//		LCD_ShowNum(30+56,180,data,3,16);//显示键值
//		for(i=0;i<8;i++)
//		{
//			if(data&(1<<i))
//			{
//				LCD_Fill(30+56,200,30+56+48,200+16,WHITE);//清除之前的显示
//				LCD_ShowString(30+56,200,200,16,16,(u8*)JOYPAD_SYMBOL_TBL[i]);//显示符号
//			}		
//		}		    
//	}
}
////////////////////////////////////////////////////////////////////////////////////////
//用户定义函数,实现fatfs diskio的接口函数 
//获取U盘状态
//返回值:0,U盘未就绪
//      1,就绪
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core_dev);//返回U盘状态
}

//读U盘
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;		 
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	u32 retry=0;
	if(HCD_IsDeviceConnected(&USB_OTG_Core_dev)&&AppState==USH_USR_FS_TEST)//连接还存在,且是APP测试状态
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core_dev,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core_dev ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core_dev))
			{
				res=1;//读写错误
				break;
			}; 
			retry++;
		}while(res==USBH_MSC_BUSY&&retry<0X7FFFFFF);
		if(retry==0X7FFFFFF)res=1;
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

//写U盘
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;		 
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core_dev)&&AppState==USH_USR_FS_TEST)//连接还存在,且是APP测试状态
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core_dev,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core_dev ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core_dev))
			{
				res=1;//读写错误
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}




















