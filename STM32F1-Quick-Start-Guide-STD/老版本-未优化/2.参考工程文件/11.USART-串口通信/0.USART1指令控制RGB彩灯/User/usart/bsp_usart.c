#include "bsp_usart.h"

// 配置嵌套向量中断控制器NVIC
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; // 定义NVIC初始化结构体变量
  
  // 1.嵌套向量中断控制器组选择
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  // 2.配置USART为中断源
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;

  // 3.设置抢占优先级和子优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

  // 4.使能中断
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  // 5.初始化配置NVIC
  NVIC_Init(&NVIC_InitStructure);
}

// USART GPIO 配置,工作参数配置
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体变量
	USART_InitTypeDef USART_InitStructure; // 定义USART初始化结构体变量

	// 1.打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 2.打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 3.将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // 使用GPIO_Pin_9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 速度为50MHz

	// 4.初始化GPIO结构体
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); 

    // 5.将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // 使用GPIO_Pin_10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入模式

	// 6.初始化GPIO结构体
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 7.配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 波特率为115200

	// 8.配置数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位

	// 9.配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位

	// 10.配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位

	// 11.配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制

	// 12.配置工作模式
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发一起工作

	// 13.完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	// 使能串口接收中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

// 发送一个字节函数
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch) // 函数参数：串口号、待发送的字节
{
	// 发送一个字节数据到USART
	USART_SendData(pUSARTx, ch);
		
	// 等待发送完成
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

// 发送一个8位的数组函数
void Usart_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num) // 函数参数：串口号、待发送的数组、数组长度
{
  uint8_t i;
	
	for(i = 0; i < num; i++)
  	{
	    // 发送一个字节数据到USART并且通过循环写入到数组中
	    Usart_SendByte(pUSARTx, array[i]);	
  	}

	// 等待发送完成
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

// 发送一个字符串函数
void Usart_SendString( USART_TypeDef *pUSARTx, char *str) // 函数参数：串口号、待发送的字符串
{
  unsigned int k=0;
  do 
  {
    Usart_SendByte( pUSARTx, *(str + k) ); // 发送一个字节数据到USART
    k++;
  } while(*(str + k)!='\0'); // 直到字符串结束
  
  // 等待发送完成
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET)
  {

  }
}

// 发送一个16位数函数
void Usart_SendHalfWord( USART_TypeDef *pUSARTx, uint16_t ch) // 函数参数：串口号、待发送的16位数
{
	uint8_t temp_h, temp_l;
	
	// 1.取高八位
	temp_h = (ch&0XFF00) >> 8;
	// 2.取出低八位
	temp_l = ch&0XFF;
	
	// 3.发送高八位
	USART_SendData(pUSARTx, temp_h);	
	// 4.等待发送完成
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	// 5.发送低八位
	USART_SendData(pUSARTx,temp_l);	
	// 6.等待发送完成
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f) // 函数参数：待发送的字符、文件指针
{
		// 发送一个字节数据到串口
		USART_SendData(DEBUG_USARTx, (uint8_t)ch);
		
		// 等待发送完毕
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		// 返回发送的字符
		return (ch);
}

// 重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f) // 函数参数：文件指针
{
		// 等待串口输入数据
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);
		
		// 返回接收到的数据
		return (int)USART_ReceiveData(DEBUG_USARTx);
}
