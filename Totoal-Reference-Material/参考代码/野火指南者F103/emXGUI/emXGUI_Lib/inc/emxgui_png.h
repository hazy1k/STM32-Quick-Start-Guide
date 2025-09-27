
#ifndef	__EMXGUI_PNG_H__
#define	__EMXGUI_PNG_H__

#ifdef	__cplusplus
extern	"C"{
#endif

#include "emXGUI.h"

/*===================================================================================*/

typedef struct __PNG_DEC PNG_DEC;

PNG_DEC* PNG_Open(const u8 *png_dat, uint32_t size);
BOOL PNG_GetBitmap(PNG_DEC *png_dec,BITMAP *bm);
void PNG_Close(PNG_DEC *png_dec);


/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
