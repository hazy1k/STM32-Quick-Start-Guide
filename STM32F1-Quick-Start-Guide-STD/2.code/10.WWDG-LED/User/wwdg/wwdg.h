#ifndef __WWDG_H__
#define __WWDG_H__

#include "stm32f10x.h"

void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t prv);
void WWDG_Feed(void);

#endif /* __WWDG_H__ */
