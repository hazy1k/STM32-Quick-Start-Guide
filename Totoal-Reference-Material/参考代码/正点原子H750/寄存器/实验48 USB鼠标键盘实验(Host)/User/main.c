/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       USB鼠标键盘(Host) 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "usbh_hid.h"
#include "usbh_hid_parser.h"



USBH_HandleTypeDef hUSBHost;    /* USB Host处理结构体 */

/* 应用状态结构体类型定义 */
typedef enum
{
    APPLICATION_IDLE = 0,       /* 挂起状态 */
    APPLICATION_DISCONNECT,     /* 断开连接 */
    APPLICATION_START,          /* 应用开始（连接上了） */
    APPLICATION_READY,          /* 准备完成（可以执行相关应用代码了） */
    APPLICATION_RUNNING,        /* 运行中 */
} HID_ApplicationTypeDef;


extern HID_MOUSE_Info_TypeDef mouse_info;
HID_ApplicationTypeDef App_State = APPLICATION_IDLE;

uint8_t g_usb_first_plugin_flag = 0;    /* USB第一次插入标志,如果为1,说明是第一次插入 */


/**
 * @brief       USB信息显示
 * @param       msg   : 信息类型
 *   @arg               0, USB无连接
 *   @arg               1, USB键盘
 *   @arg               2, USB鼠标
 *   @arg               3, 不支持的USB设备
 * @retval      无
 */
void usbh_msg_show(uint8_t msgx)
{
    switch (msgx)
    {
        case 0: /* USB无连接 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connecting...", RED);
            lcd_fill(0, 150, lcddev.width, lcddev.height, WHITE);
            break;

        case 1: /* USB键盘 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connected    ", RED);
            lcd_show_string(30, 150, 200, 16, 16, "USB KeyBoard", RED);
            lcd_show_string(30, 180, 210, 16, 16, "KEYVAL:", RED);
            lcd_show_string(30, 200, 210, 16, 16, "INPUT STRING:", RED);
            break;

        case 2: /* USB鼠标 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connected    ", RED);
            lcd_show_string(30, 150, 200, 16, 16, "USB Mouse", RED);
            lcd_show_string(30, 180, 210, 16, 16, "BUTTON:", RED);
            lcd_show_string(30, 200, 210, 16, 16, "X POS:", RED);
            lcd_show_string(30, 220, 210, 16, 16, "Y POS:", RED);
            lcd_show_string(30, 240, 210, 16, 16, "Z POS:", RED);
            break;

        case 3: /* 不支持的USB设备 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connected    ", RED);
            lcd_show_string(30, 150, 200, 16, 16, "Unknow Device", RED);
            break;
    }
}

/**
 * @brief       完成USBH不同用户状态下的数据处理
 * @param       phost : USBH句柄指针
 * @param       id    : USBH当前的用户状态
 * @retval      无
 */
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    uint8_t dev_type = 0XFF;    /* 设备类型,1,键盘;2,鼠标;其他,不支持的设备 */

    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            usbh_msg_show(0);   /* 显示已经断开连接,准备重新连接 */
            App_State = APPLICATION_DISCONNECT;
            break;

        case HOST_USER_CLASS_ACTIVE:
            App_State = APPLICATION_READY;
            dev_type = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bInterfaceProtocol;

            if (dev_type == HID_KEYBRD_BOOT_CODE)       /* 键盘设备 */
            {
                g_usb_first_plugin_flag = 1;    /* 标记第一次插入 */
                usbh_msg_show(1);   /* 显示键盘界面 */
            }
            else if (dev_type == HID_MOUSE_BOOT_CODE)   /* 鼠标设备 */
            {
                g_usb_first_plugin_flag = 1;    /* 标记第一次插入 */
                usbh_msg_show(2);   /* 显示鼠标界面 */
            }
            else
            {
                usbh_msg_show(3);   /* 显示不支持 */
            }
            
            break;

        case HOST_USER_CONNECTION:
            App_State = APPLICATION_START;
            break;

        default:
            break;
    }
}

/* 临时数组,用于存放鼠标坐标/键盘输入内容(4.3屏,最大可以输入2016字节) */
#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
__align(4) uint8_t g_temp_buffer[2017];
#else      /* 使用AC6编译器时 */
__ALIGNED(4) uint8_t g_temp_buffer[2017];
#endif

/**
 * @brief       USB鼠标数据处理
 * @param       data  : USB鼠标数据结构体指针
 * @retval      无
 */
void mouse_data_process(HID_MOUSE_Info_TypeDef *data)
{
    static signed short x, y, z;

    if (g_usb_first_plugin_flag)    /* 第一次插入,将数据清零 */
    {
        g_usb_first_plugin_flag = 0;
        x = y = z = 0;
    }

    x += (signed char)data->x;

    if (x > 9999)x = 9999;

    if (x < -9999)x = -9999;

    y += (signed char)data->y;

    if (y > 9999)y = 9999;

    if (y < -9999)y = -9999;

    z += (signed char)data->z;

    if (z > 9999)z = 9999;

    if (z < -9999)z = -9999;

    sprintf((char *)g_temp_buffer, "BUTTON:");

    if (data->button & 0X01)strcat((char *)g_temp_buffer, "LEFT ");

    if ((data->button & 0X03) == 0X02)
    {
        strcat((char *)g_temp_buffer, "RIGHT");
    }
    else
    {
        if ((data->button & 0X03) == 0X03)strcat((char *)g_temp_buffer, "+RIGHT");
    }
    
    if ((data->button & 0X07) == 0X04)
    {
        strcat((char *)g_temp_buffer, "MID  ");
    }
    else
    {
        if ((data->button & 0X07) > 0X04)strcat((char *)g_temp_buffer, "+MID");
    }
    
    lcd_fill(30 + 56, 180, lcddev.width - 1, 180 + 16, WHITE);
    lcd_show_string(30, 180, 210, 16, 16, (char *)g_temp_buffer, BLUE);
    sprintf((char *)g_temp_buffer, "X POS:%05d", x);
    lcd_show_string(30, 200, 200, 16, 16, (char *)g_temp_buffer, BLUE);
    sprintf((char *)g_temp_buffer, "Y POS:%05d", y);
    lcd_show_string(30, 220, 200, 16, 16, (char *)g_temp_buffer, BLUE);
    sprintf((char *)g_temp_buffer, "Z POS:%05d", z);
    lcd_show_string(30, 240, 200, 16, 16, (char *)g_temp_buffer, BLUE);
    //printf("btn,X,Y,Z:0x%x,%d,%d,%d\r\n",data->button,(signed char)data->x,(signed char)data->y,(signed char)data->z);
}

/**
 * @brief       USB键盘数据处理
 * @param       data  : USB键盘键值
 * @retval      无
 */
void  keybrd_data_process(uint8_t data)
{
    static uint16_t pos;
    static uint16_t endx, endy;
    static uint16_t maxinputchar;

    uint8_t buf[4];

    if (g_usb_first_plugin_flag)    /* 第一次插入,将数据清零 */
    {
        g_usb_first_plugin_flag = 0;
        endx = ((lcddev.width - 30) / 8) * 8 + 30;      /* 得到endx值 */
        endy = ((lcddev.height - 220) / 16) * 16 + 220; /* 得到endy值 */
        maxinputchar = ((lcddev.width - 30) / 8);
        maxinputchar *= (lcddev.height - 220) / 16;     /* 当前LCD最大可以显示的字符数 */
        pos = 0;
    }

    sprintf((char *)buf, "%02X", data);
    lcd_show_string(30 + 56, 180, 200, 16, 16, (char *)buf, BLUE);  /* 显示键值 */

    if (data >= ' ' && data <= '~')
    {
        g_temp_buffer[pos++] = data;
        g_temp_buffer[pos] = 0; /* 添加结束符 */

        if (pos > maxinputchar)pos = maxinputchar;  /* 最大输入这么多 */
    }
    else if (data == 0X0D)      /* 退格键 */
    {
        if (pos)pos--;

        g_temp_buffer[pos] = 0; /* 添加结束符 */
    }

    if (pos <= maxinputchar)    /* 没有超过显示区 */
    {
        lcd_fill(30, 220, endx, endy, WHITE);
        lcd_show_string(30, 220, endx - 30, endy - 220, 16, (char *)g_temp_buffer, BLUE);
    }

    //printf("KEY Board Value:%02X\r\n",data);
    //printf("KEY Board Char:%c\r\n",data);
}

/**
 * @brief       USB键盘/鼠标演示demo数据处理
 * @param       phost : USBH句柄指针
 * @retval      无
 */
void usb_demo(USBH_HandleTypeDef *phost)
{
    char c;
    HID_KEYBD_Info_TypeDef *k_pinfo;
    HID_MOUSE_Info_TypeDef *m_pinfo;

    if (App_State == APPLICATION_READY)
    {
        if (USBH_HID_GetDeviceType(phost) == HID_KEYBOARD)  /* 键盘设备 */
        {
            k_pinfo = USBH_HID_GetKeybdInfo(phost);         /* 获取键盘信息 */

            if (k_pinfo != NULL)
            {
                c = USBH_HID_GetASCIICode(k_pinfo);         /* 转换成ASCII码 */
                keybrd_data_process(c);                     /* 在LCD上显示出键盘字符 */
            }
        }
        else if (USBH_HID_GetDeviceType(phost) == HID_MOUSE)/* 鼠标设备 */
        {
            m_pinfo = USBH_HID_GetMouseInfo(phost);         /* 获取鼠标信息 */

            if (m_pinfo != NULL)
            {
                mouse_data_process(&mouse_info);            /* LCD上显示鼠标信息 */
            }
        }
    }
}

int main(void)
{
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    lcd_init();                         /* 初始化LCD */
    led_init();                         /* 初始化LED */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池(ITCM) */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB MOUSE/KEYBOARD TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "USB Connecting...", RED);

    USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
    USBH_Start(&hUSBHost);

    while (1)
    {
        USBH_Process(&hUSBHost);
        usb_demo(&hUSBHost);
    }
}






