#ifndef __MPU_H
#define __MPU_H

#include "main.h"

uint8_t mpu_config(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t de, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben);
void mpu_mem_init(void);

#endif /* __MPU_H */
