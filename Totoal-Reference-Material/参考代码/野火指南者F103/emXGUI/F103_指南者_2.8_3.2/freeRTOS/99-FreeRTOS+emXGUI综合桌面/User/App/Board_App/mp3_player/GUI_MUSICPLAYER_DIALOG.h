#ifndef _GUI_MUSICPLAYER_DIALOG_H
#define _GUI_MUSICPLAYER_DIALOG_H


#define MUSIC_MAX_NUM           10
#define FILE_NAME_LEN           50			
#define MUSIC_NAME_LEN          50		
#define LYRIC_MAX_SIZE          100				
#define MAX_LINE_LEN            100				
#define _DF1S                   0x81
#define COMDATA_SIZE				  	1024*5

#define LRC_GBK   1  //1表示LRC歌词为GBK编码，0表示UTF8编码
#define ID_TB1               0x1301
#define ID_TB2               0x1302
#define ID_TB5               0x1305

#define GUI_RGB_BACKGROUNG_PIC      "musicdesktop.jpg"

typedef struct{
   char *icon_name;//图标名
   RECT rc;        //位置信息
   BOOL state;     //状态  
}icon_S;

typedef __packed struct 
{ 
	uint8_t 	indexsize; 	
  uint8_t		length_tbl[LYRIC_MAX_SIZE];    // 200
 	uint16_t	addr_tbl[LYRIC_MAX_SIZE];										
 	int32_t		curtime;										
  int32_t		oldtime;	
  int32_t 	time_tbl[LYRIC_MAX_SIZE];	
  uint8_t flag;//读取文件标志位
}LYRIC;

extern HWND MusicPlayer_hwnd;
extern int play_index;
extern char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];//播放List
extern char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//显示list
extern uint8_t music_file_num;
extern uint8_t chgsch; //进度条调整标志位
extern HWND music_wnd_time;
extern LYRIC lrc;
void com_utf82gbk(const char *src, char *str);
void com_gbk2utf8(const char *src, char *str);
extern uint8_t *comdata;
extern signed long hCOMUTF8Memory;
extern uint8_t *comdataUTF8;
extern icon_S music_icon[8];
extern HWND wnd_lrc1;//歌词窗口句柄
extern HWND wnd_lrc2;//歌词窗口句柄
extern HWND wnd_lrc3;//歌词窗口句柄
extern HWND wnd_lrc4;//歌词窗口句柄
extern HWND wnd_lrc5;//歌词窗口句柄
extern uint8_t ReadBuffer1[];

extern HFONT Music_Player_hFont48;
extern HFONT Music_Player_hFont64;
extern HFONT Music_Player_hFont72;

#endif

