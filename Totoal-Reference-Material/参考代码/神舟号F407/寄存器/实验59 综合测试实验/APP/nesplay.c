#include "nesplay.h"
#include "nes_main.h"	
#include "usb_app.h"	
#include "spb.h"	  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-NES模拟器 代码	   
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
  
u8 *rom_file;
u8*const nes_caption_tbl[GUI_LANGUAGE_NUM]={"NES模拟器","NES模擬器","NES Emulator",};
u8*const nes_remindmsg_tbl[4][GUI_LANGUAGE_NUM]=
{
{"请先拔掉U盘...","請先拔掉U盤...","Please pull U disk first...",},
{"请插入USB手柄/键盘!","請插入USB手柄/鍵盤!","Please plug USB gamepad/keyboard!",},
{"检测到键盘!","檢測到鍵盤!","Keyboard detected!",},
{"检测到游戏手柄!","檢測到遊戲手柄!","Gamepad detected!",}, 
}; 
//错误提示
u8*const nes_errormsg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"内存不够!","內存不夠!","Out of memory!",},
{"文件读取错误!","文件讀取錯誤!","Read file error!",},
{"MAP不支持!","MAP不支持!","Not supported MAP!",},
};

//加载游戏界面
void nes_load_ui(void)
{	
	app_filebrower((u8*)nes_caption_tbl[gui_phy.language],0X05);//显示标题 
  	gui_fill_rectangle(0,20,lcddev.width,lcddev.height-20,BLACK);//填充底色 		    	  
} 
//NES游戏
u8 nes_play(void)
{
  	DIR nesdir;		//nesdir专用	  
	FILINFO nesinfo;	 	   
	u8 res;
	u8 rval=0;			//返回值	  
	u8 *pname=0;
	u8 *fn;	
	u8 remindflag=0;
  
 	_btn_obj* rbtn;		//返回按钮控件
   	_filelistbox_obj * flistbox;
	_filelistbox_list * filelistx; 	//文件

	//如果U盘在位,提示要拔掉U盘
	if(gui_phy.memdevflag&(1<<2))
	{
		window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)nes_remindmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		while(gui_phy.memdevflag&(1<<2))
		{
			delay_ms(5);//死循环等待U盘被拔出
		}
	}
	usbapp_mode_set(USBH_HID_MODE);	//设置USB为HID模式
	LCD_Clear(BLACK);
	//提示插入手柄/键盘
	window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)nes_remindmsg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	delay_ms(1000);
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[6][gui_phy.language],0X07);	//选择目标文件,并得到目标数量
   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
 	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		flistbox->fliter=FLBOX_FLT_NES;	//查找文本文件
		filelistbox_add_disk(flistbox);	//添加磁盘路径
		filelistbox_draw_listbox(flistbox);
	}
	//为长文件名申请缓存区
 	nesinfo.lfsize=_MAX_LFN*2+1;
	nesinfo.lfname=gui_memin_malloc(nesinfo.lfsize);
	if(nesinfo.lfname==NULL)rval=1;//申请内存失败 
   	else gui_memset((u8*)nesinfo.lfname,0,nesinfo.lfsize); 
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
		if(system_task_return)break;	  		//TPAD返回
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
     		rval=f_opendir(&nesdir,(const TCHAR*)flistbox->path); //打开选中的目录
			if(rval)break;	 			   
			dir_sdi(&nesdir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
	 		rval=f_readdir(&nesdir,&nesinfo);//读取文件信息
			if(rval)break;//打开成功    
			fn=(u8*)(*nesinfo.lfname?nesinfo.lfname:nesinfo.fname);
			pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//申请内存
			if(pname==NULL)rval=1;//申请失败
			else
			{
				pname=gui_path_name(pname,flistbox->path,fn);//文件名加入路径   
				LCD_Clear(BLACK); 
				app_filebrower(fn,0X05);//显示当前正在玩的游戏名字
				res=nes_load(pname);	//开始nes游戏
				if(res)
				{
					window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)nes_errormsg_tbl[res-1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
					delay_ms(1200); 
				}
				system_task_return=0;	//退出标志清零
  			}	       
			flistbox->dbclick=0;	//设置非文件浏览状态
			gui_memin_free(pname);	//释放内存		  
		    pname=NULL;
			app_filebrower((u8*)nes_caption_tbl[gui_phy.language],0X07);//选择目标文件,并得到目标数量
			btn_draw(rbtn);//画按钮
			delay_ms(100);
			filelistbox_rebuild_filelist(flistbox);//重建flistbox
 			system_task_return=0;//刚刚退出正在玩的游戏,还不能退出这个循环
		}
		while((usbx.bDeviceState&0XC0)==0X40)//USB设备插入了,但是还没连接成功,猛查询.
		{
			usbapp_pulling();	//轮询处理USB事务
		}
		usbapp_pulling();		//轮询处理USB事务
		if(remindflag==0)
		{
			if(usbx.hdevclass==3||usbx.hdevclass==4)
			{
				window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)nes_remindmsg_tbl[usbx.hdevclass-1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
				delay_ms(800);
				filelistbox_rebuild_filelist(flistbox);//重建flistbox
				remindflag=1;
			}
		}
		if(usbx.bDeviceState==0)remindflag=0;
	}	
	filelistbox_delete(flistbox);	//删除filelist
	btn_delete(rbtn);				//删除按钮	  	 
	gui_memin_free(pname);			//释放内存		  
 	gui_memin_free(nesinfo.lfname);		 
	usbapp_mode_set(USBH_MSC_MODE);	//恢复USB为USBH MSC模式
	return rval;  								  
}






















