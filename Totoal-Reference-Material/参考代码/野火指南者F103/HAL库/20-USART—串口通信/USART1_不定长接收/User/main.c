/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief  
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  HAL_Init();        
  /* 配置系统时钟为72 MHz */ 
  SystemClock_Config();
	   
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  
	/*调用printf函数，因为重定向了fputc，printf的内容会输出到串口*/
	printf("欢迎使用野火开发板\n");	

	/*自定义函数方式*/
	Usart_SendString( (uint8_t *)"串口接收不定长字节，请查看程序内注释\r\n" );
	
	/*开启第一次接收状态*/
	HAL_UART_Receive_IT(&UartHandle,(uint8_t*)RxBuff,BUFFSIZE);
	
  while(1)
	{			
		if(Rxflag == 1) 
		{			
			printf("总共接受到 %d 字节\r\n",Rxlen);	
				
			/*例程演示从简，只打印一份RxBuff内容*/
      Rxlen = Rxlen > BUFFSIZE ? BUFFSIZE : Rxlen;					
			HAL_UART_Transmit(&UartHandle,(uint8_t*)RxBuff,Rxlen,1000);
			memset(RxBuff,0,Rxlen);
			
			/*一轮接收数据以上的自定义处理完后重启接收*/
			HAL_UART_Receive_IT(&UartHandle,(uint8_t*)RxBuff,BUFFSIZE);					
			Rxflag = 0;
			Rxlen = 0;
		}	
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == DEBUG_USART)
	{		
		/*例程演示从简，当一轮接收超过了RxBuff，继续使用同一个RxBuff接收，接收字节数仍然累计，接收内容从头覆盖。
		根据实际的项目编写多份Rxbuff，类似循环队列切换到另一个Buff和及时处理已接收部分，*/
    Rxlen += UartHandle.RxXferSize;		
		HAL_UART_Receive_IT(&UartHandle,(uint8_t*)RxBuff,BUFFSIZE);		
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
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
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
