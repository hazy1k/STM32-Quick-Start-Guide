#include "i2c.h"
#include "delay.h"

// I2C延时
static void i2c_delay(void)
{
    delay_us(2);
}

// I2C起始信号
void i2c_start(void)
{
    I2C_SDA(1);
    I2C_SCL(1);
    i2c_delay();
    I2C_SDA(0); /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
    i2c_delay();
    I2C_SCL(0); /* 钳住I2C总线，准备发送或接收数据 */
    i2c_delay();
}

void i2c_stop(void)
{
    I2C_SDA(0); /* STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号 */
    i2c_delay();
    I2C_SCL(1);
    i2c_delay();
    I2C_SDA(1); /* 发送I2C总线结束信号 */
    i2c_delay();
}

// 等待应答信号 1:接收应答失败 0:接收应答成功
uint8_t i2c_wait_ack(void)
{
    uint8_t wait_time = 0;
    uint8_t wait_ack = 0;
    I2C_SDA(1);
    i2c_delay();
    I2C_SCL(1);
    i2c_delay();
    while(I2C_READ_SDA)
    {
        wait_time++;
        if(wait_time > 250)
        {
            i2c_stop();
            wait_ack = 1;
            break;
        }
    }
    I2C_SCL(0);
    i2c_delay();
    return wait_ack;
}

// 产生ACK应答
void i2c_ack(void)
{
    I2C_SDA(0);
    i2c_delay();
    I2C_SCL(1);
    i2c_delay();
    I2C_SCL(0);
    i2c_delay();
    I2C_SDA(1);
    i2c_delay();
}

// 产生NACK应答
void i2c_nack(void)
{
    I2C_SDA(1);
    i2c_delay();
    I2C_SCL(1);
    i2c_delay();
    I2C_SCL(0);
    i2c_delay();
}

// I2C发送一个字节
void i2c_send_byte(uint8_t data)
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        I2C_SDA((data&0x80)>>7);
        i2c_delay();
        I2C_SCL(1);
        i2c_delay();
        I2C_SCL(0);
        data <<= 1;
    }
    I2C_SDA(1);
}

// I2C读取一个字节
// ack=1时，发送ack; ack=0时，发送nack
uint8_t i2c_read_byte(uint8_t ack)
{
    uint8_t i,reve = 0;
    for(i=0;i<8;i++)
    {
        reve <<= 1;
        I2C_SCL(1);
        i2c_delay();
        if(I2C_READ_SDA)
        {
            reve++;
        }
        I2C_SCL(0);
        i2c_delay();
    }
    if(!ack)
    {
        i2c_nack();
    }
    else
    {
        i2c_ack();
    }
    return reve;
}

// I2C GPIO初始化
void i2c_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    // SCL-PB8 SDA-PB9
    GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出
    GPIO_InitStructure.Pull = GPIO_PULLUP; // 上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_9;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    i2c_stop();
}

