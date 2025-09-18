#ifndef __RNG_H
#define __RNG_H

#include "sys.h"

uint8_t RNG_Init(void);
uint32_t rng_get_random(void);
uint32_t rng_get_random_range(int min, int max);

#endif /* __RNG_H */
