
#include	"BSP.h"

#include 	"GUI_Drv_Cfg.h"
#include 	"GUI_Drv.h"

#if	GUI_TOUCHSCREEN_EN && 0

/*============================================================================*/


#define	SET_TOUCH_DEV(obj)	  extern const GUI_TOUCH_DEV obj; \
							  const GUI_TOUCH_DEV *pTouchDev=(GUI_TOUCH_DEV*)&obj;

SET_TOUCH_DEV(TouchDev_ADS7843);

/*============================================================================*/



#endif	/* GUI_TOUCHSCREEN_EN */

