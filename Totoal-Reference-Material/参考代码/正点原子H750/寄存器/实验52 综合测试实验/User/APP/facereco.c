/**
 ****************************************************************************************************
 * @file        facereco.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-人脸识别 代码
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

#include "facereco.h"
#include "camera.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/timer.h"
#include "./T9INPUT/t9input.h"
#include "audioplay.h"
#include "appplay.h"
#include "atk_frec.h"


/* 在setings.c里面定义 */
extern uint8_t *const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM];

/* 在camera.c里面定义 */
extern uint8_t *const camera_remind_tbl[4][GUI_LANGUAGE_NUM];

uint16_t reg_time = 0;              /* 识别一个人脸所用时间 */
uint16_t frec_curline = 0;          /* 摄像头输出数据,当前行编号 */
uint32_t *frec_dcmi_line_buf[2];    /* RGB屏时,摄像头采用一行一行读取,定义行缓存 */
_frec_obj frec_dev;                 /* 人脸识别控制结构体 */

/* 提示信息 */
uint8_t *const frec_remind_msg_tbl[14][GUI_LANGUAGE_NUM] =
{
    "提示:无可用模板,请先添加模板", "提示:無可用模板,請先添加模板", "Remind:No Model,Pls Add Model",
    "选择操作:", "選擇操作:", "Choose option:",
    "人脸列表", "人臉列表", "Face List",
    "编辑人脸", "編輯人臉", "Edit Face",
    "添加人脸", "添加人臉", "Add Face",
    "识别结果", "識別結果", "Result",
    "正在保存...", "正在保存...", "Saving...",
    "保存成功!", "保存成功!", "Save OK!",
    "保存失败!", "保存失敗!", "Save Failed!",
    "删除成功!", "刪除成功!", "Delete OK!",
    "删除失败!", "刪除失敗!", "Delete Failed!",
    "正在识别...", "正在識別...", "Recognizing...",
    "请先按KEY_UP添加模板!", "請先按KEY_UP添加模板!", "Press KEY_UP add face!",
    "无法识别!", "無法識別!", "Recognize Failed!",
};

/* 选项框条目 */
uint8_t *const frec_option_tbl[GUI_LANGUAGE_NUM][2] =
{
    {"编辑", "删除"},
    {"編輯", "刪除"},
    {"Edit", "Delete"},
};

/* 保存/取消按钮 */
uint8_t *const frec_sc_btn_tbl[GUI_LANGUAGE_NUM][2] =
{
    {"保存", "取消"},
    {"保存", "取消"},
    {"Save", "Cancel"},
};

/* 名片信息 */
uint8_t *const frec_card_info_tbl[GUI_LANGUAGE_NUM][4] =
{
    {" 姓名:", " 电话:", " 性别:", " 耗时:"},
    {" 姓名:", " 電話:", " 性別:", " 耗時:"},
    {" Name:", "Phone:", " Sex :", " Time:"},
};

/****************************************************************************************************/
/* 人脸识别链表处理部分 */

/**
 * @brief       创建FREC节点
 * @param       无
 * @retval      节点首地址
 */
frec_node *frec_node_creat(void)
{
    frec_node *pnode;
    pnode = atk_frec_malloc(sizeof(frec_node));

    if (pnode)
    {
        pnode->pname = 0;   /* 姓名指针清零 */
        pnode->pnum = 0;    /* 号码指针清零 */
        pnode->sex = 0;     /* 性别指针清零 */
        pnode->index = 0;   /* 默认编号为0 */
        pnode->next = 0;
    }

    return pnode;
}

/**
 * @brief       释放一个节点的内存
 * @param       pnode           : 节点地址
 * @param       mode            : 0,仅仅释放节点内容(只删除内容,节点头地址保留.)
 *                                1,包括节点头也释放掉(整个节点删除)
 * @retval      无
 */
void frec_node_free(frec_node *pnode, uint8_t mode)
{
    if (pnode == NULL)return;   /* 空节点 */

    if (pnode->pname)atk_frec_free(pnode->pname);   /* 释放pname内存 */

    if (pnode->pnum)atk_frec_free(pnode->pnum);     /* 释放pnum内存 */
 
    if (pnode->sex)atk_frec_free(pnode->sex);       /* 释放sex内存 */

    if (mode == 1)atk_frec_free(pnode);             /* 释放pnode内存 */
}

/**
 * @brief       在地i个节点后添加一个节点
 * @param       head            : 表头
 * @param       pnew            : 新节点
 * @param       i               : 添加在第几个节点之后,0XFFFF,在末尾添加
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t frec_node_insert(frec_node *head, frec_node *pnew, uint16_t i)
{
    uint16_t j;
    frec_node *p, *p1;
    p = p1 = head;

    for (j = 0; j < i && p1 != NULL; j++)
    {
        p1 = p1->next;

        if (p1 != NULL)
        {
            p = p1;
        }
    }

    if (p1 == NULL && (i != 0XFFFF))return 1; /* 无法插入第i个节点 */

    pnew->next = p->next;   /* 插入节点的next指向后续节点 */
    p->next = pnew;         /* 新节点插入第i个节点之后 */
    return 0;   /* 插入成功 */
}

/**
 * @brief       删除一个节点
 * @param       head            : 表头
 * @param       i               : 要删除的节点号,0,表示表头(表头不能用该函数删除)
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t frec_node_delete(frec_node *head, uint16_t i)
{
    uint16_t j;
    frec_node *p, *p1 = 0;

    if (i == 0)return 1;

    p = head;

    for (j = 0; j < i && p->next != NULL; j++)
    {
        p1 = p;
        p = p->next;
    }

    if (p == NULL || p1 == NULL)return 2; /* 不存在的节点 */

    p1->next = p->next;     /* 更改前一个节点的指向 */
    frec_node_free(p, 1);   /* 释放节点内存 */
    return 0;   /* 删除成功 */
}

/**
 * @brief       得到一个节点信息
 * @param       head            : 表头
 * @param       i               : 要得到信息的节点号,0,表示表头
 * @retval      0, 获取信息失败; 其他, 节点地址;
 */
frec_node *frec_node_getnode(frec_node *head, uint16_t i)
{
    uint16_t j;
    frec_node *p;
    p = head;

    for (j = 0; j < i && p != NULL; j++)
    {
        p = p->next;
    }

    return p;   /* 返回节点地址 */
}

/**
 * @brief       得到一个节点信息
 * @param       head            : 表头
 * @param       index           : 要得到的节点的index编号
 * @retval      0, 获取信息失败; 其他, 节点地址;
 */
frec_node *frec_node_getnode_index(frec_node *head, uint8_t index)
{
    frec_node *p;
    p = head;

    while (p != NULL)
    {
        if (p->index == index)break;

        p = p->next;

    }

    return p;   /* 返回节点地址 */
}

/**
 * @brief       得到整个链表大小
 * @param       head            : 表头
 * @retval      链表大小(节点个数)
 */
uint16_t frec_node_getsize(frec_node *head)
{
    uint16_t j;
    frec_node *p;
    p = head;

    for (j = 0; p != NULL; j++)
    {
        p = p->next;
    }

    return j;   /* 返回节点地址 */
}

/**
 * @brief       删除整个链表
 * @param       head            : 表头
 * @retval      无
 */
void frec_node_destroy(frec_node *head)
{
    frec_node *p, *p1;
    p = head;

    while (p->next != NULL)
    {
        p1 = p->next;
        p->next = p1->next;
        frec_node_free(p1, 1);  /* 删除一个节点 */
    }

    frec_node_free(p, 1);   /* 删除最后一个节点 */
}
/****************************************************************************************************/


#define ATK_FREC_INFO_PNAME     "0:ATK_FREC/INFO_%03d.bin"      /* 数据模型存储文件夹+文件名,存储在NAND FLASH */

/**
 * @brief       保存一个人脸信息数据
 *  @note       实际上就是保存一个文件.
 *              文件格式:
 *              首先是图片数据,图片尺寸(90*120),然后30个字节,存储姓名.接着20个字节,存储号码;最后1个字节存储性别.
 *              即: 图片数据(90*120*2字节)+名字(30字节)+号码(20字节)+性别(20个字节)
 *              文件总长度: 90*120*2+30+20+20= 21670字节
 * @param       frecdev         : 人脸识别控制结构体
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_save_a_faceinfo(_frec_obj *frecdev)
{
    uint8_t *path;
    FIL *fp;
    uint32_t fw;
    uint8_t res = 0;
    uint8_t *p = (uint8_t *)frecdev->databuf;
    p += 90 * 120 * 2;/* 偏移到图像数据末尾 */
    memset(p, 0, 70);/* 清零 */

    if (frecdev->face->pname)sprintf((char *)p, "%s", frecdev->face->pname); /* 存储姓名 */

    p += 30;

    if (frecdev->face->pnum)sprintf((char *)p, "%s", frecdev->face->pnum);  /* 存储号码 */

    p += 20;

    if (frecdev->face->sex)sprintf((char *)p, "%s", frecdev->face->sex);    /* 存储性别 */

    path = gui_memin_malloc(30);        /* 申请内存 */
    fp = gui_memin_malloc(sizeof(FIL)); /* 申请内存 */

    if (!fp)
    {
        gui_memin_free(path);
        return 1;
    }

    sprintf((char *)path, ATK_FREC_INFO_PNAME, frecdev->face->index);
    res = f_open(fp, (char *)path, FA_WRITE | FA_CREATE_ALWAYS);

    if (res == FR_OK)
    {
        res = f_write(fp, (uint8_t *)frecdev->databuf, 90 * 120 * 2 + 70, &fw);/* 写入文件 */
    }

    f_close(fp);

    if (res)res = 2;

    gui_memin_free(path);
    gui_memin_free(fp);
    return res;
}

/**
 * @brief       删除一个人脸信息数据
 * @param       index           : 要保存的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_delete_a_faceinfo(uint8_t index)
{
    uint8_t *path;
    uint8_t res;
    path = gui_memin_malloc(30);    /* 申请内存 */

    if (!path)
    {
        return ATK_FREC_MEMORY_ERR;
    }

    sprintf((char *)path, ATK_FREC_INFO_PNAME, index);
    res = f_unlink((char *)path);
    gui_memin_free(path);
    return res;
}

/**
 * @brief       加载卡片界面
 * @param       xoff,yoff       : 起始坐标
 * @param       width,height    : 窗口大小
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_card_ui(uint16_t xoff, uint16_t yoff, uint16_t width, uint16_t height)
{
    _window_obj *twin = 0;/* 窗体 */
    twin = window_creat(xoff, yoff, width, height, 0, 0X01, 16); /* 创建窗口 */

    if (twin)
    {
        twin->captionbkcu = APPPLAY_IN_BACKCOLOR;   /* 默认caption上部分背景色 */
        twin->captionbkcd = APPPLAY_IN_BACKCOLOR;   /* 默认caption下部分背景色 */
        twin->captioncolor = APPPLAY_IN_BACKCOLOR;  /* 默认caption的颜色 */
        twin->windowbkc = APPPLAY_IN_BACKCOLOR;     /* 默认背景色 */

        window_draw(twin);
    }
    else  return 1;

    window_delete(twin);
    return 0;
}

/**
 * @brief       显示识别结果
 * @param       pnode           : 人脸节点
 * @param       frecdev         : 人脸识别控制结构体
 * @param       time            : 耗时.(单位:ms)
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_show_result(frec_node *pnode, _frec_obj *frecdev, uint16_t time)
{
    uint8_t fontsize;       /* 字体尺寸 */
    uint16_t cxoff, cyoff, cwidth, cheight; /* 名片尺寸 */
    uint8_t cixoff, ciyoff; /* 这些偏移都是在card里面的偏移(对lcd的偏移,需要加上卡片的偏移) */
    uint16_t review_width;  /* 预览宽度 */
    uint8_t review_yoff;

    uint8_t res;
    uint8_t *pstr = 0;

    if (lcddev.width == 240)
    {
        fontsize = 12;
        cyoff = 20 + 3 + 140 + 3, cwidth = 230;
        cheight = 150;
        cixoff = 5, ciyoff = 10;
        review_width = 105, review_yoff = 20 + 3;
    }
    else if (lcddev.width == 272)
    {
        fontsize = 16;
        cxoff = 6, cyoff = 24 + 18 + 200 + 18, cwidth = 260;
        cheight = 200;
        cixoff = 8, ciyoff = 30;
        review_width = 110, review_yoff = 24 + 18;
    }
    else if (lcddev.width == 320)
    {
        fontsize = 16;
        cyoff = 24 + 18 + 200 + 18, cwidth = 300;
        cheight = 200;
        cixoff = 20, ciyoff = 30;
        review_width = 150, review_yoff = 24 + 18;
    }
    else if (lcddev.width == 480)
    {
        fontsize = 24;
        cyoff = 32 + 49 + 320 + 49, cwidth = 450;
        cheight = 300;
        cixoff = 40, ciyoff = 50;
        review_width = 240, review_yoff = 32 + 49;
    }
    else if (lcddev.width == 600)
    {
        fontsize = 24;
        cyoff = 32 + 60 + 400 + 60, cwidth = 450;
        cheight = 300;
        cixoff = 40, ciyoff = 50;
        review_width = 300, review_yoff = 32 + 49;
    }
    else if (lcddev.width == 800)
    {
        fontsize = 24;
        cyoff = 40 + 80 + 500 + 70, cwidth = 600;
        cheight = 380;
        cixoff = 50, ciyoff = 60;
        review_width = 400, review_yoff = 32 + 40;
    }

    cxoff = (lcddev.width - cwidth) / 2;

    pstr = gui_memin_malloc(100);

    if (pstr)
    {
        frec_get_image_data(frecdev->databuf, frecdev->xoff, frecdev->yoff, frecdev->width, review_width); /* 读取预览图片 */

        gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, APPPLAY_EX_BACKCOLOR); /* 填充背景色 */
        app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);  /* 下分界线 */
        gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)frec_remind_msg_tbl[5][gui_phy.language]); /* 显示标题-识别结果 */

        frec_show_picture((lcddev.width - review_width) / 2, review_yoff, review_width, review_width * 4 / 3, frec_dev.databuf); /* 显示预览图片 */
        appplay_frec_card_ui(cxoff, cyoff, cwidth, cheight);    /* 显示名片外框 */

        res = appplay_frec_read_a_face(pnode, frecdev, pnode->index); /* 读取人脸信息数据 */

        if (res == 0)
        {

            frec_show_picture(cxoff + cwidth - cixoff - 90, cyoff + ciyoff, 90, 120, frec_dev.databuf); /* 显示预览图片 */
            sprintf((char *)pstr, "%s%s", frec_card_info_tbl[gui_phy.language][0], pnode->pname);
            gui_show_string(pstr, cxoff + cixoff, cyoff + ciyoff, cwidth, fontsize, fontsize, BLACK);   /* 显示姓名 */
            sprintf((char *)pstr, "%s%s", frec_card_info_tbl[gui_phy.language][1], pnode->pnum);
            gui_show_string(pstr, cxoff + cixoff, cyoff + ciyoff + fontsize * 2, cwidth, fontsize, fontsize, BLACK);    /* 显示性别 */
            sprintf((char *)pstr, "%s%s", frec_card_info_tbl[gui_phy.language][2], pnode->sex);
            gui_show_string(pstr, cxoff + cixoff, cyoff + ciyoff + fontsize * 4, cwidth, fontsize, fontsize, BLACK);    /* 显示电话 */
            sprintf((char *)pstr, "%s%dms", frec_card_info_tbl[gui_phy.language][3], time);
            gui_show_string(pstr, cxoff + cixoff, cyoff + ciyoff + fontsize * 6, cwidth, fontsize, fontsize, RED);      /* 显示耗时 */
            system_task_return = 0;

            while (1)
            {
                delay_ms(1000 / OS_TICKS_PER_SEC);  /* 延时一个时钟节拍 */

                if (system_task_return)break;       /* TPAD返回 */
            }
        }

    }
    else res = 0XFF;

    gui_memin_free(pstr);
    return res;
}

/**
 * @brief       添加/编辑人脸
 * @param       pnode           : 人脸节点
 * @param       frecdev         : 人脸识别控制结构体
 * @param       mode            : 0,编辑人脸;  1,添加人脸.
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_add_a_face(frec_node *pnode, _frec_obj *frecdev, uint8_t mode)
{
    _btn_obj *okbtn = 0, *cbtn = 0; /* 完成和取消按钮 */
    _t9_obj *t9 = 0;                /* 输入法 */
    _edit_obj *tname = 0, *tphone = 0, *tsex = 0;
    uint16_t *pimage = 0;

    uint8_t fontsize;               /* 字体尺寸 */
    uint16_t cxoff, cyoff, cwidth, cheight;     /* 名片尺寸 */
    uint8_t cixoff, ciyoff;         /* 这些偏移都是在card里面的偏移(对lcd的偏移,需要加上卡片的偏移) */
    uint8_t btn_height;             /* 按键高度(宽度固定为高度的3倍) */
    uint8_t btnpit;                 /* 按键间隔 */
    uint16_t t9height;

    uint8_t res = 0, rval = 0;
    uint8_t person;
    uint8_t editflag = 0;       /**
                                 * 默认编辑姓名
                                 * 0,编辑姓名
                                 * 1,编辑电话
                                 * 2,编辑性别
                                 */

    if (lcddev.width == 240)
    {
        fontsize = 12;
        cyoff = 5 + 20, cwidth = 230;
        cheight = 156; /* cheight=3*ciyoff+btn_height+120 */
        cixoff = 5,
        btn_height = 20;
        t9height = 134;
    }
    else if (lcddev.width == 272)
    {
        fontsize = 12;
        cyoff = 10 + 24, cwidth = 260;
        cheight = 200;
        cixoff = 12,
        btn_height = 25;
        t9height = 176;
    }
    else if (lcddev.width == 320)
    {
        fontsize = 16;
        cyoff = 40 + 24, cwidth = 300;
        cheight = 200;
        cixoff = 20,
        btn_height = 25;
        t9height = 176;
    }
    else if (lcddev.width == 480)
    {
        fontsize = 24;
        cyoff = 101 + 32, cwidth = 450;
        cheight = 300;
        cixoff = 40,
        btn_height = 40;
        t9height = 266;
    }
    else if (lcddev.width == 600)
    {
        fontsize = 24;
        cyoff = 120 + 32, cwidth = 550;
        cheight = 340;
        cixoff = 40,
        btn_height = 50;
        t9height = 368;
    }
    else if (lcddev.width == 800)
    {
        fontsize = 24;
        cyoff = 130 + 32, cwidth = 600;
        cheight = 380;
        cixoff = 50,
        btn_height = 60;
        t9height = 488;
    }

    btnpit = (cwidth - 2 * 3 * btn_height) * 3 / 7;
    cxoff = (lcddev.width - cwidth) / 2;
    ciyoff = (cheight - btn_height - 120) / 3;

    if (mode == 1)frec_get_image_data(frecdev->databuf, frecdev->xoff, frecdev->yoff, frecdev->width, 90); /* 添加人脸,读取90*120的预览图片 */
    else    /* 编辑人脸,则直接读取SD卡的人脸信息文件 */
    {
        res = appplay_frec_read_a_face(pnode, frecdev, pnode->index); /* 读取人脸信息数据 */
    }

    gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, APPPLAY_EX_BACKCOLOR); /* 填充背景色 */
    app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);/* 下分界线 */
    gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)frec_remind_msg_tbl[3 + mode][gui_phy.language]); /* 显示标题 */
    appplay_frec_card_ui(cxoff, cyoff, cwidth, cheight);/* 显示名片外框 */

    okbtn = btn_creat(cxoff + btnpit, cyoff + cheight - ciyoff - btn_height, btn_height * 3, btn_height, 0, 0); /* 创建标准按钮 */
    cbtn = btn_creat(cxoff + cwidth - btnpit - 3 * btn_height, cyoff + cheight - ciyoff - btn_height, btn_height * 3, btn_height, 0, 0);    /* 创建标准按钮 */
    tname = edit_creat(cxoff + cixoff + fontsize * 3, cyoff + ciyoff - 3, fontsize * 7 + 6, fontsize + 6, 0, 4, fontsize);                  /* 创建编辑框 */
    tphone = edit_creat(cxoff + cixoff + fontsize * 3, cyoff + ciyoff + fontsize * 2 - 3, fontsize * 7 + 6, fontsize + 6, 0, 4, fontsize);  /* 创建编辑框 */
    tsex = edit_creat(cxoff + cixoff + fontsize * 3, cyoff + ciyoff + fontsize * 4 - 3, fontsize * 7 + 6, fontsize + 6, 0, 4, fontsize);    /* 创建编辑框 */

    if (mode == 1)   /* 添加新人脸,需要申请内存 */
    {
        pnode->pname = atk_frec_malloc(30);     /* 申请30字节内存,用于存储姓名 */
        pnode->pnum = atk_frec_malloc(20);      /* 申请20字节内存,用于存储电话号码 */
        pnode->sex = atk_frec_malloc(20);       /* 申请20字节内存,用于存储性别信息 */
        pimage = atk_frec_malloc(30 * 40 * 2);  /* 申请30*40图片缓存 */
    }

    t9 = t9_creat((lcddev.width % 5) / 2, lcddev.height - t9height, lcddev.width - (lcddev.width % 5), t9height, 0);

    if (t9)   /* 申请成功 */
    {
        okbtn->caption = frec_sc_btn_tbl[gui_phy.language][0]; /* 保存按钮 */
        okbtn->font = fontsize;
        cbtn->caption = frec_sc_btn_tbl[gui_phy.language][1]; /* 取消按钮 */
        cbtn->font = fontsize;
        tname->textbkcolor = APPPLAY_IN_BACKCOLOR;  /* 设置颜色 */
        tname->textcolor = BLUE;
        tphone->textbkcolor = APPPLAY_IN_BACKCOLOR; /* 设置颜色 */
        tphone->textcolor = BLUE;
        tsex->textbkcolor = APPPLAY_IN_BACKCOLOR;   /* 设置颜色 */
        tsex->textcolor = BLUE;
        tname->type = 0X06; /* 光标闪烁 */

        if (mode == 0)   /* 编辑人脸 */
        {
            strcpy((char *)tname->text, (char *)pnode->pname);  /* 拷贝姓名 */
            strcpy((char *)tphone->text, (char *)pnode->pnum);  /* 拷贝电话 */
            strcpy((char *)tsex->text, (char *)pnode->sex);     /* 拷贝性别 */
        }

        gui_show_string(frec_card_info_tbl[gui_phy.language][0], cxoff + cixoff, cyoff + ciyoff, fontsize * 3, fontsize, fontsize, BLACK);
        gui_show_string(frec_card_info_tbl[gui_phy.language][1], cxoff + cixoff, cyoff + ciyoff + fontsize * 2, fontsize * 3, fontsize, fontsize, BLACK);
        gui_show_string(frec_card_info_tbl[gui_phy.language][2], cxoff + cixoff, cyoff + ciyoff + fontsize * 4, fontsize * 3, fontsize, fontsize, BLACK);

        frec_show_picture(cxoff + cwidth - cixoff - 90, cyoff + ciyoff, 90, 120, frec_dev.databuf); /* 显示预览图片 */
        edit_draw(tname);   /* 姓名编辑框 */
        edit_draw(tphone);  /* 电话编辑框 */
        edit_draw(tsex);    /* 性别编辑框 */
        btn_draw(okbtn);
        btn_draw(cbtn);
        t9_draw(t9);
        rval = 0;
    }
    else rval = 1;

    system_task_return = 0;

    if (res)rval = 0XFF;    /* 前面出错了,不再进入循环 */

    while (rval == 0)
    {
        tp_dev.scan(0);
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
        delay_ms(1000 / OS_TICKS_PER_SEC);      /* 延时一个时钟节拍 */

        if (system_task_return)
        {
            rval = 0XFF;    /* TPAD返回 */
            break;
        }

        if (tname->top < in_obj.y && in_obj.y < (tname->top + tname->height) && (tname->left < in_obj.x) && in_obj.x < (tname->left + tname->width))   /* 在tname框内部 */
        {
            editflag = 0;               /* 编辑姓名 */
            edit_show_cursor(tphone, 0);/* 关闭tphone的光标 */
            tphone->type = 0X04;        /* 光标不闪烁 */
            edit_show_cursor(tsex, 0);  /* 关闭tsex的光标 */
            tsex->type = 0X04;          /* 光标不闪烁 */
            tname->type = 0X06;         /* 光标闪烁 */
        }

        if (tphone->top < in_obj.y && in_obj.y < (tphone->top + tphone->height) && (tphone->left < in_obj.x) && in_obj.x < (tphone->left + tphone->width))   /* 在tphone框内部 */
        {
            editflag = 1;               /* 编辑姓名 */
            edit_show_cursor(tname, 0); /* 关闭tname的光标 */
            tname->type = 0X04;         /* 光标不闪烁 */
            edit_show_cursor(tsex, 0);  /* 关闭tsex的光标 */
            tsex->type = 0X04;          /* 光标不闪烁 */
            tphone->type = 0X06;        /* 光标闪烁 */
        }

        if (tsex->top < in_obj.y && in_obj.y < (tsex->top + tsex->height) && (tsex->left < in_obj.x) && in_obj.x < (tsex->left + tsex->width))   /* 在tsex框内部 */
        {
            editflag = 2;               /* 编辑姓名 */
            edit_show_cursor(tphone, 0);/* 关闭tphone的光标 */
            tphone->type = 0X04;        /* 光标不闪烁 */
            edit_show_cursor(tname, 0); /* 关闭tname的光标 */
            tname->type = 0X04;         /* 光标不闪烁 */
            tsex->type = 0X06;          /* 光标闪烁 */
        }

        edit_check(tname, &in_obj);
        edit_check(tphone, &in_obj);
        edit_check(tsex, &in_obj);
        t9_check(t9, &in_obj);

        if (t9->outstr[0] != NULL)  /* 添加字符 */
        {
            if (editflag == 0)   /* 姓名 */
            {
                if (t9->outstr[0] != 0X0D)edit_add_text(tname, t9->outstr); /* 不支持回车 */
            }
            else if (editflag == 1) /* 电话 */
            {
                if ((t9->outstr[0] <= '9' && t9->outstr[0] >= '0') || t9->outstr[0] == 0X08)edit_add_text(tphone, t9->outstr); /* 数字/退格键 */
            }
            else if (editflag == 2) /* 性别 */
            {
                if (t9->outstr[0] != 0X0D)edit_add_text(tsex, t9->outstr); /* 不支持回车 */
            }

            t9->outstr[0] = NULL;   /* 清空输出字符 */
        }

        res = btn_check(okbtn, &in_obj);

        if (res && ((okbtn->sta & (1 << 7)) == 0) && (okbtn->sta & (1 << 6)))   /* 有输入,有按键按下且松开,并且TP松开了 */
        {
            if (mode == 1)frec_get_image_data(pimage, cxoff + cwidth - cixoff - 90, cyoff + ciyoff, 90, 30); /* 添加新人脸,读取30*40的图片 */

            window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)frec_remind_msg_tbl[6][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 16, 0, 0, 0); /* 显示正在连接 */
            res = 0;

            if (mode == 1)   /* 添加一个人脸 */
            {
                res = atk_frec_add_a_face(pimage, &person);
                pnode->index = person;
            }

            if (res == 0)
            {
                strcpy((char *)pnode->pname, (char *)tname->text);  /* 拷贝姓名 */
                strcpy((char *)pnode->pnum, (char *)tphone->text);  /* 拷贝电话 */
                strcpy((char *)pnode->sex, (char *)tsex->text);     /* 拷贝性别 */
                frecdev->face = pnode;
                res = appplay_frec_save_a_faceinfo(frecdev);

                if (res)rval = 2;   /* 添加人脸信息失败 */

                rval = 0;   /* 正常 */
                break;
            }
            else rval = 3;  /* 添加人脸模板失败 */
        }

        res = btn_check(cbtn, &in_obj);

        if (res && ((cbtn->sta & (1 << 7)) == 0) && (cbtn->sta & (1 << 6)))  /* 有输入,有按键按下且松开,并且TP松开了 */
        {
            rval = 0XFF;    /* 取消 */
            break;
        }
    }

    btn_delete(okbtn);
    btn_delete(cbtn);
    edit_delete(tname);
    edit_delete(tphone);
    edit_delete(tsex);
    t9_delete(t9);
    atk_frec_free(pimage);

    if (rval == 2 || rval == 3 || rval == 0)   /* 显示保存结果 */
    {
        if (rval == 0)res = 7;
        else res = 8;

        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)frec_remind_msg_tbl[res][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 16, 0, 0, 0); /* 显示失败 */

        if (rval == 0)atk_frec_load_data_model(); /* 重新加载所有识别模型 */

        delay_ms(600);
    }

    return rval;
}

/**
 * @brief       读取一个人脸
 * @param       pnode           : 人脸节点
 * @param       frecdev         : 人脸识别控制结构体
 * @param       index           : 识别ID
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_read_a_face(frec_node *pnode, _frec_obj *frecdev, uint8_t index)
{
    uint8_t *path;
    FIL *fp;
    uint32_t fr;
    uint8_t res;
    uint8_t *p;
    path = gui_memin_malloc(30);/* 申请内存 */
    fp = gui_memin_malloc(sizeof(FIL));/* 申请内存 */
    pnode->pname = gui_memin_malloc(30);/* 申请30字节内存,用于存储姓名 */
    pnode->pnum = gui_memin_malloc(20);/* 申请20字节内存,用于存储电话号码 */
    pnode->sex = gui_memin_malloc(20);/* 申请20字节内存,用于存储性别 */

    if (!fp)
    {
        gui_memin_free(path);
        return ATK_FREC_MEMORY_ERR;
    }

    pnode->index = index;
    sprintf((char *)path, ATK_FREC_INFO_PNAME, pnode->index);
    res = f_open(fp, (char *)path, FA_READ);

    if (res == FR_OK)
    {
        res = f_read(fp, (char *)frecdev->databuf, 90 * 120 * 2 + 70, &fr); /* 读取文件 */

        if (fr == 90 * 120 * 2 + 70)
        {
            p = (uint8_t *)frecdev->databuf + 90 * 120 * 2;
            strcpy((char *)pnode->pname, (char *)p);
            p += 30;
            strcpy((char *)pnode->pnum, (char *)p);
            p += 20;
            strcpy((char *)pnode->sex, (char *)p);
            res = 0;
        }
        else res = ATK_FREC_READ_WRITE_ERR;
    }

    f_close(fp);

    if (res)res = ATK_FREC_READ_WRITE_ERR;

    gui_memin_free(path);
    gui_memin_free(fp);
    return res;
}

/**
 * @brief       读取所有人脸
 * @param       head            : 链表头
 * @param       frecdev         : 人脸识别控制结构体
 * @param       fnum            : 可用的人脸数
 * @retval      0, 无数据; 其他, 人脸数;
 */
uint8_t appplay_frec_read_all_face(frec_node *head, _frec_obj *frecdev, uint8_t *fnum)
{
    frec_node *pnew;
    uint8_t i;
    uint8_t res;
    uint8_t face_num = 0;
    pnew = head;

    for (i = 0; i < MAX_LEBEL_NUM; i++)
    {
        if (model.validSample[i])   /* 该人脸模型有效 */
        {
            res = appplay_frec_read_a_face(pnew, frecdev, i); /* 读取人脸信息数据 */

            if (res == 0)
            {
                if (pnew != head)   /* 不是表头 */
                {
                    res = frec_node_insert(head, pnew, 0XFFFF); /* 在末尾追加新节点 */

                    if (res)break;  /* 追加失败 */
                }

                face_num++;
                printf("添加一个人脸%02d,编号:%02d\r\n", face_num, pnew->index);
                pnew = frec_node_creat();   /* 创建新节点 */

                if (pnew == 0)break;        /* 创建失败 */
            }
            else
            {
                model.validSample[i] = 0;   /* 标记该人脸模板不可用(缺少人脸信息数据) */
                frec_node_free(pnew, 0);    /* 删除节点信息 */
            }
        }
    }

    if (pnew != head)frec_node_free(pnew, 1); /* 删除节点信息(如果是头,就不删) */

    *fnum = face_num;
    return face_num;
}

/**
 * @brief       删除一个人脸
 * @param       head            : 链表头
 * @param       index           : 节点编号
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t appplay_frec_delete_face(frec_node **head, uint16_t index)
{
    frec_node *ptemp;
    uint8_t res;
    ptemp = frec_node_getnode(*head, index);    /* 读取节点号 */

    if (ptemp == NULL)return 1; /* 内存申请失败/得到节点信息失败 */

    res = atk_frec_delete_data(ptemp->index);   /* 删除人脸模板数据 */

    if (ptemp->index < MAX_LEBEL_NUM)model.validSample[ptemp->index] = 0; /* 标记人脸模板无效了 */

    if (res == 0)
    {
        res = appplay_frec_delete_a_faceinfo(ptemp->index); /* 删除人脸信息数据 */
    }

    if (res == 0)   /* 删除短信成功 */
    {
        if (index == 0)   /* 删除节点头 */
        {
            if ((*head)->next)   /* 存在下一个节点 */
            {
                ptemp = *head;
                *head = (*head)->next;
                frec_node_free(ptemp, 1);
            }
            else
            {
                frec_node_free(*head, 0);               /* 先释放节点内容 */
                (*head)->pname = gui_memin_malloc(30);  /* 申请30字节 */
                (*head)->pnum = gui_memin_malloc(20);   /* 申请20字节 */

                if ((*head)->pnum)
                {
                    strcpy((char *)(*head)->pnum, "0"); /* 拷贝 */
                    strcpy((char *)(*head)->pname, (char *)frec_remind_msg_tbl[0][gui_phy.language]); /* 拷贝:无模板信息到item里面 */
                }
            }
        }
        else
        {
            res = frec_node_delete(*head, index);       /* 删除这个节点 */

            if (res)res = 3;    /* 删除节点失败 */
        }
    }
    else res = 2;   /* 删除失败 */

    return res;
}

/**
 * @brief       浏览人脸
 * @param       sel             : 当前选中的条目
 * @param       top             : 当前最顶部的条目
 * @param       caption         : 选择框名字
 * @param       head            : 链表头
 * @retval      [7]:0,按下的是返回按键,或者发生了错误;1,按下了选项按键,或者双击了选中的条目.
 *              [6]:0,双击事件;1,选项按钮按下.
 *              [5:0]:0,表示无错误;1,表示要更新信息;其他非零值,错误代码.
 */
uint8_t frec_face_browser(uint8_t *sel, uint8_t *top, uint8_t *caption, frec_node *head)
{
    uint8_t  res;
    uint8_t rval = 0;       /* 返回值 */
    uint16_t i;
    uint16_t itemsize;      /* 总条目数 */
    frec_node *pnode;
    _btn_obj *rbtn = 0;     /* 返回按钮控件 */
    _btn_obj *okbtn = 0;    /* 确认按钮控件 */
    _listbox_obj *tlistbox; /* listbox */
    itemsize = frec_node_getsize(head); /* 至少得有一个节点 */

    if (itemsize == 0)return 2; /* 一个节点都没有,错误!! */

    if (*sel >= itemsize)*sel = itemsize - 1; /* 限制范围 */

    if (*top >= itemsize)*top = 0;/* 限制范围 */

    app_filebrower(caption, 0X07);  /* 显示标题 */
    tlistbox = listbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.tbfsize);  /* 创建一个filelistbox */

    if (tlistbox == NULL)rval = 1;  /* 申请内存失败 */
    else    /* 添加条目 */
    {
        for (i = 0; i < itemsize; i++)
        {
            pnode = frec_node_getnode(head, i);
            res = listbox_addlist(tlistbox, pnode->pname);

            if (res)
            {
                rval = 1;
                break;
            }
        }
    }

    if (rval == 0)   /* 成功添加了条目 */
    {
        tlistbox->scbv->topitem = *top;
        tlistbox->selindex = *sel;
        listbox_draw_listbox(tlistbox);
        rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* 创建文字按钮 */
        okbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* 创建确认文字按钮 */

        if (rbtn == NULL || okbtn == NULL)rval = 1; /* 没有足够内存够分配 */
        else
        {
            rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language];      /* 名字 */
            rbtn->font = gui_phy.tbfsize;   /* 设置新的字体大小 */
            rbtn->bcfdcolor = WHITE;        /* 按下时的颜色 */
            rbtn->bcfucolor = WHITE;        /* 松开时的颜色 */
            btn_draw(rbtn);/* 画按钮 */
            okbtn->caption = (uint8_t *)GUI_OPTION_CAPTION_TBL[gui_phy.language];   /* 名字 */
            okbtn->font = gui_phy.tbfsize;  /* 设置新的字体大小 */
            okbtn->bcfdcolor = WHITE;       /* 按下时的颜色 */
            okbtn->bcfucolor = WHITE;       /* 松开时的颜色 */
            btn_draw(okbtn);/* 画按钮 */
        }
    }

    system_task_return = 0;

    while (rval == 0)
    {
        tp_dev.scan(0);
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
        delay_ms(1000 / OS_TICKS_PER_SEC);  /* 延时一个时钟节拍 */

        if (system_task_return)break;       /* TPAD返回 */

        res = btn_check(rbtn, &in_obj);     /* 返回按钮检测 */

        if (res)if (((rbtn->sta & 0X80) == 0))break;    /* 退出 */

        res = btn_check(okbtn, &in_obj);    /* 确认按钮检测 */

        if (res)
        {
            if (((okbtn->sta & 0X80) == 0)) /* 按钮状态改变了 */
            {
                *top = tlistbox->scbv->topitem; /* 记录退出时选择的条目 */
                *sel = tlistbox->selindex;
                rval = 3 << 6; /* 标记选项按钮按下 */
            }
        }

        listbox_check(tlistbox, &in_obj);   /* 扫描 */

        if (tlistbox->dbclick == 0X80)      /* 双击了 */
        {
            *top = tlistbox->scbv->topitem; /* 记录退出时选择的条目 */
            *sel = tlistbox->selindex;
            rval = 2 << 6; /* 标记双击按下了 */
        }
    }

    listbox_delete(tlistbox);   /* 删除listbox */
    btn_delete(okbtn);          /* 删除按钮 */
    btn_delete(rbtn);           /* 删除按钮 */
    system_task_return = 0;
    return rval;
}

/**
 * @brief       设置图像到屏幕最中心
 *  @note       将摄像头输出数据,定位到屏幕正中央,且保证宽高比为3:4
 * @param       frecdev         : 人脸识别控制结构体
 * @retval      无
 */
void frec_set_image_center(_frec_obj *frecdev)
{
    frecdev->xoff = 0;
    frecdev->yoff = 0;
    frecdev->width = lcddev.width;
    frecdev->height = lcddev.height;

    if (lcddev.id == 0X1963 || lcddev.id == 0X5510 || lcddev.id == 0X5310)  /* 最大只支持300*400分辨率，内存限制 */
    {
        frecdev->width = 300;
        frecdev->height = 400;
    }

//    if(lcddev.id==0X1963||lcddev.id==0X5510||lcddev.id==0X7084)
//    {
//        frecdev->height=640;
//    }else if(lcddev.id==0X5310)
//    {
//        frecdev->width=300;
//        frecdev->height=400;
//    }else if(lcddev.id==0X7016||lcddev.id==0X1016||lcddev.id==0X8016||lcddev.id==0X1018)    /* 1024*600的RGB屏 */
//    {
//        frecdev->height=640;
//        frecdev->width=480;
//    }else if(lcddev.id==0X4342)     /* 480*272的RGB屏 */
//    {
//        frecdev->height=360;
//        frecdev->width=270;
//    }
    
    frecdev->yoff = (lcddev.height - frecdev->height) / 2;
    frecdev->xoff = (lcddev.width - frecdev->width) / 2;
    frec_curline = frec_dev.yoff;   /* 行位置偏移 */
}

/**
 * @brief       读取原始图片数据
 * @param       dbuf            : 数据缓存区
 * @param       xoff,yoff       : 要读取的图像区域起始坐标
 * @param       xsize           : 要读取的图像区域宽度
 * @param       width           : 要读取的宽度(宽高比恒为3:4)
 * @retval      无
 */
void frec_get_image_data(uint16_t *dbuf, uint16_t xoff, uint16_t yoff, uint16_t xsize, uint16_t width)
{
    int w, h;
    uint16_t height = width * 4 / 3;
    float scale = (float)xsize / width;

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            dbuf[h * width + w] = lcd_read_point(xoff + w * scale, yoff + h * scale);
        }
    }
}

/**
 * @brief       显示图片
 * @param       x,y             : 起始坐标
 * @param       w,h             : 宽度和高度
 * @param       data            : 要读取的宽度
 * @retval      无
 */
void frec_show_picture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data)
{
    uint16_t i, j;

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            lcd_draw_point(x + j, y + i, *data);
            data ++;
        }
    }
}

/**
 * @brief       FREC RGB屏数据接收回调函数
 * @param       无
 * @retval      无
 */
//void frec_rgblcd_dcmi_rx_callback(void)
//{
//    uint16_t *pbuf;

//    if (DMA1_Stream1->CR & (1 << 19))   /* DMA使用buf1,读取buf0 */
//    {
//        pbuf = (uint16_t *)frec_dcmi_line_buf[0];
//    }
//    else    /* DMA使用buf0,读取buf1
//    {
//        pbuf = (uint16_t *)frec_dcmi_line_buf[1];
//    }

//    LTDC_Color_Fill(frec_dev.xoff, frec_curline, frec_dev.xoff + frec_dev.width - 1, frec_curline, pbuf); /* DM2D填充 */

//    if (frec_curline < (frec_dev.height + frec_dev.yoff))frec_curline++;
//}

uint8_t ex_sw = 0;


extern uint8_t camera_key_scan(void);


/**
 * @brief       人脸识别
 * @param       无
 * @retval      0, 正常退出; 其他, 错误代码;
 */
uint8_t frec_play(void)
{
    uint8_t res, fac;
    uint8_t key;        /* 键值 */
    uint8_t *msgbuf;    /* 消息缓存区 */
    uint8_t person;
    uint8_t rval = 0;

    frec_node *head = 0, *ptemp = 0;

    uint8_t fnum = 0;
    uint8_t fsel;
    uint8_t selitem = 0;
    uint8_t topitem = 0;
    uint16_t *pbkcolor;

//    if(audiodev.status&(1<<7))      /* 当前在放歌??必须停止 */
//    {
//        audio_stop_req(&audiodev);  /* 停止音频播放 */
//        audio_task_delete();        /* 删除音乐播放任务 */
//    }
    
    /* 提示开始检测OV5640 */
    window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_remind_tbl[0][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);

    if (ov5640_init())              /* 初始化OV5640 */
    {
        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_remind_tbl[1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);
        delay_ms(500);
        rval = 1;
    }

    frec_set_image_center(&frec_dev);   /* 设置到屏幕正中央 */
    msgbuf = gui_memin_malloc(40);      /* 申请40个字节内存,用于存放文件名 */
    frec_dev.databuf = mymalloc(SRAM12, frec_dev.width * frec_dev.height * 2); /* 申请内存 */
    frec_dcmi_line_buf[0] = mymalloc(SRAMDTCM, frec_dev.width * 2); /* 为dma接收申请内存 */
    frec_dcmi_line_buf[1] = mymalloc(SRAMDTCM, frec_dev.width * 2); /* 为dma接收申请内存 */

    if (!frec_dcmi_line_buf[1] || !msgbuf || !frec_dev.databuf)rval = 1; /* 申请失败 */

    if (rval == 0)
    {
        memshow_flag = 0;
        sw_sdcard_mode();                   /* SD卡模式 */
        rval = atk_frec_initialization();   /* 初始化人脸识别 */
        head = frec_node_creat();           /* 创建表头 */
        fnum = appplay_frec_read_all_face(head, &frec_dev, &fnum); /* 读取所有人脸 */

        if (fnum == 0)  /* 一张人脸都没有 */
        {
            head->pname = gui_memin_malloc(30); /* 申请30字节 */
            head->pnum = gui_memin_malloc(20);  /* 申请20字节 */

            if (head->pnum)
            {
                strcpy((char *)head->pnum, "0");    /* 拷贝 */
                strcpy((char *)head->pname, (char *)frec_remind_msg_tbl[0][gui_phy.language]); /* 拷贝:无模板信息到item里面 */
            }
        }

        if (head->pnum)
        {
            sw_ov5640_mode();           /* 5640模式 */
            ov5640_rgb565_mode();       /* RGB565模式 */
            ov5640_focus_init();
            ov5640_focus_constant();    /* 启动持续对焦 */
            ov5640_light_mode(0);       /* 自动模式 */
            ov5640_brightness(4);       /* 亮度设置 */
            ov5640_color_saturation(3); /* 色度设置 */
            ov5640_contrast(3);         /* 对比度设置 */
            ov5640_exposure(3);         /* 曝光等级 */
            ov5640_sharpness(33);       /* 自动锐度 */
            dcmi_init();                /* DCMI配置 */
//            if(lcdltdc.pwidth!=0)       /* RGB屏 */
//            {
//                dcmi_rx_callback=frec_rgblcd_dcmi_rx_callback;  /* RGB屏接收数据回调函数 */
//                dcmi_dma_init((uint32_t)frec_dcmi_line_buf[0],(uint32_t)frec_dcmi_line_buf[1],frec_dev.width/2,1,1);    /* DCMI DMA配置 */
//            }else       /* MCU 屏 */
            {
                dcmi_dma_init((uint32_t)&LCD->LCD_RAM, 0, 1, 1, 0); /* DCMI DMA配置,MCU屏,竖屏 */
            }
            lcd_clear(BLACK);
            fac = 800 / frec_dev.height;    /* 得到比例因子 */
            ov5640_outsize_set((1280 - fac * frec_dev.width) / 2, (800 - fac * frec_dev.height) / 2, frec_dev.width, frec_dev.height);
            lcd_set_window(frec_dev.xoff, frec_dev.yoff, frec_dev.width, frec_dev.height); /* 设置开窗口 */
            dcmi_start();                   /* 启动传输 */
            tim6_int_init(100 - 1, 24000 - 1);  /* 10Khz计数频率,加100是10ms,用于统计人脸识别时间 */
            system_task_return = 0;

            while (rval == 0)
            {
                if (system_task_return)break;   /* TPAD返回 */

                key = camera_key_scan();        /* 不支持连按 */
                tp_dev.scan(0);

                if (tp_dev.sta & TP_PRES_DOWN)  /* 触摸屏幕,执行对焦 */
                {
                    ov5640_focus_single();/* 执行自动对焦 */
                    tp_dev.scan(0);
                    tp_dev.sta = 0;
                }

                if (key == WKUP_PRES || key == KEY1_PRES || key == 5)
                {
                    dcmi_stop();        /* 停止传输 */
                    lcd_set_window(0, 0, lcddev.width, lcddev.height); /* 设置开窗口 */
                    sw_sdcard_mode();   /* SD卡模式 */

                    switch (key)
                    {
                        case 5:         /* KEYUP长按，人脸模板列表,可以删除/编辑人脸信息 */
                            while (1)
                            {
                                res = frec_face_browser(&selitem, &topitem, frec_remind_msg_tbl[2][gui_phy.language], head);

                                if (system_task_return)break; /* TPAD返回 */

                                if (res & 0X80)
                                {
                                    fsel = 0;

                                    if (res & 1 << 6)
                                    {
                                        if (fnum == 0)res = 1;
                                        else res = app_items_sel((lcddev.width - 180) / 2, (lcddev.height - 152) / 2, 180, 72 + 40 * 2, (uint8_t **)frec_option_tbl[gui_phy.language], 2, (uint8_t *)&fsel, 0X90, (uint8_t *)frec_remind_msg_tbl[1][gui_phy.language]); /* 2个选择 */

                                        if (res == 0)   /* 设置成功 */
                                        {
                                            switch (fsel)
                                            {
                                                case 0:/* 编辑人脸 */
                                                    fsel = 0XFF; /* 标记要编辑人脸 */
                                                    break;

                                                case 1:/* 删除人脸 */
                                                    res = appplay_frec_delete_face(&head, selitem); /* 删除这条短信 */

                                                    if (res == 0)
                                                    {
                                                        fnum--;/* 人脸数减一 */
                                                    }
                                                    else res = 1;

                                                    window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)frec_remind_msg_tbl[9 + res][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 16, 0, 0, 0); /* 删除成功/失败 */
                                                    delay_ms(600);
                                                    break;
                                            }
                                        }
                                    }
                                    else fsel = 0XFF;   /* 双击,标记要编辑人脸 */

                                    if (fsel == 0XFF && fnum)   /* 有人脸的时候,编辑人脸 */
                                    {
                                        ptemp = frec_node_getnode(head, selitem);   /* 得到选中节点的信息 */

                                        if (ptemp)
                                        {
                                            frec_node_free(ptemp, 0);   /* 释放节点内容 */
                                            appplay_frec_add_a_face(ptemp, &frec_dev, 0);   /* 编辑这张人脸 */
                                        }
                                    }
                                }
                                else break;   /* 返回键/错误 */
                            }

                            break;

                        case KEY1_PRES: /* 识别人脸 */
                            pbkcolor = mymalloc(SRAMITCM, 200 * 80 * 2); /* 申请背景色缓存 */

                            if (pbkcolor == 0)continue; /* 内存申请失败 */

                            frec_get_image_data(frec_dev.databuf, frec_dev.xoff, frec_dev.yoff, frec_dev.width, 30);
                            //frec_show_picture(0,0,30,40,frec_dev.databuf);  /* 显示预览图片 */
                            app_read_bkcolor((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, pbkcolor); /* 读取背景色 */
                            window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)frec_remind_msg_tbl[11][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 16, 0, 0, 0); /* 显示失败 */
                            reg_time = 0;
                            res = atk_frec_recognition_face(frec_dev.databuf, &person);

                            if (res == ATK_FREC_MODEL_DATA_ERR)
                            {
                                window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)frec_remind_msg_tbl[12][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 16, 0, 0, 0); /* 显示失败 */
                                delay_ms(600);
                            }
                            else if (res == ATK_FREC_UNREC_FACE_ERR)
                            {
                                window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)frec_remind_msg_tbl[13][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 16, 0, 0, 0); /* 显示失败 */
                                delay_ms(600);
                            }
                            else
                            {
                                ptemp = frec_node_getnode_index(head, person); /* 得到节点信息 */

                                if (ptemp)
                                {
                                    frec_node_free(ptemp, 0);   /* 释放节点内容 */
                                    app_recover_bkcolor((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, pbkcolor); /* 恢复背景色 */
                                    appplay_frec_show_result(ptemp, &frec_dev, reg_time * 10);  /* 显示识别结果 */
                                }
                            }

                            myfree(SRAMITCM, pbkcolor);
                            break;

                        case WKUP_PRES:/* KEY UP按下,添加一个人像进入数据库 */
                            if (fnum == 0)
                            {
                                frec_node_free(head, 0);    /* 先释放之前的表头的内容 */
                                res = appplay_frec_add_a_face(head, &frec_dev, 1);  /* 添加一张人脸 */
                            }
                            else
                            {
                                ptemp = frec_node_creat(); /* 创建新节点 */
                                res = appplay_frec_add_a_face(ptemp, &frec_dev, 1); /* 添加一张人脸 */

                                if (res == 0)   /* 读取成功 */
                                {
                                    res = frec_node_insert(head, ptemp, 0XFFFF);    /* 在末尾追加新节点 */
                                }
                            }

                            if (res == 0)   /* 添加成功 */
                            {
                                fnum++;
                            }
                            else
                            {
                                frec_node_free(ptemp, 1); /* 添加失败,释放节点内存(这里不释放表头!) */

                                if (fnum == 0)  /* 只有表头的情况,且读模板失败,继续添加默认消息 */
                                {
                                    strcpy((char *)head->pnum, "0");    /* 拷贝 */
                                    strcpy((char *)head->pname, (char *)frec_remind_msg_tbl[0][gui_phy.language]); /* 拷贝:无模板信息到item里面 */
                                }
                            }

                            break;

                        default :
                            break;
                    }

                    lcd_clear(BLACK);
                    sw_ov5640_mode();   /* 5640模式 */
                    lcd_set_window(frec_dev.xoff, frec_dev.yoff, frec_dev.width, frec_dev.height); /* 设置开窗口 */
                    dcmi_start();       /* 启动传输 */
                    system_task_return = 0;
                }

                if (hsync_int)   /* 刚刚产生帧中断,可以延时 */
                {
                    delay_ms(1);
                    hsync_int = 0;
                }
            }

            dcmi_stop();
            sw_sdcard_mode();   /* 切换为SD卡模式 */
            atk_frec_destroy(); /* 结束人脸识别 */
            TIM6->CR1 &= ~(1 << 0); /* 关闭定时器6 */
            lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* 恢复默认窗口 */
            memshow_flag = 1;
        }
    }

    if (head)frec_node_destroy(head); /* 删除整个链表 */

    myfree(SRAM12, frec_dev.databuf);
    myfree(SRAMDTCM, frec_dcmi_line_buf[0]);
    myfree(SRAMDTCM, frec_dcmi_line_buf[1]);
    
    gui_memin_free(msgbuf);
    /* 恢复偏移量 */
    frec_dev.xoff = 0;
    frec_dev.yoff = 0;
    return 0;
}





















