/**
 ****************************************************************************************************
 * @file        lv_action.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       LVGL GIF
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "LVGL/GUI_APP/lv_action.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/DCMI/dcmi.h"
#include "./BSP/OV5640/ov5640.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
/*FreeRTOS**********************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"


TimerHandle_t   AutoReloadTimer_Handle;        /* 周期定时器句柄 */
void AutoReloadCallback(TimerHandle_t xTimer);  /* 周期定时器回调函数 */


lv_obj_t * lv_ation_win;

/**
 * @brief       切换为OV5640模式
 *   @note      切换PC8/PC9/PC11为DCMI复用功能(AF13)
 * @param       无
 * @retval      无
 */
void sw_ov5640_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ov5640_write_reg(0X3017, 0XFF); /* 开启OV5650输出(可以正常显示) */
    ov5640_write_reg(0X3018, 0XFF);
    
    /* GPIOC8/9/11切换为 DCMI接口 */
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF13_DCMI;        /* 复用为DCMI */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* 初始化PC8，9, 11引脚 */
}

/**
 * @brief       切换为SD卡模式
 *   @note      切换PC8/PC9/PC11为SDMMC复用功能(AF12)
 * @param       无
 * @retval      无
 */
void sw_sdcard_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ov5640_write_reg(0X3017, 0X00); /* 关闭OV5640全部输出(不影响SD卡通信) */
    ov5640_write_reg(0X3018, 0X00);
    
    /* GPIOC8/9/11切换为 SDIO接口 */
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF12_SDIO1;       /* 复用为SDIO */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* 初始化PC8，9, 11引脚 */
}

/**
 * @brief       RGB565测试
 *   @ntoe      RGB数据直接显示在LCD上面
 *
 * @param       无
 * @retval      无
 */
void rgb565_test(void)
{
    /* 自动对焦初始化 */
    ov5640_rgb565_mode();                   /* RGB565模式 */
    ov5640_focus_init();
    ov5640_brightness(0);                   /* 自动模式 */
    ov5640_color_saturation(3);             /* 色彩饱和度0 */
    ov5640_brightness(4);                   /* 亮度0 */
    ov5640_contrast(3);                     /* 对比度0 */
    ov5640_sharpness(33);                   /* 自动锐度 */
    ov5640_focus_constant();                /* 启动持续对焦 */
    dcmi_init();                            /* DCMI配置 */
    dcmi_dma_init((uint32_t)&LCD->LCD_RAM, 0, 1, DMA_MDATAALIGN_HALFWORD, DMA_MINC_DISABLE); /* DCMI DMA配置,MCU屏,竖屏 */

    dcmi_set_window(0,myFont24.line_height, lcddev.width, lcddev.height - myFont24.line_height);
    
    while(1)
    {
        vTaskDelay(100);
    }

}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_rgb565_test(void)
{
    if (ov5640_init())   /* 初始化OV5640 */
    {
        

    }
    else
    {
        sw_ov5640_mode();
        lcd_display_dir(0);                             /* 默认为竖屏 */
        rgb565_test();   
    }
}

