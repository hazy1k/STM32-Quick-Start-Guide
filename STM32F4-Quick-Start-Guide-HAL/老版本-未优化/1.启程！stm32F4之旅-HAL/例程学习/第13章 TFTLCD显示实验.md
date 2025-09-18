# 第十三章 TFTLCD显示实验

## 1. 硬件设计

本实验用到的硬件资源有：

- 指示灯DS0

- TFT LCD模块

![屏幕截图 2024 09 20 105553](https://img.picgo.net/2024/09/20/-2024-09-20-105553de0731c46735b375.png)

![屏幕截图 2024 09 20 105641](https://img.picgo.net/2024/09/20/-2024-09-20-10564168e56c0044c82e01.png)

## 2. 软件设计

在 lcd.c 里面代码比较多，我们这里就不贴出来了，只针对几个重要的函数进行讲解。

### 2.1 定义LCD操作结构体

本实验，我们用到 FSMC 驱动 LCD，通过前面的介绍，我们知道 TFTLCD 的 RS 接在 FSMC的 A6 上面， CS 接在 FSMC_NE4 上，并且是 16 位数据总线。即我们使用的是 FSMC 存储器 1的第 4 区，我们定义如下 LCD 操作结构体

```c
// LCD地址结构体
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;
// 使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A6作为数据命令区分线 
// 注意设置时STM32内部会右移一位对其! 111 1110=0X7E			    
#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
```

其中 LCD_BASE，必须根据我们外部电路的连接来确定，我们使用 Bank1.sector4 就是从地址 0X6C000000 开始，而 0X0000007E，则是 A6 的偏移量，这里很多朋友不理解这个偏移量的概念，简单说明下：以 A6 为例， 7E 转换成二进制就是： 1111110，而 16 位数据时，地址右移一位对齐，那么实际对应到地址引脚的时候，就是： A6:A0=0111111，此时 A6 是 0，但是如果 16 位地址再加 1（注意：对应到 8 位地址是加 2，即 7E+0X02），那么： A6:A0=1000000，此时 A6 就是 1 了，即实现了对 RS 的 0 和 1 的控制。

我们将这个地址强制转换为 LCD_TypeDef 结构体地址，那么可以得到 LCD->LCD_REG 的地址就是 0X6C00,007E，对应 A6 的状态为 0(即 RS=0)，而 LCD-> LCD_RAM 的地址就是0X6C00,0080（结构体地址自增），对应 A6 的状态为 1（即 RS=1）。

所以，有了这个定义，当我们要往 LCD 写命令/数据的时候，可以这样写：

```c
LCD->LCD_REG=CMD;  // 写命令
LCD->LCD_RAM=DATA; // 写数据
```

而读的时候反过来操作就可以了，如下所示：

```c
CMD = LCD->LCD_REG; // 读 LCD 寄存器
DATA = LCD->LCD_RAM;// 读 LCD 数据
```

### 2.2 LCD重要参数集

```c
// LCD重要参数集
typedef struct  
{		 	 
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		//开始写gram指令
	u16 setxcmd;		//设置x坐标指令
	u16 setycmd;		//设置y坐标指令 
}_lcd_dev; 	  

// LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
```

该结构体用于保存一些 LCD 重要参数信息，比如 LCD 的长宽、 LCD ID（驱动 IC 型号）、LCD 横竖屏状态等，这个结构体虽然占用了十几个字节的内存，但是却可以让我们的驱动函数支持不同尺寸的 LCD，同时可以实现 LCD 横竖屏切换等重要功能，所以还是利大于弊的。

### 2.3 LCD相关功能函数

```c
// 写寄存器函数
// regval:寄存器值
void LCD_WR_REG(vu16 regval)
{ 
    regval = regval; // 使用-O2 优化的时候,必须插入的延时
    LCD->LCD_REG = regval; // 写入要写的寄存器序号
}
// 写 LCD 数据
// data:要写入的值
void LCD_WR_DATA(vu16 data)
{ 
    data = data; // 使用-O2 优化的时候,必须插入的延时
    LCD->LCD_RAM = data;
}
// 读 LCD 数据
// 返回值:读到的值
u16 LCD_RD_DATA(void)
{ 
    vu16 ram; // 防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}
// 写寄存器
// LCD_Reg:寄存器地址
// LCD_RegValue:要写入的数据
void LCD_WriteReg(vu16 LCD_Reg, vu16 LCD_RegValue)
{ 
    LCD->LCD_REG = LCD_Reg; //写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue; //写入数据
}
// 读寄存器
// LCD_Reg:寄存器地址
// 返回值:读到的数据
u16 LCD_ReadReg(vu16 LCD_Reg)
{ 
    LCD_WR_REG(LCD_Reg); //写入要读的寄存器序号
    delay_us(5);
    return LCD_RD_DATA(); //返回读到的值
}
// 开始写 GRAM
void LCD_WriteRAM_Prepare(void)
{ 
    LCD->LCD_REG=lcddev.wramcmd;
}
// LCD 写 GRAM
// RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{ 
    LCD->LCD_RAM = RGB_Code; // 写十六位 GRAM
}
```

因为 FSMC 自动控制了 WR/RD/CS 等这些信号，所以这 7 个函数实现起来都非常简单，我们就不多说，注意，上面有几个函数，我们添加了一些对 MDK – O2 优化的支持，去掉的话，在-O2 优化的时候会出问题。这些函数实现功能见函数前面的备注，通过这几个简单函数的组合，我们就可以对 LCD 进行各种操作了。

### 2.4 坐标设置函数

```c
//设置光标位置(对RGB屏无效)
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0x7789||lcddev.id==0X7796)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
	}
	else if(lcddev.id==0x9486||lcddev.id==0x9488)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);	LCD_WR_DATA(Xpos&0XFF); 
		LCD_WR_DATA((lcddev.width-1)>>8);	LCD_WR_DATA((lcddev.width-1)&0XFF);	
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);	LCD_WR_DATA(Ypos&0XFF);
		LCD_WR_DATA((lcddev.height-1)>>8);	LCD_WR_DATA((lcddev.height-1)&0XFF); 	
	}
	else if(lcddev.id==0X6804||lcddev.id==0x9481)
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏时处理
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 
	}else if(lcddev.id==0X1963)
	{  			 		
		if(lcddev.dir==0)//x坐标需要变换
		{
			Xpos=lcddev.width-1-Xpos;
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0); 		
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);		 	 
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 		
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);		 	 			
		}	
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF); 			 		
		
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(Xpos>>8); 		
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(Xpos&0XFF);			 
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(Ypos>>8);  		
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(Ypos&0XFF);			
	}else
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
		LCD_WriteReg(lcddev.setxcmd, Xpos);
		LCD_WriteReg(lcddev.setycmd, Ypos);
	}	 
} 		 
```

该函数实现将 LCD 的当前操作点设置到指定坐标(x,y)。因为 9341/5310/6804/5510 等的设置同其他屏有些不太一样，所以进行了区别对待。

### 2.5 画点函数

```c
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)    
{
    LCD_SetCursor(x,y); //设置光标位置
    LCD_WriteRAM_Prepare(); //开始写入 GRAM
    LCD->LCD_RAM=POINT_COLOR;
}
```

该函数实现比较简单，就是先设置坐标，然后往坐标写颜色。其中 POINT_COLOR 是我们定义的一个全局变量，用于存放画笔颜色，顺带介绍一下另外一个全局变量： BACK_COLOR，该变量代表 LCD 的背景色。 LCD_DrawPoint 函数虽然简单，但是至关重要，其他几乎所有上层函数，都是通过调用这个函数实现的。

### 2.6 读点函数

有了画点， 当然还需要有读点的函数，第九个介绍的函数就是读点函数，用于读取 LCD的 GRAM， 这里说明一下，为什么 OLED 模块没做读 GRAM 的函数，而这里做了。因为 OLED模块是单色的，所需要全部 GRAM 也就 1K 个字节，而 TFTLCD 模块为彩色的，点数也比 OLED模块多很多，以 16 位色计算， 一款 320× 240 的液晶，需要 320× 240× 2 个字节来存储颜色值，也就是也需要 150K 字节，这对任何一款单片机来说，都不是一个小数目了。而且我们在图形叠加的时候，可以先读回原来的值，然后写入新的值，在完成叠加后，我们又恢复原来的值。这样在做一些简单菜单的时候，是很有用的。这里我们读取 TFTLCD 模块数据的函数为LCD_ReadPoint，该函数直接返回读到的 GRAM 值。该函数使用之前要先设置读取的 GRAM地址，通过 LCD_SetCursor 函数来实现。 LCD_ReadPoint 的代码如下：

```c
//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
u32 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	LCD_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963||lcddev.id==0x9481||lcddev.id==0x7789)LCD_WR_REG(0X2E);//9341/6804/3510/1963/7789 发送读GRAM指令
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 发送读GRAM指令
	else LCD_WR_REG(0X22);      		 			//其他IC发送读GRAM指令
	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320,延时2us	    
 	r=LCD_RD_DATA();								//dummy Read	   
	if(lcddev.id==0X1963)return r;					//1963直接读就可以 
	opt_delay(2);	  
 	r=LCD_RD_DATA();  		  						//实际坐标颜色
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0x9481||lcddev.id==0x7789||lcddev.id==0x9488)		//9341/NT35310/NT35510要分2次读出
 	{
		opt_delay(2);	  
		b=LCD_RD_DATA(); 
		g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	} 
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0XB505||lcddev.id==0XC505||lcddev.id==0x9486||lcddev.id==0x7796)
		return r;	//这几种IC直接返回颜色值
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0x7789||lcddev.id==0x9488)
		return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
	else 
		return LCD_BGR2RGB(r);						//其他IC
}			 
```

在 LCD_ReadPoint 函数中，因为我们的代码不止支持一种 LCD 驱动器，所以，我们根据不同的 LCD 驱动器（(lcddev.id）型号，执行不同的操作，以实现对各个驱动器兼容，提高函数的通用性。

### 2.7 字符显示函数

字符显示函数 LCD_ShowChar，该函数同前面 OLED 模块的字符显示函数差不多，但是这里的字符显示函数多了 1 个功能，就是可以以叠加方式显示，或者以非叠加方式显示。叠加方式显示多用于在显示的图片上再显示字符。非叠加方式一般用于普通的显示。该函数实现代码如下：

```c
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24/32
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else if(size==32)temp=asc2_3216[num][t];	//调用3216字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
```

在 LCD_ShowChar 函数里面，我们采用快速画点函数 LCD_Fast_DrawPoint 来画点显示字符，该函数同 LCD_DrawPoint 一样，只是带了颜色参数，且减少了函数调用的时间，详见本例程源码。

### 2.8 TFTLCD初始化函数

该函数先初始化 STM32 与TFTLCD 连接的 IO 口，并配置 FSMC 控制器，然后读取 LCD 控制器的型号，根据控制 IC 的型号执行不同的初始化代码，其简化代码如下：

```c
//初始化lcd
//该初始化函数可以初始化各种型号的LCD(详见本.c文件最前面的描述)
void LCD_Init(void)
{ 	  
	GPIO_InitTypeDef GPIO_Initure;
	FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
	FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	GPIO_Initure.Pin=GPIO_PIN_15;          	//PB15,背光控制
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 
	
	TFTSRAM_Handler.Instance=FSMC_NORSRAM_DEVICE;                
	TFTSRAM_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;    
    
	TFTSRAM_Handler.Init.NSBank=FSMC_NORSRAM_BANK4;     				//使用NE4
	TFTSRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE; 	//地址/数据线不复用
	TFTSRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;   			//SRAM
	TFTSRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16; //16位数据宽度
	TFTSRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE; //是否使能突发访问,仅对同步突发存储器有效,此处未用到
	TFTSRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;//等待信号的极性,仅在突发模式访问下有用
	TFTSRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;   //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
	TFTSRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;    //存储器写使能
	TFTSRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;           //等待使能位,此处未用到
	TFTSRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_ENABLE;        //读写使用不同的时序
	TFTSRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;//是否使能同步传输模式下的等待信号,此处未用到
	TFTSRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;           //禁止突发写
	TFTSRAM_Handler.Init.ContinuousClock=FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
    
	//FMC读时序控制寄存器
	FSMC_ReadWriteTim.AddressSetupTime=0x0F;       	//地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
	FSMC_ReadWriteTim.AddressHoldTime=0;
	FSMC_ReadWriteTim.DataSetupTime=60;				//数据保存时间为60个HCLK	=6*60=360ns
	FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;//模式A
	//FMC写时序控制寄存器
	FSMC_WriteTim.BusTurnAroundDuration=0;			//总线周转阶段持续时间为0，此变量不赋值的话会莫名其妙的自动修改为4。导致程序运行正常
	FSMC_WriteTim.AddressSetupTime=9;          		//地址建立时间（ADDSET）为77个HCLK =54ns 
	FSMC_WriteTim.AddressHoldTime=0;
	FSMC_WriteTim.DataSetupTime=17;              	//数据保存时间为6ns*9个HCLK=54n
	FSMC_WriteTim.AccessMode=FSMC_ACCESS_MODE_A;    //模式A
	HAL_SRAM_Init(&TFTSRAM_Handler,&FSMC_ReadWriteTim,&FSMC_WriteTim);	
	
	delay_ms(50); // delay 50 ms 
 	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
  	lcddev.id = LCD_ReadReg(0x0000);   
   	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
	{	
 		//尝试9341 ID的读取		
		LCD_WR_REG(0XD3);				   
		lcddev.id=LCD_RD_DATA();	//dummy read 	
 		lcddev.id=LCD_RD_DATA();	//读到0X00
  		lcddev.id=LCD_RD_DATA();   	//读取93								   
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();  	//读取41 	   			   
 		if(lcddev.id!=0X9341  && lcddev.id!=0X9486&& lcddev.id!=0X9488 &&lcddev.id!=0x7796)		//非9341,尝试是不是6804
		{	
			LCD_WR_REG(0X04);
			lcddev.id = LCD_RD_DATA();      /* dummy read */
			lcddev.id = LCD_RD_DATA();      /* 读到0X85 */
			lcddev.id = LCD_RD_DATA();      /* 读取0X85 */
			lcddev.id <<= 8;
			lcddev.id |= LCD_RD_DATA();     /* 读取0X52 */
			if (lcddev.id == 0X8552)        /* 将8552的ID转换成7789 */
			{
				lcddev.id = 0x7789;
			}
			if (lcddev.id != 0x7789)        /* 也不是ST7789, 尝试是不是 NT35310 */
			{
				LCD_WR_REG(0XBF);				   
				lcddev.id=LCD_RD_DATA(); 	//dummy read 	 
				lcddev.id=LCD_RD_DATA();   	//读回0X01			   
				lcddev.id=LCD_RD_DATA(); 	//读回0XD0 			  	
				lcddev.id=LCD_RD_DATA();	//这里读回0X68 
				lcddev.id<<=8;
				lcddev.id|=LCD_RD_DATA();	//这里读回0X04	  
				if(lcddev.id!=0X6804&& lcddev.id!=0X9481)	//也不是6804或者9481,尝试看看是不是NT35310
				{ 
					LCD_WR_REG(0XD4);				   
					lcddev.id=LCD_RD_DATA();//dummy read  
					lcddev.id=LCD_RD_DATA();//读回0X01	 
					lcddev.id=LCD_RD_DATA();//读回0X53	
					lcddev.id<<=8;	 
					lcddev.id|=LCD_RD_DATA();	//这里读回0X10	 
					if(lcddev.id!=0X5310)		//也不是NT35310,尝试看看是不是NT35510
					{
						LCD_WR_REG(0XDA00);	
						lcddev.id=LCD_RD_DATA();		//读回0X00	 
						LCD_WR_REG(0XDB00);	
						lcddev.id=LCD_RD_DATA();		//读回0X80
						lcddev.id<<=8;	
						LCD_WR_REG(0XDC00);	
						lcddev.id|=LCD_RD_DATA();		//读回0X00		
						if(lcddev.id==0x8000)lcddev.id=0x5510;//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
						if(lcddev.id!=0X5510)			//也不是NT5510,尝试看看是不是SSD1963
						{
							LCD_WR_REG(0XA1);
							lcddev.id=LCD_RD_DATA();
							lcddev.id=LCD_RD_DATA();	//读回0X57
							lcddev.id<<=8;	 
							lcddev.id|=LCD_RD_DATA();	//读回0X61	
							if(lcddev.id==0X5761)lcddev.id=0X1963;//SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
						}
					}
				}
			}
 		}  	
	} 
```

从初始化代码可以看出， LCD 初始化步骤为：

先对 FSMC 相关 IO 进行初始化，然后是 FSMC 的初始化，这个我们在前面都有介绍，最后根据读到的 LCD ID，对不同的驱动器执行不同的初始化代码，从上面的代码可以看出，这个初始化函数可以针对十多款不同的驱动 IC 执行初始化操作，这样大大提高了整个程序的通用性。

### 2.9 主函数

```c
int main(void)
{
    u8 x=0;
    u8 lcd_id[12]; // 存放LCD ID字符串

    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
    delay_init(168);            // 初始化延时函数
    uart_init(115200);          // 初始化USART
    LED_Init();                 // 初始化LED    
    LCD_Init();                 // 初始化LCD FSMC接口
    POINT_COLOR=RED;            // 画笔颜色：红色
    sprintf((char*)lcd_id, "LCD ID:%04X", lcddev.id); // 将LCD ID打印到lcd_id数组。
    while(1) 
    {         
        switch(x)
        {
            case 0:LCD_Clear(WHITE);break;
            case 1:LCD_Clear(BLACK);break;
            case 2:LCD_Clear(BLUE);break;
            case 3:LCD_Clear(RED);break;
            case 4:LCD_Clear(MAGENTA);break;
            case 5:LCD_Clear(GREEN);break;
            case 6:LCD_Clear(CYAN);break; 
            case 7:LCD_Clear(YELLOW);break;
            case 8:LCD_Clear(BRRED);break;
            case 9:LCD_Clear(GRAY);break;
            case 10:LCD_Clear(LGRAY);break;
            case 11:LCD_Clear(BROWN);break;
        }
        POINT_COLOR = RED;      
        LCD_ShowString(30,40,210,24,24,"Explorer STM32F4");    
        LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
        LCD_ShowString(30,90,200,16,16,"hello world!");
        LCD_ShowString(30,110,200,16,16,lcd_id); // 显示LCD ID                               
        LCD_ShowString(30,130,200,12,12,"2024/9/20");                               
        x++;
        if(x==12)x=0;
        LED0=!LED0;     
        delay_ms(1000);    
    } 
}
```

该部分代码将显示一些固定的字符，字体大小包括 24 * 12、 16 * 8 和 12*6 等三种，同时显示LCD 驱动 IC 的型号，然后不停的切换背景颜色，每 1s 切换一次。而 LED0 也会不停的闪烁，指示程序已经在运行了。 其中我们用到一个 sprintf 的函数，该函数用法同 printf，只是 sprintf把打印内容输出到指定的内存区间上

## 3. 小结

实际上我们会使用已经封装好的函数即可，驱动代码一般厂家都会提供，GitHub上面也有

---

2024.10.7 第一次修订


