#include "usbh_hid_gamepad.h"
#include "usart.h"


//FC游戏手柄模拟
//读取fcpad.ctrlval,即可得到当前手柄格式转换成FC手柄格式.
FC_GamePad_TypeDef fcpad;
FC_GamePad_TypeDef fcpad1;


static void  GAMEPAD_Init (void);
static void  GAMEPAD_Decode(u8	*data);
  

HID_cb_TypeDef HID_GAMEPAD_cb = 
{
	GAMEPAD_Init,
	GAMEPAD_Decode,
};  

//game pad 初始化
static void  GAMEPAD_Init ( void)
{ 
	USR_GAMEPAD_Init();
} 

//通用USB方向键解码,结果存放在fcpad里面
//data0,data1:USB得到的方向数据 
//数据格式如下:
//data0:00,左键按下;ff,右键按下,7F,没有按键按下
//data1:00,上键按下;ff,下键按下,7F,没有按键按下
void GAMEPAD_nDir_Decode(u8 data0,u8 data1)
{
	switch(data0)
	{
		case 0X7F:
			fcpad.b.left=0;
			fcpad.b.right=0;
			break;
		case 0X00:
			fcpad.b.left=1; 
			break;
		case 0XFF:
			fcpad.b.right=1; 
			break;
	}
	switch(data1)
	{
		case 0X7F:
			fcpad.b.up=0;
			fcpad.b.down=0;
			break;
		case 0X00:
			fcpad.b.up=1; 
			break;
		case 0XFF:
			fcpad.b.down=1; 
			break;
	}
}
//通用USB AB键解码,结果存放在fcpad里面
//data:USB得到的1/2/3/4键数据 
//数据格式如下:
//data:最高4位有效  
//b4=1,1键按下(FC:B键连按)
//b5=1,2键按下(FC:A键连按)
//b6=1,3键按下(FC:A键)
//b7=1,4键按下(FC:B键)
void GAMEPAD_nAB_Decode(u8 data)
{
	if(data&0X10)fcpad.b.b=!fcpad.b.b;	//B键取反
	else
	{
		if(data&0X80)fcpad.b.b=1;		//B键按下
		else fcpad.b.b=0;				//B键松开  
	}
	if(data&0X20)fcpad.b.a=!fcpad.b.a;	//A键取反 
	else
	{
		if(data&0X40)fcpad.b.a=1;		//A键按下
		else fcpad.b.a=0;				//A键松开
	}	
}
//通用USB 功能键解码,结果存放在fcpad里面
//data:USB得到的Select/Start键数据 
//数据格式如下:
//data:b4,b5有效.
//b4=1,Select键按下
//b5=1,Start键按下 
void GAMEPAD_nFun_Decode(u8 data)
{
	if(data&0X10)fcpad.b.select=1;	//Select键按下
	else fcpad.b.select=0;			//Select键松开
	if(data&0X20)fcpad.b.start=1;	//Start键按下
	else fcpad.b.start=0;			//Start键松开 
} 

//game pad数据解析
static void  GAMEPAD_Decode(uint8_t *data)
{   
	u8 i; 
	if(data[7]==0X00)		//迪龙PU201手柄,最后一个字节为0X00
	{
		GAMEPAD_nDir_Decode(data[3],data[4]);	//解码方向键
		GAMEPAD_nAB_Decode(data[5]);			//解码AB键
		GAMEPAD_nFun_Decode(data[6]);			//解码功能键
		USR_GAMEPAD_ProcessData(fcpad.ctrlval);	//显示结果
	}else if(data[7]==0XC0)	//迪龙PU401手,最后一个字节为0XC0
	{
		GAMEPAD_nDir_Decode(data[0],data[1]);	//解码方向键
		GAMEPAD_nAB_Decode(data[5]);			//解码AB键
		GAMEPAD_nFun_Decode(data[6]);			//解码功能键
		USR_GAMEPAD_ProcessData(fcpad.ctrlval);	//显示结果		
	}else
	{
		for(i=0;i<8;i++)printf("%02x ",data[i]);
		printf("\r\n");
	}
} 




















