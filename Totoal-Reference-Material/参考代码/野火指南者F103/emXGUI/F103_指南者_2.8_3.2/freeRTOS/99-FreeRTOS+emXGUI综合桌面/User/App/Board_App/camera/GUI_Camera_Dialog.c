#include <emXGUI.h>
#include <string.h>
#include "Widget.h"
#include "bsp_ov7725.h"
#include "x_libc.h"


extern void Write_Pixel_From_Buf(uint16_t sx,uint16_t sy,uint16_t width,
	uint16_t height);


TaskHandle_t h_autofocus;
BOOL update_flag = 0;//帧率更新标志
uint8_t fps=0;//帧率
HWND Cam_hwnd;//主窗口句柄
static SURFACE *pSurf;
GUI_SEM *cam_sem = NULL;//更新图像同步信号量（二值型）
//uint16_t *cam_buff;
static uint8_t OV7725_State = 0;
/*
 * @brief  读一整帧图像
 * @param  图像的缓冲区
 * @retval NONE
*/
void OV7725_Read_Frame(uint16_t *p)
{
  /* 进入临界段，临界段可以嵌套 */
//  taskENTER_CRITICAL();
  
  for(int i = 0; i < 240*320; i++)
  {
    READ_FIFO_PIXEL(*p);		/* 从FIFO读出一个 rgb565 像素到 p 变量 */
    p++;
  }
  
//  taskEXIT_CRITICAL();
}

/*
 * @brief  更新屏幕
 * @param  NONE
 * @retval NONE
*/
static void Update_Dialog()
{
  /* ov7725 场信号线初始化 */
  Ov7725_vsync = 0;
  VSYNC_Init();
  
  
	while(1) //线程已创建了
	{
    GUI_SemWait(cam_sem, 0xFFFFFFFF);

    FIFO_PREPARE;  			/*FIFO准备*/

		;    // 读一帧图像
		Write_Pixel_From_Buf(0,0,320,240);

    fps ++;                         // 帧率自加
  Ov7725_vsync = 0;
//    InvalidateRect(Cam_hwnd,NULL,FALSE);
    OV7725_State = 2;
	}
}
extern int MessageDialogBox(HWND Phwnd, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);
/*
 * @brief  摄像头窗口回调函数
*/
static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static int old_fps = 0;

  switch(msg)
  {
    case WM_CREATE:
    {
		  cam_sem = GUI_SemCreate(0,1);//同步摄像头图像
      /* ov7725 gpio 初始化 */
      Ov7725_GPIO_Config();
      OV7725_State = 1;
      
      if(Ov7725_Init())
      {
        GUI_DEBUG("OV7725 初始化成功");
        OV7725_State = 0;
        
      //创建自动对焦线程
      xTaskCreate((TaskFunction_t )(void(*)(void*))Update_Dialog,  /* 任务入口函数 */
                            (const char*    )"Update_Dialog",/* 任务名字 */
                            (uint16_t       )1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,/* 任务入口函数参数 */
                            (UBaseType_t    )5, /* 任务的优先级 */
                            (TaskHandle_t  )&h_autofocus);/* 任务控制块指针 */
      }
      else
      {
//        RECT RC;
//        MSGBOX_OPTIONS ops;
//        //const WCHAR *btn[]={L"确定"};
//        
//        const WCHAR *btn[] ={L"OK", L"晓得了"};      //对话框内按钮的文字

//        ops.Flag =MB_ICONERROR;
//        ops.pButtonText =btn;
//        ops.ButtonCount =2;
//        RC.w =200;
//        RC.h =120;
//        RC.x =(GUI_XSIZE - RC.w) >> 1;
//        RC.y =(GUI_YSIZE - RC.h) >> 1;
//        MessageDialogBox(hwnd, RC, L"没有检测到OV7725摄像头，\n请重新检查连接。", L"错误", &ops); 
        OV7725_State = 1;     // 没有检测到摄像头
        SetTimer(hwnd,1,1,TMR_START|TMR_SINGLE,NULL);  
//        PostCloseMessage(hwnd);
        break;
      }
      break;  
    }
    case WM_LBUTTONDOWN://点击屏幕，关闭窗口
    {
      PostCloseMessage(hwnd);
      break;
    }
    case WM_TIMER://摄像头状态机
    {
      update_flag = 1;
      if(OV7725_State == 1)
      {
        RECT RC;
        MSGBOX_OPTIONS ops;        
        const WCHAR *btn[] ={L"确认",L"取消"};      //对话框内按钮的文字

        ops.Flag =MB_ICONERROR;
        ops.pButtonText =btn;
        ops.ButtonCount =2;
        RC.w = 200;
        RC.h = 120;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        GUI_DEBUG(" 按下了第%d个按钮", MessageDialogBox(hwnd, RC, L"没有检测到OV7725摄像头，\n请重新检查连接。", L"错误", &ops));
        OV7725_State = 1;     // 没有检测到摄像头
        PostCloseMessage(hwnd);
      }
      break;
    }
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      WCHAR wbuf[20];
      RECT rc;
      
      hdc = BeginPaint(hwnd,&ps);
      GetClientRect(hwnd,&rc);
      if(OV7725_State != 2)
      {
        SetTextColor(hdc,MapRGB(hdc,250,250,250));
        SetBrushColor(hdc,MapRGB(hdc,50,0,0));
        SetPenColor(hdc,MapRGB(hdc,250,0,0));
//        SetTimer(hwnd,1,20,TMR_SINGLE,NULL);  
        DrawText(hdc,L"正在初始化摄像头\r\n\n请等待...",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);
      }              
      if(OV7725_State == 2)
      {   
        
      }
#if 0
      // 更新窗口分辨率
      if(update_flag)
      {
        update_flag = 0;
        old_fps = fps;
        fps = 0;
      }
      
      rc.x = 0;
      rc.y = 0;
      rc.w = 70;
      rc.h = 15;
      
      x_wsprintf(wbuf,L"帧率:%dFPS",old_fps);
      DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);
#endif
      EndPaint(hwnd,&ps);
      Ov7725_vsync = 0;    // 开始下一帧图像的采集
      break;
    }

    case WM_DESTROY:
    {
      old_fps = 0;
      fps = 0;
      
      if (OV7725_State != 1)
      {
        GUI_SemDelete(cam_sem);
        GUI_Thread_Delete(h_autofocus);
      }
			
      discameraexit();             // 关闭中断
      DisableCamera();
      return PostQuitMessage(hwnd);	
    }
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_Camera_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;  
  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                            &wcex,
                            L"GUI_Camera_Dialog",
                            WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                            0, 0, GUI_XSIZE, GUI_YSIZE,
                            NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Cam_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
  }
}
