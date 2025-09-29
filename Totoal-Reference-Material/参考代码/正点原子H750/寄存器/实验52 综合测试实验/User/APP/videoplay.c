/**
 ****************************************************************************************************
 * @file        videoplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-视频播放器 代码
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

#include "videoplay.h"
#include "settings.h"
#include "ucos_ii.h"
#include "audioplay.h"
//#include "es8388.h"
//#include "sai.h"
//#include "wavplay.h"
#include "./BSP/TIMER/btim.h"


__videodev videodev;        /* 视频播放控制器 */
__videoui *vui;             /* 视频播放界面控制器 */


extern volatile uint8_t framecnt;       /* 统一的帧计数器 */
extern volatile uint8_t framecntout;    /* 统一的帧计数器输出变量 */
volatile uint8_t aviframeup;            /* 视频播放时隙控制变量,当等于1的时候,可以更新下一帧视频 */


/* 5个图片按钮的路径 */
uint8_t *const AUDIO_BTN_PIC_TBL[2][5] =
{
    {
        "1:/SYSTEM/APP/AUDIO/ListR.bmp",
        "1:/SYSTEM/APP/AUDIO/PrevR.bmp",
        "1:/SYSTEM/APP/AUDIO/PauseR.bmp",
        "1:/SYSTEM/APP/AUDIO/NextR.bmp",
        "1:/SYSTEM/APP/AUDIO/ExitR.bmp",
    },
    {
        "1:/SYSTEM/APP/AUDIO/ListP.bmp",
        "1:/SYSTEM/APP/AUDIO/PrevP.bmp",
        "1:/SYSTEM/APP/AUDIO/PlayP.bmp",
        "1:/SYSTEM/APP/AUDIO/NextP.bmp",
        "1:/SYSTEM/APP/AUDIO/ExitP.bmp",
    },
};

uint8_t *const AUDIO_PLAYR_PIC = "1:/SYSTEM/APP/AUDIO/PlayR.bmp";   /* 播放 松开 */
uint8_t *const AUDIO_PLAYP_PIC = "1:/SYSTEM/APP/AUDIO/PlayP.bmp";   /* 播放 按下 */
uint8_t *const AUDIO_PAUSER_PIC = "1:/SYSTEM/APP/AUDIO/PauseR.bmp"; /* 暂停 松开 */
uint8_t *const AUDIO_PAUSEP_PIC = "1:/SYSTEM/APP/AUDIO/PauseP.bmp"; /* 暂停 按下 */


/**
 * @brief       显示audio播放时间
 * @param       sx,sy           : 起始坐标
 * @param       sec             : 时间(单位:s)
 * @retval      无
 */
void video_time_show(uint16_t sx, uint16_t sy, uint16_t sec)
{
    uint16_t min;
    uint8_t *buf;
    buf = gui_memin_malloc(100);

    if (buf == 0)return;

    min = sec / 60; /* 得到分钟数 */
    sec = sec % 60; /* 得到秒钟数 */
    sprintf((char *)buf, "%03d:%02d", min, sec);
    gui_fill_rectangle(sx, sy, vui->msgfsize * 3, vui->msgfsize, AUDIO_MAIN_BKCOLOR);	/* 填充底色 */
    gui_show_string(buf, sx, sy, vui->msgfsize * 3, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);
    gui_memin_free(buf);
}

/**
 * @brief       视频播放主界面UI
 * @param       无
 * @retval      无
 */
void video_load_ui(void)
{
    if (lcddev.width == 240)
    {
        vui->tpbar_height = 20;
        vui->capfsize = 12;
        vui->msgfsize = 12; /* 不能大于16 */

        vui->msgbar_height = 46;
        vui->nfsize = 12;
        vui->xygap = 4;
        vui->msgdis = 4;    /* 横向3个dis */

        vui->prgbar_height = 30;
        vui->pbarwidth = 140;   /* 两边延伸至少  14*vui->msgfsize/2 */

        vui->btnbar_height = 60;
    }
    else if (lcddev.width == 272)
    {
        vui->tpbar_height = 24;
        vui->capfsize = 12;
        vui->msgfsize = 12; /* 不能大于16 */

        vui->msgbar_height = 50;
        vui->nfsize = 12;
        vui->xygap = 4;
        vui->msgdis = 6;    /* 横向3个dis */

        vui->prgbar_height = 30;
        vui->pbarwidth = 170;   /* 两边延伸至少  14*vui->msgfsize/2 */

        vui->btnbar_height = 80;
    }
    else if (lcddev.width == 320)
    {
        vui->tpbar_height = 24;
        vui->capfsize = 12;
        vui->msgfsize = 12; /* 不能大于16 */

        vui->msgbar_height = 50;
        vui->nfsize = 12;
        vui->xygap = 4;
        vui->msgdis = 16;   /* 横向3个dis */

        vui->prgbar_height = 30;
        vui->pbarwidth = 210;   /* 两边延伸至少  14*vui->msgfsize/2 */

        vui->btnbar_height = 80;
    }
    else if (lcddev.width == 480)
    {
        vui->tpbar_height = 30;
        vui->capfsize = 16;
        vui->msgfsize = 12; /* 不能大于16 */

        vui->msgbar_height = 60;
        vui->nfsize = 12;
        vui->xygap = 6;
        vui->msgdis = 24;   /* 横向3个dis */

        vui->prgbar_height = 40;
        vui->pbarwidth = 340;   /* 两边延伸至少  14*vui->msgfsize/2 */

        vui->btnbar_height = 120;
    }
    else if (lcddev.width == 600)
    {
        vui->tpbar_height = 40;
        vui->capfsize = 24;
        vui->msgfsize = 16; /* 不能大于16 */

        vui->msgbar_height = 100;
        vui->nfsize = 16;
        vui->xygap = 10;
        vui->msgdis = 40;   /* 横向3个dis */

        vui->prgbar_height = 60;
        vui->pbarwidth = 400;   /* 两边延伸至少  12*vui->msgfsize/2 */

        vui->btnbar_height = 150;
    }
    else if (lcddev.width == 800)
    {
        vui->tpbar_height = 60;
        vui->capfsize = 32;
        vui->msgfsize = 16; /* 不能大于16 */

        vui->msgbar_height = 120;
        vui->nfsize = 16;
        vui->xygap = 10;
        vui->msgdis = 60;   /* 横向3个dis */

        vui->prgbar_height = 60;
        vui->pbarwidth = 600;   /* 两边延伸至少  12*vui->msgfsize/2 */

        vui->btnbar_height = 180;
    }

    vui->vbarheight = vui->msgfsize; /* 等于vui->msgfsize的大小 */
    vui->pbarheight = vui->msgfsize; /* 等于vui->msgfsize的大小 */
    vui->vbarwidth = lcddev.width - 16 - 2 * vui->xygap - 3 * vui->msgdis - 14 * vui->msgfsize / 2;
    vui->vbarx = vui->msgdis + 16 + vui->xygap;
    vui->vbary = vui->tpbar_height + vui->xygap * 2 + vui->msgfsize + (vui->msgbar_height - (vui->msgfsize + vui->xygap * 2 + vui->xygap / 2 + vui->msgfsize + vui->vbarheight)) / 2;
    vui->pbarx = (lcddev.width - vui->pbarwidth - 12 * vui->msgfsize / 2) / 2 + vui->msgfsize * 6 / 2;
    vui->pbary = lcddev.height - vui->btnbar_height - vui->prgbar_height + (vui->prgbar_height - vui->pbarheight) / 2;



    gui_fill_rectangle(0, 0, lcddev.width, vui->tpbar_height, AUDIO_TITLE_BKCOLOR);	/* 填充标题栏底色 */
    gui_show_strmid(0, 0, lcddev.width, vui->tpbar_height, AUDIO_TITLE_COLOR, vui->capfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[3][gui_phy.language]);    /* 显示标题 */
    gui_fill_rectangle(0, vui->tpbar_height, lcddev.width, vui->msgbar_height, AUDIO_MAIN_BKCOLOR);             /* 填充信息栏背景色 */
    minibmp_decode((uint8_t *)APP_VOL_PIC, vui->msgdis, vui->vbary - (16 - vui->msgfsize) / 2, 16, 16, 0, 0);   /* 解码音量图标 */
    gui_show_string("00%", vui->vbarx, vui->vbary + vui->vbarheight + vui->xygap / 2, 3 * vui->msgfsize / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);   /* 显示音量 */
    gui_fill_rectangle(0, lcddev.height - vui->btnbar_height - vui->prgbar_height, lcddev.width, vui->prgbar_height, AUDIO_MAIN_BKCOLOR);   /* 填充进度条栏背景色 */
    gui_fill_rectangle(0, lcddev.height - vui->btnbar_height, lcddev.width, vui->btnbar_height, AUDIO_BTN_BKCOLOR); /* 填充按钮栏背景色 */
    gui_fill_rectangle(0, vui->tpbar_height + vui->msgbar_height, lcddev.width, lcddev.height - vui->tpbar_height - vui->msgbar_height - vui->prgbar_height - vui->btnbar_height, AUDIO_MAIN_BKCOLOR); /* 填充底色 */
}

/**
 * @brief       显示音量百分比
 * @param       pctx            : 百分比值
 * @retval      无
 */
void video_show_vol(uint8_t pctx)
{
    uint8_t *buf;
    uint8_t sy = vui->vbary + vui->vbarheight + vui->xygap / 2;
    gui_phy.back_color = AUDIO_MAIN_BKCOLOR; /* 设置背景色为底色 */
    gui_fill_rectangle(vui->vbarx, sy, 4 * vui->msgfsize / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR); /* 填充背景色 */
    buf = gui_memin_malloc(32);
    sprintf((char *)buf, "%d%%", pctx);
    gui_show_string(buf, vui->vbarx, sy, 4 * vui->msgfsize / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR); /* 显示音量 */
    gui_memin_free(buf);
}

/**
 * @brief       视频信息更新
 * @param       videodevx       : video 控制器
 * @param       videoprgbx      : 视频进度条
 * @param       volprgbx        : 音量进度条
 * @param       aviinfo         : 视频信息
 * @param       flag            : 更新内容标志, 1, 需要更新名字等信息; 0, 不需要更新名字等信息;
 * @retval      无
 */
void video_info_upd(__videodev *videodevx, _progressbar_obj *videoprgbx, _progressbar_obj *volprgbx, AVI_INFO *aviinfo, uint8_t flag)
{
    static uint32_t oldsec;
    uint16_t tempx, tempy;
    uint8_t *buf;
    float ftemp;
    uint32_t totsec = 0;    /* video文件总时间 */
    uint32_t cursec;        /* 当前播放时间 */

    if (flag == 1)   /* 需要更新名字等信息 */
    {
        buf = gui_memin_malloc(100);    /* 申请100字节内存 */

        if (buf == NULL)return; /* game over */

        gui_fill_rectangle(0, vui->tpbar_height + vui->xygap - 1, lcddev.width, vui->msgfsize + 2, AUDIO_MAIN_BKCOLOR); /* 上下各多清空一点,清空之前的显示 */
        gui_show_ptstrwhiterim(vui->xygap, vui->tpbar_height + vui->xygap, lcddev.width - vui->xygap, lcddev.height, 0, 0X0000, 0XFFFF, vui->msgfsize, videodevx->name);    /* 显示新的名字 */
        gui_phy.back_color = AUDIO_MAIN_BKCOLOR; /* 设置背景色为底色 */

        /* 显示音量百分比 */
        video_show_vol((volprgbx->curpos * 100) / volprgbx->totallen); /* 显示音量百分比 */

        /* 显示曲目编号 */
        sprintf((char *)buf, "%03d/%03d", videodevx->curindex + 1, videodevx->mfilenum);
        tempx = vui->vbarx + vui->vbarwidth - 7 * (vui->msgfsize) / 2;
        tempy = vui->vbary + vui->xygap / 2 + vui->vbarheight;
        gui_fill_rectangle(tempx, tempy, 7 * (vui->msgfsize) / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);   /* 清空之前的显示 */
        gui_show_string(buf, tempx, tempy, 7 * (vui->msgfsize) / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);

        /* 显示xxxKhz */
        tempx = vui->vbarx + vui->vbarwidth + vui->msgdis;
        gui_fill_rectangle(tempx, vui->vbary, 9 * vui->msgfsize / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);    /* 清空之前的显示 */
        ftemp = (float)aviinfo->SampleRate / 1000; /* xxx.xKhz */
        sprintf((char *)buf, "%2.3fKhz", ftemp);
        gui_show_string(buf, tempx, vui->vbary, 9 * vui->msgfsize / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);

        /* 显示分辨率 */
        tempy = vui->vbary + vui->xygap / 2 + vui->vbarheight;
        gui_fill_rectangle(tempx, tempy, 11 * (vui->msgfsize) / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);	/* 清空之前的显示 */
        sprintf((char *)buf, "Pix:%dX%d", aviinfo->Width, aviinfo->Height);
        gui_show_string(buf, tempx, tempy, 11 * (vui->msgfsize) / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);	/* 显示分辨率 */

        /* 其他处理 */
        oldsec = 0;
        videoprgbx->totallen = videodevx->file->obj.objsize;        /* 更新总长度 */
        videoprgbx->curpos = 0;
        gui_memin_free(buf);/* 释放内存 */
    }

    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;   /* 歌曲总长度(单位:ms) */
    totsec /= 1000; /* 秒钟数 */
    cursec = ((double)videodevx->file->fptr / videodevx->file->obj.objsize) * totsec; /* 当前播放到多少秒了? */
    videoprgbx->curpos = f_tell(videodevx->file);   /* 得到当前的播放位置 */
    progressbar_draw_progressbar(videoprgbx);       /* 更新进度条位置 */

    if (oldsec != cursec)
    {
        oldsec = cursec;
        /* 显示帧数 */
        buf = gui_memin_malloc(100);	/* 申请100字节内存 */

        if (buf == NULL)return;

        tempx = vui->vbarx + vui->vbarwidth + vui->msgdis + 9 * (vui->msgfsize) / 2 + vui->xygap;
        gui_fill_rectangle(tempx, vui->vbary, 5 * (vui->msgfsize) / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);		/* 清空之前的显示 */
        sprintf((char *)buf, "%02dfps", framecntout);
        gui_show_string(buf, tempx, vui->vbary, 5 * (vui->msgfsize) / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);
        gui_memin_free(buf);/* 释放内存 */
        /* 显示时间 */
        tempx = vui->pbarx - 7 * vui->msgfsize / 2;
        video_time_show(tempx, vui->pbary, cursec);			/* 显示播放时间 */
        tempx = vui->pbarx + vui->pbarwidth + vui->msgfsize / 2;
        video_time_show(tempx, vui->pbary, totsec);			/* 显示总时间 */
    }
}

/* 视频播放提示语 */
uint8_t *const video_msg_tbl[3][GUI_LANGUAGE_NUM] =
{
    {"读取文件错误!!", "讀取文件錯誤!!", "File Read Error!",},
    {"内存不够!!", "內存不夠!!", "Out of memory!",},
    {"分辨率不支持", "分辨率不支持", "Image Size Error!",},
};

/**
 * @brief       视频播放
 * @param       无
 * @retval      播放结果
 */
uint8_t video_play(void)
{
    uint8_t rval = 0;   /* 返回值 */
    uint8_t *pname = 0;
    uint16_t i;
    uint8_t key;

    _btn_obj *rbtn;     /* 返回按钮控件 */
    _filelistbox_obj *flistbox;
    _filelistbox_list *filelistx;   /* 文件 */
    memset(&videodev, 0, sizeof(__videodev)); /* videodev结构体清零 */
    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[3][gui_phy.language], 0X07); /* 选择目标文件,并得到目标数量 */

    flistbox = filelistbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.listfsize); /* 创建一个filelistbox */

    if (flistbox == NULL)rval = 1;          /* 申请内存失败 */
    else
    {
        flistbox->fliter = FLBOX_FLT_VIDEO; /* 图片文件 */
        filelistbox_add_disk(flistbox);     /* 添加磁盘路径 */
        filelistbox_draw_listbox(flistbox);
    }

    rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* 创建文字按钮 */

    if (rbtn == NULL)rval = 1;  /* 没有足够内存够分配 */
    else
    {
        rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language];	/* 名字 */
        rbtn->font = gui_phy.tbfsize; /* 设置新的字体大小 */
        rbtn->bcfdcolor = WHITE;    /* 按下时的颜色 */
        rbtn->bcfucolor = WHITE;    /* 松开时的颜色 */
        btn_draw(rbtn); /* 画按钮 */
    }

    while (rval == 0)   /* 主循环 */
    {
        tp_dev.scan(0); /* 扫描触摸屏 */
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
        delay_ms(5);

        if (system_task_return)break;           /* TPAD返回 */

        filelistbox_check(flistbox, &in_obj);   /* 扫描文件 */

        if (flistbox->dbclick == 0X81)          /* 双击文件了 */
        {
            gui_memin_free(videodev.path);      /* 释放内存 */
            gui_memin_free(videodev.mfindextbl);    /* 释放内存 */
            videodev.path = (uint8_t *)gui_memin_malloc(strlen((const char *)flistbox->path) + 1); /* 为新的路径申请内存 */

            if (videodev.path == NULL)
            {
                rval = 1;    /* 内存分配失败 */
                break;
            }

            videodev.path[0] = '\0';    /* 在最开始加入结束符 */
            strcpy((char *)videodev.path, (char *)flistbox->path);
            videodev.mfindextbl = (uint32_t *)gui_memin_malloc(flistbox->filecnt * 4); /* 为新的tbl申请内存 */

            if (videodev.mfindextbl == NULL)
            {
                rval = 1;    /* 内存分配失败 */
                break;
            }

            for (i = 0; i < flistbox->filecnt; i++)videodev.mfindextbl[i] = flistbox->findextbl[i]; /* 复制 */

            videodev.mfilenum = flistbox->filecnt;  /* 记录文件个数 */
            videodev.curindex = flistbox->selindex - flistbox->foldercnt; /* 当前应该播放的文件索引 */
            printf("play:%d/%d\r\n", videodev.curindex, videodev.mfilenum);
            rval = video_play_mjpeg();

            if (rval == 0XFF)break;
            else
            {
                if (rval)   /* 存在错误 */
                {
                    if (rval < 20)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)video_msg_tbl[0][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);       /* 文件读取错误 */

                    if (rval == 0X21)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)video_msg_tbl[1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);    /* 内存错误 */

                    if (rval == 0X22)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)video_msg_tbl[2][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);    /* 分辨率错误 */

                    delay_ms(2000);
                    rval = 0;
                }

                flistbox->dbclick = 0;  /* 设置非文件浏览状态 */
                app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[3][gui_phy.language], 0X07); /* 选择目标文件,并得到目标数量 */
                btn_draw(rbtn);         /* 画按钮 */
                flistbox->selindex = flistbox->foldercnt + videodev.curindex; /* 选中条目为当前正在播放的条目 */
                filelistbox_rebuild_filelist(flistbox);/* 重建flistbox */
                system_task_return = 0; /* 还不能退出视频播放 */
            }
        }

        key = btn_check(rbtn, &in_obj);

        if (key && ((rbtn->sta & 0X80) == 0))
        {
            if (flistbox->dbclick != 0X81)   /* 在文件浏览的时候按了返回按钮,则返回上一层目录 */
            {
                filelistx = filelist_search(flistbox->list, flistbox->selindex); /* 得到此时选中的list的信息 */

                if (filelistx->type == FICO_DISK)   /* 已经不能再往上了,则退出视频浏览 */
                {
                    break;
                }
                else filelistbox_back(flistbox);   /* 退回上一层目录 */
            }
        }
    }

    filelistbox_delete(flistbox);           /* 删除filelist */
    btn_delete(rbtn);                       /* 删除按钮 */
    gui_memin_free(pname);                  /* 释放内存 */
    gui_memin_free(videodev.path);          /* 释放内存 */
    gui_memin_free(videodev.mfindextbl);    /* 释放内存 */
    return rval;
}

extern volatile uint8_t memshow_flag;/* mem打印使能标志 */

/**
 * @brief       播放mjpeg文件
 * @param       无
 * @retval      0, 正常播放结束,返回文件浏览界面
 *              0XFF, 按退出按钮,直接结束视频播放
 *              其他, 错误代码
 */
uint8_t video_play_mjpeg(void)
{
    uint8_t dirmode = 0;    /* 横竖屏模式:0,竖屏模式;1,横屏模式. */
    uint8_t *pname = 0;     /* 带路径的名字 */
    uint8_t *framebuf = 0;  /* 视频解码buf */
    uint8_t *pbuf = 0;      /* buf指针 */
    DIR videodir;           /* video目录 */
    FILINFO *videoinfo;     /* video文件信息 */
    _progressbar_obj *videoprgb = 0, *volprgb = 0;
    _btn_obj *tbtn[5];
    uint16_t lastvolpos;
    uint8_t btnsize = 0;    /* 按钮尺寸 */
    uint8_t btnxpit = 0;    /* 按钮在x方向上的间隙 */
    uint8_t i;
    uint8_t tcnt = 0;

    uint8_t  res = 0;
    uint16_t offset = 0;
    uint32_t nr;
    uint8_t saisavebuf = 0;
    uint16_t videoheight;   /* 视频显示区域高度 */
    uint16_t yoff;
    uint8_t key = 0;
    uint8_t playflag = 0;


    for (i = 0; i < 5; i++)tbtn[i] = 0;

//    if(audiodev.status&(1<<7))      /* 当前在放歌?? */
//    {
//        audio_stop_req(&audiodev);  /* 停止音频播放 */
//        audio_task_delete();        /* 删除音乐播放任务 */
//    }
//    app_es8388_volset(es8388set.mvol);
//    ES8388_ADDA_Cfg(1,0);           /* 开启DAC关闭ADC */
//    ES8388_Output_Cfg(1,1);         /* 开启DAC输出 */
//    ES8388_Input_Cfg(0);            /* 关闭输入通道 */

    videoinfo = (FILINFO *)gui_memin_malloc(sizeof(FILINFO));   /* 申请FILENFO内存 */
    framebuf = gui_memin_malloc(AVI_VIDEO_BUF_SIZE);            /* 申请视频buf */
    videodev.saibuf[0] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* 申请音频内存 */
    videodev.saibuf[1] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* 申请音频内存 */
    videodev.saibuf[2] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* 申请音频内存 */
    videodev.saibuf[3] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* 申请音频内存 */
    videodev.file = (FIL *)gui_memin_malloc(sizeof(FIL));       /* 申请videodev.file内存 */
    vui = (__videoui *)gui_memin_malloc(sizeof(__videoui));

    if (!videodev.saibuf[3] || !framebuf || !videodev.file)
    {
        res = 0X21;
    }
    else
    {
        memset(videodev.saibuf[0], 0, AVI_AUDIO_BUF_SIZE);
        memset(videodev.saibuf[1], 0, AVI_AUDIO_BUF_SIZE);
        memset(videodev.saibuf[2], 0, AVI_AUDIO_BUF_SIZE);
        memset(videodev.saibuf[3], 0, AVI_AUDIO_BUF_SIZE);
        video_load_ui();            /* 加载主界面 */
        videoprgb = progressbar_creat(vui->pbarx, vui->pbary, vui->pbarwidth, vui->pbarheight, 0X20);   /* audio播放进度条 */

        if (videoprgb == NULL)res = 0X21;

        volprgb = progressbar_creat(vui->vbarx, vui->vbary, vui->vbarwidth, vui->vbarheight, 0X20);     /* 声音大小进度条 */

        if (volprgb == NULL)res = 0X21;

        volprgb->totallen = 30;
        videoheight = lcddev.height - (vui->tpbar_height + vui->msgbar_height + vui->prgbar_height + vui->btnbar_height);
        yoff = vui->tpbar_height + vui->msgbar_height;
//        if(es8388set.mvol<=30)volprgb->curpos=es8388set.mvol;
//        else  /* 错误的数据 */
//        {
//            es8388set.mvol=0;
//            volprgb->curpos=0;
//        }
        lastvolpos = volprgb->curpos; /* 设定最近的位置 */

        switch (lcddev.width)
        {
            case 240:
                btnsize = 48;
                break;

            case 272:
                btnsize = 50;
                break;

            case 320:
                btnsize = 60;
                break;

            case 480:
                btnsize = 80;
                break;

            case 600:
                btnsize = 100;
                break;

            case 800:
                btnsize = 150;
                break;

        }

        btnxpit = (lcddev.width - 5 * btnsize) / 5;

        for (i = 0; i < 5; i++)   /* 循环创建5个按钮 */
        {
            tbtn[i] = btn_creat(btnxpit / 2 + i * (btnsize + btnxpit), lcddev.height - btnsize - (vui->btnbar_height - btnsize) / 2, btnsize, btnsize, 0, 1); /* 创建图片按钮 */

            if (tbtn[i] == NULL)
            {
                res = 0X21;    /* 创建失败 */
                break;
            }

            tbtn[i]->bcfdcolor = 0X2CFF;            /* 按下时的背景色 */
            tbtn[i]->bcfucolor = AUDIO_BTN_BKCOLOR; /* 松开时背景色 */
            tbtn[i]->picbtnpathu = (uint8_t *)AUDIO_BTN_PIC_TBL[0][i];
            tbtn[i]->picbtnpathd = (uint8_t *)AUDIO_BTN_PIC_TBL[1][i];
            tbtn[i]->sta = 0;
        }

        if (res == 0)
        {
            res = f_opendir(&videodir, (const TCHAR *)videodev.path); /* 打开选中的目录 */
        }
    }

    if (res == 0)
    {
        videoprgb->inbkcolora = 0x738E; /* 默认色 */
        videoprgb->inbkcolorb = AUDIO_INFO_COLOR;   /* 默认色 */
        videoprgb->infcolora = 0X75D;   /* 默认色 */
        videoprgb->infcolorb = 0X596;   /* 默认色 */
        volprgb->inbkcolora = AUDIO_INFO_COLOR;     /* 默认色 */
        volprgb->inbkcolorb = AUDIO_INFO_COLOR;     /* 默认色 */
        volprgb->infcolora = 0X75D;     /* 默认色 */
        volprgb->infcolorb = 0X596;     /* 默认色 */

        for (i = 0; i < 5; i++)btn_draw(tbtn[i]);   /* 画按钮 */

        tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PLAYR_PIC; /* 按下一次之后变为播放松开状态 */
        progressbar_draw_progressbar(videoprgb);    /* 画进度条 */
        progressbar_draw_progressbar(volprgb);      /* 画进度条 */
    }

    while (res == 0)
    {
        ff_enter(videodir.obj.fs);/* 进入fatfs,防止被打断 */
        dir_sdi(&videodir, videodev.mfindextbl[videodev.curindex]);
        ff_leave(videodir.obj.fs);/* 退出fatfs,继续运行os等 */
        res = f_readdir(&videodir, videoinfo); /* 读取文件信息 */

        if (res)break; /* 打开失败 */

        videodev.name = (uint8_t *)(videoinfo->fname);
        pname = gui_memin_malloc(strlen((const char *)videodev.name) + strlen((const char *)videodev.path) + 2); /* 申请内存 */

        if (pname == NULL)
        {
            res = 0X21;    /* 申请失败 */
            break;
        }

        pname = gui_path_name(pname, videodev.path, videodev.name);	/* 文件名加入路径 */
        res = f_open(videodev.file, (char *)pname, FA_READ);
        gui_memin_free(pname);  /* 释放内存 */

        if (res == 0)
        {
            pbuf = framebuf;
            res = f_read(videodev.file, pbuf, AVI_VIDEO_BUF_SIZE, &nr); /* 开始读取 */

            if (res)   /* 文件系统错误,直接退出不再播放 */
            {
                break;
            }

            /* 开始avi解析 */
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);   /* avi解析 */

            if (res)   /* 一般问题,还可以继续播放 */
            {
                printf("avi err:%d\r\n", res);
                break;
            }

            if (g_avix.Height > videoheight || g_avix.Width > lcddev.width)   /* 图片尺寸有问题,直接退出不再播放 */
            {
                if (g_avix.Height <= lcddev.width && g_avix.Width <= lcddev.height)   /* 横屏时,可以显示出来,则启用横屏模式 */
                {
                    dirmode = 1;    /* 横屏模式 */
                    lcd_display_dir(1);     /* 改为横屏 */
                    tp_dev.touchtype = 1;   /* 触摸也改成横屏 */
                    yoff = 0;
                    videoheight = lcddev.height;
                }
                else     /* 横屏也无法显示 */
                {
                    res = 0X22;
                    printf("avi size error\r\n");
                    break;
                }
            }
            else
            {
                if (g_avix.Height >= 480 && g_avix.Width >= 800 && (1000000 / g_avix.SecPerFrame) >= 15)   /* 视频分辨率大于等于800*480,且帧率大于等于15帧的时候,需要横屏显示,已满足速度要求 */
                {
                    dirmode = 1;            /* 横屏模式 */
                    lcd_display_dir(1);     /* 改为横屏 */
                    tp_dev.touchtype = 1;   /* 触摸也改成横屏 */
                    yoff = 0;
                    videoheight = lcddev.height;
                }
            }

            tim6_int_init(g_avix.SecPerFrame / 100 - 1, 24000 - 1);   /* 10Khz计数频率,加1是100us,用于视频帧间隔控制 */
            tim8_int_init(10000 - 1, 24000 - 1);        /* 10Khz计数,1秒钟中断一次,用于帧计数 */
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi"); /* 寻找movi ID */
            avi_get_streaminfo(pbuf + offset + 4);      /* 获取流信息 */
            f_lseek(videodev.file, offset + 12);        /* 跳过标志ID,读地址偏移到流数据开始处 */
            res = mjpeg_init((lcddev.width - g_avix.Width) / 2, yoff + (videoheight - g_avix.Height) / 2, g_avix.Width, g_avix.Height); /* JPG解码初始化 */

            if (g_avix.SampleRate)    /* 有音频信息,才初始化 */
            {
//                ES8388_I2S_Cfg(0, 3);   /* 飞利浦标准,16位数据长度 */
//                SAIA_Init(0, 1, 4);     /* 设置SAI,主发送,16位数据 */
//                SAIA_SampleRate_Set(g_avix.SampleRate);/* 设置采样率 */
//                SAIA_TX_DMA_Init(videodev.saibuf[1], videodev.saibuf[2], g_avix.AudioBufSize / 2, 1); /* 配置DMA */
//                sai_tx_callback = audio_sai_dma_callback;	/* 回调函数指向sai_DMA_Callback */
//                videodev.saiplaybuf = 0;
//                saisavebuf = 0;
//                SAI_Play_Start();   /* 开启sai播放 */
            }

            gui_fill_rectangle(0, yoff, lcddev.width, videoheight, BLACK);  /* 清除视频区域 */

            if (dirmode == 0)video_info_upd(&videodev, videoprgb, volprgb, &g_avix, 1);   /* 更新所有信息 */

            tcnt = 0;
            playflag = 0;
            videodev.status = 3;    /* 非暂停,非快进快退 */
            memshow_flag = 0;       /* 不打印内存使用率 */

            while (playflag == 0)   /* 播放循环 */
            {
                if (videodev.status & (1 << 0) && videodev.status & (1 << 1))
                {
                    if (g_avix.StreamID == AVI_VIDS_FLAG)   /* 视频流 */
                    {
                        pbuf = framebuf;
                        f_read(videodev.file, pbuf, g_avix.StreamSize + 8, &nr); /* 读入整帧+下一数据流ID信息 */
                        res = mjpeg_decode(pbuf, g_avix.StreamSize);

                        if (res)
                        {
                            printf("decode error!\r\n");
                        }

                        while (aviframeup == 0);    /* 等待时间到达(在TIM6的中断里面设置为1) */

                        aviframeup = 0;     /* 标志清零 */
                        framecnt++;
                    }
                    else if (g_avix.StreamID == AVI_AUDS_FLAG)     /* 音频流 */
                    {
//                        saisavebuf++;

//                        if (saisavebuf > 3)saisavebuf = 0;

//                        do
//                        {
//                            nr = videodev.saiplaybuf;

//                            if (nr)nr--;
//                            else nr = 3;
//                        } while (saisavebuf == nr); /* 碰撞等待 */

                        f_read(videodev.file, videodev.saibuf[saisavebuf], g_avix.StreamSize + 8, &nr); /* 填充videodev.saibuf */
                        pbuf = videodev.saibuf[saisavebuf];
                    }
                }
                else     /* 暂停状态 */
                {
                    delay_ms(1000 / OS_TICKS_PER_SEC);
                }

                if (dirmode == 0)   /* 竖屏模式,才检测触摸屏 */
                {
                    tp_dev.scan(0);
                    in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);	/* 得到按键键值 */

                    for (i = 0; i < 5; i++)
                    {
                        res = btn_check(tbtn[i], &in_obj);

                        if (res && ((tbtn[i]->sta & (1 << 7)) == 0) && (tbtn[i]->sta & (1 << 6)))   /* 有按键按下且松开,并且TP松开了或者TPAD返回 */
                        {
                            switch (i)
                            {
                                case 0:/* file list */
                                    playflag = 0xff;
                                    break;

                                case 1:/* 上一个视频 */
                                    if (systemset.videomode != 1)   /* 非随机播放 */
                                    {
                                        if (videodev.curindex)videodev.curindex--;
                                        else videodev.curindex = videodev.mfilenum - 1;
                                    }
                                    else     /* 随机播放 */
                                    {
                                        videodev.curindex = app_get_rand(videodev.mfilenum); /* 得到下一首歌曲的索引 */
                                    }

                                    playflag = 1;
                                    break;

                                case 3: /* 下一个视频 */
                                    if (systemset.videomode != 1)   /* 非随机播放 */
                                    {
                                        if (videodev.curindex < (videodev.mfilenum - 1))videodev.curindex++;
                                        else videodev.curindex = 0;
                                    }
                                    else     /* 随机播放 */
                                    {
                                        videodev.curindex = app_get_rand(videodev.mfilenum); /* 得到下一首歌曲的索引 */
                                    }

                                    playflag = 3;
                                    break;

                                case 2: /* 播放/暂停 */
                                    if (videodev.status & (1 << 0))   /* 是暂停 */
                                    {
                                        videodev.status &= ~(1 << 0);   /* 标记暂停 */
                                        //SAI_Play_Stop();              /* 关闭音频 */
                                        tbtn[2]->picbtnpathd = (uint8_t *)AUDIO_PLAYP_PIC;
                                        tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PAUSER_PIC;
                                    }
                                    else     /* 暂停状态 */
                                    {
                                        videodev.status |= 1 << 0;  /* 取消暂停 */
                                        //SAI_Play_Start();         /* 开启DMA播放 */
                                        tbtn[2]->picbtnpathd = (uint8_t *)AUDIO_PAUSEP_PIC;

                                        tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PLAYR_PIC;
                                    }

                                    break;

                                case 4:/* 停止播放,直接退出继续播放 */
                                    playflag = 4;
                                    break;
                            }
                        }
                    }

                    res = progressbar_check(volprgb, &in_obj); /* 检查音量进度条 */

                    if (res && lastvolpos != volprgb->curpos)   /* 被按下了,且位置变化了.执行音量调整 */
                    {
                        lastvolpos = volprgb->curpos;

//                        if (volprgb->curpos)es8388set.mvol = volprgb->curpos; /* 设置音量 */
//                        else es8388set.mvol = 0;

//                        app_es8388_volset(es8388set.mvol);
                        video_show_vol((volprgb->curpos * 100) / volprgb->totallen);	/* 显示音量百分比 */
                    }

                    res = progressbar_check(videoprgb, &in_obj);

                    if (res)   /* 被按下了,并且松开了,执行快进快退 */
                    {
                        videodev.status &= ~(1 << 1); /* 标志进入快进快退状态 */

                        //if (videodev.status & 1 << 0)SAI_Play_Stop(); /* 非暂停时,开始快进快退,关闭音频 */

                        video_seek(&videodev, &g_avix, framebuf, videoprgb->curpos);
                        pbuf = framebuf;
                        video_info_upd(&videodev, videoprgb, volprgb, &g_avix, 0); /* 更新显示信息,每100ms执行一次 */
                    }
                    else if ((videodev.status & (1 << 1)) == 0)     /* 处于快进/快退状态? */
                    {
                        //if (videodev.status & 1 << 0)SAI_Play_Start(); /* 非暂停状态,快进快退结束,打开音频 */

                        videodev.status |= 1 << 1;  /* 取消快进快退状态 */
                    }

                    tcnt++;

                    if ((tcnt % 20) == 0)video_info_upd(&videodev, videoprgb, volprgb, &g_avix, 0); /* 更新显示信息,每100ms执行一次 */
                }
                else     /* 横屏模式,检测按键 */
                {
                    key = key_scan(0);

                    switch (key)
                    {
                        case WKUP_PRES:/* KEY1按下了 */
                            if (systemset.videomode != 1)   /* 非随机播放 */
                            {
                                if (videodev.curindex < (videodev.mfilenum - 1))videodev.curindex++;
                                else videodev.curindex = 0;
                            }
                            else    /* 随机播放 */
                            {
                                videodev.curindex = app_get_rand(videodev.mfilenum); /* 得到下一首歌曲的索引 */
                            }

                            playflag = 3; /* 播放下一个视频 */
                            break;

                        case KEY1_PRES:/* KEY1按下了 */
                            if (systemset.videomode != 1)   /* 非随机播放 */
                            {
                                if (videodev.curindex)videodev.curindex--;
                                else videodev.curindex = videodev.mfilenum - 1;
                            }
                            else        /* 随机播放 */
                            {
                                videodev.curindex = app_get_rand(videodev.mfilenum); /* 得到下一首歌曲的索引 */
                            }

                            playflag = 1; /* 播放上一个视频 */
                            break;

//                        case WKUP_PRES: /* WAKE UP按下 */
//                            SAI_Play_Stop();/* 关闭音频 */
//                            video_seek_key(videodev.file, &g_avix, framebuf);
//                            pbuf = framebuf;
//                            SAI_Play_Start();/* 开启DMA播放 */
//                            break;

                    }
                }

                if (system_task_return)  	/* TPAD返回，返回目录 */
                {
                    playflag = 0XFF;
                    break;
                }

                if (videodev.status & (1 << 0) && videodev.status & (1 << 1))  	/* 非暂停状态和快进快退状态 */
                {
                    if (avi_get_streaminfo(pbuf + g_avix.StreamSize))  	/* 读取下一帧 流标志,如果出错,可能是电影结束了 */
                    {
                        pbuf = framebuf;
                        res = f_read(videodev.file, pbuf, AVI_VIDEO_BUF_SIZE, &nr);	/* 开始读取 */

                        if (res == 0 && nr == AVI_VIDEO_BUF_SIZE)  	/* 读取成功,且读取了指定长度的数据 */
                        {
                            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "00dc"); /* 寻找AVI_VIDS_FLAG,00dc */
                            avi_get_streaminfo(pbuf + offset);	/* 获取流信息 */

                            if (offset)f_lseek(videodev.file, (videodev.file->fptr - AVI_VIDEO_BUF_SIZE) + offset + 8);
                        }
                        else
                        {
                            if (nr == 0)            /* 到了文件末尾了,结束播放 */
                            {
                                playflag = 0XFE;    /* 播放错误，跳到下一个视频 */
                                break;              /* 电影结束了 */
                            }
                            else                    /* 文件还没到末尾,继续查找视频帧 */
                            {
                                //printf("shit:%x\r\n",g_avix.StreamID);
                                //printf("shit1:%d\r\n",g_avix.StreamSize);
                                //SAI_Play_Stop();  /* 关闭音频 */
                                if (video_seek_vids(&videodev, &g_avix, framebuf))    /* 查找下一个视频帧 */
                                {
                                    printf("shit:%x\r\n",g_avix.StreamID);
                                    printf("shit1:%d\r\n",g_avix.StreamSize);
                                    playflag = 0XFE;/* 播放错误，跳到下一个视频 */
                                    break;          /* 电影结束了 */
                                }

                                //SAI_Play_Start(); /* 开启DMA播放 */
                            }
                        }
                    }
                }

                if (nr == 0)break;
            }

            if (playflag == 0XFE)playflag = 0; /* 播放下一个视频 */

            memshow_flag = 1;   /* 打印内存使用率 */
            //SAI_Play_Stop();  /* 关闭音频 */
            TIM6->CR1 &= ~(1 << 0); /* 关闭定时器6 */
            TIM8->CR1 &= ~(1 << 0); /* 关闭定时器8 */
            mjpeg_free();       /* 释放内存 */
            f_close(videodev.file);

            if (dirmode)        /* 之前是横屏 */
            {
                dirmode = 0;    /* 恢复竖屏模式 */
                lcd_display_dir(0);     /* 改为竖屏 */
                tp_dev.touchtype = 0;   /* 触摸也改成竖屏 */
                video_load_ui();        /* 加载主界面 */
                videoheight = lcddev.height - (vui->tpbar_height + vui->msgbar_height + vui->prgbar_height + vui->btnbar_height);
                yoff = vui->tpbar_height + vui->msgbar_height;
                videoprgb->inbkcolora = 0x738E;     /* 默认色 */
                videoprgb->inbkcolorb = AUDIO_INFO_COLOR;   /* 默认色 */
                videoprgb->infcolora = 0X75D;       /* 默认色 */
                videoprgb->infcolorb = 0X596;       /* 默认色 */
                volprgb->inbkcolora = AUDIO_INFO_COLOR;     /* 默认色 */
                volprgb->inbkcolorb = AUDIO_INFO_COLOR;     /* 默认色 */
                volprgb->infcolora = 0X75D;         /* 默认色 */
                volprgb->infcolorb = 0X596;         /* 默认色 */

                for (i = 0; i < 5; i++)btn_draw(tbtn[i]);   /* 画按钮 */

                tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PLAYR_PIC; /* 按下一次之后变为播放松开状态 */
                progressbar_draw_progressbar(videoprgb);    /* 画进度条 */
                progressbar_draw_progressbar(volprgb);      /* 画进度条 */
            }

            if (playflag == 0)
            {
                if (systemset.videomode == 0)   /* 顺序播放 */
                {
                    if (videodev.curindex < (videodev.mfilenum - 1))videodev.curindex++;
                    else videodev.curindex = 0;
                }
                else if (systemset.videomode == 1)     /* 随机播放 */
                {
                    videodev.curindex = app_get_rand(videodev.mfilenum); /* 得到下一首歌曲的索引 */
                }
                else videodev.curindex = videodev.curindex;   /* 单曲循环 */
            }
            else if (playflag == 0XFF)
            {
                res = 0;
                break;
            }
            else if (playflag == 4)
            {
                res = 0XFF;
                break;
            }
        }
    }

    gui_memin_free(videodev.saibuf[0]);
    gui_memin_free(videodev.saibuf[1]);
    gui_memin_free(videodev.saibuf[2]);
    gui_memin_free(videodev.saibuf[3]);
    gui_memin_free(framebuf);
    gui_memin_free(videoinfo);
    gui_memin_free(videodev.file);
    gui_memin_free(vui);

    for (i = 0; i < 5; i++)if (tbtn[i])btn_delete(tbtn[i]); /* 删除按钮 */

    if (videoprgb)progressbar_delete(videoprgb);

    if (volprgb)progressbar_delete(volprgb);

//    ES8388_ADDA_Cfg(0,0);   /* 关闭DAC&ADC */
//    ES8388_Input_Cfg(0);    /* 关闭输入通道 */
//    ES8388_Output_Cfg(0,0); /* 关闭DAC输出 */
//    app_es8388_volset(0);   /* 关闭ES8388音量输出 */
    slcd_dma_init();            /* 硬件JPEG解码,会重置dma2stream0,所以必须重新初始化 */
    return res;
}

/**
 * @brief       avi 视频标志查找,用于出错后查找视频帧
 * @param       videodevx       : video 控制器
 * @param       aviinfo         : avi文件信息
 * @param       mbuf            : 数据缓冲区
 * @retval      0, 成功;  其他, 失败;
 */
uint8_t video_seek_vids(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos;
    uint8_t *pbuf;
    uint32_t br;
    uint8_t res = 0;

    while (1)
    {
        fpos = videodevx->file->fptr;       /* 保存当前位置 */
        res = f_read(videodevx->file, mbuf, AVI_VIDEO_BUF_SIZE, &br);   /* 读入整帧+下一数据流ID信息 */

        if (res || br != AVI_VIDEO_BUF_SIZE)
        {
            if (br != AVI_VIDEO_BUF_SIZE)res = 0XFF; /* 标记出错 */

            break;
        }

        pbuf = mbuf;
        br = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG);   /* 寻找视频帧 */

        if (br != 0XFFFFFFFF)
        {
            avi_get_streaminfo(pbuf + br);  /* 获取流信息 */

            if (g_avix.StreamSize < 100 || g_avix.StreamSize > AVI_VIDEO_BUF_SIZE)continue; /* 错误的视频帧尺寸 */

            f_lseek(videodevx->file, fpos + br + 8);    /* 跳过标志ID,读地址偏移到流数据开始处 */
            break;
        }
    }

    return res;
}

/**
 * @brief       avi文件查找
 * @param       videodevx       : video 控制器
 * @param       aviinfo         : avi文件信息
 * @param       mbuf            : 数据缓冲区
 * @param       dstpos          : 目标位置
 * @retval      0, 成功;  其他, 失败;
 */
uint8_t video_seek(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf, uint32_t dstpos)
{
    uint32_t fpos = dstpos;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;

    f_lseek(videodevx->file, fpos); /* /偏移到目标位置 */
    f_read(videodevx->file, mbuf, AVI_VIDEO_BUF_SIZE, &br); /* 读入整帧+下一数据流ID信息 */
    pbuf = mbuf;

    if (fpos == 0)   /* 从0开始,得先寻找movi ID */
    {
        offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
    }
    else offset = 0;

    offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, aviinfo->VideoFLAG); /* 寻找视频帧 */
    avi_get_streaminfo(pbuf + offset);              /* 获取流信息 */
    f_lseek(videodevx->file, fpos + offset + 8);    /* 跳过标志ID,读地址偏移到流数据开始处 */

    if (aviinfo->StreamID == AVI_VIDS_FLAG)
    {
        f_read(videodevx->file, mbuf, aviinfo->StreamSize + 8, &br);    /* 读入整帧 */
        mjpeg_decode(mbuf, g_avix.StreamSize);      /* 显示视频帧 */
    }

    return 0;
}

/**
 * @brief       avi文件查找
 * @param       favi            : avi文件
 * @param       aviinfo         : avi文件信息
 * @param       mbuf            : 数据缓冲区
 * @retval      0, 成功;  其他, 失败;
 */
uint8_t video_seek_key(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos = favi->fptr;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;
    uint32_t delta;
    uint32_t totsec;
    uint8_t key;
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;
    totsec /= 1000; /* 秒钟数 */
    delta = (favi->obj.objsize / totsec) * 5; /* 每次前进5秒钟的数据量 */

    while (1)
    {
        key = key_scan(1);

        if (key == WKUP_PRES)   /* 快进 */
        {
            if (fpos < favi->obj.objsize)fpos += delta;

            if (fpos > (favi->obj.objsize - AVI_VIDEO_BUF_SIZE))
            {
                fpos = favi->obj.objsize - AVI_VIDEO_BUF_SIZE;
            }
        }

//        else if (key == KEY2_PRES)  /* 快退 */
//        {
//            if (fpos > delta)fpos -= delta;
//            else fpos = 0;
//        }
        else if (g_avix.StreamID == AVI_VIDS_FLAG)break;

        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);    /* 读入整帧+下一数据流ID信息 */
        pbuf = mbuf;

        if (fpos == 0)   /* 从0开始,得先寻找movi ID */
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else offset = 0;

        offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG);	/* 寻找视频帧 */
        avi_get_streaminfo(pbuf + offset);  /* 获取流信息 */
        f_lseek(favi, fpos + offset + 8);   /* 跳过标志ID,读地址偏移到流数据开始处 */

        if (g_avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, g_avix.StreamSize + 8, &br);   /* 读入整帧 */
            mjpeg_decode(mbuf, g_avix.StreamSize);            /* 显示视频帧 */
        }
        else
        {
            printf("error flag");
        }

        delay_ms(5);/* 延时5ms */
    }

    return 0;
}





















