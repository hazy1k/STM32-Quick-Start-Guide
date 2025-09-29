/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       照相机 实验
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
#include "./PICTURE/piclib.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/DCMI/dcmi.h"
#include "./BSP/OV5640/ov5640.h"


volatile uint8_t g_bmp_request = 0;     /* bmp拍照请求:0,无bmp拍照请求;1,有bmp拍照请求,需要在帧中断里面,关闭DCMI接口 */
uint8_t g_ovx_mode = 0;                 /* bit0:0,RGB565模式;1,JPEG模式 */
uint16_t g_curline = 0;                 /* 摄像头输出数据,当前行编号 */
uint16_t g_yoffset = 0;                 /* y方向的偏移量 */


#define jpeg_buf_size   440*1024        /* 定义JPEG数据缓存jpeg_buf的大小(440K字节) */
#define jpeg_line_size  2*1024          /* 定义DMA接收数据时,一行数据的最大值 */


uint32_t *p_dcmi_line_buf[2];           /* RGB屏时,摄像头采用一行一行读取,定义行缓存 */
uint32_t *p_jpeg_data_buf;              /* JPEG数据缓存buf */


volatile uint32_t g_jpeg_data_len = 0;  /* buf中的JPEG有效数据长度 */

volatile uint8_t g_jpeg_data_ok = 0;    /* JPEG数据采集完成标志
                                         * 0,数据没有采集完;
                                         * 1,数据采集完了,但是还没处理;
                                         * 2,数据已经处理完成了,可以开始下一帧接收
                                         */

/**
 * @brief       处理JPEG数据
 *   @note      当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集
 * @param       无
 * @retval      无
 */
void jpeg_data_process(void)
{
    uint16_t i;
    uint16_t rlen;                  /* 剩余数据长度 */
    uint32_t *pbuf;
    g_curline = g_yoffset;          /* 行数复位 */

    if (g_ovx_mode & 0X01)          /* 只有在JPEG格式下,才需要做处理 */
    {
        if (g_jpeg_data_ok == 0)    /* jpeg数据还未采集完 */
        {
            __HAL_DMA_DISABLE(&g_dma_dcmi_handle);                          /* 停止当前传输 */

            rlen=jpeg_line_size-__HAL_DMA_GET_COUNTER(&g_dma_dcmi_handle);  /* 得到剩余数据长度 */

            if (g_jpeg_data_len > (jpeg_buf_size / 4 - rlen))               /* 内存不够了,直接退出 */
            {
                printf("g_jpeg_data_len1:%d\r\n", g_jpeg_data_len);         /* 打印当前长度（uint32_t）*/
                g_jpeg_data_ok = 1;                                         /* 标记JPEG数据采集完按成,等待其他函数处理 */
                return;
            }

            pbuf = p_jpeg_data_buf + g_jpeg_data_len;   /* 偏移到有效数据末尾,继续添加 */

            if (DMA1_Stream1->CR & (1 << 19))
            {
                for (i = 0; i < rlen; i++)
                {
                    pbuf[i] = p_dcmi_line_buf[1][i];    /* 读取buf1里面的剩余数据 */
                }
            }
            else
            {
                for (i = 0; i < rlen; i++)
                {
                    pbuf[i] = p_dcmi_line_buf[0][i];    /* 读取buf0里面的剩余数据 */
                }
            }
            
            g_jpeg_data_len += rlen;    /* 加上剩余长度 */
            g_jpeg_data_ok = 1;         /* 标记JPEG数据采集完按成,等待其他函数处理 */
        }

        if (g_jpeg_data_ok == 2)        /* 上一次的jpeg数据已经被处理了 */
        {
            __HAL_DMA_SET_COUNTER(&g_dma_dcmi_handle, jpeg_line_size);  /* 传输长度为jpeg_buf_size*4字节 */
            __HAL_DMA_ENABLE(&g_dma_dcmi_handle);                       /* 重新传输 */
            g_jpeg_data_ok = 0;                                         /* 标记数据未采集 */
            g_jpeg_data_len = 0;                                        /* 数据重新开始 */
        }
    }
    else
    {
        if (g_bmp_request == 1)     /* 有bmp拍照请求,关闭DCMI */
        {
            dcmi_stop();            /* 停止DCMI */
            g_bmp_request = 0;      /* 标记请求处理完成 */
        }

        lcd_set_cursor(0, 0);
        lcd_write_ram_prepare();    /* 开始写入GRAM */
    }
}

/**
 * @brief       JPEG数据接收回调函数
 * @param       无
 * @retval      无
 */
void jpeg_dcmi_rx_callback(void)
{
    uint16_t i;
    volatile uint32_t *pbuf;

    if (g_jpeg_data_len > (jpeg_buf_size / 4 - jpeg_line_size)) /* 内存不够了,直接退出 */
    {
        printf("g_jpeg_data_len:%d\r\n", g_jpeg_data_len);      /* 打印当前长度（uint32_t） */
        return;
    }

    pbuf = p_jpeg_data_buf + g_jpeg_data_len;   /* 偏移到有效数据末尾 */

    if (DMA1_Stream1->CR & (1 << 19))           /* buf0已满,正常处理buf1 */
    {
        for (i = 0; i < jpeg_line_size; i++)
        {
            pbuf[i] = p_dcmi_line_buf[0][i];    /* 读取buf0里面的数据 */
        }
        
        g_jpeg_data_len += jpeg_line_size;      /* 偏移 */
    }
    else                                        /* buf1已满,正常处理buf0 */
    {
        for (i = 0; i < jpeg_line_size; i++)
        {
            pbuf[i] = p_dcmi_line_buf[1][i];    /* 读取buf1里面的数据 */
        }
        
        g_jpeg_data_len += jpeg_line_size;      /* 偏移 */
    }
    
    SCB_CleanInvalidateDCache();                /* 清除无效化DCache */
}

/**
 * @brief       切换为OV5640模式
 *   @note      切换PC8/PC9/PC11为DCMI复用功能(AF13)
 * @param       无
 * @retval      无
 */
void sw_ov5640_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ov5640_write_reg(0X3017, 0XFF);     /* 开启OV5650输出(可以正常显示) */
    ov5640_write_reg(0X3018, 0XFF);
    
    /* GPIOC8/9/11切换为 DCMI接口 */
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF13_DCMI;        /* 复用为DCMI */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* 初始化PC8，9, 11引脚 */
}

/**
 * @brief       切换为SD卡模式
 *   @note      切换PC8/PC9/PC11为SDMMC复用功能(AF12)
 * @param       无
 * @retval      无
 */
void sw_sdcard_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ov5640_write_reg(0X3017, 0X00);     /* 关闭OV5640全部输出(不影响SD卡通信) */
    ov5640_write_reg(0X3018, 0X00);
    
    /* GPIOC8/9/11切换为 SDIO接口 */
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF12_SDIO1;       /* 复用为SDIO */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* 初始化PC8，9, 11引脚 */
}

/**
 * @brief       文件名自增（避免覆盖）
 *   @note      bmp组合成: 形如 "0:PHOTO/PIC13141.bmp" 的文件名
 *              jpg组合成: 形如 "0:PHOTO/PIC13141.jpg" 的文件名
 * @param       pname : 有效的文件名
 * @param       mode  : 0, 创建.bmp文件;  1, 创建.jpg文件;
 * @retval      无
 */
void camera_new_pathname(uint8_t *pname, uint8_t mode)
{
    uint8_t res;
    uint16_t index = 0;
    FIL *ftemp;
    
    ftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));   /* 开辟FIL字节的内存区域 */

    if (ftemp == NULL) return;  /* 内存申请失败 */

    while (index < 0XFFFF)
    {
        if (mode == 0)  /* 创建.bmp文件名 */
        {
            sprintf((char *)pname, "0:PHOTO/PIC%05d.bmp", index);
        }
        else            /* 创建.jpg文件名 */
        {
            sprintf((char *)pname, "0:PHOTO/PIC%05d.jpg", index);
        }
        
        res = f_open(ftemp, (const TCHAR *)pname, FA_READ); /* 尝试打开这个文件 */

        if (res == FR_NO_FILE) break;   /* 该文件名不存在, 正是我们需要的 */

        index++;
    }
    myfree(SRAMIN, ftemp);
}

/**
 * @brief       OV5640拍照jpg图片
 * @param       pname : 要创建的jpg文件名(含路径)
 * @retval      0, 成功; 其他,错误代码;
 */
uint8_t ov5640_jpg_photo(uint8_t *pname)
{
    FIL *f_jpg;
    uint8_t res = 0, headok = 0;
    uint32_t bwr;
    uint32_t i, jpgstart, jpglen;
    uint8_t *pbuf;
    
    f_jpg = (FIL *)mymalloc(SRAMIN, sizeof(FIL));   /* 开辟FIL字节的内存区域 */

    if (f_jpg == NULL) return 0XFF;     /* 内存申请失败 */

    g_ovx_mode = 1;
    g_jpeg_data_ok = 0;
    
    sw_ov5640_mode();                           /* 切换为OV5640模式 */
    ov5640_jpeg_mode();                         /* JPEG模式 */
    ov5640_outsize_set(4, 0, 1280, 800);        /* 设置输出尺寸(WXGA) */
    dcmi_rx_callback = jpeg_dcmi_rx_callback;   /* JPEG接收数据回调函数 */
    dcmi_dma_init((uint32_t)p_dcmi_line_buf[0], (uint32_t)p_dcmi_line_buf[1], jpeg_line_size, DMA_MDATAALIGN_WORD, DMA_MINC_ENABLE); /* DCMI DMA配置 */
    dcmi_start();                               /* 启动传输 */

    while (g_jpeg_data_ok != 1);    /* 等待第一帧图片采集完 */

    g_jpeg_data_ok = 2;             /* 忽略本帧图片,启动下一帧采集 */

    while (g_jpeg_data_ok != 1);    /* 等待第二帧图片采集完,第二帧,才保存到SD卡去 */

    dcmi_stop();        /* 停止DMA搬运 */
    g_ovx_mode = 0;
    sw_sdcard_mode();   /* 切换为SD卡模式 */
    printf("jpeg data size:%d\r\n", g_jpeg_data_len * 4);   /* 串口打印JPEG文件大小 */
    pbuf = (uint8_t *)p_jpeg_data_buf;
    jpglen = 0;         /* 设置jpg文件大小为0 */
    headok = 0;         /* 清除jpg头标记 */

    for (i = 0; i < g_jpeg_data_len * 4; i++)           /* 查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小 */
    {
        if ((pbuf[i] == 0XFF) && (pbuf[i + 1] == 0XD8)) /* 找到FF D8 */
        {
            jpgstart = i;
            headok = 1;                                 /* 标记找到jpg头(FF D8) */
        }

        if ((pbuf[i] == 0XFF) && (pbuf[i + 1] == 0XD9) && headok)   /* 找到头以后,再找FF D9 */
        {
            jpglen = i - jpgstart + 2;
            break;
        }
    }

    if (jpglen)         /* 正常的jpeg数据 */
    {
        res = f_open(f_jpg, (const TCHAR *)pname, FA_WRITE | FA_CREATE_NEW);    /* 模式0,或者尝试打开失败,则创建新文件 */

        if (res == 0)
        {
            pbuf += jpgstart;   /* 偏移到0XFF,0XD8处 */
            res = f_write(f_jpg, pbuf, jpglen, &bwr);

            if (bwr != jpglen) res = 0XFE;
        }

        f_close(f_jpg);
    }
    else
    {
        res = 0XFD;
    }

    g_jpeg_data_len = 0;
    sw_ov5640_mode();       /* 切换为OV5640模式 */
    ov5640_rgb565_mode();   /* RGB565模式 */
    dcmi_dma_init((uint32_t)&LCD->LCD_RAM, 0, 1, DMA_MDATAALIGN_HALFWORD, DMA_MINC_DISABLE); /* DCMI DMA配置,MCU屏,竖屏 */
    myfree(SRAMIN, f_jpg);
    return res;
}

int main(void)
{
    uint8_t res;
    float fac;
    uint8_t *pname;                     /* 带路径的文件名 */
    uint8_t key;                        /* 键值 */
    uint8_t i;
    uint8_t sd_ok = 1;                  /* 0,sd卡不正常;1,SD卡正常 */
    uint8_t scale = 1;                  /* 默认是全尺寸缩放 */
    uint8_t msgbuf[15];                 /* 消息缓存区 */
    uint16_t outputheight = 0;

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
    beep_init();                        /* 初始化蜂鸣器 */
    ov5640_init();                      /* 初始化OV5640 */
    sw_sdcard_mode();                   /* 首先切换为OV5640模式 */
    piclib_init();                      /* 初始化画图 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池(ITCM) */
    exfuns_init();                      /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);            /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);            /* 挂载FLASH */

//    lcd_display_dir(1);                 /* 设置成横屏 */

    while (fonts_init())                /* 检查字库 */
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!", RED);
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);   /* 清除显示 */
        delay_ms(200);
    }

    text_show_string(30, 50, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "照相机实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "KEY0:拍照(bmp格式)", 16, 0, RED);
    text_show_string(30, 110, 200, 16, "KEY1:拍照(jpg格式)", 16, 0, RED);
    text_show_string(30, 130, 200, 16, "WK_UP:FullSize/Scale", 16, 0, RED);
    
    res = f_mkdir("0:/PHOTO");              /* 创建PHOTO文件夹 */
    if (res != FR_EXIST && res != FR_OK)    /* 发生了错误 */
    {
        res = f_mkdir("0:/PHOTO");          /* 创建PHOTO文件夹 */
        text_show_string(30, 150, 240, 16, "SD卡错误!", 16, 0, RED);
        delay_ms(200);
        text_show_string(30, 150, 240, 16, "拍照功能将不可用!", 16, 0, RED);
        delay_ms(200);
        sd_ok = 0;
    }

    p_dcmi_line_buf[0] = mymalloc(SRAM12, jpeg_line_size * 4);  /* 为jpeg dma接收申请内存 */
    p_dcmi_line_buf[1] = mymalloc(SRAM12, jpeg_line_size * 4);  /* 为jpeg dma接收申请内存 */
    p_jpeg_data_buf = mymalloc(SRAMIN, jpeg_buf_size);          /* 为jpeg文件申请内存 */
    pname = mymalloc(SRAMIN, 30);                               /* 为带路径的文件名分配30个字节的内存 */

    while (pname == NULL || !p_dcmi_line_buf[0] || !p_dcmi_line_buf[1] || !p_jpeg_data_buf) /* 内存分配出错 */
    {
        text_show_string(30, 150, 240, 16, "内存分配失败!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 150, 240, 146, WHITE);                     /* 清除显示 */
        delay_ms(200);
    }

    while (ov5640_init())   /* 初始化OV5640 */
    {
        text_show_string(30, 170, 240, 16, "OV5640 错误!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 150, 239, 206, WHITE);
        delay_ms(200);
    }

    delay_ms(100);
    text_show_string(30, 170, 230, 16, "OV5640 正常", 16, 0, RED);
    
    /* 自动对焦初始化 */
    ov5640_rgb565_mode();           /* RGB565模式 */
    ov5640_focus_init();
    ov5640_light_mode(0);           /* 自动模式 */
    ov5640_color_saturation(3);     /* 色彩饱和度0 */
    ov5640_brightness(4);           /* 亮度0 */
    ov5640_contrast(3);             /* 对比度0 */
    ov5640_sharpness(33);           /* 自动锐度 */
    ov5640_focus_constant();        /* 启动持续对焦 */
    dcmi_init();                    /* DCMI配置 */
    dcmi_dma_init((uint32_t)&LCD->LCD_RAM, 0, 1, DMA_MDATAALIGN_HALFWORD, DMA_MINC_DISABLE);    /* DCMI DMA配置,MCU屏,竖屏 */

    if (lcddev.height >= 800)
    {
        g_yoffset = (lcddev.height - 800) / 2;
        outputheight = 800;
        ov5640_write_reg(0x3035, 0X51);     /* 降低输出帧率，否则可能抖动 */
    }
    else
    {
        g_yoffset = 0;
        outputheight = lcddev.height;
    }

    g_curline = g_yoffset;      /* 行数复位 */
    ov5640_outsize_set(16, 4, lcddev.width, outputheight);  /* 满屏缩放显示 */
    dcmi_start();               /* 启动传输 */
    lcd_clear(BLACK);

    while (1)
    {
        key = key_scan(0);      /* 不支持连按 */

        if (key)
        {
            if (key == KEY0_PRES)       /* 如果是BMP拍照,则等待1秒钟,去抖动,以获得稳定的bmp照片 */
            {
                delay_ms(300);
                g_bmp_request = 1;      /* 请求关闭DCMI */

                while (g_bmp_request);  /* 等带请求处理完成 */
            }
            else
            {
                dcmi_stop();
            }
            
            if (key == WKUP_PRES)       /* 缩放处理 */
            {
                scale = !scale;

                if (scale == 0)
                {
                    fac = (float)800 / outputheight;    /* 得到比例因子 */
                    ov5640_outsize_set((1280 - fac * lcddev.width) / 2, (800 - fac * outputheight) / 2, lcddev.width, outputheight);
                    sprintf((char *)msgbuf, "Full Size 1:1");
                }
                else
                {
                    ov5640_outsize_set(16, 4, lcddev.width, outputheight);
                    sprintf((char *)msgbuf, "Scale");
                }

                delay_ms(800);
            }
            else if (sd_ok)             /* SD卡正常才可以拍照 */
            {
                sw_sdcard_mode();       /* 切换为SD卡模式 */

                if (key == KEY0_PRES)   /* BMP拍照 */
                {
                    camera_new_pathname(pname, 0);  /* 得到文件名 */
                    res = bmp_encode(pname, 0, g_yoffset, lcddev.width, outputheight, 0);
                    sw_ov5640_mode();               /* 切换为OV5640模式 */
                }
                else if (key == KEY1_PRES)          /* JPG拍照 */
                {
                    camera_new_pathname(pname, 1);  /* 得到文件名 */
                    res = ov5640_jpg_photo(pname);

                    if (scale == 0)
                    {
                        fac = (float)800 / outputheight;    /* 得到比例因子 */
                        ov5640_outsize_set((1280 - fac * lcddev.width) / 2, (800 - fac * outputheight) / 2, lcddev.width, outputheight);
                    }
                    else
                    {
                        ov5640_outsize_set(16, 4, lcddev.width, outputheight);
                    }

                    if (lcddev.height >= 800) ov5640_write_reg(0x3035, 0X51); /* 降低输出帧率，否则可能抖动 */
                }

                if (res)    /* 拍照有误 */
                {
                    text_show_string(30, 130, 240, 16, "写入文件错误!", 16, 0, RED);
                }
                else
                {
                    text_show_string(30, 130, 240, 16, "拍照成功!", 16, 0, RED);
                    text_show_string(30, 150, 240, 16, "保存为:", 16, 0, RED);
                    text_show_string(30 + 56, 150, 240, 16, (char*)pname, 16, 0, RED);
                    BEEP(1);    /* 蜂鸣器短叫，提示拍照完成 */
                    delay_ms(100);
                    BEEP(0);    /* 关闭蜂鸣器 */
                }

                delay_ms(1000); /* 等待1秒钟 */
                dcmi_start();   /* 这里先使能dcmi,然后立即关闭DCMI,后面再开启DCMI,可以防止RGB屏的侧移问题 */
                dcmi_stop();
            }
            else                /* 提示SD卡错误 */
            {
                text_show_string(30, 130, 240, 16, "SD卡错误!", 16, 0, RED);
                text_show_string(30, 150, 240, 16, "拍照功能不可用!", 16, 0, RED);
            }

            dcmi_start();       /* 开始显示 */
        }

        delay_ms(10);
        i++;

        if (i == 20)
        {
            i = 0; 
            LED0_TOGGLE();
        }
    }
}








