#ifndef __DAC_H
#define __DAC_H

#include "sys.h"

void dac_init(uint8_t outx);
void dac_set_voltage(uint8_t outx, uint16_t voltage);

#endif /* __DAC_H */
