/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: simple example decoder using vorbisidec

 ********************************************************************/

/* Takes a vorbis bitstream from stdin and writes raw stereo PCM to
   stdout using vorbisfile. Using vorbisfile is much simpler than
   dealing with libvorbis. */

#include "ivorbiscodec.h"
#include "ivorbisfile.h"
#include "sys.h"
#include "malloc.h"
#include "string.h"
#include "ff.h"
#include "malloc.h" 
#include <inttypes.h>
#include "audioplayer.h"
#include "key.h"

#include "i2s.h"
#include <wm8978.h>

__align(4) char pcmout1[4096]; /* take 4k out of the data segment, not the stack */
__align(4) char pcmout2[4096]; /* take 4k out of the data segment, not the stack */

u32 mbuff[500];
u16 bufsize=0;

//ogg malloc函数实现
void* ogg_malloc(u32 size)
{
	void *p;
	p=mymalloc(SRAMIN,size);
	printf("ogg_malloc:%x,%d\r\n",(u32)p,size);
	mbuff[bufsize]=(u32)p;
	bufsize++;
	return p;
}
//ogg calloc函数实现
void* ogg_calloc(u32 count,u32 size)
{
	void *p; 
	p=mymalloc(SRAMIN,count*size); 
	if(p)memset(p,0,count*size); 
	printf("ogg_calloc:%x,%d\r\n",(u32)p,count*size);
	mbuff[bufsize]=(u32)p;
	bufsize++;
	return p;
}
//ogg realloc函数实现
void *ogg_realloc(void *ptr,u32 size)  
{
	void *p;
	p=myrealloc(SRAMIN,ptr,size);
	printf("ogg_realloc:%x,%d\r\n",(u32)p,size);
	mbuff[bufsize]=(u32)p;
	bufsize++;
	return p;
}
//ogg free函数实现
void ogg_free(void *ptr)  
{
	u16 i;
	printf("ogg_free:%x\r\n",(u32)ptr);
	for(i=0;i<bufsize;i++)
	{
		if(mbuff[i]==(u32)ptr)mbuff[i]=0;//释放掉了的,清零.
	}
	myfree(SRAMIN,ptr);
}

vu8 transferend=0;		//i2s传输完成标志
vu8 witchbuf=0;			//i2sbufx指示标志

//OGG DMA发送回调函数
void ogg_i2s_dma_tx_callback(void) 
{    
	u16 i;
	if(DMA1_Stream4->CR&(1<<19))
	{
		witchbuf=0;
		if((audiodev.status&0X01)==0)//暂停了,填充0
		{
			for(i=0;i<4096;i++)audiodev.i2sbuf1[i]=0;
		}
	}else 
	{
		witchbuf=1;
		if((audiodev.status&0X01)==0)//暂停了,填充0
		{
			for(i=0;i<4096;i++)audiodev.i2sbuf2[i]=0;
		}
	} 
	transferend=1;
} 
//填充PCM数据到DAC
//buf:PCM数据首地址
//size:pcm数据量(16位为单位)
//nch:声道数(1,单声道,2立体声)
void ogg_fill_buffer(u16* buf,u16 size,u8 nch)
{ 
	while((DMA1->HISR&(1<<5))==0);//等待传输完成
	DMA1->HIFCR|=1<<5;
	I2S_Play_Stop();//关闭DMA传输
	DMA1_Stream4->M0AR=(u32)buf;
	DMA1_Stream4->NDTR=size; 
	I2S_Play_Start();//开启DMA传输
}   
//I2S2 TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
void OGG_TX_DMA_Init(u8* buf0,u16 num)
{  
	RCC->AHB1ENR|=1<<21;		//DMA1时钟使能  
	while(DMA1_Stream4->CR&0X01);//等待DMA1_Stream4可配置 
	DMA1->HIFCR|=0X3D<<6*0;		//清空通道4上所有中断标志
	DMA1_Stream4->FCR=0X0000021;//设置为默认值	
	
	DMA1_Stream4->PAR=(u32)&SPI2->DR;		//外设地址为:SPI2->DR
	DMA1_Stream4->M0AR=(u32)buf0;//暂不设置
	//DMA1_Stream4->M1AR=(u32)buf1;//暂不设置
	DMA1_Stream4->NDTR=num;		//暂时设置长度为1
	DMA1_Stream4->CR=0;			//先全部复位CR寄存器值  
	DMA1_Stream4->CR|=1<<6;		//存储器到外设模式 
	DMA1_Stream4->CR|=0<<8;		//单次模式
	DMA1_Stream4->CR|=0<<9;		//外设非增量模式
	DMA1_Stream4->CR|=1<<10;	//存储器增量模式
	DMA1_Stream4->CR|=1<<11;	//外设数据长度:16位
	DMA1_Stream4->CR|=1<<13;	//存储器数据长度:16位
	DMA1_Stream4->CR|=2<<16;	//高优先级
	DMA1_Stream4->CR|=0<<18;	//单缓冲模式
	DMA1_Stream4->CR|=0<<21;	//外设突发单次传输
	DMA1_Stream4->CR|=0<<23;	//存储器突发单次传输
	DMA1_Stream4->CR|=0<<25;	//选择通道0 SPI2_TX通道 

	DMA1_Stream4->FCR&=~(1<<2);	//不使用FIFO模式
	DMA1_Stream4->FCR&=~(3<<0);	//无FIFO 设置
	
	//DMA1_Stream4->CR|=1<<4;		//开启传输完成中断
	//MY_NVIC_Init(0,0,DMA1_Stream4_IRQn,2);	//抢占0，子优先级2，组2  
} 

int ogg(char * path)
{
	FIL*fogg;
	OggVorbis_File vf;
	int eof=0;
	int current_section;
	int i=0;
	u8 res; 
	vorbis_comment *pov_comment;
	char **ptr;
	vorbis_info *vi; 
	long ret;
	u8 key;
	
	bufsize=0;
	memset(mbuff,0,1000);//全部清零
	
	audiodev.i2sbuf1=mymalloc(SRAMIN,4096); 
	audiodev.i2sbuf2=mymalloc(SRAMIN,4096); 
	
	
	fogg=(FIL*)mymalloc(SRAMIN,sizeof(FIL));//打开fogg文件
	fogg=(FIL*)mymalloc(SRAMIN,sizeof(FIL));//打开fogg文件
	res=f_open(fogg,path,FA_READ);
	if(res)
	{
		printf("open file:%s error\r\n",path);
	}
	if(ov_open(fogg, &vf, NULL, 0) < 0) 
	{
		printf("Input does not appear to be an Ogg bitstream.\n");
		return -1;
	}  
	pov_comment 	=(vorbis_comment *)ov_comment(&vf,-1);
	ptr= pov_comment->user_comments;
	
	vi=(vorbis_info *)ov_info(&vf,-1);
	while(*ptr)
	{
		printf("\r\n%s\r\n",*ptr);
		++ptr;
	}
	printf("\r\nBitstream is %d channel, %ldHz\r\n",vi->channels,vi->rate);
	printf("Decoded length: %ld samples\r\n",(long)ov_pcm_total(&vf,-1));
	printf("Encoded by: %s\r\n\r\n",pov_comment->vendor);

	WM8978_ADDA_Cfg(1,0);	//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);	//开启DAC输出 

	WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
	I2S2_Init(0,2,0,0);		//飞利浦标准,主机发送,时钟低电平有效,16位标准帧长度
 	I2S2_SampleRate_Set(vi->rate);		//设置采样率 
	OGG_TX_DMA_Init(pcmout1,4096/2);	//配置TX DMA
	
	//I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,4096/2);//配置TX DMA
	i2s_tx_callback=ogg_i2s_dma_tx_callback;		//回调函数指mp3_i2s_dma_tx_callback
 	audio_start(); 
	while(!eof)
	{
		// rt_sem_take(&ogg_sem, RT_WAITING_FOREVER);
		if(i++ & 1)
		{ 
			ret=ov_read(&vf,pcmout1,sizeof(pcmout1),&current_section);
			if (ret == 0) 
			{
				/* EOF */
				eof=1;
			} else if (ret < 0) 
			{
				/* error in the stream.  Not a problem, just reporting it in
				case we (the app) cares.  In this case, we don't. */
			} else 
			{
				/* we don't bother dealing with sample rate changes, etc, but
				you'll have to*/
				//  fwrite(pcmout,1,ret,stdout);
				//	rt_device_write(snd_device, 0, (int16_t *)pcmout1,  ret);
				//printf("pcm1:%d\r\n",ret);
				ogg_fill_buffer((u16*)pcmout1,ret/2,2); 
			}
		}
		else
		{
			ret=ov_read(&vf,pcmout2,sizeof(pcmout2),&current_section);
			if (ret == 0) 
			{
				/* EOF */
				eof=1;
			} else if (ret < 0) 
			{
				/* error in the stream.  Not a problem, just reporting it in
				case we (the app) cares.  In this case, we don't. */
			} else 
			{
				/* we don't bother dealing with sample rate changes, etc, but
				you'll have to*/
				//  fwrite(pcmout,1,ret,stdout);
				//printf("pcm2:%d\r\n",ret);
				ogg_fill_buffer((u16*)pcmout2,ret/2,2); 
			} 
		} 
		key=KEY_Scan(0);
		if(key==KEY0_PRES)break;
	} 
	/* cleanup */
	ov_clear(&vf);  
	myfree(SRAMIN,fogg); 
	myfree(SRAMIN,audiodev.i2sbuf1); 
	myfree(SRAMIN,audiodev.i2sbuf2); 
	for(i=0;i<bufsize;i++)
	{
		printf("mbuff[%d]:%x\r\n",i,mbuff[i]); 
		if(mbuff[i]!=0)
		{
			myfree(SRAMIN,(void*)mbuff[i]);
		}
	}
	return 0;
} 



































