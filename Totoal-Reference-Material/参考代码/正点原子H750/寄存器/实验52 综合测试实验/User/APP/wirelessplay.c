/**
 ****************************************************************************************************
 * @file        wirelessplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-无线通信测试 代码
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
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "wirelessplay.h"
#include "paint.h"
#include "./BSP/NRF24L01/nrf24l01.h"
#include "./BSP/SPI/spi.h"



/* 重画按钮的名字 */
uint8_t *const wireless_cbtn_caption_tbl[GUI_LANGUAGE_NUM] =
{
    "重画", "重畫", "CLEAR",
};

/* 提醒内容 */
uint8_t *const wireless_remind_msg_tbl[GUI_LANGUAGE_NUM] =
{
    "未检测到NRF24L01,请检查!", "未檢測到NRF24L01,請檢查!", "No NRF24L01,Please Check...",
};

/* 模式选择 */
uint8_t *const wireless_mode_tbl[GUI_LANGUAGE_NUM][2] =
{
    {"发送模式", "接收模式",},
    {"發送模式", "接收模式",},
    {"TX Mode", "RX Mode",},
};

/**
 * @brief       无线传书
 * @param       无
 * @retval      0, 正常退出; 其他, 错误代码;
 */
uint8_t wireless_play(void)
{
    _btn_obj *cbtn = 0;     /* 清除按钮控件 */
    uint8_t res;
    uint8_t *caption;       /* 标题 */
    uint8_t mode = 0;       /* 0,发送模式;1,接收模式 */
    uint8_t tmp_buf[5];     /* buf[0~3]:坐标值;buf[4]:0,正常画图;1,清屏;2,退出 */
    uint16_t x = 0, y = 0;
    uint16_t lastx = 0XFFFF, lasty = 0XFFFF;
    uint8_t rval = 0;

    /* 先选择模式 */
    res = app_items_sel((lcddev.width - 180) / 2, (lcddev.height - 152) / 2, 180, 72 + 40 * 2, (uint8_t **)wireless_mode_tbl[gui_phy.language], 2, (uint8_t *)&mode, 0X90, (uint8_t *)APP_MODESEL_CAPTION_TBL[gui_phy.language]); /* 2个选择 */

    if (res == 0) /* 确认键按下了,同时按键正常创建 */
    {
        caption = (uint8_t *)APP_MFUNS_CAPTION_TBL[14][gui_phy.language];   /* 无线传输 */
        gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, LGRAY);       /* 填充背景色 */
        nrf24l01_init();            /* 初始化NRF24L01 */
        res = 0;

        while (nrf24l01_check())    /* 检测不到24L01,连续检查5秒 */
        {
            if (res == 0)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 70) / 2 - 15, 200, 70, (uint8_t *)wireless_remind_msg_tbl[gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);

            delay_ms(500);
            res++;

            if (res > 5)return 0; /* 超时未检测到,退出 */
        }

        gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight, LGRAY); /* 填充背景色 */

        if (mode == 0)nrf24l01_tx_mode();   /* 设置模式 */
        else nrf24l01_rx_mode();

        caption = (uint8_t *)wireless_mode_tbl[gui_phy.language][mode]; /* 标题 */
        app_filebrower(caption, 0X07);      /* 显示标题 */
        cbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbheight + 8, gui_phy.tbheight - 1, 0, 0x03); /* 创建文字按钮 */

        if (cbtn == NULL)rval = 1;          /* 没有足够内存够分配 */
        else
        {
            cbtn->caption = (uint8_t *)wireless_cbtn_caption_tbl[gui_phy.language]; /* 重画 */
            cbtn->font = gui_phy.tbfsize;   /* 设置新的字体大小 */
            cbtn->bcfdcolor = WHITE;        /* 按下时的颜色 */
            cbtn->bcfucolor = WHITE;        /* 松开时的颜色 */

            if (mode == 0)btn_draw(cbtn);   /* 发送模式，需要重画按钮 */
        }

        tmp_buf[4] = tp_dev.touchtype & 0X80;   /**
                                                 * 得到触摸屏类型,tmp_buf[4]:
                                                 *  
                                                 *  b3~0:0,不做任何处理
                                                 *       1,画点
                                                 *       2,清屏
                                                 *       3,退出
                                                 *  b4~6:保留
                                                 *  b7:0,电阻屏;1,电容屏
                                                 */
        
        while (rval == 0)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);	/* 得到按键键值 */
            tmp_buf[4] &= 0X80; /* 清除原来的设置 */

            if (system_task_return) /* TPAD返回 */
            {
                tmp_buf[4] |= 0X03; /* 功能3,退出 */

                if (mode == 0)      /* 发送模式,需要发送退出指令 */
                {
                    nrf24l01_tx_packet(tmp_buf); /* 发送模式,则发送退出指令 */
                }
                else break; /* 接收模式,直接退出 */
            }

            if (mode == 0) /* 发送模式 */
            {
                res = btn_check(cbtn, &in_obj); /* 检查重画按钮 */

                if (res) /* 重画按钮有有效操作 */
                {
                    if (((cbtn->sta & 0X80) == 0))      /* 按钮状态改变了 */
                    {
                        tmp_buf[4] |= 0X02;             /* 功能2,清屏 */
                        nrf24l01_tx_packet(tmp_buf);    /* 发送清除指令 */
                    }
                }

                if (tp_dev.sta & TP_PRES_DOWN)          /* 触摸屏被按下 */
                {
                    if (tp_dev.y[0] < (lcddev.height - gui_phy.tbheight) && tp_dev.y[0] > (gui_phy.tbheight + 1))   /* 在画图区域内 */
                    {
                        x = tp_dev.x[0];
                        y = tp_dev.y[0];
                        tmp_buf[0] = tp_dev.x[0] >> 8;
                        tmp_buf[1] = tp_dev.x[0] & 0xFF;
                        tmp_buf[2] = tp_dev.y[0] >> 8;
                        tmp_buf[3] = tp_dev.y[0] & 0xFF;
                        tmp_buf[4] |= 0X01;             /* 功能为1,正常画图 */
                        nrf24l01_tx_packet(tmp_buf);    /* 发送数据 */
                    }
                }
            }
            else    /* 接收模式 */
            {
                if (nrf24l01_rx_packet(tmp_buf) == 0)   /* 一旦接收到信息,则显示出来 */
                {
                    x = tmp_buf[0];
                    x = (x << 8) + tmp_buf[1];
                    y = tmp_buf[2];
                    y = (y << 8) + tmp_buf[3];
                }
            }

            if (tmp_buf[4] & 0X7F)  /* 需要处理 */
            {
                switch (tmp_buf[4] & 0X7F)
                {
                    case 1:         /* 正常画点 */
                        if (tmp_buf[4] & 0X80)      /* 电容屏 */
                        {
                            if (lastx == 0XFFFF)    /* 第一次 */
                            {
                                lastx = x;
                                lasty = y;
                            }

                            gui_draw_bline(lastx, lasty, x, y, 2, RED); /* 画线 */
                            lastx = x;
                            lasty = y;
                        }
                        else paint_draw_point(x, y, RED, 2);            /* 画图,半径为2 */

                        break;

                    case 2:/* 清除 */
                        gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - 2 * gui_phy.tbheight, LGRAY); /* 清除屏幕 */
                        break;

                    case 3:/* 退出 */
                        rval = 1; /* 标志退出 */
                        break;
                }
            }
            else lastx = 0XFFFF;

            delay_ms(5);/* 空闲 延时一个时钟节拍 */
        }
    }

    spi2_init();                    /* 初始化SPI */
    spi2_set_speed(SPI_SPEED_4);    /* 设置到高速模式 */
    btn_delete(cbtn);               /* 删除按钮 */
    return 0;
}






















