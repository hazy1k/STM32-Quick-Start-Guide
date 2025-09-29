/**
 ****************************************************************************************************
 * @file        ledplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-LED测试 代码
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
 * V1.1 20220606
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "ledplay.h"
#include "gradienter.h"
#include "rgbledplay.h"


/* DS0按钮标题 */
uint8_t *const ds0_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    {"红灯亮", "紅燈亮", "R ON",},
    {"红灯灭", "紅燈滅", "R OFF",},
};

/* DS1按钮标题 */
uint8_t *const ds1_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    {"绿灯亮", "綠燈亮", "G ON",},
    {"绿灯灭", "綠燈滅", "G OFF",},
};

/* DS3按钮标题 */
uint8_t *const ds2_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    {"蓝灯亮", "藍燈亮", "B ON",},
    {"蓝灯灭", "藍燈滅", "B OFF",},
};

extern volatile uint8_t ledplay_ds0_sta;    /* ledplay任务,DS0的控制状态 */

/**
 * @brief       LED测试
 * @param       无
 * @retval      未用到
 */
uint8_t led_play(void)
{

    uint8_t res, rval = 0;
    uint8_t ds0sta = 1, ds1sta = 1, ds2sta = 1;

    _btn_obj *ds0btn = 0;   /* 控制按钮 */
    _btn_obj *ds1btn = 0;   /* 控制按钮 */
    _btn_obj *ds2btn = 0;   /* 控制按钮 */

    uint16_t btnw, btnh;    /* 按钮参数 */
    uint16_t btnds0x, btnds0y, btnds1x, btnds1y, btnds2x, btnds2y;  /* 按钮坐标参数 */

    uint16_t cds0x, cds0y, cds1x, cds1y, cds2x, cds2y, cr;          /* 圆坐标参数 */

    uint8_t btnfsize;       /* 字体大小 */

    if (lcddev.width >= 480)
    {
        btnfsize = 32;
    }
    else if (lcddev.width >= 320)
    {
        btnfsize = 24;
    }
    else if (lcddev.width >= 240)
    {
        btnfsize = 16;
    }

    btnw = lcddev.width / 4;
    btnh = btnw / 2;

    cr = lcddev.width / 5;
    cds0x = lcddev.width / 20 + cr;
    cds2x = cds0x + cr * 2 + lcddev.width / 10;
    cds1x = lcddev.width / 2;

    cds0y = (lcddev.height - cr * 2 - 2 * btnh) / 2 + 1.732 * cr;
    cds2y = cds0y;
    cds1y = cds0y - cr * 1.732;

    btnds0x = btnw / 6;
    btnds0y = (lcddev.height - cr * 3.5 - 2 * btnh) / 2 + 3.5 * cr + btnh;

    btnds1x = btnds0x + btnw / 3 + btnw;
    btnds1y = btnds0y;

    btnds2x = btnds1x + btnw / 3 + btnw;
    btnds2y = btnds0y;

    ds0btn = btn_creat(btnds0x, btnds0y, btnw, btnh, 0, 0);
    ds1btn = btn_creat(btnds1x, btnds1y, btnw, btnh, 0, 0);
    ds2btn = btn_creat(btnds2x, btnds2y, btnw, btnh, 0, 0);

    if (ds0btn && ds1btn)
    {
        lcd_clear(LGRAY);
        app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);  /* 下分界线 */
        gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[19][gui_phy.language]); /* 显示标题 */

        ds0btn->caption = ds0_btncaption_tbl[0][gui_phy.language];
        ds0btn->font = btnfsize;
        ds1btn->caption = ds1_btncaption_tbl[0][gui_phy.language];
        ds1btn->font = btnfsize;
        ds2btn->caption = ds2_btncaption_tbl[0][gui_phy.language];
        ds2btn->font = btnfsize;
 
        btn_draw(ds0btn);   /* 画按钮 */
        btn_draw(ds1btn);   /* 画按钮 */
        btn_draw(ds2btn);   /* 画按钮 */

        ds0btn->caption = ds0_btncaption_tbl[1][gui_phy.language];
        ds1btn->caption = ds1_btncaption_tbl[1][gui_phy.language];
        ds2btn->caption = ds2_btncaption_tbl[1][gui_phy.language];
        gui_fill_circle(cds0x, cds0y, cr, WHITE);
        gui_fill_circle(cds1x, cds1y, cr, WHITE);
        gui_fill_circle(cds2x, cds2y, cr, WHITE);
        system_task_return = 0;
        ledplay_ds0_sta = 1;    /* 关闭系统控制DS0 */

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
            res = btn_check(ds0btn, &in_obj);

            if (res && ((ds0btn->sta & (1 << 7)) == 0) && (ds0btn->sta & (1 << 6))) /* 有输入,有按键按下且松开,并且TP松开了 */
            {
                ds0sta = !ds0sta;
                ds0btn->caption = ds0_btncaption_tbl[ds0sta][gui_phy.language];

                if (ds0sta)
                {
                    gui_fill_circle(cds0x, cds0y, cr, WHITE);
                }
                else
                {
                    gui_fill_circle(cds0x, cds0y, cr, RED);
                }

                LEDR(ds0sta);
            }

            res = btn_check(ds1btn, &in_obj);

            if (res && ((ds1btn->sta & (1 << 7)) == 0) && (ds1btn->sta & (1 << 6))) /* 有输入,有按键按下且松开,并且TP松开了 */
            {
                ds1sta = !ds1sta;
                ds1btn->caption = ds1_btncaption_tbl[ds1sta][gui_phy.language];

                if (ds1sta)
                {
                    gui_fill_circle(cds1x, cds1y, cr, WHITE);
                }
                else
                {
                    gui_fill_circle(cds1x, cds1y, cr, GREEN);
                }

                LEDG(ds1sta);
            }

            res = btn_check(ds2btn, &in_obj);

            if (res && ((ds2btn->sta & (1 << 7)) == 0) && (ds2btn->sta & (1 << 6))) /* 有输入,有按键按下且松开,并且TP松开了 */
            {
                ds2sta = !ds2sta;
                ds2btn->caption = ds2_btncaption_tbl[ds2sta][gui_phy.language];

                if (ds2sta)
                {
                    gui_fill_circle(cds2x, cds2y, cr, WHITE);
                }
                else
                {
                    gui_fill_circle(cds2x, cds2y, cr, BLUE);
                }

                LEDB(ds2sta);
            }

            if (system_task_return)break;   /* TPAD返回 */

            delay_ms(10);
        }
    }

    ledplay_ds0_sta = 0;    /* 恢复系统控制DS0 */
    LEDR(1);
    LEDG(1);
    LEDB(1);            /* 关闭LED */
    btn_delete(ds0btn); /* 删除按钮 */
    btn_delete(ds1btn); /* 删除按钮 */
    btn_delete(ds2btn); /* 删除按钮 */
    return rval;
}







