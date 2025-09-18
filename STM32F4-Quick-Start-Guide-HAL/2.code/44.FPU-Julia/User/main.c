#include "bsp_init.h"

uint8_t timeout;

/* FPU模式提示 */
#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On"
#else
#define SCORE_FPU_MODE                  "FPU Off"
#endif

#define     ITERATION           128     /* 迭代次数 */
#define     REAL_CONSTANT       0.285f  /* 实部常量 */
#define     IMG_CONSTANT        0.01f   /* 虚部常量 */

/* 颜色表 */
uint16_t g_color_map[ITERATION];

/* 缩放因子列表 */
const uint16_t zoom_ratio[] =
{
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110,
};

/**
 * @brief       初始化颜色表
 * @param       clut     : 颜色表指针
 * @retval      无
 */
void julia_clut_init(uint16_t *clut)
{
    uint32_t i = 0x00;
    uint16_t  red = 0, green = 0, blue = 0;

    for (i = 0; i < ITERATION; i++) /* 产生颜色表 */
    {
        /* 产生RGB颜色值 */
        red = (i * 8 * 256 / ITERATION) % 256;
        green = (i * 6 * 256 / ITERATION) % 256;
        blue = (i * 4 * 256 / ITERATION) % 256;
        
        /* 将RGB888,转换为RGB565 */
        red = red >> 3;
        red = red << 11;
        green = green >> 2;
        green = green << 5;
        blue = blue >> 3;
        clut[i] = red + green + blue;
    }
}

/* RGB LCD 缓存*/
uint16_t g_lcdbuf[800];

/**
 * @brief        产生Julia分形图形
 * @param       size_x   : 屏幕x方向的尺寸
 * @param       size_y   : 屏幕y方向的尺寸
 * @param       offset_x : 屏幕x方向的偏移
 * @param       offset_y : 屏幕y方向的偏移
 * @param       zoom     : 缩放因子
 * @retval      无
 */
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
    uint8_t i;
    uint16_t x, y;
    float tmp1, tmp2;
    float num_real, num_img;
    float radius;

    for (y = 0; y < size_y; y++)
    {
        for (x = 0; x < size_x; x++)
        {
            num_real = y - offset_y;
            num_real = num_real / zoom;
            num_img = x - offset_x;
            num_img = num_img / zoom;
            i = 0;
            radius = 0;

            while ((i < ITERATION - 1) && (radius < 4))
            {
                tmp1 = num_real * num_real;
                tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                radius = tmp1 + tmp2;
                i++;
            }
            LCD->LCD_RAM = g_color_map[i]; /* 绘制到屏幕 */
        }
    }
}

int main(void)
{
    uint8_t key_value = 0;
    uint8_t i = 0;
    uint8_t autorun = 0;
    float time;
    char buf[50];
    bsp_init();
	LCD_ShowString(30,70,200,16,16,"FPU TEST");
    LCD_ShowString(30,110,200,16,16,"KEY0:+    KEY1:-");
    LCD_ShowString(30,130,200,16,16,"KEY_UP:AUTO/MANUL");
    delay_ms(500);
    julia_clut_init(g_color_map);
    while(1)
	{
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_Press:
                i++;
                if(i > sizeof(zoom_ratio)/2-1)
                    i = 0;
                break;
            case KEY1_Press:
                if(i)
                    i--;
                else
                    i = sizeof(zoom_ratio)/2-1;
                break;
            case WKUP_Press:
                autorun = !autorun;
                break;
            default:break;    
        }
        if(autorun)
        {
            i++;
            LED_ON(LED1_GPIO_Pin);
            if(i > sizeof(zoom_ratio)/2-1)
            {
                i = 0;
            }
        }
        else
        {
            LED_OFF(LED1_GPIO_Pin);
        }
        LCD_Set_Window(0,0,lcddev.width, lcddev.height);
        LCD_WriteRAM_Prepare();
        TIM6->CNT = 0;
        timeout = 0;
        julia_generate_fpu(lcddev.width, lcddev.height, lcddev.width/2, lcddev.height/2, zoom_ratio[i]);
        time = TIM6->CNT + (uint32_t)timeout * 65536;
        sprintf(buf, "%s: Zoom:%d  RunTime:%0.1fms", SCORE_FPU_MODE,zoom_ratio[i], time/10);
        LCD_ShowString(5,lcddev.height-17,lcddev.width-5,12,12,buf);
        printf("%s\n", buf);
        LED_TOGGLE(LED0_GPIO_Pin);
    }
}
