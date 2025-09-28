#ifndef __SPB_H
#define	__SPB_H	   
#include "spblcd.h"   
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

#define SPB_MOVE_WIN 		2 		//SPB滑动检测窗口值
#define SPB_MOVE_MIN		8		//SPB认为是滑动的最小值,超过会认为是滑动,但是不一定会翻页	   
#define SPB_MOVE_ACT 		50 		//SPB滑动的启动值,超过这个值将会进行翻页

#define SPB_ALPHA_VAL 		18 		//SPB选中透明度设置
#define SPB_ALPHA_COLOR		WHITE	//SPB透明色
#define SPB_FONT_COLOR 		BLUE 	//SPB字体颜色
#define SPB_MICO_BKCOLOR	0XA5BB	//mico图标背景色

extern u8*const spb_bkpic_path_tbl[3][2];
extern u8*const spb_icos_path_tbl[3][2][8];
extern u8*const spb_micos_path_tbl[3][3];	
//SPB 图标控制结构体
__packed typedef struct _m_spb_icos
{										    
	u16 x;			//图标坐标及尺寸
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//图标路径指针
	u8 * name;		//图标名字指针
}m_spb_icos;


//SPB 控制器
typedef struct _m_spb_dev
{										    
	u16 oldxpos;     			//上一次TP的x坐标位置
 	u16 curxpos;     			//当前tp按下的x坐标
	u16 curypos;     			//当前tp按下的y坐标
	u16	spbsta;					//spb状态
								//[15]:第一次按下标志
								//[14]:滑动标志;					 
								//[13~0]:滑动点数
	
	u16 pos;                    //当前帧的位置(x坐标)
	u8 frame;				 	//当前帧(0/1)
	
	u8 stabarheight;			//状态栏高度
	u16 spbheight;				//SPB滑动区域高度
	u16 spbwidth;				//SPB滑动区域宽度,一般等于LCD分辨率宽度
	 
	u8 selico;					//当前选中的图标.
								//0~8,被选中的图标编号	 
								//其他,没有任何图标被选中	  
	m_spb_icos icos[2][8];		//2页,每页8个图标 
	m_spb_icos micos[3];		//3个主图标
}m_spb_dev;
extern m_spb_dev spbdev;


u8 spb_init(void);
void spb_delete(void);
u8 spb_load_icos(u8 frame);
u8 spb_load_micos(void);
void spb_gsm_signal_show(u16 x,u16 y,u8 signal);
void spb_stabar_msg_show(u8 clr);
u8 spb_load_mui(void);
void spb_frame_move(u8 dir,u8 skips,u16 pos);
void spb_unsel_micos(u8 selx);
void spb_set_sel(u8 sel);
u8 spb_move_chk(void);
#endif




