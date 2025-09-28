#ifndef __USBH_HID_GAMEPAD_H
#define __USBH_HID_GAMEPAD_H

#include "usbh_hid_core.h"
 

//FC游戏手柄数据格式定义
//1,表示没有按下,0表示按下.
typedef union _FC_GamePad_TypeDef 
{
	u8 ctrlval;
	struct
	{
		u8 a:1;		//A键
		u8 b:1;		//B键
		u8 select:1;//Select键
		u8 start:1; //Start键
		u8 up:1;	//上
		u8 down:1;	//下
		u8 left:1;	//左
		u8 right:1; //右
	}b;
}FC_GamePad_TypeDef ; 
extern FC_GamePad_TypeDef fcpad;	//fc游戏手柄1
extern FC_GamePad_TypeDef fcpad1;	//fc游戏手柄2
extern HID_cb_TypeDef HID_GAMEPAD_cb; 


 

void USR_GAMEPAD_Init(void);
void USR_GAMEPAD_ProcessData(uint8_t data);
 


#endif 






















