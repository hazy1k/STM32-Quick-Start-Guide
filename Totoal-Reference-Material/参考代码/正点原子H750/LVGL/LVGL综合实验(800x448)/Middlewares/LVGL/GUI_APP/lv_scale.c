/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       进制计算器
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

#include "LVGL/GUI_APP/lv_scale.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "math.h"


/* 数字&英文键盘 */
static const char* btnm1_map[24]={"1","2","3","\n",
                                  "4","5","6","\n",
                                  "7","8","9","\n",
                                  "0","A","B","\n",
                                  "C","D","E","\n",
                                  "F",LV_SYMBOL_CLOSE,LV_SYMBOL_OK,""};

#define LV_ATK_BIT_W                     25 /* 设置位的宽度 */
#define LV_ATK_BIT_H                     35 /* 设置位的高度 */
#define LV_ATK_BIT_INDEX_H               20 /* 设置索引的高度 */
#define LV_ATK_BIT_ROW_SPACE             22 /* 设置位与位的空间 */
#define LV_ATK_BIT_COUNT                 33 /* 设置位的数量 */
char lv_bit_flag[LV_ATK_BIT_COUNT];         /* 32位 */

typedef struct
{
    lv_obj_t *lv_scale_cont;                    /* 定义容器 */
    lv_obj_t *lv_btnmatrix;                     /* 定义一个键盘 */
    lv_obj_t *lv_bit_index[LV_ATK_BIT_COUNT];   /* 设置位的编号 */
    lv_obj_t *lv_bit_text[LV_ATK_BIT_COUNT];    /* 设置位的文本 */
    lv_obj_t *lv_bin;                           /* 二进制 */
    lv_obj_t *lv_dec;                           /* 十进制 */
    lv_obj_t *lv_hex;                           /* 十六进制 */
    lv_obj_t *lv_oct;                           /* 八进制 */
    lv_obj_t *lv_bin_label;                     /* 二进制标签 */
    lv_obj_t *lv_dec_label;                     /* 十进制标签 */
    lv_obj_t *lv_hex_label;                     /* 十六进制标签 */
    lv_obj_t *lv_oct_label;                     /* 八进制标签 */
    lv_obj_t *lv_middle;                        /* 中间转换表类 */
}lv_scale_t;

lv_scale_t lv_scale;


/**
 * @brief       二进制字符串转换为十进制整数
 * @param       str：待反转的字符串
 * @retval      反转字符串
 */
static void lv_atk_bit_str_reverse(char str[])
{
    int n=strlen(str);
    int i;
    char temp;
  
    for (i = 0;i < (n/2); i++)
    {
        temp = str[i];
        str[i] = str[n-i-1];
        str[n-i-1] = temp;
    }
}

/**
 * @brief       把二进制字符串转换为十进制
 * @param       pbin：二进制
 * @retval      二进制字符串转换为十进制整数的结果
 */
static long lv_atk_bit_bin_to_dec(const char *pbin)
{
    int ii=0;
    long result=0;

    while (pbin[ii] != 0)
    {
        result = result * 2 + (pbin[ii] - '0');
        ii++;
    }

    return result;
}

/**
 * @brief       查找按下的BIT位
 * @param       parent：父类
 * @retval      返回位数
 */
int lv_atk_bit_to_dec(lv_obj_t *parent)
{
    int  j = 0;

    for (int i = 0 ; i < LV_ATK_BIT_COUNT; i++)
    {

        if (parent == lv_scale.lv_bit_text[i])
        {
            return j;
        }
        
        j ++;
    }
}

/**
 * @brief       将十进制转换为八进制
 * @param       n：十进制
 * @retval      返回八进制
 */
long lv_atk_dec_to_oct(long dec)
{
    int oct = 0, i = 0;

    i = 1;

    while (dec != 0)
    {
        oct += (dec % 8) * i;
        dec /= 8;
        i *= 10;
    }

    return oct;
}

/**
 * @brief       十进制转二进制
 * @param       n：十进制
 * @retval      返回二进制
 */
long lv_atk_bec_to_bin(long n)
{

    long result=0,k=1,i,temp;
    temp = n;

    while(temp)
    {
        i = temp%2;
        result = k * i + result;
        k = k*10;
        temp = temp/2;
    }
    printf("%ld\n", result);
    
    return result;
}

/**
 * @brief       十六进制转十进制
 * @param       n：十六进制
 * @retval      返回十进制
 */
long lv_atk_hex_to_dex(char*s)
{
    int i,t;
    long sum=0;
  
    for(i=0;s[i];i++)
    { 
        if(s[i]>= '0'  &&s[i]<='9')/* 当字符是'0'--'9'时，*-‘0'就行了 */
        {
            t=s[i]-'0';
        }
        
        if(s[i]>='a'&&s[i]<='z')
        {
            t=s[i]-'a'+10;/* 当字符是 abcdef 时，*-‘a’+10 就行了 */
        }
        
        if(s[i]>='A'&&s[i]<='Z')
        {
            t=s[i]-'A'+10;/* 当字符是 ABCDEF 时，*-‘A’+10 就行了 */
        }
        
        sum=sum*16+t; 
    }
    
    return sum;
}

/**
 * @brief       八进制转十进制
 * @param       n：八进制的数值
 * @retval      返回十进制
 */
long lv_atk_oct_to_dex(long n)
{
    int i=0,tmp,sum=0;

    while(n)
    {
        tmp=n%10;
        n=n/10;
        sum+=tmp*pow(8,i);
        i++;
    }
    
    printf("%d",sum);
    
    return sum;
}


/*
 *  函数名：   static void event_handler_regbit_map(lv_obj_t * obj, lv_event_t event)
 *  输入参数： 触发事件的对象
 *  输入参数： 触发的事件类型
 *  返回值：   无
 *  函数作用： 位按键响应事件，定位到当前选中的位按钮
*/
static void lv_event_bit_map_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_CLICKED)
    {
        static char lv_buf[32];
        int lv_bit_post = 0;
        char lv_str_buf[32];
        char lv_oct_buf[32];
        char lv_bit_flag_buf[LV_ATK_BIT_COUNT - 1] = "00000000000000000000000000000000";
      
        if (lv_scale.lv_btnmatrix != NULL)
        {
            lv_obj_del(lv_scale.lv_btnmatrix); /* 删除键盘 */
            lv_textarea_set_cursor_click_pos(lv_scale.lv_middle, true);  /* 其他文本区域有效隐藏 */
            lv_scale.lv_middle = NULL;
            lv_scale.lv_btnmatrix = NULL;
        }
        
        lv_textarea_set_text(lv_scale.lv_bin, "");
        lv_textarea_set_text(lv_scale.lv_dec, "");
        lv_textarea_set_text(lv_scale.lv_hex, "");
        lv_textarea_set_text(lv_scale.lv_oct, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_bin, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_dec, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_hex, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_oct, "");
        lv_obj_t * lv_obj_child = lv_obj_get_child(obj, NULL);
        lv_bit_post = lv_atk_bit_to_dec(obj);

        lv_snprintf(lv_buf, sizeof(lv_buf), "%s",lv_label_get_text(lv_obj_child));

        if (strcmp(lv_buf, "1") == 0)
        {
            lv_label_set_text(lv_obj_child, "0");
            lv_bit_flag[lv_bit_post] = '0';
        }
        else if (strcmp(lv_buf, "0") == 0)
        {
            lv_label_set_text(lv_obj_child, "1");
            lv_bit_flag[lv_bit_post] = '1';
        }

        strcpy(lv_bit_flag_buf, lv_bit_flag);
        
        lv_atk_bit_str_reverse(lv_bit_flag_buf);
        
        lv_textarea_set_placeholder_text(lv_scale.lv_bin, lv_bit_flag_buf);
        const char * lv_show_lv_atk_str_buffer = lv_textarea_get_placeholder_text(lv_scale.lv_bin);                    /* 获取输入内容 */
        lv_snprintf(lv_str_buf, sizeof(lv_str_buf), "%d", lv_atk_bit_bin_to_dec(lv_show_lv_atk_str_buffer));  /* 二进制转十进制 */
        lv_textarea_set_placeholder_text(lv_scale.lv_dec, lv_str_buf);
        lv_snprintf(lv_oct_buf, sizeof(lv_oct_buf), "%d", lv_atk_dec_to_oct(atof((const char *)lv_str_buf)));
        lv_textarea_set_placeholder_text(lv_scale.lv_oct, lv_oct_buf);                                                 /* 十进制转八进制 */
        lv_snprintf(lv_str_buf, sizeof(lv_str_buf),"0x%X", lv_atk_bit_bin_to_dec(lv_show_lv_atk_str_buffer)); /* 二进制转十六进制 */
        lv_textarea_set_placeholder_text(lv_scale.lv_hex, lv_str_buf);
    }
}


/**
 * @brief  btnmatrix控件回调函数
 * @param  obj: 对象
 * @param  event: 事件  
 * @return 无
 */
static void lv_btnmatrix_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        const char * txt = lv_btnmatrix_get_btn_text(lv_scale.lv_btnmatrix,lv_btnmatrix_get_selected_btn(obj));
      
        if (txt == btnm1_map[22]||txt == btnm1_map[21])
        {
            lv_obj_del(lv_scale.lv_btnmatrix); /* 删除键盘 */
            lv_scale.lv_btnmatrix = NULL;
            lv_scale.lv_middle = NULL;
        }
        
        for (int i = 0 ;i < 20 ;i ++)
        {
            if (txt == btnm1_map[i])
            {
                lv_textarea_add_text(lv_scale.lv_middle,(const char *)txt);
                break;
            }
        }

    }
}

/**
 * @brief       创建进制界面
 * @param       ta：文本区域
 * @param       e：事件
 * @retval      无
 */
static void lv_ta_cb_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * ta = lv_event_get_target(event);
  
    if((code == LV_EVENT_CLICKED) && (lv_scale.lv_btnmatrix == NULL))
    {
        lv_scale.lv_middle = NULL;
        lv_textarea_set_cursor_click_pos(ta, false);               /* 失能游标隐藏 */
        lv_scale.lv_btnmatrix = lv_btnmatrix_create(lv_scr_act()); /* 创建矩阵按键 */
        lv_btnmatrix_set_map(lv_scale.lv_btnmatrix, btnm1_map);          /* 设置矩阵按键数组 */
        lv_btnmatrix_set_btn_width(lv_scale.lv_btnmatrix, 12, 1);        /* 设置按键的比例 */
        lv_obj_set_size(lv_scale.lv_btnmatrix,lcddev.width,lcddev.height/2);    /* 设置矩阵按键的大小 */
        lv_obj_align_to(lv_scale.lv_btnmatrix, NULL, LV_ALIGN_BOTTOM_MID, 0, 0);/* 设置对齐模式 */
        lv_obj_add_event_cb(lv_scale.lv_btnmatrix, lv_btnmatrix_event_handler,LV_EVENT_ALL,NULL); /* 设置矩阵按键的回调函数 */

      
        /* 清除文本区域开始 */
        lv_textarea_set_text(lv_scale.lv_bin, "");
        lv_textarea_set_text(lv_scale.lv_dec, "");
        lv_textarea_set_text(lv_scale.lv_hex, "");
        lv_textarea_set_text(lv_scale.lv_oct, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_bin, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_dec, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_hex, "");
        lv_textarea_set_placeholder_text(lv_scale.lv_oct, "");
        /* 清除文本区域结束 */
      
        if (ta == lv_scale.lv_bin) /* 按下是二进制的文本区域实现代码 */
        {
            for (int i = 0 ; i < 18 ; i++)
            {
              if (i == 0 || i == 9 || i == 16 || i == 17) /* 矩阵按键的索引号0，9，16，17不做处理 */
                {
                    continue;
                }
                else /* 其他索引做处理 */
                {
                    lv_btnmatrix_set_btn_ctrl(lv_scale.lv_btnmatrix, i, LV_BTNMATRIX_CTRL_DISABLED); /* 失能这些索引 */
                }
            }
            lv_scale.lv_middle = lv_scale.lv_bin; /* 中间对象等于二进制文本区域对象 */
            lv_textarea_set_cursor_click_pos(lv_scale.lv_dec, true);  /* 其他文本区域有效隐藏 */
            lv_textarea_set_cursor_click_pos(lv_scale.lv_hex, true);
            lv_textarea_set_cursor_click_pos(lv_scale.lv_oct, true);
        }
        else if (ta == lv_scale.lv_dec)
        {
            for (int i = 0 ; i < 18 ; i++)
            {
                if (i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 15)
                {
                    lv_btnmatrix_set_btn_ctrl(lv_scale.lv_btnmatrix, i, LV_BTNMATRIX_CTRL_DISABLED);
                }
            }
            lv_scale.lv_middle = lv_scale.lv_dec;
            lv_textarea_set_cursor_click_pos(lv_scale.lv_bin, true);  /* 其他文本区域有效隐藏 */
            lv_textarea_set_cursor_click_pos(lv_scale.lv_hex, true);
            lv_textarea_set_cursor_click_pos(lv_scale.lv_oct, true);
        }
        else if (ta == lv_scale.lv_hex)
        {
            for (int i = 0 ; i < 19 ; i++)
            {
                lv_btnmatrix_set_btn_ctrl(lv_scale.lv_btnmatrix, i, LV_BTNMATRIX_CTRL_CHECKABLE);
            }
            lv_scale.lv_middle = lv_scale.lv_hex;
            lv_textarea_set_cursor_click_pos(lv_scale.lv_bin, true);  /* 其他文本区域有效隐藏 */
            lv_textarea_set_cursor_click_pos(lv_scale.lv_dec, true);
            lv_textarea_set_cursor_click_pos(lv_scale.lv_oct, true);
        }
        else if (ta == lv_scale.lv_oct)
        {
            for (int i = 0 ; i < 18 ; i++)
            {
                if (i > 6 && i < 16)
                {
                    lv_btnmatrix_set_btn_ctrl(lv_scale.lv_btnmatrix, i, LV_BTNMATRIX_CTRL_DISABLED);
                }
            }
            lv_scale.lv_middle = lv_scale.lv_oct;
            lv_textarea_set_cursor_click_pos(lv_scale.lv_bin, true);  /* 其他文本区域有效隐藏 */
            lv_textarea_set_cursor_click_pos(lv_scale.lv_dec, true);
            lv_textarea_set_cursor_click_pos(lv_scale.lv_hex, true);
        }
    }
    else if(code == LV_EVENT_VALUE_CHANGED) /* 如果文本区域的数值发生改变，就执行下面的代码 */
    {
        /*  根据输入类型转换数据 */
        char str_tmp[32];        /* 进制存储数组 */
        char lv_oct_buf[32];     /* 进制存储数组 */
        char lv_dex_oct_buf[32]; /* 十进制转八进制存储数组 */
        static char regbit_flag_buf[32];
        const char * lv_atk_str_buffer = lv_textarea_get_text(ta); /*  获取输入内容 */
      
        if (ta == lv_scale.lv_bin) /* 如果是二进制文本区域的数值改变 */
        {
            if (lv_atk_str_buffer != NULL)
            {
                memset(lv_oct_buf,0,sizeof(lv_oct_buf));
                lv_snprintf(str_tmp, sizeof(str_tmp),"%d", lv_atk_bit_bin_to_dec(lv_atk_str_buffer));               /*  二进制转十进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_dec, str_tmp);
                lv_snprintf(lv_oct_buf, sizeof(lv_oct_buf), "%d", lv_atk_dec_to_oct(atof((const char *)str_tmp)));
                lv_textarea_set_placeholder_text(lv_scale.lv_oct, lv_oct_buf);                                      /* 十进制转八进制 */
                lv_snprintf(str_tmp, sizeof(str_tmp), "0x%X", lv_atk_bit_bin_to_dec(lv_atk_str_buffer));            /* 二进制转十六进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, str_tmp);

            }
            else /* 其他文本区域清空 */
            {
                lv_textarea_set_placeholder_text(lv_scale.lv_dec, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_oct, "");
            }
        }
        else if (ta == lv_scale.lv_dec) /* 如果是十进制文本区域的数值改变 */
        {
            if (lv_atk_str_buffer != NULL)
            {
                /*  十进制转二进制 */
                memset(regbit_flag_buf,0,sizeof(regbit_flag_buf));
                memset(lv_oct_buf,0,sizeof(lv_oct_buf));
                sprintf(regbit_flag_buf,"%ld",lv_atk_bec_to_bin(atof((const char *)lv_atk_str_buffer)));
                lv_textarea_set_placeholder_text(lv_scale.lv_bin, regbit_flag_buf);
                lv_snprintf(lv_oct_buf, sizeof(lv_oct_buf), "%d", lv_atk_dec_to_oct(atof((const char *)lv_atk_str_buffer)));  /* 十进制转八进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_oct, lv_oct_buf);                                                /* 十进制转八进制 */
                lv_snprintf(str_tmp, sizeof(str_tmp),"0x%X", lv_atk_bit_bin_to_dec(regbit_flag_buf));                         /* 二进制转十六进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, str_tmp);
            }
            else
            {
                lv_textarea_set_placeholder_text(lv_scale.lv_bin, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_oct, "");
            }

        }
        else if (ta == lv_scale.lv_hex) /* 如果是十六进制文本区域的数值改变 */
        {
            if (lv_atk_str_buffer != NULL)
            {
                memset(lv_oct_buf,0,sizeof(lv_oct_buf));
                memset(regbit_flag_buf,0,sizeof(regbit_flag_buf));
                memset(lv_dex_oct_buf,0,sizeof(lv_dex_oct_buf));
                lv_snprintf(lv_oct_buf, sizeof(lv_oct_buf), "%ld", lv_atk_hex_to_dex((char *)lv_atk_str_buffer));              /*  十六进制转十进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_dec, lv_oct_buf);
              
                sprintf(regbit_flag_buf,"%ld",lv_atk_bec_to_bin(atof((const char *)lv_oct_buf)));
                lv_textarea_set_placeholder_text(lv_scale.lv_bin, regbit_flag_buf);

                lv_snprintf(lv_dex_oct_buf, sizeof(lv_dex_oct_buf), "%d", lv_atk_dec_to_oct(atof((const char *)lv_oct_buf)));  /* 十进制转八进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_oct, lv_dex_oct_buf);                                             /* 十进制转八进制 */
                lv_snprintf(str_tmp, sizeof(str_tmp),"0x%X", lv_atk_bit_bin_to_dec(regbit_flag_buf));                          /* 二进制转十六进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, str_tmp);
            }
            else
            {
                lv_textarea_set_placeholder_text(lv_scale.lv_dec, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_bin, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_oct, "");
            }
        }
        else if (ta == lv_scale.lv_oct) /* 如果是八进制文本区域的数值改变 */
        {
            if (lv_atk_str_buffer != NULL)
            {
                memset(lv_oct_buf,0,sizeof(lv_oct_buf));
                memset(regbit_flag_buf,0,sizeof(regbit_flag_buf));
                lv_snprintf(lv_oct_buf, sizeof(lv_oct_buf), "%ld", lv_atk_oct_to_dex(atof((const char *)lv_atk_str_buffer)));  /*  八进制转十进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_dec, lv_oct_buf);
              
                sprintf(regbit_flag_buf,"%ld",lv_atk_bec_to_bin(atof((const char *)lv_oct_buf)));                              /* 十进制换二进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_bin, regbit_flag_buf);

                lv_snprintf(str_tmp, sizeof(str_tmp),"0x%X", lv_atk_bit_bin_to_dec(regbit_flag_buf));                          /* 二进制转十六进制 */
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, str_tmp);
            }
            else
            {
                lv_textarea_set_placeholder_text(lv_scale.lv_dec, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_bin, "");
                lv_textarea_set_placeholder_text(lv_scale.lv_hex, "");
            }
        }
    }
    
}

/**
 * @brief       创建进制界面
 * @param       parent：父类
 * @retval      无
 */
void lv_atk_scale_bit_init(lv_obj_t *parent)
{
    int lv_scale_col_1 = 0;   /*  列 */
    lv_obj_t *lv_obj_text;

    for (int i = 0 ; i < LV_ATK_BIT_COUNT; i++)
    {
        /*  位按钮编号索引 */
        lv_scale.lv_bit_index[i] = lv_obj_create(parent);                                                  /* 创建obj对象 */
        lv_obj_set_style_bg_color(lv_scale.lv_bit_index[i], lv_color_hex3(0x00), LV_STATE_DEFAULT);        /* 设置颜色 */
        lv_obj_set_style_border_side(lv_scale.lv_bit_index[i], 0, LV_STATE_DEFAULT);                       /* 设置边界宽度 */
        lv_obj_set_style_text_color(lv_scale.lv_bit_index[i], lv_color_hex3(0xffffff), LV_STATE_DEFAULT);  /* 设置字符 */
        lv_obj_set_style_radius(lv_scale.lv_bit_index[i], 2, LV_STATE_DEFAULT);                            /* 设置圆角 */
        lv_obj_set_size(lv_scale.lv_bit_index[i], LV_ATK_BIT_W, LV_ATK_BIT_INDEX_H);                       /* 设置对象大小(宽度和高度) */ 

        /*  位按钮 */
        lv_scale.lv_bit_text[i] = lv_btn_create(parent);                                                       /*  创建按钮对象 */
        lv_obj_set_style_radius(lv_scale.lv_bit_text[i], 5, LV_STATE_DEFAULT);                                 /* 设置圆角 */
        lv_obj_set_style_bg_color(lv_scale.lv_bit_text[i], lv_palette_main(LV_PALETTE_RED), LV_STATE_DEFAULT); /* 设置颜色 */
        lv_obj_set_style_border_side(lv_scale.lv_bit_text[i], 0, LV_STATE_DEFAULT);                            /* 设置边界宽度*/ 
        lv_obj_set_style_text_color(lv_scale.lv_bit_text[i], lv_color_hex3(0xffffff), LV_STATE_DEFAULT);       /* 设置字符 */
        lv_obj_set_size(lv_scale.lv_bit_text[i], LV_ATK_BIT_W, LV_ATK_BIT_H);                                  /* 设置对象大小(宽度和高度) */
        lv_obj_add_event_cb(lv_scale.lv_bit_text[i], lv_event_bit_map_handler,LV_EVENT_ALL,NULL);              /* 分配点击事件处理函数 */
        /* 位按钮数字 */
        lv_obj_text  = lv_label_create(lv_scale.lv_bit_text[i]);    /* 创建标签 */
        lv_label_set_text(lv_obj_text, "0");
        lv_obj_align_to(lv_obj_text, NULL, LV_ALIGN_CENTER, 0, 0);  /* 当前画布居中 */
        lv_obj_align_to(lv_scale.lv_bit_index[i], parent, LV_ALIGN_TOP_RIGHT, -(LV_ATK_BIT_ROW_SPACE*(lv_scale_col_1 ++ )+3), 50); /* 设置摆放位置 */
        lv_obj_align_to(lv_scale.lv_bit_text[i], lv_scale.lv_bit_index[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);                         /* 设置摆放位置 */
        
        if (i == 32)
        {
            lv_obj_set_style_bg_color(lv_scale.lv_bit_text[32], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);  /* 设置颜色 */
            lv_obj_set_style_bg_color(lv_scale.lv_bit_index[32], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT); /* 设置颜色 */
            lv_label_set_text(lv_obj_get_child(lv_scale.lv_bit_text[32],NULL), "");
        }

    }
    
    lv_scale.lv_bin_label = lv_label_create(parent);
    lv_label_set_text(lv_scale.lv_bin_label,"BIN");
    lv_obj_align_to(lv_scale.lv_bin_label,lv_scale.lv_bit_text[LV_ATK_BIT_COUNT - 1],LV_ALIGN_OUT_BOTTOM_MID,0,20);
    lv_obj_set_style_text_color(lv_scale.lv_bin_label,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    /*  二进制 */
    lv_scale.lv_bin = lv_textarea_create(parent);
    lv_textarea_set_accepted_chars(lv_scale.lv_bin, "01");                                   /* 设置字符列表，文本区域只接受这些字符 */
    lv_obj_set_style_text_font(lv_scale.lv_bin, &lv_font_montserrat_14, LV_STATE_DEFAULT);   /* 设置数值字体格式 */
    lv_obj_set_style_radius(lv_scale.lv_bin, 0, LV_STATE_DEFAULT);                           /* 设置圆角 */
    lv_obj_set_size(lv_scale.lv_bin,lcddev.width/2,LV_ATK_BIT_H);
    lv_textarea_set_text(lv_scale.lv_bin, "");
    lv_textarea_set_one_line(lv_scale.lv_bin, true);
    lv_textarea_set_cursor_click_pos(lv_scale.lv_bin, true);
    lv_obj_add_event_cb(lv_scale.lv_bin, lv_ta_cb_event_handler,LV_EVENT_ALL,NULL);
    lv_obj_align_to(lv_scale.lv_bin, lv_scale.lv_bin_label, LV_ALIGN_OUT_RIGHT_MID, 12, 0);  /* 摆放位置 */
    
    
    lv_scale.lv_dec_label = lv_label_create(parent);
    lv_label_set_text(lv_scale.lv_dec_label,"DEC");
    lv_obj_align_to(lv_scale.lv_dec_label,lv_scale.lv_bin_label,LV_ALIGN_OUT_BOTTOM_MID,0,20);
    lv_obj_set_style_text_color(lv_scale.lv_dec_label,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    /*  十进制 */
    lv_scale.lv_dec = lv_textarea_create(parent);
    lv_textarea_set_accepted_chars(lv_scale.lv_dec, "0123456789");                          /* 设置字符列表，文本区域只接受这些字符 */
    lv_obj_set_style_text_font(lv_scale.lv_dec, &lv_font_montserrat_14, LV_STATE_DEFAULT);  /* 设置文本字体格式 */
    lv_obj_set_style_radius(lv_scale.lv_dec, 0, LV_STATE_DEFAULT);                          /* 设置圆角 */
    lv_obj_set_size(lv_scale.lv_dec,lcddev.width/2,LV_ATK_BIT_H);
    lv_textarea_set_text(lv_scale.lv_dec, "");
    lv_textarea_set_one_line(lv_scale.lv_dec,lv_scale.lv_dec);
    lv_textarea_set_cursor_click_pos(lv_scale.lv_dec, true);
    lv_obj_add_event_cb(lv_scale.lv_dec, lv_ta_cb_event_handler,LV_EVENT_ALL,NULL);
    lv_obj_align_to(lv_scale.lv_dec, lv_scale.lv_dec_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);  /*  摆放位置 */
    
    lv_scale.lv_hex_label = lv_label_create(parent);
    lv_label_set_text(lv_scale.lv_hex_label,"HEX");
    lv_obj_align_to(lv_scale.lv_hex_label,lv_scale.lv_dec_label,LV_ALIGN_OUT_BOTTOM_MID,0,20);
    lv_obj_set_style_text_color(lv_scale.lv_hex_label,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    /*  十六进制 */
    lv_scale.lv_hex = lv_textarea_create(parent);
    lv_obj_set_style_text_color(lv_scale.lv_hex,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    lv_textarea_set_accepted_chars(lv_scale.lv_hex, "0123456789ABCDEFabcdef");              /* 设置字符列表，文本区域只接受这些字符 */
    lv_obj_set_style_text_font(lv_scale.lv_hex, &lv_font_montserrat_14, LV_STATE_DEFAULT);  /* 设置文本字体格式 */
    lv_obj_set_style_radius(lv_scale.lv_hex, 0, LV_STATE_DEFAULT);        /*  设置圆角 */
    lv_obj_set_size(lv_scale.lv_hex,lcddev.width/2,LV_ATK_BIT_H);
    lv_textarea_set_text(lv_scale.lv_hex, "");
    lv_textarea_set_one_line(lv_scale.lv_hex, true);
    lv_textarea_set_cursor_click_pos(lv_scale.lv_hex, true);
    lv_obj_add_event_cb(lv_scale.lv_hex, lv_ta_cb_event_handler,LV_EVENT_ALL,NULL);
    lv_obj_align_to(lv_scale.lv_hex, lv_scale.lv_hex_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);  /* 摆放位置 */
    
    lv_scale.lv_oct_label = lv_label_create(parent);
    lv_label_set_text(lv_scale.lv_oct_label,"OCT");
    lv_obj_align_to(lv_scale.lv_oct_label,lv_scale.lv_hex_label,LV_ALIGN_OUT_BOTTOM_MID,0,20);
    lv_obj_set_style_text_color(lv_scale.lv_oct_label,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    /*  八进制 */
    lv_scale.lv_oct = lv_textarea_create(parent);
    lv_textarea_set_accepted_chars(lv_scale.lv_oct, "01234567");                            /* 设置字符列表，文本区域只接受这些字符 */
    lv_obj_set_style_text_font(lv_scale.lv_oct, &lv_font_montserrat_14, LV_STATE_DEFAULT);  /* 设置文本字体格式 */
    lv_obj_set_style_radius(lv_scale.lv_oct, 0, LV_STATE_DEFAULT);                          /* 设置圆角 */
    lv_obj_set_size(lv_scale.lv_oct,lcddev.width/2,LV_ATK_BIT_H);
    lv_textarea_set_text(lv_scale.lv_oct, "");
    lv_textarea_set_one_line(lv_scale.lv_oct, true);
    lv_textarea_set_cursor_click_pos(lv_scale.lv_oct, true);
    lv_obj_add_event_cb(lv_scale.lv_oct, lv_ta_cb_event_handler,LV_EVENT_ALL,NULL);
    lv_obj_align_to(lv_scale.lv_oct, lv_scale.lv_oct_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);  /*  摆放位置 */
}

/**
  * @brief  返回按键回调函数
  * @param  event:事件
  * @retval 无
  */
void lv_3d_back_btn_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_PRESSED)
    {
        lv_label_set_text(obj,"#444444 "BACK_BTN_TITLE"#");
        lv_label_set_recolor(obj,true);
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        lv_obj_del(lv_scale.lv_scale_cont);
        lv_main_window();/* 打开登录窗体 */
    }
}

/**
 * @brief       进制计算器程序入口
 * @param       无
 * @retval      无
 */
void lv_scale_demo(void)
{
    lv_snprintf(lv_bit_flag, sizeof(lv_bit_flag), "000000000000000000000000000000000"); /* 设置位的标志初始化值 */

    lv_general_dev.lv_general_win_create = NULL;
    lv_general_dev.lv_back_event = NULL;
    lv_general_dev.parent = NULL;

    lv_scale.lv_scale_cont = lv_obj_create(lv_scr_act());                    /* 创建一个容器 */
    lv_obj_set_style_bg_color(lv_scale.lv_scale_cont, lv_color_make(1, 27, 54), LV_STATE_DEFAULT);
    lv_obj_set_size(lv_scale.lv_scale_cont, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_radius(lv_scale.lv_scale_cont, 0, LV_PART_MAIN);
    lv_obj_add_flag(lv_scale.lv_scale_cont,LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_add_flag(lv_scale.lv_scale_cont,LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    
    lv_general_dev.lv_back_event = lv_3d_back_btn_event_handler;
    lv_general_dev.parent = lv_scale.lv_scale_cont;
    lv_general_dev.lv_general_win_create = lv_general_win_create;
    lv_general_dev.lv_general_win_create();
    lv_obj_set_pos(lv_scale.lv_scale_cont,0,0);                              /* 设置容器的位置 */
    lv_scale.lv_btnmatrix = NULL;                                            /* 设置键盘对象位空 */
    lv_atk_scale_bit_init(lv_scale.lv_scale_cont);                           /*  初始化界面 */
}

