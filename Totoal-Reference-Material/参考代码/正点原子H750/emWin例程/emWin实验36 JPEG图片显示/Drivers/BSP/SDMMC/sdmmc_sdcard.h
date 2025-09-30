/**
 ****************************************************************************************************
 * @file        sdmmc_sdcard.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       SD卡 驱动代码
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

#ifndef __SDMMC_SDCARD_H
#define __SDMMC_SDCARD_H

#include "./SYSTEM/sys/sys.h"


/* 用户配置区
 * SDMMC时钟计算公式: SDMMC_CK时钟 = sdmmc_ker_ck / [2 * clkdiv]; 其中,sdmmc_ker_ck来自pll1_q_ck,为240Mhz
 * 注意：SDMMC_INIT_CLK_DIV 在HAL库有定义，我们只要把它的值改为300,即(0x12C),请到stm32h7xx_ll_sdmmc.h修改。
 */
//#define SDMMC_INIT_CLK_DIV        300       /* SDMMC初始化频率，240M/(300*2)=400Khz,最大400Kh */




/******************************************************************************************/
/* SDMMC1的信号线: SD1_D0 ~ SD1_D3/SD1_CLK/SD1_CMD 引脚 定义 
 * 如果你使用了其他引脚做SDMMC1的信号线,修改这里写定义即可适配.
 * 注意, 这里仅支持SDMMC1, 我们使用的也是SDMMC1. 不支持SDMMMC2.
 * 另外, 由于SDMMC1的IO口复用功能都是AF12, 这里就不独立再定义每个IO口的AF功能了
 */

#define SD1_D0_GPIO_PORT                GPIOC
#define SD1_D0_GPIO_PIN                 GPIO_PIN_8
#define SD1_D0_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

#define SD1_D1_GPIO_PORT                GPIOC
#define SD1_D1_GPIO_PIN                 GPIO_PIN_9
#define SD1_D1_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

#define SD1_D2_GPIO_PORT                GPIOC
#define SD1_D2_GPIO_PIN                 GPIO_PIN_10
#define SD1_D2_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

#define SD1_D3_GPIO_PORT                GPIOC
#define SD1_D3_GPIO_PIN                 GPIO_PIN_11
#define SD1_D3_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

#define SD1_CLK_GPIO_PORT               GPIOC
#define SD1_CLK_GPIO_PIN                GPIO_PIN_12
#define SD1_CLK_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

#define SD1_CMD_GPIO_PORT               GPIOD
#define SD1_CMD_GPIO_PIN                GPIO_PIN_2
#define SD1_CMD_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

/******************************************************************************************/

#define SD_TIMEOUT          ((uint32_t)100000000)       /* 超时时间 */
#define SD_TRANSFER_OK      ((uint8_t)0x00)             /* 传输完成 */
#define SD_TRANSFER_BUSY    ((uint8_t)0x01)             /* 卡正忙 */

extern SD_HandleTypeDef g_sd_handle;                    /* SD卡句柄 */
extern HAL_SD_CardInfoTypeDef g_sd_card_info_handle;    /* SD卡信息结构体 */

uint8_t sd_init(void);
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo);
uint8_t get_sd_card_state(void);
uint8_t sd_read_disk(uint8_t* buf, uint32_t sector, uint32_t cnt);
uint8_t sd_write_disk(uint8_t *buf, uint32_t sector, uint32_t cnt);

#endif





