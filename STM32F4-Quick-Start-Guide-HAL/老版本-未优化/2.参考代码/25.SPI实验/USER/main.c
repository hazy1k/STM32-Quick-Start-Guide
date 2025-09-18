#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "w25qxx.h"

// 要写入到W25QXX的字符串数组
const u8 TEXT_Buffer[]={"Explorer STM32F4 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
int main(void)
{
    u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE; 
    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
	delay_init(168);            // 初始化延时函数
	uart_init(115200);          // 初始化USART
	usmart_dev.init(84); 		// 初始化USMART
	LED_Init();					// 初始化LED	
	KEY_Init();					// 初始化KEY
 	LCD_Init();           		// 初始化LCD
    W25QXX_Init();				// W25QXX初始化
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"SPI TEST");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//显示提示信息		
//	while(W25QXX_ReadID()!=W25Q128)								//检测不到W25Q256
	while(W25QXX_ReadID()!=W25Q128 && W25QXX_ReadID()!=NM25Q128 && W25QXX_ReadID()!=BY25Q128)  /* 读取QSPI FLASH ID */
	{
		LCD_ShowString(30,150,200,16,16,"W25Q128 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0=!LED0;		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"W25Q128 Ready!"); 
	FLASH_SIZE=32*1024*1024;	//FLASH 大小为32M字节
  	POINT_COLOR=BLUE;			//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入W25Q128
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q128....");
			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//提示传送完成
		}
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q128.... ");
			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);					//显示读到的字符串
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	}	
}

