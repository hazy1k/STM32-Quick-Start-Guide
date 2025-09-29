/**
 ****************************************************************************************************
 * @file        camera.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-照相机 代码
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

#include "camera.h"
#include "common.h"
#include "calendar.h"
#include "audioplay.h"
#include "facereco.h"
#include "qrplay.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/OV5640/ov5640.h"


extern uint8_t framecnt;                /* 统一的帧计数器,在timer.c里面定义 */
volatile uint8_t hsync_int = 0;         /* 帧中断标志 */
uint8_t ov_flash = 0;                   /* 闪光灯 */
uint8_t bmp_request = 0;                /* bmp拍照请求:0,无bmp拍照请求;1,有bmp拍照请求,需要在帧中断里面,关闭DCMI接口 */
uint8_t ovx_mode = 0;                   /* bit0:0,RGB565模式;1,JPEG模式 */
uint16_t curline = 0;                   /* 摄像头输出数据,当前行编号 */
uint16_t yoffset = 0;                   /* y方向的偏移量 */

#define jpeg_buf_size   460*1024        /* 定义JPEG数据缓存jpeg_buf的大小(460KB字节) */
#define jpeg_line_size	2*1024          /* 定义DMA接收数据时,一行数据的最大值 */

uint32_t *dcmi_line_buf[2];             /* RGB屏时,摄像头采用一行一行读取,定义行缓存 */
uint32_t *jpeg_data_buf;                /* JPEG数据缓存buf */

volatile uint32_t jpeg_data_len = 0;    /* buf中的JPEG有效数据长度 */
volatile uint8_t jpeg_data_ok = 0;      /**
                                         * JPEG数据采集完成标志
                                         * 0,数据没有采集完;
                                         * 1,数据采集完了,但是还没处理;
                                         * 2,数据已经处理完成了,可以开始下一帧接收
                                         */

/* 摄像头提示 */
uint8_t *const camera_remind_tbl[4][GUI_LANGUAGE_NUM] =
{
    {"初始化OV5640,请稍侯...", "初始化OV5640,請稍後...", "OV5640 Init,Please wait...",},
    {"未检测到OV5640,请检查...", "未檢測到OV5640,請檢查...", "No OV5640 find,Please check...",},
    {"保存为:", "保存為:", "SAVE AS:",},
    {"分辨率", "分辨率", "Resolution",},
};

/* 縮放提示 */
uint8_t *const camera_scalemsg_tbl[2][GUI_LANGUAGE_NUM] =
{
    "1:1显示(无缩放)！", "1:1顯示(無縮放)！", "1:1 Display(No Scale)！",
    "全尺寸缩放！", "全尺寸縮放！", "Full Scale！",
};

/* JPEG图片 12种尺寸 */
uint8_t *const camera_jpegimgsize_tbl[12] = 
{"QQVGA", "QVGA", "VGA", "SVGA", "XGA", "WXGA", "WXGA+", "SXGA", "UXGA", "1080P", "QXGA", "500W"};
/* 拍照成功提示框标题 */
uint8_t *const camera_saveok_caption[GUI_LANGUAGE_NUM] =
{
    "拍照成功！", "拍照成功！", "Take Photo OK！",
};

/* 拍照失败提示信息 */
uint8_t *const camera_failmsg_tbl[3][GUI_LANGUAGE_NUM] =
{
    "创建文件失败,请检查!", "創建文件失敗,請檢查!", "Creat File Failed,Please check!",
    "内存不足!", "內存不足!", "Out of memory!",
    "数据错误(图片尺寸太大)!", "數據錯誤(圖片尺寸太大)!", "Data Error(Picture is too big)!",
};

/* 00级功能选项表标题 */
uint8_t *const camera_l00fun_caption[GUI_LANGUAGE_NUM] =
{
    "相机设置", "相機設置", "Camera Set",
};

/* 00级功能选项表 */
uint8_t *const camera_l00fun_table[GUI_LANGUAGE_NUM][6] =
{
    {"场景设置", "特效设置", "亮度设置", "色度设置", "对比度设置", "闪光灯设置",},
    {"場景設置", "特效設置", "亮度設置", "色度設置", "對比度設置", "閃光燈設置",},
    {"Scene", "Effects", "Brightness", "Saturation", "Contrast", "Flashlight",},
};

/* 10级功能选项表 */
/* 场景模式 */
uint8_t *const camera_l10fun_table[GUI_LANGUAGE_NUM][5] =
{
    {"自动", "晴天", "办公室", "阴天", "家庭",},
    {"自動", "晴天", "辦公室", "陰天", "家庭",},
    {"Auto", "Sunny", "Office", "Cloudy", "Home"},
};

/* 11级功能选项表 */
/* 特效设置 */
uint8_t *const camera_l11fun_table[GUI_LANGUAGE_NUM][7] =
{
    {"正常", "冷色", "暖色", "黑白", "偏黄", "反色", "偏绿"},
    {"正常", "冷色", "暖色", "黑白", "偏黃", "反色", "偏綠"},
    {"Normal", "Cool", "Warm", "W&B", "Yellowish", "Invert", "Greenish",},
};

/* 12~14级功能选项表 */
/* 都是-3~3的7個值 */
uint8_t *const camera_l124fun_table[GUI_LANGUAGE_NUM][7] =
{
    {"-3", "-2", "-1", "0", "+1", "+2", "+3",},
    {"-3", "-2", "-1", "0", "+1", "+2", "+3",},
    {"-3", "-2", "-1", "0", "+1", "+2", "+3",},
};

/* 15级功能选项表 */
/* 闪光灯设置 */
uint8_t *const camera_l15fun_table[GUI_LANGUAGE_NUM][2] =
{
    {"关闭", "打开",},
    {"關閉", "打開",},
    {"OFF", "ON",},
};

/**
 * @brief       处理JPEG数据
 *  @note       当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
 * @param       无
 * @retval      无
 */
void jpeg_data_process(void)
{
    uint16_t i;
    uint16_t rlen;  /* 剩余数据长度 */
    uint32_t *pbuf;

    if (qr_dcmi_rgbbuf_sta & 0X02)      /* QR识别模式 */
    {
        qr_dcmi_curline = 0;            /* 行数清零 */
        qr_dcmi_rgbbuf_sta |= 1 << 0;   /* 标记数据准备好了 */
        return ;        /* 直接返回,无需执行剩下的代码 */
    }

    curline = yoffset;  /* 行数清零 */
    framecnt++;

    if (ovx_mode & 0X01)    /* 只有在JPEG格式下,才需要做处理 */
    {
        if (jpeg_data_ok == 0)  /* jpeg数据还未采集完? */
        {
            DMA1_Stream1->CR &= ~(1 << 0);      /* 停止当前传输 */

            while (DMA1_Stream1->CR & 0X01);    /* 等待DMA1_Stream1可配置 */

            rlen = jpeg_line_size - DMA1_Stream1->NDTR;     /* 得到剩余数据长度 */

            if (jpeg_data_len > (jpeg_buf_size / 4 - rlen)) /* 内存不够了,直接退出 */
            {
                printf("jpeg_data_len1:%d\r\n", jpeg_data_len); /* 打印当前长度（uint32_t） */
                jpeg_data_ok = 1;   /* 标记JPEG数据采集完按成,等待其他函数处理 */
                return;
            }

            pbuf = jpeg_data_buf + jpeg_data_len;   /* 偏移到有效数据末尾,继续添加 */

            if (DMA1_Stream1->CR & (1 << 19))for (i = 0; i < rlen; i++)pbuf[i] = dcmi_line_buf[1][i]; /* 读取buf1里面的剩余数据 */
            else for (i = 0; i < rlen; i++)pbuf[i] = dcmi_line_buf[0][i]; /* 读取buf0里面的剩余数据 */

            jpeg_data_len += rlen;  /* 加上剩余长度 */
            jpeg_data_ok = 1;       /* 标记JPEG数据采集完按成,等待其他函数处理 */
        }

        if (jpeg_data_ok == 2)      /* 上一次的jpeg数据已经被处理了 */
        {
            DMA1_Stream1->NDTR = jpeg_line_size; /* 传输长度为jpeg_buf_size*4字节 */
            DMA1_Stream1->CR |= 1 << 0; /* 重新传输 */
            jpeg_data_ok = 0;           /* 标记数据未采集 */
            jpeg_data_len = 0;          /* 数据重新开始 */
        }
    }
    else
    {
        if (bmp_request == 1)   /* 有bmp拍照请求,关闭DCMI */
        {
            dcmi_stop();        /* 停止DCMI */
            bmp_request = 0;    /* 标记请求处理完成 */
        }

        lcd_set_cursor(frec_dev.xoff, frec_dev.yoff);
        frec_curline = frec_dev.yoff;
        lcd_write_ram_prepare(); /* 开始写入GRAM */
        hsync_int = 1;
    }
}

/**
 * @brief       jpeg数据接收回调函数
 * @param       无
 * @retval      无
 */
void jpeg_dcmi_rx_callback(void)
{
    uint16_t i;
    uint32_t *pbuf;

    if (jpeg_data_len > (jpeg_buf_size / 4 - jpeg_line_size))   /* 内存不够了,直接退出 */
    {
        printf("jpeg_data_len:%d\r\n", jpeg_data_len);  /* 打印当前长度（uint32_t） */
        return;
    }

    pbuf = jpeg_data_buf + jpeg_data_len;   /* 偏移到有效数据末尾 */

    if (DMA1_Stream1->CR & (1 << 19))       /* buf0已满,正常处理buf1 */
    {
        for (i = 0; i < jpeg_line_size; i++)pbuf[i] = dcmi_line_buf[0][i]; /* 读取buf0里面的数据 */

        jpeg_data_len += jpeg_line_size; /* 偏移 */
    }
    else     /* buf1已满,正常处理buf0 */
    {
        for (i = 0; i < jpeg_line_size; i++)pbuf[i] = dcmi_line_buf[1][i]; /* 读取buf1里面的数据 */

        jpeg_data_len += jpeg_line_size; /* 偏移 */
    }
}

/**
 * @brief       RGB屏数据接收回调函数
 * @param       无
 * @retval      无
 */
//void rgblcd_dcmi_rx_callback(void)
//{
//    uint16_t *pbuf;

//    if (DMA1_Stream1->CR & (1 << 19))   /* DMA使用buf1,读取buf0 */
//    {
//        pbuf = (uint16_t *)dcmi_line_buf[0];
//    }
//    else    /* DMA使用buf0,读取buf1 */
//    {
//        pbuf = (uint16_t *)dcmi_line_buf[1];
//    }

//    LTDC_Color_Fill(0, curline, lcddev.width - 1, curline, pbuf); /* DM2D填充 */

//    if (curline < lcddev.height)curline++;
//}

/**
 * @brief       切换为OV5640模式
 *   @note      切换PC8/PC9/PC11为DCMI复用功能(AF13)
 * @param       无
 * @retval      无
 */
void sw_ov5640_mode(void)
{
    ov5640_write_reg(0X3017, 0XFF); /* 开启OV5650输出(可以正常显示) */
    ov5640_write_reg(0X3018, 0XFF);
    
    /* GPIOC8/9/11切换为 DCMI接口 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN8, 13);  /* PC8 , AF13  DCMI_D2 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN9, 13);  /* PC9 , AF13  DCMI_D3 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN11, 13); /* PC11, AF13  DCMI_D4 */
}

/**
 * @brief       切换为SD卡模式
 *   @note      切换PC8/PC9/PC11为SDMMC复用功能(AF12)
 * @param       无
 * @retval      无
 */
void sw_sdcard_mode(void)
{
    ov5640_write_reg(0X3017, 0X00); /* 关闭OV5640全部输出(不影响SD卡通信) */
    ov5640_write_reg(0X3018, 0X00);
    
    /* GPIOC8/9/11切换为 SDIO接口 */ 
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN8, 12);  /* PC8 , AF12  SD1_D0 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN9, 12);  /* PC9 , AF12  SD1_D1 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN11, 12); /* PC11, AF12  SD1_D3 */
}

/**
 * @brief       得到文件名,按文日期时间命名
 *  @note       bmp组合成:形如"0:PHOTO/PIC20120321210633.bmp"/"2:PHOTO/PIC20120321210633.bmp"的文件名
 *              jpg组合成:形如"0:PHOTO/PIC20120321210633.jpg"/"2:PHOTO/PIC20120321210633.jpg"的文件名
 * @param       pname           : 带路径的名字
 * @param       mode            : 0,创建.bmp文件;    1,创建.jpg文件;
 * @retval      无
 */
void camera_new_pathname(uint8_t *pname, uint8_t mode)
{
    calendar_get_time(&calendar);
    calendar_get_date(&calendar);

    if (mode == 0)
    {
        if (gui_phy.memdevflag & (1 << 0))sprintf((char *)pname, "0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);         /* 首选保存在SD卡 */
        else if (gui_phy.memdevflag & (1 << 2))sprintf((char *)pname, "2:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);    /* SD卡不存在,则保存在U盘 */
    }
    else
    {
        if (gui_phy.memdevflag & (1 << 0))sprintf((char *)pname, "0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);         /* 首选保存在SD卡 */
        else if (gui_phy.memdevflag & (1 << 2))sprintf((char *)pname, "2:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);    /* SD卡不存在,则保存在U盘 */
    }
}

/**
 * @brief       OV5640拍照jpg图片
 * @param       pname           : 带路径的名字
 * @retval      0, 成功
 *              其他, 失败
 */
uint8_t ov5640_jpg_photo(uint8_t *pname)
{
    FIL *f_jpg;
    uint8_t res = 0, headok = 0;
    uint32_t bwr;
    uint32_t i, jpgstart, jpglen;
    uint8_t *pbuf;
    f_jpg = (FIL *)mymalloc(SRAMIN, sizeof(FIL));   /* 开辟FIL字节的内存区域 */

    if (f_jpg == NULL)return 0XFF;  /* 内存申请失败 */

    ovx_mode = 1;
    jpeg_data_ok = 0;
    sw_ov5640_mode();   /* 切换为OV5640模式 */
    ov5640_jpeg_mode(); /* JPEG模式 */
    ov5640_outsize_set(4, 0, 1280, 800);        /* 设置输出尺寸(WXGA) */
    dcmi_rx_callback = jpeg_dcmi_rx_callback;   /* JPEG接收数据回调函数 */
    dcmi_dma_init((uint32_t)dcmi_line_buf[0], (uint32_t)dcmi_line_buf[1], jpeg_line_size, 2, 1); /* DCMI DMA配置 */
    dcmi_start();   /* 启动传输 */

    while (jpeg_data_ok != 1);  /* 等待第一帧图片采集完 */

    jpeg_data_ok = 2;   /* 忽略本帧图片,启动下一帧采集 */

    if (ov_flash)
    {
        while (jpeg_data_ok == 2);

        ov5640_flash_ctrl(1);/* 打开闪光灯 */
    }

    while (jpeg_data_ok != 1);  /* 等待第二帧图片采集完,第二帧,才保存到SD卡去 */

    if (ov_flash)ov5640_flash_ctrl(0); /* 关闭闪光灯 */

    dcmi_stop();    /* 停止DMA搬运 */
    ovx_mode = 0;
    sw_sdcard_mode();   /* 切换为SD卡模式 */
    res = f_open(f_jpg, (const TCHAR *)pname, FA_WRITE | FA_CREATE_NEW); /* 模式0,或者尝试打开失败,则创建新文件 */

    if (res == 0)
    {
        printf("jpeg data size:%d\r\n", jpeg_data_len * 4); /* 串口打印JPEG文件大小 */
        pbuf = (uint8_t *)jpeg_data_buf;
        jpglen = 0; /* 设置jpg文件大小为0 */
        headok = 0; /* 清除jpg头标记 */

        for (i = 0; i < jpeg_data_len * 4; i++)   /* 查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小 */
        {
            if ((pbuf[i] == 0XFF) && (pbuf[i + 1] == 0XD8))   /* 找到FF D8 */
            {
                jpgstart = i;
                headok = 1;	/* 标记找到jpg头(FF D8) */
            }

            if ((pbuf[i] == 0XFF) && (pbuf[i + 1] == 0XD9) && headok)   /* 找到头以后,再找FF D9 */
            {
                jpglen = i - jpgstart + 2;
                break;
            }
        }

        if (jpglen) /* 正常的jpeg数据 */
        {
            pbuf += jpgstart;   /* 偏移到0XFF,0XD8处 */
            res = f_write(f_jpg, pbuf, jpglen, &bwr);

            if (bwr != jpglen)res = 0XFE;

        }
        else res = 0XFD;
    }

    jpeg_data_len = 0;
    f_close(f_jpg);
    sw_ov5640_mode();       /* 切换为OV5640模式 */
    ov5640_rgb565_mode();   /* RGB565模式 */
//    if(lcdltdc.pwidth!=0)   /* RGB屏 */
//    {
//        dcmi_rx_callback=rgblcd_dcmi_rx_callback;/* RGB屏接收数据回调函数 */
//        dcmi_dma_init((uint32_t)dcmi_line_buf[0],(uint32_t)dcmi_line_buf[1],lcddev.width/2,1,1);/* DCMI DMA配置 */
//    }else   /* MCU 屏 */
    {
        dcmi_dma_init((uint32_t)&LCD->LCD_RAM, 0, 1, 1, 0); /* DCMI DMA配置,MCU屏,竖屏 */
    }
    myfree(SRAMIN, f_jpg);
    return res;
}

/**
 * @brief       检测KEY1和WKUP按键的长按
 * @param       无
 * @retval      0,没有按键按下
 *              1,KEY0按下
 *              2,KEY1短按
 *              3,WKUP短按
 *              4,KEY1长按
 *              5,WKUP长按
 */
uint8_t camera_key_scan(void)
{
#define LONG_PRES_VAL   30      /* 触发长按的条件 */
    static uint8_t k1lcnt = 0;  /* key1，长按计数器 */
    static uint8_t kulcnt = 0;  /* keyup，长按计数器 */
    static uint8_t keysta = 0;  /* 上一次的按键状态 */
    uint8_t key;
    key = key_scan(1);

    if (keysta == (key + 2) && key)return 0; /* 等待按键松开/切换 */

    switch (key)
    {
        case WKUP_PRES:
            keysta = key;
            key = 0;
            k1lcnt = 0;
            kulcnt++;

            if (kulcnt >= LONG_PRES_VAL)
            {
                kulcnt = 0;
                key = keysta = 5;
            }

            break;

        case KEY1_PRES:
            keysta = key;
            key = 0;
            kulcnt = 0;
            k1lcnt++;

            if (k1lcnt >= LONG_PRES_VAL)
            {
                k1lcnt = 0;
                key = keysta = 4;
            }

            break;

        case KEY0_PRES:
            keysta = 0;
            k1lcnt = 0;
            kulcnt = 0;
            break;

        default:
            if (keysta < 4)key = keysta;

            keysta = 0;
            k1lcnt = 0;
            kulcnt = 0;
            break;
    }

    return key;
}

/**
 * @brief       摄像头功能
 *  @note       所有照片文件,均保存在SD卡PHOTO文件夹内.
 * @param       无
 * @retval      未用到
 */
uint8_t camera_play(void)
{
    uint8_t rval = 0;
    uint8_t res, fac;
    uint8_t *caption = 0;
    uint8_t *pname;
    uint8_t selx = 0;
    uint8_t l00sel = 0, l10sel = 0, l11sel = 0; /* 默认选择项 */
    uint8_t l2345sel[3];
    uint8_t *psn;
    uint8_t key;
    uint8_t scale = 1;              /* 默认是全尺寸缩放 */
    uint8_t tcnt = 0;
    uint16_t outputheight = 0;      /* RGB 图像输出的宽度 */
//    if(audiodev.status&(1<<7))      /* 当前在放歌??必须停止 */
//    {
//        audio_stop_req(&audiodev);  /* 停止音频播放 */
//        audio_task_delete();        /* 删除音乐播放任务 */
//    }
    /* 提示开始检测OV5640 */
    window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_remind_tbl[0][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);

    if (gui_phy.memdevflag & (1 << 0))f_mkdir("0:PHOTO"); /* 强制创建文件夹,给照相机用 */

    if (gui_phy.memdevflag & (1 << 2))f_mkdir("2:PHOTO"); /* 强制创建文件夹,给照相机用 */

    if (ov5640_init())   /* 初始化OV5640 */
    {
        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_remind_tbl[1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);
        delay_ms(500);
        rval = 1;
    }

    dcmi_line_buf[0] = mymalloc(SRAM12, jpeg_line_size * 4);    /* 为jpeg dma接收申请内存 */
    dcmi_line_buf[1] = mymalloc(SRAM12, jpeg_line_size * 4);    /* 为jpeg dma接收申请内存 */
    jpeg_data_buf = gui_memex_malloc(jpeg_buf_size);            /* 为jpeg文件申请内存(最大4MB) */
    pname = gui_memin_malloc(40);   /* 申请40个字节内存,用于存放文件名 */
    psn = gui_memin_malloc(50);     /* 申请50个字节内存,用于存放类似：“保存为:0:PHOTO/PIC20120321210633.bmp"”的提示语 */

    if (!dcmi_line_buf[1] || !jpeg_data_buf || !pname || !psn)rval = 1; /* 申请失败 */

    if (rval == 0)   /* OV5640正常 */
    {
        l2345sel[0] = 3;        /* 亮度默认为3,实际值0 */
        l2345sel[1] = 3;        /* 色度默认为3,实际值0 */
        l2345sel[2] = 3;        /* 对比度默认为3,实际值0 */
        ov5640_rgb565_mode();   /* RGB565模式 */
        ov5640_focus_init();
        ov5640_focus_constant();/* 启动持续对焦 */
        ov5640_light_mode(0);   /* 自动模式 */
        ov5640_brightness(l2345sel[0] + 1);     /* 亮度设置 */
        ov5640_color_saturation(l2345sel[1]);   /* 色度设置 */
        ov5640_contrast(l2345sel[2]);           /* 对比度设置 */
        ov5640_exposure(3);     /* 曝光等级 */
        ov5640_sharpness(33);   /* 自动锐度 */
        dcmi_init();            /* DCMI配置 */
//        if(lcdltdc.pwidth!=0)   /* RGB屏 */
//        {
//            dcmi_rx_callback=rgblcd_dcmi_rx_callback;   /* RGB屏接收数据回调函数 */
//            dcmi_dma_init((uint32_t)dcmi_line_buf[0],(uint32_t)dcmi_line_buf[1],lcddev.width/2,1,1);    /* DCMI DMA配置 */
//        }else                   /* MCU 屏 */
        {
            dcmi_dma_init((uint32_t)&LCD->LCD_RAM, 0, 1, 1, 0); /* DCMI DMA配置,MCU屏,竖屏 */
        }

        if (lcddev.height >= 1280)
        {
            yoffset = (lcddev.height - 600) / 2;
            outputheight = 600;
            ov5640_write_reg(0x3035, 0X51);    /* 降低输出帧率，否则可能抖动 */
        }
        else if (lcddev.height > 800)
        {
            yoffset = (lcddev.height - 800) / 2;
            outputheight = 800;
            ov5640_write_reg(0x3035, 0X51);    /* 降低输出帧率，否则可能抖动 */
        }
        else
        {
            yoffset = 0;
            outputheight = lcddev.height;
        }

        curline = yoffset;          /* 行数复位 */
        ov5640_outsize_set(16, 4, lcddev.width, outputheight); /* 设置摄像头输出尺寸为LCD的尺寸大小 */
        dcmi_start();               /* 启动传输 */
        lcd_clear(BLACK);
        memshow_flag = 0;           /* 禁止打印内存信息 */
        system_task_return = 0;     /* 清除TPAD */

        while (1)
        {
            tp_dev.scan(0);

            if (tp_dev.sta & TP_PRES_DOWN)
            {
                ov5640_focus_single();  /* 执行自动对焦 */
                tp_dev.scan(0);
                tp_dev.sta = 0;
            }

            key = camera_key_scan();    /* 按键扫描 */

            if (key)
            {
                if (key == WKUP_PRES)   /* 如果是BMP拍照,则等待300ms,去抖动,以获得稳定的bmp照片 */
                {
                    delay_ms(300);
                    hsync_int = 0;

                    while (hsync_int == 0); /* 等待帧中断 */

                    hsync_int = 0;

                    if (ov_flash)ov5640_flash_ctrl(1);	/* 打开闪光灯 */

                    bmp_request = 1;        /* 请求关闭DCMI */

                    while (hsync_int == 0);

                    if (ov_flash)ov5640_flash_ctrl(0);  /* 关闭闪光灯 */

                    while (bmp_request);    /* 等带请求处理完成 */
                }
                else
                {
                    dcmi_stop();
                }
                
                while (key_scan(1)); /* 等待按键松开 */

                tcnt = 0;

                switch (key)
                {
                    case KEY1_PRES: /* KEY1按下,JPEG拍照 */
                    case WKUP_PRES: /* WKUP按下,BMP拍照 */
                        LED1(0);    /* LED1亮,提示拍照中 */
                        sw_sdcard_mode();       /* 切换为SD卡模式 */

                        if (key == KEY1_PRES)   /* JPEG拍照 */
                        {
                            camera_new_pathname(pname, 1); /* 得到jpg文件名 */
                            res = ov5640_jpg_photo(pname);

                            if (lcddev.height > 800)ov5640_write_reg(0x3035, 0X51); /* 大于800的屏,降低输出帧率，否则可能抖动 */
                        }
                        else    /* BMP拍照 */
                        {
                            camera_new_pathname(pname, 0); /* 得到bmp文件名 */
                            res = bmp_encode(pname, 0, yoffset, lcddev.width, outputheight, 0); /* bmp拍照 */
                        }

                        if (res)    /* 拍照失败了 */
                        {
                            if (res == 0XFF)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)camera_failmsg_tbl[1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* 内存错误 */
                            else if (res == 0XFD)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)camera_failmsg_tbl[2][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* 数据错误 */
                            else window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)camera_failmsg_tbl[0][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* 提示SD卡是否存在 */
                        }
                        else
                        {
                            strcpy((char *)psn, (const char *)camera_remind_tbl[2][gui_phy.language]);
                            strcat((char *)psn, (const char *)pname);
                            window_msg_box((lcddev.width - 180) / 2, (lcddev.height - 80) / 2, 180, 80, psn, (uint8_t *)camera_saveok_caption[gui_phy.language], 12, 0, 0, 0);
                            BEEP(1);        /* 蜂鸣器短叫，提示拍照完成 */
                            delay_ms(100);
                        }

                        LED1(1);            /* LED1灭,提示保存完成 */
                        BEEP(0);            /* 关闭蜂鸣器 */
                        delay_ms(2000);
                        sw_ov5640_mode();   /* 切换为OV5640模式 */

                        if (scale == 0)
                        {
                            fac = 800 / outputheight; /* 得到比例因子 */
                            ov5640_outsize_set((1280 - fac * lcddev.width) / 2, (800 - fac * outputheight) / 2, lcddev.width, outputheight);
                        }
                        else
                        {
                            ov5640_outsize_set(16, 4, lcddev.width, outputheight);
                        }

                        system_task_return = 0; /* 清除TPAD */
                        break;

                    case 4: /* KEY1长按,缩放/1:1显示(不缩放) */
                        scale = !scale;

                        if (scale == 0)
                        {
                            if (lcddev.height == 1280)
                            {
                                fac = 600 / outputheight;   /* 得到比例因子 */
                                ov5640_outsize_set((1280 - fac * lcddev.width) / 2, (600 - fac * outputheight) / 2, lcddev.width, outputheight);
                            }
                            else
                            {
                                fac = 800 / outputheight;   /* 得到比例因子 */
                                ov5640_outsize_set((1280 - fac * lcddev.width) / 2, (800 - fac * outputheight) / 2, lcddev.width, outputheight);
                            }
                        }
                        else
                        {
                            ov5640_outsize_set(16, 4, lcddev.width, outputheight);
                        }

                        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_scalemsg_tbl[scale][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);

                        while (key_scan(1) == 0 && tcnt < 80)   /* 等待800ms,如果没有按键按下的话 */
                        {
                            delay_ms(10);
                            tcnt++;
                        }

                        break;

                    case 5:                 /* WKUP长按，设置摄像头参数 */
                        sw_sdcard_mode();   /* 切换为SD卡模式 */
                        caption = (uint8_t *)camera_l00fun_caption[gui_phy.language];
                        res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 6) / 2, 160, 72 + 32 * 6, (uint8_t **)camera_l00fun_table[gui_phy.language], 6, (uint8_t *)&l00sel, 0X90, caption); /* 单选 */
                        sw_ov5640_mode();   /* 切换为OV5640模式 */

                        if (res == 0)
                        {
                            dcmi_start();
                            delay_ms(200);
                            dcmi_stop();
                            sw_sdcard_mode();   /* 切换为SD卡模式 */
                            caption = (uint8_t *)camera_l00fun_table[gui_phy.language][l00sel];

                            switch (l00sel)
                            {
                                case 0:/* 场景设置 */
                                    res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 5) / 2, 160, 72 + 32 * 5, (uint8_t **)camera_l10fun_table[gui_phy.language], 5, (uint8_t *)&l10sel, 0X90, caption); /* 单选 */
                                    sw_ov5640_mode();/* 切换为OV5640模式 */

                                    if (res == 0)
                                    {
                                        ov5640_light_mode(l10sel);
                                    }

                                    break;

                                case 1:/* 特效设置 */
                                    res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 7) / 2, 160, 72 + 32 * 7, (uint8_t **)camera_l11fun_table[gui_phy.language], 7, (uint8_t *)&l11sel, 0X90, caption); /* 单选 */
                                    sw_ov5640_mode();/* 切换为OV5640模式 */

                                    if (res == 0)
                                    {
                                        ov5640_special_effects(l11sel);
                                    }

                                    break;

                                case 2:/* 亮度设置 */
                                case 3:/* 色度设置 */
                                case 4:/* 对比度设置 */
                                    selx = l2345sel[l00sel - 2]; /* 得到之前的选择 */
                                    res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 7) / 2, 160, 72 + 32 * 7, (uint8_t **)camera_l124fun_table[gui_phy.language], 7, (uint8_t *)&selx, 0X90, caption); /* 单选 */
                                    sw_ov5640_mode();/* 切换为OV5640模式 */

                                    if (res == 0)
                                    {
                                        l2345sel[l00sel - 2] = selx; /* 记录新值 */

                                        if (l00sel == 2)ov5640_brightness(selx + 1);    /* 亮度设置 */

                                        if (l00sel == 3)ov5640_color_saturation(selx);  /* 色度设置 */

                                        if (l00sel == 4)ov5640_contrast(selx);          /* 对比度设置 */
                                    }

                                    break;

                                case 5:/* 闪光灯设置 */
                                    selx = ov_flash;
                                    res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 2) / 2, 160, 72 + 32 * 2, (uint8_t **)camera_l15fun_table[gui_phy.language], 2, (uint8_t *)&selx, 0X90, caption); /* 单选 */

                                    if (res == 0)ov_flash = selx;

                                    break;
                            }
                        }

                        break;

                }

                sw_ov5640_mode();   /* 切换为OV5640模式 */
                dcmi_start();       /* 这里先使能dcmi,然后立即关闭DCMI,后面再开启DCMI,可以防止RGB屏的侧移问题 */
                dcmi_stop();
                dcmi_start();
            }

            if (system_task_return)
            {
                break;      /* TPAD返回,再次确认,排除干扰 */
            }

            if (hsync_int)  /* 刚刚产生帧中断,可以延时 */
            {
                delay_ms(10);
                hsync_int = 0;
            }
        }
    }

    dcmi_stop();        /* 停止摄像头工作 */
    sw_sdcard_mode();   /* 切换为SD卡模式 */
    myfree(SRAM12, dcmi_line_buf[0]);
    myfree(SRAM12, dcmi_line_buf[1]);
    gui_memex_free(jpeg_data_buf);
    gui_memin_free(pname);
    gui_memin_free(psn);
    memshow_flag = 1;   /* 允许打印内存信息 */
    return 0;
}







