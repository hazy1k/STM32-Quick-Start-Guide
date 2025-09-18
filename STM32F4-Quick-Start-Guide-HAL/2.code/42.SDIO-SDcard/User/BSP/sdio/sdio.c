#include "sdio.h"

SD_HandleTypeDef sdcard_handle;
HAL_SD_CardInfoTypeDef sdcard_info_handle;

#define  SDIO_TRANSF_CLK_DIV  1   

uint8_t sd_init(void)
{
    uint8_t SD_status;
    sdcard_handle.Instance = SDIO;
    sdcard_handle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING; // 采样时钟沿上升沿
    sdcard_handle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE; // 空闲时不关闭时钟电源
    sdcard_handle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE; // 不使用Bypass模式
    sdcard_handle.Init.BusWide = SDIO_BUS_WIDE_1B; // 1bit模式
    sdcard_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE; // 不使用硬件流控
    /*  
    *  SD传输时钟分频，由于HAL库运行效率低，很容易产生上溢（读SD卡时）/下溢错误（写SD卡时）
    *  使用4bit模式时，需降低SDIO时钟频率，将该宏改为 1，SDIO时钟频率：48/( SDIO_TRANSF_CLK_DIV + 2 ) = 16M * 4bit = 64Mbps 
    *  使用1bit模式时，该宏SDIO_TRANSF_CLK_DIV改为 0，SDIO时钟频率：48/( SDIO_TRANSF_CLK_DIV + 2 ) = 24M * 1bit = 24Mbps 
    */
    sdcard_handle.Init.ClockDiv = SDIO_TRANSF_CLK_DIV;
    SD_status = HAL_SD_Init(&sdcard_handle);
    if(SD_status != HAL_OK)
    {
        return 1;
    }
    HAL_SD_GetCardInfo(&sdcard_handle, &sdcard_info_handle); // 获取SD卡信息
    SD_status = HAL_SD_ConfigWideBusOperation(&sdcard_handle, SDIO_BUS_WIDE_4B); // 开启4bit模式
    {
        return 2;
    }
    return 0;
}

// SDIO GPIO配置
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_SDIO_CLK_ENABLE();
    // D0-PC8 D1-PC9 D2-PC10 D3-PC11 CLK-PC12 CMD-PD2
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_Initure.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Initure.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
    GPIO_Initure.Pin = GPIO_PIN_2;
    GPIO_Initure.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);
}

/**
 * @brief       获取卡信息函数
 * @param       cardinfo:SD卡信息句柄
 * @retval      返回值:读取卡信息状态值
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&sdcard_handle, cardinfo);
    return sta;
}

/**
 * @brief       判断SD卡是否可以传输(读写)数据
 * @param       无
 * @retval      返回值:SD_TRANSFER_OK      传输完成，可以继续下一次传输
                       SD_TRANSFER_BUSY SD 卡正忙，不可以进行下一次传输
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&sdcard_handle) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       读SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      0, 正常;  其他, 错误代码(详见SD_Error定义);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    __disable_irq();                                                                       
    sta = HAL_SD_ReadBlocks(&sdcard_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); /* 多个sector的读操作 */
    /* 等待SD卡读完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq(); 
    return sta;
}

/**
 * @brief       写SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      0, 正常;  其他, 错误代码(详见SD_Error定义);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;  
    __disable_irq();                                                                        
    sta = HAL_SD_WriteBlocks(&sdcard_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); 
    /* 等待SD卡写完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq();  
    return sta;
}
