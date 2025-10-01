#include "lvgl/lvgl.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "LVGL/GUI_APP/lv_calculator.h"
# include "stdio.h"
# include "stdlib.h"


static const char * kb_map_num[36] = {
                                    "sin", "cos", "tan", "log","ln", "\n",
                                    "x^2", "x^y", "sqrt", "1/x","FMT", "\n",
                                    "7", "8", "9", "DEL","AC", "\n",
                                    "4", "5", "6", "+","-", "\n",
                                    "1", "2", "3", "*","/", "\n",
                                    "0", "+/-", ".", "=", ""};

lv_obj_t * lv_calculator_cont;
lv_obj_t * phone_ta;               /* 账号文字区域控件 */
lv_obj_t * label_dec;
static double calc_x1 = 0,calc_x2 = 0; /* 2个操作数 */
int lv_calc_x1 = 0;
int lv_calc_num = 0;
int lv_calc_pos = 0;
int calc_ec =0;
double cbp_and_negative = 0;
int lv_cbp_and_negative = 0;
int lv_cbp_and_negativee = 0;
int lv_calc_math = 0;
uint8_t lv_math_flag = 0;
double lv_math_x1 = 0;
double lv_math_x2 = 0;
double lv_math_result = 0;
char lv_chx_buf[100];
lv_font_t lv_font_ttf_36;
                                    

/* π值 */
#define CALC_PI 3.1415926535897932384626433832795

/**
  * @brief  格式化显示
  * @param  res:要转换的数据,最大支持0XFFFF
  * @param  fmt:0:十进制
                1:十六进制
                2:二进制
  * @retval 无
  */
void calc_fmt_show(int x2,const char * buf,uint8_t fmt)
{
    x2 = atof((const char *)buf); /* 转换为数字 */
    char *fmtstr="";
    char outbuf[17];
    char fmt_chx_buf[100];
    memset(fmt_chx_buf, 0, sizeof(fmt_chx_buf));
    fmtstr="HEX";
    sprintf((char*)outbuf,"%X",x2); /* 输出结果到outbuf里面 */
    lv_textarea_set_text(phone_ta,(const char *)outbuf);
    strcat((char *)fmt_chx_buf, fmtstr);
    lv_label_set_text(label_dec,fmt_chx_buf);
}

/**
  * @brief  获取计算加减乘除标志
  * @param  ctype:计算的类型
  * @retval 返回计算类型
  */
uint8_t lv_math_calc_label(uint8_t ctype)
{
    char *chx;
    memset(lv_chx_buf, 0, sizeof(lv_chx_buf));
  
    switch(ctype)
    {
        case 0:/* 加法计算 */
          chx = "+";
          break;
       case 1:/* 减法计算 */
          chx = "-";
          break;
       case 2:/* 乘法计算 */
          chx = "*";
          break;
       case 3:/* 除法计算 */
          chx = "/";
          break;
       case 4:/* x^y次方计算 */
          chx = "x ^ y";
          break;
    }

    strcat((char *)lv_chx_buf, chx);
    strcat((char *)lv_chx_buf, " DEG");
    lv_label_set_text(label_dec,lv_chx_buf);
    return  ctype;
}

/**
  * @brief  等于=按键计算
  * @param  x1:获取第一个数值
  * @param  x2:获取第二个数值
  * @param  ctype:计算的类型
  * @retval 返回计算数值
  */
double lv_math_calc(double x1,double x2,uint8_t ctype)
{
    switch(ctype)
    {
        case 0:/* 加法 */
          x1=x1+x2;
          break;
        case 1:/* 减法 */
          x1=x1-x2;
          break;
        case 2:/* 乘法 */
          x1=x1*(x2);
          break;
        case 3:/* 除法 */
          x1=x1/(x2);
          break;
        case 4:/* x^y次方 */
          x1=pow(x1,x2);
          break;
        case 5:/* 没有任何运算符 */
          x1=x2;
          break;
    }
    
    return x1;
}

/**
  * @brief  计算特殊的类型
  * @param  x1:获取第一个数值
  * @param  x2:无
  * @param  buf:获取的字符串
  * @param  ctype:计算的类型
  * @retval 无
  */
uint8_t lv_calc_exe(double *x1,double *x2,const char * buf,uint8_t ctype)
{
    *x1 = atof((const char *)buf); /* 转换为数字 */
    char *chx;
    memset(lv_chx_buf, 0, sizeof(lv_chx_buf));
    
    switch(ctype)
    {
        case 0:/* sin计算 */
          *x1 = sin((*x1*CALC_PI)/180);/* 转换为角度计算结果 */
          chx = "sin";
          break;
        case 1:/* cos计算 */
          *x1 = cos((*x1*CALC_PI)/180);/* 转换为角度计算结果 */
          chx = "cos";
          break;
        case 2:/* tan计算 */
          *x1 = tan((*x1*CALC_PI)/180);/* 转换为角度计算结果 */
          chx = "tan";
          break;
        case 3:/* log计算 */
          *x1 = log10(*x1);
          chx = "log";
          break;
        case 4:/* ln计算 */
          *x1 = log(*x1);
          chx = "ln";
          break;
        case 5:/* x^2计算 */
          *x1 = *x1*(*x1);
          chx = "x^2";
          break;
        case 6:/* 开方计算 */
          *x1=sqrt(*x1);
          chx = "^";
          break;
        case 7:/* 倒数计算 */
          *x1=1/(*x1);
          chx = "1/x";
          break;
    }
    
    strcat((char *)lv_chx_buf, chx);
    strcat((char *)lv_chx_buf, " DEG");
    lv_label_set_text(label_dec,lv_chx_buf);
}

/**
  * @brief  回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_event_handler(lv_event_t *event)
{
    char str[25];
    char chx_buf[100];
    memset(chx_buf, 0, sizeof(chx_buf));
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);

    if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t* dsc = lv_event_get_param(event);
        
        if (dsc->id == 0)
        {
            dsc->rect_dsc->radius = 0;
            dsc->rect_dsc->border_width = 2;
            dsc->rect_dsc->bg_color = lv_color_make(1, 27, 54);
            dsc->rect_dsc->border_color = lv_color_white();
        }
        else
        {
            dsc->rect_dsc->border_width = 2;
            dsc->rect_dsc->radius = 0;
            dsc->rect_dsc->bg_color = lv_color_make(1, 27, 54);
            dsc->rect_dsc->border_color = lv_color_white();
            dsc->label_dsc->color = lv_color_white();
        }
    }
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt =  lv_btnmatrix_get_btn_text (obj,id);
        uint32_t cur = lv_textarea_get_cursor_pos(phone_ta);

        if (atof(lv_textarea_get_text(phone_ta)) == 0&&lv_calc_pos == 0&&lv_cbp_and_negative ==0&&lv_calc_math == 0)
        {
            lv_textarea_set_text(phone_ta, "");
        }

        if (txt == kb_map_num[0])  /* sin */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),0);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[1])/* cos */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),1);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[2]) /* tan */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),2);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[3]) /* log */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),3);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[4]) /* ln */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),4);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[6]) /* x^2 */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),5);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[8]) /* sqrt */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),6);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[9])/* 1/x */
        {
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_calc_exe(&calc_x1,0,lv_textarea_get_text(phone_ta),7);
            sprintf(str,"%g",calc_x1);
            lv_textarea_set_text(phone_ta,(const char *)str);
        }
        else if (txt == kb_map_num[16]) /* AC */
        {
            lv_textarea_set_text(phone_ta, "0");
            lv_label_set_text(label_dec,"DEG");
            lv_calc_pos = 0;
            lv_cbp_and_negativee = 0;
            lv_cbp_and_negative = 0;
        }
        else if(txt == kb_map_num[15]) /* 删除一个字符 */
        {
            if (calc_ec == 0)
            {
                lv_textarea_del_char(phone_ta);

                if (atof(lv_textarea_get_text(phone_ta)) == 0)
                {
                    lv_textarea_set_text(phone_ta, "0");
                }
            }
            else
            {
                lv_textarea_set_text(phone_ta, "0");
                lv_calc_math = 0;
                lv_calc_pos = 0;
                lv_cbp_and_negativee = 0;
                lv_cbp_and_negative = 0;
            }
        }
        else if (txt == kb_map_num[12]||txt == kb_map_num[13]||txt == kb_map_num[14]
                 ||txt == kb_map_num[18]||txt == kb_map_num[19]||txt == kb_map_num[20]
                 ||txt == kb_map_num[24]||txt == kb_map_num[25]||txt == kb_map_num[26]
                 ||txt == kb_map_num[30]||txt == kb_map_num[32])
        {
            if (calc_ec == 1)
            {
                lv_textarea_set_text(phone_ta, "");
                lv_label_set_text(label_dec,"DEG");
                calc_ec = 0; 
                lv_calc_num = 0;
                lv_calc_pos = 0;
                lv_cbp_and_negativee = 0;
                lv_cbp_and_negative = 0;
            }
            
            if (lv_math_x1!=0&&atof(lv_textarea_get_text(phone_ta)) == 0&&!strstr(lv_textarea_get_text(phone_ta), ".")&&!strstr(lv_textarea_get_text(phone_ta), "-"))
            {
                lv_textarea_set_text(phone_ta, "");
            }
            
            if (lv_cbp_and_negative == 1&&strstr(lv_textarea_get_text(phone_ta), "-")&&lv_cbp_and_negativee == 0)
            {
                lv_textarea_del_char(phone_ta);
                lv_cbp_and_negative = 0;
                lv_cbp_and_negativee = 1;
            }

            lv_textarea_set_cursor_pos(phone_ta, (int32_t)cur);

            if (atof(lv_textarea_get_text(phone_ta)) == 0&&txt == kb_map_num[32]&&lv_calc_pos != 1)
            {
                lv_calc_pos = 1;
                strcat((char *)chx_buf, "0.");
                lv_textarea_add_text(phone_ta,chx_buf);
            }
            else
            {
                if (txt == kb_map_num[32]&&strstr(lv_textarea_get_text(phone_ta), "."))
                {
                    
                }
                else
                {
                    lv_textarea_add_text(phone_ta,txt);
                }
            }

        }
        else if (txt == kb_map_num[10]) /* FMT */
        {
            lv_calc_num ++;
            if (lv_calc_num == 1)
            {
                calc_ec = 1;
                calc_fmt_show(lv_calc_x1,lv_textarea_get_text(phone_ta),lv_calc_num);
            }

            if (lv_calc_num > 1)
            {
                lv_cbp_and_negative = 0;
                lv_cbp_and_negativee = 0;
                lv_calc_num = 0;
                lv_calc_pos = 0;
                lv_label_set_text(label_dec,"DEG");
                lv_textarea_set_text(phone_ta, "0");
            }
       }
       else if (txt == kb_map_num[31]) /* +/- */
       {
           cbp_and_negative = atof((const char *)lv_textarea_get_text(phone_ta)); /* 转换为数字 */
           cbp_and_negative = - cbp_and_negative;
           memset(str, 0, sizeof(str));
           sprintf(str,"%g",cbp_and_negative);
           lv_textarea_set_text(phone_ta,(const char *)str);
           lv_cbp_and_negative = 1;
       }
       else if (txt == kb_map_num[21]) /* + */
       {
            lv_calc_math = 1;
            if (lv_math_x1 == 0)
            {
                lv_math_x1 = atof((const char *)lv_textarea_get_text(phone_ta));
                lv_textarea_set_text(phone_ta, "0");
            }
            else
            {
                lv_calc_math = 0;
            }
            lv_math_flag = lv_math_calc_label(0);

       }
       else if (txt == kb_map_num[22]) /* - */
       {
            lv_calc_math = 1;
            if (lv_math_x1 == 0)
            {
                lv_math_x1 = atof((const char *)lv_textarea_get_text(phone_ta));
                lv_textarea_set_text(phone_ta, "0");
            }
            else
            {
                lv_calc_math = 0;
            }
            lv_math_flag = lv_math_calc_label(1);
       }
       else if (txt == kb_map_num[27]) /* X */
       {
            lv_calc_math = 1;
            if (lv_math_x1 == 0)
            {
                lv_math_x1 = atof((const char *)lv_textarea_get_text(phone_ta));
                lv_textarea_set_text(phone_ta, "0");
            }
            else
            {
                lv_calc_math = 0;
            }
            lv_math_flag = lv_math_calc_label(2);
       }
       else if (txt == kb_map_num[28]) /* / */
       {
            lv_calc_math = 1;
            if (lv_math_x1 == 0)
            {
                lv_math_x1 = atof((const char *)lv_textarea_get_text(phone_ta));
                lv_textarea_set_text(phone_ta, "0");
            }
            else
            {
                lv_calc_math = 0;
            }
            lv_math_flag = lv_math_calc_label(3);
       }
       else if (txt == kb_map_num[7]) /* x^y */
       {
            lv_calc_math = 1;
            if (lv_math_x1 == 0)
            {
                lv_math_x1 = atof((const char *)lv_textarea_get_text(phone_ta));
                lv_textarea_set_text(phone_ta, "0");
            }
            else
            {
                lv_calc_math = 0;
            }
            lv_math_flag = lv_math_calc_label(4);
       }
       else if (txt == kb_map_num[33]) /* = */
       {
         
            lv_math_x2 = atof((const char *)lv_textarea_get_text(phone_ta));
            lv_math_result = lv_math_calc(lv_math_x1,lv_math_x2,lv_math_flag);
            lv_label_set_text(label_dec,"DEG");
            memset(str, 0, sizeof(str));
            calc_ec = 1;
            lv_math_x1 = 0;
            sprintf(str,"%g",lv_math_result);
            lv_textarea_set_text(phone_ta,(const char *)str);
       }
    }
}

/**
  * @brief  返回按键回调函数
  * @param  event:事件
  * @retval 无
  */
void lv_calculator_back_btn_event_handler(lv_event_t *event)
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
        lv_obj_del(lv_calculator_cont);
        lv_mainstart();/* 打开登录窗体 */
    }
}

/**
  * @brief  测试
  * @param  无
  * @retval 无
  */
void lv_calculator_demo(void)
{
    lv_general_dev.lv_general_win_create = NULL;
    lv_general_dev.lv_back_event = NULL;
    lv_general_dev.parent = NULL;

    lv_calculator_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(lv_calculator_cont,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_radius(lv_calculator_cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(lv_calculator_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(lv_calculator_cont, lv_color_make(1, 27, 54), LV_STATE_DEFAULT);
  
    lv_general_dev.lv_back_event = lv_calculator_back_btn_event_handler;
    lv_general_dev.parent = lv_calculator_cont;
    lv_general_dev.lv_general_win_create = lv_general_win_create;
    lv_general_dev.lv_general_win_create();
  
    lv_obj_t *label_title = lv_label_create(lv_calculator_cont);
    lv_label_set_text(label_title,"科学计算器");
    lv_obj_set_style_text_color(label_title, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_title,&myFont14,0);
    lv_obj_align(label_title,LV_ALIGN_TOP_MID,0,10);
  
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_calculator_cont);
    lv_btnmatrix_set_map(btnm1, kb_map_num);
    lv_btnmatrix_set_btn_width(btnm1, 0, 5);                                /* 设置第一行按钮宽度 */
    lv_btnmatrix_set_btn_width(btnm1, 1, 5);
    lv_btnmatrix_set_btn_width(btnm1, 2, 5);
    lv_btnmatrix_set_btn_width(btnm1, 3, 5);
    lv_btnmatrix_set_btn_width(btnm1, 4, 5);

    lv_btnmatrix_set_btn_width(btnm1, 5, 5);
    lv_btnmatrix_set_btn_width(btnm1, 6, 5);                                /* 设置第二行按钮宽度 */
    lv_btnmatrix_set_btn_width(btnm1, 7, 5);
    lv_btnmatrix_set_btn_width(btnm1, 8, 5);
    lv_btnmatrix_set_btn_width(btnm1, 9, 5);

    lv_btnmatrix_set_btn_width(btnm1, 10, 1);
    lv_btnmatrix_set_btn_width(btnm1, 11, 1);                               /* 设置第三行按钮宽度 */
    lv_btnmatrix_set_btn_width(btnm1, 12, 1);
    lv_btnmatrix_set_btn_width(btnm1, 13, 7);
    lv_btnmatrix_set_btn_width(btnm1, 14, 7);

    lv_btnmatrix_set_btn_width(btnm1, 15, 1);
    lv_btnmatrix_set_btn_width(btnm1, 16, 1);
    lv_btnmatrix_set_btn_width(btnm1, 17, 1);                                /* 设置第四行按钮宽度 */
    lv_btnmatrix_set_btn_width(btnm1, 18, 7);
    lv_btnmatrix_set_btn_width(btnm1, 19, 7);

    lv_btnmatrix_set_btn_width(btnm1, 20, 1);
    lv_btnmatrix_set_btn_width(btnm1, 21, 1);
    lv_btnmatrix_set_btn_width(btnm1, 22, 1);                                /* 设置第五行按钮宽度 */
    lv_btnmatrix_set_btn_width(btnm1, 23, 7);
    lv_btnmatrix_set_btn_width(btnm1, 24, 7);

    lv_btnmatrix_set_btn_width(btnm1, 25, 1);
    lv_btnmatrix_set_btn_width(btnm1, 26, 5);
    lv_btnmatrix_set_btn_width(btnm1, 27, 5);                                /* 设置第六行按钮宽度 */
    lv_btnmatrix_set_btn_width(btnm1, 28, 5);
    lv_btnmatrix_set_btn_width(btnm1, 29, 5);


    lv_obj_set_size(btnm1,lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act())/2);
    lv_obj_align(btnm1, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(btnm1, lv_event_handler, LV_EVENT_ALL, NULL);
//    lv_obj_set_style_text_font(btnm1,&myFont18,0);

    /* 创建输入框 */
    phone_ta = lv_textarea_create(lv_calculator_cont);
    lv_textarea_set_text(phone_ta, "0");
    lv_textarea_set_one_line(phone_ta, true);           /* 将文本区域配置为一行或恢复正常 */
    lv_textarea_set_cursor_click_pos(phone_ta,false);   /* 隐藏光标 */
    lv_obj_set_size(phone_ta, lv_obj_get_width(lv_scr_act()) - 20,myFont36.line_height + 20);
    lv_obj_clear_flag(phone_ta, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(phone_ta, btnm1, LV_ALIGN_OUT_TOP_MID, 0, -50);
    lv_obj_set_style_text_font(phone_ta,&myFont36,0);


    label_dec = lv_label_create(lv_calculator_cont);
    lv_label_set_text(label_dec,"DEG");
    lv_obj_set_style_text_font(label_dec,&myFont18,0);
    lv_obj_align_to(label_dec,phone_ta,LV_ALIGN_BOTTOM_RIGHT,-50,-10);
}
