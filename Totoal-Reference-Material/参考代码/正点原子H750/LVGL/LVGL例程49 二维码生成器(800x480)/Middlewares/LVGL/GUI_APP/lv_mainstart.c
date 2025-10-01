/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL 二维码生成器 实验
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

lv_keybtm_t *ext = NULL;
lv_obj_t *ta;
lv_obj_t * lv_qr_win;
lv_obj_t * lv_qr_cont;
lv_obj_t * lv_qr_btn_win;
lv_obj_t * lv_qr_label;
lv_obj_t * lv_qr_c_label;
const char * data;

static void lv_lv_t9_btnm_def_action(lv_event_t *event);
void lv_ta_add_keyboard_handle(const char * txt,lv_obj_t *parent,lv_obj_t *ta);
static void lv_ta_add_text(lv_obj_t *ta, const char *txt);
static void lv_example_keyboard(void);
void lv_qr_windowm(void);


static const char* kb_map_t9[] = { "1#", "abc", "def",LV_SYMBOL_LEFT, "\n",
                                   "ghi", "jkl", "mno", LV_SYMBOL_OK, "\n",
                                   "pqrs", "tuv", "wxyz",LV_SYMBOL_CLOSE,"\n",
                                   LV_SYMBOL_PREV, " ", LV_SYMBOL_NEXT, "" };

/**
 * @brief  设置中间矩阵按键的汉字
 * @param  parent：父类对象
 * @return 无
 */
static void lv_set_matrix_chinese(lv_obj_t *parent)
{
    size_t btni, total_btn;
    uint32_t id, id_next;
    const char *pchs;
    size_t chsindex;

    if(ext->lv_keydata.chslist[ext->lv_keydata.pyindex])
    {
        pchs = ext->lv_keydata.chslist[ext->lv_keydata.pyindex];
        id = _lv_txt_encoded_get_byte_id(pchs, ext->lv_keydata.chsindex);
    }
    else
    {
        pchs = NULL;
        id = 0;
    }

    chsindex = ext->lv_keydata.chsindex;
    total_btn = sizeof(ext->lv_keydata.mapbuf) / sizeof(ext->lv_keydata.mapbuf[0]);

    for(btni = 0; btni < sizeof(ext->lv_keydata.mapbuf) / sizeof(ext->lv_keydata.mapbuf[0]); btni ++)
    {
        if((btni == total_btn - 3) && (ext->lv_keydata.chscnt > 16))
        {
            strcpy(ext->lv_keydata.mapbuf[btni], LV_SYMBOL_UP);
        }
        else if((btni == total_btn - 2) && (ext->lv_keydata.chscnt > 16))
        {
            strcpy(ext->lv_keydata.mapbuf[btni], LV_SYMBOL_DOWN);
        }
        else if(btni == total_btn - 1)
        {
            /* 最后一个结束btn */
            ext->lv_keydata.mapbuf[btni][0] = '\0';
        }
        else if((btni % 5) == 4)
        {
            /* 换行符 */
            ext->lv_keydata.mapbuf[btni][0] = '\n';
            ext->lv_keydata.mapbuf[btni][1] = '\0';
        }
        else if(chsindex < ext->lv_keydata.chscnt)
        {
            id_next = id;
            _lv_txt_encoded_next(pchs, &id_next);
            ext->lv_keydata.mapbuf[btni][0] = '\201';
            memcpy(&ext->lv_keydata.mapbuf[btni][1], &pchs[id], id_next - id);
            ext->lv_keydata.mapbuf[btni][id_next - id + 1] = '\0';
            id = id_next;
            chsindex ++;
        }
        else
        {
            ext->lv_keydata.map[btni] = ext->lv_keydata.mapbuf[btni];
            ext->lv_keydata.mapbuf[btni][0] = '\211';
            ext->lv_keydata.mapbuf[btni][1] = '\0';
        }
        ext->lv_keydata.map[btni] = ext->lv_keydata.mapbuf[btni];
    }

    lv_btnmatrix_set_map(ext->lv_select_btnm, ext->lv_keydata.map);
}

/**
 * @brief  拼音选择
 * @param  event：事件
 * @return 无
 */
static void lv_chskb_roller_py_action(lv_event_t *event)
{
    uint16_t id;
    lv_obj_t *spell;
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_CLICKED)
    {
        id = lv_roller_get_selected(obj);   /* 获取滚筒的选项ID */
        spell = lv_obj_get_parent(obj);     /* 获取滚筒的父类 */

        ext->lv_keydata.pyindex = id;       /* 设置汉字对应的pylist索引 */
        ext->lv_keydata.chsindex = 0;       /* 汉字在chslist[pyindex]中的起始索引 */
        ext->lv_keydata.chscnt = _lv_txt_get_encoded_length(ext->lv_keydata.chslist[id]); /* chslist[pyindex]中所有的汉字的个数 */

        lv_set_matrix_chinese(spell);       /* 设置矩阵中文 */
    }
}

/**
 * @brief  中间的键盘汉字选择
 * @param  event：事件
 * @return 无
 */
static void lv_chskb_chs_action(lv_event_t *event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t *hanzi_keyboard;
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);   /* 获取文本 */

        hanzi_keyboard = lv_obj_get_parent(obj);                 /* 获取父类 */

        if(strstr(txt, LV_SYMBOL_DOWN) != 0)                     /* 判断是否按下LV_SYMBOL_DOWN */
        {
            if(ext->lv_keydata.chsindex < ext->lv_keydata.chscnt - 14)
            {
                ext->lv_keydata.chsindex += 14;
                lv_set_matrix_chinese(hanzi_keyboard);           /* 设置矩阵中文 */
            }
            
            return;
        }
        else if(strstr(txt, LV_SYMBOL_UP) != 0)                  /* 判断是否按下LV_SYMBOL_UP */
        {
            if(ext->lv_keydata.chsindex >= 14)
            {
                ext->lv_keydata.chsindex -= 14;
                lv_set_matrix_chinese(hanzi_keyboard);           /* 设置矩阵中文 */
            }
            
            return;
        }
        else
        {
            /* 设置文本区域的增删改查 */
            lv_ta_add_text(ta, txt);
            /* 汉字选择完成以后，需要清空拼音和汉字选择区 */
            ext->t9inbuf[0] = '\0';
            lv_roller_set_options(ext->roller_py, "", LV_ROLLER_MODE_NORMAL); /* 设置选项为空 */
            lv_roller_set_selected(ext->roller_py, 0, LV_ANIM_OFF);           /* 设置选择索引为0 */
            memset(&ext->lv_keydata, 0, sizeof(ext->lv_keydata));             /* 清空数据 */
            lv_set_matrix_chinese(hanzi_keyboard);                            /* 设置矩阵中文 */
        }
    }
}

/**
 * @brief  创建T9输入法
 * @param  par：父类
 * @param  copy：复制属性
 * @return 无
 */
lv_obj_t * lv_chinese_keyboard_create(lv_obj_t * par, const lv_obj_t * copy)
{
    lv_obj_t *new_chskb = lv_obj_create(par);
    lv_obj_clear_flag(new_chskb,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(new_chskb,0,LV_STATE_DEFAULT);
    
    /* 初始化新的中文键盘 */
    if(copy == NULL)
    {
        ext->lv_t9_btnm = lv_btnmatrix_create(new_chskb);
        ext->roller_py = lv_roller_create(new_chskb);
        ext->lv_select_btnm = lv_btnmatrix_create(new_chskb);
        lv_obj_set_style_text_font(ext->roller_py,&myFont14,LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ext->lv_select_btnm,&myFont14,LV_STATE_DEFAULT);
        lv_obj_clear_flag(ext->lv_t9_btnm,LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(ext->roller_py,LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(ext->lv_select_btnm,LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ext->lv_t9_btnm,0,LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ext->roller_py,0,LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ext->lv_select_btnm,0,LV_STATE_DEFAULT);
      
        if(ext->lv_t9_btnm == NULL || ext->roller_py == NULL || ext->lv_select_btnm == NULL)
        {
            lv_obj_del(new_chskb);
        }

        lv_obj_set_size(new_chskb, lcddev.width, lcddev.height / 2);
        lv_obj_align_to(new_chskb, NULL, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_size(ext->lv_t9_btnm, lcddev.width, lcddev.height / 2);
        lv_obj_align_to(ext->lv_t9_btnm, NULL, LV_ALIGN_BOTTOM_MID, 0, 0);

        lv_obj_set_size(ext->roller_py, 100, lcddev.height / 2);
        lv_obj_align_to(ext->roller_py, NULL, LV_ALIGN_TOP_LEFT, -20, 0);


        lv_roller_set_options(ext->roller_py, "", LV_ROLLER_MODE_NORMAL);
        lv_obj_add_event_cb(ext->roller_py, lv_chskb_roller_py_action,LV_EVENT_ALL,NULL);

        lv_obj_set_size(ext->lv_select_btnm, lcddev.width/2 - lv_obj_get_width(ext->roller_py), lcddev.height/2);
        lv_obj_align_to(ext->lv_select_btnm, ext->roller_py, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

        lv_obj_add_event_cb(ext->lv_select_btnm, lv_chskb_chs_action,LV_EVENT_ALL,NULL);
        lv_obj_add_event_cb(ext->lv_t9_btnm, lv_lv_t9_btnm_def_action,LV_EVENT_ALL,NULL);
    }

    lv_obj_set_size(ext->lv_t9_btnm, lv_obj_get_width(new_chskb)/2, lv_obj_get_height(new_chskb));
    lv_obj_align_to(ext->lv_t9_btnm, new_chskb, LV_ALIGN_TOP_RIGHT, 20, 0);
    lv_btnmatrix_set_map(ext->lv_t9_btnm, kb_map_t9);
    lv_roller_set_options(ext->roller_py, "", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ext->roller_py, 0, false);
    memset(&ext->lv_keydata, 0, sizeof(ext->lv_keydata));
    lv_set_matrix_chinese(new_chskb);
}


/**
 * @brief  T9键盘的动作
 * @param  event：事件
 * @return 无
 */
static void lv_lv_t9_btnm_def_action(lv_event_t *event)
{
  
    char c;
    size_t i;
    size_t len;
    char options[200];

    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
  
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id); /* 获取文本 */
        lv_obj_t *lv_t9_parent = lv_obj_get_parent(obj);
        lv_ta_add_keyboard_handle(txt,lv_t9_parent,ta);        /* 按键对的操作文件区域 */
      
        if(ext->mode == LV_KEYBOARD_MODE_T9)                   /* T9键盘模式 */
        {
            len = strlen(ext->t9inbuf);                          /* 获取T9输入缓冲区大小 */

            if(strstr(txt, " ") != 0)                          /* 如果为空 */
            {
                lv_textarea_add_char(ta, ' ');                 /* 设置文本区域为空 */
            }
            else
            {
                if(strstr(txt, "abc") != 0)
                    c = '2';
                else if(strstr(txt, "def") != 0)
                    c = '3';
                else if(strstr(txt, "ghi") != 0)
                    c = '4';
                else if(strstr(txt, "jkl") != 0)
                    c = '5';
                else if(strstr(txt, "mno") != 0)
                    c = '6';
                else if(strstr(txt, "pqrs") != 0)
                    c = '7';
                else if(strstr(txt, "tuv") != 0)
                    c = '8';
                else if(strstr(txt, "wxyz") != 0)
                    c = '9';
                else
                    return;
                
                ext->t9inbuf[len++] = c;   /* 把c字符存到t9inbuf中 */
                ext->t9inbuf[len] = '\0';  /* 最后一位为空字符 */

                if(get_matched_py_count((uint8_t *)ext->t9inbuf) == 0) /* 匹配拼音 */
                {
                    ext->t9inbuf[len-1] = '\0'; /* 如果没有匹配成功减一位设置为空字符 */
                }
            }

            if(len == 0)
            {
                ext->lv_keydata.pyindex = 0;
                ext->lv_keydata.chsindex = 0;
                ext->lv_keydata.chscnt = 0;
                lv_roller_set_options(ext->roller_py, "", LV_ROLLER_MODE_NORMAL); /* 滚筒控制块设置选项 */
            }
            else
            {
                size_t cnt;
                size_t free;
                char *p;

                p = options;
                free = sizeof(options) - 1;
                *p = '\0';
                len = get_matched_pymb((uint8_t *)ext->t9inbuf, ext->lv_keydata.pylist, ext->lv_keydata.chslist);

                for(i = 0; i < len; i ++)
                {
                    cnt = strlen(ext->lv_keydata.pylist[i]);

                    if(cnt < free)
                    {
                        if(p != options)
                        {
                            *p ++ = '\n';
                            free --;
                        }

                        strcpy(p, ext->lv_keydata.pylist[i]);
                        p += cnt;
                        free -= cnt;
                        *p = '\0';
                    }
                }

                lv_roller_set_options(ext->roller_py, options, LV_ROLLER_MODE_NORMAL); /* 滚筒控制块设置选项 */

                if(len)
                {
                    ext->lv_keydata.pyindex = 0;
                    ext->lv_keydata.chscnt = _lv_txt_get_encoded_length(ext->lv_keydata.chslist[0]); /* 返回编码大小 */
                    ext->lv_keydata.chsindex = 0;
                }
                else
                {
                    ext->lv_keydata.pyindex = 0;
                    ext->lv_keydata.chsindex = 0;
                    ext->lv_keydata.chscnt = 0;
                }
            }

            lv_set_matrix_chinese(obj);
            lv_roller_set_selected(ext->roller_py, 0, LV_ANIM_OFF); /* 设置选中的选项 */
            return;
        }

    }
}


/**
 * @brief  按键对的操作文件区域
 * @param  txt：键盘的按键值
 * @return parent：键盘对象
 * @return ta：对文本的操作
 */
void lv_ta_add_keyboard_handle(const char * txt,lv_obj_t *parent,lv_obj_t *ta)
{
    if(strstr(txt, LV_SYMBOL_OK) != 0||strstr(txt, LV_SYMBOL_CLOSE) != 0) /* 按下OK键或者CLOSE */
    {
        lv_obj_del(parent);                   /* 删除T9键盘 */
        ext->lv_select_btnm = NULL;           /* 选择文本键盘控制块 */
        ext->lv_t9_btnm = NULL;               /* T9键盘控制块 */
        ext->roller_py = NULL;                /* 选择拼音控制块 */
        ext->mode = LV_KEYBOARD_MODE_T9;      /* 设置键盘的模式 */
        return;
    }
    else if(strstr(txt, LV_SYMBOL_PREV) != 0) /* 按下前一个 */
    {
        lv_textarea_cursor_left(ta);          /* 文本光标左移 */
        return;
    }
    else if(strstr(txt, LV_SYMBOL_NEXT) != 0) /* 按下一个 */
    {
        lv_textarea_cursor_right(ta);         /* 文本光标右移 */
        return;
    }
    else if(strstr(txt, LV_SYMBOL_LEFT) != 0) /* LV_SYMBOL_LEFT */
    {
        lv_textarea_del_char(ta);             /* 文本删除 */
        return;
    }
    else if (strstr(txt, "1#") != 0)
    {
        lv_obj_del(parent);                   /* 删除T9键盘 */
        ext->lv_select_btnm = NULL;           /* 选择文本键盘控制块 */
        ext->lv_t9_btnm = NULL;               /* T9键盘控制块 */
        ext->roller_py = NULL;                /* 选择拼音控制块 */
        lv_example_keyboard();                /* 创建键盘 */
    }
}

/**
 * @brief  键盘回调
 * @param  event：事件
 * @return 无
 */
void lv_keyboard_cb(lv_event_t *event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)
    {
        if (lv_keyboard_get_selected_btn ((lv_obj_t *)event->user_data) == 39)
        {
            lv_obj_del((lv_obj_t *)event->user_data); /* 删除键盘 */
        }
    }
}

/**
 * @brief  创建键盘
 * @param  无
 * @return 无
 */
static void lv_example_keyboard(void)
{
    lv_obj_t* keyboard = lv_keyboard_create(lv_scr_act());                      /* 定义并创建键盘 */
    lv_keyboard_set_textarea(keyboard, ta);
    lv_obj_add_event_cb(keyboard, lv_keyboard_cb, LV_EVENT_ALL, keyboard);      /* 设置文本框事件回调 */
}

/**
 * @brief  文本区域回调
 * @param  event：事件
 * @return 无
 */
void lv_ta_action_click(lv_event_t *event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)    /* 文本区域点击 */
    {
        if(ext->lv_t9_btnm == NULL && ext->mode == LV_KEYBOARD_MODE_T9) /* 判断T9是否为空 */
        {
            lv_chinese_keyboard_create(lv_scr_act(), NULL);             /* 创建T9，选择拼音矩阵，滚筒 */
        }
    }
}

/**
 * @brief  添加文本区域内容
 * @param  ta：文本区域对象
 * @param  txt：内容
 * @return 无
 */
static void lv_ta_add_text(lv_obj_t *ta, const char *txt)
{
    if(ta == NULL)
    {
        return;
    }

    lv_textarea_add_text(ta, txt); /* 添加文本 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_qr_windowm();
}

/**
 * @brief 退出回调函数
 * @param  event：事件
 * @return 无
 */
static void lv_qr_event_handler(lv_event_t * event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)    /*点击 */
    {
        lv_obj_del(lv_qr_win);
        lv_mainstart();
    }
}

/**
 * @brief 二维码生成回调函数
 * @param  event：事件
 * @return 无
 */
static void lv_btn_create_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    
    if (code == LV_EVENT_CLICKED)
    {
        lv_label_set_text(lv_qr_label,"Generated QR code successfully!");
        
        data = lv_textarea_get_text(ta);
        /* 创建一个lcddev.width/2的二维码 */
        lv_obj_t * qr = lv_qrcode_create(lv_qr_btn_win, lv_obj_get_width(lv_qr_btn_win) - 10, lv_color_hex3(0x33f), lv_color_hex3(0xeef));
        /* 设置数据 */
        lv_qrcode_update(qr, data, strlen(data));
        /* 二维码中间对齐 */
        lv_obj_center(qr);
        lv_obj_invalidate(qr);
    }
}

/**
 * @brief  二维码界面
 * @param  无
 * @return 无
 */
void lv_qr_windowm(void)
{
    lv_qr_win = lv_win_create(lv_scr_act(),myFont24.line_height);
    lv_obj_set_size(lv_qr_win,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_text_font(lv_qr_win,&lv_font_montserrat_14,LV_STATE_DEFAULT);
    lv_win_add_title(lv_qr_win, "QR V1.0");
    lv_obj_t * btn = lv_win_add_btn(lv_qr_win, LV_SYMBOL_CLOSE, 50);
    lv_obj_add_event_cb(btn, lv_qr_event_handler, LV_EVENT_CLICKED, NULL);
    
    lv_qr_cont = lv_win_get_content(lv_qr_win);
    
    ext->lv_select_btnm = NULL;                                    /* 选择文本键盘控制块 */
    ext->lv_t9_btnm = NULL;                                        /* T9键盘控制块 */
    ext->roller_py = NULL;                                         /* 选择拼音控制块 */
    ext->mode = LV_KEYBOARD_MODE_T9;                               /* 设置键盘的模式 */
    memset(ext->t9inbuf,0,sizeof(ext->t9inbuf));                   /* T9键盘输入缓冲区清零 */
  
    ta = lv_textarea_create(lv_qr_cont);                           /* 创建文本区域控件 */
    lv_obj_set_style_radius(ta, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(ta,&myFont14,LV_STATE_DEFAULT);     /* 设置文本字体 */
    lv_obj_set_style_border_color(ta,lv_color_make(0,0,0),LV_STATE_DEFAULT);
    lv_obj_set_size(ta, lv_obj_get_width(lv_scr_act())/2, lv_obj_get_height(lv_scr_act())/2);    /* 文本区域的大小 */
    lv_obj_align_to(ta, lv_qr_cont, LV_ALIGN_TOP_LEFT, 0, 30);     /* 对齐 */
    lv_textarea_set_cursor_click_pos(ta, true);                    /* 单击文本区域上的文本启用/禁用光标定位 */
    lv_obj_add_event_cb(ta, lv_ta_action_click,LV_EVENT_ALL,NULL); /* 设置回调函数 */
    lv_textarea_set_text(ta, " ");                                 /* 设置初始状态的文本为空 */
    
    lv_qr_btn_win = lv_obj_create(lv_qr_cont);
    lv_obj_set_style_border_color(lv_qr_btn_win,lv_color_make(0,0,0),LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_qr_btn_win, 0, LV_PART_MAIN);
    lv_obj_set_size(lv_qr_btn_win,lv_obj_get_width(ta)/2 + 40,lv_obj_get_width(ta)/2 + 40);
    lv_obj_invalidate(lv_qr_btn_win);
    lv_obj_align_to(lv_qr_btn_win, ta, LV_ALIGN_OUT_RIGHT_MID, lv_obj_get_width(ta)/4, 0);     /* 对齐 */
    
    lv_obj_t *lv_qr_btn = lv_btn_create(lv_qr_cont);
    lv_obj_set_style_bg_color(lv_qr_btn,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    lv_obj_align_to(lv_qr_btn, lv_qr_btn_win, LV_ALIGN_OUT_BOTTOM_LEFT, 0, lv_obj_get_height(lv_qr_btn_win)/6);     /* 对齐 */
    lv_obj_set_size(lv_qr_btn,lv_obj_get_width(lv_qr_btn_win),lv_obj_get_height(lv_qr_btn_win)/3);
    lv_obj_add_event_cb(lv_qr_btn,lv_btn_create_cb,LV_EVENT_ALL,NULL);
    
    lv_obj_t *lv_qr_btn_label = lv_label_create(lv_qr_btn);
    lv_obj_set_style_text_color(lv_qr_btn_label,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_qr_btn_label,&myFont14,LV_STATE_DEFAULT);     /* 设置文本字体 */
    lv_obj_clear_flag(lv_qr_btn_win,LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_text(lv_qr_btn_label,"Create Qr");
    lv_obj_center(lv_qr_btn_label);
    
    lv_qr_label = lv_label_create(lv_qr_cont);
    lv_obj_set_style_text_color(lv_qr_label,lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);
    lv_label_set_text(lv_qr_label,"");
    lv_obj_align_to(lv_qr_label, ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, lv_obj_get_height(ta)/6);            /* 对齐 */
    
    lv_qr_c_label = lv_label_create(lv_qr_cont);
    lv_obj_set_style_text_color(lv_qr_c_label,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    lv_label_set_text(lv_qr_c_label,"Alientek:http://www.openedv.com/");
    lv_obj_align_to(lv_qr_c_label, lv_qr_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, lv_obj_get_height(ta)/6); /* 对齐 */
}
