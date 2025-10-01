/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"
#include "../../lvgl.h"

/* 导入驱动头文件 */
#include "./BSP/TOUCH/touch.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* 触摸屏 */
static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

/* 鼠标 */
//static void mouse_init(void);
//static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static bool mouse_is_pressed(void);
//static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

/* 键盘 */
//static void keypad_init(void);
//static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static uint32_t keypad_get_key(void);

/* 编码器 */
//static void encoder_init(void);
//static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static void encoder_handler(void);

/* 按钮 */
//static void button_init(void);
//static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static int8_t button_get_pressed_id(void);
//static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;    // 触摸屏
//lv_indev_t * indev_mouse;       // 鼠标
//lv_indev_t * indev_keypad;      // 键盘
//lv_indev_t * indev_encoder;     // 编码器
//lv_indev_t * indev_button;      // 按钮

/* 编码器相关 */
//static int32_t encoder_diff;
//static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief       初始化并注册输入设备
 * @param       无
 * @retval      无
 */
void lv_port_indev_init(void)
{
    /**
     * 
     * 在这里你可以找到 LittlevGL 支持的出入设备的实现示例:
     *  - 触摸屏
     *  - 鼠标 (支持光标)
     *  - 键盘 (仅支持按键的 GUI 用法)
     *  - 编码器 (支持的 GUI 用法仅包括: 左, 右, 按下)
     *  - 按钮 (按下屏幕上指定点的外部按钮)
     *
     *  函数 `..._read()` 只是示例
     *  你需要根据具体的硬件来完成这些函数
     */

    static lv_indev_drv_t indev_drv;

    /*------------------
     * 触摸屏
     * -----------------*/

    /* 初始化触摸屏(如果有) */
    touchpad_init();

    /* 注册触摸屏输入设备 */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

    /*------------------
     * 鼠标
     * -----------------*/

    /* 初始化鼠标(如果有) */
//    mouse_init();

    /* 注册鼠标输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_POINTER;
//    indev_drv.read_cb = mouse_read;
//    indev_mouse = lv_indev_drv_register(&indev_drv);

    /* 设置光标，为了简单起见，现在设置为一个 HOME 符号 */
//    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
//    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
//    lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * 键盘
     * -----------------*/

//    /* 初始化键盘(如果有) */
//    keypad_init();

//    /* 注册键盘输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
//    indev_drv.read_cb = keypad_read;
//    indev_keypad = lv_indev_drv_register(&indev_drv);

//    /* 接着你需要用 `lv_group_t * group = lv_group_create()` 来创建组
//     * 用 `lv_group_add_obj(group, obj)` 往组中添加物体
//     * 并将这个输入设备分配到组中，以导航到它:
//     * `lv_indev_set_group(indev_keypad, group);` */

    /*------------------
     * 编码器
     * -----------------*/

//    /* 初始化编码器(如果有) */
//    encoder_init();

//    /* 注册编码器输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_ENCODER;
//    indev_drv.read_cb = encoder_read;
//    indev_encoder = lv_indev_drv_register(&indev_drv);

//    /* 接着你需要用 `lv_group_t * group = lv_group_create()` 来创建组
//     * 用 `lv_group_add_obj(group, obj)` 往组中添加物体
//     * 并将这个输入设备分配到组中，以导航到它:
//     * `lv_indev_set_group(indev_keypad, group);` */

    /*------------------
     * 按钮
     * -----------------*/

//    /* 初始化按钮(如果有) */
//    button_init();

//    /* 注册按钮输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_BUTTON;
//    indev_drv.read_cb = button_read;
//    indev_button = lv_indev_drv_register(&indev_drv);

//    /* 为按钮分配屏幕上的点
//     * 以此来用按钮模拟点击屏幕上对应的点 */
//    static const lv_point_t btn_points[2] = {
//            {10, 10},   /*Button 0 -> x:10; y:10*/
//            {40, 100},  /*Button 1 -> x:40; y:100*/
//    };
//    lv_indev_set_button_points(indev_button, btn_points);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * 触摸屏
 * -----------------*/

/**
 * @brief       初始化触摸屏
 * @param       无
 * @retval      无
 */
static void touchpad_init(void)
{
    /*Your code comes here*/
    tp_dev.init();
    
    /* 电阻屏坐标矫正 */
    if (key_scan(0) == KEY0_PRES)           /* KEY0按下,则执行校准程序 */
    {
        lcd_clear(WHITE);                   /* 清屏 */
        tp_adjust();                        /* 屏幕校准 */
        tp_save_adjust_data();
    }
}

/**
 * @brief       图形库的触摸屏读取回调函数
 * @param       indev_drv   : 触摸屏设备
 *   @arg       data        : 输入设备数据结构体
 * @retval      无
 */
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /* 保存按下的坐标和状态 */
    if(touchpad_is_pressed())
    {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    } 
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /* 设置最后按下的坐标 */
    data->point.x = last_x;
    data->point.y = last_y;
}

/**
 * @brief       获取触摸屏设备的状态
 * @param       无
 * @retval      返回触摸屏设备是否被按下
 */
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
    tp_dev.scan(0);

    if (tp_dev.sta & TP_PRES_DOWN)
    {
        return true;
    }

    return false;
}

/**
 * @brief       在触摸屏被按下的时候读取 x、y 坐标
 * @param       x   : x坐标的指针
 *   @arg       y   : y坐标的指针
 * @retval      无
 */
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/
    (*x) = tp_dev.x[0];
    (*y) = tp_dev.y[0];
}

/*------------------
 * 鼠标
 * -----------------*/

/**
 * @brief       初始化鼠标
 * @param       无
 * @retval      无
 */
//static void mouse_init(void)
//{
//    /*Your code comes here*/
//    tp_dev.init();
//    /* 电阻屏如果发现显示屏XY镜像现象，需要坐标矫正 */
//    if (0 == (tp_dev.touchtype & 0x80))
//    {
//        tp_adjust();
//        tp_save_adjust_data();
//    }
//}

/**
 * @brief       图形库的鼠标读取回调函数
 * @param       indev_drv   : 鼠标设备
 *   @arg       data        : 输入设备数据结构体
 * @retval      无
 */
//static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//    /* 获取当前的 x、y 坐标 */
//    mouse_get_xy(&data->point.x, &data->point.y);

//    /* 获取是否按下或释放鼠标按钮 */
//    if(mouse_is_pressed()) {
//        data->state = LV_INDEV_STATE_PR;
//    } else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//}

/**
 * @brief       获取鼠标设备是否被按下
 * @param       无
 * @retval      返回鼠标设备是否被按下
 */
//static bool mouse_is_pressed(void)
//{
//    /*Your code comes here*/
//    tp_dev.scan(0);
//    
//    if (tp_dev.sta & TP_PRES_DOWN)
//    {
//        return true;
//    }
//    
//    return false;
//}

/**
 * @brief       当鼠标被按下时，获取鼠标的 x、y 坐标
 * @param       x   : x坐标的指针
 *   @arg       y   : y坐标的指针
 * @retval      无
 */
//static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
//{
//    /*Your code comes here*/

//    (*x) = tp_dev.x[0];
//    (*y) = tp_dev.y[0];
//}

/*------------------
 * 键盘
 * -----------------*/

///**
// * @brief       初始化键盘
// * @param       无
// * @retval      无
// */
//static void keypad_init(void)
//{
//    /*Your code comes here*/
//}

///**
// * @brief       图形库的键盘读取回调函数
// * @param       indev_drv : 键盘设备
// *   @arg       data      : 输入设备数据结构体
// * @retval      无
// */
//static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//    static uint32_t last_key = 0;

////    /* 这段代码是 LVGL 给出的例子，这里获取坐标好像是多余的 */
////    /*Get the current x and y coordinates*/
////    mouse_get_xy(&data->point.x, &data->point.y);

//    /* 获取按键是否被按下，并保存键值 */
//    uint32_t act_key = keypad_get_key();
//    if(act_key != 0) {
//        data->state = LV_INDEV_STATE_PR;

//        /* 将键值转换成 LVGL 的控制字符 */
//        switch(act_key) {
//        case 1:
//            act_key = LV_KEY_NEXT;
//            break;
//        case 2:
//            act_key = LV_KEY_PREV;
//            break;
//        case 3:
//            act_key = LV_KEY_LEFT;
//            break;
//        case 4:
//            act_key = LV_KEY_RIGHT;
//            break;
//        case 5:
//            act_key = LV_KEY_ENTER;
//            break;
//        }

//        last_key = act_key;
//    } else {
//        data->state = LV_INDEV_STATE_REL;
//    }

//    data->key = last_key;
//}

///**
// * @brief       获取当前正在按下的按键
// * @param       无
// * @retval      0 : 按键没有被按下
// */
//static uint32_t keypad_get_key(void)
//{
//    /*Your code comes here*/

//    return 0;
//}

/*------------------
 * 编码器
 * -----------------*/

///**
// * @brief       初始化编码器
// * @param       无
// * @retval      无
// */
//static void encoder_init(void)
//{
//    /*Your code comes here*/
//}


///**
// * @brief       图形库的编码器读取回调函数
// * @param       indev_drv : 编码器设备
// *   @arg       data      : 输入设备数据结构体
// * @retval      无
// */
//static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{

//    data->enc_diff = encoder_diff;
//    data->state = encoder_state;
//}

///**
// * @brief       在中断中调用此函数以处理编码器事件(旋转、按下)
// * @param       无
// * @retval      无
// */
//static void encoder_handler(void)
//{
//    /*Your code comes here*/

//    encoder_diff += 0;
//    encoder_state = LV_INDEV_STATE_REL;
//}

/*------------------
 * 按钮
 * -----------------*/


///**
// * @brief       初始化按钮
// * @param       无
// * @retval      无
// */
//static void button_init(void)
//{
//    /*Your code comes here*/
//}

///**
// * @brief       图形库的按钮读取回调函数
// * @param       indev_drv : 按钮设备
// *   @arg       data      : 输入设备数据结构体
// * @retval      无
// */
//static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{

//    static uint8_t last_btn = 0;

//    /* 获取被按下按钮的ID */
//    int8_t btn_act = button_get_pressed_id();

//    if(btn_act >= 0) {
//        data->state = LV_INDEV_STATE_PR;
//        last_btn = btn_act;
//    } else {
//        data->state = LV_INDEV_STATE_REL;
//    }

//    /* 保存最后被按下按钮的ID */
//    data->btn_id = last_btn;
//}

///**
// * @brief       获取被按下按钮的ID
// * @param       无
// * @retval      被按下按钮的ID
// */
//static int8_t button_get_pressed_id(void)
//{
//    uint8_t i;

//    /* 检查那个按键被按下(这里给出的示例适用于两个按钮的情况) */
//    for(i = 0; i < 2; i++) {
//        /* 返回被按下按钮的ID */
//        if(button_is_pressed(i)) {
//            return i;
//        }
//    }

//    /* 没有按钮被按下 */
//    return -1;
//}

///**
// * @brief       检查指定ID的按钮是否被按下
// * @param       无
// * @retval      按钮是否被按下
// */
//static bool button_is_pressed(uint8_t id)
//{

//    /*Your code comes here*/

//    return false;
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
