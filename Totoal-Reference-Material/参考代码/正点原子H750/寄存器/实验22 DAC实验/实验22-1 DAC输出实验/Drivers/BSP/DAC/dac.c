/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       DAC 驱动代码
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
 * V1.0 20200328
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       DAC初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=120Mhz=8.3ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 2us (H750数据手册有写)
 *              因此DAC输出的最高速度约为:500Khz, 以10个点为一个周期, 最大能输出50Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint8_t outx)
{
    uint8_t offset = (outx - 1) * 16;   /* OUT1 / OUT2在DAC寄存器里面的偏移量(OUT1 = 0, OUT2 = 16) */

    RCC->AHB4ENR |= 1 << 0;;        /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    RCC->APB1LENR |= 1 << 29;       /* 使能DAC1的时钟 */

    sys_gpio_set(GPIOA, 1 << (4 + outx),/* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* DAC引脚模式设置,模拟输入 */

    DAC1->CCR &= ~(7 << offset);    /* OTRIM1/2[4:0]清零, 偏移调整值清零 */
    DAC1->MCR &= ~(7 << offset);    /* MODE1/2[2:0]清零 */
    DAC1->MCR |= 2 << offset;       /* MODE1/2[2:0]=2, DAC通道1/2连接到外部引脚,关闭BUFFER */

    DAC1->CR = 0;                   /* DR寄存器清零 */
    DAC1->CR |= 0 << (1 + offset);  /* TEN1/2=0,不使用触发功能 */
    DAC1->CR |= 0 << (2 + offset);  /* TSEL1/2[3:0]=0,软件触发 */
    DAC1->CR |= 0 << (6 + offset);  /* WAVE1/2[1:0]=0,不使用波形发生 */
    DAC1->CR |= 0 << (12 + offset); /* DMAEN1/2=0,DAC1/2 DMA不使能 */
    DAC1->CR |= 0 << (14 + offset); /* CEN1/2=0,DAC1/2工作在普通模式 */

    DAC1->CR |= 1 << (0 + offset);  /* 使能DAC1_OUT1/2 */
    DAC1->DHR12R1 = 0;  /* 通道1输出 0 */
    DAC1->DHR12R2 = 0;  /* 通道2输出 0 */
}

/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* 如果值大于等于4096, 则取4095 */

    if (outx == 1)   /* 通道1 */
    {
        DAC1->DHR12R1 = temp;
    }
    else            /* 通道2 */
    {
        DAC1->DHR12R2 = temp;
    }
}





















































