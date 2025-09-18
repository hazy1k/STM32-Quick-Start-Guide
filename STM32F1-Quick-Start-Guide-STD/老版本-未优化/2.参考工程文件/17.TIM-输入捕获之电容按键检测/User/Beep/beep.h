#ifndef __BEEP_H
#define __BEEP_H
#include "stm32f10x.h"

// 使用宏定义，方便迁移和代码简洁可读
#define BEEP_GPIO GPIOA // 定义蜂鸣器使用的GPIO
#define BEEP_GPIO_CLK RCC_APB2Periph_GPIOA // 设置GPIO时钟
#define BEEP_GPIO_PIN GPIO_Pin_8 // define the pin for the BEEP
#define BEEP_GPIO_Mode GPIO_Mode_Out_PP // 设置为推挽输出模式
#define BEEP_GPIO_Speed GPIO_Speed_50MHz // 设置GPIO速度为50MHz

// 蜂鸣器高电平时工作
#define ON 1 // 代表开启蜂鸣器
#define OFF 0 // to turn off the beep

// 控制蜂鸣器引脚宏
#define BEEP(a) do { \
                    if(a) GPIO_SetBits(BEEP_GPIO, BEEP_GPIO_PIN);  /* 设置高电平 */ \
                    else GPIO_ResetBits(BEEP_GPIO, BEEP_GPIO_PIN); /* 设置低电平 */ \
                 } while(0)

void BEEP_GPIO_Config(void); 

#endif
