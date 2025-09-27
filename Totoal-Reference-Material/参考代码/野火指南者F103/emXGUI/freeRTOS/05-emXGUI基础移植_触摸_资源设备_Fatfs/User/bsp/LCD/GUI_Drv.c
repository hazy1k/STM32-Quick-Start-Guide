
#include	"Include.h"
#include	"GUI_Drv.h"
#include  "GUI_Drv_Cfg.h"

//#include    "x_file.h"

/*===================================================================================*/

HFONT	XFT_CreateFont(const void *xft_dat);
//void LCD_HardInit(void);
extern SURFACE*	EXT_LCD_CreateSurfaceRGB565(int Width,int Height);
//void LCD_BkLight(int on);
extern BOOL Load_state;
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*
函数功能: GUI低级别的初始化
参数: 无
返回: TRUE:成功; FALSE：失败.
说明: 这是GUI的第一个初始化函数.
*/
//void GUI_VMEM_Init(void);
//BOOL GUI_LowLevelInit(void)
//{
//  GUI_VMEM_Init();
//  
//	return TRUE;
//}


/*===================================================================================*/
/*===================================================================================*/
/*
函数功能: GUI显示设备的初始化
参数: 无
返回: 显示设备Surface对象指针
说明:
*/
#if 0
SURFACE* GUI_DisplayInit(void)
{
	int width_bytes;
	SURFACE *pSurf;

	//LCD硬件初始化
	LCD_HardInit();

	pSurf =EXT_LCD_CreateSurfaceRGB565(LCD_XSIZE,LCD_YSIZE);

	pSurf->GL->FillArea(pSurf,0,0,LCD_XSIZE,LCD_YSIZE,pSurf->CC->MapRGB(0,0,0)); //清屏
//	LCD_BkLight(TRUE); //打开背光

	while(0)
	{ //测试
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,255,255));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,0,0));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,255,0));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,255));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,0));
		GUI_msleep(200);
		break;
	}

#if FRAMEBUFFER_EN //使用帧缓冲支持(帧缓冲使用屏幕相同的格式和尺寸).
	{
		const SURFACE  *pSurf_FB;

		pSurf_FB =CreateSurface(pSurf->Format,pSurf->Width,pSurf->Height,0,NULL);
		GUI_SetFrameBufferSurface(pSurf_FB);
	}
#endif

	return pSurf;
}
#endif

/*===================================================================================*/
/*
函数功能: GUI输入设备的初始化
参数: 无
返回: 无
说明:
*/
//void GUI_InputInit(void)
//{
//#if(GUI_KEYBOARD_EN)
//{
//	KeyBoardDev_Init();
//}
//#endif

//#if(GUI_TOUCHSCREEN_EN)
//{
//	if(TouchDev_Init()) /*触摸屏设备初始化*/
//	{
//#if(GUI_TOUCHSCREEN_CALIBRATE)
//		TS_CFG_DATA ts_cfg;
//		if(TouchDev_LoadCfg(&ts_cfg)) /*加载校正数据(电阻屏需要)*/
//		{
//			TouchPanel_TranslateInit(&ts_cfg); /*初始化坐标转换函数(电阻屏需要)*/
//		}
//#endif
//	}

//}
//#endif

//#if(GUI_MOUSE_EN)
//{
//	MouseDev_Init();
//}
//#endif

//}

/*===================================================================================*/
/*
函数功能: GUI输入设备的执行
参数: 无
返回: 无
说明: 该函数会被周期性轮询调用
*/
//void GUI_InputHandler(void)
//{

//#if(GUI_KEYBOARD_EN)
//{
//	if(KeyBoardDev_IsActive())
//	{
//		KeyBoardDev_Handler();
//	}
//}
//#endif

//#if(GUI_TOUCHSCREEN_EN)
//{
//	TouchDev_Handler(); //调用触摸屏处理函数
//}
//#endif

//#if(GUI_MOUSE_EN)
//{
//	if(MouseDev_IsActive())
//	{
//		MouseDev_Handler();
//	}
//}
//#endif

//}

/*===================================================================================*/
/*
函数功能: 动态内存申请(用于GUI内核对象)
参数: size: 需要申请内存的字节数
返回: 内存首址
说明:
*/

//void*	GUI_MEM_Alloc(U32 size)
//{
//	void *p=NULL;

//	p =vmalloc(size);
//	if(p==NULL)
//	{
//	    GUI_Printf("#Error: GUI_MEM_Alloc.\r\n");
//	}

//	return p;
//}

/*===================================================================================*/
/*
函数功能: 动态内存释放(用于GUI内核对象)
参数: p:需要释放的内存首址
返回: 无
说明:
*/
//void	vfree(void *p);
//void	GUI_MEM_Free(void *p)
//{
//	vfree(p);
//}

/*===================================================================================*/
/*
函数功能: 显示动态内存申请(用于GUI显示缓存)
参数: size: 需要申请内存的字节数
返回: 内存首址
说明:
*/

//void*	GUI_GRAM_Alloc(U32 size)
//{
//	return vmalloc(size);
//}

/*===================================================================================*/
/*
函数功能: 显示动态内存释放(用于GUI显示缓存)
参数: p:需要释放的内存首址
返回: 无
说明:
*/

//void	GUI_GRAM_Free(void *p)
//{
//	vfree(p);
//}

/*===================================================================================*/

/*
函数功能: 格式化字符串输出
参数: 可变参数
返回: 无
说明: 如果不用输出GUI调试信息,这个函数可以为空
*/
#if 0
void	GUI_Printf(const char *fmt,...)
{

  	x_va_list ap;
    char *buf;
    static	char str_buf[256];
    static  GUI_MUTEX *mutex_gui_printf=NULL;
  	////
  	buf	=str_buf/*(char*)GUI_MEM_Alloc(256)*/;
  	if(buf)
  	{

    	if(GUI_MutexLock(mutex_gui_printf,0xFFFFFFFF))
    	{
    	    x_va_start(ap,fmt);
       	 	x_vsprintf(buf,fmt,ap);
       		DebugPuts(buf);
       		//DebugOutput(buf);

       		GUI_MutexUnlock(mutex_gui_printf);
       		x_va_end(ap);
   		}

		//GUI_MEM_Free(buf);
	}

}
#endif
/*===================================================================================*/
/*===================================================================================*/

#if FONT_XFT_EN

//#include "GUI_Font_XFT.h"

extern const char GB2312_16_2BPP[];
extern const char GB2312_20_2BPP[];

extern const char GB2312_16_4BPP[];
extern const char GB2312_20_2BPP[];
extern const char GB2312_24_2BPP[];
extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];

static const char font_path_xft[] ="B:kernel/fonts/GB2312_20_4BPP.xft";
static u8 *pFontData_XFT=NULL;

#endif

//////////////


#if FONT_TTF_EN

#include    "FontDrv_FreeType.h"

struct __freetype_font_obj *ft_obj;


#endif


/*===================================================================================*/

#if 1

int RES_GetOffset(const char *res_name);
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);

static int read_data(void *buf,int offset,int size,LONG lParam)
{
	offset += lParam;
	RES_DevRead(buf,offset,size);
	return size;
}

#endif

/*===================================================================================*/
/*
函数功能: GUI默认字体初始化
参数: 无
返回: 返回默认字体的句柄
说明:
*/
#if 0
HFONT GUI_FontInit(void)
{

	HFONT hFont=NULL;

#if	FONT_XFT_EN  /*XFT格式字体*/
{

#if 0
{
	/*丛文件系统加载本地内存*/

    X_FILE *fp;
    int fsize;

    fp =x_fopen(font_path_xft,"rb");
    if(fp != NULL)
    {
    	fsize =x_fsize(fp);

    	pFontData_XFT =(u8*)vmalloc(fsize);
    	if(pFontData_XFT!=NULL)
    	{
    		x_fread((u8*)pFontData_XFT,fsize,1,fp);
    	}
    	x_fclose(fp);
    	////
    	hFont 	=XFT_CreateFont(pFontData_XFT);
    }
}
#endif

#if 0
    if(hFont==NULL)
    { /*丛流设备加载(RES储存设备)*/

    	int offset;
    	offset =RES_GetOffset("GB2312_16_4BPP.xft");
    	//offset =RES_GetOffset("GB2312_20_4BPP.xft");
    	if(offset > 0)
    	{
    		hFont =XFT_CreateFontEx(read_data,offset);
    	}
    }
#endif

    if(hFont==NULL)
    { /*丛本地加载(本地数组数据)*/

    	//hFont =XFT_CreateFont(ASCII_16_1BPP);
    	hFont =XFT_CreateFont(ASCII_16_4BPP);
    	//hFont =XFT_CreateFont(ASCII_20_4BPP);

    	//hFont =XFT_CreateFont(GB2312_16_2BPP);
    	//hFont =XFT_CreateFont(GB2312_20_2BPP);
    }
    
    /* 不使用外部字体时，默认把Load_state至1 */  
#if ((!GUI_EXTERN_FONT_EN) || (!GUI_APP_BOOT_INTERFACE_EN)) 
    Load_state = TRUE;
#endif  

}
#endif

#if	FONT_TTF_EN /*TTF格式字体*/
{

    X_FILE *fp;
    u8 *pFontBuffer=NULL;
    const char *font_path ="C:kernel/fonts/def_font.ttf";
    int fsize;

    fp =x_fopen(font_path,"rb");
    if(fp != NULL)
    {
    	fsize =x_fsize(fp);
    	pFontBuffer =(u8*)vmalloc(fsize);
    	{
    		x_fread((u8*)pFontBuffer,fsize,1,fp);
    	}
    	x_fclose(fp);
    	////
    }

    ft_obj=freetype_font_obj_new(20,20,72,72,pFontBuffer);
    hFont =freetype_CreateFont(ft_obj);

}
#endif

	return hFont;
}
#endif

/*===================================================================================*/
void	GUI_UserAppStart(void);
//void    ShellWindowExecu(void);


static	int	gui_app_thread(void *p)
{

//	SYS_thread_set_priority(NULL,-2);

    #if (GUI_TOUCHSCREEN_EN & GUI_TOUCHSCREEN_CALIBRATE)
    {
        int i=0;
      //  while(TouchPanel_IsPenDown())
    	{
    		GUI_msleep(100);
    		//if(i++>10)
    		{
    			ShowCursor(FALSE);
    			TouchScreenCalibrate();
    			ShowCursor(TRUE);
    			//break;
    		}
    	}
    }
    #endif

    GUI_UserAppStart();
//   	ShellWindowStartup();
    return 0;
}

/*===================================================================================*/


static	int	gui_input_thread(void *p)
{
//	SYS_thread_set_priority(NULL,+4);
	while(1)
	{
		GUI_InputHandler(); //处理输入设备
		GUI_msleep(20);
	}
}

//用户的桌面回调函数.
static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    RECT rc;

	switch(msg)
	{
		case	WM_CREATE:	//桌面创建时,会产生该消息,可以在这里做一些初始化工作.

		        //初始化输入设备
//		        GUI_InputInit();

				////创建1个30ms定时器，处理循环事件.
				//SetTimer(hwnd,1,30,TMR_START,NULL);

//				SYS_thread_create(gui_input_thread,NULL,4*KB,NULL,0);

				//创建App线程
//				SYS_thread_create(gui_app_thread,NULL,10*KB,NULL,0);

				break;
				////

		////////
		case	WM_TIMER:
		{
			u16 id;

			id =LOWORD(wParam);
			if(id==1)
			{
			//	GUI_InputHandler(); //处理输入设备
			}
		}
		break;
		////
		case WM_ERASEBKGND:
		{

			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapRGB(hdc,50,80,150));
			FillRect(hdc,&rc);
		}
		return TRUE;
		////
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;

			//GetClientRect(hwnd,&rc);
			hdc =BeginPaint(hwnd,&ps); //开始绘图


			rc.x =10;
			rc.y =20;
			rc.w =150;
			rc.h =28;
			SetBrushColor(hdc,MapRGB(hdc,40,60,100));
			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			DrawText(hdc,L"emXGUI Desktop",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////


		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

//void GUI_DesktopStartup(void)
//{
//	WNDCLASS	wcex;
//	HWND hwnd;
//	MSG msg;

//	wcex.Tag 		    = WNDCLASS_TAG;

//	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= desktop_proc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= NULL;//hInst;
//	wcex.hIcon			= NULL;
//	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

//	//创建桌面窗口.
//	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
//								&wcex,
//								L"DESKTOP",
//								WS_VISIBLE|WS_CLIPCHILDREN,
//								0,0,GUI_XSIZE,GUI_YSIZE,
//								NULL,0,NULL,NULL);

//	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

//	//显示桌面窗口.
//	ShowWindow(hwnd,SW_SHOW);

//	//设置系统打开光标显示(可以按实际情况看是否需要).
////	ShowCursor(TRUE);

//	while(GetMessage(&msg,hwnd))
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}

//}
