#ifndef __SPI_H__
#define __SPI_H__

#include "sys.h"

/* SPI总线速度设置 */
#define SPI_SPEED_2     0
#define SPI_SPEED_4     1
#define SPI_SPEED_8     2
#define SPI_SPEED_16    3
#define SPI_SPEED_32    4
#define SPI_SPEED_64    5
#define SPI_SPEED_128   6
#define SPI_SPEED_256   7

void spi_init(void);
void spi1_set_speed(uint8_t speed);
uint8_t spi1_read_write_byte(uint8_t txdata);

#endif /* __SPI_H__ */
