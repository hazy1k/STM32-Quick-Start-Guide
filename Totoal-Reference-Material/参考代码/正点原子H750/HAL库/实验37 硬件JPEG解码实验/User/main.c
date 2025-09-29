/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       硬件JPEG解码 实验
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./PICTURE/piclib.h"
#include "string.h"
#include "math.h"


/**
 * @brief       得到path路径下,目标文件的总个数
 * @param       path : 路径
 * @retval      总有效文件数
 */
uint16_t pic_get_tnum(uint8_t *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;           /* 临时目录 */
    FILINFO *tfileinfo; /* 临时文件信息 */
    tfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));   /* 申请内存 */
    res = f_opendir(&tdir, (const TCHAR *)path);    /* 打开目录 */

    if (res == FR_OK && tfileinfo)
    {
        while (1)       /* 查询总的有效文件数 */
        {
            res = f_readdir(&tdir, tfileinfo);  /* 读取目录下的一个文件 */

            if (res != FR_OK || tfileinfo->fname[0] == 0)break; /* 错误了/到末尾了,退出 */

            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0XF0) == 0X50)   /* 取高四位,看看是不是图片文件 */
            {
                rval++; /* 有效文件数增加1 */
            }
        }
    }

    myfree(SRAMIN, tfileinfo);  /* 释放内存 */
    return rval;
}

int main(void)
{
    uint8_t res;
    DIR picdir;             /* 图片目录 */
    FILINFO *picfileinfo;   /* 文件信息 */
    char *pname;            /* 带路径的文件名 */
    uint16_t totpicnum;     /* 图片文件总数 */
    uint16_t curindex;      /* 图片当前索引 */
    uint8_t key;            /* 键值 */
    uint8_t pause = 0;      /* 暂停标记 */
    uint8_t t;
    uint16_t temp;
    uint32_t *picoffsettbl; /* 图片文件offset索引表 */

    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    usmart_dev.init(240);               /* 初始化USMART */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池(ITCM) */
    exfuns_init();                      /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);            /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);            /* 挂载FLASH */

    while (fonts_init())         /* 检查字库 */
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!", RED);
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);   /* 清除显示 */
        delay_ms(200);
    }

    text_show_string(30, 50, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "硬件JPEG解码 实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "KEY0:NEXT KEY1:PREV", 16, 0, RED);
    text_show_string(30, 110, 200, 16, "KEY_UP:PAUSE", 16, 0, RED);
    text_show_string(30, 130, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);

    while (f_opendir(&picdir, "0:/PICTURE"))    /* 打开图片文件夹 */
    {
        text_show_string(30, 170, 240, 16, "PICTURE文件夹错误!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 170, 240, 186, WHITE);     /* 清除显示 */
        delay_ms(200);
    }

    totpicnum = pic_get_tnum((uint8_t*)"0:/PICTURE");   /* 得到总有效文件数 */

    while (totpicnum == NULL)   /* 图片文件为0 */
    {
        text_show_string(30, 170, 240, 16, "没有图片文件!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 170, 240, 186, WHITE); /* 清除显示 */
        delay_ms(200);
    }

    picfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO)); /* 申请内存 */
    pname = mymalloc(SRAMIN, FF_MAX_LFN * 2 + 1);   /* 为带路径的文件名分配内存 */
    picoffsettbl = mymalloc(SRAMIN, 4 * totpicnum); /* 申请4*totpicnum个字节的内存,用于存放图片索引 */

    while (!picfileinfo || !pname || !picoffsettbl) /* 内存分配出错 */
    {
        text_show_string(30, 170, 240, 16, "内存分配失败!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 170, 240, 186, WHITE);         /* 清除显示 */
        delay_ms(200);
    }

    /* 记录索引 */
    res = f_opendir(&picdir, "0:/PICTURE");         /* 打开目录 */

    if (res == FR_OK)
    {
        curindex = 0;   /* 当前索引为0 */

        while (1)       /* 全部查询一遍 */
        {
            temp = picdir.dptr; /* 记录当前dptr偏移 */
            res = f_readdir(&picdir, picfileinfo);  /* 读取目录下的一个文件 */

            if (res != FR_OK || picfileinfo->fname[0] == 0)break;   /* 错误了/到末尾了,退出 */

            res = exfuns_file_type(picfileinfo->fname);

            if ((res & 0XF0) == 0X50)   /* 取高四位,看看是不是图片文件 */
            {
                picoffsettbl[curindex] = temp;  /* 记录索引 */
                curindex++;
            }
        }
    }

    text_show_string(30, 170, 240, 16, "开始显示...", 16, 0, RED);
    delay_ms(1500);
    piclib_init();  /* 初始化画图 */
    curindex = 0;   /* 从0开始显示 */
    res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE");  /* 打开目录 */

    while (res == FR_OK)   /* 打开成功 */
    {
        dir_sdi(&picdir, picoffsettbl[curindex]);   /* 改变当前目录索引 */
        res = f_readdir(&picdir, picfileinfo);      /* 读取目录下的一个文件 */

        if (res != FR_OK || picfileinfo->fname[0] == 0)break;   /* 错误了/到末尾了,退出 */

        strcpy((char *)pname, "0:/PICTURE/");       /* 复制路径(目录) */
        strcat((char *)pname, (const char *)picfileinfo->fname);        /* 将文件名接在后面 */
        lcd_clear(BLACK);
        piclib_ai_load_picfile(pname, 0, 0, lcddev.width, lcddev.height, 1);    /* 显示图片 */
        text_show_string(2, 2, lcddev.width, 16, (char*)pname, 16, 1, RED);     /* 显示图片名字 */
        t = 0;

        while (1)
        {
            key = key_scan(0);      /* 扫描按键 */

            if (t > 250)key = 1;    /* 模拟一次按下KEY0 */

            if ((t % 20) == 0)
            {
                LED0_TOGGLE();      /* LED0闪烁,提示程序正在运行. */
            }
            
            if (key == KEY1_PRES)   /* 上一张 */
            {
                if (curindex)
                {
                    curindex--;
                }
                else
                {
                    curindex = totpicnum - 1;
                }
                
                break;
            }
            else if (key == KEY0_PRES)  /* 下一张 */
            {
                curindex++;

                if (curindex >= totpicnum)curindex = 0; /* 到末尾的时候,自动从头开始 */

                break;
            }
            else if (key == WKUP_PRES)
            {
                pause = !pause;
                LED1(!pause);   /* 暂停的时候LED1亮. */
            }

            if (pause == 0)t++;

            delay_ms(10);
        }

        res = 0;
    }

    myfree(SRAMIN, picfileinfo);    /* 释放内存 */
    myfree(SRAMIN, pname);          /* 释放内存 */
    myfree(SRAMIN, picoffsettbl);   /* 释放内存 */
}









