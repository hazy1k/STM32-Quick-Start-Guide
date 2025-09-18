/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN Private defines */
#define PWM_CH1N_Pin GPIO_PIN_8
#define PWM_CH1N_GPIO_Port GPIOE
#define PWM_CH1_Pin GPIO_PIN_9
#define PWM_CH1_GPIO_Port GPIOE
#define PWM_BKIN_Pin GPIO_PIN_15
#define PWM_BKIN_GPIO_Port GPIOE

/* USER CODE END Private defines */
void MX_TIM1_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void tim1_set_deadtime(uint16_t ccr, uint8_t dtg);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

