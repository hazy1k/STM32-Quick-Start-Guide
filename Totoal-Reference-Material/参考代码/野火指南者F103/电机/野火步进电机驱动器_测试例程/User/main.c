/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   步进电机驱动例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 指南者 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_exti.h" 
#include "MicroStepDriver.h" 
#include "bsp_usart.h"
#include "SMD_test.h" 
 
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
    //初始化LED
    LED_GPIO_Config();
    //初始化按键
    EXTI_Key_Config();
    //初始化串口
    USART_Config();
	//步进电机初始化
	SMD_Init();
    //打印帮助命令
    ShowHelp();
    
    while(1) 
    {
        //处理串口数据
        DealSerialData();
    }
                          
}
/*********************************************END OF FILE**********************/

