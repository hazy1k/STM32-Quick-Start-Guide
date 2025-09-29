/**
 ****************************************************************************************************
 * @file        memo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-文本框 代码
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
 * V1.1 20140814
 * 1,修改memo_draw_memo函数,添加模式控制,实现从头显示/从尾显示
 * V1.2 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "memo.h"


/**
 * @brief       创建memo
 * @param       top,left        : 起始坐标
 * @param       width,height    : 宽度和高度
 * @param       id              : ID号
 * @param       type            : [7:1]:保留; [0]:0,不允许编辑;1,允许编辑;
 * @param       font            : 字体大小
 * @param       textlen         : 文本长度
 * @retval      memo 指针(0,表示创建失败)
 */
_memo_obj *memo_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t id, uint8_t type, uint8_t font, uint32_t textlen)
{
    _memo_obj *memo_crt;
    uint8_t sta = 0;

    if (height <= SCROLLBAR_MIN_THICK * 2)return NULL;      /* 高度小了,不够画scrollbar */

    if (width < SCROLLBAR_PART_LEN + font / 2)return NULL;  /* 至少应该够scrollbar的宽度加上半个font/2的宽度 */

    memo_crt = (_memo_obj *)gui_memin_malloc(sizeof(_memo_obj));    /* 分配内存 */

    if (memo_crt == NULL)return NULL;               /* 内存分配不够 */

    gui_memset(memo_crt, 0, sizeof(_memo_obj));     /* 清零 */
    memo_crt->scbv = scrollbar_creat(left + width - MEMO_SCB_WIDTH, top, MEMO_SCB_WIDTH, height, 0x80); /* 创建scrollbar */

    if (memo_crt->scbv == NULL)sta = 1;
    else   /* 分配成功 */
    {
        memo_crt->scbv->itemsperpage = height / font;   /* 每页显示的条目数 */

        if ((height % font) > 2)memo_crt->scbv->itemsperpage++; /* 每页显示的条目增多1条 */
    }

    memo_crt->top = top;
    memo_crt->left = left;
    memo_crt->width = width;
    memo_crt->height = height;
    memo_crt->id = id;
    memo_crt->type = type;  /* 类型 */
    memo_crt->sta = 0;      /* 状态为0 */
    memo_crt->text = gui_memex_malloc(textlen);     /* 申请字符存储空间 */

    if (memo_crt->text == NULL)sta = 1;
    else gui_memset(memo_crt->text, 0, textlen);    /* 分配成功,则对内存清零 */

    memo_crt->offsettbl = gui_memex_malloc(4);      /* 申请4个字节 */

    if (memo_crt->offsettbl == NULL)sta = 1;
    else gui_memset(memo_crt->offsettbl, 0, 4);     /* 分配成功,则对内存清零 */

    if (sta)   /* 有分配不成功的情况 */
    {
        memo_delete(memo_crt);/* 删除 */
        return NULL;
    }

    memo_crt->textlen = textlen;
    memo_crt->font = font;  /* text文字字体 */

    memo_crt->lin = 0;      /* 行坐标 */
    memo_crt->col = 0;      /* 列坐标 */
    memo_crt->typos = 0;    /* 临时y坐标 */
    memo_crt->txpos = 0;    /* 临时x坐标 */
    memo_crt->memo_timer_old = 0;   /* 上一次时间 */

    memo_crt->textcolor = MEMO_DFT_TXTC;    /* 文字颜色 */
    memo_crt->textbkcolor = MEMO_DFT_TBKC;  /* 文字背景色 */
    return memo_crt;
}

/**
 * @brief       删除memo控件
 * @param       memo_del        : 要删除的memo指针
 * @retval      无
 */
void memo_delete(_memo_obj *memo_del)
{
    if (memo_del == NULL)return;        /* 非法的地址,直接退出 */

    gui_memex_free(memo_del->text);     /* 内存在外部申请的 */
    gui_memex_free(memo_del->offsettbl);/* 内存在外部申请的 */
    scrollbar_delete(memo_del->scbv);   /* 删除滚动条 */
    gui_memin_free(memo_del);
}

void memo_cursor_set(_memo_obj *memox, uint16_t x);

/**
 * @brief       检查memo控件
 * @param       memox           : memo指针
 * @param       in_key          : 输入按键指针
 * @retval      未用到
 */
uint8_t memo_check(_memo_obj *memox, void *in_key)
{
    _in_obj *key = (_in_obj *)in_key;
    uint16_t i;
    uint16_t temptopitem;
    uint16_t disitems = 1;
    uint16_t temp;
    static uint8_t scrollbarflag = 0;

    switch (key->intype)
    {
        case IN_TYPE_TOUCH:     /* 触摸屏按下了 */
            memo_cursor_flash(memox);   /* 只有允许编辑的时候才需要光标闪烁 */

            if (memox->top < key->y && key->y < (memox->top + memox->height) && (memox->left < key->x) && key->x < (memox->left + memox->width))   /* 在edit框内部 */
            {
                memox->sta |= 1 << 7;   /* 标记有按下事件 */

                if ((key->x < (memox->left + memox->width - memox->scbv->width)) && (scrollbarflag == 0))   /* 在文本框内,且之前不是scrollbar模式 */
                {
                    if (memox->sta & (1 << 6))
                    {
                        if (gui_disabs(key->y, memox->typos) > 10)   /* 偏移超过10个单位了 */
                        {
                            memox->sta |= 1 << 5;   /* 标记滑动 */

                            if (memox->scbv->totalitems > memox->scbv->itemsperpage)   /* 至少要大于一页的内容才能开始滚动 */
                            {
                                temptopitem = memox->scbv->topitem;
                                disitems = gui_disabs(memox->typos, key->y) / memox->font;

                                if (disitems == 0)disitems = 1; /* 最小单位为1; */

                                if (memox->typos > key->y)  /* topitem需要增加 */
                                {
                                    if (memox->scbv->topitem < (memox->scbv->totalitems - memox->scbv->itemsperpage))
                                    {
                                        temp = memox->scbv->totalitems - memox->scbv->itemsperpage - memox->scbv->topitem;

                                        if (temp > disitems)memox->scbv->topitem += disitems;
                                        else memox->scbv->topitem += temp;
                                    }
                                }
                                else     /* topitem需要减小 */
                                {
                                    if (memox->scbv->topitem > disitems)memox->scbv->topitem -= disitems;
                                    else memox->scbv->topitem = 0;
                                }

                                if (temptopitem != memox->scbv->topitem)   /* 防止每次都更新 */
                                {
                                    memo_draw_text(memox);  /* 重新显示文字 */
                                    scrollbar_draw_scrollbar(memox->scbv);  /* 重新画scrollbar */
                                }
                            }

                            memox->typos = key->y;  /* 记录新的位置 */
                        }
                    }
                    else
                    {
                        memox->sta |= 1 << 6;
                        memox->txpos = key->x; /* 保存上一次的坐标值 */
                        memox->typos = key->y;

                    }
                }
                else     /* 在滚动条内 */
                {
                    scrollbarflag = 1;  /* 标记在滚动条内 */
                    temptopitem = memox->scbv->topitem;
                    key->x = memox->scbv->left + 1; /* 强制让x坐标在scrollbar里面 */
                    scrollbar_check(memox->scbv, in_key); /* 滚动条检测 */

                    if (temptopitem != memox->scbv->topitem)   /* topitem位置发生了变化 */
                    {
                        memo_draw_text(memox);
                    }
                }
            }
            else if (memox->sta & (1 << 7))     /* 之前是有按下事件的 */
            {
                scrollbarflag = 0; /* 标记scrollbar模式结束 */

                if ((memox->sta & 0x60) == (1 << 6))   /* 是单击的 */
                {
                    for (i = 0; i < memox->scbv->itemsperpage; i++)   /* 得到lin-memox->scbv->topitem的编号 */
                    {
                        if (memox->typos <= (memox->top + (i + 1)*memox->font) && memox->typos >= (memox->top + i * memox->font))break; /* 在该行内 */
                    }

                    if (memox->scbv->topitem + i < memox->scbv->totalitems)   /* 不能超过总行数的长度 */
                    {
                        memo_show_cursor(memox, 0);             /* 清除前一次光标的位置 */
                        memox->lin = memox->scbv->topitem + i;  /* 得到当前的lin */
                        memo_cursor_set(memox, memox->txpos);
                    }
                }

                scrollbar_check(memox->scbv, in_key); /* 最后再检测scrollbar */
                memox->sta &= ~(7 << 5); /* 清除高三位 */
            }

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

/**
 * @brief       根据x坐标重设cursor的位置
 * @param       memox           : memo指针
 * @param       x               : x坐标
 * @retval      无
 */
void memo_cursor_set(_memo_obj *memox, uint16_t x)
{
    uint16_t newpos;
    uint16_t strlen;
    uint32_t i;
    uint16_t maxpos;
    uint8_t tchr;

    strlen = (memox->width - MEMO_SCB_WIDTH) / (memox->font / 2);

    for (i = 0; i < strlen; i++)
    {
        tchr = *(memox->text + memox->offsettbl[memox->lin] + i);

        if (tchr == 0 || tchr == 0x0D || tchr == 0X0A)break; /* 遇到结束符了 */
    }

    maxpos = i;

    if (x > memox->left + 3)
    {
        x -= memox->left + 3;
        newpos = x / (memox->font / 2);

        if ((x % (memox->font / 2)) > memox->font / 2)newpos += 1; /* 大于font/2,则认为在下一个位置 */

        if (newpos > maxpos)newpos = maxpos;    /* 不能超过text的长度 */

        if (newpos > 0)
        {
            maxpos = gui_string_forwardgbk_count(memox->text + memox->offsettbl[memox->lin], newpos - 1); /* 得到之前的汉字内码个数 */

            if ((maxpos % 2) != 0)newpos++;     /* 是在汉字中间,需要跳过一个字节 */
        }

        memox->col = newpos;
        memo_show_cursor(memox, 1); /* 重新显示新的光标 */
    }
}

/**
 * @brief       得到光标的当前x坐标
 * @param       memox           : memo指针
 * @retval      光标的x坐标
 */
uint16_t memo_get_cursorxpos(_memo_obj *memox)
{
    return memox->left + memox->col * (memox->font / 2); 	/* 光标的X坐标 */
}

/**
 * @brief       得到光标的当前y坐标
 * @param       memox           : memo指针
 * @retval      光标的y坐标
 */
uint16_t memo_get_cursorypos(_memo_obj *memox)
{
    return (memox->lin - memox->scbv->topitem) * memox->font + memox->top; /* 光标的Y坐标 */
}

/**
 * @brief       得到当前光标所在位置在text里面的位置
 * @param       memox           : memo指针
 * @retval      光标位置
 */
uint32_t memo_get_cursortextpos(_memo_obj *memox)
{
    return memox->offsettbl[memox->lin] + memox->col;
}

/**
 * @brief       画出光标
 * @param       memox           : memo指针
 * @param       color           : 光标颜色
 * @retval      无
 */
void memo_draw_cursor(_memo_obj *memox, uint16_t color)
{
    uint16_t x, y;
    uint8_t i;
    x = memo_get_cursorxpos(memox); /* 光标的X坐标 */
    y = memo_get_cursorypos(memox); /* 光标的Y坐标 */

    for (i = 0; i < memox->font; i++)
    {
        if (i > (memox->top + memox->height - y - 1))break; /* 剩余部分不需要显示 */

        gui_phy.draw_point(x, y + i, color);
        gui_phy.draw_point(x + 1, y + i, color);
    }
}

/**
 * @brief       显示光标
 * @param       memox           : memo指针
 * @param       sta             : 0,不显示出来;  1,显示出来;
 * @retval      无
 */
void memo_show_cursor(_memo_obj *memox, uint8_t sta)
{
    uint8_t txt[3];
    uint16_t color;
    uint32_t textpos;
    uint16_t y;

    if ((memox->type & 0x01) == 0)return; /* 不用显示光标 */

    if (sta == 0)
    {
        color = memox->textbkcolor; /* 不需要显示,用背景色 */
        memox->sta &= ~(1 << 1);    /* 设置状态为未显示 */
    }
    else
    {
        color = memox->textcolor;   /* 需要显示,用文字色 */
        memox->sta |= 1 << 1;       /* 设置状态为显示 */
    }

    if ((memox->lin < memox->scbv->topitem) || (memox->lin >= (memox->scbv->topitem + memox->scbv->itemsperpage)))return; /* 如果lin<topitem,直接就不显示了 */

    memo_draw_cursor(memox, color); /* 显示光标 */

    if (sta == 0)   /* 不要显示光标 */
    {
        textpos = memo_get_cursortextpos(memox);
        txt[0] = memox->text[textpos];

        if (txt[0] > 0X80)
        {
            txt[1] = memox->text[textpos + 1];
            txt[2] = '\0';
        }
        else
        {
            if (txt[0] < ' ')txt[0] = '\0';

            txt[1] = '\0';
        }

        if (txt[1] == '\0')y = memox->font / 2; /* 是字符 */
        else y = memox->font;   /* 是汉字 */

        if (memo_get_cursorxpos(memox) + y <= memox->left + memox->width - MEMO_SCB_WIDTH)   /* 只有不超过最左边的长度的时候,才允许显示 */
        {
            y = memo_get_cursorypos(memox); /* 得到y坐标 */
            gui_phy.back_color = memox->textbkcolor; /* 设置背景色 */

            if (y < memox->top + memox->height)gui_show_ptstr(memo_get_cursorxpos(memox), y, memox->left + memox->width - MEMO_SCB_WIDTH, memox->top + memox->height - 1, 0, memox->textcolor, memox->font, txt, 0);
        }
    }

}

/**
 * @brief       光标闪烁
 * @param       memox           : memo指针
 * @retval      无
 */
void memo_cursor_flash(_memo_obj *memox)
{
    if (gui_disabs(memox->memo_timer_old, GUI_TIMER_10MS) >= 50)   /* 超过ms了 */
    {
        memox->memo_timer_old = GUI_TIMER_10MS;

        if (memox->sta & 0x02)memo_show_cursor(memox, 0); /* 之前是显示出来了的 */
        else memo_show_cursor(memox, 1); /* 之前没有显示出来 */
    }
}

/**
 * @brief       显示文字串
 * @param       memox           : memo指针
 * @retval      无
 */
void memo_draw_text(_memo_obj *memox)
{
    gui_fill_rectangle(memox->left, memox->top, memox->width - MEMO_SCB_WIDTH, memox->height, memox->textbkcolor); /* 填充内部 */
    gui_show_string(memox->text + memox->offsettbl[memox->scbv->topitem], memox->left, memox->top, memox->width - MEMO_SCB_WIDTH, memox->height, memox->font, memox->textcolor);
    memo_show_cursor(memox, 1); /* 显示光标 */
}

/**
 * @brief       更新memo的信息
 * @param       memox           : memo指针
 * @param       curpos          : 光标在字符串里面的位置
 * @retval      0, 成功; 其他, 失败;
 */
uint8_t memo_info_update(_memo_obj *memox, uint32_t curpos)
{
    uint16_t xpos = 0;
    uint32_t temp = 0;
    uint8_t *str = memox->text;
    uint16_t linelenth = memox->width - MEMO_SCB_WIDTH; /* 每行长度 */
    uint32_t lincnt = 0;         /* 总行数 */
    
    lincnt = gui_get_stringline(memox->text, linelenth, memox->font);
    gui_memex_free(memox->offsettbl);   /* 释放之前的 */
    memox->offsettbl = gui_memex_malloc(lincnt * 4); /* 重新申请 */

    if (memox->offsettbl == NULL)return 1;

    memox->scbv->totalitems = lincnt;
    lincnt = 0;
    memox->offsettbl[0] = 0;

    /* 得到新的offset表 */
    while (*str != '\0')
    {
        if ((*str == 0x0D && (*(str + 1) == 0X0A)) || (*str == 0X0A))   /* 是回车/换行符 */
        {
            if (*str == 0X0D)
            {
                str += 2;
                temp += 2;
            }
            else
            {
                str += 1;
                temp += 1;
            }

            lincnt++;/* 行数加1 */
            memox->offsettbl[lincnt] = temp;
            xpos = 0;
        }
        else if (*str >= 0X81 && (*(str + 1) >= 0X40))     /* 是gbk汉字 */
        {
            xpos += memox->font;
            str += 2;
            temp += 2;

            if (xpos > linelenth)   /* 已经不在本行之内 */
            {
                xpos = memox->font;
                lincnt++;/* 行数加1 */
                memox->offsettbl[lincnt] = temp - 2;
            }
        }
        else     /* 是字符 */
        {
            xpos += memox->font / 2;
            str += 1;
            temp += 1;

            if (xpos > linelenth)   /* 已经不在本行之内 */
            {
                xpos = memox->font / 2;
                lincnt++;/* 行数加1 */
                memox->offsettbl[lincnt] = temp - 1;
            }
        }
    }

    if (memox->offsettbl[memox->scbv->topitem] > curpos)   /* 往上一层 */
    {
        while (memox->offsettbl[memox->scbv->topitem] > curpos)
        {
            if (memox->scbv->topitem)memox->scbv->topitem--;
            else
            {
                memox->lin = 0;
                memox->col = 0;
                return 0;
            }
        }

        memox->lin = memox->scbv->topitem; /* 更新行 */
        memox->col = curpos - memox->offsettbl[memox->scbv->topitem];
    }
    else
    {
        temp = memox->scbv->topitem;

        while (1)
        {
            temp++;

            if (temp == memox->scbv->totalitems)
            {
                temp--;
                break;
            }

            if (memox->offsettbl[temp] > curpos)
            {
                temp--;
                break;
            }
        }

        if ((temp - memox->scbv->topitem) >= memox->scbv->itemsperpage)
        {
            memox->scbv->topitem = temp - memox->scbv->itemsperpage + 1; /* 更新topitem */
        }
        else if (memox->scbv->totalitems < memox->scbv->itemsperpage)memox->scbv->topitem = 0;

        memox->lin = temp;
        memox->col = curpos - memox->offsettbl[temp];
    }

    return 0;
}

/**
 * @brief       在memox里面添加字符串
 * @param       memox           : memo指针
 * @param       str             : 字符串
 * @retval      无
 */
void memo_add_text(_memo_obj *memox, uint8_t *str)
{
    uint8_t *tempstr;
    uint32_t temp;
    uint32_t curpos = 0;
    uint8_t *laststr = '\0';

    if ((memox->type & 0x01) == 0)return; /* 不是可编辑的状态.不能添加内容 */

    if (*str != NULL)   /* 有内容 */
    {
        curpos = memo_get_cursortextpos(memox);
        tempstr = memox->text + curpos; /* 光标所在行的字符串开始位置 */

        if (*str != 0x08)   /* 不是退格 */
        {
            temp = strlen((const char *)str) + strlen((const char *)memox->text); /* 得到他们相加后的长度 */

            if (memox->textlen < temp)
            {
                return;/* 太长了.直接不执行操作 */
            }
        }

        if (*tempstr != '\0')   /* 不是结束符 */
        {
            temp = strlen((const char *)tempstr); /* 得到tempstr的长度 */
            laststr = gui_memex_malloc(temp + 1); /* 申请内存 */

            if (laststr == NULL)return; /* 申请失败,直接退出 */

            laststr[0] = '\0'; /* 结束符 */
            strcpy((char *)laststr, (const char *)tempstr); /* 复制tempstr的内容到laststr */
        }

        if (*str == 0x08)   /* 退格 */
        {
            if (curpos > 0)   /* 可以退格 */
            {
                curpos--;

                if (memox->text[curpos] > 0x80)   /* 是汉字,要退2格 */
                {
                    curpos--;
                }
                else if (curpos)     /* 还有字符 */
                {
                    if (memox->text[curpos] == 0X0A && memox->text[curpos - 1] == 0X0D)   /* 回车换行 */
                    {
                        curpos--;
                    }
                }

                memox->text[curpos] = '\0'; /* 添加结束符 */

                if (laststr)strcat((char *)memox->text, (const char *)laststr); /* 再将他们拼起来 */
            }
        }
        else     /* 是字符串 */
        {
            memox->text[curpos] = '\0'; /* 添加结束符 */
            strcat((char *)memox->text, (const char *)str); /* 凑起来 */
            curpos += strlen((const char *)str);            /* 光标偏移 */

            if (laststr)strcat((char *)memox->text, (const char *)laststr); /* 后续的再凑起来 */
        }

        gui_memex_free(laststr);/* 释放内存 */
        memo_info_update(memox, curpos);
        memo_draw_text(memox);
        scrollbar_draw_scrollbar(memox->scbv);
    }
}

/**
 * @brief       画memo
 * @param       memox           : memo指针
 * @param       mode            : 0,从头开始显示到末尾; 1,显示到最后;
 * @retval      无
 */
void memo_draw_memo(_memo_obj *memox, uint8_t mode)
{
    if (memox == NULL)return; /* 无效,直接退出 */

    gui_fill_rectangle(memox->left, memox->top, memox->width - MEMO_SCB_WIDTH, memox->height, MEMO_DFT_TBKC); /* 填充内部 */
    memo_info_update(memox, 0);

    if (memox->scbv->totalitems > memox->scbv->itemsperpage && mode)
    {
        memox->scbv->topitem = memox->scbv->totalitems - memox->scbv->itemsperpage;
    }

    memo_draw_text(memox);
    scrollbar_draw_scrollbar(memox->scbv);
}

/****************************************************************************************************/
/* 测试代码 */
void memo_test(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint8_t sta, uint16_t textlen)
{
    _memo_obj *tmemo;
    tmemo = memo_creat(x, y, width, height, 0, type, 16, textlen); /* 创建按钮 */

    if (tmemo == NULL)return; /* 创建失败 */

    //tmemo->sta=sta;
    //strcpy((char*)tmemo->text,(const char *)text);
    memo_draw_memo(tmemo, 0); /* 画按钮 */
    memo_delete(tmemo);/* 删除按钮 */
}




























