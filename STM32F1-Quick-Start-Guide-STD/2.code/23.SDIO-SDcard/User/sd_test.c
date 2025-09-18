#include "sd_test.h"
#include "sdio_sdcard.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

// 测试缓冲区
static uint8_t SD_TestBuffer[SD_TEST_BUFFER_SIZE];
static uint8_t SD_ReadBuffer[SD_TEST_BUFFER_SIZE];

// SD卡测试函数
void SD_Test(void) {
    SDStatus status;
    SD_CardInfo cardinfo;
    uint32_t i;
    
    printf("Starting SD Card Test...\r\n");
    
    // 1. 初始化SD卡
    status = SD_Init();
    if (status != SD_OK) {
        printf("SD Initialization Failed! Error: %d\r\n", status);
        return;
    }
    printf("SD Initialization Succeeded!\r\n");
    
    // 2. 获取卡信息
    status = SD_GetCardInfo(&cardinfo);
    if (status != SD_OK) {
        printf("Get Card Info Failed! Error: %d\r\n", status);
        return;
    }
    
    // 打印卡信息
    printf("SD Card Information:\r\n");
    printf("  Type: %s\r\n", (cardinfo.CardType == SD_CARD_TYPE_HIGH_CAPACITY) ? "SDHC/SDXC" : "SDSC");
    printf("  Capacity: %lu MB\r\n", (cardinfo.Capacity * cardinfo.BlockSize) / (1024 * 1024));
    printf("  Block Size: %lu bytes\r\n", cardinfo.BlockSize);
    printf("  RCA: 0x%04lX\r\n", cardinfo.RCA);
    
    // 3. 准备测试数据
    for (i = 0; i < SD_TEST_BUFFER_SIZE; i++) {
        SD_TestBuffer[i] = i % 256; // 填充模式数据
    }
    
    // 4. 写入数据（块地址0x1000）
    printf("Writing test data to block 0x1000...\r\n");
    status = SD_WriteBlock(SD_TestBuffer, 0x1000, 1);
    if (status != SD_OK) {
        printf("Write Failed! Error: %d\r\n", status);
        return;
    }
    printf("Write Succeeded!\r\n");
    
    // 5. 读取数据
    printf("Reading data from block 0x1000...\r\n");
    status = SD_ReadBlock(SD_ReadBuffer, 0x1000, 1);
    if (status != SD_OK) {
        printf("Read Failed! Error: %d\r\n", status);
        return;
    }
    printf("Read Succeeded!\r\n");
    
    // 6. 比较数据
    printf("Verifying data...\r\n");
    for (i = 0; i < SD_TEST_BUFFER_SIZE; i++) {
        if (SD_TestBuffer[i] != SD_ReadBuffer[i]) {
            printf("Data mismatch at byte %lu! Expected: 0x%02X, Read: 0x%02X\r\n", 
                   i, SD_TestBuffer[i], SD_ReadBuffer[i]);
            printf("Verification Failed!\r\n");
            return;
        }
    }
    printf("Verification Succeeded! All data matches.\r\n");
    
    printf("SD Card Test Completed Successfully!\r\n");
}
