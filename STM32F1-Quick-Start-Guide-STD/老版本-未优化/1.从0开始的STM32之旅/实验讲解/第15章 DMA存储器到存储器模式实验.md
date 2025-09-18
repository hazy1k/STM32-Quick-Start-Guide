# 第十五章 DMA存储器到存储器模式实验

## 1. 硬件设计

存储器到存储器模式可以实现数据在两个内存的快速拷贝。我们先定义一个静态的源数据，存放在内部FLASH， 然后使用DMA传输把源数据拷贝到目标地址上（内部SRAM），最后对比源数据和目标地址的数据，看看是否传输准确 。

DMA存储器到存储器实验不需要其他硬件要求，只用到RGB彩色灯用于指示程序状态。

## 2. 软件设计

### 2.1 编程大纲

1. 使能DMA时钟

2. 配置DMA数据参数

3. 使能DMA，进行传输

4. 等待传输完成，并对源数据和目标地址数据进行比较

### 2.2 代码分析

#### 2.2.1 DMA宏定义

```c
// 选择使用的通道和时钟
#define DMA_CHx  DMA1_Channel6
#define DMA_CLK  RCC_AHBPeriph_DMA1
// 传输完成标志
#define DMA_FLAG_TC   DMA1_FLAG_TC6
// 要发送的数据的最长长度
#define DMA_BUFFER_SIZE  32

extern uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE];
extern uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];
```

使用宏定义设置外设配置方便程序修改和升级。

存储器到存储器传输通道没有硬性规定，可以随意选择。

Send_DMA_Buffer[DMA_BUFFER_SIZE]定义用来存放源数据

#### 2.2.2 DMA配置

```c
void DMA_Config(void)
{
    // 1.定义一个DMA初始化结构体
    DMA_InitTypeDef DMA_InitStructure;
    // 2.使能DMA时钟
    RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
    // 3.选择源数据地址和目标数据地址（我们这个工程源是FLASH，目标是SRAM）
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)Send_DMA_BUFFER;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Rece_DMA_BUFFER;
    // 4.设置传输方向和大小（我们工程使用的外设道存储器，这里的外设指内部的FLASH）
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
    // 5.设置外设地址递增和内存地址递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 6.设置外设和内存数据单位
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 外设数据单位为字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    // 7.设置模式（一次或者循环模式）
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    // 8.设置优先级
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    // 9.初始化
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_ClearFlag(DMA_FLAG_TC);
    DMA_Cmd(DMA_CHx, ENABLE);
}
```

使用DMA_InitTypeDef结构体定义一个DMA初始化变量，参考：

```c
typedef struct
{
uint32_t DMA_PeripheralBaseAddr;   // 外设地址
uint32_t DMA_MemoryBaseAddr;       // 存储器地址
uint32_t DMA_DIR;                  // 传输方向
uint32_t DMA_BufferSize;           // 传输数目
uint32_t DMA_PeripheralInc;        // 外设地址增量模式
uint32_t DMA_MemoryInc;            // 存储器地址增量模式
uint32_t DMA_PeripheralDataSize;   // 外设数据宽度
uint32_t DMA_MemoryDataSize;       // 存储器数据宽度
uint32_t DMA_Mode;                 // 模式选择
uint32_t DMA_Priority;             // 通道优先级
uint32_t DMA_M2M;                  // 存储器到存储器模式
}DMA_InitTypeDef;
```

调用RCC_AHBPeriphClockCmd函数开启DMA时钟，使用DMA控制器之前必须开启对应的时钟。

源地址和目标地址使用之前定义的数组首地址，传输的数据量为宏BUFFER_SIZE决定，源和目标地址指针地址递增， 使用一次传输模式不能循环传输，因为只有一个DMA通道，优先级随便设置，最后调用DMA_Init函数完成DMA的初始化配置。

DMA_ClearFlag函数用于清除DMA标志位，代码用到传输完成标志位，使用之前先清除传输完成标志位以免产生不必要干扰。 DMA_ClearFlag函数需要1个形参，即事件标志位，可选有传输完成标志位、半传输标志位、FIFO错误标志位、传输错误标志位等等， 非常多，我们这里选择传输完成标志位，由宏DMA_FLAG_TC定义。

DMA_Cmd函数用于启动或者停止DMA数据传输，它接收两个参数，第一个是DMA通道，另外一个是开启ENABLE或者停止DISABLE。

#### 2.2.3 存储器数据对比

```c
// 判断两个数据是否相等
uint8_t Buffer_Compare(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t Length)
{
	// 数据长度递增
	while(Length--)
	{
		if(*pBuffer != *pBuffer1)
		{
			return 0; // 如果不同，返回0
		}
		pBuffer++;
		pBuffer1++;
	}
	return 1; // 如果相同，返回1
}
```

判断指定长度的两个数据源是否完全相等，如果完全相等返回1；只要其中一对数据不相等返回0。 它需要三个形参，前两个是两个数据源的地址，第三个是要比较数据长度。

#### 2.2.4 主函数测试

```c
int main()
{
	uint8_t Buffer_Result; // 两个数据比较结果
	LED_Init();
	LED_PURPLE();
	Delay(0xFFFFFF);
	DMA_Config(); // 启动DMA传输
	while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET); // 等待DMA传输完成
	// 比较通过DMA传输的数据和预设数据是否相同
	Buffer_Result = Buffer_Compare(Send_DMA_BUFFER, Rece_DMA_BUFFER, DMA_BUFFER_SIZE);
	if(Buffer_Result == 0)
	{
		LED_RED(); // 显示不同
	}
	else
	{
		LED_GREEN(); // 显示相同
	}
	while(1)
	{

	}
}
```

首先定义一个变量用来保存存储器数据比较结果。

RGB彩色灯用来指示程序进程，使用之前需要初始化它，开始设置RGB彩色灯为紫色

Delay函数只是一个简单的延时函数。

调用DMA_Config函数完成DMA数据流配置并启动DMA数据传输。

DMA_GetFlagStatus函数获取DMA事件标志位的当前状态，这里获取DMA数据传输完成这个标志位，使用循环持续等待直到该标志位被置位， 即DMA传输完成这个事件发生，然后退出循环，运行之后程序。

确定DMA传输完成之后就可以调用Buffercmp函数比较源数据与DMA传输后目标地址的数据是否一一对应。保存比较结果， 如果为1表示两个数据源一一对应相等说明DMA传输成功；相反，如果为0表示两个数据源数据存在不等情况，说明DMA传输出错。

如果DMA传输成功设置RGB彩色灯为蓝色，如果DMA传输出错设置RGB彩色灯为红色。

## 3. 小节

这一章相比于之前的串口通信就简单多了，主要就是理解一个配置的问题，然后直接调用即可，我们可以再来梳理一下：

1. 首先宏定义DAM基本参数嘛：选择通道、时钟、传输完成标志，传输数据大小，内部flash数据，目标sram（定义一个空间用来接收）

2. 各种函数声明，不必多说

3. 最重要DMA配置：初始化结构体->开启DMA时钟->设置源数据地址（我们的flash中存储的）->设置目标存储地址（sram）->设置传输方向（DMA_DIR_PeripheralSRC，意思是外设到存储器）->设置传输大小（我们已经宏定义过了）->设置外设地址递增（ DMA_PeripheralInc_Enable）->设置内存地址递增（DMA_MemoryInc_Enable）->设置外设数据单位（DMA_PeripheralDataSize_Word）->设置内存数据单位（DMA_MemoryDataSize_Word）->设置MDA模式，这里我们选择一次模式->配置通道优先级为高->使能内存到内存传输->接下来三件套（配置结构体，清除标志位，使能）

4. 判断两个数据长度是否相等：

```c
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* 数据长度递减 */
  while(BufferLength--)
  {
    /* 判断两个数据源是否对应相等 */
    if(*pBuffer != *pBuffer1)
    {
      /* 对应数据源不相等马上退出函数，并返回0 */
      return 0;
    }
    /* 递增两个数据源的地址指针 */
    pBuffer++;
    pBuffer1++;
  }
  /* 完成判断并且对应数据相对 */
  return 1; // 数据完全相等
}
```

5. 主函数往往最简单了，自行看注释理解吧

当然了出现新的库函数，我们应该介绍一下：

1. **`DMA_Init(DMA_CHANNEL, &DMA_InitStructure);`**  
   这个函数用于初始化DMA通道。`DMA_CHANNEL`是要配置的DMA通道的标识符，例如DMA1_Channel1，`&DMA_InitStructure`是一个指向`DMA_InitTypeDef`结构体的指针，包含了所有的配置参数，如数据方向、传输大小、优先级等。通过调用此函数，DMA通道会根据提供的配置结构体进行初始化，设置DMA的各项操作参数。

2. **`DMA_ClearFlag(DMA_FLAG_TC);`**  
   这个函数用于清除DMA的传输完成标志位。`DMA_FLAG_TC`是指传输完成的标志位，表示DMA传输完成时产生的标志。清除这个标志位是为了确保之前的传输完成标志不会干扰当前的DMA传输操作。在开始新的DMA传输前，清除标志位是一个重要步骤，以确保数据传输过程的正确性。

3. **`DMA_Cmd(DMA_CHANNEL, ENABLE);`**  
   这个函数用于启用DMA通道。`DMA_CHANNEL`指定要使能的DMA通道，例如DMA1_Channel1，`ENABLE`表示要启用该通道。调用此函数后，DMA通道将开始进行数据传输。这个步骤是启动DMA传输的关键，确保DMA能够按照预设的配置开始实际的数据搬运工作。

```c
while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET)
{
    // 空循环，等待DMA传输完成标志位置位
}
```

1. **`DMA_GetFlagStatus(DMA_FLAG_TC)`**  
   这个函数用于获取DMA标志位的状态。`DMA_FLAG_TC`代表传输完成标志（Transfer Complete Flag），它指示DMA传输是否已完成。函数返回标志位的当前状态。

2. **`== RESET`**  
   `RESET`是一个常量，用于表示标志位尚未被置位。DMA传输完成标志位在传输完成时会被置位为`SET`。在标志位为`RESET`时，表示传输尚未完成。

3. **`while`循环**  
   这个`while`循环持续检查DMA的传输完成标志位是否为`RESET`。也就是说，它会一直等待，直到`DMA_GetFlagStatus(DMA_FLAG_TC)`返回`SET`（传输完成）。在此之前，循环体内部是空的（没有任何操作），只是不断地轮询标志位状态。

---

2024.8.28 第一次修订，后期不再维护

2025.1.13 优化代码
