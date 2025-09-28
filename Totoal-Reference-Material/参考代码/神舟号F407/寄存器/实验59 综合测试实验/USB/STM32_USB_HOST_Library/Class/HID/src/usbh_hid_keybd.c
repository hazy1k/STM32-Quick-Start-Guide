/**
  ******************************************************************************
  * @file    usbh_hid_keybd.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file is the application layer for USB Host HID Keyboard handling
  *          QWERTY and AZERTY Keyboard are supported as per the selection in 
  *          usbh_hid_keybd.h              
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_keybd.h"
#include "usbh_hid_gamepad.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_HID_CLASS
* @{
*/

/** @defgroup USBH_HID_KEYBD 
* @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup USBH_HID_KEYBD_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_HID_KEYBD_Private_FunctionPrototypes
* @{
*/ 
static void  KEYBRD_Init (void);
static void  KEYBRD_Decode(uint8_t *data);

/**
* @}
*/ 

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
 #if defined   (__CC_ARM) /*!< ARM Compiler */
  __align(4) 
 #elif defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
 #elif defined (__GNUC__) /*!< GNU Compiler */
 #pragma pack(4) 
 #elif defined  (__TASKING__) /*!< TASKING Compiler */                           
  __align(4) 
 #endif /* __CC_ARM */
#endif
 
/** @defgroup USBH_HID_KEYBD_Private_Variables
* @{
*/
HID_cb_TypeDef HID_KEYBRD_cb= 
{
  KEYBRD_Init,
  KEYBRD_Decode
};
 

/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Functions
* @{
*/ 


/**
* @brief  KEYBRD_Init.
*         Initialize the keyboard function.
* @param  None
* @retval None
*/
static void  KEYBRD_Init (void)
{
  /* Call User Init*/
  USR_KEYBRD_Init();
} 

//数组元素对应关系:A  A连击  B  B连击 Select Start  上  下   左  右
//                |   |     |   |     |       |    |   |   |   |
//          手柄1:K   I     J   U    空格    回车   W   S   A   D
//                |   |     |   |     |       |    |   |   |   |
//          手柄2:3   6     2   5    空格    回车  ↑   ↓  ←  →

const u8 KEYBD_FCPAD1_TABLE[10]={0X0E,0X0C,0X0D,0X18,0X2C,0X28,0X1A,0X16,0X04,0X07};//模拟手柄1
const u8 KEYBD_FCPAD2_TABLE[10]={0X5B,0X5E,0X5A,0X5D,0X2C,0X28,0X52,0X51,0X50,0X4F};//模拟手柄2
//键盘数据解码成FC手柄数据.
//fcbuf:键盘输入数据
//mode:0,不更新键盘输入数据;
//     1,更新键盘输入数据
void KEYBRD_FCPAD_Decode(uint8_t *fcbuf,uint8_t mode)
{
	static u8 pbuf[6];		//保存最近一次fc手柄的值
	u8 j;
	u8 da1=0,db1=0;
	u8 da2=0,db2=0; 
	if(mode)
	{
		for(j=0;j<6;j++)pbuf[j]=fcbuf[j];//更新数据
	}
	for(j=0;j<6;j++)//先判断A连击键?
	{ 
		if(KEYBD_FCPAD1_TABLE[1]==pbuf[j])
		{
			fcpad.b.a=!fcpad.b.a;	//手柄1 A键取反
			da1=1; 					//标记手柄1 A连击有效
		} 
		if(KEYBD_FCPAD2_TABLE[1]==pbuf[j])
		{
			fcpad1.b.a=!fcpad1.b.a;	//手柄2 A键取反
			da2=1; 					//标记手柄2 A连击有效
		}
		if(KEYBD_FCPAD1_TABLE[3]==pbuf[j])
		{
			fcpad.b.b=!fcpad.b.b;	//手柄1 B键取反
			db1=1; 					//标记手柄1 B连击有效
		} 
		if(KEYBD_FCPAD2_TABLE[3]==pbuf[j])
		{
			fcpad1.b.b=!fcpad1.b.b;	//手柄2 B键取反
			db2=1; 					//标记手柄2 B连击有效
		} 		
	}	
	if(da1==0)fcpad.b.a=0;
	if(da2==0)fcpad1.b.a=0;
	if(db1==0)fcpad.b.b=0;
	if(db2==0)fcpad1.b.b=0;
	fcpad.b.select=0;
	fcpad1.b.select=0;
 	fcpad.b.start=0;
	fcpad1.b.start=0;
 	fcpad.b.up=0;
	fcpad1.b.up=0;
 	fcpad.b.down=0;
	fcpad1.b.down=0;
 	fcpad.b.left=0;
	fcpad1.b.left=0;
 	fcpad.b.right=0;
	fcpad1.b.right=0; 
	for(j=0;j<6;j++)//先判断A连击键?
	{ 
		if(da1==0)//手柄1,A连击没有按下
		{
			if(KEYBD_FCPAD1_TABLE[0]==pbuf[j])fcpad.b.a=1;	//手柄1 A键按下 
		}
		if(da2==0)//手柄2,A连击没有按下
		{
			if(KEYBD_FCPAD2_TABLE[0]==pbuf[j])fcpad1.b.a=1;	//手柄2 A键按下 
		}
		if(db1==0)//手柄1,B连击没有按下
		{
			if(KEYBD_FCPAD1_TABLE[2]==pbuf[j])fcpad.b.b=1;	//手柄1 B键按下 
		}
		if(db2==0)//手柄2,B连击没有按下
		{
			if(KEYBD_FCPAD2_TABLE[2]==pbuf[j])fcpad1.b.b=1;	//手柄2 B键按下 
		} 
		if(KEYBD_FCPAD1_TABLE[4]==pbuf[j])fcpad.b.select=1;	//手柄1 select按下 
		if(KEYBD_FCPAD2_TABLE[4]==pbuf[j])fcpad1.b.select=1;//手柄2 select按下 
		if(KEYBD_FCPAD1_TABLE[5]==pbuf[j])fcpad.b.start=1;	//手柄1 start按下 
		if(KEYBD_FCPAD2_TABLE[5]==pbuf[j])fcpad1.b.start=1;	//手柄2 start按下 		
		if(KEYBD_FCPAD1_TABLE[6]==pbuf[j])fcpad.b.up=1;		//手柄1 up按下 
		if(KEYBD_FCPAD2_TABLE[6]==pbuf[j])fcpad1.b.up=1;	//手柄2 up按下 
		if(KEYBD_FCPAD1_TABLE[7]==pbuf[j])fcpad.b.down=1;	//手柄1 down按下 
		if(KEYBD_FCPAD2_TABLE[7]==pbuf[j])fcpad1.b.down=1;	//手柄2 down按下 
		if(KEYBD_FCPAD1_TABLE[8]==pbuf[j])fcpad.b.left=1;	//手柄1 left按下 
		if(KEYBD_FCPAD2_TABLE[8]==pbuf[j])fcpad1.b.left=1;	//手柄2 left按下 
		if(KEYBD_FCPAD1_TABLE[9]==pbuf[j])fcpad.b.right=1;	//手柄1 right按下 
		if(KEYBD_FCPAD2_TABLE[9]==pbuf[j])fcpad1.b.right=1;	//手柄2 right按下 
	}	
//	printf("pad1:%x\r\n",fcpad);
//	printf("pad2:%x\r\n",fcpad1);
//	for(i=0;i<8;i++)
//	{
//		printf("%02x ",pbuf[i]);
//	}
//	printf("\r\n"); 
}

//键盘数据解码
static void KEYBRD_Decode(uint8_t *pbuf)
{
	static u8 fcbuf[6];//保存最近一次fc手柄的值
	u8 i;
	pbuf+=2;
	for(i=0;i<6;i++)if(pbuf[i]!=fcbuf[i])break;//不相等,说明有新的按键情况变化 
 	if(i==6)return;						//本次按键没有任何变化,直接返回
	for(i=0;i<6;i++)fcbuf[i]=pbuf[i];	//拷贝新的按键值 
	KEYBRD_FCPAD_Decode(fcbuf,1);		//更新FC手柄数据
}






















