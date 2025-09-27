#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"

#ifndef _GUI_RECORDER_DIALOG_H
#define _GUI_RECORDER_DIALOG_H

#define MUSIC_MAX_NUM           10
#define FILE_NAME_LEN           50			
#define MUSIC_NAME_LEN          50		
#define LYRIC_MAX_SIZE          100				
#define MAX_LINE_LEN            100				
#define _DF1S                   0x81

#define LRC_GBK   1  //1表示LRC歌词为GBK编码，0表示UTF8编码
#define ID_TB1               0x1301
#define ID_TB2               0x1302
#define ID_TB5               0x1305

typedef enum 
{ 
  /******************按钮控件ID值*******************/
  ID_RECORD_bPOWER = 0x1000,   // 音量 
  ID_RECORD_BACK,              // 上一首
  ID_RECORD_NEXT,              // 下一首
  ID_RECORD_PLAY,              // 播放暂停按钮
  ID_RECORD_BUGLE,             // 喇叭按钮
  ID_RECORD_PADNC,             // 录音继续暂停
  ID_RECORD_START,             // 录音开始
  ID_RECORD_STOP,              // 停止录音
  ID_RECORD_EXIT,              // 退出按钮（放最后：与其他按钮重绘函数不一样）
  /*****************滑动条控件ID值*******************/
  ID_RECORD_sPOWER,            // 音量条
  ID_PLAY_PROGRESS,            // 进度条
  /*****************文本控件ID值*********************/
  ID_RECORD_STATE = 0x1301 - 2,                // 录音状态
  ID_RECORD_TIME,                 // 录音时长
  ID_PLAY_TOTAL_TIME = 0x1301,    // 播放总时长（与音乐播放器的是同一个ID）
  ID_PLAY_TIME = 0x1302,          // 播放时长（与音乐播放器的是同一个ID）
  /*****************列表控件ID值*********************/
  ID_RECORD_LIST,              // 文件列表
  /*****************列表控件ID值*********************/
  ID_Record_Timer,
  /******************用户自定义消息******************/
  eMSG_SCAN_FILE = WM_USER + 1,

}record_id_t;

typedef struct{
   WCHAR *icon_name; // 图标名
   RECT rc;         // 位置信息
   record_id_t id;      // 按钮ID
}recorder_icon_t;

#endif

