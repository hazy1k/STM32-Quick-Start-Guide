#include "24c02.h"

void at24cxx_init(void)
{
    i2c_init();
}

/**
 * @brief       在AT24CXX指定地址读出一个数据
 * @param       readaddr: 开始读数的地址
 * @retval      读到的数据
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t temp = 0;
    i2c_start(); // 发送起始信号
    /* 根据不同的24CXX型号, 发送高位地址
     * 1, 24C16以上的型号, 分2个字节发送地址
     * 2, 24C16及以下的型号, 分1个低字节地址 + 占用器件地址的bit1~bit3位 用于表示高位地址, 最多11位地址
     *    对于24C01/02, 其器件地址格式(8bit)为: 1  0  1  0  A2  A1  A0  R/W
     *    对于24C04,    其器件地址格式(8bit)为: 1  0  1  0  A2  A1  a8  R/W
     *    对于24C08,    其器件地址格式(8bit)为: 1  0  1  0  A2  a9  a8  R/W
     *    对于24C16,    其器件地址格式(8bit)为: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : 读/写控制位 0,表示写; 1,表示读;
     *    A0/A1/A2 : 对应器件的1,2,3引脚(只有24C01/02/04/8有这些脚)
     *    a8/a9/a10: 对应存储整列的高位地址, 11bit地址最多可以表示2048个位置, 可以寻址24C16及以内的型号
     */    
    if(MY_TYPE > AT24C16) // 24C16以上的型号, 分2个字节发送地址
    {
        i2c_send_byte(0xA0); // 发送写命令, IIC规定最低位是0, 表示写入 
        i2c_wait_ack(); // 每次发送完一个字节,都要等待ACK 
        i2c_send_byte(addr >> 8); // 发送高字节地址 
    }
    else 
    {
        i2c_send_byte(0xA0 + ((addr >> 8) << 1)); // 发送器件 0xA0 + 高位a8/a9/a10地址,写数据
    }
    
    i2c_wait_ack();           // 每次发送完一个字节,都要等待ACK
    i2c_send_byte(addr % 256);// 发送低位地址
    i2c_wait_ack();           // 等待ACK, 此时地址发送完成了
    i2c_start();              // 重新发送起始信号 
    i2c_send_byte(0xA1);      // 进入接收模式, IIC规定最低位是1, 表示读取
    i2c_wait_ack();           // 次发送完一个字节,都要等待ACK 
    temp = i2c_read_byte(0);  // 接收一个字节数据 
    i2c_stop();               // 产生一个停止条件 
    return temp;
}

/**
 * @brief       在AT24CXX指定地址写入一个数据
 * @param       addr: 写入数据的目的地址
 * @param       data: 要写入的数据
 * @retval      无
 */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    /* 原理说明见:at24cxx_read_one_byte函数, 本函数完全类似 */
    i2c_start();
    if (MY_TYPE > AT24C16)    
    {
        i2c_send_byte(0xA0);    
        i2c_wait_ack();        
        i2c_send_byte(addr >> 8);   
    }
    else
    {
        i2c_send_byte(0xA0 + ((addr >> 8) << 1));   
    }
    i2c_wait_ack();             
    i2c_send_byte(addr % 256);  
    i2c_wait_ack();            
    /* 因为写数据的时候,不需要进入接收模式了,所以这里不用重新发送起始信号了 */
    i2c_send_byte(data); // 发送1字节 
    i2c_wait_ack();            
    i2c_stop();                 
    delay_ms(10); // 注意: EEPROM 写入比较慢,必须等到10ms后再写下一个字节 
}

/**
 * @brief       检查AT24CXX是否正常
 *   @note      检测原理: 在器件的末地址写如0X55, 然后再读取, 如果读取值为0X55
 *              则表示检测正常. 否则,则表示检测失败.
 *
 * @param       无
 * @retval      检测结果
 *              0: 检测成功
 *              1: 检测失败
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = MY_TYPE;
    temp = at24cxx_read_one_byte(addr); // 避免每次开机都写AT24CXX
    if (temp == 0x55)  // 读取数据正常
    {
        return 0;
    }
    else
    {
        at24cxx_write_one_byte(addr, 0x55); // 先写入数据
        temp = at24cxx_read_one_byte(255);  // 再读取数据 
        if (temp == 0x55)return 0;
    }
    return 1;
}

// 在AT24CXX里面指定地址开始读出指定个数的数据
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    while (len--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

// 在AT24CXX里面的指定地址开始写入指定个数的数据
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    while(len--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
