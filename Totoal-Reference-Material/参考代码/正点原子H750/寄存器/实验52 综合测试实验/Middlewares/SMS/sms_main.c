/**
 ****************************************************************************************************
 * @file        sms_main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       SMS主函数 代码
 *              本程序移植自网友ye781205的NES模拟器工程, 特此感谢!
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
 * V1.1 20220531
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"

#include "audioplay.h"
#include "sms_vdp.h"
#include "sms_main.h"
#include "sms_sn76496.h"
#include "sms_z80a.h"

#include "nes_main.h"
#include "usbh_hid_gamepad.h"



uint16_t *smssaibuf1;       /* 音频缓冲帧,占用内存数 367*2 字节@22050Hz */
uint16_t *smssaibuf2;       /* 音频缓冲帧,占用内存数 367*2 字节@22050Hz */

uint8_t *sms_rom;           /* SMS ROM存储地址指针 */
uint8_t *SMS_romfile;       /* ROM指针=sms_rom/sms_rom+512 */
uint8_t  *S_RAM;            /* internal SMS RAM	16k  [0x4000] */
uint8_t  *E_RAM;            /* external cartridge RAM (2 X 16K Bank Max) [0x8000] */



/**
 * @brief       释放SMS申请的所有内存
 * @param       无
 * @retval      无
 */
void sms_sram_free(void)
{
    myfree(SRAMIN, sms_rom);
    myfree(SRAM12, E_RAM);
    myfree(SRAM12, cache);

    myfree(SRAMDTCM, S_RAM);
    myfree(SRAMDTCM, SMS_VDP);
    myfree(SRAMDTCM, psg_buffer);
    myfree(SRAMDTCM, SN76496);
    myfree(SRAMDTCM, Z80A);
    myfree(SRAMDTCM, VRam);

    myfree(SRAM12, smssaibuf1);
    myfree(SRAM12, smssaibuf2);
    myfree(SRAM12, lut);
}

/**
 * @brief       为SMS运行申请内存
 * @param       romsize         : sms文件大小
 * @retval      0, 成功;  其他, 失败;
 */
uint8_t sms_sram_malloc(uint32_t romsize)
{
    E_RAM = mymalloc(SRAM12, 0X8000);       /* 申请2*16K字节 */
    cache = mymalloc(SRAM12, 0x20000);      /* 128K */
    sms_rom = mymalloc(SRAMIN, romsize);    /* 开辟romsize字节的内存区域 */
    S_RAM = mymalloc(SRAMDTCM, 0X4000);     /* 申请16K字节 */
    SMS_VDP = mymalloc(SRAMDTCM, sizeof(SVDP));
    psg_buffer = mymalloc(SRAMDTCM, SNBUF_size * 2);    /* 申请内存SNBUF_size */
    SN76496 = mymalloc(SRAMDTCM, sizeof(t_SN76496));    /* 申请内存184 */
    Z80A = mymalloc(SRAMDTCM, sizeof(CPU80));           /* 申请内存 */
    VRam = mymalloc(SRAMDTCM, 0x4000);      /* 申请16K字节 */

    smssaibuf1 = mymalloc(SRAM12, SNBUF_size * 4 + 10);
    smssaibuf2 = mymalloc(SRAM12, SNBUF_size * 4 + 10);
    lut = mymalloc(SRAM12, 0x10000);        /* 64K */

    if (sms_rom && cache && VRam && lut)
    {
        memset(E_RAM, 0, 0X8000);           /* 清零 */
        memset(cache, 0, 0x20000);          /* 清零 */
        memset(S_RAM, 0, 0X4000);           /* 清零 */
        memset(SMS_VDP, 0, sizeof(SVDP));   /* 清零 */

        memset(psg_buffer, 0, SNBUF_size * 2);  /* 清零 */
        memset(SN76496, 0, sizeof(t_SN76496));  /* 清零 */
        memset(Z80A, 0, sizeof(CPU80));         /* 清零 */
        memset(VRam, 0, 0X4000);                /* 清零 */

        memset(smssaibuf1, 0, SNBUF_size * 4 + 10); /* 清零 */
        memset(smssaibuf2, 0, SNBUF_size * 4 + 10); /* 清零 */
        memset(lut, 0, 0x10000);                    /* 清零 */
        return 0;
    }
    else
    {
        sms_sram_free();    /* 释放所有内存 */
        return 1;
    }
}

uint16_t sms_xoff = 0;      /* 显示在x轴方向的偏移量(实际显示宽度=256-2*sms_xoff) */
uint16_t sms_yoff = 0;      /* 显示在y轴方向的偏移量 */


/**
 * @brief       设置游戏显示窗口
 * @param       无
 * @retval      无
 */
void sms_set_window(void)
{
    uint16_t xoff = 0, yoff = 0;
    uint16_t lcdwidth, lcdheight;

    if (lcddev.width == 240)
    {
        lcdwidth = 240;
        lcdheight = 192;
        sms_xoff = (256 - lcddev.width) / 2;    /* 得到x轴方向的偏移量 */
    }
    else if (lcddev.width <= 320)
    {
        lcdwidth = 240;
        lcdheight = 192;
        sms_xoff = 8; /* sms需要偏移8像素 */
    }
    else if (lcddev.width >= 480)
    {
        lcdwidth = 480;
        lcdheight = 192 * 2;
        sms_xoff = (256 - (lcdwidth / 2)) / 2; /* 得到x轴方向的偏移量 */
    }

    xoff = (lcddev.width - lcdwidth) / 2;
    yoff = (lcddev.height - lcdheight - gui_phy.tbheight) / 2 + gui_phy.tbheight; /* 屏幕高度 */
    sms_yoff = yoff;
    lcd_set_window(xoff, yoff, lcdwidth, lcdheight); /* 让sms始终在屏幕的正中央显示 */
    lcd_set_cursor(xoff, yoff);
    lcd_write_ram_prepare();    /* 写入LCD RAM的准备 */
}

extern volatile uint8_t framecnt;
extern uint8_t nes_run_flag;

/**
 * @brief       模拟器启动，各种初始化,然后循环运行模拟器
 * @param       bank_mun        : 输入参数
 * @retval      无
 */
void sms_start(uint8_t bank_mun)
{
    uint8_t zhen;
    uint8_t res = 0;
    res = VDP_init();
    res += Z80A_Init(S_RAM, E_RAM, SMS_romfile, bank_mun); /* 0x8080000,0x0f,"Sonic the Hedgehog '91" */
    res += sms_audio_init();

    if (res == 0)
    {
        nes_run_flag = 1;
        tim8_int_init(10000 - 1, 24000 - 1); /* 启动TIM8  1s中断一次 */
        sms_set_window();           /* 设置窗口 */

        while (1)
        {
            SMS_frame(zhen);        /* +FB_OFS  (24+240*32) */
            nes_get_gamepadval();   /* 借用sms的手柄数据获取函数 */
            sms_update_Sound();
            sms_update_pad();       /* 获取手柄值 */
            zhen++;
            framecnt++;

            if (zhen > 2)zhen = 0;  /* 跳2帧 */

            if (system_task_return)break;   /* TPAD返回 */

            if (lcddev.id == 0X1963)        /* 对于1963,每更新一帧,都要重设窗口 */
            {
                nes_set_window();
            }
        }

        TIM8->CR1 &= ~(1 << 0); /* 关闭定时器8 */
        lcd_set_window(0, 0, lcddev.width, lcddev.height); /* 恢复屏幕窗口 */
    }

    nes_run_flag = 0;
    sms_sound_close();          /* 关闭音频输出 */
}

/**
 * @brief       更新手柄数据
 *  @note       SMS键值 1111 1111 全1表示没按键
 *                     D7  D6  D5   D4   D3  D2  D1  D0
 *              SMS    B   A   右   左   下  上
 *               FC    右  左  下   上   ST   S   B   A
 * @param       无
 * @retval      无
 */
void sms_update_pad(void)
{
    uint8_t key, key1;
    key = 255 - fcpad.ctrlval;  /* 将FC手柄的值取反 */
    key1 = (key >> 4) | 0xf0;   /* 转换为SMS手柄的值 */
    key1 &= ((key << 4) | 0xcf);
    SetController(key1);
}

/**
 * @brief       加载SMS游戏
 * @param       pname           : sms游戏路径
 * @retval      0, 正常退出
 *              1, 内存错误
 *              2, 文件错误
 */
uint8_t sms_load(uint8_t *pname)
{
    uint8_t bank_mun;   /* 16K bank的数量 */
    uint8_t res = 0;
    FIL *f_sms;
    uint8_t *buf;       /* 缓存 */
    uint8_t *p;
    uint32_t readlen;   /* 总读取长度 */
    uint16_t bread;     /* 读取的长度 */

//    if (audiodev.status & (1 << 7))   /* 当前在放歌?? */
//    {
//        audio_stop_req(&audiodev);  /* 停止音频播放 */
//        audio_task_delete();        /* 删除音乐播放任务. */
//    }

    buf = mymalloc(SRAM12, 1024);
    f_sms = (FIL *)mymalloc(SRAM12, sizeof(FIL));   /* 开辟FIL字节的内存区域 */

    if (f_sms == NULL)
    {
        myfree(SRAM12, buf);    /* 释放内存 */
        return 1;   /* 申请失败 */
    }

    res = f_open(f_sms, (const TCHAR *)pname, FA_READ); /* 打开文件 */

    if (res == 0)res = sms_sram_malloc(f_sms->obj.objsize); /* 申请内存 */

    if (res == 0)
    {
        if ((f_sms->obj.objsize / 512) & 1)   /* 照顾图像标题,如果存在 */
        {
            SMS_romfile = sms_rom + 512;
            bank_mun = ((f_sms->obj.objsize - 512) / 0x4000) - 1;   /* 16K bank的数量 */
        }
        else
        {
            SMS_romfile = sms_rom;
            bank_mun = (f_sms->obj.objsize / 0x4000) - 1;   /* 16K bank的数量 */
        }

        p = sms_rom;
        readlen = 0;

        while (readlen < f_sms->obj.objsize)    /* 循环读取整个SMS游戏文件 */
        {
            res = f_read(f_sms, buf, 1024, (UINT *)&bread); /* 读出文件内容 */
            readlen += bread;
            my_mem_copy(p, buf, bread);
            p += bread;

            if (res)break;
        }

        //res=f_read(f_sms,sms_rom,f_sms->obj.objsize,&br);
        if (res)res = 2;    /* 文件错误 */

        f_close(f_sms);     /* 关闭文件 */
    }

    myfree(SRAM12, buf);    /* 释放内存 */
    myfree(SRAM12, f_sms);  /* 释放内存 */

    if (res == 0)
    {
        sms_start(bank_mun);    /* 开始游戏 */
    }

    sms_sram_free();
    return res;
}


volatile uint8_t smstransferend = 0;    /* sai传输完成标志 */
volatile uint8_t smswitchbuf = 0;       /* saibufx指示标志 */

/**
 * @brief       SAI音频播放回调函数
 * @param       无
 * @retval      无
 */
void sms_sai_dma_tx_callback(void)
{
    uint16_t i;

    if (DMA2_Stream3->CR & (1 << 19))smswitchbuf = 0;
    else smswitchbuf = 1;

    if (smstransferend)
    {
        if (smswitchbuf)for (i = 0; i < SNBUF_size * 2; i++)smssaibuf2[i] = 0;
        else for (i = 0; i < SNBUF_size * 2; i++)smssaibuf1[i] = 0;
    }

    smstransferend = 1;
}

/**
 * @brief       SMS打开音频输出
 * @param       sample_rate     : 音频采样率
 * @retval      无
 */
void sms_sound_open(int sample_rate)
{
//    printf("sound open:%d\r\n", sample_rate);
//    app_es8388_volset(es8388set.mvol);
//    ES8388_ADDA_Cfg(1, 0);  /* 开启DAC */
//    ES8388_Input_Cfg(0);    /* 关闭输入通道 */
//    ES8388_Output_Cfg(1, 0);    /* 开启DAC输出 */
//    ES8388_I2S_Cfg(2, 0);   /* 飞利浦标准,16位数据长度 */

//    SAIA_Init(0, 1, 4); /* 设置SAI,主发送,16位数据 */
//    SAIA_SampleRate_Set(sample_rate);   /* 设置采样率 */
//    SAIA_TX_DMA_Init((uint8_t *)smssaibuf1, (uint8_t *)smssaibuf2, 2 * SNBUF_size, 1); /* DMA配置 */
//    sai_tx_callback = sms_sai_dma_tx_callback; /* 回调函数指sms_sai_dma_tx_callback */
//    SAI_Play_Start();   /* 开启DMA */
}

/**
 * @brief       SMS关闭音频输出
 * @param       无
 * @retval      无
 */
void sms_sound_close(void)
{
//    SAI_Play_Stop();
//    app_es8388_volset(0);   /* 关闭ES8388音量输出 */
}

/**
 * @brief       SMS音频输出到SAI缓存
 * @param       无
 * @retval      无
 */
void sms_apu_fill_buffer(int samples, uint16_t *wavebuf)
{
    uint8_t i = 5;

    while (i)
    {
        nes_get_gamepadval();   /* 每3帧查询一次USB */
        delay_ms(2);
        i--;
    }

//    int i;

//    while (!smstransferend) {};

//    smstransferend = 0;

//    if (smswitchbuf == 0)
//    {
//        for (i = 0; i < SNBUF_size; i++)
//        {
//            smssaibuf1[2 * i] = wavebuf[i];
//            smssaibuf1[2 * i + 1] = wavebuf[i];
//        }
//    }
//    else
//    {
//        for (i = 0; i < SNBUF_size; i++)
//        {
//            smssaibuf2[2 * i] = wavebuf[i];
//            smssaibuf2[2 * i + 1] = wavebuf[i];
//        }
//    }
}







