#ifndef __APPPLAY_CAN_H
#define __APPPLAY_CAN_H 

#include "common.h"  




u8 CAN_Play(u8* caption);
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
u8 CAN1_Send_Msg(u8* msg,u8 len);
u8 CAN1_Receive_Msg(u8 *buf);


#endif



