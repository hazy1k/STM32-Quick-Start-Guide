#include "stm32f10x.h"
#include "usart.h"
#include "crc.h"

extern __IO uint32_t CRCValue;

int main(void)
{
	uint8_t i = 0;
	USART_Config();
	CRC_Config();
	/* Compute the CRC of "DataBuffer" */
	for(i=0; i<BUFFER_SIZE; i++ ) 
	{
		// 对该寄存器进行写操作时，作为输入寄存器，可以输入要进行CRC计算的新数据。
		// 对该寄存器进行读操作时，返回上一次CRC计算的结果。
		// 每一次写入数据寄存器，其计算结果是前一次CRC计算结果和新计算结果的组合(对整个32位字
		// 进行CRC计算，而不是逐字节地计算)。
		CRCValue = CRC_CalcBlockCRC((uint32_t *)DataBuffer, BUFFER_SIZE);
		printf("\r\n32-bit CRC 校验码为:0X%X\r\n", CRCValue);
	}
	printf("\r\nCRC(循环冗余校验)测试成功\r\n");
	while(1);
}
