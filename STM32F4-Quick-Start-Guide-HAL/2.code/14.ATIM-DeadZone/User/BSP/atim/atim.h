#ifndef __ATIM_H
#define __ATIM_H

#include <sys.h>

void ATIM_PWM_Init(uint16_t arr, uint16_t psc);
void ATIM_PWM_SetDeadZone(uint16_t ccr, uint8_t dtg);

#endif /* __ATIM_H */
