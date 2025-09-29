/*
*********************************************************************************************************
*                                              uC/OS-II
*                                        The Real-Time Kernel
*
*                    Copyright 1992-2021 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Filename : os.h
* Version  : V2.93.01
*********************************************************************************************************
* Note : This file is included in the uC/OS-II for compatibility with uC/OS-III and should not be used
*        in normal circumstances.
*********************************************************************************************************
*/

#ifndef   OS_H
#define   OS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ucos_ii.h"
#include "os_cpu.h"
#include "os_cfg.h"

#include <stm32h7xx.h>


#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/LSENS/lsens.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./BSP/TIMER/timer.h"
#include "./BSP/DCMI/dcmi.h"
#include "./BSP/OV5640/ov5640.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/USART2/usart2.h"
#include "./BSP/SPBLCD/spblcd.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/NORFLASH/norflash_ex.h"

#include "./USMART/usmart.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./TEXT/fonts.h"
#include "./FATFS/source/ff.h"
#include "./PICTURE/piclib.h"

#include "string.h"
#include "math.h"
#include "gui.h"


extern volatile uint8_t system_task_return;  /* 任务强制返回标志 */


typedef  INT8U  OS_ERR;

#endif
