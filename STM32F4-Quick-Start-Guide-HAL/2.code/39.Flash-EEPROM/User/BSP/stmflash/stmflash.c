#include "stmflash.h"
#include "delay.h"

/**
 * @brief       从指定地址读取一个字 (32位数据)
 * @param       faddr   : 读取地址 (此地址必须为4倍数!!)
 * @retval      读取到的数据 (32位)
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

/**
 * @brief       获取某个地址所在的flash扇区
 * @param       addr    : flash地址
 * @retval      0~11,即addr所在的扇区
 */
uint8_t  stmflash_get_flash_sector(uint32_t addr)
{
    if (addr < ADDR_FLASH_SECTOR_1) return FLASH_SECTOR_0;
    else if (addr < ADDR_FLASH_SECTOR_2) return FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_3) return FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_4) return FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_5) return FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_6) return FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_7) return FLASH_SECTOR_6;
    else if (addr < ADDR_FLASH_SECTOR_8) return FLASH_SECTOR_7;
    else if (addr < ADDR_FLASH_SECTOR_9) return FLASH_SECTOR_8;
    else if (addr < ADDR_FLASH_SECTOR_10) return FLASH_SECTOR_9;
    else if (addr < ADDR_FLASH_SECTOR_11) return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}

/**
 * @brief       获取扇区大小
 * @param       sector  : 扇区号(0-11)
 * @retval      扇区大小(字节)
 */
static uint32_t get_sector_size(uint8_t sector)
{
    if (sector < 4) return 16 * 1024;      // 扇区0-3: 16KB
    else if (sector == 4) return 64 * 1024; // 扇区4: 64KB
    else return 128 * 1024;                // 扇区5-11: 128KB
}

/**
 * @brief       在FLASH 指定位置, 写入指定长度的数据(自动擦除)
 *   @note      因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数写地址如果非0XFF
 *              ,那么会先擦除整个扇区且不保存扇区数据.所以写非0XFF的地址,将导致整个扇区数据丢失.
 *              建议写之前确保扇区里没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
 *              该函数对OTP区域也有效!可以用来写OTP区!
 *              OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(注意：最后16字节，用于OTP数据块锁定，别乱写！！)
 * @param       waddr   : 起始地址 (此地址必须为4的倍数!!,否则写入出错!)
 * @param       pbuf    : 数据指针
 * @param       length  : 要写入的 字(32位)数(就是要写入的32位数据的个数)
 * @retval      无
 */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t addrx = waddr;
    uint32_t endaddr = waddr + length * 4;
    uint32_t sectorerror = 0;
    
    /* 写入地址小于 STM32_FLASH_BASE, 或不是4的整数倍, 非法. */
    /* 写入地址大于 STM32_FLASH_BASE + STM32_FLASH_SIZE, 非法. */
    if(waddr < STM32_FLASH_BASE || waddr % 4 || waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE)){
        return;
    }
    
    HAL_FLASH_Unlock(); // 解锁FLASH
    FLASH->ACR &= ~(1<<10); // 禁止指令缓存
    
    if(addrx < 0x1FFF0000) // 只有主存储区,才需要执行擦除操作!!
    {
        while(addrx < endaddr) // 扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
        {
            // 修复: 检查当前地址的值是否为0xFFFFFFFF
            if(stmflash_read_word(addrx) != 0xFFFFFFFF)
            {
                uint8_t sector = stmflash_get_flash_sector(addrx);
                uint32_t sector_size = get_sector_size(sector);
                
                EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS; // 擦除类型：扇区擦除
                EraseInitStruct.Sector = sector; // 要擦除的扇区号
                EraseInitStruct.NbSectors = 1; // 一次只擦除一个扇区
                EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // VCC=2.7~3.6V之间
                
                // 等待上次操作完成
                FLASH_WaitForLastOperation(FLASH_WATTETIME);
                
                if(HAL_FLASHEx_Erase(&EraseInitStruct, &sectorerror) != HAL_OK) // 擦除失败,退出循环
                {
                    break;
                }
                
                // 重要: 擦除后跳过整个扇区
                addrx = ADDR_FLASH_SECTOR_0 + (sector + 1) * sector_size;
            }
            else
            {
                addrx += 4; // 地址已经是0xFFFFFFFF, 跳过
            }
        }
    }
    
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WATTETIME); // 等待上次操作完成
    if(FlashStatus == HAL_OK)
    {
        while(waddr < endaddr) // 写入数据
        {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, waddr, *pbuf) != HAL_OK)
            {
                break;
            }
            waddr += 4;
            pbuf++;
        }
    }
    
    FLASH->ACR |= (1<<10); // 开启指令缓存
    HAL_FLASH_Lock(); // 锁定FLASH
}

/**
 * @brief       从指定地址开始读出指定长度的数据
 * @param       raddr : 起始地址
 * @param       pbuf  : 数据指针
 * @param       length: 要读取的字(32)数,即4个字节的整数倍
 * @retval      无
 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_word(raddr);   
        raddr += 4; 
    }
}

/**
* @brief       擦除指定扇区
* @param       sector  : 要擦除的扇区号(0~11)
* @retval      无
*/
void stmflash_erase_sector(uint8_t sector)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t sector_error = 0; // 用于存储擦除过程中出错的扇区地址
    // 检查扇区号是否有效
    if (sector > 11) {
        return;
    }
    HAL_FLASH_Unlock(); // 解锁FLASH，允许编程和擦除操作
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS; // 设置擦除类型为扇区擦除
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 设置工作电压范围，对于STM32F407，通常为VCC=2.7~3.6V
    EraseInitStruct.Sector = sector; // 指定要擦除的扇区
    EraseInitStruct.NbSectors = 1; // 指定要擦除的扇区数量，这里只擦除一个扇区
    // 等待上一次FLASH操作完成，确保FLASH空闲
    // FLASH_WaitForLastOperation(FLASH_WATTETIME); // HAL_FLASHEx_Erase 内部会等待
    // 执行扇区擦除操作
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &sector_error) != HAL_OK)
    {
        // 擦除操作失败
    }
    else
    {
        // 擦除操作成功
    }
    HAL_FLASH_Lock(); // 锁定FLASH，禁止编程和擦除操作
}
