#include "emXGUI.h"
#include "./clock/GUI_CLOCK_DIALOG.h"
#include "x_libc.h"
#include <stdlib.h>
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./clock/rtc/bsp_rtc.h"

#define ICON_BTN_NUM     3     // 按钮数量
#define ICON_TEXT_NUM   (1 + ICON_BTN_NUM)    // 文本数量

//#define CLOCK_BACK_COLOR     230, 230, 230
#define CLOCK_BACK_COLOR     208, 208, 240//188, 213, 253

struct
{
	uint8_t page;    // 当前设置页面
	uint8_t dial;    // 选中的表盘
}Set_Start;

uint8_t clock_dial = 0;    // 表盘
static HWND clock_hwnd;

const WCHAR Week_List[][4] = {{L"星期日"}, {L"星期一"}, {L"星期二"}, {L"星期三"}, {L"星期四"}, {L"星期五"}, {L"星期六"}};
const uint8_t month_list[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    // 记录每月的最多天数

//图标管理数组
const clock_icon_t clock_icon[] = {

  /* 按钮 */
  {L"O",              {286, 10,  23,  23},  ID_CLOCK_EXIT},            // 0. 退出
  {L"2019年",         {182, 82, 115, 115},  ID_CLOCK_DATE},            // 1. 日期（年月日周）
  {L" ",              {45, 82, 115, 115},  ID_CLOCK_TIME},            // 2. 表盘显示区域
  
  /* 文本 */
  {L"时钟&日期",      {50,   0, 220,  40},  ID_CLOCK_TITLE},           // 3. 标题
  
  /************ 设置窗口控件 **************/
  /* 按钮 */
  {L"F",              {  0,  5,  24,  24},  ID_CLOCK_WAIVE},           // 4. 放弃设置
  {L"完成",           {123, 197, 71,  30},  ID_CLOCK_OK},              // 5. 完成设置

  /* 文本 */
  {L"设置时间",       {50,  0,  220,  40},  ID_CLOCK_SETTITLE},        // 6. 设置窗口标题
  {L"00:00",          {106, 46, 132,  22},  ID_CLOCK_SETTIME},         // 7. 设置窗口标题
  {L"2000年01月01日", {106, 46, 132,  22},  ID_CLOCK_SETDATE},         // 8. 设置窗口标题

  /* 时钟&日历选择列表 */
  {L" ",              {85, 68, 82, 124},  ID_CLOCK_SetHour},         // 9.  设置小时的列表
  {L" ",              {166, 68, 82, 124},  ID_CLOCK_SetMinute},       // 10. 设置分钟的列表

  {L" ",              {85, 68, 55, 124},  ID_CLOCK_SetYear},         // 11. 设置年的列表
  {L" ",              {139, 68, 55, 124},  ID_CLOCK_SetMonth},        // 12. 设置月的列表
  {L" ",              {194, 68, 55, 124},  ID_CLOCK_SetDate},         // 13. 设置日的列表
 
};

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

static void waive_btn_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;
  WCHAR wbuf[128];

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));
  }
  else
  { //按钮是弹起状态
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  SetFont(hdc, controlFont_24);
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
  
  DeleteFont(controlFont_24);
}

static void btn_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  WCHAR wbuf[128];
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 66, 254, 255));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    OffsetRect(&rc, 1, 1);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  { //按钮是弹起状态
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  InflateRect(&rc, -5, -5);
  SetBrushColor(hdc, MapRGB(hdc, 13, 148, 214));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));
  
  EnableAntiAlias(hdc, FALSE);
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

/*
 * @brief  重绘列表
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void _draw_listbox(HDC hdc,HWND hwnd,COLOR_RGB32 text_c,COLOR_RGB32 back_c,COLOR_RGB32 sel_c, int fontsize)
{

	RECT rc,rc_cli;
	int i,count;
	WCHAR wbuf[128];


	GetClientRect(hwnd,&rc_cli);

	SetBrushColor(hdc,MapRGB888(hdc,back_c));
	FillRect(hdc,&rc_cli);

	SetTextColor(hdc,MapRGB888(hdc,text_c));

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
  
  HFONT controlFont_16;
  controlFont_16 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_16);

  if (fontsize == 16)
  {
    SetFont(hdc,controlFont_16);
  }
  else
  {
    SetFont(hdc,defaultFont);
  }

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > rc_cli.h)
		{
			break;
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}
  
  DeleteFont(controlFont_16);
}

static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc_m;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//定义一个中间的矩形．
	rc_m.w =ds->rc.w;
	rc_m.h =25;
	rc_m.x =0;
	rc_m.y =(ds->rc.h-rc_m.h)>>1;

	//一个listbox绘到hdc0中．
	_draw_listbox(hdc,hwnd,RGB888(168,168,168),RGB888(250,250,250),RGB888(10,100,100), 12);

	//一个listbox绘到hdc1中．
//	_draw_listbox(hdc1,hwnd,RGB888(236,126,65),RGB888(232,232,232),RGB888(150,200,220), 16);

	SetPenColor(hdc,MapRGB(hdc,0,0,0));
  HLine(hdc, rc_m.x, rc_m.y, rc_m.x + rc_m.w);
  HLine(hdc, rc_m.x, rc_m.y+rc_m.h-1, rc_m.x + rc_m.w);
}



static void Dial_OwnerDraw(DRAWITEM_HDR *ds)  // 绘制表盘
{
  HDC hdc;
	RECT rc;
  WCHAR wbuf[128];
  uint8_t Sec = 0;
  uint8_t Min = 0;
  uint8_t Hour = 0;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
  FillRect(hdc, &rc);

  struct rtc_time systmtime;
  RTC_TimeCovr(&systmtime);
  Hour = systmtime.tm_hour;
  Min  = systmtime.tm_min;
  Sec  = systmtime.tm_sec;

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);

  SetPenSize(hdc, 2);
  SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
  SetBrushColor(hdc, MapRGB(hdc, 128, 128, 255));
  FillArc(hdc, rc.w/2, rc.h/2, 0, MIN(rc.w, rc.h)/2 - 1, 90, Sec / 60.0 * 360 + 90);
  
  x_wsprintf(wbuf, L"%02d %02d", Hour, Min);
  
  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  
  SetFont(hdc, controlFont_24);
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);
  
  /* 画两个圆点 */
  SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
  FillCircle(hdc, rc.w/2, rc.h/2 - 10, 2);
  FillCircle(hdc, rc.w/2, rc.h/2 + 10, 2);

  DeleteFont(controlFont_24);
  EnableAntiAlias(hdc, FALSE);
}

/*
 * @brief  重绘日期显示区域
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void date_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
	RECT rc;
  WCHAR wbuf[20];
  
  RECT rc1 = {0, 10, 110, 72};
  RECT rc3 = {40, 161, 36, 18};

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
  FillRect(hdc, &rc);
  
  struct rtc_time systmtime;
  RTC_TimeCovr(&systmtime);

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);

  SetPenSize(hdc, 2);
  SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
  EnableAntiAlias(hdc, FALSE);
  
  /* 创建字体 */
  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  
  /* 显示年 */
  x_wsprintf(wbuf, L"%d", systmtime.tm_year);
  
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	
  SetFont(hdc, controlFont_24);
  DrawText(hdc, wbuf, -1, &rc1, DT_VCENTER|DT_CENTER);
	
  SetFont(hdc, defaultFont);
  rc1.y += 3;
	rc1.x += 35;
  DrawText(hdc, L"年", -1, &rc1, DT_VCENTER|DT_CENTER);
  
  /* 显示月日 */
  RECT rc_mont    = {20, 60, 25, 25};
  RECT rc_mont_zh = {42, 67, 15, 15};
  RECT rc_mday    = {62, 60, 25, 25};
  RECT rc_mday_zh = {84, 67, 15, 15};
  
  x_wsprintf(wbuf, L"%02d", systmtime.tm_mon);
  
  SetFont(hdc, controlFont_24);
  DrawText(hdc, wbuf, -1, &rc_mont, DT_RIGHT | DT_BOTTOM);    // 显示月份
  
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"月", -1, &rc_mont_zh, DT_RIGHT | DT_BOTTOM);    // 显示“月”
  
  x_wsprintf(wbuf, L"%02d", systmtime.tm_mday);
  
  SetFont(hdc, controlFont_24);
  DrawText(hdc, wbuf, -1, &rc_mday, DT_RIGHT | DT_BOTTOM);     // 显示日期
  
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"日", -1, &rc_mday_zh, DT_RIGHT | DT_BOTTOM);     // 显示“日”
  
  /* 显示周 */
  SetFont(hdc, defaultFont);
  DrawText(hdc, Week_List[systmtime.tm_wday], -1, &rc3, DT_VCENTER|DT_CENTER);

  DeleteFont(controlFont_24);
}

/*
 * @brief  重绘标题透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Title_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  /* 背景 */
  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	GetWindowText(hwnd, wbuf, 128);                        // 获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  重绘白色背景文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void WhiteBK_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  /* 背景 */
  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	GetWindowText(hwnd, wbuf, 128);                        // 获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  获得列表选中项的数值（字符串转整型）
 * @param  hwnd:	窗口句柄
 * @param  ID：   列表 ID
 * @retval NONE
*/
static uint16_t GetListCurselVal(HWND hwnd, uint32_t ID)
{
  HWND wnd;
  uint8_t csl;    // 选中项
  WCHAR wbuf[10];
  char cbuf[10];

  wnd = GetDlgItem(hwnd, ID);
  csl = SendMessage(wnd, LB_GETCURSEL, 0, 0);    // 获得选中项
  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // 获得选中项的文本
  x_wcstombs(cbuf, wbuf, 9);

  return x_atoi(cbuf);                           // 返回选中项的值
}

/*
 * @brief  获得列表指定项的数值（字符串转整型）
 * @param  hwnd:	窗口句柄
 * @param  ID：   列表 ID
 * @param  csl:   列表的项
 * @retval NONE
*/
//static uint16_t GetListItemVal(HWND hwnd, uint32_t ID, uint32_t csl)
//{
//  HWND wnd;
//  WCHAR wbuf[10];
//  char cbuf[10];

//  wnd = GetDlgItem(hwnd, ID);
//  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // 获得选中项的文本
//  wcstombs(cbuf, wbuf, 9);

//  return x_atoi(cbuf);                           // 返回选中项的值
//}

static LRESULT setting_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg){
      case WM_CREATE:
      {
        
        for (uint8_t xC=4; xC<6; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }
        
        /* 创建设置标题文本框 */
        CreateWindow(TEXTBOX, clock_icon[6].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                      clock_icon[6].rc.x, clock_icon[6].rc.y,
                      clock_icon[6].rc.w,clock_icon[6].rc.h,
                      hwnd, clock_icon[6].id, NULL, NULL);
        
        /* 根据设置时间或设置日期来创建不同的控件 */
        if (Set_Start.page == 0)
        {
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");
          
          /* 循环时间显示7文本框 */
          CreateWindow(TEXTBOX, clock_icon[7].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                        clock_icon[7].rc.x, clock_icon[7].rc.y,
                        clock_icon[7].rc.w,clock_icon[7].rc.h,
                          hwnd, clock_icon[7].id, NULL, NULL);

          for (uint8_t xC=9; xC<11; xC++)
          {
            /* 创建设置列表（创建为不立即可见） */
            CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY | WS_OVERLAPPED | WS_VISIBLE,
                              clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                              clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
          }

          for (uint8_t xC=0; xC<24; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetHour);

            /* 在列表中增加小时 */
            x_wsprintf(wbuf, L"%02d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
          }
          
          for (uint8_t xC=0; xC<60; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMinute);

            /* 在列表中增加分钟 */
            x_wsprintf(wbuf, L"%02d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
          }
        }
        else
        {
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置日期");
          
          /* 循环时间显示7文本框 */
          CreateWindow(TEXTBOX, clock_icon[8].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                        clock_icon[8].rc.x, clock_icon[8].rc.y,
                        clock_icon[8].rc.w,clock_icon[8].rc.h,
                          hwnd, clock_icon[8].id, NULL, NULL);

          for (uint8_t xC=11; xC<14; xC++)
          {
            /* 创建设置列表 */
            CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY | WS_OVERLAPPED | WS_VISIBLE,
                              clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                              clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
          }
          
          uint8_t i=0;
          for (uint16_t xC=2000; xC<2050; xC++)
          {
            WCHAR wbuf[10];
            
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetYear);

            /* 在列表中增加年 */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }

          i=0;
          for (uint8_t xC=1; xC<13; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMonth);

            /* 在列表中增加月 */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }

          i=0;
          for (uint8_t xC=1; xC<32; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

            /* 在列表中增加日期 */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }
        }
        SetTimer(hwnd, 1, 50, TMR_START, NULL);
        
        break;
      }

      case WM_TIMER:
      {
        int cur_sel, i, y;
        HWND wnd;
        RECT rc0, rc;

        if(GetKeyState(VK_LBUTTON)==0) //鼠标左键／触摸屏　没有按下时.
        {
          for (uint32_t xC=ID_CLOCK_SetYear; xC<ID_CLOCK_SetMinute+1; xC++)
          {
            wnd =GetDlgItem(hwnd, xC); //获得LISTBOX 设置时的HWND.

            cur_sel=SendMessage(wnd,LB_GETCURSEL,0,0); //获得LISTBOX顶项.

            GetClientRect(wnd,&rc0);
            SendMessage(wnd,LB_GETITEMRECT,cur_sel,(LPARAM)&rc); //获得顶项的矩形位置.

            y = rc.y+(rc.h/2);

            if(y < (rc0.h/2)) //如果选中的项在Listbox　中线的上面,则往下移动.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//与中线隔得很近了，就只移1个offset(缓慢移动)．
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,i);
            }

            if(y > (rc0.h/2)) //如果选中的项在Listbox　中线的下面,则往上移动.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//与中线隔得很近了，就只移1个offset(缓慢移动)．
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,-i);
            }
          }
        }
        
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code, id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         
         NMHDR *nr;
         nr =(NMHDR*)lParam;      

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_CLOCK_WAIVE:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 完成设置按钮按下 */
            case ID_CLOCK_OK:
            {
//              if (Set_Start.page == 0)    // 只设置表盘
//              {
//                // clock_dial = Set_Start.dial;    // 设置表盘
//              }
//              else 
              if (Set_Start.page == 0)    // 修改表盘和时间
              {
                struct rtc_time systmtime;
                RTC_TimeCovr(&systmtime);    // 不修改日期，读出日期

                // clock_dial = Set_Start.dial;    // 设置表盘
                systmtime.tm_hour = GetListCurselVal(hwnd, ID_CLOCK_SetHour);        // 读取列表的时
                systmtime.tm_min = GetListCurselVal(hwnd, ID_CLOCK_SetMinute);       // 读取列表的分
                systmtime.tm_sec = 0;

                Time_Adjust(&systmtime);    // 设置时间
              }
              else if (Set_Start.page == 1)    // 修改表盘、时间和日历
              {
                struct rtc_time systmtime;
                RTC_TimeCovr(&systmtime);    // 不修改时间，读出时间

                systmtime.tm_year = GetListCurselVal(hwnd, ID_CLOCK_SetYear);    // 读取列表的年
                systmtime.tm_mon  = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);          // 读取列表的月
                systmtime.tm_mday = GetListCurselVal(hwnd, ID_CLOCK_SetDate);           // 读取列表的日

                /* 基姆拉尔森周计算公式 (Time_Adjust()函数内部计算，不在这里计算) */
                //  systmtime.tm_wday = (systmtime.tm_mday + 2 * systmtime.tm_mon + 3     \
                //                          * systmtime.tm_mon + 1) / 5 + systmtime.tm_year \
                //                          + systmtime.tm_year / 4 - systmtime.tm_year / 100  \
                //                          + systmtime.tm_year / 400) % 7 + 1;

                Time_Adjust(&systmtime);    // 设置时间
              }

              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

//            /* 表盘选择 */
//            case ID_CLOCK_Background00:
//            {
//              Set_Start.dial = 0;    // 选择表盘 1
//            }
//            break;

//            /* 表盘选择 */
//            case ID_CLOCK_Background01:
//            {
//              Set_Start.dial = 1;    // 选择表盘 2
//            }
//            break;

//            /* 表盘选择 */
//            case ID_CLOCK_Background02:
//            {
//              Set_Start.dial = 2;    // 选择表盘 3
//            }
//            break;

            /* 下一步 */
            case ID_CLOCK_NEXT:
            {
//              if (Set_Start.page == 0)
//              {
                Set_Start.page = 1;    // 标记为时钟设置界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置日期");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background00), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background01), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background02), SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_SHOW);
//              }
//              else if (Set_Start.page == 1)
//              {
//                Set_Start.page = 2;    // 标记为日期设置界面
//                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置日期");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),   SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),      SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),   SW_HIDE);

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_SHOW);
//              }
              
            }
            break;

            /* 上一步 */
            case ID_CLOCK_BACK:
            {
//              if (Set_Start.page == 2)
//              {
//                Set_Start.page = 1;    // 标记为时间设置界面
//                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),   SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),      SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),  SW_SHOW);

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_HIDE);
//              }
//              else 
//                if (Set_Start.page == 1)
//              {
                Set_Start.page = 0;    // 标记为设置时间界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background00), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background01), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background02), SW_SHOW);

               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),         SW_SHOW);
//              }
            }
            break;
          }
        }

        /* 列表处理 */
        if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetMinute)
        {
          if(nr->code == LBN_SELCHANGE)    // 列表选中项被改变
          {
            
          }
          
          if(nr->code == LBN_POSCHANGE)    // 有列表位置被改变
          {
            RECT rc0,rc;
            HWND wnd;
            int i;

            wnd = GetDlgItem(hwnd, id);     // 获得被改变列表的 HWND

            GetClientRect(wnd,&rc0);
            /* 使列表循环 */
            i = 0;
            SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
            if(rc.y > 2)
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_MOVEINDEX,i,0);
            }
            else
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
              if((rc.y+rc.h) < (rc0.h-8))
              {
                SendMessage(wnd,LB_MOVEINDEX,0,0xFFFF);
              }
            }

            /* 设置靠近中间项为选中项 */
            int min = 0;    // 记录最近的（最后的选中项）
            int len = 0xFFFFFFF;
            int count = SendMessage(wnd,LB_GETCOUNT,0,0);
            i = SendMessage(wnd, LB_GETTOPINDEX, 0, 0);            // 首个显示的子项目
            
            while (1)
            {
              SendMessage(wnd, LB_GETITEMRECT, i, (LPARAM)&rc);    // 获得子项目的矩形参数

              if (rc.y >= rc0.h || i >= count - 1)          // 判断是不是在显示区域内
              {
                SendMessage(wnd, LB_SETCURSEL, min, 0);     // 设置当前选中的子项目
                break;                                      // 不是在显示区域，结束循环
              }

              rc.y += rc.h/2;                // 计算这项的中间位置
              rc.y = abs(rc.y - rc0.h/2);    // 计算当前项到中间的距离

              if (len > rc.y)
              {
                len = rc.y;    // 记录较小距离
                min = i;       // 记录较小项
              }
              i++;         // 查找下一项
              if (i>=count) 
              {
                i = 0;
              }
            }

            WCHAR wbuf[5];
            if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetDate)    // 这在设置日历
            {
              x_wsprintf(wbuf, L"%d年%02d月%02d日", GetListCurselVal(hwnd, ID_CLOCK_SetYear),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetMonth),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetDate));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETDATE), wbuf);    // 设置日期
            }
            else    // 正在设置时间
            {
              x_wsprintf(wbuf, L"%02d:%02d", GetListCurselVal(hwnd, ID_CLOCK_SetHour),
                                             GetListCurselVal(hwnd, ID_CLOCK_SetMinute));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTIME), wbuf);    // 设置日期
            }
      
            uint8_t month = 0;
            month = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);    // 获得选中的月
            if (id == ID_CLOCK_SetMonth || (id == ID_CLOCK_SetYear && month == 2))    // 设置月份，不同月份需调整日期
            {
              uint8_t date_max = 0;

              date_max = month_list[month - 1];

              if (month == 2)    // 调整平年 闰年
              {
                uint16_t year = GetListCurselVal(hwnd, ID_CLOCK_SetYear);    // 获得选中的年

                if( year % 400 == 0 || (year % 4 ==  0 && year % 100 != 0))
                {
                  date_max = 29;    // 闰年
                }
              }

              HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

              SendMessage(ListHwnd, LB_RESETCONTENT, 0, 0);

              for (uint8_t xC=1; xC<date_max+1; xC++)
              {
                WCHAR wbuf[10];

                /* 在列表中增加日期 */
                x_wsprintf(wbuf, L"%d", xC);
                SendMessage(ListHwnd, LB_ADDSTRING, xC-1, (LPARAM)wbuf);
                SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC-1, (LPARAM)25);
              }
              SendMessage(ListHwnd, LB_SETCURSEL, 2, 0);

              // uint8_t list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);
              // uint8_t DateCursel = GetListCurselVal(hwnd, ID_CLOCK_SetDate);    // 获得选中的日期
              // uint8_t max = 0;
              // uint8_t item = 0;

              // if (list_conut > date_max)
              // {
              //   /* 列表项目比实际要多需要删除 */
              //   for (uint8_t xC=0; xC<list_conut; xC++)
              //   {
              //     if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > date_max)
              //     {
              //       SendMessage(ListHwnd, LB_DELETESTRING, xC, 0);
              //     }
              //   }
              // }
              // else if (list_conut < date_max)
              // {
              //   /* 列表项目比实际要少需要增加 */
              //   for (uint8_t xC=0; xC<list_conut; xC++)    // 找到最大项目
              //   {
              //     if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > max)
              //     {
              //       item = xC;    // 记录较大项目
              //       max = GetListItemVal(hwnd, ID_CLOCK_SetDate, item);       // 得到最大项的内容
              //     }
              //   }

                
              //   list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);    // 得到现有的项目数
              //   uint8_t ii = max;    // 项目

              //   for (uint8_t xY=item; xY<list_conut+date_max-max+1; xY++)    // 在最大的项目后面增加需要增加的项目
              //   {
              //     x_wsprintf(wbuf, L"%d", ii+1);
              //     ii++;
              //     SendMessage(ListHwnd, LB_ADDSTRING, xY, (LPARAM)wbuf);      // 增加项目
              //     SendMessage(ListHwnd, LB_SETITEMHEIGHT, xY, (LPARAM)45);    // 设置项目高度
              //   }
              // }

              // /* 设置列表的选中项目 */
              // list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);
              // max = 0;
              // uint8_t xC=0;
              // for (xC=0; xC<list_conut; xC++)    // 找到最大项目
              // {
              //   if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) == DateCursel)
              //   {
              //     SendMessage(ListHwnd, LB_SETCURSEL, xC, 0);    // 设置选中的项目
              //     break;
              //   }

              //   if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > max)
              //   {
              //     item = xC;    // 记录较大项目
              //     max = GetListItemVal(hwnd, ID_CLOCK_SetDate, xC);
              //   }
              // }

              // if (xC >= list_conut)    // 没找到原来的项，设置最大项为选中项
              // {
              //   SendMessage(ListHwnd, LB_SETCURSEL, item, 0);    // 设置选中的项目
              // }
              // printf("xC=%d,item=%d\n", xC, item);
            }
          }
        }

      break;
    }

      // 重绘制函数消息
      case WM_DRAWITEM:
      {
        DRAWITEM_HDR *ds;
        ds = (DRAWITEM_HDR*)lParam;        
        if (ds->ID >= ID_CLOCK_BACK && ds->ID <= ID_CLOCK_OK)
        {
          btn_owner_draw(ds);             // 重绘按钮
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTITLE)
        {
          Title_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTIME || ds->ID == ID_CLOCK_SETDATE)
        {
          WhiteBK_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_WAIVE)
        {
          waive_btn_owner_draw(ds);
          return TRUE;
        }
//        else if (ds->ID >= ID_CLOCK_Background00 && ds->ID <= ID_CLOCK_Background02)
//        {
//          radiobox_owner_draw(ds, ds->ID);
//          return TRUE;
//        }
        else if (ds->ID >= ID_CLOCK_SetYear && ds->ID <= ID_CLOCK_SetMinute)
        {
          listbox_owner_draw(ds);
          return TRUE;
        }

        return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
//        HDC hdc;//屏幕hdc

        //开始绘制
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
      }
      break;

      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        
        RECT rc_title = {0, 0, GUI_XSIZE, 40};
        RECT rc_title_grad = {0, 40, GUI_XSIZE, 3};
        RECT rc_lyric = {0, 40, GUI_XSIZE, 200};
        RECT rc1 = {85, 46, 164, 150};

        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc_title);

        SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
        FillRect(hdc, &rc_lyric);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, CLOCK_BACK_COLOR), TRUE);
        
        SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
        EnableAntiAlias(hdc, TRUE);
        FillRoundRect(hdc, &rc1, 5);    // 画列表背景
        EnableAntiAlias(hdc, FALSE);
    

        return FALSE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {
        SetTimer(clock_hwnd, 1, 400, TMR_START, NULL);
        Set_Start.page = 0;

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}

static uint8_t sec_old = ~0;
static uint8_t sec_date = ~0;

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg){
      case WM_CREATE:
      {
        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        for (uint8_t xC=ICON_BTN_NUM; xC<ICON_TEXT_NUM; xC++)    // 文本框
        {
          /* 循环创建文本框 */
          CreateWindow(TEXTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w,clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

//        /* 初始化日期 */
//        WCHAR wbuf[5];
//        struct rtc_time systmtime;
//        RTC_TimeCovr(&systmtime);
//        
//        x_wsprintf(wbuf, L"%d", systmtime.tm_mday);
//        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DAY), wbuf);    // 设置日期
//        
//        x_wsprintf(wbuf, L"%d月", systmtime.tm_mon);
//        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
//        
//        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[systmtime.tm_wday]);    // 设置星期

        SetTimer(hwnd, 1, 400, TMR_START, NULL);

        break;
      }

      case WM_TIMER:
      {
        int tmr_id;

				tmr_id = wParam;    // 定时器 ID

				if (tmr_id == 1)
        {
          struct rtc_time systmtime;
          RTC_TimeCovr(&systmtime);
          
          if (systmtime.tm_sec != sec_old)
          {
            sec_old = systmtime.tm_sec;
            InvalidateRect(GetDlgItem(hwnd, ID_CLOCK_TIME), NULL, TRUE);
          }

          if (systmtime.tm_mday != sec_date)
          {
            sec_date = systmtime.tm_mday;
            InvalidateRect(GetDlgItem(hwnd, ID_CLOCK_DATE), NULL, TRUE);
          }
        }
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型   

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_CLOCK_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 显示日期区域按下 */
            case ID_CLOCK_DATE:
            {
              KillTimer(hwnd, 1);
              /* 创建设置窗口 */
              WNDCLASS wcex;
              RECT rc;
              
              Set_Start.page = 1;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // 创建"设置"窗口.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
            break;
            
            /* 显示时间区域按下 */
            case ID_CLOCK_TIME:
            {
              KillTimer(hwnd, 1);
              /* 创建设置窗口 */
              WNDCLASS wcex;
              RECT rc;
              
              Set_Start.page = 0;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // 创建"设置"窗口.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
          }
        }
        break;
      }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_CLOCK_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_DATE)
         {
            date_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TITLE)
         {
            Title_Textbox_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TIME)
         {
            Dial_OwnerDraw(ds);
            return TRUE;
         }

         return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;

        //开始绘制
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        
        RECT rc_title = {0, 0, GUI_XSIZE, 40};
        RECT rc_title_grad = {0, 40, GUI_XSIZE, 3};
        RECT rc_lyric = {0, 40, GUI_XSIZE, 200};

        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc_title);

        SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
        FillRect(hdc, &rc_lyric);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, CLOCK_BACK_COLOR), TRUE);

        return FALSE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {        
        sec_old = ~0;
        sec_date = ~0;
        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_CLOCK_DIALOG(void)
{ 	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	clock_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI CLOCK DIALOG",
                                    WS_VISIBLE|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(clock_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, clock_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


