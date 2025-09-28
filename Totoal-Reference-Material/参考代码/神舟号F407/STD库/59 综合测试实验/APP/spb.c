#include "spb.h" 
#include "piclib.h"
#include "touch.h"
#include "delay.h"
#include "string.h"
#include "common.h"
#include "calendar.h"
#include "sim900a.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SPB效果实现 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//SPB 控制器
m_spb_dev spbdev; 


//背景图路径,根据不同的lcd选择不同的路径
u8*const spb_bkpic_path_tbl[3][2]=
{
	{
		"1:/SYSTEM/SPB/BACKPIC/left_240224.jpg",
		"1:/SYSTEM/SPB/BACKPIC/right_240224.jpg",	
	},
	{
		"1:/SYSTEM/SPB/BACKPIC/left_320364.jpg",
		"1:/SYSTEM/SPB/BACKPIC/right_320364.jpg",	
	},
	{
		"1:/SYSTEM/SPB/BACKPIC/back_480610.jpg",
		"",	
	},	
};	
//界面图标的路径表
u8*const spb_icos_path_tbl[3][2][8]=
{
{
	{
		"1:/SYSTEM/SPB/ICOS/ebook_56.bmp",
	 	"1:/SYSTEM/SPB/ICOS/picture_56.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/music_56.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/video_56.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/clock_56.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/set_56.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/game_56.bmp",		    
	  	"1:/SYSTEM/SPB/ICOS/notepad_56.bmp",  
	},	    
 	{ 
	 	"1:/SYSTEM/SPB/ICOS/setup_56.bmp",	   
		"1:/SYSTEM/SPB/ICOS/paint_56.bmp",
	 	"1:/SYSTEM/SPB/ICOS/camera_56.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/recorder_56.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/usb_56.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/earthnet_56.bmp",
	  	"1:/SYSTEM/SPB/ICOS/wireless_56.bmp",	
	  	"1:/SYSTEM/SPB/ICOS/calc_56.bmp",	  
	},	
},
{
	{
		"1:/SYSTEM/SPB/ICOS/ebook_70.bmp",
	 	"1:/SYSTEM/SPB/ICOS/picture_70.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/music_70.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/video_70.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/clock_70.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/set_70.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/game_70.bmp",		    
	  	"1:/SYSTEM/SPB/ICOS/notepad_70.bmp",  
	},	    
 	{ 
	 	"1:/SYSTEM/SPB/ICOS/setup_70.bmp",	   
		"1:/SYSTEM/SPB/ICOS/paint_70.bmp",
	 	"1:/SYSTEM/SPB/ICOS/camera_70.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/recorder_70.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/usb_70.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/earthnet_70.bmp",
	  	"1:/SYSTEM/SPB/ICOS/wireless_70.bmp",	
	  	"1:/SYSTEM/SPB/ICOS/calc_70.bmp",	  
	},	
},
{
	{
		"1:/SYSTEM/SPB/ICOS/ebook_110.bmp",
	 	"1:/SYSTEM/SPB/ICOS/picture_110.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/music_110.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/video_110.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/clock_110.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/set_110.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/game_110.bmp",		    
	  	"1:/SYSTEM/SPB/ICOS/notepad_110.bmp",  
	},	    
 	{ 
	 	"1:/SYSTEM/SPB/ICOS/setup_110.bmp",	   
		"1:/SYSTEM/SPB/ICOS/paint_110.bmp",
	 	"1:/SYSTEM/SPB/ICOS/camera_110.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/recorder_110.bmp",	    
	  	"1:/SYSTEM/SPB/ICOS/usb_110.bmp",	    
	 	"1:/SYSTEM/SPB/ICOS/earthnet_110.bmp",
	  	"1:/SYSTEM/SPB/ICOS/wireless_110.bmp",	
	  	"1:/SYSTEM/SPB/ICOS/calc_110.bmp",	  
	},	
},
};
//三个主图标的路径表
u8*const spb_micos_path_tbl[3][3]=
{ 
{
	"1:/SYSTEM/SPB/ICOS/phone_56.bmp",
	"1:/SYSTEM/SPB/ICOS/app_56.bmp",	    
	"1:/SYSTEM/SPB/ICOS/sms_56.bmp",	   
},	    
{ 
	"1:/SYSTEM/SPB/ICOS/phone_70.bmp",
	"1:/SYSTEM/SPB/ICOS/app_70.bmp",	    
	"1:/SYSTEM/SPB/ICOS/sms_70.bmp",  
},	    
{ 
	"1:/SYSTEM/SPB/ICOS/phone_110.bmp",
	"1:/SYSTEM/SPB/ICOS/app_110.bmp",	    
	"1:/SYSTEM/SPB/ICOS/sms_110.bmp",
},	 
};
//界面图标ICOS的对应功能名字
u8*const icos_name_tbl[GUI_LANGUAGE_NUM][2][8]=
{
	{
		{
			"电子图书","数码相框","音乐播放","视频播放",
			"时钟","系统设置","FC游戏机","记事本",	   
		},
		{
			"运行器","手写画笔","照相机","录音机",
			"USB 连接","网络通信","无线传书","计算器",
		},
	},
	{
		{
			"電子圖書","數碼相框","音樂播放","視頻播放",
			"時鐘","系統設置","FC遊戲機","記事本",	   
		},
		{
			"運行器","手寫畫筆","照相机","錄音機 ",
			"USB 連接","網絡通信","無線傳書","計算器",
		},
	},
	{
		{
			"EBOOK","PHOTOS","MUSIC","VIDEO",
			"TIME","SETTINGS","FC GAMES","NOTEPAD",	   
		},
		{
			"EXE","PAINT","CAMERA","RECODER",
			"USB","ETHERNET","WIRELESS","CALC",
		},
	},
};						  
//主图标对应的名字
u8*const micos_name_tbl[GUI_LANGUAGE_NUM][3]=
{ 
{
	"拨号","应用中心","短信",    
},
{
	"撥號","應用中心","短信", 
}, 
{
	"PHONE","APPS","SMS", 
}, 
}; 
//初始化spb各个参数	
//返回值:0,操作成功
//    其他,错误代码
u8 spb_init(void)
{
	u16 i,j;
	u8 res=0;
	u8 lcdtype=0;//0,240*320;1,320*480;2,480*800
	memset((void*)&spbdev,0,sizeof(spbdev)); 
	spbdev.selico=0xff; 
	if(lcddev.width==240)	//对于240*320的LCD屏幕
	{ 
		lcdtype=0;
		spbdev.stabarheight=20;
		spbdev.spbheight=224;
		spbdev.spbwidth =240; 
		for(i=0;i<2;i++)
		{
			for(j=0;j<8;j++)
			{
				spbdev.icos[i][j].x=2+(j%4)*60;
				spbdev.icos[i][j].y=spbdev.stabarheight+4+(j/4)*74;
				spbdev.icos[i][j].width=56;//必须 等于图片的宽度尺寸
				spbdev.icos[i][j].height=56+2+12+1;
				spbdev.icos[i][j].path=(u8*)spb_icos_path_tbl[lcdtype][i][j];
				spbdev.icos[i][j].name=(u8*)icos_name_tbl[gui_phy.language][i][j];
 			}
		}
		for(i=0;i<3;i++)
		{
			spbdev.micos[i].x=30+i*60;
			spbdev.micos[i].y=spbdev.stabarheight+spbdev.spbheight+4;
			spbdev.micos[i].width=56;//必须 等于图片的宽度尺寸
			spbdev.micos[i].height=56+12+2;
			spbdev.micos[i].path=(u8*)spb_micos_path_tbl[lcdtype][i];
			spbdev.micos[i].name=(u8*)micos_name_tbl[gui_phy.language][i]; 
		}
	}else if(lcddev.width==320)	//对于320*480的LCD屏幕
	{
		lcdtype=1;
		spbdev.stabarheight=24;
		spbdev.spbheight=364;
		spbdev.spbwidth =320; 
		for(i=0;i<2;i++)
		{
			for(j=0;j<8;j++)
			{
				spbdev.icos[i][j].x=5+(j%4)*80;
				spbdev.icos[i][j].y=spbdev.stabarheight+6+(j/4)*90;
				spbdev.icos[i][j].width=70;//必须 等于图片的宽度尺寸
				spbdev.icos[i][j].height=70+2+12+1;
				spbdev.icos[i][j].path=(u8*)spb_icos_path_tbl[lcdtype][i][j];
				spbdev.icos[i][j].name=(u8*)icos_name_tbl[gui_phy.language][i][j];
 			}
		} 
		for(i=0;i<3;i++)
		{
			spbdev.micos[i].x=40+i*80;
			spbdev.micos[i].y=spbdev.stabarheight+spbdev.spbheight+6;
			spbdev.micos[i].width=70;//必须 等于图片的宽度尺寸
			spbdev.micos[i].height=70+12+2;
			spbdev.micos[i].path=(u8*)spb_micos_path_tbl[lcdtype][i];
			spbdev.micos[i].name=(u8*)micos_name_tbl[gui_phy.language][i]; 
		}	
	}else if(lcddev.width==480)	//对于480*800的LCD屏幕
	{ 
		lcdtype=2;
		spbdev.stabarheight=30;
		spbdev.spbheight=610;
		spbdev.spbwidth =480; 
		for(i=0;i<16;i++)
		{
			spbdev.icos[i/8][i%8].x=5+(i%4)*120;
			spbdev.icos[i/8][i%8].y=spbdev.stabarheight+10+(i/4)*150;
			spbdev.icos[i/8][i%8].width=110;//必须 等于图片的宽度尺寸
			spbdev.icos[i/8][i%8].height=110+6+16+2;
			spbdev.icos[i/8][i%8].path=(u8*)spb_icos_path_tbl[lcdtype][i/8][i%8];
			spbdev.icos[i/8][i%8].name=(u8*)icos_name_tbl[gui_phy.language][i/8][i%8]; 
		} 		
		for(i=0;i<3;i++)
		{
			spbdev.micos[i].x=60+i*120;
			spbdev.micos[i].y=spbdev.stabarheight+spbdev.spbheight+18;
			spbdev.micos[i].width=110;
			spbdev.micos[i].height=110+16+6;
			spbdev.micos[i].path=(u8*)spb_micos_path_tbl[lcdtype][i];
			spbdev.micos[i].name=(u8*)micos_name_tbl[gui_phy.language][i]; 
		}	
	} 
	//指向EX SRAM LCD BUF  
	pic_phy.read_point=slcd_read_point;
	pic_phy.draw_point=slcd_draw_point;	 
	pic_phy.fillcolor=slcd_fill_color; 
	gui_phy.read_point=slcd_read_point;
	gui_phy.draw_point=slcd_draw_point;	 
	if(lcdtype==2)
	{ 
		sramlcdbuf=gui_memex_malloc(lcddev.width*lcddev.height*2);	//1帧缓存就够了
		if(sramlcdbuf==NULL)return 1;//错误	
		spbdev.frame=0;
		res=ai_load_picfile(spb_bkpic_path_tbl[2][0],0,0,spbdev.spbwidth,spbdev.spbheight,0);//4.3寸屏,只有一张背景图片
		if(res==0)res=spb_load_icos(0);		//加载前半图标
		if(res==0)res=spb_load_icos(1);		//加载后半图标 
	}else
	{
		sramlcdbuf=gui_memex_malloc(spbdev.spbheight*spbdev.spbheight*4);			//需要2帧缓存
		if(sramlcdbuf==NULL)return 1;//错误	
		spbdev.frame=0;
		res=ai_load_picfile(spb_bkpic_path_tbl[lcdtype][0],0,0,spbdev.spbwidth,spbdev.spbheight,0);//加载第一页背景图片
		if(res==0)res=spb_load_icos(0);		//加载第0帧图标 
		spbdev.frame=1;
		if(res==0)res=ai_load_picfile(spb_bkpic_path_tbl[lcdtype][1],0,0,spbdev.spbwidth,spbdev.spbheight,0);//加载第二页背景图片
		if(res==0)res=spb_load_icos(1);		//加载第1帧图标
	}  
	//指向LCD
	pic_phy.read_point=LCD_ReadPoint;
	pic_phy.draw_point=LCD_Fast_DrawPoint;	 
	pic_phy.fillcolor=piclib_fill_color;  
	gui_phy.read_point=LCD_ReadPoint;
	gui_phy.draw_point=LCD_Fast_DrawPoint;
	spbdev.frame=0;//默认是低0帧
	return 0;
} 
//删除SPB
void spb_delete(void)
{
	memset((void*)&spbdev,0,sizeof(spbdev));
	gui_memex_free(sramlcdbuf);
}
//装载主界面ICOS
//frame:帧编号 
//返回值:0,操作成功
//    其他,错误代码
u8 spb_load_icos(u8 frame)
{
	u8 j;
	u8 res=0; 
	u8 fontsize=12;
	for(j=0;j<8;j++)
	{
		res=minibmp_decode(spbdev.icos[frame][j].path,spbdev.icos[frame][j].x,spbdev.icos[frame][j].y-spbdev.stabarheight,spbdev.icos[frame][j].width,spbdev.icos[frame][j].width,0,0);
 		if(res)return 1;
		if(lcddev.width==480)fontsize=16;//4.3屏采用16*16字体 
		gui_show_strmid(spbdev.icos[frame][j].x,spbdev.icos[frame][j].y+spbdev.icos[frame][j].width-spbdev.stabarheight,spbdev.icos[frame][j].width,fontsize,SPB_FONT_COLOR,fontsize,spbdev.icos[frame][j].name);//显示名字  
	} 
	return 0;
} 
//装载主界面ICOS
//frame:帧编号 
//返回值:0,操作成功
//    其他,错误代码
u8 spb_load_micos(void)
{
	u8 j;
	u8 res=0;  
	u8 fontsize=12;
	gui_fill_rectangle(0,spbdev.stabarheight+spbdev.spbheight,lcddev.width,lcddev.height-spbdev.stabarheight-spbdev.spbheight,SPB_MICO_BKCOLOR);
	for(j=0;j<3;j++)
	{
		res=minibmp_decode(spbdev.micos[j].path,spbdev.micos[j].x,spbdev.micos[j].y,spbdev.micos[j].width,spbdev.micos[j].width,0,0);
		if(res)return 1;
		if(lcddev.width==480)fontsize=16;//4.3屏采用16*16字体 
		gui_show_strmid(spbdev.micos[j].x,spbdev.micos[j].y+spbdev.micos[j].width,spbdev.micos[j].width,fontsize,SPB_FONT_COLOR,fontsize,spbdev.micos[j].name);//显示名字  
	} 
	return 0;
}
//SD卡图标
//PCtoLCD2002取模方式:阴码,逐行式,顺向
const u8 SPB_SD_ICO[60]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,
0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFC,
0x00,0x0F,0xFE,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFC,0x00,0x0A,0xAC,0x00,
0x0A,0xAC,0x00,0x0A,0xAC,0x00,0x0F,0xFC,0x00,0x00,0x00,0x00,
};
//USB 图标 
//PCtoLCD2002取模方式:阴码,逐行式,顺向
const u8 SPB_USB_ICO[60]=
{
0x00,0x00,0x00,0x00,0x60,0x00,0x00,0xF0,0x00,0x01,0xF8,0x00,0x00,0x60,0x00,0x00,
0x67,0x00,0x04,0x67,0x00,0x0E,0x62,0x00,0x0E,0x62,0x00,0x04,0x62,0x00,0x04,0x7C,
0x00,0x06,0x60,0x00,0x03,0xE0,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x60,0x00,
0x00,0xF0,0x00,0x00,0xF0,0x00,0x00,0x60,0x00,0x00,0x00,0x00,
}; 
//显示gsm信号强度,占用20*20像素大小
//x,y:起始坐标
//signal:信号强度,范围:0~30
void spb_gsm_signal_show(u16 x,u16 y,u8 signal)
{ 
	u8 t;
	u16 color;
	signal/=5;
	if(signal>5)signal=5;
	for(t=0;t<5;t++)
	{
		if(signal)//有信号
		{
			signal--;
			color=WHITE;
		}else color=0X6B4D;//无信号
		gui_fill_rectangle(x+1+t*4,y+20-6-t*3,3,(t+1)*3,color); 
	}
}
//GSM模塊提示信息 
u8* const spb_sim900a_msg[GUI_LANGUAGE_NUM][3]=
{
"无移动网","中国移动","中国联通",
"無移動網","中國移動","中國聯通",
" NO NET ","CHN Mobi","CHN Unic",
}; 
//更新顶部信息条信息数据
//clr:0,不清除背景
//    1,清除背景
void spb_stabar_msg_show(u8 clr)
{ 
	u8 temp; 
	if(clr)
	{
		gui_fill_rectangle(0,0,lcddev.width,spbdev.stabarheight,BLACK);
		gui_show_string("CPU:  %",24+64+20+2+20+2,(spbdev.stabarheight-16)/2,64,16,16,WHITE);//显示CPU数据
		gui_show_string(":",lcddev.width-42+16,(spbdev.stabarheight-16)/2,8,16,16,WHITE);		//显示':'
  	}
	//SIM900A信息显示
	spb_gsm_signal_show(2,(spbdev.stabarheight-20)/2,sim900dev.csq);//显示信号质量
	gui_fill_rectangle(2+20+2,(spbdev.stabarheight-16)/2,64,16,BLACK);
	if(sim900dev.status&(1<<5))
	{
		if(sim900dev.status&(1<<4))temp=2;
		else temp=1;
		gui_show_string(spb_sim900a_msg[gui_phy.language][temp],2+20+2,(spbdev.stabarheight-16)/2,64,16,16,WHITE);//显示运营商名字
	}else gui_show_string(spb_sim900a_msg[gui_phy.language][0],2+20+2,(spbdev.stabarheight-16)/2,64,16,16,WHITE);//显示运营商名字	
	//更新SD卡信息 
	if(gui_phy.memdevflag&(1<<0))app_show_mono_icos(24+64+2,(spbdev.stabarheight-20)/2,20,20,(u8*)SPB_SD_ICO,WHITE,BLACK);
	else gui_fill_rectangle(24+64+2,0,20,spbdev.stabarheight,BLACK);
	//更新U盘信息 
	if(gui_phy.memdevflag&(1<<2))app_show_mono_icos(24+64+2+20,(spbdev.stabarheight-20)/2,20,20,(u8*)SPB_USB_ICO,WHITE,BLACK);
	else gui_fill_rectangle(24+64+2+20,0,20,spbdev.stabarheight,BLACK);
	//显示CPU使用率
	gui_phy.back_color=BLACK;
	temp=OSCPUUsage;
	if(OSCPUUsage>99)temp=99;//最多显示到99%
	gui_show_num(24+64+2+20+20+2+32,(spbdev.stabarheight-16)/2,2,WHITE,16,temp,0);//显示CPU使用率
	//显示时间
	calendar_get_time(&calendar);
	gui_show_num(lcddev.width-42,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.hour,0X80);	//显示时钟
	gui_show_num(lcddev.width-2-16,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.min,0X80);//显示分钟	
}
extern u8*const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM];
//加载SPB主界面UI
//返回值:0,操作成功
//    其他,错误代码
u8 spb_load_mui(void)
{
	u8 res=0;
	if(spbdev.spbheight==0&&spbdev.spbwidth==0)
	{
		LCD_Clear(BLACK);				//黑屏
		window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)sysset_remindmsg_tbl[1][gui_phy.language],(u8*)sysset_remindmsg_tbl[0][gui_phy.language],12,0,0,0);
 		res=spb_init(); 
	}
	if(res==0)
	{
		spb_stabar_msg_show(1);			//显示状态栏信息,清除所有后显示
		slcd_frame_show(spbdev.pos);	//显示主界面ICOS
		res=spb_load_micos();			//加载主图标
	}
	return res;
}
//移动屏幕
//dir:方向,0:左移;1,右移
//skips:每次跳跃列数
//pos:起始位置
void spb_frame_move(u8 dir,u8 skips,u16 pos)
{
	u16 x ;				 
	if(dir)//屏幕右移
	{
		for(x=pos;x>0;)
		{
			if(x>skips)x-=skips;
			else x=0;
			slcd_frame_show(x);
 	 	}
		spbdev.frame=0;		
	}else //屏幕左移
	{
		for(x=pos;x<lcddev.width;)
		{
			x+=skips;
			if(x>lcddev.width)x=lcddev.width;
			slcd_frame_show(x);
 	 	}
		spbdev.frame=1;		
	} 
} 
//清除某个mico图标的选中状态
//selx:16~18,表示将要清除选中状态的mico编号.
void spb_unsel_micos(u8 selx)
{ 
	u8 fontsize=12;
	if(selx>=16&&selx<19)	//合法的编号
	{
		selx-=16;
		gui_fill_rectangle(spbdev.micos[selx].x,spbdev.micos[selx].y,spbdev.micos[selx].width,spbdev.micos[selx].height,SPB_MICO_BKCOLOR); 
		minibmp_decode(spbdev.micos[selx].path,spbdev.micos[selx].x,spbdev.micos[selx].y,spbdev.micos[selx].width,spbdev.micos[selx].width,0,0);
 		if(lcddev.width==480)fontsize=16;//4.3屏采用16*16字体
		gui_show_strmid(spbdev.micos[selx].x,spbdev.micos[selx].y+spbdev.micos[selx].width,spbdev.micos[selx].width,fontsize,SPB_FONT_COLOR,fontsize,spbdev.micos[selx].name);//显示名字
	}	
}
//设置选中哪个图标
//sel:0~18代表当前页的选中ico 
void spb_set_sel(u8 sel)
{ 
	u8 fontsize=12;
	slcd_frame_show(spbdev.pos);	//刷新背景  
	spb_unsel_micos(spbdev.selico);	//清除主图标选中状态
	spbdev.selico=sel;
	if(lcddev.width==480)fontsize=16;//4.3屏采用16*16字体
	if(sel<16)
	{
		sel%=8;
		gui_alphablend_area(spbdev.icos[spbdev.frame][sel].x,spbdev.icos[spbdev.frame][sel].y,spbdev.icos[spbdev.frame][sel].width,spbdev.icos[spbdev.frame][sel].height,SPB_ALPHA_COLOR,SPB_ALPHA_VAL);
		minibmp_decode(spbdev.icos[spbdev.frame][sel].path,spbdev.icos[spbdev.frame][sel].x,spbdev.icos[spbdev.frame][sel].y,spbdev.icos[spbdev.frame][sel].width,spbdev.icos[spbdev.frame][sel].width,0,0);
		gui_show_strmid(spbdev.icos[spbdev.frame][sel].x,spbdev.icos[spbdev.frame][sel].y+spbdev.icos[spbdev.frame][sel].width,spbdev.icos[spbdev.frame][sel].width,fontsize,SPB_FONT_COLOR,fontsize,spbdev.icos[spbdev.frame][sel].name);//显示名字
	}else
	{
		sel-=16;
		gui_alphablend_area(spbdev.micos[sel].x,spbdev.micos[sel].y,spbdev.micos[sel].width,spbdev.micos[sel].height,SPB_ALPHA_COLOR,SPB_ALPHA_VAL); 
		minibmp_decode(spbdev.micos[sel].path,spbdev.micos[sel].x,spbdev.micos[sel].y,spbdev.micos[sel].width,spbdev.micos[sel].width,0,0);
		gui_show_strmid(spbdev.micos[sel].x,spbdev.micos[sel].y+spbdev.micos[sel].width,spbdev.micos[sel].width,fontsize,SPB_FONT_COLOR,fontsize,spbdev.micos[sel].name);//显示名字
	}
} 
//屏幕滑动及按键检测
//返回值:0~17,被双击的图标编号.		    
//       0xff,没有任何图标被双击或者按下
u8 spb_move_chk(void)
{		 
	u8 i=0xff;
	u16 movecnt=0;	//得到滑动点数
	u8 skips=5;		//默认每次跳5个像素
	tp_dev.scan(0);	//扫描										 
	if(lcddev.width==320)skips=10;	//3.5屏跳10个像素
	if(lcddev.width==480)			//4.3屏,不支持滑动,单独处理
	{
		if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
		{ 		  		    
			spbdev.spbsta|=1<<15;				//标记按下	    
			spbdev.curxpos=tp_dev.x[0];			//记录当前坐标
			spbdev.curypos=tp_dev.y[0];			//记录当前坐标	
		}else if(spbdev.spbsta&0x8000)//之前有按下
		{
			for(i=0;i<19;i++)
			{
				if(i<16)//界面icos
				{
					spbdev.frame=i/8;
					if((spbdev.curxpos>spbdev.icos[spbdev.frame][i%8].x)&&(spbdev.curxpos<spbdev.icos[spbdev.frame][i%8].x+spbdev.icos[spbdev.frame][i%8].width)&&(spbdev.curxpos>spbdev.icos[spbdev.frame][i%8].x)&&
					   (spbdev.curypos<spbdev.icos[spbdev.frame][i%8].y+spbdev.icos[spbdev.frame][i%8].height))
					{
						break;//得到选中的编号	
					}
				}else	//主icos
				{
					if((spbdev.curxpos>spbdev.micos[i-16].x)&&(spbdev.curxpos<spbdev.micos[i-16].x+spbdev.micos[i-16].width)&&
					   (spbdev.curypos>spbdev.micos[i-16].y)&&(spbdev.curypos<spbdev.micos[i-16].y+spbdev.micos[i-16].height))
					{
						break;//得到选中的编号	
					}
				}
			}
			if(i<19)
			{
				if(i!=spbdev.selico)//选中了不同的图标,切换图标
				{
					spb_set_sel(i);
					i=0xff;
				}else
				{
					spbdev.selico=0XFF;//发生了双击,重新复位selico.
				}
			}else i=0xff;//无效的点按.
			spbdev.spbsta=0;//清空标志
		} 
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
		{
			if(spbdev.spbsta&0X8000)//已经被标记了
			{
				movecnt=spbdev.spbsta&0X3FFF;//得到滑动点数
				if(gui_disabs(spbdev.curxpos,tp_dev.x[0])>=SPB_MOVE_WIN)//移动大于等于SPB_MOVE_WIN个点
				{
					if(movecnt<SPB_MOVE_MIN/SPB_MOVE_WIN)spbdev.spbsta++;//点数增加1	
				}
			}			  		    
			spbdev.curxpos=tp_dev.x[0];				//记录当前坐标
			spbdev.curypos=tp_dev.y[0];				//记录当前坐标
			if((spbdev.spbsta&0X8000)==0)			//按键第一次被按下
			{
				if(spbdev.curxpos>4096||spbdev.curypos>4096)return 0XFF;//非法的数据
				spbdev.spbsta=0;				 	//状态清零
				spbdev.spbsta|=1<<15;				//标记按下	    
				spbdev.oldxpos=tp_dev.x[0];			//记录按下时的坐标 
			}else if(spbdev.spbsta&0X4000)			//有滑动
			{
				if(spbdev.oldxpos>tp_dev.x[0]) 		//x左移,屏幕pos右移
				{
					if(spbdev.pos<lcddev.width)spbdev.pos+=spbdev.oldxpos-tp_dev.x[0]; 
					if(spbdev.pos>lcddev.width)spbdev.pos=lcddev.width;	//溢出了. 
				}else 								//右移,屏幕pos左移
				{			    
					if(spbdev.pos>0)spbdev.pos-=tp_dev.x[0]-spbdev.oldxpos;
					if(spbdev.pos>lcddev.width)spbdev.pos=0;		//溢出了.	   		  
				}	    
				spbdev.oldxpos=tp_dev.x[0];
				slcd_frame_show(spbdev.pos);
			}else if((gui_disabs(spbdev.curxpos,spbdev.oldxpos)>=SPB_MOVE_MIN)&&(movecnt>=SPB_MOVE_MIN/SPB_MOVE_WIN))//滑动距离超过SPB_MOVE_MIN,并且检测到的有效滑动数不少于SPB_MOVE_MIN/SPB_MOVE_WIN.
			{ 
				spbdev.spbsta|=1<<14;//标记滑动	
			}   
		}else //按键松开了
		{
			
			if(spbdev.spbsta&0x8000)//之前有按下
			{	  
				if(spbdev.spbsta&0X4000)//有滑动
				{										    
					if(spbdev.frame==1)//原来在第二帧,只能右移 
					{	
						if((lcddev.width-spbdev.pos)>SPB_MOVE_ACT)
						{
							spb_frame_move(1,skips,spbdev.pos);
							spbdev.pos=0;
						}else 
						{
							spb_frame_move(0,skips,spbdev.pos);
							spbdev.pos=lcddev.width;
						}			 
					}else	   //原来在第一帧,只能左移  0开始
					{
						if(spbdev.pos>SPB_MOVE_ACT)
						{
							spb_frame_move(0,skips,spbdev.pos);
							spbdev.pos=lcddev.width;
						}else 
						{	
							spb_frame_move(1,skips,spbdev.pos);
							spbdev.pos=0;
						}		   

					}
					spb_unsel_micos(spbdev.selico);	//清除主图标选中状态
					spbdev.selico=0xff;				//取消spbdev.selico原先的设置  
				}else	//属于点按.
				{
					for(i=0;i<11;i++)
					{
						if(i<8)
						{
							if((spbdev.curxpos>spbdev.icos[spbdev.frame][i].x)&&(spbdev.curxpos<spbdev.icos[spbdev.frame][i].x+spbdev.icos[spbdev.frame][i].width)&&(spbdev.curxpos>spbdev.icos[spbdev.frame][i].x)&&
							   (spbdev.curypos<spbdev.icos[spbdev.frame][i].y+spbdev.icos[spbdev.frame][i].height))
							{
								break;//得到选中的编号	
							}
						}else
						{
							if((spbdev.curxpos>spbdev.micos[i-8].x)&&(spbdev.curxpos<spbdev.micos[i-8].x+spbdev.micos[i-8].width)&&
							   (spbdev.curypos>spbdev.micos[i-8].y)&&(spbdev.curypos<spbdev.micos[i-8].y+spbdev.micos[i-8].height))
							{
								break;//得到选中的编号	
							}
						}
					}
					if(i<11)
					{
						if(i>=8)i+=8;
						if(i!=spbdev.selico)//选中了不同的图标,切换图标
						{
							spb_set_sel(i);
							i=0xff;
						}else
						{
							if(spbdev.frame==1&&i<8)i+=8;//第二页内容,编号偏移到8~15
							spbdev.selico=0XFF;//发生了双击,重新复位selico.
						}
					}else i=0xff;//无效的点按.
				}
			} 							
			spbdev.spbsta=0;//清空标志
		}
	}
	return i;

}











