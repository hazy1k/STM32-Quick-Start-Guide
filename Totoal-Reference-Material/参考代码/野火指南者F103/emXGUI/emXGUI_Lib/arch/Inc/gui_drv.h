/**
  *********************************************************************
  * @file    gui_drv.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   总的驱动头文件
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#ifndef	__GUI_DRV_H__
#define	__GUI_DRV_H__


#include <emXGUI.h>
#include <emXGUI_Arch.h>
#include	<stddef.h>
#include	<math.h>

#include "gui_drv_cfg.h"
#include "gui_mem_port.h"
#include "gui_lcd_port.h"
#include "gui_font_port.h"
#include "gui_log_port.h"

#if(GUI_INPUT_DEV_EN)
  #include "gui_input_port.h"
#endif

#if(GUI_RES_DEV_EN)
  #include "gui_resource_port.h"
#endif

#if(GUI_FS_EN)
  #include "gui_fs_port.h"
#endif

#ifndef	MIN
#define	MIN(a,b)	(((a)<(b))?(a):(b))
#endif

#ifndef	MAX
#define	MAX(a,b)	(((a)>(b))?(a):(b))
#endif


/*============================================================================*/
/*============================================================================*/
#if 0
struct	LCD_TIMING_CFG
{
   
	U8	VBPD;
	U8	VFPD;
	U8	VSPW;
   
	U8	HBPD;
   	U8	HFPD;
   	U8	HSPW;
   	
   	U8	DPL;
   	U8	EPL;

   	U8	HSPL;
   	U8	VSPL;
   	
   	U8  PCLK;

};

/*============================================================================*/

void	LCD_SPI_Init(void);
void	LCD_SPI_CS(int s);
void	LCD_SPI_MOSI(int s);
int		LCD_SPI_MISO(void);
void	LCD_SPI_SCLK(int s);
U8		LCD_SPI_RW(U8 byte);
#endif
/*============================================================================*/
////Touch State
#define	TS_ACT_NONE	 	0
#define	TS_ACT_DOWN	 	1
#define TS_ACT_UP		2

typedef	struct
{
	U16	rsv,ChkSum;
	U16	LUx,LUy;
	U16	RUx,RUy;
	U16	RDx,RDy;
	U16	LDx,LDy;
	
}TS_CFG_DATA;

typedef	struct
{

	BOOL (*Init)(void);
	int	 (*GetState)(void);
	BOOL (*GetSample)(POINT *pt);
	BOOL (*GetPoint)(POINT *pt);

	BOOL (*LoadCfg)(TS_CFG_DATA *cfg);
	BOOL (*SaveCfg)(TS_CFG_DATA *cfg);

}GUI_TOUCH_DEV;

#define	TouchPanel_IsPenDown() (TouchDev_GetAction()==TS_ACT_DOWN)
//#define	GUI_TouchHandler()	TouchDev_Handler()

BOOL	TouchDev_Init(void);
int  	TouchDev_GetAction(void);
BOOL	TouchDev_GetSample(POINT *pt);
BOOL	TouchDev_GetPoint(POINT *pt);
void 	TouchDev_Handler(void);
BOOL 	TouchDev_SaveCfg(TS_CFG_DATA *cfg);
BOOL 	TouchDev_LoadCfg(TS_CFG_DATA *cfg);

/////

void 	TouchPanel_TranslateInit(const TS_CFG_DATA *cfg);
BOOL	TouchPanel_TranslatePoint(int width,int height,POINT *pt);
BOOL	TouchPanel_SetCalibrationSample(POINT *pt,int idx);
int		TouchPanel_GetCalibrationPoints(POINT *pt);
BOOL	TouchPanel_CalibrationStart(void);
BOOL	TouchPanel_CalibrationFinish(void);
BOOL	TouchPanel_CalibrationSucceed(void);
BOOL	TouchPanel_CalibrationFailed(void);

BOOL	TouchScreenCalibrate(void);

/*============================================================================*/
BOOL	MouseDev_Init(void);
BOOL	MouseDev_IsActive(void);
void	MouseDev_Handler(void);

/*============================================================================*/
BOOL	KeyBoardDev_Init(void);
BOOL	KeyBoardDev_IsActive(void);
void	KeyBoardDev_Handler(void);

/*============================================================================*/

#if 0
void	GUI_memset8(U8 *dst,U8 dat,int count);
void	GUI_memset8_step(U8 *dst,U8 dat,int count,int step);
void	GUI_memset16(U16 *dst,U16 dat,int count);
void	GUI_memset16_step(U16 *dst,U16 dat,int count,int step);
void	GUI_memset32(U32 *dst,U32 dat,int count);
void	GUI_memset32_step(U32 *dst,U32 dat,int count,int step);

U8 GUI_AlphaBlendValue(U8 dst,U8 src,U8 src_a);
#endif

/*============================================================================*/

#if 0
void GL_MEM_8BPP_Init(GL_OP *gd);
void GL_MEM_16BPP_Init(GL_OP *gd);
void GL_MEM_32BPP_Init(GL_OP *gd);
void GL_MPU_16BPP_Init(GL_OP *gd);

extern	GL_OP GL_MEM_8PP;
extern	GL_OP GL_MEM_16PP;
extern	GL_OP GL_MEM_32PP;

void	SurfaceInit_RGB332(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_RGB565(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_ARGB4444(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_XRGB8888(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_ARGB8888(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
#endif

#if	GUI_KEYBOARD_EN
BOOL	gdrvKeyboardInit(void);
void	gdrvKeyBoardHandler(void);
#endif

#if	GUI_TOUCHSCREEN_EN
BOOL	gdrvTouchScreenInit(void);
void	gdrvTouchScreenHandler(void);
#endif

#if	GUI_MOUSE_EN
BOOL	gdrvMouseInit(void);
void	gdrvMouseDataInput(const char *data);
void	gdrvMouseHandler(void);
#endif

SURFACE*	GUI_DisplayInit(void);
BOOL		gdrvSetRotate(int rotate);

BOOL    GUI_Init(void);

/*============================================================================*/

void	GUI_Printf(const char *fmt,...);

/*============================================================================*/

/*============================================================================*/

#endif
