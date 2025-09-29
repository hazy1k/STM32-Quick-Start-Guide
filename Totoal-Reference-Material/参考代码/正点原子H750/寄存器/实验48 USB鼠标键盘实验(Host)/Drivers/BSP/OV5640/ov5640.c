/**
 ****************************************************************************************************
 * @file        ov5640.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       OV5640 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

#include "./BSP/OV5640/sccb.h"
#include "./BSP/OV5640/ov5640.h"
#include "./BSP/OV5640/ov5640af.h"
#include "./BSP/OV5640/ov5640cfg.h"


/**
 * @brief       OV5640 读寄存器
 * @param       reg : 寄存器地址
 * @retval      读到的寄存器值
 */
uint8_t ov5640_read_reg(uint16_t reg)
{
    uint8_t data = 0;
    sccb_start();                   /* 启动SCCB传输 */
    sccb_send_byte(OV5640_ADDR);    /* 写器件ID */
    sccb_send_byte(reg >> 8);       /* 写寄存器高8位地址 */
    sccb_send_byte(reg);            /* 写寄存器低8位地址 */
    sccb_stop();
    
    /* 设置寄存器地址后，才是读 */
    sccb_start();
    sccb_send_byte(OV5640_ADDR | 0X01); /* 发送读命令 */
    data = sccb_read_byte();        /* 读取数据 */
    sccb_nack();
    sccb_stop();
    return data;
}

/**
 * @brief       OV5640 写寄存器
 * @param       reg : 寄存器地址
 * @param       data: 要写入寄存器的值
 * @retval      0, 成功; 1, 失败;
 */
uint8_t ov5640_write_reg(uint16_t reg, uint8_t data)
{
    uint8_t res = 0;
    sccb_start();   /* 启动SCCB传输 */

    if (sccb_send_byte(OV5640_ADDR))res = 1;/* 写器件ID */

    if (sccb_send_byte(reg >> 8))res = 1;   /* 写寄存器高8位地址 */

    if (sccb_send_byte(reg))res = 1;        /* 写寄存器低8位地址 */

    if (sccb_send_byte(data))res = 1;       /* 写数据 */

    sccb_stop();
    return res;
}

/**
 * @brief       OV5640 初始化
 * @param       无
 * @retval      0, 成功; 1, 失败;
 */
uint8_t ov5640_init(void)
{
    uint16_t i = 0;
    uint16_t reg;
    
    OV_PWDN_GPIO_CLK_ENABLE();      /* 使能OV_PWDN脚时钟 */
    OV_RESET_GPIO_CLK_ENABLE();     /* 使能OV_RESET脚时钟 */

    sys_gpio_set(OV_PWDN_GPIO_PORT, OV_PWDN_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* PWDN引脚模式设置,推挽输出 */

    sys_gpio_set(OV_RESET_GPIO_PORT, OV_RESET_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* RESET引脚模式设置,推挽输出 */

    OV5640_RST(0);      /* 必须先拉低OV5640的RST脚,再上电 */
    delay_ms(20);
    OV5640_PWDN(0);     /* POWER ON */
    delay_ms(5);
    OV5640_RST(1);      /* 结束复位 */
    delay_ms(20);
    sccb_init();        /* 初始化SCCB 的IO口 */
    delay_ms(5);
    reg = ov5640_read_reg(OV5640_CHIPIDH);  /* 读取ID 高八位 */
    reg <<= 8;
    reg |= ov5640_read_reg(OV5640_CHIPIDL); /* 读取ID 低八位 */

    if (reg != OV5640_ID)   /* ID是否正常? */
    {
        printf("ID:%d\r\n", reg);
        return 1;           /* 失败 */
    }

    ov5640_write_reg(0x3103, 0X11); /* system clock from pad, bit[1] */
    ov5640_write_reg(0X3008, 0X82); /* 软复位 */
    delay_ms(10);

    /* 初始化 OV5640 */
    for (i = 0; i < sizeof(ov5640_init_reg_tbl) / 4; i++)
    {
        ov5640_write_reg(ov5640_init_reg_tbl[i][0], ov5640_init_reg_tbl[i][1]);
    }

    /* 检查闪光灯是否正常 */
    ov5640_flash_ctrl(1);   /* 打开闪光灯 */
    delay_ms(50);
    ov5640_flash_ctrl(0);   /* 关闭闪光灯 */
    return 0;               /* ok */
}

/**
 * @brief       OV5640 切换为JPEG模式
 * @param       无
 * @retval      无
 */
void ov5640_jpeg_mode(void)
{
    uint16_t i = 0;

    /* 设置:输出JPEG数据 */
    for (i = 0; i < (sizeof(OV5640_jpeg_reg_tbl) / 4); i++)
    {
        ov5640_write_reg(OV5640_jpeg_reg_tbl[i][0], OV5640_jpeg_reg_tbl[i][1]); /* 发送配置数组 */
    }
}

/**
 * @brief       OV5640 切换为RGB565模式
 * @param       无
 * @retval      无
 */
void ov5640_rgb565_mode(void)
{
    uint16_t i = 0;

    /* 设置:RGB565输出 */
    for (i = 0; i < (sizeof(ov5640_rgb565_reg_tbl) / 4); i++)
    {
        ov5640_write_reg(ov5640_rgb565_reg_tbl[i][0], ov5640_rgb565_reg_tbl[i][1]); /* 发送配置数组 */
    }
}

/* EV曝光补偿设置参数表，支持7个等级 */
const static uint8_t OV5640_EXPOSURE_TBL[7][6] =
{
    0x10, 0x08, 0x10, 0x08, 0x20, 0x10, /* -3 */
    0x20, 0x18, 0x41, 0x20, 0x18, 0x10, /* - */
    0x30, 0x28, 0x61, 0x30, 0x28, 0x10, /* -1 */
    0x38, 0x30, 0x61, 0x38, 0x30, 0x10, /* 0 */
    0x40, 0x38, 0x71, 0x40, 0x38, 0x10, /* +1 */
    0x50, 0x48, 0x90, 0x50, 0x48, 0x20, /* +2 */
    0x60, 0x58, 0xa0, 0x60, 0x58, 0x20, /* +3 */
};

/**
 * @brief       OV5640 EV曝光补偿
 * @param       exposure : 0~6, 代表补偿 -3 ~ 3.
 * @retval      无
 */
void ov5640_exposure(uint8_t exposure)
{
    ov5640_write_reg(0x3212, 0x03);     /* start group 3 */
    ov5640_write_reg(0x3a0f, OV5640_EXPOSURE_TBL[exposure][0]);
    ov5640_write_reg(0x3a10, OV5640_EXPOSURE_TBL[exposure][1]);
    ov5640_write_reg(0x3a1b, OV5640_EXPOSURE_TBL[exposure][2]);
    ov5640_write_reg(0x3a1e, OV5640_EXPOSURE_TBL[exposure][3]);
    ov5640_write_reg(0x3a11, OV5640_EXPOSURE_TBL[exposure][4]);
    ov5640_write_reg(0x3a1f, OV5640_EXPOSURE_TBL[exposure][5]);
    ov5640_write_reg(0x3212, 0x13);     /* end group 3 */
    ov5640_write_reg(0x3212, 0xa3);     /* launch group 3 */
}

/* 灯光模式参数表,支持5个模式 */
const static uint8_t OV5640_WHITEBALANCE_TBL[5][7] =
{
    0x04, 0X00, 0X04, 0X00, 0X04, 0X00, 0X00,   /* Auto,自动 */
    0x06, 0X1C, 0X04, 0X00, 0X04, 0XF3, 0X01,   /* Sunny,日光 */
    0x05, 0X48, 0X04, 0X00, 0X07, 0XCF, 0X01,   /* Office,办公室 */
    0x06, 0X48, 0X04, 0X00, 0X04, 0XD3, 0X01,   /* Cloudy,阴天 */
    0x04, 0X10, 0X04, 0X00, 0X08, 0X40, 0X01,   /* Home,室内 */
};

/**
 * @brief       OV5640 白平衡设置
 * @param       mode : 0~4, 白平衡模式
 *   @arg       0: 自动
 *   @arg       1: 日光sunny
 *   @arg       2: 办公室office
 *   @arg       3: 阴天cloudy
 *   @arg       4: 家里home
 * @retval      无
 */
void ov5640_light_mode(uint8_t mode)
{
    uint8_t i;
    ov5640_write_reg(0x3212, 0x03);     /* start group 3 */

    for (i = 0; i < 7; i++)
    {
        ov5640_write_reg(0x3400 + i, OV5640_WHITEBALANCE_TBL[mode][i]); /* 设置白平衡 */
    }
    ov5640_write_reg(0x3212, 0x13);     /* end group 3 */
    ov5640_write_reg(0x3212, 0xa3);     /* launch group 3 */
}

/* 色彩饱和度设置参数表,支持7个等级 */
const static uint8_t OV5640_SATURATION_TBL[7][6] =
{
    0X0C, 0x30, 0X3D, 0X3E, 0X3D, 0X01, /* -3 */
    0X10, 0x3D, 0X4D, 0X4E, 0X4D, 0X01, /* -2 */
    0X15, 0x52, 0X66, 0X68, 0X66, 0X02, /* -1 */
    0X1A, 0x66, 0X80, 0X82, 0X80, 0X02, /* +0 */
    0X1F, 0x7A, 0X9A, 0X9C, 0X9A, 0X02, /* +1 */
    0X24, 0x8F, 0XB3, 0XB6, 0XB3, 0X03, /* +2 */
    0X2B, 0xAB, 0XD6, 0XDA, 0XD6, 0X04, /* +3 */
};

/**
 * @brief       OV5640 色彩饱和度设置
 * @param       set : 0~6, 代表色彩饱和度 -3 ~ 3.
 * @retval      无
 */
void ov5640_color_saturation(uint8_t sat)
{
    uint8_t i;
    ov5640_write_reg(0x3212, 0x03);     /* start group 3 */
    ov5640_write_reg(0x5381, 0x1c);
    ov5640_write_reg(0x5382, 0x5a);
    ov5640_write_reg(0x5383, 0x06);

    for (i = 0; i < 6; i++)
    {
        ov5640_write_reg(0x5384 + i, OV5640_SATURATION_TBL[sat][i]);    /* 设置饱和度 */
    }
    ov5640_write_reg(0x538b, 0x98);
    ov5640_write_reg(0x538a, 0x01);
    ov5640_write_reg(0x3212, 0x13);     /* end group 3 */
    ov5640_write_reg(0x3212, 0xa3);     /* launch group 3 */
}

/**
 * @brief       OV5640 亮度设置
 * @param       bright : 0~8, 代表亮度 -4 ~ 4.
 * @retval      无
 */
void ov5640_brightness(uint8_t bright)
{
    uint8_t brtval;

    if (bright < 4)brtval = 4 - bright;
    else brtval = bright - 4;

    ov5640_write_reg(0x3212, 0x03);	/* start group 3 */
    ov5640_write_reg(0x5587, brtval << 4);

    if (bright < 4)ov5640_write_reg(0x5588, 0x09);
    else ov5640_write_reg(0x5588, 0x01);

    ov5640_write_reg(0x3212, 0x13); /* end group 3 */
    ov5640_write_reg(0x3212, 0xa3); /* launch group 3 */
}

/**
 * @brief       OV5640 对比度设置
 * @param       contrast : 0~6, 代表对比度 -3 ~ 3.
 * @retval      无
 */
void ov5640_contrast(uint8_t contrast)
{
    uint8_t reg0val = 0X00;     /* contrast = 3, 默认对比度 */
    uint8_t reg1val = 0X20;

    switch (contrast)
    {
        case 0: /* -3 */
            reg1val = reg0val = 0X14;
            break;

        case 1: /* -2 */
            reg1val = reg0val = 0X18;
            break;

        case 2: /* -1 */
            reg1val = reg0val = 0X1C;
            break;

        case 4: /* 1 */
            reg0val = 0X10;
            reg1val = 0X24;
            break;

        case 5: /* 2 */
            reg0val = 0X18;
            reg1val = 0X28;
            break;

        case 6: /* 3 */
            reg0val = 0X1C;
            reg1val = 0X2C;
            break;
    }

    ov5640_write_reg(0x3212, 0x03); /* start group 3 */
    ov5640_write_reg(0x5585, reg0val);
    ov5640_write_reg(0x5586, reg1val);
    ov5640_write_reg(0x3212, 0x13); /* end group 3 */
    ov5640_write_reg(0x3212, 0xa3); /* launch group 3 */
}

/**
 * @brief       OV5640 锐度设置
 * @param       sharp : 0~33.
 *   @arg       0   : 关闭 
 *   @arg       1~32: 锐度范围
 *   @arg       33  : auto
 *
 * @retval      无
 */
void ov5640_sharpness(uint8_t sharp)
{
    if (sharp < 33) /* 设置锐度值 */
    {
        ov5640_write_reg(0x5308, 0x65);
        ov5640_write_reg(0x5302, sharp);
    }
    else    /* 自动锐度 */
    {
        ov5640_write_reg(0x5308, 0x25);
        ov5640_write_reg(0x5300, 0x08);
        ov5640_write_reg(0x5301, 0x30);
        ov5640_write_reg(0x5302, 0x10);
        ov5640_write_reg(0x5303, 0x00);
        ov5640_write_reg(0x5309, 0x08);
        ov5640_write_reg(0x530a, 0x30);
        ov5640_write_reg(0x530b, 0x04);
        ov5640_write_reg(0x530c, 0x06);
    }
}

/* 特效设置参数表,支持7个特效 */
const static uint8_t OV5640_EFFECTS_TBL[7][3] =
{
    0X06, 0x40, 0X10, /* 正常 */
    0X1E, 0xA0, 0X40, /* 冷色 */
    0X1E, 0x80, 0XC0, /* 暖色 */
    0X1E, 0x80, 0X80, /* 黑白 */
    0X1E, 0x40, 0XA0, /* 泛黄 */
    0X40, 0x40, 0X10, /* 反色 */
    0X1E, 0x60, 0X60, /* 偏绿 */
};

/**
 * @brief       OV5640 特效设置
 * @param       eft : 0~6, 特效效果
 *   @arg       0: 正常
 *   @arg       1: 冷色
 *   @arg       2: 暖色
 *   @arg       3: 黑白
 *   @arg       4: 偏黄
 *   @arg       5: 反色
 *   @arg       6: 偏绿
 * @retval      无
 */
void ov5640_special_effects(uint8_t eft)
{
    ov5640_write_reg(0x3212, 0x03); /* start group 3 */
    ov5640_write_reg(0x5580, OV5640_EFFECTS_TBL[eft][0]);
    ov5640_write_reg(0x5583, OV5640_EFFECTS_TBL[eft][1]); /*  sat U */
    ov5640_write_reg(0x5584, OV5640_EFFECTS_TBL[eft][2]); /*  sat V */
    ov5640_write_reg(0x5003, 0x08);
    ov5640_write_reg(0x3212, 0x13); /* end group 3 */
    ov5640_write_reg(0x3212, 0xa3); /* launch group 3 */
}

/**
 * @brief       OV5640 测试序列
 * @param       mode : 0~2
 *   @arg       0: 关闭 
 *   @arg       1: 彩条
 *   @arg       2: 色块
 *
 * @retval      无
 */
void ov5640_test_pattern(uint8_t mode)
{
    if (mode == 0)      /* 关闭测试序列 */
    {
        ov5640_write_reg(0X503D, 0X00);
    }
    else if (mode == 1) /* 彩条测试 */
    {
        ov5640_write_reg(0X503D, 0X80);
    }
    else if (mode == 2) /* 色块测试 */
    {
        ov5640_write_reg(0X503D, 0X82);
    }
}

/**
 * @brief       OV5640 闪光灯控制
 * @param       sw : 闪光灯状态
 *   @arg       0: 关闭
 *   @arg       1: 打开
 *
 * @retval      无
 */
void ov5640_flash_ctrl(uint8_t sw)
{
    ov5640_write_reg(0x3016, 0X02);
    ov5640_write_reg(0x301C, 0X02);

    if (sw) /* 打开 */
    {
        ov5640_write_reg(0X3019, 0X02);
    }
    else    /* 关闭 */
    {
        ov5640_write_reg(0X3019, 0X00);
    }
}

/**
 * @brief       设置图像输出大小
 *   @note      OV5640输出图像的大小(分辨率),完全由该函数确定
 *              由于开启了scale功能,用于输出的图像窗口为: xsize - 2 * offx, ysize - 2 * offy
 *              实际输出(width,height), 是在: xsize - 2 * offx, ysize - 2 * offy的基础上进行缩放处理.
 *              一般设置offx和offy的值为16和4,更小也是可以,不过默认是16和4
 *
 * @param       offx,offy    : 输出图像在ov5640_image_window_set设定窗口(假设长宽为xsize和ysize)上的偏移
 * @param       width,height : 实际输出图像的宽度和高度
 * @retval      0, 成功; 其他, 失败;
 */
uint8_t ov5640_outsize_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{
    ov5640_write_reg(0X3212, 0X03);         /* 开始组3 */
    
    /* 以下设置决定实际输出尺寸(带缩放) */
    ov5640_write_reg(0x3808, width >> 8);   /* 设置实际输出宽度高字节 */
    ov5640_write_reg(0x3809, width & 0xff); /* 设置实际输出宽度低字节 */
    ov5640_write_reg(0x380a, height >> 8);  /* 设置实际输出高度高字节 */
    ov5640_write_reg(0x380b, height & 0xff);/* 设置实际输出高度低字节 */
    
    /* 以下设置决定输出尺寸在ISP上面的取图范围
     * 范围: xsize-2*offx,ysize-2*offy
     */
    ov5640_write_reg(0x3810, offx >> 8);    /* 设置X offset高字节 */
    ov5640_write_reg(0x3811, offx & 0xff);  /* 设置X offset低字节 */

    ov5640_write_reg(0x3812, offy >> 8);    /* 设置Y offset高字节 */
    ov5640_write_reg(0x3813, offy & 0xff);  /* 设置Y offset低字节 */

    ov5640_write_reg(0X3212, 0X13);         /* 结束组3 */
    ov5640_write_reg(0X3212, 0Xa3);         /* 启用组3设置 */
    return 0;
}

/**
 * @brief       设置图像开窗大小(ISP大小),非必要,一般无需调用此函数
 *   @note      在整个传感器上面开窗(最大2592*1944),用于ov5640_outsize_set的输出
 *              注意:本函数的宽度和高度,必须大于等于ov5640_outsize_set函数的宽度和高度
 *              ov5640_outsize_set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
 *              自动计算缩放比例,输出给外部设备.
 *
 * @param       offx,offy    : 图像在传感器上面的坐标偏移值
 * @param       width,height : 宽度(对应:horizontal)和高度(对应:vertical)
 * @retval      0, 成功; 其他, 失败;
 */
uint8_t ov5640_image_window_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{
    uint16_t xst, yst, xend, yend;
    xst = offx;
    yst = offy;
    xend = offx + width - 1;
    yend = offy + height - 1;
    ov5640_write_reg(0X3212, 0X03);     /* 开始组3 */
    ov5640_write_reg(0X3800, xst >> 8);
    ov5640_write_reg(0X3801, xst & 0XFF);
    ov5640_write_reg(0X3802, yst >> 8);
    ov5640_write_reg(0X3803, yst & 0XFF);
    ov5640_write_reg(0X3804, xend >> 8);
    ov5640_write_reg(0X3805, xend & 0XFF);
    ov5640_write_reg(0X3806, yend >> 8);
    ov5640_write_reg(0X3807, yend & 0XFF);
    ov5640_write_reg(0X3212, 0X13);     /* 结束组3 */
    ov5640_write_reg(0X3212, 0Xa3);     /* 启用组3设置 */
    return 0;
}

/**
 * @brief       OV5640 初始化自动对焦
 * @param       无
 * @retval      0, 成功; 1, 失败;
 */
uint8_t ov5640_focus_init(void)
{
    uint16_t i;
    uint16_t addr = 0x8000;
    uint8_t state = 0x8F;
    ov5640_write_reg(0x3000, 0x20); /* reset MCU */

    for (i = 0; i < sizeof(OV5640_AF_Config); i++)
    {
        ov5640_write_reg(addr, OV5640_AF_Config[i]);    /* 发送配置数组 */
        addr++;
    }

    ov5640_write_reg(0x3022, 0x00);
    ov5640_write_reg(0x3023, 0x00);
    ov5640_write_reg(0x3024, 0x00);
    ov5640_write_reg(0x3025, 0x00);
    ov5640_write_reg(0x3026, 0x00);
    ov5640_write_reg(0x3027, 0x00);
    ov5640_write_reg(0x3028, 0x00);
    ov5640_write_reg(0x3029, 0x7f);
    ov5640_write_reg(0x3000, 0x00);
    i = 0;

    do
    {
        state = ov5640_read_reg(0x3029);
        delay_ms(5);
        i++;

        if (i > 1000)return 1;
    } while (state != 0x70);

    return 0;
}

/**
 * @brief       OV5640 执行一次自动对焦
 * @param       无
 * @retval      0, 成功; 1, 失败;
 */
uint8_t ov5640_focus_single(void)
{
    uint8_t temp;
    uint16_t retry = 0;
    ov5640_write_reg(0x3022, 0x03);		/* 触发一次自动对焦 */

    while (1)
    {
        retry++;
        temp = ov5640_read_reg(0x3029);	/* 检查对焦完成状态 */

        if (temp == 0x10)break;		/*  focus completed */

        delay_ms(5);

        if (retry > 1000)return 1;
    }

    return 0;
}

/**
 * @brief       OV5640 持续自动对焦,当失焦后,会自动继续对焦
 * @param       无
 * @retval      0, 成功; 1, 失败;
 */
uint8_t ov5640_focus_constant(void)
{
    uint8_t temp = 0;
    uint16_t retry = 0;
    ov5640_write_reg(0x3023, 0x01);
    ov5640_write_reg(0x3022, 0x08); /* 发送IDLE指令 */

    do
    {
        temp = ov5640_read_reg(0x3023);
        retry++;

        if (retry > 1000)return 2;

        delay_ms(5);
    } while (temp != 0x00);

    ov5640_write_reg(0x3023, 0x01);
    ov5640_write_reg(0x3022, 0x04); /* 发送持续对焦指令 */
    retry = 0;

    do
    {
        temp = ov5640_read_reg(0x3023);
        retry++;

        if (retry > 1000)return 2;

        delay_ms(5);
    } while (temp != 0x00);     /* 0, 对焦完成; 1, 正在对焦; */

    return 0;
}






