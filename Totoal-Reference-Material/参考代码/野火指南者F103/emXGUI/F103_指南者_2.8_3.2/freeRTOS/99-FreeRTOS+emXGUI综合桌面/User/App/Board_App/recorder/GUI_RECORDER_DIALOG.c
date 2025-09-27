#include "emXGUI.h"
#include "./recorder/GUI_RECORDER_DIALOG.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./recorder/Backend_Recorder.h"

//图标管理数组
recorder_icon_t record_icon[] = {

   {L"A",        { 25, 180, 26, 24},   ID_RECORD_bPOWER},    // 0. 音量
   {L"Q",        { 68, 180, 24, 24},   ID_RECORD_BUGLE},     // 1. 喇叭按钮
   {L"S",        {164, 205, 24, 24},   ID_RECORD_BACK},      // 2. 上一首
   {L"T",        {203, 201, 32, 32},   ID_RECORD_PLAY},      // 3. 播放
   {L"V",        {250, 204, 24, 24},   ID_RECORD_NEXT},      // 4. 下一首
   {L"U",        { 25, 146, 24, 24},   ID_RECORD_STOP},      // 5. 停止录音
   {L"U",        { 68, 146, 24, 24},   ID_RECORD_START},     // 6. 开始录音
   {L"U",        { 68, 146, 24, 24},   ID_RECORD_PADNC},     // 7. 暂停继续
   {L"O",        {286,   8, 23, 23},   ID_RECORD_EXIT},      // 8. 退出
 
   {L"录音机",   {30,   41, 57, 27},   ID_RECORD_STATE},     // 9. 正在录音
   {L"00:00",    {38,   86, 42, 27},   ID_RECORD_TIME},      // 10. 录音时长
   {L"00:00",    {119, 177, 31, 18},   ID_PLAY_TIME},        // 11. 播放时长
   {L"00:00",    {287, 177, 32, 18},   ID_PLAY_TOTAL_TIME},  // 12. 录音总时长
  
   {L" ",        {135, 26, 158,138},   ID_RECORD_LIST},      // 13. 音乐列表
   {L" ",        {152, 179,130, 16},   ID_PLAY_PROGRESS},    // 14. 播放进度条
   {L" ",        { 24, 211, 69, 16},   ID_RECORD_sPOWER},    // 15. 音量进度条
  
};

#define RECORDER_BACK_COLOR     95, 137, 159//188, 213, 253

//音乐播放器句柄
HWND	Recorer_hwnd;
TaskHandle_t h_record;    // 播放任务控制句柄
TaskHandle_t h_play_record;

/* 以下在 MUSIC 中定义的，这里直接用，不重新定义，以节约内存 */
extern char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];  // 播放List
extern char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];  // 显示list
extern uint8_t  music_file_num;                   // 文件个数
extern char music_name[FILE_NAME_LEN];    // 歌曲名数组
extern int play_index;      // 播放歌曲的编号值
extern HWND music_wnd_time; // 歌曲进度条窗口句柄
extern uint8_t chgsch;      // 调整进度条标志位

static char path[100];     // 文件根目录
int  power_rec = 220;    // 音量值

/**
  * @brief  scan_files 递归扫描sd卡内的录音文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_record_files(char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];
	WCHAR wbuf[128];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
     if (res != FR_OK || fno.fname[0] == 0)
     {
       f_closedir(&dir);
       break; 	//为空时表示所有项目读取完毕，跳出
     }
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //if(strstr(path,"recorder")!=NULL)continue;      // 逃过录音文件
      if (*fn == '.') continue; 											  // 点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							  // 目录，递归读取
        sprintf(&path[i], "/%s", fn); 							    // 合成完整目录名
        res = scan_record_files(path);									// 递归遍历 
        if (res != FR_OK) 
        {
          f_closedir(&dir);
					break; 																	     	// 打开失败，跳出循环
        }
        path[i] = 0; 
      } 
      else 
      { 
//				printf("%s/%s\r\n", path, fn);								// 输出文件名
				if(strstr(fn,".wav")||strstr(fn,".WAV"))      // 判断是否wav文件
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(music_file_num<MUSIC_MAX_NUM))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(music_playlist[music_file_num], file_name, strlen(file_name) + 1);
                  printf("%s\r\n", music_playlist[music_file_num]);
						//memcpy(music_lcdlist[music_file_num],fn,strlen(fn));
						x_mbstowcs(wbuf, fn, sizeof(wbuf));	    // 将Ansi字符转换成GUI的unicode字符.
						//在Listbox中增加一个Item项，记录文件名和文件属性.
						i = SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_GETCOUNT, 0, 0);
						SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_ADDSTRING, i, (LPARAM)wbuf);
						music_file_num++;//记录文件个数
					}
				}//if mp3||wav
      }//else
     } //for
  } 
  return res; 
}

/**
  * @brief  播放录音任务
  * @param  hwnd:主窗口句柄
  * @retval 无
  */

/* 在 Backend_Recorder.c 中定义 */
extern uint8_t Isread;              /* DMA传输完成标志 */
extern uint8_t bufflag;             /* 数据缓存区选择标志 */
extern uint32_t wavsize;            /* wav音频数据大小 */
extern FIL AF_File_Handle;			/* file objects */
char recfilename[25]; 
//extern WavHead rec_wav;             /* WAV设备  */
extern FRESULT result; 
extern UINT bw;            					/* File R/W count */
extern REC_TYPE Recorder;           /* 录音设备 */

static void App_Record(void *p)
{
  char *name = p;
//  vTaskSuspend(h_record);    // 挂起线程
  uint8_t app = 1;
  while(app) //线程已创建了
  {     
    recorder(name);
    app = 0;    // 只运行一次
  }
  printf("录音结束\n\r");
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

/**
  * @brief  播放录音任务
  * @param  hwnd:主窗口句柄
  * @retval 无
  */
static int thread=0;
static void App_PlayRecord(HWND hwnd)
{                          
	
  thread =1;
  vTaskSuspend(h_play_record);    // 挂起线程
  
	while(thread) //线程已创建了
	{     
    int i = 0;      
     
    i = 0;
     //得到播放曲目的文件名
    while(music_playlist[play_index][i]!='\0')
    {
      music_name[i]=music_playlist[play_index][i];
      i++;
    }
    music_name[i]='\0';
    if (play_index != SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL, 0, 0))     // 检查当前要播放的项目是不是已经选中
    {
      SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, play_index, 0);            // 设置当前播放的歌曲为选中状态
    }

    if(strstr(music_name,".wav")||strstr(music_name,".WAV"))
    {
      record_play((uint8_t *)music_name, hwnd);
    }

    printf("播放结束\n");

    //进行任务调度
    GUI_msleep(20);
	   
   }
   thread = 1;
   printf("录音播放线程退出\n");
   GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //设置画笔色
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

static void stop_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  EnableAntiAlias(hdc, TRUE);
   
  if (ds->Style & WS_DISABLED)    // 窗口是禁止的
  {
    SetBrushColor(hdc, MapRGB(hdc, 200, 200, 200));
    SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //按钮是按下状态
      SetPenColor(hdc, MapRGB(hdc, 185, 185, 185));
      SetBrushColor(hdc, MapRGB(hdc, 185, 185, 185));      //设置文字色
    }
    else
    { //按钮是弹起状态
      SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
    }
  }
  
  /* 画圆环 */
//  SetPenSize(hdc, 2);
  DrawCircle(hdc, rc.x+rc.w/2, rc.y+rc.h/2, rc.w/2);

  /* 绘制圆角矩形 */
	InflateRect(&rc, -5, -5);
  FillRoundRect(hdc, &rc, 4);
  
  EnableAntiAlias(hdc, FALSE);
}

static void start_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  EnableAntiAlias(hdc, TRUE);
 
  //设置按键的颜色
	if (ds->Style & WS_DISABLED)    // 窗口是禁止的
  {
    SetBrushColor(hdc, MapRGB(hdc, 180, 20, 20));      //设置文字色
    SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //按钮是按下状态
      SetBrushColor(hdc, MapRGB(hdc, 180, 20, 20));      //设置背景色
      SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
    }
    else
    { //按钮是弹起状态
      SetBrushColor(hdc, MapRGB(hdc, 240, 10, 10));
      SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
    }
  }
  
  /* 画圆环 */
//	SetPenSize(hdc, 2);
  DrawCircle(hdc, rc.x+rc.w/2, rc.y+rc.h/2, rc.w/2);

  /* 绘制圆角矩形 */
				 
  FillCircle(hdc, rc.x+rc.w/2, rc.x+rc.w/2, 7);
  
  EnableAntiAlias(hdc, FALSE);
}

/***********************控件重绘函数********************************/
/**
  * @brief  button_owner_draw 按钮控件的重绘制
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */

static void button_owner_draw(DRAWITEM_HDR *ds)
{
  HDC hdc; //控件窗口HDC
	
  HWND hwnd; //控件句柄 
  RECT rc_cli;//控件的位置大小矩形
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 
													
  //获取控件的位置大小信息
  GetClientRect(hwnd, &rc_cli);

  GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
  GetClientRect(hwnd, &rc_cli);//得到控件的位置

  //播放键使用100*100的字体
  if(ds->ID == ID_RECORD_PLAY)
  {
    SetFont(hdc, controlFont_32);
  }
  else
  {
    SetFont(hdc, controlFont_24);
  }

  //设置按键的颜色
  if (ds->Style & WS_DISABLED)    // 窗口是禁止的
  {
    SetTextColor(hdc, MapARGB(hdc, 250, 220, 220, 220));      //设置文字色
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //按钮是按下状态
      SetTextColor(hdc, MapARGB(hdc, 250,105,105,105));      //设置文字色
    }
    else
    { //按钮是弹起状态
      SetTextColor(hdc, MapARGB(hdc, 250,250,250,250));
    }
  }													   

  DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
}

/*
 * @brief  重绘显示亮度的透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
   RECT rc,rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.

	/* 背景 */
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
//  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
//	FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

/*
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  back_c：背景颜色
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
  RECT rc;
  RECT rc_scrollbar;

  /* 背景 */
  GetClientRect(hwnd, &rc);//得到控件的位置

  SetBrushColor(hdc,MapARGB(hdc, 0, RECORDER_BACK_COLOR));
  FillRect(hdc, &rc);

  rc_scrollbar.x = rc.x;
  rc_scrollbar.y = rc.h/2-1;
  rc_scrollbar.w = rc.w;
  rc_scrollbar.h = 2;

  SetBrushColor(hdc, MapRGB888(hdc, Page_c));
  FillRect(hdc, &rc_scrollbar);

  /* 滑块 */
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

  SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
  //rc.y += (rc.h >> 2) >> 1;
  //rc.h -= (rc.h >> 2);
  /* 边框 */
  //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
  InflateRect(&rc, -2, -2);

  SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
}

/*
 * @brief  自定义滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	RECT rc_cli;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc, 0, RGB888(	50, 205, 50), RGB888(50, 205, 50));
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   
}

/*
 * @brief  重绘列表
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];
	hwnd =ds->hwnd;
	hdc =ds->hDC;

	rc =ds->rc;

  /* 背景 */																			 
  GetClientRect(hwnd, &rc);//得到控件的位置

  SetBrushColor(hdc, MapRGB(hdc, RECORDER_BACK_COLOR));
  FillRect(hdc, &rc);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* 列表为空，显示提示信息然后直接返回 */
    DrawText(hdc, L"还没有录音文件！", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}

    SetTextColor(hdc, MapRGB(hdc,50,10,10));

    if(i==cursel)
    {
      SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
    }
    else
    {
      SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
    }

    /* 画前面的图标 */
    // SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
    // EnableAntiAlias(hdc, TRUE);
    // FillPolygon(hdc, 0, 0, pt, 5);
    // EnableAntiAlias(hdc, FALSE);

    SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);

    DrawText(hdc, wbuf, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);      // 显示列表文本
    
    SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                         // 画一条线

    i++; 
  }
}

extern int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static uint8_t  BUGLE_STATE = 0;     // 喇叭状态
	 static uint32_t Record_Timer = 0;    // 录音计时器
  
   switch(msg){
      case WM_CREATE:
      {
         vsset.mvol=140;			//音量大小
         vsset.bflimit=6;		//低音限制
         vsset.bass=15;			//低音效果
         vsset.tflimit =10;	//高音限制
         vsset.treble =15;		//高音效果
         vsset.effect =0;		//无耳机效果
         vsset.speed =1;			//正常速度
          
         recset.input =0;			//MIC
         recset.samplerate =1;	//8K
         recset.channel =0;		//左声道
         recset.agc = 2;				//实际增益为1
        
         VS_Init();
         GUI_msleep(100);
        
         if (VS_HD_Reset())
         {
           GUI_DEBUG("vs1053硬件复位失败");
         }
        
         VS_Soft_Reset();   
		  
         for (uint8_t xC=0; xC<9; xC++)     // 0~7 ：按钮
         {
            /* 循环创建按钮 */
            CreateWindow(BUTTON, record_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_TRANSPARENT,
                         record_icon[xC].rc.x, record_icon[xC].rc.y,
                         record_icon[xC].rc.w,record_icon[xC].rc.h,
                         hwnd, record_icon[xC].id, NULL, NULL); 
         }
				 ShowWindow(GetDlgItem(hwnd, ID_RECORD_BUGLE), SW_HIDE);     // 隐藏外放喇叭的按钮
         ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_HIDE);     // 窗口隐藏继续和暂停的按钮
         EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), DISABLE);    // 禁用停止按钮

         for (uint8_t xC=9; xC<13; xC++)    // 8~11 ：文本框
         {
            /* 循环创建文本框 */
            CreateWindow(TEXTBOX, record_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_TRANSPARENT,
                         record_icon[xC].rc.x, record_icon[xC].rc.y,
                         record_icon[xC].rc.w,record_icon[xC].rc.h,
                         hwnd, record_icon[xC].id, NULL, NULL); 
         }

         /*********************歌曲进度条******************/
         SCROLLINFO sif;
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 255;
         sif.nValue = 0;//初始值
         sif.TrackSize = 15;//滑块值
         sif.ArrowSize = 0;//两端宽度为0（水平滑动条）          
         music_wnd_time = CreateWindow(SCROLLBAR, record_icon[14].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                           record_icon[14].rc.x, record_icon[14].rc.y, record_icon[14].rc.w,
                           record_icon[14].rc.h, hwnd, record_icon[14].id, NULL, NULL);
         SendMessage(music_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         

         /*********************音量值滑动条******************/
         SCROLLINFO sif_power;
         sif_power.cbSize = sizeof(sif_power);
         sif_power.fMask = SIF_ALL;
         sif_power.nMin = 0;
         sif_power.nMax = 254;//音量最大值为63
         sif_power.nValue = 220;//初始音量值
         sif_power.TrackSize = 15;//滑块值
         sif_power.ArrowSize = 0;//两端宽度为0（水平滑动条）
         
         HWND wnd;
         wnd = CreateWindow(SCROLLBAR, record_icon[15].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                           record_icon[15].rc.x, record_icon[15].rc.y, record_icon[15].rc.w,
                           record_icon[15].rc.h, hwnd, record_icon[15].id, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_power);
    
         BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
         xReturn = xTaskCreate((TaskFunction_t )(void(*)(void*))App_PlayRecord,  /* 任务入口函数 */
                            (const char*    )"App_PlayMusic",          /* 任务名字 */
                            (uint16_t       )2*1024/4,                   /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )hwnd,                     /* 任务入口函数参数 */
                            (UBaseType_t    )5,                        /* 任务的优先级 */
                            (TaskHandle_t  )&h_play_record);           /* 任务控制块指针 */

				if(xReturn != pdPASS )
				{
					GUI_ERROR("GUI rec play Thread Create failed");
				} 
        
         /* 创建录音文件列表 */
         CreateWindow(LISTBOX, record_icon[13].icon_name, WS_OWNERDRAW |  WS_VISIBLE | LBS_NOTIFY,
                           record_icon[13].rc.x, record_icon[13].rc.y, record_icon[13].rc.w,
                           record_icon[13].rc.h, hwnd, record_icon[13].id, NULL, NULL);
         
        PostAsyncMessage(hwnd, eMSG_SCAN_FILE, 0, 0);     // 执行一次文件扫描(使用异步的方式发送消息)
				InvalidateRect(hwnd,NULL,TRUE);
        break;
      }

      case WM_TIMER:
      {
        int tmr_id;
				WCHAR wbuf[20];

				tmr_id = wParam;    // 定时器 ID

				if (tmr_id == ID_Record_Timer)    // 每秒录音计时定时器
				{
					Record_Timer++;
					x_wsprintf(wbuf, L"%02d:%02d", Record_Timer / 60, Record_Timer % 60);
          SetWindowText(GetDlgItem(hwnd, ID_RECORD_TIME), wbuf);
				}
      }  
			break;     
      
      case eMSG_SCAN_FILE:
      {
        HWND wnd = GetDlgItem(hwnd, ID_RECORD_LIST);
               
        SendMessage(wnd, LB_RESETCONTENT, 0, 0);
        memset(music_playlist, '\0', sizeof(music_playlist));
				printf("sizeof(music_lcdlist) : %d\r\n",sizeof(music_lcdlist));
//        memset(music_lcdlist, '\0', sizeof(music_lcdlist));
        scan_record_files(path);
        SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, 0, 0);    // 默认选中第一项
        InvalidateRect(wnd, NULL, TRUE);
      }
      break;
      
      case WM_NOTIFY:
      {
         u16 code,  id, ctr_id;;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         
         NMHDR *nr;        

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_RECORD_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;
            
            /* 上一首 */
            case ID_RECORD_BACK:
            {
              int i, num;
              WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
              i = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);     // 获得当前选中项
              num = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCOUNT,0,0);    // 获得总项数
              SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, i == 0 ? num-1 : --i, 0);     // 获得设置当前选中项
            
              GetWindowText(wnd, wbuf, 3);
              if (wbuf[0] == L'T')
              {
                vTaskResume(h_play_record);            // 当前是挂起的，恢复任务
                SetWindowText(wnd, L"U");
                EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
              }
                
//              I2S_Play_Start();
              play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // 获得当前选中项
              Replay = 0;
            }
            break;
            
            /* 下一首 */
            case ID_RECORD_NEXT:
            {
              int i, num;
              WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
              i = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);     // 获得当前选中项
              num = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCOUNT,0,0);    // 获得总项数
              SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, i==(num-1) ? 0 : ++i, 0);     // 获得设置当前选中项
              
              GetWindowText(wnd, wbuf, 3);
              if (wbuf[0] == L'T')
              {
                vTaskResume(h_play_record);            // 当前是挂起的，恢复任务
                SetWindowText(wnd, L"U");
                EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
              }
                
//              I2S_Play_Start();
              play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // 获得当前选中项
              Replay = 0;
            }
            break;
            
            /* 播放录音 */
            case ID_RECORD_PLAY:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
							GetWindowText(wnd, wbuf, 3);
              
							if (wbuf[0] == L'U')
							{
                vTaskSuspend(h_play_record);
								SetWindowText(wnd, L"T");
								EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), ENABLE);      // 使能开始录音按钮
							}
							else
							{
                vTaskResume(h_play_record);
//                Replay = 1;
								SetWindowText(wnd, L"U");
								EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
							}
            }
            break;
            
            /* 开始录音 */
            case ID_RECORD_START:
						{
              int i = 0;
              DIR dir;
              
              /*  如果路径不存在，创建文件夹  */
              result = f_opendir(&dir,RECORDERDIR);
              while(result != FR_OK)
              {
                i++;
                if (i > 5)
                {
                  /* 初始化失败 */
                  RECT RC;
                  MSGBOX_OPTIONS ops;
                  const WCHAR *btn[] = { L"确认",L"取消" };      //对话框内按钮的文字

                  ops.Flag = MB_ICONERROR;
                  ops.pButtonText = btn;
                  ops.ButtonCount = 2;
                  RC.w = 180;
                  RC.h = 120;
										  
                  RC.x = (GUI_XSIZE - RC.w) >> 1;
                  RC.y = (GUI_YSIZE - RC.h) >> 1;
                  SelectDialogBox(hwnd, RC, L"没有检测到SD卡\n请确认SD已插入。", L"错误", &ops);    // 显示错误提示框
                  break;
                }
                f_mkdir(RECORDERDIR);
                result = f_opendir(&dir,RECORDERDIR);
              }
              
              if (i >= 5)break;
              
              /* 寻找合适文件名 */
              for(i=1;i<0xff;++i)
              {
                sprintf(recfilename,"0:/recorder/rec%03d.wav",i);
                result=f_open(&AF_File_Handle,(const TCHAR *)recfilename,FA_READ);
                if(result==FR_NO_FILE)break;					
              }
              f_close(&AF_File_Handle);
              
              if(i==0xff)
              {
                break;
              }
              /* 开始录音 创建录音任务 */
              RecordFlag = 0;
              BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
              xReturn = xTaskCreate((TaskFunction_t )(void(*)(void*))App_Record,  /* 任务入口函数 */
                            (const char*    )"Record Task",       /* 任务名字 */
                            (uint16_t       )(2*1024)/4,            /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )recfilename,         /* 任务入口函数参数 */
                            (UBaseType_t    )4,                   /* 任务的优先级 */
                            (TaskHandle_t  )&h_record);           /* 任务控制块指针 */

              if(xReturn != pdPASS )
              {
                GUI_ERROR("GUI rec thread create failed");
              }

							SetTimer(hwnd, ID_Record_Timer, 1000, TMR_START, NULL);                    // 开启录音计时定时器
							/* 按钮处理 */
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_BUGLE), L"Q");
							SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"正在录音");             // 设置录音机状态
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_START), SW_HIDE);                    // 窗口隐藏开始录音的按钮
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_SHOW);                    // 窗口显示继续和暂停的按钮
							SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_LOCKCURSEL, TRUE, 0);     // 锁定列表不能重新修改选中的项
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_PLAY), DISABLE);                   // 禁用播放按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_BACK), DISABLE);                   // 禁用上一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_NEXT), DISABLE);                   // 禁用下一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), ENABLE);                    // 使能停止录音按钮
            }
            break;

            case ID_RECORD_STOP:
            {
              RecordFlag = 2;
              vTaskResume(h_record);                  // 恢复录音任务
              KillTimer(hwnd, ID_Record_Timer);       // 删除录音计时定时器
              Record_Timer = 0;
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_TIME), L"00:00");
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_PADNC), L"U");         // 设置录音机状态

              /* 文件记录处理 */		
              char *cbuf = NULL;
              WCHAR wbuf[128];
              int i = 0;
              memcpy(music_playlist[music_file_num], recfilename, strlen(recfilename) + 1);
              printf("%s\r\n", music_playlist[music_file_num]);
              cbuf = strstr(recfilename, "/");    // 跳过第一个 / 
              cbuf = strstr(cbuf+1, "/");         // 跳过第二个 /
              if (cbuf != NULL)
              { 
              }
              x_mbstowcs(wbuf, cbuf + 1, sizeof(wbuf));
              //在Listbox中增加一个Item项
              i = SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_GETCOUNT, 0, 0);
              SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_ADDSTRING, i, (LPARAM)wbuf);
              music_file_num++;//记录文件个数
              InvalidateRect(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), NULL, TRUE);    // 无效化列表，使其重绘
              
              /* 停止录音 */
							SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"录音机");                // 设置录音机状态
							SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_LOCKCURSEL, FALSE, 0);     // 解锁列表可以重新修改选中的项
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_START), SW_SHOW);                     // 显示开始录音的按钮
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_HIDE);                     // 隐藏继续和暂停的按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_BACK), ENABLE);                     // 使能上一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_NEXT), ENABLE);                     // 使能下一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_PLAY), ENABLE);                     // 使能播放按钮
							EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), DISABLE);                    // 禁用停止录音按钮
            }
            break;

						/* 暂停继续录音按钮 */
            case ID_RECORD_PADNC:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PADNC);
              
							GetWindowText(wnd, wbuf, 3);
							if (wbuf[0] == L'U')
							{
								KillTimer(hwnd, ID_Record_Timer);       // 删除录音计时定时器
                vTaskSuspend(h_record);    // 挂起录音任务
//                I2Sxext_Recorde_Stop();    // 停止录音
								SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"已暂停");          // 设置录音机状态
								SetWindowText(wnd, L"T");
							}
							else
							{
								SetTimer(hwnd, ID_Record_Timer, 1000, TMR_START, NULL);               // 继续录音计时定时器
                vTaskResume(h_record);      // 恢复录音任务
//                I2Sxext_Recorde_Start();    // 继续录音
								SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"正在录音");         // 设置录音机状态
								SetWindowText(wnd, L"U");
							}
            }
            break;
#if 0
            /* 音频输出选择按钮 */
            case ID_RECORD_BUGLE:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_BUGLE);
              
							GetWindowText(wnd, wbuf, 3);
							if (wbuf[0] == L'P')
							{
								SetWindowText(wnd, L"Q");
                VS_Horn_Set(DISABLE);
							}
							else
							{
								SetWindowText(wnd, L"P");
                VS_Horn_Set(ENABLE);
							}
            }
            break;
#endif
            /* 音量调节按钮被按下 */
            case ID_RECORD_bPOWER:
            {
							if (BUGLE_STATE == 1)
							{
                BUGLE_STATE = 0;
								ShowWindow(GetDlgItem(hwnd, ID_RECORD_sPOWER), SW_HIDE); //窗口隐藏
							}
							else
							{
                BUGLE_STATE = 1;
								ShowWindow(GetDlgItem(hwnd, ID_RECORD_sPOWER), SW_SHOW); //窗口显示
							}
            }
            break;
            
          }
        }
        else if (LBN_SELCHANGE == code)   // 列表选中项改变
        {
          if (id == ID_RECORD_LIST)
          {
            
            WCHAR wbuf[3];
            HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
            
            GetWindowText(wnd, wbuf, 3);
            if (wbuf[0] == L'T')
            {
              vTaskResume(h_play_record);            // 当前是挂起的，恢复任务
              SetWindowText(wnd, L"U");
						  EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
            }
              
//            I2S_Play_Start();
            play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // 获得当前选中项
//            mp3player.ucStatus = STA_SWITCH;
            Replay = 0;
          }
        }
         
         /* 进度条处理 */
          nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
          //音量条处理case
          if (ctr_id == ID_RECORD_sPOWER)
          {
             NM_SCROLLBAR *sb_nr;
             sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
             static int ttt = 0;
             switch (nr->code)
             {
                case SBN_THUMBTRACK: //R滑块移动
                {
                   power_rec = sb_nr->nTrackValue; //得到当前的音量值
                   if(power_rec == 0) 
                   {
//                      wm8978_OutMute(1);//静音
                      SetWindowText(GetDlgItem(hwnd, ID_RECORD_bPOWER), L"J");
                      ttt = 1;
                   }
                   else
                   {
                      if(ttt == 1)
                      {
                         ttt = 0;
                         SetWindowText(GetDlgItem(hwnd, ID_RECORD_bPOWER), L"A");
                      }
                     
                   } 
                   SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power_rec); //发送SBM_SETVALUE，设置音量值
                }
                break;
                
               case SBN_CLICKED:
               {
                 VS_Set_Vol(power_rec);     // 设置vs1053的音量
               }
               break;
             }
          }

          //进度条处理case
          if (ctr_id == ID_PLAY_PROGRESS)
          {
             NM_SCROLLBAR *sb_nr;
             int i = 0;
             sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
             switch (nr->code)
             {
                case SBN_THUMBTRACK: //R滑块移动
                {
                   i = sb_nr->nTrackValue; //获得滑块当前位置值                
                   SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //设置进度值
                   chgsch = 2;
                }
                break;
                
               case SBN_CLICKED:
               {
                 //置位进度条变更位置
                   chgsch = 1;
               }
               break;
             }
          }         
      
      break;
    }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_RECORD_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_STOP)
         {
            stop_owner_draw(ds);    // 重绘停止录音按钮
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_START)
         {
            start_owner_draw(ds);    // 重绘开始录音按钮
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_sPOWER || ds->ID == ID_PLAY_PROGRESS)   
         {
            scrollbar_owner_draw(ds);    // 进度条重绘
            return TRUE;
         }
         else if (ds->ID >= ID_RECORD_bPOWER && ds->ID<= ID_RECORD_PADNC)
         {
            button_owner_draw(ds);    // 按钮重绘
            return TRUE;
         }
         else if (ds->ID >= ID_RECORD_STATE && ds->ID<= ID_PLAY_TIME)
         {
            Brigh_Textbox_OwnerDraw(ds);    // 重绘透明文本
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_LIST)
         {
            listbox_owner_draw(ds);    // 重绘列表
            return TRUE;
         }
         return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc;//屏幕hdc
        RECT rc = {181, 3, 75, 20};

        //开始绘制
        hdc = BeginPaint(hwnd, &ps); 

				DrawText(hdc, L"录音文件", -1, &rc, NULL);//绘制文字(居中对齐方式)

        EndPaint(hwnd, &ps);

        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
        SetBrushColor(hdc, MapRGB(hdc, 95, 137, 159));//130, 195, 213
        FillRect(hdc, &rc);
				
//				RECT rc1 = {120, 0, 200, GUI_YSIZE};
//        SetBrushColor(hdc, MapRGB(hdc, RECORDER_BACK_COLOR)); 
//        FillRect(hdc, &rc1);
        
				RECT rc_grad = {110, 0, 3, GUI_YSIZE};
        GradientFillRect(hdc, &rc_grad, MapRGB(hdc, 50, 50, 50), MapRGB(hdc, 95, 137, 159), FALSE);
        return TRUE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {        
        Record_Timer = 0;     // 复位录音计时
//        mp3player.ucStatus = STA_IDLE;
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {
        vTaskResume(h_play_record);
        BUGLE_STATE = 0;
									
        chgsch = 0;
        RecordFlag = 2;
        Replay = 0;
        thread = 0;
        GUI_msleep(10);    // 进行任务调度
        while(!thread)
        {
          GUI_msleep(10);
        }    // 等待任务被删除
        
//        vTaskDelete(h_record);
//        vTaskDelete(h_play_record);
        music_file_num = 0;   // 复位文件记录

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_RECORDER_DIALOG(void)
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
	Recorer_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI RECORDER DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Recorer_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Recorer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


