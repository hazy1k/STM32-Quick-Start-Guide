#include "stm32f10x.h"
#include "spwm.h"

__IO uint32_t rgb_color = 0x008080;

int main(void)
{			
	SPWM_Init();
	while(1)
	{
	
	}		
}
