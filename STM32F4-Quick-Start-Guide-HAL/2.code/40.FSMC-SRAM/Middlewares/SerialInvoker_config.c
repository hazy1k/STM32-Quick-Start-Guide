#include "SerialInvoker.h"

/* 用户配置区 */
/******************************************************************************************/
// 用户头文件包含
#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "sram.h"
// 用户函数名列表初始化
struct _m_serial_invoker_nametab serial_invoker_nametab[] = {
#if SERIAL_INVOKER_USE_WRFUNS == 1
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
#endif
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",
    (void *)LCD_Clear, "void LCD_Clear(uint32_t color)",
    (void *)LCD_Fill, "void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)",
    (void *)LCD_DrawLine, "void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)",
    (void *)LCD_Draw_Circle, "void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)",
    (void *)LCD_DrawRectangle, "void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)",
    (void *)LCD_ShowNum, "void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)",
    (void *)LCD_ShowxNum, "void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)",
    (void *)LCD_ShowString, "void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)",
    (void *)LCD_DrawPoint, "void LCD_DrawPoint(u16 x,u16 y)",
    (void *)LCD_ReadPoint, "u32  LCD_ReadPoint(u16 x,u16 y)",
    (void *)LCD_Scan_Dir, "void LCD_Scan_Dir(u8 dir)",
	  (uint8_t*)sram_test_read, "uint8_t sram_test_read(uint32_t addr)",
    (void *)sram_test_write, "void sram_test_write(uint32_t addr, uint8_t data)",
};
/******************************************************************************************/

/* 控制管理器初始化 */
struct _m_serial_invoker_dev serial_invoker_dev = {
    serial_invoker_nametab,
    serial_invoker_init,
    serial_invoker_cmd_rec,
    serial_invoker_exe,
    serial_invoker_scan,
    sizeof(serial_invoker_nametab) / sizeof(struct _m_serial_invoker_nametab),
    0,      // 参数数量
    0,      // 函数ID
    1,      // 参数显示类型(16进制)
    0,      // 参数类型
    {0},    // 参数长度表
    {0},    // 参数缓冲区
};
