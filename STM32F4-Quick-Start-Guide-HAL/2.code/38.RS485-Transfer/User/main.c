#include "bsp_init.h"
#include "stdio.h"
#include "rs485.h"

int main(void)
{
  uint8_t key_value = 0;
  uint8_t i, t, cnt = 0;
  uint8_t rs485buf[5];
  bsp_init();
  rs485_init(115200);
  LCD_ShowString(30,50,200,16,16,"STM32 RS485 Test");
  LCD_ShowString(30,110,200,16,16,"KEY0:Send");
  LCD_ShowString(30,130,200,16,16,"Count:");
  LCD_ShowString(30,150,200,16,16,"Sned Data:");
  LCD_ShowString(30,190,200,16,16,"Receive Data:");
  while(1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_Press) // Send data
    {
      for(i=0;i<5;i++)
      {
        rs485buf[i] = cnt+i; // 填充发送缓冲区
        LCD_ShowxNum(30+i*32,170,rs485buf[i],3,16,0x80); // 显示发送数据
      }
      rs485_send_data(rs485buf,5); // 发送数据
    }
    rs485_receive_data(rs485buf,&key_value); // 接收数据
    if(key_value) // 接收到数据
    {
      if(key_value > 5)
      {
        key_value = 5;
      }
      for(i=0;i<key_value;i++)
      {
        LCD_ShowxNum(30+i*32,210,rs485buf[i],3,16,0x80); // 显示接收数据
      }
    }
    t++;
    delay_ms(10);
    if(t == 20)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      t = 0;
      cnt++;
      LCD_ShowxNum(78,130,cnt,3,16,0x80); // 显示计数
    }
  }
}
