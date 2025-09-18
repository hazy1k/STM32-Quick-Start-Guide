#ifndef __BSP_INIT_H
#define __BSP_INIT_H

/* 系统头文件 */
#include <sys.h>
#include <usart.h>
#include <delay.h>
/* User header file */
#include <led.h>
#include <key.h>
#include <lcd.h>
#include "sram.h"
#include "malloc.h"
#include "SerialInvoker.h"

void bsp_init(void);

#endif /* __BSP_INIT_H */
