#include "includes.h"
#include  "app.h"

#define BK_COLOR       GUI_BLACK
#define MIDWIN_COLOR   0X303030
#define ICON_COLOR     GUI_LIGHTCYAN
#define TEXT_COLOR		 GUI_WHITE
/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/
#define ICONVIEW_Width     48   /* 控件ICONVIEW的宽 */  
#define ICONVIEW_Height    48   /* 控件ICONVIEW的高，包括图标的Y方向间距 */  
#define ICONVIEW_XSpace    10   /* 控件ICONVIEW的X方向间距 */ 
#define ICONVIEW_YSpace    12   /* 控件ICONVIEW的Y方向间距 */ 
#define ICONVIEW_XPos	     3    /* 控件ICONVIEW的X方向调整距离*/
#define BOTWIN_YSpace	     62   /* 屏幕下方部分窗口Y方向间距*/
#define MIDWIN_yPos		     20   /* 中间窗口Y方向起始位置即顶部文字可用高度*/
#define MIDWIN_xPos		     0 	  /* 中间窗口X方向起始位置*/

/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
WM_HWIN hFrameClose=0;
GUI_XBF_DATA XBF_Data;
GUI_FONT     XBF_Font;

/*时间结构体*/
extern struct rtc_time systmtime;
extern char TimeDisplay;

uint8_t UserApp_Flag=0;//任务界面标志

uint8_t Flag_ICON000  = 0;   /* ICONVIEW控件按下的标志，0表示未按下，1表示按下 */
uint8_t Flag_ICON001  = 0;

uint8_t Flag_ICON100  = 0;
uint8_t Flag_ICON101  = 0;
uint8_t Flag_ICON102  = 0;
uint8_t Flag_ICON103  = 0;

uint8_t Flag_ICON104  = 0;
uint8_t Flag_ICON105  = 0;
uint8_t Flag_ICON106  = 0;
uint8_t Flag_ICON107  = 0;

uint8_t Flag_ICON108  = 0;
uint8_t Flag_ICON109  = 0;
uint8_t Flag_ICON110  = 0;
//uint8_t Flag_ICON111  = 0;


/* 用于桌面ICONVIEW图标的创建 */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

typedef struct WIN_PARA{			//窗口使用到的用户定义参数，方便在回调函数中使用
	int xSizeLCD, ySizeLCD;			//LCD屏幕尺寸
	int xPosWin,  yPosWin;			//窗口的起始位置
	int xSizeWin, ySizeWin;			//窗口尺寸	
	int xSizeBM,  ySizeBM;
	int ySizeBotWin;
	
	WM_HWIN hWinMid;				//主显示窗的句柄（中部）
	
}WIN_PARA;
WIN_PARA WinPara;			//用户使用的窗口额外数据

/* 用于桌面ICONVIEW0图标的创建 */
static const BITMAP_ITEM _aBitmapItem0[] = {
	{&bmPhone,     "Phone" 		},
	{&bmmessages,  "Message"	}
};
/* 用于桌面ICONVIEW1图标的创建 */
static const BITMAP_ITEM _aBitmapItem1[] = {
  {&bmkey,  	 	"KEY"		    },
  {&bmrgbled,	 	"Breathing"	}, 
  {&bmadc,  	 	"ADC"				},
	{&bmeeprom,   "EEPROM" 		},
	
  {&bmclock,    "Clock" 		},
	{&bmusb,  	  "USB"		    },
	{&bmwifi,			"Wifi"		  },	
	{&bmWeather,  "Humiture"  }, 
	
  {&bmcamera,   "Camera"		}, 
	{&bmcalculator,"Calculator"},
	{&bmAPP1,    	"UserApp" 	},	
	{&bmAPP2,    	"UserApp" 	},	
};

//void FUN_ICON000Clicked(void)  {printf("FUN_ICON000Clicked\n");}
//void FUN_ICON001Clicked(void)  {printf("FUN_ICON001Clicked\n");}

//void FUN_ICON100Clicked(void)  {printf("FUN_ICON100Clicked\n");}
//void FUN_ICON101Clicked(void)  {printf("FUN_ICON101Clicked\n");}
//void FUN_ICON102Clicked(void)  {printf("FUN_ICON102Clicked\n");}
//void FUN_ICON103Clicked(void)  {printf("FUN_ICON103Clicked\n");}

//void FUN_ICON104Clicked(void)  {printf("FUN_ICON104Clicked\n");}
//void FUN_ICON105Clicked(void)  {printf("FUN_ICON105Clicked\n");}
//void FUN_ICON106Clicked(void)  {printf("FUN_ICON106Clicked\n");}
//void FUN_ICON107Clicked(void)  {printf("FUN_ICON107Clicked\n");}

//void FUN_ICON108Clicked(void)  {printf("FUN_ICON108Clicked\n");}
//void FUN_ICON109Clicked(void)  {printf("FUN_ICON109Clicked\n");}
//void FUN_ICON110Clicked(void)  {printf("FUN_ICON110Clicked\n");}
//void FUN_ICON111Clicked(void)  {printf("FUN_ICON111Clicked\n");}


/*
*********************************************************************************************************
*	函 数 名: _cbBkWindow
*	功能说明: 桌面窗口的回调函数 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	WM_HWIN hText;
	char text_buffer[20]={0};
	int NCode, Id;
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW0:
					switch (NCode) 
					{
						/* ICON控件点击消息 */
						case WM_NOTIFICATION_CLICKED:							
							break;
						
						/* ICON控件释放消息 */
						case WM_NOTIFICATION_RELEASED: 
							UserApp_Flag = 1;
							/* 打开相应选项 */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{
								/* Phone *******************************************************************/
								case 0:	
									Flag_ICON000 = 1;
									FUN_ICON000Clicked();			
									break;	
								
								/* Message ***********************************************************************/
								case 1:
									Flag_ICON001 = 1;
									FUN_ICON001Clicked();
									break;
								}	
						 break;
					}
				break;
			}			
			break;
		case WM_TIMER://处理时间显示的信息，每秒钟修改一次时间
			if(!TimeDisplay||UserApp_Flag)
			{
				WM_RestartTimer(pMsg->Data.v, 250);
				break;
			}
			/* 获取text句柄 */      
			hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
			
			/* 转换rtc值至北京时间 */
			RTC_TimeCovr(&systmtime);
  
			/* 转换成字符串 */
			sprintf(text_buffer,"%02d:%02d:%02d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
			/* 输出时间 */
			TEXT_SetText(hText,text_buffer);
			WM_RestartTimer(pMsg->Data.v, 250);
		break;
		/* 重绘消息*/
		case WM_PAINT:		
				GUI_SetBkColor(BK_COLOR);
				GUI_Clear();		
				ICONVIEW_SetSel(WM_GetDialogItem(WinPara.hWinMid, GUI_ID_ICONVIEW1),-1);
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbMidWin
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbMidWin(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW1:
					switch (NCode) 
					{
						/* ICON控件点击消息 */
						case WM_NOTIFICATION_CLICKED:							
							break;
						
						/* ICON控件释放消息 */
						case WM_NOTIFICATION_RELEASED: 
							UserApp_Flag = 1;
							/* 打开相应选项 */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{								
								/* KEY  ******************************************************************/
								case 0:	
									Flag_ICON100 = 1;
									FUN_ICON100Clicked();			
									break;	
								
								/* Breathing ***********************************************************************/
								case 1:
									Flag_ICON101 = 1;
									FUN_ICON101Clicked();
									break;
								
								/* ADC *********************************************************************/
								case 2:
									Flag_ICON102 = 1;
									FUN_ICON102Clicked();	
									break;
								
								/* EEPROM ********************************************************************/
								case 3:
									Flag_ICON103 = 1;
									FUN_ICON103Clicked();
									break;
								
								/* Clock **********************************************************************/
								case 4:
									Flag_ICON104 = 1;
									FUN_ICON104Clicked();
									break;
								
								/* Music ********************************************************************/
								case 5:
									Flag_ICON105 = 1;
									FUN_ICON105Clicked();
									break;
								
								/* Recorder ******************************************************************/
								case 6:					
									Flag_ICON106 = 1;
									FUN_ICON106Clicked();
									break;
								
								/* USB *******************************************************************/
								case 7:
									Flag_ICON107 = 1;
									FUN_ICON107Clicked();
									break;
								
								/* Wifi ******************************************************************/
								case 8:
									Flag_ICON108 = 1;
									FUN_ICON108Clicked();
									break;
								 
								 /* Humiture ******************************************************************/
								case 9:
									Flag_ICON109 = 1;
									FUN_ICON109Clicked();
									break;
								 
								 /* Camera ******************************************************************/
								case 10:
									Flag_ICON110 = 1;
									FUN_ICON110Clicked();
									break;
								 
								/* Calculator*****************************************************************/
								case 11:
									Flag_ICON110 = 1;
									FUN_ICON110Clicked();
//									Flag_ICON111 = 1;
//									FUN_ICON111Clicked();
									break;
								default:break;
								}
							 break;
						}
					break;					
			}			
			break;
			
		/* 重绘消息*/
		case WM_PAINT:
			{
				GUI_SetBkColor(MIDWIN_COLOR);
				GUI_Clear();
			}			
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
  * @brief  CreateTopWin，创建顶部的窗口
  * @param  none
  * @retval none
  */
static void CreateTopWin(void)
{
	WM_HWIN hText;
	/* 顶部的 "wildfire OS "文本 */
	hText = TEXT_CreateEx(0, 2, 60 , 16, WM_HBKWIN, WM_CF_SHOW, GUI_TA_LEFT|TEXT_CF_VCENTER, GUI_ID_TEXT0, "Wildfire OS");
	TEXT_SetFont(hText, GUI_FONT_10_ASCII);
	TEXT_SetTextColor(hText,TEXT_COLOR);
	
	/* 状态栏的时间显示文本 */
	hText = TEXT_CreateEx(WinPara.xSizeLCD/2-25,2,50,16,WM_HBKWIN,WM_CF_SHOW,GUI_TA_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT1,"11:56:00");
	//TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,TEXT_COLOR);
	TEXT_SetFont(hText,GUI_FONT_8_ASCII);
}
/**
  * @brief  CreateMidWin，创建中间的窗口
  * @param  none
  * @retval none
  */
static void CreateMidWin(void)
{
	uint8_t i=0;
	WM_HWIN MIDWIN;
	WinPara.hWinMid= WM_CreateWindowAsChild(
											WinPara.xPosWin,											
											WinPara.yPosWin,
											WinPara.xSizeWin,
											WinPara.ySizeWin,	
											WM_HBKWIN, 
											WM_CF_SHOW, 
											_cbMidWin, 
											sizeof(WIN_PARA *)
											);
/*-------------------------------------------------------------------------------------*/
					/*在指定位置创建指定尺寸的ICONVIEW1 小工具*/
					MIDWIN=ICONVIEW_CreateEx(
											 ICONVIEW_XPos, 					/* 小工具的最左像素（在父坐标中）*/
											 0, 								/* 小工具的最上像素（在父坐标中）*/
											 WinPara.xSizeLCD-ICONVIEW_XPos,	/* 小工具的水平尺寸（单位：像素）*/
											 3*(WinPara.ySizeBM+20), 			/* 小工具的垂直尺寸（单位：像素）*/
											 WinPara.hWinMid, 				    /* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
											 WM_CF_SHOW | WM_CF_HASTRANS,       /* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */ 
											 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* 默认是0，如果不够现实可设置增减垂直滚动条 */
											 GUI_ID_ICONVIEW1, 			        /* 小工具的窗口ID */
											 ICONVIEW_Width, 				    /* 图标的水平尺寸 */
											 ICONVIEW_Height+10);				/* 图标的垂直尺寸 */
											 
		/* 向ICONVIEW 小工具添加新图标 */
		for (i = 0; i < GUI_COUNTOF(_aBitmapItem1); i++) 
		{
			ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem1[i].pBitmap,_aBitmapItem1[i].pText);
			ICONVIEW_SetTextColor(MIDWIN,i,TEXT_COLOR);
		}	
		/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
		ICONVIEW_SetBkColor(MIDWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
		/* 设置字体 */
		ICONVIEW_SetFont(MIDWIN, &GUI_Font10_ASCII);
		/* 设置初始选择的图标为 -1 (表示尚未选择)*/
		ICONVIEW_SetSel(MIDWIN,-1);
		/* 设置图标在x 或y 方向上的间距。*/
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_X, ICONVIEW_XSpace);
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_Y, ICONVIEW_YSpace);
	//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
		/* 设置对齐方式 在5.22版本中最新加入的 */
		ICONVIEW_SetIconAlign(MIDWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
}

/**
  * @brief  CreateBotWin，创建底部的窗口
  * @param  none
  * @retval none
  */
static void CreateBotWin(void)
{
	uint8_t i;
	WM_HWIN BOTWIN;
	/*在指定位置创建指定尺寸的ICONVIEW 小工具*/
	BOTWIN=ICONVIEW_CreateEx(
							 62,                					      /* 小工具的最左像素（在父坐标中）*/
							 320-62, 								                /* 小工具的最上像素（在父坐标中）*/
							 116,                               /* 小工具的水平尺寸（单位：像素）*/
							 WinPara.ySizeBotWin,				        /* 小工具的垂直尺寸（单位：像素）*/
								WM_HBKWIN, 				          /* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */ 
							 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* 默认是0，如果不够现实可设置增减垂直滚动条 */
							 GUI_ID_ICONVIEW0, 			            /* 小工具的窗口ID */
							 ICONVIEW_Width, 				            /* 图标的水平尺寸 */
							 ICONVIEW_Height);					        /* 图标的垂直尺寸 */
	/* 向ICONVIEW 小工具添加新图标 */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem0); i++) 
	{
		ICONVIEW_AddBitmapItem(BOTWIN, _aBitmapItem0[i].pBitmap,"");
	}	
	/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
	ICONVIEW_SetBkColor(BOTWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
	/* 设置字体 */
//	ICONVIEW_SetFont(hWinicon0, &GUI_Font10_ASCII);
	/* 设置初始选择的图标为 -1 (表示尚未选择)*/
	ICONVIEW_SetSel(BOTWIN,-1);
	/* 设置图标在x 或y 方向上的间距。*/
	ICONVIEW_SetSpace(BOTWIN, GUI_COORD_X, ICONVIEW_XSpace);
//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
	/* 设置对齐方式 在5.22版本中最新加入的 */
	ICONVIEW_SetIconAlign(BOTWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
}
/*
*********************************************************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*********************************************************************************************************
*/
static int _cbGetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
	SPI_FLASH_BufferRead(pBuffer,60*4096+Offset,NumBytes);
	return 0;
}

static void _CreateXBF(void) 
{
	//
	// Create XBF font
	//
	GUI_XBF_CreateFont(&XBF_Font,       // Pointer to GUI_FONT structure in RAM
					   &XBF_Data,         // Pointer to GUI_XBF_DATA structure in RAM
					   GUI_XBF_TYPE_PROP,//GUI_XBF_TYPE_PROP_EXT, 		// Font type to be created
					   _cbGetData,        // Pointer to callback function
					   0);        // Pointer to be passed to GetData function
}

/*
*********************************************************************************************************
*	函 数 名: UserAPP
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void UserAPP(void)
{
	UserApp_Flag=0;
	//准备建立3个窗口，以下是使用到的用户定义参数，方便在回调函数中使用
	WinPara.xSizeLCD = LCD_GetXSize();				//LCD屏幕尺寸
	WinPara.ySizeLCD = LCD_GetYSize();				//LCD屏幕尺寸
	WinPara.xSizeBM  = ICONVIEW_Width;				//图标宽度
	WinPara.ySizeBM  = ICONVIEW_Height;				//图标高度
	WinPara.ySizeBotWin=BOTWIN_YSpace;				//界面下方窗口高度
	WinPara.xPosWin	 = MIDWIN_xPos;							//窗口的起始位置
	WinPara.yPosWin  = MIDWIN_yPos;							//窗口的起始位置
	WinPara.xSizeWin = WinPara.xSizeLCD;						//窗口尺寸
	WinPara.ySizeWin = WinPara.ySizeLCD-WinPara.ySizeBotWin-WinPara.yPosWin;	//窗口尺寸	
	
	/***************************设置皮肤色*****************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	/***************************设置默认字体**********************************/
	_CreateXBF();
	GUI_UC_SetEncodeUTF8();
  GUI_SetDefaultFont(&XBF_Font);
	/*************************************************************************/
	/* 创建三个窗口 状态栏、控制栏、主窗口*/	
	CreateBotWin();
	GUI_Delay(10);
	CreateMidWin();
	CreateTopWin();
	GUI_Delay(50);
	WM_BringToTop(WM_GetDialogItem(WM_HBKWIN, GUI_ID_ICONVIEW0));
	
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	WM_CreateTimer(WM_HBKWIN, 0, 250, 0);
	LCD_BK_EN;
	while(1)
	{
		GUI_Delay(5);
	}
}
