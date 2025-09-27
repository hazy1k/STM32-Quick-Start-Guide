/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   直线圆弧插补
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./stepper/bsp_stepper_init.h"
#include "./key/bsp_key.h"
#include "./led/bsp_led.h"
#include "./stepper/bsp_circular_interpolation.h"
#include "./stepper/bsp_linear_interpolation.h"

/* 进给速度 */
#define INTERPOLATION_SPEED  1000

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
  HAL_InitTick(0);
	/* 初始化系统时钟为72MHz */
	SystemClock_Config();
	/*初始化USART 配置模式为 115200 8-N-1，中断接收*/
	DEBUG_USART_Config();
	printf("欢迎使用野火 电机开发板 步进电机 任意象限圆弧插补 例程\r\n");
  /* LED初始化 */
  LED_GPIO_Config();
  /* 按键初始化 */
  Key_GPIO_Config();
  /*步进电机初始化*/
	stepper_Init();
	MOTOR_EN(ON);
	while(1)
	{
    /* 圆弧插补 */
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      Circular_InterPolation(6400 * 10, 0, 0, -6400 * 10, INTERPOLATION_SPEED, CW);
      while(circular_para.motionstatus);
      Circular_InterPolation(0, -6400 * 10, -6400 * 10, 0, INTERPOLATION_SPEED, CW);
      while(circular_para.motionstatus);
      Circular_InterPolation(-6400 * 10, 0, 0, 6400 * 10, INTERPOLATION_SPEED, CW);
      while(circular_para.motionstatus);
      Circular_InterPolation(0, 6400 * 10, 6400 * 10, 0, INTERPOLATION_SPEED, CW);
    }
    
    /* 直线插补，最终得到的图形是圆内接两个正三角形 坐标参数是手动计算的 */
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      Linear_Interpolation(-6400 * 10, 6400 * 10, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);

      HAL_Delay(500);

      Linear_Interpolation(-0.86603 * 6400 * 10, -1.5 * 6400 * 10, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);
      Linear_Interpolation(1.73206 * 6400 * 10, 0, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);
      Linear_Interpolation(-0.86603 * 6400 * 10, 1.5 * 6400 * 10, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);

      HAL_Delay(50);

      Linear_Interpolation(0, -6400 * 20, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);

      HAL_Delay(500);

      Linear_Interpolation(0.86603 * 6400 * 10, 1.73206 * 6400 * 10, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);
      Linear_Interpolation(-1.73206 * 6400 * 10, 0, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);
      Linear_Interpolation(0.86603 * 6400 * 10, -1.5 * 6400 * 10, INTERPOLATION_SPEED);
      while(linear_para.motionstatus);
		}
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
