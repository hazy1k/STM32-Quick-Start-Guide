/**
 ****************************************************************************************************
 * @file        adc3.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       ADC3(开启内部温度传感器) 驱动代码
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
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc3.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ADC3初始化函数
 *   @note      本函数专用于支持ADC3, 和ADC1/2区分开来, 方便使用
 *              我们使用16位精度, ADC采样时钟=32M, 转换时间为:采样周期 + 8.5个ADC周期
 *              设置最大采样周期: 810.5, 则转换时间 = 819个ADC周期 = 25.6us
 * @param       无
 * @retval      无
 */
void adc3_init(void)
{
    RCC->AHB4ENR |= 1 << 24;        /* 使能ADC3时钟 */

    RCC->AHB4RSTR |= 1 << 24;       /* ADC3复位 */
    RCC->AHB4RSTR &= ~(1 << 24);    /* 复位结束 */

    RCC->D3CCIPR &= ~(3 << 16);     /* ADCSEL[1:0]清零 */
    RCC->D3CCIPR |= 2 << 16;        /* ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz */
    ADC3_COMMON->CCR |= 0 << 18;    /* PRESC[3:0]=0,输入时钟不分频,即adc_ker_ck=per_ck=64Mhz,ADC采样时钟=输入时钟/2=32M */

    ADC3_COMMON->CCR |= 1 << 23;    /* VSENSEEN=1,使能内部温度传感器通道 */

    ADC3->CR = 0;                   /* CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒. */
    ADC3->CR |= 1 << 28;            /* ADVREGEN=1,使能ADC稳压器 */

    delay_ms(10);                   /* 等待稳压器启动完成,约10us,这里延时大一点,没关系. */

    ADC3->CR |= 3 << 8;             /* BOOST[1:0]=11,ADC工作在boost模式3(25M≤ADC频率≤50M) */
    ADC3->CFGR &= ~(1 << 13);       /* CONT=0,单次转换模式 */
    ADC3->CFGR |= 1 << 12;          /* OVRMOD=1,复写模式(DR寄存器可被复写) */
    ADC3->CFGR &= ~(3 << 10);       /* EXTEN[1:0]=0,软件触发 */
    ADC3->CFGR &= ~(7 << 2);        /* RES[2:0]位清零 */
    ADC3->CFGR |= 0 << 2;           /* RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.) */

    ADC3->CFGR2 &= ~((uint32_t)15 << 28);   /* LSHIFT[3:0]=0,不左移,数据右对齐. */
    ADC3->CFGR2 &= ~((uint32_t)0X3FF << 16);/* OSR[9:0]=0,不使用过采样 */


    ADC3->CR &= ~((uint32_t)1 << 30);/* ADCALDIF=0,校准单端转换通道 */
    ADC3->CR |= 1 << 16;            /* ADCALLIN=1,线性校准 */
    ADC3->CR |= (uint32_t)1 << 31;  /* 开启校准 */

    while (ADC3->CR & ((uint32_t)1 << 31)); /* 等待校准完成 */

    ADC3->SQR1 &= ~(0XF << 0);      /* L[3:0]清零 */
    ADC3->SQR1 |= 0 << 0;           /* L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 */

    ADC3->CR |= 1 << 0; /* 开启AD转换器 */
}

/**
 * @brief       设置ADC3通道采样时间
 * @param       ch   : 通道号, 0~19
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       000, 1.5个ADC时钟周期        001, 2.5个ADC时钟周期
 *   @arg       010, 8.5个ADC时钟周期        011, 16.5个ADC时钟周期
 *   @arg       100, 32.5个ADC时钟周期       101, 64.5个ADC时钟周期
 *   @arg       110, 387.5个ADC时钟周期      111, 810.5个ADC时钟周期
 * @retval      无
 */
void adc3_channel_set(uint8_t ch, uint8_t stime)
{
    if (ch < 10)              /* 通道0~9,使用SMPR1配置 */
    {
        ADC3->SMPR1 &= ~(7 << (3 * ch));        /* 通道ch 采样时间清空 */
        ADC3->SMPR1 |= 7 << (3 * ch);           /* 通道ch 采样周期设置,周期越高精度越高 */
    }
    else     /* 通道10~19,使用SMPR2配置 */
    {
        ADC3->SMPR2 &= ~(7 << (3 * (ch - 10))); /* 通道ch 采样时间清空 */
        ADC3->SMPR2 |= 7 << (3 * (ch - 10));    /* 通道ch 采样周期设置,周期越高精度越高 */
    }
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道号, 0~19
 * @retval      无
 */
uint32_t adc3_get_result(uint8_t ch)
{
    adc3_channel_set(ch, 7);        /* 设置ADCX对应通道采样时间为810.5个时钟周期 */

    ADC3->PCSEL |= 1 << ch;         /* ADC转换通道预选择 */
    /* 设置转换序列 */
    ADC3->SQR1 &= ~(0X1F << 6 * 1); /* 规则序列1通道清零 */
    ADC3->SQR1 |= ch << 6 * 1;      /* 设置规则序列1的转换通道为ch */
    ADC3->CR |= 1 << 2;             /* 启动规则转换通道 */

    while (!(ADC3->ISR & 1 << 2));  /* 等待转换结束 */

    return ADC3->DR;                /* 返回adc值 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~19
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc3_get_result_average(uint8_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)   /* 获取times次数据 */
    {
        temp_val += adc3_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* 返回平均值 */
}

/**
 * @brief       获取内部温度传感器温度值
 * @param       无
 * @retval      温度值(扩大了100倍,单位:℃.)
 */
short adc3_get_temperature(void)
{
    uint32_t adcx;
    short result;
    double temperature;
    float temp = 0;
    uint16_t ts_cal1, ts_cal2;
    
    ts_cal1 = *(volatile uint16_t *)(0X1FF1E820);   /* 获取TS_CAL1 */
    ts_cal2 = *(volatile uint16_t *)(0X1FF1E840);   /* 获取TS_CAL2 */
    temp = (float)((110.0 - 30.0) / (ts_cal2 - ts_cal1));   /* 获取比例因子 */
    
    adcx = adc3_get_result_average(ADC3_TEMPSENSOR_CHX, 10);/* 读取内部温度传感器通道,10次取平均 */
    temperature = (float)(temp * (adcx - ts_cal1) + 30);    /* 计算温度 */
    
    result = temperature *= 100;  /* 扩大100倍. */
    return result;

}



















