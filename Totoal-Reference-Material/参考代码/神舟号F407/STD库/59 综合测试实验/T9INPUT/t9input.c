#include "t9input.h"
#include "atk_ncr.h"
#include "touch.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//T9拼音输入法 代码	   
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

//12个主按钮的3中字符串表
u8*const t9_mbtnstr_tbl[3][12]=
{   		
{				   
	";","abc","def","←",
	"ghi","jkl","mno","└┘",
	"pqrs","tuv","wxyz","←┘", 
},
{				   
	"1","2","3","←",
	"4","5","6",".",
	"7","8","9","0", 
}, 
{				   
	";","ABC","DEF","←",
	"GHI","JKL","MNO","└┘",
	"PQRS","TUV","WXYZ","←┘", 
},
};	 
//标点符号集
u8*const t9_mbtnsign_tbl[T9_SIGNTBL_SIZE][9]=
{
{				   
	",",".","?", 
	"!",":","、", 
	";","…","()", 
},
{				   
	"+","-","*", 
	"/","=","±", 
	"≈","%","‰", 
}, 
{				   
	"√","≠",">", 
	"<","≥","≤", 
	"≯","≮","|", 
},
{				   
	"‖","^","∑", 
	"&","《》","『』", 
	"{}","@","#",   
},
{				   
	"￥","$","℃", 
	"~","β","α", 
	"∵","∴","※",   
},
};							  
//5个纵向按钮
u8*const t9_vbtnstr_tbl[5]={"拼音","123","abc","ABC","手写"};

//创建输入法控件 
//left,top:起始坐标.
//width,height:宽度(必须为5的倍数)和高度(height-8,必须是6的倍数).
//mode:模式
//[7:5]:0,保留
//[4:3]:(左侧纵向按钮状态)0,输入法切换状态;1,汉字输入状态;2,标点符号状态;
//[2:0]:0,拼音;1,123;2,abc;3,ABC,4,手写.
_t9_obj * t9_creat(u16 left,u16 top,u16 width,u16 height,u8 mode)
{
	u8 i,t;
	u16 temp;
	u8 sta=0; 
	u16 tbtnw=0;
	u16 tbtnh=0; 
	u8 xoff=0;
	u8 yoff=0; 
 	_t9_obj * t9_crt; 
	if((width%5)||((height-8)%6))return NULL;//长宽比例不对.
  	t9_crt=(_t9_obj*)gui_memin_malloc(sizeof(_t9_obj));//分配内存
	gui_memset(t9_crt,0,sizeof(_t9_obj));
	if(t9_crt==NULL)return NULL;//内存分配不够.
	//12个主按键
	tbtnw=(width-10)/5;			//按键宽度
	temp=height-(height-8)/6-2;	//5/12主按键的显示高度区域		
	tbtnh=(temp-7)/3;			//按键的高度 
	yoff=height-temp+(temp-3*tbtnh-7)/2+2;//在中间  
	t9_crt->ncr_top=top+(height-8)/6+4;			//手写识别区域的TOP坐标
	t9_crt->ncr_left=left+(width-10)/5;			//手写识别区域的LEFT坐标
	t9_crt->ncr_width=width-2*(width-10)/5-4;	//手写识别区域的宽度
	t9_crt->ncr_height=temp-4;					//手写识别区域的高度 
 	t=1; 
	for(i=0;i<12;i++)
	{
		t9_crt->mbtn[i]=btn_creat(left+(tbtnw+2)*t,top+yoff,tbtnw,tbtnh,i,0X02);
		if(t==4)
		{
			t=1;
			yoff+=tbtnh+2;//向下偏移

		}else t++;				 
		if(t9_crt->mbtn[i]==NULL)
		{
			sta=1;//标记有内存申请失败.
			break;
		}
		if(tbtnh>50)t9_crt->mbtn[i]->font=24;		//为24字体 
		else t9_crt->mbtn[i]->font=16;				//为16字体
		t9_crt->mbtn[i]->caption=(u8*)t9_mbtnstr_tbl[0][i];
		t9_crt->mbtn[i]->bcfucolor=WHITE;//松开时为白色
		t9_crt->mbtn[i]->bcfdcolor=BLACK;//按下时为黑色
	}
 	//8个文字按键
	tbtnw=width/8-1;			//文字选择按钮的宽度			                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   //按键宽度
	tbtnh=(height-8)/6;			//文字选择按键的高度 
	xoff=(width-(tbtnw+1)*8)/2;	//在中间   
	for(i=0;i<8;i++)
	{
		t9_crt->hbtn[i]=btn_creat(left+(tbtnw+1)*i+xoff,top+1,tbtnw,tbtnh,i+12,0X04);//文字按钮,背景色单一
		t9_crt->hbtn[i]->bcfucolor=BLACK;//松开时为黑色
		t9_crt->hbtn[i]->bcfdcolor=WHITE;//按下时为白色
 		if(t9_crt->hbtn[i]==NULL)
		{
			sta=1;//标记有内存申请失败.
			break;
		}
		if(tbtnh>30)t9_crt->hbtn[i]->font=24;		//为24字体 
		else t9_crt->hbtn[i]->font=16;				//为16字体
	}
 	tbtnw=(width-10)/5-2;	//左侧按钮的宽度	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               //按键宽度
	tbtnh=(height-8)/6;	 	//左侧按键的高度  
 	yoff=3+(height-8)/6;	//给5个纵向按钮占用的高度   
	for(i=0;i<5;i++)
	{
		t9_crt->vbtn[i]=btn_creat(left+1,top+i*(tbtnh+1)+yoff,tbtnw,tbtnh,i+20,0X04);//文字按钮,背景色单一
  		if(t9_crt->vbtn[i]==NULL)
		{
			sta=1;//标记有内存申请失败.
			break;
		}
		t9_crt->vbtn[i]->caption=(u8*)t9_vbtnstr_tbl[i];
		if(tbtnh>30)t9_crt->vbtn[i]->font=24;		//为24字体
		else if(tbtnh>25)t9_crt->vbtn[i]->font=16;	//为16字体
		else t9_crt->vbtn[i]->font=12;				//为12字体
		t9_crt->vbtn[i]->bcfucolor=0XA535;//松开时为灰白色
		t9_crt->vbtn[i]->bcfdcolor=0X0000;//按下时为黑色
		t9_crt->vbtn[i]->bkctbl[0]=T9_PANEL_BKC;//松开时为输入法的背景
		t9_crt->vbtn[i]->bkctbl[1]=0XFFFF;//按下时背景为白色
	} 			  	 
  	if(sta)//未申请成功
	{
		t9_delete(t9_crt);
		return NULL;
	}  
	t9_crt->top=top;
	t9_crt->left=left;
	t9_crt->width=width;
	t9_crt->height=height;		 
	t9_crt->mode=mode;
	t9_crt->sta=0;					   
	t9_crt->disstr=NULL;					   
	t9_crt->dispagenum=0;					   
	t9_crt->dispagecur=0;					   
	t9_crt->instr[0]=NULL;					   
	t9_crt->outstr[0]=NULL;					   
	t9_set_inputtype(t9_crt,0);//设置为拼音输入法
	return t9_crt;
}
//删除t9输入法
//t9_del:要删除的输入法.
void t9_delete(_t9_obj * t9_del)
{
	u8 i;
	i=0;
	if(t9_del==NULL)return;//非法的地址,直接退出
	while(i<12)
	{
		btn_delete(t9_del->mbtn[i]);		//删除mbtn
		if(i<8)btn_delete(t9_del->hbtn[i]);	//删除hbtn
		if(i<5)btn_delete(t9_del->vbtn[i]);	//删除vbtn
 		i++;
    }
 	gui_memin_free(t9_del);	//释放申请到的内存
	alientek_ncr_stop();	//释放手写识别的内存
}
	 	 
//清除一次识别的计数
void t9_ncr_clr(_t9_obj * t9x)
{
 	t9x->ncr_tcnt=0;
 	t9x->ncr_status=0;//结束一次识别
  	t9x->ncr_timer_old=0;		 
	gui_fill_rectangle(t9x->ncr_left,t9x->ncr_top,t9x->ncr_width,t9x->ncr_height,0XFFFF);//填充白色 
}

//检查	
//t9x:输入法
//in_key:输入按键指针
//返回值:未用到			 					   
u8 t9_check(_t9_obj * t9x,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	u8 i;
	static u16 x=0XFFFF,y=0;
	u8 tcnt=0;	 
 	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//触摸屏按下了
 			if((t9x->mode&0X07)!=0X04)
			{
				for(i=0;i<12;i++)btn_check(t9x->mbtn[i],in_key);//非手写输入法.
			}else  //手写识别
			{	
				btn_check(t9x->mbtn[3],in_key);//检查这3个按键
				btn_check(t9x->mbtn[7],in_key); 
				btn_check(t9x->mbtn[11],in_key);
				if((t9x->ncr_top+1)<key->y&&key->y<(t9x->ncr_top+t9x->ncr_height-2)&&(t9x->ncr_left+1)<key->x&&key->x<(t9x->ncr_left+t9x->ncr_width-1))//在手写框内部
				{	    
					t9x->ncr_status=0X80;		//有按键按下	  
					if(x==0XFFFF)
					{
						x=key->x;
						y=key->y;
					}
					gui_draw_bline(x,y,key->x,key->y,2,RED);//画线
					x=key->x;
					y=key->y;   
					if(t9x->ncr_tcnt<200)//记录最多200个坐标轨迹.
					{
						tcnt=t9x->ncr_tcnt;
						if(tcnt)tcnt--;
						if(t9x->pbuf[tcnt].x!=(key->x-t9x->ncr_left)||t9x->pbuf[tcnt].y!=(key->y-t9x->top))//新坐标,必须和老坐标不相等,否则不予采集!!!
						{									 
							t9x->pbuf[t9x->ncr_tcnt].x=key->x-t9x->ncr_left;
							t9x->pbuf[t9x->ncr_tcnt].y=key->y-t9x->ncr_top;
	 						t9x->ncr_tcnt++;
						}		   								    
					}
  				}else if(t9x->ncr_status&0x80)//没有按键按下,且之前有按下
				{		
					x=0XFFFF;	 					
					if(t9x->ncr_timer_old!=GUI_TIMER_10MS)
					{
						t9x->ncr_timer_old=GUI_TIMER_10MS;
						t9x->ncr_status++;
						if((t9x->ncr_status&0x7F)>50&&t9x->ncr_tcnt>1)//等待连续的500ms
						{
							alientek_ncr(t9x->pbuf,t9x->ncr_tcnt ,6,4,(char*)t9x->instr);
 							t9x->disstr=t9x->instr;//disstr指向当前按下按钮的caption	
 							t9x->dispagenum=0;//字符串的总页数为0
							t9x->dispagecur=0;
							t9_hbtn_updc(t9x,t9x->disstr);//添加
 							t9_draw_hbtn(t9x);//画出来
  							t9_ncr_clr(t9x);
						}
					}  
				} 
			}
			for(i=0;i<8;i++)
			{	  
 				btn_check(t9x->hbtn[i],in_key);
				if(i<5)btn_check(t9x->vbtn[i],in_key);
			}	   
 			break;
		case IN_TYPE_KEY:	//按键数据
			break;
		case IN_TYPE_JOYPAD://手柄数据
			break;
		case IN_TYPE_MOUSE:	//鼠标数据
			break;
		default:
			break;
	}
	t9_process(t9x);//处理按键信息
	return 0;
}
//画12个主按钮
//t9x:输入法
void t9_draw_mbtn(_t9_obj * t9x)
{
	u8 i;
 	for(i=0;i<12;i++)btn_draw(t9x->mbtn[i]);	  
}
//画横向的8个选择按钮
//t9x:输入法.
void t9_draw_hbtn(_t9_obj * t9x)
{
	u8 i;
 	for(i=0;i<8;i++)btn_draw(t9x->hbtn[i]);	  
}
//画纵向的5个选择按钮
//t9x:输入法.
void t9_draw_vbtn(_t9_obj * t9x)
{
	u8 i;
 	for(i=0;i<5;i++)btn_draw(t9x->vbtn[i]);	 
}
//画t9输入界面
//t9x:输入法.
void t9_draw(_t9_obj * t9x)
{
	gui_fill_rectangle(t9x->left,t9x->top,t9x->width,t9x->height,T9_PANEL_BKC);//填充内部
	if((t9x->mode&0X07)!=0x4)t9_draw_mbtn(t9x);//非手写模式
	else//手写识别输入法.
	{
		btn_draw(t9x->mbtn[3]);//只需要3个按钮
		btn_draw(t9x->mbtn[7]);		   
		btn_draw(t9x->mbtn[11]); 
		t9_ncr_clr(t9x);//清除一次识别
	}
	t9_draw_hbtn(t9x);
	t9_draw_vbtn(t9x);
}

//更新横向8个按钮的caption
//t9x:输入法.
//str:按钮caption摘取的字符串.
void t9_hbtn_updc(_t9_obj * t9x,u8* str)
{
	u8 i=0;//		  
	if(t9x->dispagecur>=t9x->dispagenum&&t9x->dispagenum)return;//当前页等于或者大于总页数
 	for(i=0;i<8;i++)t9x->hbtn[i]->caption=NULL;//清空之前的caption	   
	i=0;
	while(*str!=NULL&&str)
	{
		if(*str>0x80)//是中文,有2个字节组成
		{
			t9x->disbuf[i][0]=*str++;
			t9x->disbuf[i][1]=*str++;
			t9x->disbuf[i][2]='\0';	    
			t9x->hbtn[i+1]->caption=t9x->disbuf[i]; 	
		}else //英文
		{
			t9x->disbuf[i][0]=*str++;
			t9x->disbuf[i][1]='\0';		 
			t9x->hbtn[i+1]->caption=t9x->disbuf[i]; 	
		}
		i++;
		if(i>5)break;
	}							 
	t9x->hbtn[0]->caption="＜";
	t9x->hbtn[7]->caption="＞";	
	if(t9x->dispagecur==0)t9x->hbtn[0]->bcfucolor=0XA535;//前面没有页了
	else t9x->hbtn[0]->bcfucolor=0X0000;//前面还有页.

	if(t9x->dispagecur==(t9x->dispagenum-1))t9x->hbtn[7]->bcfucolor=0XA535;//到最后一页了
	else if(t9x->dispagenum>1)t9x->hbtn[7]->bcfucolor=0X0000;//后面还有页.
	else t9x->hbtn[7]->bcfucolor=0XA535;//后面没有页了			   
}	   
//切换输入法
//t9x:t9
//type:0~4;0,拼音;1,123;2,abc;3,ABC,4,手写.
void t9_set_inputtype(_t9_obj *t9x,u8 type)
{
	u8 i;
	u8 mx=0;
	if(type>4)return;//非法的type.
	for(i=0;i<5;i++)
	{
		t9x->vbtn[i]->caption=(u8*)t9_vbtnstr_tbl[i];
 		if(i==type)t9x->vbtn[i]->bcfucolor=0XFFFF;	//当前选中的模式,松开时为白色
		else t9x->vbtn[i]->bcfucolor=0XA535;		//松开时为灰白色 
 	}		
	for(i=0;i<8;i++)t9x->hbtn[i]->caption=NULL;		//清空横向的8个按钮
	if(type!=4)//不是手写识别模式
	{
		alientek_ncr_stop();//停止手写识别
		if(type==0||type==2)mx=0;
		else if(type==1)mx=1;
		else if(type==3)mx=2;
		for(i=0;i<12;i++)
		{
			t9x->mbtn[i]->caption=(u8*)t9_mbtnstr_tbl[mx][i];//12个主按钮的名字
			t9x->mbtn[i]->bcfucolor=0XFFFF;//松开时颜色为白色
		}
	}else
	{
		alientek_ncr_init();//初始化手写识别	  
	}
	t9x->instr[0]='\0';
	t9x->disstr='\0';	//横向显示字符串归零
	t9x->dispagenum=0;
	t9x->dispagecur=0;
	t9x->mode&=0xE7;	//取消标记(字符/拼音),切换为输入法切换状态
	t9x->mode&=~(0x07);	//重设输入法状态
	t9x->mode|=type; 	 
}
//获得松开时按键的id
//t9x:输入法
//返回值:0~24,正确的id;
//		 0xff,没有正确的id
u8 t9_keyup_scan(_t9_obj *t9x)
{
	u8 i;
	for(i=0;i<12;i++)
	{   

		if((t9x->mbtn[i]->sta&(1<<6))&&(t9x->mbtn[i]->sta&0x03)==0)//按键是松开的,且有效
		{
			t9x->mbtn[i]->sta&=~(1<<6);//清除标志
			return t9x->mbtn[i]->id;
		}
		if(i<8)if((t9x->hbtn[i]->sta&(1<<6))&&(t9x->hbtn[i]->sta&0x03)==0)//按键是松开的,且有效
		{
			t9x->hbtn[i]->sta&=~(1<<6);//清除标志
			return t9x->hbtn[i]->id;
		}
		if(i<5)if((t9x->vbtn[i]->sta&(1<<6))&&(t9x->vbtn[i]->sta&0x03)==0)//按键是松开的,且有效
		{
			t9x->vbtn[i]->sta&=~(1<<6);//清除标志
			return t9x->vbtn[i]->id;
		}
	}
	return 0xff;
}
//将按键ID转换为数字返回
//id:按钮的id.
//返回值:对应的数字
u8 t9_id2num(u8 id)
{
	if(id<3)return id+1;
	if(id>3&&id<7)return id;
	if(id>7&id<11)return id-1;
	if(id==11)return 0; 
	return 0;//非法id直接返回0
}
	   
//清除按钮的caption
//t9x:输入法
//btnx:[2],0,不清除vbtn;1,清除vbtn;
//	   [1],0,不清除hbtn;1,清除hbtn;
//	   [0],0,不清除mbtn;1,清除mbtn;
void t9_clrhbtn_caption(_t9_obj *t9x,u8 btnx)
{
	u8 i;
	if(btnx&0x01)for(i=0;i<12;i++)t9x->mbtn[i]->caption=NULL;	//按键是松开的,且有效
	if(btnx&0x02)for(i=0;i<8;i++)t9x->hbtn[i]->caption=NULL;	//按键是松开的,且有效 
	if(btnx&0x04)for(i=0;i<5;i++)t9x->vbtn[i]->caption=NULL;	//按键是松开的,且有效  
}   
//加载标点符号输入界面
//t9x:输入法
void t9_load_sign(_t9_obj *t9x)
{
	u8 i=0;
	u8 offset=0;
	if(t9x->signpage>T9_SIGNTBL_SIZE-1)return;//超区域了
	t9x->mode&=0xE7;
	t9x->mode|=0x10;//标记模式为标点符号输入 
	for(i=0;i<9;i++)//更新caption
	{
		if(i==3)offset=1;
		else if(i==6)offset=2;
		t9x->mbtn[i+offset]->caption=(u8*)t9_mbtnsign_tbl[t9x->signpage][i];
	}
	t9x->mbtn[3]->caption="∧";
	t9x->mbtn[7]->caption="∨";
	t9x->mbtn[11]->caption="返回";
	if(t9x->signpage==0)t9x->mbtn[3]->bcfucolor=0XA535;//灰色
	else t9x->mbtn[3]->bcfucolor=0XFFFF;
	if(t9x->signpage==T9_SIGNTBL_SIZE-1)t9x->mbtn[7]->bcfucolor=0XA535;//灰色
	else t9x->mbtn[7]->bcfucolor=0XFFFF;
}

//垂直按钮的外框
//t9x:输入法
//rimcolor:边框颜色.
void t9_drawvbtn_rim(_t9_obj *t9x,u16 rimcolor)
{
	u8 i;
	for(i=0;i<6;i++)gui_draw_hline(t9x->left,t9x->vbtn[0]->top-1+(t9x->vbtn[0]->height+1)*i,t9x->vbtn[0]->width+2,rimcolor);
	gui_draw_vline(t9x->left,t9x->vbtn[0]->top-1,(t9x->vbtn[0]->height+1)*5,rimcolor);
	gui_draw_vline(t9x->vbtn[0]->left+t9x->vbtn[0]->width,t9x->vbtn[0]->top-1,(t9x->vbtn[0]->height+1)*5,rimcolor);
}

//字符输入处理
//t9x:输入法
//id:按键id
void t9_sign_process(_t9_obj *t9x,u8 id)
{
	if(id==3||id==7)//上一页/下一页
	{
		if(id==3&&t9x->signpage)t9x->signpage--;	  
		if(id==7&&(t9x->signpage<T9_SIGNTBL_SIZE-1))t9x->signpage++;
		t9_load_sign(t9x);//
		t9_draw_mbtn(t9x);//重画主按钮
		return;
	}else if(id!=11)//不是返回,而是真正的标点符号   
	{
		t9x->outstr[0]='\0';
		strcat((char*)t9x->outstr,(const char*)t9x->mbtn[id]->caption);	//复制标点符号到输出字符串里面
	}	 
	t9_set_inputtype(t9x,t9x->mode&0x07);//切换为输入法状态
	t9_draw(t9x);//重画 
}
//退格键的处理
//t9x:输入法   
void t9_back_process(_t9_obj *t9x)
{
	u8 cnt=0;
	if(t9x->disstr==0||t9x->disstr[0]==NULL)//输入字符为0,加入0X8,表示退格 
	{
		t9x->outstr[cnt++]=0x08;//输入0X8,代表退格  
	}else 
	{														    
		t9x->disstr=NULL;				//横向按钮字符串清零
	 	t9_clrhbtn_caption(t9x,0x02);	//清除横向按钮			 
		t9_draw_hbtn(t9x);				//重画横向按钮		   
 	}
	t9x->outstr[cnt]='\0';			//加入结束符
}
//空格键的处理
//t9x:输入法   
void t9_space_process(_t9_obj *t9x)
{
	u8 cnt=0;
	if(t9x->disstr!=0&&t9x->disstr[0]!=NULL)//输入字符不为0,取输入字符串的第一个字符/汉字 
	{
		t9x->outstr[cnt++]=t9x->disstr[0];
		if(t9x->disstr[0]>0X80)//是汉字
		{
			t9x->outstr[cnt++]=t9x->disstr[1];
 		}
	}else t9x->outstr[cnt++]=' ';	//加入空格
	t9x->outstr[cnt]='\0';			//加入结束符
	t9x->disstr=NULL;				//横向按钮字符串清零
 	t9_clrhbtn_caption(t9x,0x02);	//清除横向按钮			 
	t9_draw_hbtn(t9x);				//重画横向按钮		   
}
//回车键的处理
//t9x:输入法   
void t9_enter_process(_t9_obj *t9x)
{
	u8 cnt=0;
	if(t9x->disstr!=0&&t9x->disstr[0]!=NULL)//输入字符不为0,取输入字符串的第一个字符/汉字 
	{
		t9x->outstr[cnt++]=t9x->disstr[0];
		if(t9x->disstr[0]>0X80)//是汉字
		{
			t9x->outstr[cnt++]=t9x->disstr[1];
 		}
	}else //输入回车
	{
		t9x->outstr[cnt++]=0X0D;//回车
		t9x->outstr[cnt++]=0X0A;//回车		  
	}				    
	t9x->outstr[cnt]='\0'; 			//加入结束符  
 	t9x->disstr=NULL;				//横向按钮字符串清零
	t9_clrhbtn_caption(t9x,0x02);	//清除横向按钮			 
	t9_draw_hbtn(t9x);				//重画横向按钮		   
}	  

//横向按钮处理
//t9x:输入法
//id:按键id
void t9_hbtn_process(_t9_obj *t9x,u8 id)
{
 	if(id>19||id<12)return ;
	if(t9x->disstr!=NULL)//输入字符不为0    
	{
		if(id==12||id==19)//向前/向后
		{
			if(t9x->mode&0x03)return ;//只有拼音输入法需要前后偏移
			else
			{
				if(id==12)//向前
				{	 
					if(t9x->dispagecur)t9x->dispagecur--;//页数
					else return ;//无法再向前
				}else
				{
					if(t9x->dispagecur<(t9x->dispagenum-1))t9x->dispagecur++;//页数
					else return ;//无法再后
				}
			}
		}else
		{		 
			t9x->outstr[0]='\0';
			if(t9x->hbtn[id-12]->caption!=NULL)
			{
				strcpy((char *)t9x->outstr,(const char*)t9x->hbtn[id-12]->caption);//复制
			}else return;//此按钮无效
			t9x->disstr=NULL;		   
			t9x->dispagenum=0;
			t9x->dispagecur=0;	  
			if((t9x->mode&0X07)==0)//拼音输入法的时候
			{
				t9x->instr[0]='\0';//清空输入字符串
				t9_drawvbtn_rim(t9x,T9_PANEL_BKC);//还原
				t9_set_inputtype(t9x,0);//切换为拼音输入法
		 		t9_draw_vbtn(t9x);		//重画垂直按钮 	    
			}
		}  
	}else
	{
		t9x->dispagenum=0;
		t9x->dispagecur=0;	  
	}  
	t9_hbtn_updc(t9x,t9x->disstr+((u16)t9x->dispagecur*12));//更新按钮caption
	t9_draw_hbtn(t9x);//重画横向按钮		   
}

//纵向按钮处理
//t9x:输入法
//id:按键id:0~24,正常的按键ID. 
void t9_vbtn_process(_t9_obj *t9x,u8 id)
{
	u8 i=0;
	u16 slen=0;
	u8 temp=0;
	temp=(t9x->mode>>3)&0x03;//取得此时纵向按钮的状态
	if(temp!=1)//不是汉字输入状态
	{
		t9_set_inputtype(t9x,id-20);//切换输入法
		t9_draw(t9x);				//重画
		return ;					//非汉字输入状态,直接返回
	}
	//下面属于汉字输入状态才用到.
	if(id<24&&id>20)//在中间
	{
		if(t9x->vbtn[id-20]->caption!=NULL)
		{						   
			t9x->disstr=t9x->pymb[(t9x->pypage)*3+id-21]->pymb;//选择新的拼音
		}else return;
	}else if(id==20||id==24)//上/下一页
	{
	    if(id==24)//下一页
		{
			if(((t9x->pypage+1)*3)<t9x->pynum)t9x->pypage++;//下一页
			else return; 
		}else	 //上一页
		{
			if(t9x->pypage>0)t9x->pypage--;//上一页的位置
	        else return ;
		} 		 
		t9_clrhbtn_caption(t9x,0x04);//清除垂直按钮
		temp=t9x->pypage*3; 
		for(i=0;i<3;i++)
		{								
			if((temp+i)>=(t9x->pynum&0X7F))break;//超过pynum
 		 	t9x->vbtn[i+1]->caption=t9x->pymb[temp+i]->py;
 			t9x->vbtn[i+1]->bcfucolor=0XFFFF;//白色
			if(i==0)t9x->disstr=t9x->pymb[temp+i]->pymb;//第一个码表赋值给disstr.	 
		}					    
	}	 
	t9x->vbtn[0]->caption="∧";//添加上下页的符号
 	t9x->vbtn[4]->caption="∨";
	temp=t9x->pynum/3+((t9x->pynum%3)?1:0);//pynum项的最大页数
	if(t9x->pypage<(temp-1))t9x->vbtn[4]->bcfucolor=0XFFFF;//有下一页
	else t9x->vbtn[4]->bcfucolor=0XA535;				   //没有下一页	  
	if(t9x->pypage>0)t9x->vbtn[0]->bcfucolor=0XFFFF;//有上一页
	else t9x->vbtn[0]->bcfucolor=0XA535;			//没有上一页
 	t9_draw_vbtn(t9x);						//重画垂直按钮 	  
	slen=strlen((const char*)t9x->disstr);	//得到字符串长度
	slen/=2;							  	//因为是中文,每个字视为2个字符.
	t9x->dispagenum=slen/6+((slen%6)?1:0);	//得到字符串的总页数
	t9x->dispagecur=0;					  	//从0开始
	t9_hbtn_updc(t9x,t9x->disstr);			//更新字符串到横向的8个按钮
	t9_draw_hbtn(t9x);
}

//输入拼音处理
//t9x:输入法
//id:按键id
void t9_pinyin_process(_t9_obj *t9x,u8 id)
{
	u8 temp;
	u8 str[2];
 	temp=strlen((char*)t9x->instr);//不包括结束符
	switch(id)
	{
		case 0://标点符号切换
			t9x->signpage=0;
			t9_load_sign(t9x);	//装载标点符号按钮
			t9_draw_mbtn(t9x);	//重画主按钮
		 	t9_clrhbtn_caption(t9x,0x02);//清除水平按钮的caption内容
		 	t9_draw_hbtn(t9x);	//重画水平按钮  
			return;				//切换为标点符号
		case 3://退格
			if(temp>1)
			{
				t9x->instr[temp-1]='\0';//输入字符串不为空
				temp--;
			}else if(temp==1)//最后一个字符了. 
			{
				t9_drawvbtn_rim(t9x,T9_PANEL_BKC);//还原
				t9_set_inputtype(t9x,0);//切换为拼音输入法
		 		t9_draw_vbtn(t9x);		//重画垂直按钮 
		 		t9_draw_hbtn(t9x);		//重画水平按钮  
				return;
			}else//退格 
			{	
				t9_back_process(t9x);	
				return;
			}
			break;
		case 7://空格
 			t9_space_process(t9x);	//空格处理
			t9_set_inputtype(t9x,0);//切换为拼音输入法
	 		t9_draw_vbtn(t9x);		//重画垂直按钮 
	 		t9_draw_hbtn(t9x);		//重画水平按钮  
			return;	   
		case 11://回车处理
			t9_enter_process(t9x);		
			t9_set_inputtype(t9x,0);//切换为拼音输入法
	 		t9_draw_vbtn(t9x);		//重画垂直按钮 
	 		t9_draw_hbtn(t9x);		//重画水平按钮  
			return;
	}	  
	if(temp<6)//输入字符串还没有满
	{
		t9_drawvbtn_rim(t9x,0XA535);//画边框	  
		t9x->mode&=0xE7;
		t9x->mode|=0x08;//标记模式为汉字输入 
		if(id!=3)
		{
			str[0]=t9_id2num(id)+'0';
			str[1]='\0';
		}else str[0]='\0';	  
		strcat((char*)t9x->instr,(const char*)str);	//添加转义符
		t9x->pynum=get_matched_pymb(t9x->instr,(py_index **)t9x->pymb);//得到匹配的拼音数组
		t9x->pypage=1;//先预设为1,在执行一次上一页操作,即将page减为0.
		if(t9x->pynum)
		{
			if(t9x->pynum&0X80)//部分匹配
			{
				t9x->pynum&=0X7F;//部分匹配的个数
				t9x->instr[t9x->pynum]='\0';//去掉无效输入！	 
				if(t9x->pynum>1)t9x->pynum=get_matched_pymb(t9x->instr,(py_index **)t9x->pymb);//重新获取完全匹配字符个数
			}  
		}else t9x->pynum=1;//至少有一个 
		if(t9x->pynum&0X80)t9x->pynum=1;
		t9_vbtn_process(t9x,20);
	}	 
}

//t9输入法处理.
//t9x:输入法
void t9_process(_t9_obj *t9x)
{				   
	u8 id;
 	u16 slen;  
	u8 temp; 
	id=t9_keyup_scan(t9x);
	if(id!=0xff)//有效按键
	{
		if(id>=20)//按中纵向的5个按钮了
		{		   
			t9_vbtn_process(t9x,id);//处理纵向按钮	   
		}else if(id<=11)//主按钮
		{	
			temp=t9x->mode&0X07;
			switch(temp)
			{
				case 0://拼音输入法
					if(((t9x->mode>>3)&0x03)==0X02)//标点符号状态
					{
						t9_sign_process(t9x,id);
					}else//正常输入状态
					{
						t9_pinyin_process(t9x,id);								   
					}
					break;
				case 1://123    
					if(id==3)
					{
						t9_back_process(t9x);
						break;//退格.
					}
					else if(id==7)t9x->outstr[0]='.';//小数点
				    else t9x->outstr[0]=t9_id2num(id)+'0';
 					t9x->outstr[1]='\0';//加入结束符   
					break;
				case 2://abc
 				case 3://ABC
				case 4://手写
					if(((t9x->mode>>3)&0x03)==0X02)//标点符号状态
					{
						t9_sign_process(t9x,id);
					}else
					{			    
						switch(id)
						{
							case 0://切换为标点符号
	 							t9x->signpage=0;
								t9_load_sign(t9x);	//装载标点符号按钮
								t9_draw_mbtn(t9x);	//重画主按钮
							 	t9_clrhbtn_caption(t9x,0x02);//清除水平按钮的caption内容
							 	t9_draw_hbtn(t9x);	//重画水平按钮  
								break;				//切换为标点符号
							case 3://退格.
					 			t9_back_process(t9x);	//退格处理		 
								break;	   
							case 7://空格
					 			t9_space_process(t9x);	//空格处理		 
								break;	   
							case 11://回车键
 					 			t9_enter_process(t9x);	//回车键处理		 
								break;
							default://输入的是字母/标点符号切换
								t9x->disstr=t9x->mbtn[id]->caption;//disstr指向当前按下按钮的caption	
	 							slen=strlen((const char*)t9x->disstr);//得到字符串长度
								t9x->dispagenum=slen/6+(slen%6)?1:0;//得到字符串的总页数
								t9x->dispagecur=0;
								t9_hbtn_updc(t9x,t9x->disstr);//添加
								t9_draw_hbtn(t9x);
								break;

						}	   								 
					}   
					break;	  
			}	
		}else//id 在12~19之间
		{	
			switch(t9x->mode&0X07)
			{
				case 0://拼音输入法	   
 				case 2://abc   
  				case 3://ABC   
				case 4://手写
				    t9_hbtn_process(t9x,id);//处理横向按钮
					break;
 				case 1://数字输入
					break;
			}	 
		}
	}
}	





