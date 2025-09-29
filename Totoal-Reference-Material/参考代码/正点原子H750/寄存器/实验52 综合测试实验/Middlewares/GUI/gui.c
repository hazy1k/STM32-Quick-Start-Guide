/**
 ****************************************************************************************************
 * @file        gui.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-05-26
 * @brief       GUI-底层实现 代码
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
 * V1.1 20140218
 * 新增gui_draw_bline函数
 * V1.2 20140801
 * 1,修改gui_show_ptchar和gui_show_ptfont函数,支持24*24字体（数字与#、*，支持更大的字体）
 * 2,修改gui_show_ptchar和gui_show_ptfont函数的offset参数为uint16_t.
 * V1.3 20160309
 * 修正gui_show_ptchar显示大字体的bug,并新增7236,8844,14472等字体的支持
 * V1.4 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "guix.h"
#include "icos.h"
#include "./BSP/TOUCH/touch.h"
#include "./TEXT/text.h"



/* 此处必须在外部申明asc2_1206和asc2_1608; */
/* #include "font.h" */
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const unsigned char asc2_3216[95][128];

extern uint8_t *asc2_2814;	/* 普通字体28*14大字体点阵集 */
extern uint8_t *asc2_3618;	/* 普通字体36*18大字体点阵集 */
extern uint8_t *asc2_5427;	/* 普通字体54*27大字体点阵集 */
extern uint8_t *asc2_s6030;	/* 数码管字体60*30大字体点阵集 */
extern uint8_t *asc2_7236;	/* 普通字体72*36大字体点阵集 */
extern uint8_t *asc2_8844;	/* 普通字体88*44大字体点阵集 */
extern uint8_t *asc2_14472;	/* 普通144*72大字体点阵集 */



/* GUI通用字符串集 */
/* 确认 */
uint8_t *const GUI_OK_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "确定", "確定", "OK",
};

/* 选项 */
uint8_t *const GUI_OPTION_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "选项", "選項", "MENU",
};

/* 返回 */
uint8_t *const GUI_BACK_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "返回", "返回", "BACK",
};

/* 取消 */
uint8_t *const GUI_CANCEL_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "取消", "取消", "CANCEL",
};

/* 退出 */
uint8_t *const GUI_QUIT_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "退出", "退出", "QUIT",
};

/* 输入接口 */
_in_obj in_obj =
{
    gui_get_key,    /* 获取键值参数 */
    0,              /* 坐标 */
    0,
    0,              /* 按键键值 */
    IN_TYPE_ERR,    /* 输入类型,默认就是错误的状态 */
};
_gui_phy gui_phy;   /* gui物理层接口 */

/****************************************************************************************************/
/* 统一标准函数 */
extern uint32_t *ltdc_framebuf[2];      /* LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域 */


/**
 * @brief       填充颜色
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       color           : 颜色数组
 * @retval      无
 */
void gui_fill_color(uint16_t x, uint16_t y, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t width, height;
    width = ex - x + 1;
    height = ey - y + 1;
    lcd_color_fill(x, y, x + width - 1, y + height - 1, color); /* 其他情况,直接填充 */
}

/**
 * @brief       gui初始化
 * @param       无
 * @retval      无
 */
void gui_init(void)
{
    gui_phy.read_point = lcd_read_point;
    gui_phy.draw_point = lcd_draw_point;    /* 快速画点 */
    gui_phy.fill = lcd_fill;
    gui_phy.colorfill = gui_fill_color;         /* 使用piclib里面的piclib_fill_color函数来实现 */
    gui_phy.back_color = g_back_color;
    gui_phy.lcdwidth = lcddev.width;
    gui_phy.lcdheight = lcddev.height;

    /* 设置listbox/filelistbox参数 */
    if (lcddev.width == 240)
    {
        gui_phy.tbfsize = 16;
        gui_phy.tbheight = 20;
        gui_phy.listfsize = 12;
        gui_phy.listheight = 20;
    }
    else if (lcddev.width <= 320)
    {
        gui_phy.tbfsize = 16;
        gui_phy.tbheight = 24;
        gui_phy.listfsize = 16;
        gui_phy.listheight = 24;
    }
    else if (lcddev.width >= 480)
    {
        gui_phy.tbfsize = 24;
        gui_phy.tbheight = 32;
        gui_phy.listfsize = 24;
        gui_phy.listheight = 32;
    }
}

/**
 * @brief       获取键值参数
 * @param       obj             : 结构体指针
 * @param       type            : 输入设备类型
 * @retval      无
 */
void gui_get_key(void *obj, uint8_t type)
{
    _m_tp_dev *tp_dev;

    switch (type)
    {
        case IN_TYPE_TOUCH:
            in_obj.intype = IN_TYPE_TOUCH;
            tp_dev = (_m_tp_dev *)obj;
            in_obj.x = tp_dev->x[0];        /* 得到触屏坐标 */
            in_obj.y = tp_dev->y[0];

            if (tp_dev->sta & TP_PRES_DOWN)in_obj.ksta |= 0X01; /* 触摸被按下 */
            else in_obj.ksta &= ~(0X01);    /* 触摸松开了 */

            break;

        case IN_TYPE_KEY:
            in_obj.keyval = (uint32_t)obj;
            in_obj.intype = IN_TYPE_KEY;
            break;

        case IN_TYPE_JOYPAD:
            in_obj.intype = IN_TYPE_JOYPAD;

        //break;
        case IN_TYPE_MOUSE:
            in_obj.intype = IN_TYPE_MOUSE;

        //break;
        default:
            in_obj.intype = IN_TYPE_ERR;
            break;

    }
}

/**
 * @brief       颜色转换
 * @note        将32bit颜色转换成16bit颜色
 * @param       rgb             : 32位颜色
 * @retval      16位色
 */
uint16_t gui_color_chg(uint32_t rgb)
{
    uint16_t r, g, b;
    r = rgb >> 19;
    g = (rgb >> 10) & 0x3f;
    b = (rgb >> 3) & 0x1f;
    return ((r << 11) | (g << 5) | b);
}

/**
 * @brief       颜色转换
 * @note        将rgb332颜色转换成rgb565颜色
 * @param       rgb332          : RGB332颜色
 * @retval      RGB565颜色
 */
uint16_t gui_rgb332torgb565(uint16_t rgb332)
{
    uint16_t rgb565 = 0x18E7;
    rgb565 |= (rgb332 & 0x03) << 3;
    rgb565 |= (rgb332 & 0x1c) << 6;
    rgb565 |= (rgb332 & 0xe0) << 8;
    return rgb565;
}

/**
 * @brief       m^n函数
 * @param       m               : 底数
 * @param       n               : 指数
 * @retval      m^n结果
 */
long long gui_pow(uint8_t m, uint8_t n)
{
    long long result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * @brief       两个数的差的绝对值
 * @param       x1              : 参数1
 * @param       x2              : 参数2
 * @retval      |x1 - x2|
 */
uint32_t gui_disabs(uint32_t x1, uint32_t x2)
{
    return x1 > x2 ? x1 - x2 : x2 - x1;
}

/**
 * @brief       对指定区域进行alphablend
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       color           : alphablend的颜色
 * @param       aval            : 透明度(0~32)
 * @retval      无
 */
void gui_alphablend_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t aval)
{
    uint16_t i, j;
    uint16_t tempcolor;

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            tempcolor = gui_phy.read_point(x + i, y + j);
            tempcolor = gui_alpha_blend565(tempcolor, color, aval);
            gui_phy.draw_point(x + i, y + j, tempcolor);
        }
    }
}

/**
 * @brief       画大点函数
 * @note        以(x0,y0)为中心,画一个9个点的大点
 * @param       x0,y0           : 起始坐标
 * @param       color           : 颜色
 * @param       x2              : 参数2
 * @retval      无
 */
void gui_draw_bigpoint(uint16_t x0, uint16_t y0, uint16_t color)
{
    uint16_t i, j;
    uint16_t x, y;

    if (x0 >= 1)x = x0 - 1;
    else x = x0;

    if (y0 >= 1)y = y0 - 1;
    else y = y0;

    for (i = y; i < y0 + 2; i++)
    {
        for (j = x; j < x0 + 2; j++)gui_phy.draw_point(j, i, color);
    }
}

/**
 * @brief       画任意线
 * @param       x0,y0           : 起始坐标
 * @param       x1,y1           : 结束坐标
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x1 - x0;          /* 计算坐标增量 */
    delta_y = y1 - y0;
    uRow = x0;
    uCol = y0;

    if (delta_x > 0)incx = 1;   /* 设置单步方向 */
    else if (delta_x == 0)incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* 水平线 */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )        /* 画线输出 */
    {
        gui_phy.draw_point(uRow, uCol, color);  /* 画点 */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief       画一条粗线(方法0)
 * @param       x1,y1           : 起始坐标
 * @param       x2,y2           : 结束坐标
 * @param       size            : 线条的粗细程度
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    if (x1 < size || x2 < size || y1 < size || y2 < size)return;

    if (size == 0)
    {
        gui_phy.draw_point(x1, y1, color);
        return;
    }

    delta_x = x2 - x1; /* 计算坐标增量 */
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1; /* 设置单步方向 */
    else if (delta_x == 0)incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* 水平线 */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;      /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )            /* 画线输出 */
    {
        gui_fill_circle(uRow, uCol, size, color);   /* 画点 */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief       画一条粗线(方法1)
 * @param       x0,y0           : 起始坐标
 * @param       x1,y1           : 结束坐标
 * @param       size            : 线粗细,仅支持:0~2.
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_bline1(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x1 - x0; /* 计算坐标增量 */
    delta_y = y1 - y0;
    uRow = x0;
    uCol = y0;

    if (delta_x > 0)incx = 1; /* 设置单步方向 */
    else if (delta_x == 0)incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* 水平线 */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* 画线输出 */
    {
        if (size == 0)gui_phy.draw_point(uRow, uCol, color);/* 画点 */

        if (size == 1)
        {
            gui_phy.draw_point(uRow, uCol, color);          /* 画点 */
            gui_phy.draw_point(uRow + 1, uCol, color);      /* 画点 */
            gui_phy.draw_point(uRow, uCol + 1, color);      /* 画点 */
            gui_phy.draw_point(uRow + 1, uCol + 1, color);  /* 画点 */
        }

        if (size == 2)
        {
            gui_phy.draw_point(uRow, uCol, color);          /* 画点 */
            gui_phy.draw_point(uRow + 1, uCol, color);      /* 画点 */
            gui_phy.draw_point(uRow, uCol + 1, color);      /* 画点 */
            gui_phy.draw_point(uRow + 1, uCol + 1, color);  /* 画点 */
            gui_phy.draw_point(uRow - 1, uCol + 1, color);  /* 画点 */
            gui_phy.draw_point(uRow + 1, uCol - 1, color);  /* 画点 */
            gui_phy.draw_point(uRow - 1, uCol - 1, color);  /* 画点 */
            gui_phy.draw_point(uRow - 1, uCol, color);      /* 画点 */
            gui_phy.draw_point(uRow, uCol - 1, color);      /* 画点 */
        }

        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief       画垂直线
 * @param       x0,y0           : 起始坐标
 * @param       len             : 线长度
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_vline(uint16_t x0, uint16_t y0, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x0 > gui_phy.lcdwidth) || (y0 > gui_phy.lcdheight))return;

    gui_phy.fill(x0, y0, x0, y0 + len - 1, color);
}

/**
 * @brief       画水平线
 * @param       x0,y0           : 起始坐标
 * @param       len             : 线长度
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_hline(uint16_t x0, uint16_t y0, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x0 > gui_phy.lcdwidth) || (y0 > gui_phy.lcdheight))return;

    gui_phy.fill(x0, y0, x0 + len - 1, y0, color);
}

/**
 * @brief       填充一个色块
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       ctbl            : 颜色表,横向填充时width,纵向填充时大小为height
 * @param       mode            : 填充模式(纵向:从左到右,从上到下填充;横向:从上到下,从左到右填充)
 *              [7:1]:保留
 *              [bit0]:0,纵向;1,横向
 * @retval      无
 */
void gui_fill_colorblock(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t *ctbl, uint8_t mode)
{
    uint16_t i;

    if (height == 0 || width == 0)return;

    if (mode & 0x01)   /* 横向 */
    {
        for (i = 0; i < width; i++)
        {
            gui_draw_vline(x0 + i, y0, height - 1, ctbl[i]);
        }
    }
    else
    {
        for (i = 0; i < height; i++)
        {
            gui_draw_hline(x0, y0 + i, width - 1, ctbl[i]);
        }
    }
}

/**
 * @brief       将一种颜色平滑过渡到另外一种颜色
 * @param       srgb            : 起始颜色
 * @param       ergb            : 终止颜色
 * @param       cbuf            : 颜色缓存区(≥len)
 * @param       len             : 过渡级数(就是输出的颜色值总数)
 * @retval      无
 */
void gui_smooth_color(uint32_t srgb, uint32_t ergb, uint16_t *cbuf, uint16_t len)
{
    uint8_t sr, sg, sb;
    short dr, dg, db;
    uint16_t r, g, b;
    uint16_t i = 0;
    sr = srgb >> 16;
    sg = (srgb >> 8) & 0XFF;
    sb = srgb & 0XFF;
    dr = (ergb >> 16) - sr;
    dg = ((ergb >> 8) & 0XFF) - sg;
    db = (ergb & 0XFF) - sb;

    for (i = 0; i < len; i++)
    {
        r = sr + (dr * i) / len;
        g = sg + (dg * i) / len;
        b = sb + (db * i) / len;
        r >>= 3;
        g >>= 2;
        b >>= 3;
        cbuf[i] = ((r << 11) | (g << 5) | b);
    }
}

/**
 * @brief       画一条平滑过渡的彩色矩形(或线)
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       srgb            : 起始颜色
 * @param       ergb            : 终止颜色
 * @retval      无
 */
void gui_draw_smooth_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t srgb, uint32_t ergb)
{
    uint16_t i, j;
    uint16_t *colortbl = NULL;
    colortbl = (uint16_t *)gui_memin_malloc(width * 2); /* 分配内存 */

    if (colortbl == NULL)return ; /* 内存申请失败 */

    gui_smooth_color(srgb, ergb, colortbl, width); /* 获得颜色组 */

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            gui_phy.draw_point(x + i, y + j, colortbl[i]); /* 画点 */
        }
    }

    gui_memin_free(colortbl);
}

/**
 * @brief       画矩形
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color)
{
    gui_draw_hline(x0, y0, width, color);
    gui_draw_hline(x0, y0 + height - 1, width, color);
    gui_draw_vline(x0, y0, height, color);
    gui_draw_vline(x0 + width - 1, y0, height, color);
}

/**
 * @brief       画圆角矩形/填充圆角矩形
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       r               : 圆角的半径
 * @param       mode            : 0,画矩形框; 1,填充矩形;
 * @param       upcolor         : 上半部分颜色
 * @param       downcolor       : 下半部分颜色
 * @retval      无
 */
void gui_draw_arcrectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t mode, uint16_t upcolor, uint16_t downcolor)
{
    uint16_t btnxh = 0;

    if (height % 2)btnxh = height + 1; /* 基偶数处理 */
    else btnxh = height;

    if (mode)/* 填充 */
    {
        gui_fill_rectangle(x + r, y, width - 2 * r, btnxh / 2, upcolor);        /* 中上 */
        gui_fill_rectangle(x + r, y + btnxh / 2, width - 2 * r, btnxh / 2, downcolor);  /* 中下 */

        gui_fill_rectangle(x, y + r, r, btnxh / 2 - r, upcolor);                /* 左上 */
        gui_fill_rectangle(x, y + btnxh / 2, r, btnxh / 2 - r, downcolor);      /* 左下 */
        gui_fill_rectangle(x + width - r, y + r, r, btnxh / 2 - r, upcolor);    /* 右上 */
        gui_fill_rectangle(x + width - r, y + btnxh / 2, r, btnxh / 2 - r, downcolor);  /* 右下 */
    }
    else
    {
        gui_draw_hline (x + r, y, width - 2 * r, upcolor);                  /* 上 */
        gui_draw_hline (x + r, y + btnxh - 1, width - 2 * r, downcolor);    /* 下 */
        gui_draw_vline (x, y + r, btnxh / 2 - r, upcolor);                  /* 左上 */
        gui_draw_vline (x, y + btnxh / 2, btnxh / 2 - r, downcolor);        /* 左下 */
        gui_draw_vline (x + width - 1, y + r, btnxh / 2 - r, upcolor);  /* 右上 */
        gui_draw_vline (x + width - 1, y + btnxh / 2, btnxh / 2 - r, downcolor);        /* 右下 */
    }

    gui_draw_arc(x, y, x + r, y + r, x + r, y + r, r, upcolor, mode); /* 左上 */
    gui_draw_arc(x, y + btnxh - r, x + r, y + btnxh - 1, x + r, y + btnxh - r - 1, r, downcolor, mode); /* 左下 */
    gui_draw_arc(x + width - r, y, x + width, y + r, x + width - r - 1, y + r, r, upcolor, mode);	 /* 右上 */
    gui_draw_arc(x + width - r, y + btnxh - r, x + width, y + btnxh - 1, x + width - r - 1, y + btnxh - r - 1, r, downcolor, mode); /* 右下 */
}

/**
 * @brief       画ico
 * @param       x0,y0           : 起始坐标
 * @param       size            : ico图片尺寸(16/20/28)
 * @param       index           : icos编号
 * @retval      无
 */
void gui_draw_icos(uint16_t x, uint16_t y, uint8_t size, uint8_t index)
{
    uint16_t *colorbuf;
    uint16_t i = 0;
    uint16_t icosize;
    uint8_t *pico;
    icosize = size * size;
    colorbuf = gui_memin_malloc(2 * icosize); /* 申请内存 */

    if (colorbuf == NULL)return ;

    if (size == 16)pico = (uint8_t *)icostbl_16[index];
    else if (size == 20)pico = (uint8_t *)icostbl_20[index];
    else if (size == 28)pico = (uint8_t *)icostbl_28[index];

    if (colorbuf)
    {
        for (i = 0; i < icosize; i++)colorbuf[i] = gui_rgb332torgb565(pico[i]); /* RGB332转换为RGB565 */

        gui_phy.colorfill(x, y, x + size - 1, y + size - 1, colorbuf); /* 画出ICOS */
    }

    gui_memin_free(colorbuf);/* 释放内存 */

}

/**
 * @brief       画背景透明的ICO,仅限16*16大小的
 * @param       x0,y0           : 起始坐标
 * @param       size            : ico图片尺寸(16/20/28)
 * @param       index           : icos编号
 * @retval      无
 */
void gui_draw_icosalpha(uint16_t x, uint16_t y, uint8_t size, uint8_t index)
{
    uint16_t i, j;
    uint8_t *pico;
    uint16_t color;
    uint8_t alphabend;

    if (size == 16)pico = (uint8_t *)pathico_16[index];
    else if (size == 20)pico = (uint8_t *)pathico_20[index];
    else if (size == 28)pico = (uint8_t *)pathico_28[index];

    for (i = y; i < y + size; i++)
    {
        for (j = x; j < x + size; j++)
        {
            color = (*pico++) >> 3; /* B */
            color += ((uint16_t)(*pico++) << 3) & 0X07E0;   /* G */
            color += (((uint16_t) * pico++) << 8) & 0XF800; /* R */
            alphabend = *pico++;    /* ALPHA通道 */

            if (alphabend == 0)     /* 只对透明的颜色进行画图 */
            {
                if (color)gui_phy.draw_point(j, i, color);
            }
        }
    }
}

/**
 * @brief       填充矩形
 * @param       x0,y0           : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       color           : 颜色
 * @retval      无
 */
void gui_fill_rectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color)
{
    if (width == 0 || height == 0)return; /* 非法 */

    gui_phy.fill(x0, y0, x0 + width - 1, y0 + height - 1, color);
}

/**
 * @brief       画实心圆
 * @param       x0,y0           : 起始坐标
 * @param       r               : 半径
 * @param       color           : 颜色
 * @retval      无
 */
void gui_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t x = r;
    gui_draw_hline(x0 - r, y0, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + x * x) > sqmax)
        {
            /*  draw lines from outside */
            if (x > imax)
            {
                gui_draw_hline (x0 - i + 1, y0 + x, 2 * (i - 1), color);
                gui_draw_hline (x0 - i + 1, y0 - x, 2 * (i - 1), color);
            }

            x--;
        }

        /*  draw lines from inside (center) */
        gui_draw_hline(x0 - x, y0 + i, 2 * x, color);
        gui_draw_hline(x0 - x, y0 - i, 2 * x, color);
    }
}

/**
 * @brief       在区域内画点
 * @param       sx,sy           : 起始坐标
 * @param       ex,ey           : 结束坐标
 * @param       x,y             : 坐标
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_expoint(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t x, uint16_t y, uint16_t color)
{
    if (x <= ex && x >= sx && y <= ey && y >= sy)
    {
        gui_phy.draw_point(x, y, color);
    }
}

/**
 * @brief       画8点(Bresenham算法)
 * @param       sx,sy           : 起始坐标
 * @param       ex,ey           : 结束坐标
 * @param       rx,ry,a,b       : 参数
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_circle8(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t rx, uint16_t ry, int a, int b, uint16_t color)
{
    gui_draw_expoint(sx, sy, ex, ey, rx + a, ry - b, color);
    gui_draw_expoint(sx, sy, ex, ey, rx + b, ry - a, color);
    gui_draw_expoint(sx, sy, ex, ey, rx + b, ry + a, color);
    gui_draw_expoint(sx, sy, ex, ey, rx + a, ry + b, color);
    gui_draw_expoint(sx, sy, ex, ey, rx - a, ry + b, color);
    gui_draw_expoint(sx, sy, ex, ey, rx - b, ry + a, color);
    gui_draw_expoint(sx, sy, ex, ey, rx - b, ry - a, color);
    gui_draw_expoint(sx, sy, ex, ey, rx - a, ry - b, color);
}

/**
 * @brief       在指定位置画一个指定大小的圆
 * @param       sx,sy           : 起始坐标
 * @param       ex,ey           : 结束坐标
 * @param       rx,ry           : 圆心
 * @param       r               : 半径
 * @param       color           : 颜色
 * @param       mode            : 0,不填充; 1,填充;
 * @retval      无
 */
void gui_draw_arc(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t rx, uint16_t ry, uint16_t r, uint16_t color, uint8_t mode)
{
    int a, b, c;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);	/* 判断下个点位置的标志 */

    while (a <= b)
    {
        if (mode)for (c = a; c <= b; c++)gui_draw_circle8(sx, sy, ex, ey, rx, ry, a, c, color); /* 画实心圆 */
        else gui_draw_circle8(sx, sy, ex, ey, rx, ry, a, b, color); /* 画空心圆 */

        a++;

        /* 使用Bresenham算法画圆 */
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief       画椭圆
 * @param       x0,y0           : 坐标
 * @param       rx,ry           : x方向和y方向半径
 * @param       color           : 颜色
 * @retval      无
 */
void gui_draw_ellipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color)
{
    uint32_t OutConst, Sum, SumY;
    uint16_t x, y;
    uint16_t xOld;
    uint32_t _rx = rx;
    uint32_t _ry = ry;

    if (rx > x0 || ry > y0)return; /* 非法 */

    OutConst = _rx * _rx * _ry * _ry + (_rx * _rx * _ry >> 1); /*  Constant as explaint above */
    /*  To compensate for rounding */
    xOld = x = rx;

    for (y = 0; y <= ry; y++)
    {
        if (y == ry)x = 0;
        else
        {
            SumY = ((uint32_t)(rx * rx)) * ((uint32_t)(y * y)); /*  Does not change in loop */

            while (Sum = SumY + ((uint32_t)(ry * ry)) * ((uint32_t)(x * x)), (x > 0) && (Sum > OutConst)) x--;
        }

        /*  Since we draw lines, we can not draw on the first iteration */
        if (y)
        {
            gui_draw_line(x0 - xOld, y0 - y + 1, x0 - x, y0 - y, color);
            gui_draw_line(x0 - xOld, y0 + y - 1, x0 - x, y0 + y, color);
            gui_draw_line(x0 + xOld, y0 - y + 1, x0 + x, y0 - y, color);
            gui_draw_line(x0 + xOld, y0 + y - 1, x0 + x, y0 + y, color);
        }

        xOld = x;
    }
}

/**
 * @brief       填充椭圆
 * @param       x0,y0           : 坐标
 * @param       rx,ry           : x方向和y方向半径
 * @param       color           : 颜色
 * @retval      无
 */
void gui_fill_ellipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color)
{
    uint32_t OutConst, Sum, SumY;
    uint16_t x, y;
    uint32_t _rx = rx;
    uint32_t _ry = ry;
    OutConst = _rx * _rx * _ry * _ry + (_rx * _rx * _ry >> 1); /*  Constant as explaint above */
    /*  To compensate for rounding */
    x = rx;

    for (y = 0; y <= ry; y++)
    {
        SumY = ((uint32_t)(rx * rx)) * ((uint32_t)(y * y)); /*  Does not change in loop */

        while (Sum = SumY + ((uint32_t)(ry * ry)) * ((uint32_t)(x * x)), (x > 0) && (Sum > OutConst))x--;

        gui_draw_hline(x0 - x, y0 + y, 2 * x, color);

        if (y)gui_draw_hline(x0 - x, y0 - y, 2 * x, color);
    }
}

/**
 * @brief       快速ALPHA BLENDING算法
 * @param       src             : 源颜色
 * @param       dst             : 目标颜色
 * @param       alpha           : 透明程度(0~32)
 * @retval      混合后的颜色.
 */
uint16_t gui_alpha_blend565(uint16_t src, uint16_t dst, uint8_t alpha)
{
    uint32_t src2;
    uint32_t dst2;
    /* Convert to 32bit |-----GGGGGG-----RRRRR------BBBBB| */
    src2 = ((src << 16) | src) & 0x07E0F81F;
    dst2 = ((dst << 16) | dst) & 0x07E0F81F;
    /* Perform blending R:G:B with alpha in range 0..32 */
    /* Note that the reason that alpha may not exceed 32 is that there are only */
    /* 5bits of space between each R:G:B value, any higher value will overflow */
    /* into the next component and deliver ugly result. */
    dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
    return (dst2 >> 16) | dst2;
}

/**
 * @brief       在中间位置显示一个字符串
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       color           : 文字颜色.
 * @param       size            : 文字大小
 * @param       str             : 字符串
 * @retval      无
 */
void gui_show_strmid(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t size, uint8_t *str)
{
    uint16_t xoff = 0, yoff = 0;
    uint16_t strlenth;
    uint16_t strwidth;

    if (str == NULL)return;

    strlenth = strlen((const char *)str);	/* 得到字符串长度 */
    strwidth = strlenth * size / 2;		/* 字符串显示占用宽度 */

    if (height > size)yoff = (height - size) / 2;

    if (strwidth <= width)   /* 字符串没超过宽度 */
    {
        xoff = (width - strwidth) / 2;
    }

    gui_show_ptstr(x + xoff, y + yoff, x + width - 1, y + height - 1, 0, color, size, str, 1);
}

/**
 * @brief       在中间位置显示一个字符串
 * @param       x,y             : 起始坐标
 * @param       xend,yend       : 结束坐标
 * @param       offset          : 开始显示的偏移
 * @param       color           : 文字颜色.
 * @param       size            : 文字大小
 * @param       chr             : 字符
 * @param       mode            : 0,非叠加显示; 1,叠加显示; 2,大点叠加(只适用于叠加模式);
 * @retval      无
 */
void gui_show_ptchar(uint16_t x, uint16_t y, uint16_t xend, uint16_t yend, uint16_t offset, uint16_t color, uint16_t size, uint8_t chr, uint8_t mode)
{
    uint8_t temp;
    uint16_t t1, t;
    uint16_t tempoff;
    uint16_t y0 = y;
    uint16_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		/* 得到字体一个字符对应点阵集所占的字节数 */

    if (chr > ' ')chr = chr - ' '; /* 得到偏移后的值 */
    else chr = 0; /* 小于空格的一律用空格代替,比如TAB键(键值为9) */

    for (t = 0; t < csize; t++)
    {
        switch (size)
        {
            case 12:
                temp = asc2_1206[chr][t];           /* 调用1206字体 */
                break;

            case 16:
                temp = asc2_1608[chr][t];           /* 调用1608字体 */
                break;

            case 24:
                temp = asc2_2412[chr][t];           /* 调用2412字体 */
                break;

            case 28:
                if (asc2_2814 == NULL)return;       /* 非法的数组 */

                temp = asc2_2814[chr * csize + t];  /* 调用2814字体 */
                break;

            case 32:
                temp = asc2_3216[chr][t];           /* 调用3216字体 */
                break;

            case 36:
                if (asc2_3618 == NULL)return;       /* 非法的数组 */

                temp = asc2_3618[chr * csize + t];  /* 调用3618字体 */
                break;

            case 54:
                if (asc2_5427 == NULL)return;       /* 非法的数组 */
    
                temp = asc2_5427[chr * csize + t];  /* 调用5427字体 */
                break;

            case 60:
                if (asc2_s6030 == NULL)return;      /* 非法的数组 */

                temp = asc2_s6030[chr * csize + t]; /* 调用6030字体 */
                break;

            case 72:
                if (asc2_7236 == NULL)return;       /* 非法的数组 */

                temp = asc2_7236[chr * csize + t];  /* 调用6030字体 */
                break;

            case 88:
                if (asc2_8844 == NULL)return;       /* 非法的数组 */

                temp = asc2_8844[chr * csize + t];  /* 调用6030字体 */
                break;

            case 144:
                if (asc2_14472 == NULL)return;      /* 非法的数组 */

                temp = asc2_14472[chr * csize + t]; /* 调用6030字体 */
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
                y = y0;             /* y坐标归零 */
                tempoff--;
            }
        }
    }
}

/**
 * @brief       显示len个数字
 * @param       x,y             : 起始坐标
 * @param       len             : 数字的位数
 * @param       color           : 文字颜色.
 * @param       size            : 文字大小
 * @param       num             : 数值(0~2^64);
 * @param       mode            : 模式
 *              [7]:0,不填充; 1,填充0;
 *              [3:0]:0,非叠加显示; 1,叠加显示; 2,大点叠加(只适用于叠加模式);
 * @retval      无
 */
void gui_show_num(uint16_t x, uint16_t y, uint8_t len, uint16_t color, uint8_t size, long long num, uint8_t mode)
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
                if (mode & 0X80)gui_show_ptchar(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, '0', mode & 0xf); /* 填充0 */
                else gui_show_ptchar(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, ' ', mode & 0xf);

                continue;
            }
            else enshow = 1;

        }

        gui_show_ptchar(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, temp + '0', mode & 0xf);
    }
}

/**
 * @brief       将数字转为字符串
 * @param       str             : 字符串存放地址
 * @param       num             : 数字
 * @retval      无
 */
uint8_t *gui_num2str(uint8_t *str, uint32_t num)
{
    uint8_t t, temp;
    uint8_t enrec = 0;

    for (t = 0; t < 10; t++)
    {
        temp = (num / gui_pow(10, 10 - t - 1)) % 10;

        if (enrec == 0 && t < 9)
        {
            if (temp == 0)continue; /* 还不需要记录 */
            else enrec = 1;         /* 需要记录了 */
        }

        *str = temp + '0';
        str++;
    }

    *str = '\0'; /* 加入结束符 */
    return str;
}

/**
 * @brief       在指定地址开始显示一个汉字
 * @param       x,y             : 起始坐标
 * @param       xend,yend       : 结束坐标
 * @param       offset          : 开始显示的偏移
 * @param       color           : 文字颜色.
 * @param       size            : 文字大小
 * @param       chr             : 汉字内码(高字节在前,低字节在后)
 * @param       mode            : 0,非叠加显示; 1,叠加显示; 2,大点叠加(只适用于叠加模式);
 * @retval      无
 */
void gui_show_ptfont(uint16_t x, uint16_t y, uint16_t xend, uint16_t yend, uint16_t offset, uint16_t color, uint16_t size, uint8_t *chr, uint8_t mode)
{
    uint8_t temp;
    uint8_t t1, t;
    uint16_t tempoff;
    uint16_t y0 = y;
    uint8_t dzk[128];           /* 32*32字体,最大要128个字节作为点阵存储 */
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); /* 得到字体一个字符对应点阵集所占的字节数 */
    text_get_hz_mat(chr, dzk, size);    /* 得到相应大小的点阵数据 */
    tempoff = offset;

    for (t = 0; t < csize; t++)
    {
        if (x > xend)break; /* 到达终点坐标 */

        temp = dzk[t];      /* 得到点阵数据 */

        if (tempoff == 0)   /* 偏移地址到了 */
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
                y = y0;             /* y坐标归零 */
                tempoff--;
            }
        }
    }
}

/**
 * @brief       在指定位置开始,显示指定长度范围的字符串
 * @param       x,y             : 起始坐标
 * @param       xend            : x方向结束坐标
 * @param       offset          : 开始显示的偏移
 * @param       color           : 文字颜色.
 * @param       size            : 文字大小
 * @param       str             : 字符串
 * @param       mode            : 0,非叠加显示; 1,叠加显示; 2,大点叠加(只适用于叠加模式);
 * @retval      无
 */
void gui_show_ptstr(uint16_t x, uint16_t y, uint16_t xend, uint16_t yend, uint16_t offset, uint16_t color, uint8_t size, uint8_t *str, uint8_t mode)
{
    uint8_t bHz = 0;    /* 字符或者中文 */
    uint8_t EnDisp = 0; /* 使能显示 */

    while (*str != 0)   /* 数据未结束 */
    {
        if (!bHz)
        {
            if (*str > 0x80)bHz = 1;    /* 中文 */
            else    /* 字符 */
            {
                if (EnDisp == 0)        /* 还未使能显示 */
                {
                    if (offset >= size / 2) /* 超过了一个字符 */
                    {
                        offset -= size / 2; /* 减少一个字符的偏移 */
                    }
                    else    /* 未超过一个字符的偏移 */
                    {
                        offset = offset % (size / 2);   /* 得到字符偏移量 */
                        EnDisp = 1; /* 可以开始显示了 */
                    }
                }

                if (EnDisp == 1)    /* 使能显示 */
                {
                    gui_show_ptchar(x, y, xend, yend, offset, color, size, *str, mode); /* 显示一个字符 */

                    if ((xend - x) > size / 2)x += size / 2;    /* 字符,为全字的一半 */
                    else x += xend - x; /* 未完全显示 */

                    if (offset)
                    {
                        x -= offset;
                        offset = 0; /* 清除偏移 */
                    }
                }

                if (x >= xend)return; /* 超过了,退出 */

                str++;
            }
        }
        else     /* 中文 */
        {
            bHz = 0; /* 有汉字库 */

            if (EnDisp == 0)   /* 还未使能 */
            {
                if (offset >= size)   /* 超过了一个字 */
                {
                    offset -= size; /* 减少 */
                }
                else
                {
                    offset = offset % (size); /* 得到字符偏移 */
                    EnDisp = 1; /* 可以开始显示了 */
                }
            }

            if (EnDisp)
            {
                gui_show_ptfont(x, y, xend, yend, offset, color, size, str, mode); /* 显示这个汉字,空心显示 */

                if ((xend - x) > size)x += size; /* 字符,为全字的一半 */
                else x += xend - x; /* 未完全显示 */

                if (offset)
                {
                    x -= offset;
                    offset = 0; /* 清除偏移 */
                }
            }

            if (x >= xend)return; /* 超过了,退出 */

            str += 2;
        }
    }
}

/**
 * @brief       在指定位置开始,显示指定长度范围的字符串.(带边特效显示)
 * @param       x,y             : 起始坐标
 * @param       width           : 最大显示宽度
 * @param       offset          : 开始显示的偏移
 * @param       color           : 文字颜色.
 * @param       rimcolor        : 文字边的颜色
 * @param       size            : 文字大小
 * @param       str             : 字符串
 * @retval      无
 */
void gui_show_ptstrwhiterim(uint16_t x, uint16_t y, uint16_t xend, uint16_t yend, uint16_t offset, uint16_t color, uint16_t rimcolor, uint8_t size, uint8_t *str)
{
    gui_show_ptstr(x, y, xend, yend, offset, rimcolor, size, str, 2);   /* 先画边(用大号点画) */
    gui_show_ptstr(x, y, xend, yend, offset, color, size, str, 1);      /* 画字(用小点画) */
}

/**
 * @brief       把name加入到path后面.自动添加\和结束符.但是要确保pname的内存空间足够,否则可能导致死机.
 * @param       pname           : 带路径的文件名.path的内存大小至少为:strlen(path)+strlen(name)+2!
 * @param       path            : 路径
 * @param       name            : 名字
 * @retval      带路径的文件名
 */
uint8_t *gui_path_name(uint8_t *pname, uint8_t *path, uint8_t *name)
{
    const uint8_t chgchar[2] = {0X5C, 0X00};        /* 转义符 等效"\" */
    strcpy((char *)pname, (const char *)path);      /* 拷贝path到pname里面 */
    strcat((char *)pname, (const char *)chgchar);   /* 添加转义符 */
    strcat((char *)pname, (const char *)name);      /* 添加新增的名字 */
    return pname;
}

/**
 * @brief       得到字符串所占的行数
 * @param       str             : 字符串指针;
 * @param       linelenth       : 每行的长度
 * @param       font            : 字体大小
 * @retval      字符串所占的行数
 */
uint32_t gui_get_stringline(uint8_t *str, uint16_t linelenth, uint8_t font)
{
    uint16_t xpos = 0;
    uint32_t	lincnt = 1; /* 最少就是1行 */

    if (linelenth < font / 2)return 0XFFFFFFFF; /* 无法统计完成 */

    while (*str != '\0' && lincnt != 0xffffffff)
    {
        if ((*str == 0x0D && (*(str + 1) == 0X0A)) || (*str == 0X0A))   /* 是回车/换行符 */
        {
            if (*str == 0X0D)str += 2;
            else str += 1;

            lincnt++;/* 行数加1 */
            xpos = 0;
        }
        else if (*str >= 0X81 && (*(str + 1) >= 0X40))     /* 是gbk汉字 */
        {
            xpos += font;
            str += 2;

            if (xpos > linelenth)   /* 已经不在本行之内 */
            {
                xpos = font;
                lincnt++;/* 行数加1 */
            }
        }
        else     /* 是字符 */
        {
            xpos += font / 2;
            str += 1;

            if (xpos > linelenth)   /* 已经不在本行之内 */
            {
                xpos = font / 2;
                lincnt++;/* 行数加1 */
            }
        }
    }

    return lincnt;
}

/**
 * @brief       在设定区域内显示字符串,超过区域后不再显示
 * @param       str             : 字符串
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       font            : 字体大小
 * @param       fcolor          : 字体颜色,以叠加方式写字
 * @retval      无
 */
void gui_show_string(uint8_t *str, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font, uint16_t fcolor)
{
    uint16_t xpos = x;
    uint16_t ypos = y;
    uint16_t endx = x + width - 1;
    uint16_t endy = y + height - 1;

    if (width < font / 2)return ; /* 无法显示完成 */

    while (*str != '\0')   /* 未结束 */
    {
        if ((*str == 0x0D && (*(str + 1) == 0X0A)) || (*str == 0X0A))   /* 是回车/换行符 */
        {
            if (*str == 0X0D)str += 2;
            else str += 1;

            xpos = x;
            ypos += font; /* y增加16 */
        }
        else if (*str >= 0X81 && (*(str + 1) >= 0X40))     /* 是gbk汉字 */
        {
            if ((xpos + font) > (endx + 1))   /* 已经不在本行之内 */
            {
                xpos = x;
                ypos += font;
            }

            gui_show_ptfont(xpos, ypos, endx, endy, 0, fcolor, font, str, 1);
            xpos += font; /* 偏移 */
            str += 2;

        }
        else     /* 是字符 */
        {
            if ((xpos + font / 2) > (endx + 1))
            {
                xpos = x;
                ypos += font;
            }

            gui_show_ptchar(xpos, ypos, endx, endy, 0, fcolor, font, *str, 1);
            xpos += font / 2;
            str += 1;
        }

        if (ypos > endy)break; /* 超过了显示区域了 */
    }
}

/**
 * @brief       由此处开始向前寻找gbk码(即大于0x80的字节)的个数
 * @param       str             : 字符串指针
 * @param       pos             : 开始查找的地址
 * @retval      gbk码个数
 */
uint16_t gui_string_forwardgbk_count(uint8_t *str, uint16_t pos)
{
    uint16_t t = 0;

    while (str[pos] > 0x80)
    {
        t++;

        if (pos == 0)break;

        pos--;
    }

    return t;
}

/**
 * @brief       内存复制函数(因为用系统memset函数会导致莫名的死机,故全部换成这个函数)
 * @param       p               : 内存首地址
 * @param       c               : 要统一设置的值
 * @param       len             : 设置长度
 * @retval      无
 */
void gui_memset(void *p, uint8_t c, uint32_t len)
{
    uint8_t *pt = (uint8_t *)p;

    while (len)
    {
        *pt = c;
        pt++;
        len--;
    }
}

/**
 * @brief       内部内存分配
 * @param       size            : 要分配的内存大小(uint8_t为单位)
 * @retval      NULL:分配失败;  其他,内存地址;
 */
void *gui_memin_malloc(uint32_t size)
{
    return (void *)mymalloc(SRAMIN, size);
}

/**
 * @brief       内部内存释放
 * @param       ptr             : 要释放的内存首地址
 * @retval      无
 */
void gui_memin_free(void *ptr)
{
    if (ptr)myfree(SRAMIN, ptr);
}

/**
 * @brief       外部内存分配
 * @param       size            : 要分配的内存大小(uint8_t为单位)
 * @retval      NULL:分配失败;  其他,内存地址;
 */
void *gui_memex_malloc(uint32_t size)
{
    void *temp;
    temp = (void *)mymalloc(SRAMIN, size);
    return temp;
}


/**
 * @brief       外部内存释放
 * @param       ptr             : 要释放的内存首地址
 * @retval      无
 */
void gui_memex_free(void *ptr)
{
    if (ptr)myfree(SRAMIN, ptr);
}

/**
 * @brief       重新分配内存
 * @param       ptr             : 旧的内存首地址
 * @param       size            : 要申请的内存大小
 * @retval      新分配到的内存首地址.
 */
void *gui_memin_realloc(void *ptr, uint32_t size)
{
    return myrealloc(SRAMIN, ptr, size);
}





















