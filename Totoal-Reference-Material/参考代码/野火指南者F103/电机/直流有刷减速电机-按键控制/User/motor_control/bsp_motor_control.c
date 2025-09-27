/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   电机控制接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include ".\motor_control\bsp_motor_control.h"
#include "./usart/bsp_debug_usart.h"
#include <math.h>
#include <stdlib.h>

static motor_dir_t direction  = MOTOR_FWD;     // 记录方向
static uint16_t    dutyfactor = 0;             // 记录占空比

static void sd_gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 定时器通道功能引脚端口时钟使能 */
	SHUTDOWN_GPIO_CLK_ENABLE();
  
  /* 引脚IO初始化 */
	/*设置输出类型*/
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	/*设置引脚速率 */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*选择要控制的GPIO引脚*/	
	GPIO_InitStruct.Pin = SHUTDOWN_PIN;
  
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
  HAL_GPIO_Init(SHUTDOWN_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  电机初始化
  * @param  无
  * @retval 无
  */
void motor_init(void)
{
  TIMx_Configuration();     // 初始化电机 1
  sd_gpio_config();
}

/**
  * @brief  设置电机速度
  * @param  v: 速度（占空比）
  * @retval 无
  */
void set_motor_speed(uint16_t v)
{
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  设置电机方向
  * @param  无
  * @retval 无
  */
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;
  
  SET_FWD_COMPAER(0);     // 设置速度为 0
  SET_REV_COMPAER(0);     // 设置速度为 0
  
  HAL_Delay(200);         // 延时一会
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
    SET_REV_COMPAER(0);              // 设置速度
  }
  else
  {
    SET_FWD_COMPAER(0);              // 设置速度
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  使能电机
  * @param  无
  * @retval 无
  */
void set_motor_enable(void)
{
  MOTOR_ENABLE_SD();
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

/**
  * @brief  禁用电机
  * @param  无
  * @retval 无
  */
void set_motor_disable(void)
{
  MOTOR_DISABLE_SD();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

/**
  * @brief  打印帮助命令
  * @param  无
  * @retval 无
  */
void show_help(void)
{
    printf("——————————————野火直流减速电机驱动演示程序——————————————\n\r");
    printf("输入命令(以回车结束)：\n\r");
    printf("< ? >       -帮助菜单\n\r");
    printf("v [data]     -设置电机的速度（范围：0—%d）\n\r", PWM_MAX_PERIOD_COUNT);
    printf("d [data]     -设置电机的方向，%d:正向转，%d:反向转\n\r", MOTOR_FWD, MOTOR_REV);
		printf("命令示例:d 0,注意d与0间含一个空格,发送时末尾需有换行符或在串口助手中勾选发送新行\n\r");
}
#if 0
/**
  * @brief  处理串口接收到的数据
  * @param  无
  * @retval 无
  */
void deal_serial_data(void)
{
    static char showflag =1;
    int dec_temp=0;
    int speed_temp=0;
    
    //接收到正确的指令才为1
    char okCmd = 0;
  
    if (showflag)
    {
      show_help();
      showflag = !showflag;
    }

    //检查是否接收到指令
    if(receive_cmd == 1)
    {
      if(UART_RxBuffer[0] == 'v' || UART_RxBuffer[0] == 'V')
      {
        //设置速度
        if(UART_RxBuffer[1] == ' ')
        {
          speed_temp = atoi((char const *)UART_RxBuffer+2);
          if(speed_temp>=0 && speed_temp <= PWM_MAX_PERIOD_COUNT)
          {
            set_motor_speed(speed_temp);
            printf("\n\r速度: %d\n\r", speed_temp);
            okCmd = 1;
          }
        }
      }
      else if(UART_RxBuffer[0] == 'd')
      {
        //设置方向
        if(UART_RxBuffer[1] == ' ')
        {
          dec_temp = atoi((char const *)UART_RxBuffer+2);

          if(dec_temp>=0)
          {
            set_motor_direction(dec_temp);
            printf("\n\r方向:%s\n\r", dec_temp ? "反向转" : "正向转");
            okCmd = 1;
          }
        }
      }
      else if(UART_RxBuffer[0] == '?')
      {
        //打印帮助命令
        show_help();
        okCmd = 1;
      }
      //如果指令有无则打印帮助命令
      if(okCmd != 1)
      {
        printf("\n\r 输入有误，请重新输入...\n\r");
        show_help();
      }

      //清空串口接收缓冲数组
      receive_cmd = 0;
      uart_FlushRxBuffer();

    }
}
#endif
/*********************************************END OF FILE**********************/

