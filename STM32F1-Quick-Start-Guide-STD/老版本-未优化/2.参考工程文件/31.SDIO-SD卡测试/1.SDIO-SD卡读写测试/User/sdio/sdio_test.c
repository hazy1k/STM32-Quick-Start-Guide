#include "sdio_test.h"
#include "led.h"
#include "sdio_sdcard.h"
#include "usart.h"

/* Private typedef */
typedef enum {FAILED = 0, PASSED = !FAILED}TestStatus; // 测试状态枚举类型

/* Private define */
#define BLOCK_SIZE 512 // 块大小
#define NUMBER_OF_BLOCKS 10 // 块数量
#define MULTI_BUFFER_SIZE (BLOCK_SIZE * NUMBER_OF_BLOCKS) // 多块读写缓冲区大小

/* Private variables */
uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;

/* Private function prototypes */
static void SD_EraseTest(void);
static void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
static void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
static TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength);

/* Private functions */
void SD_Test(void)
{
  LED_BLUE();
  if((Status = SD_Init()) != SD_OK)
  {
    LED_RED();
    printf("SD init error\r\n");
  }
  else
  {
    printf("SD init success\r\n");
  }
  if(Status == SD_OK)
  {
    LED_BLUE();
    SD_EraseTest(); // 扇区擦除测试
    LED_BLUE();
    SD_SingleBlockTest(); // 单块读写测试
    LED_BLUE();
    SD_MultiBlockTest(); // 多块读写测试
  }
}

// 扇区擦除测试
void SD_EraseTest(void)
{
  if(Status == SD_OK)
  {
    Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS)); // 擦除扇区
  }
  if(Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS); // 读取擦除后的扇区内容
    Status = SD_WaitReadOperation(); // 等待读取操作完成
    while(SD_GetStatus() != SD_TRANSFER_OK); // 等待传输完成
  }
  if(Status == SD_OK)
  {
    EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  if(EraseStatus == PASSED)
  {
    LED_GREEN();
    printf("SD erase test success\r\n");
  }
  else
  {
    LED_RED();
    printf("SD erase test failed\r\n");
  }
}

// 单块读写测试
void SD_SingleBlockTest(void)
{
  Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);
  if(Status == SD_OK)
  {
    Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE); // 写入单块数据
    Status = SD_WaitWriteOperation(); // 等待写入操作完成
    while(SD_GetStatus() != SD_TRANSFER_OK); // 等待传输完成
  }
  if(Status == SD_OK)
  {
    Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE); // 读取单块数据
    Status = SD_WaitReadOperation(); // 等待读取操作完成
    while(SD_GetStatus() != SD_TRANSFER_OK); // 等待传输完成
  }
  if(Status == SD_OK)
  {
    TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE); // 比较读写数据
  }
  if(TransferStatus1 == PASSED)
  {
    LED_GREEN();
    printf("SD single block test success\r\n");
  }
  else
  {
    LED_RED();
    printf("SD single block test failed\r\n");
  }
}

// 多块读写测试
void SD_MultiBlockTest(void)
{
  Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x00);
  if(Status == SD_OK)
  {
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS); // 写入多块数据
    Status = SD_WaitWriteOperation(); // 等待写入操作完成
    while(SD_GetStatus() != SD_TRANSFER_OK); // 等待传输完成
  }
  if(Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS); // 读取多块数据
    Status = SD_WaitReadOperation(); // 等待读取操作完成
    while(SD_GetStatus() != SD_TRANSFER_OK); // 等待传输完成
  }
  if(Status == SD_OK)
  {
    TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE); // 比较读写数据
  }
  if(TransferStatus2 == PASSED)
  {
    LED_GREEN();
    printf("SD multi block test success\r\n");
  }
  else
  {
    LED_RED();
    printf("SD multi block test failed\r\n");
  }
}

// 比较两个缓冲区是否相同
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint16_t index = 0;
  for(index = 0; index < BufferLength; index++)
  {
    pBuffer[index] = (uint8_t)(index + Offset);
  }
}

TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if((*pBuffer != 0xFF)&&(pBuffer != 0x00))
    {
      return FAILED;
    }
    pBuffer++;
  }
  return PASSED;
}
