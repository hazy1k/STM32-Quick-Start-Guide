/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-1-16
 * @brief       最精简STM32工程, 除了启动文件(.s文件), 未使用任何库文件
 *              该代码实现功能：通过PB4控制LED0闪烁
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
 * V1.0 20200116
 * 第一次发布
 *
 ****************************************************************************************************
 */


/* 总线基地址定义 */
#define PERIPH_BASE         0x40000000                          /* 外设基地址 */

#define D2_APB1PERIPH_BASE  PERIPH_BASE                         /* APB1总线基地址 */
#define D3_AHB1PERIPH_BASE  (PERIPH_BASE + 0x18020000UL)        /* AHB4总线基地址 */

/* 外设基地址定义 */
#define RCC_BASE            (D3_AHB1PERIPH_BASE + 0x4400UL)     /* RCC基地址 */
#define GPIOB_BASE          (D3_AHB1PERIPH_BASE + 0x0400UL)     /* GPIOB基地址 */


/* 外设相关寄存器映射(定义) */
#define RCC_AHB4ENR         *(volatile unsigned int *)(RCC_BASE + 0xE0)     /* RCC_AHB4ENR寄存器映射 */
    
#define GPIOB_MODER         *(volatile unsigned int *)(GPIOB_BASE + 0x00)   /* GPIOB_MODER寄存器映射 */
#define GPIOB_OTYPER        *(volatile unsigned int *)(GPIOB_BASE + 0x04)   /* GPIOB_OTYPER寄存器映射 */
#define GPIOB_OSPEEDR       *(volatile unsigned int *)(GPIOB_BASE + 0x08)   /* GPIOB_OSPEEDR寄存器映射 */
#define GPIOB_PUPDR         *(volatile unsigned int *)(GPIOB_BASE + 0x0C)   /* GPIOB_PUPDR寄存器映射 */
#define GPIOB_IDR           *(volatile unsigned int *)(GPIOB_BASE + 0x10)   /* GPIOB_IDR寄存器映射 */
#define GPIOB_ODR           *(volatile unsigned int *)(GPIOB_BASE + 0x14)   /* GPIOB_ODR寄存器映射 */


/* 延时函数 */
static void delay_x(volatile unsigned int t)
{
    while(t--);
}

/* main函数 */
int main(void)
{
    /* 未执行任何PLL时钟配置, 默认使用HSI(64M)工作, 相当于工作在主频64Mhz频率下 */

    RCC_AHB4ENR |= 1 << 1;           /* GPIOB 时钟使能 */
    GPIOB_MODER &= ~(0X03UL << 8);   /* MODER4[1:0], 清零 */
    GPIOB_MODER |= 0X01UL << 8;      /* MODER4[1:0]=1, PB4输出模式 */
    GPIOB_OTYPER &= ~(0X01UL << 4);  /* OT4, 清零, 推挽输出 */
    GPIOB_OSPEEDR &= ~(0X03UL << 8); /* OSPEEDR4[1:0], 清零 */
    GPIOB_OSPEEDR |= 0X01UL << 8;    /* OSPEEDR4[1:0]=1, 中速 */
    GPIOB_PUPDR &= ~(0X03UL << 8);   /* PUPDR4[1:0], 清零 */
    GPIOB_PUPDR |= 0X01UL << 8;      /* PUPDR4[1:0]=1, 上拉 */

    while(1)
    {
        GPIOB_ODR |= 1 << 4;        /* PB4 = 1, LED0灭 */
        delay_x(5000000);           /* 延时一定时间 */
        GPIOB_ODR &= ~(1UL << 4);   /* PB4 = 0, LED0亮 */
        delay_x(5000000);           /* 延时一定时间 */
    }
}











