#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "pymb.h"
#include "pyinput.h"
#include "string.h"

/* 拼音输入法  */
pyinput t9 =
{
    get_pymb,
    0,
};

/**
 * @brief       比较两个字符串的匹配情况
 * @param       返回值:0xff,表示完全匹配
 * @retval      其他,匹配的字符数
 */
uint8_t str_match(uint8_t *str1, uint8_t *str2)
{
    uint8_t i = 0;

    while (1)
    {
        if (*str1 != *str2)break; /* 部分匹配 */

        if (*str1 == '\0')
        {
            i = 0XFF;            /* 完全匹配 */
            break;
        }

        i++;
        str1++;
        str2++;
    }

    return i; /* 两个字符串相等  */
}

/**
 * @brief       获取匹配的拼音码表
 * @param       strin:输入的字符串,形如:"726"
 * @param       matchlist:输出的匹配表
 * @retval      [7],0,表示完全匹配；1，表示部分匹配（仅在没有完全匹配的时候才会出现）
                [6:0],完全匹配的时候，表示完全匹配的拼音个数
                部分匹配的时候，表示有效匹配的位数
 */
uint8_t get_matched_pymb(uint8_t *strin, py_index **matchlist)
{
    py_index *bestmatch = 0; /* 最佳匹配 */
    uint16_t pyindex_len = 0;
    uint16_t i = 0;
    uint8_t temp, mcnt = 0, bmcnt = 0;
    bestmatch = (py_index *)&py_index3[0];                  /* 默认为a的匹配  */
    pyindex_len = sizeof(py_index3) / sizeof(py_index3[0]); /* 得到py索引表的大小. */

    for (i = 0; i < pyindex_len; i++)
    {
        temp = str_match(strin, (uint8_t *)py_index3[i].py_input);

        if (temp)
        {
            if (temp == 0XFF)matchlist[mcnt++] = (py_index *)&py_index3[i];
            else if (temp > bmcnt)                     /* 找最佳匹配 */
            {
                bmcnt = temp;
                bestmatch = (py_index *)&py_index3[i]; /* 最好的匹配. */
            }
        }
    }

    if (mcnt == 0 && bmcnt)  /* 没有完全匹配的结果,但是有部分匹配的结果 */
    {
        matchlist[0] = bestmatch;
        mcnt = bmcnt | 0X80; /* 返回部分匹配的有效位数 */
    }

    return mcnt;/* 返回匹配的个数 */
}

/**
 * @brief       得到拼音码表
 * @param       str:输入字符串
 * @retval      匹配个数
 */
uint8_t get_pymb(uint8_t *str)
{
    return get_matched_pymb(str, t9.pymb);
}
