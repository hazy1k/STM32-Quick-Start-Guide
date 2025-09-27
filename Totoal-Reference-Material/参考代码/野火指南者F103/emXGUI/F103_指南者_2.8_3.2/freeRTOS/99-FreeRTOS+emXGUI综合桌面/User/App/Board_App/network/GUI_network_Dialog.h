#include <emXGUI.h>

#ifndef __GUI_NETWORK_DIALOG
#define	__GUI_NETWORK_DIALOG

/* 控件 ID 定义 */
enum
{   
   /* 窗口 ID */
  ID_TEXTBOX_Title      = 0x00,     // 标题栏
  ID_TEXTBOX_Send        ,     // 发送显示
  ID_TEXTBOX_Receive     ,     // 接收显示
  ID_TEXTBOX_RemoteIP   ,     // 远端IP
//  ID_TEXTBOX_RemoteIP2   ,     // 远端IP
//  ID_TEXTBOX_RemoteIP3   ,     // 远端IP
//  ID_TEXTBOX_RemoteIP4   ,     // 远端IP
  ID_TEXTBOX_RemotePort  ,     // 远端端口
  ID_MODE_GROUP,
  
  /* 按钮 ID */
  eID_BTN_EXIT    ,    // 0x03
  eID_BTN_STATE   ,    // 0x04
  eID_BTN_SEND    ,    // 0x05
  eID_BTN_CLEAR   ,    // 0x06
  
  ID_Hint_Win,
  
};

#endif


