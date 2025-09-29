#include "./USMART/usmart.h"
#include "./USMART/usmart_str.h"

/******************************************************************************************/
/* 用户配置区
 * 这下面要包含所用到的函数所申明的头文件(用户自己添加)
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/OV5640/sccb.h"
#include "./BSP/OV5640/ov5640.h"
#include "./BSP/DCMI/dcmi.h"


/* 函数名列表初始化(用户自己添加)
 * 用户直接在这里输入要执行的函数名及其查找串
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* 如果使能了读写操作 */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr,uint32_t val)",
#endif
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",

    (void *)ov5640_sharpness, "void ov5640_sharpness(uint8_t sharp)",
    (void *)ov5640_brightness, "void ov5640_brightness(uint8_t bright)",
    (void *)ov5640_contrast, "void ov5640_contrast(uint8_t contrast)",
    (void *)ov5640_exposure, "void ov5640_exposure(uint8_t exposure)",
    (void *)ov5640_light_mode, "void ov5640_white_balance(uint8_t mode)",
    (void *)ov5640_special_effects, "void ov5640_special_effects(uint8_t eft)",
    (void *)ov5640_color_saturation, "void ov5640_color_saturation(uint8_t sat)",
        
    (void *)ov5640_flash_ctrl, "void ov5640_flash_ctrl(uint8_t sw)",
    (void *)ov5640_test_pattern, "void ov5640_test_pattern(uint8_t mode)",
        
    (void *)ov5640_focus_single, "uint8_t ov5640_focus_single(void)",
    (void *)ov5640_focus_constant, "uint8_t ov5640_focus_constant(void)",
    (void *)ov5640_flash_ctrl, "void ov5640_flash_ctrl(uint8_t sw)",
    (void *)ov5640_outsize_set, "uint8_t ov5640_outsize_set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)",
    (void *)ov5640_image_window_set, "uint8_t ov5640_image_window_set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)",
        
    (void *)ov5640_read_reg, "uint8_t ov5640_read_reg(uint16_t reg)",
    (void *)ov5640_write_reg, "uint8_t ov5640_write_reg(uint16_t reg,uint8_t data)",
    (void *)dcmi_cr_set, "void dcmi_cr_set(uint8_t pclk,uint8_t hsync,uint8_t vsync)",
    (void *)dcmi_set_window, "void dcmi_set_window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)",

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



















