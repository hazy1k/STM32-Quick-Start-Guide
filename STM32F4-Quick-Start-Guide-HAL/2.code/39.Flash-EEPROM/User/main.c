#include "bsp_init.h"
#include "stdio.h"
#include "stmflash.h"
#include "string.h"

const uint8_t text_buf[] = {"STM32 FLASH TEST"};
#define TEXT_LEN sizeof(text_buf)

/* 计算需要写入的字数(32位) */
#define SIZE ((TEXT_LEN + 3) / 4)  // 标准4字节对齐计算

/* 设置FLASH 保存地址(必须为4的整数倍，且其值要大于本代码所占用FLASH的大小 + 0X08000000) */
#define FLASH_SAVE_ADDR 0x08010000 

/* 获取保存地址所在的扇区 */
#define FLASH_SAVE_SECTOR stmflash_get_flash_sector(FLASH_SAVE_ADDR)

int main(void)
{
    uint8_t key_value = 0;
    uint16_t i = 0;
    uint8_t databuf[SIZE * 4];  // 确保足够大的缓冲区
    
    bsp_init();
    LCD_ShowString(30,110,200,16,16,"KEY0:Write KEY1:Read");
    LCD_ShowString(30,130,200,16,16,"KEY2:Erase Sector");
    
    // 初始化数据缓冲区
    memset(databuf, 0, sizeof(databuf)); // 清空缓冲区
    memcpy(databuf, text_buf, TEXT_LEN); // 复制实际数据
    
    while(1)
    {
        key_value = key_scan(0);
        
        if(key_value == KEY0_Press) // 按下KEY0，写入数据
        {
            LCD_Fill(30,150,240,190,WHITE); // 清除显示区域
            LCD_ShowString(30,150,200,16,16,"Start Write FLASH....");
            stmflash_write(FLASH_SAVE_ADDR, (uint32_t*)databuf, SIZE);
            LCD_ShowString(30,150,200,16,16,"Write FLASH Success !");
        }
        else if(key_value == KEY1_Press) // 按下KEY1，读取数据
        {
            LCD_Fill(30,150,240,190,WHITE); // 清除显示区域
            LCD_ShowString(30,150,200,16,16,"Start Read FLASH....");
            
            // 清空缓冲区确保显示正确
            memset(databuf, 0, sizeof(databuf));
            stmflash_read(FLASH_SAVE_ADDR, (uint32_t*)databuf, SIZE);
            
            // 确保字符串以null结尾
            databuf[sizeof(databuf) - 1] = '\0';
            
            LCD_ShowString(30,150,200,16,16,"The Data Readed Is:  ");
            
            // 检查是否为擦除状态 (全0xFF)
            uint8_t is_erased = 1;
            for(int j = 0; j < sizeof(databuf); j++) {
                if(databuf[j] != 0xFF) {
                    is_erased = 0;
                    break;
                }
            }
            
            if(is_erased) {
                LCD_ShowString(30,170,200,16,16,"[SECTOR ERASED]");
            } else {
                LCD_ShowString(30,170,200,16,16,(char*)databuf);
            }
        }
        else if(key_value == KEY2_Press) // 按下KEY2，擦除扇区
        {
            LCD_Fill(30,150,240,190,WHITE); // 清除显示区域
            LCD_ShowString(30,150,200,16,16,"Erasing Sector...");
            
            // 擦除保存地址所在的扇区
            stmflash_erase_sector(FLASH_SAVE_SECTOR);
            
            LCD_ShowString(30,150,200,16,16,"Erase Complete!   ");
            LCD_ShowString(30,170,200,16,16,"Sector Cleared to 0xFF");
        }
        
        i++;
        delay_ms(10);
        if(i == 20)
        {
            i = 0;
            LED_TOGGLE(LED0_GPIO_Pin);
        }
    }
}
