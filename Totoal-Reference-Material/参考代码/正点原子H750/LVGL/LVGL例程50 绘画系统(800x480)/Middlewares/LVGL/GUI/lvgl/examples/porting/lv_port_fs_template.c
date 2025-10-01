/**
 * @file lv_port_fs_templ.c
 *
 */

 /*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs_template.h"
#include "../../lvgl.h"
/* 导入相关头文件 */
#include "./BSP/LED/led.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./FATFS/source/ff.h"
#include "./FATFS/exfuns/exfuns.h"

/*********************
 *      DEFINES
 *********************/
/* 创建一个类型来存储有关文件的所需数据 */
typedef FIL file_t;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

//static lv_fs_res_t fs_open (lv_fs_drv_t * drv, void * file_p, const char * path, lv_fs_mode_t mode);
//static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p);
//static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
//static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
//static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
//static lv_fs_res_t fs_size (lv_fs_drv_t * drv, void * file_p, uint32_t * size_p);
//static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

//static lv_fs_res_t fs_dir_open (lv_fs_drv_t * drv, void * rddir_p, const char *path);
//static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * rddir_p, char *fn);
//static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
 
/**
 * @brief       初始化并注册文件系统
 * @param       无
 * @retval      无
 */
void lv_port_fs_init(void)
{
    /*----------------------------------------------------
     * 初始化你的存储设备和文件系统
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * 注册 LVGL 的文件系统接口
     *--------------------------------------------------*/

    /* 添加一个简单的文件系统 */
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /* 设置回调函数 */
//    fs_drv.letter = '0';
//    fs_drv.open_cb = fs_open;
//    fs_drv.close_cb = fs_close;
//    fs_drv.read_cb = fs_read;
//    fs_drv.write_cb = fs_write;
//    fs_drv.seek_cb = fs_seek;
//    fs_drv.tell_cb = fs_tell;

//    fs_drv.dir_close_cb = fs_dir_close;
//    fs_drv.dir_open_cb = fs_dir_open;
//    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief       初始化你的存储设备和文件系统
 * @param       无
 * @retval      无
 */
static void fs_init(void)
{
    /* 例如，对FatFA初始化SD卡和FatFS本身 */

    /*You code here*/
    while (sd_init())                   /* 初始化SD卡 */
    {
        LED0_TOGGLE();
        HAL_Delay(200);
    } LED0(0);
    printf("exfuns_init: %d\r\n", exfuns_init());                      /* 为fatfs相关变量申请内存 */
    printf("f_mount: %d\r\n", f_mount(fs[0], "0:", 1));            /* 挂载SD卡 */
}

///**
// * @brief       文件打开
// * @param       drv : 文件系统设备
// *   @arg       path: 文件的路径
// *   @arg       mode: 读: FS_MODE_RD, 写: FS_MODE_WR, 读写: FS_MODE_RD | FS_MODE_WR
// *
// * @retval      文件描述或NULL或错误
// */
//static void * fs_open (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    void * f = NULL;

//    if(mode == LV_FS_MODE_WR)
//    {
//        /* 写方式打开文件 */
//        f = ...         /*Add your code here*/
//    }
//    else if(mode == LV_FS_MODE_RD)
//    {
//        /* 读方式打开文件 */
//        f = ...         /*Add your code here*/
//    }
//    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
//    {
//        /* 读写方式打开文件 */
//        f = ...         /*Add your code here*/
//    }

//    return file;
//}

///**
// * @brief       关闭打开的文件
// * @param       drv     : 文件系统设备
// *   @arg       file_p  : 文件指针
// *
// * @retval      LV_FS_RES_OK: 文件关闭成功
// */
//static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

///**
// * @brief       从打开的文件中读取数据
// * @param       drv     : 文件系统设备
// *   @arg       file_p  : 文件指针
// *   @arg       buf     : 指向存储数据内存的指针
// *   @arg       btr     : 读取的字节数量
// *   @arg       br      : 实际的读取字节数量
// *
// * @retval      LV_FS_RES_OK: 文件读取成功
// */
//static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

///**
// * @brief       向文件中写入数据
// * @param       drv     : 文件系统设备
// *   @arg       file_p  : 文件指针
// *   @arg       buf     : 指向存储数据内存的指针
// *   @arg       btw     : 写入的字节数量
// *   @arg       bw      : 实际的写入字节数量
// *
// * @retval      LV_FS_RES_OK: 文件写入成功
// */
//static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

///**
// * @brief       设置读写指针。如果需要，还可以扩展文件大小。
// * @param       drv     : 文件系统设备
// *   @arg       file_p  : 文件指针
// *   @arg       pos     : 读写指针的新位置
// *   @arg       whence  : 说明从何处解释 `pos`。查看 @lv_fs_whence_t
// *
// * @retval      LV_FS_RES_OK: 成功
// */
//static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

///**
// * @brief       返回读写指针的位置
// * @param       drv     : 文件系统设备
// *   @arg       file_p  : 文件指针
// *   @arg       pos_p   : 读写指针的新位置
// *
// * @retval      LV_FS_RES_OK: 成功
// */
//static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

///**
// * @brief       打开目录
// * @param       drv     : 文件系统设备
// *   @arg       rddir_p : 指向 `lv_fs_dir_t` 变量的指针
// *   @arg       path    : 目录路径
// *
// * @retval      LV_FS_RES_OK: 成功，LV_FS_RES_NOT_IMP: 失败
// */
//static void * fs_dir_open (lv_fs_drv_t * drv, void * rddir_p, const char *path)
//{
//    void * dir = NULL;
//    /*Add your code here*/
//    dir = ...           /*Add your code here*/
//    return dir;
//}

///**
// * @brief       从目录中读取下一个文件名，目录名以“/”开头
// * @param       drv     : 文件系统设备
// *   @arg       rddir_p : 指向 `lv_fs_dir_t` 变量的指针
// *   @arg       fn      : 指向存放文件名的缓冲区的指针
// *
// * @retval      LV_FS_RES_OK: 成功，LV_FS_RES_NOT_IMP: 失败
// */
//static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * rddir_p, char *fn)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

///**
// * @brief       关闭读取目录
// * @param       drv     : 文件系统设备
// *   @arg       rddir_p : 指向 `lv_fs_dir_t` 变量的指针
// *
// * @retval      LV_FS_RES_OK: 成功，LV_FS_RES_NOT_IMP: 失败
// */
//static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * rddir_p)
//{
//    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

//    /*Add your code here*/

//    return res;
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
