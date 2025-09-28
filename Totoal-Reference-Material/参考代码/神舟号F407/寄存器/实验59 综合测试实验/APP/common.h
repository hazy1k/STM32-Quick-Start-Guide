#ifndef __COMMON_H
#define __COMMON_H 	
#include "sys.h"
#include "touch.h"	 
#include "includes.h"
#include "gui.h"
#include "wm8978.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP通用 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/2/16
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//V1.1 20140216
//新增对各种分辨率LCD的支持.
//V1.2 20140727
//修改app_show_float函数的一个bug
////////////////////////////////////////////////////////////////////////////////// 	   

//硬件平台软硬件版本定义	   	
#define HARDWARE_VERSION	   		15		//硬件版本,放大10倍,如1.0表示为10
#define SOFTWARE_VERSION	    	200		//软件版本,放大100倍,如1.00,表示为100
//硬件V2.0
//1，增加TVS电源保护。
//2，输入电源采用DCDC电源方案代替线性稳压方案。
//硬件V2.2
//1，丝印位置稍微变化.
//2，新增二维码.
//V2.25  20121027
//增加3.5寸大分辨率LCD支持
//V2.26  20131124
//1,增加对NT35310驱动器的支持
//2,采用最新的SYSTEM文件夹,支持MDK3~MDK4.
//3,全面兼容V3.5库头文件.
//4,USMART采用最新的V3.1版本,支持函数执行时间查看.
//V2.30 20140216
//1,增加对NT35510驱动器的支持
//2,增加对电容触摸屏的支持
//3,增加自适应不同分辨率LCD功能.
//4,采用最新的SYSTEM文件夹,支持MDK3~MDK5 




//系统数据保存基址			  
#define SYSTEM_PARA_SAVE_BASE 		100		//系统信息保存首地址.从100开始.
			    

////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern u8*const APP_OK_PIC;			//确认图标
extern u8*const APP_CANCEL_PIC;		//取消图标
extern u8*const APP_UNSELECT_PIC;	//未选中图标
extern u8*const APP_SELECT_PIC;		//选中图标
extern u8*const APP_VOL_PIC;		//音量图片路径

extern u8*const APP_ASCII_S6030;	//数码管大字体路径
extern u8*const APP_ASCII_5427;		//普通大字体路径
extern u8*const APP_ASCII_3618;		//普通大字体路径
extern u8*const APP_ASCII_2814;		//普通大字体路径

extern u8* asc2_s6030;				//数码管字体60*30大字体点阵集
extern u8* asc2_5427;				//普通字体54*27大字体点阵集
extern u8* asc2_3618;				//普通字体36*18大字体点阵集
extern u8* asc2_2814;				//普通字体28*14大字体点阵集

extern const u8 APP_ALIENTEK_ICO1824[];	//启动界面图标,存放在flash
extern const u8 APP_ALIENTEK_ICO2432[];	//启动界面图标,存放在flash
extern const u8 APP_ALIENTEK_ICO3648[];	//启动界面图标,存放在flash
////////////////////////////////////////////////////////////////////////////////////////////
//APP的总功能数目
#define APP_FUNS_NUM	19

//app主要功能界面标题
extern u8*const APP_MFUNS_CAPTION_TBL[APP_FUNS_NUM][GUI_LANGUAGE_NUM];
extern u8*const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM];

extern u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
//平滑线的起止颜色定义
#define WIN_SMOOTH_LINE_SEC	0XB1FFC4	//起止颜色
#define WIN_SMOOTH_LINE_MC	0X1600B1	//中间颜色

//弹出窗口选择条目的设置信息
#define APP_ITEM_BTN1_WIDTH		60	  		//有2个按键时的宽度
#define APP_ITEM_BTN2_WIDTH		100			//只有1个按键时的宽度
#define APP_ITEM_BTN_HEIGHT		30			//按键高度
#define APP_ITEM_ICO_SIZE		32			//ICO图标的尺寸

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//选择时的背景色
#define APP_WIN_BACK_COLOR	 	0XC618		//窗体背景色


//#define APP_FB_TOPBAR_HEIGHT	20			//文件浏览界面,顶部横条的高度
//#define APP_FB_BTMBAR_HEIGHT	20			//文件浏览界面/测试界面,底部横条的高度
//#define APP_TEST_TOPBAR_HEIGHT	20			//测试界面,顶部横条高度

//π值定义
#define	app_pi	3.1415926535897932384626433832795 
/////////////////////////////////////////////////////////////////////////
																			    
u32  app_get_rand(u32 max);
void app_srand(u32 seed);
void app_set_lcdsize(u8 mode);
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 app_get_numlen(long long num,u8 dir);
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor);
void app_filebrower(u8 *topname,u8 mode);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb);

u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height);
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor);
u8 * app_get_icopath(u8 mode,u8 *selpath,u8 *unselpath,u8 selx,u8 index);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *selx,u8 mode,u8*caption);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

u8 app_system_file_check(u8* diskx);//系统文件检测
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode);
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize);
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8* src);
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2);
void app_get_version(u8*buf,u32 ver,u8 len);//得到版本号

void app_usmart_getsn(void);		//USMART专用.
u8 app_system_parameter_init(void);	//系统信息初始化
void app_lcd_auto_bklight(void);	//LCD背光自动控制函数
void app_wm8978_volset(u8 vol);		//WM8978音量设置
void app_wm8978_eqset(_wm8978_obj *wmset,u8 eqx);//eq设置
void app_wm8978_setall(void);		//设置所有WM8978参数
void app_set_default(void);			//恢复默认设置
#endif




























































