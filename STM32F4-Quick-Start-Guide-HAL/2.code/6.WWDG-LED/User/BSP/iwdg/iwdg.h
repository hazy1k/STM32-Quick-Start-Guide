#ifndef __IWDG_H__
#define __IWDG_H__

#include "sys.h"

void bsp_iwdg_init(uint8_t pre, uint16_t reload);
void iwdg_feed(void);

#endif /*  __IWDG_H__ */
