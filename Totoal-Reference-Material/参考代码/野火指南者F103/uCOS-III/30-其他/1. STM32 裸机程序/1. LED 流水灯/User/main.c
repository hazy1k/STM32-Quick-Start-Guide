/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_led.h"



static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{	
	LED_Init ();	          //初始化 LED
 

	while ( 1 )
	{
		macLED1_ON ();			  // 亮
		Delay ( 0x0FFFFF );
		macLED1_OFF ();		  // 灭

		macLED2_ON ();			  // 亮
		Delay ( 0x0FFFFF );
		macLED2_OFF ();		  // 灭

		macLED3_ON ();			  // 亮
		Delay ( 0x0FFFFF );
		macLED3_OFF ();		  // 灭	   
		
	}

}

/*********************************************END OF FILE**********************/
