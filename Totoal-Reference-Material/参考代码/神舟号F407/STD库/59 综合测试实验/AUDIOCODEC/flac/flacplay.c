#include "flacplay.h"
#include "i2s.h"
#include "wm8978.h"
#include "malloc.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "audioplay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自RockBox的flac解码库
//ALIENTEK STM32F407开发板
//FLAC 解码代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/29
//版本：V1.0
//********************************************************************************
//V1.0 说明
//1,支持16/24位单声道/立体声flac的解码
//2,最高支持192K/16bit或96K/24bit的flac解码  
////////////////////////////////////////////////////////////////////////////////// 	 
 
__flacctrl * flacctrl;	//flac解码控制结构体

 
//分析FLAC文件
//fx:flac文件指针
//fc:flac解码容器
//返回值:0,分析成功
//    其他,错误代码
u8 flac_init(FIL* fx,__flacctrl* fctrl,FLACContext* fc)
{
	FLAC_Tag * flactag;
	MD_Block_Head *flacblkh;
	u8 *buf; 
	u8 endofmetadata=0;			//最后一个metadata标记
	int blocklength; 
	u32 br;
	u8 res;

	buf=mymalloc(SRAMIN,512);	//申请512字节内存
	if(!buf)return 1;			//内存申请失败 
	f_lseek(fx,0);				//偏移到文件头
	f_read(fx,buf,4,&br);		//读取4字节 
	flactag=(FLAC_Tag*)buf;		//强制转换为flac tag标签
	if(strncmp("fLaC",(char*)flactag->id,4)!=0) 
	{
		myfree(SRAMIN,buf);		//释放内存
		return 2;				//非flac文件
    } 
    while(!endofmetadata) 
	{
		f_read(fx,buf,4,&br);
        if(br<4)break;
		flacblkh=(MD_Block_Head*)buf;
		endofmetadata=flacblkh->head&0X80;	//判断是不是最后一个block?
		blocklength=((u32)flacblkh->size[0]<<16)|((u16)flacblkh->size[1]<<8)|(flacblkh->size[2]);//得到块大小
        if((flacblkh->head&0x7f)==0) 		//head最低7位为0,则表示是STREAMINFO块
        { 
			res=f_read(fx,buf,blocklength,&br);
            if(res!=FR_OK)break;  
            fc->min_blocksize=((u16)buf[0]<<8) |buf[1];					//最小块大小
            fc->max_blocksize=((u16)buf[2]<<8) |buf[3];					//最大块大小
            fc->min_framesize=((u32)buf[4]<<16)|((u16)buf[5]<<8)|buf[6];//最小帧大小
            fc->max_framesize=((u32)buf[7]<<16)|((u16)buf[8]<<8)|buf[9];//最大帧大小
            fc->samplerate=((u32)buf[10]<<12)|((u16)buf[11]<<4)|((buf[12]&0xf0)>>4);//采样率
            fc->channels=((buf[12]&0x0e)>>1)+1;							//音频通道数
            fc->bps=((((u16)buf[12]&0x01)<<4)|((buf[13]&0xf0)>>4))+1;	//采样位数16?24?32? 
            fc->totalsamples=((u32)buf[14]<<24)|((u32)buf[15]<<16)|((u16)buf[16]<<8)|buf[17];//一个声道的总采样数
			fctrl->samplerate=fc->samplerate;
			fctrl->totsec=(fc->totalsamples/fc->samplerate);//得到总时间 
        }else 	//忽略其他帧的处理 
		{ 
            if(f_lseek(fx,fx->fptr+blocklength)!=FR_OK)
            { 
				myfree(SRAMIN,buf);
				return 3;
            }
		}
    } 
	myfree(SRAMIN,buf);//释放内存.
	if(fctrl->totsec)
	{
		fctrl->outsamples=fc->max_blocksize*2;//PCM输出数据量(*2,表示2个声道的数据量)
		fctrl->bps=fc->bps;			//采样位数(16/24/32)
		fctrl->datastart=fx->fptr;	//FLAC数据帧开始的地址
		fctrl->bitrate=((fx->fsize-fctrl->datastart)*8)/fctrl->totsec;//得到FLAC的位速
	}else return 4;	//总时间为0?有问题的flac文件
	return 0;
} 
vu8 flactransferend=0;	//i2s传输完成标志
vu8 flacwitchbuf=0;		//i2sbufx指示标志 
//FLAC DMA发送回调函数
void flac_i2s_dma_tx_callback(void) 
{    
	u16 i;
	u16 size;
	if(DMA1_Stream4->CR&(1<<19))
	{
		flacwitchbuf=0;
		if((audiodev.status&0X01)==0)//暂停了,填充0
		{ 
			if(flacctrl->bps==24)size=flacctrl->outsamples*4;
			else size=flacctrl->outsamples*2;
			for(i=0;i<size;i++)audiodev.i2sbuf1[i]=0;
		}
	}else 
	{
		flacwitchbuf=1;
		if((audiodev.status&0X01)==0)//暂停了,填充0
		{
			if(flacctrl->bps==24)size=flacctrl->outsamples*4;
			else size=flacctrl->outsamples*2;
			for(i=0;i<size;i++)audiodev.i2sbuf2[i]=0;
		}
	} 
	flactransferend=1;
} 
//得到当前播放时间
//fx:文件指针
//flacctrl:flac播放控制器
void flac_get_curtime(FIL*fx,__flacctrl *flacctrl)
{
	long long fpos=0;  	 
	if(fx->fptr>flacctrl->datastart)fpos=fx->fptr-flacctrl->datastart;	//得到当前文件播放到的地方 
	flacctrl->cursec=fpos*flacctrl->totsec/(fx->fsize-flacctrl->datastart);	//当前播放到第多少秒了?	
}
//flac文件快进快退函数
//pos:需要定位到的文件位置
//返回值:当前文件位置(即定位后的结果)
u32 flac_file_seek(u32 pos)
{
	if(pos>audiodev.file->fsize)
	{
		pos=audiodev.file->fsize;
	}
	f_lseek(audiodev.file,pos);
	return audiodev.file->fptr;
}
//播放一曲FLAC音乐
//fname:FLAC文件路径.
//返回值:0,正常播放完成
//[b7]:0,正常状态;1,错误状态
//[b6:0]:b7=0时,表示操作码 
//       b7=1时,表示有错误(这里不判定具体错误,0X80~0XFF,都算是错误)
u8 flac_play_song(u8* fname) 
{ 
    FLACContext *fc=0; 
    int bytesleft;
    int consumed;
	u8 res=0; 
	u32 i=0;
	u32 br=0; 
	u8* buffer=0;    
	u8* decbuf0=0;   
	u8* decbuf1=0;  
	u8* p8=0;   
	u8* pbuf=0; 
	u32 flac_fptr=0; 
	
 	fc=mymalloc(SRAMIN,sizeof(FLACContext)); 
	flacctrl=mymalloc(SRAMIN,sizeof(__flacctrl)); 
	audiodev.file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	audiodev.file_seek=flac_file_seek;
	if(!fc||!audiodev.file||!flacctrl)res=1;//内存申请错误
	else
	{ 
		memset(fc,0,sizeof(FLACContext));//fc所有内容清零 
		res=f_open(audiodev.file,(char*)fname,FA_READ); //读取文件错误 
		if(res==FR_OK)
		{
			res=flac_init(audiodev.file,flacctrl,fc);	//flac解码初始化   
			if(fc->min_blocksize==fc->max_blocksize&&fc->max_blocksize!=0)//必须min_blocksize等于max_blocksize
			{
				if(fc->bps==24)	//24位音频数据
				{	
					audiodev.i2sbuf1=mymalloc(SRAMIN,fc->max_blocksize*8);
					audiodev.i2sbuf2=mymalloc(SRAMIN,fc->max_blocksize*8);  
				}else			//16位音频数据
				{
					audiodev.i2sbuf1=mymalloc(SRAMIN,fc->max_blocksize*4);
					audiodev.i2sbuf2=mymalloc(SRAMIN,fc->max_blocksize*4); 
				}
				buffer=mymalloc(SRAMCCM,fc->max_framesize); 	//申请解码帧缓存 
				decbuf0=mymalloc(SRAMCCM,fc->max_blocksize*4);
				decbuf1=mymalloc(SRAMCCM,fc->max_blocksize*4);
			}else res+=1;//不支持的音频格式  
		}
	}
	if(buffer&&audiodev.i2sbuf1&&audiodev.i2sbuf2&&decbuf0&&decbuf1&&res==0)
	{ 
		printf("\r\n  Blocksize: %d .. %d\r\n", fc->min_blocksize,fc->max_blocksize);
		printf("  Framesize: %d .. %d\r\n",fc->min_framesize,fc->max_framesize);
		printf("  Samplerate: %d\r\n", fc->samplerate);
		printf("  Channels: %d\r\n", fc->channels);
		printf("  Bits per sample: %d\r\n", fc->bps);
		printf("  Metadata length: %d\r\n", flacctrl->datastart);
		printf("  Total Samples: %lu\r\n",fc->totalsamples);
		printf("  Duration: %d s\r\n",flacctrl->totsec);
		printf("  Bitrate: %d kbps\r\n",flacctrl->bitrate); 
		if(flacctrl->bps==24)	//24位音频数据
		{
			WM8978_I2S_Cfg(2,2);//飞利浦标准,24位数据长度
			I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);	//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
			I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,flacctrl->outsamples*2);//配置TX DMA
			memset(audiodev.i2sbuf1,0,fc->max_blocksize*8);
			memset(audiodev.i2sbuf2,0,fc->max_blocksize*8);
		}else					//16位音频数据
		{
			WM8978_I2S_Cfg(2,0);//飞利浦标准,16位数据长度
			I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);	//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度	
			I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,flacctrl->outsamples);//配置TX DMA			
			memset(audiodev.i2sbuf1,0,fc->max_blocksize*4);
			memset(audiodev.i2sbuf2,0,fc->max_blocksize*4);
		}  
		I2S2_SampleRate_Set(fc->samplerate);		//设置采样率 
		i2s_tx_callback=flac_i2s_dma_tx_callback;	//回调函数指向flac_i2s_dma_tx_callback
		f_read(audiodev.file,buffer,fc->max_framesize,&br);//读取最大帧长数据		
		bytesleft=br;
		audio_start();					//开始播放  
		fc->decoded0=(int*)decbuf0;		//解码数组0
		fc->decoded1=(int*)decbuf1;  	//解码数组1 
		flac_fptr=audiodev.file->fptr;	//记录当前的文件位置.
		while(bytesleft) 
		{   
			while(flactransferend==0)//等待传输完成
			{ 
				delay_ms(1000/OS_TICKS_PER_SEC);
			};	
			if(flac_fptr!=audiodev.file->fptr)//说明外部有进行文件快进/快退操作
			{
				if(audiodev.file->fptr<flacctrl->datastart)//在数据开始之前??
				{
					f_lseek(audiodev.file,flacctrl->datastart);//偏移到数据开始的地方
				} 
				f_read(audiodev.file,buffer,fc->max_framesize,&br); //读取一个最大帧的数据量
				bytesleft=flac_seek_frame(buffer,br,fc);		//查找帧 
				if(bytesleft>=0)								//找到正确的帧头.
				{
					f_lseek(audiodev.file,audiodev.file->fptr-fc->max_framesize+bytesleft);
					f_read(audiodev.file,buffer,fc->max_framesize,&br); 
				}else printf("flac seek error:%d\r\n",bytesleft); 
				bytesleft=br;
			}
			flactransferend=0;  
			if(flacwitchbuf==0)p8=audiodev.i2sbuf1;
			else p8=audiodev.i2sbuf2; 
			if(fc->bps==24)res=flac_decode_frame24(fc,buffer,bytesleft,(s32*)p8);
			else res=flac_decode_frame16(fc,buffer,bytesleft,(s16*)p8); 
			if(res!=0)//解码出错了 
			{
				res=AP_ERR;
				break;
			}
			if(fc->bps==24)	//24位的时候,数据需要单独处理下
			{ 
				pbuf=p8;
				for(i=0;i<fc->blocksize*8;)
				{
					p8[i++]=pbuf[1];
					p8[i]=pbuf[2]; 
					i+=2;
					p8[i++]=pbuf[0];
					pbuf+=4;
				} 				
			}
			consumed=fc->gb.index/8;
			memmove(buffer,&buffer[consumed],bytesleft-consumed);
			bytesleft-=consumed; 
			res=f_read(audiodev.file,&buffer[bytesleft],fc->max_framesize-bytesleft,&br); 
			if(res)//读数据出错了
			{
				res=AP_ERR;
				break;
			}
			if(br>0) 
			{
				bytesleft+=br;
			}
			flac_fptr=audiodev.file->fptr;	//记录当前的文件位置.
			while(audiodev.status&(1<<1))	//正常播放中
			{		  
				flac_get_curtime(audiodev.file,flacctrl);//得到总时间和当前播放的时间 
				audiodev.totsec=flacctrl->totsec;		//参数传递
				audiodev.cursec=flacctrl->cursec;
				audiodev.bitrate=flacctrl->bitrate;
				audiodev.samplerate=flacctrl->samplerate;
				audiodev.bps=flacctrl->bps;	
  				if(audiodev.status&0X01)break;	//没有按下暂停 
				else delay_ms(1000/OS_TICKS_PER_SEC);
			}
			if((audiodev.status&(1<<1))==0)		//请求结束播放/播放完成
			{  
				break;
			} 	 
		} 
		audio_stop();
	}else res=AP_ERR;
	f_close(audiodev.file);
	myfree(SRAMIN,fc);
	myfree(SRAMIN,flacctrl);
	myfree(SRAMIN,audiodev.file);
	myfree(SRAMIN,audiodev.i2sbuf1);
	myfree(SRAMIN,audiodev.i2sbuf2); 
	myfree(SRAMCCM,buffer);
	myfree(SRAMCCM,decbuf0);
	myfree(SRAMCCM,decbuf1); 
    return res;
} 









































