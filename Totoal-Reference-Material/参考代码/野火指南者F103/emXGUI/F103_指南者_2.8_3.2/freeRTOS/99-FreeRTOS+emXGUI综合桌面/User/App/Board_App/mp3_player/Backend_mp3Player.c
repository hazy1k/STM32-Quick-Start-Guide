/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   VS1053放音功能测试
******************************************************************************
* @attention
*
* 实验平台:野火  STM32 F103 开发板  
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include  "./mp3_player/vs1053/VS1053.h"
#include "ff.h" 
#include "./mp3_player/Backend_mp3Player.h"
#include "./mp3_player/Backend_musiclist.h"
#include "emXGUI.h"
#include "x_libc.h"
#include "flac.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include "./dwt_delay/core_delay.h" 

//#define Delay_ms GUI_msleep
void FileSystem_Test(void);
RECT rc_cli = {45, 183, 230, 20};//进度条
Music_State_t Music_State;

FRESULT result;
extern FIL AF_File_Handle;
UINT bw;
uint32_t File_HeaderTime=0;//歌曲时间偏移
extern uint8_t AF_Buf_REC_MUS[BUFSIZE];

void vs1053_player(uint8_t *filepath, uint8_t vol, HDC hdc)
{
	uint16_t playtime=0,time=0;
	static uint8_t lyriccount=0;
	uint16_t f_kbps=0xffff;
	uint16_t i=0;
	uint32_t val=0;
//  char lyrictext[MAX_LINE_LEN]={0};
  Music_State=STA_PLAYING;
	uint8_t type=0;
  WCHAR wbuf[128];
	
	printf("song:%s\n",filepath);
	VS_Restart_Play();  					// 重启播放 
	VS_Set_All();        					// 设置音量等信息 
  VS_Set_Vol(vol);              // 设置音量
	VS_Reset_DecodeTime();				// 复位解码时间 

  if (music_lcdlist[play_index][0] == '\0')
  {
    SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), L"音乐播放器");
  }
  else
  {
    x_mbstowcs(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
    SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), wbuf);
  }
  
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
	{
//		VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
		GUI_msleep(1);//1ms
		type=2;
	}
	else	if(strstr((const char*)filepath,".wav")||strstr((const char*)filepath,".WAV"))
	{
		type=3;
	}
	else//if(strstr((const char*)filepath,".mp3")||strstr((const char*)filepath,".mp3"))
	{
		type=1;
	}
	
//	SLIDER_SetValue(hSchedule,0);
	lyriccount=0;
	lrc.oldtime=lrc.time_tbl[0];
	
	result=f_open(&AF_File_Handle,(const TCHAR*)filepath, FA_OPEN_EXISTING | FA_READ);//打开文件	
	
	if(result!=FR_OK) return;
//FileSystem_Test();
	VS_SPI_SpeedHigh();	//高速						   
	while(Music_State==STA_PLAYING)
	{
		if (Music_State==STA_IDLE)
		{			
			GUI_msleep(5);
			continue;											//暂停的时候跳出本循环
		}		
		if(chgsch==1&&((VS_RD_Reg(SPI_STATUS)&0x8000)==0))//快进、快退
		{     
			uint32_t sch=0;
			uint16_t numb=0;
			uint8_t 	dat[32]={0};
			VS_Set_Vol(10);
			numb=VS_Get_EndFillByte() & 0xff;
			for(i=0;i<32;i++)
        dat[i]=numb;
      
			for(i=0;i<2048+32;)
			{
				if(VS_Send_MusicData(dat)==0)
        {
          i+=32;
        }
			}
      //根据进度条调整播放位置
      numb=SendMessage(music_wnd_time, SBM_GETVALUE, NULL, NULL);     //获取当前进度条的位置
			sch = numb*AF_File_Handle.fsize/0xff;
//			printf("chgsch:%d,sch:%d\n",chgsch,sch);
			f_lseek(&AF_File_Handle,sch);
			VS_Set_Vol(vol);
			chgsch=0;
      lrc.oldtime=0;
      lyriccount=0;
		}
		
		result=f_read(&AF_File_Handle,AF_Buf_REC_MUS,BUFSIZE,(UINT*)&bw);//读出2048个字节	
		i=0;
    /* 进入临界段，临界段可以嵌套 */
//    taskENTER_CRITICAL();
		while(i<bw)
		{
			if(VS_Send_MusicData(AF_Buf_REC_MUS+i)==0)//给VS10XX发送音频数据
			{
				i+=32;
			}
		}
//    taskEXIT_CRITICAL();	
		
		if(bw!=BUFSIZE||result!=FR_OK)
		{
      printf("%s（%d）", result != FR_OK ? "读失败" : "放完了", result);
			VS_Soft_Reset();
			GUI_msleep(10);
			if(play_index<music_file_num-1)		//自动开始下一首歌曲
				play_index++;
			else
				play_index = 0;
			Music_State = STA_SWITCH;
			f_close(&AF_File_Handle);
      SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, 0);    // 读完或出错重新设置当前值
			printf("song end!!\n");
			return;//读完了.		  
		}
		GUI_msleep(1);
		if((Music_State == STA_SWITCH))break;

		if(f_kbps==0xffff)//只运行一次
		{
			uint8_t tmp[4]={0};
			uint32_t dattmp=0;
			uint16_t time=0;
			switch (type)
			{
				case 2://flac
					tmp[0]=AF_Buf_REC_MUS[22];//定位到flac文件总采样数存放的确实位置
					tmp[1]=AF_Buf_REC_MUS[23];//总采样数本身是以36位存储的，但我们只用了32位，一般文件不用这么大数据
					tmp[2]=AF_Buf_REC_MUS[24];//这里使用绝对定位到存储地址，可能部分文件会有偏差，为精确使用可使用查找定位
					tmp[3]=AF_Buf_REC_MUS[25];
					dattmp=(tmp[0]<<24)+(tmp[1]<<16)+(tmp[2]<<8)+tmp[3];//flac文件总采样数
					time=dattmp/44100;//flac文件总播放时间
					f_kbps=AF_File_Handle.fsize/125/time;//flac文件比特率
					break;
				case 3://wav
					tmp[0]=AF_Buf_REC_MUS[24];//绝对定位到wav文件存储采样频率地址
					tmp[1]=AF_Buf_REC_MUS[25];				
					dattmp=(tmp[1]*256+tmp[0])*32/1000;				
					f_kbps=(uint16_t)dattmp;
					//printf("tmp:%02X %02X,dattmp:%d,f_kbps:%d\n",tmp[0],tmp[1],dattmp,f_kbps);
					break;
				default:
					f_kbps=VS_Get_HeadInfo();
					break;
			}	

        //找不到歌词文件
        if (lrc.flag == 0)
        {
           
           SetWindowText(wnd_lrc3,L"请在SDCard放入相应的歌词文件(*.lrc)");
           SetWindowText(wnd_lrc1,L"");
           SetWindowText(wnd_lrc2,L"");
           SetWindowText(wnd_lrc4,L"");
           SetWindowText(wnd_lrc5,L"");
        }
		}
		time++;
		if((time>=(25*type+playtime))&&(chgsch==0))
		{
			if(!File_HeaderTime)
			{
				File_HeaderTime=f_tell(&AF_File_Handle)/f_kbps/125;//偏移文件头部分的时间
			}
			playtime=time;	
			if((type==1)&&((f_kbps>448)||(f_kbps<32)))f_kbps=VS_Get_HeadInfo();
			val=AF_File_Handle.fsize/f_kbps/125-File_HeaderTime;
			x_wsprintf(wbuf, L"%02d:%02d", val/60,val%60);
      SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB1), wbuf);
			
			lrc.curtime=f_tell(&AF_File_Handle)/f_kbps/125-File_HeaderTime;
			
			x_wsprintf(wbuf, L"%02d:%02d", lrc.curtime/60,lrc.curtime%60);
      SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB2), wbuf);
      SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, lrc.curtime*255/val);
			
//			musicrate=lrc.curtime*255/val;

      if(lrc.flag == 1)
      {
        //+100是提前显示，显示需要消耗一点时间
        if((lrc.oldtime <= lrc.curtime*100)&&(lrc.indexsize>7))//+100
        {
          //显示当前行的歌词
          x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount]-1], LYRIC_MAX_SIZE);
          SetWindowText(wnd_lrc3,wbuf);
          //显示第i-1行的歌词（前一行）
          if(lyriccount>0)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-1]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc2,wbuf);
          }
          else
             SetWindowText(wnd_lrc2,L" ");
          //显示第i-2行的歌词（前两行）
          if(lyriccount>0)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-2]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc1,wbuf);
          }
          else
             SetWindowText(wnd_lrc1,L" ");
          //显示第i+1行的歌词（后一行）   
          if(lyriccount < lrc.indexsize-1)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+1]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc4,wbuf);                    
          }
          else
             SetWindowText(wnd_lrc4,L" ");
          //显示第i+2行的歌词（后二行）   
          if(lyriccount < lrc.indexsize-2)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+2]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc5,wbuf);                    
          }
          else
             SetWindowText(wnd_lrc5,L" ");
                    
          do{
          lyriccount++;					
          if(lyriccount>=lrc.indexsize)
          {
             lrc.oldtime=0xffffff;
             break;
          }
          lrc.oldtime=lrc.time_tbl[lyriccount];
          }while(lrc.oldtime<=(lrc.curtime*100));
        }                  
             
        }
        
		}
	}
  
	lyriccount=0;
	f_close(&AF_File_Handle);
  if(time2exit == 1)
  {
    f_close(&AF_File_Handle);	 
    GUI_SemPost(exit_sem);
  }
	File_HeaderTime = 0;
}

/***************************** (END OF FILE) *********************************/
