#ifndef __W25QXX_H
#define __W25QXX_H
#include "sys.h"

/* FLASHÐ¾Æ¬ÁÐ±í */
#define W25Q80      0XEF13          /* W25Q80   Ð¾Æ¬ID */
#define W25Q16      0XEF14          /* W25Q16   Ð¾Æ¬ID */
#define W25Q32      0XEF15          /* W25Q32   Ð¾Æ¬ID */
#define W25Q64      0XEF16          /* W25Q64   Ð¾Æ¬ID */
#define W25Q128     0XEF17          /* W25Q128  Ð¾Æ¬ID */
#define W25Q256     0XEF18          /* W25Q256  Ð¾Æ¬ID */
#define BY25Q64     0X6816          /* BY25Q64  Ð¾Æ¬ID */
#define BY25Q128    0X6817          /* BY25Q128 Ð¾Æ¬ID */
#define NM25Q64     0X5216          /* NM25Q64  Ð¾Æ¬ID */
#define NM25Q128    0X5217          /* NM25Q128 Ð¾Æ¬ID */

extern u16 W25QXX_TYPE;					//¶¨ÒåW25QXXÐ¾Æ¬ÐÍºÅ		   

#define	W25QXX_CS 		PBout(14)  		//W25QXXµÄÆ¬Ñ¡ÐÅºÅ

////////////////////////////////////////////////////////////////////////////////// 
//Ö¸Áî±í
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//¶ÁÈ¡FLASH ID
u8 W25QXX_ReadSR(u8 regno);             //¶ÁÈ¡×´Ì¬¼Ä´æÆ÷ 
void W25QXX_4ByteAddr_Enable(void);     //Ê¹ÄÜ4×Ö½ÚµØÖ·Ä£Ê½
void W25QXX_Write_SR(u8 regno,u8 sr);   //Ð´×´Ì¬¼Ä´æÆ÷
void W25QXX_Write_Enable(void);  		//Ð´Ê¹ÄÜ 
void W25QXX_Write_Disable(void);		//Ð´±£»¤
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //¶ÁÈ¡flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//Ð´Èëflash
void W25QXX_Erase_Chip(void);    	  	//ÕûÆ¬²Á³ý
void W25QXX_Erase_Sector(u32 Dst_Addr);	//ÉÈÇø²Á³ý
void W25QXX_Wait_Busy(void);           	//µÈ´ý¿ÕÏÐ
void W25QXX_PowerDown(void);        	//½øÈëµôµçÄ£Ê½
void W25QXX_WAKEUP(void);				//»½ÐÑ

#endif
