#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	
#include "string.h"	
#include "math.h"	
#include "dcmi.h"	
#include "ov2640.h"	
#include "beep.h"	
#include "timer.h"
//ALIENTEK 探索者STM32F407开发板 实验42
//照相机 实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

 u8 ov2640_mode=0;						//工作模式:0,RGB565模式;1,JPEG模式

#define jpeg_dma_bufsize	5*1024		//定义JPEG DMA接收时数据缓存jpeg_buf0/1的大小(*4字节)
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度(*4字节)
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
										
u32 *jpeg_buf0;							//JPEG数据缓存buf,通过malloc申请内存
u32 *jpeg_buf1;							//JPEG数据缓存buf,通过malloc申请内存
u32 *jpeg_data_buf;						//JPEG数据缓存buf,通过malloc申请内存

//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;//剩余数据长度
	u32 *pbuf;
	if(ov2640_mode)//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{
			DMA_Cmd(DMA2_Stream1,DISABLE);		//停止当前传输
			while(DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);	//等待DMA2_Stream1可配置 
			rlen=jpeg_dma_bufsize-DMA_GetCurrDataCounter(DMA2_Stream1);//得到剩余数据长度	
			pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾,继续添加
			if(DMA2_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=jpeg_buf1[i];//读取buf1里面的剩余数据
			else for(i=0;i<rlen;i++)pbuf[i]=jpeg_buf0[i];//读取buf0里面的剩余数据 
			jpeg_data_len+=rlen;			//加上剩余长度
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{ DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_dma_bufsize);//传输长度为jpeg_buf_size*4字节
			DMA_Cmd(DMA2_Stream1,ENABLE); //重新传输
			jpeg_data_ok=0;					//标记数据未采集
			jpeg_data_len=0;				//数据重新开始
		}
	}
}



//jpeg数据接收回调函数
void jpeg_dcmi_rx_callback(void)
{ 
	u16 i;
	u32 *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾
	if(DMA2_Stream1->CR&(1<<19))//buf0已满,正常处理buf1
	{ 
		for(i=0;i<jpeg_dma_bufsize;i++)pbuf[i]=jpeg_buf0[i];//读取buf0里面的数据
		jpeg_data_len+=jpeg_dma_bufsize;//偏移
	}else //buf1已满,正常处理buf0
	{
		for(i=0;i<jpeg_dma_bufsize;i++)pbuf[i]=jpeg_buf1[i];//读取buf1里面的数据
		jpeg_data_len+=jpeg_dma_bufsize;//偏移 
	} 	
}


//切换为OV2640模式（GPIOC8/9/11切换为 DCMI接口）
void sw_ov2640_mode(void)
{
	OV2640_PWDN=0;//OV2640 Power Up
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_DCMI);  //PC8,AF13  DCMI_D2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_DCMI);  //PC9,AF13  DCMI_D3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_DCMI); //PC11,AF13 DCMI_D4  
 
} 
//切换为SD卡模式（GPIOC8/9/11切换为 SDIO接口）
void sw_sdcard_mode(void)
{
	OV2640_PWDN=1;//OV2640 Power Down  
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO);  //PC8,AF12
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);//PC9,AF12 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO); 
}
	
//文件名自增（避免覆盖）
//mode:0,创建.bmp文件;1,创建.jpg文件.
//bmp组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
//jpg组合成:形如"0:PHOTO/PIC13141.jpg"的文件名
void camera_new_pathname(u8 *pname,u8 mode)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		if(mode==0)sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		else sprintf((char*)pname,"0:PHOTO/PIC%05d.jpg",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
} 
//OV2640拍照jpg图片
//返回值:0,成功
//    其他,错误代码
u8 ov2640_jpg_photo(u8 *pname)
{
	FIL* f_jpg;
	u8 res=0;
	u32 bwr;
	u16 i;
	u8* pbuf;
	f_jpg=(FIL *)mymalloc(SRAMIN,sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_jpg==NULL)return 0XFF;					//内存申请失败.
	ov2640_mode=1;
	sw_ov2640_mode();		//切换为OV2640模式
	dcmi_rx_callback=jpeg_dcmi_rx_callback;//回调函数
	DCMI_DMA_Init((u32)jpeg_buf0,(u32)jpeg_buf1,jpeg_dma_bufsize,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA配置(双缓冲模式)
	OV2640_JPEG_Mode();		//切换为JPEG模式 
 	OV2640_ImageWin_Set(0,0,1600,1200);			 
	OV2640_OutSize_Set(1600,1200);//拍照尺寸为1600*1200
	DCMI_Start(); 			//启动传输 
	while(jpeg_data_ok!=1);	//等待第一帧图片采集完
	jpeg_data_ok=2;			//忽略本帧图片,启动下一帧采集
	while(jpeg_data_ok!=1);	//等待第二帧图片采集完
	jpeg_data_ok=2;			//忽略本帧图片,启动下一帧采集
	while(jpeg_data_ok!=1);	//等待第三帧图片采集完,第三帧,才保存到SD卡去.
	DCMI_Stop(); 			//停止DMA搬运
	ov2640_mode=0;
	sw_sdcard_mode();		//切换为SD卡模式
	res=f_open(f_jpg,(const TCHAR*)pname,FA_WRITE|FA_CREATE_NEW);//模式0,或者尝试打开失败,则创建新文件	 
	if(res==0)
	{
		printf("jpeg data size:%d\r\n",jpeg_data_len*4);//串口打印JPEG文件大小
		pbuf=(u8*)jpeg_data_buf;
		for(i=0;i<jpeg_data_len*4;i++)//查找0XFF,0XD8
		{
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))break;
		}
		if(i==jpeg_data_len*4)res=0XFD;//没找到0XFF,0XD8
		else//找到了
		{
			pbuf+=i;//偏移到0XFF,0XD8处
			res=f_write(f_jpg,pbuf,jpeg_data_len*4-i,&bwr);
			if(bwr!=(jpeg_data_len*4-i))res=0XFE; 
		}
	}
	jpeg_data_len=0;
	f_close(f_jpg); 
	sw_ov2640_mode();		//切换为OV2640模式
	OV2640_RGB565_Mode();	//RGB565模式 
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置   
	myfree(SRAMIN,f_jpg); 
	return res;
}  
  

int main(void)
{        
	u8 res;							 
	u8 *pname;				//带路径的文件名 
	u8 key;					//键值		   
	u8 i;						 
	u8 sd_ok=1;				//0,sd卡不正常;1,SD卡正常. 
 	u8 scale=1;				//默认是全尺寸缩放
	u8 msgbuf[15];			//消息缓存区 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
	usmart_dev.init(84);		//初始化USMART
	TIM3_Int_Init(10000-1,8400-1);//10Khz计数,1秒钟中断一次
 	LCD_Init();					//LCD初始化  
	FSMC_SRAM_Init();			//初始化外部SRAM.
 	BEEP_Init();				//蜂鸣器初始化
 	KEY_Init();					//按键初始化   
	W25QXX_Init();				//初始化W25Q128 
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMEX);		//初始化内部内存池  
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  f_mount(fs[0],"0:",1); 		//挂载SD卡  
	POINT_COLOR=RED;      
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"Explorer STM32F4开发板",16,0);	 			    	 
	Show_Str(30,70,200,16,"照相机实验",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:拍照(bmp格式)",16,0);			    	 
	Show_Str(30,110,200,16,"KEY1:拍照(jpg格式)",16,0);					    	 
	Show_Str(30,130,200,16,"WK_UP:FullSize/Scale",16,0);				    	 
	Show_Str(30,150,200,16,"2014年5月16日",16,0);
	res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹
	if(res!=FR_EXIST&&res!=FR_OK) 	//发生了错误
	{		    
		Show_Str(30,150,240,16,"SD卡错误!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"拍照功能将不可用!",16,0);
		sd_ok=0;  	
	} 	
	jpeg_buf0=mymalloc(SRAMIN,jpeg_dma_bufsize*4);	//为jpeg dma接收申请内存	
	jpeg_buf1=mymalloc(SRAMIN,jpeg_dma_bufsize*4);	//为jpeg dma接收申请内存	
	jpeg_data_buf=mymalloc(SRAMEX,300*1024);		//为jpeg文件申请内存(最大300KB)
 	pname=mymalloc(SRAMIN,30);//为带路径的文件名分配30个字节的内存	 
 	while(pname==NULL||!jpeg_buf0||!jpeg_buf1||!jpeg_data_buf)	//内存分配出错
 	{	    
		Show_Str(30,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}   
	while(OV2640_Init())//初始化OV2640
	{
		Show_Str(30,190,240,16,"OV2640 错误!",16,0);
		delay_ms(200);
	    LCD_Fill(30,190,239,206,WHITE);
		delay_ms(200);
	}	
 	Show_Str(30,190,200,16,"OV2640 正常",16,0);
	delay_ms(2000);
	OV2640_RGB565_Mode();	//JPEG模式
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置  
 	OV2640_OutSize_Set(lcddev.width,lcddev.height); 
	DCMI_Start(); 			//启动传输 
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		if(key)
		{
			DCMI_Stop(); //停止显示 
			if(key==WKUP_PRES) 
			{
				scale=!scale;  
				if(scale==0)
				{
					OV2640_ImageWin_Set((1600-lcddev.width)/2,(1200-lcddev.height)/2,lcddev.width,lcddev.height);//1:1真实尺寸
					OV2640_OutSize_Set(lcddev.width,lcddev.height); 
					sprintf((char*)msgbuf,"Full Size 1:1");
				}else 
				{
					OV2640_ImageWin_Set(0,0,1600,1200);				//全尺寸缩放
					OV2640_OutSize_Set(lcddev.width,lcddev.height); 
					sprintf((char*)msgbuf,"Scale");
				}
				LCD_ShowString(30,50,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 	
			}else if(sd_ok)//SD卡正常才可以拍照
			{    
				sw_sdcard_mode();	//切换为SD卡模式
				if(key==KEY0_PRES)	//BMP拍照
				{
					camera_new_pathname(pname,0);//得到文件名	
					res=bmp_encode(pname,0,0,lcddev.width,lcddev.height,0);
				}else if(key==KEY1_PRES)//JPG拍照
				{
					camera_new_pathname(pname,1);//得到文件名	
					res=ov2640_jpg_photo(pname);
					if(scale==0)
					{
						OV2640_ImageWin_Set((1600-lcddev.width)/2,(1200-lcddev.height)/2,lcddev.width,lcddev.height);//1:1真实尺寸
						OV2640_OutSize_Set(lcddev.width,lcddev.height); 
					}else 
					{
						OV2640_ImageWin_Set(0,0,1600,1200);	//全尺寸缩放 
					}
					OV2640_OutSize_Set(lcddev.width,lcddev.height); 					
				}
				sw_ov2640_mode();	//切换为OV2640模式
				if(res)//拍照有误
				{
					Show_Str(30,130,240,16,"写入文件错误!",16,0);		 
				}else 
				{
					Show_Str(30,130,240,16,"拍照成功!",16,0);
					Show_Str(30,150,240,16,"保存为:",16,0);
					Show_Str(30+42,150,240,16,pname,16,0);		    
					BEEP=1;	//蜂鸣器短叫，提示拍照完成
					delay_ms(100);
				}   			
			}else //提示SD卡错误
			{					    
				Show_Str(30,130,240,16,"SD卡错误!",16,0);
				Show_Str(30,150,240,16,"拍照功能不可用!",16,0);			    
			}   	
			BEEP=0;			//关闭蜂鸣器 
			if(key!=WKUP_PRES)delay_ms(1800);//非尺寸切换,等待1.8秒钟
			DCMI_Start(); 	//停止显示  
		} 
		delay_ms(10);
		i++;
		if(i==20)//DS0闪烁.
		{
			i=0;
			LED0=!LED0;
 		}
	}
}








