#ifndef __DAC_H
#define __DAC_H

#include "sys.h"

void dac_init(uint8_t outx);
void dac_set_voltage(uint8_t outx, uint16_t voltage);
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n);

#endif /* __DAC_H */
