#include "crc.h"

__IO uint32_t CRCValue = 0; // 用于存放产生的CRC校验值

void CRC_Config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE); // 使能CRC时钟
}
