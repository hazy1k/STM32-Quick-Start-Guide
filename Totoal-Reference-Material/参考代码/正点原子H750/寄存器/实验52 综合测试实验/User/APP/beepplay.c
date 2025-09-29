/**
 ****************************************************************************************************
 * @file        beepplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-蜂鸣器测试 代码
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

#include "beepplay.h"
#include "gradienter.h"
#include "./BSP/BEEP/beep.h"


/****************************************************************************************************/
/* 从gradienter.c拷贝过来的代码 */

/**
 * @brief       加载字体
 * @param       无
 * @retval      0, OK;  其他, 错误代码;
 */
uint8_t grad_load_font(void)
{
    uint8_t rval = 0;
    uint8_t res;
    uint32_t br;
    FIL *f_grad = 0;
    f_grad = (FIL *)gui_memin_malloc(sizeof(FIL));  /* 开辟FIL字节的内存区域 */

    if (f_grad == NULL)rval = 1;                    /* 申请失败 */
    else
    {
        if (lcddev.width <= 272)
        {
            res = f_open(f_grad, (const TCHAR *)APP_ASCII_S7236, FA_READ); /* 打开文件 */

            if (res == FR_OK)
            {
                asc2_7236 = (uint8_t *)gui_memex_malloc(f_grad->obj.objsize);   /* 为大字体开辟缓存地址 */

                if (asc2_7236 == 0)rval = 1;
                else res = f_read(f_grad, asc2_7236, f_grad->obj.objsize, (UINT *)&br); /* 一次读取整个文件 */
            }
        }
        else if (lcddev.width == 320)
        {
            res = f_open(f_grad, (const TCHAR *)APP_ASCII_S8844, FA_READ);      /* 打开文件 */

            if (res == FR_OK)
            {
                asc2_8844 = (uint8_t *)gui_memex_malloc(f_grad->obj.objsize);   /* 为大字体开辟缓存地址 */

                if (asc2_8844 == 0)rval = 1;
                else res = f_read(f_grad, asc2_8844, f_grad->obj.objsize, (UINT *)&br); /* 一次读取整个文件 */
            }
        }
        else if (lcddev.width >= 480)
        {
            res = f_open(f_grad, (const TCHAR *)APP_ASCII_S14472, FA_READ); /* 打开文件 */

            if (res == FR_OK)
            {
                asc2_14472 = (uint8_t *)gui_memex_malloc(f_grad->obj.objsize);  /* 为大字体开辟缓存地址 */

                if (asc2_14472 == 0)rval = 1;
                else res = f_read(f_grad, asc2_14472, f_grad->obj.objsize, (UINT *)&br);    /* 一次读取整个文件 */
            }
        }

        if (res)rval = res;
    }

    gui_memin_free(f_grad);/* 释放内存 */
    return rval;
}

/**
 * @brief       删除字体
 * @param       无
 * @retval      无
 */
void grad_delete_font(void)
{
    if (lcddev.width == 240)
    {
        gui_memex_free(asc2_7236);
        asc2_7236 = 0;
    }
    else if (lcddev.width == 320)
    {
        gui_memex_free(asc2_8844);
        asc2_8844 = 0;
    }
    else if (lcddev.width >= 480)
    {
        gui_memex_free(asc2_14472);
        asc2_14472 = 0;
    }
}

/****************************************************************************************************/


/* BEEP按钮标题 */
uint8_t *const beep_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    {"打开", "打開", "ON",},
    {"关闭", "關閉", "OFF",},
};

/**
 * @brief       蜂鸣器测试
 * @param       无
 * @retval      未用到
 */
uint8_t beep_play(void)
{

    uint8_t res, rval = 0;
    uint8_t *buf;
    uint8_t beepsta = 0;

    _btn_obj *beepbtn = 0;  /* 控制按钮 */
    uint16_t btnx, btny, btnw, btnh;    /* 按键坐标参数 */
    uint16_t cx, cy, cr;    /* 圆坐标参数 */
    uint8_t btnfsize;       /* 字体大小 */
    uint8_t fsize = 0;      /* ON/OFF字体大小 */

    if (lcddev.width > 320)btnfsize = 32;
    else btnfsize = 24;

    btnw = lcddev.width * 2 / 3;
    btnh = btnw / 4;
    btnx = (lcddev.width - btnw) / 2;
    btny = lcddev.height - 2 * btnh;

    if (lcddev.width <= 272)
    {
        fsize = 72;
    }
    else if (lcddev.width == 320)
    {
        fsize = 88;
    }
    else if (lcddev.width >= 480)
    {
        fsize = 144;
    }

    cx = lcddev.width / 2;
    cy = cx + gui_phy.tbheight;
    cr = lcddev.width / 3;
    beepbtn = btn_creat(btnx, btny, btnw, btnh, 0, 0);
    buf = gui_memin_malloc(32); /* 申请内存 */

    if (buf && beepbtn)rval = grad_load_font(); /* 加载字体 */
    else rval = 1;

    if (rval == 0)
    {
        lcd_clear(LGRAY);
        app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);  /* 下分界线 */
        gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[17][gui_phy.language]); /* 显示标题 */

        beepbtn->caption = beep_btncaption_tbl[0][gui_phy.language];
        beepbtn->font = btnfsize;

        btn_draw(beepbtn);  /* 画按钮 */
        beepbtn->caption = beep_btncaption_tbl[1][gui_phy.language];

        gui_fill_circle(cx, cy, cr, RED);
        BEEP(0);            /* 关闭蜂鸣器 */
        sprintf((char *)buf, "OFF");
        gui_show_strmid(0, cy - (fsize / 2), lcddev.width, fsize, WHITE, fsize, buf);   /* 显示新内容 */
        system_task_return = 0;

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
            res = btn_check(beepbtn, &in_obj);

            if (res && ((beepbtn->sta & (1 << 7)) == 0) && (beepbtn->sta & (1 << 6)))   /* 有输入,有按键按下且松开,并且TP松开了 */
            {
                beepbtn->caption = beep_btncaption_tbl[beepsta][gui_phy.language];
                beepsta = !beepsta;

                if (beepsta)
                {
                    sprintf((char *)buf, "ON");
                    BEEP(1);/* 打开蜂鸣器 */
                }
                else
                {
                    sprintf((char *)buf, "OFF");
                    BEEP(0);/* 关闭蜂鸣器 */
                }

                gui_fill_circle(cx, cy, cr, RED);
                gui_show_strmid(0, cy - (fsize / 2), lcddev.width, fsize, WHITE, fsize, buf);	/* 显示新内容 */
            }

            if (system_task_return)break;   /* TPAD返回 */

            delay_ms(3);
        }
    }

    BEEP(0);            /* 关闭蜂鸣器 */
    btn_delete(beepbtn);/* 删除按钮 */
    grad_delete_font(); /* 删除字体 */
    gui_memin_free(buf);/* 释放内存 */
    return rval;
}







































