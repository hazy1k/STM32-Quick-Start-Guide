#ifndef __ATIM_H
#define __ATIM_H

#include <sys.h>

void tim8_pwmin_init(void);
void restart_pwmin_capture(void);
void tim14_pwmout_init(uint16_t arr, uint16_t psc);

#endif /* __ATIM_H */
