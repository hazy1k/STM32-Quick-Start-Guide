#include "smsplay.h"
#include "usart3.h"	 
#include "spb.h"	  
#include "calendar.h"
#include "t9input.h"
#include "i2s.h"
#include "camera.h" 
#include "audioplay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-短信测试 代码	   
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

//提示信息
u8*const sms_remind_msg_tbl[9][GUI_LANGUAGE_NUM]=
{
"温馨提示:没有短消息","溫馨提示:沒有短消息","Remind:No SMS",
"选择操作:","沒有短消息","Choose option:", 
"短信读取中...","短信讀取中...","SMS Reading...", 
"删除成功!","刪除成功!","Delete OK!",
"删除失败!","刪除失敗!","Delete Failed", 
"发送:","發送:","Send:",  
"收到:","收到:","Receive:",   
"请先输入收件人号码!","請先輸入收件人號碼!","Please input Addressee NUM first!", 
"发送失败!","發送失敗!","Send failed!", 
};
u8*const sms_option_tbl[GUI_LANGUAGE_NUM][3]=
{
{"新建信息","阅读信息","删除信息"},
{"新建信息","閱讀信息","刪除信息"},		 
{"New SMS","Read SMS","Delete SMS"},
}; 
u8*const sms_send_tbl[GUI_LANGUAGE_NUM]={"发送","發送","SEND"};
u8*const sms_contact_tbl[2][GUI_LANGUAGE_NUM]=
{
"联系人:","聯繫人:","Contacts:",
"收件人:","收件人:","Addressee:",
};
 
////////////////////////////////////////////////////////////////////////////////////////
//短信链表处理部分

//创建SMS节点
//返回值:节点首地址
sms_node* sms_node_creat(void)
{
	sms_node *pnode;
	pnode=gui_memex_malloc(sizeof(sms_node));
	if(pnode)
	{
		pnode->pnum=0;
		pnode->msg=0;
		pnode->time=0;
		pnode->item=0;
		pnode->index=0;
		pnode->readed=0;
		pnode->next=0; 
	}
	return pnode; 
}
//释放一个节点的内存
//pnode:节点地址
//mode:0,仅仅释放节点内容(只删除内容,节点头地址保留.)
//     1,包括节点头也释放掉(整个节点删除)
void sms_node_free(sms_node * pnode,u8 mode)
{
	if(pnode==NULL)return;	//空节点
	if(pnode->pnum)gui_memex_free(pnode->pnum);	//释放pnum内存
	if(pnode->msg)gui_memex_free(pnode->msg);	//释放msg内存
	if(pnode->time)gui_memex_free(pnode->time);	//释放time内存 
	if(pnode->item)gui_memex_free(pnode->item);	//释放item内存 
	if(mode==1)gui_memex_free(pnode);			//释放pnode内存 
}
//在地i个节点后添加一个节点
//head:表头
//pnew:新节点
//i:添加在第几个节点之后,0XFFFF,在末尾添加
//返回值:0,添加成功
//    其他,添加失败.
u8 sms_node_insert(sms_node* head,sms_node *pnew,u16 i)
{
	u16 j;
	sms_node *p,*p1;
	p=p1=head;
	for(j=0;j<i&&p1!=NULL;j++)
	{
		p1=p1->next;
		if(p1!=NULL)
		{
			p=p1;
		}
	}
	if(p1==NULL&&(i!=0XFFFF))return 1;//无法插入第i个节点
	pnew->next=p->next;//插入节点的next指向后续节点
	p->next=pnew;//新节点插入第i个节点之后
	return 0;//插入成功	
}
//删除一个SMS节点
//head:表头
//i:要删除的节点号,0,表示表头(表头不能用该函数删除)
//返回值:0,删除成功
//    其他,删除失败/错误
u8 sms_node_delete(sms_node* head,u16 i)
{
	u16 j;
	sms_node *p,*p1=0;
	if(i==0)return 1;
	p=head;
	for(j=0;j<i&&p->next!=NULL;j++)
	{
		p1=p;
		p=p->next;
	}
	if(p==NULL||p1==NULL)return 2;//不存在的节点 
	p1->next=p->next;	//更改前一个节点的指向
	sms_node_free(p,1);	//释放节点内存 
	return 0;//删除成功	
}	
//得到一个节点信息
//head:表头
//i:要得到信息的节点号,0,表示表头
//返回值:0,得到信息失败
//    其他,节点地址
sms_node* sms_node_getnode(sms_node* head,u16 i)
{
	u16 j;
	sms_node *p; 
	p=head;
	for(j=0;j<i&&p!=NULL;j++)
	{
		p=p->next;
	}
	return p;//返回节点地址
}	
//得到整个链表大小
//head:表头 
//返回值:0,链表大小(节点个数)
u16 sms_node_getsize(sms_node* head)
{
	u16 j;
	sms_node *p; 
	p=head;
	for(j=0;p!=NULL;j++)
	{
		p=p->next;
	}
	return j;//返回节点地址
}	
//删除整个链表
//head:表头 
//返回值:0,链表大小(节点个数)
void sms_node_destroy(sms_node* head)
{ 
	sms_node *p,*p1; 
	p=head;
	while(p->next!=NULL)
	{
		p1=p->next;
		p->next=p1->next;
		sms_node_free(p1,1);//删除一个节点
	} 
	sms_node_free(p,1);//删除最后一个节点
}	
////////////////////////////////////////////////////////////////////////////////////////
//提示信息发送/接收状态
//mode:0,收到新消息
//     1,发送短信成功
//在sim900a.c里面调用
void sms_remind_msg(u8 mode)
{
	u8 i;
	if(mode==0)//收到新消息
	{
		for(i=0;i<2;i++)//2声提示
		{
			BEEP=1;
			delay_ms(30);
			BEEP=0;
			delay_ms(70);
		}
	}else //发送短信成功
	{
		BEEP=1;//1声提示
		delay_ms(60);
		BEEP=0; 
		delay_ms(140);
	}
	delay_ms(1300);
} 

//读取一条短信
//pnode:要存储该条短信的节点
//index:短信在SIM卡里的位置(1~xxxx)
//mode:1,不改变短信的状态;0,改变短信状态.
//返回值:0,读取成功
//       1,该位置为空(未存储短信)
//    其他,读取失败
u8 sms_read_sms(sms_node *pnode,u16 index,u8 mode)
{
	u8 *p,*p1,*p2; 
	u8 res,t=0;
	
	if(audiodev.status&(1<<4))
	{
		//I2S_Play_Stop();//停止音频播放
	} 
	p=gui_memin_malloc(200);//申请200个字节的内存
	if(p==0)return 2;//内存申请失败
	for(t=0;t<3;t++)//尝试3次
	{
		sprintf((char*)p,"AT+CMGR=%d,%d",index,mode);//发送读短信指令(根据mode参数,决定是不是改变短信状态)
 		res=sim900a_send_cmd(p,"+CMGR:",100);  
		if(res==0)//读取短信成功
		{
			pnode->index=index;	//记录该条短信在SIM卡的位置
			if(strstr((const char*)(USART3_RX_BUF),"OK")==0)//没有收到OK应答,则等待100ms,等OK传送过来
			{ 
				continue;//继续下一次
			}
			if(strstr((const char*)(USART3_RX_BUF),"UNREAD")==0)pnode->readed=1;
			else pnode->readed=0;
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
			if(!p1)continue;							//错误了，重新来过
			p2=(u8*)strstr((const char*)(p1+2),"\"");
			if(!p2)continue;							//错误了，重新来过
			p2[0]=0;//加入结束符
			sim900a_unigbk_exchange(p1+2,p,0);			//将unicode字符转换为gbk码 
			pnode->pnum=gui_memex_malloc(20);			//申请20字节内存,用于存储电话号码
			if(pnode->pnum)strcpy((char*)pnode->pnum,(char*)p);//拷贝号码
			p1=(u8*)strstr((const char*)(p2+1),"/");
			if(!p1)										//出错误了,终止
			{
				gui_memex_free(pnode->pnum);			//释放内存
				continue;
			}
			p2=(u8*)strstr((const char*)(p1),"+");
			if(!p2)										//出错误了,终止
			{
				gui_memex_free(pnode->pnum);			//释放内存
				continue;
			}
			p2[0]=0;//加入结束符
			pnode->time=gui_memex_malloc(20);			//申请20字节内存,用于存储短信接收时间
			if(pnode->time)sprintf((char*)pnode->time,"20%s",(char*)(p1-2));//拷贝接收时间,添加20字样  
			p1=(u8*)strstr((const char*)(p2+1),"\r");	//寻找回车符
			if(!p1)
			{
				gui_memex_free(pnode->pnum);			//释放内存
				gui_memex_free(pnode->time);			//释放内存
				continue;
			}
			sim900a_unigbk_exchange(p1+2,p,0);			//将unicode字符转换为gbk码 
			pnode->msg=gui_memex_malloc(strlen((char*)p));//申请内存,用于存储短信内容
			if(pnode->msg)strcpy((char*)pnode->msg,(char*)p);//拷贝短信内容 
			pnode->item=gui_memex_malloc(strlen((char*)pnode->pnum)+strlen((char*)pnode->msg)+strlen((char*)pnode->time));
			if(pnode->item)
			{ 
				strncpy((char*)p,(char*)(pnode->time+5),5);//拷贝月份/日期到数组p
				p[5]=0;//加入结束符
				sprintf((char*)pnode->item,"%s%s(%s):%s",pnode->readed?"○":"●",pnode->pnum,p,pnode->msg);
			}
			break;
		}else if(res==1)
		{
			//printf("check sta:%d\r\n",res);
			if(sim900a_check_cmd("OK"))break;//接收到OK,但是没内容,说明这个地方不存在短信
		}
	}
	sim900a_cmd_over();	//处理完毕
	gui_memin_free(p);	//释放内存
	if(audiodev.status&(1<<4))
	{
		//I2S_Play_Start();//开启音频播放	 
	} 	
	return res;
} 
//删除1条短息
//head:指向短信节点头指针的指针
//index:节点编号.
//返回值:0,删除成功
//    其他,删除失败
u8 sms_delete_sms(sms_node **head,u16 index)
{
	sms_node*ptemp;
	u8 *p;	
	u8 res,t=0; 
	p=gui_memin_malloc(200);//申请200个字节的内存
	ptemp=sms_node_getnode(*head,index);//读取节点号
	if(p==0||ptemp==NULL)return 1;//内存申请失败/得到节点信息失败 
	res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)APP_DELETE_CAPTION_TBL[gui_phy.language],12,0,0X03,0);
	if(res==1)//确定删除
	{
		for(t=0;t<3;t++)//尝试3次
		{	
			sprintf((char*)p,"AT+CMGD=%d",ptemp->index);//发送删除短信指令
			res=sim900a_send_cmd(p,"OK",100);//等待删除成功
			if(res==0)//删除短信成功
			{ 
				if(index==0)//删除节点头
				{
					if((*head)->next)//存在下一个节点
					{
						ptemp=*head;
						*head=(*head)->next;
						sms_node_free(ptemp,1);
					}else
					{
						sms_node_free(*head,0);	//先释放节点内容 
						(*head)->pnum=gui_memex_malloc(30);	//申请30字节
						(*head)->msg=gui_memex_malloc(30);	//申请30字节
						(*head)->time=gui_memex_malloc(30);	//申请30字节
						(*head)->item=gui_memex_malloc(30);	//申请30字节
						if((*head)->item)
						{
							strcpy((char*)(*head)->pnum,"ALIENTEK");//拷贝
							strcpy((char*)(*head)->msg,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//拷贝:无短消息到item里面
							calendar_get_time(&calendar);
							sprintf((char*)(*head)->pnum,"%d/%d/%d,%d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//拷贝时间			
							strcpy((char*)(*head)->item,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//拷贝:无短消息到item里面
						} 
					}
				}else
				{
					res=sms_node_delete(*head,index);//删除这个节点
					if(res)res=3;//删除节点失败
				}
				break;   
			}else res=2;//删除短信失败
		} 
		if(res==0)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sms_remind_msg_tbl[3][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		else window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sms_remind_msg_tbl[4][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(800);
	}
	sim900a_send_cmd("AT+CPMS?","+CPMS:",50);
	sim900a_cmd_over();//处理完毕
	gui_memin_free(p);//释放内存
	return res;
}
//读取短信条数
//返回值:0,读取成功
//    其他,失败
u8 sms_get_smsnum(u16 *num,u16 *max)
{
	u8 *p1,*p2;
	u8 res,i;
	if(audiodev.status&(1<<4))
	{ 	 
		I2S_Play_Stop();//停止音频播放
	} 
	for(i=0;i<3;i++)//尝试3次
	{
		res=sim900a_send_cmd("AT+CPMS?","+CPMS:",50);
		if(res==0)	//查询优选消息存储器
		{ 
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");   
			p2=(u8*)strstr((const char*)(p1+1),",");
			if((p2-p1)==2)*num=p1[1]-'0';//1位
			else if((p2-p1)==3)*num=(p1[1]-'0')*10+p1[2]-'0';//2位
			else if((p2-p1)==4)*num=(p1[1]-'0')*100+(p1[2]-'0')*10+p1[2]-'0';//3位 
			if(p2[3]==',')//小于64K SIM卡，最多存储几十条短信
			{
				*max=(p2[1]-'0')*10+p2[2]-'0';//获取最大存储短信条数
				p2[3]=0;
			}else //如果是64K SIM卡，则能存储100条以上的信息
			{
				*max=(p2[1]-'0')*100+(p2[2]-'0')*10+p2[3]-'0';//获取最大存储短信条数
				p2[4]=0;
			}
			break;
		}
	}
	if(audiodev.status&(1<<4))
	{
		I2S_Play_Start();//开启音频播放  
	} 
	sim900a_cmd_over();//处理完毕
	return res;
}

//新建选择框
//sel:当前选中的条目
//top:当前最顶部的条目
//caption:选择框名字
//head:短信链表头 
//返回值:[7]:0,按下的是返回按键,或者发生了错误;1,按下了选项按键,或者双击了选中的条目.
//       [6]:0,双击事件;1,选项按钮按下.
//     [5:0]:0,表示无错误;1,表示要更新信息;其他非零值,错误代码.
u8 sms_msg_select(u16 *sel,u16 *top,u8 * caption,sms_node* head)
{
	u8  res;
	u8 rval=0;			//返回值	  
  	u16 i;	
	u16 itemsize;		//总条目数
	sms_node *pnode;
	_btn_obj* rbtn=0;			//返回按钮控件
 	_btn_obj* okbtn=0;			//确认按钮控件
	_listbox_obj * tlistbox;	//listbox  
	itemsize=sms_node_getsize(head);			//至少得有一个节点
	if(itemsize==0)return 2;//一个节点都没有,错误!!
	if(*sel>=itemsize)*sel=itemsize-1;//限制范围
	if(*top>=itemsize)*top=0;//限制范围
	app_filebrower(caption,0X07);				//显示标题  
   	tlistbox=listbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.tbfsize);	//创建一个filelistbox
	if(tlistbox==NULL)rval=1;					//申请内存失败.
	else	//添加条目
	{
		for(i=0;i<itemsize;i++)
		{
			pnode=sms_node_getnode(head,i);
			res=listbox_addlist(tlistbox,pnode->item);
			if(res)
			{
				rval=1;
				break;
			}
		}
	}
	if(rval==0)//成功添加了条目
	{
		tlistbox->scbv->topitem=*top;
		tlistbox->selindex=*sel;
		listbox_draw_listbox(tlistbox);
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
 		okbtn=btn_creat(0,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建确认文字按钮
		if(rbtn==NULL||okbtn==NULL)rval=1;		//没有足够内存够分配
		else
		{
		 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];	//名字
		 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 
			rbtn->bcfdcolor=WHITE;	//按下时的颜色
			rbtn->bcfucolor=WHITE;	//松开时的颜色
			btn_draw(rbtn);//画按钮
		 	okbtn->caption=(u8*)GUI_OPTION_CAPTION_TBL[gui_phy.language];//名字
		 	okbtn->font=gui_phy.tbfsize;//设置新的字体大小	 
			okbtn->bcfdcolor=WHITE;	//按下时的颜色
			okbtn->bcfucolor=WHITE;	//松开时的颜色
			btn_draw(okbtn);//画按钮
		}	   
 	}  
   	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(1000/OS_TICKS_PER_SEC);		//延时一个时钟节拍
 		if(system_task_return)break;			//TPAD返回
		res=btn_check(rbtn,&in_obj);		    //返回按钮检测
		if(res)if(((rbtn->sta&0X80)==0))break;	//退出 
		res=btn_check(okbtn,&in_obj);//确认按钮检测
		if(res)
		{
			if(((okbtn->sta&0X80)==0))//按钮状态改变了
			{ 
				*top=tlistbox->scbv->topitem;//记录退出时选择的条目
				*sel=tlistbox->selindex;
				rval=3<<6;//标记选项按钮按下							 				   			   
  			}	 
		}   
		listbox_check(tlistbox,&in_obj);	//扫描 
		if(tlistbox->dbclick==0X80)//双击了
		{	  
			*top=tlistbox->scbv->topitem;//记录退出时选择的条目
			*sel=tlistbox->selindex;
			rval=2<<6;//标记双击按下了								 				   			   
		}
		if(sim900dev.newmsg)	//有新消息,要补充进来
		{
			rval=0X01;
			break;	
		}
	}	
	listbox_delete(tlistbox);		//删除listbox
	btn_delete(okbtn);				//删除按钮	  	 
	btn_delete(rbtn);				//删除按钮 
	return rval; 
}   
//读取所有短消息
//head:短信节点头
//maxnum:最大短信条数
//返回值:读取的短消息数量,0,一条都没有读到
u16 sms_read_all_sms(sms_node *head,u16 *maxnum)
{	
	_window_obj* twin=0;			//窗体
	_progressbar_obj * curprog=0;	//当前进度
	sms_node* pnew;
	u8 res;
	u16 i=0,j=0;
	u16 msgnum=0,msgmax=0;  
	u16 sx,sy;
	sx=(lcddev.width-200)/2;
	sy=(lcddev.height-100)/2; 
	twin=window_creat(sx,sy,200,32+20+20+28,0,1,16);//创建窗口
	curprog=progressbar_creat(sx+10,sy+32+20,180,20,0X61);		//创建进度条
	sim900dev.newmsg=0;//新消息条数清零  
	sms_get_smsnum(&msgnum,&msgmax);
	*maxnum=msgmax;//记录最大短信条数
	if(msgnum&&twin&&curprog)
	{
		twin->caption=sms_remind_msg_tbl[2][gui_phy.language];
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		window_draw(twin);						//画出窗体
		progressbar_draw_progressbar(curprog);	//显示进度条
		curprog->totallen=msgnum; 				//最大是msgnum
		pnew=head;
		for(i=1;i<msgmax+1;i++)
		{
			res=sms_read_sms(pnew,i,1);//读取一条短信
			if(res==0)//成功读取了一条
			{
				if(pnew!=head)//不是表头
				{
					res=sms_node_insert(head,pnew,0XFFFF);//在末尾追加新节点
					if(res)break;//追加失败
				} 
				j++;
				curprog->curpos=j;
				progressbar_draw_progressbar(curprog);//显示进度条
				if(j==msgnum)break;
				pnew=sms_node_creat();//创建新节点
				if(pnew==0)break;//创建失败 
			}else if(res==2)//读取失败
			{
				if(pnew!=head)sms_node_free(pnew,1);//删除最后创建的节点
				break;
			} 
		}
		msgnum=j;//记录总共读取了多少条短信
		delay_ms(200);
	}  
	window_delete(twin);//删除窗口
	progressbar_delete(curprog);//删除进度条	
	system_task_return=0;		//清除退出标志
	return msgnum;
} 

 
//发送一条短信
//pnode:当前短信节点
//mode:模式
//     0,阅读/回复当前短信
//     1,新建一条短信
void sms_send_sms(sms_node* pnode,u8 mode)
{
	_edit_obj* tedit; 
  	_btn_obj* sbtn=0;				//发送短信按钮
	_t9_obj * t9=0;					//输入法  
	_memo_obj * rmemo=0,* smemo=0;	//memo控件
	sms_node *pnew;
	
	u8 pnum_height,pnum_fsize;
	u8 rmsg_height,rmsg_fsize;
	u16 rmemo_height,smemo_width;
	u8 smemo_height,smemo_offx,sarea_height;
	u8 sbtn_width,sbtn_height,sbtn_fize;
	u8 smsg_height,smsg_fsize; 
	u16 t9height; 
	u16 res; 
	u8 rval=0;
	u8 ccnt=70;		//字符计数器,一条短信,最多70个字符
	u8 editflag=0;	//0,编辑的是smemo
					//1,编辑的是收件人(tedit)
	u8 *str=sms_contact_tbl[mode][gui_phy.language];
	u8 *p;
	u8 curmsg=0; 
	
	LCD_Clear(SMS_RMEMO_BACK_COLOR);//清屏 
	if(lcddev.width==240)
	{
		pnum_height=24;pnum_fsize=16;
		rmsg_height=16;rmsg_fsize=12;
		rmemo_height=80;
		smemo_height=48;smemo_width=176;smemo_offx=4;sarea_height=66;
		sbtn_width=40;sbtn_height=30;sbtn_fize=16;
		smsg_height=16;smsg_fsize=12;
		t9height=134;
	}else if(lcddev.width==320)
	{
		pnum_height=44;pnum_fsize=24;
		rmsg_height=26;rmsg_fsize=16;
		rmemo_height=136;
		smemo_height=80;smemo_width=208;smemo_offx=12;sarea_height=100;
		sbtn_width=60;sbtn_height=40;sbtn_fize=24;
		smsg_height=20;smsg_fsize=16;	
		t9height=176;
	}else if(lcddev.width==480)
	{
		pnum_height=54;pnum_fsize=24;
		rmsg_height=36;rmsg_fsize=16;
		rmemo_height=304;
		smemo_height=112;smemo_width=304;smemo_offx=26;sarea_height=140;
		sbtn_width=90;sbtn_height=60;sbtn_fize=24;
		smsg_height=20;smsg_fsize=16;	
		t9height=266;
	} 
	gui_fill_rectangle(0,0,lcddev.width,pnum_height,SMS_PNUM_BACK_COLOR);//填充联系人区域背景
	gui_fill_rectangle(0,pnum_height,lcddev.width,rmsg_height,SMS_RMSG_BACK_COLOR);//接收时间区域背景色
	gui_draw_hline(0,pnum_height+rmsg_height-1,lcddev.width,SMS_COM_RIM_COLOR);//分割线
	gui_fill_rectangle(0,pnum_height+rmsg_height+rmemo_height,lcddev.width,sarea_height,SMS_SAREA_BACK_COLOR);//发送区域背景色
	gui_draw_hline(0,pnum_height+rmsg_height+rmemo_height,lcddev.width,SMS_COM_RIM_COLOR);//分割线
	gui_show_string(str,10,(pnum_height-pnum_fsize)/2,lcddev.width,pnum_fsize,pnum_fsize,SMS_PNUM_NAME_COLOR);//显示联系人/收件人
	rmemo=memo_creat(0,pnum_height+rmsg_height,lcddev.width,rmemo_height,0,0,16,SMS_RMEMO_MAXLEN);//创建memo控件,最多SMS_RMEMO_MAXLEN个字符	
 	tedit=edit_creat(strlen((char*)str)*pnum_fsize/2+10,(pnum_height-pnum_fsize-6)/2,lcddev.width-strlen((char*)str)*pnum_fsize/2-10,pnum_fsize+6,0,4,pnum_fsize);//创建编辑框
	smemo=memo_creat(smemo_offx,pnum_height+rmsg_height+rmemo_height+(sarea_height-smemo_height)/2,smemo_width,smemo_height,0,1,16,140);//最多140个字符	
	t9=t9_creat(0,lcddev.height-t9height,lcddev.width,t9height,0); 	
	sbtn=btn_creat(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2,sbtn_width,sbtn_height,0,2);	//创建边角按钮
	str=gui_memin_malloc(300);	//申请300字节内存
	p=gui_memin_malloc(300);	//申请300字节内存
	if(!tedit||!rmemo||!smemo||!t9||!sbtn||!str||!p)rval=1;//创建失败. 
	if(rval==0)//创建成功
	{ 
		sbtn->bkctbl[0]=0X6BF6;	//边框颜色
		sbtn->bkctbl[1]=0X545E;	//0X8C3F.第一行的颜色				
		sbtn->bkctbl[2]=0X5C7E;	//0X545E,上半部分颜色
		sbtn->bkctbl[3]=0X2ADC;	//下半部分颜色	 
		sbtn->bcfucolor=WHITE;	//松开时为白色
		sbtn->bcfdcolor=BLACK;	//按下时为黑色 
		sbtn->caption=sms_send_tbl[gui_phy.language];
		sbtn->font=sbtn_fize;
		tedit->textbkcolor=SMS_PNUM_BACK_COLOR;
		tedit->textcolor=SMS_PNUM_NUM_COLOR;	
		rmemo->textbkcolor=SMS_RMEMO_BACK_COLOR;
		rmemo->textcolor=SMS_RMEMO_FONT_COLOR;
		smemo->textbkcolor=SMS_SMEMO_BACK_COLOR;
		smemo->textcolor=SMS_SMEMO_FONT_COLOR;
		if(mode==0)
		{
			gui_show_string(pnode->time,10,pnum_height+(rmsg_height-rmsg_fsize)/2,lcddev.width-10,rmsg_fsize,rmsg_fsize,SMS_RMSG_FONT_COLOR);
			strcpy((char*)tedit->text,(const char *)pnode->pnum);//拷贝电话号码
			strcpy((char*)rmemo->text,(const char *)pnode->msg);//拷贝信息
			memo_draw_memo(rmemo,0);
		}else
		{
			editflag=1;			//默认编辑联系人
			tedit->type=0X07;	//光标闪烁 
			smemo->type=0X00;	//memo不可编辑,光标不闪烁
		} 
		sprintf((char*)str,"(%02d)",ccnt);
 		gui_show_strmid(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,BLACK,smsg_fsize,str);//显示字符
		edit_draw(tedit);//画编辑框
		memo_draw_memo(smemo,0);//画memo控件
		t9_draw(t9);	
		btn_draw(sbtn);
	} 
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(1000/OS_TICKS_PER_SEC);		//延时一个时钟节拍
		if(system_task_return)break;			//TPAD返回
		if(mode==1)//新建短信的时候,要切换tedit/rmemo的选择
		{
			if(tedit->top<in_obj.y&&in_obj.y<(tedit->top+tedit->height)&&(tedit->left<in_obj.x)&&in_obj.x<(tedit->left+tedit->width))//在edit框内部 
			{
				editflag=1;			//默认编辑联系人
				memo_show_cursor(smemo,0);//关闭smemo的光标
				tedit->type=0X06;	//光标闪烁 
				smemo->type=0X00;	//memo不可编辑,光标不闪烁
			}
			if(smemo->top<in_obj.y&&in_obj.y<(smemo->top+smemo->height)&&(smemo->left<in_obj.x)&&in_obj.x<(smemo->left+smemo->width))//在smemo内部 
			{ 
				editflag=0;			//默认编辑联系人
				edit_show_cursor(tedit,0);//关闭edit的光标
				tedit->type=0X04;	//光标不闪烁 
				smemo->type=0X01;	//memo可编辑,闪烁光标  
			}
		}		
		edit_check(tedit,&in_obj);
		t9_check(t9,&in_obj);		   
		memo_check(smemo,&in_obj);
		memo_check(rmemo,&in_obj);//检测rmemo
		if(t9->outstr[0]!=NULL)//添加字符
		{
			if(editflag)
			{
				if((t9->outstr[0]<='9'&&t9->outstr[0]>='0')||t9->outstr[0]==0X08)edit_add_text(tedit,t9->outstr);//数字/退格键
			}else if(t9->outstr[0]==0X08||ccnt)//退格键,总有效
			{ 
				if(t9->outstr[0]==0X08){if(ccnt<70)ccnt++;}
				else ccnt--;
				if(t9->outstr[0]==0X0D)	//添加回车/换行的时候,去掉回车,仅仅保留换行.
				{
					t9->outstr[0]=0X0A;	//换行符
					t9->outstr[1]=0;	//结束符
				}
				sprintf((char*)str,"(%02d)",ccnt);
				gui_fill_rectangle(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,SMS_SAREA_BACK_COLOR);
				gui_show_strmid(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,BLACK,smsg_fsize,str);//显示字符
				memo_add_text(smemo,t9->outstr);
			}
			t9->outstr[0]=NULL;	 			//清空输出字符 
		}
		res=btn_check(sbtn,&in_obj);   
		if(ccnt!=70&&res&&((sbtn->sta&(1<<7))==0)&&(sbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
		{ 
			if(strlen((char*)tedit->text)>0)
			{
				//开始发送短信
				sim900a_unigbk_exchange(tedit->text,str,1); //将电话号码转换为unicode字符串
				sprintf((char*)p,"AT+CMGS=\"%s\"",str); 
				if(sim900a_send_cmd(p,">",200)==0)			//发送短信命令+电话号码
				{ 		 	
					sim900a_unigbk_exchange(smemo->text,str,1);	//将短信内容转换为unicode字符串.
					u3_printf("%s",str);	 
					sim900a_send_cmd((u8*)0X1A,"+CMGS:",0);		//发送结束符,开启发送,不等待
					calendar_get_time(&calendar);
					sprintf((char*)(rmemo->text+strlen((char*)rmemo->text)),"\n\n[%d/%d/%d %d:%d]%s\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,(char*)sms_remind_msg_tbl[5][gui_phy.language]);//添加2个换行符
					strcat(((char*)rmemo->text),(char*)smemo->text);//添加回复内容			
					smemo->text[0]=0;		//smemo,text内容清零
					memo_draw_memo(smemo,0);//重画smemo
					memo_draw_memo(rmemo,1);//重画rmemo 
					ccnt=70;				//重置字符计数器
					sprintf((char*)str,"(%02d)",ccnt);
					gui_fill_rectangle(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,SMS_SAREA_BACK_COLOR);
					gui_show_strmid(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,BLACK,smsg_fsize,str);//显示字符
				}else//不响应AT+CMGS指令,发送失败
				{ 
					window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)sms_remind_msg_tbl[8][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);	
 				}
				sim900a_cmd_over();//退出指令模式
			}else//沒有联系人?提示要先输入联系人
			{  
				window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)sms_remind_msg_tbl[7][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);	
 			}
		}
		if(curmsg<sim900dev.newmsg)	//有未读的新消息
		{
			res=1;
			pnew=sms_node_creat();	//创建新节点
			if(pnew)//创建成功
			{
				res=sms_read_sms(pnew,sim900dev.newmsgindex[curmsg],1);//读取短信
				if(res==0)//读取成功
				{ 
					if(strcmp((char*)pnew->pnum,(const char *)tedit->text)==0)//对比两个号码,相等
					{ 
						calendar_get_time(&calendar);
						sprintf((char*)(rmemo->text+strlen((char*)rmemo->text)),"\n\n[%d/%d/%d %d:%d]%s\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,(char*)sms_remind_msg_tbl[6][gui_phy.language]);//添加2个换行符
						strcat(((char*)rmemo->text),(char*)pnew->msg);//添加回复内容		
						memo_draw_memo(rmemo,1);//重画rmemo 		
					} 
					curmsg++;
				}
			}
			if(res)curmsg=sim900dev.newmsg;
			sms_node_free(pnew,1);
		}
	}
	gui_memin_free(str);
	gui_memin_free(p);
	memo_delete(rmemo);
	memo_delete(smemo);
	t9_delete(t9);
	btn_delete(sbtn);
 	edit_delete(tedit); 
	system_task_return=0;
}
//短信功能
u8 sms_play(void)
{
	u8 res;
	u16 msgnum=0,msgmax=0;
	sms_node *head,*ptemp;
	u16 selitem=0;
	u16 topitem=0; 
	u8 fsel=0;		//功能选择
	u8 readnewsms=0;//0,不读取短信
					//1,读取短信
					//2,新建短信	
	
	head=sms_node_creat();//创建表头
	msgnum=sms_read_all_sms(head,&msgmax);//读取所有短信
	if(msgnum==0)//一条短信都没有
	{ 
		head->pnum=gui_memex_malloc(30);//申请30字节
		head->msg=gui_memex_malloc(30);	//申请30字节
		head->time=gui_memex_malloc(30);//申请30字节
		head->item=gui_memex_malloc(30);//申请30字节
		if(head->item)
		{
			strcpy((char*)head->pnum,"ALIENTEK");//拷贝
			strcpy((char*)head->msg,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//拷贝:无短消息到item里面
			calendar_get_time(&calendar);
			sprintf((char*)head->pnum,"%d/%d/%d,%d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//拷贝时间			
			strcpy((char*)head->item,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//拷贝:无短消息到item里面
		}
	} 
	if(head->item)//item有数据
	{
		LCD_Clear(BLACK);  
		while(1)
		{
			res=sms_msg_select(&selitem,&topitem,APP_MFUNS_CAPTION_TBL[18][gui_phy.language],head);
 			if(system_task_return)break;//TPAD返回
			if(res&0X80)
			{
				readnewsms=0;
				if(res&1<<6)
				{
					fsel=0;
					if(msgnum)res=app_items_sel((lcddev.width-180)/2,(lcddev.height-192)/2,180,72+40*3,(u8**)sms_option_tbl[gui_phy.language],3,(u8*)&fsel,0XD0,(u8*)sms_remind_msg_tbl[1][gui_phy.language]);//3个选择
					else res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)sms_option_tbl[gui_phy.language],2,(u8*)&fsel,0X90,(u8*)sms_remind_msg_tbl[1][gui_phy.language]);//2个选择
 					if(res==0)//设置成功
					{
						switch(fsel)
						{
							case 0://新建短信 
								readnewsms=2;
								break;
							case 1://读取短信 
								readnewsms=1;
								break;
							case 2:
								//删除短信
								res=sms_delete_sms(&head,selitem);//删除这条短信
								if(res==0)
								{
									msgnum--;//短信条数减1 
								} 
								break;
						}  
					}  	
				}else readnewsms=1;//双击,读取短信 
				if(readnewsms==2)sms_send_sms(ptemp,1);//新建短信 
				else if(readnewsms==1&&msgnum)//有短信的时候,阅读/回复当前短信
				{
					ptemp=sms_node_getnode(head,selitem);//读取节点信息
					if(ptemp)//读取成功
					{  
						sms_node_free(ptemp,0);	//释放节点内容
						sms_read_sms(ptemp,ptemp->index,0);	//重新读取短信内容,并改变短信状态 
						sms_node_free(ptemp,0);	//释放节点内容 
						res=sms_read_sms(ptemp,ptemp->index,0);	//再重新读取短信,并改变短信状态
						if(res==0)sms_send_sms(ptemp,0);//阅读/回复当前短信 					
					}
				} 				
			}else if(res==0)break;	//返回按钮按下了,退出
			while(sim900dev.newmsg)//收到新消息了
			{
				if(msgnum==0)
				{
					sms_node_free(head,0);//先释放之前的表头的内容 
					res=sms_read_sms(head,sim900dev.newmsgindex[sim900dev.newmsg-1],1);//读取短信
				}else
				{
					ptemp=sms_node_creat();//创建新节点
					res=sms_read_sms(ptemp,sim900dev.newmsgindex[sim900dev.newmsg-1],1);//读取短信
					if(res==0)//读取成功
					{
						res=sms_node_insert(head,ptemp,0XFFFF);//在末尾追加新节点 
					}
				}
				if(res)
				{ 
					sim900dev.newmsg=0;//剩下的不再读取了
					sms_node_free(ptemp,1);//添加失败,释放节点内存
					if(msgnum==0)//只有表头的情况,且读短信失败,继续添加默认信息
					{		
						head->pnum=gui_memex_malloc(30);//申请30字节
						head->msg=gui_memex_malloc(30);	//申请30字节
						head->time=gui_memex_malloc(30);//申请30字节
						head->item=gui_memex_malloc(30);//申请30字节
						if(head->item)
						{
							strcpy((char*)head->pnum,"ALIENTEK");//拷贝
							strcpy((char*)head->msg,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//拷贝:无短消息到item里面
							calendar_get_time(&calendar);
							sprintf((char*)head->pnum,"%d/%d/%d,%d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//拷贝时间			
							strcpy((char*)head->item,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//拷贝:无短消息到item里面
						}
					}
				}else 
				{
					msgnum++;//短信条目加1
					sim900dev.newmsg--;
				}
			}			
		} 
	}
	sms_node_destroy(head);//删除整个链表
	return 0;
	
}






















