
#include "stm32f10x.h"
#include "GUI_Drv.h"

/*============================================================================*/
#if 0
void	LCD_Init(void);

void LCD_HardInit(void)
{
	

	LCD_Init();
}

#endif


/*===================================================================================*/


#if 0

BOOL	LCD_SetRotate(int rotate)
{

	return FALSE;
}

/*===================================================================================*/

BOOL	LCD_SetRotate(int rotate);

BOOL	gdrvSetRotate(int rotate)
{
	BOOL res=FALSE;

	switch(rotate)
	{
		case ROTATE_0:
			if(LCD_SetRotate(ROTATE_0))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_XSIZE;
				surf_screen.Height =LCD_YSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		case ROTATE_90:
			if(LCD_SetRotate(ROTATE_90))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_YSIZE;
				surf_screen.Height =LCD_XSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		case ROTATE_180:
			if(LCD_SetRotate(ROTATE_180))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_XSIZE;
				surf_screen.Height =LCD_YSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		case ROTATE_270:
			if(LCD_SetRotate(ROTATE_270))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_YSIZE;
				surf_screen.Height =LCD_XSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		default:
			 break;

	}
	return res;
}
#endif

