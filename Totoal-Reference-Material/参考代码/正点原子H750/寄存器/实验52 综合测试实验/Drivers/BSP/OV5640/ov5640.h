/**
 ****************************************************************************************************
 * @file        ov5640.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       OV5640 驱动代码
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
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef _OV5640_H
#define _OV5640_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/OV5640/sccb.h"

/******************************************************************************************/
/* PWDN和RESET 引脚 定义 */

#define OV_PWDN_GPIO_PORT               GPIOC
#define OV_PWDN_GPIO_PIN                SYS_GPIO_PIN4
#define OV_PWDN_GPIO_CLK_ENABLE()       do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* PC口时钟使能 */

#define OV_RESET_GPIO_PORT              GPIOA
#define OV_RESET_GPIO_PIN               SYS_GPIO_PIN7
#define OV_RESET_GPIO_CLK_ENABLE()      do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

/******************************************************************************************/

/* IO控制函数 */
#define OV5640_PWDN(x)      sys_gpio_pin_set(OV_PWDN_GPIO_PORT, OV_PWDN_GPIO_PIN, x)    /* POWER DOWN控制信号 */
#define OV5640_RST(x)       sys_gpio_pin_set(OV_RESET_GPIO_PORT, OV_RESET_GPIO_PIN, x)  /* 复位控制信号 */

/* OV5640的ID和访问地址 */
#define OV5640_ID           0X5640      /* OV5640的芯片ID */
#define OV5640_ADDR         0X78        /* OV5640的IIC地址 */
 
/* OV5640相关寄存器定义 */
#define OV5640_CHIPIDH      0X300A      /* OV5640芯片ID高字节 */
#define OV5640_CHIPIDL      0X300B      /* OV5640芯片ID低字节 */
 

/* 对外接口函数 */
uint8_t ov5640_read_reg(uint16_t reg);
uint8_t ov5640_write_reg(uint16_t reg,uint8_t data);

uint8_t ov5640_init(void);  
void ov5640_jpeg_mode(void);
void ov5640_rgb565_mode(void);

uint8_t ov5640_focus_init(void);
uint8_t ov5640_focus_single(void);
uint8_t ov5640_focus_constant(void);

void ov5640_flash_ctrl(uint8_t sw);
void ov5640_test_pattern(uint8_t mode);

void ov5640_sharpness(uint8_t sharp);
void ov5640_brightness(uint8_t bright);
void ov5640_contrast(uint8_t contrast);
void ov5640_exposure(uint8_t exposure);
void ov5640_light_mode(uint8_t mode);
void ov5640_special_effects(uint8_t eft);
void ov5640_color_saturation(uint8_t sat);

uint8_t ov5640_outsize_set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height);
uint8_t ov5640_image_window_set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height); 

#endif





















