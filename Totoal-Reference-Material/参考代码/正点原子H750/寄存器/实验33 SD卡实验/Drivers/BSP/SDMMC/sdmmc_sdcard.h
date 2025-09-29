/**
 ****************************************************************************************************
 * @file        sdmmc_sdcard.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-25
 * @brief       SD卡 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200403
 * 第一次发布
 * V1.1 20220525
 * 1,去掉全局数组,将数据读写方式改成HAL库版本形式,以提高读写性能
 * 2,修改sd_read_disk/sd_write_disk/sdmmc_read_blocks/sdmmc_write_blocks等4个函数
 *
 ****************************************************************************************************
 */

#ifndef __SDMMC_SDCARD_H
#define __SDMMC_SDCARD_H

#include "./SYSTEM/sys/sys.h"


/* 用户配置区
 * SDMMC时钟计算公式: SDMMC_CK时钟 = sdmmc_ker_ck / [2 * clkdiv]; 其中,sdmmc_ker_ck来自pll1_q_ck,为240Mhz
 * 如果出现驱动错误,请尝试将SDMMC_TRANSFER_CLK_DIV频率降低
 * 直接使用pll1_q_ck时钟源的时候,操作SD卡时,非常容易出现CRC错误.
 */
#define SDMMC_INIT_CLK_DIV        300       /* SDMMC初始化频率，240M/(300*2)=400Khz,最大400Kh */
#define SDMMC_TRANSFER_CLK_DIV    3         /* SDMMC传输频率,该值太小可能会导致读写文件出错 */



/******************************************************************************************/
/* SDMMC1的信号线: SD1_D0 ~ SD1_D3/SD1_CLK/SD1_CMD 引脚 定义 
 * 如果你使用了其他引脚做SDMMC1的信号线,修改这里写定义即可适配.
 * 注意, 这里仅支持SDMMC1, 我们使用的也是SDMMC1. 不支持SDMMMC2.
 * 另外, 由于SDMMC1的IO口复用功能都是AF12, 这里就不独立再定义每个IO口的AF功能了
 */

#define SD1_D0_GPIO_PORT                GPIOC
#define SD1_D0_GPIO_PIN                 SYS_GPIO_PIN8
#define SD1_D0_GPIO_CLK_ENABLE()        do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* 所在IO口时钟使能 */

#define SD1_D1_GPIO_PORT                GPIOC
#define SD1_D1_GPIO_PIN                 SYS_GPIO_PIN9
#define SD1_D1_GPIO_CLK_ENABLE()        do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* 所在IO口时钟使能 */

#define SD1_D2_GPIO_PORT                GPIOC
#define SD1_D2_GPIO_PIN                 SYS_GPIO_PIN10
#define SD1_D2_GPIO_CLK_ENABLE()        do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* 所在IO口时钟使能 */

#define SD1_D3_GPIO_PORT                GPIOC
#define SD1_D3_GPIO_PIN                 SYS_GPIO_PIN11
#define SD1_D3_GPIO_CLK_ENABLE()        do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* 所在IO口时钟使能 */

#define SD1_CLK_GPIO_PORT               GPIOC
#define SD1_CLK_GPIO_PIN                SYS_GPIO_PIN12
#define SD1_CLK_GPIO_CLK_ENABLE()       do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* 所在IO口时钟使能 */

#define SD1_CMD_GPIO_PORT               GPIOD
#define SD1_CMD_GPIO_PIN                SYS_GPIO_PIN2
#define SD1_CMD_GPIO_CLK_ENABLE()       do{ RCC->AHB4ENR |= 1 << 3; }while(0)   /* 所在IO口时钟使能 */

/******************************************************************************************/



/* SD卡操作 各种错误枚举定义 */
typedef enum
{
    /* 特殊错误定义 */
    SD_CMD_CRC_FAIL                    = (1),   /*!< Command response received (but CRC check failed)              */
    SD_DATA_CRC_FAIL                   = (2),   /*!< Data block sent/received (CRC check failed)                   */
    SD_CMD_RSP_TIMEOUT                 = (3),   /*!< Command response timeout                                      */
    SD_DATA_TIMEOUT                    = (4),   /*!< Data timeout                                                  */
    SD_TX_UNDERRUN                     = (5),   /*!< Transmit FIFO underrun                                        */
    SD_RX_OVERRUN                      = (6),   /*!< Receive FIFO overrun                                          */
    SD_START_BIT_ERR                   = (7),   /*!< Start bit not detected on all data signals in wide bus mode   */
    SD_CMD_OUT_OF_RANGE                = (8),   /*!< Command's argument was out of range.                          */
    SD_ADDR_MISALIGNED                 = (9),   /*!< Misaligned address                                            */
    SD_BLOCK_LEN_ERR                   = (10),  /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
    SD_ERASE_SEQ_ERR                   = (11),  /*!< An error in the sequence of erase command occurs.            */
    SD_BAD_ERASE_PARAM                 = (12),  /*!< An invalid selection for erase groups                        */
    SD_WRITE_PROT_VIOLATION            = (13),  /*!< Attempt to program a write protect block                     */
    SD_LOCK_UNLOCK_FAILED              = (14),  /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
    SD_COM_CRC_FAILED                  = (15),  /*!< CRC check of the previous command failed                     */
    SD_ILLEGAL_CMD                     = (16),  /*!< Command is not legal for the card state                      */
    SD_CARD_ECC_FAILED                 = (17),  /*!< Card internal ECC was applied but failed to correct the data */
    SD_CC_ERROR                        = (18),  /*!< Internal card controller error                               */
    SD_GENERAL_UNKNOWN_ERROR           = (19),  /*!< General or unknown error                                     */
    SD_STREAM_READ_UNDERRUN            = (20),  /*!< The card could not sustain data transfer in stream read operation. */
    SD_STREAM_WRITE_OVERRUN            = (21),  /*!< The card could not sustain data programming in stream mode   */
    SD_CID_CSD_OVERWRITE               = (22),  /*!< CID/CSD overwrite error                                      */
    SD_WP_ERASE_SKIP                   = (23),  /*!< Only partial address space was erased                        */
    SD_CARD_ECC_DISABLED               = (24),  /*!< Command has been executed without using internal ECC         */
    SD_ERASE_RESET                     = (25),  /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
    SD_AKE_SEQ_ERROR                   = (26),  /*!< Error in sequence of authentication.                         */
    SD_INVALID_VOLTRANGE               = (27),
    SD_ADDR_OUT_OF_RANGE               = (28),
    SD_SWITCH_ERROR                    = (29),
    SD_SDMMC_DISABLED                  = (30),
    SD_SDMMC_FUNCTION_BUSY             = (31),
    SD_SDMMC_FUNCTION_FAILED           = (32),
    SD_SDMMC_UNKNOWN_FUNCTION          = (33),
    /* 标准错误定义 */
    SD_INTERNAL_ERROR                  = (34),
    SD_NOT_CONFIGURED                  = (35),
    SD_REQUEST_PENDING                 = (36),
    SD_REQUEST_NOT_APPLICABLE          = (37),
    SD_INVALID_PARAMETER               = (38),
    SD_UNSUPPORTED_FEATURE             = (39),
    SD_UNSUPPORTED_HW                  = (40),
    SD_ERROR                           = (41),
    SD_OK                              = (0)
} SD_Error;

/* SD卡CSD寄存器数据 */
typedef struct
{
    uint8_t  CSDStruct;            /*!< CSD structure */
    uint8_t  SysSpecVersion;       /*!< System specification version */
    uint8_t  Reserved1;            /*!< Reserved */
    uint8_t  TAAC;                 /*!< Data read access-time 1 */
    uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
    uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
    uint16_t CardComdClasses;      /*!< Card command classes */
    uint8_t  RdBlockLen;           /*!< Max. read data block length */
    uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
    uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
    uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
    uint8_t  DSRImpl;              /*!< DSR implemented */
    uint8_t  Reserved2;            /*!< Reserved */
    uint32_t DeviceSize;           /*!< Device Size */
    uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
    uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
    uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
    uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
    uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
    uint8_t  EraseGrSize;          /*!< Erase group size */
    uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
    uint8_t  WrProtectGrSize;      /*!< Write protect group size */
    uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
    uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
    uint8_t  WrSpeedFact;          /*!< Write speed factor */
    uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
    uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
    uint8_t  Reserved3;            /*!< Reserded */
    uint8_t  ContentProtectAppli;  /*!< Content protection application */
    uint8_t  FileFormatGrouop;     /*!< File format group */
    uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
    uint8_t  PermWrProtect;        /*!< Permanent write protection */
    uint8_t  TempWrProtect;        /*!< Temporary write protection */
    uint8_t  FileFormat;           /*!< File Format */
    uint8_t  ECC;                  /*!< ECC code */
    uint8_t  CSD_CRC;              /*!< CSD CRC */
    uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;

/* SD卡CID寄存器数据 */
typedef struct
{
    uint8_t  ManufacturerID;       /*!< ManufacturerID */
    uint16_t OEM_AppliID;          /*!< OEM/Application ID */
    uint32_t ProdName1;            /*!< Product Name part1 */
    uint8_t  ProdName2;            /*!< Product Name part2*/
    uint8_t  ProdRev;              /*!< Product Revision */
    uint32_t ProdSN;               /*!< Product Serial Number */
    uint8_t  Reserved1;            /*!< Reserved1 */
    uint16_t ManufactDate;         /*!< Manufacturing Date */
    uint8_t  CID_CRC;              /*!< CID CRC */
    uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

/* SD卡状态 */
typedef enum
{
    SD_CARD_READY                  = ((uint32_t)0x00000001),
    SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
    SD_CARD_STANDBY                = ((uint32_t)0x00000003),
    SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
    SD_CARD_SENDING                = ((uint32_t)0x00000005),
    SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
    SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
    SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
    SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;

/* SD卡信息,包括CSD,CID等数据 */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  long long CardCapacity;       /* SD卡容量,单位:字节,最大支持2^64字节大小的卡. */
  uint32_t CardBlockSize;       /* SD卡块大小 */
  uint16_t RCA;                 /* 卡相对地址 */
  uint8_t CardType;             /* 卡类型 */
} SD_CardInfo;

extern SD_CardInfo g_sd_card_info;  /* SD卡信息 */

/******************************************************************************************/
/* SDMMC卡 指令集 */
/* 拷贝自:stm32f7xx_hal_sd.h */
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0U)   /*!< Resets the SD memory card.                                                               */
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1U)   /*!< Sends host capacity support information and activates the card's initialization process. */
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2U)   /*!< Asks any card connected to the host to send the CID numbers on the CMD line.             */
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3U)   /*!< Asks the card to publish a new relative address (RCA).                                   */
#define SD_CMD_SET_DSR                             ((uint8_t)4U)   /*!< Programs the DSR of all cards.                                                           */
#define SD_CMD_SDMMC_SEN_OP_COND                   ((uint8_t)5U)   /*!< Sends host capacity support information (HCS) and asks the accessed card to send its 
                                                                       operating condition register (OCR) content in the response on the CMD line.              */
#define SD_CMD_HS_SWITCH                           ((uint8_t)6U)   /*!< Checks switchable function (mode 0) and switch card function (mode 1).                   */
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7U)   /*!< Selects the card by its own relative address and gets deselected by any other address    */
#define SD_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8U)   /*!< Sends SD Memory Card interface condition, which includes host supply voltage information 
                                                                       and asks the card whether card supports voltage.                                         */
#define SD_CMD_SEND_CSD                            ((uint8_t)9U)   /*!< Addressed card sends its card specific data (CSD) on the CMD line.                       */
#define SD_CMD_SEND_CID                            ((uint8_t)10U)  /*!< Addressed card sends its card identification (CID) on the CMD line.                      */
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11U)  /*!< SD card doesn't support it.                                                              */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12U)  /*!< Forces the card to stop transmission.                                                    */
#define SD_CMD_SEND_STATUS                         ((uint8_t)13U)  /*!< Addressed card sends its status register.                                                */
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14U)
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15U)  /*!< Sends an addressed card into the inactive state.                                         */
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16U)  /*!< Sets the block length (in bytes for SDSC) for all following block commands 
                                                                       (read, write, lock). Default block length is fixed to 512 Bytes. Not effective 
                                                                       for SDHS and SDXC.                                                                       */
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17U)  /*!< Reads single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of 
                                                                       fixed 512 bytes in case of SDHC and SDXC.                                                */
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18U)  /*!< Continuously transfers data blocks from card to host until interrupted by 
                                                                       STOP_TRANSMISSION command.                                                               */
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19U)  /*!< 64 bytes tuning pattern is sent for SDR50 and SDR104.                                    */
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20U)  /*!< Speed class control command.                                                             */
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23U)  /*!< Specify block count for CMD18 and CMD25.                                                 */
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24U)  /*!< Writes single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of 
                                                                       fixed 512 bytes in case of SDHC and SDXC.                                                */
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25U)  /*!< Continuously writes blocks of data until a STOP_TRANSMISSION follows.                    */
#define SD_CMD_PROG_CID                            ((uint8_t)26U)  /*!< Reserved for manufacturers.                                                              */
#define SD_CMD_PROG_CSD                            ((uint8_t)27U)  /*!< Programming of the programmable bits of the CSD.                                         */
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28U)  /*!< Sets the write protection bit of the addressed group.                                    */
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29U)  /*!< Clears the write protection bit of the addressed group.                                  */
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30U)  /*!< Asks the card to send the status of the write protection bits.                           */
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32U)  /*!< Sets the address of the first write block to be erased. (For SD card only).              */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33U)  /*!< Sets the address of the last write block of the continuous range to be erased.           */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35U)  /*!< Sets the address of the first write block to be erased. Reserved for each command 
                                                                       system set by switch function command (CMD6).                                            */
#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36U)  /*!< Sets the address of the last write block of the continuous range to be erased. 
                                                                       Reserved for each command system set by switch function command (CMD6).                  */
#define SD_CMD_ERASE                               ((uint8_t)38U)  /*!< Reserved for SD security applications.                                                   */
#define SD_CMD_FAST_IO                             ((uint8_t)39U)  /*!< SD card doesn't support it (Reserved).                                                   */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40U)  /*!< SD card doesn't support it (Reserved).                                                   */
#define SD_CMD_LOCK_UNLOCK                         ((uint8_t)42U)  /*!< Sets/resets the password or lock/unlock the card. The size of the data block is set by 
                                                                       the SET_BLOCK_LEN command.                                                               */
#define SD_CMD_APP_CMD                             ((uint8_t)55U)  /*!< Indicates to the card that the next command is an application specific command rather 
                                                                       than a standard command.                                                                 */
#define SD_CMD_GEN_CMD                             ((uint8_t)56U)  /*!< Used either to transfer a data block to the card or to get a data block from the card 
                                                                       for general purpose/application specific commands.                                       */
#define SD_CMD_NO_CMD                              ((uint8_t)64U)

/**
  * @brief Following commands are SD Card Specific commands.
  *        SDMMC_APP_CMD should be sent before sending these commands.
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6U)   /*!< (ACMD6) Defines the data bus width to be used for data transfer. The allowed data bus 
                                                                       widths are given in SCR register.                                                          */
#define SD_CMD_SD_APP_STATUS                       ((uint8_t)13U)  /*!< (ACMD13) Sends the SD status.                                                              */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22U)  /*!< (ACMD22) Sends the number of the written (without errors) write blocks. Responds with 
                                                                       32bit+CRC data block.                                                                      */
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41U)  /*!< (ACMD41) Sends host capacity support information (HCS) and asks the accessed card to 
                                                                       send its operating condition register (OCR) content in the response on the CMD line.       */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42U)  /*!< (ACMD42) Connects/Disconnects the 50 KOhm pull-up resistor on CD/DAT3 (pin 1) of the card. */
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51U)  /*!< Reads the SD Configuration Register (SCR).                                                 */
#define SD_CMD_SDMMC_RW_DIRECT                     ((uint8_t)52U)  /*!< For SD I/O card only, reserved for security specification.                                 */
#define SD_CMD_SDMMC_RW_EXTENDED                   ((uint8_t)53U)  /*!< For SD I/O card only, reserved for security specification.                                 */

/**
  * @brief Following commands are SD Card Specific security commands.
  *        SD_CMD_APP_CMD should be sent before sending these commands.
  */
#define SD_CMD_SD_APP_GET_MKB                      ((uint8_t)43U)  /*!< For SD card only */
#define SD_CMD_SD_APP_GET_MID                      ((uint8_t)44U)  /*!< For SD card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((uint8_t)45U)  /*!< For SD card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((uint8_t)46U)  /*!< For SD card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((uint8_t)47U)  /*!< For SD card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((uint8_t)48U)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((uint8_t)18U)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((uint8_t)25U)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((uint8_t)38U)  /*!< For SD card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((uint8_t)49U)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((uint8_t)48U)  /*!< For SD card only */

/* CMD8指令 */
#define SD_SDMMC_SEND_IF_COND                      ((uint32_t)SD_CMD_HS_SEND_EXT_CSD)
/******************************************************************************************/

/* 支持的SD卡定义 */
#define STD_CAPACITY_SD_CARD_V1_1       ((uint32_t)0x00000000U)
#define STD_CAPACITY_SD_CARD_V2_0       ((uint32_t)0x00000001U)
#define HIGH_CAPACITY_SD_CARD           ((uint32_t)0x00000002U)
#define MULTIMEDIA_CARD                 ((uint32_t)0x00000003U)
#define SECURE_DIGITAL_IO_CARD          ((uint32_t)0x00000004U)
#define HIGH_SPEED_MULTIMEDIA_CARD      ((uint32_t)0x00000005U)
#define SECURE_DIGITAL_IO_COMBO_CARD    ((uint32_t)0x00000006U)
#define HIGH_CAPACITY_MMC_CARD          ((uint32_t)0x00000007U)

/* SDMMC相关参数定义 */
#define NULL 0
#define SDMMC_STATIC_FLAGS              ((uint32_t)0x000205FF)
#define SDMMC_CMD0TIMEOUT               ((uint32_t)0x00010000)
#define SDMMC_CMD1TIMEOUT               ((uint32_t)0x00FFFFFF)
#define SDMMC_DATATIMEOUT               ((uint32_t)0X00FFFFFF)

/* Mask for errors Card Status R1 (OCR Register) */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

/* Masks for R6 Response */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((uint32_t)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define SD_ALLZERO                      ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000)
#define SD_CARD_PROGRAMMING             ((uint32_t)0x00000007)
#define SD_CARD_RECEIVING               ((uint32_t)0x00000006)
#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

/* Command Class Supported */
#define SD_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT              ((uint32_t)0x00000040)
#define SD_CCCC_ERASE                   ((uint32_t)0x00000020)

/******************************************************************************************/

/* 静态函数, 仅sdmmc_sdcard.c内部使用 */
static SD_Error sdmmc_cmd_error(void);
static SD_Error sdmmc_cmd_resp1_error(uint8_t cmd);
static SD_Error sdmmc_cmd_resp2_error(void);
static SD_Error sdmmc_cmd_resp3_error(void);
static SD_Error sdmmc_cmd_resp6_error(uint8_t cmd, uint16_t *prca);
static SD_Error sdmmc_cmd_resp7_error(void);

static SD_Error sdmmc_wide_bus_enable(uint8_t enx);
static SD_Error sdmmc_wide_bus_operation(uint32_t bwide);
static SD_Error sdmmc_is_card_programming(uint8_t *pstatus);

static SD_Error sdmmc_send_status(uint32_t *pstatus);
static SD_Error sdmmc_find_scr(uint16_t rca, uint32_t *pscr);

static SD_Error sdmmc_power_on(void);
static void sdmmc_clock_set(uint16_t clkdiv);
static SD_Error sdmmc_initialize_cards(void);
static SD_Error sdmmc_select_deselect(uint32_t addr);
static SD_Error sdmmc_get_card_info(SD_CardInfo *cardinfo);
static void sdmmc_send_cmd(uint8_t cmdindex, uint8_t waitrsp, uint32_t arg);
static void sdmmc_send_data_cfg(uint32_t datatimeout, uint32_t datalen, uint8_t blksize, uint8_t dir);

static SD_Error sdmmc_read_blocks(uint8_t *pbuf, long long addr, uint16_t blksize, uint32_t nblks);
static SD_Error sdmmc_write_blocks(uint8_t *pbuf, long long addr, uint16_t blksize, uint32_t nblks);


/* 接口函数定义 */
SD_Error sd_init(void);
SDCardState sdmmc_get_status(void);

uint8_t  sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
uint8_t  sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);

#endif





