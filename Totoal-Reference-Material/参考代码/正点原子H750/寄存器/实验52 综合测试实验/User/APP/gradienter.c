#include "gradienter.h"
#include "gyroscope.h"
#include "icm20608.h"
#include "inv_icm20608.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-水平仪 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/6/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
  
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void grad_draw_hline(short x0,short y0,u16 len,u16 color)
{  
	short ex;
	ex=x0+len-1;
	if(y0<0)return;
	if(y0>(mlcddev.height-1))return;
	if(ex<0)return;
	if(ex>(mlcddev.width-1))ex=mlcddev.width-1;
	if(x0>(mlcddev.width-1))return;
	if(x0<0)x0=0;
	mlcd_fill(x0,y0,ex,y0,color);	
}

//画实心圆
//x0,y0:坐标
//r半径
//color:颜色
void grad_fill_circle(short x0,short y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	grad_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax) 
		{
			// draw lines from outside  
			if (x>imax) 
			{
				grad_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				grad_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		grad_draw_hline(x0-x,y0+i,2*x,color);
		grad_draw_hline(x0-x,y0-i,2*x,color);
	}
}

//加载字体
//返回值:0,正常;
//    其他,失败.
u8 grad_load_font(void)
{
	u8 rval=0;
	u8 res;
	FIL* f_grad=0;	 
	f_grad=(FIL *)gui_memin_malloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_grad==NULL)rval=1;							//申请失败
	else
	{
		if(lcddev.width<=272)
		{
			res=f_open(f_grad,(const TCHAR*)APP_ASCII_S7236,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_7236=(u8*)gui_memex_malloc(f_grad->obj.objsize);	//为大字体开辟缓存地址
				if(asc2_7236==0)rval=1;
				else res=f_read(f_grad,asc2_7236,f_grad->obj.objsize,(UINT*)&br);	//一次读取整个文件
			} 
		}else if(lcddev.width==320)
		{
			res=f_open(f_grad,(const TCHAR*)APP_ASCII_S8844,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_8844=(u8*)gui_memex_malloc(f_grad->obj.objsize);	//为大字体开辟缓存地址
				if(asc2_8844==0)rval=1;
				else res=f_read(f_grad,asc2_8844,f_grad->obj.objsize,(UINT*)&br);	//一次读取整个文件
			} 
		}else if(lcddev.width>=480)
		{
			res=f_open(f_grad,(const TCHAR*)APP_ASCII_S14472,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_14472=(u8*)gui_memex_malloc(f_grad->obj.objsize);	//为大字体开辟缓存地址
				if(asc2_14472==0)rval=1;
				else res=f_read(f_grad,asc2_14472,f_grad->obj.objsize,(UINT*)&br);	//一次读取整个文件
			}  
		} 
		if(res)rval=res;
	}
	gui_memin_free(f_grad);//释放内存
	return rval;
}
//删除字体 
void grad_delete_font(void)
{
	if(lcddev.width==240)
	{
		gui_memex_free(asc2_7236);
		asc2_7236=0;
	}else if(lcddev.width==320)
	{
		gui_memex_free(asc2_8844);
		asc2_8844=0;
	}else if(lcddev.width>=480)
	{
		gui_memex_free(asc2_14472);
		asc2_14472=0;
	}  
} 

//水平仪功能
u8 grad_play(void)
{ 
    OS_CPU_SR cpu_sr=0;
	u8 res,rval=0;   
	u8 t=0;
	u16 angy;				//显示角度的纵坐标起始位置
	u8 fsize;
	u8 *buf;
	u8 len;  
	u8 stable;
	u16 bkcolor=BLACK;		//默认背景色为黑色
	
	float pitch,roll,yaw; 	//欧拉角  	
	
	short xcenter,ycenter;	//中心坐标
	short xr1,yr1,wr1;		//圆1坐标和边长
	short xr2,yr2,wr2;		//圆2坐标和边长
	short scalefac;			//缩放因子
	short oldxr2,oldyr2;
	double angle;
	//提示开始检测ICM20608 的MPL
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)gyro_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	app_es8388_volset(0);				//静音 

	OS_ENTER_CRITICAL();				//进入临界区(无法被中断打断) 
	SCB_CleanInvalidateDCache();		//清空&失效cache
	res=icm_mpl_init();					//初始化DMP   
	OS_EXIT_CRITICAL();					//退出临界区(可以被中断打断)
	
	app_es8388_volset(es8388set.mvol);	//重新恢复音量	
	if(res)
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)gyro_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);   
		rval=1;
	}else rval=grad_load_font();//加载字体 
	if(lcddev.width==240)
	{
		fsize=72;angy=230;
	}else if(lcddev.width==272)
	{
		fsize=72;angy=350;
	}else if(lcddev.width==320)
	{
		fsize=88;angy=370;
	}else if(lcddev.width==480)
	{
		fsize=144;angy=600;
	}else if(lcddev.width==600)
	{
		fsize=144;angy=750;
	}else if(lcddev.width==800)
	{
		fsize=144;angy=800;
	}
	buf=gui_memin_malloc(32);	//申请内存  
	if(rval==0) 
	{
		mlcd_init(lcddev.width,lcddev.height,1);//初始化mlcd
		mlcd_clear(BLACK); 
		xcenter=lcddev.width/2;
		ycenter=lcddev.height/2; 
		wr1=lcddev.width/6;
		wr2=wr1+2; 
		scalefac=10;
		xr1=xcenter-wr1/2;
		yr1=ycenter-wr1/2;
		xr2=xcenter-wr2/2;
		yr2=ycenter-wr2/2;
		gui_phy.draw_point=(void(*)(u16,u16,u32))mlcd_draw_point;
		system_task_return=0;
		while(1)
		{ 
			if(system_task_return)
			{
				delay_ms(15);
				if(TPAD_Scan(1))break;//TPAD返回,再次确认,排除干扰	
				else system_task_return=0;
			}
			if(icm_mpl_get_data(&pitch,&roll,&yaw)==0)
			{
				if((t%10)==0)
				{  
					angle=sqrt(pitch*pitch+roll*roll); 
					if(angle>=0.1)
					{
						if(bkcolor!=BLACK)
						{
							bkcolor=BLACK; 
							oldxr2=0;oldyr2=0;
						}
						angle=-angle;
						stable=0;
					}else 
					{ 
						if(stable<10)stable++;	//等待状态稳定
						else
						{
							if(bkcolor!=GREEN)
							{
								bkcolor=GREEN; 
								oldxr2=0;oldyr2=0;
							} 
						}
					}
					yr1=ycenter+pitch*scalefac;
					yr2=ycenter-pitch*scalefac;
					xr1=xcenter+roll*scalefac;
					xr2=xcenter-roll*scalefac; 
					if(oldxr2!=xr2||oldyr2!=yr2)
					{  
						mlcd_clear(bkcolor);
						oldxr2=xr2;oldyr2=yr2; 
						grad_fill_circle(xr2,yr2,wr2,RED); 
						grad_fill_circle(xr1,yr1,wr1,BLUE); 
						sprintf((char*)buf,"%.1f",angle);
						len=strlen((char*)buf);
						buf[len]=95+' ';
						buf[len+1]=0; 
						gui_phy.back_color=bkcolor; 
						gui_show_strmid(0,angy,lcddev.width,fsize,WHITE,fsize,buf);	//显示新角度 
						mlcd_display_layer(0);
					}  
					t++;
					continue; 
				}
			}
			t++; 
			delay_ms(3);
		} 
		mlcd_delete();							//删除mlcd
		gui_phy.draw_point=LCD_Fast_DrawPoint;	//恢复为原来的画点函数 
	}
	grad_delete_font();	//删除字体
	gui_memin_free(buf);//释放内存
	return rval;
}


