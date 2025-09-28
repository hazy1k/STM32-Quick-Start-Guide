#ifndef __T9INPUT_H
#define __T9INPUT_H 			 
#include "guix.h"  
#include "button.h"
#include "pyinput.h"
#include "atk_ncr.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//T9拼音输入法 代码	   
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

//标准按钮边框颜色定义
#define EDIT_RIM_LTOC 	0XA535	    //左上外线颜色
#define EDIT_RIM_LTIC 	0X8431		//左上内线颜色
#define EDIT_RIM_RBOC 	0XFFFF		//右下外线颜色
#define EDIT_RIM_RBIC 	0XDEFC		//右下内线颜色
#define EDIT_BACK_COLOR	0XFFFF		//编辑框背景色

#define EDIT_DFT_TBKC 	0XFFFF		//默认的文本背景色
#define EDIT_DFT_TXTC 	0X0000		//默认的文本颜色


#define T9_SIGNTBL_SIZE 5 

#define T9_OUTSEL_BKC	0XFFFF		//输出选择背景色
#define T9_OUTSEL_SELC	0X0000		//输出选中背景色
#define T9_BTN_EBC		0XD6BA		//按钮失效颜色
#define T9_BTN_DBC		0XFFFF		//按钮有效颜色


#define T9_PANEL_BKC	0X4A6A		//输入法背景色

//EDIT结构体定义
__packed typedef struct 
{
	u16 top; 				  		//按钮顶端坐标
	u16 left;                       //按钮左端坐标
	u16 width; 				  		//宽度(固定240)			    
	u16 height;						//高度(固定134) 
 	u8 mode;						//输入框类型
									//[7:5]:0,保留
									//[4:3]:(左侧纵向按钮状态)0,输入法切换状态;1,汉字输入状态;2,标点符号状态;
									//[2:0]:0,拼音;1,123;2,abc;3,ABC,4,手写.
	u8 sta;							//输入法状态
									//[7]:0,横向上一页按钮无效.1,横向上一页按钮有效
									//[6]:0,横向下一页按钮无效.1,横向下一页按钮有效
									//[5:2]:保留
									//[0]:0,没有任何输入数据
	u8 signpage;					//当前字符页面 
	u8 pynum;						//拼音总匹配数 
	
	_btn_obj * mbtn[12];			//12个主按钮  
 	_btn_obj * hbtn[8];				//横向8个选择按钮  
	_btn_obj * vbtn[5];				//纵向5个选择按钮

	py_index *pymb[MAX_MATCH_PYMB];	//码表存放位置
	u8 pypage;						//当前拼音所在页
	  
	u8 *disstr;						//横向显示的字符串指针
	u8 dispagenum;			 		//字符串总页数
	u8 dispagecur;					//当前页
	u8 disbuf[6][3];				//横向按钮的caption缓存		

	u8 instr[7];					//输入字符串,最大为6个字节+结束符1个.用于拼音输入和手写识别
	u8 outstr[5];					//输出字符,最大为2个字节*2+结束符1个,本来只要3个字节,因为标点符号可能成对.所以需要5个字节

	atk_ncr_point pbuf[200];		//识别输入最大点数	 
	u8 ncr_status;					//识别状态
									//[7]:0,没有输入.1,有输入;
									//[6:0]:计时器,用于记录连续多久没有输入
	u8 ncr_tcnt;					//记录的触摸按键次数
	u32 ncr_timer_old;				//gui定时器的上一次时间值
	
	u16 ncr_top;
	u16 ncr_left;
	u16 ncr_width;
	u16 ncr_height;
}_t9_obj;

_t9_obj * t9_creat(u16 left,u16 top,u16 width,u16 height,u8 mode);
void t9_delete(_t9_obj * t9_del);

void t9_ncr_clr(_t9_obj * t9x);
u8 t9_check(_t9_obj * t9x,void * in_key);
void t9_draw_mbtn(_t9_obj * t9x);
void t9_draw_hbtn(_t9_obj * t9x);
void t9_draw_vbtn(_t9_obj * t9x);
void t9_draw(_t9_obj * t9x);
void t9_hbtn_updc(_t9_obj * t9x,u8* str);

void t9_set_inputtype(_t9_obj *t9x,u8 type);
u8 t9_keyup_scan(_t9_obj *t9x);
u8 t9_id2num(u8 id);
void t9_clrhbtn_caption(_t9_obj *t9x,u8 btnx);
void t9_load_sign(_t9_obj *t9x);
void t9_drawvbtn_rim(_t9_obj *t9x,u16 rimcolor);
void t9_sign_process(_t9_obj *t9x,u8 id);
void t9_back_process(_t9_obj *t9x);
void t9_space_process(_t9_obj *t9x);
void t9_enter_process(_t9_obj *t9x);
void t9_hbtn_process(_t9_obj *t9x,u8 id);
void t9_vbtn_process(_t9_obj *t9x,u8 id);
void t9_pinyin_process(_t9_obj *t9x,u8 id);	   
void t9_process(_t9_obj *t9x);		 
void t9_test_task(void);	 
void t9_test(u16 x,u16 y,u8 mode);
void t9_tsdel(void);
void t9_tsdraw(void);
#endif

















