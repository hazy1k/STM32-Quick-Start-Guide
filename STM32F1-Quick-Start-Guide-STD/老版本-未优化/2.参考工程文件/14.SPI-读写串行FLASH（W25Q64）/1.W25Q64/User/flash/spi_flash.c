#include "spi_flash.h"

static __IO uint32_t SPITimeout = SPIT_FLAG_TIMEOUT; // 超时时间初始化

// SPI_FLASH接口初始化
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  // 使能时钟
  FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK, ENABLE); // 使能SPI时钟
  FLASH_SPI_CS_APBxClock_FUN(FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MOSI_CLK|FLASH_SPI_MISO_CLK, ENABLE); // 使能SPI的接口引脚时钟
  // 配置GPIO
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);  // 初始化CS引脚
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure); // 初始化SCK引脚
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
  GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);// 初始化MOSI引脚
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
  GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);// 初始化MISO引脚
  SPI_FLASH_CS_HIGH(); // CS引脚置高，FLASH停止信号
  // SPI工作模式配置
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工模式
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // 主机模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 8位数据模式
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // 时钟极性高
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // 第二个时钟沿采样
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // 软件NSS管理
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 时钟预分频器
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // MSB位先行
  SPI_InitStructure.SPI_CRCPolynomial = 7; // CRC校验位
  SPI_Init(FLASH_SPIx, &SPI_InitStructure); // 初始化SPI
  SPI_Cmd(FLASH_SPIx, ENABLE); // 使能SPI
}

// 超时回调函数
uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorcode)
{
  FLASH_ERROR("SPI 等待超时！errorcode=%d\r\n", errorcode);
  return 0;
}

// 使用SPI发送一个字节
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT; // 超时时间初始化
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) // 等待发送缓冲区空(TXE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(0);
  }
  SPI_I2S_SendData(FLASH_SPIx, byte); // 写入数据寄存器
  SPITimeout = SPIT_FLAG_TIMEOUT;
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) // 等待接收缓冲区非空(RXNE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(1);
  }
  return SPI_I2S_ReceiveData(FLASH_SPIx); // 读取数据寄存器
}

// 使用SPI发送两个字节
uint16_t SPI_FLASH_SendHalfWord(uint16_t halfword)
{
  SPITimeout = SPIT_FLAG_TIMEOUT; // 超时时间初始化
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) // 等待发送缓冲区空(TXE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(2);
  }
  SPI_I2S_SendData(FLASH_SPIx, halfword); // 写入数据寄存器
  SPITimeout = SPIT_FLAG_TIMEOUT;
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) // 等待接收缓冲区非空(RXNE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(3);
  }
  return SPI_I2S_ReceiveData(FLASH_SPIx); // 读取数据寄存器
}

// 使用SPI读取一个字节
uint8_t SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

// 读取FLASH的状态寄存器
void SPI_FLASH_ReadStatusReg(uint32_t ReadAddr)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadData);
  // 发送地址的高、中、低位
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

// FALSH写入数据前，需要发送写使能命令
void SPI_FLASH_WriteEnable(void)
{
  SPI_FLASH_CS_LOW(); // 通讯开始，拉低CS
  SPI_FLASH_SendByte(W25X_WriteEnable);
  SPI_FLASH_CS_HIGH(); // 通讯结束，拉高CS
}

// 等待FLASH写入操作结束，即busy标志位为0
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t status = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadStatusReg); // 发送读取状态寄存器命令
  do{
    status = SPI_FLASH_SendByte(Dummy_Byte); // 读取FLASH的状态寄存器
  }
  while((status & 0x01) == SET); // 等待WTP位清零
  SPI_FLASH_CS_HIGH();
}

// 进入掉电模式
void SPI_FLASH_PowerDowm(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_PowerDown); // 发送掉电命令
  SPI_FLASH_CS_HIGH();
}

// 唤醒FLASH
void SPI_FLASH_WakeUp(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReleasePowerDown); // 发送唤醒命令
  SPI_FLASH_CS_HIGH();
}

// 擦除FLASH扇区，函数参数：扇区地址
void SPI_FLASH_SectorErase(uint32_t SectorAddress)
{
  SPI_FLASH_WriteEnable(); // 发送写使能命令
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_SectorErase); // 发送扇区擦除命令
  // 发送扇区地址的高、中、低位
  SPI_FLASH_SendByte((SectorAddress & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((SectorAddress & 0xFF00) >> 8);
  SPI_FLASH_SendByte(SectorAddress & 0xFF);
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
}

// 整片擦除FLASH
void SPI_FLASH_BulkErase(void)
{
  SPI_FLASH_WriteEnable(); // 发送写使能命令
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ChipErase); // 发送整片擦除命令
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
}

// 对FLASH按页写入数据
// 函数参数：待写入数据指针、写入地址、写入长度
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  SPI_FLASH_WriteEnable(); // 发送写使能命令
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_PageProgram); // 发送页写入命令
  // 分别写入高、中、低位地址
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(WriteAddr & 0xFF);
  // 判断压迫写入的数据是否大于单页最大长度(256)
  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
    NumByteToWrite = SPI_FLASH_PerWritePageSize; // 截取写入长度
    FLASH_ERROR("FLASH写入数据长度超过单页最大长度！\r\n");
  }
  while(NumByteToWrite--)
  {
    SPI_FLASH_SendByte(*pBuffer); // 写入数据
    pBuffer++;
  }
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
}

// 对FLASH任意写入数据
// 函数参数：待写入数据指针、写入地址、写入长度
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  uint8_t NumOfPage = 0; // 定义页面数量
  uint8_t NumOfSingle = 0; // 定义单页写入数据长度
  uint8_t Addr = 0; // 定义地址偏移量
  uint8_t count = 0; // 计数器
  uint8_t temp = 0; // 临时变量
	// 1.mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0
  Addr = WriteAddr % SPI_FLASH_PageSize;
	// 2.差count个数据值，刚好可以对齐到页地址
  count = SPI_FLASH_PageSize - Addr;
	// 3.计算出要写多少整数页
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	// 4.mod运算求余，计算出剩余不满一页的字节数
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	// 若Addr=0,则WriteAddr 刚好按页对齐，直接写
  if (Addr == 0)
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    { 
			/*先把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize); // 写入数据
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入剩余数据
    }
  }
	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
			/*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				/*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				/*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			/* 先写完count个数据，为的是让下一次要写的地址对齐 */
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
			/* 接下来就重复地址对齐的情况 */
      WriteAddr +=  count;
      pBuffer += count;
			/*把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

// 读取FLASH数据
// 函数参数：读取数据指针、读取地址、读取长度
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadData); // 发送读取数据命令
  // 发送地址的高、中、低位
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  while(NumByteToRead--)
  {
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte); // 读取数据
    pBuffer++;
  }
  SPI_FLASH_CS_HIGH();
}

// 读取FLASH Device ID
uint32_t SPI_FLASH_ReadDeviceID(void)
{
  uint32_t Temp = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return Temp;
}

// 读取FLASH ID
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp_H = 0, Temp_M = 0, Temp_L = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_JedecDeviceID);
  Temp_H = SPI_FLASH_SendByte(Dummy_Byte);
  Temp_M = SPI_FLASH_SendByte(Dummy_Byte);
  Temp_L = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  Temp = (Temp_H << 16) | (Temp_M << 8) | Temp_L;
  return Temp;
}
