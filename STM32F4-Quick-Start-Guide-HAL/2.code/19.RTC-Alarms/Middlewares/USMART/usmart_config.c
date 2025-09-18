#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* 用户配置区
 * 这下面要包含所用到的函数所申明的头文件(用户自己添加)
 */

#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "rtc.h"


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
    (HAL_StatusTypeDef *)rtc_set_time, "HAL_StatusTypeDef rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)",
    (HAL_StatusTypeDef *)rtc_set_date, "HAL_StatusTypeDef rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)",
    (void *)rtc_get_time, "void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm)",
    (void *)rtc_get_date, "void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week)",
    (void *)rtc_set_alarm, "void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)",
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



