#include "bsp_init.h"
#include "sdio.h"

void show_sd_info(void);
void test_sd_read(uint32_t addr, uint32_t cnt);
void test_sd_write(uint32_t addr, uint32_t cnt);

int main(void)
{
  uint32_t i = 0;
  uint8_t key_value;
  bsp_init();
  sd_init();
  show_sd_info();
  LCD_ShowString(30,110,200,16,16,"KEY0:Read Sector 0");
  LCD_ShowString(30,130,200,16,16,"SD Card OK    ");
  LCD_ShowString(30,150,200,16,16,"SD Card Size:     MB");
  LCD_ShowNum(30+13*8, 150, SD_TOTAL_SIZE_MB(&sdcard_handle),5,16);
	while(1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      test_sd_read(0,1);
    }
    i++;
    delay_ms(10);
    if(i == 20)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      i= 0;
    }
  }
}

// 通过串口打印SD卡相关信息
void show_sd_info(void)
{
  HAL_SD_CardCIDTypeDef CID;
  HAL_SD_GetCardCID(&sdcard_handle, &CID);
  get_sd_card_info(&sdcard_info_handle);
  switch(sdcard_info_handle.CardType)
  {
    case CARD_SDSC:
      if(sdcard_info_handle.CardVersion == CARD_V1_X)
      {
        printf("Card Type:SDSC V1\r\n");
      }
      else if(sdcard_info_handle.CardVersion == CARD_V2_X)
      {
        printf("Card Type:SDSC V2\r\n");
      }
      break;
    case CARD_SDHC_SDXC:
      printf("Card Type:SDHC\r\n");
      break;
    default:
      printf("Unknown Card Type\r\n");
      break;
  }
  printf("Card ManufacturerID:%d\r\n", CID.ManufacturerID);               /* 制造商ID */
  printf("Card RCA:%d\r\n", sdcard_info_handle.RelCardAdd);                    /* 卡相对地址 */
  printf("LogBlockNbr:%d \r\n", (uint32_t)(sdcard_info_handle.LogBlockNbr));   /* 显示逻辑块数量 */
  printf("LogBlockSize:%d \r\n", (uint32_t)(sdcard_info_handle.LogBlockSize)); /* 显示逻辑块大小 */
  printf("Card Capacity:%d MB\r\n", (uint32_t)SD_TOTAL_SIZE_MB(&sdcard_handle));  /* 显示容量 */
  printf("Card BlockSize:%d\r\n\r\n", sdcard_info_handle.BlockSize);           /* 显示块大小 */
}

// 测试SD卡读取,从addr地址开始,读取cnt个扇区的数据
// addr:扇区地址 cnt:扇区数
void test_sd_read(uint32_t addr, uint32_t cnt)
{
  uint32_t i;
  uint8_t *buf;
  uint8_t sta = 0;
  buf = mymalloc(SRAMIN, cnt * 512); // 申请内存
  sta = sd_read_disk(buf, addr, cnt); // 读取数据
  if(sta == 0)
  {
    LCD_ShowString(30,170,200,16,16,"USART1 Sending Data...");
     printf("SECTOR %d DATA:\r\n", addr);
     for(i = 0; i < cnt * 512; i++)
     {
      printf("%x ", buf[i]);
     }
     printf("\r\n Data End!\r\n");
     LCD_ShowString(30,170,200,16,16,"USART1 Send Data Done!");
  }
  else
  {
    printf("error: %d\r\n", sta);
    LCD_ShowString(30,170,200,16,16,"USART1 Send Data Failed!");
  }
  myfree(SRAMIN, buf); // 释放内存
}

// 从addr地址开始,写入cnt个扇区的数据
void test_sd_write(uint32_t addr, uint32_t cnt)
{
  uint32_t i;
  uint8_t *buf;
  uint8_t sta = 0;
  buf = mymalloc(SRAMIN, cnt * 512); // 申请内存
  // 填充数据
  for(i = 0; i < cnt * 512; i++)
  {
    buf[i] = i * 3;
  }
  sta = sd_write_disk(buf, addr, cnt); // 写入数据
  if(sta == 0)
  {
    printf("Write over\r\n");
  }
  else
  {
    printf("error: %d\r\n", sta);
  }
  myfree(SRAMIN, buf);
}








