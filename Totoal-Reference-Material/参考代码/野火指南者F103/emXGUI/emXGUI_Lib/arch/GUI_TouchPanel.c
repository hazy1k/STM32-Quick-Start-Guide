
#include    <string.h>

#include <emXGUI.h>
#include	"GUI_Drv.h"

#if 1/*(GUI_TOUCHSCREEN_EN > 0)*/

/*============================================================================*/

#define	TS_CAL_XOFFSET		(40)
#define	TS_CAL_YOFFSET		(40)

/*============================================================================*/

static	BOOL ts_xy_swap =FALSE;	     //是否XY交换
static	TS_CFG_DATA	ts_cal_data;	//校正数据
static	TS_CFG_DATA	ts_cfg_data;	//配置数据


/*============================================================================*/

static int	touch_screen_data_init(void);

void TouchPanel_TranslateInit(const TS_CFG_DATA *cfg)
{
	memcpy(&ts_cfg_data,cfg,sizeof(TS_CFG_DATA));
	touch_screen_data_init();
}

/*============================================================================*/

#if 0
void	GUI_TouchHandler(void)
{
	int act;
	POINT pt;

	act =TouchDev_GetAction();
	if(act==TS_ACT_DOWN)
	{
		if(TouchDev_GetPoint(&pt))
		{
			MouseInput(pt.x,pt.y,MK_LBUTTON);
		}
	}

	if(act==TS_ACT_UP)
	{
		GetCursorPos(&pt);
		MouseInput(pt.x,pt.y,0);
	}

}
#endif

/*============================================================================*/

BOOL	TouchPanel_TranslatePoint(int width,int height,POINT *pt)
{
	s16 x,y,adx,ady;
	s16	adx_min,adx_max,ady_min,ady_max;

	if(pt != NULL)
	{
		adx	=pt->x;
		ady	=pt->y;

		adx_min	=ts_cfg_data.LUx;
		ady_min	=ts_cfg_data.LUy;

		adx_max	=ts_cfg_data.RDx;
		ady_max	=ts_cfg_data.RDy;

		if((adx_min==adx_max) || (ady_min==ady_max))
		{
			pt->x = -1;
			pt->y = -1;
			return	FALSE;
		}

		if(ts_xy_swap)
		{
			x = TS_CAL_XOFFSET+(width-(TS_CAL_XOFFSET*2))*(ady-ady_min) / (ady_max-ady_min);
			y = TS_CAL_YOFFSET+(height-(TS_CAL_YOFFSET*2))*(adx-adx_min) / (adx_max-adx_min);
		}
		else
		{
			x = TS_CAL_XOFFSET+(width-(TS_CAL_XOFFSET*2))*(adx-adx_min) / (adx_max-adx_min);
			y = TS_CAL_YOFFSET+(height-(TS_CAL_YOFFSET*2))*(ady-ady_min) / (ady_max-ady_min);
		}

		pt->x =x;
		pt->y =y;
		return	TRUE;
			
	}
	return	FALSE;
}

/*============================================================================*/
//设置每个校准点的采样数据

int	TouchPanel_SetCalibrationSample(POINT *pt,int idx)
{
	switch(idx)
	{
		
		case	0:
				ts_cal_data.LUx	=pt->x;
				ts_cal_data.LUy	=pt->y;
				return	1;
				////
			
		case	1:
				ts_cal_data.RUx	=pt->x;
				ts_cal_data.RUy	=pt->y;
				return	1;
				////
				
		case	2:
				ts_cal_data.RDx	=pt->x;
				ts_cal_data.RDy	=pt->y;
				return	1;
				////
				
		case	3:
				ts_cal_data.LDx	=pt->x;
				ts_cal_data.LDy	=pt->y;
				return	1;
				////
		default:
				return	0;				
				
			
	}
}


/*============================================================================*/
//获取校准点的屏幕坐标

int	TouchPanel_GetCalibrationPoints(POINT *pt)
{
	int xsize,ysize;

	if(pt!=NULL)
	{
		xsize =GUI_XSIZE;
		ysize =GUI_YSIZE;

		pt[0].x	=TS_CAL_XOFFSET;
		pt[0].y	=TS_CAL_YOFFSET;
		
		pt[1].x	=xsize-TS_CAL_XOFFSET;
		pt[1].y	=TS_CAL_YOFFSET;

		pt[2].x	=xsize-TS_CAL_XOFFSET;
		pt[2].y	=ysize-TS_CAL_YOFFSET;
		
		pt[3].x	=TS_CAL_XOFFSET;
		pt[3].y	=ysize-TS_CAL_YOFFSET;
				
	}
	
	return	4;
}

/*============================================================================*/
/*============================================================================*/
//触摸屏校正开始

int	TouchPanel_CalibrationStart(void)
{

	GUI_Printf("TouchPanelCalibrationStart.\r\n");

	return TRUE;//TouchPanel_HardInit();
}

/*============================================================================*/
//触摸屏校正结束

int	TouchPanel_CalibrationFinish(void)
{

	ts_cal_data.ChkSum	= 0;
	ts_cal_data.ChkSum	+= ts_cal_data.LUx+ts_cal_data.LUy;
	ts_cal_data.ChkSum	+= ts_cal_data.RUx+ts_cal_data.RUy;
	ts_cal_data.ChkSum	+= ts_cal_data.RDx+ts_cal_data.RDy;
	ts_cal_data.ChkSum	+= ts_cal_data.LDx+ts_cal_data.LDy;

	memcpy(&ts_cfg_data,&ts_cal_data,sizeof(TS_CFG_DATA));
	
	touch_screen_data_init();
	GUI_Printf("TouchPanelCalibrationFinish.\r\n");
	return TRUE;
}

/*============================================================================*/
//触摸屏校正成功

int	TouchPanel_CalibrationSucceed(void)
{

	TouchDev_SaveCfg(&ts_cal_data);
	GUI_Printf("TouchPanelCalibrationSucceed.\r\n");
	return TRUE;

}

/*============================================================================*/
//触摸屏校正失败

int	TouchPanel_CalibrationFailed(void)
{
	if(TouchDev_LoadCfg(&ts_cfg_data))
	{
		TouchPanel_TranslateInit(&ts_cfg_data);
	}
	else
	{
	}
	
	GUI_Printf("TouchPanelCalibrationFailed.\r\n");
	return	TRUE;
}




/*============================================================================*/

static	U32	x_abs(int a)
{
	return	a>0 ? a:0-a;
}

//检查触摸屏XY是否交换
static BOOL	check_touch_screen_swap(const TS_CFG_DATA *pDat)
{
	int lu_x,ru_x;
	int	lu_y,ru_y;
	int	dx,dy;
	////
	
	lu_x = pDat->LUx;
	ru_x = pDat->RUx;
	
	lu_y = pDat->LUy;
	ru_y = pDat->RUy;
	
	dx	=x_abs(lu_x-ru_x);
	dy	=x_abs(lu_y-ru_y);
	
	if(dx > dy)
	{
		ts_xy_swap	=FALSE;		//XY不交换.
	}
	else
	{
		ts_xy_swap	=TRUE;		//XY交换.
	}
	
	return ts_xy_swap;

}

/*============================================================================*/
//初始化触摸屏数据

static int	touch_screen_data_init(void)
{
	
	check_touch_screen_swap(&ts_cfg_data);


#if TS_DBG

	GUI_Printf("TS_LUAddata =%d, %d\n",ts_cfg_data.LUAdx,ts_cfg_data.LUAdy);
	GUI_Printf("TS_RUAddata =%d, %d\n",ts_cfg_data.RUAdx,ts_cfg_data.RUAdy);
	GUI_Printf("TS_RDAddata =%d, %d\n",ts_cfg_data.RDAdx,ts_cfg_data.RDAdy);
	GUI_Printf("TS_LDAddata =%d, %d\n",ts_cfg_data.LDAdx,ts_cfg_data.LDAdy);
	GUI_Printf("TS_XYSwap   =%d\n",ts_xy_swap);
	GUI_Printf("ts_org  =%d\n",ts_org);

#endif

	return 1;
	
}


/*============================================================================*/

/*============================================================================*/

extern const GUI_TOUCH_DEV *pTouchDev;

static int ts_state=TS_ACT_NONE;



//BOOL TouchDev_Init(void)
BOOL GTP_Init_Panel(void)
{
	ts_state=TS_ACT_NONE;


	if(pTouchDev)
	{
		pTouchDev->Init();
	}

	return 0;
}


int TouchDev_GetAction(void)
{
	static u8 act=0;
//	int old;

	act<<=1;
	act|=pTouchDev->GetState();

//	old =ts_state;

	switch(act&0x03)
	{
		case 0:
			ts_state = TS_ACT_NONE;
			break;
		case 1:
			ts_state = TS_ACT_DOWN;
			break;
		case 2:
			ts_state = TS_ACT_UP;
			break;
	}
	return ts_state;
/*
	if(ts_state!=old)
	{
		old =ts_state;
		return ts_state;
	}
	return TS_ACT_NONE;
*/

}


BOOL TouchDev_GetSample(POINT *pt)
{
	return pTouchDev->GetSample(pt);
}

//BOOL TouchDev_GetPoint(POINT *pt)
//{
//	return pTouchDev->GetPoint(pt);
//}

BOOL TouchDev_LoadCfg(TS_CFG_DATA *cfg)
{
	return pTouchDev->LoadCfg(cfg);
}

BOOL TouchDev_SaveCfg(TS_CFG_DATA *cfg)
{
	return pTouchDev->SaveCfg(cfg);
}

/*============================================================================*/

//void	TouchDev_Handler(void)
//{
//	int act;
//	POINT pt;

//	act =TouchDev_GetAction();
//	if(act==TS_ACT_DOWN)
//	{
//		if(TouchDev_GetPoint(&pt))
//		{
//			MouseInput(pt.x,pt.y,MK_LBUTTON);
//		}

//	}

//	if(act==TS_ACT_UP)
//	{
//		GetCursorPos(&pt);
//		MouseInput(pt.x,pt.y,0);
//	}

//}

/*============================================================================*/
/*============================================================================*/

#endif /*(GUI_TOUCHSCREEN_EN)*/

