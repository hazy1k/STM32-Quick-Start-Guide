#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "stm32f10x.h"
#include "stm32f10x_sdio.h"  // 添加SDIO相关定义

// SD卡类型定义
typedef enum {
    SD_CARD_TYPE_UNKNOWN = 0,
    SD_CARD_TYPE_STD_CAPACITY = 1,
    SD_CARD_TYPE_HIGH_CAPACITY = 2,
    SD_CARD_TYPE_MMC = 3
} SDCardType;

// SD卡状态定义
typedef enum {
    SD_OK = 0,
    SD_ERROR,
    SD_TIMEOUT,
    SD_ERROR_CMD_CRC,
    SD_ERROR_DATA_CRC,
    SD_ERROR_RESP_TIMEOUT,
    SD_ERROR_VOLTAGE,
    SD_ERROR_CMD_RSP,
    SD_CARD_LOCKED,
    SD_INVALID_VOLTRANGE
} SDStatus;

// SD卡信息结构体
typedef struct {
    SDCardType CardType;       // SD卡类型
    uint32_t CardVersion;      // SD卡版本
    uint32_t Class;            // SD卡类别
    uint32_t RCA;              // 相对卡地址
    uint32_t Capacity;         // 容量（块数）
    uint32_t BlockSize;        // 块大小
} SD_CardInfo;

// SDIO命令定义
#define SD_CMD0_GO_IDLE_STATE           0
#define SD_CMD8_SEND_IF_COND            8
#define SD_CMD55_APP_CMD                55
#define SD_CMD41_SD_APP_OP_COND         41
#define SD_CMD2_ALL_SEND_CID            2
#define SD_CMD3_SEND_RELATIVE_ADDR      3
#define SD_CMD9_SEND_CSD                9
#define SD_CMD7_SELECT_DESELECT_CARD    7
#define SD_CMD16_SET_BLOCKLEN           16
#define SD_CMD17_READ_SINGLE_BLOCK      17
#define SD_CMD24_WRITE_SINGLE_BLOCK     24
#define SD_CMD13_SEND_STATUS            13
#define SD_ACMD41_SD_APP_OP_COND        41
#define SD_ACMD6_SET_BUS_WIDTH          6
#define SD_CMD18_READ_MULTIPLE_BLOCK    18
#define SD_CMD25_WRITE_MULTIPLE_BLOCK   25

// 公共接口函数
void SDIO_Cmd(FunctionalState NewState);
SDStatus SD_Init(void);
SDStatus SD_GetCardInfo(SD_CardInfo *cardinfo);
SDStatus SD_ReadBlock(uint8_t *buffer, uint32_t block, uint16_t count);
SDStatus SD_WriteBlock(uint8_t *buffer, uint32_t block, uint16_t count);
uint8_t SD_IsDetected(void);
SDStatus SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t response);  // 添加声明

#endif /* __SDIO_SDCARD_H */
