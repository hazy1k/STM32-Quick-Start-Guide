#include "bsp_init.h" 
#include "lcd.h"      
#include "stdio.h"    
#include "delay.h"    

// 定义一些测试用的颜色
#define TEST_RED      0xF800
#define TEST_GREEN    0x07E0
#define TEST_BLUE     0x001F
#define TEST_YELLOW   0xFFE0
#define TEST_CYAN     0x7FFF
#define TEST_MAGENTA  0xF81F
#define TEST_WHITE    0xFFFF
#define TEST_BLACK    0x0000

// Main函数
int main(void)
{
    u8 test_stage = 0; 
    char lcd_id_str[16];   
    char resolution_str[30]; 
    
    bsp_init(); 
    LCD_Init();           			
    sprintf(lcd_id_str, "LCD ID:%04X", lcddev.id);

    while(1) 
    {		 
        switch(test_stage % 4) 
        {
            case 0: BACK_COLOR = WHITE;   break;
            case 1: BACK_COLOR = GREEN;   break;
            case 2: BACK_COLOR = BLUE;    break;
            case 3: BACK_COLOR = RED;     break;
        }
        LCD_Clear(BACK_COLOR); 
        
        POINT_COLOR = BLACK; 
        
        sprintf(resolution_str, "Current W:H = %d:%d", lcddev.width, lcddev.height);
        LCD_ShowString(10, 10, lcddev.width - 20, 24, 24, (u8*)"Explorer STM32F4");	
        LCD_ShowString(10, 40, lcddev.width - 20, 16, 16, (u8*)resolution_str); 
        LCD_ShowString(10, 60, lcddev.width - 20, 16, 16, (u8*)"HAZY1K");
        LCD_ShowString(10, 80, lcddev.width - 20, 16, 16, (u8*)lcd_id_str); 	      					 
        LCD_ShowString(10, 100, lcddev.width - 20, 12, 12, (u8*)"2025/7/30 (Updated)"); 

        switch(test_stage)
        {
            case 0: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 0: Basic Info Display");
                break;
            case 1: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 1: Draw Points Test");
                POINT_COLOR = TEST_BLUE;
                for (u16 i = 0; i < 50; i++)
                {
                    if((i + 10) < lcddev.width && (i + 150) < lcddev.height) 
                        LCD_DrawPoint(i + 10, i + 150);
                    if((i + 10) < lcddev.width && (200 - i) < lcddev.height) 
                        LCD_Fast_DrawPoint(i + 10, 200 - i, TEST_RED);
                }
                break;
           case 2: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 2: Draw Lines Test");
                POINT_COLOR = TEST_GREEN;
                LCD_DrawLine(10, 150, 150, 200);
                LCD_DrawLine(10, 200, 150, 150);
                LCD_DrawLine(80, 150, 80, 250);
                LCD_DrawLine(10, 220, 200, 220);
                break;
            case 3: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 3: Draw Rects Test");
                POINT_COLOR = TEST_MAGENTA;
                LCD_DrawRectangle(20, 150, 120, 250); 
                
                POINT_COLOR = TEST_CYAN;
                LCD_Fill(150, 160, 250, 210, TEST_YELLOW); 
                break; 
            case 4: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 4: Draw Circles Test");
                POINT_COLOR = TEST_BLUE;
                LCD_Draw_Circle(80, 200, 40); 
                POINT_COLOR = TEST_GREEN;
                LCD_Draw_Circle(200, 180, 20); 
                break;
            case 5: 
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 5: Chars & Nums Test");
                POINT_COLOR = TEST_RED;
                LCD_ShowChar(10, 160, 'G', 32, 0);
                LCD_ShowChar(40, 160, 'A', 32, 0);
                LCD_ShowChar(70, 160, 'M', 32, 0);
                LCD_ShowChar(100, 160, 'E', 32, 0);
                LCD_ShowNum(10, 200, 12345, 5, 24);
                LCD_ShowxNum(10, 230, 007, 3, 24, 0X80);
                LCD_ShowxNum(10, 260, 987654321, 9, 16, 0);
                break;
            case 6: // 字符串显示和屏幕旋转测试
                LCD_ShowString(10, 130, lcddev.width - 20, 16, 16, (u8*)"Stage 6: String & Rotate Test");
                POINT_COLOR = BLACK;
                LCD_ShowString(10, 300, lcddev.width - 20, 50, 16, (u8*)"Testing string wrapping here with a long sentence.");
                delay_ms(1000); 
                
                // 屏幕旋转到横屏
                // 修改这一行，尝试不同的扫描方向来解决镜像问题
                // 原始: LCD_Display_Dir(1); // 内部调用 U2D_L2R
                // 尝试新的方向
                lcddev.dir = 1; // 横屏模式
                lcddev.wramcmd = 0X2C;
                lcddev.setxcmd = 0X2A;
                lcddev.setycmd = 0X2B;  
                // 这里是关键的改变：
                LCD_Scan_Dir(U2D_R2L); // 尝试使用 U2D_R2L (从上到下，从右到左，行列交换)
                // 或者你也可以尝试 D2U_L2R (从下到上，从左到右，行列交换)
                // LCD_Scan_Dir(D2U_L2R); 
                
                LCD_Clear(MAGENTA); 
                POINT_COLOR = WHITE;
                sprintf(resolution_str, "H_Mode W:H = %d:%d", lcddev.width, lcddev.height); 
                LCD_ShowString(10, 10, lcddev.width - 20, 50, 24, (u8*)"Horizontal Mode!");
                LCD_ShowString(10, 40, lcddev.width - 20, 16, 16, (u8*)resolution_str);
                LCD_ShowString(10, 60, lcddev.width - 20, 16, 16, (u8*)lcd_id_str);
                delay_ms(1000); 
                
                // 切换回竖屏
                LCD_Display_Dir(0); 
                break; 
            default: 
                test_stage = 0;
                continue; 
        }
        test_stage++; 
        delay_ms(1000);
    } 
}
