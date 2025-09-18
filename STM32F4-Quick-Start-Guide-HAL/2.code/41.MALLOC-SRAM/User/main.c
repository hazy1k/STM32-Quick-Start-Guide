#include "bsp_init.h"
#include "stdio.h"
#include "SerialInvoker.h"
#include "sram.h"
#include "malloc.h"

const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN  ", " SRAMCCM ", " SRAMEX  "};

int main(void)
{
  uint8_t paddr[20]; // 存放P Addr:+p地址的ASCII值
  uint16_t memused = 0; // 记录已用SRAM大小
  uint8_t key_value = 0;
  uint8_t i = 0;
  uint8_t *p = 0;
  uint8_t *tp = 0;
  uint8_t sramx = 0; // 默认为内部SRAM
  bsp_init();
  sram_init();
	serial_invoker_init(84);
  my_mem_init(SRAMIN); // 初始化内部SRAM
  my_mem_init(SRAMCCM); // 初始化CCM SRAM
  my_mem_init(SRAMEX); // 初始化外部SRAM
  LCD_ShowString(30,110,200,16,16,"KEY0:Malloc & WR & Show");
  LCD_ShowString(30,130,200,16,16,"KEY_UP:SRAMx KEY1:Free");
  LCD_ShowString(60,160,200,16,16," SRAMIN ");
  LCD_ShowString(30,176,200,16,16,"SRAMIN   USED:");
  LCD_ShowString(30,192,200,16,16,"SRAMCCM  USED:");
  LCD_ShowString(30,208,200,16,16,"SRAMEX   USED:");
  while(1)
  {
    key_value = key_scan(0);
    switch(key_value)
    {
      case KEY0_Press:
      p = mymalloc(sramx, 2048); // 按下KEY0申请2KB空间
      if(p!= NULL)
      {
        sprintf((char *)p, "Memory Malloc Test%03d", i); // 写入数据
        LCD_ShowString(30,260,209,16,16,(char*)p);
      }
      break;

      case KEY1_Press:
        myfree(sramx, p); // 按下KEY1释放申请的空间
        p = 0;
        break;

      case WKUP_Press:
        sramx++;
        if(sramx > SRAMBANK) sramx = 0;
        LCD_ShowString(60,160,200,16,16,(char*)SRAM_NAME_BUF[sramx]);
        break;
    }
    if(tp != p)
    {
      tp = p;
      sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)tp);
      LCD_ShowString(30,240,209,16,16,(char*)paddr);
      if(p)
      {
        LCD_ShowString(30,260,280,16,16,(char*)p);
      }
      else
      {
        LCD_Fill(30,260,280,200,WHITE);
      }
    }
    delay_ms(10);
    i++;
    if((i % 20) == 0)
    {
      memused = my_mem_perused(SRAMIN); // 获取内存使用率
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      LCD_ShowString(30+112,176,200,16,16,(char*)paddr);
      memused = my_mem_perused(SRAMCCM); // 获取内存使用率
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      LCD_ShowString(30+112,192,200,16,16,(char*)paddr);
      memused = my_mem_perused(SRAMEX); // 获取内存使用率
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      LCD_ShowString(30+112,208,200,16,16,(char*)paddr);
      LED_TOGGLE(LED0_GPIO_Pin);
    }
  }
}
