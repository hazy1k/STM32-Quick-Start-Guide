#include "phoneplay.h"
#include "spb.h" 
#include "camera.h" 
#include "i2s.h" 
#include "audioplay.h" 
#include "usart3.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-拨号器 代码	   
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

//来电任务
//设置任务优先级
#define PHONE_INCALL_TASK_PRIO      	4
//设置任务堆栈大小
#define PHONE_INCALL_STK_SIZE  		    512
//任务堆栈，采用内存管理的方式控制申请	
OS_STK * PHONE_INCALL_TASK_STK=0;	
//任务函数
void phone_incall_task(void *pdata);


//打电话图片
u8*const PHONE_CALL_PIC[3]=
{
	"1:/SYSTEM/APP/PHONE/call_28.bmp",
	"1:/SYSTEM/APP/PHONE/call_36.bmp",
	"1:/SYSTEM/APP/PHONE/call_54.bmp",		
};
//挂电话图片
u8*const PHONE_HANGUP_PIC[3]=
{
"1:/SYSTEM/APP/PHONE/hangup_28.bmp",
"1:/SYSTEM/APP/PHONE/hangup_36.bmp",
"1:/SYSTEM/APP/PHONE/hangup_54.bmp",		
};
//退格图片
u8*const PHONE_DEL_PIC[3]=
{
"1:/SYSTEM/APP/PHONE/del_28.bmp",
"1:/SYSTEM/APP/PHONE/del_36.bmp",	
"1:/SYSTEM/APP/PHONE/del_54.bmp",		
};
//电话提示信息
u8*const phone_remind_tbl[4][GUI_LANGUAGE_NUM]=
{
"拨号中...","撥號中...","Dialing...",
"来电...","来电...","Calling...",
"未找到GSM模块,请检查!","未找到GSM模塊,請檢查!","GSM Module Error!Pleas Check!",
"SIM卡异常,请检查!","SIM卡異常,請檢查!","SIM Card Error!Pleas Check!",
}; 
//键盘名称表
u8*const call_btnstr_tbl[14]=
{   						   
"1","2","3",
"4","5","6",
"7","8","9",
"*","0","#", 
"call","del",	
};
//显示区域清屏
void phone_show_clear(_phonedis_obj *pdis)
{
	gui_fill_rectangle(pdis->xoff,pdis->yoff,pdis->width,pdis->height,BLACK);//清除原来的显示
}
//显示通话时间
//pdis:显示结构体
void phone_show_time(_phonedis_obj *pdis)
{
	u8 *pbuf;
	u16 yoff;
	u16 xoff;
	pbuf=gui_memin_malloc(100);
	if(pbuf==0)return ;
	sprintf((char*)pbuf,"%03d:%02d",pdis->time/60,pdis->time%60); 
	gui_phy.back_color=BLACK;
	xoff=pdis->xoff+pdis->width/2-5*pdis->tsize/2;
	yoff=pdis->yoff+pdis->height/2-pdis->tsize/2;
	gui_fill_rectangle(xoff,yoff,5*pdis->tsize,pdis->tsize,BLACK);//清除原来的显示
	gui_show_strmid(xoff,yoff,5*pdis->tsize,pdis->tsize,WHITE,pdis->tsize,pbuf); 
	gui_memin_free(pbuf);//释放内存
}
//显示呼叫中
//pdis:显示结构体
//gsmx:GSM状态结构体
void phone_show_calling(_phonedis_obj *pdis,__sim900dev *gsmx)
{
	u16 yoff;
	u16 xoff; 
	gui_phy.back_color=BLACK;
	xoff=pdis->xoff+pdis->width/2-5*pdis->tsize/2;
	yoff=pdis->yoff+pdis->height/2-pdis->tsize/2;
	gui_fill_rectangle(xoff,yoff,5*pdis->tsize,pdis->tsize,BLACK);//清除原来的显示
	if(gsmx->mode==3)gui_show_strmid(xoff,yoff,5*pdis->tsize,pdis->tsize,WHITE,pdis->tsize,phone_remind_tbl[1][gui_phy.language]); 
	else gui_show_strmid(xoff,yoff,5*pdis->tsize,pdis->tsize,WHITE,pdis->tsize,phone_remind_tbl[0][gui_phy.language]); 
}
//显示电话号码
//pdis:显示结构体
//gsmx:GSM状态结构体
void phone_show_phone(_phonedis_obj *pdis,__sim900dev*gsmx)
{
	u16 yoff;
	u16 xoff;
	u8 maxnum;
	maxnum=pdis->width/(pdis->psize/2);
	if(pdis->plen>maxnum)maxnum=pdis->plen-maxnum;
	else maxnum=0;
	if(gsmx->mode==0)//号码输入模式,仅仅存在电话号码显示
	{
		xoff=pdis->xoff;
		yoff=pdis->yoff+pdis->height/2-pdis->psize/2;
		gui_fill_rectangle(xoff,yoff,pdis->width,pdis->psize,BLACK);//清除原来的显示
		gui_show_strmid(xoff,yoff,pdis->width,pdis->psize,WHITE,pdis->psize,pdis->phonebuf+maxnum);//显示电话号码
	}else
	{
		xoff=pdis->xoff;
		yoff=pdis->yoff+pdis->height/2-pdis->tsize/2-pdis->psize;
		gui_fill_rectangle(xoff,yoff,pdis->width,pdis->psize,BLACK);//清除原来的显示
		gui_show_strmid(xoff,yoff,pdis->width,pdis->psize,WHITE,pdis->psize,pdis->phonebuf+maxnum);//显示电话号码 
	}
}
//显示输入字符串
//pdis:显示结构体
//gsmx:GSM状态结构体
void phone_show_input(_phonedis_obj *pdis,__sim900dev*gsmx)
{	
	u16 yoff;
	u16 xoff;
	u8 maxnum;
	if(gsmx->mode==2)//仅在电话接通后有效
	{
		maxnum=pdis->width/(pdis->psize/2);
		if(pdis->inlen>maxnum)maxnum=pdis->inlen-maxnum;
		else maxnum=0;
		xoff=pdis->xoff;
		yoff=pdis->yoff+pdis->height/2+pdis->tsize;
		gui_fill_rectangle(xoff,yoff,pdis->width,pdis->psize,BLACK);//清除原来的显示
		gui_show_strmid(xoff,yoff,pdis->width,pdis->psize,WHITE,pdis->psize,pdis->inbuf+maxnum);//显示输入内容
	}
} 
//电话提示音
void phone_ring(void)
{
	u8 i;	 
	for(i=0;i<2;i++)
	{
		BEEP=1;
		delay_ms(100);
		BEEP=0;
		delay_ms(50);
	}	 
}
//来电任务,专门处理电话接入.通过其他程序创建
void phone_incall_task(void *pdata)
{ 
	u16 *bkcolor;
	_btn_obj* p_btn[14];//总共14个按键 
	_phonedis_obj *pdis;
	u8 btnxsize,btnysize; 
	u8 btnxydis;
	u8 fsize;
	u8 lcdtype;
	u16 yoff;
	 
 	u8 i,j;
	u8 rval=0;
	u8 res=0;  
	FIL* f_phone=0;	 
	u32 calltime;
	u8 ascflag=0;
	u8 dcmiflag=0;
	
	bkcolor=gui_memex_malloc(lcddev.width*lcddev.height*2);
	if(bkcolor==NULL)//直接申请失败,则将spb内存释放,再尝试
	{
		spb_delete();//释放spb内存 
 		bkcolor=gui_memex_malloc(lcddev.width*lcddev.height*2); 
	}
	pdis=gui_memin_malloc(sizeof(_phonedis_obj));
	if(pdis)
	{
		memset(pdis,0,sizeof(_phonedis_obj));			//清零数据 
		pdis->inbuf=gui_memin_malloc(PHONE_MAX_INPUT);
		pdis->phonebuf=gui_memin_malloc(PHONE_MAX_INPUT); 
	}
	f_phone=(FIL *)gui_memin_malloc(sizeof(FIL));//开辟FIL字节的内存区域 	
	if(bkcolor&&pdis->phonebuf&&f_phone)//内存申请成功了
	{
		OSTaskSuspend(6); 	//挂起主任务
		if(audiodev.status&(1<<4))
		{
			app_wm8978_volset(0);	 
			I2S_Play_Stop();//停止音频播放
		} 
		if(DCMI->CR&0X01)//摄像头正在工作?
		{
			dcmiflag=1;
			DCMI_Stop();//关闭摄像头
			sw_sdcard_mode();//切换为SD卡模式
		}
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//设置为全屏窗口
		app_read_bkcolor(0,0,lcddev.width,lcddev.height,bkcolor);	//读取背景色 
		if(lcddev.width==240&&asc2_2814==0)
		{ 
			ascflag=1;//标记申请字符集内存
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_2814,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_2814=(u8*)gui_memex_malloc(f_phone->fsize);	//为大字体开辟缓存地址
				if(asc2_2814==0)rval=1;
				else res=f_read(f_phone,asc2_2814,f_phone->fsize,(UINT*)&br);	//一次读取整个文件
				f_close(f_phone);
			} 
		}else if(lcddev.width==320&&asc2_3618==0)
		{
			ascflag=1;//标记申请字符集内存
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_3618,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_3618=(u8*)gui_memex_malloc(f_phone->fsize);	//为大字体开辟缓存地址
				if(asc2_3618==0)rval=1;
				else res=f_read(f_phone,asc2_3618,f_phone->fsize,(UINT*)&br);	//一次读取整个文件
				f_close(f_phone);
			} 
		}else if(lcddev.width==480&&asc2_5427==0)
		{
			ascflag=1;//标记申请字符集内存
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_5427,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_5427=(u8*)gui_memex_malloc(f_phone->fsize);	//为大字体开辟缓存地址
				if(asc2_5427==0)rval=1;
				else res=f_read(f_phone,asc2_5427,f_phone->fsize,(UINT*)&br);	//一次读取整个文件
				f_close(f_phone);
			}
		}
		if(res)rval=res; 
		if(rval==0)//申请成功
		{
			LCD_Clear(BLACK);
			if(lcddev.width==240)
			{ 
				btnxsize=78; 
				btnysize=40;
				btnxydis=2;
				fsize=28; 
				pdis->psize=28;
				pdis->tsize=12;
				lcdtype=0;
			}else if(lcddev.width==320)
			{ 
				btnxsize=102; 
				btnysize=60;
				btnxydis=4;
				fsize=36;
				pdis->psize=36;
				pdis->tsize=16;
				lcdtype=1;
			}else if(lcddev.width==480)
			{ 
				btnxsize=154; 
				btnysize=100;
				btnxydis=6; 
				fsize=54;
				pdis->psize=54;
				pdis->tsize=24;
				lcdtype=2;
			}
			pdis->xoff=0;
			pdis->yoff=0;
			pdis->width=lcddev.width;
			pdis->height=lcddev.height-5*btnysize-6*btnxydis;
			yoff=lcddev.height-btnysize*5-btnxydis*6;
			for(i=0;i<5;i++)//创建14个按钮
			{
				for(j=0;j<3;j++)
				{
					res=i*3+j; 
					if(i==4)
					{
						if(j==2)break;
						p_btn[res]=btn_creat(j*(btnxsize+btnxsize/2+(btnxydis*3)/2)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,(btnxsize*3)/2+btnxydis/2,btnysize,0,1);	//创建图片按钮
						if(j==0)
						{
							p_btn[res]->bcfdcolor=0X76B2;;	//按下时的背景色
							p_btn[res]->bcfucolor=0X3DEA;	//松开时背景色 
							p_btn[res]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
							p_btn[res]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
						}else
						{
							p_btn[res]->bcfdcolor=0XFBEF;;	//按下时的背景色
							p_btn[res]->bcfucolor=0XFA69;	//松开时背景色 
							p_btn[res]->picbtnpathu=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[res]->picbtnpathd=(u8*)PHONE_HANGUP_PIC[lcdtype];
						} 
					}else 
					{
						p_btn[res]=btn_creat(j*(btnxsize+btnxydis)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,btnxsize,btnysize,0,2);//创建圆角按钮
						p_btn[res]->bkctbl[0]=0X39E7;//边框颜色
						p_btn[res]->bkctbl[1]=0X73AE;//第一行的颜色				
						p_btn[res]->bkctbl[2]=0X4208;//上半部分颜色
						p_btn[res]->bkctbl[3]=0X3186;//下半部分颜色	 
						p_btn[res]->bcfucolor=WHITE;//松开时为白色
						p_btn[res]->bcfdcolor=BLACK;//按下时为黑色
						p_btn[res]->caption=(u8*)call_btnstr_tbl[res]; 
					} 
					if(lcddev.width==240)p_btn[res]->font=16;
					else p_btn[res]->font=24;	 
					if(p_btn[res]==NULL){rval=1;break;}//创建失败. 
					p_btn[res]->sta=0;
					p_btn[res]->font=fsize;
					btn_draw(p_btn[res]); 
				}	 
			}
		} 
		memset(pdis->inbuf,0,PHONE_MAX_INPUT);
		memset(pdis->phonebuf,0,PHONE_MAX_INPUT);
		strcpy((char*)pdis->phonebuf,(char*)sim900dev.incallnum);//拷贝号码到phonebuf里面
		pdis->plen=strlen((char*)pdis->phonebuf);//得到号码长度
		phone_show_phone(pdis,&sim900dev);	//显示电话号码
		phone_show_calling(pdis,&sim900dev);//来电中
		while(sim900dev.mode!=0)//非拨号模式
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(5);							//延时一个时钟节拍
			if(system_task_return)break;			//TPAD返回,即拒绝来电
			if(USART3_RX_STA&0X8000)				//接收到数据
			{  
				if(sim900a_check_cmd("NO CARRIER"))sim900dev.mode=0;	//拨号失败
				if(sim900a_check_cmd("NO ANSWER"))sim900dev.mode=0;		//拨号失败
				if(sim900a_check_cmd("ERROR"))sim900dev.mode=0;			//拨号失败 
				USART3_RX_STA=0; 
			}
			if(sim900dev.mode==2)//通话中
			{
				if((OSTime-calltime)>=200)//超过1秒钟了
				{
					calltime=OSTime;
					pdis->time++;
					phone_show_time(pdis);//显示时间
				}
			} 
			for(i=0;i<14;i++)
			{
				res=btn_check(p_btn[i],&in_obj);   
				if(res&&((p_btn[i]->sta&(1<<7))==0)&&(p_btn[i]->sta&(1<<6)))//有按键按下且松开,并且TP松开了
				{
					if(i<12)//1~#
					{
						if(sim900dev.mode==2)//仅在通话中有效
						{
							if(pdis->inlen<PHONE_MAX_INPUT-1)
							{
								u3_printf("AT+CLDTMF=2,\"%c\"\r\n",call_btnstr_tbl[i][0]);
								delay_ms(100);
								u3_printf("AT+VTS=%c\r\n",call_btnstr_tbl[i][0]); 
								pdis->inbuf[pdis->inlen]=call_btnstr_tbl[i][0];	//添加字符
								pdis->inlen++;
								pdis->inbuf[pdis->inlen]=0;//添加结束符
							}
							phone_show_input(pdis,&sim900dev);//显示输入数据 
						}
					}
					if(i==12)//接听来电
					{
						if(sim900dev.mode==3)
						{
							sim900dev.mode=2;//通话中
							p_btn[12]->bcfdcolor=0XFBEF;;	//按下时的背景色
							p_btn[12]->bcfucolor=0XFA69;	//松开时背景色 
							p_btn[12]->picbtnpathu=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[13]->bcfdcolor=0X630C;;	//按下时的背景色
							p_btn[13]->bcfucolor=0X4208;	//松开时背景色 
							p_btn[13]->picbtnpathu=(u8*)PHONE_DEL_PIC[lcdtype];
							p_btn[13]->picbtnpathd=(u8*)PHONE_DEL_PIC[lcdtype];
							btn_draw(p_btn[13]);
							sim900a_send_cmd("ATA","OK",200);//接听电话
							sim900a_cmd_over();	//处理完毕
							calltime=OSTime;	//记录刚刚建立通话时的时间 
						}else 
						{
 							sim900dev.mode=0;//恢复拨号模式
							p_btn[12]->bcfdcolor=0X76B2;;	//按下时的背景色
							p_btn[12]->bcfucolor=0X3DEA;	//松开时背景色 
							p_btn[12]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
							phone_show_clear(pdis);			//清除原来的显示
							pdis->inlen=0;
							pdis->inbuf[pdis->inlen]=0;		//结束符
							sim900dev.mode=0;				//进入拨号模式
							USART3_RX_STA=0;
						}
						btn_draw(p_btn[12]);
					}
					if(i==13)//拒绝来电
					{  
 						if(sim900dev.mode==2)//通话中
						{ 
							if(pdis->inlen)pdis->inlen--; 
							pdis->inbuf[pdis->inlen]=0;		//结束符
						}else
						{
							sim900dev.mode=0;	//进入拨号模式
							break;
						}
					}  
				}
			} 
		} 
		sim900a_send_cmd("ATH","OK",200);	//挂机
		sim900a_cmd_over();//处理完毕
		LCD_Clear(BLACK);
		app_recover_bkcolor(0,0,lcddev.width,lcddev.height,bkcolor);//恢复背景色
		if(audiodev.status&(1<<4))
		{
			I2S_Play_Start();//开启音频播放
			app_wm8978_volset(wm8978set.mvol);	 
		} 
		OSTaskResume(6); 		//恢复主任务
		for(i=0;i<14;i++)btn_delete(p_btn[i]);//删除按钮
	}  
	gui_memin_free(f_phone);		//释放内存
	gui_memin_free(pdis->inbuf);	//释放内存
	gui_memin_free(pdis->phonebuf);	//释放内存
	gui_memin_free(pdis);			//释放内存
	if(ascflag)//有内存申请,才释放
	{
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
	}
	if(dcmiflag)
	{
		sw_ov2640_mode();
		DCMI_Start();	//重新启动摄像头
	}
	gui_memex_free(bkcolor);	//释放内存
	sim900dev.mode=0;			//进入拨号模式
	system_task_return=0;		//防止退出其他任务
	phone_incall_task_delete();	//删除任务 
} 

//创建来电任务
//返回值:0,成功
//    其他,错误代码
u8 phone_incall_task_creat(void)
{ 
    OS_CPU_SR cpu_sr=0; 	
	u8 res;
	if(PHONE_INCALL_TASK_STK)return 0;//任务已经创建了
 	PHONE_INCALL_TASK_STK=gui_memin_malloc(PHONE_INCALL_STK_SIZE*sizeof(OS_STK));
	if(PHONE_INCALL_TASK_STK==0)return 1;	//内存申请失败
	OS_ENTER_CRITICAL();//进入临界区(无法被中断打断)    
	res=OSTaskCreate(phone_incall_task,(void *)0,(OS_STK*)&PHONE_INCALL_TASK_STK[PHONE_INCALL_STK_SIZE-1],PHONE_INCALL_TASK_PRIO);						   
	OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断) 
	return res;
}
//删除来电任务
void phone_incall_task_delete(void)
{  
	gui_memin_free(PHONE_INCALL_TASK_STK);//释放内存
	PHONE_INCALL_TASK_STK=0;
	OSTaskDel(PHONE_INCALL_TASK_PRIO);	//删除音乐播放任务 
}
//电话拨号主程序
u8 phone_play(void)
{
	_btn_obj* p_btn[14];//总共14个按键 
	_phonedis_obj *pdis;
	u8 btnxsize,btnysize; 
	u8 btnxydis;
	u8 fsize;
	u8 lcdtype;
	u16 yoff;
	 
 	u8 i,j;
	u8 rval=0;
	u8 res=0;  
	FIL* f_phone=0;	 
	u32 calltime;
	
	pdis=gui_memin_malloc(sizeof(_phonedis_obj));
	if(pdis==NULL)return 1;								//申请不成功,直接退出
	memset(pdis,0,sizeof(_phonedis_obj));				//清零数据
	pdis->inbuf=gui_memin_malloc(PHONE_MAX_INPUT);
	pdis->phonebuf=gui_memin_malloc(PHONE_MAX_INPUT); 
   	f_phone=(FIL *)gui_memin_malloc(sizeof(FIL));		//开辟FIL字节的内存区域 
	if(!pdis->phonebuf||!pdis->inbuf||!f_phone)rval=1;	//申请失败
	else
	{
		if(lcddev.width==240)
		{
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_2814,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_2814=(u8*)gui_memex_malloc(f_phone->fsize);	//为大字体开辟缓存地址
				if(asc2_2814==0)rval=1;
				else res=f_read(f_phone,asc2_2814,f_phone->fsize,(UINT*)&br);	//一次读取整个文件
				f_close(f_phone);
			} 
		}else if(lcddev.width==320)
		{
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_3618,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_3618=(u8*)gui_memex_malloc(f_phone->fsize);	//为大字体开辟缓存地址
				if(asc2_3618==0)rval=1;
				else res=f_read(f_phone,asc2_3618,f_phone->fsize,(UINT*)&br);	//一次读取整个文件
				f_close(f_phone);
			} 
		}else if(lcddev.width==480)
		{
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_5427,FA_READ);//打开文件 
			if(res==FR_OK)
			{
				asc2_5427=(u8*)gui_memex_malloc(f_phone->fsize);	//为大字体开辟缓存地址
				if(asc2_5427==0)rval=1;
				else res=f_read(f_phone,asc2_5427,f_phone->fsize,(UINT*)&br);	//一次读取整个文件
				f_close(f_phone);
			}
		} 
		if(res)rval=res;
	}  
	if(rval==0)//申请成功
	{
		LCD_Clear(BLACK);
		if(lcddev.width==240)
		{ 
			btnxsize=78; 
			btnysize=40;
			btnxydis=2;
			fsize=28; 
			pdis->psize=28;
			pdis->tsize=12;
			lcdtype=0;
		}else if(lcddev.width==320)
		{ 
			btnxsize=102; 
			btnysize=60;
			btnxydis=4;
			fsize=36;
			pdis->psize=36;
			pdis->tsize=16;
			lcdtype=1;
		}else if(lcddev.width==480)
		{ 
			btnxsize=154; 
			btnysize=100;
			btnxydis=6; 
			fsize=54;
			pdis->psize=54;
			pdis->tsize=24;
			lcdtype=2;
		}
		pdis->xoff=0;
		pdis->yoff=0;
		pdis->width=lcddev.width;
		pdis->height=lcddev.height-5*btnysize-6*btnxydis;
		yoff=lcddev.height-btnysize*5-btnxydis*6;
		for(i=0;i<5;i++)//创建14个按钮
		{
			for(j=0;j<3;j++)
			{
				res=i*3+j; 
				if(i==4)
				{
					if(j==2)break;
					p_btn[res]=btn_creat(j*(btnxsize+btnxsize/2+(btnxydis*3)/2)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,(btnxsize*3)/2+btnxydis/2,btnysize,0,1);	//创建图片按钮
 					if(j==0)
					{
						p_btn[res]->bcfdcolor=0X76B2;;	//按下时的背景色
						p_btn[res]->bcfucolor=0X3DEA;	//松开时背景色 
						p_btn[res]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
						p_btn[res]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
					}else
					{
						p_btn[res]->bcfdcolor=0X630C;;	//按下时的背景色
						p_btn[res]->bcfucolor=0X4208;	//松开时背景色 
						p_btn[res]->picbtnpathu=(u8*)PHONE_DEL_PIC[lcdtype];
						p_btn[res]->picbtnpathd=(u8*)PHONE_DEL_PIC[lcdtype];
					} 
				}else 
				{
					p_btn[res]=btn_creat(j*(btnxsize+btnxydis)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,btnxsize,btnysize,0,2);//创建圆角按钮
					p_btn[res]->bkctbl[0]=0X39E7;//边框颜色
					p_btn[res]->bkctbl[1]=0X73AE;//第一行的颜色				
					p_btn[res]->bkctbl[2]=0X4208;//上半部分颜色
					p_btn[res]->bkctbl[3]=0X3186;//下半部分颜色	 
					p_btn[res]->bcfucolor=WHITE;//松开时为白色
					p_btn[res]->bcfdcolor=BLACK;//按下时为黑色
					p_btn[res]->caption=(u8*)call_btnstr_tbl[res]; 
				} 
				if(lcddev.width==240)p_btn[res]->font=16;
				else p_btn[res]->font=24;	 
				if(p_btn[res]==NULL){rval=1;break;}//创建失败. 
				p_btn[res]->sta=0;
				p_btn[res]->font=fsize;
			}	 
		}
	}
	if(rval==0)
	{ 
		for(i=0;i<14;i++)btn_draw(p_btn[i]);
		sim900dev.mode=0;
		memset(pdis->inbuf,0,PHONE_MAX_INPUT);
		memset(pdis->phonebuf,0,PHONE_MAX_INPUT);
		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(5);							//延时一个时钟节拍
			if(system_task_return)break;	  		//TPAD返回
			if(USART3_RX_STA&0X8000)				//接收到数据
			{
				if(sim900dev.mode==1||sim900dev.mode==2)
				{
					if(sim900dev.mode==1)if(sim900a_check_cmd("+COLP:"))
					{
						pdis->time=0;
						calltime=OSTime;//记录刚刚建立通话时的时间
						sim900dev.mode=2;	//拨号成功
					}
					if(sim900a_check_cmd("NO CARRIER"))sim900dev.mode=0;	//拨号失败
					if(sim900a_check_cmd("NO ANSWER"))sim900dev.mode=0;		//拨号失败
					if(sim900a_check_cmd("ERROR"))sim900dev.mode=0;			//拨号失败
					if(sim900dev.mode==0)
					{
						phone_show_clear(pdis);			//清除原来的显示
						p_btn[12]->bcfdcolor=0X76B2;;	//按下时的背景色
						p_btn[12]->bcfucolor=0X3DEA;	//松开时背景色 
						p_btn[12]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
						p_btn[12]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
						btn_draw(p_btn[12]);
						pdis->inlen=0;
						pdis->inbuf[pdis->inlen]=0;		//结束符
						phone_show_phone(pdis,&sim900dev);//显示电话号码
					}
					sim900a_cmd_over();//处理完毕;
				}
			}
			if(sim900dev.mode==2)//通话中
			{
				if((OSTime-calltime)>=200)//超过1秒钟了
				{
					calltime=OSTime;
					pdis->time++;
					phone_show_time(pdis);//显示时间
				}
			}
	 		for(i=0;i<14;i++)
			{
				res=btn_check(p_btn[i],&in_obj);   
				if(res&&((p_btn[i]->sta&(1<<7))==0)&&(p_btn[i]->sta&(1<<6)))//有按键按下且松开,并且TP松开了
				{
					if(i<12)//1~#
					{
						if(sim900dev.mode==0)//号码输入模式
						{
							if(pdis->plen<PHONE_MAX_INPUT-1)
							{
								u3_printf("AT+CLDTMF=2,\"%c\"\r\n",call_btnstr_tbl[i][0]);
								pdis->phonebuf[pdis->plen]=call_btnstr_tbl[i][0];//添加字符
								pdis->plen++;
								pdis->phonebuf[pdis->plen]=0;//添加结束符
							}
						
						}else if(sim900dev.mode==2)//仅在通话中有效
						{
							if(pdis->inlen<PHONE_MAX_INPUT-1)
							{
								u3_printf("AT+CLDTMF=2,\"%c\"\r\n",call_btnstr_tbl[i][0]);
								delay_ms(150);
								u3_printf("AT+VTS=%c\r\n",call_btnstr_tbl[i][0]); 
								pdis->inbuf[pdis->inlen]=call_btnstr_tbl[i][0];	//添加字符
								pdis->inlen++;
								pdis->inbuf[pdis->inlen]=0;//添加结束符
							}
						}
					}
					if(i==12&&pdis->plen)//仅在有号码的时候进行拨号
					{
						if(sim900dev.mode==0)
						{
							if((sim900dev.status&0XC0)!=0XC0)//GSM模块/SIM卡 错误,提示错误信息
							{
 								if((sim900dev.status&0X80)==0)window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)phone_remind_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);
								else window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)phone_remind_tbl[3][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);
 								break;
							} 
							sim900dev.mode=1;//进入拨号中模式
							p_btn[12]->bcfdcolor=0XFBEF;;	//按下时的背景色
							p_btn[12]->bcfucolor=0XFA69;	//松开时背景色 
							p_btn[12]->picbtnpathu=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_HANGUP_PIC[lcdtype];
							phone_show_clear(pdis);			//清除原来的显示
							phone_show_phone(pdis,&sim900dev);//显示电话号码
							phone_show_calling(pdis,&sim900dev);//呼叫中
							u3_printf("ATD%s;\r\n",pdis->phonebuf);	//拨号
						}else 
						{
							sim900dev.mode=0;//恢复拨号模式
							p_btn[12]->bcfdcolor=0X76B2;;	//按下时的背景色
							p_btn[12]->bcfucolor=0X3DEA;	//松开时背景色 
							p_btn[12]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
							phone_show_clear(pdis);			//清除原来的显示
							pdis->inlen=0;
							pdis->inbuf[pdis->inlen]=0;		//结束符
							sim900a_send_cmd("ATH","OK",200);//挂机
							sim900a_cmd_over();//处理完毕
						}
						btn_draw(p_btn[12]);
					}
					if(i==13)//退格
					{
						if(sim900dev.mode==0)//号码输入模式
						{
							if(pdis->plen)pdis->plen--; 
							pdis->phonebuf[pdis->plen]=0;	//结束符
						}else if(sim900dev.mode==2)
						{
							if(pdis->inlen)pdis->inlen--; 
							pdis->inbuf[pdis->inlen]=0;		//结束符
						}
					}
					if(sim900dev.mode==0)phone_show_phone(pdis,&sim900dev);//显示电话号码
					if(sim900dev.mode==2)phone_show_input(pdis,&sim900dev);//显示输入数据 
				}
			} 
		}
	}
	if(sim900dev.mode)
	{
		sim900a_send_cmd("ATH","OK",200);//挂机
		sim900a_cmd_over();//处理完毕
	}
	gui_memin_free(f_phone);		//释放内存
	gui_memin_free(pdis->inbuf);	//释放内存
	gui_memin_free(pdis->phonebuf);	//释放内存
	gui_memin_free(pdis);			//释放内存
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
	for(i=0;i<14;i++)btn_delete(p_btn[i]);//删除按钮
	return 0;
}








































