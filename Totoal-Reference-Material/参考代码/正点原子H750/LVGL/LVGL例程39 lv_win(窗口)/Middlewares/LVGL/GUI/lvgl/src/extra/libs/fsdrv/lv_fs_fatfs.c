/**
 * @file lv_fs_fatfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_FATFS
#include "ff.h"

/* 添加相关头文件 */
#include <stdio.h>
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./FATFS/exfuns/exfuns.h"

/*********************
 *      DEFINES
 *********************/

#if LV_FS_FATFS_LETTER == '\0'
    #error "LV_FS_FATFS_LETTER must be an upper case ASCII letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_fatfs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv; /*A driver descriptor*/
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = LV_FS_FATFS_LETTER;
    fs_drv.cache_size = LV_FS_FATFS_CACHE_SIZE;

    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief       初始化存储设备和文件系统
 * @param       无
 * @retval      无
 */
static void fs_init(void)
{
    /*Initialize the SD card and FatFS itself.
     *Better to do it in your code to keep this library untouched for easy updating*/
    uint8_t res;
    
    /* 初始化 SD 卡和 FatFS 本身
     * 最好在自己的库中完成，一遍以后更新 */
    while (sd_init())               /* 初始化 SD 卡 */
    {
        lcd_show_string(10, 10, 200, 24, 24, "SD Card Error!", RED);
        printf("SD Card Error, Please Check!\r\n");
        LED0_TOGGLE();
        HAL_Delay(200);
    }
    
    LED0(0);
    
    exfuns_init();                  /* 为 fatfs 相关变量申请内存 */
    res = f_mount(fs[0], "0:", 1);  /* 挂载 SD 卡 */
    
    if (0 != res)
    {
        lcd_show_string(10, 40, 200, 24, 24, "SD Card Mount Fail!", RED);
        printf("SD Card Mount Fail, Please Check!\r\n");
        LED0_TOGGLE();
        HAL_Delay(200);
    }
}

/**
 * @brief 打开一个文件
 * @param drv：指向该函数所属的驱动程序
 * @param path：以驱动符开头的文件路径(例如S:/folder/file.txt)
 * @param mode: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @retval 非NULL: 成功, NULLP：失败
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    uint8_t flags = 0;

    if(mode == LV_FS_MODE_WR) flags = FA_WRITE | FA_OPEN_ALWAYS;
    else if(mode == LV_FS_MODE_RD) flags = FA_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;

    FIL * f = lv_mem_alloc(sizeof(FIL));
    if(f == NULL) return NULL;

    FRESULT res = f_open(f, path, flags);
    if(res == FR_OK) {
        return f;
    }
    else {
        lv_mem_free(f);
        return NULL;
    }
}

 /**
 * @brief  关闭已打开的文件
 * @param  drv：指向该函数所属的驱动程序
 * @param  file_p：指向file_t变量的指针。(与lv_ufs_open打开)
 * @retval LV_FS_RES_OK:没有错误，文件被读取来自lv_fs_res_t enum的任何错误
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    f_close(file_p);
    lv_mem_free(file_p);
    return LV_FS_RES_OK;
}

/**
 * @brief 从打开的文件中读取数据
 * @param drv：指向该函数所属的驱动程序
 * @param file_p：指向file_t变量的指针。
 * @param buf：指针，指向存储读数据的内存块
 * @param btr：要读取的字节数
 * @param br：实际读字节数(字节读)
 * @retval LV_FS_RES_OK:没有错误，文件被读取任何错误来自lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    FRESULT res = f_read(file_p, buf, btr, (UINT *)br);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * @brief  写入文件
 * @param  drv：   指向该函数所属的驱动程序
 * @param  file_p：指向file_t变量的指针
 * @param  buf：   指针，指向一个带有要写入字节的缓冲区
 * @param  btw：   要写的btr字节数
 * @param  br：    实际写入的字节数(已写入的字节数)。如果未使用NULL。
 * @retval LV_FS_RES_OK:没有错误，文件被读取任何错误来自lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    FRESULT res = f_write(file_p, buf, btw, (UINT *)bw);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * @brief  设置读写指针。如果有必要，也可以扩展文件大小。
 * @param  drv：   指向该函数所属的驱动程序
 * @param  file_p：指向file_t变量的指针。(使用lv_ufs_open打开)
 * @param  pos：   读写指针的新位置
 * @retval LV_FS_RES_OK:没有错误，文件被读取任何错误来自lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    switch(whence) {
        case LV_FS_SEEK_SET:
            f_lseek(file_p, pos);
            break;
        case LV_FS_SEEK_CUR:
            f_lseek(file_p, f_tell((FIL *)file_p) + pos);
            break;
        case LV_FS_SEEK_END:
            f_lseek(file_p, f_size((FIL *)file_p) + pos);
            break;
        default:
            break;
    }
    return LV_FS_RES_OK;
}

/**
 * @brief  返回读写指针的位置
 * @param  drv：   指向该函数所属的驱动程序
 * @param  file_p：指向file_t变量的指针
 * @param  pos_p： 用于存储结果的指针
 * @retval LV_FS_RES_OK:没有错误，文件被读取任何错误来自lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = f_tell((FIL *)file_p);
    return LV_FS_RES_OK;
}

/**
 * @brief  打开目录
 * @param  drv：    指向该函数所属的驱动程序
 * @param  rddir_p：指向'lv_fs_dir_t'变量的指针
 * @param  path：   目录路径
 * @retval 指向初始化的'DIR'变量的指针
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    DIR * d = lv_mem_alloc(sizeof(DIR));
    if(d == NULL) return NULL;

    FRESULT res = f_opendir(d, path);
    if(res != FR_OK) {
        lv_mem_free(d);
        d = NULL;
    }
    return d;
}

/**
 * @brief  从一个目录中读取下一个文件名
 * @param  drv：    指向该函数所属的驱动程序
 * @param  rddir_p：指向初始化的“lv_fs_dir_t”变量的指针
 * @param  fn：     指向存放文件名的缓冲区的指针
 * @retval LV_FS_RES_OK:没有错误，文件被读取任何错误来自lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn)
{
    LV_UNUSED(drv);
    FRESULT res;
    FILINFO fno;
    fn[0] = '\0';

    do {
        res = f_readdir(dir_p, &fno);
        if(res != FR_OK) return LV_FS_RES_UNKNOWN;

        if(fno.fattrib & AM_DIR) {
            fn[0] = '/';
            strcpy(&fn[1], fno.fname);
        }
        else strcpy(fn, fno.fname);

    } while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

/**
 * @brief  关闭目录读取
 * @param  drv：    指向该函数所属的驱动程序
 * @param  rddir_p：指向初始化的“lv_fs_dir_t”变量的指针
 * @retval LV_FS_RES_OK:没有错误，文件被读取任何错误来自lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
    f_closedir(dir_p);
    lv_mem_free(dir_p);
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_FATFS == 0*/

#if defined(LV_FS_FATFS_LETTER) && LV_FS_FATFS_LETTER != '\0'
    #warning "LV_USE_FS_FATFS is not enabled but LV_FS_FATFS_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/

