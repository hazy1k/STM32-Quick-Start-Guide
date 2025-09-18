#ifndef __BSP_INIT_H
#define __BSP_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include "led.h"
#include "beep.h"
#include "key.h"

void bsp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_INIT_H */
