# 第五十章 LCD显示中英文-字库外部FLASH

## 1. 硬件设计

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCDdis10.png)

## 2. 软件设计

### 2.1 编程大纲

1. SPI_FLASH初始化

2. 字模数据

3. LCD驱动函数

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 SPI_FLASH工作初始化

```c
#include "flash.h"

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
```

#### 2.2.2 LCD相关参数宏定义

```c
#ifndef __LCD_H
#define    __LCD_H

#include "stm32f10x.h"
#include "fonts.h"

// 根据液晶扫描方向而变化的XY像素宽度
// 调用ILI9341_GramScan函数设置方向时会自动更改
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 
// 液晶屏扫描模式
// 参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

/*选择BANK1-BORSRAM1 连接 TFT，地址范围为0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X60000000
RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000*/

// FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define FSMC_Addr_ST7789V_CMD  ((uint32_t)0x60000000)
// FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define FSMC_Addr_ST7789V_DATA ((uint32_t)0x60020000)
// 由片选引脚决定的NOR/SRAM块
#define FSMC_Bank1_NORSRAMx FSMC_Bank1_NORSRAM1
/* 控制引脚 */
// 片选，选择NOR/SRAM块
#define ST7789V_CS_CLK  RCC_APB2Periph_GPIOD // CS->PD7
#define ST7789V_CS_PORT GPIOD
#define ST7789V_CS_PIN  GPIO_Pin_7
// DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
#define ST7789V_DC_CLK  RCC_APB2Periph_GPIOD // DC->PD11
#define ST7789V_DC_PORT GPIOD
#define ST7789V_DC_PIN  GPIO_Pin_11
// 写使能
#define ST7789V_WR_CLK  RCC_APB2Periph_GPIOD // WR->PD5  
#define ST7789V_WR_PORT GPIOD
#define ST7789V_WR_PIN  GPIO_Pin_5
// 读使能
#define ST7789V_RD_CLK  RCC_APB2Periph_GPIOD // RD->PD4     
#define ST7789V_RD_PORT GPIOD
#define ST7789V_RD_PIN  GPIO_Pin_4
// 复位引脚
#define ST7789V_RST_CLK  RCC_APB2Periph_GPIOE // RST->PE1  
#define ST7789V_RST_PORT GPIOE
#define ST7789V_RST_PIN  GPIO_Pin_1
// 背光引脚
#define ST7789V_BK_CLK  RCC_APB2Periph_GPIOD // BK->PD12   
#define ST7789V_BK_PORT GPIOD
#define ST7789V_BK_PIN  GPIO_Pin_12
/* 数据线 */
#define ST7789V_D0_CLK  RCC_APB2Periph_GPIOD // D0->PD14   
#define ST7789V_D0_PORT GPIOD
#define ST7789V_D0_PIN  GPIO_Pin_14
#define ST7789V_D1_CLK  RCC_APB2Periph_GPIOD // D1->PD15   
#define ST7789V_D1_PORT GPIOD
#define ST7789V_D1_PIN  GPIO_Pin_15
#define ST7789V_D2_CLK  RCC_APB2Periph_GPIOD // D2->PD0   
#define ST7789V_D2_PORT GPIOD
#define ST7789V_D2_PIN  GPIO_Pin_0
#define ST7789V_D3_CLK  RCC_APB2Periph_GPIOD // D3->PD1   
#define ST7789V_D3_PORT GPIOD
#define ST7789V_D3_PIN  GPIO_Pin_1
#define ST7789V_D4_CLK  RCC_APB2Periph_GPIOE // D4->PE7   
#define ST7789V_D4_PORT GPIOE
#define ST7789V_D4_PIN  GPIO_Pin_7
#define ST7789V_D5_CLK  RCC_APB2Periph_GPIOE // D5->PE8   
#define ST7789V_D5_PORT GPIOE
#define ST7789V_D5_PIN  GPIO_Pin_8
#define ST7789V_D6_CLK  RCC_APB2Periph_GPIOE // D6->PE9   
#define ST7789V_D6_PORT GPIOE
#define ST7789V_D6_PIN  GPIO_Pin_9
#define ST7789V_D7_CLK  RCC_APB2Periph_GPIOE // D7->PE10  
#define ST7789V_D7_PORT GPIOE
#define ST7789V_D7_PIN  GPIO_Pin_10
#define ST7789V_D8_CLK  RCC_APB2Periph_GPIOE // D8->PE11   
#define ST7789V_D8_PORT GPIOE
#define ST7789V_D8_PIN  GPIO_Pin_11
#define ST7789V_D9_CLK  RCC_APB2Periph_GPIOE // D9->PE12   
#define ST7789V_D9_PORT GPIOE
#define ST7789V_D9_PIN  GPIO_Pin_12
#define ST7789V_D10_CLK  RCC_APB2Periph_GPIOE // D10->PE13  
#define ST7789V_D10_PORT GPIOE
#define ST7789V_D10_PIN  GPIO_Pin_13
#define ST7789V_D11_CLK  RCC_APB2Periph_GPIOE // D11->PE14   
#define ST7789V_D11_PORT GPIOE
#define ST7789V_D11_PIN  GPIO_Pin_14
#define ST7789V_D12_CLK  RCC_APB2Periph_GPIOE // D12->PE15   
#define ST7789V_D12_PORT GPIOE
#define ST7789V_D12_PIN  GPIO_Pin_15
#define ST7789V_D13_CLK  RCC_APB2Periph_GPIOD // D13->PD8   
#define ST7789V_D13_PORT GPIOD
#define ST7789V_D13_PIN  GPIO_Pin_8
#define ST7789V_D14_CLK  RCC_APB2Periph_GPIOD // D14->D9   
#define ST7789V_D14_PORT GPIOD
#define ST7789V_D14_PIN  GPIO_Pin_9
#define ST7789V_D15_CLK  RCC_APB2Periph_GPIOD // D15->PD10   
#define ST7789V_D15_PORT GPIOD
#define ST7789V_D15_PIN  GPIO_Pin_10
/* LCD显示区域 */
#define ST7789V_X_Star   0 // 起始点的X坐标
#define ST7789V_Y_Star   0 // 起始点的Y坐标
#define ST7789V_LESS_PIXEL    240 // 屏较短方向的像素宽度
#define ST7789V_MORE_PIXEL    320 // 屏较长方向的像素宽度
/* 颜色定义 */
#define WHITE   0xFFFF       
#define BLACK   0x0000       
#define GREY    0xF7DE       
#define BLUE    0x001F       
#define BLUE2   0x051F        
#define RED     0xF800        
#define MAGENTA 0xF81F        
#define GREEN   0x07E0        
#define CYAN    0x7FFF       
#define YELLOW  0xFFE0        
#define BRED    0xF81F
#define GRED    0xFFE0
#define GBLUE   0x07FF
/* ST7789V命令 */
#define CMD_SetCoordinateX 0x2A    // 设置X坐标
#define CMD_SetCoordinateY 0x2B    // 设置Y坐标
#define CMD_SetPixel       0x2C    // 填充像素
/* 定义 LCD 驱动芯片 ID */
#define LCDID_ST7789V 0x9341
/* 字符显示 */
void ST7789V_DisplayStringEx(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *ptr, uint16_t DrawModel);
void ST7789V_DisplayStringEx_YDir(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *ptr, uint16_t DrawModel);
/* 字模缩放 */
void ST7789V_zoomChar(uint16_t in_width, uint16_t in_heig, uint16_t out_width, uint16_t out_heig, uint8_t *in_ptr, uint8_t *out_ptr, uint8_t en_cn);
void ST7789V_DrawChar_Ex(uint16_t usX, uint16_t usY, uint16_t Font_width, uint16_t Font_Height, uint8_t *c, uint16_t DrawModel);
/* 函数声明 */
void     ST7789V_Init(void);
uint16_t ST7789V_ReadID(void);
void     ST7789V_Rst(void);
void     ST7789V_BackLed_Control(FunctionalState enumState);
void     ST7789V_GramScan(uint8_t ucOtion);
void     ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void     ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void     ST7789V_SetPointPixel(uint16_t usX, uint16_t usY);
uint16_t ST7789V_GetPointPixel(uint16_t usX , uint16_t usY);
void     ST7789V_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);
void     ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight,uint8_t ucFilled);
void     ST7789V_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled);
void     ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar);
void     ST7789V_DispStringLine_EN(uint16_t line, char* pStr);
void     ST7789V_DispString_EN(uint16_t usX, uint16_t usY, char* pStr);
void      ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY ,  char* pStr);
void     ST7789V_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar);
void     ST7789V_DispString_CH(uint16_t usX , uint16_t usY, char* pStr);
void     ST7789V_DispStringLine_EN_CH(uint16_t line, char* pStr);
void     ST7789V_DispString_EN_CH(uint16_t usX, uint16_t usY, char* pStr);
void     ST7789V_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY , char * pStr);
void      LCD_SetFont(sFONT *fonts);
sFONT   *LCD_GetFont(void);
void      LCD_ClearLine(uint16_t Line);
void      LCD_SetBackColor(uint16_t Color);
void     LCD_SetTextColor(uint16_t Color)    ;
void      LCD_SetColors(uint16_t TextColor, uint16_t BackColor);
void      LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor);

#endif /* __LCD_H */
```

#### 2.2.3 LCD驱动函数

```c
#include "lcd.h"
#include "fonts.h"    
// 根据液晶扫描方向而变化的XY像素宽度
uint16_t LCD_X_LENGTH = ST7789V_LESS_PIXEL; // 240
uint16_t LCD_Y_LENGTH = ST7789V_MORE_PIXEL; // 320
uint8_t LCD_SCAN_MODE = 6; // 液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
uint16_t lcdid = LCDID_ST7789V; // 保存液晶屏驱动ic的 ID
static sFONT *LCD_Currentfonts = &Font8x16; // 字体
static uint16_t CurrentTextColor = BLACK; // 字体颜色-黑
static uint16_t CurrentBackColor = WHITE; // 背景颜色-白
/* 函数声明 */
static void     ST7789V_Write_Cmd(uint16_t usCmd);
static void     ST7789V_Write_Data(uint16_t usData);
static uint16_t ST7789V_Read_Data(void);
static void     ST7789V_Delay( __IO uint32_t nCount);
static void     ST7789V_GPIO_Config(void);
static void     ST7789V_FSMC_Config(void);
static void     ST7789V_REG_Config(void);
static void     ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
static uint16_t ST7789V_Read_PixelData(void);
// 向ST7789V写入命令
static void ST7789V_Write_Cmd(uint16_t usCmd)
{
    *(__IO uint16_t*)(FSMC_Addr_ST7789V_CMD) = usCmd;    
}
// 向ST7789V写入数据
static void ST7789V_Write_Data(uint16_t usData)
{
    *(__IO uint16_t*)(FSMC_Addr_ST7789V_DATA) = usData;
}
// 从ST7789V读取数据    
static uint16_t ST7789V_Read_Data(void)
{
    return(*( __IO uint16_t *)(FSMC_Addr_ST7789V_DATA));

}
// 简单延时函数    
static void ST7789V_Delay(__IO uint32_t nCount)
{
  for(;nCount != 0; nCount--);

}
// 初始化ST7789V的GPIO
static void ST7789V_GPIO_Config ( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( // 控制信号
    ST7789V_CS_CLK|ST7789V_DC_CLK|ST7789V_WR_CLK|ST7789V_RD_CLK    |ST7789V_BK_CLK|ST7789V_RST_CLK|
    // 数据信号
    ST7789V_D0_CLK|ST7789V_D1_CLK|ST7789V_D2_CLK|ST7789V_D3_CLK|ST7789V_D4_CLK|ST7789V_D5_CLK|
    ST7789V_D6_CLK|ST7789V_D7_CLK|ST7789V_D8_CLK|ST7789V_D9_CLK|ST7789V_D10_CLK|ST7789V_D11_CLK|
    ST7789V_D12_CLK|ST7789V_D13_CLK|ST7789V_D14_CLK|ST7789V_D15_CLK, ENABLE);
    // 配置FSMC相对应的数据线    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = ST7789V_D0_PIN;
    GPIO_Init(ST7789V_D0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D1_PIN;
    GPIO_Init(ST7789V_D1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D2_PIN;
    GPIO_Init(ST7789V_D2_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D3_PIN;
    GPIO_Init(ST7789V_D3_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D4_PIN;
    GPIO_Init(ST7789V_D4_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D5_PIN;
    GPIO_Init(ST7789V_D5_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D6_PIN;
    GPIO_Init(ST7789V_D6_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D7_PIN;
    GPIO_Init(ST7789V_D7_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D8_PIN;
    GPIO_Init(ST7789V_D8_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D9_PIN;
    GPIO_Init(ST7789V_D9_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D10_PIN;
    GPIO_Init(ST7789V_D10_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D11_PIN;
    GPIO_Init(ST7789V_D11_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D12_PIN;
    GPIO_Init(ST7789V_D12_PORT, &GPIO_InitStructure);    
    GPIO_InitStructure.GPIO_Pin = ST7789V_D13_PIN;
    GPIO_Init(ST7789V_D13_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D14_PIN;
    GPIO_Init(ST7789V_D14_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_D15_PIN;
    GPIO_Init(ST7789V_D15_PORT, &GPIO_InitStructure);
    /* 配置FSMC相对应的控制线
    FSMC_NOE   :LCD-RD
    FSMC_NWE   :LCD-WR
    FSMC_NE1   :LCD-CS
    FSMC_A16   :LCD-DC */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = ST7789V_RD_PIN; 
    GPIO_Init(ST7789V_RD_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_WR_PIN; 
    GPIO_Init(ST7789V_WR_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ST7789V_CS_PIN; 
    GPIO_Init(ST7789V_CS_PORT, &GPIO_InitStructure);  
    GPIO_InitStructure.GPIO_Pin = ST7789V_DC_PIN; 
    GPIO_Init(ST7789V_DC_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = ST7789V_RST_PIN; 
    GPIO_Init(ST7789V_RST_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Pin = ST7789V_BK_PIN; 
    GPIO_Init(ST7789V_BK_PORT, &GPIO_InitStructure);
}
// LCD FSMC 模式配置
static void ST7789V_FSMC_Config ( void )
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure = {0};
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    readWriteTiming.FSMC_AddressSetupTime = 0x01;// 地址建立时间
    readWriteTiming.FSMC_DataSetupTime = 0x04;     // 数据保持时间
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B; // 模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配    
    /* 以下配置与模式B无关 */
    readWriteTiming.FSMC_AddressHoldTime = 0x00; 
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;    
    /* FSMC配置 */
    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx; // 使用NOR FLASH的BANK1
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable; // 禁止地址复用
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR; // 使用NOR FLASH
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b; // 数据宽度为16位
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable; // 禁止突发访问模式
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low; // 时钟低电平有效
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable; // 禁止WRAP模式
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState; // 等待状态前有效
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable; // 使能写操作
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable; // 禁止等待信号
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable; // 禁止扩展模式
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; // 禁止写突发模式
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; // 设置时序参数
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming; // 设置时序参数
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  
}
// 初始化ST7789V的寄存器
static void ST7789V_REG_Config ( void )
{
    lcdid = ST7789V_ReadID(); // 读取LCD ID
    if(lcdid == LCDID_ST7789V)
    {
        ST7789V_Write_Cmd(0xCF);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x81);
        ST7789V_Write_Data(0x30);
        /* Power on sequence control (EDh) */
        ST7789V_Write_Cmd(0xED);
        ST7789V_Write_Data(0x64);
        ST7789V_Write_Data(0x03);
        ST7789V_Write_Data(0x12);
        ST7789V_Write_Data(0x81);
        /* Driver timing control A (E8h) */
        ST7789V_Write_Cmd(0xE8);
        ST7789V_Write_Data(0x85);
        ST7789V_Write_Data(0x10);
        ST7789V_Write_Data(0x78);
        /* Power control A (CBh) */
        ST7789V_Write_Cmd(0xCB);
        ST7789V_Write_Data(0x39);
        ST7789V_Write_Data(0x2C);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x34);
        ST7789V_Write_Data(0x06); // 改为0x06可防止液晶显示白屏时有条纹的情况
        /* Pump ratio control (F7h) */
        ST7789V_Write_Cmd(0xF7);
        ST7789V_Write_Data(0x20);
        /* Driver timing control B */
        ST7789V_Write_Cmd(0xEA);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
        ST7789V_Write_Cmd(0xB1);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x1B);
        /* Display Function Control (B6h) */
        ST7789V_Write_Cmd(0xB6);
        ST7789V_Write_Data(0x0A);
        ST7789V_Write_Data(0xA2);
        /* Power Control 1 (C0h) */
        ST7789V_Write_Cmd(0xC0);
        ST7789V_Write_Data(0x35);
        /* Power Control 2 (C1h) */
        ST7789V_Write_Cmd(0xC1);
        ST7789V_Write_Data(0x11);
        /* VCOM Control 1 (C5h) */
        ST7789V_Write_Cmd(0xC5);
        ST7789V_Write_Data(0x45);
        ST7789V_Write_Data(0x45);
        /* VCOM Control 2 (C7h) */
        ST7789V_Write_Cmd(0xC7);
        ST7789V_Write_Data(0xA2);
        /* Enable 3G (F2h) */
        ST7789V_Write_Cmd(0xF2);
        ST7789V_Write_Data(0x00);
        /* Gamma Set (26h) */
        ST7789V_Write_Cmd(0x26);
        ST7789V_Write_Data(0x01);
        /* Positive Gamma Correction */
        ST7789V_Write_Cmd(0xE0); // Set Gamma
        ST7789V_Write_Data(0x0F);
        ST7789V_Write_Data(0x26);
        ST7789V_Write_Data(0x24);
        ST7789V_Write_Data(0x0B);
        ST7789V_Write_Data(0x0E);
        ST7789V_Write_Data(0x09);
        ST7789V_Write_Data(0x54);
        ST7789V_Write_Data(0xA8);
        ST7789V_Write_Data(0x46);
        ST7789V_Write_Data(0x0C);
        ST7789V_Write_Data(0x17);
        ST7789V_Write_Data(0x09);
        ST7789V_Write_Data(0x0F);
        ST7789V_Write_Data(0x07);
        ST7789V_Write_Data(0x00);
        /* Negative Gamma Correction (E1h) */
        ST7789V_Write_Cmd(0xE1); // Set Gamma
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x19);
        ST7789V_Write_Data(0x1B);
        ST7789V_Write_Data(0x04);
        ST7789V_Write_Data(0x10);
        ST7789V_Write_Data(0x07);
        ST7789V_Write_Data(0x2A);
        ST7789V_Write_Data(0x47);
        ST7789V_Write_Data(0x39);
        ST7789V_Write_Data(0x03);
        ST7789V_Write_Data(0x06);
        ST7789V_Write_Data(0x06);
        ST7789V_Write_Data(0x30);
        ST7789V_Write_Data(0x38);
        ST7789V_Write_Data(0x0F);
        /* Memory access control set */
        ST7789V_Write_Cmd(0x36); 
        ST7789V_Write_Data(0xC8);    /* 竖屏，左上角到右下角扫描方式 */
        /* Column address control set */
        ST7789V_Write_Cmd(CMD_SetCoordinateX); 
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0xEF);
        /* Page address control set */
        ST7789V_Write_Cmd(CMD_SetCoordinateY); 
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x00);
        ST7789V_Write_Data(0x01);
        ST7789V_Write_Data(0x3F);
        /* Pixel Format Set (3Ah) */
        ST7789V_Write_Cmd(0x3a); 
        ST7789V_Write_Data(0x55);
        /* Sleep Out (11h) */
        ST7789V_Write_Cmd(0x11);
        ST7789V_Delay(0xAFFf<<2);
        /* Display ON (29h) */
        ST7789V_Write_Cmd(0x29);
    }
}
// ST7789V初始化函数
void ST7789V_Init ( void )
{
    ST7789V_GPIO_Config (); 
    ST7789V_FSMC_Config (); 
    ST7789V_BackLed_Control(ENABLE);
    ST7789V_Rst(); 
    ST7789V_REG_Config();
    // 设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向  
    ST7789V_GramScan(LCD_SCAN_MODE);
}
// ST7789V背光控制函数
void ST7789V_BackLed_Control(FunctionalState enumState )
{
    if(enumState) // 点亮背光灯
        GPIO_ResetBits(ST7789V_BK_PORT, ST7789V_BK_PIN); // 低电平点亮
    else
        GPIO_SetBits(ST7789V_BK_PORT, ST7789V_BK_PIN);
}
// 读取LCD驱动芯片ID函数
uint16_t ST7789V_ReadID(void)
{
    uint16_t id = 0;    
    ST7789V_Write_Cmd(0xD3); // 写入命令
    ST7789V_Read_Data();
    ST7789V_Read_Data();
    id = ST7789V_Read_Data();
    id <<= 8;
    id |= ST7789V_Read_Data();
    if(id == LCDID_ST7789V)
    {
        return id; // 成功读取到ST7789V ID
      }
    return 0; // 读取失败
}
// ST7789V软件复位
void ST7789V_Rst(void)
{
    GPIO_ResetBits(ST7789V_RST_PORT, ST7789V_RST_PIN); // 低电平复位
    ST7789V_Delay(0xAFF);                        
    GPIO_SetBits (ST7789V_RST_PORT, ST7789V_RST_PIN);              
    ST7789V_Delay(0xAFF); 
}
// 设置ST7789V的GRAM的扫描方向 
void ST7789V_GramScan(uint8_t ucOption)
{    
    // 参数检查，只可输入0-7
    if(ucOption > 7)
    {
        return;
    }
    LCD_SCAN_MODE = ucOption; // 根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
    if(ucOption%2 == 0)    
    {
        // 0 2 4 6模式下X方向像素宽度为240，Y方向为320
        LCD_X_LENGTH = ST7789V_LESS_PIXEL;
        LCD_Y_LENGTH = ST7789V_MORE_PIXEL;
    }
    else                
    {
        // 1 3 5 7模式下X方向像素宽度为320，Y方向为240
        LCD_X_LENGTH = ST7789V_MORE_PIXEL;
        LCD_Y_LENGTH = ST7789V_LESS_PIXEL; 
    }
    ST7789V_Write_Cmd(0x36);    // 0x36命令参数的高3位可用于设置GRAM扫描方向    
    if(lcdid == LCDID_ST7789V)
      {
        ST7789V_Write_Data(0x00 |(ucOption<<5)); // 根据ucOption的值设置LCD参数，共0-7种模式
      }
    ST7789V_Write_Cmd(CMD_SetCoordinateX); 
    ST7789V_Write_Data(0x00);        
    ST7789V_Write_Data(0x00);    
    ST7789V_Write_Data(((LCD_X_LENGTH-1)>>8)&0xFF); 
    ST7789V_Write_Data((LCD_X_LENGTH-1)&0xFF);                
    ST7789V_Write_Cmd(CMD_SetCoordinateY); 
    ST7789V_Write_Data(0x00);        
    ST7789V_Write_Data(0x00);        
    ST7789V_Write_Data(((LCD_Y_LENGTH-1)>>8)&0xFF); 
    ST7789V_Write_Data((LCD_Y_LENGTH-1)&0xFF);            
    ST7789V_Write_Cmd(CMD_SetPixel);    
}
// ST7789V显示器上开辟一个窗口
void ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{    
    ST7789V_Write_Cmd(CMD_SetCoordinateX);                 
    ST7789V_Write_Data(usX>>8);     
    ST7789V_Write_Data(usX&0xff);     
    ST7789V_Write_Data((usX+usWidth-1)>>8);
    ST7789V_Write_Data((usX+usWidth-1)&0xff);
    ST7789V_Write_Cmd(CMD_SetCoordinateY);               
    ST7789V_Write_Data(usY>>8);
    ST7789V_Write_Data(usY&0xff);
    ST7789V_Write_Data((usY+usHeight-1)>>8);
    ST7789V_Write_Data((usY+usHeight-1)&0xff);
}
// 设置ST7789V光标坐标
static void ST7789V_SetCursor(uint16_t usX, uint16_t usY)    
{
    ST7789V_OpenWindow(usX, usY, 1, 1 );
}
// 填充颜色
static void ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
    uint32_t i = 0;
    ST7789V_Write_Cmd(CMD_SetPixel);    
    for(i = 0; i < ulAmout_Point; i++)
    {
        ST7789V_Write_Data(usColor);
    }
}
// 清屏
void ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
    ST7789V_OpenWindow(usX, usY, usWidth, usHeight);
    ST7789V_FillColor(usWidth * usHeight, CurrentBackColor) ;        
}
// 对LCD的某一点以某种颜色进行填充
void ST7789V_SetPointPixel(uint16_t usX, uint16_t usY)    
{    
    if((usX < LCD_X_LENGTH)&&(usY < LCD_Y_LENGTH))
      {
        ST7789V_SetCursor(usX, usY);    
        ST7789V_FillColor(1, CurrentTextColor);
    }
}
// 读取 GRAM 的一个像素数据
static uint16_t ST7789V_Read_PixelData(void)    
{    
    uint16_t usRG=0, usB=0;
    ST7789V_Write_Cmd(0x2E);
    ST7789V_Read_Data (); // 去掉前一次读取结果
    // 获取红色通道与绿色通道的值
    usRG = ST7789V_Read_Data();  
    usB = ST7789V_Read_Data();      
      return((usRG&0xF800)|((usRG<<3)&0x7E0)|(usB>>11));
}
// 获取LCD的某一点的颜色值
uint16_t ST7789V_GetPointPixel(uint16_t usX, uint16_t usY)
{ 
    uint16_t usPixelData;
    ST7789V_SetCursor(usX, usY);
    usPixelData = ST7789V_Read_PixelData();
    return usPixelData;
}
// 在LCD使用 Bresenham 算法画线段 
void ST7789V_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
    uint16_t us; 
    uint16_t usX_Current, usY_Current;
    int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
    int32_t lIncrease_X, lIncrease_Y;     
    lDelta_X = usX2 - usX1; // 计算坐标增量 
    lDelta_Y = usY2 - usY1; 
    usX_Current = usX1; 
    usY_Current = usY1; 
    if ( lDelta_X > 0 ) 
    {
        lIncrease_X = 1; // 设置单步方向
    } 
    else if(lDelta_X == 0) 
    {
        lIncrease_X = 0; // 垂直线
    } 
    else 
      { 
        lIncrease_X = -1;
        lDelta_X = - lDelta_X;
      } 
    if(lDelta_Y > 0)
    {
        lIncrease_Y = 1; 
    }
    else if(lDelta_Y == 0)
    {
        lIncrease_Y = 0; // 水平线 
    }
    else 
      {
        lIncrease_Y = -1;
        lDelta_Y = - lDelta_Y;
      } 
    if(lDelta_X > lDelta_Y)
    {
        lDistance = lDelta_X; // 选取基本增量坐标轴 
    }
    else 
    {
        lDistance = lDelta_Y; 
    }
    for(us = 0; us <= lDistance + 1; us++ ) // 画线输出 
    {  
        ST7789V_SetPointPixel(usX_Current, usY_Current ); // 画点 
        lError_X += lDelta_X ; 
        lError_Y += lDelta_Y ;         
        if(lError_X > lDistance) 
        { 
            lError_X -= lDistance; 
            usX_Current += lIncrease_X; 
        }  
        if(lError_Y > lDistance) 
        { 
            lError_Y -= lDistance; 
            usY_Current += lIncrease_Y; 
        } 
    }  
}   
// 在LCD上画一个矩形
void ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
    if(ucFilled)
    {
        ST7789V_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
        ST7789V_FillColor(usWidth * usHeight ,CurrentTextColor);    
    }
    else
    {
        ST7789V_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
        ST7789V_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
        ST7789V_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
        ST7789V_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);        
    }
}
// 在LCD上使用 Bresenham 算法画圆
void ST7789V_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled)
{
    int16_t sCurrentX, sCurrentY;
    int16_t sError;
    sCurrentX = 0; sCurrentY = usRadius;      
    sError = 3 - ( usRadius << 1 ); // 判断下个点位置的标志
    while(sCurrentX <= sCurrentY)
    {
        int16_t sCountY;
        if(ucFilled)
        {            
            for(sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++) 
            {                      
                ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCountY);          
                ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCountY);                  
                ST7789V_SetPointPixel(usX_Center - sCountY,   usY_Center + sCurrentX);         
                ST7789V_SetPointPixel(usX_Center - sCountY,   usY_Center - sCurrentX);        
                ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCountY);               
                ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCountY);           
                ST7789V_SetPointPixel(usX_Center + sCountY,   usY_Center - sCurrentX);              
                ST7789V_SetPointPixel(usX_Center + sCountY,   usY_Center + sCurrentX);                        
            }
        }
        else
        {          
            ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCurrentY);            
            ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCurrentY);              
            ST7789V_SetPointPixel(usX_Center - sCurrentY, usY_Center + sCurrentX);            
            ST7789V_SetPointPixel(usX_Center - sCurrentY, usY_Center - sCurrentX);            
            ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCurrentY);             
            ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCurrentY);            
            ST7789V_SetPointPixel(usX_Center + sCurrentY, usY_Center - sCurrentX);             
            ST7789V_SetPointPixel(usX_Center + sCurrentY, usY_Center + sCurrentX);           
        }            
        sCurrentX ++;
        if(sError < 0 )
        { 
            sError += 4 * sCurrentX + 6;
        }      
        else
        {
            sError += 10 + 4 * (sCurrentX - sCurrentY);   
            sCurrentY--;
        }     
    }
}
// 在LCD上显示一个英文字符
void ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
    uint8_t  byteCount, bitCount,fontLength;    
    uint16_t ucRelativePositon;
    uint8_t *Pfont;
    // 对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
    ucRelativePositon = cChar - ' ';
    // 每个字模的字节数
    fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
    //字模首地址
    /*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
    Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
    // 设置显示窗口
    ST7789V_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);    
    ST7789V_Write_Cmd(CMD_SetPixel);            
    // 按字节读取字模数据
    for(byteCount = 0; byteCount < fontLength; byteCount++)
    {
        // 一位一位处理要显示的颜色
        for(bitCount = 0; bitCount < 8; bitCount++ )
        {
            if(Pfont[byteCount] & (0x80>>bitCount))
                ST7789V_Write_Data(CurrentTextColor);            
            else
                ST7789V_Write_Data(CurrentBackColor);
        }    
    }    
}
// 在LCD上显示英文字符串
void ST7789V_DispStringLine_EN(uint16_t line,  char* pStr)
{
    uint16_t usX = 0;
    while(*pStr != '\0')
    {
        if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            line += LCD_Currentfonts->Height;
        }
        if ( ( line - ST7789V_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
        {
            usX = ST7789V_X_Star;
            line = ST7789V_Y_Star;
        }
        ST7789V_DispChar_EN(usX, line, * pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;    
    }
}
// 在LCD上显示英文字符串
void ST7789V_DispString_EN(uint16_t usX ,uint16_t usY,  char* pStr)
{
    while(*pStr != '\0')
    {
        if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY += LCD_Currentfonts->Height;
        }        
        if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY = ST7789V_Y_Star;
        }    
        ST7789V_DispChar_EN(usX, usY, * pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;    
    }
}
// 在LCD上显示英文字符串(沿Y轴方向)
void ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY, char* pStr)
{    
    while(* pStr != '\0')
    {
        if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height)>LCD_Y_LENGTH)
        {
            usY = ST7789V_Y_Star;
            usX += LCD_Currentfonts->Width;
        }
        if((usX - ST7789V_X_Star + LCD_Currentfonts->Width)> LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY = ST7789V_Y_Star;
        }
        ST7789V_DispChar_EN(usX, usY, *pStr);
        pStr ++;
        usY += LCD_Currentfonts->Height;        
    }    
}
// 在LCD上显示一个中文字符
void ST7789V_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar)
{
    uint8_t rowCount, bitCount;
    uint8_t usBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];
    uint16_t usTemp;
    ST7789V_OpenWindow(usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR);
    ST7789V_Write_Cmd(CMD_SetPixel);
    GetGBKCode(usBuffer, usChar);
    for(rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++)
    {
        usTemp = usBuffer[rowCount*2];
        usTemp = (usTemp<<8);
        usTemp |= usBuffer[rowCount*2+1];
        for(bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount++)
        {
            if(usTemp & (0x8000>>bitCount))
            {
                ST7789V_Write_Data(CurrentTextColor);
            }
            else
            {
                ST7789V_Write_Data(CurrentBackColor);
            }
        }
    }
}
// 在LCD上显示中文字符串
void ST7789V_DispString_CH(uint16_t usX , uint16_t usY, char* pStr)
{    
    uint16_t usCh;
    while(*pStr != '\0') 
    {        
        if((usX - ST7789V_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY += HEIGHT_CH_CHAR;
        }
        if((usY - ST7789V_Y_Star + HEIGHT_CH_CHAR ) > LCD_Y_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY = ST7789V_Y_Star;
        }            
        usCh = *(uint16_t*)pStr;    
          usCh = (usCh << 8 ) + (usCh >> 8);
        ST7789V_DispChar_CH(usX, usY, usCh);
        usX += WIDTH_CH_CHAR;
        pStr += 2;           
    }       
}
// 在LCD上显示中英文字符串
void ST7789V_DispStringLine_EN_CH(uint16_t line, char* pStr)
{
    uint16_t usCh;
    uint16_t usX = 0;
    while(*pStr != '\0')
    {
        if(*pStr <= 126 ) // 英文字符
        {
            if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
            {
                usX = ST7789V_X_Star;
                line += LCD_Currentfonts->Height;
            }    
            if((line - ST7789V_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
            {
                usX = ST7789V_X_Star;
                line = ST7789V_Y_Star;
            }            
            ST7789V_DispChar_EN(usX, line, *pStr);
            usX +=  LCD_Currentfonts->Width;
            pStr ++;
        }
        else // 汉字字符
        {
            if((usX - ST7789V_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
            {
                usX = ST7789V_X_Star;
                line += HEIGHT_CH_CHAR;
            }    
            if((line - ST7789V_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
            {
                usX = ST7789V_X_Star;
                line = ST7789V_Y_Star;
            }            
            usCh = *(uint16_t*)pStr;    
            usCh = (usCh << 8) + (usCh >> 8);        
            ST7789V_DispChar_CH(usX, line, usCh);
            usX += WIDTH_CH_CHAR;
            pStr += 2; // 一个汉字两个字节 
    }
  }    
} 
// 在LCD上显示中英文字符串
void ST7789V_DispString_EN_CH(uint16_t usX, uint16_t usY, char* pStr)
{
    uint16_t usCh;
    while(*pStr != '\0')
    {
        if(*pStr <= 126)                   
        {
            if ( ( usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
            {
                usX = ST7789V_X_Star;
                usY += LCD_Currentfonts->Height;
            }
            if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
            {
                usX = ST7789V_X_Star;
                usY = ST7789V_Y_Star;
            }            
              ST7789V_DispChar_EN(usX, usY, *pStr);
            usX += LCD_Currentfonts->Width;
              pStr++;
        }
        else                         
        {
            if((usX - ST7789V_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH )
            {
                usX = ST7789V_X_Star;
                usY += HEIGHT_CH_CHAR;
            }
            if((usY - ST7789V_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
            {
                usX = ST7789V_X_Star;
                usY = ST7789V_Y_Star;
            }    
            usCh = *(uint16_t*)pStr;    
            usCh = (usCh << 8) + (usCh >> 8);        
            ST7789V_DispChar_CH(usX, usY, usCh);
            usX += WIDTH_CH_CHAR;
            pStr += 2;         
    }
  }    
} 
// 在LCD上显示中英文字符串(沿Y轴方向)
void ST7789V_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY , char * pStr)
{
    uint16_t usCh;
    while(*pStr != '\0')
    {        
        if((usY - ST7789V_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
        {
            usY = ST7789V_Y_Star;
            usX += WIDTH_CH_CHAR;
        }            
        if((usX - ST7789V_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
        {
            usX = ST7789V_X_Star;
            usY = ST7789V_Y_Star;
        }
        if(*pStr <= 126)                   
        {            
            ST7789V_DispChar_EN(usX, usY, *pStr);
            pStr ++;
            usY += HEIGHT_CH_CHAR;        
        }
        else                                
        {            
            usCh = * (uint16_t*)pStr;    
            usCh = (usCh << 8) + (usCh >> 8);        
            ST7789V_DispChar_CH(usX, usY, usCh);
            usY += HEIGHT_CH_CHAR;
            pStr += 2;     
    }        
  }    
} 
// 设置英文字体类型
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}
// 获取当前字体类型
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
// 设置LCD的前景(字体)及背景颜色,RGB565
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}
// 获取LCD的前景(字体)及背景颜色,RGB565
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}
// 设置LCD的前景(字体)颜色,RGB565
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}
// 设置LCD的背景颜色,RGB565
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}
// 清除某行文字
void LCD_ClearLine(uint16_t Line)
{
  ST7789V_Clear(0,Line,LCD_X_LENGTH,((sFONT *)LCD_GetFont())->Height);
}
/* 缩放字体 */
#define ZOOMMAXBUFF 16384
uint8_t zoomBuff[ZOOMMAXBUFF] = {0}; // 用于缩放的缓存，最大支持到128*128
uint8_t zoomTempBuff[1024] = {0};
// 缩放字模，缩放后的字模由1个像素点由8个数据位来表示,0x01表示笔迹，0x00表示空白区
void ST7789V_zoomChar(uint16_t in_width,    // 原始字符宽度
                      uint16_t in_heig,        // 原始字符高度
                      uint16_t out_width,    // 缩放后的字符宽度
                      uint16_t out_heig,    // 缩放后的字符高度
                      uint8_t *in_ptr,        // 字库输入指针    注意：1pixel 1bit
                      uint8_t *out_ptr,     // 缩放后的字符输出指针 注意: 1pixel 8bit
                      uint8_t en_cn)        // 0为英文，1为中文    
{
    uint8_t *pts,*ots;
    // 根据源字模及目标字模大小，设定运算比例因子，左移16是为了把浮点运算转成定点运算
    unsigned int xrIntFloat_16=(in_width<<16)/out_width+1; 
    unsigned int yrIntFloat_16=(in_heig<<16)/out_heig+1;
    unsigned int srcy_16=0;
    unsigned int y,x;
    uint8_t *pSrcLine;
    uint16_t byteCount,bitCount;
    // 检查参数是否合法
    if(in_width >= 32) return;                                                
    if(in_width * in_heig == 0) return;    
    if(in_width * in_heig >= 1024 ) return;                 
    if(out_width * out_heig == 0) return;    
    if(out_width * out_heig >= ZOOMMAXBUFF) return; 
    pts = (uint8_t*)&zoomTempBuff;
    // 为方便运算，字库的数据由1 pixel/1bit 映射到1pixel/8bit
    // 0x01表示笔迹，0x00表示空白区
    if(en_cn == 0x00) // 英文
    {
        // 英文和中文字库上下边界不对，可在此处调整。需要注意tempBuff防止溢出
        for(byteCount=0;byteCount<in_heig*in_width/8;byteCount++)    
        {
            for(bitCount=0;bitCount<8;bitCount++)
            {                        
                // 把源字模数据由位映射到字节
                // in_ptr里bitX为1，则pts里整个字节值为1
                // in_ptr里bitX为0，则pts里整个字节值为0
                *pts++ = (in_ptr[byteCount] & (0x80>>bitCount))?1:0; 
            }
        }                
    }
    else // 中文
    {            
        for(byteCount=0;byteCount<in_heig*in_width/8;byteCount++)    
        {
            for(bitCount=0;bitCount<8;bitCount++)
            {                        
                *pts++ = (in_ptr[byteCount] & (0x80>>bitCount))?1:0; 
            }
        }        
    }
    // zoom过程
    pts = (uint8_t*)&zoomTempBuff;// 映射后的源数据指针
    ots = (uint8_t*)&zoomBuff;      // 输出数据的指针
    for (y=0;y<out_heig;y++)    /*行遍历*/
    {
        unsigned int srcx_16=0;
        pSrcLine=pts+in_width*(srcy_16>>16);                
        for(x=0;x<out_width;x++) /*行内像素遍历*/
        {
            ots[x]=pSrcLine[srcx_16>>16]; // 把源字模数据复制到目标指针中
            srcx_16+=xrIntFloat_16;          // 按比例偏移源像素点
        }
        srcy_16+=yrIntFloat_16;    // 按比例偏移源像素点
        ots+=out_width;                        
    }
    /* 缩放后的字模数据直接存储到全局指针zoomBuff里了 */
    out_ptr = (uint8_t*)&zoomBuff;    // out_ptr没有正确传出，后面调用直接改成了全局变量指针！
    out_ptr++; 
}            
// 利用缩放后的字模显示字符
void ST7789V_DrawChar_Ex(uint16_t usX,        // 字符显示位置x
                         uint16_t usY,        // 字符显示位置y
                         uint16_t Font_width, // 字符宽度
                         uint16_t Font_Height,// 字符高度 
                         uint8_t *c,          // 字模数据
                         uint16_t DrawModel)  // 是否反色显示
{
    uint32_t index = 0, counter = 0;
    ST7789V_OpenWindow(usX, usY, Font_width, Font_Height);
    ST7789V_Write_Cmd(CMD_SetPixel);        
    // 按字节读取字模数据
    for(index = 0; index < Font_Height; index++)
    {
        // 一位一位处理要显示的颜色
        for(counter = 0; counter < Font_width; counter++)
        {
            if(*c++ == DrawModel)
            {
                ST7789V_Write_Data(CurrentBackColor);
            }            
            else
            {
                ST7789V_Write_Data(CurrentTextColor);
            }
        }    
    }    
}
// 利用缩放后的字模显示字符串
void ST7789V_DisplayStringEx(uint16_t x, uint16_t y, uint16_t Font_width, uint16_t Font_Height,    uint8_t *ptr, uint16_t DrawModel)
{
    uint16_t Charwidth = Font_width; // 默认为Font_width，英文宽度为中文宽度的一半
    uint8_t *psr;
    uint8_t Ascii;    // 英文
    uint16_t usCh;  // 中文
    uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];    
    while(*ptr != '\0')
    {
        if((x - ST7789V_X_Star + Charwidth) > LCD_X_LENGTH)
            {
                x = ST7789V_X_Star;
                y += Font_Height;
            }
        if((y - ST7789V_Y_Star + Font_Height) > LCD_Y_LENGTH)
        {
            x = ST7789V_X_Star;
            y = ST7789V_Y_Star;
        }        
        if(*ptr > 0x80) // 如果是中文
        {            
            Charwidth = Font_width;
            usCh = *(uint16_t*)ptr;                
            usCh = (usCh << 8) + (usCh >> 8);
            GetGBKCode(ucBuffer, usCh);    // 取字模数据
            // 缩放字模数据，源字模为16*16
            ST7789V_zoomChar(WIDTH_CH_CHAR,HEIGHT_CH_CHAR,Charwidth,Font_Height,(uint8_t *)&ucBuffer,psr,1); 
            //显示单个字符
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            x+=Charwidth;
            ptr+=2;
        }
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32;
            // 使用16*24字体缩放字模数据
            ST7789V_zoomChar(16,24,Charwidth,Font_Height,(uint8_t *)&Font16x24.table[Ascii * Font16x24.Height*Font16x24.Width/8],psr,0);
            //显示单个字符
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            x+=Charwidth;
            ptr++;
        }
    }
}
// 利用缩放后的字模显示字符串(沿Y轴方向)
void ST7789V_DisplayStringEx_YDir(uint16_t x, uint16_t y, uint16_t Font_width, uint16_t Font_Height, uint8_t *ptr, uint16_t DrawModel)  
{
    uint16_t Charwidth = Font_width; 
    uint8_t *psr;
    uint8_t Ascii;    
    uint16_t usCh;  
    uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];    
    while(*ptr != '\0')
    {            
        // 统一使用汉字的宽高来计算换行
        if((y - ST7789V_X_Star + Font_width) > LCD_X_LENGTH)
        {
            y = ST7789V_X_Star;
            x += Font_width;
        }
        if(( x - ST7789V_Y_Star + Font_Height) > LCD_Y_LENGTH)
        {
            y = ST7789V_X_Star;
            x = ST7789V_Y_Star;
        }        
        if(*ptr > 0x80)
        {            
            Charwidth = Font_width;
            usCh =* (uint16_t*)ptr;                
            usCh = (usCh << 8) + (usCh >> 8);
            GetGBKCode(ucBuffer, usCh);
            ST7789V_zoomChar(WIDTH_CH_CHAR,HEIGHT_CH_CHAR,Charwidth,Font_Height,(uint8_t *)&ucBuffer,psr,1); 
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            y+=Font_Height;
            ptr+=2;
        }
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32;
            ST7789V_zoomChar(16,24,Charwidth,Font_Height,(uint8_t *)&Font16x24.table[Ascii * Font16x24.Height*Font16x24.Width/8],psr,0);
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            y+=Font_Height;
            ptr++;
        }
    }
}
```

#### 2.2.4 主函数测试

```c
#include "stm32f10x.h"
#include "lcd.h"
#include "flash.h"
#include <stdio.h>
#include "usart.h"

static void LCD_Test(void);    
static void Delay (__IO uint32_t nCount);

int main(void)
{
    USART_Config();
    ST7789V_Init(); 
      ST7789V_GramScan(6);
    while(1)
    {
        LCD_Test();
    }
}
extern uint16_t lcdid;
void LCD_Test(void)
{
    static uint8_t testCNT = 0;    
    char dispBuff[100];
    testCNT++;
    LCD_SetFont(&Font8x16); 
    LCD_SetColors(RED, BLACK); 
      ST7789V_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
      /* 显示字符串测试 */
      if(lcdid == LCDID_ST7789V)
      {
           ST7789V_DispStringLine_EN_CH(LINE(1),"ST7789V 驱动 3.2寸LCD");
      }
    LCD_SetTextColor(GREEN);
    sprintf(dispBuff,"显示变量： %d ",testCNT);
    LCD_ClearLine(LINE(2));    
    ST7789V_DispStringLine_EN_CH(LINE(2),dispBuff);
      LCD_ClearLine(LINE(2));
    LCD_SetTextColor(BLUE);
      ST7789V_DispStringLine_EN_CH(LINE(2),"画直线："); 
    LCD_SetTextColor(RED);
      ST7789V_DrawLine(50,170,210,230);  
      ST7789V_DrawLine(50,200,210,240);
    LCD_SetTextColor(GREEN);
      ST7789V_DrawLine(100,170,200,230);  
      ST7789V_DrawLine(200,200,220,240);
    LCD_SetTextColor(BLUE);
      ST7789V_DrawLine(110,170,110,230);  
      ST7789V_DrawLine(130,200,220,240);
      Delay(0xFFFFFF);
      ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
      LCD_ClearLine(LINE(2));
    LCD_SetTextColor(BLUE);
      ST7789V_DispStringLine_EN_CH(LINE(2),"画矩形：");
    LCD_SetTextColor(RED);
      ST7789V_DrawRectangle(50,200,100,30,1);
    LCD_SetTextColor(GREEN);
      ST7789V_DrawRectangle(160,200,20,40,0);
    LCD_SetTextColor(BLUE);
      ST7789V_DrawRectangle(170,200,50,20,1);
      Delay(0xFFFFFF);
    ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
      LCD_ClearLine(LINE(2));
    LCD_SetTextColor(BLUE);
    ST7789V_DispStringLine_EN_CH(LINE(2),"画圆");
    LCD_SetTextColor(RED);
      ST7789V_DrawCircle(100,200,20,0);
    LCD_SetTextColor(GREEN);
      ST7789V_DrawCircle(100,200,10,1);    
    LCD_SetTextColor(BLUE);
    ST7789V_DrawCircle(140,200,20,0);
    Delay(0xFFFFFF);
    ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
}
static void Delay(__IO uint32_t nCount) 
{
  for(; nCount != 0; nCount --);

}
```

---

2024.9.28 第一次修订，后期不再维护

2025.2.5 程序大修
