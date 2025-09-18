#ifndef __WWDG_H
#define	__WWDG_H

#include "stm32f10x.h"

// 窗口计数器值，范围为：0x40-0x7F
#define WWDG_CNT 0x7F
void WWDG_Mode_Init(uint8_t tr, uint8_t wr, uint32_t prv);
void WWDG_Feed(void);

#endif /*__WWDG_H */
