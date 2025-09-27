
#include "stm32f10x.h"
#include "board.h"
#include "bsp_fsmc.h"
/*=========================================================================================*/


void FSMC_Init(void)
{
#if 0
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
	                         RCC_APB2Periph_GPIOF, ENABLE);

	/*-- GPIO Configuration ------------------------------------------------------*/
	  /* SRAM Data lines configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
	                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                                GPIO_Pin_15 ;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);

	  /* SRAM Address lines configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
	                                GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	                                GPIO_Pin_4 | GPIO_Pin_5;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  /* NOE and NWE configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  /* NE3 configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* NE4 configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* NBL0, NBL1 configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能FSMC对应相应管脚时钟*/
	RCC_APB2PeriphClockCmd ( 	
													/*控制信号*/
													ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
													ILI9341_RD_CLK	|ILI9341_BK_CLK|ILI9341_RST_CLK|
													/*数据信号*/
													ILI9341_D0_CLK|ILI9341_D1_CLK|	ILI9341_D2_CLK | 
													ILI9341_D3_CLK | ILI9341_D4_CLK|ILI9341_D5_CLK|
													ILI9341_D6_CLK | ILI9341_D7_CLK|ILI9341_D8_CLK|
													ILI9341_D9_CLK | ILI9341_D10_CLK|ILI9341_D11_CLK|
													ILI9341_D12_CLK | ILI9341_D13_CLK|ILI9341_D14_CLK|
													ILI9341_D15_CLK	, ENABLE );
		
	
	/* 配置FSMC相对应的数据线,FSMC-D0~D15 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
	GPIO_Init ( ILI9341_D0_PORT, & GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
	GPIO_Init ( ILI9341_D1_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D2_PIN;
	GPIO_Init ( ILI9341_D2_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D3_PIN;
	GPIO_Init ( ILI9341_D3_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D4_PIN;
	GPIO_Init ( ILI9341_D4_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D5_PIN;
	GPIO_Init ( ILI9341_D5_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D6_PIN;
	GPIO_Init ( ILI9341_D6_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D7_PIN;
	GPIO_Init ( ILI9341_D7_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D8_PIN;
	GPIO_Init ( ILI9341_D8_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D9_PIN;
	GPIO_Init ( ILI9341_D9_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D10_PIN;
	GPIO_Init ( ILI9341_D10_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D11_PIN;
	GPIO_Init ( ILI9341_D11_PORT, & GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = ILI9341_D12_PIN;
	GPIO_Init ( ILI9341_D12_PORT, & GPIO_InitStructure );	
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D13_PIN;
	GPIO_Init ( ILI9341_D13_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D14_PIN;
	GPIO_Init ( ILI9341_D14_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D15_PIN;
	GPIO_Init ( ILI9341_D15_PORT, & GPIO_InitStructure );
	

	
	/* 配置FSMC相对应的控制线
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A16  	:LCD-DC
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN; 
	GPIO_Init (ILI9341_RD_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN; 
	GPIO_Init (ILI9341_WR_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN; 
	GPIO_Init ( ILI9341_CS_PORT, & GPIO_InitStructure );  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN; 
	GPIO_Init ( ILI9341_DC_PORT, & GPIO_InitStructure );
	

  /* 配置LCD复位RST控制管脚*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN; 
	GPIO_Init ( ILI9341_RST_PORT, & GPIO_InitStructure );
	
	
	/* 配置LCD背光控制管脚BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN; 
	GPIO_Init ( ILI9341_BK_PORT, & GPIO_InitStructure );
}

void FSMC_InitSRAM(void)
{
//	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
//	FSMC_NORSRAMTimingInitTypeDef  p;



//	 /*-- FSMC Configuration ------------------------------------------------------*/
//	  p.FSMC_AddressSetupTime = 1;
//	  p.FSMC_AddressHoldTime = 0;
//	  p.FSMC_DataSetupTime = 1;
//	  p.FSMC_BusTurnAroundDuration = 0;
//	  p.FSMC_CLKDivision = 0;
//	  p.FSMC_DataLatency = 0;
//	  p.FSMC_AccessMode = FSMC_AccessMode_A;


//	  FSMC_NORSRAMInitStructure.FSMC_Bank = EXT_SRAM_FSMC_BANK;
//	  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//	  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//	  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;

//	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//	  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//	  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//	  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
//	  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &p;

//	  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

//	  /* Enable FSMC Bank1_SRAM Bank */
//	  FSMC_NORSRAMCmd(EXT_SRAM_FSMC_BANK, ENABLE);
}

void FSMC_InitLCD(void)
{
#if 0
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  wr_t,rd_t;

	GPIO_InitTypeDef GPIO_InitStructure;

	//PD12 -> FSMC_A23 - LCD_RS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	wr_t.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
	wr_t.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
	wr_t.FSMC_DataSetupTime         = 0x01;	 //数据建立时间
	wr_t.FSMC_BusTurnAroundDuration = 0x00;
	wr_t.FSMC_CLKDivision           = 0x00;
	wr_t.FSMC_DataLatency           = 0x00;
	wr_t.FSMC_AccessMode            = FSMC_AccessMode_B;

	rd_t.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
	rd_t.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
	rd_t.FSMC_DataSetupTime         = 0x02;	 //数据建立时间
	rd_t.FSMC_BusTurnAroundDuration = 0x00;
	rd_t.FSMC_CLKDivision           = 0x00;
	rd_t.FSMC_DataLatency           = 0x00;
	rd_t.FSMC_AccessMode            = FSMC_AccessMode_B;

	FSMC_NORSRAMInitStructure.FSMC_Bank                  = EXT_LCD_FSMC_BANK;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	//FSMC_NORSRAMInitStructure.FSMC_MemoryType          = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait 	 = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rd_t;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &wr_t;

	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure );


	/* 使能 FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( EXT_LCD_FSMC_BANK, ENABLE );
#endif
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  wr_t,rd_t; 	
	
	/* 使能FSMC时钟*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

//	//地址建立时间（ADDSET）为1个HCLK 2/72M=28ns
//	wr_t.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
//	//数据保持时间（DATAST）+ 1个HCLK = 5/72M=70ns	
//	wr_t.FSMC_DataSetupTime         = 0x01;	 //数据建立时间
//	//选择控制的模式
//	//模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配
//	wr_t.FSMC_AccessMode            = FSMC_AccessMode_B;	
//	
//	/*以下配置与模式B无关*/
//	//地址保持时间（ADDHLD）模式A未用到
//	wr_t.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
//	//设置总线转换周期，仅用于复用模式的NOR操作
//	wr_t.FSMC_BusTurnAroundDuration = 0x00;
//	//设置时钟分频，仅用于同步类型的存储器
//	wr_t.FSMC_CLKDivision           = 0x00;
//	//数据保持时间，仅用于同步型的NOR	
//	wr_t.FSMC_DataLatency           = 0x00;	
	
	wr_t.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
	wr_t.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
	wr_t.FSMC_DataSetupTime         = 0x01;	 //数据建立时间
	wr_t.FSMC_BusTurnAroundDuration = 0x00;
	wr_t.FSMC_CLKDivision           = 0x00;
	wr_t.FSMC_DataLatency           = 0x00;
	wr_t.FSMC_AccessMode            = FSMC_AccessMode_B;

	rd_t.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
	rd_t.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
	rd_t.FSMC_DataSetupTime         = 0x02;	 //数据建立时间
	rd_t.FSMC_BusTurnAroundDuration = 0x00;
	rd_t.FSMC_CLKDivision           = 0x00;
	rd_t.FSMC_DataLatency           = 0x00;
	rd_t.FSMC_AccessMode            = FSMC_AccessMode_B;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rd_t;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &wr_t;  
	
	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure ); 
	
	/* 使能 FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );  
		
}

/*=========================================================================================*/
/*=========================================================================================*/









