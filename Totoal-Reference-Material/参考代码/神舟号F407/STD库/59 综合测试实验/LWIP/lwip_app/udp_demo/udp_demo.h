#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "sys.h"
#include "includes.h"
 
 
#define UDP_RX_BUFSIZE		200		//定义udp最大接收数据长度
#define UDP_PORT			8088	//定义udp连接的端口

void udp_test(void);
#endif

