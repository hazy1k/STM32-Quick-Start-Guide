#include "tcp_client_demo.h"
#include "lwip_comm.h"
#include "lwip/api.h"
 
static struct netconn *tcp_clientconn;			//TCP CLIENT网络连接结构体
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收缓冲区
//待发送的数据
const u8 *tcp_client_sendbuf="Explorer STM32F407 TCP Client send data\r\n";

void tcp_client_test(void)
{
	err_t err; 
	ip_addr_t server_ipaddr,loca_ipaddr;
	u16 client_port,loca_port;
	struct netbuf *recvbuf;
	u8 key;
 	u8 rval=0;//退出标志
	u8 t=0;
	
	client_port=TCP_CLIENT_PORT;
	//设置远端IP地址
	IP4_ADDR(&server_ipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
	tcp_clientconn=netconn_new(NETCONN_TCP); //创建一个TCP链接
	while(tcp_clientconn&&rval==0)//创建成功
	{
		err=netconn_connect(tcp_clientconn,&server_ipaddr,client_port);	//连接服务器 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)rval=1;//退出tcp client测试.	
		if(err==ERR_OK)//创建成功
		{ 
			tcp_clientconn->recv_timeout=10;
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); 		//获取本地IP主机IP地址和端口号
			printf("连接上服务器%d.%d.%d.%d,主机端口号为:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES)//发送数据
				{
					err=netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //发送tcp_server_sentbuf中的数据
					if(err!=ERR_OK)printf("发送失败\r\n");				
				}
				if(key==WKUP_PRES)//退出tcp server测试.
				{
					rval=1;
					break;
				}
				err=netconn_recv(tcp_clientconn,&recvbuf);//查看是否接收到数据
				if(err==ERR_OK)  //接收到数据
				{		 
					if((recvbuf->p->tot_len)>=TCP_CLIENT_RX_BUFSIZE) //udp数据大于tcp服务器接收缓冲区
					{
						((char*)recvbuf->p->payload)[TCP_CLIENT_RX_BUFSIZE-1]=0;//末尾加入结束符
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,TCP_CLIENT_RX_BUFSIZE);//拷贝数据
					}else
					{
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						tcp_client_recvbuf[recvbuf->p->tot_len]=0;//末尾添加结束符
					}
					printf("服务器%d.%d.%d.%d发来数据:\r\n%s\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					netconn_close(tcp_clientconn);//关闭tcp_clientconn连接
					printf("服务器%d.%d.%d.%d断开连接\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
				delay_ms(10);
			}
		}else//连接失败.
		{
			netconn_close(tcp_clientconn);			//关闭tcp_clientconn连接
			netconn_delete(tcp_clientconn); 		//删除tcp_clientconn连接
			tcp_clientconn=netconn_new(NETCONN_TCP);//创建一个新TCP链接
		}
		delay_ms(10);
		t++;
		if(t==100)
		{
			t=0;
			printf("TCP Client 尝试连接服务器...\r\n");
		}		
	}
	if(tcp_clientconn!=NULL)//当tcp_clientconn连接存在的情况下删除连接
	{
 		netconn_close(tcp_clientconn);  	//关闭tcp_clientconn连接
		netconn_delete(tcp_clientconn); 	//删除tcp_clientconn连接
	} 
}  




























