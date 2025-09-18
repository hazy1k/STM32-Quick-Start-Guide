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
#define TIM_LED_Pin GPIO_PIN_4
#define TIM_LED_GPIO_Port GPIOB
#define TIM5_IN_Pin GPIO_PIN_0
#define TIM5_IN_GPIO_Port GPIOA
#define TIM2_KEY_Pin GPIO_PIN_0
#define TIM2_KEY_GPIO_Port GPIOA
#define TIM15_LED_Pin GPIO_PIN_5
#define TIM15_LED_GPIO_Port GPIOE
/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim5;

extern TIM_HandleTypeDef htim6;

extern TIM_HandleTypeDef htim15;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM5_Init(void);
void MX_TIM6_Init(void);
void MX_TIM15_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void tim15_set_pwm(uint32_t num);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

