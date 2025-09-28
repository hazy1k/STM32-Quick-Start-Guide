#include "appplay.h"
#include "appplay_remote.h"
#include "appplay_gyro.h"
#include "appplay_wlvideo.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-应用中心 代码	   
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

#define APPPLAY_EX_BACKCOLOR	0X0000			//窗体外部背景色
#define APPPLAY_IN_BACKCOLOR	0X8C51			//窗体内部背景色
#define APPPLAY_NAME_COLOR		0X001F			//程序名颜色

#define APPPLAY_ALPHA_VAL 		18 				//APP选中透明度设置
#define APPPLAY_ALPHA_COLOR		WHITE			//APP透明色
	

//应用程序名列表
//这里列表为16个,名字长度不要超过8个字节(4个汉字)
u8* const appplay_appname_tbl[3][16]=
{
{
	"红外遥控","陀螺仪","应用程序","应用程序",
	"应用程序","应用程序","应用程序","应用程序",
	"应用程序","应用程序","应用程序","应用程序",
	"应用程序","应用程序","应用程序","应用程序",
},
{
	"紅外遙控","陀螺儀","應用程序","應用程序",
	"應用程序","應用程序","應用程序","應用程序",
	"應用程序","應用程序","應用程序","應用程序",
	"應用程序","應用程序","應用程序","應用程序",
},
{
	"REMOTE","GYRO","APP","APP",
	"APP","APP","APP","APP",
	"APP","APP","APP","APP",   
	"APP","APP","APP","APP",
},
};
 
//appplay的图标路径表
u8* const appplay_icospath_tbl[3][16]=
{
{
"1:/SYSTEM/APP/APPS/ICOS/remote_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/3d_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/facerec_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app01_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app02_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app03_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app04_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app05_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app06_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app07_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app08_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app09_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app10_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app11_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app12_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app13_48.bmp",
},

{
"1:/SYSTEM/APP/APPS/ICOS/remote_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/3d_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/facerec_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app01_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app02_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app03_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app04_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app05_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app06_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app07_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app08_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app09_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app10_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app11_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app12_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app13_60.bmp",
},
{
"1:/SYSTEM/APP/APPS/ICOS/remote_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/3d_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/facerec_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app01_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app02_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app03_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app04_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app05_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app06_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app07_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app08_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app09_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app10_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app11_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app12_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app13_90.bmp",
},
}; 

//加载主界面
//返回值:0,成功;其他,失败;
u8 appplay_load_ui(m_app_dev *appdev) 
{
	u8 i;
	u8 xdis,ydis;
	u8 wxoff,wyoff;
	u8 width,height;
	u8 icowidth;
	u8 icofsize;
	u8 dis;
	u8 lcdtype=0;
	
	u8 rval=0;
 	_window_obj* twin=0;	//窗体
	if(lcddev.width==240)
	{
		wxoff=4;
		wyoff=4;
		xdis=4;
		ydis=5;
		icowidth=48;
		icofsize=12;
		width=48+3*2;
		height=48+12+3*2+2; 
		lcdtype=0;
	}else if(lcddev.width==320)
	{
		wxoff=8;
		wyoff=8;
		xdis=6;
		ydis=6;
		icowidth=60;
		icofsize=12;
		width=60+5*2;
		height=60+12+5*2+2; 
		lcdtype=1;
	}else if(lcddev.width==480)
	{
		wxoff=10;
		wyoff=10;
		xdis=9;
		ydis=4;
		icowidth=90;
		icofsize=16;
		width=90+8*2;
		height=90+16+8*2+2; 
		lcdtype=2;
	}
	dis=(width-icowidth)/2;
	twin=window_creat(wxoff,gui_phy.tbheight+wyoff,lcddev.width-2*wxoff,lcddev.height-gui_phy.tbheight-2*wyoff,0,0X01,16);//创建窗口
	if(twin)
	{
		twin->captionbkcu=APPPLAY_IN_BACKCOLOR;		//默认caption上部分背景色
		twin->captionbkcd=APPPLAY_IN_BACKCOLOR;		//默认caption下部分背景色
		twin->captioncolor=APPPLAY_IN_BACKCOLOR;	//默认caption的颜色
	 	twin->windowbkc=APPPLAY_IN_BACKCOLOR;	 	//默认背景色
	
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,APPPLAY_EX_BACKCOLOR);//填充背景色
	  	app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);//下分界线	 
 		gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize
		,(u8*)APP_MFUNS_CAPTION_TBL[17][gui_phy.language]);//显示标题  
		window_draw(twin);
		for(i=0;i<16;i++)
		{			 
			appdev->icos[i].x=wxoff+xdis/2+(i%4)*(width+xdis);		
			appdev->icos[i].y=gui_phy.tbheight+wxoff+ydis/2+(i/4)*(height+ydis);	
			appdev->icos[i].width=width;
			appdev->icos[i].height=height; 
			appdev->icos[i].path=(u8*)appplay_icospath_tbl[lcdtype][i];
			appdev->icos[i].name=(u8*)appplay_appname_tbl[gui_phy.language][i];
			rval=minibmp_decode(appdev->icos[i].path,appdev->icos[i].x,appdev->icos[i].y+dis,appdev->icos[i].width,icowidth,0,0);
			if(rval)break;//解码出错了
			gui_show_strmid(appdev->icos[i].x,appdev->icos[i].y+dis+icowidth+2,appdev->icos[i].width,icofsize,APPPLAY_NAME_COLOR,icofsize,appdev->icos[i].name);
	 	}
		appdev->selico=0XFF;//默认不选择任何一个
	}else rval=1;
	window_delete(twin);
	return rval;
}	   
//设置选中哪个图标
//sel:0~15代表当前页的选中ico
void appplay_set_sel(m_app_dev* appdev,u8 sel)
{
	u8 icowidth;
	u8 icofsize;
	u8 dis;
	if(sel>=16)return;//非法的输入	
	if(lcddev.width==240)
	{ 
		icowidth=48;
		icofsize=12;
	}else if(lcddev.width==320)
	{ 
		icowidth=60; 
		icofsize=12;
	}else if(lcddev.width==480)
	{ 
		icowidth=90; 
		icofsize=16;
	}
	if(appdev->selico<16)
	{
		dis=(appdev->icos[appdev->selico].width-icowidth)/2;
		gui_fill_rectangle(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_IN_BACKCOLOR);//清除之前的图片
		minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis,appdev->icos[appdev->selico].width,icowidth,0,0);
 		gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis+icowidth+2,appdev->icos[appdev->selico].width,icofsize,APPPLAY_NAME_COLOR,icofsize,appdev->icos[appdev->selico].name);
	};				   
	appdev->selico=sel;
	dis=(appdev->icos[appdev->selico].width-icowidth)/2;
	gui_alphablend_area(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_ALPHA_COLOR,APPPLAY_ALPHA_VAL);
	minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis,appdev->icos[appdev->selico].width,icowidth,0,0);
	gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis+icowidth+2,appdev->icos[appdev->selico].width,icofsize,APPPLAY_NAME_COLOR,icofsize,appdev->icos[appdev->selico].name);
}
//触摸屏扫描
//返回值:0~15,有效按键.其他,无效
u8 appplay_tpscan(m_app_dev *appdev)
{
	static u8 firsttpd=0;	//按键处理标志,防止一次按下,多次返回
	u8 i=0XFF;
	tp_dev.scan(0);//扫描																		 
	if((tp_dev.sta&TP_PRES_DOWN))//有按键被按下
	{
		if(firsttpd==0)//第一次处理?
		{
			firsttpd=1;	  		//标记已经处理了此次按键
			for(i=0;i<16;i++)
			{
				if((tp_dev.x[0]>appdev->icos[i].x)&&(tp_dev.x[0]<appdev->icos[i].x+appdev->icos[i].width)
				 &&(tp_dev.y[0]>appdev->icos[i].y)&&(tp_dev.y[0]<appdev->icos[i].y+appdev->icos[i].height))//在区域内
				{
					break;//得到选中的编号	
				}
			}
		}
	}else firsttpd=0;//松开了
	return i;
}

//app应用
u8 app_play(void)
{		
	u8 selx=0XFF;
	u8 rval=0;
	m_app_dev *appdev;
	appdev=(m_app_dev*)gui_memin_malloc(sizeof(m_app_dev));
	if(appdev==NULL)rval=1;
	else if(appplay_load_ui(appdev))rval=1;
 	while(rval==0)
	{
		selx=appplay_tpscan(appdev);
		if(selx<16)//有有效按键
		{
			if(selx==appdev->selico)//第二次选择此条目
			{
				//printf("selx:%d\r\n",appdev->selico);
				switch(selx)
				{
					case 0://红外遥控器
						appplay_remote(appdev->icos[selx].name);
						system_task_return=0;
						continue; 
					case 1://陀螺仪
						appplay_gyro(appdev->icos[selx].name);
						system_task_return=0;
						break; 
					case 2://无线视频
						//appplay_wlvideo(appdev->icos[selx].name);
						system_task_return=0;
						break;					
				} 
				appplay_load_ui(appdev);//重新加载主界面
			}
			appplay_set_sel(appdev,selx);		 
		}
		if(system_task_return)break;//TPAD返回
		delay_ms(1000/OS_TICKS_PER_SEC);//延时一个时钟节拍
 	}
	gui_memin_free(appdev);//释放内存
	return 0;
}
 










