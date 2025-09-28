#include "ebook.h"
#include "spb.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-电子图书实现 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/2/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//提醒内容
u8*const ebook_remind_msg_tbl[GUI_LANGUAGE_NUM]=
{
"内存不足!","內存不足!","Out of memory!",	 
};
//在setings.c里面定义
extern u8*const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM];
//电子书功能
u8 ebook_play(void)
{
	_memo_obj * tmemo=0;	    
	FIL* f_txt;		 
	DIR ebookdir;		//ebookdir专用	  
	FILINFO ebookinfo;	 	   
	u8 res;
	u8 rval=0;			//返回值	  
	u8 *pname=0;
	u8 *fn;
	u8 errtype=0;
	u8 ebooksta=0;		//ebook状态
						//0,属于文件浏览状态    
						//1,已经开始文件阅读
 	u8 uiflag=0;		//标记是否需要重新更新主界面,0,不需要;1需要.
	u32 temp;			//临时变量
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
 	_btn_obj* rbtn;		//返回按钮控件
   	_filelistbox_obj * flistbox;
	_filelistbox_list * filelistx; 	//文件 
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[0][gui_phy.language],0X07);//选择目标文件,并得到目标数量
   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
 	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		flistbox->fliter=FLBOX_FLT_TEXT|FLBOX_FLT_LRC;	//查找文本文件
		filelistbox_add_disk(flistbox);					//添加磁盘路径
 		filelistbox_draw_listbox(flistbox);
	} 	  
	//为长文件名申请缓存区
 	ebookinfo.lfsize = _MAX_LFN * 2 + 1;
	ebookinfo.lfname = gui_memin_malloc(ebookinfo.lfsize);
	if(ebookinfo.lfname==NULL)rval=1;//申请内存失败 
   	else gui_memset((u8*)ebookinfo.lfname,0,ebookinfo.lfsize);
	f_txt=(FIL *)gui_memin_malloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_txt==NULL)rval=1;//申请失败 
	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
 	if(rbtn==NULL)rval=1;	//没有足够内存够分配
	else
	{																				
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);//画按钮
	}
	if(rval)errtype=1;//内存不足 
   	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5);
		if(system_task_return)	 				//TPAD返回
		{
			if(ebooksta)						//文件浏览状态,先退出文件浏览
			{			  
				memo_delete(tmemo);		//释放内存  
				tmemo=NULL;
				gui_memin_free(pname);	//释放内存		  
			    pname=NULL;
			    ebooksta=0;				//回到文件浏览状态
				app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[0][gui_phy.language],0X07);	//选择目标文件,并得到目标数量
				btn_draw(rbtn);			//画按钮
				filelistbox_rebuild_filelist(flistbox);//重建flistbox
				system_task_return=0;
			}else break;	  		
		}
		if(ebooksta==0)							//还未开始文件浏览
		{
			filelistbox_check(flistbox,&in_obj);//扫描文件
			res=btn_check(rbtn,&in_obj);		//扫描返回按键
			if(res)
			{
				if(((rbtn->sta&0X80)==0))//按钮状态改变了
				{
 					filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
					if(filelistx->type==FICO_DISK)//已经不能再往上了
					{				 
						break;
					}else filelistbox_back(flistbox);//退回上一层目录	 
	 			}	 
			}
			if(flistbox->dbclick==0X81)//双击文件了
			{
	     		rval=f_opendir(&ebookdir,(const TCHAR*)flistbox->path); //打开选中的目录
				if(rval)break;
				ff_enter(ebookdir.fs);//进入fatfs,防止被打断.
				dir_sdi(&ebookdir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
		 		ff_leave(ebookdir.fs);//退出fatfs,继续运行os等
				rval=f_readdir(&ebookdir,&ebookinfo);//读取文件信息
				if(rval)break;//打开成功    
				fn=(u8*)(*ebookinfo.lfname?ebookinfo.lfname:ebookinfo.fname);
				pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//申请内存
				if(pname==NULL)rval=1;//申请失败
				else
				{
					app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);//下分界线
					gui_show_string(fn,2,(gui_phy.tbheight-gui_phy.tbfsize)/2,lcddev.width-2,gui_phy.tbfsize,gui_phy.tbfsize,WHITE);			//显示文件名
					pname=gui_path_name(pname,flistbox->path,fn);	//文件名加入路径  
					rval=f_open(f_txt,(const TCHAR*)pname,FA_READ);	//只读方式打开文件
					if(rval)break;//打开成功   
					temp=my_mem_perused(SRAMEX);
					temp=(MEM2_MAX_SIZE*(100-temp))/100;//得到外部内存剩余内存总数
					if((f_txt->fsize+1)>temp)
					{
						spb_delete();//释放内存
						uiflag=1;				
					} 
					tmemo=memo_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight,0,1,gui_phy.tbfsize,f_txt->fsize+1);//申请整个txt这么多的内存加一个.	
					if(tmemo==NULL)
					{
						errtype=1;//内存不足
						break;
					}
	 				rval=f_read(f_txt,tmemo->text,f_txt->fsize,(UINT*)&br);	//读出txt里面的内容  
					if(rval)break;//打开成功      
					memo_draw_memo(tmemo,0);
				}
				flistbox->dbclick=0;//标记已处理
				ebooksta=1;
			}
		}
		if(ebooksta==1)//处于文本阅读状态.
		{
			memo_check(tmemo,&in_obj); 
		}
	}
	if(errtype)//需要错误提示
	{
		window_msg_box((lcddev.width-160)/2,(lcddev.height-70)/2-15,160,70,(u8*)ebook_remind_msg_tbl[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);		
 		delay_ms(500);
	}
	filelistbox_delete(flistbox);	//删除filelist
	btn_delete(rbtn);				//删除按钮	 	  
	memo_delete(tmemo);				//释放内存  	 
	gui_memin_free(pname);			//释放内存		  
 	gui_memin_free(ebookinfo.lfname);
	gui_memin_free(f_txt);					   
	if(uiflag)
	{
		LCD_Clear(BLACK);				//黑屏
		window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sysset_remindmsg_tbl[1][gui_phy.language],(u8*)sysset_remindmsg_tbl[0][gui_phy.language],12,0,0,0);
		spb_init();						//重新初始化SPB
	}
	return rval;   
}















