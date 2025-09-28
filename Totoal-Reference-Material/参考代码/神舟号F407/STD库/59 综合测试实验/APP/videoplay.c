#include "videoplay.h"	 
#include "audioplay.h"
#include "settings.h"
#include "ucos_ii.h"
#include "wm8978.h"
#include "i2s.h"
#include "wavplay.h"   
#include "mjpeg.h" 
#include "timer.h" 
#include "avi.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-视频播放器 代码	   
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
 
__videodev videodev;		//视频播放控制器
 

extern vu8 framecnt;		//统一的帧计数器
extern vu8 framecntout;		//统一的帧计数器输出变量
vu8 aviframeup;				//视频播放时隙控制变量,当等于1的时候,可以更新下一帧视频

//音频数据I2S DMA传输回调函数
void audio_i2s_dma_callback(void) 
{      
	videodev.i2splaybuf++;
	if(videodev.i2splaybuf>3)videodev.i2splaybuf=0;
	if(DMA1_Stream4->CR&(1<<19))
	{	 
		DMA1_Stream4->M0AR=(u32)videodev.i2sbuf[videodev.i2splaybuf];//指向下一个buf
	}
	else 
	{   		
		DMA1_Stream4->M1AR=(u32)videodev.i2sbuf[videodev.i2splaybuf];//指向下一个buf
	} 
}  
/////////////////////////////////////////////////////////////////////////
//显示audio播放时间
//sx,sy:起始坐标
//sec:时间
void video_time_show(u16 sx,u16 sy,u16 sec)
{
	u16 min;
	u8 *buf;
	buf=gui_memin_malloc(100);
	if(buf==0)return;
	min=sec/60;//得到分钟数 
	sec=sec%60;//得到秒钟数 
	sprintf((char*)buf,"%03d:%02d",min,sec);
	gui_fill_rectangle(sx,sy,36,12,AUDIO_MAIN_BKCOLOR);	//填充底色 
	gui_show_string(buf,sx,sy,36,12,12,AUDIO_INFO_COLOR);	
	gui_memin_free(buf);
} 
//video加载主界面
void video_load_ui(void)
{	
	u8 vui_tpbar_height=0;	//顶部标题栏高度
	u8 vui_msgbar_height=0;	//信息栏高度
	u8 vui_prgbar_height=0;	//进度条区域高度
	u8 vui_btnbar_height=0;	//底部按钮条高度 
	u8 pixpitch=0;
	u8 toffy=0;  
 	if(lcddev.width==240)
	{
		vui_tpbar_height=20;
		vui_msgbar_height=46;
		vui_prgbar_height=30;
		vui_btnbar_height=60;
		pixpitch=6;  
	}else if(lcddev.width==320)
	{
		vui_tpbar_height=24;
		vui_msgbar_height=50;
		vui_prgbar_height=30;
		vui_btnbar_height=80;
		pixpitch=20;  
	}else if(lcddev.width==480)
	{
		vui_tpbar_height=30;
		vui_msgbar_height=60;
		vui_prgbar_height=40;
		vui_btnbar_height=120;
		pixpitch=30;  
	} 
	gui_fill_rectangle(0,0,lcddev.width,vui_tpbar_height,AUDIO_TITLE_BKCOLOR);	//填充标题栏底色 
	gui_show_strmid(0,0,lcddev.width,vui_tpbar_height,AUDIO_TITLE_COLOR,16,(u8*)APP_MFUNS_CAPTION_TBL[3][gui_phy.language]);	//显示标题
	gui_fill_rectangle(0,vui_tpbar_height,lcddev.width,vui_msgbar_height,AUDIO_MAIN_BKCOLOR);									//填充信息栏背景色   
	toffy=(vui_msgbar_height-44)/2;
	minibmp_decode((u8*)APP_VOL_PIC,pixpitch+2,vui_tpbar_height+20+toffy-2,16,16,0,0);				//解码音量图标
 	gui_show_string("00%",pixpitch+20,vui_tpbar_height+20+toffy+12,66,12,12,AUDIO_INFO_COLOR); 		//显示音量
  	gui_fill_rectangle(0,lcddev.height-vui_btnbar_height-vui_prgbar_height,lcddev.width,vui_prgbar_height,AUDIO_MAIN_BKCOLOR);	//填充进度条栏背景色
	gui_fill_rectangle(0,lcddev.height-vui_btnbar_height,lcddev.width,vui_btnbar_height,AUDIO_BTN_BKCOLOR);						//填充按钮栏背景色   
	gui_fill_rectangle(0,vui_tpbar_height+vui_msgbar_height,lcddev.width,lcddev.height-vui_tpbar_height-vui_msgbar_height-vui_prgbar_height-vui_btnbar_height,BLACK);//填充底色
}
//显示音量百分比  
//pctx:百分比值
void video_show_vol(u8 pctx)
{
	u16 sx,sy;
	if(lcddev.width==240)
	{
		sx=6+20;
		sy=20+20+1+12; 
	}else if(lcddev.width==320)
	{
		sx=20+20;
		sy=24+20+3+12;  
	}else if(lcddev.width==480)
	{
		sx=30+20;
		sy=30+20+8+12;  
	}
	gui_phy.back_color=AUDIO_MAIN_BKCOLOR;//设置背景色为底色
 	gui_fill_rectangle(sx,sy,24,12,AUDIO_MAIN_BKCOLOR);//填充背景色 
	if(pctx==100)
	{
		gui_show_num(sx,sy,3,AUDIO_INFO_COLOR,12,pctx,0x80);//显示音量百分比   
		gui_show_ptchar(sx+18,sy,sx+18+6,sy+12,0,AUDIO_INFO_COLOR,12,'%',0);	//显示百分号
	}else 
	{
		gui_show_num(sx,sy,2,AUDIO_INFO_COLOR,12,pctx,0x80);//显示音量百分比   
 		gui_show_ptchar(sx+12,sy,sx+12+6,sy+12,0,AUDIO_INFO_COLOR,12,'%',0);	//显示百分号
  	}	 
} 
//videodevx:audio控制器
//videoprgbx:进度条
//aviinfo:视频信息 
void video_info_upd(__videodev *videodevx,_progressbar_obj* videoprgbx,_progressbar_obj* volprgbx,AVI_INFO *aviinfo,u8 flag)
{
	static u32 oldsec;
	u8 vui_tpbar_height;	//顶部标题栏高度
	u8 vui_msgbar_height;	//信息栏高度 
	u8 pixpitch;
	u8 toffy; 
	u16 tempx;
	u8 *buf;
	float ftemp;
	u32 totsec=0;		//video文件总时间 
	u32 cursec; 		//当前播放时间 
	if(lcddev.width==240)
	{
		vui_tpbar_height=20;
		vui_msgbar_height=46;
		pixpitch=6;
	}else if(lcddev.width==320)
	{
		vui_tpbar_height=24;
		vui_msgbar_height=50;
		pixpitch=20;
	}else if(lcddev.width==480)
	{
		vui_tpbar_height=30;
		vui_msgbar_height=60;
		pixpitch=30;
	}
	toffy=(vui_msgbar_height-44)/2; 
	if(flag==1)//需要更新名字等信息
	{ 
		buf=gui_memin_malloc(100);	//申请100字节内存
		if(buf==NULL)return;		//game over  
		gui_fill_rectangle(0,vui_tpbar_height+3,lcddev.width,14,AUDIO_MAIN_BKCOLOR);//上下各多清空一点,清空之前的显示 
		gui_show_ptstrwhiterim(4,vui_tpbar_height+4,lcddev.width-4,lcddev.height,0,0X0000,0XFFFF,12,videodevx->name);	//显示新的名字		
 		gui_phy.back_color=AUDIO_MAIN_BKCOLOR;//设置背景色为底色 
		//显示音量百分比
		audio_show_vol((volprgbx->curpos*100)/volprgbx->totallen);//显示音量百分比 
		//显示曲目编号
		sprintf((char*)buf,"%03d/%03d",videodevx->curindex+1,videodevx->mfilenum);
		gui_fill_rectangle(pixpitch+20+volprgbx->width-42,vui_tpbar_height+20+toffy+12,42,12,AUDIO_MAIN_BKCOLOR);	//清空之前的显示  
		gui_show_string(buf,pixpitch+20+volprgbx->width-42,vui_tpbar_height+20+toffy+12,42,12,12,AUDIO_INFO_COLOR);	
 		//显示xxxKhz	
		tempx=pixpitch*2+20+volprgbx->width;//起始x坐标
		gui_fill_rectangle(tempx,vui_tpbar_height+20+toffy,54,12,AUDIO_MAIN_BKCOLOR);		//清空之前的显示  
		ftemp=(float)aviinfo->SampleRate/1000;//xx.xxxKhz
		sprintf((char*)buf,"%2.3fKhz",ftemp);
		gui_show_string(buf,tempx,vui_tpbar_height+20+toffy,54,12,12,AUDIO_INFO_COLOR);	 
		//显示分辨率
		tempx=pixpitch*2+20+volprgbx->width;//起始x坐标 
		gui_fill_rectangle(tempx,vui_tpbar_height+20+toffy+12,66,12,AUDIO_MAIN_BKCOLOR);	//清空之前的显示   
		sprintf((char*)buf,"Pix:%dX%d",aviinfo->Width,aviinfo->Height);  
		gui_show_string(buf,tempx,vui_tpbar_height+20+toffy+12,66,12,12,AUDIO_INFO_COLOR);	//显示分辨率
 		//其他处理
 	 	oldsec=0;
 		videoprgbx->totallen=videodevx->file->fsize;	//更新总长度	
		videoprgbx->curpos=0;	  
		gui_memin_free(buf);//释放内存
	}  
	totsec=(aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;	//歌曲总长度(单位:ms) 
	totsec/=1000;		//秒钟数. 
  	cursec=((double)videodevx->file->fptr/videodevx->file->fsize)*totsec;//当前播放到多少秒了?   
	videoprgbx->curpos=f_tell(videodevx->file);	//得到当前的播放位置
	progressbar_draw_progressbar(videoprgbx);	//更新进度条位置 
	if(oldsec!=cursec)
	{
		oldsec=cursec;  
		//显示帧数 
		buf=gui_memin_malloc(100);	//申请100字节内存
		if(buf==NULL)return;
		tempx=pixpitch*2+20+volprgbx->width+60;//起始x坐标
		gui_fill_rectangle(tempx,vui_tpbar_height+20+toffy,30,12,AUDIO_MAIN_BKCOLOR);		//清空之前的显示
		sprintf((char*)buf,"%02dfps",framecntout); 
		gui_show_string(buf,tempx,vui_tpbar_height+20+toffy,30,12,12,AUDIO_INFO_COLOR);	   	
		gui_memin_free(buf);//释放内存
		//显示时间
		if(lcddev.width==240)
		{
			video_time_show(2,lcddev.height-48-12-12-9,cursec);			//显示播放时间   
			video_time_show(40+160+3,lcddev.height-48-12-12-9,totsec);	//显示总时间   
		}else if(lcddev.width==320)
		{
			video_time_show(4,lcddev.height-60-20-12-9,cursec);		//显示播放时间   
			video_time_show(45+230+5,lcddev.height-60-20-12-9,totsec);	//显示总时间   
		}else if(lcddev.width==480)
		{
			video_time_show(24,lcddev.height-80-40-12-14,cursec);		//显示播放时间   
			video_time_show(70+340+10,lcddev.height-80-40-12-14,totsec);//显示总时间   
		} 
	}
}
//视频播放提示语
u8*const video_msg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"读取文件错误!!","讀取文件錯誤!!","File Read Error!",},
{"内存不够!!","內存不夠!!","Out of memory!",},	 	
{"分辨率不支持","分辨率不支持","Image Size Error!",},
}; 
//视频播放
u8 video_play(void)
{				 	 
	u8 rval=0;			//返回值	  
	u8 *pname=0; 
	u16 i;
	u8 key; 
	
 	_btn_obj* rbtn;		//返回按钮控件
   	_filelistbox_obj * flistbox;
	_filelistbox_list * filelistx; 	//文件  
	memset(&videodev,0,sizeof(__videodev));//videodev结构体清零.
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[3][gui_phy.language],0X07);//选择目标文件,并得到目标数量

   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
 	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		flistbox->fliter=FLBOX_FLT_VIDEO;		//图片文件
		filelistbox_add_disk(flistbox);			//添加磁盘路径
 		filelistbox_draw_listbox(flistbox);
	} 	   
	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
 	if(rbtn==NULL)rval=1;	//没有足够内存够分配
	else
	{
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];	//名字
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);//画按钮
	} 
	while(rval==0)//主循环
	{
		tp_dev.scan(0);    							//扫描触摸屏
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);		//得到按键键值   
		delay_ms(5);
		if(system_task_return)break;				//TPAD返回   	 
		filelistbox_check(flistbox,&in_obj);		//扫描文件
		
		if(flistbox->dbclick==0X81)					//双击文件了
		{	
			gui_memin_free(videodev.path);			//释放内存
			gui_memex_free(videodev.mfindextbl);	//释放内存
			videodev.path=(u8*)gui_memin_malloc(strlen((const char*)flistbox->path)+1);//为新的路径申请内存
			if(videodev.path==NULL){rval=1;break;}	//内存分配失败
			videodev.path[0]='\0';					//在最开始加入结束符.
			strcpy((char *)videodev.path,(char *)flistbox->path);
			videodev.mfindextbl=(u16*)gui_memex_malloc(flistbox->filecnt*2);//为新的tbl申请内存
			if(videodev.mfindextbl==NULL){rval=1;break;}//内存分配失败
			for(i=0;i<flistbox->filecnt;i++)videodev.mfindextbl[i]=flistbox->findextbl[i];//复制
			videodev.mfilenum=flistbox->filecnt;		//记录文件个数	 
			videodev.curindex=flistbox->selindex-flistbox->foldercnt;//当前应该播放的文件索引 
			printf("play:%d/%d\r\n",videodev.curindex,videodev.mfilenum);
			rval=video_play_mjpeg();
			if(rval==0XFF)break;
			else
			{
				if(rval)//存在错误
				{ 
					if(rval<20)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)video_msg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);	//文件读取错误
					if(rval==0X21)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)video_msg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//内存错误
					if(rval==0X22)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)video_msg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//分辨率错误			
					delay_ms(2000);
					rval=0;
				}
				flistbox->dbclick=0;	//设置非文件浏览状态
				app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[3][gui_phy.language],0X07);//选择目标文件,并得到目标数量
				btn_draw(rbtn);			//画按钮
				flistbox->selindex=flistbox->foldercnt+videodev.curindex;//选中条目为当前正在播放的条目			
				filelistbox_rebuild_filelist(flistbox);//重建flistbox
 				system_task_return=0;	//还不能退出图片浏览			
			} 
		} 
		key=btn_check(rbtn,&in_obj);
  		if(key&&((rbtn->sta&0X80)==0))
		{		  
			if(flistbox->dbclick!=0X81)//在文件浏览的时候按了返回按钮,则返回上一层目录
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
				if(filelistx->type==FICO_DISK)//已经不能再往上了,则退出视频浏览
				{				 
					break;
				}else filelistbox_back(flistbox);//退回上一层目录	 
			} 
		}  
	}	
	filelistbox_delete(flistbox);		//删除filelist
	btn_delete(rbtn);					//删除按钮	 	  
 	gui_memin_free(pname);				//释放内存	
	gui_memin_free(videodev.path);		//释放内存
	gui_memex_free(videodev.mfindextbl);//释放内存 
 	return rval; 
} 

 
//播放mjpeg文件
//返回值:
//0,正常播放结束,返回文件浏览状态.
//0XFF,按退出按钮了,直接结束视频播放
//其他,错误代码
u8 video_play_mjpeg(void)
{    
	u8 *pname=0;		//带路径的名字
	u8* framebuf=0;		//视频解码buf	 
	u8* pbuf=0;			//buf指针   
	DIR videodir;		//video目录	  
 	FILINFO videoinfo;	//video文件信息	
	_progressbar_obj* videoprgb=0,*volprgb=0;
	_btn_obj* tbtn[5];	
	u16 lastvolpos; 
	u8 i;
	u8 tcnt=0;

	u8  res=0;
	u16 offset=0; 
	u32	nr;  
    u8 i2ssavebuf; 
	u16 videoheight;	//视频显示区域高度
	u16 yoff;
	u8 key=0;	
	
	for(i=0;i<5;i++)tbtn[i]=0;
	if(audiodev.status&(1<<7))//当前在放歌??
	{
		audio_stop_req(&audiodev);	//停止音频播放
		audio_task_delete();		//删除音乐播放任务.
	}
	app_wm8978_volset(wm8978set.mvol);	 
	WM8978_ADDA_Cfg(1,0);	//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);	//开启DAC输出 
	videoinfo.lfsize=_MAX_LFN*2+1;								//长文件名最大长度
	framebuf=gui_memin_malloc(AVI_VIDEO_BUF_SIZE);				//申请视频buf
	videoinfo.lfname=gui_memin_malloc(videoinfo.lfsize);		//为长文件缓存区分配内存  
	videodev.i2sbuf[0]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//申请音频内存
	videodev.i2sbuf[1]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//申请音频内存
	videodev.i2sbuf[2]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//申请音频内存
	videodev.i2sbuf[3]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//申请音频内存 
	videodev.file=(FIL*)gui_memin_malloc(sizeof(FIL)); 			//申请videodev.file内存 
	if(!videodev.i2sbuf[3]||!framebuf||!videodev.file||!videoinfo.lfname)
	{ 
		res=0X21;
	}else 
	{	
		memset(videodev.i2sbuf[0],0,AVI_AUDIO_BUF_SIZE);
		memset(videodev.i2sbuf[1],0,AVI_AUDIO_BUF_SIZE); 
		memset(videodev.i2sbuf[2],0,AVI_AUDIO_BUF_SIZE);
		memset(videodev.i2sbuf[3],0,AVI_AUDIO_BUF_SIZE);  
		if(lcddev.width==240)
		{
			videoprgb=progressbar_creat(40,lcddev.height-48-12-10-11,160,12,0X20);//video播放进度条
			if(videoprgb==NULL)res=0X21;
			volprgb=progressbar_creat(6+20,20+20+1+1,110,10,0X20);	//声音大小进度条
			if(volprgb==NULL)res=0X21;	   
			volprgb->totallen=63;	
			videoheight=164;
			yoff=66;
		}else if(lcddev.width==320)
		{
			videoprgb=progressbar_creat(45,lcddev.height-60-20-10-11,230,12,0X20);//video播放进度条
			if(videoprgb==NULL)res=0X21;
			volprgb=progressbar_creat(20+20,24+20+3+1,150,10,0X20);	//声音大小进度条
			if(volprgb==NULL)res=0X21;	   
			volprgb->totallen=63;	//更新总长度,音频从100~250.偏移为100.	
			videoheight=296;
			yoff=74;
		}else if(lcddev.width==480)
		{
			videoprgb=progressbar_creat(70,lcddev.height-80-40-10-16,340,12,0X20);//video播放进度条
			if(videoprgb==NULL)res=0X21;
			volprgb=progressbar_creat(30+20,30+20+8+1,280,10,0X20);	//声音大小进度条
			if(volprgb==NULL)res=0X21;	   
			volprgb->totallen=63;	//更新总长度,音频从100~250.偏移为100.	
			videoheight=550;
			yoff=90;
		} 
		if(wm8978set.mvol<=63)volprgb->curpos=wm8978set.mvol;
		else//错误的数据 
		{
			wm8978set.mvol=0;
			volprgb->curpos=0;
		}	  
		lastvolpos=volprgb->curpos;//设定最近的位置
		for(i=0;i<5;i++)//循环创建5个按钮
		{
			if(lcddev.width==240)tbtn[i]=btn_creat(0+i*48,lcddev.height-48-6,48,48,0,1);			//创建图片按钮
			else if(lcddev.width==320)tbtn[i]=btn_creat(2+i*(60+4),lcddev.height-60-10,60,60,0,1);	//创建图片按钮
			else if(lcddev.width==480)tbtn[i]=btn_creat(8+i*(80+16),lcddev.height-80-20,80,80,0,1);	//创建图片按钮
			if(tbtn[i]==NULL){res=0X21;break;}		//创建失败.
			tbtn[i]->bcfdcolor=0X2CFF;				//按下时的背景色
			tbtn[i]->bcfucolor=AUDIO_BTN_BKCOLOR;	//松开时背景色 
			tbtn[i]->picbtnpathu=(u8*)AUDIO_BTN_PIC_TBL[0][i];
			tbtn[i]->picbtnpathd=(u8*)AUDIO_BTN_PIC_TBL[1][i];
			tbtn[i]->sta=0;	 
		} 
		if(res==0)
		{
			res=f_opendir(&videodir,(const TCHAR*)videodev.path);//打开选中的目录 
		} 
	}  		
	if(res==0)
	{ 
		videoprgb->inbkcolora=0x738E;			//默认色
		videoprgb->inbkcolorb=AUDIO_INFO_COLOR;	//默认色 
		videoprgb->infcolora=0X75D;				//默认色
		videoprgb->infcolorb=0X596;				//默认色  
		volprgb->inbkcolora=AUDIO_INFO_COLOR;	//默认色
		volprgb->inbkcolorb=AUDIO_INFO_COLOR;	//默认色 
		volprgb->infcolora=0X75D;				//默认色
		volprgb->infcolorb=0X596;				//默认色   
		video_load_ui();						//加载主界面
		for(i=0;i<5;i++)btn_draw(tbtn[i]);		//画按钮
		tbtn[2]->picbtnpathu=(u8*)AUDIO_PLAYR_PIC;//按下一次之后变为播放松开状态
		progressbar_draw_progressbar(videoprgb);//画进度条	 
		progressbar_draw_progressbar(volprgb);	//画进度条
	}
	while(res==0)
	{ 
		ff_enter(videodir.fs);//进入fatfs,防止被打断.
		dir_sdi(&videodir,videodev.mfindextbl[videodev.curindex]);
		ff_leave(videodir.fs);//退出fatfs,继续运行os等
		res=f_readdir(&videodir,&videoinfo);//读取文件信息
		if(res)break;//打开失败   
		videodev.name=(u8*)(*videoinfo.lfname?videoinfo.lfname:videoinfo.fname);
		pname=gui_memin_malloc(strlen((const char*)videodev.name)+strlen((const char*)videodev.path)+2);//申请内存
		if(pname==NULL){res=0X21;break;}	//申请失败	    
		pname=gui_path_name(pname,videodev.path,videodev.name);	//文件名加入路径    
		res=f_open(videodev.file,(char *)pname,FA_READ);
		gui_memin_free(pname);			//释放内存 
		if(res==0)
		{
			pbuf=framebuf;			
			res=f_read(videodev.file,pbuf,AVI_VIDEO_BUF_SIZE,&nr);//开始读取	
			if(res)//文件系统错误,直接退出不再播放
			{ 
				break;
			} 	 
			//开始avi解析
			res=avi_init(pbuf,AVI_VIDEO_BUF_SIZE);	//avi解析
			if(res)//一般问题,还可以继续播放
			{
				printf("avi err:%d\r\n",res);
				break;
			} 	 
			if(avix.Height>videoheight||avix.Width>lcddev.width)//图片尺寸有问题,直接退出不再播放
			{
				res=0X22;
				printf("avi size error\r\n");
				break;
			}
			TIM6_Int_Init(avix.SecPerFrame/100-1,8400-1);	//10Khz计数频率,加1是100us,用于视频帧间隔控制
			TIM3_Int_Init(10000-1,8400-1);					//10Khz计数,1秒钟中断一次,用于帧计数
			offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi");//寻找movi ID	 
			avi_get_streaminfo(pbuf+offset+4);			//获取流信息 
			f_lseek(videodev.file,offset+12);			//跳过标志ID,读地址偏移到流数据开始处	 
			res=mjpegdec_init((lcddev.width-avix.Width)/2,yoff+(videoheight-avix.Height)/2);//JPG解码初始化 
			if(avix.SampleRate)							//有音频信息,才初始化
			{
				WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,主机发送,时钟低电平有效,16位帧长度
				I2S2_SampleRate_Set(avix.SampleRate);	//设置采样率
				I2S2_TX_DMA_Init(videodev.i2sbuf[1],videodev.i2sbuf[2],avix.AudioBufSize/2); //配置DMA
				i2s_tx_callback=audio_i2s_dma_callback;	//回调函数指向I2S_DMA_Callback
				videodev.i2splaybuf=0;
				i2ssavebuf=0; 
				I2S_Play_Start(); //开启I2S播放 
			}
			gui_fill_rectangle(0,yoff,lcddev.width,videoheight,BLACK);	//清除视频区域
			video_info_upd(&videodev,videoprgb,volprgb,&avix,1);		//更新所有信息		
			tcnt=0;
			key=0;
			videodev.status=3;//非暂停,非快进快退
 			while(key==0)//播放循环
			{		
				if(videodev.status&(1<<0)&&videodev.status&(1<<1))
				{
					if(avix.StreamID==AVI_VIDS_FLAG)//视频流
					{
						pbuf=framebuf;
						f_read(videodev.file,pbuf,avix.StreamSize+8,&nr);//读入整帧+下一数据流ID信息  
						res=mjpegdec_decode(pbuf,avix.StreamSize);
						if(res)
						{
							printf("decode error!\r\n");
						} 
						while(aviframeup==0)delay_ms(1000/OS_TICKS_PER_SEC);//等待时间到达(在TIM6的中断里面设置为1)
						aviframeup=0;	//标志清零 
						framecnt++;
					}else 	//音频流
					{		  
						//video_time_show(videodev.file,&avix); 	//显示当前播放时间
						i2ssavebuf++;
						if(i2ssavebuf>3)i2ssavebuf=0;
						do
						{
							nr=videodev.i2splaybuf;
							if(nr)nr--;
							else nr=3; 
						}while(i2ssavebuf==nr);//碰撞等待. 
						f_read(videodev.file,videodev.i2sbuf[i2ssavebuf],avix.StreamSize+8,&nr);//填充videodev.i2sbuf	 
						pbuf=videodev.i2sbuf[i2ssavebuf];  
					}  
				}else	//暂停状态
				{
					delay_ms(1000/OS_TICKS_PER_SEC);
				}
				
				tp_dev.scan(0);    
				in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
				for(i=0;i<5;i++)
				{
					res=btn_check(tbtn[i],&in_obj);	 
					if((res&&((tbtn[i]->sta&(1<<7))==0)&&(tbtn[i]->sta&(1<<6)))||system_task_return)//有按键按下且松开,并且TP松开了或者TPAD返回
					{ 
						if(system_task_return)i=4;//TPAD返回
						switch(i)
						{
							case 0://file list
								key=0xff; 
								break;
							case 1://上一个视频
								if(videodev.curindex)videodev.curindex--;
								else videodev.curindex=videodev.mfilenum-1;
								key=1;
								break;
							case 3://下一个视频 
								if(videodev.curindex<(videodev.mfilenum-1))videodev.curindex++;
								else videodev.curindex=0;						
								key=3;
								break;
							case 2://播放/暂停
								if(videodev.status&(1<<0))//是暂停
								{ 
									videodev.status&=~(1<<0);	//标记暂停 
									I2S_Play_Stop();			//关闭音频
									tbtn[2]->picbtnpathd=(u8*)AUDIO_PLAYP_PIC; 
									tbtn[2]->picbtnpathu=(u8*)AUDIO_PAUSER_PIC;
								}else//暂停状态
								{
									videodev.status|=1<<0;		//取消暂停
									I2S_Play_Start();			//开启DMA播放 
									tbtn[2]->picbtnpathd=(u8*)AUDIO_PAUSEP_PIC; 
									tbtn[2]->picbtnpathu=(u8*)AUDIO_PLAYR_PIC;
								}
								break;
							case 4://停止播放,直接退出继续播放 
								key=4;  
								break; 
						}
					}  
				}				
				res=progressbar_check(volprgb,&in_obj);//检查音量进度条
				if(res&&lastvolpos!=volprgb->curpos)//被按下了,且位置变化了.执行音量调整
				{		  
					lastvolpos=volprgb->curpos;
					if(volprgb->curpos)wm8978set.mvol=volprgb->curpos;//设置音量
					else wm8978set.mvol=0;	  
					app_wm8978_volset(wm8978set.mvol);	 
					audio_show_vol((volprgb->curpos*100)/volprgb->totallen);	//显示音量百分比   
				}	  
				res=progressbar_check(videoprgb,&in_obj);
				if(res)//被按下了,并且松开了,执行快进快退
				{
					videodev.status&=~(1<<1);//标志进入快进快退状态
					if(videodev.status&1<<0)I2S_Play_Stop();//非暂停时,开始快进快退,关闭音频
					printf("videoprgb->curpos:%d\r\n",videoprgb->curpos);  
					video_seek(&videodev,&avix,framebuf,videoprgb->curpos);
					pbuf=framebuf;
					video_info_upd(&videodev,videoprgb,volprgb,&avix,0);//更新显示信息,每100ms执行一次	
				}else if((videodev.status&(1<<1))==0)//处于快进/快退状态?
				{
					if(videodev.status&1<<0)I2S_Play_Start();//非暂停状态,快进快退结束,打开音频
					videodev.status|=1<<1;	//取消快进快退状态
				}
				if(videodev.status&(1<<0)&&videodev.status&(1<<1))//非暂停状态和快进快退状态
				{
					if(avi_get_streaminfo(pbuf+avix.StreamSize))//读取下一帧 流标志,如果出错,可能是电影结束了
					{
						printf("frame error \r\n"); 
						break; 
					}
				}
				if((tcnt%20)==0)video_info_upd(&videodev,videoprgb,volprgb,&avix,0);//更新显示信息,每100ms执行一次			
			}
			I2S_Play_Stop();	//关闭音频
			TIM6->CR1&=~(1<<0); //关闭定时器6 
			TIM3->CR1&=~(1<<0); //关闭定时器3  
			mjpegdec_free();	//释放内存
			f_close(videodev.file); 
			if(key==0)
			{
				if(systemset.videomode==0)//顺序播放
				{
					if(videodev.curindex<(videodev.mfilenum-1))videodev.curindex++;
					else videodev.curindex=0;
				}else if(systemset.videomode==1)//随机播放
				{						    
					videodev.curindex=app_get_rand(videodev.mfilenum);//得到下一首歌曲的索引	  
				}else videodev.curindex=videodev.curindex;//单曲循环		 
			}else if(key==0XFF){res=0;break;}
			else if(key==4){res=0XFF;break;}
		}
	} 
	gui_memin_free(videoinfo.lfname);
	gui_memin_free(videodev.i2sbuf[0]);
	gui_memin_free(videodev.i2sbuf[1]);
	gui_memin_free(videodev.i2sbuf[2]);
	gui_memin_free(videodev.i2sbuf[3]);
	gui_memin_free(framebuf);
	gui_memin_free(videodev.file);
 	for(i=0;i<5;i++)if(tbtn[i])btn_delete(tbtn[i]);//删除按钮	
	if(videoprgb)progressbar_delete(videoprgb);
	if(volprgb)progressbar_delete(volprgb);
	WM8978_ADDA_Cfg(0,0);				//关闭DAC&ADC
	WM8978_Input_Cfg(0,0,0);			//关闭输入通道
	WM8978_Output_Cfg(0,0);				//关闭DAC输出   	
	app_wm8978_volset(0);				//关闭WM8978音量输出
	return res;
}
//avi文件查找
u8 video_seek(__videodev *videodevx,AVI_INFO *aviinfo,u8 *mbuf,u32 dstpos)
{
	u32 fpos=dstpos;
	u8 *pbuf;
	u16 offset;
	u32 br;  
	
	f_lseek(videodevx->file,fpos);///偏移到目标位置
	f_read(videodevx->file,mbuf,AVI_VIDEO_BUF_SIZE,&br);	//读入整帧+下一数据流ID信息 
	pbuf=mbuf; 
	if(fpos==0) //从0开始,得先寻找movi ID
	{
		offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi"); 
	}else offset=0;
	offset+=avi_srarch_id(pbuf+offset,AVI_VIDEO_BUF_SIZE,aviinfo->VideoFLAG);	//寻找视频帧	
	avi_get_streaminfo(pbuf+offset);			//获取流信息 
	f_lseek(videodevx->file,fpos+offset+8);		//跳过标志ID,读地址偏移到流数据开始处	 
	if(aviinfo->StreamID==AVI_VIDS_FLAG)
	{
		f_read(videodevx->file,mbuf,aviinfo->StreamSize+8,&br);	//读入整帧 
		mjpegdec_decode(mbuf,aviinfo->StreamSize); 		//显示视频帧
	} 
	return 0;
}






















