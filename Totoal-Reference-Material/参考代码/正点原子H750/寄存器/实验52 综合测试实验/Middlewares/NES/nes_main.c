/**
 ****************************************************************************************************
 * @file        nes_main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       NES主函数 代码
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

#include "nes_main.h"
#include "nes_ppu.h"
#include "nes_mapper.h"
#include "nes_apu.h"

#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "audioplay.h"
#include "string.h"
#include "usb_app.h"


extern volatile uint8_t framecnt;   /* nes帧计数器 */
int MapperNo;           /* map编号 */
int NES_scanline;       /* nes扫描线 */
int VROM_1K_SIZE;
int VROM_8K_SIZE;
uint32_t NESrom_crc32;

uint8_t PADdata0;       /* 手柄1键值 [7:0]右7 左6 下5 上4 Start3 Select2 B1 A0 */
uint8_t PADdata1;       /* 手柄2键值 [7:0]右7 左6 下5 上4 Start3 Select2 B1 A0 */
uint8_t *NES_RAM;       /* 保持1024字节对齐 */
uint8_t *NES_SRAM;
NES_header *RomHeader;  /* rom文件头 */
MAPPER *NES_Mapper;
MapperCommRes *MAPx;


uint8_t *spr_ram;       /* 精灵RAM,256字节 */
ppu_data *ppu;          /* ppu指针 */
uint8_t *VROM_banks;
uint8_t *VROM_tiles;

apu_t *apu;             /* apu指针 */
uint16_t *wave_buffers;
uint16_t *saibuf1;      /* 音频缓冲帧,占用内存数 367*4 字节@22050Hz */
uint16_t *saibuf2;      /* 音频缓冲帧,占用内存数 367*4 字节@22050Hz */

uint8_t *romfile;       /* nes文件指针,指向整个nes文件的起始地址 */


/**
 * @brief       加载ROM
 * @param       无
 * @retval      0, 成功;
 *              1, 内存错误
 *              3, map错误
 */
uint8_t nes_load_rom(void)
{
    uint8_t *p;
    uint8_t i;
    uint8_t res = 0;
    p = (uint8_t *)romfile;

    if (strncmp((char *)p, "NES", 3) == 0)
    {
        RomHeader->ctrl_z = p[3];
        RomHeader->num_16k_rom_banks = p[4];
        RomHeader->num_8k_vrom_banks = p[5];
        RomHeader->flags_1 = p[6];
        RomHeader->flags_2 = p[7];

        if (RomHeader->flags_1 & 0x04)p += 512; /* 有512字节的trainer */

        if (RomHeader->num_8k_vrom_banks > 0)   /* 存在VROM,进行预解码 */
        {
            VROM_banks = p + 16 + (RomHeader->num_16k_rom_banks * 0x4000);
#if NES_RAM_SPEED == 1                          /* 1:内存占用小 0:速度快 */
            VROM_tiles = VROM_banks;
#else
            VROM_tiles = mymalloc(SRAMIN, RomHeader->num_8k_vrom_banks * 8 * 1024);     /* 这里可能申请多达1MB内存!!! */

            if (VROM_tiles == 0)VROM_tiles = VROM_banks;                                /* 内存不够用的情况下,尝试VROM_titles与VROM_banks共用内存 */

            compile(RomHeader->num_8k_vrom_banks * 8 * 1024 / 16, VROM_banks, VROM_tiles);
#endif
        }
        else
        {
            VROM_banks = mymalloc(SRAMIN, 8 * 1024);
            VROM_tiles = mymalloc(SRAMIN, 8 * 1024);

            if (!VROM_banks || !VROM_tiles)res = 1;
        }

        VROM_1K_SIZE = RomHeader->num_8k_vrom_banks * 8;
        VROM_8K_SIZE = RomHeader->num_8k_vrom_banks;
        MapperNo = (RomHeader->flags_1 >> 4) | (RomHeader->flags_2 & 0xf0);

        if (RomHeader->flags_2 & 0x0E)MapperNo = RomHeader->flags_1 >> 4; /* 忽略高四位，如果头看起来很糟糕 */

        printf("use map:%d\r\n", MapperNo);

        for (i = 0; i < 255; i++)   /*  查找支持的Mapper号 */
        {
            if (MapTab[i] == MapperNo)break;

            if (MapTab[i] == -1)res = 3;
        }

        if (res == 0)
        {
            switch (MapperNo)
            {
                case 1:
                    MAP1 = mymalloc(SRAM12, sizeof(Mapper1Res));

                    if (!MAP1)res = 1;

                    break;

                case 4:
                case 6:
                case 16:
                case 17:
                case 18:
                case 19:
                case 21:
                case 23:
                case 24:
                case 25:
                case 64:
                case 65:
                case 67:
                case 69:
                case 85:
                case 189:
                    MAPx = mymalloc(SRAM12, sizeof(MapperCommRes));

                    if (!MAPx)res = 1;

                    break;

                default:
                    break;
            }
        }
    }

    return res; /* 返回执行结果 */
}

/**
 * @brief       释放内存
 * @param       无
 * @retval      无
 */
void nes_sram_free(void)
{
    myfree(SRAM12, NES_RAM);
    myfree(SRAM12, NES_SRAM);
    myfree(SRAM12, RomHeader);
    myfree(SRAM12, NES_Mapper);
    myfree(SRAM12, spr_ram);
    myfree(SRAM12, ppu);
    myfree(SRAM12, apu);
    myfree(SRAM12, wave_buffers);
    myfree(SRAM12, saibuf1);
    myfree(SRAM12, saibuf2);
    myfree(SRAMIN, romfile);

    if ((VROM_tiles != VROM_banks) && VROM_banks && VROM_tiles)   /* 如果分别为VROM_banks和VROM_tiles申请了内存,则释放 */
    {
        myfree(SRAMIN, VROM_banks);
        myfree(SRAMIN, VROM_tiles);
    }

    switch (MapperNo)   /* 释放map内存 */
    {
        case 1:         /* 释放内存 */
            myfree(SRAM12, MAP1);
            break;

        case 4:
        case 6:
        case 16:
        case 17:
        case 18:
        case 19:
        case 21:
        case 23:
        case 24:
        case 25:
        case 64:
        case 65:
        case 67:
        case 69:
        case 85:
        case 189:
            myfree(SRAM12, MAPx);
            break;      /* 释放内存 */

        default:
            break;
    }

    NES_RAM = 0;
    NES_SRAM = 0;
    RomHeader = 0;
    NES_Mapper = 0;
    spr_ram = 0;
    ppu = 0;
    apu = 0;
    wave_buffers = 0;
    saibuf1 = 0;
    saibuf2 = 0;
    romfile = 0;
    VROM_banks = 0;
    VROM_tiles = 0;
    MAP1 = 0;
    MAPx = 0;
}

/**
 * @brief       为NES运行申请内存
 * @param       mbuf            : nes文件大小
 * @retval      0, 成功;  其他, 失败;
 */
uint8_t nes_sram_malloc(uint32_t romsize)
{
    uint16_t i = 0;

    for (i = 0; i < 64; i++)   /* 为NES_RAM,查找1024对齐的内存 */
    {
        NES_SRAM = mymalloc(SRAM12, i * 64);
        NES_RAM = mymalloc(SRAM12, 0X800);  /* 申请2K字节,必须1024字节对齐 */

        if ((uint32_t)NES_RAM % 1024)       /* 不是1024字节对齐 */
        {
            myfree(SRAM12, NES_RAM);        /* 释放内存,然后重新尝试分配 */
            myfree(SRAM12, NES_SRAM);
        }
        else
        {
            myfree(SRAM12, NES_SRAM);       /* 释放内存 */
            break;
        }
    }

    NES_SRAM = mymalloc(SRAM12, 0X2000);
    RomHeader = mymalloc(SRAM12, sizeof(NES_header));
    NES_Mapper = mymalloc(SRAM12, sizeof(MAPPER));
    spr_ram = mymalloc(SRAM12, 0X100);
    ppu = mymalloc(SRAM12, sizeof(ppu_data));
    apu = mymalloc(SRAM12, sizeof(apu_t));      /* sizeof(apu_t)=  12588 */
    wave_buffers = mymalloc(SRAM12, APU_PCMBUF_SIZE * 2);
    saibuf1 = mymalloc(SRAM12, APU_PCMBUF_SIZE * 4 + 10);
    saibuf2 = mymalloc(SRAM12, APU_PCMBUF_SIZE * 4 + 10);
    romfile = mymalloc(SRAMIN, romsize);        /* 申请游戏rom空间,等于nes文件大小 */

    if (i == 64 || !NES_RAM || !NES_SRAM || !RomHeader || !NES_Mapper || !spr_ram || !ppu || !apu || !wave_buffers || !saibuf1 || !saibuf2 || !romfile)
    {
        nes_sram_free();
        return 1;
    }

    memset(NES_SRAM, 0, 0X2000);                /* 清零 */
    memset(RomHeader, 0, sizeof(NES_header));   /* 清零 */
    memset(NES_Mapper, 0, sizeof(MAPPER));      /* 清零 */
    memset(spr_ram, 0, 0X100);                  /* 清零 */
    memset(ppu, 0, sizeof(ppu_data));           /* 清零 */
    memset(apu, 0, sizeof(apu_t));              /* 清零 */
    memset(wave_buffers, 0, APU_PCMBUF_SIZE * 2);   /* 清零 */
    memset(saibuf1, 0, APU_PCMBUF_SIZE * 4 + 10);   /* 清零 */
    memset(saibuf2, 0, APU_PCMBUF_SIZE * 4 + 10);   /* 清零 */
    memset(romfile, 0, romsize);                    /* 清零 */
    return 0;
}

/**
 * @brief       开始nes游戏
 * @param       pname           : nes游戏路径
 * @retval      0, 正常退出
 *              1, 内存错误
 *              2, 文件错误
 *              3, map错误
 */
uint8_t nes_load(uint8_t *pname)
{
    FIL *file;
    uint8_t *buf;       /* 缓存 */
    uint8_t *p;
    uint32_t readlen;   /* 总读取长度 */
    uint16_t bread;     /* 读取的长度 */

    uint8_t res = 0;

//    if (audiodev.status & (1 << 7)) /* 当前在放歌?? */
//    {
//        audio_stop_req(&audiodev);  /* 停止音频播放 */
//        audio_task_delete();        /* 删除音乐播放任务 */
//    }

//    app_es8388_volset(es8388set.mvol);
//    ES8388_ADDA_Cfg(1, 0);      /* 开启DAC */
//    ES8388_Input_Cfg(0);        /* 关闭输入通道 */
//    ES8388_Output_Cfg(1, 0);    /* 开启DAC输出 */

    buf = mymalloc(SRAM12, 1024);
    file = mymalloc(SRAM12, sizeof(FIL));

    if (file == 0)      /* 内存申请失败 */
    {
        myfree(SRAM12, buf);
        return 1;
    }

    res = f_open(file, (char *)pname, FA_READ);

    if (res != FR_OK)   /* 打开文件失败 */
    {
        myfree(SRAM12, buf);
        myfree(SRAM12, file);
        return 2;
    }

    res = nes_sram_malloc(file->obj.objsize);   /* 申请内存 */

    if (res == 0)
    {
        p = romfile;
        readlen = 0;

        while (readlen < file->obj.objsize)
        {
            res = f_read(file, buf, 1024, (UINT *)&bread); /* 读出文件内容 */
            readlen += bread;
            my_mem_copy(p, buf, bread);
            p += bread;

            if (res)break;
        }

        NESrom_crc32 = get_crc32(romfile + 16, file->obj.objsize - 16); /* 获取CRC32的值 */
        res = nes_load_rom();   /* 加载ROM */

        if (res == 0)
        {
            cpu6502_init();     /* 初始化6502,并复位 */
            Mapper_Init();      /* map初始化 */
            PPU_reset();        /* ppu复位 */
            apu_init();         /* apu初始化 */
            nes_sound_open(0, APU_SAMPLE_RATE); /* 初始化播放设备 */
            nes_emulate_frame();/* 进入NES模拟器主循环 */
            nes_sound_close();  /* 关闭声音输出 */
        }
    }

    f_close(file);
    myfree(SRAM12, file);   /* 释放内存 */
    myfree(SRAM12, buf);    /* 释放内存 */
    nes_sram_free();        /* 释放内存 */
    return res;
}

uint16_t nes_xoff = 0;      /* 显示在x轴方向的偏移量(实际显示宽度=256-2*nes_xoff) */
uint16_t nes_yoff = 0;      /* 显示在y轴方向的偏移量 */

/**
 * @brief       设置游戏显示窗口
 * @param       无
 * @retval      无
 */
void nes_set_window(void)
{
    uint16_t xoff = 0, yoff = 0;
    uint16_t lcdwidth, lcdheight;

    if (lcddev.width == 240)
    {
        lcdwidth = 240;
        lcdheight = 240;
        nes_xoff = (256 - lcddev.width) / 2;    /* 得到x轴方向的偏移量 */
    }
    else if (lcddev.width <= 320)
    {
        lcdwidth = 256;
        lcdheight = 240;
        nes_xoff = 0;
    }
    else if (lcddev.width >= 480)
    {
        lcdwidth = 480;
        lcdheight = 480;
        nes_xoff = (256 - (lcdwidth / 2)) / 2; /* 得到x轴方向的偏移量 */
    }

    xoff = (lcddev.width - lcdwidth) / 2;
    yoff = (lcddev.height - lcdheight - gui_phy.tbheight) / 2 + gui_phy.tbheight; /* 屏幕高度 */
    nes_yoff = yoff;
    lcd_set_window(xoff, yoff, lcdwidth, lcdheight);    /* 让NES始终在屏幕的正中央显示 */
    lcd_set_cursor(xoff, yoff);
    lcd_write_ram_prepare();                            /* 写入LCD RAM的准备 */
}

extern void KEYBRD_FCPAD_Decode(uint8_t *fcbuf, uint8_t mode);

/**
 * @brief       读取游戏手柄数据
 * @param       无
 * @retval      无
 */
void nes_get_gamepadval(void)
{
    uint8_t *pt;

    while ((usbx.bDeviceState & 0XC0) == 0X40)   /* USB设备插入了,但是还没连接成功,猛查询 */
    {
        usbapp_pulling();   /* 轮询处理USB事务 */
    }

    usbapp_pulling();       /* 轮询处理USB事务 */

    if (usbx.hdevclass == 4)    /* USB游戏手柄 */
    {
        PADdata0 = fcpad.ctrlval;
        PADdata1 = 0;
    }
    else if (usbx.hdevclass == 3)   /* USB键盘模拟手柄 */
    {
        KEYBRD_FCPAD_Decode(pt, 0);
        PADdata0 = fcpad.ctrlval;
        PADdata1 = fcpad1.ctrlval;
    }
}

uint8_t nes_run_flag = 0;

/**
 * @brief       nes模拟器主循环
 * @param       无
 * @retval      无
 */
void nes_emulate_frame(void)
{
    uint8_t nes_frame;
    nes_run_flag = 1;
    tim8_int_init(10000 - 1, 24000 - 1);    /* 启动TIM8,1s中断一次 */
    nes_set_window();/* 设置窗口 */

    while (1)
    {
        /*  LINES 0-239 */
        PPU_start_frame();

        for (NES_scanline = 0; NES_scanline < 240; NES_scanline++)
        {
            run6502(113 * 256);
            NES_Mapper->HSync(NES_scanline);

            /* 扫描一行 */
            if (nes_frame == 0)scanline_draw(NES_scanline);
            else do_scanline_and_dont_draw(NES_scanline);
        }

        NES_scanline = 240;
        run6502(113 * 256); /* 运行1线 */
        NES_Mapper->HSync(NES_scanline);
        start_vblank();

        if (NMI_enabled())
        {
            cpunmi = 1;
            run6502(7 * 256); /* 运行中断 */
        }

        NES_Mapper->VSync();

        /*  LINES 242-261 */
        for (NES_scanline = 241; NES_scanline < 262; NES_scanline++)
        {
            run6502(113 * 256);
            NES_Mapper->HSync(NES_scanline);
        }

        end_vblank();
        //nes_get_gamepadval(); /* 每3帧查询一次USB */
        apu_soundoutput();  /* 输出游戏声音 */
        framecnt++;
        nes_frame++;

        if (nes_frame > NES_SKIP_FRAME)nes_frame = 0; /* 跳帧 */

        if (system_task_return)break;

        if (lcddev.id == 0X1963)   /* 对于1963,每更新一帧,都要重设窗口 */
        {
            nes_set_window();
        }
    }

    nes_run_flag = 0;
    lcd_set_window(0, 0, lcddev.width, lcddev.height); /* 恢复屏幕窗口 */
    TIM8->CR1 &= ~(1 << 0); /* 关闭定时器8 */
}

/**
 * @brief       6502调试输出
 *  @note       在6502.s里面被调用
 * @param       reg0            : 寄存器0
 * @param       reg1            : 寄存器1
 * @retval      无
 */
void debug_6502(uint16_t reg0, uint8_t reg1)
{
    printf("6502 error:%x,%d\r\n", reg0, reg1);
}

/* nes,音频输出支持部分 */
volatile uint8_t nestransferend = 0;    /* sai传输完成标志 */
volatile uint8_t neswitchbuf = 0;       /* saibufx指示标志 */

/**
 * @brief       SAI音频播放回调函数
 * @param       无
 * @retval      无
 */
void nes_sai_dma_tx_callback(void)
{
    if (DMA2_Stream3->CR & (1 << 19))neswitchbuf = 0;
    else neswitchbuf = 1;

    nestransferend = 1;
}

/**
 * @brief       NES打开音频输出
 * @param       samples_per_sync: 未用到
 * @param       sample_rate     : 音频采样率
 * @retval      无
 */
int nes_sound_open(int samples_per_sync, int sample_rate)
{
//    printf("sound open:%d\r\n", sample_rate);
//    ES8388_ADDA_Cfg(1, 0);      /* 开启DAC */
//    ES8388_Input_Cfg(0);        /* 关闭输入通道 */
//    ES8388_Output_Cfg(1, 0);    /* 开启DAC输出 */
//    ES8388_I2S_Cfg(2, 0);       /* 飞利浦标准,16位数据长度 */
//    app_es8388_volset(es8388set.mvol);
//    SAIA_Init(0, 1, 4);         /* 设置SAI,主发送,16位数据 */
//    SAIA_SampleRate_Set(sample_rate);   /* 设置采样率 */
//    SAIA_TX_DMA_Init((uint8_t *)saibuf1, (uint8_t *)saibuf2, 2 * APU_PCMBUF_SIZE, 1);   /* DMA配置 */
//    sai_tx_callback = nes_sai_dma_tx_callback;  /* 回调函数指wav_sai_dma_callback */
//    SAI_Play_Start();           /* 开启DMA */
    return 1;
}

/**
 * @brief       NES关闭音频输出
 * @param       无
 * @retval      无
 */
void nes_sound_close(void)
{
//    SAI_Play_Stop();
//    app_es8388_volset(0);       /* 关闭ES8388音量输出 */
}

/**
 * @brief       NES音频输出到SAI缓存
 * @param       无
 * @retval      无
 */
void nes_apu_fill_buffer(int samples, uint16_t *wavebuf)
{
    uint8_t i = 3;

    while (i)
    {
        nes_get_gamepadval();   /* 每3帧查询一次USB */
        delay_ms(3);
        i--;
    }

//    int i;

//    while (!nestransferend)   /* 等待音频传输结束 */
//    {
//        delay_ms(5);
//    }

//    nestransferend = 0;

//    if (neswitchbuf == 0)
//    {
//        for (i = 0; i < APU_PCMBUF_SIZE; i++)
//        {
//            saibuf1[2 * i] = wavebuf[i];
//            saibuf1[2 * i + 1] = wavebuf[i];
//        }
//    }
//    else
//    {
//        for (i = 0; i < APU_PCMBUF_SIZE; i++)
//        {
//            saibuf2[2 * i] = wavebuf[i];
//            saibuf2[2 * i + 1] = wavebuf[i];
//        }
//    }
}



















