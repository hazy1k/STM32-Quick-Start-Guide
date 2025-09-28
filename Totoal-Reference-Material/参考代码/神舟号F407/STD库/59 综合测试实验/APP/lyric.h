#ifndef __LYRIC_H
#define __LYRIC_H
#include "sys.h"
#include "includes.h" 	   	 
#include "common.h"	    
#include "ff.h"			
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//歌词显示实现 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 			    
#define LYRIC_BUFFER_SIZE		512		//歌词缓存,512字节
#define LYRIC_MAX_SIZE			200		//最大歌词条数
//LYRIC结构
__packed typedef struct 
{ 
	u32 time_tbl[LYRIC_MAX_SIZE];	//LRC时间表
	u16 addr_tbl[LYRIC_MAX_SIZE];	//LRC地址表
	u16 indexsize; 					//LRC大小,歌词总条数(包括重复的).
	u16 curindex;					//当前位置
	u32	curtime;					//当前时间,单位10ms
 	//oldostime,给定时器用
 	u32	oldostime;					//上一次ucos的时间
	//实现滚动效果的4个必要参数
 	u8  detatime;					//上一次滚动后,经过的10ms次数
	u8  updatetime;					//更新时间:即当前根据前后两句歌词的时间差,以及本句歌词的长度确定的滚动时间间隔
  	u16 namelen;
	u16 curnamepos;
	//
	//u16 bkcolor;					//歌词背景色
	u16 color;						//歌词颜色
	u8 	font;						//歌词字体

	FIL* flrc;						//LRC文件
	u8 buf[LYRIC_BUFFER_SIZE];		//LRC歌词文件缓存区	
	u16 *lrcbkcolor[7];				//7条歌词的背景色表
}_lyric_obj;


void lrc_chg_suffix(u8*name,u8*sfx);
_lyric_obj* lrc_creat(void);
void lrc_delete(_lyric_obj* lcrdelete);
u8 lrc_str2num(u8 *str);
void lrc_analyze(_lyric_obj* lrcx,u16 lrcpos,u8*str);
void lrc_sequence(_lyric_obj* lrcx);
u8 lrc_read(_lyric_obj* lrcx,u8*path,u8 *name);
void lrc_show_linelrc(_lyric_obj* lrcx,u16 x,u16 y,u16 width,u16 height);


u8 lrc_test(u8 * path,u8 *name); 
#endif












