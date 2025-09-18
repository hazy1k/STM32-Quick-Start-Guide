#include "spi_flash.h"

/* =============== 硬件配置 =============== */
static SPI_PinConfig CS_Config = FLASH_CS_CONFIG;
static SPI_PinConfig SCK_Config = FLASH_SCK_CONFIG;
static SPI_PinConfig MISO_Config = FLASH_MISO_CONFIG;
static SPI_PinConfig MOSI_Config = FLASH_MOSI_CONFIG;

/* =============== 局部变量 =============== */
static __IO uint32_t SPITimeout = SPI_FLAG_TIMEOUT;  // SPI操作超时计数器

/* =============== 辅助函数 =============== */

// 简化CS控制宏
#define CS_LOW()    GPIO_ResetBits(CS_Config.port, CS_Config.pin)
#define CS_HIGH()   GPIO_SetBits(CS_Config.port, CS_Config.pin)

/**
  * @brief  SPI超时回调函数
  * @param  errorcode: 错误代码(0=发送超时, 1=接收超时)
  * @retval 总是返回0(用于SPI传输)
  */
static uint8_t SPI_TIMEOUT_Callback(uint8_t errorcode)
{
    FLASH_ERROR("SPI timeout! Error code: %d", errorcode);
    return 0;  // 返回0作为安全值
}

/**
  * @brief  SPI单字节传输
  * @param  data: 要发送的数据
  * @retval 接收到的数据
  */
static uint8_t SPI_TransferByte(uint8_t data)
{
    // 等待发送缓冲区空(TXE=1)
    uint32_t timeout = SPI_FLAG_TIMEOUT;
    while (!SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE)) {
        if (timeout-- == 0) return SPI_TIMEOUT_Callback(0);
    }
    
    // 写入数据到数据寄存器
    SPI_I2S_SendData(FLASH_SPI, data);
    
    // 等待接收缓冲区非空(RXNE=1)
    timeout = SPI_FLAG_TIMEOUT;
    while (!SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE)) {
        if (timeout-- == 0) return SPI_TIMEOUT_Callback(1);
    }
    
    // 读取接收到的数据
    return SPI_I2S_ReceiveData(FLASH_SPI);
}

/**
  * @brief  发送24位地址
  * @param  addr: 24位地址(0-16MB范围)
  */
static void SPI_SendAddress(uint32_t addr)
{
    SPI_TransferByte((addr >> 16) & 0xFF); // 地址高字节(23-16位)
    SPI_TransferByte((addr >> 8) & 0xFF);  // 地址中字节(15-8位)
    SPI_TransferByte(addr & 0xFF);         // 地址低字节(7-0位)
}

/**
  * @brief  等待Flash操作完成
  * @note   轮询状态寄存器，直到WIP标志清除
  */
static void SPI_Flash_WaitBusy(void)
{
    uint8_t status;
    uint32_t timeout = SPI_LONG_TIMEOUT;  // 使用长超时等待擦除/写入完成
    
    do {
        CS_LOW();
        SPI_TransferByte(READ_STATUS_REG);  // 发送读状态寄存器命令
        status = SPI_TransferByte(DUMMY_BYTE); // 读取状态寄存器值
        CS_HIGH();
        
        if (timeout-- == 0) {
            FLASH_ERROR("WaitBusy timeout!");
            break;
        }
    } while (status & WIP_FLAG);  // 检查忙标志(WIP)
}

/* =============== 公共函数 =============== */

/**
  * @brief  初始化SPI Flash
  * @note   配置SPI引脚和SPI外设
  */
void SPI_Flash_Init(void)
{
    SPI_InitTypeDef  spiInit = {0};
    GPIO_InitTypeDef gpioInit = {0};
    
    // 1. 使能时钟 (SPI1和所有GPIO端口)
    RCC_APB2PeriphClockCmd(FLASH_SPI_CLK | CS_Config.rcc_clk | 
                          SCK_Config.rcc_clk | MISO_Config.rcc_clk | 
                          MOSI_Config.rcc_clk, ENABLE);
    
    // 2. 配置CS引脚 (推挽输出)
    gpioInit.GPIO_Pin = CS_Config.pin;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CS_Config.port, &gpioInit);
    
    // 3. 配置SCK和MOSI引脚 (复用推挽输出)
    gpioInit.GPIO_Pin = SCK_Config.pin | MOSI_Config.pin;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_Init(SCK_Config.port, &gpioInit);
    
    // 4. 配置MISO引脚 (浮空输入)
    gpioInit.GPIO_Pin = MISO_Config.pin;
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入模式
    GPIO_Init(MISO_Config.port, &gpioInit);
    
    // 5. 初始CS高电平(不选中芯片)
    CS_HIGH();
    
    // 6. 配置SPI参数
    spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 双线全双工
    spiInit.SPI_Mode = SPI_Mode_Master;         // 主机模式
    spiInit.SPI_DataSize = SPI_DataSize_8b;     // 8位数据
    spiInit.SPI_CPOL = SPI_CPOL_High;           // 时钟极性:空闲时高电平
    spiInit.SPI_CPHA = SPI_CPHA_2Edge;          // 时钟相位:第二个边沿采样
    spiInit.SPI_NSS = SPI_NSS_Soft;             // 软件控制NSS
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI时钟=系统时钟/4
    spiInit.SPI_FirstBit = SPI_FirstBit_MSB;    // 高位在前
    spiInit.SPI_CRCPolynomial = 7;              // CRC多项式(未使用)
    SPI_Init(FLASH_SPI, &spiInit);
    
    // 7. 使能SPI外设
    SPI_Cmd(FLASH_SPI, ENABLE);
    
    FLASH_DEBUG("SPI Flash initialized");
}

/**
  * @brief  读取设备JEDEC ID
  * @retval 24位设备ID
  */
uint32_t SPI_Flash_ReadID(void)
{
    uint32_t id = 0;
    
    CS_LOW();
    SPI_TransferByte(JEDEC_DEVICE_ID);  // 发送JEDEC ID命令
    id = SPI_TransferByte(DUMMY_BYTE) << 16; // 制造商ID
    id |= SPI_TransferByte(DUMMY_BYTE) << 8; // 存储器类型
    id |= SPI_TransferByte(DUMMY_BYTE);      // 容量标识
    CS_HIGH();
    
    FLASH_DEBUG("Device ID: 0x%06X", id);
    return id;
}

/**
  * @brief  擦除扇区 (4KB)
  * @param  sectorAddr: 扇区内的任意地址
  */
void SPI_Flash_EraseSector(uint32_t sectorAddr)
{
    // 确保地址对齐到4KB边界
    sectorAddr = sectorAddr & ~(SPI_FLASH_SectorSize - 1);
    
    // 发送写使能命令
    SPI_Flash_WriteEnable();
    
    // 发送扇区擦除命令
    CS_LOW();
    SPI_TransferByte(SECTOR_ERASE);  // 扇区擦除命令(0x20)
    SPI_SendAddress(sectorAddr);     // 发送24位地址
    CS_HIGH();
    
    // 等待擦除完成
    SPI_Flash_WaitBusy();
    
    FLASH_DEBUG("Sector 0x%06X erased", sectorAddr);
}

/**
  * @brief  页编程 (最大256字节)
  * @param  buffer: 数据缓冲区
  * @param  addr: 写入地址(必须页对齐)
  * @param  size: 写入字节数(不能超过页边界)
  */
void SPI_Flash_WritePage(uint8_t* buffer, uint32_t addr, uint16_t size)
{
    // 确保写入不跨页
    uint16_t pageOffset = addr % SPI_FLASH_PageSize;
    if (size > (SPI_FLASH_PageSize - pageOffset)) {
        size = SPI_FLASH_PageSize - pageOffset;
        FLASH_ERROR("Write size truncated to %d bytes", size);
    }
    
    // 发送写使能命令
    SPI_Flash_WriteEnable();
    
    // 发送页编程命令
    CS_LOW();
    SPI_TransferByte(PAGE_PROGRAM);  // 页编程命令(0x02)
    SPI_SendAddress(addr);           // 发送24位地址
    
    // 写入数据
    for (uint16_t i = 0; i < size; i++) {
        SPI_TransferByte(buffer[i]);
    }
    
    CS_HIGH();
    
    // 等待写入完成
    SPI_Flash_WaitBusy();
    
    FLASH_DEBUG("Written %d bytes to 0x%06X", size, addr);
}

/**
  * @brief  读取数据
  * @param  buffer: 数据缓冲区
  * @param  addr: 读取起始地址
  * @param  size: 读取字节数
  */
void SPI_Flash_ReadBuffer(uint8_t* buffer, uint32_t addr, uint16_t size)
{
    CS_LOW();
    SPI_TransferByte(READ_DATA);  // 标准读命令(0x03)
    SPI_SendAddress(addr);        // 发送24位地址
    
    // 连续读取数据
    for (uint16_t i = 0; i < size; i++) {
        buffer[i] = SPI_TransferByte(DUMMY_BYTE);
    }
    
    CS_HIGH();
    
    FLASH_DEBUG("Read %d bytes from 0x%06X", size, addr);
}

/**
  * @brief  全片擦除
  * @note   耗时较长(典型值几秒)，谨慎使用!
  */
void SPI_Flash_ChipErase(void)
{
    // 发送写使能命令
    SPI_Flash_WriteEnable();
    
    CS_LOW();
    SPI_TransferByte(CHIP_ERASE);  // 整片擦除命令(0xC7)
    CS_HIGH();
    
    // 等待擦除完成
    SPI_Flash_WaitBusy();
    
    FLASH_DEBUG("Chip erase completed");
}

/**
  * @brief  写使能
  * @note   在执行写/擦除操作前必须调用
  */
void SPI_Flash_WriteEnable(void)
{
    CS_LOW();
    SPI_TransferByte(WRITE_ENABLE);  // 写使能命令(0x06)
    CS_HIGH();
}

/**
  * @brief  写禁止
  */
void SPI_Flash_WriteDisable(void)
{
    CS_LOW();
    SPI_TransferByte(WRITE_DISABLE);  // 写禁止命令(0x04)
    CS_HIGH();
}

/**
  * @brief  进入低功耗模式
  */
void SPI_Flash_PowerDown(void)
{
    CS_LOW();
    SPI_TransferByte(POWER_DOWN);  // 掉电命令(0xB9)
    CS_HIGH();
    // 等待命令完成(小延迟)
    for(volatile int i = 0; i < 100; i++);
}

/**
  * @brief  唤醒设备
  */
void SPI_Flash_WakeUp(void)
{
    CS_LOW();
    SPI_TransferByte(RELEASE_POWER_DOWN);  // 唤醒命令(0xAB)
    CS_HIGH();
    // 等待芯片唤醒(典型值3us)
    for(volatile int i = 0; i < 1000; i++);
}

/**
  * @brief  任意位置写入数据 (自动处理页边界)
  * @param  buffer: 数据缓冲区
  * @param  addr: 起始地址
  * @param  size: 写入字节数
  */
void SPI_Flash_WriteBuffer(uint8_t* buffer, uint32_t addr, uint32_t size)
{
    uint32_t currentAddr = addr;
    uint32_t bytesRemaining = size;
    
    while (bytesRemaining > 0) {
        // 计算当前页剩余空间
        uint16_t pageOffset = currentAddr % SPI_FLASH_PageSize;
        uint16_t bytesInPage = SPI_FLASH_PageSize - pageOffset;
        
        // 如果剩余数据小于页剩余空间，则全部写入
        if (bytesInPage > bytesRemaining) {
            bytesInPage = bytesRemaining;
        }
        
        // 写入当前页
        SPI_Flash_WritePage(buffer, currentAddr, bytesInPage);
        
        // 更新指针和计数器
        buffer += bytesInPage;
        currentAddr += bytesInPage;
        bytesRemaining -= bytesInPage;
    }
    
    FLASH_DEBUG("Written %lu bytes starting from 0x%06X", size, addr);
}
