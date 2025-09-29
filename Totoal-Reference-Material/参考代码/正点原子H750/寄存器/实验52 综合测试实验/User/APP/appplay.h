/**
 ****************************************************************************************************
 * @file        appplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-06-06
 * @brief       APP-应用中心 代码
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
 * V1.1 20160904
 * 新增光纤音频接收(SPDIF RX)功能
 * V1.1 20220606
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __APPPLAY_H
#define __APPPLAY_H
#include "common.h"


#define APPPLAY_EX_BACKCOLOR    0X0000          /* 窗体外部背景色 */
#define APPPLAY_IN_BACKCOLOR    0X8C51          /* 窗体内部背景色 */
#define APPPLAY_NAME_COLOR      0X001F          /* 程序名颜色 */

#define APPPLAY_ALPHA_VAL       18              /* APP选中透明度设置 */
#define APPPLAY_ALPHA_COLOR     WHITE           /* APP透明色 */

/* 各图标/图片路径 */
extern uint8_t *const appplay_icospath_tbl[3][16];  /* icos的路径表 */


/* APP图标参数管理 */
typedef __PACKED_STRUCT _m_app_icos
{
    uint16_t x;         /* 图标坐标及尺寸 */
    uint16_t y;
    uint8_t width;
    uint8_t height;
    uint8_t *path;      /* 图标路径 */
    uint8_t *name;      /* 图标名字 */
} m_app_icos;

/* APP控制器 */
typedef struct _m_app_dev
{
    uint8_t selico; /**
                     * 当前选中的图标
                     * 0~15,被选中的图标编号
                     * 其他,没有任何图标被选中
                     */
    
    m_app_icos icos[14];    /* 总共16个图标 */
} m_app_dev;


uint8_t app_play(void);

#endif























