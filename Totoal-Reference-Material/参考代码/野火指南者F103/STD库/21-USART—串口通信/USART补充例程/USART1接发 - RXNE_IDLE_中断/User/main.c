/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   串口中断接收测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "test.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  USART_Config();
	LED_GPIO_Config();
	
	printf("欢迎使用野火STM32开发板\n\n\n\n");
	printf("使用空闲中断接收不定长字符串简易演示\n\n\n\n");
	
  while(1)
	{	
		
		if(test_data.flag == 1)
		{			
			do_process(&test_data);							
		}
		
	}	
}
/*********************************************END OF FILE**********************/
