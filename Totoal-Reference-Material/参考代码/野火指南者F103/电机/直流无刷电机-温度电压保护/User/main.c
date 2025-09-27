/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   无刷电机-按键控制
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
#include <stdlib.h>
#include ".\motor_control\bsp_motor_control.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"

#define TEMP_MAX    80    // 温度最大值
#define TEMP_MIN    10    // 温度最小值
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
  __IO uint16_t ChannelPulse = PWM_MAX_PERIOD_COUNT/10;
  uint8_t flag = 0;
  uint8_t t_max_count = 0;
	
  /* HAL 初始化 */
  HAL_Init();
	
	/* 初始化系统时钟为72MHz */
	SystemClock_Config();

	/* 开启复用寄存器时钟 */
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	
	/* 初始化按键GPIO */
	Key_GPIO_Config();
  
  /* LED 灯初始化 */
  LED_GPIO_Config();
  
  /* 调试串口初始化 */
  DEBUG_USART_Config();

  /* ADC 初始化 */
  ADC_Init();
	
  printf("野火直流无刷电机按键控制例程\r\n");

  /* 电机初始化 */
  bldcm_init();
	
	while(1)
	{
    /* 扫描KEY1 */
    if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      /* 使能电机 */
      set_bldcm_speed(ChannelPulse);
      set_bldcm_enable();
			while(1)
			{
				/* 扫描KEY1 */
				if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
				{
					/* 增大占空比 */
					ChannelPulse += PWM_MAX_PERIOD_COUNT/10;
					
					if(ChannelPulse > PWM_MAX_PERIOD_COUNT)
						ChannelPulse = PWM_MAX_PERIOD_COUNT;
					
					set_bldcm_speed(ChannelPulse);
				}
				
				/* 扫描KEY2 */
				if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
				{
					if(ChannelPulse < PWM_MAX_PERIOD_COUNT/10)
						ChannelPulse = 0;
					else
						ChannelPulse -= PWM_MAX_PERIOD_COUNT/10;

					set_bldcm_speed(ChannelPulse);
				}
				
				if (HAL_GetTick()%50 == 0 && flag == 0)    // 每50毫秒读取一次温度、电压
				{
					flag = 1;
					float temp = 0;
					temp = get_ntc_t_val();

					printf("电源电压=%0.1fV, NTC=%0.0fΩ, T=%0.1f℃.\r\n", 
								 get_vbus_val(), get_ntc_r_val(), temp);
					
					if (temp < TEMP_MIN || temp > TEMP_MAX)    // 判断是不是超过限定的值
					{
						if (t_max_count++ > 5)    // 连续5次超过
						{
							LED2_ON;
							set_bldcm_disable();
							t_max_count = 0;
							printf("温度超过限制！请检查原因，复位开发板在试！\r\n");
							while(1);
						}
					}
				}
				else if (HAL_GetTick()%50 != 0 && flag == 1)
				{
					flag = 0;
				}
			}
    }
    
    /* 扫描KEY2 */
    if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      /* 停止电机 */
      set_bldcm_disable();
    }

    if (HAL_GetTick()%50 == 0 && flag == 0)    // 每50毫秒读取一次温度、电压
    {
      flag = 1;
      float temp = 0;
      temp = get_ntc_t_val();

      printf("电源电压=%0.1fV, NTC=%0.0fΩ, T=%0.1f℃.\r\n", 
             get_vbus_val(), get_ntc_r_val(), temp);
      
      if (temp < TEMP_MIN || temp > TEMP_MAX)    // 判断是不是超过限定的值
      {
        if (t_max_count++ > 5)    // 连续5次超过
        {
          LED2_ON;
          set_bldcm_disable();
          t_max_count = 0;
          printf("温度超过限制！请检查原因，复位开发板在试！\r\n");
          while(1);
        }
      }
    }
    else if (HAL_GetTick()%50 != 0 && flag == 1)
    {
      flag = 0;
    }
//    /* 扫描KEY5 */
//    if( Key_Scan(KEY5_GPIO_PORT, KEY5_PIN) == KEY_ON)
//    {
//      /* 转换方向 */
//      set_bldcm_direction( (++i % 2) ? MOTOR_FWD : MOTOR_REV);
//    }
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
