# 第二十五章 SPI实验

## 1. 硬件设计

本章实验功能简介：开机的时候先检测 W25Q128 是否存在，然后在主循环里面检测两个按键，其中 1 个按键（KEY1） 用来执行写入 W25Q128 的操作，另外一个按键（KEY0） 用来执行读出操作，在 TFTLCD 模块上显示相关信息。同时用 DS0 提示程序正在运行。

所要用到的硬件资源如下：

- 指示灯 DS0

- KEY_UP 和 KEY1 按键

- TFTLCD 模块

- SPI

- W25Q128

板上的 W25Q128 是直接连在 STM32F4 的 SPI1上的，连接关系如图所示：

![屏幕截图 2024-10-20 143920.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/20-14-39-27-屏幕截图%202024-10-20%20143920.png)

这里，我们的 F_CS 是连接在 PB14 上面的，另外要特别注意： W25Q128 和 NRF24L01 共用 SPI1，所以这两个器件在使用的时候，必须分时复用（通过片选控制）才行。

## 2. 软件设计

### 2.1 SPI初始化

```c
SPI_HandleTypeDef SPI1_Handler;  //SPI1句柄

// 以下是SPI模块的初始化代码，配置成主机模式 						  
// SPI口初始化
// 这里针是对SPI1的初始化
void SPI1_Init(void)
{
    SPI1_Handler.Instance=SPI1;                         // SPI1
    SPI1_Handler.Init.Mode=SPI_MODE_MASTER;             // 设置SPI工作模式，设置为主模式
    SPI1_Handler.Init.Direction=SPI_DIRECTION_2LINES;   // 设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI1_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       // 设置SPI的数据大小:SPI发送接收8位帧结构
    SPI1_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    // 串行同步时钟的空闲状态为高电平
    SPI1_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI1_Handler.Init.NSS=SPI_NSS_SOFT;                 // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI1_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI1_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        // 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI1_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        // 关闭TI模式
    SPI1_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;// 关闭硬件CRC校验
    SPI1_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI1_Handler);                        // 初始化SPI 
    __HAL_SPI_ENABLE(&SPI1_Handler);                    // 使能SPI1
    SPI1_ReadWriteByte(0Xff);                           // 启动传输
}
```

### 2.2 SPI GPIO配置

```c
// SPI5底层驱动，时钟使能，引脚配置
// 此函数会被HAL_SPI_Init()调用
// hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure; 
    __HAL_RCC_GPIOB_CLK_ENABLE();  // 使能GPIOB时钟
    __HAL_RCC_SPI1_CLK_ENABLE();   // 使能SPI1时钟
    // PB3,4,5
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;    // 复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;        // 上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;   // 快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI1; // 复用为SPI1
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
}
```

### 2.3 SPI速度设置函数

```c
// SPI速度设置函数
// SPI速度=fAPB1/分频系数
// 函数参数:SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
// fAPB1时钟一般为42Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI1_Handler);            // 关闭SPI
    SPI1_Handler.Instance->CR1&=0XFFC7;          // 位3-5清零，用来设置波特率
    SPI1_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI1_Handler);             // 使能SPI
    
}
```

### 2.4 SPI读写一个字节

```c
// SPI1 读写一个字节
// TxData:要写入的字节
// 返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI1_Handler,&TxData,&Rxdata,1, 1000); // 函数参数：SPI句柄，发送数据，接收数据，数据长度，超时时间     
 	return Rxdata; // 返回收到的数据		
}

```

### 2.5 初始化W25Q256

```c
//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q256
//容量为32M字节,共有512个Block,8192个Sector 													 
//初始化SPI FLASH的IO口
void W25QXX_Init(void)
{ 
    u8 temp;
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //使能GPIOB时钟
    //PB14
    GPIO_Initure.Pin=GPIO_PIN_14;           // PB14
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          // 上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     // 快速         
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     // 初始化
	W25QXX_CS=1;			                // SPI FLASH不选中
	SPI1_Init();		   			        // 初始化SPI
	SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_2); // 设置为42M时钟,高速模式
	W25QXX_TYPE=W25QXX_ReadID();	        // 读取FLASH ID.
    if(W25QXX_TYPE==W25Q256)                // SPI FLASH为W25Q256
    {
        temp=W25QXX_ReadSR(3);              // 读取状态寄存器3，判断地址模式
        if((temp&0X01)==0)			        // 如果不是4字节地址模式,则进入4字节地址模式
		{
			W25QXX_CS=0; 			        // 选中
			SPI1_ReadWriteByte(W25X_Enable4ByteAddr); // 发送进入4字节地址模式指令   
			W25QXX_CS=1;       		        // 取消片选   
		}
    }
} 
```

### 2.6 主函数

```c
// 要写入到W25QXX的字符串数组
const u8 TEXT_Buffer[]={"Explorer STM32F4 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
int main(void)
{
    u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE; 
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	usmart_dev.init(84); 		 // 初始化USMART
	LED_Init();					 // 初始化LED	
	KEY_Init();					 // 初始化KEY
 	LCD_Init();           		 // 初始化LCD
    W25QXX_Init();				 // W25QXX初始化
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"SPI TEST");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//显示提示信息		
//	while(W25QXX_ReadID()!=W25Q128)								//检测不到W25Q256
	while(W25QXX_ReadID()!=W25Q128 && W25QXX_ReadID()!=NM25Q128 && W25QXX_ReadID()!=BY25Q128)  /* 读取QSPI FLASH ID */
	{
		LCD_ShowString(30,150,200,16,16,"W25Q128 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0=!LED0;		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"W25Q128 Ready!"); 
	FLASH_SIZE=32*1024*1024;	//FLASH 大小为32M字节
  	POINT_COLOR=BLUE;			//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入W25Q128
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q128....");
			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//提示传送完成
		}
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q128.... ");
			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);					//显示读到的字符串
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	}	
}
```

## 3. 小结

### 实验目标

通过 STM32 微控制器使用 SPI 接口与 W25Q256 进行数据的读写操作。这个实验将涵盖初始化 SPI 接口、发送命令、读取和写入数据的步骤。

### 硬件连接

| W25Q256 引脚 | STM32 引脚          |
| ---------- | ----------------- |
| VCC        | 3.3V              |
| GND        | GND               |
| SCK        | SPI SCK (如 PB13)  |
| MOSI       | SPI MOSI (如 PB15) |
| MISO       | SPI MISO (如 PB14) |
| CS         | SPI CS (如 PB12)   |

### 软件设计

#### SPI 初始化

```c
#include "main.h"

SPI_HandleTypeDef hspi1; // SPI 句柄

void MX_SPI1_Init(void) {
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER; // 主模式
    hspi1.Init.Direction = SPI_DIRECTION_2LINES; // 双线模式
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT; // 数据大小 8 位
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW; // 时钟极性
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE; // 时钟相位
    hspi1.Init.NSS = SPI_NSS_SOFT; // 软件 NSS 管理
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 预分频因子
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB; // MSB 优先
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE; // 禁用 TI 模式
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // 禁用 CRC 计算
    hspi1.Init.CRCPolynomial = 10; // CRC 多项式
    HAL_SPI_Init(&hspi1); // 初始化 SPI
}

```

#### W25Q256 操作命令

W25Q256 的常用操作命令如下：

| 操作      | 命令字  |
| ------- | ---- |
| 写入使能    | 0x06 |
| 写入禁用    | 0x04 |
| 页面编程    | 0x02 |
| 读取数据    | 0x03 |
| 按块擦除    | 0xD8 |
| 读取状态寄存器 | 0x05 |
| 读取设备 ID | 0x9F |

#### 读写数据的函数

```c
#include "main.h"

#define W25Q256_CS_LOW()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define W25Q256_CS_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)

// SPI 发送数据
void W25Q256_Write_Enable(void) {
    W25Q256_CS_LOW();
    uint8_t cmd = 0x06; // 写入使能命令
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    W25Q256_CS_HIGH();
}

void W25Q256_Write(uint32_t address, uint8_t* data, uint16_t length) {
    W25Q256_Write_Enable();

    W25Q256_CS_LOW();
    uint8_t cmd[4];
    cmd[0] = 0x02; // 页面编程命令
    cmd[1] = (address >> 16) & 0xFF; // 地址高字节
    cmd[2] = (address >> 8) & 0xFF; // 地址中字节
    cmd[3] = address & 0xFF; // 地址低字节
    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY); // 发送命令和地址
    HAL_SPI_Transmit(&hspi1, data, length, HAL_MAX_DELAY); // 发送数据
    W25Q256_CS_HIGH();
}

// 读取数据
void W25Q256_Read(uint32_t address, uint8_t* buffer, uint16_t length) {
    W25Q256_CS_LOW();
    uint8_t cmd[4];
    cmd[0] = 0x03; // 读取命令
    cmd[1] = (address >> 16) & 0xFF; // 地址高字节
    cmd[2] = (address >> 8) & 0xFF; // 地址中字节
    cmd[3] = address & 0xFF; // 地址低字节
    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY); // 发送命令和地址
    HAL_SPI_Receive(&hspi1, buffer, length, HAL_MAX_DELAY); // 读取数据
    W25Q256_CS_HIGH();
}

```

#### 主函数示例

```c
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    SystemClock_Config(); // 配置系统时钟
    MX_GPIO_Init(); // 初始化 GPIO
    MX_SPI1_Init(); // 初始化 SPI

    uint8_t write_data[256] = {0}; // 要写入的数据
    uint8_t read_data[256] = {0}; // 用于接收读取的数据
    // 示例数据填充
    for (int i = 0; i < 256; i++) {
        write_data[i] = i; // 填充 0 到 255
    }
    // 写入数据
    W25Q256_Write(0x000000, write_data, sizeof(write_data));
    // 读取数据
    W25Q256_Read(0x000000, read_data, sizeof(read_data));
    // 在这里可以添加代码来处理 read_data 数据
    while (1) {
        // 主循环
    }
}

```
