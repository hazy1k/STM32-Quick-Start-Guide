#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* �û�������
 * ������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����)
 */

#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "rtc.h"


/* �������б��ʼ��(�û��Լ����)
 * �û�ֱ������������Ҫִ�еĺ�����������Ҵ�
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* ���ʹ���˶�д���� */
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

/* �������ƹ�������ʼ��
 * �õ������ܿغ���������
 * �õ�����������
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* �������� */
    0,      /* �������� */
    0,      /* ����ID */
    1,      /* ������ʾ����,0,10����;1,16���� */
    0,      /* ��������.bitx:,0,����;1,�ַ��� */
    0,      /* ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ�� */
    0,      /* �����Ĳ���,��ҪPARM_LEN��0��ʼ�� */
};



