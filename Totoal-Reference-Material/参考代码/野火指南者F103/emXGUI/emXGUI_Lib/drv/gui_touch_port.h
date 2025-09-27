#ifndef __GUI_TOUCH_PORT_H
#define	__GUI_TOUCH_PORT_H

#ifdef __cplusplus
extern "C" {
#endif  
  
#include	"emXGUI.h"
#if defined(STM32F429_439xx)
#include "stm32f4xx.h"
#elif defined(STM32H743xx)
#include "stm32h7xx.h"
#elif STM32F10X_HD
#include "stm32f10x.h"
#endif


////Touch State
#define	TS_ACT_NONE	 	0
#define	TS_ACT_DOWN	 	1
#define   TS_ACT_UP		2

#ifdef STM32F10X_HD
  extern const GUI_TOUCH_DEV *pTouchDev;
#endif

BOOL GTP_Init_Panel(void);
int TouchDev_GetAction(void);
BOOL TouchDev_GetPoint(POINT *pt);

void 	GUI_TouchHandler(void);

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_TOUCH_PORT_H */
