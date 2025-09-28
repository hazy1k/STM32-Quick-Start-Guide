#include "appplay_gyro.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-陀螺仪测试 代码	   
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

//画圆形指针表盘
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度
void gyro_circle_panel(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;//得到半径 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1; 
	u16 i; 
	gui_fill_circle(x,y,r,WHITE);		//画外圈
	gui_fill_circle(x,y,r-4,BLACK);		//画内圈
	for(i=0;i<60;i++)//画6°格
	{ 
		px0=sx+r+(r-4)*sin((app_pi/30)*i); 
		py0=sy+r-(r-4)*cos((app_pi/30)*i); 
		px1=sx+r+(r-d)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d)*cos((app_pi/30)*i);  
		gui_draw_bline1(px0,py0,px1,py1,0,WHITE);		
	}
	for(i=0;i<12;i++)//画30°格
	{ 
		px0=sx+r+(r-5)*sin((app_pi/6)*i); 
		py0=sy+r-(r-5)*cos((app_pi/6)*i); 
		px1=sx+r+(r-d)*sin((app_pi/6)*i); 
		py1=sy+r-(r-d)*cos((app_pi/6)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);		
	}
	for(i=0;i<4;i++)//画90°格
	{ 
		px0=sx+r+(r-5)*sin((app_pi/2)*i); 
		py0=sy+r-(r-5)*cos((app_pi/2)*i); 
		px1=sx+r+(r-d-3)*sin((app_pi/2)*i); 
		py1=sy+r-(r-d-3)*cos((app_pi/2)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);		
	}
} 
//显示指针
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度 
//arg:角度 -1800~1800,单位:0.1°
//color:颜色值
void gyro_circle_show(u16 x,u16 y,u16 size,u16 d,s16 arg,u16 color)
{
	u16 r=size/2;//得到半径  
	u16 px1,py1;   
	px1=x+(r-2*d-5)*sin((app_pi/1800)*arg); 
	py1=y-(r-2*d-5)*cos((app_pi/1800)*arg); 
	gui_draw_bline1(x,y,px1,py1,1,color); 
	gui_fill_circle(x+1,y+1,d,color);		//画中心圈
}
//显示俯仰角
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度 
//parg:俯仰角 -900~900,单位:0.1°
//color:颜色值
void gyro_circle_pitch(u16 x,u16 y,u16 size,u16 d,s16 parg)
{
	static s16 oldpitch=0; 
	u8 *buf;
	float temp;
	if(oldpitch!=parg)
	{
		buf=gui_memin_malloc(100);
		gyro_circle_show(x,y,size,d,oldpitch,BLACK);//先清除原来的显示	
		temp=(float)parg/10;
		sprintf((char*)buf,"%.1f°",temp);//百分比  
		gui_fill_rectangle(x-21+6,y+(size/4)-6,42,12,BLACK);//填充底色 
		gui_show_strmid(x-21+6,y+(size/4)-6,42,12,GREEN,12,buf);//显示角度
		gyro_circle_show(x,y,size,d,parg,GREEN);//指向新的值
		oldpitch=parg;	
		gui_memin_free(buf);
	}
}
//显示横滚角
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度 
//parg:横滚角 -1800~1800,单位:0.1°
//color:颜色值
void gyro_circle_roll(u16 x,u16 y,u16 size,u16 d,s16 rarg)
{
	static s16 oldroll=0; 
	u8 *buf;
	float temp;
	if(oldroll!=rarg)
	{
		buf=gui_memin_malloc(100);
		gyro_circle_show(x,y,size,d,oldroll,BLACK);//先清除原来的显示
		temp=(float)rarg/10;
		sprintf((char*)buf,"%.1f°",temp);//百分比  
		gui_fill_rectangle(x-24+6,y+(size/4)-6,48,12,BLACK);//填充底色 
		gui_show_strmid(x-24+6,y+(size/4)-6,48,12,RED,12,buf);//显示角度
		gyro_circle_show(x,y,size,d,rarg,RED);//指向新的值
		oldroll=rarg;	
		gui_memin_free(buf);
	}
}
//显示航向角
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度 
//parg:航向角 -1800~1800,单位:0.1°
//color:颜色值
void gyro_circle_yaw(u16 x,u16 y,u16 size,u16 d,s16 yarg)
{
	static s16 oldyaw=0; 
	u8 *buf;
	float temp;
	if(oldyaw!=yarg)
	{
		buf=gui_memin_malloc(100);
		gyro_circle_show(x,y,size,d,oldyaw,BLACK);//先清除原来的显示
		temp=(float)yarg/10;
		sprintf((char*)buf,"%.1f°",temp);//百分比  
		gui_fill_rectangle(x-24+6,y+(size/4)-6,48,12,BLACK);	//填充底色 
		gui_show_strmid(x-24+6,y+(size/4)-6,48,12,YELLOW,12,buf);//显示角度
		gyro_circle_show(x,y,size,d,yarg,YELLOW);//指向新的值
		oldyaw=yarg;
		gui_memin_free(buf);
	}
}
//DMP提示
u8*const gyro_remind_tbl[2][GUI_LANGUAGE_NUM]=
{
{"初始化DMP,请稍侯...","初始化DMP,請稍後...","DMP Init,Please wait...",},	 
{"初始化错误,请检查...","初始化錯誤,請檢查...","Init Error,Please check...",},  
};
//DMP信息
u8*const gyro_msg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"俯仰角","俯仰角","PITCH",},	 
{"横滚角","橫滾角","ROLL",}, 
{"航向角","航向角","YAW",},  
};

//6轴陀螺仪演示
u8 appplay_gyro(u8* caption)
{ 
	float pitch,roll,yaw; 
	short temp;
	u8 rval=0;	
	u8 rpr,ry;
	u8 dpr,dy;
	u16 xp;
	u16 ypr;
	u8 fsize; 
 	//提示开始检测MPU6050 的DMP
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)gyro_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(mpu_dmp_init())//初始化DMP
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)gyro_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);  
		rval=1;
	}else//初始化OK
	{
		LCD_Clear(BLACK);
		app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);			//下分界线	 
		gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)caption);//显示标题  
	
		if(lcddev.width==240)
		{
			rpr=59;ry=80;
			dpr=7;dy=8;
			xp=60;ypr=60+gui_phy.tbheight;
			fsize=12;
		}else if(lcddev.width==320)
		{
			rpr=79;ry=140;
			dpr=8;dy=9;
			xp=80;ypr=80+gui_phy.tbheight;
			fsize=12;
		}else if(lcddev.width==480)
		{
			rpr=110;ry=200;
			dpr=10;dy=12;
			xp=120;ypr=120+gui_phy.tbheight;
			fsize=16;
		} 
		gyro_circle_panel(xp,ypr,2*rpr,dpr);					//画俯仰角表盘
		gui_show_strmid(xp-(3*fsize)/2,ypr+xp,3*fsize,fsize,GREEN,fsize,(u8*)gyro_msg_tbl[0][gui_phy.language]);//显示名字
		gyro_circle_panel(xp+lcddev.width/2,ypr,2*rpr,dpr);		//画横滚角表盘
		gui_show_strmid(lcddev.width/2+xp-(3*fsize)/2,ypr+xp,3*fsize,fsize,RED,fsize,(u8*)gyro_msg_tbl[1][gui_phy.language]);//显示名字
		gyro_circle_panel(lcddev.width/2,xp*2+gui_phy.tbheight+ry,2*ry,dy);	//画航向角表盘
		if(lcddev.width==480)gui_show_strmid(lcddev.width/2-(3*fsize)/2,xp*2+gui_phy.tbheight+ry*2+10,3*fsize,fsize,YELLOW,fsize,(u8*)gyro_msg_tbl[2][gui_phy.language]);//显示名字  
		else gui_show_strmid(lcddev.width/2-(3*fsize)/2,xp*2+gui_phy.tbheight+ry*2+2,3*fsize,fsize,YELLOW,fsize,(u8*)gyro_msg_tbl[2][gui_phy.language]);//显示名字  
 		gyro_circle_pitch(xp,ypr,2*rpr,dpr,360);				//让指针显示出来
		gyro_circle_roll(xp+lcddev.width/2,ypr,2*rpr,dpr,360);	//让指针显示出来
		gyro_circle_yaw(lcddev.width/2,xp*2+gui_phy.tbheight+ry,2*ry,dy,360);	//让指针显示出来
		while(1)
		{
			if(system_task_return)break;//TPAD返回
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//读取DMP数据
			{  
				temp=pitch*10;
				gyro_circle_pitch(xp,ypr,2*rpr,dpr,temp);
				temp=roll*10;
				gyro_circle_roll(xp+lcddev.width/2,ypr,2*rpr,dpr,temp);
				temp=yaw*10;
				gyro_circle_yaw(lcddev.width/2,xp*2+gui_phy.tbheight+ry,2*ry,dy,temp);
 			}
			delay_ms(10);  
		}
	}
	return rval;
}





















