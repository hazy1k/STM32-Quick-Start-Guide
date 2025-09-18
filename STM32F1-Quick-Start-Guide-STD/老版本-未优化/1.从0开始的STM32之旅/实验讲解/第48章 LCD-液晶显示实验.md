# 第四十八章 LCD-液晶显示实验

## 1. 硬件设计

本小节讲解如何使用FSMC外设控制实验板配套的3.2寸ILI9341液晶屏，该液晶屏的分辨率为320x240，支持RGB565格式。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD022.jpg)

<img title="" src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD023.jpeg" alt="" width="728">

屏幕的PCB底板引出的信号线会通过PCB底板上的FPC接口与液晶面板连接，这些信包括液晶控制相关的CS、RS等信号及DB0-DB15数据线， 其中RS引脚以高电平表示传输数据，低电平表示传输命令；另外还有引出LCD_BK引脚用于控制屏幕的背光供电，可以通过该引脚控制背光的强度， 该引脚为低电平时打开背光。图中的X+/X-/Y+/Y-引脚是液晶面板上触摸屏引出的信号线，它们会被连接到PCB底板的电阻触摸屏控制器，用于检测触摸信号， 其原理图见图

<img title="" src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD024.jpeg" alt="" width="733">

触摸检测的主体是型号为XPT2046的芯片，它接收触摸屏的X+/X-/Y+/Y-信号进行处理，把触摸信息使用SPI接口输出到STM32等控制器， 在触摸屏章节将会详细讲解其检测原理。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD025.jpg)

上图表示的是PCB底板引出的排针线序， 屏幕整体通过这些引出的排针与开发板或其它控制器连接。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD026.jpg" title="" alt="" width="757">

上图是指南者开发板上的液晶排母接口原理图， 它说明了配套的3.2寸屏幕接入到开发板上时的信号连接关系。 其中请着重关注图中液晶屏LCD_CS及LCD_RS(即DC引脚)与FSMC存储区选择引脚FSMC_NE及地址信号FSMC_A的编号， 它们会决定STM32要使用什么内存地址来控制与液晶屏的通讯。

## 2. 软件设计

### 2.1 编程大纲

1. 相关参数宏定义

2. 初始化GPIO

3. 配置FSMC

4. 初始化ST7789V的寄存器

5. 基础LCD操作函数

6. 主函数测试

### 2.2 代码分析

#### 2.2.1 硬件LCD硬件相关宏定义

```c
// FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define FSMC_Addr_ST7789V_CMD  ((uint32_t)0x60000000)
// FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define FSMC_Addr_ST7789V_DATA ((uint32_t)0x60020000)
// 由片选引脚决定的NOR/SRAM块
#define FSMC_Bank1_NORSRAMx FSMC_Bank1_NORSRAM1
/* 控制引脚 */
// 片选，选择NOR/SRAM块
#define ST7789V_CS_CLK  RCC_APB2Periph_GPIOD // CS->PD7
#define ST7789V_CS_PORT GPIOD
#define ST7789V_CS_PIN  GPIO_Pin_7
// DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
#define ST7789V_DC_CLK  RCC_APB2Periph_GPIOD // DC->PD11
#define ST7789V_DC_PORT GPIOD
#define ST7789V_DC_PIN  GPIO_Pin_11
// 写使能
#define ST7789V_WR_CLK  RCC_APB2Periph_GPIOD // WR->PD5  
#define ST7789V_WR_PORT GPIOD
#define ST7789V_WR_PIN  GPIO_Pin_5
// 读使能
#define ST7789V_RD_CLK  RCC_APB2Periph_GPIOD // RD->PD4     
#define ST7789V_RD_PORT GPIOD
#define ST7789V_RD_PIN  GPIO_Pin_4
// 复位引脚
#define ST7789V_RST_CLK  RCC_APB2Periph_GPIOE // RST->PE1  
#define ST7789V_RST_PORT GPIOE
#define ST7789V_RST_PIN  GPIO_Pin_1
// 背光引脚
#define ST7789V_BK_CLK  RCC_APB2Periph_GPIOD // BK->PD12   
#define ST7789V_BK_PORT GPIOD
#define ST7789V_BK_PIN  GPIO_Pin_12
/* 数据线 */
#define ST7789V_D0_CLK  RCC_APB2Periph_GPIOD // D0->PD14   
#define ST7789V_D0_PORT GPIOD
#define ST7789V_D0_PIN  GPIO_Pin_14
#define ST7789V_D1_CLK  RCC_APB2Periph_GPIOD // D1->PD15   
#define ST7789V_D1_PORT GPIOD
#define ST7789V_D1_PIN  GPIO_Pin_15
#define ST7789V_D2_CLK  RCC_APB2Periph_GPIOD // D2->PD0   
#define ST7789V_D2_PORT GPIOD
#define ST7789V_D2_PIN  GPIO_Pin_0
#define ST7789V_D3_CLK  RCC_APB2Periph_GPIOD // D3->PD1   
#define ST7789V_D3_PORT GPIOD
#define ST7789V_D3_PIN  GPIO_Pin_1
#define ST7789V_D4_CLK  RCC_APB2Periph_GPIOE // D4->PE7   
#define ST7789V_D4_PORT GPIOE
#define ST7789V_D4_PIN  GPIO_Pin_7
#define ST7789V_D5_CLK  RCC_APB2Periph_GPIOE // D5->PE8   
#define ST7789V_D5_PORT GPIOE
#define ST7789V_D5_PIN  GPIO_Pin_8
#define ST7789V_D6_CLK  RCC_APB2Periph_GPIOE // D6->PE9   
#define ST7789V_D6_PORT GPIOE
#define ST7789V_D6_PIN  GPIO_Pin_9
#define ST7789V_D7_CLK  RCC_APB2Periph_GPIOE // D7->PE10  
#define ST7789V_D7_PORT GPIOE
#define ST7789V_D7_PIN  GPIO_Pin_10
#define ST7789V_D8_CLK  RCC_APB2Periph_GPIOE // D8->PE11   
#define ST7789V_D8_PORT GPIOE
#define ST7789V_D8_PIN  GPIO_Pin_11
#define ST7789V_D9_CLK  RCC_APB2Periph_GPIOE // D9->PE12   
#define ST7789V_D9_PORT GPIOE
#define ST7789V_D9_PIN  GPIO_Pin_12
#define ST7789V_D10_CLK  RCC_APB2Periph_GPIOE // D10->PE13  
#define ST7789V_D10_PORT GPIOE
#define ST7789V_D10_PIN  GPIO_Pin_13
#define ST7789V_D11_CLK  RCC_APB2Periph_GPIOE // D11->PE14   
#define ST7789V_D11_PORT GPIOE
#define ST7789V_D11_PIN  GPIO_Pin_14
#define ST7789V_D12_CLK  RCC_APB2Periph_GPIOE // D12->PE15   
#define ST7789V_D12_PORT GPIOE
#define ST7789V_D12_PIN  GPIO_Pin_15
#define ST7789V_D13_CLK  RCC_APB2Periph_GPIOD // D13->PD8   
#define ST7789V_D13_PORT GPIOD
#define ST7789V_D13_PIN  GPIO_Pin_8
#define ST7789V_D14_CLK  RCC_APB2Periph_GPIOD // D14->D9   
#define ST7789V_D14_PORT GPIOD
#define ST7789V_D14_PIN  GPIO_Pin_9
#define ST7789V_D15_CLK  RCC_APB2Periph_GPIOD // D15->PD10   
#define ST7789V_D15_PORT GPIOD
#define ST7789V_D15_PIN  GPIO_Pin_10
```

#### 2.2.2 初始化FSMC的GPIO

```c
// 初始化ST7789V的GPIO
static void ST7789V_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( // 控制信号
	ST7789V_CS_CLK|ST7789V_DC_CLK|ST7789V_WR_CLK|ST7789V_RD_CLK	|ST7789V_BK_CLK|ST7789V_RST_CLK|
	// 数据信号
	ST7789V_D0_CLK|ST7789V_D1_CLK|ST7789V_D2_CLK|ST7789V_D3_CLK|ST7789V_D4_CLK|ST7789V_D5_CLK|
	ST7789V_D6_CLK|ST7789V_D7_CLK|ST7789V_D8_CLK|ST7789V_D9_CLK|ST7789V_D10_CLK|ST7789V_D11_CLK|
	ST7789V_D12_CLK|ST7789V_D13_CLK|ST7789V_D14_CLK|ST7789V_D15_CLK, ENABLE);
	// 配置FSMC相对应的数据线	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStructure.GPIO_Pin = ST7789V_D0_PIN;
	GPIO_Init(ST7789V_D0_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D1_PIN;
	GPIO_Init(ST7789V_D1_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D2_PIN;
	GPIO_Init(ST7789V_D2_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D3_PIN;
	GPIO_Init(ST7789V_D3_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D4_PIN;
	GPIO_Init(ST7789V_D4_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D5_PIN;
	GPIO_Init(ST7789V_D5_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D6_PIN;
	GPIO_Init(ST7789V_D6_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D7_PIN;
	GPIO_Init(ST7789V_D7_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D8_PIN;
	GPIO_Init(ST7789V_D8_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D9_PIN;
	GPIO_Init(ST7789V_D9_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D10_PIN;
	GPIO_Init(ST7789V_D10_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D11_PIN;
	GPIO_Init(ST7789V_D11_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D12_PIN;
	GPIO_Init(ST7789V_D12_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = ST7789V_D13_PIN;
	GPIO_Init(ST7789V_D13_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D14_PIN;
	GPIO_Init(ST7789V_D14_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D15_PIN;
	GPIO_Init(ST7789V_D15_PORT, &GPIO_InitStructure);
	/* 配置FSMC相对应的控制线
	FSMC_NOE   :LCD-RD
	FSMC_NWE   :LCD-WR
	FSMC_NE1   :LCD-CS
	FSMC_A16   :LCD-DC */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = ST7789V_RD_PIN; 
	GPIO_Init(ST7789V_RD_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_WR_PIN; 
	GPIO_Init(ST7789V_WR_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_CS_PIN; 
	GPIO_Init(ST7789V_CS_PORT, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = ST7789V_DC_PIN; 
	GPIO_Init(ST7789V_DC_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ST7789V_RST_PIN; 
	GPIO_Init(ST7789V_RST_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Pin = ST7789V_BK_PIN; 
	GPIO_Init(ST7789V_BK_PORT, &GPIO_InitStructure);
}
```

与控制SRAM中的GPIO初始化类似，对于FSMC引脚，全部直接初始化为复用推挽输出模式即可，而背光BK引脚及液晶复信RST信号则被初始化成普通的推挽输出模式， 这两个液晶控制信号直接输出普通的电平控制即可。

#### 2.2.3 配置FSMC的模式

```c
// LCD FSMC 模式配置
static void ST7789V_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure = {0};
	FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	readWriteTiming.FSMC_AddressSetupTime = 0x01;// 地址建立时间
	readWriteTiming.FSMC_DataSetupTime = 0x04;	 // 数据保持时间
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B; // 模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配	
	/* 以下配置与模式B无关 */
	readWriteTiming.FSMC_AddressHoldTime = 0x00; 
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;	
	/* FSMC配置 */
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx; // 使用NOR FLASH的BANK1
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable; // 禁止地址复用
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR; // 使用NOR FLASH
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b; // 数据宽度为16位
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable; // 禁止突发访问模式
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low; // 时钟低电平有效
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable; // 禁止WRAP模式
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState; // 等待状态前有效
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable; // 使能写操作
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable; // 禁止等待信号
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable; // 禁止扩展模式
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; // 禁止写突发模式
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; // 设置时序参数
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming; // 设置时序参数
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  
}
```

这个函数的主体是把FSMC配置成异步NOR FLASH使用的模式B，使用该方式模拟8080时序控制液晶屏，执行流程如下：

(1) 初始化FSMC时钟

> 函数开头使用库函数RCC_AHBPeriphClockCmd使能FSMC外设的时钟。

(2) 时序结构体赋值

> 接下来对时序结构体FSMC_NORSRAMTimingInitTypeDef赋值。在这个时序结构体配置中， 由于我们要使用异步NOR FLASH的方式模拟8080时序， 所以选择FSMC为模式B，在该模式下配置FSMC的控制时序结构体中， 实际上只有地址建立时间FSMC_AddressSetupTime（即ADDSET的值）以及数据建立时间FSMC_DataSetupTime（即DATAST的值）成员的配置值是有效的， 其它异步NOR FLASH没使用到的成员值全配置为0即可。而且，这些成员值使用的单位为：1个HCLK的时钟周期， 而HCLK的时钟频率为72MHz，对应每个时钟周期为1/72微秒。

ILI9341时序参数说明及要求可大致得知ILI9341的写周期为最小twc = 66ns， 而读周期最小为trdl+trod=45+20=65ns。 （对于读周期表中有参数要一个要求为trcfm和trc分别为450ns及160ns， 但经过测试并不需要遵照它们的指标要求）

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD027.jpg" title="" alt="" width="788">

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD028.jpeg" title="" alt="" width="786">

在FSMC代码中使用结构体中的FSMC_AddressSetupTime（即ADDSET的值）及FSMC_DataSetupTime（即DATAST的值）成员控制FSMC的读写周期

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD029.jpeg" title="" alt="" width="788">

结合ILI9341的时序要求和FSMC的配置图，代码中按照读写时序周期均要求至少66ns来计算，配置结果为ADDSET = 1及DATST = 4， 把时间单位1/72微秒(即1000/72纳秒)代入，因此读写周期的时间被配置为：

读周期：trc =((ADDSET+1)+(DATST+1)+2) *1000/72 = ((1+1)+(4+1)+2)*1000/72 = 125ns

写周期：twc =((ADDSET+1)+(DATST+1)) *1000/72 = ((1+1)+(4+1))*1000/72 = 97ns

所以把这两个参数值写入到FSMC后，它控制的读写周期都比ILI9341的最低要求值大。（经测试，这两个参数值也可以适当减小，您可以亲自试一下）

把计算得的参数赋值到时序结构体中的FSMC_AddressSetupTime（即ADDSET的值）及FSMC_DataSetupTime（即DATAST的值）中， 然后再把时序结构体作为指针赋值到下面的FSMC初始化结构体中，作为读写的时序参数，最后再调用FSMC_NORSRAMInit函数即可把参数写入到相应的寄存器中。

(3) 配置FSMC初始化结构体

> 函数接下来对FSMC SRAM的初始化结构体赋值。主要包括存储映射区域、存储器类型以及数据线宽度等，这些是根据外部电路设置的。

- 设置存储区域FSMC_Bank

> FSMC_Bank成员设置FSMC的NOR FLASH存储区域映射选择为宏 FSMC_Bank1_NORSRAMx (即FSMC_Bank1_NORSRAM4)， 这是由于我们的SRAM硬件连接到FSMC_NE4和NOR/PSRAM相关引脚， 所以对应到存储区域Bank1SRAM4，对应的基地址为0X6C00 0000；

- 存储器类型FSMC_MemoryType

> 由于使用异步NOR FLASH模式模拟8080时序，所以FSMC_MemoryType成员要选择相应的FSMC_MemoryType_NOR；

- 数据线宽度FSMC_MemoryDataWidth

> 根据硬件的数据线连接，数据线宽度被配置为16位宽FSMC_MemoryDataWidth_16b；

- 写使能FSMC_WriteOperation

> FSMC_WriteOperation用于设置写使能，只有使能了才能正常使用FSMC向外部存储器写入数据；

- 扩展模式以及读写时序

> 在FSMC_ExtendedMode成员中可以配置是否使用扩展模式，当设置扩展模式时，读时序使用FSMC_ReadWriteTimingStruct中的配置，写时序使用FSMC_WriteTimingStruct中的配置，两种配置互相独立，可以赋值为不同的读写时序结构体。在本实例中不使用扩展模式，即 读写时序使用相同的配置，都是赋值为前面的readWriteTiming结构体；

- 其它

> 配置FSMC还涉及到其它的结构体成员，但这些结构体成员与异步NOR FLASH控制不相关，都被设置为Disable了；

赋值完成后调用库函数FSMC_NORSRAMInit把初始化结构体配置的各种参数写入到FSMC_BCR控制寄存器及FSMC_BTR时序寄存器中。最后调用FSMC_NORSRAMCmd函数使能要控制的存储区域FSMC_Bank1_NORSRAM4。

#### 2.2.3 ST7789V基础命令

```c
// 向ST7789V写入命令
static void ST7789V_Write_Cmd(uint16_t usCmd)
{
	*(__IO uint16_t*)(FSMC_Addr_ST7789V_CMD) = usCmd;	
}
// 向ST7789V写入数据
static void ST7789V_Write_Data(uint16_t usData)
{
	*(__IO uint16_t*)(FSMC_Addr_ST7789V_DATA) = usData;
}
// 从ST7789V读取数据	
static uint16_t ST7789V_Read_Data(void)
{
	return(*( __IO uint16_t *)(FSMC_Addr_ST7789V_DATA));
	
}( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = usData;
}
```

需要写操作时，只要把要发送的命令代码或数据作为参数输入到函数然后调用即可，对于液晶屏的读操作，把向指针赋值的过程改为读取指针内容即可。

#### 2.2.4 初始化ST7789V的寄存器

```c
// 初始化ST7789V的寄存器
static void ST7789V_REG_Config ( void )
{
    lcdid = ST7789V_ReadID(); // 读取LCD ID
	if(lcdid == LCDID_ST7789V)
	{
		ST7789V_Write_Cmd(0xCF);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x81);
		ST7789V_Write_Data(0x30);
		/* Power on sequence control (EDh) */
		ST7789V_Write_Cmd(0xED);
		ST7789V_Write_Data(0x64);
		ST7789V_Write_Data(0x03);
		ST7789V_Write_Data(0x12);
		ST7789V_Write_Data(0x81);
		/* Driver timing control A (E8h) */
		ST7789V_Write_Cmd(0xE8);
		ST7789V_Write_Data(0x85);
		ST7789V_Write_Data(0x10);
		ST7789V_Write_Data(0x78);
		/* Power control A (CBh) */
		ST7789V_Write_Cmd(0xCB);
		ST7789V_Write_Data(0x39);
		ST7789V_Write_Data(0x2C);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x34);
		ST7789V_Write_Data(0x06); // 改为0x06可防止液晶显示白屏时有条纹的情况
		/* Pump ratio control (F7h) */
		ST7789V_Write_Cmd(0xF7);
		ST7789V_Write_Data(0x20);
		/* Driver timing control B */
		ST7789V_Write_Cmd(0xEA);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
		ST7789V_Write_Cmd(0xB1);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x1B);
		/* Display Function Control (B6h) */
		ST7789V_Write_Cmd(0xB6);
		ST7789V_Write_Data(0x0A);
		ST7789V_Write_Data(0xA2);
		/* Power Control 1 (C0h) */
		ST7789V_Write_Cmd(0xC0);
		ST7789V_Write_Data(0x35);
		/* Power Control 2 (C1h) */
		ST7789V_Write_Cmd(0xC1);
		ST7789V_Write_Data(0x11);
		/* VCOM Control 1 (C5h) */
		ST7789V_Write_Cmd(0xC5);
		ST7789V_Write_Data(0x45);
		ST7789V_Write_Data(0x45);
		/* VCOM Control 2 (C7h) */
		ST7789V_Write_Cmd(0xC7);
		ST7789V_Write_Data(0xA2);
		/* Enable 3G (F2h) */
		ST7789V_Write_Cmd(0xF2);
		ST7789V_Write_Data(0x00);
		/* Gamma Set (26h) */
		ST7789V_Write_Cmd(0x26);
		ST7789V_Write_Data(0x01);
		/* Positive Gamma Correction */
		ST7789V_Write_Cmd(0xE0); // Set Gamma
		ST7789V_Write_Data(0x0F);
		ST7789V_Write_Data(0x26);
		ST7789V_Write_Data(0x24);
		ST7789V_Write_Data(0x0B);
		ST7789V_Write_Data(0x0E);
		ST7789V_Write_Data(0x09);
		ST7789V_Write_Data(0x54);
		ST7789V_Write_Data(0xA8);
		ST7789V_Write_Data(0x46);
		ST7789V_Write_Data(0x0C);
		ST7789V_Write_Data(0x17);
		ST7789V_Write_Data(0x09);
		ST7789V_Write_Data(0x0F);
		ST7789V_Write_Data(0x07);
		ST7789V_Write_Data(0x00);
		/* Negative Gamma Correction (E1h) */
		ST7789V_Write_Cmd(0xE1); // Set Gamma
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x19);
		ST7789V_Write_Data(0x1B);
		ST7789V_Write_Data(0x04);
		ST7789V_Write_Data(0x10);
		ST7789V_Write_Data(0x07);
		ST7789V_Write_Data(0x2A);
		ST7789V_Write_Data(0x47);
		ST7789V_Write_Data(0x39);
		ST7789V_Write_Data(0x03);
		ST7789V_Write_Data(0x06);
		ST7789V_Write_Data(0x06);
		ST7789V_Write_Data(0x30);
		ST7789V_Write_Data(0x38);
		ST7789V_Write_Data(0x0F);
		/* Memory access control set */
		ST7789V_Write_Cmd(0x36); 
		ST7789V_Write_Data(0xC8);    /* 竖屏，左上角到右下角扫描方式 */
		/* Column address control set */
		ST7789V_Write_Cmd(CMD_SetCoordinateX); 
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0xEF);
		/* Page address control set */
		ST7789V_Write_Cmd(CMD_SetCoordinateY); 
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x01);
		ST7789V_Write_Data(0x3F);
		/* Pixel Format Set (3Ah) */
		ST7789V_Write_Cmd(0x3a); 
		ST7789V_Write_Data(0x55);
		/* Sleep Out (11h) */
		ST7789V_Write_Cmd(0x11);
		ST7789V_Delay(0xAFFf<<2);
		/* Display ON (29h) */
		ST7789V_Write_Cmd(0x29);
	}
}
```

这个初始化过程中发送的代码及参数主要是配置了液晶屏的上电过程、显示屏的伽玛参数、分辨率、像素格式等内容， 这些配置主要由液晶屏生产厂家提供，本教程后面只针对常用命令进行讲解，此处不作详细说明，关于命令及参数可以查询《ILI9341数据手册》获知， 在该文档中搜索命令代码即可方便定位到相应的说明。例如，要查找代码中的0xCF命令说明，在文档中搜索“CFh”即可。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD032.png)

#### 2.2.5 LCD基础操作函数

##### 2.2.5.1 设置液晶显示窗口

根据液晶屏的要求，在发送显示数据前，需要先设置显示窗口确定后面发送的像素数据的显示区域

```c
// ST7789V显示器上开辟一个窗口
void ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{	
	ST7789V_Write_Cmd(CMD_SetCoordinateX); 				
	ST7789V_Write_Data(usX>>8);	 
	ST7789V_Write_Data(usX&0xff);	 
	ST7789V_Write_Data((usX+usWidth-1)>>8);
	ST7789V_Write_Data((usX+usWidth-1)&0xff);
	ST7789V_Write_Cmd(CMD_SetCoordinateY); 			  
	ST7789V_Write_Data(usY>>8);
	ST7789V_Write_Data(usY&0xff);
	ST7789V_Write_Data((usY+usHeight-1)>>8);
	ST7789V_Write_Data((usY+usHeight-1)&0xff);
}
```

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD033.png" title="" alt="" width="903">

<img title="" src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD034.png" alt="" width="907">

代码中定义的ILI9341_OpenWindow函数实现了图的0x2A和0x2B命令， 它们分别用于设置显示窗口的起始及结束的X坐标和Y坐标，每个命令后包含4个8位的参数，这些参数组合后成起始坐标和结束坐标各1个用16位表示的值。

ILI9341_OpenWindow把它的四个函数输入参数X、Y起始坐标，宽度、高度转化成命令参数的格式，写入到液晶屏中，从而设置出一个显示窗口。

##### 2.2.5.2 发送像素数据

调用上面的ILI9341_OpenWindow函数设置显示窗口后，再向液晶屏发送像素数据时，这些数据就会直接显示在它设定的窗口位置中。

```c
// 填充颜色
static void ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
	uint32_t i = 0;
	ST7789V_Write_Cmd(CMD_SetPixel);	
	for(i = 0; i < ulAmout_Point; i++)
	{
		ST7789V_Write_Data(usColor);
	}
}
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD035.png)

发送像素数据的命令非常简单，首先发送命令代码0x2C，然后后面紧跟着要传输的像素数据即可。按照本液晶屏的配置， 像素点的格式为RGB565，所以像素数据就是要显示的RGB565格式的颜色值。

本ILI9341_FillColor函数包含两个输入参数，分别用于设置要发送的像素数据个数ulAmout_Point及像素点的颜色值usColor， 在代码实现中它调用ILI9341_Write_Cmd发送一次命令代码，接着使用for循环调用ILI9341_Write_Data写入ulAmout_Po int个同样的颜色值。

这些颜色值会按顺序填充到前面使用ILI9341_OpenWindow函数设置的显示窗口中，例如，若设置了一个usX=10，usY=30，usWidth=50， usHeight=20的窗口，然后再连续填充50*20个颜色值为0XFFFF的像素数据，即可在(10,30)的起始坐标处显示一个宽50像素高20像素的白色矩形。

##### 2.2.5.3 绘制矩形

类似地，使用ILI9341_OpenWindow和ILI9341_FillColor制作的绘制矩形操作见

```c
// 在LCD上画一个矩形
void ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
	if(ucFilled)
	{
		ST7789V_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
		ST7789V_FillColor(usWidth * usHeight ,CurrentTextColor);	
	}
	else
	{
		ST7789V_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
		ST7789V_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
		ST7789V_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
		ST7789V_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);		
	}
}
```

ILI9341_DrawRectangle函数分成两部分，它根据输入参数ucFilled是否为真值决定绘制的是实心矩形还是只有边框的矩形。 绘制实心矩形时，直接使用ILI9341_OpenWindow函数根据输入参数设置显示矩形窗口，然后根据实心矩形的像素点个数调用ILI9341_FillColor即可完成绘制； 而绘制空心矩形时，实质上是绘制四条边框线，它调用ILI9341_DrawLine函数绘制，ILI9341_DrawLine函数的输入参数是用于表示直接的两个坐标点(x1,y1)与(x2,y2)， 该函数内部根据数据关系，使用这两个点确定一条直线，最后调用ILI9341_SetPointPixel函数一点一点地绘制成完整的直线。

##### 2.2.5.4 设置液晶的扫描方向

控制液晶屏时，还有一个非常重要的参数，就是设置液晶屏的扫描方向

```c
// 设置ST7789V的GRAM的扫描方向 
void ST7789V_GramScan(uint8_t ucOption)
{	
	// 参数检查，只可输入0-7
	if(ucOption > 7)
	{
		return;
	}
	LCD_SCAN_MODE = ucOption; // 根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
	if(ucOption%2 == 0)	
	{
		// 0 2 4 6模式下X方向像素宽度为240，Y方向为320
		LCD_X_LENGTH = ST7789V_LESS_PIXEL;
		LCD_Y_LENGTH = ST7789V_MORE_PIXEL;
	}
	else				
	{
		// 1 3 5 7模式下X方向像素宽度为320，Y方向为240
		LCD_X_LENGTH = ST7789V_MORE_PIXEL;
		LCD_Y_LENGTH = ST7789V_LESS_PIXEL; 
	}
	ST7789V_Write_Cmd(0x36);	// 0x36命令参数的高3位可用于设置GRAM扫描方向	
	if(lcdid == LCDID_ST7789V)
  	{
    	ST7789V_Write_Data(0x00 |(ucOption<<5)); // 根据ucOption的值设置LCD参数，共0-7种模式
  	}
	ST7789V_Write_Cmd(CMD_SetCoordinateX); 
	ST7789V_Write_Data(0x00);		
	ST7789V_Write_Data(0x00);	
	ST7789V_Write_Data(((LCD_X_LENGTH-1)>>8)&0xFF); 
	ST7789V_Write_Data((LCD_X_LENGTH-1)&0xFF);				
	ST7789V_Write_Cmd(CMD_SetCoordinateY); 
	ST7789V_Write_Data(0x00);		
	ST7789V_Write_Data(0x00);		
	ST7789V_Write_Data(((LCD_Y_LENGTH-1)>>8)&0xFF); 
	ST7789V_Write_Data((LCD_Y_LENGTH-1)&0xFF);			
	ST7789V_Write_Cmd(CMD_SetPixel);	
}
```

当设置了液晶显示窗口，再连续向液晶屏写入像素点时，它会一个点一个点地往液晶屏的X方向填充， 填充完一行X方向的像素点后，向Y方向下移一行，X坐标回到起始位置，再往X方向一个点一个点地填充，如此循环直至填充完整个显示窗口。

而屏幕的坐标原点和XY方向都可以根据实际需要使用0X36命令来配置的， 该命令的说明见

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD036.jpg)

0X36命令参数中的MY、MX、MV这三个数据位用于配置扫描方向，因此一共有2^3 = 8种模式。 ILI9341_GramScan函数就是根据输入的模式设置这三个数据位，并且根据相应的模式更改XY方向的分辨率LCD_X_LENGTH和LCD_Y_LENGTH， 使得其它函数可以利用这两个全局变量获屏幕实际的XY方向分辨率信息；同时， 函数内还设置了全局变量LCD_SCAN_MODE的值用于记录当前的屏幕扫描模式，这在后面计算触摸屏坐标的时候会使用到。 设置完扫描方向后，代码中还调用设置液晶显示窗口的命令CMD_SetCoordinateX/Y（0X2A/0X2B命令）默认打开一个与屏幕大小一致的显示窗口，方便后续的显示操作。

调用ILI9341_GramScan函数设置0-7模式时，各个模式的坐标原点及XY方向如图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD037.jpg)

其中模式6最符合我们的阅读习惯，扫描方向与文字方向一致，都是从左到右，从上到下，所以本开发板中的大部分液晶程序都是默认使用模式6。

其实模式0、3、5、6的液晶扫描方向都与文字方向一致，比较适合显示文字，只要适当旋转屏幕即可，使得用屏幕四个边沿作为正面看去都有适合的文字显示模式。 而其它模式由于扫描方向与文字方向不一致，要想实现同样的效果非常麻烦，也没有实现的必要。

##### 2.2.5.5 液晶屏全局初始化

利用前面介绍的各种函数，我们把它封装成ILI9341_Init函数

```c
// ST7789V初始化函数
void ST7789V_Init ( void )
{
	ST7789V_GPIO_Config (); 
	ST7789V_FSMC_Config (); 
	ST7789V_BackLed_Control(ENABLE);
	ST7789V_Rst(); 
	ST7789V_REG_Config();
	// 设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向  
	ST7789V_GramScan(LCD_SCAN_MODE);
}
```

本函数初始化GPIO、FSMC外设，然后开启液晶屏的背光，复位液晶屏，并且写入基本的液晶屏配置， 最后调用ILI9341_GramScan函数设置默认的液晶扫描方向。在需要使用液晶屏的时候，直接调用本函数即可完成初始化。

##### 2.2.5.6 背光控制函数

```c
// ST7789V背光控制函数
void ST7789V_BackLed_Control(FunctionalState enumState )
{
	if(enumState) // 点亮背光灯
		GPIO_ResetBits(ST7789V_BK_PORT, ST7789V_BK_PIN); // 低电平点亮
	else
		GPIO_SetBits(ST7789V_BK_PORT, ST7789V_BK_PIN);
}
```

##### 2.2.5.7 读取设备ID函数

```c
// 读取LCD驱动芯片ID函数
uint16_t ST7789V_ReadID(void)
{
	uint16_t id = 0;	
	ST7789V_Write_Cmd(0xD3); // 写入命令
	ST7789V_Read_Data();
	ST7789V_Read_Data();
	id = ST7789V_Read_Data();
	id <<= 8;
	id |= ST7789V_Read_Data();
    if(id == LCDID_ST7789V)
    {
    	return id; // 成功读取到ST7789V ID
  	}
	return 0; // 读取失败
}
```

##### 2.2.5.8 软复位

```c
// ST7789V软件复位
void ST7789V_Rst(void)
{
	GPIO_ResetBits(ST7789V_RST_PORT, ST7789V_RST_PIN); // 低电平复位
	ST7789V_Delay(0xAFF); 					   
	GPIO_SetBits (ST7789V_RST_PORT, ST7789V_RST_PIN);		 	 
	ST7789V_Delay(0xAFF); 
}
```

##### 2.2.5.9 清屏函数

```c
// 清屏
void ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
	ST7789V_OpenWindow(usX, usY, usWidth, usHeight);
	ST7789V_FillColor(usWidth * usHeight, CurrentBackColor) ;		
}
// 对LCD的某一点以某种颜色进行填充
void ST7789V_SetPointPixel(uint16_t usX, uint16_t usY)	
{	
	if((usX < LCD_X_LENGTH)&&(usY < LCD_Y_LENGTH))
  	{
		ST7789V_SetCursor(usX, usY);	
		ST7789V_FillColor(1, CurrentTextColor);
	}
}
```

##### 2.2.5.10 获取像素点

```c
// 读取 GRAM 的一个像素数据
static uint16_t ST7789V_Read_PixelData(void)	
{	
	uint16_t usRG=0, usB=0;
	ST7789V_Write_Cmd(0x2E);
	ST7789V_Read_Data (); // 去掉前一次读取结果
	// 获取红色通道与绿色通道的值
	usRG = ST7789V_Read_Data();  
	usB = ST7789V_Read_Data();  	
  	return((usRG&0xF800)|((usRG<<3)&0x7E0)|(usB>>11));
}
// 获取LCD的某一点的颜色值
uint16_t ST7789V_GetPointPixel(uint16_t usX, uint16_t usY)
{ 
	uint16_t usPixelData;
	ST7789V_SetCursor(usX, usY);
	usPixelData = ST7789V_Read_PixelData();
	return usPixelData;
}
```

##### 2.2.5.11 显示相关函数

```c
// 在LCD上显示一个英文字符
void ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
	uint8_t  byteCount, bitCount,fontLength;	
	uint16_t ucRelativePositon;
	uint8_t *Pfont;
	// 对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
	ucRelativePositon = cChar - ' ';
	// 每个字模的字节数
	fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
	//字模首地址
	/*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	// 设置显示窗口
	ST7789V_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);	
	ST7789V_Write_Cmd(CMD_SetPixel);			
	// 按字节读取字模数据
	for(byteCount = 0; byteCount < fontLength; byteCount++)
	{
		// 一位一位处理要显示的颜色
		for(bitCount = 0; bitCount < 8; bitCount++ )
		{
			if(Pfont[byteCount] & (0x80>>bitCount))
				ST7789V_Write_Data(CurrentTextColor);			
			else
				ST7789V_Write_Data(CurrentBackColor);
		}	
	}	
}
// 在LCD上显示英文字符串
void ST7789V_DispStringLine_EN(uint16_t line,  char* pStr)
{
	uint16_t usX = 0;
	while(*pStr != '\0')
	{
		if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ST7789V_X_Star;
			line += LCD_Currentfonts->Height;
		}
		if ( ( line - ST7789V_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usX = ST7789V_X_Star;
			line = ST7789V_Y_Star;
		}
		ST7789V_DispChar_EN(usX, line, * pStr);
		pStr ++;
		usX += LCD_Currentfonts->Width;	
	}
}
// 在LCD上显示英文字符串
void ST7789V_DispString_EN(uint16_t usX ,uint16_t usY,  char* pStr)
{
	while(*pStr != '\0')
	{
		if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ST7789V_X_Star;
			usY += LCD_Currentfonts->Height;
		}		
		if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH)
		{
			usX = ST7789V_X_Star;
			usY = ST7789V_Y_Star;
		}	
		ST7789V_DispChar_EN(usX, usY, * pStr);
		pStr ++;
		usX += LCD_Currentfonts->Width;	
	}
}
// 在LCD上显示英文字符串(沿Y轴方向)
void ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY, char* pStr)
{	
	while(* pStr != '\0')
	{
		if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height)>LCD_Y_LENGTH)
		{
			usY = ST7789V_Y_Star;
			usX += LCD_Currentfonts->Width;
		}
		if((usX - ST7789V_X_Star + LCD_Currentfonts->Width)> LCD_X_LENGTH)
		{
			usX = ST7789V_X_Star;
			usY = ST7789V_Y_Star;
		}
		ST7789V_DispChar_EN(usX, usY, *pStr);
		pStr ++;
		usY += LCD_Currentfonts->Height;		
	}	
}
// 设置英文字体类型
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}
// 获取当前字体类型
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
// 设置LCD的前景(字体)及背景颜色,RGB565
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}
// 获取LCD的前景(字体)及背景颜色,RGB565
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}
// 设置LCD的前景(字体)颜色,RGB565
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}
// 设置LCD的背景颜色,RGB565
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}
// 清除某行文字
void LCD_ClearLine(uint16_t Line)
{
  ST7789V_Clear(0,Line,LCD_X_LENGTH,((sFONT *)LCD_GetFont())->Height);
}

```

#### 2.2.6 主函数测试

初始化完成后，调用LCD_Test函数显示各种图形进行测试(如直线、矩形、圆形)

```c
#include "stm32f10x.h"
#include "lcd.h"
#include <stdio.h>

static void LCD_Test(void);	
static void Delay ( __IO uint32_t nCount );

int main(void)
{
	ST7789V_Init (); 
  ST7789V_GramScan(6); 
	while ( 1 )
	{
		LCD_Test(); // 测试显示函数
	}
	
}
extern uint16_t lcdid;
// 用于测试各种液晶的函数
void LCD_Test(void)
{
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	testCNT++;
	LCD_SetFont(&Font8x16); 
	LCD_SetColors(RED, BLACK); 
  ST7789V_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
  /* 显示字符串测试 */
  if(lcdid == LCDID_ST7789V)
  {
    ST7789V_DispStringLine_EN(LINE(1),"ST7789V LCD driver");
  }
	/* 不同字体测试 */
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(2));	
	ST7789V_DispStringLine_EN(LINE(2),dispBuff);
	/* 显示图形测试 */
	LCD_SetFont(&Font24x32);
  /* 画直线 */
  LCD_ClearLine(LINE(3));
	LCD_SetTextColor(BLUE);
  ST7789V_DispStringLine_EN(LINE(3),"Draw line:");
	LCD_SetTextColor(RED);
  ST7789V_DrawLine(50,170,210,230);  
  ST7789V_DrawLine(50,200,210,240);
	LCD_SetTextColor(GREEN);
  ST7789V_DrawLine(100,170,200,230);  
  ST7789V_DrawLine(200,200,220,240);
	LCD_SetTextColor(BLUE);
  ST7789V_DrawLine(110,170,110,230);  
  ST7789V_DrawLine(130,200,220,240);
  Delay(0xFFFFFF); 
  ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
  /*画矩形*/
  LCD_ClearLine(LINE(3));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);
  ST7789V_DispStringLine_EN(LINE(3),"Draw Rect:");
	LCD_SetTextColor(RED);
  ST7789V_DrawRectangle(50,200,100,30,1);
	LCD_SetTextColor(GREEN);
  ST7789V_DrawRectangle(160,200,20,40,0);
	LCD_SetTextColor(BLUE);
  ST7789V_DrawRectangle(170,200,50,20,1);
  Delay(0xFFFFFF);
	ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	
  /* 画圆 */
  LCD_ClearLine(LINE(3));	
	LCD_SetTextColor(BLUE);
  ST7789V_DispStringLine_EN(LINE(3),"Draw Cir:");
	LCD_SetTextColor(RED);
  ST7789V_DrawCircle(100,200,20,0);
	LCD_SetTextColor(GREEN);
  ST7789V_DrawCircle(100,200,10,1);
	LCD_SetTextColor(BLUE);
	ST7789V_DrawCircle(140,200,20,0);
  Delay(0xFFFFFF);  
  ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
}

static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}
```

## 3. 小结

本章只需要会使用封装的LCD函数即可

---

2024.9.27 第一次修订，后期不再维护

2025.2.2 简化内容和程序
