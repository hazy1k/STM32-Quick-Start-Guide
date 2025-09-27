/*
******************************************************************************
* @file    Backend_Recorder.c
* @author  fire
* @version V1.0
* @date    2019-xx-xx
* @brief   录音功能测试
******************************************************************************
* @attention
* 
* 实验平台:野火 STM32 F103 V2 霸道 开发板  
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/

#include "bsp/key/bsp_key.h" 
#include "ff.h" 
#include "Backend_Recorder.h"
#include "GUI_RECORDER_DIALOG.h"
#include  "VS1053.h"

extern FIL	AF_File_Handle;																							//文件句柄
extern FRESULT result;																				//文件系统API函数返回值
extern UINT bw;																								//已读或已写的字节数

uint8_t Replay=0, RecordFlag=0;
static TCHAR recfilename[25]={"1:/recorder/rec001.wav"};
static __WaveHeader rechead;
uint8_t AF_Buf_REC_MUS[BUFSIZE];

static char tempstr[12]={"rec001.wav"};
_recorder_obj recset;
extern uint8_t chgsch;      // 调整进度条标志位
extern int  power_rec;    // 音量值

void recorder(const TCHAR *recfile)
{
	uint32_t sectorsize=0;
	uint16_t recsec=0;					//录音时间
	uint16_t idx=0;
	uint16_t regval=0;
	uint16_t i;	
	i=0;
	while(i<BUFSIZE)   
	{
		AF_Buf_REC_MUS[i]='\0';
		++i;		
	}


	recoder_enter_rec_mode(&recset);
	
	while(VS_RD_Reg(SPI_HDAT1)>>8);			//等到buf 较为空闲再开始

	rechead.riff.ChunkID=0X46464952;	//"RIFF"
	rechead.riff.ChunkSize=0;			//还未确定,最后需要计算
	rechead.riff.Format=0X45564157; 	//"WAVE"
	rechead.fmt.ChunkID=0X20746D66; 	//"fmt "
	rechead.fmt.ChunkSize=16; 			//大小为16个字节
	rechead.fmt.AudioFormat=0X01; 		//0X01,表示PCM;0X01,表示IMA ADPCM
	rechead.fmt.NumOfChannels=1;		//单声道	
 	rechead.fmt.SampleRate=recset.samplerate*8000;		//采样速率
 	rechead.fmt.ByteRate=rechead.fmt.SampleRate*2;//16位,即2个字节
 	rechead.fmt.BlockAlign=2;			//块大小,2个字节为一个块
 	rechead.fmt.BitsPerSample=16;		//16位PCM
  rechead.data.ChunkID=0X61746164;	//"data"
 	rechead.data.ChunkSize=0;			//数据大小,还需要计算  

	result=f_open(&AF_File_Handle,recfile, FA_CREATE_ALWAYS | FA_WRITE); 
	if(result != FR_OK)
		return;
	else
		result=f_write(&AF_File_Handle,&rechead,sizeof(__WaveHeader),&bw);//写入头数据
	//printf("write result:%d,num:%d\n",result,bw);
	while(1)
	{
		if(RecordFlag==2)
		{
			rechead.riff.ChunkSize=sectorsize*BUFSIZE+36;	//整个文件的大小-8;
			rechead.data.ChunkSize=sectorsize*BUFSIZE;		//数据大小
			f_lseek(&AF_File_Handle,0);							//偏移到文件头.
			result=f_write(&AF_File_Handle,&rechead,sizeof(__WaveHeader),&bw);//写入头数据
      if(result)
			{
				printf("err:%d\n",result);  
			}
			//printf("%s->write sector:%d,result:%d,num:%d\n",recfile,sectorsize,result,bw);
			f_close(&AF_File_Handle);
			VS_HD_Reset();		   	//硬复位
			VS_Soft_Reset();  		//软复位 
			sectorsize=0;
			RecordFlag=0;
			return;//break;
		}
		regval=VS_RD_Reg(SPI_HDAT1);	
		if((regval>=256)&&(regval<896))
		{
			idx=0;
			while(idx<BUFSIZE) 	//一次读取BUFSIZE字节
			{	 
				regval=VS_RD_Reg(SPI_HDAT0);
				AF_Buf_REC_MUS[idx]=regval&0XFF;
				AF_Buf_REC_MUS[idx+1]=regval>>8;
				idx+=2;
			}
			f_lseek(&AF_File_Handle,44+sectorsize*BUFSIZE);
			result=f_write(&AF_File_Handle,AF_Buf_REC_MUS,BUFSIZE,&bw);//写入文件
			if(result)
			{
				printf("err:%d bw:%d\n",result,bw);
				break;//写入出错.	  
			}
			sectorsize++;//扇区数增加1
		}
//		if(recsec!=(sectorsize*4/125/recset.samplerate))//录音时间显示
//		{
//			recsec=sectorsize*4/125/recset.samplerate;
//			sprintf(tempstr,"%02d:%02d",recsec/60,recsec%60);
//			tempstr[5]='\0';
//			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),tempstr);
//		}
//		WM_Exec();//GUI_Delay(1);//
	}
	//f_close(&sdfile);
}

void record_play(uint8_t *recfile,HWND hWin)
{
	uint16_t i=0,time=0,oldtime=0;
	uint16_t f_k=0xffff;    // 波形数据传输速率（每秒平均字节数）
  WCHAR wbuf[10];
	//if(strstr((const char *)recfile,"recorder")==0)return 0xff;
	i=0;
	while(i<BUFSIZE)
	{
		AF_Buf_REC_MUS[i]='\0';
		++i;		
	}
  Replay = 1;
	VS_Restart_Play();  					//重启播放 
	VS_Set_All();        	//设置音量等信息
  VS_Set_Vol(power_rec);     // 设置vs1053的音量
	VS_Reset_DecodeTime();	//复位解码时间 	 
	result=f_open(&AF_File_Handle,(const TCHAR*)recfile,FA_READ);
	if(result==0)
	{ 
			VS_SPI_SpeedHigh();		
    
			while(Replay)
			{
				i=0;	
				result=f_read(&AF_File_Handle,AF_Buf_REC_MUS,BUFSIZE,(UINT*)&bw);	
				if(f_k==0xffff)
				{
//          f_kbps=(uint16_t)((recbuf[25]*256+recbuf[24])*32/1000);
//          time = f_size(&file)/f_kbps/125;
          f_k=((((AF_Buf_REC_MUS[25] << 8) | AF_Buf_REC_MUS[24]) * AF_Buf_REC_MUS[22] * 16)/8);
          time = (f_size(&AF_File_Handle) - sizeof(rechead))/f_k;
          x_wsprintf(wbuf, L"%02d:%02d", time/60, time%60);
          SetWindowText(GetDlgItem(hWin, ID_PLAY_TOTAL_TIME), wbuf);
          time = time ? time : 1;
          SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_SETRANGE, 0, time);
          time = 0;
				}
//				OSSchedLock(&err);
				while(i<bw)
			  {  	
					if(VS_Send_MusicData(AF_Buf_REC_MUS+i)==0)
					{
						i+=32;
					}
				}
//				OSSchedUnlock(&err);
        if(result != FR_OK)
        {
          printf("读文件出错-->(%d)\r\n", result);
          break;
        }
          
				if((bw!=BUFSIZE))
				{		
          printf("播放完\r\n");
					break;
				}
				GUI_msleep(1);
        
        if (chgsch == 0)
        {
          oldtime = (f_tell(&AF_File_Handle) - sizeof(rechead))/f_k;
          if((time+1)<=oldtime)
          {
            time=oldtime;
            x_wsprintf(wbuf, L"%02d:%02d", time/60, time%60);
            SetWindowText(GetDlgItem(hWin, ID_PLAY_TIME), wbuf);
            SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_SETVALUE, TRUE, time);
          }
        }
        else if (chgsch == 1)
        {
           uint8_t temp=0;
           uint32_t pos;
          
           temp=SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_GETVALUE, NULL, NULL);  
           pos = temp*f_k;
//           pos=file.fsize/255*temp;
           if(pos<sizeof(rechead))
             pos=sizeof(rechead);
           if(rechead.fmt.BitsPerSample==24)
             temp=12;
           else 
             temp=8;
           if((pos-sizeof(rechead))%temp)
           {
             pos+=temp-(pos-sizeof(rechead))%temp;
           }
           f_lseek(&AF_File_Handle,pos);
           oldtime=0;
           time = 0;
           chgsch=0;         
         }
			}
		}

    SetWindowText(GetDlgItem(hWin, ID_PLAY_TIME), L"00:00");
    SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_SETVALUE, TRUE, 0);
    GUI_msleep(2);
		f_close(&AF_File_Handle);
}

//波形数据传输速率（每秒平均字节数） = 采样频率 × 音频通道数 × 每次采样得到的样本位数 / 8

//比特率（kbps） = 波形数据传输速率 × 8 / 1000

//WAV文件所占大小（字节) = 波形数据传输速率 × 音频文件时长

//音频文件时长（秒） = WAV文件所占容量 / 波形数据传输速率

/***************************** (END OF FILE) *********************************/
