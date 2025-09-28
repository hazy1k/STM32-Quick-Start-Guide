#include "exeplay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-运行器 代码	   
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

//假函数,让PC指针跑到新的main函数去
dummyfun jump2app;		
//代码存放地址
extern u8 mem2base[MEM2_MAX_SIZE];	

//执行BIN文件前的提示
u8*const exeplay_remindmsg_tbl[GUI_LANGUAGE_NUM]=
{
"  执行BIN文件后,系统将无法继续运行,只能复位重启.您确认执行么?",
"  執行BIN文件后,系統將無法繼續運行,只能復位重啟.您確認執行么?",
"  After run the BIN file,SYSTEM will unable to continue.Confirm?",
};
//警告
u8*const exeplay_warning_tbl[GUI_LANGUAGE_NUM]=
{
"警告:",
"警告:",
"Warning:",
};
//APP程序非法提示语
u8*const exeplay_apperrmsg_tbl[GUI_LANGUAGE_NUM]=
{					   							    
"非法的SRAM APP程序！","非法的SRAM APP程序！","Illegal SRAM APP File!",	 
};	  
  
//写入标志值
//val:标志值
void exeplay_write_appmask(u16 val)
{
	RCC->APB1ENR|=1<<28;		//使能电源接口时钟
	PWR->CR|=1<<8;				//后备区域访问使能(RTC+SRAM)
  	RTC_WriteBackupRegister(RTC_BKP_DR1,val);
}
//在主函数最开始的时候被调用.
//检测是否有app程序需要执行.如果是,则直接执行.
void exeplay_app_check(void)
{
	if(RTC_ReadBackupRegister(RTC_BKP_DR1)==0X5050)//检查BKP1,如果为0X5050,则说明需要执行app代码
	{
		exeplay_write_appmask(0X0000);	//写入0,防止复位后再次执行app代码.
		FSMC_SRAM_Init();				//初始化SRAM,因为需要从外部sram拷贝数据到内部sram
		mymemcpy((u8*)EXEPLAY_APP_BASE,(u8*)EXEPLAY_SRC_BASE,EXEPLAY_APP_SIZE);//拷贝EXEPLAY_APP_SIZE字节 
		jump2app=(dummyfun)*(vu32*)(EXEPLAY_APP_BASE+4);	//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)EXEPLAY_APP_BASE);			 		//初始化APP堆栈指针(用户代码区的第一个字用于存放堆栈地址)
		jump2app();	  										//执行app代码.
	}
}  
//运行器   
u8 exe_play(void)
{
  	FIL* f_exe;		 
	DIR exeplaydir;		//exeplaydir专用	  
	FILINFO exeplayinfo;	 	   
	u8 res;
	u8 rval=0;			//返回值	  
	u8 *pname=0;
	u8 *fn;
  
 	_btn_obj* rbtn;		//返回按钮控件
   	_filelistbox_obj * flistbox;
	_filelistbox_list * filelistx; 	//文件

	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[8][gui_phy.language],0X07);//显示信息
   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
 	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		flistbox->fliter=FLBOX_FLT_BIN;	//查找BIN文件
		filelistbox_add_disk(flistbox);	//添加磁盘路径
 		filelistbox_draw_listbox(flistbox);
	} 	 

	//为长文件名申请缓存区
 	exeplayinfo.lfsize = _MAX_LFN * 2 + 1;
	exeplayinfo.lfname = gui_memin_malloc(exeplayinfo.lfsize);
	if(exeplayinfo.lfname==NULL)rval=1;//申请内存失败 
   	else gui_memset((u8*)exeplayinfo.lfname,0,exeplayinfo.lfsize);
	f_exe=(FIL *)gui_memin_malloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_exe==NULL)rval=1;//申请失败

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
	
   	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5);
 		if(system_task_return)break;			//TPAD返回
		filelistbox_check(flistbox,&in_obj);	//扫描文件
		res=btn_check(rbtn,&in_obj);
		if(res)
		{
			if(((rbtn->sta&0X80)==0))//按钮状态改变了
			{
				if(flistbox->dbclick!=0X81)
				{
 					filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
					if(filelistx->type==FICO_DISK)//已经不能再往上了
					{				 
						break;
					}else filelistbox_back(flistbox);//退回上一层目录	 
				} 
 			}	 
		}
		if(flistbox->dbclick==0X81)//双击文件了
		{	    
     		rval=f_opendir(&exeplaydir,(const TCHAR*)flistbox->path); //打开选中的目录
			if(rval)break;	 			   
			dir_sdi(&exeplaydir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
	 		rval=f_readdir(&exeplaydir,&exeplayinfo);//读取文件信息
			if(rval)break;//打开成功    
			fn=(u8*)(*exeplayinfo.lfname?exeplayinfo.lfname:exeplayinfo.fname);
			pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//申请内存
			if(pname==NULL)rval=1;//申请失败
			{
				pname=gui_path_name(pname,flistbox->path,fn);	//文件名加入路径  
				rval=f_open(f_exe,(const TCHAR*)pname,FA_READ);	//只读方式打开文件
				if(rval)break;	//打开失败   
				if(f_exe->fsize<MEM2_MAX_SIZE)//可以放的下,并且用户确定执行
				{  
  					gui_memset(mem2base,0,f_exe->fsize);	//清空这片内存
 	 				rval=f_read(f_exe,mem2base,f_exe->fsize,(UINT*)&br);//读出BIN的所有内容       
  					if(rval)break;										//打开失败,直接退出   
					if(((*(vu32*)(mem2base+4))&0xFF000000)==0x20000000)//判断是否为0X20XXXXXX.检测APP的合法性。
					{
	  					res=window_msg_box((lcddev.width-200)/2,(lcddev.height-160)/2,200,160,(u8*)exeplay_remindmsg_tbl[gui_phy.language],(u8*)exeplay_warning_tbl[gui_phy.language],16,0,0X03,0);//显示提醒信息
						if(res==1)//按下了确认键
						{
							exeplay_write_appmask(0X5050);	//写入标志字,标志有app需要运行
							Sys_Soft_Reset();				//产生一次软复位
						}
					}else//非法APP文件 
					{
						window_msg_box((lcddev.width-160)/2,(lcddev.height-80)/2,160,80,(u8*)exeplay_apperrmsg_tbl[gui_phy.language],(u8*)exeplay_warning_tbl[gui_phy.language],12,0,0,0);//提示非法的APP文件
						delay_ms(1500);//延时1.5秒
					}
					filelistbox_rebuild_filelist(flistbox);//重建flistbox
				} 
			} 
			flistbox->dbclick=0;	//设置非文件浏览状态
 			gui_memin_free(pname);	//释放内存		  
		}
	}	
	filelistbox_delete(flistbox);	//删除filelist
	btn_delete(rbtn);				//删除按钮	 	  
 	gui_memin_free(pname);			//释放内存		  
 	gui_memin_free(exeplayinfo.lfname);
	gui_memin_free(f_exe);					   
	return rval;   
}















