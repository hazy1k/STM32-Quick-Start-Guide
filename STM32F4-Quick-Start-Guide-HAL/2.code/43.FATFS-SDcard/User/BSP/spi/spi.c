#include "spi.h"
#include "usart.h"
#include <stdio.h>

SPI_HandleTypeDef spi_handle;

void spi_init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();
    spi_handle.Instance = SPI1;
    spi_handle.Init.Mode = SPI_MODE_MASTER; // 设置SPI工作模式为主模式
    spi_handle.Init.Direction = SPI_DIRECTION_2LINES; // 设置SPI数据传输方向，2线模式
    spi_handle.Init.DataSize = SPI_DATASIZE_8BIT; // 设置SPI的数据大小为8位
    spi_handle.Init.CLKPolarity = SPI_POLARITY_HIGH; // 时钟极性为高
    spi_handle.Init.CLKPhase = SPI_PHASE_2EDGE; // 第二个时钟沿采样
    spi_handle.Init.NSS = SPI_NSS_SOFT; // 设置NSS信号由硬件管理
    spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // 设置波特率预分频值为256
    spi_handle.Init.TIMode = SPI_TIMODE_DISABLED; // 禁用TI模式
    spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED; // 禁用CRC计算
    spi_handle.Init.CRCPolynomial = 7; // 设置CRC计算多项式为7
    __HAL_SPI_ENABLE(&spi_handle);
    spi1_read_write_byte(0xFF);
    printf("SPI init success!\r\n");
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==SPI1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        // SPI1_SCK PB3 SPI1_MISO PB4 SPI1_MOSI PB5
        GPIO_InitStruct.Pin = GPIO_PIN_3; // SCK复用输出
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_4; // MISO复用输输出
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_5; // MOSI复用输出
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
/**
 * @brief       SPI1速度设置函数
 *   @note      SPI1时钟选择来自APB1, 即PCLK1, 为 42MHz
 *              SPI速度 = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI1时钟分频系数
                        取值为SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      无
 */
void spi1_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed));// 判断有效性 
    __HAL_SPI_DISABLE(&spi_handle);                // 关闭SPI 
    spi_handle.Instance->CR1 &= 0XFFC7;            //位3-5清零，用来设置波特率 
    spi_handle.Instance->CR1 |= speed << 3;        //设置SPI速度
    __HAL_SPI_ENABLE(&spi_handle);                 // 使能SPI 
}

uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&spi_handle, &txdata, &rxdata, 1, 1000);
    return rxdata;
}
