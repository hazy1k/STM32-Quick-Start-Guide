#ifndef __CAN2_H
#define __CAN2_H	 
#include "common.h"	 

//////////////////////////////////////////////////////////////////////////////////	 


	
//CAN2接收RX0中断使能
#define CAN2_RX0_INT_ENABLE	0		//0,不使能;1,使能.								    
										 							 				    
u8 CAN2_Mode_Init(u8 mode);//CAN初始化
 
u8 CAN2_Send_Msg(u8* msg,u8 len);						//发送数据

u8 CAN2_Receive_Msg(u8 *buf);							//接收数据
#endif

















