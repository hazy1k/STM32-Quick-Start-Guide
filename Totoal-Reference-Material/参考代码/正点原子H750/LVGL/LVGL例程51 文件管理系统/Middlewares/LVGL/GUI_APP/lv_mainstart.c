/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL 文件管理 实验
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
#include "./FATFS/exfuns/exfuns.h"
#include "./MALLOC/malloc.h"
#include "./BSP/LCD/lcd.h"
#include "./SYSTEM/usart/usart.h"
#include "lvgl.h"
#include <stdio.h>


LV_FONT_DECLARE(myFont24)
LV_FONT_DECLARE(myFont18)
LV_FONT_DECLARE(myFont14)

lv_file_struct lv_flie;
/* 文件的后缀名，可以在这个数组添加未知的后缀 */
char *lv_suffix [] ={".txt",".avi",".png","jpeg",".jpg",".bin",".gif",".bmp",".FON",".dat",".sif",".BIN",".xbf",".ttf"};
#define LV_SUFFIX(x)    (int)(sizeof(x)/sizeof(x[0])) /* 计算lv_suffix数组的大小 */

uint16_t lv_scan_files (const char* path,lv_obj_t *parent);
void lv_del_list(lv_obj_t *parent);
void lv_create_list(lv_obj_t *parent);
void lv_mainstart(void);
void list_init(lv_obj_t *parent);
lv_obj_t * lv_create_page(lv_obj_t *parent);
char * lv_pash_joint(void);

/**
 * @brief  告诉文件的位置
 * @param  fd：文件指针
 * @return 返回位置
 */
long lv_tell(lv_fs_file_t *fd)
{
    uint32_t pos = 0;
    lv_fs_tell(fd, &pos);
    printf("\nlv_tcur pos is: %d\n", pos);
    return pos;
}

/**
 * @brief  读取文件内容
 * @param  path：文件路径
 * @return LV_FS_RES_OK：读取成功
 */
lv_fs_res_t lv_file_read(const char *path)
{
    uint32_t rsize = 0;
    lv_fs_file_t fd;
    lv_fs_res_t res;

    res = lv_fs_open(&fd, path, LV_FS_MODE_RD);
    
    if (res != LV_FS_RES_OK)
    {
        printf("open %s ERROR\n",path);
        return LV_FS_RES_UNKNOWN;
    }

    lv_tell(&fd);
    lv_fs_seek(&fd,0,LV_FS_SEEK_SET);
    lv_tell(&fd);
    res = lv_fs_read(&fd, lv_flie.rbuf, FILE_SEZE, &rsize);

    if (res != LV_FS_RES_OK)
    {
        printf("read %s ERROR\n",path);
        return LV_FS_RES_UNKNOWN;
    }

    lv_tell(&fd);
    
    lv_fs_close(&fd);
    
    return LV_FS_RES_OK;
}

/**
  * @brief  页面返回按键回调函数
  * @param  obj  : 对象
  * @param  event: 事件
  * @retval 无
  */
void lv_btn_close_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);

    if (code == LV_EVENT_RELEASED)
    {
        if (lv_flie.lv_image_read != NULL)     /* 判断图片路径释放为空 */
        {
            lv_obj_del(lv_flie.lv_image_read); /* 删除图片对象 */
            lv_flie.lv_image_read = NULL;      /* 设置图片对象为空 */
        }
        
        lv_flie.lv_prev_file_flag -- ;         /* 文件返回标志位减一 */
        
        lv_obj_del(lv_flie.lv_page_cont);      /* 当按下返回时，删除页面 */
    }
}

/**
  * @brief  创建page页面
  * @param  parent:父类
  * @retval 无
  */
lv_obj_t * lv_create_page(lv_obj_t *parent)
{
    lv_flie.lv_page_cont = lv_obj_create(parent);    /* 创建容器 */
    lv_obj_set_size(lv_flie.lv_page_cont, lcddev.width, lcddev.height);
    lv_obj_set_style_radius(lv_flie.lv_page_cont,0,LV_STATE_DEFAULT);/* 设置圆半径为0 */
    lv_obj_clear_flag(lv_flie.lv_page_cont,LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(lv_flie.lv_page_cont,LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_align_to(lv_flie.lv_page_cont,parent,LV_ALIGN_CENTER,0,0);
  
    lv_obj_t *lv_page_obj = lv_obj_create(lv_flie.lv_page_cont); /* 创建返回按键的区域 */
    lv_obj_set_style_bg_color(lv_page_obj,lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);
    lv_obj_align(lv_page_obj,LV_ALIGN_BOTTOM_MID,0,10);
    lv_obj_set_size(lv_page_obj, lcddev.width, myFont24.line_height);

    lv_obj_t * lv_page_back_btn = lv_label_create(lv_page_obj);  /* 创建lable作为返回的对象 */
    lv_obj_set_style_text_font(lv_page_back_btn,&myFont24,LV_STATE_DEFAULT);
    lv_label_set_text(lv_page_back_btn,"返回");
    lv_obj_align_to(lv_page_back_btn,NULL,LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(lv_page_back_btn,LV_OBJ_FLAG_CLICKABLE); /* 设置标签可点击 */
    lv_obj_add_event_cb(lv_page_back_btn,lv_btn_close_event,LV_EVENT_ALL,NULL); /* 设置回调函数 */

    return lv_flie.lv_page_cont;
}

/**
  * @brief  显示.txt文件
  * @param  parent:父类
  * @retval 无
  */
void lv_show_filetxt(lv_obj_t *parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(label,&myFont18,LV_STATE_DEFAULT);
    lv_obj_set_width(label, lv_obj_get_width(parent));
    lv_label_set_text(label, (char *)lv_flie.rbuf); /* 显示读取的数据 */
    memset(lv_flie.rbuf,0,sizeof(lv_flie.rbuf));
}

/**
  * @brief  显示.bin图片
  * @param  parent:父类
  * @param  path:路径
  * @retval 无
  */
void lv_show_imgbin(lv_obj_t *parent,const char *path)
{
    lv_flie.lv_image_read = lv_img_create(parent);                     /* 创建 image 控件 */   
    lv_img_set_src(lv_flie.lv_image_read,path);                        /* 为控件设置图片 */
//    lv_img_set_auto_size(lv_flie.lv_image_read, true);                 /* 自动设置大小 */
    lv_obj_align_to(lv_flie.lv_image_read,parent,LV_ALIGN_CENTER,0,0); /* 设置控件的对齐方式,相对坐标 */
}

/**
  * @brief  显示.bmp图片
  * @param  parent:父类
  * @param  path:路径
  * @retval 无
  */
void lv_show_imgbmp(lv_obj_t *parent,const char *path)
{
    lv_bmp_init();                                                     /* 初始化bmp库 */
    lv_flie.lv_image_read = lv_img_create(parent);                     /* 创建 image 控件 */   
    lv_img_set_src(lv_flie.lv_image_read,path);                        /* 为控件设置图片 */
//    lv_img_set_auto_size(lv_flie.lv_image_read, true);                 /* 自动设置大小 */
//    int ext = lv_obj_get_ext_attr(lv_flie.lv_image_read);              /* 获取图片属性 */
    lv_obj_align_to(lv_flie.lv_image_read,parent,LV_ALIGN_CENTER,0,0); /* 设置控件的对齐方式,相对坐标 */
}

/**
  * @brief  显示.jpeg图片
  * @param  parent:父类
  * @param  path:路径
  * @retval 无
  */
void lv_show_imgjpeg(lv_obj_t *parent,const char *path)
{
    lv_split_jpeg_init();                                              /* 初始化jpeg库 */
    lv_flie.lv_image_read = lv_img_create(parent);                     /* 创建 image 控件 */   
    lv_img_set_src(lv_flie.lv_image_read,path);                        /* 为控件设置图片 */
//    lv_img_set_auto_size(lv_flie.lv_image_read, true);               /* 自动设置大小 */
//    int ext = lv_obj_get_ext_attr(lv_flie.lv_image_read);            /* 获取图片属性 */
    lv_obj_align_to(lv_flie.lv_image_read,parent,LV_ALIGN_CENTER,0,0); /* 设置控件的对齐方式,相对坐标 */
}

/**
  * @brief  文件路径拼接
  * @param  无
  * @retval 无
  */
char * lv_pash_joint(void)
{
    lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag] = (char *)lv_flie.lv_pash;/* 把上一个路径保存到这个数组里 */
    lv_flie.lv_prev_file_flag ++;                              /* 前一个路径数量标志位加一 */
  
    strcpy((char *)lv_flie.pname, lv_flie.lv_pash);            /* 复制路径(目录) */ 
    strcat((char *)lv_flie.pname, "/");                        /* 复制路径(目录) */ 
    strcat((char *)lv_flie.pname, (char *)lv_flie.lv_pname);   /* 将文件名接在后面 */
    return lv_flie.pname;
}

/**
  * @brief  列表按键回调函数
  * @param  obj  : 对象
  * @param  event: 事件
  * @retval 无
  */
static void lv_list_btn_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i <= lv_flie.list_flie_nuber ;i++)  /* 轮询列表项 */
        {
            if (obj == lv_flie.list_btn[i]) /* 判断列表项的那个按键按下 */
            {
                lv_flie.lv_pname = mymalloc(SRAMIN, sizeof(lv_flie.lv_pname));         /* 获取文件名分配内存 */
                lv_flie.pname = mymalloc(SRAMIN, sizeof(lv_flie.pname));               /* 为带路径的文件名分配内存 */
                lv_flie.lv_pname = (char *)lv_list_get_btn_text(lv_flie.list,lv_flie.list_btn[i]);  /* 获取列表项的值 */
              
                for (int suffix = 0;suffix < LV_SUFFIX(lv_suffix) ;suffix ++)     /* 轮询文件后缀 */
                {
                    if (strstr(lv_flie.lv_pname,lv_suffix[suffix]) != NULL)       /* 如果不是文件夹 */
                    {
                        lv_flie.lv_suffix_flag = 0;                               /* 设置后缀的标志位为0 */
                        break;
                    }
                }

                if (lv_flie.lv_suffix_flag == 1)                                  /* 该标志位不为0就是文件夹操作 */
                {   
                    lv_flie.lv_pash = lv_pash_joint();                            /* 把文件路径转递给lv_pash参数 */
                    lv_del_list(lv_flie.list);                                    /* 删除列表 */
                    lv_scan_files(lv_flie.pname,lv_scr_act());                    /* 重新创建文件列表 */
                }
                else
                {
                    if (strstr(lv_flie.lv_pname,".txt") != NULL)                  /* 如果不是文件夹且是txt文件 */
                    {
                        if (lv_file_read(lv_pash_joint()) == LV_FS_RES_OK)        /* 判断读取txt文件是否成功 */
                        {
                            lv_flie.lv_return_page = lv_create_page(lv_scr_act());/* 创建页面 */
                            lv_show_filetxt(lv_flie.lv_return_page);              /* 在页面显示txt文件数据 */
                        }
                    }
                    else if (strstr(lv_flie.lv_pname,".bin") != NULL)             /* 如果不是文件夹且是bin文件 */
                    {
                        lv_flie.lv_return_page = lv_create_page(lv_scr_act());    /* 创建页面 */
                        lv_show_imgbin(lv_flie.lv_return_page,lv_pash_joint());   /* 显示bin图片 */
                    }
                    else if (strstr(lv_flie.lv_pname,".bmp") != NULL)             /* 如果不是文件夹且是bmp文件 */
                    {
                        lv_flie.lv_return_page = lv_create_page(lv_scr_act());    /* 创建页面 */
                        lv_show_imgbmp(lv_flie.lv_return_page,lv_pash_joint());   /* 显示bmp图片 */
                    }
                    else if (strstr(lv_flie.lv_pname,".jpeg") != NULL||strstr(lv_flie.lv_pname,".jpg") != NULL) /* 如果不是文件夹且是jpg文件 */
                    {
                        lv_flie.lv_return_page = lv_create_page(lv_scr_act());    /* 创建页面 */
                        lv_show_imgjpeg(lv_flie.lv_return_page,lv_pash_joint());  /* 显示jpeg图片 */
                    }

                    lv_flie.lv_suffix_flag = 1; /* 恢复文件夹点击 */
                }
            }
        }
    }
}

/**
  * @brief  读取文件名
  * @param  char* path: 要扫描的文件路径
  * @retval FR_OK：成功，否则失败
  */
uint16_t lv_scan_files (const char* path,lv_obj_t *parent)
{
    lv_flie.fr = f_opendir(&lv_flie.lv_dir, path);         /* 打开文件目录* */
    memset(lv_flie.list_btn,0,sizeof(lv_flie.list_btn));   /* 清除列表项的数组 */
    lv_flie.list_flie_nuber = 0;                           /* 设置文件索引为0 */
    lv_create_list(parent);                                /* 创建列表 */

    if (lv_flie.pname != NULL||lv_flie.lv_pname != NULL)
    {
        myfree(SRAMIN, lv_flie.pname);                    /* 释放pname内存 */
        myfree(SRAMIN, lv_flie.lv_pname);                 /* 释放lv_pname_shift内存 */
    }

    if (lv_flie.fr == FR_OK)
    {   /* 如果打开成功循环读出文件名字到buff中 */
        while(1)
        {   /* 循环读出文件名字，循环次数等于SD卡根目录下的文件数目 */
            lv_flie.fr = f_readdir(&lv_flie.lv_dir, &lv_flie.SD_fno);        /* 读取文件名* */

            if (lv_flie.fr != LV_FS_RES_OK||lv_flie.SD_fno.fname[0] == 0) break;  /* 读取错误或者读完所有文件结束就跳出循环 */
            lv_flie.list_flie_nuber ++;     /* 文件数量加1 */
           
          if (lv_flie.SD_fno.fattrib& AM_DIR) /* 读取的是文件夹名字 */
            {
               
                /* 复制文件名字到缓存并打印文件名 */
                lv_flie.list_btn[lv_flie.list_flie_nuber] = lv_list_add_btn(lv_flie.list, LV_SYMBOL_DIRECTORY, lv_flie.SD_fno.fname);      /* 添加列表项(文件夹) */
                lv_obj_set_style_img_recolor_filtered(lv_flie.list_btn[lv_flie.list_flie_nuber],lv_color_hex(0xFFD700),LV_STATE_DEFAULT);  /* 设置文件夹重新着色 */
            }
            else /* 读取的是文件名称 */
            {
              
                if (strstr(lv_flie.SD_fno.fname,".png") != NULL    /* 判断文件名称 */
                    ||strstr(lv_flie.SD_fno.fname,".jpeg") != NULL
                    ||strstr(lv_flie.SD_fno.fname,".jpg") != NULL
                    ||strstr(lv_flie.SD_fno.fname,".bmp") != NULL
                    ||strstr(lv_flie.SD_fno.fname,".gif") != NULL
                    ||strstr(lv_flie.SD_fno.fname,".avi") != NULL)
                {
                    lv_flie.image_scr = LV_SYMBOL_IMAGE;
                }
                else
                {
                    lv_flie.image_scr = LV_SYMBOL_FILE;
                }
                
                lv_flie.list_btn[lv_flie.list_flie_nuber] = lv_list_add_btn(lv_flie.list, lv_flie.image_scr, lv_flie.SD_fno.fname);         /* 添加列表项(文件) */ 
                lv_obj_set_style_img_recolor_filtered(lv_flie.list_btn[lv_flie.list_flie_nuber],lv_color_hex(0x87CEFA),LV_STATE_DEFAULT);   /* 设置文件重新着色 */
            }
            
            lv_obj_set_style_pad_left(lv_flie.list_btn[lv_flie.list_flie_nuber],5,LV_STATE_DEFAULT);   /* 设置列表项与列表左边的距离 */
            lv_obj_set_style_pad_right(lv_flie.list_btn[lv_flie.list_flie_nuber],5,LV_STATE_DEFAULT);  /* 设置列表项与列表右边的距离 */
            lv_obj_add_event_cb(lv_flie.list_btn[lv_flie.list_flie_nuber], lv_list_btn_event,LV_EVENT_ALL,NULL); /* 设置列表回调函数 */

        }
        
        f_closedir(&lv_flie.lv_dir); /* 关闭文件目录 */
    }
    
    return lv_flie.fr;                   /* 返回 */
}

/**
  * @brief  删除列表
  * @param  parent: 父类
  * @retval 无
  */
void lv_del_list(lv_obj_t *parent)
{
    lv_obj_del(parent);   /* 删除对象 */
    lv_flie.list = NULL;  /* 设置列表为空 */
}

/**
  * @brief  设置列表动画
  * @param  parent: 父类
  * @retval 无
  */
void lv_animation(lv_obj_t *parent)
{
    lv_anim_t a;                                                /* 第一步：定义一个动画 */
    lv_anim_init(&a);                                           /* 第二步初始化动画 */
    lv_anim_set_var(&a, parent);                                /* 第三步：设置实施动画效果的控件 */
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);  /* 设置动画功能 var */
    int32_t start = lv_obj_get_width(lv_scr_act());
    int32_t end = 0;
    lv_anim_set_values(&a, start, end);                         /* 设置开始值和结束值。例如0,150 */
    lv_anim_set_time(&a, 300);                                  /* 动画的长度[ms]设置300mS */
    lv_anim_start(&a);                                          /* 第四步：创建动画 */
}

/**
  * @brief  创建列表
  * @param  parent: 父类
  * @retval 无
  */
void lv_create_list(lv_obj_t *parent)
{
    lv_flie.list = lv_list_create(parent);  /* 创建列表 */
    lv_animation(lv_flie.list);
    lv_obj_set_size(lv_flie.list, lcddev.width, lcddev.height - myFont24.line_height*2 - 10);  /* 设置列表的大小 */
    lv_obj_align_to(lv_flie.list,lv_flie.lv_page_obj,LV_ALIGN_OUT_BOTTOM_LEFT,0,0);            /* 设置列表的对齐模式 */
    lv_obj_set_style_text_font(lv_flie.list,&lv_font_montserrat_24,LV_STATE_DEFAULT);          /* 设置字体 */
    lv_obj_set_style_radius(lv_flie.list,0,LV_STATE_DEFAULT);/* 设置圆半径为0 */
}

/**
  * @brief  创建页面标题
  * @param  parent: 父类
  * @retval 无
  */
void lv_page_tile(lv_obj_t *parent)
{
    lv_flie.lv_page_obj = lv_obj_create(parent);
    lv_obj_set_size(lv_flie.lv_page_obj,lcddev.width,myFont24.line_height);
    lv_obj_set_style_bg_color(lv_flie.lv_page_obj,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_flie.lv_page_obj,0,LV_STATE_DEFAULT);/* 设置圆半径为0 */
    lv_obj_align_to(lv_flie.lv_page_obj,parent,LV_ALIGN_TOP_LEFT,0,0);
    
    lv_obj_t *lv_page_label = lv_label_create(lv_flie.lv_page_obj);
    lv_label_set_text(lv_page_label,"文件管理系统");
    lv_obj_set_style_text_color(lv_page_label,lv_palette_main(LV_PALETTE_RED),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_page_label,&myFont24,LV_STATE_DEFAULT);
    lv_obj_align_to(lv_page_label,lv_flie.lv_page_obj,LV_ALIGN_CENTER,0,0);
}

/**
  * @brief  返回按键回调函数
  * @param  obj:对象
  * @param  event:事件
  * @retval 无
  */
void lv_back_btn_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_SHORT_CLICKED)
    {
        if (obj == lv_flie.lv_back_btn)
        {
            lv_del_list(lv_flie.list);         /* 删除列表 */
            list_init(lv_scr_act());           /* 返回菜单 */
        }
        if (obj == lv_flie.lv_prev_btn)
        { 
            lv_flie.lv_prev_file_flag -- ;     /* 文件返回标志位减一 */

            if (lv_flie.lv_prev_file_flag <= 0)
            {
                lv_flie.lv_prev_file_flag = 0; /* 如果小于等于0，就强制设置为0 */
            }
          
            lv_del_list(lv_flie.list);         /* 立刻删除列表控件 */
            lv_flie.lv_pash = lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag]; /* 上级路径复制到lv_pash参数 */
            lv_scan_files(lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag],lv_scr_act());/* 读取文件路径 */
           
        }
    }
}

/**
  * @brief  返回按键
  * @param  parent:父类
  * @retval 无
  */
void lv_general_win_create(lv_obj_t *parent)
{
    lv_flie.lv_back_btn = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_flie.lv_back_btn,&myFont24,LV_STATE_DEFAULT); /* 设置字体 */
    
    lv_label_set_text(lv_flie.lv_back_btn,"菜单");
    lv_obj_align_to(lv_flie.lv_back_btn,parent,LV_ALIGN_RIGHT_MID,-10,0);
    lv_obj_add_flag(lv_flie.lv_back_btn,LV_OBJ_FLAG_CLICKABLE); /* 设置标签可点击 */
    lv_obj_add_event_cb(lv_flie.lv_back_btn,lv_back_btn_event_handler,LV_EVENT_ALL,NULL); /* 设置回调函数 */
    lv_flie.lv_prev_btn = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_flie.lv_prev_btn,&myFont24,LV_STATE_DEFAULT);
    lv_label_set_text(lv_flie.lv_prev_btn,"返回");
    lv_obj_align_to(lv_flie.lv_prev_btn,parent,LV_ALIGN_LEFT_MID,10,0);
    lv_obj_add_flag(lv_flie.lv_prev_btn,LV_OBJ_FLAG_CLICKABLE); /* 设置标签可点击 */
    lv_obj_add_event_cb(lv_flie.lv_prev_btn,lv_back_btn_event_handler,LV_EVENT_ALL,NULL);
}

/**
  * @brief  文件系统返回/菜单按键的区域
  * @param  parent: 父类
  * @retval 无
  */
void lv_page_back(lv_obj_t *parent)
{
    lv_flie.lv_back_obj = lv_obj_create(parent);                                                            /* 创建文件返回对象区域 */
    lv_obj_set_size(lv_flie.lv_back_obj,lcddev.width,myFont24.line_height+10);                              /* 设置改区域的大小 */
    lv_obj_set_style_bg_color(lv_flie.lv_back_obj,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);       /* 设置该区域的颜色为灰色 */
    lv_obj_set_style_radius(lv_flie.lv_back_obj,0,LV_STATE_DEFAULT);                                        /* 设置圆半径为0 */
    lv_obj_align_to(lv_flie.lv_back_obj,parent,LV_ALIGN_BOTTOM_MID,0,0);                                    /* 设置对齐模式 */
    lv_general_win_create(lv_flie.lv_back_obj);                                                             /* 创建返回和菜单按键 */
}

/**
  * @brief  列表初始化
  * @param  parent: 父类
  * @retval 无
  */
void list_init(lv_obj_t *parent)
{
    lv_flie.lv_pash = "0:";                                 /* 初始路径 */
    lv_flie.lv_prev_file_flag = 0;                          /* 上一个文件路径标志位清0 */
    lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag] = "0:"; /* 初始上个文件夹路径 */
    lv_flie.list_flie_nuber = 0;                            /* 初始文件数量 */
    lv_flie.lv_suffix_flag = 1;                             /* 文件后缀标志位 */
    lv_scan_files(lv_flie.lv_pash,parent);                  /* 读取文件路径 */
}

/**
  * @brief  控件测试函数
  * @param  无
  * @retval 无
  */
void lv_mainstart(void)
{
    lv_page_tile(lv_scr_act());         /* 设置页面标题 */
    lv_page_back(lv_scr_act());         /* 设置页面返回 */
    list_init(lv_scr_act());            /* 列表初始化 */
}
