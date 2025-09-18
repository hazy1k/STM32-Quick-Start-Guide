#ifndef __EXFUNS_H
#define __EXFUNS_H

#include "sys.h"
#include "ff.h"

extern FATFS *fs[FF_VOLUMES];
extern FIL *file;
extern FIL *ftemp;
extern UINT br, bw;
extern FILINFO fileinfo;
extern DIR dir;
extern uint8_t *fatbuf;     /* SD卡数据缓存区 */



/* exfuns_file_type返回的类型定义
 * 根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
 */
#define T_BIN       0X00    /* bin文件 */
#define T_LRC       0X10    /* lrc文件 */

#define T_NES       0X20    /* nes文件 */
#define T_SMS       0X21    /* sms文件 */

#define T_TEXT      0X30    /* .txt文件 */
#define T_C         0X31    /* .c文件 */
#define T_H         0X32    /* .h文件 */

#define T_WAV       0X40    /* WAV文件 */
#define T_MP3       0X41    /* MP3文件 */
#define T_OGG       0X42    /* OGG文件 */
#define T_FLAC      0X43    /* FLAC文件 */
#define T_AAC       0X44    /* AAC文件 */
#define T_WMA       0X45    /* WMA文件 */
#define T_MID       0X46    /* MID文件 */

#define T_BMP       0X50    /* bmp文件 */
#define T_JPG       0X51    /* jpg文件 */
#define T_JPEG      0X52    /* jpeg文件 */
#define T_GIF       0X53    /* gif文件 */

#define T_AVI       0X60    /* avi文件 */


uint8_t exfuns_init(void);                  /* 申请内存 */
uint8_t exfuns_file_type(char *fname);   /* 识别文件类型 */

uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free);    /* 得到磁盘总容量和剩余容量 */
uint32_t exfuns_get_folder_size(uint8_t *fdname);   /* 得到文件夹大小 */
uint8_t *exfuns_get_src_dname(uint8_t *dpfn);
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);       /* 文件复制 */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode);   /* 文件夹复制 */
#endif
























