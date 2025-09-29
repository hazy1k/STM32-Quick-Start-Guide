/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
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
 * V1.1 20200326
 * 1,支持ADC单通道DMA采集 
 * 2,新增adc_dma_init和adc_dma_enable函数.
 * V1.2 20200327
 * 1,支持ADC多通道DMA采集 
 * 2,新增adc_nch_dma_init函数.
 * V1.3 20200327
 * 1,支持ADC单通道过采样
 * 2,新增adc_oversample_init函数.
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


uint8_t g_adc_dma_sta = 0;  /* DMA传输状态标志, 0,未完成; 1, 已完成 */

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
    ADC12_COMMON->CCR |= 0 << 18;   /* PRESC[3:0]=0,输入时钟不分频,即adc_ker_ck=per_ck=64Mhz,ADC采样时钟=输入时钟/2=32M */

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

    ADC_ADCX->SQR1 &= ~(0XF << 0);      /* L[3:0]清零 */
    ADC_ADCX->SQR1 |= 0 << 0;           /* L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 */

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

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无 
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    if (ADC_ADCX_DMASx_IS_TC())
    {
        g_adc_dma_sta = 1;          /* 标记DMA传输完成 */
        ADC_ADCX_DMASx_CLR_TC();    /* 清除DMA1 数据流7 传输完成中断 */
    }
}

/**
 * @brief       ADC DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 * @param       ADC_ADCX_DMASx : DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param       ch         :  DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址 
 * @retval      无
 */
void adc_dma_init(uint32_t par, uint32_t mar)
{
    adc_init(); /* 先初始化ADC */

    adc_channel_set(ADC_ADCX, ADC_ADCX_CHY, 7); /* 设置ADCX对应通道采样时间为810.5个时钟周期 */
    
    /* 通道预选择 */
    ADC_ADCX->PCSEL |= 1 << ADC_ADCX_CHY;       /* ADC转换通道预选择 */
    
    /* 设置转换序列 */
    ADC_ADCX->SQR1 &= ~(0X1F << 6 * 1);         /* 规则序列1通道清零 */
    ADC_ADCX->SQR1 |= ADC_ADCX_CHY << 6 * 1;    /* 设置规则序列1的转换通道为ch */

    /* 配置ADC连续转换, DMA单次传输ADC数据 */
    ADC_ADCX->CFGR |= 1 << 0;   /* DMNGT[1:0] = 01, DMA单次传输ADC数据 */
    ADC_ADCX->CFGR |= 1 << 13;  /* CONT = 1, 连续转换模式 */
   
    /* DMA相关配置 */
    dma_mux_init(ADC_ADCX_DMASx, ADC_ADCX_DMASx_REQ);  /* 初始化DMA 请求复用器 */    
    ADC_ADCX_DMASx->PAR = par;      /* DMA外设地址 */
    ADC_ADCX_DMASx->M0AR = mar;     /* DMA 存储器0地址 */
    ADC_ADCX_DMASx->NDTR = 0;       /* DMA 存储器0地址 */
    ADC_ADCX_DMASx->CR = 0;         /* 先全部复位CR寄存器值 */

    ADC_ADCX_DMASx->CR |= 0 << 6;   /* 外设到存储器模式 */
    ADC_ADCX_DMASx->CR |= 0 << 8;   /* 非循环模式(即使用普通模式) */
    ADC_ADCX_DMASx->CR |= 0 << 9;   /* 外设非增量模式 */
    ADC_ADCX_DMASx->CR |= 1 << 10;  /* 存储器增量模式 */
    ADC_ADCX_DMASx->CR |= 1 << 11;  /* 外设数据长度:16位 */
    ADC_ADCX_DMASx->CR |= 1 << 13;  /* 存储器数据长度:16位 */
    ADC_ADCX_DMASx->CR |= 1 << 16;  /* 中等优先级 */
    ADC_ADCX_DMASx->CR |= 0 << 21;  /* 外设突发单次传输 */
    ADC_ADCX_DMASx->CR |= 0 << 23;  /* 存储器突发单次传输 */
    
    ADC_ADCX_DMASx->CR |= 1 << 4;   /* TCIE = 1, DMA传输完成中断使能 */ 
    
    sys_nvic_init(3, 3, ADC_ADCX_DMASx_IRQn, 2);  /* 组2，最低优先级 */

}

/**
 * @brief       使能一次ADC DMA传输 
 * @param       ndtr: DMA传输的次数
 * @retval      无
 */
void adc_dma_enable(uint16_t ndtr)
{
    ADC_ADCX->CR &= ~(1 << 0);              /* 先关闭ADC */

    dma_enable(ADC_ADCX_DMASx, ndtr);       /* 重新使能DMA传输 */

    ADC_ADCX->CR |= 1 << 0;                 /* 重新启动ADC */
    ADC_ADCX->CR |= 1 << 2;                 /* 启动规则转换通道 */
}

/**
 * @brief       ADC N通道(6通道) DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 *              另外,由于本函数用到了6个通道, 宏定义会比较多内容, 因此,本函数就不采用宏定义的方式来修改通道了,
 *              直接在本函数里面修改, 这里我们默认使用PA0~PA5这6个通道.
 *
 *              注意: 本函数还是使用 ADC_ADCX(默认=ADC1) 和 ADC_ADCX_DMASx(默认=DMA1_Stream7) 及其相关定义
 *              不要乱修改adc.h里面的这两部分内容, 必须在理解原理的基础上进行修改, 否则可能导致无法正常使用.
 *
 * @param       ADC_ADCX_DMASx : DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param       ch         :  DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址 
 * @retval      无
 */
void adc_nch_dma_init(uint32_t par, uint32_t mar)
{
    adc_init(); /* 先初始化ADC */

    /* 配置ADC连续转换, DMA单次传输ADC数据 */
    ADC_ADCX->CFGR |= 1 << 0;       /* DMNGT[1:0] = 01, DMA单次传输ADC数据 */
    ADC_ADCX->CFGR |= 1 << 13;      /* CONT = 1, 连续转换模式 */
   
    ADC_ADCX->SQR1 = 0;             /* SQR1清零 */
    ADC_ADCX->SQR1 |= 5 << 0;       /* L[3:0]=5,6个转换在规则序列中 也就是转换规则序列1~6 */

    /* 设置规则序列1~6对应的ADC转换通道 */
    ADC_ADCX->SQR1 |= 14 << 6;      /* SQ1[4:0] = 14, 规则序列1 = 通道14(PA2) */
    ADC_ADCX->SQR1 |= 15 << 12;     /* SQ2[4:0] = 15, 规则序列2 = 通道15(PA3) */
    ADC_ADCX->SQR1 |= 16 << 18;     /* SQ3[4:0] = 16, 规则序列3 = 通道16(PA0) */
    ADC_ADCX->SQR1 |= 17 << 24;     /* SQ4[4:0] = 17, 规则序列4 = 通道17(PA1) */
    ADC_ADCX->SQR2 |= 18 << 0;      /* SQ5[4:0] = 18, 规则序列5 = 通道18(PA4) */
    ADC_ADCX->SQR2 |= 19 << 6;      /* SQ6[4:0] = 19, 规则序列5 = 通道19(PA5) */

    /* 设置ADC1通道14~19对应的IO口模拟输入 */
    /* PA2对应ADC1_INP14 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN2,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */
    
    /* PA3对应ADC1_INP15 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN3,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */
    
    /* PA0对应ADC1_INP16 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN0,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */
    
    /* PA1对应ADC1_INP17 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN1,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */
    
    /* PA4对应ADC1_INP18 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN4,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */
                 
    /* PA5对应ADC1_INP19 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN5,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */

    /* 设置ADC1通道14~19的采样时间 */
    adc_channel_set(ADC_ADCX, 14, 7); /* 设置ADCX对通道14(对应PA2脚)的采样时间为810.5个时钟周期 */
    adc_channel_set(ADC_ADCX, 15, 7); /* 设置ADCX对通道15(对应PA3脚)的采样时间为810.5个时钟周期 */
    adc_channel_set(ADC_ADCX, 16, 7); /* 设置ADCX对通道16(对应PA0脚)的采样时间为810.5个时钟周期 */
    adc_channel_set(ADC_ADCX, 17, 7); /* 设置ADCX对通道17(对应PA1脚)的采样时间为810.5个时钟周期 */
    adc_channel_set(ADC_ADCX, 18, 7); /* 设置ADCX对通道18(对应PA4脚)的采样时间为810.5个时钟周期 */
    adc_channel_set(ADC_ADCX, 19, 7); /* 设置ADCX对通道19(对应PA5脚)的采样时间为810.5个时钟周期 */
    
    /* 设置通道预选,选择通道14~19,6个通道 */
    ADC_ADCX->PCSEL |= 0X3F << 14;  /* ADC转换通道预选择 */

    /* DMA相关配置 */
    dma_mux_init(ADC_ADCX_DMASx, ADC_ADCX_DMASx_REQ);  /* 初始化DMA 请求复用器 */    
    ADC_ADCX_DMASx->PAR = par;      /* DMA外设地址 */
    ADC_ADCX_DMASx->M0AR = mar;     /* DMA 存储器0地址 */
    ADC_ADCX_DMASx->NDTR = 0;       /* DMA 存储器0地址 */
    ADC_ADCX_DMASx->CR = 0;         /* 先全部复位CR寄存器值 */

    ADC_ADCX_DMASx->CR |= 0 << 6;   /* 外设到存储器模式 */
    ADC_ADCX_DMASx->CR |= 0 << 8;   /* 非循环模式(即使用普通模式) */
    ADC_ADCX_DMASx->CR |= 0 << 9;   /* 外设非增量模式 */
    ADC_ADCX_DMASx->CR |= 1 << 10;  /* 存储器增量模式 */
    ADC_ADCX_DMASx->CR |= 1 << 11;  /* 外设数据长度:16位 */
    ADC_ADCX_DMASx->CR |= 1 << 13;  /* 存储器数据长度:16位 */
    ADC_ADCX_DMASx->CR |= 1 << 16;  /* 中等优先级 */
    ADC_ADCX_DMASx->CR |= 0 << 21;  /* 外设突发单次传输 */
    ADC_ADCX_DMASx->CR |= 0 << 23;  /* 存储器突发单次传输 */
    
    ADC_ADCX_DMASx->CR |= 1 << 4;   /* TCIE = 1, DMA传输完成中断使能 */ 
    
    sys_nvic_init(3, 3, ADC_ADCX_DMASx_IRQn, 2);  /* 组2，最低优先级 */
}

/**
 * @brief       ADC 过采样 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 *              本函数可以控制ADC过采样范围从1x ~ 1024x, 得到最高26位分辨率的AD转换结果
 * @param       osr : 过采样倍率, 0 ~ 1023, 表示:1x ~ 1024x过采样倍率
 * @param       ovss: 过采样右移位数, 0~11, 表示右移0位~11位. 
 *   @note      过采样后, ADC的转换时间相应的会慢 osr倍. 
 * @retval      无
 */
void adc_oversample_init(uint32_t osr, uint8_t ovss)
{
    adc_init(); /* 先初始化ADC */
    
    ADC_ADCX->CR &= ~(1 << 0);  /* 关闭AD转换器 */

    ADC_ADCX->CFGR2 = 0;        /* CFGR2清零 */
    ADC_ADCX->CFGR2 |= (osr & 0X3FF) << 16; /* OSR[9:0]=osr, 设置过采样 */
    ADC_ADCX->CFGR2 |= 0 << 10; /* ROVSM=0, 连续模式 */
    ADC_ADCX->CFGR2 |= 0 << 9;  /* TROVS=0, 触发后连续完成过采样 */
    ADC_ADCX->CFGR2 |= (uint32_t)ovss << 5; /* OVSS[3:0]=ovss, 设置过采样右移位数 */
    ADC_ADCX->CFGR2 |= 1 << 0;  /* ROVSE=1, 使能常规过采样 */
   
    ADC_ADCX->CR |= 1 << 0;     /* 开启AD转换器 */
}

























