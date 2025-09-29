/**
 ****************************************************************************************************
 * @file        filelistbox.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-05-26
 * @brief       GUI-文件列表框 代码
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
 * V1.1 20140715
 * 1,修改滚动条检测机制,使用更方便
 * 2,去掉FLBOX_PATH_HEIGHT等宏定义,改用变量方式控制(在gui_phy结构体里面设置)
 * V1.2 20160528
 * 根据最新fatfs(0.12版本),修改文件索引保存方式,采用dptr方式.
 * V1.3 20160901
 * 1,将所有内存申请都放到内部SRAM!!
 * 2,修正filelistbox_get_pathdepth函数的bug,添加对参数地址为0时的判断.
 * V1.4 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __FILELISTBOX_H
#define __FILELISTBOX_H

#include "guix.h"
#include "scrollbar.h"


/* 文件图标定义 */
#define FICO_DISK               0           /* 磁盘 */
#define FICO_FOLDER             1           /* 文件夹 */
#define FICO_UNKNOW             2           /* 未知文件 */
#define FICO_BIN                3           /* BIN文件 */
#define FICO_LRC                4           /* LRC文件 */
#define FICO_NES                5           /* NES文件 */
#define FICO_TEXT               6           /* 文本文件 */
#define FICO_MUSIC              7           /* 音乐文件 */
#define FICO_PICTURE            8           /* 图片文件 */
#define FICO_VIDEO              9           /* 视频文件 */

/* 文件过滤器个位功能定义 */
#define FLBOX_FLT_BIN           1<<0        /* BIN文件 */
#define FLBOX_FLT_LRC           1<<1        /* LRC文件 */
#define FLBOX_FLT_NES           1<<2        /* NES文件 */
#define FLBOX_FLT_TEXT          1<<3        /* 文本文件 */
#define FLBOX_FLT_MUSIC         1<<4        /* 音乐文件 */
#define FLBOX_FLT_PICTURE       1<<5        /* 图片文件 */
#define FLBOX_FLT_VIDEO         1<<6        /* 视频文件 */
#define FLBOX_FLT_UNKNOW        1<<15       /* 无法识别的一类文件 */

/* filelistbox上的符号的默认颜色 */
#define FLBOX_DFT_LBKCOLOR      0XFFFF      /* 内部背景色 */
#define FLBOX_DFT_LSELCOLOR     0XFFFF      /* 选中list后的字体颜色 */
#define FLBOX_DFT_LSELBKCOLOR   0X001F      /* 0X07E0, 选中list后的背景色 */
#define FLBOX_DFT_LNCOLOR       0X0000      /* 未选中的list字体颜色 */
#define FLBOX_DFT_RIMCOLOR      0XF800      /* 边框颜色 */
#define FLBOX_DFT_PTHINFBKCOLOR 0XCE7E      /* 路径/信息栏的背景颜色 */

/* filelistbox文件存储深度定义,如果超过这个深度了,将不再记忆上一层所选项目的位置 */
#define FLBOX_PATH_DEPTH        0X0A        /* 文件路径深度 */

/* filelistbox单个文件夹下面的最大目标文件索引数目 */
#define FLBOX_MAX_DFFILE        2000        /* 使用12*12的字体 2K个,在搜索文件夹的时候,需要占用2K*2的内存 */

/* 固定宽高度定义 */
#define FLBOX_SCB_WIDTH         0X0E        /* 滚动条宽度为15个像素 */


/* filelist结构体.链表结构 */
typedef __PACKED_STRUCT
{
    void *prevlist;
    void *nextlist;
    uint8_t type;       /* 文件类型 */
    uint8_t *name;      /* 文件名字 */
} _filelistbox_list;

/* filelistbox结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;           /* filelistbox顶端坐标 */
    uint16_t left;          /* filelistbox左端坐标 */
    uint16_t width;         /* 宽度 */
    uint16_t height;        /* 高度 必须为12/16的倍数,如果有边框,在倍数基础上加2 */

    uint8_t type;           /**
                             * 类型标记字
                             * [bit7]:1,需要画滚动条出来(条件是totalitems>itemsperpage);  0,不需要画出来.(此位由软件自动控制)
                             * [bit6:0]:文件类型 
                             */

    uint8_t sta;            /* filelistbox状态,[bit7]:滑动标志;[bit6]:编号有效的标志;[bit5:0]:第一次按下的编号 */
    uint8_t id;             /* filelistbox 的id */
    uint8_t dbclick;        /**
                             * 双击
                             * [7]:0,没有双击.1,有双击.
                             * [0]:0,非目标文件(是文件夹/磁盘).1,目标文件(非文件夹/磁盘)
                             */

    uint8_t font;           /* 文字字体 12/16 */
    uint16_t selindex;      /* 选中的索引 0<selindex<foldercnt+filecnt */

    uint16_t foldercnt;     /* 文件夹数目 */
    uint16_t filecnt;       /* 文件数目 */

    uint8_t icosize;        /* ico图标大小 16/20/28等 */

    uint16_t lbkcolor;      /* 内部背景颜色 */
    uint16_t lnselcolor;    /* list name 选中后的颜色 */
    uint16_t lnselbkcolor;  /* list name 选中后的背景颜色 */
    uint16_t lncolor;       /* list name 未选中的颜色 */
    uint16_t rimcolor;      /* 边框颜色 */
    uint16_t pthinfbkcolor; /* 路径/信息栏的背景颜色 */

    uint8_t *path;          /* 父目录路径 */
    //uint8_t  seltblindex; /* 当前的目录层数 */
    uint16_t seltbl[FLBOX_PATH_DEPTH];  /* 选中的条目列表,FLBOX_PATH_DEPTH为深度 */
    uint16_t fliter;        /* 文件过滤器,大类过滤 */
    
    /* 刷选表见上面的定义 */
    uint16_t fliterex;      /* 文件扩展的过滤器 */
                            /* 用来选择大类里面的小类,最多支持16种不同的文件.默认0XFFFF,即不对小类进行刷选 */

    uint8_t memdevflag;     /** 
                             * 磁盘在位状态(最多可以表示8个磁盘)
                             * bit7:0,存储设备7~0在位状态;0,不在位;1,在位;
                             * 本例中,存储设备0表示:SD卡
                             *               1表示:SPI FLASH
                             *               2表示:U盘
                             */
                             
    uint8_t *fname;         /* 当前选中的index的名字 */
    uint16_t namelen;       /* name所占的点数 */
    uint16_t curnamepos;    /* 当前的偏移 */
    uint32_t oldtime;       /* 上一次更新时间 */

    /* 以下只与目标文件相关(不含文件夹) */
    uint32_t *findextbl;    /* 目标文件的dptr索引表 */

    _scrollbar_obj *scbv;   /* 垂直滚动条 */
    _filelistbox_list *list;/* 链表 */
} _filelistbox_obj;


_filelistbox_obj *filelistbox_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t type, uint8_t font);  /* 创建filelistbox */
_filelistbox_list *filelist_search(_filelistbox_list *filelistx, uint16_t index);   /* 查找编号为index的list,并取得详细信息 */
void filelist_delete(_filelistbox_obj *filelistbox);    /* 删除filelist链表 */
void filelistbox_delete(_filelistbox_obj *filelistbox_del); /* 删除filelistbox */
uint8_t filelistbox_rebuild_filelist(_filelistbox_obj *filelistbox);    /* 重建filelistbox */
uint8_t filelistbox_check(_filelistbox_obj *filelistbox, void *in_key); /* 检查filelistbox的按下状态 */
uint8_t filelistbox_addlist(_filelistbox_obj *filelistbox, uint8_t *name, uint8_t type);    /* 增加一个filelist */
void filelistbox_draw_list(_filelistbox_obj *filelistbox);              /* 画filelist */
void filelistbox_draw_listbox(_filelistbox_obj *filelistbox);           /* 画filelistbox */
void filelistbox_show_path(_filelistbox_obj *filelistbox);              /* 显示路径 */
void filelistbox_show_info(_filelistbox_obj *filelistbox);
uint8_t   filelistbox_scan_filelist(_filelistbox_obj *filelistbox);
void filelistbox_add_disk(_filelistbox_obj *filelistbox);
void filelistbox_check_filelist(_filelistbox_obj *filelistbox);
void filelistbox_back(_filelistbox_obj *filelistbox);
void filelistbox_2click_hook(_filelistbox_obj *filelistbox);            /* 双击钩子函数 */
#endif





