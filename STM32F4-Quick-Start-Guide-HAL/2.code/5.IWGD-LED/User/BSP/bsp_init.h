#ifndef __BSP_INIT_H
#define __BSP_INIT_H

/* 系统头文件 */
#include "sys.h"
#include "usart.h"
#include "delay.h"
/* User header file */
#include "led.h"
#include "beep.h"
#include "key.h"
#include "iwdg.h"

void bsp_init(void);

#endif /* __BSP_INIT_H */
