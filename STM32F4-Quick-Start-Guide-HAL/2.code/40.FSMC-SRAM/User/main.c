#include "bsp_init.h"
#include "stdio.h"
#include "SerialInvoker.h"
#include "sram.h"

// 测试数据
// uint32_t test_data[250000] __attribute__((at(SRAM_BASE_ADDR))); // AC5写法
uint32_t test_data[250000] __attribute__((section(".ARM.__at_0x68000000"))); // AC6写法

// 外扩SRAM测试
void sram_test(uint16_t x, uint16_t y)
{
  uint32_t i = 0;
  uint8_t temp = 0;
  uint16_t addr = 0; // 在地址0读到的数据
  LCD_ShowString(x,y,230,y+16,16,"Ex Memory Test:   0KB");
  // 每隔4k字节写入一个数据，总共写入256次，即写入1MB数据
  for(i=0;i<1024*1024;i+=4096)
  {
    sram_write(&temp,i,1);
    temp++;
  }
  // 读取数据进行校验
  for(i=0;i<1024*1024;i+=4096)
  {
    sram_read(&temp,i,1);
    if(i == 0)
    {
      addr = temp; // 记录地址0读到的数据
    }
    else if(temp <= addr)
    {
      break; // 校验失败，退出循环
    }
    LCD_ShowxNum(x+15*8,y,(uint16_t)(temp-addr+1)*4,4,16,0);
  }
}

int main(void)
{
  uint8_t key_value = 0;
  uint8_t i = 0;
  uint32_t data = 0;
  bsp_init();
  sram_init();
	serial_invoker_init(84);
  LCD_ShowString(30,110,200,16,16,"KEY0:Test Sram");
  LCD_ShowString(30,130,200,16,16,"KEY1:TEST Data");
  for(data=0;data<250000;data++)
  {
    test_data[data] = data; // 初始化测试数据
  }
  while(1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      sram_test(30,150); // 测试外扩SRAM
    }
    else if(key_value == KEY1_Press)
    {
      for(data=0;data<250000;data++)
      {
        LCD_ShowxNum(30,170,test_data[data],6,16,0); // 显示测试数据
      }
    }
    else
    {
      delay_ms(10);
    }
    i++;
    if(i == 10)
    {
      i = 0;
      LED_TOGGLE(LED0_GPIO_Pin);
    }
  }
}
