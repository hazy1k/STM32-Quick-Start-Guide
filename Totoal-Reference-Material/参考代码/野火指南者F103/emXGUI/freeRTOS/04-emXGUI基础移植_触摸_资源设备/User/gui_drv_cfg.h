/**
  *********************************************************************
  * @file    gui_drv_cfg.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI驱动配置文件
  *          包含显示器、输入设备、存储器、字体、资源设备等内容
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#ifndef	__GUI_DRV_CFG_H__
#define	__GUI_DRV_CFG_H__

#include "board.h"

/*===========显示器配置===gui_lcd_port.c===============================================*/
//野火5.0 / 7.0TFT,800x480 ,4.3寸480*272

/* 显存基地址 */
#define  LCD_FRAME_BUFFER   LCD_BUFFER

/* 使用的显示格式 */
#define	LCD_FORMAT	  COLOR_FORMAT_RGB565
//#define	LCD_FORMAT	  COLOR_FORMAT_XRGB8888

/* 当前使用液晶的宽高 */
#define	LCD_XSIZE	    320
#define	LCD_YSIZE	    240

/* 使用最大液晶屏的宽高，用于计算显存空间 */
#define	LCD_MAX_XSIZE	    320//LCD_MAX_PIXEL_WIDTH
#define	LCD_MAX_YSIZE	    240//LCD_MAX_PIXEL_HEIGHT

/* 使用的LCD种类数，方便支持不同的分辨率，使用不同的字体 */
#define  GUI_LCD_TYPE_NUM     LCD_TYPE_NUM

//屏幕旋转，默认
//#define	LCD_ROTATE      ROTATE_0

/* 是否使用硬件图形加速器（DMA2D之类，驱动需要重新定制） */
#define	DMA2D_EN                   0

/* 是否使用缓冲型控件，使用的话会增加一层液晶像素数据的消耗，显示效果减少闪烁*/
#define  FRAME_BUFFER_EN            0

/*==========输入设备配置===gui_input_port.c==================================================*/
/* 是否使用输入设备 */
#define  GUI_INPUT_DEV_EN        1

/* 是否使能键盘、鼠标、触摸屏 */
#define	GUI_TOUCHSCREEN_EN      1
#define	GUI_KEYBOARD_EN          0
#define	GUI_MOUSE_EN             0

/* 是否需要触摸校准-电阻屏才需要 */
#define	GUI_TOUCHSCREEN_CALIBRATE       1

/* 是否需要触摸校准-电阻屏才需要 */
#define	GUI_TOUCH_CALIBRATEParamAddr       0x0

/* 是否显示光标 */
#define  GUI_SHOW_CURSOR_EN        0

/*===========存储器配置===gui_mem_port.c===============================================*/
/* 通常GUI内核使用MEM内存堆，VMEM内存堆给用户使用 */

/* GUI内核使用的存储区的基地址 */  
//#define	GUI_CORE_MEM_BASE	  0xD0100000  //本例子使用RTT管理，使用内部sram，不指定地址

/* GUI内核使用的存储区大小，推荐最小值为8KB */
#define  GUI_CORE_MEM_SIZE  (32*1024) //本例子使用系统管理，在rtt系统在board.c实现，freertos在heap_4.c实现
/* 最小分配粒度，单位为字节*/  
#define	GUI_CORE_MEM_ALLOC_UNIT   (64)         


/* 配置vmem的基地址，大小以及分配粒度 */
/* 是否使能VMEM内存堆 */
#define  GUI_VMEM_EN      0

/* 液晶驱动显存使用的空间 */
#if (LCD_FORMAT == COLOR_FORMAT_RGB565)

  #define LCD_FRAME_SIZE  (LCD_MAX_XSIZE*LCD_MAX_YSIZE*2)

#elif (LCD_FORMAT == COLOR_FORMAT_XRGB8888)

  #define LCD_FRAME_SIZE (LCD_MAX_XSIZE*LCD_MAX_YSIZE*4)

#endif
/*
 * SDRAM 基地址：0xD0000000
 * 前 LCD_FRAME_SIZE 大小的空间作为液晶驱动显存
 * 剩余的空间作为VMEM动态分配使用
*/
/* 内存堆的基地址，可以为内部SRAM、外扩的SDRAM等 */  
#define	VMEM_BASE	        (EXT_SRAM_BASE)
/* 内存堆的总大小，单位为字节 */ 
#define	VMEM_SIZE	        ((600*1024))
/* 最小分配粒度，单位为字节*/  
#define	VMEM_ALLOC_UNIT   (64)         //64字节   

/*===========字体配置===gui_font_port.c===============================================*/
/* 默认字体使用XFT，保留，这个宏不起作用 */
#define	GUI_FONT_XFT_EN		      1
/* 是否支持TTF字体，保留，这个宏不起作用 */
#define	GUI_FONT_TTF_EN		      0


/* 默认内部英文字体数组名，USE_EXTERN_FONT为0或 外部字体加载失败时会采用的字体 */
#define GUI_DEFAULT_FONT_EN         ASCII_16_4BPP     // gui_font_param[cur_lcd].default_en

/* 是否使用内部中文字体，中文字体太大，不建议放在内部 */
#define GUI_INER_CN_FONT_EN      0
/* 默认内部中文字体数组名 */
#define GUI_DEFAULT_FONT_CN          GB2312_16_2BPP


/* 是否使用外部FLASH中的字体
*  流设备和整体加载方式都要把这个宏设置为1
*  使用本功能时需要把GUI_RES_DEV_EN设置为1
*/
#define GUI_EXTERN_FONT_EN       0

/*
* 是否把整个外部字体数据加载至VMEM区域，初始化加载时需要较长时间，
* 加载后可大幅提高字符显示的速度
* 若设置为真，则使用整体加载方式，否则使用流设备方式
*/
#define GUI_FONT_LOAD_TO_RAM_EN    (1 && GUI_EXTERN_FONT_EN)

/* 要使用的外部默认字体文件，USE_EXTERN_FONT为1时生效 */
#define GUI_DEFAULT_EXTERN_FONT   "GB2312_16_2BPP.xft"    // gui_font_param[cur_lcd].default_extern_cn


/*===========日志输出设备配置===gui_log_port.c===============================================*/

/* 是否开启调试输出、数组输出、进入函数的输出功能 */
#define GUI_DEBUG_EN         	      1
#define GUI_DEBUG_ARRAY_EN          0
#define GUI_DEBUG_FUNC_EN   	       0



/*===========资源设备配置===gui_resource_port.c===============================================*/
/* 是否使用资源设备 */
#define GUI_RES_DEV_EN         1

/* 是否支持文件系统接口,需要移植fatfs文件系统 */
#define GUI_FS_EN         0

/* 资源所在的基地址 */
#define GUI_RES_BASE             4096

/* 存储在FLASH中的资源目录大小 */
#define GUI_CATALOG_SIZE         (8*1024)

/*===========图片接口配置===gui_picture_port.c===============================================*/
/* 是否支持文件系统图片接口,需要移植fatfs文件系统 */
#define GUI_PIC_FS_EN         0

/* 是否支持显示JPEG图片,需要添加jpeg解码库 */
#define GUI_PIC_JPEG_EN       0

/* 是否支持显示JPEG图片,需要添加png解码库 */
#define GUI_PIC_PNG_EN       0

/* 截图 */
#define GUI_PIC_CAPTURE_SCREEN_EN  ( 1 && GUI_PIC_FS_EN)

/*===========是否启用各种APP===============================================*/

/* 启动界面 */
#define GUI_APP_BOOT_INTERFACE_EN     0

/* 资源烧录器，使能后缺少资源会自动进入资源烧录界面 */
/* 需要sd文件系统/flash资源设备支持 */
#define GUI_APP_RES_WRITER_EN          0
/*============================================================================*/

/*============================================================================*/
//#define	DMA2D_EN                          0
#define	GUI_KEYBOARD_EN                 0
#define	GUI_MOUSE_EN                    0
//#define	GUI_TOUCHSCREEN_EN              1

//#define	GUI_TOUCHSCREEN_CALIBRATE       1

#define	FONT_XFT_EN		1
#define	FONT_TTF_EN		0

#define	FRAMEBUFFER_EN	0


/*============================================================================*/

#ifndef	MIN
#define	MIN(a,b)	(((a)<(b))?(a):(b))
#endif

#ifndef	MAX
#define	MAX(a,b)	(((a)>(b))?(a):(b))
#endif


/*============================================================================*/

#define	LCD_FORMAT	COLOR_FORMAT_RGB565
//#define	LCD_FORMAT	COLOR_FORMAT_XRGB8888

//#define	LCD_XSIZE	    240
//#define	LCD_YSIZE	    320
#define LCD_FRAME_RATE	60
#define	LCD_ROTATE      ROTATE_0

/*============================================================================*/
#if	(LCD_FORMAT==COLOR_FORMAT_RGB565)

#define	LCD_COLOR	    U16
#define	LCD_BUF_SIZE	(LCD_XSIZE*LCD_YSIZE*2)

#endif

////////

#if	(LCD_FORMAT==COLOR_FORMAT_XRGB8888)

#define	LCD_COLOR       U32
#define	LCD_BUF_SIZE	(LCD_XSIZE*LCD_YSIZE*4)

#endif

#endif	/*__GUI_DRV_CFG_H__*/
