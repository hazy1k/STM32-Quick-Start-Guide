/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
#include "utility.h"
#include "w5500.h"
#include "./dhcp/dhcp.h"
#include "../TimBase/bsp_TiMbase.h" 
#include ".\bsp\EEPROM\bsp_i2c_ee.h"


CONFIG_MSG  ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*定义默认IP信息*/
uint8 local_ip[4]={192,168,0,122};												/*定义W5500默认IP地址*/
uint8 subnet[4]={255,255,255,0};												/*定义W5500默认子网掩码*/
uint8 gateway[4]={192,168,1,1};													/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};									/*定义W5500默认DNS*/

uint16 local_port=6000;	                       					/*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]={192,168,0,130};											/*远端IP地址*/
uint16 remote_port=6000;																/*远端端口号*/

/*IP配置方法选择，请自行选择*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8 dhcp_ok=0;																				/*dhcp成功获取IP*/
uint32	ms=0;																						/*毫秒计数*/
uint32	dhcp_time= 0;																		/*DHCP运行计数*/
vu8	ntptimer = 0;																				/*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
uint8 set_w5500_ip(void)
{	
		
 /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
//	if(ip_from==IP_FROM_DEFINE)	
//		printf(" 使用定义的IP信息配置W5500\r\n");
	
	/*使用EEPROM存储的IP参数*/	
	if(ip_from==IP_FROM_EEPROM)
	{
		ee_CheckOk();
		
		/*从EEPROM中读取IP配置信息*/
		read_config_from_eeprom();		
		
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			printf(" IP from EEPROM\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
			printf(" EEPROM未配置,使用定义的IP信息配置W5500,并写入EEPROM\r\n");
			write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}			
	}

	/*使用DHCP获取IP参数，需调用DHCP子函数*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*复制DHCP获取的配置信息到配置结构体*/
		if(dhcp_ok==1)
		{
			printf(" IP from DHCP\r\n");		 
			memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
		}
		else
		{
			printf(" DHCP子程序未运行,或者不成功\r\n");
			printf(" 使用定义的IP信息配置W5500\r\n");
		}
	}
		
	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);
	printf(" W5500 IP地址   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf(" W5500 子网掩码 : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf(" W5500 网关     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
	if((local_ip[0]!=ConfigMsg.lip[0])||(local_ip[1]!=ConfigMsg.lip[1])||
		 (local_ip[2]!=ConfigMsg.lip[2])||(local_ip[3]!=ConfigMsg.lip[3]))
		return 1;
	if((subnet[0]!=ConfigMsg.sub[0])||(subnet[1]!=ConfigMsg.sub[1])||
	 (subnet[2]!=ConfigMsg.sub[2])||(subnet[3]!=ConfigMsg.sub[3]))
		return 2;
	if((gateway[0]!=ConfigMsg.gw[0])||(gateway[1]!=ConfigMsg.gw[1])||
	 (gateway[2]!=ConfigMsg.gw[2])||(gateway[3]!=ConfigMsg.gw[3]))
		return 3;
	
	return 0;
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	memcpy(DHCP_GET.mac, mac, 6);
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void gpio_for_w5500_config(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(WIZ_SPIx_RESET_CLK|WIZ_SPIx_INT_CLK, ENABLE);
	/*初始化STM32 SPI2接口*/
  RCC_APB2PeriphClockCmd(WIZ_SPIx_GPIO_CLK|WIZ_SPIx_SCS_CLK, ENABLE);
	
	WIZ_SPIx_CLK_CMD(WIZ_SPIx_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = WIZ_SPIx_SCLK | WIZ_SPIx_MISO | WIZ_SPIx_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // 复用输出
	GPIO_Init(WIZ_SPIx_GPIO_PORT, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin = WIZ_SPIx_SCS;
   //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // 推免输出
   GPIO_Init(WIZ_SPIx_SCS_PORT, &GPIO_InitStructure);						  
//   GPIO_SetBits(WIZ_GPIO_PORT, WIZ_SCS);										 // 先把片选拉高，真正用的时候再拉低 


	/* SPI2 配置 */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(WIZ_SPIx, &SPI_InitStructure);
	
	/* 使能 SPI2 */
	SPI_Cmd(WIZ_SPIx, ENABLE); 
	
	
	/*使用野火ISO标准开发板*/
	#ifdef STM32F103ZET6
		/*定义RESET引脚*/
		GPIO_InitStructure.GPIO_Pin = WIZ_RESET;					 /*选择要控制的GPIO引脚*/		 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz */		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*设置引脚模式为通用推挽输出*/	
		GPIO_Init(WIZ_RESET_INT_PORT, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
		GPIO_SetBits(WIZ_RESET_INT_PORT, WIZ_RESET);		
		/*定义INT引脚*/	
		GPIO_InitStructure.GPIO_Pin = WIZ_INT;						 /*选择要控制的GPIO引脚*/		 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz*/		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*设置引脚模式为通用推挽模拟上拉输入*/		
		GPIO_Init(WIZ_RESET_INT_PORT, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
	#endif
	
	/*使用野火MINI开发板*/
	#ifdef STM32F103VET6
		/*定义RESET引脚*/
		GPIO_InitStructure.GPIO_Pin = WIZ_RESET;					       /*选择要控制的GPIO引脚*/		 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     /*设置引脚速率为50MHz */		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     /*设置引脚模式为通用推挽输出*/	
		GPIO_Init(WIZ_SPIx_RESET_PORT, &GPIO_InitStructure);		 /*调用库函数，初始化GPIO*/
		GPIO_SetBits(WIZ_SPIx_RESET_PORT, WIZ_RESET);		
		/*定义INT引脚*/	
		GPIO_InitStructure.GPIO_Pin = WIZ_INT;						       /*选择要控制的GPIO引脚*/		 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     /*设置引脚速率为50MHz*/		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				     /*设置引脚模式为通用推挽模拟上拉输入*/		
		GPIO_Init(WIZ_SPIx_INT_PORT, &GPIO_InitStructure);			 /*调用库函数，初始化GPIO*/
	#endif
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
		GPIO_ResetBits(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS); 
	}
	else if (val == HIGH)
	{
		GPIO_SetBits(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS); 
	}
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
void reset_w5500(void)
{
	/*使用野火ISO标准开发板*/
	#ifdef STM32F103ZET6
		GPIO_ResetBits(WIZ_RESET_INT_PORT, WIZ_RESET);
		delay_us(2);  
		GPIO_SetBits(WIZ_RESET_INT_PORT, WIZ_RESET);
		delay_ms(1600);
	#endif
	
	/*使用野火MINI开发板*/
	#ifdef STM32F103VET6
		GPIO_ResetBits(WIZ_SPIx_RESET_PORT, WIZ_RESET);
		delay_us(2);  
		GPIO_SetBits(WIZ_SPIx_RESET_PORT, WIZ_RESET);
		delay_ms(1600);	
	#endif
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
unsigned char SPI2_ReadWrite(unsigned char writedat)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(WIZ_SPIx,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(WIZ_SPIx, writedat);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(WIZ_SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(WIZ_SPIx);
}

/**
*@brief		STM32 SPI2读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI2_ReadWrite(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   if(len == 0) printf("Unexpected2 length 0\r\n");
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf("Unexpected2 length 0\r\n");
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
	uint16 dAddr=0;
	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
	delay_ms(10);																							
}

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	delay_us(10);
}

/**
*@brief		STM32定时器2初始化
*@param		无
*@return	无
*/
void timer2_init(void)
{
	TIM2_Configuration();																		/* TIM2 定时配置 */
	TIM2_NVIC_Configuration();															/* 定时器的中断优先级 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 重新开时钟，开始计时 */
}

/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void dhcp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		ntp用到的定时器初始化
*@param		无
*@return	无
*/
void ntp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		定时器2中断函数
*@param		无
*@return	无
*/
void timer2_isr(void)
{
	ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP定时加1S*/
		#ifndef	__NTP_H__
		ntptimer++;																						/*NTP重试时间加1S*/
		#endif
  }

}
/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" 系统重启中……\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

