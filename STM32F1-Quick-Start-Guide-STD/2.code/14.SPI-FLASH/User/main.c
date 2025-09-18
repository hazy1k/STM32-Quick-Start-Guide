#include "stm32f10x.h"
#include "SysTick.h"
#include "usart.h"
#include "spi_flash.h"
#include <stdio.h>
#include <string.h>

// 测试数据缓冲区
#define TEST_DATA_SIZE 256
uint8_t write_buffer[TEST_DATA_SIZE];
uint8_t read_buffer[TEST_DATA_SIZE];

// 测试地址 (使用第一个扇区的起始地址)
#define TEST_ADDRESS 0x000000

/**
  * @brief  填充测试数据
  * @param  buffer: 数据缓冲区
  * @param  size: 数据大小
  */
void fill_test_data(uint8_t* buffer, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        // 生成可读性较好的测试数据
        buffer[i] = (i % 26) + 'A';  // 循环填充A-Z
    }
}

/**
  * @brief  比较两个缓冲区数据
  * @param  buf1: 缓冲区1
  * @param  buf2: 缓冲区2
  * @param  size: 比较大小
  * @retval 0: 相等, 其他: 不相等的位置
  */
int compare_buffers(uint8_t* buf1, uint8_t* buf2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        if (buf1[i] != buf2[i]) {
            return i + 1;  // 返回不相等的位置(从1开始)
        }
    }
    return 0;
}

int main(void) 
{
    uint32_t flash_id;
    int compare_result;
    
    // 初始化外设
    SysTick_Init();
    USARTx_Init();
    
    printf("\r\n===== SPI Flash Function Test =====\r\n");
    
    // 初始化SPI Flash
    printf("Initializing SPI Flash...\r\n");
    SPI_Flash_Init();
    printf("SPI Flash initialized successfully!\r\n");
    
    // 测试1: 读取设备ID
    printf("\r\n[Test 1] Reading Device ID...\r\n");
    flash_id = SPI_Flash_ReadID();
    printf("Device ID: 0x%06lX\r\n", flash_id);
    
    // 检查ID是否正确 (W25Q64的ID是0xEF4017)
    if (flash_id == sFLASH_ID) {
        printf("Device ID verification successful!\r\n");
    } else {
        printf("Error: Device ID mismatch!\r\n");
    }
    
    // 测试2: 扇区擦除
    printf("\r\n[Test 2] Erasing Sector (0x%06lX)...\r\n", TEST_ADDRESS);
    SPI_Flash_EraseSector(TEST_ADDRESS);
    printf("Sector erase completed!\r\n");
    
    // 测试3: 写入数据
    printf("\r\n[Test 3] Writing test data...\r\n");
    
    // 填充测试数据
    fill_test_data(write_buffer, TEST_DATA_SIZE);
    
    // 写入数据到Flash
    SPI_Flash_WriteBuffer(write_buffer, TEST_ADDRESS, TEST_DATA_SIZE);
    printf("Written %d bytes of data\r\n", TEST_DATA_SIZE);
    
    // 打印部分写入数据
    printf("First 16 bytes of written data: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", write_buffer[i]);
    }
    printf("\r\n");
    
    // 测试4: 读取数据
    printf("\r\n[Test 4] Reading data...\r\n");
    SPI_Flash_ReadBuffer(read_buffer, TEST_ADDRESS, TEST_DATA_SIZE);
    printf("Read %d bytes of data\r\n", TEST_DATA_SIZE);
    
    // 打印部分读取数据
    printf("First 16 bytes of read data: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", read_buffer[i]);
    }
    printf("\r\n");
    
    // 测试5: 数据验证
    printf("\r\n[Test 5] Verifying data...\r\n");
    compare_result = compare_buffers(write_buffer, read_buffer, TEST_DATA_SIZE);
    
    if (compare_result == 0) {
        printf("Data verification successful! Write and read data match.\r\n");
    } else {
        printf("Error: Data mismatch at position: %d\r\n", compare_result);
        printf("Expected: 0x%02X, Actual: 0x%02X\r\n", 
               write_buffer[compare_result-1], 
               read_buffer[compare_result-1]);
    }
    
    // 测试6: 低功耗模式测试
    printf("\r\n[Test 6] Power down mode test...\r\n");
    printf("Entering power down mode...\r\n");
    SPI_Flash_PowerDown();
    Delay_ms(1000);  // 保持1秒低功耗状态
    
    printf("Waking up device...\r\n");
    SPI_Flash_WakeUp();
    Delay_ms(100);  // 等待唤醒完成
    printf("Device awakened successfully!\r\n");
    
    // 最终状态指示
    printf("\r\n===== Test Completed =====\r\n");
    
    while(1) {
        // 主循环中不执行任何操作
    }
}
