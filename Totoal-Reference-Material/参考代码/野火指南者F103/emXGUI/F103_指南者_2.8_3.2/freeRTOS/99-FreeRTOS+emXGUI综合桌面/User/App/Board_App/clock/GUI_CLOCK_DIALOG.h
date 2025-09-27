#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"

#ifndef _GUI_CLOCK_DIALOG_H
#define _GUI_CLOCK_DIALOG_H          

typedef enum 
{ 
  /****************** 按钮控件 ID 值 *******************/
  ID_CLOCK_BACK = 0x1000,      // 上一步
  ID_CLOCK_NEXT,               // 下一步
  ID_CLOCK_OK,                 // 完成设置
  ID_CLOCK_WAIVE,              // 放弃本次设置
  ID_CLOCK_EXIT,               // 退出按钮
	ID_CLOCK_SET,                // 设置
  /***************** 文本控件 ID 值 *********************/
  ID_CLOCK_TITLE,              // 窗口标题栏
  ID_CLOCK_MONTH,              // 月
  ID_CLOCK_WEEK,               // 周
  ID_CLOCK_DATE,                // 日
	ID_CLOCK_SETTITLE,           // 设置窗口标题栏
	ID_CLOCK_SETTIME,            // 设置显示时间
	ID_CLOCK_SETDATE,            // 设置显示日期
  ID_CLOCK_TIME,               // 时间显示区域（创建为控件方便重绘）
	/****************** 单选按钮 ***********************/
//	ID_CLOCK_Background00,
//	ID_CLOCK_Background01,
	ID_CLOCK_Background02,
	/********************* 列表 **************************/
	ID_CLOCK_SetWin,
  ID_CLOCK_SetYear,
	ID_CLOCK_SetMonth,
  ID_CLOCK_SetDate,
  ID_CLOCK_SetHour,
  ID_CLOCK_SetMinute,
}clock_id_t;

typedef struct{
	WCHAR *icon_name;    // 图标名
	RECT rc;             // 位置信息
	clock_id_t id;      // 按钮ID
}clock_icon_t;

typedef enum
{
	hdc_clock_btn = 0,
	hdc_clock_btn_press,
	hdc_clock_calendar,
//	hdc_clock_back_00,
//	hdc_clock_h_00,
//	hdc_clock_m_00,
//	hdc_clock_s_00,
//	hdc_clock_back_01,
//	hdc_clock_h_01,
//	hdc_clock_m_01,
//	hdc_clock_s_01,
	hdc_clock_back_02,
	hdc_clock_h_02,
	hdc_clock_m_02,
	hdc_clock_s_02,
//	hdc_clock_chcked,

	hdc_clock_end,              // 这个必须放在最后
}hdc_clock_png_t;

typedef struct{
	char *pic_name;     // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	hdc_clock_png_t id;        // hdc 编号
}clock_hdc_t;



#endif

