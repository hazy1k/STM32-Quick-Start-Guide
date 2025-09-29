/**
 ****************************************************************************************************
 * @file        appplay.c
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

#include "appplay.h"
#include "appplay_remote.h"
#include "appplay_wlvideo.h"


#define APPPLAY_EX_BACKCOLOR    0X0000          /* 窗体外部背景色 */
#define APPPLAY_IN_BACKCOLOR    0X8C51          /* 窗体内部背景色 */
#define APPPLAY_NAME_COLOR      0X001F          /* 程序名颜色 */

#define APPPLAY_ALPHA_VAL       18              /* APP选中透明度设置 */
#define APPPLAY_ALPHA_COLOR     WHITE           /* APP透明色 */


/* 应用程序名列表 */
/* 这里列表为16个,名字长度不要超过8个字节(4个汉字) */
uint8_t *const appplay_appname_tbl[3][16] =
{
    {
        "红外遥控", "应用程序", "应用程序", "应用程序",
        "应用程序", "应用程序", "应用程序", "应用程序",
        "应用程序", "应用程序", "应用程序", "应用程序",
        "应用程序", "应用程序", "应用程序", "应用程序",
    },
    {
        "紅外遙控", "應用程序", "應用程序", "應用程序",
        "應用程序", "應用程序", "應用程序", "應用程序",
        "應用程序", "應用程序", "應用程序", "應用程序",
        "應用程序", "應用程序", "應用程序", "應用程序",
    },
    {
        "REMOTE", "APP", "APP", "APP",
        "APP", "APP", "APP", "APP",
        "APP", "APP", "APP", "APP",
        "APP", "APP", "APP", "APP",
    },
};

/* appplay的图标路径表 */
uint8_t *const appplay_icospath_tbl[3][16] =
{
    {
        "1:/SYSTEM/APP/APPS/ICOS/remote_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/spdif_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app01_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app02_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app03_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app04_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app05_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app06_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app07_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app08_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app09_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app10_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app11_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app12_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app13_48.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app14_48.bmp",
    },

    {
        "1:/SYSTEM/APP/APPS/ICOS/remote_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/spdif_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app01_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app02_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app03_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app04_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app05_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app06_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app07_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app08_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app09_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app10_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app11_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app12_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app13_60.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app14_60.bmp",
    },
    {
        "1:/SYSTEM/APP/APPS/ICOS/remote_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/spdif_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app01_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app02_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app03_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app04_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app05_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app06_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app07_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app08_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app09_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app10_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app11_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app12_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app13_90.bmp",
        "1:/SYSTEM/APP/APPS/ICOS/app14_90.bmp",
    },
};

/**
 * @brief       加载主界面
 * @param       appdev          : APP控制结构体
 * @retval      0, OK;  其他, 错误代码;
 */
uint8_t appplay_load_ui(m_app_dev *appdev)
{
    uint8_t i;
    uint8_t xdis, ydis;
    uint8_t wxoff, wyoff;
    uint8_t width, height;
    uint8_t icowidth;
    uint8_t icofsize;
    uint8_t dis;
    uint8_t lcdtype = 0;

    uint8_t rval = 0;
    _window_obj *twin = 0;  /* 窗体 */

    if (lcddev.width == 240)
    {
        wxoff = 4;
        wyoff = 4;
        xdis = 4;
        ydis = 5;
        icowidth = 48;
        icofsize = 12;
        width = 48 + 3 * 2;
        height = 48 + 12 + 3 * 2 + 2;
        lcdtype = 0;
    }
    else if (lcddev.width == 272)
    {
        wxoff = 6;
        wyoff = 6;
        xdis = 7;
        ydis = 7;
        icowidth = 48;
        icofsize = 12;
        width = 48 + 5 * 2;
        height = 48 + 12 + 5 * 2 + 2;
        lcdtype = 0;
    }
    else if (lcddev.width == 320)
    {
        wxoff = 8;
        wyoff = 8;
        xdis = 6;
        ydis = 6;
        icowidth = 60;
        icofsize = 12;
        width = 60 + 5 * 2;
        height = 60 + 12 + 5 * 2 + 2;
        lcdtype = 1;
    }
    else if (lcddev.width == 480)
    {
        wxoff = 10;
        wyoff = 10;
        xdis = 9;
        ydis = 8;
        icowidth = 90;
        icofsize = 16;
        width = 90 + 8 * 2;
        height = 90 + 16 + 8 * 2 + 2;
        lcdtype = 2;
    }
    else if (lcddev.width == 600)
    {
        wxoff = 20;
        wyoff = 20;
        xdis = 30;
        ydis = 10;
        icowidth = 90;
        icofsize = 16;
        width = 90 + 10 * 2;
        height = 90 + 16 + 10 * 2 + 2;
        lcdtype = 2;
    }
    else if (lcddev.width == 800)
    {
        wxoff = 20;
        wyoff = 20;
        xdis = 50;
        ydis = 20;
        icowidth = 90;
        icofsize = 16;
        width = 90 + 20 * 2;
        height = 90 + 16 + 20 * 2 + 2;
        lcdtype = 2;
    }

    dis = (width - icowidth) / 2;
    twin = window_creat(wxoff, gui_phy.tbheight + wyoff, lcddev.width - 2 * wxoff, lcddev.height - gui_phy.tbheight - 2 * wyoff, 0, 0X01, 16); /* 创建窗口 */

    if (twin)
    {
        twin->captionbkcu = APPPLAY_IN_BACKCOLOR;   /* 默认caption上部分背景色 */
        twin->captionbkcd = APPPLAY_IN_BACKCOLOR;   /* 默认caption下部分背景色 */
        twin->captioncolor = APPPLAY_IN_BACKCOLOR;  /* 默认caption的颜色 */
        twin->windowbkc = APPPLAY_IN_BACKCOLOR;     /* 默认背景色 */

        gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, APPPLAY_EX_BACKCOLOR); /* 填充背景色 */
        app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02); /* 下分界线 */
        gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize
                        , (uint8_t *)APP_MFUNS_CAPTION_TBL[21][gui_phy.language]); /* 显示标题 */
        window_draw(twin);

        for (i = 0; i < 16; i++)
        {
            appdev->icos[i].x = wxoff + xdis / 2 + (i % 4) * (width + xdis);
            appdev->icos[i].y = gui_phy.tbheight + wxoff + ydis / 2 + (i / 4) * (height + ydis);
            appdev->icos[i].width = width;
            appdev->icos[i].height = height;
            appdev->icos[i].path = (uint8_t *)appplay_icospath_tbl[lcdtype][i];
            appdev->icos[i].name = (uint8_t *)appplay_appname_tbl[gui_phy.language][i];
            rval = minibmp_decode(appdev->icos[i].path, appdev->icos[i].x, appdev->icos[i].y + dis, appdev->icos[i].width, icowidth, 0, 0);

            if (rval)break; /* 解码出错了 */

            gui_show_strmid(appdev->icos[i].x, appdev->icos[i].y + dis + icowidth + 2, appdev->icos[i].width, icofsize, APPPLAY_NAME_COLOR, icofsize, appdev->icos[i].name);
        }

        appdev->selico = 0XFF; /* 默认不选择任何一个 */
    }
    else rval = 1;

    window_delete(twin);
    return rval;
}

/**
 * @brief       设置选中哪个图标
 * @param       appdev          : APP控制结构体
 * @param       sel             : 0~15代表当前页的选中ico
 * @retval      无
 */
void appplay_set_sel(m_app_dev *appdev, uint8_t sel)
{
    uint8_t icowidth;
    uint8_t icofsize;
    uint8_t dis;

    if (sel >= 16)return; /* 非法的输入 */

    if (lcddev.width <= 272)
    {
        icowidth = 48;
        icofsize = 12;
    }
    else if (lcddev.width == 320)
    {
        icowidth = 60;
        icofsize = 12;
    }
    else if (lcddev.width >= 480)
    {
        icowidth = 90;
        icofsize = 16;
    }

    if (appdev->selico < 16)
    {
        dis = (appdev->icos[appdev->selico].width - icowidth) / 2;
        gui_fill_rectangle(appdev->icos[appdev->selico].x, appdev->icos[appdev->selico].y, appdev->icos[appdev->selico].width, appdev->icos[appdev->selico].height, APPPLAY_IN_BACKCOLOR); /* 清除之前的图片 */
        minibmp_decode(appdev->icos[appdev->selico].path, appdev->icos[appdev->selico].x, appdev->icos[appdev->selico].y + dis, appdev->icos[appdev->selico].width, icowidth, 0, 0);
        gui_show_strmid(appdev->icos[appdev->selico].x, appdev->icos[appdev->selico].y + dis + icowidth + 2, appdev->icos[appdev->selico].width, icofsize, APPPLAY_NAME_COLOR, icofsize, appdev->icos[appdev->selico].name);
    };

    appdev->selico = sel;

    dis = (appdev->icos[appdev->selico].width - icowidth) / 2;

    gui_alphablend_area(appdev->icos[appdev->selico].x, appdev->icos[appdev->selico].y, appdev->icos[appdev->selico].width, appdev->icos[appdev->selico].height, APPPLAY_ALPHA_COLOR, APPPLAY_ALPHA_VAL);

    minibmp_decode(appdev->icos[appdev->selico].path, appdev->icos[appdev->selico].x, appdev->icos[appdev->selico].y + dis, appdev->icos[appdev->selico].width, icowidth, 0, 0);

    gui_show_strmid(appdev->icos[appdev->selico].x, appdev->icos[appdev->selico].y + dis + icowidth + 2, appdev->icos[appdev->selico].width, icofsize, APPPLAY_NAME_COLOR, icofsize, appdev->icos[appdev->selico].name);
}

/**
 * @brief       触摸屏扫描
 * @param       appdev          : APP控制结构体
 * @retval      0~15,有效按键;  其他,无效;
 */
uint8_t appplay_tpscan(m_app_dev *appdev)
{
    static uint8_t firsttpd = 0;    /* 按键处理标志,防止一次按下,多次返回 */
    uint8_t i = 0XFF;
    tp_dev.scan(0);         /* 扫描 */

    if ((tp_dev.sta & TP_PRES_DOWN)) /* 有按键被按下 */
    {
        if (firsttpd == 0)  /* 第一次处理? */
        {
            firsttpd = 1;   /* 标记已经处理了此次按键 */

            for (i = 0; i < 16; i++)
            {
                if ((tp_dev.x[0] > appdev->icos[i].x) && (tp_dev.x[0] < appdev->icos[i].x + appdev->icos[i].width)
                        && (tp_dev.y[0] > appdev->icos[i].y) && (tp_dev.y[0] < appdev->icos[i].y + appdev->icos[i].height)) /* 在区域内 */
                {
                    break;/* 得到选中的编号 */
                }
            }
        }
    }
    else firsttpd = 0; /* 松开了 */

    return i;
}

/**
 * @brief       app应用
 * @param       无
 * @retval      无
 */
uint8_t app_play(void)
{
    uint8_t selx = 0XFF;
    uint8_t rval = 0;
    m_app_dev *appdev;
    appdev = (m_app_dev *)gui_memin_malloc(sizeof(m_app_dev));

    if (appdev == NULL)rval = 1;
    else if (appplay_load_ui(appdev))rval = 1;

    while (rval == 0)
    {
        selx = appplay_tpscan(appdev);

        if (selx < 16) /* 有有效按键 */
        {
            if (selx == appdev->selico) /* 第二次选择此条目 */
            {
                //printf("selx:%d\r\n",appdev->selico);
                switch (selx)
                {
                    case 0:/* 红外遥控器 */
                        appplay_remote(appdev->icos[selx].name);
                        break;

                    case 1:
                        //appplay_spdifrx(appdev->icos[selx].name);
                        break;
                }

                appplay_load_ui(appdev);/* 重新加载主界面 */
            }

            appplay_set_sel(appdev, selx);
            system_task_return = 0;
        }

        if (system_task_return)
        {
            break;
        }

        delay_ms(1000 / OS_TICKS_PER_SEC); /* 延时一个时钟节拍 */
    }

    gui_memin_free(appdev);/* 释放内存 */
    return 0;
}























