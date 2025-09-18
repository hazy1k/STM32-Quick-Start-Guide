# 第五十三章 LCD变量示例-整数浮点等

## 1. 导入

测试一下我们所写的LCD驱动函数显示不同字体和不同类型变量

## 2. 软件设计

### 2.1 编程大纲

1. 初始化LCD驱动

2. 显示变量测试

3. 居中显示测试

### 2.2 代码分析

#### 2.2.1 不同变量测试

```c
    /* 测试显示变量 */
    static uint8_t testIntCNT = 0;    
    static float testFloatCNT = 0;    
    char dispBuff[100];
    char *pStr = 0;
    testIntCNT++;    
    testFloatCNT += 0.1;
    LCD_SetFont(&Font8x16);    
    LCD_SetColors(RED,BLACK);
      ST7789V_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
    sprintf(dispBuff,"显示整数变量： %d ",testIntCNT);
    LCD_ClearLine(LINE(4));
    ST7789V_DispStringLine_EN_CH(LINE(4),dispBuff);
    sprintf(dispBuff,"显示浮点型变量： %f ",testFloatCNT);
      LCD_ClearLine(LINE(5));
    ST7789V_DispStringLine_EN_CH(LINE(5),dispBuff);
    sprintf(dispBuff,"浮点型(保留2位小数)： %.2f ",testFloatCNT);
      LCD_ClearLine(LINE(6));    
    ST7789V_DispStringLine_EN_CH(LINE(6),dispBuff);
```

#### 2.2.2 居中显示测试

```c
    /* 测试居中显示 */
    LCD_SetTextColor(GREEN);
    pStr = "插入2个英文空格示例";
    // 使用 %*c 在字符串前插入指定个数的英文空格
    sprintf(dispBuff,"%*c%s",2,' ',pStr);
      LCD_ClearLine(LINE(9));                     
    ST7789V_DispStringLine_EN_CH(LINE(9),dispBuff);
    ST7789V_DispStringLine_EN_CH(LINE(11),"纯英文24x32居中示例：");
    LCD_SetFont(&Font24x32);    
    pStr = "ABCDEF";
    // 居中时，要插入的空格个数 = (液晶宽度/单个字体宽度 - 字符串长度)/2
    sprintf(dispBuff,"%*c%s",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width))-strlen(pStr))/2,' ',pStr);
    LCD_ClearLine(LINE(6));    
    ST7789V_DispStringLine_EN_CH(LINE(6),dispBuff);
    LCD_SetFont(&Font8x16);    
    pStr = "中文居中示例";
    // 居中时，要插入的空格个数 = (液晶宽度/字体宽度 - 字符串长度)/2
    // strlen计算长度时，一个中文等于2个字节，即2个英文字符，而且插入的是英文空格
    // 所以用(WIDTH_CH_CHAR/2)来计算字体宽度
    sprintf(dispBuff,"%*c%s",(LCD_X_LENGTH/(WIDTH_CH_CHAR/2)-strlen(pStr))/2,' ',pStr);
      LCD_ClearLine(LINE(15));                     
    ST7789V_DispStringLine_EN_CH(LINE(15),dispBuff);
    pStr = "含英文居中示例ABCDEFG";
    sprintf(dispBuff,"%*c%s",(LCD_X_LENGTH/(WIDTH_CH_CHAR/2)-strlen(pStr))/2,' ',pStr);
    LCD_ClearLine(LINE(16));                                                 
    ST7789V_DispStringLine_EN_CH(LINE(16),dispBuff);
    Delay(0xffffff);
```

#### 2.2.3 整体示例

```c
#include "stm32f10x.h"
#include "usart.h"    
#include "lcd.h"
#include "flash.h"
#include <stdio.h>
#include <string.h>

static void LCD_Test(void);    
static void Delay(__IO uint32_t nCount);

int main(void)
{    
    ST7789V_Init();         
    USART_Config();  
      ST7789V_GramScan(6);
    while(1)
    {
        LCD_Test();
    }
}

void LCD_Test(void)
{
    /* 测试显示变量 */
    static uint8_t testIntCNT = 0;    
    static float testFloatCNT = 0;    
    char dispBuff[100];
    char *pStr = 0;
    testIntCNT++;    
    testFloatCNT += 0.1;
    LCD_SetFont(&Font8x16);    
    LCD_SetColors(RED,BLACK);
      ST7789V_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
    sprintf(dispBuff,"显示整数变量： %d ",testIntCNT);
    LCD_ClearLine(LINE(4));
    ST7789V_DispStringLine_EN_CH(LINE(4),dispBuff);
    sprintf(dispBuff,"显示浮点型变量： %f ",testFloatCNT);
      LCD_ClearLine(LINE(5));
    ST7789V_DispStringLine_EN_CH(LINE(5),dispBuff);
    sprintf(dispBuff,"浮点型(保留2位小数)： %.2f ",testFloatCNT);
      LCD_ClearLine(LINE(6));    
    ST7789V_DispStringLine_EN_CH(LINE(6),dispBuff);
    /* 测试居中显示 */
    LCD_SetTextColor(GREEN);
    pStr = "插入2个英文空格示例";
    // 使用 %*c 在字符串前插入指定个数的英文空格
    sprintf(dispBuff,"%*c%s",2,' ',pStr);
      LCD_ClearLine(LINE(9));                     
    ST7789V_DispStringLine_EN_CH(LINE(9),dispBuff);
    ST7789V_DispStringLine_EN_CH(LINE(11),"纯英文24x32居中示例：");
    LCD_SetFont(&Font24x32);    
    pStr = "ABCDEF";
    // 居中时，要插入的空格个数 = (液晶宽度/单个字体宽度 - 字符串长度)/2
    sprintf(dispBuff,"%*c%s",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width))-strlen(pStr))/2,' ',pStr);
    LCD_ClearLine(LINE(6));    
    ST7789V_DispStringLine_EN_CH(LINE(6),dispBuff);
    LCD_SetFont(&Font8x16);    
    pStr = "中文居中示例";
    // 居中时，要插入的空格个数 = (液晶宽度/字体宽度 - 字符串长度)/2
    // strlen计算长度时，一个中文等于2个字节，即2个英文字符，而且插入的是英文空格
    // 所以用(WIDTH_CH_CHAR/2)来计算字体宽度
    sprintf(dispBuff,"%*c%s",(LCD_X_LENGTH/(WIDTH_CH_CHAR/2)-strlen(pStr))/2,' ',pStr);
      LCD_ClearLine(LINE(15));                     
    ST7789V_DispStringLine_EN_CH(LINE(15),dispBuff);
    pStr = "含英文居中示例ABCDEFG";
    sprintf(dispBuff,"%*c%s",(LCD_X_LENGTH/(WIDTH_CH_CHAR/2)-strlen(pStr))/2,' ',pStr);
    LCD_ClearLine(LINE(16));                                                 
    ST7789V_DispStringLine_EN_CH(LINE(16),dispBuff);
    Delay(0xffffff);
}

static void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount --);    
}
```
