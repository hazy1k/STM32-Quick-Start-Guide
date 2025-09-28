#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/err.h"
#include "lwip/netif.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32F407开发板
//lwip-网络接口驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/8/15
//版本：V1.0 							  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//网卡的名字
#define IFNAME0 'e'
#define IFNAME1 'n'
 

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);
#endif
