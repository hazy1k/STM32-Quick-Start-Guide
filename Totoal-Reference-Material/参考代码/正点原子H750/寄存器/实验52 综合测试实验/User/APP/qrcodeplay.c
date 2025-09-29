#include "qrcodeplay.h"
#include "qr_encode.h"
#include "t9input.h"


//qr编码提示信息
u8*const qr_remind_msg_tbl[3][GUI_LANGUAGE_NUM]=
{ 
{"输入内容:","輸入內容:","Input Text:",},
{"请输入内容!","請輸入內容!","Pls Input Text!",}, 
{"内容太多!","內容太多!","Too Many Text",},  
};
//qr编码按钮标题
u8*const qr_enc_tbl[GUI_LANGUAGE_NUM]= 
{"编码","編碼","Encode",};
//qr结果窗标题
u8*const qr_result_tbl[GUI_LANGUAGE_NUM]= 
{"编码结果","編碼結果","Result",};
 
//二维码编码应用
void qrencode_play(void)
{
 	u8 res; 
	u8 rval=0; 
 	
	u16 i,j;
	u16 pixsize;		//点大小
	u16 wwidth,wheight;	//QR结果窗口大小
	u16 wpitch;			//间隙

	u16 textlen;		//输入字符串长度
	u16 h1,h2;			//纵向间隔
	u16 w1;				//横向间隔
	u16 t9height=0;		//T9输入法高度
	u16 btnw,btnh;		//按钮宽度/高度
	u16 memow,memoh;	//文本框宽度/高度 
	
	u8 cbtnfsize=0;		//按钮字体大小
	u16 tempy;  
	
 	_btn_obj* encbtn=0;	//编码按钮
	_memo_obj * tmemo=0;//memo控件
	_t9_obj * t9=0;		//输入法  
	u16 *bkcolorbuf;	//背景色缓存区
	 
	 
	if(lcddev.width==240)
	{
		btnw=65;btnh=30;
		memow=160,memoh=132;
		cbtnfsize=16;t9height=134;  
	}else if(lcddev.width==272)
	{ 
		btnw=75;btnh=38;
		memow=180,memoh=242; 	 
		cbtnfsize=16;t9height=176;
	}else if(lcddev.width==320)
	{ 
		btnw=90;btnh=40;
		memow=200,memoh=242; 	 
		cbtnfsize=16;t9height=176;
	}else if(lcddev.width==480)
	{ 	
		btnw=120;btnh=40; 
		memow=300,memoh=434; 
		cbtnfsize=24;t9height=266;
	}else if(lcddev.width==600)
	{ 	
		btnw=160;btnh=60; 
		memow=400,memoh=582; 
		cbtnfsize=24;t9height=368;
	}
	h1=(lcddev.height-gui_phy.tbheight-memoh-t9height-cbtnfsize)*4/9;
	h2=h1/4;
	w1=(lcddev.width-memow-btnw)/3;
	
	LCD_Clear(QR_BACK_COLOR);//画背景色
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[16][gui_phy.language],0X05);	//显示标题  
	POINT_COLOR=WHITE;  
	gui_show_string(qr_remind_msg_tbl[0][gui_phy.language],w1,gui_phy.tbheight+h1,lcddev.width,gui_phy.tbfsize,gui_phy.tbfsize,BLACK);//显示提示	
 	tempy=gui_phy.tbheight+h1+h2+gui_phy.tbfsize;
 	tmemo=memo_creat(w1,tempy,memow,memoh,0,1,gui_phy.tbfsize,2000);		//申请2000个字节的内存. 
	t9=t9_creat((lcddev.width%5)/2,lcddev.height-t9height,lcddev.width-(lcddev.width%5),t9height,0); 
	tempy=gui_phy.tbheight+h1+h2+gui_phy.tbfsize+(memoh-btnh)/2;
 	encbtn=btn_creat(w1*2+memow,tempy,btnw,btnh,0,0); 
	qrx=gui_memin_malloc(sizeof(_qr_encode));	
	if(!t9||!encbtn||!tmemo||!qrx)rval=1;//内存申请失败  
 	if(rval==0)
	{
		encbtn->caption=qr_enc_tbl[gui_phy.language];
		encbtn->font=cbtnfsize;
		t9_draw(t9);				//画T9输入法
		btn_draw(encbtn);			//画按钮
		memo_draw_memo(tmemo,0);	//画输入memo框
		while(1)
		{					  
 			if(system_task_return)break;			//TPAD返回
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值    
			memo_check(tmemo,&in_obj);   
			t9_check(t9,&in_obj);		   
			if(t9->outstr[0]!=NULL)//添加字符
			{  
				memo_add_text(tmemo,t9->outstr);  
				t9->outstr[0]=NULL;//清空输出字符 
			} 		
			res=btn_check(encbtn,&in_obj);   
			if(res&&((encbtn->sta&(1<<7))==0)&&(encbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
			{   
				textlen=strlen((char*)tmemo->text);
				rval=0;
				if(textlen)
				{
					EncodeData((char *)tmemo->text);
					if(qrx->m_nSymbleSize*2>(lcddev.width-40))rval=2;			//提示信息2
					else
					{
						for(wpitch=5;wpitch>1;wpitch--)
						{
							if(qrx->m_nSymbleSize*2<(lcddev.width-10*4*wpitch))break;
						}
						wpitch*=10;//得到间隙大小
						pixsize=(lcddev.width-4*wpitch)/qrx->m_nSymbleSize;		//点大小
						if(pixsize>8)pixsize=8;									//最大不超过8个像素
						 
						wwidth=pixsize*qrx->m_nSymbleSize+2*wpitch;
						wheight=pixsize*qrx->m_nSymbleSize+2*wpitch+WIN_CAPTION_HEIGHT;   
						bkcolorbuf=gui_memex_malloc(wwidth*wheight*2);			//申请内存
						app_read_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//读取背景色 
						window_msg_box((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,(u8*)"",(u8*)qr_result_tbl[gui_phy.language],0,0,1<<5,0);
						for(i=0;i<qrx->m_nSymbleSize;i++)
						{
							for(j=0;j<qrx->m_nSymbleSize;j++)
							{ 
								if(qrx->m_byModuleData[i][j]==1)
								{
									gui_fill_rectangle((lcddev.width-wwidth)/2+wpitch+pixsize*i,(lcddev.height-wheight)/2+WIN_CAPTION_HEIGHT+wpitch+pixsize*j,pixsize,pixsize,BLACK); 
								}
							} 
						}
						while(1)
						{
							if(system_task_return)break;	//TPAD返回
							delay_ms(10);
						} 
						app_recover_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//恢复背景色  				  
						gui_memex_free(bkcolorbuf);			//释放内存 
						system_task_return=0;
					}						
				}else rval=1;	//提示信息1
				if(rval)
				{
					bkcolorbuf=gui_memex_malloc(220*100*2);	//申请内存
					app_read_bkcolor((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,bkcolorbuf);//读取背景色
					window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)qr_remind_msg_tbl[rval][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
					delay_ms(1000);
					app_recover_bkcolor((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,bkcolorbuf);//恢复背景色  				  
					gui_memex_free(bkcolorbuf);				//释放内存
				}
			}
			delay_ms(5);			
		} 
	}		  
	t9_delete(t9);
 	btn_delete(encbtn);
 	memo_delete(tmemo); 
	gui_memin_free(qrx);
}





