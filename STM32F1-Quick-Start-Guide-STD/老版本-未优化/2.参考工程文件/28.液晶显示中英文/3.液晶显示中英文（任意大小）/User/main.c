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
    
    /* ◊÷ÃÂ≤‚ ‘ */
    ST7789V_DispStringLine_EN_CH(LINE(0), "◊÷ÃÂ≤‚ ‘");
    ST7789V_DisplayStringEx(0, 1*24, 24, 24, (uint8_t*)"◊÷ÃÂ≤‚ ‘", 0);
    ST7789V_DisplayStringEx(2*48, 0*48, 48, 48, (uint8_t*)"◊÷ÃÂ≤‚ ‘", 0);

    LCD_SetTextColor(GREEN);
    sprintf(dispBuff, "±‰¡ø≤‚ ‘: %d", testCNT); 
    LCD_ClearLine(LINE(5));
    ST7789V_DispStringLine_EN_CH(LINE(5), dispBuff);
    /* Õº–Œ≤‚ ‘ */
    LCD_ClearLine(LINE(7));
    LCD_SetTextColor(BLUE);
    ST7789V_DispStringLine_EN_CH(LINE(7), "ª≠÷±œﬂ");  
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
    ST7789V_DispStringLine_EN_CH(LINE(7), "ª≠æÿ–Œ");   
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
    ST7789V_DispStringLine_EN_CH(LINE(7),"ª≠‘≤");   
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
