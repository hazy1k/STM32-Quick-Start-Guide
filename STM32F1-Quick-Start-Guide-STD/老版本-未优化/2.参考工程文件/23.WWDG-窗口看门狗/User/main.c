#include "stm32f10x.h"
#include "led.h"
#include "key.h" 
#include "wwdg.h" 

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

int main(void)
{	
	uint8_t wwdg_tr, wwdg_wr;
	LED_Init();
	LED1_ON();
	Delay(0xFFFFFF);
	// 初始化WWDG：配置计数器初始值，配置上窗口值，启动WWDG，使能提前唤醒中断
	WWDG_Mode_Init(0X7F, 0X5F, WWDG_Prescaler_8);
	wwdg_wr = WWDG->CFR & 0X7F;
	while(1)
	{	
		LED1_OFF();
		//-----------------------------------------------------
		// 这部分应该写需要被WWDG监控的程序，这段程序运行的时间
		// 决定了窗口值应该设置成多大。
		//-----------------------------------------------------
		// 计时器值，初始化成最大0X7F，当开启WWDG时候，这个值会不断减小
		// 当计数器的值大于窗口值时喂狗的话，会复位，当计数器减少到0X40
		// 还没有喂狗的话就非常非常危险了，计数器再减一次到了0X3F时就复位
		// 所以要当计数器的值在窗口值和0X40之间的时候喂狗，其中0X40是固定的。
		wwdg_tr = WWDG->CR & 0X7F;
		if(wwdg_tr < wwdg_wr)
		{
			// 喂狗，重新设置计数器的值为最大0X7F
			WWDG_Feed();
		}
	}
}
