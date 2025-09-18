#include "stm32f10x.h"
#include "atim.h"
#include "key.h" 

extern uint16_t ATIM_Buffer[3];

int main(void)
{
  KEY_GPIO_Init();
  ATIMx_Init();
  while(1)
  {
    if(KEY_Scan(KEY1_GPIO, KEY1_GPIO_PIN) == KEY_ON)
    {
      ATIM_Buffer[0] = 800;
      ATIM_Buffer[1] = 800;
      ATIM_Buffer[2] = 100;
    }
    if(KEY_Scan(KEY2_GPIO, KEY2_GPIO_PIN) == KEY_ON)
    {
      ATIM_Buffer[0] = 300;
      ATIM_Buffer[1] = 300;
      ATIM_Buffer[2] = 100;
    }
  }
}
