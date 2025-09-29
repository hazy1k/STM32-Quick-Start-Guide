/**
 ****************************************************************************************************
 * @file        ds18b20.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-2
 * @brief       DS18B20数字温度传感器 驱动代码
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
 * V1.0 20200402
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/delay/delay.h"
#include "./BSP/DS18B20/ds18b20.h"



/**
 * @brief       复位DS18B20
 * @param       data: 要写入的数据
 * @retval      无
 */
static void ds18b20_reset(void)
{
    DS18B20_DQ_OUT(0);  /* 拉低DQ,复位 */
    delay_us(750);      /* 拉低750us */
    DS18B20_DQ_OUT(1);  /* DQ=1, 释放复位 */
    delay_us(15);       /* 延迟15US */
}

/**
 * @brief       等待DS18B20的回应
 * @param       无
 * @retval      0, DS18B20正常
 *              1, DS18B20异常/不存在
 */
uint8_t ds18b20_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DS18B20_DQ_IN && retry < 200)    /* 等待DQ变低, 等待200us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 200)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DS18B20_DQ_IN && retry < 240)   /* 等待DQ变高, 等待240us */
        {
            retry++;
            delay_us(1);
        }

        if (retry >= 240) rval = 1;
    }

    return rval;
}

/**
 * @brief       从DS18B20读取一个位
 * @param       无
 * @retval      读取到的位值: 0 / 1
 */
static uint8_t ds18b20_read_bit(void)
{
    uint8_t data = 0;
    DS18B20_DQ_OUT(0);
    delay_us(2);
    DS18B20_DQ_OUT(1);
    delay_us(12);

    if (DS18B20_DQ_IN)
    {
        data = 1;
    }

    delay_us(50);
    return data;
}

/**
 * @brief       从DS18B20读取一个字节
 * @param       无
 * @retval      读到的数据
 */
static uint8_t ds18b20_read_byte(void)
{
    uint8_t i, b, data = 0;

    for (i = 0; i < 8; i++)
    {
        b = ds18b20_read_bit(); /* DS18B20先输出低位数据 ,高位数据后输出 */
        
        data |= b << i;         /* 填充data的每一位 */ 
    }

    return data;
}

/**
 * @brief       写一个字节到DS18B20
 * @param       data: 要写入的字节
 * @retval      无
 */
static void ds18b20_write_byte(uint8_t data)
{
    uint8_t j;

    for (j = 1; j <= 8; j++)
    {
        if (data & 0x01)
        {
            DS18B20_DQ_OUT(0);  /*  Write 1 */
            delay_us(2);
            DS18B20_DQ_OUT(1);
            delay_us(60);
        }
        else
        {
            DS18B20_DQ_OUT(0);  /*  Write 0 */
            delay_us(60);
            DS18B20_DQ_OUT(1);
            delay_us(2);
        }

        data >>= 1;             /* 右移,获取高一位数据 */
    }
}

/**
 * @brief       开始温度转换
 * @param       无
 * @retval      无
 */
static void ds18b20_start(void)
{
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0x44);   /*  convert */
}

/**
 * @brief       初始化DS18B20的IO口 DQ 同时检测DS18B20的存在
 * @param       无
 * @retval      0, 正常
 *              1, 不存在/不正常
 */
uint8_t ds18b20_init(void)
{
    DS18B20_DQ_GPIO_CLK_ENABLE();   /* 开启DQ引脚时钟 */

    /* DS18B20_DQ引脚模式设置,开漏输出,上拉, 这样就不用再设置IO方向了, 开漏输出的时候(=1), 也可以读取外部信号的高低电平 */
    sys_gpio_set(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_OD, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    ds18b20_reset();
    return ds18b20_check();
}

/**
 * @brief       从ds18b20得到温度值(精度：0.1C)
 * @param       无
 * @retval      温度值 （-550~1250）
 *   @note      返回的温度值放大了10倍.
 *              实际使用的时候,要除以10才是实际温度.
 */
short ds18b20_get_temperature(void)
{
    uint8_t flag = 1;           /* 默认温度为正数 */
    uint8_t TL, TH;
    short temp;
    
    ds18b20_start();            /*  ds1820 start convert */
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0xbe);   /*  convert */
    TL = ds18b20_read_byte();   /*  LSB */
    TH = ds18b20_read_byte();   /*  MSB */

    if (TH > 7)
    {
        /* 温度为负，查看DS18B20的温度表示法与计算机存储正负数据的原理一致：
           正数补码为寄存器存储的数据自身，负数补码为寄存器存储值按位取反后+1
           所以我们直接取它实际的负数部分，但负数的补码为取反后加一，但考虑到低位可能+1后有进位和代码冗余，
           我们这里先暂时没有作+1的处理，这里需要留意 */
        TH = ~TH;
        TL = ~TL;
        flag = 0;   /* 温度为负 */
    }

    temp = TH;      /* 获得高八位 */
    temp <<= 8;
    temp += TL;     /* 获得底八位 */

    /* 转换成实际温度 */
    if (flag == 0)
    {
        /* 将温度转换成负温度，这里的+1参考前面的说明 */
        temp = (double)(temp + 1) * 0.625;
        temp = -temp;
    }
    else
    {
        temp = (double)temp * 0.625;
    }
    
    return temp;
}
















