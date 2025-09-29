/**
 ****************************************************************************************************
 * @file        common.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
 * @date        2022-05-26
 * @brief       APP通用 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.3 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __COMMON_H
#define __COMMON_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/TOUCH/touch.h"
#include "os.h"
#include "gui.h"


/* 硬件平台软硬件版本定义 */
#define HARDWARE_VERSION            15          /* 硬件版本,放大10倍,如1.0表示为10 */
#define SOFTWARE_VERSION            200         /* 软件版本,放大100倍,如1.00,表示为100 */

/* 系统数据保存基址 */
#define SYSTEM_PARA_SAVE_BASE       100         /* 系统信息保存首地址.从100开始 */


/* 各图标/图片路径 */
extern uint8_t*const APP_OK_PIC;                /* 确认图标 */
extern uint8_t*const APP_CANCEL_PIC;            /* 取消图标 */
extern uint8_t*const APP_UNSELECT_PIC;          /* 未选中图标 */
extern uint8_t*const APP_SELECT_PIC;            /* 选中图标 */
extern uint8_t*const APP_VOL_PIC;               /* 音量图片路径 */

extern uint8_t*const APP_ASCII_S14472;          /* 数码管字体144*72大数字字体路径 */
extern uint8_t*const APP_ASCII_S8844;           /* 数码管字体88*44大数字字体路径 */
extern uint8_t*const APP_ASCII_S7236;           /* 数码管字体72*36大数字字体路径 */
extern uint8_t*const APP_ASCII_S6030;           /* 数码管大字体路径 */
extern uint8_t*const APP_ASCII_5427;            /* 普通大字体路径 */
extern uint8_t*const APP_ASCII_3618;            /* 普通大字体路径 */
extern uint8_t*const APP_ASCII_2814;            /* 普通大字体路径 */

extern uint8_t* asc2_14472;                     /* 普通144*72大字体点阵集 */
extern uint8_t* asc2_8844;                      /* 普通字体88*44大字体点阵集 */
extern uint8_t* asc2_7236;                      /* 普通字体72*36大字体点阵集 */
extern uint8_t* asc2_s6030;                     /* 数码管字体60*30大字体点阵集 */
extern uint8_t* asc2_5427;                      /* 普通字体54*27大字体点阵集 */
extern uint8_t* asc2_3618;                      /* 普通字体36*18大字体点阵集 */
extern uint8_t* asc2_2814;                      /* 普通字体28*14大字体点阵集 */

extern const uint8_t APP_ALIENTEK_ICO2424[];    /* 启动界面图标,存放在flash */
extern const uint8_t APP_ALIENTEK_ICO3232[];    /* 启动界面图标,存放在flash */
extern const uint8_t APP_ALIENTEK_ICO4848[];    /* 启动界面图标,存放在flash */


/* APP的总功能数目 */
#define APP_FUNS_NUM                23


/* app主要功能界面标题 */
extern uint8_t*const APP_MFUNS_CAPTION_TBL[APP_FUNS_NUM][GUI_LANGUAGE_NUM];
extern uint8_t*const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM];

extern uint8_t*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];

/* 平滑线的起止颜色定义 */
#define WIN_SMOOTH_LINE_SEC         0XB1FFC4    /* 起止颜色 */
#define WIN_SMOOTH_LINE_MC          0X1600B1    /* 中间颜色 */

/* 弹出窗口选择条目的设置信息 */
#define APP_ITEM_BTN1_WIDTH         60          /* 有2个按键时的宽度 */
#define APP_ITEM_BTN2_WIDTH         100         /* 只有1个按键时的宽度 */
#define APP_ITEM_BTN_HEIGHT         30          /* 按键高度 */
#define APP_ITEM_ICO_SIZE           32          /* ICO图标的尺寸 */

#define APP_ITEM_SEL_BKCOLOR        0X0EC3      /* 选择时的背景色 */
#define APP_WIN_BACK_COLOR          0XC618      /* 窗体背景色 */


//#define APP_FB_TOPBAR_HEIGHT        20          /* 文件浏览界面,顶部横条的高度 */
//#define APP_FB_BTMBAR_HEIGHT        20          /* 文件浏览界面/测试界面,底部横条的高度 */
//#define APP_TEST_TOPBAR_HEIGHT      20          /* 测试界面,顶部横条高度 */

/* π值定义 */
#define app_pi      3.14159


uint32_t app_get_rand(uint32_t max);
void app_srand(uint32_t seed);
void app_set_lcdsize(uint8_t mode);
void app_read_bkcolor(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *ctbl);
void app_recover_bkcolor(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *ctbl);
void app_gui_tcbar(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t mode);
uint8_t app_get_numlen(long long num,uint8_t dir);
void app_show_float(uint16_t x,uint16_t y,long long num,uint8_t flen,uint8_t clen,uint8_t font,uint16_t color,uint16_t bkcolor);
void app_filebrower(uint8_t *topname,uint8_t mode);
void app_show_nummid(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint32_t num,uint8_t len,uint8_t size,uint16_t ptcolor,uint16_t bkcolor);
void app_draw_smooth_line(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint32_t sergb,uint32_t mrgb);

uint8_t app_tp_is_in_area(_m_tp_dev *tp,uint16_t x,uint16_t y,uint16_t width,uint16_t height);
void app_show_items(uint16_t x,uint16_t y,uint16_t itemwidth,uint16_t itemheight,uint8_t*name,uint8_t*icopath,uint16_t color,uint16_t bkcolor);
uint8_t * app_get_icopath(uint8_t mode,uint8_t *selpath,uint8_t *unselpath,uint8_t selx,uint8_t index);
uint8_t app_items_sel(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *items[],uint8_t itemsize,uint8_t *selx,uint8_t mode,uint8_t*caption);
uint8_t app_listbox_select(uint8_t *sel,uint8_t *top,uint8_t * caption,uint8_t *items[],uint8_t itemsize);
void app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor);

uint8_t app_system_file_check(uint8_t* diskx);              /* 系统文件检测 */
uint8_t app_boot_cpdmsg(uint8_t*pname,uint8_t pct,uint8_t mode);
void app_boot_cpdmsg_set(uint16_t x,uint16_t y,uint8_t fsize);
uint8_t app_system_update(uint8_t(*fcpymsg)(uint8_t*pname,uint8_t pct,uint8_t mode),uint8_t* src);
void app_getstm32_sn(uint32_t *sn0,uint32_t *sn1,uint32_t *sn2);
void app_get_version(uint8_t*buf,uint32_t ver,uint8_t len); /* 得到版本号 */

void app_usmart_getsn(void);                /* USMART专用. */
uint8_t app_system_parameter_init(void);    /* 系统信息初始化 */
void app_lcd_auto_bklight(void);            /* LCD背光自动控制函数 */
//void app_es8388_volset(uint8_t vol);        /* ES8388音量设置 */
//void app_es8388_eqset(_es8388_obj *wmset,uint8_t eqx);  /* eq设置 */
//void app_es8388_setall(void);               /* 设置所有ES8388参数 */
void app_set_default(void);                 /* 恢复默认设置 */
#endif




























































