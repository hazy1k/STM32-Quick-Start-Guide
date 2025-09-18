#include "stm32f10x.h"
#include "led.h"
#include "AdvanceTim.h" 
#include "GeneralTim.h" 
#include "usart.h"

int main(void)
{	
	USART_Config();
	GTIM_Init();
	ATIMx_Init();
	while(1)
	{      
  	}
}
