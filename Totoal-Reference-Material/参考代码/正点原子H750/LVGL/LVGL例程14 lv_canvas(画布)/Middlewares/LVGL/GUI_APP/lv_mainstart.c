/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_canvas(画布) 实验
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
 
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>



LV_FONT_DECLARE(myFont14) \
LV_FONT_DECLARE(myFont24)

/* 画布的宽高 */
#define canvas_width()  200
#define canvas_height() 200
/* 图片的宽高 */
#define img_width()     150
#define img_height()    100

/**
 * @brief  例
 * @param  无
 * @return 无
 */
static void lv_example_canvas(void)
{
    static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(canvas_width(), canvas_height())];       /* 定义画布缓冲区 */
    lv_obj_t* canvas = lv_canvas_create(lv_scr_act());                                                  /* 定义并初始化画布 */
    lv_canvas_set_buffer(canvas, canvas_buf, canvas_width(), canvas_height(), LV_IMG_CF_TRUE_COLOR);    /* 设置画布缓冲区 */
    lv_obj_center(canvas);                                                                              /* 设置画布位置 */
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);                    /* 设置画布背景颜色 */

    lv_draw_rect_dsc_t rect_dsc;                                                                        /* 定义绘画矩形 */
    lv_draw_rect_dsc_init(&rect_dsc);                                                                   /* 初始化绘画矩形 */
    rect_dsc.radius = 10;                                                                               /* 设置圆角 */
    rect_dsc.bg_opa = LV_OPA_COVER;                                                                     /* 设置透明度 */
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;                                                             /* 设置颜色渐变方向 */
    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);                                  /* 设置开始颜色 */
    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);                                 /* 设置结束颜色 */
    rect_dsc.border_width = 2;                                                                          /* 设置边缘宽度 */
    rect_dsc.border_opa = LV_OPA_90;                                                                    /* 设置边缘透明度 */
    rect_dsc.border_color = lv_color_white();                                                           /* 设置边缘颜色 */
    lv_canvas_draw_rect(canvas,                                                                         /* 在画布上绘制矩形 */
                        (canvas_width() - img_width()) / 2,
                        (canvas_height() - img_height()) / 2,
                        img_width(),
                        img_height(),
                        &rect_dsc);

    lv_draw_label_dsc_t label_dsc;                                                                      /* 定义绘制标签 */
    lv_draw_label_dsc_init(&label_dsc);                                                                 /* 初始化绘制标签 */
    label_dsc.color = lv_color_black();                                                                 /* 设置标签颜色 */
    lv_canvas_draw_text(canvas,                                                                         /* 在画布上绘制标签 */
                        canvas_width() / 8,
                        canvas_height() / 8,
                        100,
                        &label_dsc,
                        "Some text on text canvas");

    static lv_color_t canvas_buf_temp[LV_CANVAS_BUF_SIZE_TRUE_COLOR(canvas_width(), canvas_height())];  /* 定义图片缓冲区 */
    lv_memcpy(canvas_buf_temp, canvas_buf, sizeof(canvas_buf_temp));                                    /* 复制旧缓冲区 */
    lv_img_dsc_t img;                                                                                   /* 定义图片 */
    img.data = (uint8_t*)canvas_buf_temp;                                                               /* 设置图片数据 */
    img.header.cf = LV_IMG_CF_TRUE_COLOR;                                                               /* 图片颜色格式 */
    img.header.w = canvas_width();                                                                      /* 宽度 */
    img.header.h = canvas_height();                                                                     /* 高度 */

    lv_canvas_transform(canvas,                                                                         /* 旋转画布 */
                        &img, 30,
                        LV_IMG_ZOOM_NONE,
                        0, 0,
                        canvas_width() / 2,
                        canvas_height() / 2,
                        true);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_canvas();
}
