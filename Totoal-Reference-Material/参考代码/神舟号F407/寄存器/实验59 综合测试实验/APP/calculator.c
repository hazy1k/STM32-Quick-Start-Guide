#include "calculator.h"
#include "math.h"
#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-科学计算器 代码	   
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

#define CALC_DISP_COLOR  	0X0000	//显示数字的颜色
#define CALC_DISP_BKCOLOR  	0XA5F4	//显示区域的背景色
#define CALC_MAX_EXP		200		//最大的正指数范围,这里设定为200,如果内存够的话,可以设置到300以上.
									//不过要考虑浮点数的数值范围
//π值
#define CALC_PI 			3.1415926535897932384626433832795 	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//计算器需要用到的全局数据
//这些数据本来可以定义为局部变量,但是由于未知原因(怀疑堆栈问题),在本工程里面,必须定义为全局变量.
//否则结果出错!!!
static double calc_x1=0,calc_x2=0;	//2个操作数
static double calc_temp=0;		   	//临时数据
u8 *outbuf;					//输出缓存区
u8 *tempbuf;				//临时数据存储区
_calcdis_obj *cdis;			//计算结果显示结构体


//加载计算器主界面UI	
//calcdis:显示结构体  
void calc_load_ui(_calcdis_obj*calcdis)
{	
 	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[15][gui_phy.language],0X05);//显示标题  
  	gui_fill_rectangle(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight,BLACK);				//填充底色 		  
  	gui_fill_rectangle(calcdis->xoff,gui_phy.tbheight+calcdis->yoff,calcdis->width,calcdis->height,CALC_DISP_BKCOLOR);//显示区域底色填充	  
} 

//显示计算器的输入数据
//calcdis:显示结构体
//buf:字符串地址
void calc_show_inbuf(_calcdis_obj*calcdis,u8 *buf)
{
	u16 xoff;
 	u8 len=strlen((const char*)buf);//得到字符串长度
   	xoff=calcdis->xoff+calcdis->xdis*2+calcdis->fsize/2; 
	gui_fill_rectangle(xoff,gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,13*(calcdis->fsize/2),calcdis->fsize,CALC_DISP_BKCOLOR);//清空之前的显示	  
	gui_phy.back_color=CALC_DISP_BKCOLOR;
	gui_show_string(buf,xoff+13*(calcdis->fsize/2)-len*(calcdis->fsize/2),gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,lcddev.width,lcddev.height,calcdis->fsize,BLACK);//显示字符串
}	 				
//计算器核心运算
//calcdis:显示结构体
//x1,x2:两个操作数
//buf:输入数据存储区
//ctype:运算法则
//calc_sta:计算器状态
//返回值:0,没有更新显示;1,已经更新显示;
u8 calc_exe(_calcdis_obj*calcdis,double *x1,double *x2,u8 *buf,u8 ctype,u8 *calc_sta)
{
	u8 inlen=0;
	u8 res=0;
	inlen=(*calc_sta)&0X0F;//得到输入长度
	if((*calc_sta&0X80)==0)	//还没有操作数
	{	  
		*x1=atof((const char *)buf);//转换为数字
		if((*calc_sta&0X40))
		{
			*x1=-*x1;
			*calc_sta&=~(0X40);//符号位改为正   
 		}
		*calc_sta|=0X80; 		//标记已经有一个操作数了.
	}else if(inlen)				//有数据输入
	{	 	
		*x2=atof((const char *)buf);	//转换为数字
		if((*calc_sta&0X40))*x2=-*x2; 	//输入的是负数
 		switch(ctype)
		{
			case 1://加法
				*x1=*x1+*x2;
				break;
			case 2://减法
				*x1=*x1-*x2;
				break;
			case 3://乘法
				*x1=*x1*(*x2);
				break;
			case 4://除法
				*x1=*x1/(*x2);
				break;
			case 5://x^y次方
				*x1=pow(*x1,*x2);
				break;
			case 0://没有任何运算符
				*x1=*x2;
 				break;
		}	 
		res=1;//需要更新显示
	}
	if(ctype>5)//单操作数运算
	{
		switch(ctype)
		{
			case 6://sin计算
				*x1=sin((*x1*CALC_PI)/180);//转换为角度计算结果
				break;
			case 7://cos计算
				*x1=cos((*x1*CALC_PI)/180);//转换为角度计算结果
				break;
			case 8://tan计算
				*x1=tan((*x1*CALC_PI)/180);//转换为角度计算结果
				break;
			case 9://log计算
				*x1=log10(*x1);
				break;
			case 10://ln计算
				*x1=log(*x1);
				break;
			case 11://x^2计算
				*x1=*x1*(*x1);
				break;
			case 12://开方计算
				*x1=sqrt(*x1);
				break;
			case 13://倒数计算
				*x1=1/(*x1);
				break;	   	    
		}					 
		res=1;//需要更新显示
	}
	if(res)//更新显示
	{
		*calc_sta&=~(0X40);				//符号位改为正
 		if(calc_show_res(calcdis,*x1))	//显示有错误,数据清空
		{
			*calc_sta=0;
			*x1=0;
			*x2=0;
			gui_memset(buf,0,14); 	//输入缓存清零
 		}	    
	}
	*calc_sta&=0XF0;  		//清除输入长度
	gui_memset(buf,0,14); 	//输入缓存清零
 	return res;
}
//显示指数 
//calcdis:显示结构体
//exp:指数值
void calc_show_exp(_calcdis_obj*calcdis,short exp)			   
{	 
	u16 xoff,yoff; 
	xoff=calcdis->xoff+calcdis->xdis*3+(calcdis->fsize/2)*14;
	yoff=gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12+calcdis->fsize-28-calcdis->fsize/7;
	gui_fill_rectangle(xoff,yoff,32,28,CALC_DISP_BKCOLOR);//清空之前的显示	  
 	if(exp!=0) 
	{ 
 		if(exp<0)
		{
			exp=-exp;
 			gui_show_ptchar(xoff,yoff,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,16,'-',1);	//显示负号
		}
		gui_show_num(xoff+8,yoff,3,CALC_DISP_COLOR,16,exp,0X81);							//填充显示指数
		gui_show_ptchar(xoff,yoff+16,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,12,'X',1);	//叠加方式显示乘号
		gui_show_num(xoff+6,yoff+16,2,CALC_DISP_COLOR,12,10,0X81);						//填充显示底数
 	}
}
//显示正负号
//calcdis:显示结构体
//fg:0,去掉负号;1,显示负号.
void calc_show_flag(_calcdis_obj*calcdis,u8 fg)
{ 
	gui_phy.back_color=CALC_DISP_BKCOLOR;
	if(fg==0)gui_show_ptchar(calcdis->xoff+calcdis->xdis,gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,lcddev.width,lcddev.height,0,BLACK,cdis->fsize,' ',0);//去掉负号
	else gui_show_ptchar(calcdis->xoff+calcdis->xdis,gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,lcddev.width,lcddev.height,0,BLACK,cdis->fsize,'-',0);	//显示负号
}  
//更新输入显示
//calcdis:显示结构体
//calc_sta:计算器状态
//inbuf:输入数组
//len:输入长度
void calc_input_fresh(_calcdis_obj*calcdis,u8* calc_sta,u8 *inbuf,u8 len)
{	     
	*calc_sta&=0XF0;
	*calc_sta|=len&0X0F;						//更新操作数长度
	calc_show_flag(calcdis,(*calc_sta)&0X40);	//负号处理
	calc_show_inbuf(calcdis,inbuf);  			//显示输入数据
 	calc_show_exp(calcdis,0);					//显示指数部分	 
} 
//显示计算方法
//calcdis:显示结构体
//ctype:计算方法
void calc_ctype_show(_calcdis_obj*calcdis,u8 ctype)
{
	u8 *chx;
	u16 xoff,yoff;
	xoff=calcdis->xoff+calcdis->xdis*2+calcdis->fsize/2;
	yoff=gui_phy.tbheight+calcdis->yoff+calcdis->ydis/2;
	switch(ctype)
	{			    
		case 1:	//加法
			chx="＋";
			break;
		case 2:	//减法
			chx="－";
			break;
		case 3:	//乘法
			chx="×";
			break;
		case 4:	//除法
			chx="÷";
			break;
		case 5://x^y次方
			chx="x^y";
			break;		  
		case 6://sin
			chx="sin";
			break;
		case 7://cos
			chx="cos";
			break;
		case 8://tan
			chx="tan";
			break;
		case 9://log
			chx="log";
			break;
		case 10://ln
			chx="ln";
			break;
		case 11://x^2
			chx="x^2";
			break;
		case 12://
			chx="√￣";
			break;
		case 13://1/x
			chx="1/x";
			break;	    
		default:
			chx="";
			break;
	}
	gui_fill_rectangle(xoff,yoff,6*4,12,CALC_DISP_BKCOLOR);//清空之前的显示	  
	gui_show_ptstr(xoff,yoff,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,12,chx,1);//显示计算方法				 
}


//显示结果
//calcdis:显示结构体
//res:结果
//返回值:0,无错误;1,有错误
u8 calc_show_res(_calcdis_obj*calcdis,double res)
{									
	signed short exp=0;
	u8 temp=13;
	u8 i;
	if(res<0)
	{
		res=-res;									    
		calc_show_flag(calcdis,1);		//显示负号
	}else calc_show_flag(calcdis,0);	//取消负号		
	gui_memset(outbuf,0,20);
	gui_memset(tempbuf,0,14);
  	sprintf((char*)outbuf,"%0.11e",res);//输出结果到outbuf里面
   	outbuf[temp]=0;
  	exp=(signed short)atof((const char*)(outbuf+temp+1));		//得到数据的指数部分.
	if((outbuf[0]=='i')||(outbuf[0]=='n')||exp>CALC_MAX_EXP)	//错误的结果
	{
		calc_show_flag(calcdis,0);	//取消负号
 		outbuf[0]='E';
		outbuf[1]=0;
		calc_show_inbuf(calcdis,outbuf);//输出结果到LCD
 		calc_show_exp(calcdis,0);	 
		return 1;
 	}	  
 	if(exp>-5&&exp<12)
	{
		if(exp<0)
		{
			for(i=0;i<(-exp+1);i++)
			{	   
				if(i==1)tempbuf[i]='.';
				else tempbuf[i]='0';	
			}
			exp+=12;
			for(i=0;i<exp;i++)
			{	   
				if(i==0)tempbuf[12+1-exp+i]=outbuf[i];	//复制	
				else tempbuf[12+1-exp+i]=outbuf[i+1];		//复制	   	
			}   
			tempbuf[13]=0;//末尾添加结束符
			outbuf[0]=0;
			strcpy((char*)outbuf,(char*)tempbuf);//拷贝tempbuf的内容到outbuf.
		}else//exp>=0
		{
			for(i=0;i<exp;i++)
			{	   
				temp=outbuf[2+i];
			 	outbuf[1+i]=temp;
				outbuf[2+i]='.';	
			}   									  
		}
		exp=0;//不需要显示exp	 					    
 	}
	for(i=12;i>0;i--)//把多余的0去掉
	{
		if(outbuf[i]=='0')outbuf[i]=0;
		else if(outbuf[i]=='.')
		{
			outbuf[i]=0;
			break;
		}else break;
	}	
	calc_show_inbuf(calcdis,outbuf);//输出结果到LCD
	calc_show_exp(calcdis,exp);	 
 	return 0;   
}
//格式化显示
//calcdis:显示结构体
//res:要转换的数据,最大支持0XFFFF
//fmt:0:十进制
//    1:十六进制
//    2:二进制
void calc_fmt_show(_calcdis_obj*calcdis,u16 res,u8 fmt)
{
	u8 *fmtstr="";
	u8 outbuf[17];
	u8 i=0;
	u8 len;
	u16 xoff,yoff;
	xoff=calcdis->xoff+calcdis->xdis*2+calcdis->fsize/2;
	yoff=gui_phy.tbheight+calcdis->yoff+calcdis->ydis;
	gui_memset(outbuf,0,17);
  	gui_fill_rectangle(calcdis->xoff,gui_phy.tbheight+calcdis->yoff,calcdis->width,calcdis->height,CALC_DISP_BKCOLOR);	//整个显示区域底色填充	  
	switch(fmt)
	{
		case 0://十进制
			fmtstr="DEG"; 
			sprintf((char*)outbuf,"%d",res);	//输出结果到outbuf里面	  
			break;
		case 1://十六进制 
			fmtstr="HEX";
			sprintf((char*)outbuf,"%X",res);	//输出结果到outbuf里面	  
			calc_show_inbuf(calcdis,outbuf);	//输出结果到LCD
 			break;
  		case 2://二进制
			fmtstr="BIN"; 
 		  	for(i=0;i<16;i++)
			{
				outbuf[15-i]='0'+(res&0x01);
				res>>=1;
			}
			for(i=0;i<16;i++)
			{
				if(outbuf[i]=='0')outbuf[i]=0;
				else break;
			}   
			len=strlen((const char*)(outbuf+i));//得到字符串长度
			gui_phy.back_color=CALC_DISP_BKCOLOR;
			if(i>1)gui_show_string(outbuf+i,xoff+(calcdis->fsize/2)*13-len*(calcdis->fsize/2),yoff+12,lcddev.width,lcddev.height,calcdis->fsize,BLACK);//显示字符串
			else gui_show_string(outbuf+i,xoff+(calcdis->fsize/2)*13+(calcdis->fsize/2)*(2-i)-len*(calcdis->fsize/2),yoff+12,lcddev.width,lcddev.height,calcdis->fsize,BLACK);//显示字符串	
   			break;	 
	}    
	if(fmt<2)calc_show_inbuf(calcdis,outbuf);	//十进制和16进制,输出结果到LCD
	gui_show_ptstr(xoff+40,yoff-calcdis->ydis/2,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,12,fmtstr,1);//显示当前格式				 
}
u8*const calc_btnstr_tbl[29]=
{   						   
"sin","cos","tan","log","ln",
"x^2","x^y","sqrt","1/x","FMT",
"7","8","9","DEL","AC",
"4","5","6","＋","－",
"1","2","3","×","÷",
"0","+/-",".","=",	
};
	  
//计算器主程序
u8 calc_play(void)
{
	_btn_obj* c_btn[29];
	u8 mbtnysize,fbtnysize;
	u8 btnxsize; 
	u8 btnxdis,btnydis;
	
	u8 *inbuf;		//最多输入12个字符+结束符,总共13个 
	u8 calc_sta=0;		//计算器状态
						//[7]:0,还没有操作数;1,已经有一个操作数了
						//[6]:符号位;0,正数;1,负数;
						//[5]:0,操作数2未保存;1,操作数2已保存;
						//[3:0]:输入操作数长度
	
	u8 ctype=0;			//计算法则
						//0,没有计算法则
						//1,加法.
						//2,减法
						//3,乘法
						//4,除法
						//5,x^y次方.
						//6,sin计算
						//7,cos计算
						//8,tan计算
						//9,log计算
						//10,ln计算
						//11,x^2计算
						//12,开方
						//13,1/x计算
						//14,格式转换
 	u8 maxlen=12;		//最大输入字符数.默认12个,当输入了小数点以后,变为13个.
	   
 	u8 i,j;
	u8 rval=0;
	u8 res;
	u8 ttype=0;			//上一次操作的运算符
	u8 cfmt=0;			//格式,默认为十进制格式 

	FIL* f_calc=0;	  
	inbuf=(u8*)gui_memin_malloc(14);	//输入缓存区
	outbuf=(u8*)gui_memin_malloc(20);	//输出缓存区 
	tempbuf=(u8*)gui_memin_malloc(14);	//临时存储区 
	cdis=gui_memin_malloc(sizeof(_calcdis_obj));
  	if(!inbuf||!outbuf||!tempbuf||!cdis)rval=1;//申请失败 
   	f_calc=(FIL *)gui_memin_malloc(sizeof(FIL));//开辟FIL字节的内存区域 
	if(f_calc==NULL)rval=1;		//申请失败
	else
	{
		if(lcddev.width==240)
		{
			res=f_open(f_calc,(const TCHAR*)APP_ASCII_2814,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_2814=(u8*)gui_memex_malloc(f_calc->fsize);	//为大字体开辟缓存地址
				if(asc2_2814==0)rval=1;
				else res=f_read(f_calc,asc2_2814,f_calc->fsize,(UINT*)&br);	//一次读取整个文件
			} 
		}else if(lcddev.width==320)
		{
			res=f_open(f_calc,(const TCHAR*)APP_ASCII_3618,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_3618=(u8*)gui_memex_malloc(f_calc->fsize);	//为大字体开辟缓存地址
				if(asc2_3618==0)rval=1;
				else res=f_read(f_calc,asc2_3618,f_calc->fsize,(UINT*)&br);	//一次读取整个文件
			} 
		}else if(lcddev.width==480)
		{
			res=f_open(f_calc,(const TCHAR*)APP_ASCII_5427,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_5427=(u8*)gui_memex_malloc(f_calc->fsize);	//为大字体开辟缓存地址
				if(asc2_5427==0)rval=1;
				else res=f_read(f_calc,asc2_5427,f_calc->fsize,(UINT*)&br);	//一次读取整个文件
			}  
		} 
		if(res)rval=res;
	} 	
	if(rval==0)
	{
		if(lcddev.width==240)
		{
			cdis->xoff=2;
			cdis->yoff=10;
			cdis->width=236;
			cdis->height=50;
			cdis->xdis=2;
			cdis->ydis=5; 
			cdis->fsize=28; 
			btnxsize=44;
			mbtnysize=30;
			fbtnysize=20;
			btnxdis=4;
			btnydis=8;
			
		}else if(lcddev.width==320)
		{
			cdis->xoff=10;
			cdis->yoff=20;
			cdis->width=300;
			cdis->height=70;
			cdis->xdis=4;
			cdis->ydis=11; 
			cdis->fsize=36; 
			btnxsize=60;
			mbtnysize=40;
			fbtnysize=30;
			btnxdis=4;
			btnydis=18;
		}else if(lcddev.width==480)
		{
			cdis->xoff=10;
			cdis->yoff=50;
			cdis->width=460;
			cdis->height=110;
			cdis->xdis=2;
			cdis->ydis=19; 
			cdis->fsize=54; 
			btnxsize=90;
			mbtnysize=60;
			fbtnysize=40;
			btnxdis=6;
			btnydis=35;
		}
		for(i=0;i<6;i++)//创建29个按钮
		{
			for(j=0;j<5;j++)
			{
				res=i*5+j;
				if(i<2)
				{
					c_btn[res]=btn_creat(j*(btnxsize+btnxdis)+btnxdis/2,i*(mbtnysize+btnydis)+gui_phy.tbheight+cdis->yoff*2+cdis->height+(mbtnysize-fbtnysize)/2,btnxsize,fbtnysize,0,2);//创建圆角按钮
					c_btn[res]->bkctbl[0]=BLACK;//边框颜色
					c_btn[res]->bkctbl[1]=0X8410;//第一行的颜色				
					c_btn[res]->bkctbl[2]=0X8410;//上半部分颜色
					c_btn[res]->bkctbl[3]=0X630C;//下半部分颜色	 
				}else if(i==5&&j==3)
				{
					c_btn[res]=btn_creat(j*(btnxsize+btnxdis)+btnxdis/2,i*(mbtnysize+btnydis)+gui_phy.tbheight+cdis->yoff*2+cdis->height,btnxsize*2+btnxdis,mbtnysize,0,2);//创建圆角按钮
					c_btn[res]->bkctbl[0]=BLACK;//边框颜色
					c_btn[res]->bkctbl[1]=0X4A49;//第一行的颜色				
					c_btn[res]->bkctbl[2]=0X4A49;//上半部分颜色
					c_btn[res]->bkctbl[3]=0X3186;//下半部分颜色	 
					j=4;
				}else 
				{
					c_btn[res]=btn_creat(j*(btnxsize+btnxdis)+btnxdis/2,i*(mbtnysize+btnydis)+gui_phy.tbheight+cdis->yoff*2+cdis->height,btnxsize,mbtnysize,0,2);//创建圆角按钮
					if(i==2&&(j==3||j==4))//DEL.AC按钮,全红色
					{
						c_btn[res]->bkctbl[0]=BLACK;//边框颜色
						c_btn[res]->bkctbl[1]=RED;//第一行的颜色				
						c_btn[res]->bkctbl[2]=RED;//上半部分颜色
						c_btn[res]->bkctbl[3]=RED;//下半部分颜色	 
					}else
					{
						c_btn[res]->bkctbl[0]=BLACK;//边框颜色
						c_btn[res]->bkctbl[1]=0X4A49;//第一行的颜色				
						c_btn[res]->bkctbl[2]=0X4A49;//上半部分颜色
						c_btn[res]->bkctbl[3]=0X3186;//下半部分颜色	 
					}
				}
				if(i<2)//功能按键
				{
					if(lcddev.width==240)c_btn[res]->font=12;
					else c_btn[res]->font=16;			
					c_btn[res]->bcfucolor=BLACK;//松开时为黑色
					c_btn[res]->bcfdcolor=WHITE;//按下时为白色
				}else	//主按键
				{
					if(lcddev.width==240)c_btn[res]->font=16;
					else c_btn[res]->font=24;			
					c_btn[res]->bcfucolor=WHITE;//松开时为白色
					c_btn[res]->bcfdcolor=BLACK;//按下时为黑色
				}
				if(c_btn[res]==NULL){rval=1;break;}//创建失败. 
				c_btn[res]->caption=(u8*)calc_btnstr_tbl[res]; 
				c_btn[res]->sta=0;
			}	 
		}
	}
	if(rval==0)//准备工作完成了
	{	   
		calc_load_ui(cdis);					//加载主界面
		calc_fmt_show(cdis,calc_x1,cfmt);	//显示格式化
		for(i=0;i<29;i++)btn_draw(c_btn[i]);
		gui_memset(inbuf,0,14);				//输入缓存清零
		calc_show_inbuf(cdis,"0");			//显示0
		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(5);							//延时2个时钟节拍
			if(system_task_return)break;	  		//TPAD返回
 
	 		for(i=0;i<29;i++)
			{
				res=btn_check(c_btn[i],&in_obj);   
				if(res&&((c_btn[i]->sta&(1<<7))==0)&&(c_btn[i]->sta&(1<<6)))//有按键按下且松开,并且TP松开了
				{	
					res=calc_sta&0X0F;		//得到输入长度
					if((i!=9)&&(ctype==14))	//不是格式化按钮,且上一次操作时格式转换
					{
						cfmt=0;
						calc_x1=0;
						calc_x2=0;
						calc_sta=0;
						calc_temp=0;
						ttype=1;	//模拟一次清零						  
						ctype=0;	   
						calc_fmt_show(cdis,calc_x1,cfmt);
					}
 					switch(i)
					{
						case 0://sin计算
						case 1://cos计算
						case 2://tan计算
						case 3://log计算
						case 4://ln计算
						case 5://x^2计算   
						case 7://sqrt计算   
						case 8://倒数计算   
							if(i<6)ctype=6+i; 
							else ctype=5+i;   
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//计算处理  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);	 	//输入缓存清零
							maxlen=12;
							calc_ctype_show(cdis,ctype);		//显示这次的操作符
							ctype=0;
							ttype=0;
							break;		 
						case 6://x^y计算
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//计算处理  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//输入缓存清零
							maxlen=12;
							ctype=5;   
							break; 	  
						case 9://格式转换
							cfmt++;
							if(cfmt>2)cfmt=0;
						 	if((calc_sta&0X80)==0)
							{
								calc_x1=atof((const char *)inbuf);	//转换为数字	  
								calc_sta|=0X80; 				//标记已经有一个操作.
							} 
							calc_sta=0XF0;
							gui_memset(inbuf,0,14);				//输入缓存清零
							maxlen=12;	
							ctype=14;//标记为格式转换					   	
							calc_fmt_show(cdis,calc_x1,cfmt);
							break; 
						case 10://输入7
						case 11://输入8
						case 12://输入9
							if(res==1&&inbuf[0]=='0')res=0;					//第一个数据不能为0 
							if(res<maxlen)inbuf[res++]='7'+i-10;			//输入7/8/9
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//更新显示
							break;	    
						case 13://退格
							if(res>1)
							{	       
								res--;
								if(inbuf[res]=='.')maxlen=12;//恢复12个数据长度
								inbuf[res]=0;
  							}else if(inbuf[0]!='0')inbuf[0]='0';//最后一个数字总是为0
 							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//更新显示  
							break;
						case 14://清零
							maxlen=12;
							gui_memset(inbuf,0,14);//输入缓存清零
							calc_sta=0;
							calc_show_inbuf(cdis,"0");	//显示0
							calc_show_flag(cdis,calc_sta&0X40);	//负号	  
						  	calc_show_exp(cdis,0);				//去掉指数部分	
							calc_x1=0;
							calc_x2=0;
							calc_temp=0;
							ttype=1;	//模拟一次清零						  
							ctype=0;
							break;
						case 15://输入4
						case 16://输入5
						case 17://输入6
							if(res==1&&inbuf[0]=='0')res=0;					//第一个数据不能为0 
							if(res<maxlen)inbuf[res++]='4'+i-15;			//输入4/5/6
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//更新显示
							break;
						case 18://加法				 
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//计算处理  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//输入缓存清零
							maxlen=12;
							ctype=1;   
							break;
						case 19://减法
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//计算处理  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//输入缓存清零
							maxlen=12;
							ctype=2;   
							break; 	 
						case 20://输入1
						case 21://输入2
						case 22://输入3
							if(res==1&&inbuf[0]=='0')res=0;					//第一个数据不能为0 
							if(res<maxlen)inbuf[res++]='1'+i-20;			//输入1/2/3
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//更新显示
							break;	  
						case 23://乘法	   
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//计算处理  
 							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//输入缓存清零
							maxlen=12;
							ctype=3;   
							break;
						case 24://除法	    
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//计算处理  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//输入缓存清零
							maxlen=12;
							ctype=4;   
							break; 	 
						case 25://输入0
							if(res!=1||inbuf[0]!='0')
							{
								if(res<maxlen)inbuf[res++]='0';//输入0
							} 
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//更新显示  
							break;
 						case 26://符号反转						
							if(calc_sta&(1<<6))calc_sta&=~(1<<6);	 
							else calc_sta|=1<<6;	   
							calc_show_flag(cdis,calc_sta&0X40);	//负号处理
							break;
						case 27://输入小数点
							if(maxlen==12)
							{
								if(res<maxlen)
								{
									if(res==0)inbuf[res++]='0';//先输入.	  
									inbuf[res++]='.';//输入.
									maxlen=13;
								}
							}
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//更新显示  
							break;
						case 28://输出结果	   
							if(calc_sta&0X80)//已经有一个操作数了
							{
								if(res==0)		//没有输入新数据
								{
									if((calc_sta&0X10)==0)//操作数2还未保存
									{
										calc_sta|=0X10;	//标记已经保存了
										calc_temp=calc_x1;		//操作数2即等于操作数1
									} 
								}else calc_sta&=~0X10;	//取消保存操作数2标志 
							} 
							if(calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta)==0)		//有无执行计算?
							{															//无
								if(calc_sta&0X10)//有操作数2
								{
									switch(ctype)
									{
										case 1://加法
											calc_x1=calc_x1+calc_temp;
											break;
										case 2://减法
											calc_x1=calc_x1-calc_temp;
											break;
										case 3://乘法
											calc_x1=calc_x1*calc_temp;
											break;
										case 4://除法
											calc_x1=calc_x1/calc_temp;
											break;
									}							
								}
						 		if(calc_show_res(cdis,calc_x1))	//显示有错误,数据清空
								{
									calc_sta=0;
									calc_x1=0;
									calc_x2=0;
									gui_memset(inbuf,0,14); 	//输入缓存清零
						 		}									   
							}
							if((calc_sta&0X10)==0)//操作数2还未保存
							{
								calc_sta|=0X10;	//标记已经保存了
								calc_temp=calc_x2;		//保存操作数2
							} 	 				   
							break;	  
					}
					//printf("key:%d pressed!\r\n",i);
				}
				if(ctype!=ttype)//运算符变了
				{
					ttype=ctype;
					calc_ctype_show(cdis,ctype);
				}
			}
		}
	}			 
   	gui_memin_free(inbuf);	//释放内存
   	gui_memin_free(outbuf);	//释放内存
   	gui_memin_free(tempbuf);//释放内存
 	gui_memin_free(f_calc);	//释放内存 
	gui_memex_free(cdis);	//释放内存
	if(lcddev.width==240)
	{
		gui_memex_free(asc2_2814);
		asc2_2814=0;
	}else if(lcddev.width==320)
	{
		gui_memex_free(asc2_3618);
		asc2_3618=0;
	}else if(lcddev.width==480)
	{
		gui_memex_free(asc2_5427);
		asc2_5427=0;
	} 
	for(i=0;i<29;i++)
	{
		btn_delete(c_btn[i]);//删除这些按键
	}
	return 0;
}


























