#include "stm32f10x.h"
#include "breathing.h"

// 定义RGB灯的颜色变量
__IO uint32_t rgb_color = 0x00FFFF;

int main(void)
{			
	breathing_Init();
	while(1)
	{
	}		
}
