/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "malloc.h"		 
#include "guix.h"
#include "ucos_ii.h"
#include "usbh_usr.h"
#include "calendar.h" 	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//FATFS底层(diskio) 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


#define SD_CARD	 0  		//SD卡,卷标为0
#define EX_FLASH 1			//外部flash,卷标为1
#define USB_DISK 2			//U盘,卷标为2

#define FLASH_SECTOR_SIZE 	512		//W25Q128扇区大小为512字节
#define FLASH_SECTOR_NUM    2048*12	//W25Q128用于FATFS管理的扇区数量,前12M字节给FATFS占用
//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M.	15.10M字节以后,给客户自己用	 			    
u16 FLASH_SECTOR_COUNT;				//W25Q128扇区数量
#define FLASH_BLOCK_SIZE   	8  		//每个BLOCK有8个扇区


//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	u8 res=0;	 
 	switch(pdrv)
	{
		case SD_CARD:		//SD卡
			res=SD_Init();	//SD_Init()  
  			break;
		case EX_FLASH:		//外部flash
			W25QXX_Init();
			FLASH_SECTOR_COUNT=FLASH_SECTOR_NUM;//W25Q1218,前12M字节给FATFS占用 
 			break;
		case USB_DISK:		//U盘  
	  		res=!USBH_UDISK_Status();//U盘连接成功,则返回1.否则返回0	
			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功
}  

//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

//读扇区
//drv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	u8 res=0;  
	u8 tcnt=0;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误	 
	switch(pdrv)
	{
		case SD_CARD://SD卡 
			res=SD_ReadDisk(buff,sector,count);	 
			while(res&&tcnt<10)//读出错
			{
				tcnt++; 
				res=SD_Init();	//重新初始化SD卡
				res=SD_ReadDisk(buff,sector,count); 
				//printf("sd rd error:%d\r\n",res);
			}
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case USB_DISK://U盘    
			res=USBH_UDISK_Read(buff,sector,count);  								    
			break;
		default:
			res=1; 
	} 
   //处理返回值,将返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

//写扇区
//drv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	u8 res=0;   
 	u8 tcnt=0;
   if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误	 
 	switch(pdrv)
	{
		case SD_CARD://SD卡
			res=SD_WriteDisk((u8*)buff,sector,count);
			while(res&&tcnt<10)//读出错
			{
				tcnt++;
				res=SD_Init();	//重新初始化SD卡
				res=SD_WriteDisk((u8*)buff,sector,count);	 
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{		 
				W25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case USB_DISK://U盘 
			res=USBH_UDISK_Write((u8*)buff,sector,count); 
			break;
		default:
			res=1; 
	}  
   //处理返回值,将返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif


//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						  			     
	if(pdrv==SD_CARD)//SD卡
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==EX_FLASH)	//外部FLASH  
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==USB_DISK)	//U盘
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff=512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff=512;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff=USBH_MSC_Param.MSCapacity;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }		
	}else res=RES_ERROR;//其他的不支持
    return res;
}
#endif 
vu8 cnt0=0;
vu8 cnt1=0;
OS_CPU_SR cpu_sr=0; 
//进入临界区
void ff_enter(FATFS *fs)
{   
	if(fs->drv!=2)
	{
		OS_ENTER_CRITICAL();	//进入临界区(无法被中断打断)     
		cnt0++;
	}else
	{  
		OSSchedLock();			//阻止ucos调度
		cnt1++;
	}
}
//退出临界区
void ff_leave(FATFS* fs)
{ 
	if(cnt0)
	{
		cnt0--; 
		OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断) 
	}
	if(cnt1)
	{ 
		cnt1--;
		OSSchedUnlock();	//开启ucos调度 	
	}
}  
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{	
	u32 time=0;
	calendar_get_date(&calendar);
	calendar_get_time(&calendar);
	if(calendar.w_year<1980)calendar.w_year=1980;
	time=(calendar.w_year-1980)<<25;//年份
	time|=(calendar.w_month)<<21;	//月份
	time|=(calendar.w_date)<<16;	//日期
	time|=(calendar.hour)<<11;		//时
	time|=(calendar.min)<<5;		//分
	time|=(calendar.sec/2);			//秒	
	return time;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}




















