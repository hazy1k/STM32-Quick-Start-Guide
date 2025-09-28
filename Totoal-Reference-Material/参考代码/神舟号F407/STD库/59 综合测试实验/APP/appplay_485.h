#ifndef __APPPLAY_485_H
#define __APPPLAY_485_H 

#include "common.h"  




u8 RS485_Play(u8* caption);
void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);

#endif



