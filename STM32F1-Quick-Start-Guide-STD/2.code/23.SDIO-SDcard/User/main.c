#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_sdio.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>
#include <string.h>

/* 硬件配置 */
#define SD_DETECT_PIN     GPIO_Pin_8
#define SD_DETECT_PORT    GPIOC
#define SD_DETECT_CLK     RCC_APB2Periph_GPIOC

/* 调试串口配置 */
#define DEBUG_USART       USART1
#define DEBUG_USART_CLK   RCC_APB2Periph_USART1
#define DEBUG_USART_TX    GPIO_Pin_9
#define DEBUG_USART_RX    GPIO_Pin_10
#define DEBUG_USART_PORT  GPIOA

/* SD卡状态定义 */
typedef enum {
    SD_OK = 0,
    SD_ERROR,
    SD_TIMEOUT,
    SD_ERROR_CMD_CRC,
    SD_ERROR_DATA_CRC,
    SD_ERROR_RESP_TIMEOUT,
    SD_ERROR_VOLTAGE,
    SD_ERROR_CMD_RSP,
    SD_CARD_LOCKED,
    SD_INVALID_VOLTRANGE
} SDStatus;

/* SD卡信息结构 */
typedef struct {
    uint32_t CardType;     // 卡类型
    uint32_t RCA;          // 相对卡地址
    uint32_t Capacity;     // 容量(块数)
    uint32_t BlockSize;    // 块大小
} SD_CardInfo;

/* 全局变量 */
static SD_CardInfo SDCardInfo;
static volatile uint32_t SysTickCount = 0;

/* 函数声明 */
void SystemClock_Config(void);
void SysTick_Init(void);
void USART_Config(void);
void GPIO_Config(void);
void SDIO_GPIO_Init(void);
void SDIO_Config(void);
SDStatus SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t response);
SDStatus SD_GetResponse(uint32_t *response);
SDStatus SD_Init(void);
SDStatus SD_ReadBlock(uint8_t *buffer, uint32_t block, uint16_t count);
SDStatus SD_WriteBlock(uint8_t *buffer, uint32_t block, uint16_t count);
uint8_t SD_IsDetected(void);
void SD_DMA_Init(void);
void SDIO_Cmd(FunctionalState NewState);
void DMA2_Channel4_5_IRQHandler(void);
void Delay_ms(uint32_t ms);
void SD_Test(void);

/***************************** 串口重定向 *****************************/
// 确保printf正常工作
#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
    USART_SendData(DEBUG_USART, (uint8_t)ch);
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
    return ch;
}

/***************************** 系统初始化 *****************************/
void SystemClock_Config(void) {
    // 设置系统时钟为72MHz
    SystemInit();  // 使用标准库提供的初始化
    
    // 启用外部晶振
    RCC_HSEConfig(RCC_HSE_ON);
    while (!RCC_WaitForHSEStartUp());
    
    // 配置PLL
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    // 切换系统时钟到PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);
    
    // 设置AHB、APB1、APB2时钟
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
}

void SysTick_Init(void) {
    // 配置SysTick为1ms中断
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
    NVIC_SetPriority(SysTick_IRQn, 0);
}

void USART_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置TX引脚
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_PORT, &GPIO_InitStructure);
    
    // 配置RX引脚
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_PORT, &GPIO_InitStructure);
    
    // 配置USART
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(DEBUG_USART, &USART_InitStructure);
    
    USART_Cmd(DEBUG_USART, ENABLE);
    
    // 确保串口已准备好
    USART_ClearFlag(DEBUG_USART, USART_FLAG_TC);
}

void GPIO_Config(void) {
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | 
                          RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    
    // 配置SD卡检测引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SD_DETECT_PORT, &GPIO_InitStructure);
    
    // 关闭JTAG以释放引脚（如果需要）
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

/***************************** SDIO驱动 *****************************/
void SDIO_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置SDIO CMD引脚(PD2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // 配置SDIO CLK引脚(PC12)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置SDIO数据引脚(PC8-11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 可选：配置上拉电阻（如果硬件上没有）
    GPIO_WriteBit(GPIOC, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
}

// 实现SDIO_Cmd函数
void SDIO_Cmd(FunctionalState NewState) {
    if (NewState != DISABLE) {
        SDIO->CLKCR |= SDIO_CLKCR_CLKEN;  // 使能SDIO时钟
    } else {
        SDIO->CLKCR &= ~SDIO_CLKCR_CLKEN; // 禁用SDIO时钟
    }
}

void SDIO_Config(void) {
    SDIO_InitTypeDef SDIO_InitStructure;
    
    // 使能SDIO时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);
    
    // 初始配置为低速(400kHz)
    SDIO_InitStructure.SDIO_ClockDiv = 0xB4;   // 72MHz/(180+2) ≈ 400kHz
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_Init(&SDIO_InitStructure);
    
    // 使能SDIO
    SDIO_Cmd(ENABLE);
}

SDStatus SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t response) {
    SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
    uint32_t timeout = 0xFFFF;
    
    // 清除所有标志
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND);
    
    // 配置命令
    SDIO_CmdInitStructure.SDIO_Argument = arg;
    SDIO_CmdInitStructure.SDIO_CmdIndex = cmd;
    SDIO_CmdInitStructure.SDIO_Response = response;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    
    // 等待响应或超时
    while (!(SDIO->STA & (SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CCRCFAIL))) {
        if (timeout-- == 0) return SD_TIMEOUT;
    }
    
    // 检查错误标志
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT)) {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_TIMEOUT;
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL)) {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_ERROR_CMD_CRC;
    }
    
    return SD_OK;
}

SDStatus SD_GetResponse(uint32_t *response) {
    if (response == NULL) return SD_ERROR;
    *response = SDIO_GetResponse(SDIO_RESP1);
    return SD_OK;
}

SDStatus SD_Init(void) {
    SDStatus status;
    uint32_t response = 0;
    uint8_t validvoltage = 0;
    uint32_t startTime = SysTickCount;
    
    // 初始化GPIO
    SDIO_GPIO_Init();
    
    // 配置SDIO外设
    SDIO_Config();
    
    // 1. 发送CMD0使SD卡进入空闲状态
    status = SD_SendCommand(0, 0, SDIO_Response_No); // CMD0
    if (status != SD_OK) {
        printf("CMD0 failed: %d\r\n", status);
        return status;
    }
    
    // 2. 发送CMD8检查电压范围
    status = SD_SendCommand(8, 0x1AA, SDIO_Response_Short); // CMD8
    if (status == SD_OK) {
        SD_GetResponse(&response);
        if ((response & 0xFFF) != 0x1AA) {
            printf("Voltage mismatch: 0x%lX\r\n", response);
            return SD_ERROR_VOLTAGE;
        }
        validvoltage = 1;
        printf("CMD8 response: 0x%lX\r\n", response);
    } else {
        printf("CMD8 failed: %d\r\n", status);
    }
    
    // 3. 初始化SD卡
    do {
        if (SysTickCount - startTime > 1000) {
            printf("ACMD41 timeout\r\n");
            return SD_TIMEOUT; // 1秒超时
        }
        
        // 发送CMD55
        status = SD_SendCommand(55, 0, SDIO_Response_Short); // CMD55
        if (status != SD_OK) {
            printf("CMD55 failed: %d\r\n", status);
            continue;
        }
        
        // 发送ACMD41
        status = SD_SendCommand(41, validvoltage ? 0x40300000 : 0x00, 
                               SDIO_Response_Short); // ACMD41
        if (status != SD_OK) {
            printf("ACMD41 failed: %d\r\n", status);
            continue;
        }
        
        SD_GetResponse(&response);
        printf("ACMD41 response: 0x%lX\r\n", response);
        
    } while ((response & 0x80000000) == 0); // 等待初始化完成
    
    // 检查卡类型
    SDCardInfo.CardType = (response & 0x40000000) ? 2 : 1; // 2: SDHC, 1: SDSC
    printf("Card type: %s\r\n", SDCardInfo.CardType == 2 ? "SDHC/SDXC" : "SDSC");
    
    // 4. 获取CID (CMD2)
    status = SD_SendCommand(2, 0, SDIO_Response_Long); // CMD2
    if (status != SD_OK) {
        printf("CMD2 failed: %d\r\n", status);
        return status;
    }
    
    // 5. 获取相对地址 (CMD3)
    status = SD_SendCommand(3, 0, SDIO_Response_Short); // CMD3
    if (status != SD_OK) {
        printf("CMD3 failed: %d\r\n", status);
        return status;
    }
    
    SD_GetResponse(&response);
    SDCardInfo.RCA = response >> 16;
    printf("RCA: 0x%lX\r\n", SDCardInfo.RCA);
    
    // 6. 选择卡 (CMD7)
    status = SD_SendCommand(7, SDCardInfo.RCA << 16, SDIO_Response_Short); // CMD7
    if (status != SD_OK) {
        printf("CMD7 failed: %d\r\n", status);
        return status;
    }
    
    // 7. 设置总线宽度为4位
    // 发送CMD55
    status = SD_SendCommand(55, SDCardInfo.RCA << 16, SDIO_Response_Short); // CMD55
    if (status != SD_OK) {
        printf("CMD55 (bus width) failed: %d\r\n", status);
        return status;
    }
    
    // 发送ACMD6
    status = SD_SendCommand(6, 0x2, SDIO_Response_Short); // ACMD6
    if (status != SD_OK) {
        printf("ACMD6 failed: %d\r\n", status);
        return status;
    }
    
    // 设置高速模式(24MHz)
    SDIO_InitTypeDef SDIO_InitStruct;
    SDIO_StructInit(&SDIO_InitStruct);
    SDIO_InitStruct.SDIO_ClockDiv = 2; // 72MHz / (2+2) = 18MHz
    SDIO_InitStruct.SDIO_BusWide = SDIO_BusWide_4b;
    SDIO_Init(&SDIO_InitStruct);
    
    // 初始化DMA
    SD_DMA_Init();
    
    // 设置块大小为512字节
    status = SD_SendCommand(16, 512, SDIO_Response_Short); // CMD16
    if (status != SD_OK) {
        printf("CMD16 failed: %d\r\n", status);
        return status;
    }
    
    printf("SD Card initialized successfully!\r\n");
    return SD_OK;
}

/***************************** DMA驱动 *****************************/
static volatile uint8_t dmaTransferComplete = 0;

void SD_DMA_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能DMA2时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    
    // 配置DMA通道4 (SDIO RX)
    DMA_DeInit(DMA2_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO;
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    
    // 配置DMA中断
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Channel4, DMA_IT_TC, ENABLE);
    
    // 配置DMA通道5 (SDIO TX)
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
}

SDStatus SD_ReadBlock(uint8_t *buffer, uint32_t block, uint16_t count) {
    SDIO_DataInitTypeDef SDIO_DataInitStructure;
    uint32_t timeout;
    
    // 对于标准容量卡，块地址需要乘以块大小
    if (SDCardInfo.CardType == 1) { // SDSC
        block *= 512;
    }
    
    // 发送读命令
    SDStatus status = SD_SendCommand(count > 1 ? 18 : 17, block, SDIO_Response_Short);
    if (status != SD_OK) {
        printf("Read cmd failed: %d\r\n", status);
        return status;
    }
    
    // 配置数据路径
    SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFF;
    SDIO_DataInitStructure.SDIO_DataLength = count * 512;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);
    
    // 配置DMA
    DMA_Cmd(DMA2_Channel4, DISABLE);
    DMA2_Channel4->CMAR = (uint32_t)buffer;
    DMA2_Channel4->CNDTR = (count * 512) / 4;
    DMA_Cmd(DMA2_Channel4, ENABLE);
    
    // 使能DMA传输
    SDIO_DMACmd(ENABLE);
    
    // 等待传输完成
    dmaTransferComplete = 0;
    timeout = 0xFFFFF;
    while (!dmaTransferComplete && timeout--) {
        if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_RXOVERR)) {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_RXOVERR);
            return SD_ERROR_DATA_CRC;
        }
    }
    
    if (!timeout) {
        printf("Read timeout\r\n");
        return SD_TIMEOUT;
    }
    
    return SD_OK;
}

SDStatus SD_WriteBlock(uint8_t *buffer, uint32_t block, uint16_t count) {
    SDIO_DataInitTypeDef SDIO_DataInitStructure;
    uint32_t timeout;
    
    // 对于标准容量卡，块地址需要乘以块大小
    if (SDCardInfo.CardType == 1) { // SDSC
        block *= 512;
    }
    
    // 发送写命令
    SDStatus status = SD_SendCommand(count > 1 ? 25 : 24, block, SDIO_Response_Short);
    if (status != SD_OK) {
        printf("Write cmd failed: %d\r\n", status);
        return status;
    }
    
    // 配置数据路径
    SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFF;
    SDIO_DataInitStructure.SDIO_DataLength = count * 512;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);
    
    // 配置DMA
    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA2_Channel5->CMAR = (uint32_t)buffer;
    DMA2_Channel5->CNDTR = (count * 512) / 4;
    DMA_Cmd(DMA2_Channel5, ENABLE);
    
    // 使能DMA传输
    SDIO_DMACmd(ENABLE);
    
    // 等待传输完成
    dmaTransferComplete = 0;
    timeout = 0xFFFFF;
    while (!dmaTransferComplete && timeout--) {
        if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR)) {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR);
            return SD_ERROR_DATA_CRC;
        }
    }
    
    if (!timeout) {
        printf("Write timeout\r\n");
        return SD_TIMEOUT;
    }
    
    // 等待写入完成
    timeout = 0xFFFF;
    while (SDIO_GetFlagStatus(SDIO_FLAG_TXACT) && timeout--);
    
    if (!timeout) {
        printf("Write complete timeout\r\n");
        return SD_TIMEOUT;
    }
    
    return SD_OK;
}

/***************************** 中断处理 *****************************/
void SysTick_Handler(void) {
    SysTickCount++;
}

void DMA2_Channel4_5_IRQHandler(void) {
    // 通道4传输完成 (读)
    if (DMA_GetITStatus(DMA2_IT_TC4)) {
        DMA_ClearITPendingBit(DMA2_IT_TC4);
        DMA_Cmd(DMA2_Channel4, DISABLE);
        dmaTransferComplete = 1;
    }
    
    // 通道5传输完成 (写)
    if (DMA_GetITStatus(DMA2_IT_TC5)) {
        DMA_ClearITPendingBit(DMA2_IT_TC5);
        DMA_Cmd(DMA2_Channel5, DISABLE);
        dmaTransferComplete = 1;
    }
}

/***************************** 辅助函数 *****************************/
void Delay_ms(uint32_t ms) {
    uint32_t start = SysTickCount;
    while ((SysTickCount - start) < ms);
}

uint8_t SD_IsDetected(void) {
    return (GPIO_ReadInputDataBit(SD_DETECT_PORT, SD_DETECT_PIN) == Bit_RESET);
}

/***************************** SD卡测试 *****************************/
void SD_Test(void) {
    SDStatus status;
    uint8_t writeBuffer[512];
    uint8_t readBuffer[512];
    uint32_t i;
    
    printf("\r\nStarting SD Card Test...\r\n");
    
    // 1. 检测SD卡是否存在
    if (!SD_IsDetected()) {
        printf("Error: No SD card detected!\r\n");
        return;
    }
    printf("SD card detected.\r\n");
    
    // 2. 初始化SD卡
    printf("Initializing SD card...\r\n");
    status = SD_Init();
    
    if (status != SD_OK) {
        printf("Initialization failed: %d\r\n", status);
        return;
    }
    
    // 3. 准备测试数据
    for (i = 0; i < 512; i++) {
        writeBuffer[i] = i % 256;
    }
    
    // 4. 写入测试数据 (块地址0x1000)
    printf("Writing test data to block 0x1000...\r\n");
    status = SD_WriteBlock(writeBuffer, 0x1000, 1);
    if (status != SD_OK) {
        printf("Write failed: %d\r\n", status);
        return;
    }
    printf("Write completed.\r\n");
    
    // 5. 读取数据
    printf("Reading data from block 0x1000...\r\n");
    status = SD_ReadBlock(readBuffer, 0x1000, 1);
    if (status != SD_OK) {
        printf("Read failed: %d\r\n", status);
        return;
    }
    printf("Read completed.\r\n");
    
    // 6. 验证数据
    printf("Verifying data...\r\n");
    for (i = 0; i < 512; i++) {
        if (writeBuffer[i] != readBuffer[i]) {
            printf("Data mismatch at byte %lu: Wrote 0x%02X, Read 0x%02X\r\n", 
                   i, writeBuffer[i], readBuffer[i]);
            printf("Verification failed!\r\n");
            return;
        }
    }
    printf("Verification successful! All data matches.\r\n");
    printf("SD card test completed successfully!\r\n");
}

/***************************** 主函数 *****************************/
int main(void) {
    // 系统初始化
    SystemClock_Config();
    SysTick_Init();
    GPIO_Config();
    USART_Config();
    
    // 确保串口正常工作
    printf("\r\nSystem Initialized\r\n");
    printf("Clock Frequency: %lu Hz\r\n", SystemCoreClock);
    printf("This message confirms UART is working!\r\n");
    
    // 运行SD卡测试
    SD_Test();
    
    while (1) {
        // 主循环
        Delay_ms(1000);
        printf("System running...\r\n");
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    printf("Assert failed in %s, line %lu\r\n", file, line);
    while(1);
}
#endif
