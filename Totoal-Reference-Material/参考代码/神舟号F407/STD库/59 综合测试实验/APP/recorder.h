#ifndef __RECORDER_H
#define __RECORDER_H 	
#include "common.h"
#include "wavplay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-录音机 代码	   
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

#define I2S_RX_DMA_BUF_SIZE    	4096		//定义RX DMA 数组大小

////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern u8*const RECORDER_DEMO_PIC;			//demo图片路径 	      
extern u8*const RECORDER_RECR_PIC;			//录音 松开
extern u8*const RECORDER_RECP_PIC;			//录音 按下
extern u8*const RECORDER_PAUSER_PIC;		//暂停 松开
extern u8*const RECORDER_PAUSEP_PIC;		//暂停 按下
extern u8*const RECORDER_STOPR_PIC;			//停止 松开
extern u8*const RECORDER_STOPP_PIC;			//停止 按下
////////////////////////////////////////////////////////////////////////////////////////////
 
	 		
void rec_i2s_dma_rx_callback(void) ; 
void recorder_enter_rec_mode(void);
void recorder_stop_rec_mode(void);
void recoder_set_samplerate(__WaveHeader* wavhead,u16 samplerate); 
void recorder_wav_init(__WaveHeader* wavhead);//初始化WAV头	
u8 recorder_vu_get(u16 signallevel);
void recorder_vu_meter(u16 x,u16 y,u8 level);
void recorder_show_time(u16 x,u16 y,u32 tsec);
void recorder_show_name(u16 x,u16 y,u8 *name);
void recorder_show_samplerate(u16 x,u16 y,u16 samplerate);
void recorder_load_ui(void);
void recorder_new_pathname(u8 *pname);
void recorder_show_agc(u16 x,u16 y,u8 agc);
u8 recorder_agc_set(u16 x,u16 y,u8 *agc,u8*caption);
u8 recorder_play(void);		   			   
#endif























