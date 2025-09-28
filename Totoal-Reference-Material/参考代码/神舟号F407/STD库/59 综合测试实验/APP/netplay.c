#include "netplay.h" 
#include "audioplay.h"
#include "t9input.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-网络测试 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

//netplay提示信息
u8*const netplay_remindmsg_tbl[5][GUI_LANGUAGE_NUM]=
{
{"请插入网线!正在初始化网卡...","請插入網線!正在初始化網卡...","Pls insert cable!Ethernet Initing..",}, 
{"初始化失败!请检查网线!","初始化失敗!請檢查網線!","Init failed!Check the cable!",},  
{"正在DHCP获取IP...","正在DHCP獲取IP...","DHCP IP configing...",},  
{"DHCP获取IP成功!","DHCP獲取IP成功!","DHCP IP config OK!",},  
{"DHCP获取IP失败,使用默认IP!","DHCP獲取IP失敗,使用默認IP!","DHCP IP config fail!Use default IP",},  
};
//netplay IP信息
u8*const netplay_ipmsg[5][GUI_LANGUAGE_NUM]=
{
{"本机MAC地址:","本機MAC地址:","Local MAC Addr:",}, 
{" 远端IP地址:"," 遠端IP地址:","Remote IP Addr:",}, 
{" 本机IP地址:"," 本机IP地址:"," Local IP Addr:",}, 
{"   子网掩码:","   子網掩碼:","   Subnet MASK:",},
{"       网关:","       網關:","       Gateway:",},  
};                     
//网速提示                                                                         
u8*const netplay_netspdmsg[GUI_LANGUAGE_NUM]={"   网络速度:","   網絡速度:","Ethernet Speed:"};
//netplay 测试提示信息
u8*const netplay_testmsg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"可检查连接状态.","可檢查連接狀態.","to check the connection.",}, 
{"2,在浏览器输入:","2,在瀏覽器輸入:","2,Input:",}, 	
{"可登录web界面。","可登錄web界面。","in browser,you can log on to website.",}, 	
};
//netplay memo提示信息
u8*const netplay_memoremind_tb[2][GUI_LANGUAGE_NUM]=
{
{"接收区:","接收區:","Receive:",},
{"发送区:","發送區:","Send:",},
};
//netplay 测试按钮标题
u8*const netplay_tbtncaption_tb[GUI_LANGUAGE_NUM]={"开始测试","開始測試","Start Test",};
//netplay 协议标题
u8*const netplay_protcaption_tb[GUI_LANGUAGE_NUM]={"协议","協議","PROT",};
//netplay 协议名字
u8*const netplay_protname_tb[3]={"TCP Server","TCP Client","UDP",};
//netplay 端口标题
u8*const netplay_portcaption_tb[GUI_LANGUAGE_NUM]={"端口:","端口:","Port:",};
//netplay IP地址标题
u8*const netplay_ipcaption_tb[2][GUI_LANGUAGE_NUM]=
{
{"目标IP:","目標IP:","Target IP:",},
{"本机IP:","本機IP:"," Local IP:",},
};
//netplay 按钮标题
u8*const netplay_btncaption_tbl[5][GUI_LANGUAGE_NUM]=
{
{"协议选择","協議選擇","PROT SEL",},
{"连接","連接","Conn",},
{"断开","斷開","Dis Conn",},
{"清除接收","清除接收","Clear",},
{"发送","發送","Send",},
};
//网络模式选择
u8*const netplay_mode_tbl[3]={"TCP Server","TCP Client","UDP"};
//网络连接提示信息
u8*const netplay_connmsg_tbl[4][GUI_LANGUAGE_NUM]=
{
{"正在连接...","正在連接...","Connecting...",},
{"连接失败!","連接失敗!","Connect fail!",},
{"连接成功!","連接成功!","Connect OK!",},
{"LwIP错误!","LwIP錯誤!","LwIP Error!",}, 
};
//加载主界面
void net_load_ui(void)
{
	u8 *buf;
	u8 fsize=0;
	u16 length;
	u8 temp;
	buf=gui_memin_malloc(100);//申请100字节 
	BACK_COLOR=LGRAY;  
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[13][gui_phy.language],0X05);	//显示标题  
	if(lcddev.width==240)fsize=12;
	else fsize=16;
	length=strlen((char*)netplay_ipmsg[0][gui_phy.language]);//得到字符串长度
  	gui_fill_rectangle(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight,BACK_COLOR);//填充底色 
	gui_show_string(netplay_ipmsg[0][gui_phy.language],10,gui_phy.tbheight+5,lcddev.width,fsize,fsize,BLUE);	 	
	gui_show_string(netplay_ipmsg[2][gui_phy.language],10,(4+fsize)*1+gui_phy.tbheight+5,lcddev.width,fsize,fsize,BLUE);	
	gui_show_string(netplay_ipmsg[3][gui_phy.language],10,(4+fsize)*2+gui_phy.tbheight+5,lcddev.width,fsize,fsize,BLUE);	
	gui_show_string(netplay_ipmsg[4][gui_phy.language],10,(4+fsize)*3+gui_phy.tbheight+5,lcddev.width,fsize,fsize,BLUE);	
	gui_show_string(netplay_netspdmsg[gui_phy.language],10,(4+fsize)*4+gui_phy.tbheight+5,lcddev.width,fsize,fsize,BLUE);//网速提示	
	sprintf((char*)buf,"%02X-%02X-%02X-%02X-%02X-%02X",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//显示MAC地址
	gui_show_string(buf,10+length*fsize/2,gui_phy.tbheight+5,lcddev.width,fsize,fsize,RED);	
	sprintf((char*)buf,"%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//显示IP地址
	gui_show_string(buf,10+length*fsize/2,(4+fsize)*1+gui_phy.tbheight+5,lcddev.width,fsize,fsize,RED);	
	sprintf((char*)buf,"%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);//显示子网掩码
	gui_show_string(buf,10+length*fsize/2,(4+fsize)*2+gui_phy.tbheight+5,lcddev.width,fsize,fsize,RED);	
	sprintf((char*)buf,"%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);//显示网关
	gui_show_string(buf,10+length*fsize/2,(4+fsize)*3+gui_phy.tbheight+5,lcddev.width,fsize,fsize,RED);	 
	temp=LAN8720_Get_Speed();
	if(temp&0X02)temp=100;//100M网速
	else temp=10;//10M网速
	sprintf((char*)buf,"%dM",temp);//网速
	gui_show_string(buf,10+length*fsize/2,(4+fsize)*4+gui_phy.tbheight+5,lcddev.width,fsize,fsize,RED);	 
	
	sprintf((char*)buf,"1,Ping %d.%d.%d.%d %s",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3],netplay_testmsg_tbl[0][gui_phy.language]);//1,ping xxx.xxx.xxx.xxx 可检查连接状态.
	gui_show_string(buf,10,(4+fsize)*6+gui_phy.tbheight+5,lcddev.width-20,2*fsize,fsize,BLUE);	 
	sprintf((char*)buf,"%s%d.%d.%d.%d %s",netplay_testmsg_tbl[1][gui_phy.language],lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3],netplay_testmsg_tbl[2][gui_phy.language]);//1,ping xxx.xxx.xxx.xxx 可检查连接状态.
	gui_show_string(buf,10,(4+fsize)*8+gui_phy.tbheight+5,lcddev.width-20,2*fsize,fsize,BLUE);	 
	gui_memin_free(buf);
}
//显示提示信息
//y:y坐标,x坐标恒定从0开始
//height:区域高度
//fsize:字体大小
//tx:发送字节数
//rx:接收字节数
//prot:协议类型
//     0:TCP Server 
//     1:TCP Client
//     2:UDP
//flag:更新标记,详见下面的描述
//bit0,1,更新tx数据,0,不更新
//bit1,1,更新rx数据,0,不更新
//bit2,1,更新port数据,0,不更新
void net_msg_show(u16 y,u16 height,u8 fsize,u32 tx,u32 rx,u8 prot,u8 flag)
{
	u16 xdis;
	u8 *pbuf;
	pbuf=gui_memin_malloc(100);
	if(pbuf==NULL)return;//内存申请失败
	if(prot>2)prot=2;
	xdis=(lcddev.width-(35*fsize/2))/3;//间隙
	BACK_COLOR=NET_MSG_BACK_COLOR;
	if(flag&1<<0)//更新TX数据
	{
		gui_fill_rectangle(xdis/2,y+(height-fsize)/2,10*fsize/2,fsize,NET_MSG_BACK_COLOR);//清除之前的显示
		sprintf((char*)pbuf,"TX:%d",tx);
		gui_show_string(pbuf,xdis/2,y+(height-fsize)/2,lcddev.width,fsize,fsize,NET_MSG_FONT_COLOR);//TX字节数显示
	}
	if(flag&1<<1)//更新RX数据
	{ 
		gui_fill_rectangle(xdis/2+10*fsize/2+xdis,y+(height-fsize)/2,10*fsize/2,fsize,NET_MSG_BACK_COLOR);//清除之前的显示
		sprintf((char*)pbuf,"RX:%d",rx);
		gui_show_string(pbuf,xdis/2+10*fsize/2+xdis,y+(height-fsize)/2,lcddev.width,fsize,fsize,NET_MSG_FONT_COLOR);//RX字节数显示
	}
	if(flag&1<<2)//更新prot数据
	{
		gui_fill_rectangle(xdis/2+20*fsize/2+xdis*2,y+(height-fsize)/2,15*fsize/2,fsize,NET_MSG_BACK_COLOR);//清除之前的显示
		sprintf((char*)pbuf,"%s:%s",netplay_protcaption_tb[gui_phy.language],netplay_protname_tb[prot]);//协议
		gui_show_string(pbuf,xdis/2+20*fsize/2+xdis*2,y+(height-fsize)/2,lcddev.width,fsize,fsize,NET_MSG_FONT_COLOR);//显示协议
	}
	gui_memin_free(pbuf);//释放内存
	
}
//设置编辑框颜色
//ipx:ip编辑框
//portx:port编辑框
//prot:协议
//connsta:连接状态
void net_edit_colorset(_edit_obj *ipx,_edit_obj *portx,u8 prot,u8 connsta)
{
	if(connsta==1)//连接成功?没的说,都是不可编辑
	{
		ipx->textcolor=WHITE;
		portx->textcolor=WHITE;
	}else//非连接状态
	{
		switch(prot)
		{
			case 0://TCP Server协议
				portx->textcolor=GREEN;	//绿色,表示可以编辑
				ipx->textcolor=WHITE;	//白色,固定死了
				break;
			case 1://TCP Client协议
			case 2://UDP协议
				portx->textcolor=GREEN;	//绿色,表示可以编辑
				ipx->textcolor=GREEN;	//绿色,表示可以编辑 
				break;
		}		
	}
	edit_draw(ipx);		//画编辑框
	edit_draw(portx);	//画编辑框
} 
//将字符串形式的port转换为数字形式的port
//str:字符串形式的port号
//返回值:转换成数字形式的port号
u16 net_get_port(u8 *str)
{
	u16 port;
	port=atoi((char*)str);
	return port;
}
//将字符串形式的ip地址转换为数字形式的ip
//返回值:0,错误的IP,其他,正确的IP.
u32 net_get_ip(u8 *str)
{
	u8 *p1,*p2,*ipstr;
	struct ip_addr ipx;
	u8 ip[4];
	ipstr=gui_memin_malloc(30);
	if(ipstr==NULL)return 0;
	strcpy((char*)ipstr,(char*)str);//拷贝字符串
	p1=ipstr;p2=(u8*)strstr((const char*)p1,".");
	if(p2==NULL){gui_memin_free(ipstr);return 0;}//IP错误
	p2[0]=0;ip[0]=atoi((char*)p1);//得到第一个值
	p1=p2+1;p2=(u8*)strstr((const char*)p1,".");
	if(p2==NULL){gui_memin_free(ipstr);return 0;}//IP错误
	p2[0]=0;ip[1]=atoi((char*)p1);//得到第二个值 
	p1=p2+1;p2=(u8*)strstr((const char*)p1,".");
	if(p2==NULL){gui_memin_free(ipstr);return 0;}//IP错误
	p2[0]=0;ip[2]=atoi((char*)p1);//得到第三个值 
	p1=p2+1;ip[3]=atoi((char*)p1);//得到第四个值 
	IP4_ADDR(&ipx,ip[0],ip[1],ip[2],ip[3]);
	gui_memin_free(ipstr);
	return ipx.addr;//返回得到的IP地址
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//在 tcp.c里面 
extern struct tcp_pcb *tcp_active_pcbs;			//在 tcp.c里面 
extern struct tcp_pcb *tcp_tw_pcbs;				//在 tcp.c里面  
//强制删除TCP Server主动断开时的time wait
void net_tcpserver_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	while(tcp_active_pcbs!=NULL)delay_ms(10);//等待tcp_active_pcbs为空 
	pcb=tcp_tw_pcbs;
	while(pcb!=NULL)//如果有等待状态的pcbs
	{
		tcp_pcb_purge(pcb); 
		tcp_tw_pcbs=pcb->next;
		pcb2=pcb;
		pcb=pcb->next;
		memp_free(MEMP_TCP_PCB,pcb2);	
	}
}
//断开连接
//netconn1:网络连接结构体1
//netconn2:网络连接结构体2
void net_disconnect(struct netconn *netconn1,struct netconn *netconn2)
{
	if(netconn1!=NULL)//连接结构体有效?
	{
		if(netconn1->type==NETCONN_TCP)netconn_close(netconn1);//关闭TCP netconn1连接
		else if(netconn1->type==NETCONN_UDP)netconn_disconnect(netconn1);//关闭UDP netconn1连连接
		netconn_delete(netconn1);  //删除netconn1连接
	}
	if(netconn2!=NULL)//连接结构体有效?
	{
		if(netconn2->type==NETCONN_TCP)netconn_close(netconn2);//关闭TCP netconn2连接
		else if(netconn2->type==NETCONN_UDP)netconn_disconnect(netconn2);//关闭UDP netconn2连连接
		netconn_delete(netconn2);  //删除netconn2连接
	}
}
//网络测试主函数
u8 net_test(void)
{
	struct netconn *netconncom=0;//通用TCP/UDP网络连接结构体指针(TCP Server/TCP Client/UDP通用)
	struct netconn *netconnnew=0;//新TCP/UDP网络连接结构体指针(只有TCP Server会用到这个)
	_edit_obj* eip=0;	//IP编辑框
	_edit_obj* eport=0; //端口编辑框
  	_btn_obj* protbtn=0;//协议选择按钮
  	_btn_obj* sendbtn=0;//发送按钮
  	_btn_obj* connbtn=0;//连接按钮
  	_btn_obj* clrbtn=0;	//清除按钮
	_memo_obj * rmemo=0,* smemo=0;	//memo控件 
	_t9_obj * t9=0;					//输入法  

	u8 ip_height,ip_fsize;			//IP/PORT区域高度和字体大小
	u8 msg_height;					//信息区域高度和字体大小
	u16 memo_width,btn_width;		//memo控件宽度,按钮的宽度
	u16 rmemo_height,smemo_height;	//接收memo和发送memo的高度
	u16 rbtn_height;				//接收区按钮的高度
	u8 m_offx,sm_offy,rm_offy; 		//memo x方向的偏移;smemo和rmemo y方向偏移  
	u8 fsize,sbtnfsize;				//字体大小,和发送按钮字体大小
	u16 t9height; 					//输入法的高度
	u16 tempx,tempy;
	u8 *ipcaption=netplay_ipcaption_tb[1][gui_phy.language];//默认是TCP Server模式,显示本机IP
	
	u16 res; 
	u8 rval=0;
	u8 editflag=0;	//0,编辑的是smemo
					//1,编辑的是eip
					//2,编辑的是eport
	u8 *p,*ptemp; 
	u32 rxcnt=0;
	u32 txcnt=0;
	u8 protocol=0;	//默认TCP Server协议
					//0,TCP Server协议
					//1,TCP Client协议
					//2,UDP协议
	u8 oldconnstatus=0;//老的状态
	u8 connstatus=0;//0,未连接,1,已连接
	u8 tcpconn=0;	//TCP连接是否建立:0,未建立;1,建立了
	u32 oldaddr=0;	//最近一次数据来自的ip地址
	u16 oldport=0;	//最近一次数据来自的port
	/////////////////////////////////
	err_t err; 			//错误标志 
	ip_addr_t tipaddr;	//临时IP地址
	u16	tport=8088;		//临时端口号,(要连接的端口号)默认为8088;		 
	struct netbuf *recvbuf;//接收缓冲区
	struct netbuf *sendbuf;//发送缓冲区	
	u16 *bkcolor;
	

	LCD_Clear(NET_MEMO_BACK_COLOR);//清屏 
	if(lcddev.width==240)
	{
		ip_height=20,ip_fsize=12;
		msg_height=16;
		memo_width=172,btn_width=56;
		rmemo_height=72,smemo_height=36;
		rbtn_height=20;
		m_offx=4,sm_offy=4,rm_offy=5;
		fsize=12,sbtnfsize=16;
		t9height=134;  
	}else if(lcddev.width==320)
	{ 
		ip_height=24,ip_fsize=16;
		msg_height=20;
		memo_width=208,btn_width=80;
		rmemo_height=144,smemo_height=48;
		rbtn_height=40;
		m_offx=12,sm_offy=8,rm_offy=10;
		fsize=16,sbtnfsize=24;
		t9height=176;
	}else if(lcddev.width==480)
	{ 	
		ip_height=36,ip_fsize=24;
		msg_height=28;
		memo_width=304,btn_width=100;
		rmemo_height=336,smemo_height=64;
		rbtn_height=50;
		m_offx=16,sm_offy=10,rm_offy=9;
		fsize=16,sbtnfsize=24;
		t9height=266;
	} 
	gui_fill_rectangle(0,0,lcddev.width,ip_height,NET_IP_BACK_COLOR);			//填充IP地址区域背景
	gui_fill_rectangle(0,ip_height,lcddev.width,msg_height,NET_MSG_BACK_COLOR);	//信息区域背景
	gui_draw_hline(0,ip_height+msg_height-1,lcddev.width,NET_COM_RIM_COLOR);	//分割线
	tempy=ip_height+msg_height+rmemo_height+fsize+2*rm_offy; 
 	gui_draw_hline(0,tempy,lcddev.width,NET_COM_RIM_COLOR);	//分割线
	tempx=(lcddev.width-35*ip_fsize/2)/3;
	gui_show_string(ipcaption,tempx,(ip_height-ip_fsize)/2,lcddev.width,ip_fsize,ip_fsize,WHITE);//本地IP/目标IP
	tempx=lcddev.width-tempx-10*ip_fsize/2;
	gui_show_string(netplay_portcaption_tb[gui_phy.language],tempx,(ip_height-ip_fsize)/2,lcddev.width,ip_fsize,ip_fsize,WHITE);//端口:
	tempy=ip_height+msg_height+rm_offy+fsize; 
	gui_show_string(netplay_memoremind_tb[0][gui_phy.language],m_offx,tempy-fsize-rm_offy/3,lcddev.width,ip_fsize,fsize,NET_MSG_FONT_COLOR);//显示接收区
	rmemo=memo_creat(m_offx,tempy,memo_width,rmemo_height,0,0,fsize,NET_RMEMO_MAXLEN);//创建memo控件,最多NET_RMEMO_MAXLEN个字符	
 	tempx=lcddev.width-tempx-10*ip_fsize/2;	
	eip=edit_creat(strlen((char*)ipcaption)*ip_fsize/2+tempx,(ip_height-ip_fsize-6)/2,15*ip_fsize/2+6,ip_fsize+6,0,4,ip_fsize);//创建ip编辑框
	tempx=(lcddev.width-35*ip_fsize/2)/3;
 	tempx=lcddev.width-tempx-10*ip_fsize/2;	
 	eport=edit_creat(tempx+5*ip_fsize/2,(ip_height-ip_fsize-6)/2,5*ip_fsize/2+6,ip_fsize+6,0,4,ip_fsize);//创建eport编辑框
	tempy=ip_height+msg_height+rm_offy*2+rmemo_height+fsize*2+sm_offy; 
	gui_show_string(netplay_memoremind_tb[1][gui_phy.language],m_offx,tempy-fsize-sm_offy/3,lcddev.width,fsize,fsize,NET_MSG_FONT_COLOR);//显示发送区
	smemo=memo_creat(m_offx,tempy,memo_width,smemo_height,0,1,fsize,NET_SMEMO_MAXLEN);//最多NET_SMEMO_MAXLEN个字符	
	tempy=ip_height+msg_height+rm_offy*2+rmemo_height+fsize*2+sm_offy*2+smemo_height; 
	t9=t9_creat(0,tempy,lcddev.width,t9height,0); 	
	tempy=ip_height+msg_height+rm_offy+fsize; 
	tempx=(lcddev.width-(m_offx+memo_width+btn_width))/2+m_offx+memo_width;
 	protbtn=btn_creat(tempx,tempy,btn_width,rbtn_height,0,0);	
	memo_width=(rmemo_height-3*rbtn_height)/2;//借用一下memo_width.
	if(memo_width>rbtn_height/2)memo_width=rbtn_height/2;
 	connbtn=btn_creat(tempx,tempy+memo_width+rbtn_height,btn_width,rbtn_height,0,0);
 	clrbtn=btn_creat(tempx,tempy+memo_width*2+rbtn_height*2,btn_width,rbtn_height,0,0);	
	tempy=ip_height+msg_height+rm_offy*2+rmemo_height+fsize*2+sm_offy; 
	sendbtn=btn_creat(tempx,tempy,btn_width,smemo_height,0,2);	//创建边角按钮
 	p=gui_memin_malloc(1500);	//申请1500字节内存
 	ptemp=gui_memin_malloc(100);//申请100字节内存
	if(!rmemo||!eip||!eport||!smemo||!t9||!protbtn||!connbtn||!clrbtn||!sendbtn||!p||!ptemp)rval=1;//创建失败. 
	if(rval==0)//创建成功
	{ 
		protbtn->caption=netplay_btncaption_tbl[0][gui_phy.language];
		protbtn->font=fsize;
		connbtn->caption=netplay_btncaption_tbl[1][gui_phy.language];
		connbtn->font=fsize;
		clrbtn->caption=netplay_btncaption_tbl[3][gui_phy.language];
		clrbtn->font=fsize;
		sendbtn->bkctbl[0]=0X6BF6;	//边框颜色
		sendbtn->bkctbl[1]=0X545E;	//0X8C3F.第一行的颜色				
		sendbtn->bkctbl[2]=0X5C7E;	//0X545E,上半部分颜色
		sendbtn->bkctbl[3]=0X2ADC;	//下半部分颜色	 
		sendbtn->bcfucolor=WHITE;	//松开时为白色
		sendbtn->bcfdcolor=BLACK;	//按下时为黑色 
		sendbtn->caption=netplay_btncaption_tbl[4][gui_phy.language];
		sendbtn->font=sbtnfsize;
		eip->textbkcolor=NET_IP_BACK_COLOR;
		eip->textcolor=WHITE;
		eport->textbkcolor=NET_IP_BACK_COLOR;
		eport->textcolor=GREEN;//GREEN,表示可以编辑
		rmemo->textbkcolor=WHITE;
		rmemo->textcolor=BLACK;
		smemo->textbkcolor=WHITE;
		smemo->textcolor=BLACK; 
		sprintf((char*)ptemp,"%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
 		strcpy((char*)eip->text,(const char *)ptemp);	//拷贝IP地址
		sprintf((char*)ptemp,"%d",tport);
		strcpy((char*)eport->text,(const char *)ptemp);	//拷贝端口号
 		edit_draw(eip);			//画编辑框
 		edit_draw(eport);		//画编辑框
		memo_draw_memo(smemo,0);//画memo控件
		memo_draw_memo(rmemo,0);//画memo控件
		btn_draw(protbtn);
		btn_draw(connbtn);
		btn_draw(clrbtn);
		btn_draw(sendbtn); 
		t9_draw(t9);	
		net_msg_show(ip_height,msg_height,fsize,txcnt,rxcnt,protocol,0X07);//显示信息 
	} 
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(1000/OS_TICKS_PER_SEC);		//延时一个时钟节拍
		if(system_task_return)break;			//TPAD返回 
		if(connstatus==0)//仅在连接未建立的时候,可以切换输入窗口
		{
			if(smemo->top<in_obj.y&&in_obj.y<(smemo->top+smemo->height)&&(smemo->left<in_obj.x)&&in_obj.x<(smemo->left+smemo->width))//在smemo内部 
			{ 
				editflag=0;			//编辑的是smemo
				edit_show_cursor(eip,0);	//关闭edit的光标
				edit_show_cursor(eport,0);	//关闭eport的光标
				eip->type=0X04;		//eip光标不闪烁 
				eport->type=0X04;	//eport光标不闪烁 
				smemo->type=0X01;	//memo可编辑,闪烁光标  
			} 
			if(eip->top<in_obj.y&&in_obj.y<(eip->top+eip->height)&&(eip->left<in_obj.x)&&in_obj.x<(eip->left+eip->width))//在eip框内部 
			{
				if(protocol==0)continue;//tcp server协议的时候,不需要设置IP地址
				editflag=1;			//编辑的是eip
				memo_show_cursor(smemo,0);	//关闭smemo的光标
				edit_show_cursor(eport,0);	//关闭eport的光标
				eip->type=0X06;		//eip光标闪烁 
				eport->type=0X04;	//eport光标不闪烁 
				smemo->type=0X00;	//smemo不可编辑,光标不闪烁
			}
			if(eport->top<in_obj.y&&in_obj.y<(eport->top+eport->height)&&(eport->left<in_obj.x)&&in_obj.x<(eport->left+eport->width))//在eport框内部 
			{
				editflag=2;			//编辑的是eport
				memo_show_cursor(smemo,0);	//关闭smemo的光标
				edit_show_cursor(eip,0);	//关闭eip的光标
				eport->type=0X06;	//eport光标闪烁 
				eip->type=0X04;		//eip光标不闪烁 
				smemo->type=0X00;	//smemo不可编辑,光标不闪烁
			}
		}
		edit_check(eip,&in_obj);
		edit_check(eport,&in_obj);
		t9_check(t9,&in_obj);		   
		memo_check(smemo,&in_obj);
		memo_check(rmemo,&in_obj);//检测rmemo
		if(t9->outstr[0]!=NULL)//添加字符
		{
			if(editflag==1)//eip
			{
				if((t9->outstr[0]<='9'&&t9->outstr[0]>='0')||t9->outstr[0]=='.'||t9->outstr[0]==0X08)edit_add_text(eip,t9->outstr);
			}else if(editflag==2)//eport
			{
				if((t9->outstr[0]<='9'&&t9->outstr[0]>='0')||t9->outstr[0]==0X08)edit_add_text(eport,t9->outstr);
			}else //smemo
			{   
  				memo_add_text(smemo,t9->outstr);
			}
			t9->outstr[0]=NULL;//清空输出字符 
		}
		res=btn_check(protbtn,&in_obj);   
		if(res&&((protbtn->sta&(1<<7))==0)&&(protbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
		{  
			//先选择模式    
			tempx=protocol;
			app_items_sel((lcddev.width-180)/2,(lcddev.height-192)/2,180,72+40*3,(u8**)netplay_mode_tbl,3,(u8*)&tempx,0XD0,(u8*)netplay_btncaption_tbl[0][gui_phy.language]);//3个选择
			if(tempx!=protocol)
			{
				protocol=tempx;  
				if(protocol!=0)ipcaption=netplay_ipcaption_tb[0][gui_phy.language];//TCP Client/UDP模式,显示目标IP
				else 
				{ 
					sprintf((char*)ptemp,"%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
					strcpy((char*)eip->text,(const char *)ptemp);	//恢复默认IP地址 
					ipcaption=netplay_ipcaption_tb[1][gui_phy.language];//默认是TCP Server/UDP模式,显示本机IP  
				}
				tempx=(lcddev.width-35*ip_fsize/2)/3;
				gui_fill_rectangle(tempx,(ip_height-ip_fsize)/2,ip_fsize*strlen((char*)ipcaption)/2,ip_fsize,NET_IP_BACK_COLOR);//清除原来的显示
				gui_show_string(ipcaption,tempx,(ip_height-ip_fsize)/2,lcddev.width,ip_fsize,ip_fsize,WHITE);//本地IP/目标IP
				net_edit_colorset(eip,eport,protocol,connstatus);//重画edit框 
				net_msg_show(ip_height,msg_height,fsize,txcnt,rxcnt,protocol,1<<2);//更新prot信息 
			}
		} 
		res=btn_check(connbtn,&in_obj);   
		if(res&&((connbtn->sta&(1<<7))==0)&&(connbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
		{   
			connstatus=!connstatus;
			tcpconn=0;				//标记TCP连接未建立
			if(connstatus==1)//建立连接
			{
				bkcolor=gui_memex_malloc(200*80*2);//申请内存
				if(bkcolor==NULL)//读取背景色失败了,直接继续运行,不执行后续操作
				{
					connstatus=0;
					printf("netplay ex outof memory\r\n");
					continue;
				}
				app_read_bkcolor((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,bkcolor);//读取背景色
				window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)netplay_connmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//显示正在连接	
				switch(protocol)
				{
					case 0://TCP Server协议 
						tport=net_get_port(eport->text);		//得到port号
						netconnnew=netconn_new(NETCONN_TCP);  	//创建一个TCP链接
						netconnnew->recv_timeout=10;  			//禁止阻塞线程
						err=netconn_bind(netconnnew,IP_ADDR_ANY,tport);//绑定端口
						if(err==ERR_OK)err=netconn_listen(netconnnew);  //进入监听模式
						else
						{
							connstatus=0;//连接失败
							net_disconnect(netconnnew,NULL);//关闭连接 
						}
						break;
					case 1://TCP Client协议 
						tipaddr.addr=net_get_ip(eip->text);
						if(tipaddr.addr!=0)
						{
							netconncom=netconn_new(NETCONN_TCP); //创建一个TCP链接
							netconncom->recv_timeout=10;
							tport=net_get_port(eport->text); 
 							err=netconn_connect(netconncom,&tipaddr,tport);//连接服务器 
							if(err==ERR_OK)tcpconn=1;//连接成功 
							else
							{
								connstatus=0;//连接失败
								net_disconnect(netconncom,NULL);//关闭连接
							}
						} 
						break;
					case 2://UDP协议  
						tipaddr.addr=net_get_ip(eip->text);
						if(tipaddr.addr!=0)
						{
							netconncom=netconn_new(NETCONN_UDP);  	//创建一个UDP链接
							netconncom->recv_timeout=10;  			//接收超时函数
							tport=net_get_port(eport->text); 
							err=netconn_bind(netconncom,IP_ADDR_ANY,tport);	//绑定UDP_PORT端口
  							if(err==ERR_OK)err=netconn_connect(netconncom,&tipaddr,tport);//连接到远端主机端口
							if(err!=ERR_OK)//连接失败 
							{ 
								connstatus=0;//连接失败
								net_disconnect(netconncom,NULL);//关闭连接
							} 
						}
						break;
				} 
				if(err==ERR_OK)window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)netplay_connmsg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//显示连接成功
				else window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)netplay_connmsg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//显示连接失败
				delay_ms(800);//延时等待提示
				app_recover_bkcolor((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,bkcolor);//恢复背景色
				gui_memex_free(bkcolor);//释放内存
			}				
 		} 
		res=btn_check(clrbtn,&in_obj);   
		if(res&&((clrbtn->sta&(1<<7))==0)&&(clrbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
		{   
			rxcnt=0;//发送总数清零
			txcnt=0;//接收总数清零
			rmemo->text[0]=0;//清空rmemo,从头开始
			memo_draw_memo(rmemo,1);//重画rmemo 		 
			net_msg_show(ip_height,msg_height,fsize,txcnt,rxcnt,protocol,0X07);//更新所有信息 
		} 
		res=btn_check(sendbtn,&in_obj);   
		if(res&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
		{  
			tempx=strlen((char*)smemo->text);//必须有数据才发送
			if(connstatus==1&&tempx)//有数据,且连接OK
			{
				if(tcpconn==1&&protocol!=2)//TCP Client/TCP Server发送数据
				{
					err=netconn_write(netconncom ,smemo->text,tempx,NETCONN_COPY);//发送smemo->text中的数据 
					if(err==ERR_OK)//发送成功
					{
						txcnt+=strlen((char*)smemo->text);//总发送长度增加 	 
						net_msg_show(ip_height,msg_height,fsize,txcnt,rxcnt,protocol,1<<0);//更新TX信息 
					}
				}else
				{
					sendbuf=netbuf_new();
					netbuf_alloc(sendbuf,strlen((char *)smemo->text));
					strcpy(sendbuf->p->payload,(void*)smemo->text);//拷贝数据到sendbuf数组
					err=netconn_send(netconncom,sendbuf);//将netbuf中的数据发送出去
					if(err!=ERR_OK)printf("netconn_send fail\r\n"); 
					else 
					{
						txcnt+=strlen((char*)smemo->text);//总发送长度增加 	
						net_msg_show(ip_height,msg_height,fsize,txcnt,rxcnt,protocol,1<<0);//更新TX信息 
					}
					netbuf_delete(sendbuf);  //删除buf									
				}	
			}
		} 
		if(connstatus==1)//连接状态
		{
			if(tcpconn==0&&protocol==0)//TCP Server模式下,连接还未建立,检查TCP连接
			{
				err=netconn_accept(netconnnew,&netconncom);//接收连接请求
				if(err==ERR_OK)//成功监测到连接
				{ 
					netconncom->recv_timeout=10; 
   					tcpconn=1;
				}
			}else
			{			
				//处理接收包
				err=netconn_recv(netconncom,&recvbuf);//查看是否接收到数据
				if(err==ERR_OK)  //接收到数据
				{		 
					netconn_getaddr(netconncom,&tipaddr,&tport,0); //获取远端IP地址和端口号
					if(tipaddr.addr!=oldaddr||tport!=oldport)//新地址/端口号??
					{
						oldaddr=tipaddr.addr;
						oldport=tport;
						sprintf((char*)ptemp,"[From:%d.%d.%d.%d:%d]:\r\n",oldaddr&0XFF,(oldaddr>>8)&0XFF,(oldaddr>>16)&0XFF,(oldaddr>>24)&0XFF,oldport); 
						tempx=strlen((char*)rmemo->text)+strlen((char*)ptemp);//得到新的总长度
						if(tempx>NET_RMEMO_MAXLEN)rmemo->text[0]=0;//清空rmemo,从头开始
						strcat(((char*)rmemo->text),(char*)ptemp);//添加收到的数据	 
					}
					memcpy(p,recvbuf->p->payload,recvbuf->p->tot_len);
					p[recvbuf->p->tot_len]=0;	//末尾加入结束符  
					tempx=strlen((char*)rmemo->text)+strlen((char*)p);//得到新的总长度
					if(tempx>NET_RMEMO_MAXLEN)rmemo->text[0]=0;//清空rmemo,从头开始
					strcat(((char*)rmemo->text),(char*)p);//添加收到的数据		
					rxcnt+=strlen((char*)p);//总接收长度增加
					memo_draw_memo(rmemo,1);//重画rmemo 		 
					net_msg_show(ip_height,msg_height,fsize,txcnt,rxcnt,protocol,1<<1);//更新RX信息 
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					if(protocol==0)tcpconn=0;//进入连接断开状态
					else connstatus=0;
					net_disconnect(netconncom,NULL);//断开netconncom连接  
				} 
			}				
		}
		if(oldconnstatus!=connstatus)//连接状态改变了
		{		
			oldconnstatus=connstatus;
			if(connstatus==0)//连接断开了?强制断开连接?
			{
				net_disconnect(netconnnew,netconncom);//断开连接 
				netconncom=NULL;
				netconnnew=NULL; 
				if(protocol==0)net_tcpserver_remove_timewait();//TCP Server,删除等待状态
				protbtn->sta=0;//协议选择按钮进入激活状态
				connbtn->caption=netplay_btncaption_tbl[1][gui_phy.language];  			
			}else//连接成功
			{
				protbtn->sta=2;//协议选择按钮进入非激活状态
				connbtn->caption=netplay_btncaption_tbl[2][gui_phy.language]; 
				editflag=0;			//只允许编辑smemo
				edit_show_cursor(eip,0);	//关闭edit的光标
				edit_show_cursor(eport,0);	//关闭eport的光标
				eip->type=0X04;		//eip光标不闪烁 
				eport->type=0X04;	//eport光标不闪烁 
				smemo->type=0X01;	//memo可编辑,闪烁光标  
			}
			btn_draw(protbtn);//重画按钮
			btn_draw(connbtn);
			net_edit_colorset(eip,eport,protocol,connstatus);//重画edit框
		}
	} 
	if(connstatus)//连接状态退出?断开连接!
	{
		net_disconnect(netconnnew,netconncom);//断开连接  
		if(protocol==0)net_tcpserver_remove_timewait();//TCP Server,删除等待状态
	}
	gui_memin_free(ptemp); 
	gui_memin_free(p); 
 	edit_delete(eip);	
 	edit_delete(eport);	
	memo_delete(rmemo);
	memo_delete(smemo);
	t9_delete(t9);
	btn_delete(protbtn);
	btn_delete(connbtn);
	btn_delete(clrbtn);
	btn_delete(sendbtn);
	system_task_return=0;	
	return 0;
} 
//网络功能测试
u8 net_play(void)
{
 	u8 res;
	u16 yoff=0;
   	_btn_obj* tbtn=0;//测试按钮控件 
	if(lcddev.width==240)yoff=170;
	else yoff=210; 
	tbtn=btn_creat((lcddev.width-180)/2,yoff+(lcddev.height-yoff-60)/2,180,60,0,0);//创建标准按钮
	if(tbtn==NULL)return 1;//按键创建失败
	if(audiodev.status&(1<<7))//当前在放歌??
	{
		audio_stop_req(&audiodev);	//停止音频播放
		audio_task_delete();		//删除音乐播放任务.
	} 
	window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)netplay_remindmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
  	res=lwip_comm_init();	//lwip初始化 LwIP_Init一定要在OSInit之后和其他LWIP线程创建之前初始化!!!!!!!!
	if(res==0)				//网卡初始化成功
	{
		lwip_comm_dhcp_creat();	//创建DHCP任务 
		//提示正在DHCP获取IP
		window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)netplay_remindmsg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		while(lwipdev.dhcpstatus==0||lwipdev.dhcpstatus==1)//等待DHCP分配成功
		{
			delay_ms(10);//等待.
		}
		if(lwipdev.dhcpstatus==2)window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)netplay_remindmsg_tbl[3][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//DHCP成功
		else window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)netplay_remindmsg_tbl[4][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//DHCP失败	
		delay_ms(1000);
		tbtn->caption=netplay_tbtncaption_tb[gui_phy.language];
		tbtn->font=24;	//24号字体
		net_load_ui();	//加载主界面UI
		btn_draw(tbtn);	//画出按钮
		httpd_init(); 	//初始化http
		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(5);							//延时一个时钟节拍
			if(system_task_return)break;	  		//TPAD返回 
			res=btn_check(tbtn,&in_obj);   
			if(res&&((tbtn->sta&(1<<7))==0)&&(tbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
			{
				net_test();
				net_load_ui();	//加载主界面UI
				btn_draw(tbtn);	//重画按钮
			}	 
		}
	}else//提示网卡初始化失败!
	{
		window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)netplay_remindmsg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 		delay_ms(2000);
	} 
	system_task_return=0;
	lwip_comm_destroy(); 
	LAN8720_RST=0;//保持LAN8720复位状态,减少功耗.
	return 0;
} 
































































