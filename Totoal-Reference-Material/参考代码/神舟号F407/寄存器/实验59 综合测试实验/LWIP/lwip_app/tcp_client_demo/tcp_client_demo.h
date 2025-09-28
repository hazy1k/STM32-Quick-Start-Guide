#ifndef __TCP_CLIENT_DEMO_H
#define __TCP_CLIENT_DEMO_H
#include "sys.h"
#include "includes.h"
 
 
#define TCP_CLIENT_RX_BUFSIZE	200		//定义tcp client最大接收数据长度
#define TCP_CLIENT_PORT			8088	//定义tcp client连接的端口

void tcp_client_test(void);
#endif

