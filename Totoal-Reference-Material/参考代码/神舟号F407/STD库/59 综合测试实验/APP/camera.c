#include "camera.h"
#include "common.h"	
#include "calendar.h"  
#include "audioplay.h"  
#include "ov2640.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-照相机 代码	   
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

vu8 hsync_int=0;						//帧中断标志
vu8 jpeg_size;							//jpeg图片分辨率
vu8 ov2640_mode=OV2640_RGB565_MODE;		//工作模式:0,RGB565模式;1,JPEG模式

#define jpeg_dma_bufsize	5*1024		//定义JPEG DMA接收时数据缓存jpeg_buf0/1的大小(*4字节)
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度(*4字节)
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
										
u32 *jpeg_buf0;							//JPEG数据缓存buf,通过malloc申请内存
u32 *jpeg_buf1;							//JPEG数据缓存buf,通过malloc申请内存
u32 *jpeg_data_buf;						//JPEG数据缓存buf,通过malloc申请内存 

///////////////////////////////////////////////////////////////////////////////////
//JPEG尺寸支持列表
const u16 camera_jpeg_img_size_tbl[][2]=
{
	160,120,	//QQVGA
	176,144,	//QCIF
	320,240,	//QVGA
	400,240,	//WQVGA
	352,288,	//CIF
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,800,	//WXGA
	1280,960,	//XVGA
	1440,900,	//WXGA+
	1280,1024,	//SXGA
	1600,1200,	//UXGA	
}; 
//摄像头提示
u8*const camera_remind_tbl[4][GUI_LANGUAGE_NUM]=
{
{"初始化OV2640,请稍侯...","初始化OV2640,請稍後...","OV2640 Init,Please wait...",},	 
{"未检测到OV2640,请检查...","未檢測到OV2640,請檢查...","No OV2640 find,Please check...",},	 
{"保存为:","保存為:","SAVE AS:",}, 
{"分辨率","分辨率","Resolution",},	 	 
};
//縮放提示
u8*const camera_scalemsg_tbl[2][GUI_LANGUAGE_NUM]=
{
"1:1显示(无缩放)！","1:1顯示(無縮放)！","1:1 Display(No Scale)！",
"全尺寸缩放！","全尺寸縮放！","Full Scale！",
};
u8*const camera_jpegimgsize_tbl[13]=	//JPEG图片 13种尺寸 
{"QQVGA","QCIF","QVGA","WQVGA","CIF","VGA","SVGA","XGA","WXGA","XVGA","WXGA+","SXGA","UXGA"};	 
//拍照成功提示框标题
u8*const camera_saveok_caption[GUI_LANGUAGE_NUM]=
{
"拍照成功！","拍照成功！","Take Photo OK！",
}; 		
//拍照失败提示信息
u8*const camera_failmsg_tbl[3][GUI_LANGUAGE_NUM]=
{	                      						 
"创建文件失败,请检查!","創建文件失敗,請檢查!","Creat File Failed,Please check!",
"内存不足!","內存不足!","Out of memory!",
"数据错误(图片尺寸太大)!","數據錯誤(圖片尺寸太大)!","Data Error(Picture is too big)!",
}; 
//00级功能选项表标题
u8*const camera_l00fun_caption[GUI_LANGUAGE_NUM]=
{
"相机设置","相機設置","Camera Set",
};
//00级功能选项表
u8*const camera_l00fun_table[GUI_LANGUAGE_NUM][6]=
{
{"场景设置","特效设置","曝光等级","亮度设置","色度设置","对比度设置",},
{"場景設置","特效設置","曝光等級","亮度設置","色度設置","對比度設置",},
{"Scene","Effects","Exposure","Brightness","Saturation","Contrast"},
};	
//////////////////////////////////////////////////////////////////////////	   
//10级功能选项表
//场景模式
u8*const camera_l10fun_table[GUI_LANGUAGE_NUM][5]=
{
{"自动","晴天","阴天","办公室","家庭",},
{"自動","晴天","陰天","辦公室","家庭",},
{"Auto","Sunny","Cloudy","Office","Home"},
};	
//11级功能选项表
//特效设置
u8*const camera_l11fun_table[GUI_LANGUAGE_NUM][7]=
{
{"普通","负片","黑白","偏红色","偏绿色","偏蓝色","复古"},
{"普通","負片","黑白","偏紅色","偏綠色","偏藍色","復古"},
{"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique",},
};	
//12~15级功能选项表
//都是-2~2的5個值
u8*const camera_l125fun_table[GUI_LANGUAGE_NUM][5]=
{
{"-2","-1","0","+1","+2",},					   
{"-2","-1","0","+1","+2",},					   
{"-2","-1","0","+1","+2",},					   
};	   
//////////////////////////////////////////////////////////////////////////	  

//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;//剩余数据长度
	u32 *pbuf;
	if(ov2640_mode==OV2640_JPEG_MODE)//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{
			DMA2_Stream1->CR&=~(1<<0);		//停止当前传输
			while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置 
			rlen=jpeg_dma_bufsize-DMA2_Stream1->NDTR;//得到剩余数据长度	
			pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾,继续添加
			if(DMA2_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=jpeg_buf1[i];//读取buf1里面的剩余数据
			else for(i=0;i<rlen;i++)pbuf[i]=jpeg_buf0[i];//读取buf0里面的剩余数据 
			jpeg_data_len+=rlen;			//加上剩余长度
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_dma_bufsize;//传输长度为jpeg_buf_size*4字节
			DMA2_Stream1->CR|=1<<0;			//重新传输
			jpeg_data_ok=0;					//标记数据未采集
			jpeg_data_len=0;				//数据重新开始
		}
	}else hsync_int=1;
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
//切换为OV2640模式
void sw_ov2640_mode(void)
{
	OV2640_PWDN=0;//OV2640 Power Up
	//GPIOC6/8/9/11切换为 DCMI接口
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_DCMI);  //PC8,AF13  DCMI_D2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_DCMI);  //PC8,AF13  DCMI_D2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_DCMI);  //PC9,AF13  DCMI_D3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_DCMI); //PC11,AF13 DCMI_D4
} 
//切换为SD卡模式
void sw_sdcard_mode(void)
{
	OV2640_PWDN=1;//OV2640 Power Down 
	//GPIOC6/8/9/11切换为 SDIO接口
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF5_SPI3);  
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO);  //PC8,AF12
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);//PC9,AF12 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO); 
} 
//得到文件名,按文日期时间命名
//mode:0,创建.bmp文件;1,创建.jpg文件.
//bmp组合成:形如"0:PHOTO/PIC20120321210633.bmp"/"2:PHOTO/PIC20120321210633.bmp"的文件名
//jpg组合成:形如"0:PHOTO/PIC20120321210633.jpg"/"2:PHOTO/PIC20120321210633.jpg"的文件名
void camera_new_pathname(u8 *pname,u8 mode)
{	  
	calendar_get_time(&calendar);
	calendar_get_date(&calendar); 
	if(mode==0)
	{
		if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//首选保存在SD卡
		else if(gui_phy.memdevflag&(1<<2))sprintf((char*)pname,"2:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD卡不存在,则保存在U盘	
	}else 
	{
		if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//首选保存在SD卡
		else if(gui_phy.memdevflag&(1<<2))sprintf((char*)pname,"2:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD卡不存在,则保存在U盘	
	}
} 
//OV2640拍照jpg图片
//返回值:0,成功
//    其他,错误代码
u8 ov2640_jpg_photo(u8 *pname)
{
	FIL* f_jpg;
	u8 res;
	u32 bwr;
	u16 i;
	u8* pbuf;
	f_jpg=(FIL *)gui_memin_malloc(sizeof(FIL));		//开辟FIL字节的内存区域 
	jpeg_buf0=gui_memin_malloc(jpeg_dma_bufsize*4);	//为jpeg dma接收申请内存	
	jpeg_buf1=gui_memin_malloc(jpeg_dma_bufsize*4);	//为jpeg dma接收申请内存	
	jpeg_data_buf=gui_memex_malloc(210*1024);		//为jpeg文件申请内存(最大210KB)	
	if(!f_jpg||!jpeg_buf0||!jpeg_buf1||!jpeg_data_buf)//内存申请失败.
	{
		gui_memin_free(f_jpg); 
		gui_memin_free(jpeg_buf0);
		gui_memin_free(jpeg_buf1);
		gui_memex_free(jpeg_data_buf); 
		return 0XFF;			
	}
	ov2640_mode=OV2640_JPEG_MODE;				//设置为JPEG模式
	sw_ov2640_mode();		//切换为OV2640模式
	dcmi_rx_callback=jpeg_dcmi_rx_callback;//dcmi dma接收回调函数
	DCMI_DMA_Init((u32)jpeg_buf0,(u32)jpeg_buf1,jpeg_dma_bufsize,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);;//DCMI DMA配置(双缓冲模式)
	OV2640_JPEG_Mode();		//切换为JPEG模式 
 	OV2640_ImageWin_Set(0,0,1600,1200);			 
	OV2640_OutSize_Set(camera_jpeg_img_size_tbl[jpeg_size][0],camera_jpeg_img_size_tbl[jpeg_size][1]);//设置输出尺寸 
	DCMI_Start(); 			//启动传输 
	while(jpeg_data_ok!=1);	//等待第一帧图片采集完
	jpeg_data_ok=2;			//忽略本帧图片,启动下一帧采集
	while(jpeg_data_ok!=1);	//等待第二帧图片采集完
	jpeg_data_ok=2;			//忽略本帧图片,启动下一帧采集
	while(jpeg_data_ok!=1);	//等待第三帧图片采集完,第三帧,才保存到SD卡去.
	DCMI_Stop(); 			//停止DMA搬运
	ov2640_mode=OV2640_RGB565_MODE;//切换为RGB565模式
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
	gui_memin_free(f_jpg); 
	gui_memin_free(jpeg_buf0);
	gui_memin_free(jpeg_buf1);
	gui_memex_free(jpeg_data_buf); 
	return res;
} 

//摄像头功能
//所有照片文件,均保存在SD卡PHOTO文件夹内.
u8 camera_play(void)
{												   
	u8 rval=0;
	u8 res;
 	u8 *caption=0;
	u8 *pname;
	u8 selx=0;
	u8 l00sel=0,l10sel=0,l11sel=0;//默认选择项
	u8 l2345sel[4];
	u8 *psn;
	u8 key;
 	u8 scale=1;				//默认是全尺寸缩放
	u8 tcnt=0;

	if(audiodev.status&(1<<7))		//当前在放歌??必须停止
	{
		audio_stop_req(&audiodev);	//停止音频播放
		audio_task_delete();		//删除音乐播放任务.
	}
 	//提示开始检测OV2640
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(gui_phy.memdevflag&(1<<0))f_mkdir("0:PHOTO");//强制创建文件夹,给照相机用
	if(gui_phy.memdevflag&(1<<2))f_mkdir("2:PHOTO");//强制创建文件夹,给照相机用
	if(OV2640_Init())//初始化ov2640
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);  
		rval=1;
	} 
	pname=gui_memin_malloc(40);	//申请40个字节内存,用于存放文件名
	psn=gui_memin_malloc(50);	//申请50个字节内存,用于存放类似：“保存为:0:PHOTO/PIC20120321210633.bmp"”的提示语
	if(!pname||!psn)rval=1;//申请失败	  
	if(rval==0)//OV2640正常
	{ 
 		OV2640_RGB565_Mode();	//RGB565模式
		
		l2345sel[0]=2;//曝光设置2,实际值0
		l2345sel[1]=2;//亮度默认为2,实际值0
		l2345sel[2]=2;//色度默认为2,实际值0
		l2345sel[3]=2;//对比度默认为2,实际值0 
		
		OV2640_Auto_Exposure(l2345sel[0]);   	//自动曝光设置
		OV2640_Brightness(l2345sel[1]);	   		//亮度设置
		OV2640_Color_Saturation(l2345sel[2]);	//色度设置
		OV2640_Contrast(l2345sel[3]);			//对比度设置 
		
		My_DCMI_Init();			//DCMI配置
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置  
		OV2640_OutSize_Set(lcddev.width,lcddev.height);//设置摄像头输出尺寸为LCD的尺寸大小		
		DCMI_Start(); 			//启动传输 
		delay_ms(200);
 		//因为摄像头HREF和PCLK吧TPAD输入脚夹在中间,开启摄像头后,会有较大影响,故重新初始化TPAD
		OSTaskSuspend(3); 		//挂起watch_task
		TPAD_Init(12);			//重新初始化TPAD
		OSTaskResume(3);		//恢复watch_task 
		delay_ms(200);
		system_task_return=0;	//清除TPAD	
		jpeg_size=12;			//JPEG拍照,默认设置为1600*1200分辨率(UXGA)
		while(1)	 
		{		 	
			tp_dev.scan(0);
			if(tp_dev.sta&TP_PRES_DOWN)
			{    		 
				DCMI_Stop();
				sw_sdcard_mode();	//切换为SD卡模式
				caption=(u8*)camera_l00fun_caption[gui_phy.language];
				res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*6)/2,160,72+32*6,(u8**)camera_l00fun_table[gui_phy.language],6,(u8*)&l00sel,0X90,caption);//单选
				sw_ov2640_mode();	//切换为OV2640模式
				if(res==0)
				{ 
					DCMI_Start();
					delay_ms(200);
					DCMI_Stop();
					sw_sdcard_mode();	//切换为SD卡模式 
					caption=(u8*)camera_l00fun_table[gui_phy.language][l00sel]; 
					switch(l00sel)
					{
						case 0://场景设置	  
							res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*5)/2,160,72+32*5,(u8**)camera_l10fun_table[gui_phy.language],5,(u8*)&l10sel,0X90,caption);//单选
							sw_ov2640_mode();//切换为OV2640模式
							if(res==0)
							{
 								OV2640_Light_Mode(l10sel);
							}
							break;
						case 1://特效设置		 
							res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*7)/2,160,72+32*7,(u8**)camera_l11fun_table[gui_phy.language],7,(u8*)&l11sel,0X90,caption);//单选
							sw_ov2640_mode();//切换为OV2640模式
							if(res==0)
							{
 								OV2640_Special_Effects(l11sel);  
							}
							break; 
						case 2://曝光设置
						case 3://亮度设置    
						case 4://色度设置
						case 5://对比度设置
							selx=l2345sel[l00sel-2];//得到之前的选择
							res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*5)/2,160,72+32*5,(u8**)camera_l125fun_table[gui_phy.language],5,(u8*)&selx,0X90,caption);//单选
							sw_ov2640_mode();//切换为OV2640模式
							if(res==0)
							{
								l2345sel[l00sel-2]=selx;//记录新值
							 	if(l00sel==2)OV2640_Auto_Exposure(selx);   	//自动曝光设置
							 	if(l00sel==3)OV2640_Brightness(selx);	   	//亮度设置
								if(l00sel==4)OV2640_Color_Saturation(selx);	//色度设置
								if(l00sel==5)OV2640_Contrast(selx);			//对比度设置
							}
							break; 
					}
				}     
				DCMI_Start();
 			}
 			if(system_task_return)break;//TPAD返回	
			key=KEY_Scan(0);//按键扫描
			if(key)
			{
				DCMI_Stop();
				while(KEY_Scan(1));//等待按键松开
				tcnt=0;
				switch(key)
				{
					case KEY0_PRES:	//KEY0按下,JPEG拍照
					case KEY2_PRES:	//KEY2按下,BMP拍照
						LED1=0;		//DS1亮,提示拍照中
						sw_sdcard_mode();	//切换为SD卡模式
						if(key==KEY0_PRES)	//JPEG拍照
						{
							camera_new_pathname(pname,OV2640_JPEG_MODE);//得到jpg文件名	
							res=ov2640_jpg_photo(pname);
						}else//BMP拍照
						{
							camera_new_pathname(pname,OV2640_RGB565_MODE);//得到bmp文件名	
							res=bmp_encode(pname,0,0,lcddev.width,lcddev.height,0);//bmp拍照
						}	
 						if(res)//拍照失败了
						{
							if(res==0XFF)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//内存错误
							else if(res==0XFD)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//数据错误
							else window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//提示SD卡是否存在
						}else
						{
							strcpy((char*)psn,(const char *)camera_remind_tbl[2][gui_phy.language]);
							strcat((char*)psn,(const char *)pname);
							window_msg_box((lcddev.width-180)/2,(lcddev.height-80)/2,180,80,psn,(u8*)camera_saveok_caption[gui_phy.language],12,0,0,0);
							BEEP=1;	//蜂鸣器短叫，提示拍照完成
							delay_ms(100); 
						}
						sw_ov2640_mode();	//切换为OV2640模式   
						LED1=1;				//DS1灭,提示保存完成
						BEEP=0;				//蜂鸣器短叫
						delay_ms(2000); 
						if(scale==0)
						{
							OV2640_ImageWin_Set((1600-lcddev.width)/2,(1200-lcddev.height)/2,lcddev.width,lcddev.height);//1:1真实尺寸
							OV2640_OutSize_Set(lcddev.width,lcddev.height); 
						}else 
						{
							OV2640_ImageWin_Set(0,0,1600,1200);	//全尺寸缩放 
						}
						OV2640_OutSize_Set(lcddev.width,lcddev.height); 	
						system_task_return=0;//清除TPAD
						break;
					case KEY1_PRES:	//KEY1按下,缩放/1:1显示(不缩放)
						scale=!scale;  
						if(scale==0)
						{
							OV2640_ImageWin_Set((1600-lcddev.width)/2,(1200-lcddev.height)/2,lcddev.width,lcddev.height);//1:1真实尺寸
							OV2640_OutSize_Set(lcddev.width,lcddev.height);  
						}else 
						{
							OV2640_ImageWin_Set(0,0,1600,1200);				//全尺寸缩放
							OV2640_OutSize_Set(lcddev.width,lcddev.height);  
						} 
						window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_scalemsg_tbl[scale][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 						while(KEY_Scan(1)==0&&tcnt<80)//等待800ms,如果没有按键按下的话.
						{
							delay_ms(10);
							tcnt++;
						}	
						break;
					case WKUP_PRES://分辨率设置
						jpeg_size++;
						if(jpeg_size>12)jpeg_size=0;  
						sprintf((char*)psn,"%s:%dX%d(%s)",camera_remind_tbl[3][gui_phy.language],camera_jpeg_img_size_tbl[jpeg_size][0],camera_jpeg_img_size_tbl[jpeg_size][1],camera_jpegimgsize_tbl[jpeg_size]);
						window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,psn,(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 						while(KEY_Scan(1)==0&&tcnt<80)//等待800ms,如果没有按键按下的话.
						{
							delay_ms(10);
							tcnt++;
						}							
						break;
						
				}
				sw_ov2640_mode();//切换为OV2640模式  
				DCMI_Start();
			}
			if(hsync_int)//刚刚产生帧中断,可以延时
			{
				delay_ms(10);
				hsync_int=0;
			}  
 		}
	}
	DCMI_Stop();		//停止摄像头工作
	sw_sdcard_mode();	//切换为SD卡模式 
	gui_memin_free(pname);				
	gui_memin_free(psn);
	delay_ms(200);	
	//退出摄像头后,重新初始化TPAD
	OSTaskSuspend(3); 	//挂起watch_task
	TPAD_Init(8);		//重新初始化TPAD
	OSTaskResume(3);	//恢复watch_task 									    
	return 0;	 
}















