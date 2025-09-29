/**
 ****************************************************************************************************
 * @file        vmeterplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-电压表测试 代码
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

#include "vmeterplay.h"
#include "vmeterfont.c"
#include "paint.h"
#include "./BSP/TIMER/timer.h"
#include "./BSP/ADC/adc.h"


/**
 * @brief       在指定地址开始显示一个数码管字符
 * @param       x,y             : 起始坐标
 * @param       xend,yend       : 终点坐标
 * @param       offset          : 开始显示的偏移
 * @param       color           : 颜色
 * @param       size            : 字体大小
 * @param       chr             : 显示的字符
 * @param       mode            : 0,非叠加显示;  1,叠加显示;  2,大点叠加(只适用于叠加模式)
 * @retval      无
 */
void vmeter_show_7seg(uint16_t x, uint16_t y, uint16_t xend, uint16_t yend, uint16_t offset, uint16_t color, uint16_t size, uint8_t chr, uint8_t mode)
{
    uint8_t temp;
    uint16_t t1, t;
    uint16_t tempoff;
    uint16_t y0 = y;
    uint16_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);    /* 得到字体一个字符对应点阵集所占的字节数 */

    if (chr == ' ')chr = 0;
    else if (chr == '.')chr = 1;
    else if (chr >= '0')chr = chr - '0' + 2; /* 如果是0~9,则减去'0'+2 */

    if (chr > 11)return;    /* 只有12个字符 */

    for (t = 0; t < csize; t++)
    {
        switch (size)
        {
            case 48:
                temp = dig7_4824[chr][t];   /* 调用1206字体 */
                break;

            case 64:
                temp = dig7_6432[chr][t];   /* 调用1608字体 */
                break;

            case 96:
                temp = dig7_9648[chr][t];   /* 调用2412字体 */
                break;

            default:/* 不支持的字体 */
                return;
        }

        tempoff = offset;

        if (x > xend)return;    /* 超区域了 */

        if (tempoff == 0)       /* 偏移地址到了 */
        {
            for (t1 = 0; t1 < 8; t1++)
            {
                if (y <= yend)
                {
                    if (temp & 0x80)
                    {
                        if (mode == 0x02)gui_draw_bigpoint(x, y, color);
                        else gui_phy.draw_point(x, y, color);
                    }
                    else if (mode == 0)gui_phy.draw_point(x, y, gui_phy.back_color);
                }

                temp <<= 1;
                y++;

                if ((y - y0) == size)
                {
                    y = y0;
                    x++;
                    break;
                }
            }
        }
        else
        {
            y += 8;

            if ((y - y0) >= size)   /* 大于一个字的高度了 */
            {
                y = y0; /* y坐标归零 */
                tempoff--;
            }
        }
    }
}

/**
 * @brief       显示len个数字
 * @param       x,y             : 起始坐标
 * @param       len             : 数字的位数
 * @param       color           : 颜色
 * @param       size            : 字体大小
 * @param       num             : 数值(0~2^64)
 * @param       mode            : 模式
 *                                [7]:0,不填充;1,填充0.
 *                                [3:0]:0,非叠加显示;  1,叠加显示;  2,大点叠加(只适用于叠加模式)
 * @retval      无
 */
void vmeter_show_num(uint16_t x, uint16_t y, uint8_t len, uint16_t color, uint8_t size, long long num, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / gui_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)vmeter_show_7seg(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, '0', mode & 0xf); /* 填充0 */
                else vmeter_show_7seg(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, ' ', mode & 0xf);

                continue;
            }
            else enshow = 1;

        }

        vmeter_show_7seg(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, temp + '0', mode & 0xf);
    }
}

/**
 * @brief       显示电压值,末尾是0则不显示
 * @param       x,y             : 起始坐标
 * @param       color           : 颜色
 * @param       size            : 字体大小
 * @param       res             : 电压值
 * @retval      无
 */
//void vmeter_show_vol(uint16_t x, uint16_t y, uint16_t color, uint8_t size, float res)
//{
//    uint8_t voli = 0;   /* 整数部分 */
//    uint16_t volf = 0;  /* 小数部分 */
//    uint16_t offx = 0;
//    uint8_t i = 0;

//    voli = res;         /* 取得整数部分 */
//    volf = (res - voli) * 1000; /* 取得小数部分 */

//    if (volf == 0)
//    {
//        offx = size * 2;    /* 只显示一个整数 */
//        vmeter_show_num(x, y, 5, color, size, voli, 0);
//    }
//    else
//    {
//        while (i < 3)
//        {
//            if ((volf % 10) == 0)
//            {
//                volf /= 10;
//                vmeter_show_7seg(x + offx, y, x + offx + size / 2, y + size, 0, color, size, ' ', 0); /* 清除之前的显示 */
//                offx += size / 2;
//            }
//            else break;

//            i++;
//        }

//        x = x + i * size / 2;
//        vmeter_show_num(x, y, 1, color, size, voli, 0); /* 显示整数部分 */
//        vmeter_show_7seg(x + size / 2, y, x + size, y + size, 0, color, size, '.', 0); /* 显示小数点 */
//        vmeter_show_num(x + size, y, (3 - i), color, size, volf, 1 << 7); /* 显示小数点 */
//    }
//}



/**
 * @brief       电压表测试
 * @param       无
 * @retval      未用到
 */
uint8_t vmeter_play(void)
{
    uint16_t hx, hy, vx, vy;    /* 16进制数据和电压值x,y坐标 */
    uint16_t wx, wy;            /* 主界面窗口x,y坐标 */
    uint8_t segsize = 0;
    uint8_t fontsize = 16;
    uint8_t res = 0;
    uint8_t wr = 0;
    uint8_t segyoff = 0;
    FIL *f_seg = 0;

    float vol;
    uint32_t adcx;
    uint32_t oldadcx = 0;
    uint8_t t = 0;
    uint32_t br;


    lcd_clear(LGRAY);
    app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02); /* 下分界线 */
    gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[16][gui_phy.language]); /* 显示标题 */

    if (lcddev.width <= 272)
    {
        segsize = 48;
        fontsize = 24;
        segyoff = 5;
        wr = 10;
    }
    else if (lcddev.width == 320)
    {
        segsize = 64;
        fontsize = 32;
        segyoff = 7;
        wr = 13;
    }
    else if (lcddev.width >= 480)
    {
        segsize = 96;
        fontsize = 36;
        segyoff = 14;
        wr = 20;
        f_seg = (FIL *)gui_memin_malloc(sizeof(FIL)); /* 申请FILENFO内存 */
        res = f_open(f_seg, (const TCHAR *)APP_ASCII_3618, FA_READ); /* 打开文件 */

        if (res == FR_OK)
        {
            asc2_3618 = (uint8_t *)gui_memex_malloc(f_seg->obj.objsize);    /* 为大字体开辟缓存地址 */

            if (asc2_3618)res = f_read(f_seg, asc2_3618, f_seg->obj.objsize, (UINT *)&br);  /* 一次读取整个文件 */
            else res = 0XFF;
        }
    }

    if (res == 0)
    {
        adc_init();
        wx = (lcddev.width - segsize * 2.5 - 4 * fontsize) / 2;
        wy = (lcddev.height - gui_phy.tbheight - segsize * 3 - fontsize / 2) / 2 + gui_phy.tbheight;
        hx = wx + 5 * fontsize / 2;
        hy = wy + segsize / 2;
        vx = hx;
        vy = hy + segsize + fontsize / 2;
        gui_draw_arcrectangle(wx, wy, segsize * 2.5 + fontsize * 4, 3 * segsize + fontsize / 2, wr, 1, BLACK, BLACK);
        gui_show_string("ADC", wx + fontsize / 2, hy + segsize - fontsize - segyoff, 1.5 * fontsize, fontsize, fontsize, WHITE);            /* 显示ADC */
        gui_show_string("D", hx + segsize * 2.5 + fontsize / 2, hy + segsize - fontsize - segyoff, fontsize, fontsize, fontsize, WHITE);    /* 显示H */
        gui_show_string("Uin", wx + fontsize / 2, vy + segsize - fontsize - segyoff, 1.5 * fontsize, fontsize, fontsize, WHITE);            /* 显示ADC */
        gui_show_string("V", hx + segsize * 2.5 + fontsize / 2, vy + segsize - fontsize - segyoff, fontsize, fontsize, fontsize, WHITE);    /* 显示H */

        while (1)
        {
            t++;

            if (t >= 10)
            {
                t = 0;
                OSSchedLock();      /* 禁止任务调度 */
                adcx = adc_get_result_average(ADC_ADCX_CHY, 10);  /* 获取通道5的转换值，10次取平均 */
                OSSchedUnlock();    /* 允许任务调度 */

                if (oldadcx != adcx)    /* 电压有变化 */
                {
                    oldadcx = adcx;
                    vol = (float)adcx * (3.3 / 65535);
                    vmeter_show_num(hx, hy, 5, RED, segsize, oldadcx, 0);   /* 显示原始数据 */
                    adcx = vol;         /* 取整数部分 */
                    vmeter_show_num(vx, vy, 1, RED, segsize, adcx, 0);      /* 显示整数部分 */
                    vmeter_show_7seg(vx + segsize / 2, vy, vx + segsize, vy + segsize, 0, RED, segsize, '.', 0); /* 显示小数点 */
                    vol = vol - adcx;
                    adcx = vol * 10000;

                    if ((adcx % 10) >= 5)adcx = adcx / 10 + 1; /* 四舍五入 */
                    else adcx = adcx / 10;

                    vmeter_show_num(vx + segsize, vy, 3, RED, segsize, adcx, 1 << 7); /* 显示小数部分 */
                }
            }
            else delay_ms(10);

            if (system_task_return)break;   /* TPAD返回 */
        }
    }

    gui_memin_free(f_seg);
    gui_memex_free(asc2_3618);
    ADC1->CR &= ~(1 << 0);      /* 关闭AD转换器 */
    return res;
}
























