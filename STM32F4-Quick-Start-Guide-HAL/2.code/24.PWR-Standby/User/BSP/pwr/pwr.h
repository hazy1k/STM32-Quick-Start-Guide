#ifndef __PWR_H
#define __PWR_H

#include "sys.h"

void pwr_wkup_key_init(void);
void pwr_enter_sleep(void);
void pwr_enter_stop(void);
void pwr_enter_standby(void);

#endif /* __PWR_H */
