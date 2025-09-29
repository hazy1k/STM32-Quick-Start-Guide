/**
 ****************************************************************************************************
 * @file        exeplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-运行器 代码
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

#include "exeplay.h"


/* 假函数,让PC指针跑到新的main函数去 */
dummyfun jump2app;


/* 执行BIN文件前的提示 */
uint8_t *const exeplay_remindmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "  执行BIN文件后,系统将无法继续运行,只能复位重启.您确认执行么?",
    "  執行BIN文件后,系統將無法繼續運行,只能復位重啟.您確認執行么?",
    "  After run the BIN file,SYSTEM will unable to continue.Confirm?",
};

/* 警告 */
uint8_t *const exeplay_warning_tbl[GUI_LANGUAGE_NUM] =
{
    "警告:",
    "警告:",
    "Warning:",
};

/* APP程序非法提示语 */
uint8_t *const exeplay_apperrmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "非法的SRAM APP程序！", "非法的SRAM APP程序！", "Illegal SRAM APP File!",
};

/**
 * @brief       写入标志值
 * @param       val             : 标志值
 * @retval      无
 */
void exeplay_write_appmask(uint16_t val)
{
    PWR->CR1 |= 1 << 8;     /* 后备区域访问使能(RTC+SRAM) */
    rtc_write_bkr(1, val);
}

/**
 * @brief       检查是否执行APP代码
 *  @note       在主函数最开始的时候被调用. 检测是否有app程序需要执行.如果是,则直接执行.
 * @param       无
 * @retval      无
 */
void exeplay_app_check(void)
{
    if (rtc_read_bkr(1) == 0X5050) /* 检查BKP1,如果为0X5050,则说明需要执行app代码 */
    {
        my_mem_copy((uint8_t *)EXEPLAY_APP_BASE, (uint8_t *)EXEPLAY_SRC_BASE, EXEPLAY_APP_SIZE);/* 拷贝EXEPLAY_APP_SIZE字节 */
        jump2app = (dummyfun) * (volatile uint32_t *)(EXEPLAY_APP_BASE + 4);                    /* 用户代码区第二个字为程序开始地址(复位地址) */
        exeplay_write_appmask(0X0000);                                                          /* 写入0,防止复位后再次执行app代码. */
        sys_msr_msp(*(volatile uint32_t *)EXEPLAY_APP_BASE);                                    /* 初始化APP堆栈指针(用户代码区的第一个字用于存放堆栈地址) */
        SysTick->CTRL = 0;                                                                      /* 关闭SYSTICK中断,以免影响APP执行 */
        jump2app();                                                                             /* 执行app代码. */
    }
}

/**
 * @brief       运行器
 * @param       无
 * @retval      0, 正常退出; 其他, 错误代码;
 */
uint8_t exe_play(void)
{
    FIL *f_exe;
    DIR exeplaydir;     /* exeplaydir专用 */
    FILINFO *exeplayinfo;
    uint8_t res;
    uint8_t rval = 0;   /* 返回值 */
    uint8_t *pname = 0;
    uint8_t *fn;

    uint8_t *buf;       /* 缓存 */
    uint8_t *p;
    uint8_t *pwr;       /* 写入时的首地址指针 */
    uint32_t readlen;   /* 总读取长度 */
    uint32_t bread;     /* 读取的长度 */
    uint32_t waddr;


    _btn_obj *rbtn;     /* 返回按钮控件 */
    _filelistbox_obj *flistbox;
    _filelistbox_list *filelistx;   /* 文件 */

    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[8][gui_phy.language], 0X07); /* 显示信息 */
    flistbox = filelistbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.listfsize); /* 创建一个filelistbox */

    if (flistbox == NULL)rval = 1;  /* 申请内存失败 */
    else
    {
        flistbox->fliter = FLBOX_FLT_BIN;   /* 查找BIN文件 */
        filelistbox_add_disk(flistbox);     /* 添加磁盘路径 */
        filelistbox_draw_listbox(flistbox);
    }

    buf = gui_memin_malloc(4096);           /* 申请4K字节内存 */
    f_exe = (FIL *)gui_memin_malloc(sizeof(FIL));   /* 开辟FIL字节的内存区域 */

    if (!f_exe || !buf)rval = 1; /* 申请失败 */

    rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* 创建文字按钮 */
    exeplayinfo = (FILINFO *)gui_memin_malloc(sizeof(FILINFO)); /* 申请FILENFO内存 */

    if (!exeplayinfo || !rbtn)rval = 1;	/* 没有足够内存够分配 */
    else
    {
        rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language]; /* 返回 */
        rbtn->font = gui_phy.tbfsize; /* 设置新的字体大小 */
        rbtn->bcfdcolor = WHITE;    /* 按下时的颜色 */
        rbtn->bcfucolor = WHITE;    /* 松开时的颜色 */
        btn_draw(rbtn);             /* 画按钮 */
    }

    while (rval == 0)
    {
        tp_dev.scan(0);
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
        delay_ms(5);

        if (system_task_return)break;           /* TPAD返回 */

        filelistbox_check(flistbox, &in_obj);   /* 扫描文件 */
        res = btn_check(rbtn, &in_obj);

        if (res)
        {
            if (((rbtn->sta & 0X80) == 0)) /* 按钮状态改变了 */
            {
                if (flistbox->dbclick != 0X81)
                {
                    filelistx = filelist_search(flistbox->list, flistbox->selindex); /* 得到此时选中的list的信息 */

                    if (filelistx->type == FICO_DISK) /* 已经不能再往上了 */
                    {
                        break;
                    }
                    else filelistbox_back(flistbox); /* 退回上一层目录 */
                }
            }
        }

        if (flistbox->dbclick == 0X81) /* 双击文件了 */
        {
            rval = f_opendir(&exeplaydir, (const TCHAR *)flistbox->path); /* 打开选中的目录 */

            if (rval)break;

            dir_sdi(&exeplaydir, flistbox->findextbl[flistbox->selindex - flistbox->foldercnt]);
            rval = f_readdir(&exeplaydir, exeplayinfo); /* 读取文件信息 */

            if (rval)break; /* 打开成功 */

            fn = (uint8_t *)(exeplayinfo->fname);
            pname = gui_memin_malloc(strlen((const char *)fn) + strlen((const char *)flistbox->path) + 2); /* 申请内存 */

            if (pname == NULL)rval = 1; /* 申请失败 */

            {
                pname = gui_path_name(pname, flistbox->path, fn);       /* 文件名加入路径 */
                rval = f_open(f_exe, (const TCHAR *)pname, FA_READ);    /* 只读方式打开文件 */

                if (rval)break;	/* 打开失败 */

                if (f_exe->obj.objsize < EXEPLAY_APP_SIZE) /* 可以放的下,并且用户确定执行 */
                {
                    p = mymalloc(SRAMIN, f_exe->obj.objsize);   /* 申请内存 */
                    pwr = p;    /* 记录数据首地址 */
                    readlen = 0;

                    while (readlen < f_exe->obj.objsize && (rval == 0))     /* 循环方式,读出BIN的所有内容 */
                    {
                        rval = f_read(f_exe, buf, 4096, (UINT *)&bread);    /* 读出bin里面的内容 */
                        readlen += bread;
                        my_mem_copy(p, buf, bread);
                        p += bread;
                    }

                    if (rval)break;	/* 打开失败,直接退出 */

                    if (((*(volatile uint32_t *)(pwr + 4)) & 0xFF000000) == 0x24000000) /* 判断是否为0X20XXXXXX.检测APP的合法性 */
                    {
                        res = window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 160) / 2, 200, 160, (uint8_t *)exeplay_remindmsg_tbl[gui_phy.language], (uint8_t *)exeplay_warning_tbl[gui_phy.language], 16, 0, 0X03, 0); /* 显示提醒信息 */

                        if (res == 1) /* 按下了确认键 */
                        {
                            waddr = EXEPLAY_SRC_BASE - 0X90000000;

                            while (readlen > 4096)	/* 循环方式,将数据写入SPI FLASH */
                            {
                                norflash_ex_write(pwr, waddr, 4096); /* 每次写入4096字节 */
                                pwr += 4096;
                                waddr += 4096;
                                readlen -= 4096;
                            }

                            if (readlen)
                            {
                                norflash_ex_write(pwr, waddr, readlen);	/* 写入最后一点数据 */
                            }

                            exeplay_write_appmask(0X5050);  /* 写入标志字,标志有app需要运行 */
                            norflash_ex_read(&res, SPILCD_END_ADDR - 1, 1); /* 读取标志位（存储在SPILCD存储区域的最后一个字节） */

                            if (res >= 2)	/* 是480*800或者以上分辨率的屏幕，直接清空标志，以便下次启动更新SPB主界面 */
                            {
                                res = 0XFF;
                                norflash_ex_write(&res, SPILCD_END_ADDR - 1, 1);
                            }

                            sys_soft_reset();   /* 产生一次软复位 */
                        }
                    }
                    else    /* 非法APP文件 */
                    {
                        window_msg_box((lcddev.width - 160) / 2, (lcddev.height - 80) / 2, 160, 80, (uint8_t *)exeplay_apperrmsg_tbl[gui_phy.language], (uint8_t *)exeplay_warning_tbl[gui_phy.language], 12, 0, 0, 0); /* 提示非法的APP文件 */
                        delay_ms(1500);/* 延时1.5秒 */
                    }

                    filelistbox_rebuild_filelist(flistbox);/* 重建flistbox */
                }
            }
            flistbox->dbclick = 0;  /* 设置非文件浏览状态 */
            gui_memin_free(pname);  /* 释放内存 */
        }
    }

    filelistbox_delete(flistbox);   /* 删除filelist */
    btn_delete(rbtn);               /* 删除按钮 */
    gui_memin_free(pname);          /* 释放内存 */
    gui_memin_free(exeplayinfo);    /* 释放内存 */
    gui_memin_free(f_exe);
    gui_memin_free(buf);            /* 释放内存 */
    return rval;
}















