/**
 ****************************************************************************************************
 * @file        atk_frec.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-02
 * @brief       人脸识别库 代码
 *              本人脸识别程序由ALIENTEK提供,我们提供2个LIB(即:ATKFREC_DF.lib和ATKFREC_SF.lib),供大家使用
 *              ATKFREC_DF.lib:适合具有双精度硬件浮点计算单元的STM32F7系列，比如STM32F767/769等
 *              ATKFREC_SF.lib:适合具有单精度硬件浮点计算单元的STM32F4/F7系列，比如STM32F407/429/746等
 *              请选择合适的lib进行测试！
 *  
 *              功能:利用摄像头,实现人脸识别.
 *              说明:本识别库,需要用到内存管理,内存总占用数在560KB左右(20个人脸).每增加一张人脸,内存占增加10KB左右.
 *              限制:由于本识别库以M3/M4/M7为目标处理器,内存有限,算法上进行了大量阉割,所以,很多功能不太完善,效果也不
 *                   是很好.且没有做识别有效检查(输入错误的人脸,也会有结果输出).所以,次代码,仅供大家参考用.
 *              
 *              其他需求:
 *              1,摄像头模块一个.
 *              2,SD卡一张
 *              
 *              使用方法:
 *              第一步:调用atk_frec_initialization函数,初始化人脸识别库
 *              第二步:调用atk_frec_add_a_face函数,添加人脸模板(如果已经有了,可以忽略次步)
 *              第三步:调用atk_frec_load_data_model函数,加载所有模板到内存里面(仅在添加新模板后需要,如没有添加新模板,则可忽略此步)
 *              第四步:调用atk_frec_recognition_face函数,获取识别结果.
 *              第五步:调用atk_frec_delete_data函数,可以删除一个人脸模板
 *              第六步:如果不想再用识别库,则调用atk_frec_destroy函数,释放所有内存,结束人脸识别.
 *
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.1 20220602
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "atk_frec.h"
#include "stdio.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"


/**
 * @brief       内存设置函数
 * @param       p               : 内存首地址
 * @param       c               : 要设置的值
 * @param       len             : 需要设置的内存大小(字节为单位)
 * @retval      无
 */
void atk_frec_memset(char *p, char c, unsigned long len)
{
    my_mem_set((uint8_t *)p, (uint8_t)c, (uint32_t)len);
}

/**
 * @brief       内存申请函数
 * @param       size            : 申请的内存大小
 * @retval      0,失败; 其他, 内存首地址;
 */
void *atk_frec_malloc(unsigned int size)
{
    void *p;
    p = mymalloc(SRAMDTCM, size);

    if (p == 0)
    {
        p = mymalloc(SRAMIN, size);
    }

    return p;
}

/**
 * @brief       内存释放函数
 * @param       ptr             : 内存首地址
 * @retval      无
 */
void atk_frec_free(void *ptr)
{
    if (((uint32_t)ptr & 0XFF000000) == 0X24000000)
    {
        myfree(SRAMIN, ptr);
    }
    else
    {
        myfree(SRAMDTCM, ptr);
    }
}

/**
 * @brief       内存快速申请函数
 * @param       size            : 申请的内存大小
 * @retval      0,失败; 其他, 内存首地址;
 */
void *atk_frec_fastmalloc(unsigned int size)
{
    return mymalloc(SRAMITCM, size);
}

/**
 * @brief       内存快速释放函数
 * @param       ptr             : 内存首地址
 * @retval      无
 */
void atk_frec_fastfree(void *ptr)
{
    myfree(SRAMITCM, ptr);
}

/**
 * @brief       保存人脸识别所需的数据
 * @param       index           : 要保存的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1
 * @param       buf             : 要保存的数据缓存区首地址
 * @param       size            : 要保存的数据大小
 * @retval      0, 成功; 其他, 取消设置;
 */
uint8_t atk_frec_save_data(uint8_t index, uint8_t *buf, uint32_t size)
{
    uint8_t *path;
    FIL *fp;
    DIR fdir;
    uint32_t fw;
    uint8_t res;
    path = atk_frec_fastmalloc(30);         /* 申请内存 */
    fp = atk_frec_fastmalloc(sizeof(FIL));  /* 申请内存 */

    if (!fp)
    {
        atk_frec_fastfree(path);
        return ATK_FREC_MEMORY_ERR;
    }

    sprintf((char *)path, ATK_FREC_DATA_PNAME, index);
    res = f_opendir(&fdir, (const TCHAR *)ATK_FREC_DATA_PDIR); /* 尝试打开ATK_FREC_DATA_PDIR目录 */

    if (res)    /* 打开失败 */
    {
        f_mkdir(ATK_FREC_DATA_PDIR);    /* 创建文件夹 */
    }
    else
    {
        f_closedir(&fdir);  /* 关闭dir */
    }

    res = f_open(fp, (char *)path, FA_WRITE | FA_CREATE_NEW);

    if (res == FR_OK)
    {
        res = f_write(fp, buf, size, &fw);  /* 写入文件 */
    }

    f_close(fp);

    if (res)res = ATK_FREC_READ_WRITE_ERR;

    atk_frec_fastfree(path);
    atk_frec_fastfree(fp);
    return res;
}

/**
 * @brief       读取人脸识别所需的数据
 * @param       index           : 要读取的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1
 * @param       buf             : 要读取的数据缓存区首地址
 * @param       size            : 要读取的数据大小
 * @retval      0, 成功; 其他, 取消设置;
 */
uint8_t atk_frec_read_data(uint8_t index, uint8_t *buf, uint32_t size)
{
    uint8_t *path;
    FIL *fp;
    uint32_t fr;
    uint8_t res;
    path = atk_frec_fastmalloc(30);         /* 申请内存 */
    fp = atk_frec_fastmalloc(sizeof(FIL));  /* 申请内存 */

    if (!fp)
    {
        atk_frec_fastfree(path);
        return ATK_FREC_MEMORY_ERR;
    }

    sprintf((char *)path, ATK_FREC_DATA_PNAME, index);
    res = f_open(fp, (char *)path, FA_READ);

    if (res == FR_OK && size)
    {
        res = f_read(fp, buf, size, &fr);   /* 读取文件 */

        if (fr == size)res = 0;
        else res = ATK_FREC_READ_WRITE_ERR;
    }

    f_close(fp);

    if (res)res = ATK_FREC_READ_WRITE_ERR;

    atk_frec_fastfree(path);
    atk_frec_fastfree(fp);
    return res;
}

/**
 * @brief       删除一个人脸数据
 * @param       index           : 要删除的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1
 * @retval      0, 成功; 其他, 取消设置;
 */
uint8_t atk_frec_delete_data(uint8_t index)
{
    uint8_t *path;
    uint8_t res;
    path = atk_frec_fastmalloc(30); /* 申请内存 */

    if (!path)
    {
        return ATK_FREC_MEMORY_ERR;
    }

    sprintf((char *)path, ATK_FREC_DATA_PNAME, index);
    res = f_unlink((char *)path);
    atk_frec_fastfree(path);
    return res;
}





