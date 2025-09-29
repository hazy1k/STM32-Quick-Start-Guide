/**
 ****************************************************************************************************
 * @file        sccb.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       SCCB 驱动代码
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

#include "./BSP/OV5640/sccb.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief       初始化SCCB
 * @param       无
 * @retval      无
 */
void sccb_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    SCCB_SCL_GPIO_CLK_ENABLE();     /* SCL引脚时钟使能 */
    SCCB_SDA_GPIO_CLK_ENABLE();     /* SDA引脚时钟使能 */

    gpio_init_struct.Pin = SCCB_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;            /* 开漏输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 高速 */
    HAL_GPIO_Init(SCCB_SCL_GPIO_PORT, &gpio_init_struct);   /* 初始化SCL引脚 */

    gpio_init_struct.Pin = SCCB_SDA_GPIO_PIN;
    HAL_GPIO_Init(SCCB_SDA_GPIO_PORT, &gpio_init_struct);   /* 初始化SDA引脚 */
    /* SDA引脚模式设置,开漏输出,上拉, 这样就不用再设置IO方向了, 开漏输出的时候(=1), 也可以读取外部信号的高低电平 */

    sccb_stop();     /* 停止总线上所有设备 */
}

/**
 * @brief       SCCB延时函数,用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static void sccb_delay(void)
{
    delay_us(5);
}

/**
 * @brief       产生SCCB起始信号
 * @param       无
 * @retval      无
 */
void sccb_start(void)
{
    SCCB_SDA(1);
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(0);    /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
    sccb_delay();
    SCCB_SCL(0);    /* 钳住SCCB总线，准备发送或接收数据 */
    sccb_delay();
}

/**
 * @brief       产生SCCB停止信号
 * @param       无
 * @retval      无
 */
void sccb_stop(void)
{
    SCCB_SDA(0);    /* STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号 */
    sccb_delay();
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(1);    /* 发送SCCB总线结束信号 */
    sccb_delay();
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void sccb_nack(void)
{
    SCCB_SDA(1);    /* SCL 0 -> 1  时 SDA = 1,表示不应答 */
    sccb_delay();
    SCCB_SCL(1);    /* 产生一个时钟 */
    sccb_delay();
    SCCB_SCL(0);
    sccb_delay();
}

/**
 * @brief       SCCB 发送一个字节
 * @param       data: 要发送的数据
 * @retval      无
 */
uint8_t sccb_send_byte(uint8_t data)
{
    uint8_t t, res;
    
    for (t = 0; t < 8; t++)
    {
        SCCB_SDA((data & 0x80) >> 7);   /* 高位先发送 */
        sccb_delay();
        SCCB_SCL(1);
        sccb_delay();
        SCCB_SCL(0);
        data <<= 1;     /* 左移1位,用于下一次发送 */
    }
    SCCB_SDA(1);        /* 发送完成, 主机释放SDA线 */
    sccb_delay();
    SCCB_SCL(1);        /* 接收第九位,以判断是否发送成功 */
    sccb_delay();

    if (SCCB_READ_SDA)
    {
        res = 1;        /* SDA=1发送失败，返回1 */
    }
    else 
    {
        res = 0;        /* SDA=0发送成功，返回0 */
    } 

    SCCB_SCL(0);
    return res;
}

/**
 * @brief       SCCB 读取一个字节
 * @param       无
 * @retval      读取到的数据
 */
uint8_t sccb_read_byte(void)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* 接收1个字节数据 */
    {
        receive <<= 1;          /* 高位先输出,所以先收到的数据位要左移 */
        SCCB_SCL(1);
        sccb_delay();

        if (SCCB_READ_SDA)
        {
            receive++;
        }
        
        SCCB_SCL(0);
        sccb_delay();
    }
    return receive;
}


















