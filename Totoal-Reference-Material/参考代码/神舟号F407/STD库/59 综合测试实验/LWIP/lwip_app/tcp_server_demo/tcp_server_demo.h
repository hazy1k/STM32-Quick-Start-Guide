#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "includes.h"
 
 
#define TCP_SERVER_RX_BUFSIZE	200		//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			8088	//定义tcp server的端口



void tcp_server_test(void);
#endif

