/**
 ****************************************************************************************************
 * @file        rtc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-24
 * @brief       RTC 驱动代码
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
 * V1.0 20200324
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/RTC/rtc.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       等待RSF同步
 * @param       无
 * @retval      0,成功;1,失败;
 */
static uint8_t rtc_wait_synchro(void)
{
    uint32_t retry = 0XFFFFF;
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    RTC->ISR &= ~(1 << 5);  /* 清除RSF位 */

    while (retry && ((RTC->ISR & (1 << 5)) == 0x00))
    {
        retry--;    /* 等待影子寄存器同步 */
    }
    
    if (retry == 0)return 1;/* 同步失败 */

    RTC->WPR = 0xFF;        /* 使能RTC寄存器写保护 */
    return 0;
}

/**
 * @brief       RTC进入初始化模式
 * @param       无
 * @retval      0,成功;1,失败;
 */
static uint8_t rtc_init_mode(void)
{
    uint32_t retry = 0XFFFFF;

    if (RTC->ISR & (1 << 6))return 0;

    RTC->ISR |= 1 << 7; /* 进入RTC初始化模式 */

    while (retry && ((RTC->ISR & (1 << 6)) == 0x00))
    {
        retry--;    /* 等待进入RTC初始化模式成功 */
    }
   
    if (retry == 0)
    {
        return 1;   /* 同步失败 */
    }
    else 
    {
        return 0;   /* 同步成功 */
    }
}

/**
 * @brief       RTC写入后备区域SRAM
 * @param       bkrx : 后备区寄存器编号,范围:0~31
 * @param       data : 要写入的数据,32位长度
 * @retval      无
 */
void rtc_write_bkr(uint32_t bkrx, uint32_t data)
{
    uint32_t temp = 0;
    temp = RTC_BASE + 0x50 + bkrx * 4;
    (*(uint32_t *)temp) = data;
}

/**
 * @brief       RTC读取后备区域SRAM
 * @param       bkrx : 后备区寄存器编号,范围:0~31 
 * @retval      读取到的值
 */
uint32_t rtc_read_bkr(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = RTC_BASE + 0x50 + bkrx * 4;
    return (*(uint32_t *)temp); /* 返回读取到的值 */
}

/**
 * @brief       十进制转换为BCD码
 * @param       val : 要转换的十进制数 
 * @retval      BCD码
 */
static uint8_t rtc_dec2bcd(uint8_t val)
{
    uint8_t bcdhigh = 0;

    while (val >= 10)
    {
        bcdhigh++;
        val -= 10;
    }

    return ((uint8_t)(bcdhigh << 4) | val);
}

/**
 * @brief       BCD码转换为十进制数据
 * @param       val : 要转换的BCD码 
 * @retval      十进制数据
 */
static uint8_t rtc_bcd2dec(uint8_t val)
{
    uint8_t temp = 0;
    temp = (val >> 4) * 10;
    return (temp + (val & 0X0F));
}

/**
 * @brief       RTC时间设置
 * @param       hour,min,sec: 小时,分钟,秒钟 
 * @param       ampm        : AM/PM, 0=AM/24H; 1=PM/12H;
 * @retval      0,成功
 *              1,进入初始化模式失败
 */
uint8_t rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
    uint32_t temp = 0;
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    if (rtc_init_mode())return 1;   /* 进入RTC初始化模式失败 */

    temp = (((uint32_t)ampm & 0X01) << 22) | ((uint32_t)rtc_dec2bcd(hour) << 16) | ((uint32_t)rtc_dec2bcd(min) << 8) | (rtc_dec2bcd(sec));
    RTC->TR = temp;
    RTC->ISR &= ~(1 << 7);  /* 退出RTC初始化模式 */
    return 0;
}

/**
 * @brief       RTC日期设置
 * @param       year,month,date : 年(0~99),月(1~12),日(0~31)
 * @param       week            : 星期(1~7,0,非法!)
 * @retval      0,成功
 *              1,进入初始化模式失败
 */
uint8_t rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    uint32_t temp = 0;
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    if (rtc_init_mode())return 1;   /* 进入RTC初始化模式失败 */

    temp = (((uint32_t)week & 0X07) << 13) | ((uint32_t)rtc_dec2bcd(year) << 16) | ((uint32_t)rtc_dec2bcd(month) << 8) | (rtc_dec2bcd(date));
    RTC->DR = temp;
    RTC->ISR &= ~(1 << 7);  /* 退出RTC初始化模式 */
    return 0;
}

/**
 * @brief       获取RTC时间
 * @param       *hour,*min,*sec : 小时,分钟,秒钟
 * @param       *ampm           : AM/PM,0=AM/24H,1=PM.
 * @retval      无
 */
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm)
{
    uint32_t temp = 0;

    while (rtc_wait_synchro()); /* 等待同步 */

    temp = RTC->TR;
    *hour = rtc_bcd2dec((temp >> 16) & 0X3F);
    *min = rtc_bcd2dec((temp >> 8) & 0X7F);
    *sec = rtc_bcd2dec(temp & 0X7F);
    *ampm = temp >> 22;
}

/**
 * @brief       获取RTC日期
 * @param       *year,*mon,*date: 年,月,日
 * @param       *week           : 星期
 * @retval      无
 */
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week)
{
    uint32_t temp = 0;

    while (rtc_wait_synchro());	/* 等待同步 */

    temp = RTC->DR;
    *year = rtc_bcd2dec((temp >> 16) & 0XFF);
    *month = rtc_bcd2dec((temp >> 8) & 0X1F);
    *date = rtc_bcd2dec(temp & 0X3F);
    *week = (temp >> 13) & 0X07;
}

/**
 * @brief       RTC初始化
 *   @note 
 *              默认尝试使用LSE,当LSE启动失败后,切换为LSI.
 *              通过BKP寄存器0的值,可以判断RTC使用的是LSE/LSI:
 *              当BKP0==0X5050时,使用的是LSE
 *              当BKP0==0X5051时,使用的是LSI
 *              注意:切换LSI/LSE将导致时间/日期丢失,切换后需重新设置.
 *
 * @param       无
 * @retval      0,成功
 *              1,进入初始化模式失败
 */
uint8_t rtc_init(void)
{
    uint16_t ssr;
    uint16_t bkpflag = 0;
    uint16_t retry = 200;
    uint32_t tempreg = 0;
    PWR->CR1 |= 1 << 8;     /* DBP=1,后备区域写使能 */
    bkpflag = rtc_read_bkr(0);  /* 读取BKP0的值 */

    if (bkpflag != 0X5050)      /* 之前使用的不是LSE */
    {
        RCC->CSR |= 1 << 0;     /* LSI总是使能 */

        while (!(RCC->CSR & 0x02)); /* 等待LSI就绪 */

        RCC->BDCR |= 1 << 0;    /* 尝试开启LSE */

        while (retry && ((RCC->BDCR & 0X02) == 0))  /* 等待LSE准备好 */
        {
            retry--;
            delay_ms(5);
        }

        tempreg = RCC->BDCR;    /* 读取BDCR的值 */
        tempreg &= ~(3 << 8);   /* 清零8/9位 */

        if (retry == 0)tempreg |= 1 << 9;   /* LSE开启失败,启动LSI. */
        else tempreg |= 1 << 8; /* 选择LSE,作为RTC时钟 */

        tempreg |= 1 << 15;     /* 使能RTC时钟 */
        RCC->BDCR = tempreg;    /* 重新设置BDCR寄存器 */
        /* 关闭RTC寄存器写保护 */
        RTC->WPR = 0xCA;
        RTC->WPR = 0x53;
        RTC->CR = 0;

        if (rtc_init_mode())
        {
            RCC->BDCR = 1 << 16;/* 复位BDCR */
            delay_ms(10);
            RCC->BDCR = 0;      /*  结束复位 */
            return 2;           /* 进入RTC初始化模式失败 */
        }

        RTC->PRER = 0XFF;       /* RTC同步分频系数(0~7FFF),必须先设置同步分频,再设置异步分频,Frtc=Fclks/((Sprec+1)*(Asprec+1)) */
        RTC->PRER |= 0X7F << 16;/* RTC异步分频系数(1~0X7F) */
        RTC->CR &= ~(1 << 6);   /* RTC设置为,24小时格式 */
        RTC->ISR &= ~(1 << 7);  /* 退出RTC初始化模式 */
        RTC->WPR = 0xFF;        /* 使能RTC寄存器写保护 */

        if (bkpflag != 0X5051)  /* BKP0的内容既不是0X5050,也不是0X5051,说明是第一次配置,需要设置时间日期. */
        {
            rtc_set_time(23, 59, 56, 0);/* 设置时间 */
            rtc_set_date(20, 1, 13, 7); /* 设置日期 */
            //rtc_set_alarma(7,0,0,10); /* 设置闹钟时间 */
        }

        if (retry == 0)
        {
            rtc_write_bkr(0, 0X5051);   /* 标记已经初始化过了,使用LSI */
        }
        else 
        {
            rtc_write_bkr(0, 0X5050);   /* 标记已经初始化过了,使用LSE */
        }
    }
    else
    {
        retry = 10;     /* 连续10次SSR的值都没变化,则LSE死了. */
        ssr = RTC->SSR; /* 读取初始值 */

        while (retry)   /* 检测ssr寄存器的动态,来判断LSE是否正常 */
        {
            delay_ms(10);

            if (ssr == RTC->SSR)        /* 对比 */
            {
                retry--;
            }
            else 
            {
                break;
            }
        }

        if (retry == 0) /* LSE挂了,清除配置等待下次进入重新设置 */
        {
            rtc_write_bkr(0, 0XFFFF);   /* 标记错误的值 */
            RCC->BDCR = 1 << 16;        /* 复位BDCR */
            delay_ms(10);
            RCC->BDCR = 0;              /* 结束复位 */
        }
    }

    //rtc_set_wakeup(4,0);  /* 配置WAKE UP中断,1秒钟中断一次 */
    return 0;
}

/**
 * @breif       设置闹钟时间(按星期闹铃,24小时制)
 * @param       week        : 星期几(1~7) 
 * @param       hour,min,sec: 小时,分钟,秒钟
 * @retval      无
 */
void rtc_set_alarma(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    RTC->CR &= ~(1 << 8);   /* 关闭闹钟A */

    while ((RTC->ISR & 0X01) == 0); /* 等待闹钟A修改允许 */

    RTC->ALRMAR = 0;        /* 清空原来设置 */
    RTC->ALRMAR |= 1 << 30; /* 按星期闹铃 */
    RTC->ALRMAR |= 0 << 22; /* 24小时制 */
    RTC->ALRMAR |= (uint32_t)rtc_dec2bcd(week) << 24;   /* 星期设置 */
    RTC->ALRMAR |= (uint32_t)rtc_dec2bcd(hour) << 16;   /* 小时设置 */
    RTC->ALRMAR |= (uint32_t)rtc_dec2bcd(min) << 8;     /* 分钟设置 */
    RTC->ALRMAR |= (uint32_t)rtc_dec2bcd(sec);          /* 秒钟设置 */
    RTC->ALRMASSR = 0;      /* 不使用SUB SEC */
    RTC->CR |= 1 << 12;     /* 开启闹钟A中断 */
    RTC->CR |= 1 << 8;      /* 开启闹钟A */
    RTC->WPR = 0XFF;        /* 禁止修改RTC寄存器 */

    RTC->ISR &= ~(1 << 8);  /* 清除RTC闹钟A的标志 */
    EXTI_D1->PR1 = 1 << 17; /* 清除LINE17上的中断标志位 */
    EXTI_D1->IMR1 |= 1 << 17;/* 开启line17上的中断 */
    EXTI->RTSR1 |= 1 << 17; /* line17上事件上升降沿触发 */
    sys_nvic_init(2, 2, RTC_Alarm_IRQn, 2); /* 抢占2，子优先级2，组2 */
}

/**
 * @breif       周期性唤醒定时器设置
 * @param       wksel
 *   @arg       000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
 *   @arg       10x,ck_spre,1Hz;11x,1Hz,且cnt值增加2^16(即cnt+2^16)
 *   @note      注意:RTC就是RTC的时钟频率,即RTCCLK!
 * @param       cnt: 自动重装载值.减到0,产生中断.
 * @retval      无
 */
void rtc_set_wakeup(uint8_t wksel, uint16_t cnt)
{
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    RTC->CR &= ~(1 << 10);  /* 关闭WAKE UP */

    while ((RTC->ISR & 0X04) == 0);	/* 等待WAKE UP修改允许 */

    RTC->CR &= ~(7 << 0);   /* 清除原来的设置 */
    RTC->CR |= wksel & 0X07;/* 设置新的值 */
    RTC->WUTR = cnt;        /* 设置WAKE UP自动重装载寄存器值 */
    RTC->ISR &= ~(1 << 10); /* 清除RTC WAKE UP的标志 */
    RTC->CR |= 1 << 14;     /* 开启WAKE UP 定时器中断 */
    RTC->CR |= 1 << 10;     /* 开启WAKE UP 定时器 */
    RTC->WPR = 0XFF;        /* 禁止修改RTC寄存器 */
    EXTI_D1->PR1 = 1 << 19; /* 清除LINE19上的中断标志位 */
    EXTI_D1->IMR1 |= 1 << 19;/* 开启line19上的中断 */
    EXTI->RTSR1 |= 1 << 19; /* line19上事件上升降沿触发 */
    sys_nvic_init(2, 2, RTC_WKUP_IRQn, 2);  /* 抢占2，子优先级2，组2 */
}

/**
 * @breif       RTC闹钟中断服务函数
 * @param       无
 * @retval      无
 */
void RTC_Alarm_IRQHandler(void)
{
    if (RTC->ISR & (1 << 8))    /* ALARM A中断? */
    {
        RTC->ISR &= ~(1 << 8);  /* 清除中断标志 */
        printf("ALARM A!\r\n");
    }

    EXTI_D1->PR1 |= 1 << 17;    /* 清除中断线17的中断标志 */
}

/**
 * @breif       RTC WAKE UP中断服务函数
 * @param       无
 * @retval      无
 */
void RTC_WKUP_IRQHandler(void)
{ 
    if (RTC->ISR & (1 << 10))   /* WK_UP中断? */
    {
        RTC->ISR &= ~(1 << 10); /* 清除中断标志 */
        LED1_TOGGLE();
    }

    EXTI_D1->PR1 |= 1 << 19;    /* 清除中断线19的中断标志 */
}

/* 月修正数据表 */
uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};

/**
 * @breif       获得现在是星期几, 输入公历日期得到星期(只允许1901-2099年)
 * @param       year,month,day：公历年月日
 * @retval      星期号(1~7,代表周1~周日)
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;
    yearH = year / 100;
    yearL = year % 100;

    /*  如果为21世纪,年份数加100 */
    if (yearH > 19)yearL += 100;

    /*  所过闰年数只算1900年之后的 */
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)temp2--;

    temp2 %= 7;

    if (temp2 == 0)temp2 = 7;

    return temp2;
}













