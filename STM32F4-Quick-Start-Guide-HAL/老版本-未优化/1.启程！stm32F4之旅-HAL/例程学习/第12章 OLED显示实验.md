# 第十二章 OLED显示实验

## 1. 硬件设计

本实验使用到的硬件资源有：

- 指示灯DS0

- OLED模块

OLED 模块的电路在前面已有详细说明了，这里我们介绍 OLED 模块与探索者 STM32F4开发板的连接， 开发板底板的 OLED/CAMERA 接口（P8 接口） 可以和 ALIENTEK OLED 模块直接对插

![屏幕截图 2024 09 19 105921](https://img.picgo.net/2024/09/19/-2024-09-19-1059219b89e6e642bcc0da.png)

图中圈出来的部分就是连接 OLED 的接口，这里在硬件上， OLED 与探索者 STM32F4 开发板的 IO 口对应关系如下：

- OLED_CS 对应 DCMI_VSYNC，即： PB7; 

- OLED_RS 对应 DCMI_SCL，即： PD6; 

- OLED_WR 对应 DCMI_HREF，即： PA4; 

- OLED_RD 对应 DCMI_SDA，即： PD7; 

- OLED_RST 对应 DCMI_RESET， 即： PG15;

- OLED_D[7:0]对应 DCMI_D[7:0]，即： PE6/PE5/PB6/PC11/PC9/PC8/PC7/PC6;

![屏幕截图 2024 09 19 110604](https://img.picgo.net/2024/09/19/-2024-09-19-1106045d3374e5c384a8da.png)

## 2. 软件设计

### 2.1 OLED_Init函数

首先是 OLED_Init 函数， 该函数的结构比较简单，开始是对 IO 口的初始化，这里我们用了宏定义OLED_MODE 来决定要设置的 IO 口，其他就是一些初始化序列了，我们按照厂家提供的资料来做就可以。

后要说明一点的是，因为 OLED 是无背光的，在初始化之后，我们把显存都清空了，所以我们在屏幕上是看不到任何内容的，跟没通电一个样，不要以为这就是初始化失败，要写入数据模块才会显示的。 OLED_Init 函数代码如下：

```c
// 初始化SSD1306                        
void OLED_Init(void)
{               
    GPIO_InitTypeDef  GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();   //使能GPIOA时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();   //使能GPIOD时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();   //使能GPIOE时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();   //使能GPIOG时钟

#if OLED_MODE == 1        //使用8080并口模式        
    //GPIO初始化设置      
    GPIO_Initure.Pin=GPIO_PIN_4;           // PA4
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP; // 推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         // 上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;    // 快速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);    // 初始化
    //PB6,7
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;    
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);//初始化
    //PC6,7,8,9,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;    
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);//初始化    
    //PD6,7
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;    
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);//初始化    
    //PE5,6
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6;    
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);//初始化    
    //PG15
    GPIO_Initure.Pin=GPIO_PIN_15;    
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);//初始化    
    OLED_WR = 1; // 8080并口模式下,WR为高电平有效，所以先置1
    OLED_RD = 1; // 8080并口模式下,RD为高电平有效，所以先置1
#else  // 使用4线SPI 串口模式

    //GPIO初始化设置      
    GPIO_Initure.Pin=GPIO_PIN_7;             //PB7
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;    //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;       //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);       //初始化
    //PC6,7
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;    
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);//初始化
    //PD6
    GPIO_Initure.Pin=GPIO_PIN_6;    
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);//初始化    
    //PG15
    GPIO_Initure.Pin=GPIO_PIN_15;    
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);//初始化
    OLED_SDIN=1;
    OLED_SCLK=1;
#endif
    OLED_CS=1;
    OLED_RS=1;     

    OLED_RST=0;
    delay_ms(100);
    OLED_RST=1; 

    OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
    OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
    OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
    OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
    OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
    OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
    OLED_WR_Byte(0X00,OLED_CMD); //默认为0
    OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.                                            
    OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
    OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
    OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
    OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
    OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
    OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
    OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
    OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置     
    OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
    OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
    OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
    OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
    OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
    OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
    OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示                                   
    OLED_WR_Byte(0xAF,OLED_CMD); //开启显示     
    OLED_Clear();
}  
```

以上代码是配置GPIO，还有厂家提供的一些命令，不必理解适当了解即可。当然，我们的读写时序还是要明白的

```c
OLED_CS=1;
OLED_RS=1;     
OLED_RST=0;
delay_ms(100);
OLED_RST=1; 
```

### 2.2 更新显存到LCD

接着， 要介绍的是OLED_Refresh_Gram 函数。我们在 STM32F4内部定义了一个块GRAM： u8 OLED_GRAM[128][8];此部分 GRAM 对应 OLED 模块上的 GRAM。在操作的时候，我们只要修改 STM32F4 内部的 GRAM 就可以了，然后通过 OLED_Refresh_Gram 函数把 GRAM 一次刷新到 OLED 的 GRAM 上。该函数代码如下

```c
u8 OLED_GRAM[128][8];

// 更新显存到LCD         
void OLED_Refresh_Gram(void)
{
    u8 i,n;            
    for(i = 0; i < 8; i++)  
    {  
        OLED_WR_Byte(0xb0+i, OLED_CMD); // 设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);   // 设置显示位置—列低地址
        OLED_WR_Byte(0x10, OLED_CMD);   // 设置显示位置—列高地址   
        for(n = 0; n < 128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA); 
    }   
}
```

OLED_Refresh_Gram 函数先设置页地址，然后写入列地址（也就是纵坐标），然后从 0 开始写入 128 个字节，写满该页，最后循环把 8 页的内容都写入，就实现了整个从 STM32F4 显存到 OLED 显存的拷贝。

### 2.3 写入字节函数

OLED_Refresh_Gram 函数还用到了一个外部函数， 也就是我们接着要介绍的函数： OLED_WR_Byte，该函数直接和硬件相关，函数代码如下：

```c
// 通过拼凑的方法向OLED输出一个8位数据
// data:要输出的数据
void OLED_Data_Out(u8 data)
{
    u16 dat=data&0X0F;
    GPIOC->ODR&=~(0XF<<6);        // 清空6~9
    GPIOC->ODR|=dat<<6;           // D[3:0]-->PC[9:6]
    GPIOC->ODR&=~(0X1<<11);       // 清空11
    GPIOC->ODR|=((data>>4)&0x01)<<11;
    GPIOB->ODR&=~(0X1<<6);        // 清空6
    GPIOB->ODR|=((data>>5)&0x01)<<6;
    GPIOE->ODR&=~(0X3<<5);        // 清空5,6
    GPIOE->ODR|=((data>>6)&0x01)<<5;
    GPIOE->ODR|=((data>>7)&0x01)<<6;
} 
// 向SSD1306写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    OLED_Data_Out(dat); // 输出数据    
     OLED_RS = cmd; // 写命令 
    OLED_CS = 0;   // 拉低片选    
    OLED_WR = 0;   // 拉低写数据线
    OLED_WR = 1;   // 拉高写数据线，准备发送数据
    OLED_CS = 1;   // 拉高片选，结束数据传输
    OLED_RS = 1;   // 写命令 
} // 下面的代码是SPI方式的驱动方式,可以参考                   
#else
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{    
    u8 i;              
    OLED_RS=cmd; //写命令 
    OLED_CS=0;          
    for(i=0;i<8;i++)
    {              
        OLED_SCLK=0;
        if(dat&0x80)OLED_SDIN=1;
        else OLED_SDIN=0;
        OLED_SCLK=1;
        dat<<=1;   
    }                 
    OLED_CS=1;          
    OLED_RS=1;         
} 
#endif
```

首先，我们看 OLED_Data_Out 函数，这就是我们前面说的，因为 OLED 的 D0~D7 不是接的连续 IO，所以必须将数据，拆分到各个 IO，以实现一次完整的数据传输，该函数就是根据我们 OLED_D[7:0]具体连接的 IO，对数据进行拆分，然后输出给对应位的各个 IO，实现并口数据输出。 这种方式会降低并口速度，但是我们 OLED 模块，是单色的，数据量不是很大，所以这种方式也不会造成视觉上的影响，大家可以放心使用，但是如果是 TFTLCD，就不推荐了。

然后，看 OLED_WR_Byte 函数， 这里有 2 个一样的函数，通过宏定义 OLED_MODE 来决定使用哪一个。如果 OLED_MODE=1，就定义为并口模式，选择第一个函数，而如果为 0，则为 4 线串口模式，选择第二个函数。这两个函数输入参数均为 2 个： dat 和 cmd， dat 为要写入的数据， cmd 则表明该数据是命令还是数据。这两个函数的时序操作就是根据上面我们对 8080接口以及 4 线 SPI 接口的时序来编写的。

### 2.4 OLED显示函数

x下面展示常见的OLED操作函数，这些代码厂家都会提供，我们适当理解就好了。最重要的是会调用这些函数

```c
//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}                        
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!      
void OLED_Clear(void)  
{  
    u8 i,n;  
    for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
    OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空                   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
    u8 pos,bx,temp=0;
    if(x>127||y>63)return;//超出范围了.
    pos=7-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)OLED_GRAM[x][pos]|=temp;
    else OLED_GRAM[x][pos]&=~temp;        
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63          
//dot:0,清空;1,填充      
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
    u8 x,y;  
    for(x=x1;x<=x2;x++)
    {
        for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
    }                                                        
    OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示                 
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{                      
    u8 temp,t,t1;
    u8 y0=y;
    u8 csize=(size/8+((size%8)?1:0))*(size/2);        //得到字体一个字符对应点阵集所占的字节数
    chr=chr-' ';//得到偏移后的值         
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206[chr][t];          //调用1206字体
        else if(size==16)temp=asc2_1608[chr][t];    //调用1608字体
        else if(size==24)temp=asc2_2412[chr][t];    //调用2412字体
        else return;                                //没有的字库
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)OLED_DrawPoint(x,y,mode);
            else OLED_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }       
    }          
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
    u32 result=1;     
    while(n--)result*=m;    
    return result;
}                  
//显示2个数字
//x,y :起点坐标     
//len :数字的位数
//size:字体大小
//mode:模式    0,填充模式;1,叠加模式
//num:数值(0~4294967295);               
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{             
    u8 t,temp;
    u8 enshow=0;                           
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
                continue;
            }else enshow=1; 

        }
         OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
    }
} 
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{    
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);     
        x+=size/2;
        p++;
    }  
}
```

### 2.5 主函数

```c
int main(void)
{
    u8 t = 0; 

    HAL_Init();                       //初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);      //设置时钟,168Mhz
    delay_init(168);                   //初始化延时函数
    uart_init(115200);                 //初始化USART
    LED_Init();                        //初始化LED    
    OLED_Init();                    //初始化OLED    
    OLED_ShowString(0,0,"ALIENTEK",24);  
    OLED_ShowString(0,24, "0.96' OLED TEST",16);  
     OLED_ShowString(0,40,"ATOM 2024/10/7",12);  
     OLED_ShowString(0,52,"ASCII:",12);  
     OLED_ShowString(64,52,"CODE:",12);  
    OLED_Refresh_Gram(); // 更新显示到OLED     
    t=' ';  

       while(1)
    {                                                         
        OLED_ShowChar(36,52,t,12,1); // 显示ASCII字符    
        OLED_ShowNum(94,52,t,3,12);     // 显示ASCII字符的码值    
        OLED_Refresh_Gram();         // 更新显示到OLED
        t++;
        if(t>'~')t=' ';  
        delay_ms(500);
        LED0=!LED0;;
    }
}
```

我们的主函数就主要是运用了下面这一些函数

```c
OLED_ShowString();  // 函数参数：起始行，起始列，显示字符串，字体大小
OLED_Refresh_Gram(); // 更新显示到OLED
OLED_ShowChar(); // 显示ASCII字符，参数：起始列，起始行，ASCII码，字体大小，显示模式
OLED_ShowNum();     // 显示ASCII字符的码值，参数：起始列，起始行，数字，数字长度，字体大小 
```

## 3. 小结

整个OLED实验主要就是学习8080和SPI了，我们其实只要在主函数内调用这些OLED函数就行了，下面我们复习一下：

### 3.1 8080与SPI的区别

#### 1. 数据传输方式

- **8080接口**：
  
  - 是一种并行通信协议，通常使用8位或16位的数据总线。
  - 数据通过多个引脚同时传输，适合在短距离内进行高速数据传输。

- **SPI（串行外设接口）**：
  
  - 是一种串行通信协议，通过单一数据线逐位传输数据。
  - 通常有4个主要引脚：MOSI（主设备输出从设备输入）、MISO（主设备输入从设备输出）、SCK（时钟线）、CS（片选线）。

#### 2. 引脚数量

- **8080接口**：
  
  - 需要较多的引脚，包括数据线和控制线（如读/写、片选等），通常需要10个以上的引脚。

- **SPI**：
  
  - 只需4到5个引脚，随着从设备数量的增加，可能需要多个片选引脚。

#### 3. 速度

- **8080接口**：
  
  - 由于并行传输，可以在短距离内提供较高的数据传输速率，但受限于电气特性和引脚数量。

- **SPI**：
  
  - 通常也能提供较高的速度，尤其是在较短的距离内，且支持全双工通信。

#### 4. 复杂性

- **8080接口**：
  
  - 由于涉及到多条数据线和控制信号，硬件设计和连接相对复杂。

- **SPI**：
  
  - 硬件连接较为简单，且实现相对容易，特别是在微控制器中广泛使用。

### 3.2 两者使用分别举例

#### 实验一：使用SPI接口驱动OLED显示器

##### 1. 实验目标

使用STM32通过SPI接口控制OLED显示器（如SSD1306），实现显示字符和图形。

##### 2. 硬件需求

- STM32开发板（如STM32F103C8T6）
- OLED显示模块（如SSD1306）
- 连接线

##### 3. 硬件连接

![屏幕截图 2024 10 06 111410](https://img.picgo.net/2024/10/06/-2024-10-06-1114108997f3b2cb43d844.png)

##### 4. 软件需求

- STM32 HAL库
- SSD1306 OLED驱动库（适用于SPI）

##### 5. 实验步骤

1. 初始化GPIO

初始化与OLED连接的GPIO引脚，包括DC、CS、RES、SCL、SDA。

```c
void GPIO_Init(void) {
    // 启用GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化控制引脚
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2; // DC, CS, RES
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 初始化SPI引脚
    // 具体代码依赖于你的SPI配置，假设使用SPI1
}
```

2. OLED初始化函数

编写OLED初始化代码：

```c
void OLED_Init(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // 复位
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

    OLED_WriteCommand(0xAE); // 关闭显示
    // 更多初始化命令...
    OLED_WriteCommand(0xAF); // 开启显示
}
```

3. 命令和数据发送函数

实现向OLED发送命令和数据的函数：

```c
void OLED_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // DC = 0
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS = 0
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // CS = 1
}

void OLED_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // DC = 1
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS = 0
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // CS = 1
}
```

4. 显示字符

编写函数以显示字符：

```c
void OLED_DisplayCharacter(char c) {
    uint8_t *font_ptr = getFontData(c); // 获取字体数据
    for (int i = 0; i < FONT_WIDTH; i++) {
        OLED_WriteData(font_ptr[i]); // 逐列发送字体数据
    }
}

void OLED_DisplayString(const char *str) {
    while (*str) {
        OLED_DisplayCharacter(*str++);
    }
}
```

5. 主程序

在主函数中调用初始化和显示函数：

```c
int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    OLED_Init();

    OLED_DisplayString("Hello, STM32!");

    while (1) {
        // 其他逻辑
    }
}
```

---

#### 实验二：使用8080接口驱动OLED显示器

##### 1. 实验目标

使用STM32通过8080接口控制OLED显示器，实现显示字符和图形。

##### 2. 硬件需求

- STM32开发板（如STM32F103C8T6）
- OLED显示模块（如SSD1306）
- 连接线

##### 3. 硬件连接

![屏幕截图 2024 10 06 111748](https://img.picgo.net/2024/10/06/-2024-10-06-1117486866f9b7ae41e653.png)

##### 4. 软件需求

- STM32 HAL库
- SSD1306 OLED驱动库（需修改以支持8080接口）

##### 5. 实验步骤

1. 初始化GPIO

初始化GPIO引脚：

```c
void GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化控制引脚
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4; // CS, DC, RST, WR, RD
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 初始化数据线
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8; // D0-D7
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
```

2. OLED初始化函数

初始化OLED：

```c
void OLED_Init(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // RST拉低
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // RST拉高
    // 发送初始化命令
    OLED_WriteCommand(0xAE); // 关闭显示
    // 更多初始化命令...
    OLED_WriteCommand(0xAF); // 开启显示
}
```

3. 命令和数据发送函数

发送命令和数据：

```c
void OLED_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // DC = 0
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS = 0
    // 发送命令到数据线
    // 假设使用某种方法将cmd写入D0-D7
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // CS = 1
}
void OLED_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // DC = 1
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS = 0
    // 发送数据到数据线
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // CS = 1
}
```

4. 显示字符

显示字符的函数：

```c
void OLED_DisplayCharacter(char c) {
    uint8_t *font_ptr = getFontData(c); // 获取字体数据
    for (int i = 0; i < FONT_WIDTH; i++) {
        OLED_WriteData(font_ptr[i]); // 逐列发送字体数据
    }
}

void OLED_DisplayString(const char *str) {
    while (*str) {
        OLED_DisplayCharacter(*str++);
    }
}
```

5. 主程序

主函数调用：

```c
int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    OLED_Init();

    OLED_DisplayString("Hello, STM32!");

    while (1) {
        // 其他逻辑
    }
}
```

---

2024.10.6 第一次修订，后期不再维护
