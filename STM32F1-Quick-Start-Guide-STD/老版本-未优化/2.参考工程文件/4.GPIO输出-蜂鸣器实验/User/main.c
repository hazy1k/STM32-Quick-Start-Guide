#include "beep.h"
#include "stm32f10x.h"

// 延时函数声明
void Delay(__IO uint32_t nCount);

int main(void)
{
    // 只需初始化一次蜂鸣器
    BEEP_GPIO_Config(); // 蜂鸣器初始化

    while(1)
    {
        BEEP(ON); // 蜂鸣器响起
        Delay(1000); // 延时1s
        BEEP(OFF); // 蜂鸣器停止
        Delay(1000);
    }
}

void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
