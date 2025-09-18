#ifndef __DAC_H
#define __DAC_H

#include "sys.h"

void pwmdac_init(uint16_t arr, uint16_t psc);
void pwmdac_set_value(uint16_t value);

#endif /* __DAC_H */
