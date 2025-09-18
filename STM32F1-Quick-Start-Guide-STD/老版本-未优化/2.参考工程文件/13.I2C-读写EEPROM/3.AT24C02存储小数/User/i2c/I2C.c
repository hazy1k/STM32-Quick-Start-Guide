#include "I2C.h"
#include "usart.h"		

uint16_t EEPROM_ADDRESS;
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化   

// I2C GPIO配置
void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  // 使能时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  // I2C_SCL、I2C_SDA配置
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       
  GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);   
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;      
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       
  GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);	  
}

// I2C工作参数配置
static void I2C_Mode_Configu(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; // I2C模式
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // 400KHz时钟频率
  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7; // 设置I2C自己的地址
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // 使能应答
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址模式
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed; // 快速模式
	// I2C 初始化
  I2C_Init(I2Cx, &I2C_InitStructure);
  I2C_Cmd(I2Cx, ENABLE);   
}

// I2C 外设(EEPROM)初始化
void I2C_EE_Init(void)
{
  I2C_GPIO_Config();  
  I2C_Mode_Configu();
  // 根据头文件I2C.h中的定义来选择EEPROM要写入的地址
#ifdef EEPROM_Block0_ADDRESS
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS; // 选择 EEPROM Block0 来写入
#endif
#ifdef EEPROM_Block1_ADDRESS  
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif
#ifdef EEPROM_Block2_ADDRESS  
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif
#ifdef EEPROM_Block3_ADDRESS  
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

// 等待I2C EEPROM进入待机模式
void I2C_EE_WaitEepromStandbyState(void)      
{
  vu16 Temp_SR = 0;
  do
  {
    I2C_GenerateSTART(I2Cx, ENABLE); // 发送I2C开始信号
    Temp_SR = I2C_ReadRegister(I2Cx, I2C_Register_SR1); // 读取I2C状态寄存器1
    I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送I2C地址信号
  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002)); // 等待I2C状态寄存器1的BUSY位清零
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF); // 清除I2C忙标志   
  I2C_GenerateSTOP(I2Cx, ENABLE); // I2C停止信号 
}

// 错误代码回调函数
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  EEPROM_ERROR("I2C 等待超时!errorCode = %d",errorCode); // 通过不同的错误码，可以判断哪部分超时
  return 0;
}
/*
  I2C写数据基本流程：
  1.发送I2C开始信号
  2.发送I2C写命令
  3.发送要写入的地址
  4.发送要写入的数据
  5.发送I2C停止信号，进入待机模式
*/
// 通过I2C从EEPROM中写入一个字节
// 函数参数：pBuffer，要写入的数据，WriteAddr，要写入的地址
uint32_t I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr) 
{
  I2C_GenerateSTART(I2Cx, ENABLE); // 发送I2C开始信号
  I2CTimeout = I2CT_FLAG_TIMEOUT;  // 超时时间初始化 
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) // 等待I2C空闲状态  
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(0); // 返回错误码0，代表发送I2C开始信号超时
  } 
  I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送I2C写命令
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(1); // 返回错误码1，代表发送I2C地址信号时超时
  }  
  I2C_SendData(I2Cx, WriteAddr); // 发送要写入的地址
  I2CTimeout = I2CT_FLAG_TIMEOUT; 
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(2); // 返回错误码2，代表发送I2C要写入的地址时超时
  } 
  I2C_SendData(I2Cx, *pBuffer); // 发送要写入的字节 
  I2CTimeout = I2CT_FLAG_TIMEOUT;  
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(3); // 返回错误码3，代表发送I2C要写入的字节时超时
  } 
  I2C_GenerateSTOP(I2Cx, ENABLE); // 发送字节字节完毕，停止信号
  return 1;
}

// 在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数，不能超过EEPROM页的大小，AT24C02每页有8个字节
// 函数参数：pBuffer，要写入的数据，WriteAddr，要写入的地址，NumByteToWrite，要写入的字节数
uint32_t I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // 等待I2C空闲状态
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(4); // 返回错误码4，代表写入多个数据时等待I2C空闲状态超时
  } 
  I2C_GenerateSTART(I2Cx, ENABLE); // I2C开始信号
  I2CTimeout = I2CT_FLAG_TIMEOUT; 
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(5); // 返回错误码5，代表写入多个数据发送I2C开始信号超时
  } 
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送I2C写命令
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(6); // 返回错误码6，代表写入多个数据发送I2C地址信号超时
  } 
  I2C_SendData(I2Cx, WriteAddr); // 发送要写入的地址  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(! I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(7); // 返回错误码7，代表写入多个数据发送I2C要写入的地址超时
  } 
  while(NumByteToWrite--) // 写入多个字节  
  {
    I2C_SendData(I2Cx, *pBuffer); // 发送要写入的字节 
    pBuffer++; // 数据指针递增
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
      if((I2CTimeout--) == 0) 
        return I2C_TIMEOUT_UserCallback(8); // 返回错误码8，代表写入多个数据发送I2C要写入的字节超时
    } 
  }
  I2C_GenerateSTOP(I2Cx, ENABLE); // 发送多字节完毕，停止信号
  return 1;
}

// 将缓冲区中的数据写到I2C EEPROM中
// 函数参数：pBuffer，要写入的数据，WriteAddr，要写入的地址，NumByteToWrite，要写入的字节数
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite) 
{
  u8 NumOfPage = 0; // 页数
  u8 NumOfSingle = 0; // 单独的字节
  u8 Addr = 0; // 地址
  u8 count = 0; // 计数器
  Addr = WriteAddr % I2C_PageSize; // 计算要写入的地址在页中的偏移
  count = I2C_PageSize - Addr; // 计算页剩余空间大小
  NumOfPage =  NumByteToWrite / I2C_PageSize; // 计算页数
  NumOfSingle = NumByteToWrite % I2C_PageSize; // 计算单独的字节
  if(Addr == 0) // 如果一页可以写入完
  {
    if(NumOfPage == 0) // 如果NumByteToWrite < I2C_PageSize 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入单独的字节
      I2C_EE_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
    }
    else // 如果NumByteToWrite > I2C_PageSize 
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);  
    	  I2C_EE_WaitEepromStandbyState();  
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }
      if(NumOfSingle!=0) // 如果还有单独的字节
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  else // 如果一页不能写入完 
  {
    if(NumOfPage== 0) // 如果If NumByteToWrite < I2C_PageSize
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    else // If NumByteToWrite > I2C_PageSize
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

// 从I2C EEPROM中读取数据到缓冲区中
// 函数参数：pBuffer，要读取的数据，ReadAddr，要读取的地址，NumByteToRead，要读取的字节数
uint32_t I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // 等待I2C空闲状态
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(9); // 返回错误码9，代表读取多个数据时等待I2C空闲状态超时
  }
  I2C_GenerateSTART(I2Cx, ENABLE); // I2C开始信号
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(10); // 返回错误码10，代表读取多个数据发送I2C开始信号超时
  }
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送I2C读命令
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(11); // 返回错误码11，代表读取多个数据发送I2C地址信号超时
  }  
  I2C_Cmd(I2Cx, ENABLE); // 使能I2C
  I2C_SendData(I2Cx, ReadAddr); // 发送要读取的地址
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(12); // 返回错误码12，代表读取多个数据发送I2C要读取的地址超时
  }
  I2C_GenerateSTART(I2Cx, ENABLE); // 发送I2C开始信号
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(13); // 返回错误码13，代表读取多个数据发送I2C开始信号超时
  }
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Receiver); // 发送I2C读命令
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(14); // 返回错误码14，代表读取多个数据发送I2C地址信号超时
  }
  while(NumByteToRead) // 循环读入数据 
  {
    if(NumByteToRead == 1) // 最后一次读入数据
    {
      I2C_AcknowledgeConfig(I2Cx, DISABLE); // 禁止I2C应答
      I2C_GenerateSTOP(I2Cx, ENABLE); // 发送I2C停止信号
    }
    I2CTimeout = I2CT_LONG_TIMEOUT;
		while(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)   
		{
			if((I2CTimeout--) == 0) 
        return I2C_TIMEOUT_UserCallback(3); // 返回错误码3，代表读取多个数据接收I2C数据超时
		} 
    {      
      *pBuffer = I2C_ReceiveData(I2Cx); // 接收I2C数据
      pBuffer++;  // 数据指针递增
      NumByteToRead--; // 读取字节数递减        
    }   
  }
  I2C_AcknowledgeConfig(I2Cx, ENABLE); // 使能I2C应答
  return 1;
}
