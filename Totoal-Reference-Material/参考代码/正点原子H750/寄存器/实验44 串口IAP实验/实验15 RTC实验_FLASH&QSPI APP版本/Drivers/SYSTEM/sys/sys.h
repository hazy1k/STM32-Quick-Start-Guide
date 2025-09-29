/**
 ****************************************************************************************************
 * @file        sys.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-03-12
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200312
 * 第一次发布
 * V1.1 20200401
 * 1, 将头文件包含路径改成相对路径,避免重复设置包含路径的麻烦
 * 2, 将函数改成带一个参数,用于支持W25Q256, 实现了支持所有25Q系列芯片
 * 3, 将sys_nvic_set_vector_table函数改成普通函数, 方便外部调用
 ****************************************************************************************************
 */


#ifndef __SYS_H
#define __SYS_H

#include "stm32h7xx.h"



/**
 * SYS_SUPPORT_OS用于定义系统文件夹是否支持OS
 * 0,不支持OS
 * 1,支持OS
 */
#define SYS_SUPPORT_OS          0


/* sys_nvic_ex_config专用宏定义 */
#define SYS_GPIO_FTIR           1       /* 下降沿触发 */
#define SYS_GPIO_RTIR           2       /* 上升沿触发 */
#define SYS_GPIO_BTIR           3       /* 任意边沿触发 */

/* GPIO设置专用宏定义 */
#define SYS_GPIO_MODE_IN        0       /* 普通输入模式 */
#define SYS_GPIO_MODE_OUT       1       /* 普通输出模式 */
#define SYS_GPIO_MODE_AF        2       /* AF功能模式 */
#define SYS_GPIO_MODE_AIN       3       /* 模拟输入模式 */

#define SYS_GPIO_SPEED_LOW      0       /* GPIO速度(低速,12M) */
#define SYS_GPIO_SPEED_MID      1       /* GPIO速度(中速,60M) */
#define SYS_GPIO_SPEED_FAST     2       /* GPIO速度(快速,85M) */
#define SYS_GPIO_SPEED_HIGH     3       /* GPIO速度(高速,100M) */

#define SYS_GPIO_PUPD_NONE      0       /* 不带上下拉 */
#define SYS_GPIO_PUPD_PU        1       /* 上拉 */
#define SYS_GPIO_PUPD_PD        2       /* 下拉 */
#define SYS_GPIO_PUPD_RES       3       /* 保留 */

#define SYS_GPIO_OTYPE_PP       0       /* 推挽输出 */
#define SYS_GPIO_OTYPE_OD       1       /* 开漏输出 */

/* GPIO引脚位置宏定义  */
#define SYS_GPIO_PIN0           1<<0
#define SYS_GPIO_PIN1           1<<1
#define SYS_GPIO_PIN2           1<<2
#define SYS_GPIO_PIN3           1<<3
#define SYS_GPIO_PIN4           1<<4
#define SYS_GPIO_PIN5           1<<5
#define SYS_GPIO_PIN6           1<<6
#define SYS_GPIO_PIN7           1<<7
#define SYS_GPIO_PIN8           1<<8
#define SYS_GPIO_PIN9           1<<9
#define SYS_GPIO_PIN10          1<<10
#define SYS_GPIO_PIN11          1<<11
#define SYS_GPIO_PIN12          1<<12
#define SYS_GPIO_PIN13          1<<13
#define SYS_GPIO_PIN14          1<<14
#define SYS_GPIO_PIN15          1<<15


/*函数申明*******************************************************************************************/
/* 静态函数(仅在sys.c里面用到) */
static void sys_nvic_priority_group_config(uint8_t group);                      /* 设置NVIC分组 */


/* 普通函数 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);             /* 设置中断偏移量 */
void sys_nvic_init(uint8_t pprio, uint8_t sprio, uint8_t ch, uint8_t group);    /* 设置NVIC */
void sys_nvic_ex_config(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t tmode);   /* 外部中断配置函数,只针对GPIOA~GPIOK */
void sys_gpio_af_set(GPIO_TypeDef *gpiox, uint16_t pinx, uint8_t afx);          /* GPIO复用功能选择设置  */
void sys_gpio_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint32_t mode, 
                  uint32_t otype, uint32_t ospeed, uint32_t pupd);              /*  GPIO通用设置 */
void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status);    /* 设置GPIO某个引脚的输出状态 */
uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx);                 /* 读取GPIO某个引脚的状态 */
void sys_standby(void);         /* 进入待机模式 */
void sys_soft_reset(void);      /* 系统软复位 */
void sys_cache_enable(void);    /* 使能 Cache */
uint8_t sys_clock_set(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);      /* 时钟设置函数 */
void sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);  /* 系统时钟初始化函数 */
void sys_qspi_enable_memmapmode(uint8_t ftype); /* QSPI进入内存映射模式 */

/* 以下为汇编函数 */
void sys_wfi_set(void);             /* 执行WFI指令 */
void sys_intx_disable(void);        /* 关闭所有中断 */
void sys_intx_enable(void);         /* 开启所有中断 */
void sys_msr_msp(uint32_t addr);    /* 设置栈顶地址 */

#endif











