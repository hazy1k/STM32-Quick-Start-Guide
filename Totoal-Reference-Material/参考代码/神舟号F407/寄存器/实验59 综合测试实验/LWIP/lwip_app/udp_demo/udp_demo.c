#include "udp_demo.h" 
#include "lwip_comm.h"
#include "lwip/api.h"


static struct netconn *udp_netconn;
u8 udp_recvbuf[UDP_RX_BUFSIZE];			//UDP接收缓冲区
//待发送的数据
const u8 *udp_sendbuf="Explorer STM32F407 UDP send data\r\n";

 
void udp_test(void)
{
	err_t err;
	struct ip_addr udp_ipaddr;
	struct netbuf  *recvbuf;
	struct netbuf  *sendbuf;	
 	u8 key;  

	udp_netconn=netconn_new(NETCONN_UDP);  	//创建一个UDP链接
	udp_netconn->recv_timeout=10;  			//接收超时函数
	if(udp_netconn!=NULL)  //创建UDP连接成功
	{
		netconn_bind(udp_netconn,IP_ADDR_ANY,UDP_PORT);	//绑定UDP_PORT端口
		IP4_ADDR(&udp_ipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2], lwipdev.remoteip[3]); //构造目的IP地址
		err=netconn_connect(udp_netconn,&udp_ipaddr,UDP_PORT); 	//连接到远端主机端口
		if(err==ERR_OK)//绑定完成
		{
			while(1)
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES)//发送数据
				{
					sendbuf=netbuf_new();
					netbuf_alloc(sendbuf,strlen((char *)udp_sendbuf));
					sendbuf->p->payload=(void*)udp_sendbuf;   		//指向udp_sentbuf数组
					err = netconn_send(udp_netconn,sendbuf);//将netbuf中的数据发送出去
					if(err!=ERR_OK)printf("发送失败\r\n"); 
					netbuf_delete(sendbuf);  //删除buf				
				}
				if(key==WKUP_PRES)break;//退出udp测试.
				err=netconn_recv(udp_netconn,&recvbuf);//查看是否接收到数据
				if(err==ERR_OK)
				{
					if(recvbuf->p->tot_len>=UDP_RX_BUFSIZE) //udp数据大于udp接收缓冲区
					{
						((char*)recvbuf->p->payload)[UDP_RX_BUFSIZE-1]=0;//末尾加入结束符
						memcpy(udp_recvbuf,recvbuf->p->payload,UDP_RX_BUFSIZE);//拷贝数据
					}else
					{
						memcpy(udp_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						udp_recvbuf[recvbuf->p->tot_len] = 0;
					}
					printf("接收到发送端发送来数据:%s\r\n",udp_recvbuf);//打印接收到的数据
					netbuf_delete(recvbuf);      //删除buf
				} 
				delay_ms(10);
			}
		}
	}
	if(udp_netconn)
	{
		netconn_disconnect(udp_netconn);//关闭UDP连接
		netconn_delete(udp_netconn);//删除udp连接	
	}
}

  
























