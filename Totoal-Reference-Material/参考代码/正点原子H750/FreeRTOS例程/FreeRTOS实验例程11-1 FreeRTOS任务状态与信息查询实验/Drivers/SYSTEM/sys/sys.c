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
 * V1.0 20200401
 * 1, 将头文件包含路径改成相对路径,避免重复设置包含路径的麻烦
 * 2, 将函数改成带一个参数,用于支持W25Q256, 实现了支持所有25Q系列芯片
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
 * @retval      错误代码: 0, 成功; 1, 错误;
 */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_ClkInitTypeDef rcc_clk_init_handle;
    RCC_OscInitTypeDef rcc_osc_init_handle;
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_init_handle;

    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);                         /*使能供电配置更新 */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  /* VOS = 1, Scale1, 1.2V内核电压,FLASH访问可以得到最高性能 */
    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);     /* 等待电压稳定 */

    /* 使能HSE，并选择HSE作为PLL时钟源，配置PLL1，开启USB时钟 */
    rcc_osc_init_handle.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI48;
    rcc_osc_init_handle.HSEState = RCC_HSE_ON;
    rcc_osc_init_handle.HSIState = RCC_HSI_OFF;
    rcc_osc_init_handle.CSIState = RCC_CSI_OFF;
    rcc_osc_init_handle.HSI48State = RCC_HSI48_ON;
    rcc_osc_init_handle.PLL.PLLState = RCC_PLL_ON;
    rcc_osc_init_handle.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    rcc_osc_init_handle.PLL.PLLN = plln;
    rcc_osc_init_handle.PLL.PLLM = pllm;
    rcc_osc_init_handle.PLL.PLLP = pllp;
    rcc_osc_init_handle.PLL.PLLQ = pllq;
    rcc_osc_init_handle.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    rcc_osc_init_handle.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    rcc_osc_init_handle.PLL.PLLFRACN = 0;
    ret=HAL_RCC_OscConfig(&rcc_osc_init_handle);
    if(ret != HAL_OK)
    {
        return 1;
    }

    /*
     *  选择PLL的输出作为系统时钟
     *  配置RCC_CLOCKTYPE_SYSCLK系统时钟,480M
     *  配置RCC_CLOCKTYPE_HCLK 时钟,240Mhz,对应AHB1，AHB2，AHB3和AHB4总线
     *  配置RCC_CLOCKTYPE_PCLK1时钟,120Mhz,对应APB1总线
     *  配置RCC_CLOCKTYPE_PCLK2时钟,120Mhz,对应APB2总线
     *  配置RCC_CLOCKTYPE_D1PCLK1时钟,120Mhz,对应APB3总线
     *  配置RCC_CLOCKTYPE_D3PCLK1时钟,120Mhz,对应APB4总线
     */

    rcc_clk_init_handle.ClockType = (RCC_CLOCKTYPE_SYSCLK \
                                    | RCC_CLOCKTYPE_HCLK \
                                    | RCC_CLOCKTYPE_PCLK1 \
                                    | RCC_CLOCKTYPE_PCLK2 \
                                    | RCC_CLOCKTYPE_D1PCLK1 \
                                    | RCC_CLOCKTYPE_D3PCLK1);

    rcc_clk_init_handle.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    rcc_clk_init_handle.SYSCLKDivider = RCC_SYSCLK_DIV1;
    rcc_clk_init_handle.AHBCLKDivider = RCC_HCLK_DIV2;
    rcc_clk_init_handle.APB1CLKDivider = RCC_APB1_DIV2; 
    rcc_clk_init_handle.APB2CLKDivider = RCC_APB2_DIV2; 
    rcc_clk_init_handle.APB3CLKDivider = RCC_APB3_DIV2;  
    rcc_clk_init_handle.APB4CLKDivider = RCC_APB4_DIV2; 
    ret = HAL_RCC_ClockConfig(&rcc_clk_init_handle, FLASH_LATENCY_4);
    if(ret != HAL_OK)
    {
        return 1;
    }

    /*
     *  配置PLL2的R分频输出, 为220Mhz
     *  配置FMC时钟源是PLL2R
     *  配置QSPI时钟源是PLL2R
     *  配置串口1 和 串口6 的时钟源来自: PCLK2 = 120Mhz
     *  配置串口2 / 3 / 4 / 5 / 7 / 8 的时钟源来自: PCLK1 = 120Mhz
     *  USB工作需要48MHz的时钟,可以由PLL1Q,PLL3Q和HSI48提供,这里配置时钟源是HSI48
     */
    rcc_periph_clk_init_handle.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART1
                                                      | RCC_PERIPHCLK_USB| RCC_PERIPHCLK_QSPI
                                                      | RCC_PERIPHCLK_FMC;
    rcc_periph_clk_init_handle.PLL2.PLL2M = 8;
    rcc_periph_clk_init_handle.PLL2.PLL2N = 440;
    rcc_periph_clk_init_handle.PLL2.PLL2P = 2;
    rcc_periph_clk_init_handle.PLL2.PLL2Q = 2;
    rcc_periph_clk_init_handle.PLL2.PLL2R = 2;
    rcc_periph_clk_init_handle.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
    rcc_periph_clk_init_handle.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    rcc_periph_clk_init_handle.PLL2.PLL2FRACN = 0;
    rcc_periph_clk_init_handle.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
    rcc_periph_clk_init_handle.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
    rcc_periph_clk_init_handle.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    rcc_periph_clk_init_handle.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    rcc_periph_clk_init_handle.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    ret = HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_handle);
    if(ret != HAL_OK)
    {
        return 1;
    }

    sys_qspi_enable_memmapmode(0);          /* 使能QSPI内存映射模式, FLASH容量为普通类型 */

    HAL_PWREx_EnableUSBVoltageDetector();   /* 使能USB电压电平检测器 */
    __HAL_RCC_CSI_ENABLE() ;                /* 使能CSI时钟 */
    __HAL_RCC_SYSCFG_CLK_ENABLE() ;         /* 使能SYSCFG时钟 */
    HAL_EnableCompensationCell();           /* 使能IO补偿单元 */
    return 0;
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief       当编译提示出错的时候此函数用来报告错误的文件和所在行
 * @param       file：指向源文件
 *              line：指向在文件中的行数
 * @retval      无
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif

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
    GPIO_InitTypeDef qspi_gpio;

    __HAL_RCC_GPIOB_CLK_ENABLE();                            /* 使能PORTB时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE();                            /* 使能PORTD时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE();                            /* 使能PORTE时钟 */
    __HAL_RCC_QSPI_CLK_ENABLE();                             /* QSPI时钟使能 */

    qspi_gpio.Pin = GPIO_PIN_6;                              /* PB6 AF10 */
    qspi_gpio.Mode = GPIO_MODE_AF_PP;
    qspi_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    qspi_gpio.Pull = GPIO_PULLUP;
    qspi_gpio.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_2;                              /* PB2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13; /* PD11,12,13 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOD, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_2;                              /* PE2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOE, &qspi_gpio);

    /* QSPI设置，参考QSPI实验的QSPI_Init函数 */
    RCC->AHB3RSTR |= 1 << 14;       /* 复位QSPI */
    RCC->AHB3RSTR &= ~(1 << 14);    /* 停止复位QSPI */

    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    /* QSPI时钟源已经在sys_stm32_clock_init()函数中设置 */
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



