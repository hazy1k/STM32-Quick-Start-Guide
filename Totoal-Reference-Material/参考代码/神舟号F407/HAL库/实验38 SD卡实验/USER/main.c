#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "sram.h"
#include "malloc.h" 
#include "sdio_sdcard.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验38
 SD卡实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	uint64_t CardCap;	//SD卡容量
	HAL_SD_CardCIDTypeDef SDCard_CID;

	HAL_SD_GetCardCID(&SDCARD_Handler,&SDCard_CID);	//获取CID
	SD_GetCardInfo(&SDCardInfo);					//获取SD卡信息
	switch(SDCardInfo.CardType)
	{
		case CARD_SDSC:
		{
			if(SDCardInfo.CardVersion == CARD_V1_X)
				printf("Card Type:SDSC V1\r\n");
			else if(SDCardInfo.CardVersion == CARD_V2_X)
				printf("Card Type:SDSC V2\r\n");
		}
		break;
		case CARD_SDHC_SDXC:printf("Card Type:SDHC\r\n");break;
	}	
	CardCap=(uint64_t)(SDCardInfo.LogBlockNbr)*(uint64_t)(SDCardInfo.LogBlockSize);	//计算SD卡容量
  	printf("Card ManufacturerID:%d\r\n",SDCard_CID.ManufacturerID);					//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RelCardAdd);								//卡相对地址
	printf("LogBlockNbr:%d \r\n",(u32)(SDCardInfo.LogBlockNbr));					//显示逻辑块数量
	printf("LogBlockSize:%d \r\n",(u32)(SDCardInfo.LogBlockSize));					//显示逻辑块大小
	printf("Card Capacity:%d MB\r\n",(u32)(CardCap>>20));							//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.BlockSize);						//显示块大小
}

//测试SD卡的读取
//从secaddr地址开始,读取seccnt个扇区的数据
//secaddr:扇区地址
//seccnt:扇区数
void sd_test_read(u32 secaddr,u32 seccnt)
{
	u32 i; u8 *buf; u8 sta=0;
	buf=mymalloc(SRAMEX,seccnt*512);	//申请内存
	sta=SD_ReadDisk(buf,secaddr,seccnt);//读取secaddr扇区开始的内容
	if(sta==0)						
	{	 
		printf("SECTOR %d DATA:\r\n",secaddr);
		for(i=0;i<seccnt*512;i++)printf("%x ",buf[i]);//打印secaddr开始的扇区数据    	   
		printf("\r\nDATA ENDED\r\n"); 
	}else printf("err:%d\r\n",sta);
	myfree(SRAMEX,buf);	//释放内存	   
}
//测试SD卡的写入(慎用,最好写全是0XFF的扇区,否则可能损坏SD卡.)
//从secaddr地址开始,写入seccnt个扇区的数据
//secaddr:扇区地址
//seccnt:扇区数
void sd_test_write(u32 secaddr,u32 seccnt)
{
	u32 i;
	u8 *buf; u8 sta=0; 
	buf=mymalloc(SRAMEX,seccnt*512);			//从SDRAM申请内存
	for(i=0;i<seccnt*512;i++) buf[i]=i*3; 		//初始化写入的数据,是3的倍数.
	sta=SD_WriteDisk(buf,secaddr,seccnt);		//从secaddr扇区开始写入seccnt个扇区内容
	if(sta==0) printf("Write over!\r\n");		  
	else printf("err:%d\r\n",sta);
	myfree(SRAMEX,buf);							//释放内存	   
}

int main(void)
{
    u8 key;		 
	u32 sd_size;
	u8 t=0;	
	u8 *buf;
	uint64_t CardCap;				//SD卡容量
	
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	usmart_dev.init(84); 		    //初始化USMART
	LED_Init();						//初始化LED	
	KEY_Init();						//初始化KEY
	LCD_Init();                     //初始化LCD
	SRAM_Init();					//初始化外部SRAM  
	
	my_mem_init(SRAMIN);			//初始化内部内存池
	my_mem_init(SRAMEX);			//初始化外部内存池

	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"SD CARD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/4/18");     
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 0");	   
 	while(SD_Init())//检测不到SD卡
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	show_sdcard_info();	//打印SD卡相关信息
 	POINT_COLOR=BLUE;	//设置字体为蓝色 
	//检测SD卡成功 											    
	LCD_ShowString(30,150,200,16,16,"SD Card OK    ");
	LCD_ShowString(30,170,200,16,16,"SD Card Size:     MB");
	CardCap=(uint64_t)(SDCardInfo.LogBlockNbr)*(uint64_t)(SDCardInfo.LogBlockSize);	//计算SD卡容量
	LCD_ShowNum(30+13*8,170,CardCap>>20,5,16);//显示SD卡容量	
    while(1)
    {
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下了
		{
			buf=mymalloc(0,512);		//申请内存
			if(SD_ReadDisk(buf,0,1)==0)	//读取0扇区的内容
			{	
				LCD_ShowString(30,190,200,16,16,"USART1 Sending Data...");
				printf("SECTOR 0 DATA:\r\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//打印0扇区数据    	   
				printf("\r\nDATA ENDED\r\n");
				LCD_ShowString(30,190,200,16,16,"USART1 Send Data Over!");
			}
			myfree(0,buf);//释放内存	   
		}   
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;
			t=0;
		}
	} 	
}

