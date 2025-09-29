/**
 ****************************************************************************************************
 * @file        usbh_conf.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       usbh_conf 驱动代码
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
 * V1.0 20200406
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "usbh_conf.h"
#include "usbh_core.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"


HCD_HandleTypeDef hhcd;


/**
 * @brief       初始化HSI48和CRS校准,选择HSI48作为USB时钟源(带CRS自动校准)
 * @param       无
 * @retval      无
 */
void usbh_otg_hsi48_crs_init(void)
{
    RCC->CR |= 1 << 12;             /* HSI480N=1,使能HSI48 */

    while ((RCC->CR & (1 << 13)) == 0); /* 等待HSI48RDY=1,等待HSI48稳定 */

    RCC->APB1HENR |= 1 << 1;        /* CRSEN=1,使能CRS */
    RCC->APB1HRSTR |= 1 << 1;       /* CRSRST=1,复位CRS */
    RCC->APB1HRSTR &= ~(1 << 1);    /* CRSRST=0,取消复位 */
    CRS->CFGR &= ~(3 << 28);        /* SYNCSRC[1:0]=0,选择USB2 SOF作为SYNC信号 */
    CRS->CR |= 3 << 5;              /* CEN和AUTIOTRIMEN都为1,使能自动校准以及计数器 */
    RCC->D2CCIP2R &= ~(3 << 20);    /* USBSEL[1:0]=0,清零原来的设置 */
    RCC->D2CCIP2R |= 3 << 20;       /* USBSEL[1:0]=3,USB时钟源来自hsi48_ck */
}

/**
 * @brief       初始化HCD MSP
 *   @note      这是一个回调函数, 在stm32h7xx_hal_hcd.c里面调用
 * @param       hhcd        : HCD结构体指针
 * @retval      无
 */
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
    if (hhcd->Instance == USB2_OTG_FS)
    {
        RCC->AHB4ENR |= 1 << 0;             /* 使能PORTA时钟 */
        RCC->AHB1ENR |= 1 << 27;            /* 使能USB2 OTG时钟 */
        PWR->CR3 |= 1 << 24;                /* 使能USB VDD3电压检测 */
        usbh_otg_hsi48_crs_init();          /* 设置USB时钟来自hsi48_ck,使能CRS */

        sys_gpio_set(GPIOA, SYS_GPIO_PIN11,
                     SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_NONE); /* PA11引脚模式设置,复用功能 */

        sys_gpio_set(GPIOA, SYS_GPIO_PIN12,
                     SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_NONE); /* PA12引脚模式设置,复用功能 */

        sys_gpio_af_set(GPIOA, SYS_GPIO_PIN11, 10);     /* PA11, AF10(USB) */
        sys_gpio_af_set(GPIOA, SYS_GPIO_PIN12, 10);     /* PA12, AF10(USB) */

        sys_nvic_init(0, 3, OTG_FS_IRQn, 2);            /* 优先级设置为抢占0,子优先级3，组2 */
    }
    else if (hhcd->Instance == USB1_OTG_HS)
    {
        /* USB1 OTG本例程没用到,故不做处理 */
    }
}

/**
 * @brief       USB OTG 中断服务函数
 *   @note      处理所有USB中断
 * @param       无
 * @retval      无
 */
void OTG_FS_IRQHandler(void)
{
    HAL_HCD_IRQHandler(&hhcd);
}

/******************************************************************************************/
/* 以下是: USBH LL HCD 驱动的回调函数(HCD->USB Host Library) */

/**
 * @brief       USBH SOF回调函数
 * @param       hhcd        : HCD结构体指针
 * @retval      无
 */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
    USBH_LL_IncTimer(hhcd->pData);
}

/**
 * @brief       USBH 连接成功回调函数
 * @param       hhcd        : HCD结构体指针
 * @retval      无
 */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
    printf("Connected!\r\n\r\n");
    USBH_LL_Connect(hhcd->pData);
}

/**
 * @brief       USBH 断开连接回调函数
 * @param       hhcd        : HCD结构体指针
 * @retval      无
 */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
    printf("Disconnected!\r\n\r\n");
    USBH_LL_Disconnect(hhcd->pData);
}

/**
 * @brief       USBH 接口使能回调函数(V3.3.3 USB库新增回调函数)
 * @param       hhcd        : HCD结构体指针
 * @retval      无
 */
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
    USBH_LL_PortEnabled(hhcd->pData);
}

/**
 * @brief       USBH 接口关闭回调函数(V3.3.3 USB库新增回调函数)
 * @param       hhcd        : HCD结构体指针
 * @retval      无
 */
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
    USBH_LL_PortDisabled(hhcd->pData);
}

/**
 * @brief       USBH 通知URB变化回调函数
 * @param       hhcd        : HCD结构体指针
 * @param       chnum       : 端点编号
 * @param       urb_state   : URB状态
 * @retval      无
 */
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
    /* To be used with OS to sync URB state with the global state machine */
}


/******************************************************************************************/
/* 以下是: USBH LL HCD 驱动接口函数(HCD->USB Host Library) */

/**
 * @brief       USBH 底层初始化函数
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{
#ifdef USE_USB_FS
    /* 设置LL驱动相关参数 */
    hhcd.Instance = USB2_OTG_FS;            /* 使用USB2 OTG */
    hhcd.Init.Host_channels = 11;           /* 主机通道数为11个 */
    hhcd.Init.dma_enable = 0;               /* 不使用DMA */
    hhcd.Init.low_power_enable = 0;         /* 不使能低功耗模式 */
    hhcd.Init.phy_itface = HCD_PHY_EMBEDDED;/* 使用内部PHY */
    hhcd.Init.Sof_enable = 0;               /* 禁止SOF中断 */
    hhcd.Init.speed = HCD_SPEED_FULL;       /* USB全速(12Mbps) */
    hhcd.Init.vbus_sensing_enable = 0;      /* 不使能VBUS检测 */
    hhcd.Init.lpm_enable = 0;               /* 使能连接电源管理 */

    hhcd.pData = phost;                     /* hhcd的pData指向phost */
    phost->pData = &hhcd;                   /* phost的pData指向hhcd */

    HAL_HCD_Init(&hhcd);                    /* 初始化LL驱动 */
#endif

#ifdef USE_USB_HS
    /* 未实现 */
#endif
    USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd));
    return USBH_OK;
}

/**
 * @brief       USBH 底层取消初始化(恢复默认复位状态)函数
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
    HAL_HCD_DeInit(phost->pData);
    return USBH_OK;
}

/**
 * @brief       USBH 底层驱动开始工作
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
    HAL_HCD_Start(phost->pData);
    return USBH_OK;
}

/**
 * @brief       USBH 底层驱动停止工作
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
    HAL_HCD_Stop(phost->pData);
    return USBH_OK;
}

/**
 * @brief       USBH 获取USB设备的速度
 * @param       phost       : USBH句柄指针
 * @retval      USBH设备速度
 */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
    USBH_SpeedTypeDef speed = USBH_SPEED_FULL;

    switch (HAL_HCD_GetCurrentSpeed(phost->pData))
    {
        case 0:
            speed = USBH_SPEED_HIGH;
            printf("USB Host [HS]\r\n");
            break;

        case 1:
            speed = USBH_SPEED_FULL;
            printf("USB Host [FS]\r\n");
            break;

        case 2:
            speed = USBH_SPEED_LOW;
            printf("USB Host [LS]\r\n");
            break;

        default:
            speed = USBH_SPEED_FULL;
            printf("USB Host [FS]\r\n");
            break;
    }

    return speed;
}

/**
 * @brief       USBH 复位USB HOST端口
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost)
{
    HAL_HCD_ResetPort(phost->pData);
    printf("USB Reset Port\r\n");
    return USBH_OK;
}

/**
 * @brief       USBH 获取最近一次传输的包大小
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      包大小
 */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

/**
 * @brief       USBH 打开一个管道(通道)
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @param       epnum       : 端点号
 * @param       dev_address : 设备地址
 * @param       speed       : 设备速度
 * @param       ep_type     : 端点类型
 * @param       mps         : 端点最大包大小
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost,
                                    uint8_t pipe,
                                    uint8_t epnum,
                                    uint8_t dev_address,
                                    uint8_t speed,
                                    uint8_t ep_type, uint16_t mps)
{
    HAL_HCD_HC_Init(phost->pData, pipe, epnum, dev_address, speed, ep_type, mps);
    return USBH_OK;
}

/**
 * @brief       USBH 关闭一个管道(通道)
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    HAL_HCD_HC_Halt(phost->pData, pipe);
    return USBH_OK;
}

/**
 * @brief       USBH 提交一个新的URB
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @param       direction   : 方向设置
 * @param       ep_type     : 端点类型
 * @param       token       : 端点标志
 * @param       pbuff       : URB数据区首地址
 * @param       length      : URB数据长度
 * @param       do_ping     : 激活do ping protocol,USB HS才用到
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost,
                                     uint8_t pipe,
                                     uint8_t direction,
                                     uint8_t ep_type,
                                     uint8_t token,
                                     uint8_t *pbuff,
                                     uint16_t length, uint8_t do_ping)
{
    HAL_HCD_HC_SubmitRequest(phost->pData, pipe, direction, ep_type, token, pbuff, length, do_ping);
    return USBH_OK;
}

/**
 * @brief       USBH 获取URB状态
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      URB状态
 */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    return (USBH_URBStateTypeDef) HAL_HCD_HC_GetURBState(phost->pData, pipe);
}

/**
 * @brief       USBH 控制VBUS状态
 * @param       phost       : USBH句柄指针
 * @param       state       : vbus状态. 0,激活; 1,不激活
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
#ifdef USE_USB_FS

    if (state == 0)
    {
    }
    else
    {
    }

#endif
    HAL_Delay(500);
    return USBH_OK;
}

/**
 * @brief       USBH 设置管道的翻转
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @param       toggle      : 翻转状态
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
    if (hhcd.hc[pipe].ep_is_in)
    {
        hhcd.hc[pipe].toggle_in = toggle;
    }
    else
    {
        hhcd.hc[pipe].toggle_out = toggle;
    }

    return USBH_OK;
}

/**
 * @brief       USBH 获取管道翻转状态
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      翻转状态
 */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    uint8_t toggle = 0;

    if (hhcd.hc[pipe].ep_is_in)
    {
        toggle = hhcd.hc[pipe].toggle_in;
    }
    else
    {
        toggle = hhcd.hc[pipe].toggle_out;
    }

    return toggle;
}

/**
 * @brief       USBH 延时函数(以ms为单位)
 * @param       Delay       : 延时的ms数
 * @retval      翻转状态
 */
void USBH_Delay(uint32_t Delay)
{
    delay_ms(Delay);
}
























