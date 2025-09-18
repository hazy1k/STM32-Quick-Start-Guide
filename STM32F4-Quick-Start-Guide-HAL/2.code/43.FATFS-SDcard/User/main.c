#include "bsp_init.h"
#include "exfuns.h"
#include "sdio.h"

int main(void)
{
  uint32_t total, free;
  uint8_t i = 0;
  uint8_t res =0;
  bsp_init();
  LCD_ShowString(30,70,200,16,16,"FATFS-Test");
  while(1);
  while(sd_init())
  {
    LCD_ShowString(30,150,200,16,16,"SD Card Error!");
    delay_ms(500);
    LCD_ShowString(30,150,200,16,16,"Please Check! ");
    delay_ms(500);
  }
  exfuns_init(); // 为fatfs相关变量申请内存
  f_mount(fs[0],"0:",1); // 挂载SD卡
  res = f_mount(fs[1],"1:",1); // 挂载flash
  if(res = 0x0D)
  {
    LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");
    res = f_mkfs("1:",0,0,FF_MAX_SS);
    if(res == 0)
    {
      f_setlabel((const TCHAR*)"1:FLASH");
      LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");
    }
    else
    {
      LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");
    }
    delay_ms(1000);
  }
  while(exfuns_get_free("0",&total,&free)) // 获取SD卡剩余空间
  {
    LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
    delay_ms(200);
    LED_TOGGLE(LED0_GPIO_Pin);
  }
  LCD_ShowString(30,150,200,16,16,"FATFS OK!");
  LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");
  LCD_ShowString(30,190,200,16,16,"SD Free Size:      MB");
  while(1)
  {
    i++;
    delay_ms(200);
    LED_TOGGLE(LED1_GPIO_Pin);
  }

}