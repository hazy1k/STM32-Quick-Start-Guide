# 第十一章 USMART调试组件介绍及应用

## 1. USMART 调试组件简介

USMART 是由正点原子开发的一个灵巧的串口调试互交组件，通过它你可以通过串口助手调用程序里面的任何函数，并执行。因此，你可以随意更改函数的输入参数（支持数字（10/16进制， 支持负数)、字符串、函数入口地址等作为参数），单个函数最多支持 10 个输入参数，并支持函数返回值显示，目前最新版本为 V3.5。

USMART 的特点如下：

1. 可以调用绝大部分用户直接编写的函数。

2. 资源占用极少（最少情况： FLASH:4K； SRAM:72B）。

3. 支持参数类型多（数字（包含 10/16 进制，支持负数）、字符串、函数指针等）。

4. 支持函数返回值显示。

5. 支持参数及返回值格式设置。

6. 支持函数执行时间计算（V3.1 及以后的版本新特性）。

有了 USMART，你可以轻易的修改函数参数、查看函数运行结果，从而快速解决问题。比如你调试一个摄像头模块，需要修改其中的几个参数来得到最佳的效果，普通的做法：写函数→修改参数→下载→看结果→不满意→修改参数→下载→看结果→不满意….不停的循环，直到满意为止。这样做很麻烦不说，单片机也是有寿命的啊，老这样不停的刷，很折寿的。而利用USMART，则只需要在串口调试助手里面输入函数及参数，然后直接串口发送给单片机，就执行了一次参数调整，不满意的话，你在串口调试助手修改参数在发送就可以了，直到你满意为止。这样，修改参数十分方便，不需要编译、不需要下载、不会让单片机折寿。

USMART 支持的参数类型基本满足任何调试了，支持的类型有： 10 或者 16 进制数字、字符串指针（如果该参数是用作参数返回的话，可能会有问题！）、函数指针等。因此绝大部分函数，可以直接被 USMART 调用，对于不能直接调用的，你只需要重写一个函数，把影响调用的参数去掉即可，这个重写后的函数，即可以被 USMART 调用了。

USMART 的实现流程简单概括就是：第一步，添加需要调用的函数（在 usmart_config.c 里面的 usmart_nametab 数组里面添加）；第二步，初始化串口；第三步，初始化 USMART（通过usmart_init 函数实现）；第四步，轮询 usmart_scan 函数，处理串口数据。

接下来我们看下 USMART 的组成， USMART 组件总共包含 8 个文件

![屏幕截图 2025-07-31 162057.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/31-16-21-11-屏幕截图%202025-07-31%20162057.png)

## 2. USMART 调试组件使用

需要在 usmart_config.c 文件里面添加想要被 USMART 调用的函数。

```c
#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* 用户配置区
 * 这下面要包含所用到的函数所申明的头文件(用户自己添加)
 */

#include "sys.h"
#include "delay.h"
#include "lcd.h"


/* 函数名列表初始化(用户自己添加)
 * 用户直接在这里输入要执行的函数名及其查找串
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* 如果使能了读写操作 */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
#endif
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",
        
    (void *)LCD_Clear, "void LCD_Clear(uint32_t color)",
    (void *)LCD_Fill, "void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)",
    (void *)LCD_DrawLine, "void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)",
    (void *)LCD_Draw_Circle, "void LCD_Draw_Circle(u16 x0,u16 y0,u8 r); ",
    (void *)LCD_DrawRectangle, "void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)",
        
    (void *)LCD_ShowNum, "void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)",
    (void *)LCD_ShowxNum, "void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)",
    (void *)LCD_ShowString, "void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)",
    (void *)LCD_DrawPoint, "void LCD_DrawPoint(u16 x,u16 y)",
    (void *)LCD_ReadPoint, "u32  LCD_ReadPoint(u16 x,u16 y)",
    (void *)LCD_Scan_Dir, "void LCD_Scan_Dir(u8 dir)",
};

/******************************************************************************************/

/* 函数控制管理器初始化
 * 得到各个受控函数的名字
 * 得到函数总数量
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* 函数数量 */
    0,      /* 参数数量 */
    0,      /* 函数ID */
    1,      /* 参数显示类型,0,10进制;1,16进制 */
    0,      /* 参数类型.bitx:,0,数字;1,字符串 */
    0,      /* 每个参数的长度暂存表,需要MAX_PARM个0初始化 */
    0,      /* 函数的参数,需要PARM_LEN个0初始化 */
};

```

这里的添加函数很简单，只要把函数所在头文件添加进来，并把函数名按上图所示的方式增加即可，默认我们添加了两个函数： delay_ms 和 delay_us。另外， read_addr 和 write_addr 属于 usmart 自带的函数，用于读写指定地址的数据，通过配置 USMART_USE_WRFUNS 宏定义，可以使能或者禁止这两个函数。

这里我们根据自己的需要按上图的格式添加其他函数， usmart_config.c 文件中我们已经添加了 LCD 的相关函数

```c
#include "bsp_init.h" 
#include "lcd.h"      
#include "stdio.h"    
#include "delay.h"    
#include "usmart.h"

int main(void)
{
	bsp_init();
	usmart_dev.init(84); 
	LCD_Init();
	LCD_ShowString(30, 50, 200, 16, 16, "LCD TEST");
	while(1)
	{
		LED_TOGGLE(LED0_GPIO_Pin);
		delay_ms(1000);
	}
}

```

将程序下载到开发板后，可以看到 LED0 不停的闪烁，提示程序已经在运行了。 同时，屏幕上显示了一些字符（就是主函数里面要显示的字符）。


