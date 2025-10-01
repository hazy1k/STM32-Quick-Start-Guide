/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL雷达 实验
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
#include "./BSP/LCD/lcd.h"
#include "lvgl.h"
#include <stdio.h>



LV_FONT_DECLARE(myFont14) \
LV_FONT_DECLARE(myFont24)

static lv_obj_t *sreen;

lv_obj_t *time_label;         /* 时间标签 */
lv_obj_t *data_label;         /* 数据标签 */
lv_obj_t *image_right_bpttom; /* 右底下图片 */
lv_obj_t *image_right_top;    /* 右顶部图片 */
lv_obj_t *image_left_bpttom;  /* 左底下图片 */
lv_obj_t *image_left_bpttom1; /* 右左底下红色图片 */
lv_obj_t *image_left_top;     /* 左顶部图片 */
lv_obj_t *font_left_top;      /* 左顶部字体 */
lv_obj_t *font_left_bottom;   /* 左底下字体 */
lv_obj_t *image_biao;         /* 中间图片 */
lv_obj_t *image_scan;         /* 中间扫描下图片 */

LV_IMG_DECLARE(biao)
LV_IMG_DECLARE(right_bottom)
LV_IMG_DECLARE(right_bottom1)
LV_IMG_DECLARE(right_top)
LV_IMG_DECLARE(left_bottom)
LV_IMG_DECLARE(left_bottom1)
LV_IMG_DECLARE(left_bottom2)
LV_IMG_DECLARE(left_bottom3)
LV_IMG_DECLARE(left_bottom4)
LV_IMG_DECLARE(left_bottom5)
LV_IMG_DECLARE(left_bottom6)
LV_IMG_DECLARE(left_top)
LV_IMG_DECLARE(left_top1)
LV_IMG_DECLARE(left_top2)
LV_IMG_DECLARE(left_top3)
LV_IMG_DECLARE(left_top4)
LV_IMG_DECLARE(left_top5)
LV_IMG_DECLARE(left_top6)
LV_IMG_DECLARE(left_top7)
LV_IMG_DECLARE(left_top8)
LV_IMG_DECLARE(left_top9)
LV_IMG_DECLARE(left_top10)
LV_IMG_DECLARE(left_top11)
LV_IMG_DECLARE(scan)
LV_IMG_DECLARE(scan60)
LV_IMG_DECLARE(scan120)
LV_IMG_DECLARE(scan180)
LV_IMG_DECLARE(scan240)
LV_IMG_DECLARE(scan360)

void lv_img_show(lv_obj_t *parent);

/**
 * @brief       设置图标
 * @param       parent：父类
 * @retval      无
 */
void lv_set_symbol(lv_obj_t *parent)
{
    lv_obj_t *right_symbol_label;
    lv_obj_t *left_symbol_label;

    right_symbol_label = lv_label_create(parent);
    lv_obj_align_to(right_symbol_label, NULL, LV_ALIGN_TOP_RIGHT, -25, 0);
    lv_label_set_text(right_symbol_label, LV_SYMBOL_USB LV_SYMBOL_WIFI LV_SYMBOL_BATTERY_3);

    left_symbol_label = lv_label_create(parent);
    lv_obj_align_to(left_symbol_label, NULL, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(left_symbol_label, LV_SYMBOL_HOME LV_SYMBOL_AUDIO);
}

/**
 * @brief       设置时间
 * @param       parent：父类
 * @retval      无
 */
void lv_set_time(lv_obj_t *parent)
{
    time_label = lv_label_create(parent);
    lv_obj_align_to(time_label, NULL, LV_ALIGN_TOP_MID, -20, 20);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_38, LV_STATE_DEFAULT);
    lv_label_set_text(time_label, "12:44");

    data_label = lv_label_create(parent);
    lv_obj_align_to(data_label, time_label, LV_ALIGN_OUT_BOTTOM_MID, -10, 0);
    lv_obj_set_style_text_font(data_label, &lv_font_montserrat_22, LV_STATE_DEFAULT);
    lv_label_set_text(data_label, "2021/3/27");
}

/**
 * @brief       添加选项按键
 * @param       parent：父类
 * @retval      无
 */
void lv_add_time(lv_obj_t *parent)
{
    lv_set_time(parent);
}

int right_num = 0;

/**
 * @brief       右边底部图片更新函数
 * @param       task：任务
 * @retval      无
 */
void lv_right_bottom(lv_timer_t *task)
{
    right_num++;

    lv_img_set_src(image_right_bpttom, (right_num != 0) ? &right_bottom : &right_bottom1);

    if (right_num > 2)
    {
        right_num = 0;
        lv_img_set_src(image_right_bpttom, (right_num != 0) ? &right_bottom : &right_bottom1);
    }
}

int image_number = 0;
/**
 * @brief       右边顶部图片更新函数
 * @param       task：任务
 * @retval      无
 */
void lv_image_right_top(lv_timer_t *task)
{
    uint32_t * user_data = task->user_data;

    if (*user_data == 0)
    {
        image_number = image_number + 100;   /* 0~3600转成0°~360°必须除以10 */
        lv_img_set_angle(image_right_top, image_number);/* 设置角度 */
        lv_img_set_antialias(image_right_top, true);    /* 抗锯齿 */
    }

    if (image_number >= 3600)
    {
        image_number = 0;
        *user_data = 0;
    }
}

lv_img_dsc_t scr[6];
int scr_num = 0;
int task_user = 0;

/**
 * @brief       左边底下图片更新函数
 * @param       task：任务
 * @retval      无
 */
void lv_image_left_bpttom(lv_timer_t *task)
{
    int  user_data = task_user;

    if (user_data == 0)
    {
        scr_num++;
        lv_img_set_src(image_left_bpttom1, &scr[scr_num]);
    }

    if (scr_num > 5 || (int)user_data == 1)
    {
        task_user = 1;
        scr_num--;
        lv_img_set_src(image_left_bpttom1, &scr[scr_num]);

        if (scr_num == 0)
        {
            task_user = 0;
        }
    }
}

int left_bottom_text_num = 0;         /* 图片索引 */
int left_bottom_text_user = 0;
static char left_bottom_text_buf[32]; /* 数据缓冲区 */

/**
 * @brief       左边底下图片上的字体更新函数
 * @param       task：任务
 * @retval      无
 */
void lv_image_text_task(lv_timer_t *task)
{
    int user_data = left_bottom_text_user;

    if (user_data == 0)
    {
        memset(left_bottom_text_buf, 0, sizeof(left_bottom_text_buf));
        left_bottom_text_num++;
        lv_snprintf(left_bottom_text_buf, sizeof(left_bottom_text_buf), "%d %K/m", left_bottom_text_num);
        lv_label_set_text(font_left_bottom, left_bottom_text_buf);
    }

    if (left_bottom_text_num > 100 || user_data == 1)
    {
        memset(left_bottom_text_buf, 0, sizeof(left_bottom_text_buf));
        left_bottom_text_user = 1;
        left_bottom_text_num --;
        lv_snprintf(left_bottom_text_buf, sizeof(left_bottom_text_buf), "%d %K/m", left_bottom_text_num);
        lv_label_set_text(font_left_bottom, left_bottom_text_buf);

        if (left_bottom_text_num == 0)
        {
            left_bottom_text_user = 0;
        }
    }
}

lv_img_dsc_t left_top_image[11]; /* 图片源 */
int left_top_image_num = 0;      /* 图片索引 */
int left_top_ext = 0;            /* 速度变量 */
int left_task_user = 0;          /* 中间变量 */
lv_obj_t *left_top_flag;         /* 标志位 */
static char buf[32];             /* 速度数据缓冲区 */

/**
 * @brief       左边顶部定时函数
 * @param       task：任务
 * @retval      无
 */
void lv_image_left_top_task(lv_timer_t *task)
{
    int user_data = left_task_user;


    if (user_data == 0)
    {
        memset(buf,0,sizeof(buf));
        left_top_image_num++;                  /* 图片索引加1 */
        left_top_ext = left_top_ext + 10;      /* 速度加10操作 */
        lv_snprintf(buf, sizeof(buf), "%d %%", left_top_ext);
        lv_label_set_text(font_left_top, buf); /* 左上角的数据更新 */
        lv_img_set_src(left_top_flag, &left_top_image[left_top_image_num]); /* 设置图片源 */
    }

    /* 当图片索引大于10或者user_data等于1 */
    if (left_top_image_num > 10 || user_data == 1)
    {
        memset(buf, 0, sizeof(buf));
        left_task_user = 1;
        left_top_image_num--;            /* 图片索引减1 */
        left_top_ext = left_top_ext - 10;/* 速度减10操作 */
        lv_snprintf(buf, sizeof(buf), "%d %%", left_top_ext);
        lv_label_set_text(font_left_top, buf); /* 左上角的数据更新 */
        lv_img_set_src(left_top_flag, &left_top_image[left_top_image_num]);/* 设置图片源 */

        if (left_top_image_num == 0)
        {
            left_task_user = 0;
        }
    }
}

lv_img_dsc_t scanimage[6]; /* 扫描图片源 */
int scanimage_num = 0;     /* 扫描图片的索引 */

/**
 * @brief       中间扫描定时函数
 * @param       task：任务
 * @retval      无
 */
void lv_scanimage_task(lv_timer_t *task)
{
    scanimage_num++;      /* 扫描数量加1 */

    if (scanimage_num > 5)/* 当大于5时设置scanimage_num为0 */
    {
        scanimage_num = 0;
    }
    
    /* 按照图片的索引设置图片 */
    lv_img_set_src(image_scan, &scanimage[scanimage_num]);
}

/**
 * @brief       图片显示
 * @param       parent：父类
 * @retval      无
 */
void lv_img_show(lv_obj_t *parent)
{
    static uint32_t user_data = 0;
    /* 左边底下的视图 */
    scr[0] = left_bottom1;
    scr[1] = left_bottom2;
    scr[2] = left_bottom3;
    scr[3] = left_bottom4;
    scr[4] = left_bottom5;
    scr[5] = left_bottom6;
  
    /* 中间的视图 */
    scanimage[0] = scan;
    scanimage[1] = scan60;
    scanimage[2] = scan120;
    scanimage[3] = scan180;
    scanimage[4] = scan240;
    scanimage[5] = scan360;
  
    /* 左边顶部的视图 */
    left_top_image[0] = left_top1;
    left_top_image[1] = left_top2;
    left_top_image[2] = left_top3;
    left_top_image[3] = left_top4;
    left_top_image[4] = left_top5;
    left_top_image[5] = left_top6;
    left_top_image[6] = left_top7;
    left_top_image[7] = left_top8;
    left_top_image[8] = left_top9;
    left_top_image[9] = left_top11;
    left_top_image[10] = left_top10;

    /* 中间的图片 */
    image_biao = lv_img_create(parent);
    lv_img_set_src(image_biao, &biao);
    lv_obj_align_to(image_biao, parent, LV_ALIGN_CENTER, 0, 0);

    /* 中间扫描的图片 */
    image_scan = lv_img_create(image_biao);
    lv_img_set_src(image_scan, &scanimage[0]);
    lv_obj_align_to(image_scan, image_biao, LV_ALIGN_CENTER, 0, 0);
    lv_timer_create(lv_scanimage_task, 500, NULL);

    /* 右边底下的图片 */
    image_right_bpttom = lv_img_create(parent);
    lv_img_set_src(image_right_bpttom, &right_bottom);
    lv_obj_align_to(image_right_bpttom, parent, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_timer_create(lv_right_bottom, 300, NULL);

    /* 右边顶部的图片 */
    image_right_top = lv_img_create(parent);
    lv_img_set_src(image_right_top, &right_top);
    lv_obj_align_to(image_right_top, parent, LV_ALIGN_TOP_RIGHT, 0, 20);
    lv_timer_create(lv_image_right_top, 500, &user_data);

    /* 左边底下的红色的图片 */
    image_left_bpttom = lv_img_create(parent);
    lv_img_set_src(image_left_bpttom, &left_bottom);
    lv_obj_align_to(image_left_bpttom, parent, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_timer_create(lv_image_left_bpttom, 200, &user_data);

    /* 左边底下图片上的字体 */
    font_left_bottom = lv_label_create(image_left_bpttom);
    lv_label_set_text(font_left_bottom, "00");
    lv_obj_set_style_text_color(font_left_bottom, lv_color_hex(0x4DD1E1), LV_STATE_DEFAULT);
    lv_obj_align_to(font_left_bottom, image_left_bpttom, LV_ALIGN_CENTER, -20, -10);
    lv_timer_create(lv_image_text_task, 10, &user_data);

    /* 左边地下的图片 */
    image_left_bpttom1 = lv_img_create(parent);
    lv_img_set_src(image_left_bpttom1, &scr[0]);
    lv_obj_align_to(image_left_bpttom1, parent, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    /* 左边顶部的红色的图片 */
    image_left_top = lv_img_create(parent);
    lv_img_set_src(image_left_top, &left_top);
    lv_obj_align_to(image_left_top, parent, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_timer_create(lv_image_left_top_task, 200, &user_data);

    /* 左边顶部的图片 */
    left_top_flag = lv_img_create(image_left_top);
    lv_img_set_src(left_top_flag, &left_top_image[0]);
    lv_obj_align_to(left_top_flag, image_left_top, LV_ALIGN_CENTER, 0, 0);

    /* 左边顶部的字体 */
    font_left_top = lv_label_create(image_left_top);
    lv_label_set_text(font_left_top, "00");
    lv_obj_set_style_text_color(font_left_top, lv_color_hex(0x4DD1E1), LV_STATE_DEFAULT);
    lv_obj_align_to(font_left_top, image_left_top, LV_ALIGN_CENTER, 5, -5);
}

/**
 * @brief       LVGL程序入口
 * @param       无
 * @retval      无
 */
void lv_interface_show(void)
{
    sreen = lv_scr_act();   /* 创建屏幕 */
    lv_obj_t *lv_obj = lv_obj_create(sreen);
    /* 设置容器大小 */
    lv_obj_set_size(lv_obj,lcddev.width,lcddev.height);
    /* 设置背景颜色 */
    lv_obj_set_style_bg_color(lv_obj,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    /* 设置改容器不可点击 */
    lv_obj_add_flag(lv_obj, false);
    lv_set_symbol(lv_obj); /* 创建图标 */
    lv_add_time(lv_obj);   /* 添加选项卡 */
    lv_img_show(lv_obj);   /* 第一个页签 */
}

/**
 * @brief       LVGL程序入口
 * @param       无
 * @retval      无
 */
void lv_mainstart(void)
{
    lv_interface_show(); 
}
