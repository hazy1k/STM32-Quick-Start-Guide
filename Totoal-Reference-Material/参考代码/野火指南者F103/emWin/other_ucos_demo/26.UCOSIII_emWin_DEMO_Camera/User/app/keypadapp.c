#include "includes.h"
#include  "app.h"
#include "pysearch.h"
/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/ 

#define COLOR_BORDER           0xBD814F
#define COLOR_KEYPAD0          0xA8D403
#define COLOR_KEYPAD1          0xBF5C00
#define ID_BUTTON              (GUI_ID_USER + 0)

#define BUTTON_WHITH   22
#define BUTTON_HIGHT   27
#define BUTTON_SpaceX  (2+BUTTON_WHITH) 
#define BUTTON_SpaceY  (3+BUTTON_HIGHT)

/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
static TEXT_Handle ahText[2];
static WM_HWIN hEdit;
WM_HWIN KEYBOARD;
extern GUI_FONT     XBF_Font;

static uint8_t s_SelectLangue = 1;	/* 0表示中文，1表示英文 */
static uint8_t s_LeftFlag = 0;	    /* 1表示当前条目左侧有未显示汉字, 否则反之 */
static uint8_t s_RightFlag = 0;	    /* 1表示当前条目右侧有未显示汉字, 否则反之 */
static uint8_t s_CapsLock = 0;      /* 0表示小写，1表示大写 */

static uint8_t s_Key;

static unsigned char s_tempbuf[256]={0};
static char s_Textbuf[10]={0};
static char s_Hzbuf[512]={0};
static char s_Bufview[128]={0};
static uint16_t  s_Offset = 0;
static uint16_t  s_Hznum=0;          
static uint16_t  s_Hzoff=0;
uint8_t  keypadflag=0;//键盘是否在使用：1：使用中，0:无效中
static  uint8_t  editflag=0;//判断是edit还是multiedit：1:multiedit，0：edit
typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;   /* 按钮对应的小写字符 */
  const char * acLabel1;  /* 按钮对应的大写字符 */
  char         Control;   /* 按钮对应的控制位，1表示功能按钮，0表示普通按钮 */
} BUTTON_DATA;

static const BUTTON_DATA _aButtonData[] = 
{

	/* 第1排按钮 */
	{ 2,                   5, 				 	BUTTON_WHITH, 		BUTTON_HIGHT, "1" ,   	"1",  	0},//0
	{ 2+BUTTON_SpaceX,     5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "2",    	"2",  	0},
	{ 2+BUTTON_SpaceX*2,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "3",    	"3",  	0},
	{ 2+BUTTON_SpaceX*3,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "4",    	"4",  	0},
	{ 2+BUTTON_SpaceX*4,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "5",   	"5",  	0},
	{ 2+BUTTON_SpaceX*5,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "6",   	"6",  	0},
	{ 2+BUTTON_SpaceX*6,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "7",    	"7",  	0},
	{ 2+BUTTON_SpaceX*7,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "8",    	"8",  	0},
	{ 2+BUTTON_SpaceX*8,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "9",    	"9",  	0},
	{ 2+BUTTON_SpaceX*9,   5,  					BUTTON_WHITH-1, 	BUTTON_HIGHT, "0",    	"0",  	0},

	/* 第2排按钮 */
	{ 2,                   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "q" ,   	"Q",  	0},//10
	{ 2+BUTTON_SpaceX,     5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "w",    	"W",  	0},
	{ 2+BUTTON_SpaceX*2,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "e",    	"E",  	0},
	{ 2+BUTTON_SpaceX*3,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "r",    	"R",  	0},
	{ 2+BUTTON_SpaceX*4,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "t",    	"T",  	0},
	{ 2+BUTTON_SpaceX*5,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "y",    	"Y",  	0},
	{ 2+BUTTON_SpaceX*6,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "u",    	"U",  	0},
	{ 2+BUTTON_SpaceX*7,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "i",    	"I",  	0},
	{ 2+BUTTON_SpaceX*8,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "o",    	"O",  	0},
	{ 2+BUTTON_SpaceX*9,   5+BUTTON_SpaceY,  	BUTTON_WHITH-1, 	BUTTON_HIGHT, "p",    	"P",  	0},

	/* 第3排按钮 */
	{ 12,                   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "a" ,   	"A",  	0},//20
	{ 12+BUTTON_SpaceX,     5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "s",    	"S",  	0},
	{ 12+BUTTON_SpaceX*2,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "d",    	"D",  	0},
	{ 12+BUTTON_SpaceX*3,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "f",    	"F",  	0},
	{ 12+BUTTON_SpaceX*4,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "g",    	"G",  	0},
	{ 12+BUTTON_SpaceX*5,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "h",    	"H",  	0},
	{ 12+BUTTON_SpaceX*6,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "j",    	"J",  	0},
	{ 12+BUTTON_SpaceX*7,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "k",    	"K",  	0},
	{ 12+BUTTON_SpaceX*8,   5+BUTTON_SpaceY*2,  BUTTON_WHITH, 		BUTTON_HIGHT, "l",    	"L",  	0},

	/* 第4排按钮 */
	{ 2,                   5+BUTTON_SpaceY*3,  BUTTON_WHITH+5,  	BUTTON_HIGHT, "cl",   	"CL", 	1},//29
	{ 7+BUTTON_SpaceX,     5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "z",    	"Z",  	0},
	{ 7+BUTTON_SpaceX*2,   5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "x",    	"X",  	0},
	{ 7+BUTTON_SpaceX*3,   5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "c",    	"C", 	0},
	{ 7+BUTTON_SpaceX*4,   5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "v",    	"V",  	0},
	{ 7+BUTTON_SpaceX*5,   5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "b",    	"B",  	0},
	{ 7+BUTTON_SpaceX*6,   5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "n",    	"N",  	0},
	{ 7+BUTTON_SpaceX*7,   5+BUTTON_SpaceY*3,  BUTTON_WHITH, 		BUTTON_HIGHT, "m",    	"M",  	0},
	{ 7+BUTTON_SpaceX*8,   5+BUTTON_SpaceY*3,  BUTTON_WHITH*2-6, 	BUTTON_HIGHT, "Back", 	"Back", 1},//37


	/* 第5排按钮 */
	{ 2,                   5+BUTTON_SpaceY*4,  BUTTON_WHITH+7, 		BUTTON_HIGHT, "c/E",   "c/E", 	1},//38
	{ 10+BUTTON_SpaceX,    5+BUTTON_SpaceY*4,  BUTTON_WHITH, 		BUTTON_HIGHT, "_",   	"_", 	0},
	{ 10+BUTTON_SpaceX*2,  5+BUTTON_SpaceY*4,  BUTTON_WHITH, 		BUTTON_HIGHT, ".",    	".",  	0},
	{ 10+BUTTON_SpaceX*3,  5+BUTTON_SpaceY*4,  BUTTON_WHITH*3, 		BUTTON_HIGHT, " ",  	" ",	0},
	{ 7+BUTTON_SpaceX*6,   5+BUTTON_SpaceY*4,  BUTTON_WHITH,		BUTTON_HIGHT, ",",    	",",  	0},
	{ 7+BUTTON_SpaceX*7,   5+BUTTON_SpaceY*4,  BUTTON_WHITH+8, 		BUTTON_HIGHT, "Ok",    	"Ok",  	1},//43
	{ 15+BUTTON_SpaceX*8,  5+BUTTON_SpaceY*4,  BUTTON_WHITH*2-14, 	BUTTON_HIGHT, "En",    	"En",	1},//44
};
/*
*********************************************************************************************************
*	函 数 名: _cbKeyPad
*	功能说明: 回调函数
*	形    参：pMsg  指针参数
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbKeyPad(WM_MESSAGE * pMsg) 
{
	WM_HWIN    hWin;
	WM_HWIN    hButton;
	int        Id;
	int        NCode;
	int        xSize;
	int        ySize;
	unsigned char  i;
	char s_Editbuf[100]={0};
	char s_Edit[100]={0};
	hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		/* 创建所需的按钮 */
		case WM_CREATE:
			for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
			{
				hButton = BUTTON_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, 
				hWin, WM_CF_SHOW, 0, ID_BUTTON + i);
				BUTTON_SetFont(hButton, &GUI_Font16B_ASCII);
				BUTTON_SetText(hButton, _aButtonData[i].acLabel);
				BUTTON_SetTextAlign(hButton,GUI_TA_HCENTER|GUI_TA_VCENTER);
				BUTTON_SetFocussable(hButton, 0);
			}
			break;
		case WM_DELETE:
			s_Offset=0;
			s_SelectLangue = 1;
			s_CapsLock = 0;
			keypadflag=0;//使能键盘
			editflag=0;//默认是edit控件书写
			WM_DeleteWindow(ahText[0]);
			WM_DeleteWindow(ahText[1]);
		break;
		/* 绘制背景 */	
		case WM_PAINT:	
			xSize = WM_GetWindowSizeX(hWin);
			ySize = WM_GetWindowSizeY(hWin);
			GUI_SetColor(COLOR_BORDER);
			GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
			GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD1);
			break;
		
		/* 用于处理按钮的消息 */
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED:
					if (_aButtonData[Id - ID_BUTTON].acLabel) 
					{
						/* 第一步：实现字母大小写的切换 *******************************************/
						if(Id==(ID_BUTTON+29))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel,"CL", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							/* 当前是小写，切换到大写，必须在英文模式下实现大小写切换*/
							if((s_CapsLock == 0)&&(s_SelectLangue == 1))
							{
								for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
								{
									BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON + i), _aButtonData[i].acLabel1);
								}
								s_CapsLock = 1;
							}
							else if((s_CapsLock == 1)&&(s_SelectLangue == 1))
							{
								for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
								{
									BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON + i), _aButtonData[i].acLabel);
								}
								s_CapsLock = 0;
							}

							/* 功能键处理完要退出 */
							break;
						}

						/* 第二步： 实现中英文的切换 ********************************************/
						if(Id==(ID_BUTTON+38))
//						if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "C/e", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0
//						|strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "c/E", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							/* 当前是英文，必须是小写字母的时候才可以切换  */
							if((s_SelectLangue == 1)&&(s_CapsLock == 0))
							{
								s_SelectLangue = 0;
								BUTTON_SetText(WM_GetDialogItem(hWin, Id), "C/e");
							}
							else if((s_SelectLangue == 0)&&(s_CapsLock == 0))
							{
								s_SelectLangue = 1;
								BUTTON_SetText(WM_GetDialogItem(hWin, Id), "c/E");
							}
							WM_HideWindow(ahText[0]);
							WM_HideWindow(ahText[1]);
							WM_InvalidateWindow(ahText[0]);
							WM_InvalidateWindow(ahText[1]);

							/* 功能键处理完要退出 */
							break;
						}

						/* 第三步：字母大小写的识别 ********************************************/
						if(s_CapsLock == 0)
						{
							s_Key = _aButtonData[Id - ID_BUTTON].acLabel[0];
						}
						/* 大写字母 */
						else
						{
							s_Key = _aButtonData[Id - ID_BUTTON].acLabel1[0];
						}

						/* 第四步：中文处理 **************************************************/
						if(s_SelectLangue == 0)
						{		
							int j;
							char *ptr;

							/* 按键1按下***********************************************************/
							if(s_Key == '1')
							{
								if(!s_Offset)break;
								/* 如果左侧有未显示的汉字 */
								if(s_LeftFlag == 1)
								{
									/* 因为每行最多可以显示6个汉字，所以出现数字6的地方很多 */
									if(s_Hzoff%6 == 1)
									{
										s_Hzoff -= 6;	
									}
									else
									{
										s_Hzoff = s_Hzoff - (s_Hzoff % 6)+1;	
									}
									s_Hzoff -= 6;

									s_RightFlag = 1;

									/* 如果偏移地址之前还有6个以上的汉字 */
									if(s_Hzoff > 7)
									{
										s_Bufview[0] = '1';
										s_Bufview[1] = '<';
										s_Bufview[2] = ' ';
										
										for (j = 0; j < 6;j++)
										{
											s_Bufview[j*5+3] = j+'2';
											s_Bufview[j*5+4] = s_Hzbuf[(s_Hzoff)*3];
											s_Bufview[j*5+5] = s_Hzbuf[(s_Hzoff)*3+1];
											s_Bufview[j*5+6] = s_Hzbuf[(s_Hzoff)*3+2];
											s_Bufview[j*5+7] = ' ';
											s_Hzoff++;
										}

										s_Bufview[38] = '8';
										s_Bufview[39] = '>';
										s_Bufview[40] = 0;
										s_LeftFlag = 1;
									}
									/* 如果偏移地址之前汉字数小于7 */
									else 
									{	
										s_LeftFlag = 0;
										s_Hzoff=0;
										for (j = 0; j < 7;j++)
										{
											s_Bufview[j*5+0] = j+'1';
											s_Bufview[j*5+1] = s_Hzbuf[(s_Hzoff)*3];
											s_Bufview[j*5+2] = s_Hzbuf[(s_Hzoff)*3+1];
											s_Bufview[j*5+3] = s_Hzbuf[(s_Hzoff)*3+2];
											s_Bufview[j*5+4] = ' ';
											s_Hzoff++;
										}

										s_Bufview[35] = '8';
										s_Bufview[36] = '>';
										s_Bufview[37] = '\0';
									}
								}
								/* 左侧没有汉字，直接显示第一个汉字 */
								else
								{
									s_Textbuf[0] = s_Hzbuf[0];
									s_Textbuf[1] = s_Hzbuf[1];
									s_Textbuf[2] = s_Hzbuf[2];
									s_Textbuf[3] = '\0';
									if(editflag)MULTIEDIT_AddText(hEdit, s_Textbuf);
									else
									{								
										EDIT_GetText(hEdit,s_Editbuf,100);
										sprintf(s_Edit,"%s%s",s_Editbuf,s_Textbuf);
										EDIT_SetText(hEdit,s_Edit);		
										//EDIT_SetText(hEdit,s_Textbuf);										
									}
									s_Bufview[0]='\0';
									while(--s_Offset)
										s_tempbuf[s_Offset]= '\0';
									TEXT_SetText(ahText[0],s_Bufview);
									WM_HideWindow(ahText[0]);
									WM_HideWindow(ahText[1]);
								}								
								TEXT_SetText(ahText[1],s_Bufview);
							}
							/* 按键8按下***********************************************************/
							else if(s_Key == '8')
							{		
								if(!s_Offset)break;
								if(s_RightFlag == 1)
								{
									s_LeftFlag = 1;
									s_Bufview[0] = '1';
									s_Bufview[1] = '<';
									s_Bufview[2] = ' ';		
									if (s_Hznum <= (s_Hzoff + 6))
									{
										int k = s_Hznum - s_Hzoff;
										for (j = 0; j < k;j++)
										{
											s_Bufview[j*5+3] = j+'2';
											s_Bufview[j*5+4] = s_Hzbuf[(s_Hzoff)*3];
											s_Bufview[j*5+5] = s_Hzbuf[(s_Hzoff)*3+1];
											s_Bufview[j*5+6] = s_Hzbuf[(s_Hzoff)*3+2];
											s_Bufview[j*5+7] = ' ';
											s_Hzoff++;
										}
										s_Bufview[k*5+3] = 0;	
										s_RightFlag = 0;
									}
									else
									{
										for (j = 0; j < 6;j++)
										{
											s_Bufview[j*5+3] = j + '2';
											s_Bufview[j*5+4] = s_Hzbuf[s_Hzoff*3];
											s_Bufview[j*5+5] = s_Hzbuf[s_Hzoff*3+1];
											s_Bufview[j*5+6] = s_Hzbuf[s_Hzoff*3+2];
											s_Bufview[j*5+7] = ' ';
											s_Hzoff++;
										}

										s_Bufview[38] = '8';
										s_Bufview[39] = '>';
										s_Bufview[40] = 0;

										s_RightFlag = 1;
									}
									TEXT_SetText(ahText[1],s_Bufview);
								}
							}
							/* 按键2-7按下***********************************************************/
							else if((s_Key>='2')&&(s_Key<='7'))
							{
								if(!s_Offset)break;
								if(s_LeftFlag == 1)
								{
									if(s_Hzoff%6 == 1)
									{
										s_Textbuf[0]=s_Hzbuf[(s_Key-'2' + s_Hzoff - 6)*3];
										s_Textbuf[1]=s_Hzbuf[(s_Key-'2' + s_Hzoff - 6)*3+1];
										s_Textbuf[2]=s_Hzbuf[(s_Key-'2' + s_Hzoff - 6)*3+2];
									}
									else
									{
										s_Textbuf[0]=s_Hzbuf[(s_Key-'2' + (s_Hzoff/6)*6+1)*3];
										s_Textbuf[1]=s_Hzbuf[(s_Key-'2' + (s_Hzoff/6)*6+1)*3+1];
										s_Textbuf[2]=s_Hzbuf[(s_Key-'2' + (s_Hzoff/6)*6+1)*3+2];
									}
									
									s_Textbuf[3] = '\0';				
								}
								else
								{
									s_Textbuf[0] = s_Hzbuf[(s_Key-'1')*3];
									s_Textbuf[1] = s_Hzbuf[(s_Key-'1')*3+1];
									s_Textbuf[2] = s_Hzbuf[(s_Key-'1')*3+2];
									s_Textbuf[3] = '\0';
								}
								if(editflag)MULTIEDIT_AddText(hEdit, s_Textbuf);
								else
								{								
										EDIT_GetText(hEdit,s_Editbuf,100);
										sprintf(s_Edit,"%s%s",s_Editbuf,s_Textbuf);
										EDIT_SetText(hEdit,s_Edit);		
										//EDIT_SetText(hEdit,s_Textbuf);										
								}
								s_Bufview[0]='\0';
								while(--s_Offset)
									s_tempbuf[s_Offset]= '\0';
								TEXT_SetText(ahText[0],s_Bufview);
								TEXT_SetText(ahText[1],s_Bufview);
								WM_HideWindow(ahText[0]);
								WM_HideWindow(ahText[1]);
							}
							/* 任意其它按键 ***************************************************/
							else
							{			
								/* 在还没有输入汉字的情况下，对于功能键的处理 */
								if(s_Offset == 0)
								{
									/* 点下回车键 */
									if(Id==(ID_BUTTON+44))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "En", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
									{
										if(editflag)MULTIEDIT_AddKey(hEdit, GUI_KEY_ENTER);
										break;
									}
									/* 点下回格键 */
									if(Id==(ID_BUTTON+37))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Back", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
									{
										if(editflag)MULTIEDIT_AddKey(hEdit, GUI_KEY_BACKSPACE);
										else EDIT_AddKey(hEdit,GUI_KEY_BACKSPACE);
										break;
									}
									/* 点击确认键 */
									if(Id==(ID_BUTTON+43))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Ok", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
									{
//										WM_HideWindow(hWin);
//										WM_InvalidateWindow(hWin);
										WM_DeleteWindow(hWin);
										keypadflag=0;
										break;
									}
								}
								
								TEXT_SetBkColor(ahText[0],GUI_LIGHTGRAY);							
								/* 点击回格键*/
								if(Id==(ID_BUTTON+37))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Back", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
								{
									/* 实现汉字拼音的逐个删除 */
									if(s_Offset > 0)
									{
										s_Offset--;
										s_tempbuf[s_Offset]= '\0';
									}
									
									/* 拼音删除完毕，退出汉字和拼音的显示 */
									if(s_Offset == 0)
									{
										WM_HideWindow(ahText[0]);
										WM_HideWindow(ahText[1]);
										WM_InvalidateWindow(ahText[0]);
										WM_InvalidateWindow(ahText[1]);
										break;
									}
								}
								else
								{
									/* 除了上面的两个控制键，输入其它的控制键退出显示 */
									if(_aButtonData[Id - ID_BUTTON].Control == 1)
									{
										break;
									}
									
									/* 如果是其余拼音，装入缓存 */
									s_tempbuf[s_Offset++]= s_Key;
									
									WM_ShowWindow(ahText[0]);
									WM_ShowWindow(ahText[1]);
								}

								TEXT_SetText(ahText[0], (const char *)s_tempbuf);
								ptr = PYSearch(s_tempbuf, (int *)&s_Hznum);

								if (ptr != '\0')
								{
									for (j = 0; j < s_Hznum*3; j++)
									{
										s_Hzbuf[j] = *ptr++;
									}
									s_Hzbuf[s_Hznum*3] = '\0';						
									TEXT_SetBkColor(ahText[1],GUI_LIGHTGRAY);

									if (s_Hznum > 0)
									{	
										//printf("s_Hznum = %d\r\n", s_Hznum);
										s_Hzoff = 0;
										s_LeftFlag = 0;	//1表示当前条目左侧有未显示汉字

										if ((s_Hzoff +7) >= s_Hznum )
										{
											int k = s_Hznum - s_Hzoff;									
											for (j = 0; j < k;j++)
											{
												s_Bufview[j*5+0] = j+'1';
												s_Bufview[j*5+1] = s_Hzbuf[s_Hzoff*3];
												s_Bufview[j*5+2] = s_Hzbuf[s_Hzoff*3+1];
												s_Bufview[j*5+3] = s_Hzbuf[s_Hzoff*3+2];
												s_Bufview[j*5+4] = ' ';
												s_Hzoff++;
											}
											s_Bufview[k*5] = '\0';
										}
										else
										{
											for (j = 0; j < 7;j++)
											{
												s_Bufview[j*5+0] = j + '1';
												s_Bufview[j*5+1] = s_Hzbuf[s_Hzoff*3];
												s_Bufview[j*5+2] = s_Hzbuf[s_Hzoff*3+1];
												s_Bufview[j*5+3] = s_Hzbuf[s_Hzoff*3+2];
												s_Bufview[j*5+4] = ' ';
												s_Hzoff++;
											}

											s_Bufview[35] = '8';
											s_Bufview[36] = '>';
											s_Bufview[37] = '\0';

											s_RightFlag = 1;	//1表示当前条目右侧有未显示汉字
										}
									}
									TEXT_SetText(ahText[1],s_Bufview);
								}
								else
								{
									TEXT_SetText(ahText[1],"");
								}
							}
						}
					/* 字符显示*******************************************************************/
					else
					{
						/* 点击回车键 */
						if(Id==(ID_BUTTON+44))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "En", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							if(editflag)MULTIEDIT_AddKey(hEdit, GUI_KEY_ENTER);
							break;
						}
						
						/* 点击回格键 */
						if(Id==(ID_BUTTON+37))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Back", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							if(editflag)MULTIEDIT_AddKey(hEdit, GUI_KEY_BACKSPACE);
							else EDIT_AddKey(hEdit,GUI_KEY_BACKSPACE);
							break;
						}
						/* 点击确认键 文字输入完成，删除键盘窗口*/
						if(Id==(ID_BUTTON+43))//if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Ok", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							WM_DeleteWindow(hWin);
							keypadflag=0;
							break;
						}

						/* 其它按键，但不能是功能键 */
						if(_aButtonData[Id - ID_BUTTON].Control == 0)
						{
							s_Textbuf[0] = s_Key;
							s_Textbuf[1] = '\0';
							if(editflag)MULTIEDIT_AddText(hEdit, s_Textbuf);
							else EDIT_AddKey(hEdit,s_Textbuf[0]);
						}
					}
				}
				break;
			}
	}
}
/*
*********************************************************************************************************
*	函 数 名: MainAPP
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void keypadapp(uint16_t message,uint8_t IsEdit)
{
	//printf("pend size=%d,hedit[0]=%d,hedit[1]=%d\n",GUI_COUNTOF(message),message[0],message[1]);
	s_Offset=0;
	s_SelectLangue = 1;
	s_CapsLock = 0;
	keypadflag=1;//使能键盘
	editflag=0;//默认是edit控件书写
	/***************************设置默认字体**********************************/
	GUI_UC_SetEncodeUTF8();
    //GUI_SetDefaultFont(&XBF_Font);
	/*************************************************************************/
	
	/* 创建键盘窗口 */
	KEYBOARD=WM_CreateWindowAsChild(0, 320-155, 240, 155, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, _cbKeyPad, 0);

	/* 两个文本控件用于汉字输入时，拼音和汉字的显示 */
	ahText[0] = TEXT_Create( 0, 125, 240, 20, GUI_ID_TEXT0, WM_CF_SHOW | WM_CF_STAYONTOP,"",TEXT_CF_LEFT | TEXT_CF_BOTTOM);
	TEXT_SetFont(ahText[0],&XBF_Font);

	ahText[1] = TEXT_Create( 0, 145, 240, 20, GUI_ID_TEXT1, WM_CF_SHOW | WM_CF_STAYONTOP,"",TEXT_CF_LEFT | TEXT_CF_BOTTOM);
	TEXT_SetFont(ahText[1],&XBF_Font);	
	TEXT_SetTextColor(ahText[1],GUI_RED);
	//控件句柄
	hEdit = message;
	//当前编辑的是multiedit控件
	if(IsEdit==0)
		editflag=1;
	
	/* 设置聚焦 */
	WM_SetFocus(hEdit);
	while(keypadflag)
	{
		GUI_Delay(10);
	}
}
