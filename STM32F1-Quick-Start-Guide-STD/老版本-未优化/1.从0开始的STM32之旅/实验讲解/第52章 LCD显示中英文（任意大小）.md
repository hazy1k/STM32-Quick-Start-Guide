# 第五十二章 LCD显示中英文-任意大小

## 1. 导入

前文中无论是ASCII字符还是GB2312的字符，都只能显示字库中设定的字体大小，例如，我们想显示一些像素大小为48x48的字符， 那我们又得制作相应的字库，非常麻烦。为此我们编写了一些函数，简便地实现显示任意大小字符的目的。

## 2. 软件设计

### 2.1 编程大纲

1. 编写缩放字模数据的函数

2. 编写利用缩放字模的结果进行字符显示的函数

3. 编写测试代码，控制显示不同大小的字符

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 缩放字模数据

显示任意大小字符的功能，其核心是缩放字模，通过LCD_zoomChar函数对原始字模数据进行缩放

```c
/* 缩放字体 */
#define ZOOMMAXBUFF 16384
uint8_t zoomBuff[ZOOMMAXBUFF] = {0}; // 用于缩放的缓存，最大支持到128*128
uint8_t zoomTempBuff[1024] = {0};
// 缩放字模，缩放后的字模由1个像素点由8个数据位来表示,0x01表示笔迹，0x00表示空白区
void ST7789V_zoomChar(uint16_t in_width,    // 原始字符宽度
                      uint16_t in_heig,        // 原始字符高度
                      uint16_t out_width,    // 缩放后的字符宽度
                      uint16_t out_heig,    // 缩放后的字符高度
                      uint8_t *in_ptr,        // 字库输入指针    注意：1pixel 1bit
                      uint8_t *out_ptr,     // 缩放后的字符输出指针 注意: 1pixel 8bit
                      uint8_t en_cn)        // 0为英文，1为中文    
{
    uint8_t *pts,*ots;
    // 根据源字模及目标字模大小，设定运算比例因子，左移16是为了把浮点运算转成定点运算
    unsigned int xrIntFloat_16=(in_width<<16)/out_width+1; 
    unsigned int yrIntFloat_16=(in_heig<<16)/out_heig+1;
    unsigned int srcy_16=0;
    unsigned int y,x;
    uint8_t *pSrcLine;
    uint16_t byteCount,bitCount;
    // 检查参数是否合法
    if(in_width >= 32) return;                                                
    if(in_width * in_heig == 0) return;    
    if(in_width * in_heig >= 1024 ) return;                 
    if(out_width * out_heig == 0) return;    
    if(out_width * out_heig >= ZOOMMAXBUFF) return; 
    pts = (uint8_t*)&zoomTempBuff;
    // 为方便运算，字库的数据由1 pixel/1bit 映射到1pixel/8bit
    // 0x01表示笔迹，0x00表示空白区
    if(en_cn == 0x00) // 英文
    {
        // 英文和中文字库上下边界不对，可在此处调整。需要注意tempBuff防止溢出
        for(byteCount=0;byteCount<in_heig*in_width/8;byteCount++)    
        {
            for(bitCount=0;bitCount<8;bitCount++)
            {                        
                // 把源字模数据由位映射到字节
                // in_ptr里bitX为1，则pts里整个字节值为1
                // in_ptr里bitX为0，则pts里整个字节值为0
                *pts++ = (in_ptr[byteCount] & (0x80>>bitCount))?1:0; 
            }
        }                
    }
    else // 中文
    {            
        for(byteCount=0;byteCount<in_heig*in_width/8;byteCount++)    
        {
            for(bitCount=0;bitCount<8;bitCount++)
            {                        
                *pts++ = (in_ptr[byteCount] & (0x80>>bitCount))?1:0; 
            }
        }        
    }
    // zoom过程
    pts = (uint8_t*)&zoomTempBuff;// 映射后的源数据指针
    ots = (uint8_t*)&zoomBuff;      // 输出数据的指针
    for (y=0;y<out_heig;y++)    /*行遍历*/
    {
        unsigned int srcx_16=0;
        pSrcLine=pts+in_width*(srcy_16>>16);                
        for(x=0;x<out_width;x++) /*行内像素遍历*/
        {
            ots[x]=pSrcLine[srcx_16>>16]; // 把源字模数据复制到目标指针中
            srcx_16+=xrIntFloat_16;          // 按比例偏移源像素点
        }
        srcy_16+=yrIntFloat_16;    // 按比例偏移源像素点
        ots+=out_width;                        
    }
    /* 缩放后的字模数据直接存储到全局指针zoomBuff里了 */
    out_ptr = (uint8_t*)&zoomBuff;    // out_ptr没有正确传出，后面调用直接改成了全局变量指针！
    out_ptr++; 
}            
```

缩放字模的本质是按照缩放比例，减少或增加矩阵中的像素点，只要把左侧的矩阵隔一行、 隔一列地取出像素点，即可得到右侧按比例缩小了的矩阵，而右侧的小矩阵按比例填复制像素点即可得到左侧放大的矩阵，上述函数就是完成了这样的工作。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCDdis009.jpeg" title="" alt="" width="487">

该函数的说明如下：

(1) 输入参数

> 函数包含输入参数源字模、缩放后字模的宽度及高度：in_width、inheig、out_width、out_heig。源字模数据指针in_ptr， 缩放后的字符指针out_ptr以及用于指示字模是英文还是中文的标志en_cn。其中out_ptr指针实质上没有用到，这个函数缩放后的数据最后直接存储在全局变量zoomBuff中了。

(2) 计算缩放比例

> 根据输入字模与要求的输出字模大小，计算出缩放比例到xrIntFloat_16及yrIntFloat_16变量中，运算式中的左移16位是典型的把浮点型运算转换成定点运算的处理方式。理解的时候可把左移16位的运算去掉，把它当成一个自然的数学小数运算即可。

(3) 检查输入参数

> 由于运算变量及数组的一些限制，函数中要检查输入参数的范围，本函数限制最大输出字模的大小为128*128像素，输入字模限制不可以超过32x32像素。

(4) 映射字模

> 输入源的字模都是1个数据位表示1个像素点的，为方便后面的运算，函数把输入字模转化成1个字节(8个数据位)表示1个像素点，该字节的值为0x01表示笔迹像素，0x00表示空白像素。把字模数据的1个数据位映射为1个字节，可以方便后面直接使用指针和数组索引运算。

(5) 缩放字符

> 缩放字符这部分代码比较难理解，但总的来说它就是利用前面计算得的比例因子，以它为步长复制源字模的数据到目标字模的缓冲区中， 具体的抽象运算只能意会了。其中的右移16位是把比例因子由定点数转换回原始的数值。如果还是觉得难以理解， 可以把函数的宽度及高度输入参数in_width、inheig、out_width及out_heig都设置成16，然后代入运算来阅读这段代码。

(6) 缩放结果

> 经过运算，缩放的结果存储在zoomBuff中，它只是存储了一个字模的缩放结果，所以每显示一个字模都需要先调用这个函数更新zoomBuff中的字模数据，而且它也是用1个字节表示1个像素位的。

#### 2.2.2 利用缩放的字模数据显示字符

由于缩放后的字模数据格式与我们原来用的字模数据格式不一样，所以我们也要重新编写字符显示函数

```c
// 利用缩放后的字模显示字符
void ST7789V_DrawChar_Ex(uint16_t usX,        // 字符显示位置x
                         uint16_t usY,        // 字符显示位置y
                         uint16_t Font_width, // 字符宽度
                         uint16_t Font_Height,// 字符高度 
                         uint8_t *c,          // 字模数据
                         uint16_t DrawModel)  // 是否反色显示
{
    uint32_t index = 0, counter = 0;
    ST7789V_OpenWindow(usX, usY, Font_width, Font_Height);
    ST7789V_Write_Cmd(CMD_SetPixel);        
    // 按字节读取字模数据
    for(index = 0; index < Font_Height; index++)
    {
        // 一位一位处理要显示的颜色
        for(counter = 0; counter < Font_width; counter++)
        {
            if(*c++ == DrawModel)
            {
                ST7789V_Write_Data(CurrentBackColor);
            }            
            else
            {
                ST7789V_Write_Data(CurrentTextColor);
            }
        }    
    }    
}el);
            x+=Charwidth;
            ptr++;
        }
    }
}
```

注意在这个函数中，它并没有对中英文模区分显示代码，因为本函数的字模是由输入参数c指针中获取的，在调用本函数时， 需要输入要显示的字模数据指针，而不是字符编码。在其它方面这个函数主体与前面介绍的字符显示函数都很类似， 只是它在判断字模数据位的时候，直接用一整个字节来判断，区分显示分支，而且还支持了反色显示模式

#### 2.2.3 利用缩放的字模显示字符串

单个字符显示的函数并不包含字模的获取过程，为便于使用，我们把它直接封装成字符串显示函数

```c
// 利用缩放后的字模显示字符串
void ST7789V_DisplayStringEx(uint16_t x, uint16_t y, uint16_t Font_width, uint16_t Font_Height,    uint8_t *ptr, uint16_t DrawModel)
{
    uint16_t Charwidth = Font_width; // 默认为Font_width，英文宽度为中文宽度的一半
    uint8_t *psr;
    uint8_t Ascii;    // 英文
    uint16_t usCh;  // 中文
    uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];    
    while(*ptr != '\0')
    {
        if((x - ST7789V_X_Star + Charwidth) > LCD_X_LENGTH)
            {
                x = ST7789V_X_Star;
                y += Font_Height;
            }
        if((y - ST7789V_Y_Star + Font_Height) > LCD_Y_LENGTH)
        {
            x = ST7789V_X_Star;
            y = ST7789V_Y_Star;
        }        
        if(*ptr > 0x80) // 如果是中文
        {            
            Charwidth = Font_width;
            usCh = *(uint16_t*)ptr;                
            usCh = (usCh << 8) + (usCh >> 8);
            GetGBKCode(ucBuffer, usCh);    // 取字模数据
            // 缩放字模数据，源字模为16*16
            ST7789V_zoomChar(WIDTH_CH_CHAR,HEIGHT_CH_CHAR,Charwidth,Font_Height,(uint8_t *)&ucBuffer,psr,1); 
            //显示单个字符
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            x+=Charwidth;
            ptr+=2;
        }
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32;
            // 使用16*24字体缩放字模数据
            ST7789V_zoomChar(16,24,Charwidth,Font_Height,(uint8_t *)&Font16x24.table[Ascii * Font16x24.Height*Font16x24.Width/8],psr,0);
            //显示单个字符
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            x+=Charwidth;
            ptr++;
        }
    }
}
// 利用缩放后的字模显示字符串(沿Y轴方向)
void ST7789V_DisplayStringEx_YDir(uint16_t x, uint16_t y, uint16_t Font_width, uint16_t Font_Height, uint8_t *ptr, uint16_t DrawModel)  
{
    uint16_t Charwidth = Font_width; 
    uint8_t *psr;
    uint8_t Ascii;    
    uint16_t usCh;  
    uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];    
    while(*ptr != '\0')
    {            
        // 统一使用汉字的宽高来计算换行
        if((y - ST7789V_X_Star + Font_width) > LCD_X_LENGTH)
        {
            y = ST7789V_X_Star;
            x += Font_width;
        }
        if(( x - ST7789V_Y_Star + Font_Height) > LCD_Y_LENGTH)
        {
            y = ST7789V_X_Star;
            x = ST7789V_Y_Star;
        }        
        if(*ptr > 0x80)
        {            
            Charwidth = Font_width;
            usCh =* (uint16_t*)ptr;                
            usCh = (usCh << 8) + (usCh >> 8);
            GetGBKCode(ucBuffer, usCh);
            ST7789V_zoomChar(WIDTH_CH_CHAR,HEIGHT_CH_CHAR,Charwidth,Font_Height,(uint8_t *)&ucBuffer,psr,1); 
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            y+=Font_Height;
            ptr+=2;
        }
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32;
            ST7789V_zoomChar(16,24,Charwidth,Font_Height,(uint8_t *)&Font16x24.table[Ascii * Font16x24.Height*Font16x24.Width/8],psr,0);
            ST7789V_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
            y+=Font_Height;
            ptr++;
        }
    }
}
```

这个函数包含了从字符编码到源字模获取、字模缩放及单个字符显示的过程，多个这样的过程组合起来，就实现了简单易用的字符串显示函数。 而且可了解到它使用的英文源字模数据是Font16x24字体，而中文源字模数据仍是采用GetGBKCode函数获取，使得数据源的获取与上层分离， 支持从SPIFLASH及SD卡中获取数据源。

#### 2.2.4 利用缩放的字模显示示例

利用缩放的字模显示时，液晶的初始化过程与前面的工程无异，以下我们给出LCD_Test函数中调用字符串函数显示不同字符时的示例

```c
#include "stm32f10x.h"
#include "lcd.h"
#include "flash.h"
#include <stdio.h>
#include "usart.h"

static void LCD_Test(void);    
static void Delay (__IO uint32_t nCount);

int main(void)
{
    USART_Config();
    ST7789V_Init(); 
    ST7789V_GramScan(6);
    while(1)
    {
        LCD_Test();
    }
}

extern uint16_t lcdid;

void LCD_Test(void)
{
    static uint8_t testCNT = 0;    
    char dispBuff[100];
    testCNT++;
    LCD_SetFont(&Font8x16); 
    LCD_SetColors(RED, BLACK); 
    ST7789V_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);

    /* 字体测试 */
    ST7789V_DispStringLine_EN_CH(LINE(0), "字体测试");
    ST7789V_DisplayStringEx(0, 1*24, 24, 24, (uint8_t*)"字体测试", 0);
    ST7789V_DisplayStringEx(2*48, 0*48, 48, 48, (uint8_t*)"字体测试", 0);

    LCD_SetTextColor(GREEN);
    sprintf(dispBuff, "变量测试: %d", testCNT); 
    LCD_ClearLine(LINE(5));
    ST7789V_DispStringLine_EN_CH(LINE(5), dispBuff);
    /* 图形测试 */
    LCD_ClearLine(LINE(7));
    LCD_SetTextColor(BLUE);
    ST7789V_DispStringLine_EN_CH(LINE(7), "画直线");  
    LCD_SetTextColor(RED);
    ST7789V_DrawLine(50, 170, 210, 230);  
    ST7789V_DrawLine(50, 200, 210, 240);
    LCD_SetTextColor(GREEN);
    ST7789V_DrawLine(100, 170, 200, 230);  
    ST7789V_DrawLine(200, 200, 220, 240);
    LCD_SetTextColor(BLUE);
    ST7789V_DrawLine(110, 170, 110, 230);  
    ST7789V_DrawLine(130, 200, 220, 240);
    Delay(0xFFFFFF);
    ST7789V_Clear(0, 16*8, LCD_X_LENGTH, LCD_Y_LENGTH - 16*8);
    LCD_ClearLine(LINE(7));
    LCD_SetTextColor(BLUE);
    ST7789V_DispStringLine_EN_CH(LINE(7), "画矩形");   
    LCD_SetTextColor(RED);
    ST7789V_DrawRectangle(50, 200, 100, 30, 1);
    LCD_SetTextColor(GREEN);
    ST7789V_DrawRectangle(160, 200, 20, 40, 0);
    LCD_SetTextColor(BLUE);
    ST7789V_DrawRectangle(170, 200, 50, 20, 1);
    Delay(0xFFFFFF);
    ST7789V_Clear(0, 16*8, LCD_X_LENGTH, LCD_Y_LENGTH - 16*8);

    LCD_ClearLine(LINE(7));
    LCD_SetTextColor(BLUE);
    ST7789V_DispStringLine_EN_CH(LINE(7),"画圆");   
    LCD_SetTextColor(RED);
    ST7789V_DrawCircle(100, 200, 20, 0);
    LCD_SetTextColor(GREEN);
    ST7789V_DrawCircle(100, 200, 10, 1);    
    LCD_SetTextColor(BLUE);
    ST7789V_DrawCircle(140, 200, 20, 0);
    Delay(0xFFFFFF);
    ST7789V_Clear(0, 16*8, LCD_X_LENGTH, LCD_Y_LENGTH - 16*8);
}

static void Delay(__IO uint32_t nCount) 
{
    for(; nCount != 0; nCount --);
}
```

---

2024.9.29 第一次修订，后期不再维护

2025.2.6 简化程序
