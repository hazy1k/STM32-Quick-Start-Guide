#include "sram.h"

SRAM_HandleTypeDef sram_handle;

void sram_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAM_TimingTypeDef fsmc_handle;
    __HAL_RCC_FSMC_CLK_ENABLE(); 
    __HAL_RCC_GPIOD_CLK_ENABLE(); 
    __HAL_RCC_GPIOE_CLK_ENABLE(); 
    __HAL_RCC_GPIOF_CLK_ENABLE(); 
    __HAL_RCC_GPIOG_CLK_ENABLE();
    // WR-PD5 RD-PD4 CS-PG10
    GPIO_InitStructure.Pin = GPIO_PIN_10;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_5|GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    // PD0,1,4,5,8~15
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    // PE0,1,7~15
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    // PF0~5,12~15
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
    // PG0~5,10
    GPIO_InitStructure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    // FSMC Configuration
    sram_handle.Instance = FSMC_NORSRAM_DEVICE;
    sram_handle.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    sram_handle.Init.NSBank = (SRAM_FSMC_NEX == 1) ? FSMC_NORSRAM_BANK1 : \
                              (SRAM_FSMC_NEX == 2) ? FSMC_NORSRAM_BANK2 : \
                              (SRAM_FSMC_NEX == 3) ? FSMC_NORSRAM_BANK3 : FSMC_NORSRAM_BANK4; /* 根据配置选择FSMC_NE1~4 */
    sram_handle.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; // 地址/数据线不复用
    sram_handle.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM; // SRAM
    sram_handle.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; // 16位数据宽度
    sram_handle.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; // 突发访问模式关闭
    sram_handle.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; // 等待信号极性低
    sram_handle.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS; // 等待信号在前
    sram_handle.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE; // 写使能
    sram_handle.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE; // 等待信号关闭
    sram_handle.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE; // 扩展模式关闭
    sram_handle.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; // 异步等待关闭
    sram_handle.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE; // 写突发关闭
    /* FSMC读时序控制寄存器 */
    fsmc_handle.AddressSetupTime = 0x02; // 地址设置时间
    fsmc_handle.AddressHoldTime = 0x00; // 地址保持时间
    fsmc_handle.DataSetupTime = 0x08; // 数据设置时间
    fsmc_handle.BusTurnAroundDuration = 0x00; // 总线转向持续时间
    fsmc_handle.AccessMode = FSMC_ACCESS_MODE_A; // 访问模式A
    HAL_SRAM_Init(&sram_handle, &fsmc_handle, &fsmc_handle); // 初始化SRAM
}

/**
 * @brief       往SRAM指定地址写入指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大32bit)
 * @retval      无
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}

/**
 * @brief       从SRAM指定地址读取指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大32bit)
 * @retval      无
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *pbuf++ = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        addr++;
    }
}


/**
 * @brief       测试函数 在SRAM指定地址写入1个字节
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       data    : 要写入的字节
 * @retval      无
 */
void sram_test_write(uint32_t addr, uint8_t data)
{
    sram_write(&data, addr, 1); /* 写入1个字节 */
}

/**
 * @brief       测试函数 在SRAM指定地址读取1个字节
 * @param       addr    : 开始读取的地址(最大32bit)
 * @retval      读取到的数据(1个字节)
 */
uint8_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    sram_read(&data, addr, 1); /* 读取1个字节 */
    return data;
}
