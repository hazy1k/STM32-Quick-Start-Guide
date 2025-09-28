#include "tcp_server_demo.h" 
#include "lwip_comm.h"
#include "lwip/api.h"

static struct netconn *tcp_serverconn;			//TCP Server网络连接结构体(监听用)
static struct netconn *tcp_servernewconn;		//TCP Server网络连接结构体(存储监听成功后的新连接)

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP服务器接收缓冲区
const u8 *tcp_server_sendbuf="Explorer STM32F407 TCP Server send data\r\n";

void tcp_server_test(void)
{
	err_t err;
	u8 remot_addr[4]; 
	ip_addr_t ipaddr;
	struct netbuf *recvbuf; 
	u16	port;
	u8 key;
	u8 rval=0;//退出标志
	u8 t=0;
	
	tcp_serverconn=netconn_new(NETCONN_TCP);  //创建一个TCP链接
	netconn_bind(tcp_serverconn,IP_ADDR_ANY,TCP_SERVER_PORT);  //绑定TCP_SERVER_PORT端口
	netconn_listen(tcp_serverconn);  		//进入监听模式
	tcp_serverconn->recv_timeout=10;  		//禁止阻塞线程 
	while(rval==0) 
	{
		err=netconn_accept(tcp_serverconn,&tcp_servernewconn);  //接收连接请求
		tcp_servernewconn->recv_timeout=10; 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)rval=1;//退出tcp server测试.	
		if(err==ERR_OK)//需要处理新连接的数据
		{ 
			netconn_getaddr(tcp_servernewconn,&ipaddr,&port,0); //获取远端IP地址和端口号
			remot_addr[3]=ipaddr.addr>>24; 
			remot_addr[2]=ipaddr.addr>>16;
			remot_addr[1]=ipaddr.addr>>8;
			remot_addr[0]=ipaddr.addr;
			printf("主机%d.%d.%d.%d 连接上服务器,主机端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			while(1)//处理连接
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES)//发送数据
				{ 
					err=netconn_write(tcp_servernewconn ,tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf),NETCONN_COPY); //发送tcp_server_sendbuf中的数据
					if(err!=ERR_OK)printf("发送失败\r\n");	 
				} 
				if(key==WKUP_PRES)//退出tcp server测试.
				{
					rval=1;
					break;
				}
				err=netconn_recv(tcp_servernewconn,&recvbuf);//查看是否接收到数据
				if(err==ERR_OK)  //接收到数据
				{		 
					if((recvbuf->p->tot_len)>=TCP_SERVER_RX_BUFSIZE) //收到的数据大于tcp服务器接收缓冲区
					{
						((char*)recvbuf->p->payload)[TCP_SERVER_RX_BUFSIZE-1]=0;//末尾加入结束符
						memcpy(tcp_server_recvbuf,recvbuf->p->payload,TCP_SERVER_RX_BUFSIZE);//拷贝数据
					}else
					{
						memcpy(tcp_server_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						tcp_server_recvbuf[recvbuf->p->tot_len]=0;//末尾加入结束符
					}
					printf("客户端%d.%d.%d.%d发来数据:%s\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],tcp_server_recvbuf);
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					printf("主机:%d.%d.%d.%d断开与服务器的连接\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					netconn_close(tcp_servernewconn);   //关闭tcp_servernewconn连接
					netconn_delete(tcp_servernewconn);  //删除tcp_servernewconn连接
					break;
				} 
				delay_ms(10);
			}
		} 
		delay_ms(10);
		t++;
		if(t==100)
		{
			t=0;
			printf("TCP Server等待连接中...\r\n");
		}
	}
	if(tcp_serverconn!=NULL)//监听连接结构体有效?
	{
		netconn_close(tcp_serverconn);   //关闭tcp_serverconn连接
		netconn_delete(tcp_serverconn);  //删除tcp_serverconn连接
	}
	if(tcp_servernewconn!=NULL)//新连接结构体有效?
	{
		netconn_close(tcp_servernewconn);   //关闭tcp_servernewconn连接
		netconn_delete(tcp_servernewconn);  //删除tcp_servernewconn连接
	}
}
 




















