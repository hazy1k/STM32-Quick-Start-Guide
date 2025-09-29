/**
 ****************************************************************************************************
 * @file        sys.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-03-12
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)
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
 * V1.0 20200312
 * 第一次发布
 * V1.1 20200401
 * 1, 将头文件包含路径改成相对路径,避免重复设置包含路径的麻烦
 * 2, 将函数改成带一个参数,用于支持W25Q256, 实现了支持所有25Q系列芯片
 * 3, 将sys_nvic_set_vector_table函数改成普通函数, 方便外部调用
 ****************************************************************************************************
 */


#include "./SYSTEM/sys/sys.h"


/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基址
 * @param       offset: 偏移量
 * @retval      无
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

/**
 * @brief       设置NVIC分组
 * @param       group: 0~4,共5组, 详细解释见: sys_nvic_init函数参数说明
 * @retval      无
 */
static void sys_nvic_priority_group_config(uint8_t group)
{
    uint32_t temp, temp1;
    temp1 = (~group) & 0x07;/* 取后三位 */
    temp1 <<= 8;
    temp = SCB->AIRCR;      /* 读取先前的设置 */
    temp &= 0X0000F8FF;     /* 清空先前分组 */
    temp |= 0X05FA0000;     /* 写入钥匙 */
    temp |= temp1;
    SCB->AIRCR = temp;      /* 设置分组 */
}

/**
 * @brief       设置NVIC(包括分组/抢占优先级/子优先级等)
 * @param       pprio: 抢占优先级(PreemptionPriority)
 * @param       sprio: 子优先级(SubPriority)
 * @param       ch: 中断编号(Channel)
 * @param       group: 中断分组
 *   @arg       0, 组0: 0位抢占优先级, 4位子优先级
 *   @arg       1, 组1: 1位抢占优先级, 3位子优先级
 *   @arg       2, 组2: 2位抢占优先级, 2位子优先级
 *   @arg       3, 组3: 3位抢占优先级, 1位子优先级
 *   @arg       4, 组4: 4位抢占优先级, 0位子优先级
 * @note        注意优先级不能超过设定的组的范围! 否则会有意想不到的错误
 * @retval      无
 */
void sys_nvic_init(uint8_t pprio, uint8_t sprio, uint8_t ch, uint8_t group)
{
    uint32_t temp;
    sys_nvic_priority_group_config(group);  /* 设置分组 */
    temp = pprio << (4 - group);
    temp |= sprio & (0x0f >> group);
    temp &= 0xf;                            /* 取低四位 */
    NVIC->ISER[ch / 32] |= 1 << (ch % 32);  /* 使能中断位(要清除的话,设置ICER对应位为1即可) */
    NVIC->IP[ch] |= temp << 4;              /* 设置响应优先级和抢断优先级 */
}

/**
 * @brief       外部中断配置函数, 只针对GPIOA~GPIOK
 * @note        该函数会自动开启对应中断, 以及屏蔽线
 * @param       p_gpiox: GPIOA~GPIOK, GPIO指针
 * @param       pinx: 0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @param       tmode: 1~3, 触发模式
 *   @arg       SYS_GPIO_FTIR, 1, 下降沿触发
 *   @arg       SYS_GPIO_RTIR, 2, 上升沿触发
 *   @arg       SYS_GPIO_BTIR, 3, 任意电平触发
 * @retval      无
 */
void sys_nvic_ex_config(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t tmode)
{
    uint8_t offset;
    uint32_t gpio_num = 0;      /* gpio编号, 0~10, 代表GPIOA~GPIOK */
    uint32_t pinpos = 0, pos = 0, curpin = 0;

    gpio_num = ((uint32_t)p_gpiox - (uint32_t)GPIOA) / 0X400 ;/* 得到gpio编号 */
    RCC->APB4ENR |= 1 << 1;     /* SYSCFGEN = 1,使能SYSCFG时钟 */

    for (pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;      /* 一个个位检查 */
        curpin = pinx & pos;    /* 检查引脚是否要设置 */

        if (curpin == pos)      /* 需要设置 */
        {
            offset = (pinpos % 4) * 4;
            SYSCFG->EXTICR[pinpos / 4] &= ~(0x000F << offset);  /* 清除原来设置！！！ */
            SYSCFG->EXTICR[pinpos / 4] |= gpio_num << offset;   /* EXTI.BITx映射到gpiox.bitx */

            EXTI_D1->IMR1 |= 1 << pinpos;   /* 开启line BITx上的中断(如果要禁止中断，则反操作即可) */

            if (tmode & 0x01) EXTI->FTSR1 |= 1 << pinpos;       /* line bitx上事件下降沿触发 */
            if (tmode & 0x02) EXTI->RTSR1 |= 1 << pinpos;       /* line bitx上事件上升降沿触发 */
        }
    }
}

/**
 * @brief       GPIO复用功能选择设置
 * @param       p_gpiox: GPIOA~GPIOK, GPIO指针
 * @param       pinx: 0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @param       afx:0~15, 代表AF0~AF15.
 *              AF0~15设置情况(这里仅是列出常用的, 详细的请见STM32H750xx数据手册, Table 8~18):
 *   @arg       AF0: MCO/SWD/SWCLK/RTC;        AF1: TIM1/2/TIM16/17/LPTIM1;     AF2: TIM3~5/TIM12/HRTIM1/SAI1;   AF3: TIM8/LPTIM2~5/HRTIM1/LPUART1;
 *   @arg       AF4: I2C1~I2C4/TIM15/USART1;   AF5: SPI1~SPI6/CEC;              AF6: SPI3/SAI1~3/UART4/I2C4;     AF7: SPI2/3/6/USART1~3/6/UART7/SDIO1;
 *   @arg       AF8: USART4/5/8/SPDIF/SAI2/4;  AF9; FDCAN1~2/TIM13/14/LCD/QSPI; AF10: USB_OTG1/2/SAI2/4/QSPI;    AF11: ETH/UART7/SDIO2/I2C4/COMP1/2;
 *   @arg       AF12: FMC/SDIO1/OTG2/LCD;      AF13: DCIM/DSI/LCD/COMP1/2;      AF14: LCD/UART5;                 AF15: EVENTOUT;
 * @retval      无
 */
void sys_gpio_af_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t afx)
{
    uint32_t pinpos = 0, pos = 0, curpin = 0;;

    for (pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;      /* 一个个位检查 */
        curpin = pinx & pos;    /* 检查引脚是否要设置 */

        if (curpin == pos)      /* 需要设置 */
        {
            p_gpiox->AFR[pinpos >> 3] &= ~(0X0F << ((pinpos & 0X07) * 4));
            p_gpiox->AFR[pinpos >> 3] |= (uint32_t)afx << ((pinpos & 0X07) * 4);
        }
    }
}

/**
 * @brief       GPIO通用设置
 * @param       p_gpiox: GPIOA~GPIOK, GPIO指针
 * @param       pinx: 0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 *
 * @param       mode: 0~3; 模式选择, 设置如下:
 *   @arg       SYS_GPIO_MODE_IN,  0, 输入模式(系统复位默认状态)
 *   @arg       SYS_GPIO_MODE_OUT, 1, 输出模式
 *   @arg       SYS_GPIO_MODE_AF,  2, 复用功能模式
 *   @arg       SYS_GPIO_MODE_AIN, 3, 模拟输入模式
 *
 * @param       otype: 0 / 1; 输出类型选择, 设置如下:
 *   @arg       SYS_GPIO_OTYPE_PP, 0, 推挽输出
 *   @arg       SYS_GPIO_OTYPE_OD, 1, 开漏输出
 *
 * @param       ospeed: 0~3; 输出速度, 设置如下:
 *   @arg       SYS_GPIO_SPEED_LOW,  0, 低速
 *   @arg       SYS_GPIO_SPEED_MID,  1, 中速
 *   @arg       SYS_GPIO_SPEED_FAST, 2, 快速
 *   @arg       SYS_GPIO_SPEED_HIGH, 3, 高速
 *
 * @param       pupd: 0~3: 上下拉设置, 设置如下:
 *   @arg       SYS_GPIO_PUPD_NONE, 0, 不带上下拉
 *   @arg       SYS_GPIO_PUPD_PU,   1, 上拉
 *   @arg       SYS_GPIO_PUPD_PD,   2, 下拉
 *   @arg       SYS_GPIO_PUPD_RES,  3, 保留
 *
 * @note:       注意: 在输入模式(普通输入/模拟输入)下, OTYPE和OSPEED参数无效!!
 * @retval      无
 */
void sys_gpio_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint32_t mode, uint32_t otype, uint32_t ospeed, uint32_t pupd)
{
    uint32_t pinpos = 0, pos = 0, curpin = 0;

    for (pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;      /* 一个个位检查 */
        curpin = pinx & pos;    /* 检查引脚是否要设置 */

        if (curpin == pos)      /* 需要设置 */
        {
            p_gpiox->MODER &= ~(3 << (pinpos * 2)); /* 先清除原来的设置 */
            p_gpiox->MODER |= mode << (pinpos * 2); /* 设置新的模式 */

            if ((mode == 0X01) || (mode == 0X02))   /* 如果是输出模式/复用功能模式 */
            {
                p_gpiox->OSPEEDR &= ~(3 << (pinpos * 2));       /* 清除原来的设置 */
                p_gpiox->OSPEEDR |= (ospeed << (pinpos * 2));   /* 设置新的速度值 */
                p_gpiox->OTYPER &= ~(1 << pinpos) ;             /* 清除原来的设置 */
                p_gpiox->OTYPER |= otype << pinpos;             /* 设置新的输出模式 */
            }

            p_gpiox->PUPDR &= ~(3 << (pinpos * 2)); /* 先清除原来的设置 */
            p_gpiox->PUPDR |= pupd << (pinpos * 2); /* 设置新的上下拉 */
        }
    }
}

/**
 * @brief       设置GPIO某个引脚的输出状态
 * @param       p_gpiox: GPIOA~GPIOK, GPIO指针
 * @param       0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @param       status: 0/1, 引脚状态(仅最低位有效), 设置如下:
 *   @arg       0, 输出低电平
 *   @arg       1, 输出高电平
 * @retval      无
 */
void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status)
{
    if (status & 0X01)
    {
        p_gpiox->BSRR |= pinx;              /* 设置GPIOx的pinx为1 */
    }
    else
    {
        p_gpiox->BSRR |= (uint32_t)pinx << 16;   /* 设置GPIOx的pinx为0 */
    }
}

/**
 * @brief       读取GPIO某个引脚的状态
 * @param       p_gpiox: GPIOA~GPIOK, GPIO指针
 * @param       0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @retval      返回引脚状态, 0, 低电平; 1, 高电平
 */
uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx)
{
    if (p_gpiox->IDR & pinx)
    {
        return 1;   /* pinx的状态为1 */
    }
    else
    {
        return 0;   /* pinx的状态为0 */
    }
}

/**
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       设置栈顶地址
 * @note        左侧的红X, 属于MDK误报, 实际是没问题的
 * @param       addr: 栈顶地址
 * @retval      无
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* 设置栈顶地址 */
}

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void sys_standby(void)
{
    PWR->WKUPEPR &= ~(1 << 0);  /* WKUPEN0 = 0, PA0不用于WKUP唤醒 */
    PWR->WKUPEPR |= 1 << 0;     /* WKUPEN0 = 1, PA0用于WKUP唤醒 */
    PWR->WKUPEPR &= ~(1 << 8);  /* WKUPP0 = 0, PA0高电平唤醒(上升沿) */
    PWR->WKUPEPR &= ~(3 << 16); /* 清除WKUPPUPD原来的设置 */
    PWR->WKUPEPR |= 2 << 16;    /* WKUPPUPD = 10, PA0下拉 */
    PWR->WKUPCR |= 0X3F << 0;   /* 清除WKUP0~5唤醒标志 */
    PWR->CPUCR |= 7 << 0;       /* PDDS_D1/D2/D3 = 1, 允许D1/D2/D3进入深度睡眠模式(PDDS) */
    SCB->SCR |= 1 << 2;         /* 使能SLEEPDEEP位 (SYS->CTRL) */
    sys_wfi_set();              /* 执行WFI指令, 进入待机模式 */
}

/**
 * @brief       系统软复位
 * @param       无
 * @retval      无
 */
void sys_soft_reset(void)
{
    SCB->AIRCR = 0X05FA0000 | (uint32_t)0x04;
}

/**
 * @brief       使能STM32H7的L1-Cache, 同时开启D cache的强制透写
 * @param       无
 * @retval      无
 */
void sys_cache_enable(void)
{
    SCB_EnableICache(); /* 使能I-Cache,函数在core_cm7.h里面定义 */
    SCB_EnableDCache(); /* 使能D-Cache,函数在core_cm7.h里面定义 */
    SCB->CACR |= 1 << 2;/* 强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题 */
}

/**
 * @brief       时钟设置函数
 * @param       plln: PLL1倍频系数(PLL倍频), 取值范围: 4~512.
 * @param       pllm: PLL1预分频系数(进PLL之前的分频), 取值范围: 2~63.
 * @param       pllp: PLL1的p分频系数(PLL之后的分频), 分频后作为系统时钟, 取值范围: 2~128.(且必须是2的倍数)
 * @param       pllq: PLL1的q分频系数(PLL之后的分频), 取值范围: 1~128.
 * @note
 *
 *              Fvco: VCO频率
 *              Fsys: 系统时钟频率, 也是PLL1的p分频输出时钟频率
 *              Fq:   PLL1的q分频输出时钟频率
 *              Fs:   PLL输入时钟频率, 可以是HSI, CSI, HSE等.
 *              Fvco = Fs * (plln / pllm);
 *              Fsys = Fvco / pllp = Fs * (plln / (pllm * pllp));
 *              Fq   = Fvco / pllq = Fs * (plln / (pllm * pllq));
 *
 *              外部晶振为25M的时候, 推荐值: plln = 192, pllm = 5, pllp = 2, pllq = 4.
 *              外部晶振为 8M的时候, 推荐值: plln = 240, pllm = 2, pllp = 2, pllq = 4.
 *              得到:Fvco = 8 * (240 / 2) = 960Mhz
 *                   Fsys = pll1_p_ck = 960 / 2 = 480Mhz
 *                   Fq   = pll1_q_ck = 960 / 4 = 240Mhz
 *
 *              H750默认需要配置的频率如下:
 *              CPU频率(rcc_c_ck) = sys_d1cpre_ck = 480Mhz
 *              rcc_aclk = rcc_hclk3 = 240Mhz
 *              AHB1/2/3/4(rcc_hclk1/2/3/4) = 240Mhz
 *              APB1/2/3/4(rcc_pclk1/2/3/4) = 120Mhz
 *              pll2_p_ck = (8 / 8) * 440 / 2) = 220Mhz
 *              pll2_r_ck = FMC时钟频率 = ((8 / 8) * 440 / 2) = 220Mhz
 *
 * @retval      错误代码: 0, 成功; 1, HSE错误; 2, PLL1错误; 3, PLL2错误; 4, 切换时钟错误;
 */
uint8_t sys_clock_set(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    uint32_t retry = 0;
    uint8_t retval = 0;
    uint8_t swsval = 0;
    PWR->CR3 &= ~(1 << 2);      /* SCUEN = 0, 锁定LDOEN和BYPASS位的设置 */
    PWR->D3CR |= 3 << 14;       /* VOS = 3, Scale1, 1.2V内核电压,FLASH访问可以得到最高性能 */
    /* 480M版本H750芯片（V版本）新增Scale0设置,在进入睡眠模式之前, 必须先退出Scale0模式！！ */
    RCC->APB4ENR |= 1 << 1;     /* 使能SYSCFGEN位 */
    SYSCFG->PWRCR |= 1 << 0;    /* 设置ODEN位为1, 使能Overdrive,此时VCORE = 1.35V */

    while ((PWR->D3CR & (1 << 13)) == 0);   /* 等待电压稳定 */

    RCC->CR |= 1 << 16; /* HSEON = 1, 开启HSE */

    while (((RCC->CR & (1 << 17)) == 0) && (retry < 0X7FFF))
    {
        retry++;        /* 等待HSE RDY */
    }

    if (retry == 0X7FFF)
    {
        retval = 1;     /* HSE无法就绪 */
    }
    else
    {
        RCC->PLLCKSELR |= 2 << 0;           /* PLLSRC[1:0] = 2, 选择HSE作为PLL的输入时钟源 */
        RCC->PLLCKSELR |= pllm << 4;        /* DIVM1[5:0] = pllm, 设置PLL1的预分频系数 */
        RCC->PLL1DIVR |= (plln - 1) << 0;   /* DIVN1[8:0] = plln - 1, 设置PLL1的倍频系数, 设置值需减1 */
        RCC->PLL1DIVR |= (pllp - 1) << 9;   /* DIVP1[6:0] = pllp - 1, 设置PLL1的p分频系数, 设置值需减1 */
        RCC->PLL1DIVR |= (pllq - 1) << 16;  /* DIVQ1[6:0] = pllq - 1, 设置PLL1的q分频系数, 设置值需减1 */
        RCC->PLL1DIVR |= 1 << 24;           /* DIVR1[6:0] = pllr - 1, 设置PLL1的r分频系数, 设置值需减1, r分频出来的时钟没用到 */
        RCC->PLLCFGR |= 2 << 2;             /* PLL1RGE[1:0] = 2, PLL1输入时钟频率在4~8Mhz之间(8 / 2 = 4Mhz), 如修改pllm, 请确认此参数 */
        RCC->PLLCFGR |= 0 << 1;             /* PLL1VCOSEL = 0, PLL1中的VCO范围, 192~836Mhz(实际可以到960, 以满足480M主频设置要求) */
        RCC->PLLCFGR |= 3 << 16;            /* DIVP1EN = 1, DIVQ1EN = 1, 使能pll1_p_ck和pll1_q_ck */
        RCC->CR |= 1 << 24;                 /* PLL1ON = 1, 使能PLL1 */
        retry = 0;

        while ((RCC->CR & (1 << 25)) == 0)   /* PLL1RDY = 1?, 等待PLL1准备好 */
        {
            retry++;

            if (retry > 0X1FFFFF)
            {
                retval = 2; /* PLL1无法就绪 */
                break;
            }
        }

        /* 设置PLL2的R分频输出, 为220Mhz, 后续做TFTLCD时钟, 可得到110M的fmc_ker_ck时钟频率 */
        RCC->PLLCKSELR |= 8 << 12;          /* DIVM2[5:0] = 8, 设置PLL2的预分频系数 */
        RCC->PLL2DIVR |= (440 - 1) << 0;    /* DIVN2[8:0] = 440 - 1, 设置PLL2的倍频系数, 设置值需减1 */
        RCC->PLL2DIVR |= (2 - 1) << 9;      /* DIVP2[6:0] = 2 - 1, 设置PLL2的p分频系数, 设置值需减1 */
        RCC->PLL2DIVR |= (2 - 1) << 24;     /* DIVR2[6:0] = 2 - 1, 设置PLL2的r分频系数, 设置值需减1 */
        RCC->PLLCFGR |= 0 << 6;             /* PLL2RGE[1:0] = 0, PLL2输入时钟频率在1~2Mhz之间(8/8 = 1Mhz) */
        RCC->PLLCFGR |= 0 << 5;             /* PLL2VCOSEL = 0, PLL2宽的VCO范围, 192~836Mhz */
        RCC->PLLCFGR |= 1 << 19;            /* DIVP2EN = 1, 使能pll2_p_ck */
        RCC->PLLCFGR |= 1 << 21;            /* DIVR2EN = 1, 使能pll2_r_ck */
        RCC->D1CCIPR &= ~(3 << 0);          /* 清除FMCSEL[1:0]原来的设置 */
        RCC->D1CCIPR |= 2 << 0;             /* FMCSEL[1:0] = 2, FMC时钟来自于pll2_r_ck */
        RCC->CR |= 1 << 26;                 /* PLL2ON = 1, 使能PLL2 */
        retry = 0;

        while ((RCC->CR & (1 << 27)) == 0)  /* PLL2RDY = 1?, 等待PLL2准备好 */
        {
            retry++;

            if (retry > 0X1FFFFF)
            {
                retval = 3; /* PLL2无法就绪 */
                break;
            }
        }

        RCC->D1CFGR |= 8 << 0;              /* HREF[3:0] = 8, rcc_hclk1/2/3/4  =  sys_d1cpre_ck / 2 = 480/2 = 240Mhz, 即AHB1/2/3/4 = 240Mhz */
        RCC->D1CFGR |= 0 << 8;              /* D1CPRE[2:0] = 0, sys_d1cpre_ck = sys_clk/1 = 480 / 1 = 480Mhz, 即CPU时钟 = 480Mhz */
        RCC->CFGR |= 3 << 0;                /* SW[2:0] = 3, 系统时钟(sys_clk)选择来自pll1_p_ck, 即480Mhz */
        retry = 0;

        while (swsval != 3)                 /* 等待成功将系统时钟源切换为pll1_p_ck */
        {
            swsval = (RCC->CFGR & (7 << 3)) >> 3;   /* 获取SWS[2:0]的状态, 判断是否切换成功 */
            retry++;

            if (retry > 0X1FFFFF)
            {
                retval = 4; /* 无法切换时钟 */
                break;
            }
        }

        FLASH->ACR |= 2 << 0;               /* LATENCY[2:0] = 2, 2个CPU等待周期(@VOS1 Level, maxclock = 210Mhz) */
        FLASH->ACR |= 2 << 4;               /* WRHIGHFREQ[1:0] = 2, flash访问频率<285Mhz */
        RCC->D1CFGR |= 4 << 4;              /* D1PPRE[2:0] = 4,  rcc_pclk3 = rcc_hclk3/2 = 120Mhz, 即APB3 = 120Mhz */
        RCC->D2CFGR |= 4 << 4;              /* D2PPRE1[2:0] = 4, rcc_pclk1 = rcc_hclk1/2 = 120Mhz, 即APB1 = 120Mhz */
        RCC->D2CFGR |= 4 << 8;              /* D2PPRE2[2:0] = 4, rcc_pclk2 = rcc_hclk1/2 = 120Mhz, 即APB2 = 120Mhz */
        RCC->D3CFGR |= 4 << 4;              /* D3PPRE[2:0] = 4,  rcc_pclk4 = rcc_hclk4/2 = 120Mhz, 即APB4 = 120Mhz */
        RCC->CR |= 1 << 7;                  /* CSION = 1, 使能CSI, 为IO补偿单元提供时钟 */
        RCC->APB4ENR |= 1 << 1;             /* SYSCFGEN = 1, 使能SYSCFG时钟 */
        SYSCFG->CCCSR |= 1 << 0;            /* EN = 1, 使能IO补偿单元 */
    }

    return retval;
}

/**
 * @brief       系统时钟初始化函数
 * @param       plln: PLL1倍频系数(PLL倍频), 取值范围: 4~512.
 * @param       pllm: PLL1预分频系数(进PLL之前的分频), 取值范围: 2~63.
 * @param       pllp: PLL1的p分频系数(PLL之后的分频), 分频后作为系统时钟, 取值范围: 2~128.(且必须是2的倍数)
 * @param       pllq: PLL1的q分频系数(PLL之后的分频), 取值范围: 1~128.
 * @retval      无
 */
void sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    RCC->CR = 0x00000001;           /* 设置HISON, 开启内部高速RC振荡，其他位全清零 */
    RCC->CFGR = 0x00000000;         /* CFGR清零 */
    RCC->D1CFGR = 0x00000000;       /* D1CFGR清零 */
    RCC->D2CFGR = 0x00000000;       /* D2CFGR清零 */
    RCC->D3CFGR = 0x00000000;       /* D3CFGR清零 */
    RCC->PLLCKSELR = 0x00000000;    /* PLLCKSELR清零 */
    RCC->PLLCFGR = 0x00000000;      /* PLLCFGR清零 */
    RCC->CIER = 0x00000000;         /* CIER清零, 禁止所有RCC相关中断 */

    /* AXI_TARG7_FN_MOD寄存器, 由于没有在stm32h750xx.h里面定义, 所以, 只能用直接 */
    /* 操作地址的方式, 来修改, 该寄存器在 << STM32H750参考手册>>第115页, AXI_TARGx_FN_MOD */
    *((volatile uint32_t *)0x51008108) = 0x00000001;/* 设置AXI SRAM的矩阵读取能力为1 */

    sys_clock_set(plln, pllm, pllp, pllq);  /* 设置时钟 */
    sys_qspi_enable_memmapmode(0);          /* 使能QSPI内存映射模式, FLASH容量为普通类型 */
    sys_cache_enable();                     /* 使能L1 Cache */

    /* 配置中断向量偏移 */
#ifdef  VECT_TAB_RAM
    sys_nvic_set_vector_table(D1_AXISRAM_BASE, 0x0);
#else
    sys_nvic_set_vector_table(FLASH_BANK1_BASE, 0x0);
#endif
}

/**
 * @brief       QSPI进入内存映射模式（执行QSPI代码必备前提）
 *   @note      必须根据所使用QSPI FLASH的容量设置正确的ftype值!
 * @param       ftype: flash类型
 *   @arg           0, 普通FLASH, 容量在128Mbit及以内的
 *   @arg           1, 大容量FLASH, 容量在256Mbit及以上的.
 * @retval      无
 */
void sys_qspi_enable_memmapmode(uint8_t ftype)
{
    uint32_t tempreg = 0;
    RCC->AHB4ENR |= 1 << 1;     /* 使能PORTB时钟 */
    RCC->AHB4ENR |= 1 << 3;     /* 使能PORTD时钟 */
    RCC->AHB4ENR |= 1 << 4;     /* 使能PORTE时钟 */
    RCC->AHB3ENR |= 1 << 14;    /* QSPI时钟使能 */

    sys_gpio_set(GPIOB, SYS_GPIO_PIN2,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU); /* PB2复用功能输出 */

    sys_gpio_set(GPIOB, SYS_GPIO_PIN6,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU); /* PB6复用功能输出 */

    sys_gpio_set(GPIOD, SYS_GPIO_PIN11 | SYS_GPIO_PIN12 | SYS_GPIO_PIN13,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU); /* PD11, 12, 13复用功能输出 */

    sys_gpio_set(GPIOE, SYS_GPIO_PIN2,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU); /* PE2复用功能输出 */

    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN2,  9);  /* PB2,  AF9 */
    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN6,  10); /* PB6,  AF10 */
    sys_gpio_af_set(GPIOD, SYS_GPIO_PIN11, 9);  /* PD11, AF9 */
    sys_gpio_af_set(GPIOD, SYS_GPIO_PIN12, 9);  /* PD12, AF9 */
    sys_gpio_af_set(GPIOD, SYS_GPIO_PIN13, 9);  /* PD13, AF9 */
    sys_gpio_af_set(GPIOE, SYS_GPIO_PIN2,  9);  /* PE2,  AF9 */

    /* QSPI设置，参考QSPI实验的QSPI_Init函数 */
    RCC->AHB3RSTR |= 1 << 14;       /* 复位QSPI */
    RCC->AHB3RSTR &= ~(1 << 14);    /* 停止复位QSPI */

    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    RCC->D1CCIPR &= ~(3 << 4);      /* 清除QSPISEL[1:0] */
    RCC->D1CCIPR |= 2 << 4;         /* QSPI时钟来自pll2_r_ck = 220Mhz */
    QUADSPI->CR = 0X01000310;       /* 设置CR寄存器, 这些值怎么来的，请参考QSPI实验/看H750参考手册寄存器描述分析 */
    QUADSPI->DCR = 0X00180201;      /* 设置DCR寄存器(FLASH容量32M(最大容量设置为32M, 默认用16M的), tSHSL = 3个时钟) */
    QUADSPI->CR |= 1 << 0;          /* 使能QSPI */

    /*
     *  注意:QSPI QE位的使能，在QSPI烧写算法里面，就已经设置了
     *  所以, 这里可以不用设置QE位，否则需要加入对QE位置1的代码
     *  不过, 代码必须通过仿真器下载, 直接烧录到外部QSPI FLASH, 是不可用的
     *  如果想直接烧录到外部QSPI FLASH也可以用, 则需要在这里添加QE位置1的代码
     *
     *  另外, 对与W25Q256,还需要使能4字节地址模式,或者设置S3的ADP位为1.
     *  我们在QSPI烧写算法里面已经设置了ADP=1(上电即32位地址模式),因此这里也
     *  不需要发送进入4字节地址模式指令/设置ADP=1了, 否则还需要设置ADP=1
     */

    /* BY/W25QXX 写使能（0X06指令）*/
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->CCR = 0X00000106;      /* 发送0X06指令，BY/W25QXX写使能 */

    while ((QUADSPI->SR & (1 << 1)) == 0);/* 等待指令发送完成 */

    QUADSPI->FCR |= 1 << 1;         /* 清除发送完成标志位 */

    /* MemroyMap 模式设置 */
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->ABR = 0;               /* 交替字节设置为0，实际上就是25QXX 0XEB指令的, M0~M7 = 0 */
    tempreg = 0XEB;                 /* INSTRUCTION[7:0] = 0XEB, 发送0XEB指令（Fast Read QUAD I/O） */
    tempreg |= 1 << 8;              /* IMODE[1:0] = 1, 单线传输指令 */
    tempreg |= 3 << 10;             /* ADDRESS[1:0] = 3, 四线传输地址 */
    tempreg |= (2 + ftype) << 12;   /* ADSIZE[1:0] = 2/3, 24位(ftype = 0) / 32位(ftype = 1)地址长度 */
    tempreg |= 3 << 14;             /* ABMODE[1:0] = 3, 四线传输交替字节 */
    tempreg |= 0 << 16;             /* ABSIZE[1:0] = 0, 8位交替字节(M0~M7) */
    tempreg |= 4 << 18;             /* DCYC[4:0] = 4, 4个dummy周期 */
    tempreg |= 3 << 24;             /* DMODE[1:0] = 3, 四线传输数据 */
    tempreg |= 3 << 26;             /* FMODE[1:0] = 3, 内存映射模式 */
    QUADSPI->CCR = tempreg;         /* 设置CCR寄存器 */

    /* 设置QSPI FLASH空间的MPU保护 */
    SCB->SHCSR &= ~(1 << 16);       /* 禁止MemManage */
    MPU->CTRL &= ~(1 << 0);         /* 禁止MPU */
    MPU->RNR = 0;                   /* 设置保护区域编号为0(1~7可以给其他内存用) */
    MPU->RBAR = 0X90000000;         /* 基地址为0X9000 000, 即QSPI的起始地址 */
    MPU->RASR = 0X0303002D;         /* 设置相关保护参数(禁止共用, 允许cache, 允许缓冲), 详见MPU实验的解析 */
    MPU->CTRL = (1 << 2) | (1 << 0);/* 使能PRIVDEFENA, 使能MPU */
    SCB->SHCSR |= 1 << 16;          /* 使能MemManage */
}






