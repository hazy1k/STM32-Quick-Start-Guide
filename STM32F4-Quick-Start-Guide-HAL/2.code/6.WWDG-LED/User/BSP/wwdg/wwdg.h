#ifndef __WWDG_H__
#define __WWDG_H__

#include "sys.h"
#include "led.h"

void bsp_wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer);

#endif /* __WWDG_H__ */
