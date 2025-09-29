/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-26
 * @brief       ADC 驱动代码
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
 * V1.0 20200326
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道,但是不支持ADC3
 *              我们使用16位精度, ADC采样时钟=32M, 转换时间为:采样周期 + 8.5个ADC周期
 *              设置最大采样周期: 810.5, 则转换时间 = 819个ADC周期 = 25.6us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* IO口时钟使能 */
    ADC_ADCX_CHY_CLK_ENABLE();      /* ADC时钟使能 */
    
    sys_gpio_set(ADC_ADCX_CHY_GPIO_PORT, ADC_ADCX_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */
    
    RCC->AHB1RSTR |= 1 << 5;        /* ADC1/2复位 */
    RCC->AHB1RSTR &= ~(1 << 5);     /* 复位结束 */

    RCC->D3CCIPR &= ~(3 << 16);     /* ADCSEL[1:0]清零 */
    RCC->D3CCIPR |= 2 << 16;        /* ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz */
    ADC12_COMMON->CCR |= 1 << 18;   /* PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck=64Mhz,ADC采样时钟=输入时钟/2=32M */

    ADC_ADCX->CR = 0;               /* CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒. */
    ADC_ADCX->CR |= 1 << 28;        /* ADVREGEN=1,使能ADC稳压器 */
    
    delay_ms(10);                   /* 等待稳压器启动完成,约10us,这里延时大一点,没关系. */
    
    ADC_ADCX->CR |= 3 << 8;         /* BOOST[1:0]=11,ADC工作在boost模式3(25M≤ADC频率≤50M) */
    ADC_ADCX->CFGR &= ~(1 << 13);   /* CONT=0,单次转换模式 */
    ADC_ADCX->CFGR |= 1 << 12;      /* OVRMOD=1,复写模式(DR寄存器可被复写) */
    ADC_ADCX->CFGR &= ~(3 << 10);   /* EXTEN[1:0]=0,软件触发 */
    ADC_ADCX->CFGR &= ~(7 << 2);    /* RES[2:0]位清零 */
    ADC_ADCX->CFGR |= 0 << 2;       /* RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.) */

    ADC_ADCX->CFGR2 &= ~((uint32_t)15 << 28);   /* LSHIFT[3:0]=0,不左移,数据右对齐. */
    ADC_ADCX->CFGR2 &= ~((uint32_t)0X3FF << 16);/* OSR[9:0]=0,不使用过采样 */


    ADC_ADCX->CR &= ~((uint32_t)1 << 30);   /* ADCALDIF=0,校准单端转换通道 */
    ADC_ADCX->CR |= 1 << 16;            /* ADCALLIN=1,线性校准 */
    ADC_ADCX->CR |= (uint32_t)1 << 31;  /* 开启校准 */

    while (ADC_ADCX->CR & ((uint32_t)1 << 31)); /* 等待校准完成 */

    ADC_ADCX->SQR1 &= ~(0XF << 0);  /* L[3:0]清零 */
    ADC_ADCX->SQR1 |= 0 << 0;       /* L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 */
 
    ADC_ADCX->CR |= 1 << 0; /* 开启AD转换器 */
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc结构体指针, ADC1 / ADC2
 * @param       ch   : 通道号, 0~19
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       000, 1.5个ADC时钟周期        001, 2.5个ADC时钟周期
 *   @arg       010, 8.5个ADC时钟周期        011, 16.5个ADC时钟周期
 *   @arg       100, 32.5个ADC时钟周期       101, 64.5个ADC时钟周期
 *   @arg       110, 387.5个ADC时钟周期      111, 810.5个ADC时钟周期 
 * @retval      无
 */
void adc_channel_set(ADC_TypeDef *adcx, uint8_t ch, uint8_t stime)
{
    if (ch < 10)              /* 通道0~9,使用SMPR1配置 */
    { 
        adcx->SMPR1 &= ~(7 << (3 * ch));        /* 通道ch 采样时间清空 */
        adcx->SMPR1 |= 7 << (3 * ch);           /* 通道ch 采样周期设置,周期越高精度越高 */
    }
    else    /* 通道10~19,使用SMPR2配置 */
    { 
        adcx->SMPR2 &= ~(7 << (3 * (ch - 10))); /* 通道ch 采样时间清空 */
        adcx->SMPR2 |= 7 << (3 * (ch - 10));    /* 通道ch 采样周期设置,周期越高精度越高 */
    } 
}

/**
 * @brief       获得ADC转换后的结果 
 * @param       ch: 通道号, 0~19
 * @retval      无
 */
uint32_t adc_get_result(uint8_t ch)
{
    adc_channel_set(ADC_ADCX, ch, 7);   /* 设置ADCX对应通道采样时间为810.5个时钟周期 */
    
    ADC_ADCX->PCSEL |= 1 << ch;         /* ADC转换通道预选择 */
    /* 设置转换序列 */
    ADC_ADCX->SQR1 &= ~(0X1F << 6 * 1); /* 规则序列1通道清零 */
    ADC_ADCX->SQR1 |= ch << 6 * 1;      /* 设置规则序列1的转换通道为ch */
    ADC_ADCX->CR |= 1 << 2;             /* 启动规则转换通道 */

    while (!(ADC_ADCX->ISR & 1 << 2));  /* 等待转换结束 */

    return ADC_ADCX->DR;                /* 返回adc值 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~19
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint8_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++) /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* 返回平均值 */
}









