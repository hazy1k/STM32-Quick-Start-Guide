#include "i2c_ee.h"

// 超时处理
static uint8_t I2C_CheckTimeout(uint32_t timeout) {
    while (timeout--) {
        if (!I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY)) {
            return 1; // 操作成功
        }
    }
    return 0; // 超时
}

// I2C初始化
void I2C_EEPROM_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;
    
    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(EEPROM_I2C_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(EEPROM_GPIO_CLK, ENABLE);
    
    // 2. 配置GPIO为开漏输出
    GPIO_InitStruct.GPIO_Pin = EEPROM_SCL_PIN | EEPROM_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(EEPROM_GPIO_PORT, &GPIO_InitStruct);
    
    // 3. 配置I2C参数
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x0A; // 主机地址，可设为任意值
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_ClockSpeed = 400000; // 400kHz
    
    I2C_Init(EEPROM_I2C, &I2C_InitStruct);
    I2C_Cmd(EEPROM_I2C, ENABLE);
}

// 单字节写入
uint8_t I2C_EEPROM_WriteByte(uint16_t addr, uint8_t data) {
    // 1. 生成START条件
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    if (!I2C_CheckTimeout(I2C_TIMEOUT)) return 0;
    
    // 2. 发送设备地址+写模式
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
        return 0;
    
    // 3. 发送内存地址高字节 (对于16位地址)
    I2C_SendData(EEPROM_I2C, (addr >> 8) & 0xFF);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        return 0;
    
    // 4. 发送内存地址低字节
    I2C_SendData(EEPROM_I2C, addr & 0xFF);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        return 0;
    
    // 5. 发送数据
    I2C_SendData(EEPROM_I2C, data);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        return 0;
    
    // 6. 生成STOP条件
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    return 1;
}

// 单字节读取
uint8_t I2C_EEPROM_ReadByte(uint16_t addr, uint8_t* data) {
    // 1. 生成START条件（写模式）
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    if (!I2C_CheckTimeout(I2C_TIMEOUT)) return 0;
    
    // 2. 发送设备地址+写模式
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
        return 0;
    
    // 3. 发送内存地址高字节
    I2C_SendData(EEPROM_I2C, (addr >> 8) & 0xFF);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        return 0;
    
    // 4. 发送内存地址低字节
    I2C_SendData(EEPROM_I2C, addr & 0xFF);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        return 0;
    
    // 5. 重新生成START条件（读模式）
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    if (!I2C_CheckTimeout(I2C_TIMEOUT)) return 0;
    
    // 6. 发送设备地址+读模式
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Receiver);
    if (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        return 0;
    
    // 7. 准备接收单个字节（禁用ACK，生成STOP）
    I2C_AcknowledgeConfig(EEPROM_I2C, DISABLE);
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    
    // 8. 等待数据接收完成
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));
    
    // 9. 读取数据
    *data = I2C_ReceiveData(EEPROM_I2C);
    
    // 10. 重新使能ACK
    I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);
    return 1;
}

// 页写入
uint8_t I2C_EEPROM_WritePage(uint16_t startAddr, uint8_t* data, uint8_t len) {
    // 检查长度是否超过页边界
    uint8_t pageOffset = startAddr % EEPROM_PAGE_SIZE;
    if (len > (EEPROM_PAGE_SIZE - pageOffset)) 
        return 0;
    
    // 1. 生成START条件
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    
    // 等待START完成
    uint32_t timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (timeout-- == 0) return 0;
    }
    
    // 2. 发送设备地址+写模式
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    
    // 等待地址发送完成
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (timeout-- == 0) return 0;
    }
    
    // 3. 发送8位内存地址（AT24C02只需要一个字节地址）
    I2C_SendData(EEPROM_I2C, startAddr & 0xFF);
    
    // 等待地址发送完成
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (timeout-- == 0) return 0;
    }
    
    // 4. 发送数据
    for (uint8_t i = 0; i < len; i++) {
        I2C_SendData(EEPROM_I2C, data[i]);
        
        // 等待数据发送完成
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
            if (timeout-- == 0) return 0;
        }
    }
    
    // 5. 等待最后一个字节传输完成
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (timeout-- == 0) return 0;
    }
    
    // 6. 生成STOP条件
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    
    return 1;
}

// 多字节读取（适用于8位地址的EEPROM）
uint8_t I2C_EEPROM_ReadBuffer(uint16_t startAddr, uint8_t* buffer, uint16_t len) 
{
    // 1. 生成START条件（写模式）
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    
    // 等待START完成
    uint32_t timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (timeout-- == 0) return 0;
    }
    
    // 2. 发送设备地址+写模式
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    
    // 等待地址发送完成
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (timeout-- == 0) return 0;
    }
    
    // 3. 发送8位内存地址
    I2C_SendData(EEPROM_I2C, startAddr & 0xFF);
    
    // 等待地址发送完成 - 使用更通用的状态检查
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
        if (timeout-- == 0) return 0;
    }
    
    // 4. 重新生成START条件（读模式）
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    
    // 等待START完成
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (timeout-- == 0) return 0;
    }
    
    // 5. 发送设备地址+读模式
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Receiver);
    
    // 等待地址发送完成
    timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (timeout-- == 0) return 0;
    }
    
    // 6. 读取多个字节
    for (uint16_t i = 0; i < len; i++) {
        if (i == len - 1) {
            // 在接收最后一个字节前禁用ACK并生成STOP
            I2C_AcknowledgeConfig(EEPROM_I2C, DISABLE);
            I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
        }
        
        // 等待数据接收完成
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (timeout-- == 0) return 0;
        }
        
        // 读取数据
        buffer[i] = I2C_ReceiveData(EEPROM_I2C);
    }
    
    // 7. 重新使能ACK
    I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);
    return 1;
}
