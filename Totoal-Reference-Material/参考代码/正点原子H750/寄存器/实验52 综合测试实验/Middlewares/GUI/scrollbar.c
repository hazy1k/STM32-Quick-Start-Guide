/**
 ****************************************************************************************************
 * @file        scrollbar.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       GUI-滚动条 代码
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
 * V1.0 20140722
 * 第一次发布
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "scrollbar.h"


/**
 * @brief       创建滚动条
 * @param       top,left        : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       type            : 按键类型
 *              [7]: 方向,0,水平;1,垂直;
 *              [6~2]: 保留;
 *              [1~0]: 按钮类型,0,标准滚动条;1,没有端按钮的滚动条;2,3,保留.
 * @retval      scrollbar指针(0,表示创建失败)
 */
_scrollbar_obj *scrollbar_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t type)
{
    _scrollbar_obj *scrollbar_crt;

    if ((type & 0x80) == SCROLLBAR_DIR_HOR && (width < 2 * SCROLLBAR_PART_LEN || height < SCROLLBAR_MIN_THICK))return NULL; /* 尺寸不能太小 */

    if ((type & 0x80) == SCROLLBAR_DIR_VER && (width < SCROLLBAR_MIN_THICK || height < 2 * SCROLLBAR_PART_LEN))return NULL; /* 尺寸不能太小 */

    scrollbar_crt = (_scrollbar_obj *)gui_memin_malloc(sizeof(_scrollbar_obj)); /* 分配内存 */

    if (scrollbar_crt == NULL)return NULL; /* 内存分配不够 */

    gui_memset((uint8_t *)scrollbar_crt, 0, sizeof(_scrollbar_obj)); /* 将scrollbar_crt的值全部设置为0 */
    scrollbar_crt->top = top;
    scrollbar_crt->left = left;
    scrollbar_crt->width = width;
    scrollbar_crt->height = height;
    scrollbar_crt->type = type;
    scrollbar_crt->sta = 0;
    scrollbar_crt->id = 0;
    scrollbar_crt->totalitems = 0;
    scrollbar_crt->itemsperpage = 0;
    scrollbar_crt->topitem = 0;
    scrollbar_crt->scbbarlen = 0;

    scrollbar_crt->inbkcolor = SCLB_DFT_INBKCOLOR;
    scrollbar_crt->btncolor = SCLB_DFT_BTNCOLOR;
    scrollbar_crt->rimcolor = SCLB_DFT_RIMCOLOR;
    return scrollbar_crt;
}

/**
 * @brief       删除滚动条
 * @param       scrollbar_del   : 要删除的滚动条指针
 * @retval      无
 */
void scrollbar_delete(_scrollbar_obj *scrollbar_del)
{
    gui_memin_free(scrollbar_del);
}

/**
 * @brief       检查scrollbar
 * @param       scbx            : 滚动条指针
 * @param       in_key          : 输入按键指针
 * @retval      未用到
 */
uint8_t scrollbar_check(_scrollbar_obj *scbx, void *in_key)
{
    _in_obj *key = (_in_obj *)in_key;
    uint8_t signx;
    uint16_t x, y, btnw, btnh, b1xoff, b1yoff, b2xoff, b2yoff;
    uint16_t alphacolor;
    uint32_t temp, temp1;
    uint16_t maxtopitem;/* 最大的顶部条目 */
    uint8_t  scbplen;

    if ((scbx->type & 0X03) == SCROLLBAR_TYPE_NBT)scbplen = 0; /* 无端按钮的情况 */
    else scbplen = SCROLLBAR_PART_LEN;      /* 两端有按钮 */

    if (scbx->totalitems >= scbx->itemsperpage)maxtopitem = scbx->totalitems - scbx->itemsperpage;
    else maxtopitem = 0;

    switch (key->intype)
    {
        case IN_TYPE_TOUCH:     /* 触摸屏按下了 */
            if ((scbx->type & 0x80) == SCROLLBAR_DIR_HOR) /* 水平滚动条 */
            {
                temp = key->x;  /* 当前x坐标 */
                temp1 = scbx->left + scbplen + scbx->scbbarlen / 2;	/* 滑动条起始位置 */

                x = scbx->left;                 /* 滑动条x坐标 */
                y = scbx->top;                  /* 滑动条y坐标 */
                btnw = scbplen;                 /* 按钮宽度 */
                btnh = scbx->height;            /* 按钮高度 */
                b1xoff = scbplen;               /* 第一个按钮的x方向偏移 */
                b1yoff = 0;                     /* 第一个按钮的y方向偏移 */
                b2xoff = scbx->width - scbplen; /* 第二个按钮的x方向偏移 */
                b2yoff = 0;                     /* 第二个按钮的y方向偏移 */
            }
            else
            {
                temp = key->y; /* 当前y坐标 */
                temp1 = scbx->top + scbplen + scbx->scbbarlen / 2; /* 滑动条起始位置 */

                x = scbx->left;                 /* 滑动条x坐标 */
                y = scbx->top;                  /* 滑动条y坐标 */
                btnw = scbx->width;             /* 按钮宽度 */
                btnh = scbplen;                 /* 按钮高度 */
                b1xoff = 0;                     /* 第一个按钮的x方向偏移 */
                b1yoff = scbplen;               /* 第一个按钮的y方向偏移 */
                b2xoff = 0;                     /* 第二个按钮的x方向偏移 */
                b2yoff = scbx->height - scbplen;/* 第二个按钮的y方向偏移 */
            }

            if ((scbx->type & 0x80) == SCROLLBAR_DIR_HOR)signx = SCROLLBAR_SIGN_LEFT; /* 水平 */
            else signx = SCROLLBAR_SIGN_UP; /* 水平 */

            /* 在最左/上端按钮内部 */
            if (y < key->y && key->y < (y + btnh) && x < key->x && key->x < (x + btnw))
            {
                if (scbx->topitem) /* 修改进度条的位置 */
                {
                    if ((scbx->sta & SCROLLBAR_BTN_LTP) == 0) /* 原来不是SCROLLBAR_BTN_LTP被按下 */
                    {
                        scbx->sta |= SCROLLBAR_BTN_LTP; /* 按下 */
                        alphacolor = gui_alpha_blend565(scbx->btncolor, GUI_COLOR_WHITE, 16);
                        /* 画按钮 */
                        scrollbar_draw_btn(x, y, btnw, btnh, alphacolor, scbx->rimcolor, SCROLLBAR_SIGN_PCOLOR, signx);
                    }

                    scbx->topitem--;
                    scrollbar_setpos(scbx);
                }
            }
            else if (scbx->sta & SCROLLBAR_BTN_LTP) /* 原来状态是按下的 */
            {
                scbx->sta &= ~SCROLLBAR_BTN_LTP;
                scrollbar_draw_btn(x, y, btnw, btnh, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, signx);
            }

            if ((scbx->type & 0x80) == SCROLLBAR_DIR_HOR)signx = SCROLLBAR_SIGN_RIGHT; /* 水平 */
            else signx = SCROLLBAR_SIGN_DOWN; /* 水平 */

            /* 在最右/下端按钮内部 */
            if ((y + b2yoff) < key->y && key->y < (y + scbx->height) && (x + b2xoff) < key->x && key->x < (x + scbx->width))
            {
                if (maxtopitem && scbx->topitem < maxtopitem) /* 修改进度条的位置 */
                {
                    if ((scbx->sta & SCROLLBAR_BTN_RBP) == 0) /* 原来不是SCROLLBAR_BTN_RBP被按下 */
                    {
                        scbx->sta |= SCROLLBAR_BTN_RBP; /* 按下 */
                        alphacolor = gui_alpha_blend565(scbx->btncolor, GUI_COLOR_WHITE, 16);
                        /* 画按钮 */
                        scrollbar_draw_btn(x + b2xoff, y + b2yoff, btnw, btnh, alphacolor, scbx->rimcolor, SCROLLBAR_SIGN_PCOLOR, signx);
                    }

                    scbx->topitem++;
                    scrollbar_setpos(scbx);
                }
            }
            else if (scbx->sta & SCROLLBAR_BTN_RBP) /* 原来状态是按下的 */
            {
                scbx->sta &= ~SCROLLBAR_BTN_RBP;
                scrollbar_draw_btn(x + b2xoff, y + b2yoff, btnw, btnh, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, signx);
            }

            /* 在中间 */
            if ((y + b1yoff) < key->y && key->y < (y + scbx->height - b1yoff) && (x + b1xoff) < key->x && key->x < (x + scbx->width - b1xoff))
            {
                if ((scbx->sta & SCROLLBAR_BTN_MID) == 0)scbx->sta = SCROLLBAR_BTN_MID; /* 原来不是SCROLLBAR_BTN_MID被按下 */

                if (temp >= temp1)temp -= temp1;
                else temp = 0;

                temp = (temp * maxtopitem) / (b2xoff + b2yoff - scbplen - scbx->scbbarlen);

                if (temp > maxtopitem)temp = maxtopitem;

                if (scbx->topitem != temp) /* 得到当前坐标 */
                {
                    scbx->topitem = temp;
                    scrollbar_setpos(scbx);
                }
            }
            else if (scbx->sta & SCROLLBAR_BTN_MID)scbx->sta &= ~SCROLLBAR_BTN_MID; /* 原来状态是按下的 */

            break;

        case IN_TYPE_KEY:   /* 按键数据 */
            break;

        case IN_TYPE_JOYPAD:/* 手柄数据 */
            break;

        case IN_TYPE_MOUSE: /* 鼠标数据 */
            break;

        default:
            break;
    }

    return 0;
}

/* 6个 8*8的两个标志图案 */
const uint8_t scrollsign[6][8] =
{
    {0x00, 0x00, 0x10, 0x38, 0x7C, 0xFE, 0x00, 0x00},   /* 上图标 */
    {0x00, 0x00, 0xFE, 0x7C, 0x38, 0x10, 0x00, 0x00},   /* 下图标 */
    {0x04, 0x0C, 0x1C, 0x3C, 0x1C, 0x0C, 0x04, 0x00},   /* 左图标 */
    {0x20, 0x30, 0x38, 0x3C, 0x38, 0x30, 0x20, 0x00},   /* 右图标 */
    {0x00, 0xFE, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0x00},   /* 垂直图标 */
    {0x00, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54},   /* 水平图标 */
};

/**
 * @brief       画符号
 * @param       x,y             : 坐标
 * @param       signcolor       : 符号颜色
 * @param       signx           : 符号编号(0,up;1,down;2,left;3,right;4,horsign;5,versign;)
 * @retval      无
 */
void scrollbar_draw_sign(uint16_t x, uint16_t y, uint16_t signcolor, uint8_t signx)
{
    uint8_t i, j;
    uint8_t *signbuf;
    uint8_t temp;
    signbuf = (uint8_t *)scrollsign[signx]; /* 得到图标像素阵列 */

    for (i = 0; i < 8; i++)
    {
        temp = signbuf[i];

        for (j = 0; j < 8; j++)
        {
            if (temp & 0x80)gui_phy.draw_point(x + j, y + i, signcolor);

            temp <<= 1;
        }
    }
}

/**
 * @brief       画滚动条的按键
 * @param       x,y             : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       bkcolor         : 背景色
 * @param       rimcolor        : 边框颜色
 * @param       signcolor       : 符号的颜色
 * @param       signx           : 符号编号(0,up;1,down;2,left;3,right;4,horsign;5,versign;)
 * @retval      无
 */
void scrollbar_draw_btn(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bkcolor, uint16_t rimcolor, uint16_t signcolor, uint8_t signx)
{
    gui_fill_rectangle(x, y, width, height, bkcolor); /* 填充内部颜色 */
    gui_draw_rectangle(x, y, width, height, rimcolor); /* 画外边框 */

    if (width >= 8 && height >= 8) /* 足够画图标 */
    {
        scrollbar_draw_sign(x + width / 2 - 4, y + height / 2 - 4, signcolor, signx);
    }
}

/**
 * @brief       设置滚动条的位置
 * @param       scbx            : 滚动条指针
 * @retval      无
 */
void scrollbar_setpos(_scrollbar_obj *scbx)
{
    uint16_t scbpos;
    uint16_t scblen;
    uint16_t x, y, xlen, ylen;
    uint8_t scbtype;
    uint8_t scbplen;

    if ((scbx->type & 0X03) == SCROLLBAR_TYPE_NBT)scbplen = 0; /* 两端没有按钮 */
    else scbplen = SCROLLBAR_PART_LEN;  /* 两端有按钮 */

    if ((scbx->type & 0x80) == SCROLLBAR_DIR_VER) /* 垂直滚动条 */
    {
        scblen = scbx->height - 2 * scbplen - scbx->scbbarlen; /* 得到滚动条滚动空间的长度 */
        x = scbx->left + 1;
        y = scbx->top + scbplen + 1;
        xlen = scbx->width - 2;
        ylen = scblen + scbx->scbbarlen - 2;
    }
    else   /* 水平滚动条 */
    {
        scblen = scbx->width - 2 * scbplen - scbx->scbbarlen;
        x = scbx->left + scbplen + 1;
        y = scbx->top + 1;
        xlen = scblen + scbx->scbbarlen - 2;
        ylen = scbx->height - 2;
    }

    gui_fill_rectangle(x, y, xlen, ylen, scbx->inbkcolor);  /* 清空上次的位置 */

    if (scbx->totalitems <= scbx->itemsperpage) /* 总条目不够一页显示的 */
    {
        scbpos = scblen;
    }
    else if (scbx->topitem >= (scbx->totalitems - scbx->itemsperpage)) /* 当前选择项大于等于最后一页的第一项 */
    {
        scbx->topitem = scbx->totalitems - scbx->itemsperpage; /* 范围限定在最后一页第一项 */
        scbpos = scblen;
    }
    else    /* 不是最后一页 */
    {
        scbpos = (scbx->topitem * scblen) / (scbx->totalitems - scbx->itemsperpage); /* 计算当前滚动条的位置 */
    }

    if ((scbx->type & 0x80) == SCROLLBAR_DIR_VER) /* 垂直滚动条 */
    {
        x = scbx->left;
        y = scbx->top + scbplen + scbpos;
        xlen = scbx->width;
        ylen = scbx->scbbarlen;
        scbtype = SCROLLBAR_SIGN_HOR;
    }
    else /* 水平滚动条 */
    {
        x = scbx->left + scbplen + scbpos;
        y = scbx->top;
        xlen = scbx->scbbarlen;
        ylen = scbx->height;
        scbtype = SCROLLBAR_SIGN_VER;
    }

    scrollbar_draw_btn(x, y, xlen, ylen, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, scbtype); /* 重画滚动条 */
    scrollbar_hook(scbx);/* 执行钩子函数 */
}

/**
 * @brief       画滚动条
 * @param       scbx            : 滚动条指针
 * @retval      无
 */
void scrollbar_draw_scrollbar(_scrollbar_obj *scbx)
{
    uint32_t scbactlen = 0; /* 滚动条可以滚动的空间长度 */

    if (scbx == NULL)return; /* 无效,直接退出 */

    gui_fill_rectangle(scbx->left, scbx->top, scbx->width, scbx->height, scbx->inbkcolor);
    gui_draw_rectangle(scbx->left, scbx->top, scbx->width, scbx->height, scbx->rimcolor);

    if ((scbx->type & 0X03) == 0) /* 标准滚动条 */
    {
        if ((scbx->type & 0x80) == 0) /* 水平滚动条 */
        {
            scrollbar_draw_btn(scbx->left, scbx->top, SCROLLBAR_PART_LEN, scbx->height, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, SCROLLBAR_SIGN_LEFT);
            scrollbar_draw_btn(scbx->left + scbx->width - SCROLLBAR_PART_LEN, scbx->top, SCROLLBAR_PART_LEN, scbx->height, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, SCROLLBAR_SIGN_RIGHT);
            scbactlen = scbx->width - 2 * SCROLLBAR_PART_LEN; /* 可供 滚动条滚动的空间范围 */
        }
        else  /* 垂直滚动条 */
        {
            scrollbar_draw_btn(scbx->left, scbx->top, scbx->width, SCROLLBAR_PART_LEN, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, SCROLLBAR_SIGN_UP);
            scrollbar_draw_btn(scbx->left, scbx->top + scbx->height - SCROLLBAR_PART_LEN, scbx->width, SCROLLBAR_PART_LEN, scbx->btncolor, scbx->rimcolor, SCROLLBAR_SIGN_RCOLOR, SCROLLBAR_SIGN_DOWN);
            scbactlen = scbx->height - 2 * SCROLLBAR_PART_LEN; /* 可供 滚动条滚动的空间范围 */
        }
    }
    else /* 没有两端按钮的滚动条 */
    {
        if ((scbx->type & 0x80) == 0)scbactlen = scbx->width; /* 水平滚动条 */
        else scbactlen = scbx->height; /* 垂直滚动条 */
    }

    if (scbx->itemsperpage > scbx->totalitems)scbx->scbbarlen = scbactlen;
    else scbx->scbbarlen = (scbactlen * scbx->itemsperpage) / scbx->totalitems; /* 得到滚动条的长度 */

    scrollbar_setpos(scbx);
}

/**
 * @brief       滚动条的钩子函数
 * @param       scbx            : 滚动条指针
 * @retval      无
 */
void scrollbar_hook(_scrollbar_obj *scbx)
{
    //printf("scrollbar%d pos:%d\r\n",scbx->id,scbx->topitem);
}

/****************************************************************************************************/
/* 测试函数 */
void test_scb(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint16_t icur, uint16_t inbkcolor, uint16_t btncolor, uint16_t rimcolor)
{
    _scrollbar_obj *tscb;
    tscb = scrollbar_creat(y, x, width, height, type); /* 创建滚动条 */

    if (tscb == NULL)return; /* 创建失败 */

    tscb->totalitems = 50;
    tscb->itemsperpage = 8;
    tscb->topitem = icur;

    tscb->inbkcolor = inbkcolor;
    tscb->btncolor = btncolor;
    tscb->rimcolor = rimcolor;

    scrollbar_draw_scrollbar(tscb); /* 画按钮 */
    scrollbar_delete(tscb);         /* 删除按钮 */
}



















