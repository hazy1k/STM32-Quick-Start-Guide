/**
 ****************************************************************************************************
 * @file        oled.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-22
 * @brief       OLED 驱动代码
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
 * V1.0 20200322
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "stdlib.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/OLED/oledfont.h"
#include "./SYSTEM/delay/delay.h"

/* 
 * OLED的显存
 * 每个字节表示8个像素, 128,表示有128列, 8表示有64行, 高位表示高行数. 
 * 比如:g_oled_gram[0][0],包含了第一列,第1~8行的数据. g_oled_gram[0][0].0,即表示坐标(0,0)
 * 类似的: g_oled_gram[1][0].1,表示坐标(1,1), g_oled_gram[10][1].2,表示坐标(10,10), 
 * 
 * 存放格式如下(高位表示高行数).
 * [0]0 1 2 3 ... 127
 * [1]0 1 2 3 ... 127
 * [2]0 1 2 3 ... 127
 * [3]0 1 2 3 ... 127
 * [4]0 1 2 3 ... 127
 * [5]0 1 2 3 ... 127
 * [6]0 1 2 3 ... 127
 * [7]0 1 2 3 ... 127
 */
static uint8_t g_oled_gram[128][8];

/**
 * @brief       更新显存到OLED
 * @param       无
 * @retval      无
 */
void oled_refresh_gram(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        oled_wr_byte (0xb0 + i, OLED_CMD); /* 设置页地址（0~7） */
        oled_wr_byte (0x00, OLED_CMD);     /* 设置显示位置—列低地址 */
        oled_wr_byte (0x10, OLED_CMD);     /* 设置显示位置—列高地址 */

        for (n = 0; n < 128; n++)
        {
            oled_wr_byte(g_oled_gram[n][i], OLED_DATA);
        }
    }
}

#if OLED_MODE == 1    /* 使用8080并口驱动OLED */

/**
 * @brief       通过拼凑的方法向OLED输出一个8位数据
 * @param       data: 要输出的数据
 * @retval      无
 */
static void oled_data_out(uint8_t data)
{
    uint16_t dat = data & 0X0F;
    GPIOC->ODR &= ~(0XF << 6);      /* 清空6~9 */
    GPIOC->ODR |= dat << 6;         /* D[3:0]-->PC[9:6] */
    
    GPIOC->ODR &= ~(0X1 << 11);              /* 清空11 */
    GPIOC->ODR |= ((data >> 4) & 0x01) << 11;/* D4 */
    
    GPIOD->ODR &= ~(0X1 << 3);               /* 清空3 */
    GPIOD->ODR |= ((data >> 5) & 0x01) << 3; /* D5 */
    
    GPIOB->ODR &= ~(0X3<<8);                 /* 清空8,9 */
    GPIOB->ODR |= ((data >> 6) & 0x01) << 8; /* D6 */
    GPIOB->ODR |= ((data >> 7) & 0x01) << 9; /* D7 */
}

/**
 * @brief       向OLED写入一个字节
 * @param       data: 要输出的数据
 * @param       cmd: 数据/命令标志 0,表示命令;1,表示数据;
 * @retval      无
 */
static void oled_wr_byte(uint8_t data, uint8_t cmd)
{
    oled_data_out(data);
    OLED_RS(cmd);
    OLED_CS(0);
    OLED_WR(0);
    OLED_WR(1);
    OLED_CS(1);
    OLED_RS(1);
}

#else   /* 使用SPI驱动OLED */

/**
 * @brief       向OLED写入一个字节
 * @param       data: 要输出的数据
 * @param       cmd: 数据/命令标志 0,表示命令;1,表示数据;
 * @retval      无
 */
static void oled_wr_byte(uint8_t data, uint8_t cmd)
{
    uint8_t i;
    OLED_RS(cmd);   /* 写命令 */
    OLED_CS(0);

    for (i = 0; i < 8; i++)
    {
        OLED_SCLK(0);

        if (data & 0x80)
        {
            OLED_SDIN(1);
        }
        else 
        {
            OLED_SDIN(0);
        }
        OLED_SCLK(1);
        data <<= 1;
    }

    OLED_CS(1);
    OLED_RS(1);
}

#endif

/**
 * @brief       开启OLED显示
 * @param       无
 * @retval      无
 */
void oled_display_on(void)
{
    oled_wr_byte(0X8D, OLED_CMD);   /* SET DCDC命令 */
    oled_wr_byte(0X14, OLED_CMD);   /* DCDC ON */
    oled_wr_byte(0XAF, OLED_CMD);   /* DISPLAY ON */
}
 
/**
 * @brief       关闭OLED显示
 * @param       无
 * @retval      无
 */
void oled_display_off(void)
{
    oled_wr_byte(0X8D, OLED_CMD);   /* SET DCDC命令 */
    oled_wr_byte(0X10, OLED_CMD);   /* DCDC OFF */
    oled_wr_byte(0XAE, OLED_CMD);   /* DISPLAY OFF */
}

/**
 * @brief       清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
 * @param       无
 * @retval      无
 */
void oled_clear(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)g_oled_gram[n][i] = 0X00;

    oled_refresh_gram();    /* 更新显示 */
}

/**
 * @brief       OLED画点 
 * @param       x  : 0~127
 * @param       y  : 0~63
 * @param       dot: 1 填充 0,清空
 * @retval      无
 */ 
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63) return;  /* 超出范围了 */ 
    
    pos = y / 8;        /* 计算GRAM里面的y坐标所在的字节, 每个字节可以存储8个行坐标 */
    
    bx = y % 8;         /* 取余数,方便计算y在对应字节里面的位置,及行(y)位置 */
    temp = 1 << bx;     /* 高位表示高行号, 得到y对应的bit位置,将该bit先置1 */

    if (dot)    /* 画实心点 */
    {
        g_oled_gram[x][pos] |= temp;
    }
    else        /* 画空点,即不显示 */
    {
        g_oled_gram[x][pos] &= ~temp;
    }
}

/**
 * @brief       OLED填充区域填充 
 *   @note:     注意:需要确保: x1<=x2; y1<=y2  0<=x1<=127  0<=y1<=63
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标 
 * @param       dot: 1 填充 0,清空
 * @retval      无
 */ 
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)oled_draw_point(x, y, dot);
    }

    oled_refresh_gram();    /* 更新显示 */
}

/**
 * @brief       在指定位置显示一个字符,包括部分字符 
 * @param       x   : 0~127
 * @param       y   : 0~63
 * @param       size: 选择字体 12/16/24
 * @param       mode: 0,反白显示;1,正常显示
 * @retval      无
 */ 
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t *pfont = 0;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';        /* 得到偏移后的值,因为字库是从空格开始存储的,第一个字符是空格 */

    if (size == 12)         /* 调用1206字体 */
    {
        pfont = (uint8_t *)oled_asc2_1206[chr];        
    }
    else if (size == 16)    /* 调用1608字体 */ 
    {
        pfont = (uint8_t *)oled_asc2_1608[chr];
    }
    else if (size == 24)    /* 调用2412字体 */
    {
        pfont = (uint8_t *)oled_asc2_2412[chr];
    }
    else                    /* 没有的字库 */
    {
        return;   
    }
    
    for (t = 0; t < csize; t++)
    { 
        temp = pfont[t];
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)oled_draw_point(x, y, mode);
            else oled_draw_point(x, y, !mode);

            temp <<= 1;
            y++;

            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
 * @brief       平方函数, m^n
 * @param       m: 底数
 * @param       n: 指数 
 * @retval      无
 */
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       显示len个数字 
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24
 * @retval      无
 */  
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)  /* 按总显示位数循环 */ 
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;  /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                oled_show_char(x + (size / 2)*t, y, ' ', size, 1); /* 显示空格,站位 */
                continue;   /* 继续下个一位 */
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }
        }

        oled_show_char(x + (size / 2)*t, y, temp + '0', size, 1);    /* 显示字符 */
    }
} 

/**
 * @brief       显示字符串 
 * @param       x,y : 起始坐标
 * @param       size: 选择字体 12/16/24
 * @param       *p  : 字符串指针,指向字符串首地址 
 * @retval      无
 */ 
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))  /* 判断是不是非法字符! */
    {
        if (x > (128 - (size / 2)))     /* 宽度越界 */
        {
            x = 0;
            y += size;      /* 换行 */
        }

        if (y > (64 - size))/* 高度越界 */
        {
            y = x = 0;
            oled_clear();  
        }

        oled_show_char(x, y, *p, size, 1);   /* 显示一个字符 */
        x += size / 2;      /* ASCII字符宽度为汉字宽度的一半 */
        p++;
    }
}

/**
 * @brief       初始化OLED(SSD1306)
 * @param       无
 * @retval      无
 */
void oled_init(void)
{
    GPIO_InitTypeDef  gpio_init_struct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

#if OLED_MODE==1         /* 使用8080并口模式 */

    gpio_init_struct.Pin=GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8; /* PA4,6,7,8设置 */
    gpio_init_struct.Mode=GPIO_MODE_OUTPUT_PP;          /* 推挽输出 */
    gpio_init_struct.Pull=GPIO_PULLUP;                  /* 上拉 */
    gpio_init_struct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   /* 高速 */
    HAL_GPIO_Init(GPIOA,&gpio_init_struct);

    /* PB7,8,9,10,11设置 */
    gpio_init_struct.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB,&gpio_init_struct);

    /* PC4,6~9,PC11设置 */
    gpio_init_struct.Pin=GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC,&gpio_init_struct);

    /* PD3 设置 */
    gpio_init_struct.Pin=GPIO_PIN_3;
    HAL_GPIO_Init(GPIOD,&gpio_init_struct);

    OLED_WR(1);
    OLED_RD(1);

#else               /* 使用4线SPI 串口模式 */

    gpio_init_struct.Pin=OLED_SPI_RST_PIN;
    gpio_init_struct.Mode=GPIO_MODE_OUTPUT_PP;              /* 推挽输出 */
    gpio_init_struct.Pull=GPIO_PULLUP;                      /* 上拉 */
    gpio_init_struct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;       /* 高速 */
    HAL_GPIO_Init(OLED_SPI_RST_PORT,&gpio_init_struct);     /* RST引脚模式设置 */

    gpio_init_struct.Pin=OLED_SPI_CS_PIN;
    HAL_GPIO_Init(OLED_SPI_CS_PORT,&gpio_init_struct);      /* CS引脚模式设置 */

    gpio_init_struct.Pin=OLED_SPI_RS_PIN;
    HAL_GPIO_Init(OLED_SPI_RS_PORT,&gpio_init_struct);      /* RS引脚模式设置 */

    gpio_init_struct.Pin=OLED_SPI_SCLK_PIN;
    HAL_GPIO_Init(OLED_SPI_SCLK_PORT,&gpio_init_struct);    /* SCLK引脚模式设置 */
    
    gpio_init_struct.Pin=OLED_SPI_SDIN_PIN;
    HAL_GPIO_Init(OLED_SPI_SDIN_PORT,&gpio_init_struct);    /* SDIN引脚模式设置 */
    
    OLED_SDIN(1);
    OLED_SCLK(1);
#endif
    OLED_CS(1);
    OLED_RS(1);

    OLED_RST(0);
    delay_ms(100);
    OLED_RST(1);

    oled_wr_byte(0xAE, OLED_CMD);   /* 关闭显示 */
    oled_wr_byte(0xD5, OLED_CMD);   /* 设置时钟分频因子,震荡频率 */
    oled_wr_byte(80, OLED_CMD);     /* [3:0],分频因子;[7:4],震荡频率 */
    oled_wr_byte(0xA8, OLED_CMD);   /* 设置驱动路数 */
    oled_wr_byte(0X3F, OLED_CMD);   /* 默认0X3F(1/64) */
    oled_wr_byte(0xD3, OLED_CMD);   /* 设置显示偏移 */
    oled_wr_byte(0X00, OLED_CMD);   /* 默认为0 */

    oled_wr_byte(0x40, OLED_CMD);   /* 设置显示开始行 [5:0],行数. */

    oled_wr_byte(0x8D, OLED_CMD);   /* 电荷泵设置 */
    oled_wr_byte(0x14, OLED_CMD);   /* bit2，开启/关闭 */
    oled_wr_byte(0x20, OLED_CMD);   /* 设置内存地址模式 */
    oled_wr_byte(0x02, OLED_CMD);   /* [1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10; */
    oled_wr_byte(0xA1, OLED_CMD);   /* 段重定义设置,bit0:0,0->0;1,0->127; */
    oled_wr_byte(0xC8, OLED_CMD);   /* 设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数 */
    oled_wr_byte(0xDA, OLED_CMD);   /* 设置COM硬件引脚配置 */
    oled_wr_byte(0x12, OLED_CMD);   /* [5:4]配置 */

    oled_wr_byte(0x81, OLED_CMD);   /* 对比度设置 */
    oled_wr_byte(0xEF, OLED_CMD);   /* 1~255;默认0X7F (亮度设置,越大越亮) */
    oled_wr_byte(0xD9, OLED_CMD);   /* 设置预充电周期 */
    oled_wr_byte(0xf1, OLED_CMD);   /* [3:0],PHASE 1;[7:4],PHASE 2; */
    oled_wr_byte(0xDB, OLED_CMD);   /* 设置VCOMH 电压倍率 */
    oled_wr_byte(0x30, OLED_CMD);   /* [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc; */

    oled_wr_byte(0xA4, OLED_CMD);   /* 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏) */
    oled_wr_byte(0xA6, OLED_CMD);   /* 设置显示方式;bit0:1,反相显示;0,正常显示 */
    oled_wr_byte(0xAF, OLED_CMD);   /* 开启显示 */
    oled_clear();
}  
