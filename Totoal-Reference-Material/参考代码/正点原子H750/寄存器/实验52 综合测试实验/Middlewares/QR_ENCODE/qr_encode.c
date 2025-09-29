/**
 ****************************************************************************************************
 * @file        qr_encode.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-02
 * @brief       二维码编码 代码
 *  @note       本程序移植自网友旮旯旭的二维码生成工程, 特此感谢!
 *              二维码生成原理详见 http://coolshell.cn/articles/10590.html#jtss-tsina
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
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "qr_encode.h"
#include "qr_data.h"



_qr_encode *qrx;        /* 二维码编码,各种参数结构体 */


/**
 * @brief       编码数据
 * @param       lpsSource       : 源数据
 * @retval      0, 成功; 其他, 失败;
 */
uint8_t EncodeData(char *lpsSource)
{
    int i, j, nVersion = 1, bAutoExtent = 1, ncSource;
    int ncLength, nEncodeVersion, ncDataCodeu32, ncTerminater;
    uint8_t byPaddingCode = 0xec;
    int nDataCwIndex = 0, ncBlock1, ncBlock2, ncBlockSum;
    int nBlockNo = 0, ncDataCw1, ncDataCw2;
    int ncRSCw1, ncRSCw2;
    ncSource = 0;
    qrx->m_nLevel = 0;  /* 纠错等级为LOW,7%可修复 */
    qrx->m_nMaskingNo = 0;
    ncLength = ncSource > 0 ? ncSource : strlen(lpsSource);/* 计算要编码的数据的长度 */

    //printf("ncLength = %d\n",ncLength);
    if (ncLength == 0) return QR_FALSE;

    nEncodeVersion = GetEncodeVersion(nVersion, lpsSource, ncLength);/* 计算编码数据需要的版本 */

    //printf("nEncodeVersion = %d\n",nEncodeVersion);
    if (nEncodeVersion == 0) return QR_FALSE;

    if (nVersion == 0) qrx->m_nVersion = nEncodeVersion;
    else
    {
        if (nEncodeVersion <= nVersion) qrx->m_nVersion = nVersion;
        else
        {
            if (bAutoExtent)qrx->m_nVersion = nEncodeVersion;
            else return QR_FALSE;
        }
    }

    //printf("qrx->m_nVersion = %d\n",qrx->m_nVersion);
    ncDataCodeu32 = QR_VersonInfo[qrx->m_nVersion].ncDataCodeu32[qrx->m_nLevel];
    ncTerminater = min(4, (ncDataCodeu32 * 8) - qrx->m_ncDataCodeu32Bit);

    if (ncTerminater > 0)qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 0, ncTerminater);

    for (i = (qrx->m_ncDataCodeu32Bit + 7) / 8; i < ncDataCodeu32; ++i)
    {
        qrx->m_byDataCodeu32[i] = byPaddingCode;
        byPaddingCode = (uint8_t)(byPaddingCode == 0xec ? 0x11 : 0xec);
    }

    qrx->m_ncAllCodeu32 = QR_VersonInfo[qrx->m_nVersion].ncAllCodeu32;
    memset(qrx->m_byAllCodeu32, 0, qrx->m_ncAllCodeu32);
    ncBlock1 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo1[qrx->m_nLevel].ncRSBlock;
    ncBlock2 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo2[qrx->m_nLevel].ncRSBlock;
    ncBlockSum = ncBlock1 + ncBlock2;
    ncDataCw1 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo1[qrx->m_nLevel].ncDataCodeu32;
    ncDataCw2 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo2[qrx->m_nLevel].ncDataCodeu32;

    for (i = 0; i < ncBlock1; ++i)
    {
        for (j = 0; j < ncDataCw1; ++j)
        {
            qrx->m_byAllCodeu32[(ncBlockSum * j) + nBlockNo] = qrx->m_byDataCodeu32[nDataCwIndex++];
        }

        ++nBlockNo;
    }

    for (i = 0; i < ncBlock2; ++i)
    {
        for (j = 0; j < ncDataCw2; ++j)
        {
            if (j < ncDataCw1) qrx->m_byAllCodeu32[(ncBlockSum * j) + nBlockNo] = qrx->m_byDataCodeu32[nDataCwIndex++];
            else qrx->m_byAllCodeu32[(ncBlockSum * ncDataCw1) + i]  = qrx->m_byDataCodeu32[nDataCwIndex++];
        }

        ++nBlockNo;
    }

    ncRSCw1 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo1[qrx->m_nLevel].ncAllCodeu32 - ncDataCw1;
    ncRSCw2 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo2[qrx->m_nLevel].ncAllCodeu32 - ncDataCw2;
    nDataCwIndex = 0;
    nBlockNo = 0;

    for (i = 0; i < ncBlock1; ++i)
    {
        memset(qrx->m_byRSWork, 0, sizeof(qrx->m_byRSWork));
        memmove(qrx->m_byRSWork, qrx->m_byDataCodeu32 + nDataCwIndex, ncDataCw1);
        GetRSCodeu32(qrx->m_byRSWork, ncDataCw1, ncRSCw1);

        for (j = 0; j < ncRSCw1; ++j)
        {
            qrx->m_byAllCodeu32[ncDataCodeu32 + (ncBlockSum * j) + nBlockNo] = qrx->m_byRSWork[j];
        }

        nDataCwIndex += ncDataCw1;
        ++nBlockNo;
    }

    for (i = 0; i < ncBlock2; ++i)
    {
        memset(qrx->m_byRSWork, 0, sizeof(qrx->m_byRSWork));
        memmove(qrx->m_byRSWork, qrx->m_byDataCodeu32 + nDataCwIndex, ncDataCw2);
        GetRSCodeu32(qrx->m_byRSWork, ncDataCw2, ncRSCw2);

        for (j = 0; j < ncRSCw2; ++j)
        {
            qrx->m_byAllCodeu32[ncDataCodeu32 + (ncBlockSum * j) + nBlockNo] = qrx->m_byRSWork[j];
        }

        nDataCwIndex += ncDataCw2;
        ++nBlockNo;
    }

    qrx->m_nSymbleSize = qrx->m_nVersion * 4 + 17;
    FormatModule();
    return QR_TRUE;
}

/**
 * Number of bits per length field
 * Encoding  Ver.1–9     10–26       27–40
 * Numeric      10          12          14
 * Alphanumeric 9           11          13
 * u8           8           16          16
 * Kanji        8           10          12
 */

/**
 * @brief       根据位数长度返回版本
 * @param       nVersion        : 版本
 * @param       lpsSource       : 源数据
 * @param       ncLength        : 长度
 * @retval      版本
 */
int GetEncodeVersion(int nVersion, char *lpsSource, int ncLength)
{
    int nVerGroup = nVersion >= 27 ? QR_VRESION_L : (nVersion >= 10 ? QR_VRESION_M : QR_VRESION_S);
    int i, j;

    for (i = nVerGroup; i <= QR_VRESION_L; ++i)
    {
        if (EncodeSourceData(lpsSource, ncLength, i))
        {
            if (i == QR_VRESION_S)
            {
                for (j = 1; j <= 9; ++j)
                {
                    if ((qrx->m_ncDataCodeu32Bit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeu32[qrx->m_nLevel])
                        return j;
                }
            }
            else if (i == QR_VRESION_M)
            {
                for (j = 10; j <= 26; ++j)
                {
                    if ((qrx->m_ncDataCodeu32Bit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeu32[qrx->m_nLevel])
                        return j;
                }
            }
            else if (i == QR_VRESION_L)
            {
                for (j = 27; j <= 40; ++j)
                {
                    if ((qrx->m_ncDataCodeu32Bit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeu32[qrx->m_nLevel])
                        return j;
                }
            }
        }
    }

    return 0;
}

/**
 * @brief       模式指标，编码模式
 * @param       nVersion        : 版本
 * @param       lpsSource       : 源数据
 * @param       ncLength        : 长度
 * @param       nVerGroup       : 版本组
 * @retval      编码结果
 */
int EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup)
{
    int i, j;
    int ncSrcBits, ncDstBits;
    int nBlock = 0;
    int ncComplete = 0;
    uint32_t wBinCode;
    memset(qrx->m_nBlockLength, 0, sizeof(qrx->m_nBlockLength));

    for (qrx->m_ncDataBlock = i = 0; i < ncLength; ++i)
    {
        uint8_t byMode;

        if (i < ncLength - 1 && IsKanjiData(lpsSource[i], lpsSource[i + 1]))
            byMode = QR_MODE_KANJI;             /* 日文 */
        //else if(i < ncLength - 1 && IsChineseData(lpsSource[i], lpsSource[i + 1]))
        //byMode = QR_MODE_CHINESE;             /* 中文 */
        else if (IsNumeralData(lpsSource[i]))
            byMode = QR_MODE_NUMERAL;           /* 数字 */
        else if (IsAlphabetData(lpsSource[i]))
            byMode = QR_MODE_ALPHABET;          /* 字符 */
        else
            byMode = QR_MODE_8BIT;              /* 字节 */

        if (i == 0) qrx->m_byBlockMode[0] = byMode;

        if (qrx->m_byBlockMode[qrx->m_ncDataBlock] != byMode)/* 如果前后两个数据的模式不一样 */
            qrx->m_byBlockMode[++qrx->m_ncDataBlock] = byMode;

        ++qrx->m_nBlockLength[qrx->m_ncDataBlock];

        if (byMode == QR_MODE_KANJI)   /* 如果是日文模式，那么前后2个字节为一个汉字 */
        {
            ++qrx->m_nBlockLength[qrx->m_ncDataBlock];
            ++i;
        }
    }

    ++qrx->m_ncDataBlock;

    while (nBlock < qrx->m_ncDataBlock - 1)
    {
        int ncJoinFront, ncJoinBehind;
        int nJoinPosition = 0;

        if ((qrx->m_byBlockMode[nBlock] == QR_MODE_NUMERAL  && qrx->m_byBlockMode[nBlock + 1] == QR_MODE_ALPHABET) ||
                (qrx->m_byBlockMode[nBlock] == QR_MODE_ALPHABET && qrx->m_byBlockMode[nBlock + 1] == QR_MODE_NUMERAL))
        {
            /* 如果前后2个数据块为字母和数字，可以合并成字母数字模式(字母模式包含了数字)  计算数据位流 */
            ncSrcBits = GetBitLength(qrx->m_byBlockMode[nBlock], qrx->m_nBlockLength[nBlock], nVerGroup) +
                        GetBitLength(qrx->m_byBlockMode[nBlock + 1], qrx->m_nBlockLength[nBlock + 1], nVerGroup);
            ncDstBits = GetBitLength(QR_MODE_ALPHABET, qrx->m_nBlockLength[nBlock] + qrx->m_nBlockLength[nBlock + 1], nVerGroup);

            if (ncSrcBits > ncDstBits)
            {
                /* 如果合并前大于合并后长度 */
                if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)
                {
                    /* 判断之前的数据块是不是8bit模式，如果是的话合并数据(必须要有3个数据块以上判断) */
                    ncJoinFront = GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock - 1] + qrx->m_nBlockLength[nBlock], nVerGroup) +
                                  GetBitLength(qrx->m_byBlockMode[nBlock + 1], qrx->m_nBlockLength[nBlock + 1], nVerGroup);

                    if (ncJoinFront > ncDstBits + GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock - 1], nVerGroup))
                        ncJoinFront = 0; /* 如果合并后的数据超过长度 非法 */
                }
                else ncJoinFront = 0;   /* 不满足合并的条件 */

                if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
                {
                    /* 如果后面的数据块为8bit模式，合并数据(必须要有3个数据块以上判断) */
                    ncJoinBehind = GetBitLength(qrx->m_byBlockMode[nBlock], qrx->m_nBlockLength[nBlock], nVerGroup) +
                                   GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock + 1] + qrx->m_nBlockLength[nBlock + 2], nVerGroup);

                    if (ncJoinBehind > ncDstBits + GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock + 2], nVerGroup))
                        ncJoinBehind = 0; /* 如果合并后的数据超过长度 非法 */
                }
                else ncJoinBehind = 0;   /* 不满足合并的条件 */

                if (ncJoinFront != 0 && ncJoinBehind != 0)   /* 前后的数据块合并后都不为0 */
                {
                    nJoinPosition = (ncJoinFront < ncJoinBehind) ? -1 : 1;/* 位置的标记，如果前面的小于后面的为-1；大于等于为1 */
                }
                else     /* 如果有一个或者都为0  如果前面不为0为-1；后面不为0为1  否则为0 */
                {
                    nJoinPosition = (ncJoinFront != 0) ? -1 : ((ncJoinBehind != 0) ? 1 : 0);
                }

                if (nJoinPosition != 0)
                {
                    /* 如果合并后的数据不为0 */
                    if (nJoinPosition == -1)   /* 前面有数据 */
                    {
                        qrx->m_nBlockLength[nBlock - 1] += qrx->m_nBlockLength[nBlock];

                        for (i = nBlock; i < qrx->m_ncDataBlock - 1; ++i)
                        {
                            qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                            qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                        }
                    }
                    else
                    {
                        /* 后面有数据 */
                        qrx->m_byBlockMode[nBlock + 1] = QR_MODE_8BIT;/* 后面数据块为8BIT模式，将新的数据块模式定义成8BIT */
                        qrx->m_nBlockLength[nBlock + 1] += qrx->m_nBlockLength[nBlock + 2];/* 长度 */

                        for (i = nBlock + 2; i < qrx->m_ncDataBlock - 1; ++i)
                        {
                            qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                            qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                        }
                    }

                    --qrx->m_ncDataBlock;
                }
                else
                {
                    /* 合并后数据异常，或者不满足合并条件 */
                    if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_ALPHABET)
                    {
                        /* 如果后面一块数据也是字母数字，合并数据 */
                        qrx->m_nBlockLength[nBlock + 1] += qrx->m_nBlockLength[nBlock + 2];

                        for (i = nBlock + 2; i < qrx->m_ncDataBlock - 1; ++i)
                        {
                            qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                            qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                        }

                        --qrx->m_ncDataBlock;
                    }

                    /* 设置新数据块为字母数字 */
                    qrx->m_byBlockMode[nBlock] = QR_MODE_ALPHABET;
                    qrx->m_nBlockLength[nBlock] += qrx->m_nBlockLength[nBlock + 1];

                    for (i = nBlock + 1; i < qrx->m_ncDataBlock - 1; ++i)
                    {
                        qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                        qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                    }

                    --qrx->m_ncDataBlock;

                    /* 如果前面一块数据也是字母数字，合并数据 */
                    if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_ALPHABET)
                    {
                        qrx->m_nBlockLength[nBlock - 1] += qrx->m_nBlockLength[nBlock];

                        for (i = nBlock; i < qrx->m_ncDataBlock - 1; ++i)
                        {
                            qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                            qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                        }

                        --qrx->m_ncDataBlock;
                    }
                }

                continue;
            }
        }

        ++nBlock;
    }

    nBlock = 0;

    while (nBlock < qrx->m_ncDataBlock - 1)
    {
        /* 合并2个部分数据 */
        ncSrcBits = GetBitLength(qrx->m_byBlockMode[nBlock], qrx->m_nBlockLength[nBlock], nVerGroup)
                    + GetBitLength(qrx->m_byBlockMode[nBlock + 1], qrx->m_nBlockLength[nBlock + 1], nVerGroup);
        ncDstBits = GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock] + qrx->m_nBlockLength[nBlock + 1], nVerGroup);

        if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)/* 前一数据块为BIT模式 */
            ncDstBits -= (4 + nIndicatorLen8Bit[nVerGroup]);

        if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
            ncDstBits -= (4 + nIndicatorLen8Bit[nVerGroup]);

        if (ncSrcBits > ncDstBits)
        {
            /* 如果合并后数据增加，前一块数据是8bit数据 */
            if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)
            {
                qrx->m_nBlockLength[nBlock - 1] += qrx->m_nBlockLength[nBlock];

                for (i = nBlock; i < qrx->m_ncDataBlock - 1; ++i)
                {
                    qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                    qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                }

                --qrx->m_ncDataBlock;
                --nBlock;
            }

            if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
            {
                /* 数据块+2也是8bit */
                qrx->m_nBlockLength[nBlock + 1] += qrx->m_nBlockLength[nBlock + 2];

                for (i = nBlock + 2; i < qrx->m_ncDataBlock - 1; ++i)
                {
                    qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                    qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
                }

                --qrx->m_ncDataBlock;
            }

            /* 设置新的数据块为8BIT模式 */
            qrx->m_byBlockMode[nBlock] = QR_MODE_8BIT;
            qrx->m_nBlockLength[nBlock] += qrx->m_nBlockLength[nBlock + 1];

            for (i = nBlock + 1; i < qrx->m_ncDataBlock - 1; ++i)
            {
                qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
                qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
            }

            --qrx->m_ncDataBlock;

            if (nBlock >= 1) --nBlock;

            continue;
        }

        ++nBlock;
    }

    qrx->m_ncDataCodeu32Bit = 0;
    memset(qrx->m_byDataCodeu32, 0, MAX_DATACODEu32);

    for (i = 0; i < qrx->m_ncDataBlock && qrx->m_ncDataCodeu32Bit != -1; ++i)
    {
        if (qrx->m_byBlockMode[i] == QR_MODE_NUMERAL)
        {
            /* 如果数据块是数字模式 */
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 1, 4);
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (uint32_t)qrx->m_nBlockLength[i], nIndicatorLenNumeral[nVerGroup]);

            for (j = 0; j < qrx->m_nBlockLength[i]; j += 3)
            {
                if (j < qrx->m_nBlockLength[i] - 2)
                {
                    wBinCode = (uint32_t)(((lpsSource[ncComplete + j]	  - '0') * 100) +
                                          ((lpsSource[ncComplete + j + 1] - '0') * 10) +
                                          (lpsSource[ncComplete + j + 2] - '0'));
                    qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 10);
                }
                else if (j == qrx->m_nBlockLength[i] - 2)
                {
                    wBinCode = (uint32_t)(((lpsSource[ncComplete + j] - '0') * 10) +
                                          (lpsSource[ncComplete + j + 1] - '0'));
                    qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 7);
                }
                else if (j == qrx->m_nBlockLength[i] - 1)
                {
                    wBinCode = (uint32_t)(lpsSource[ncComplete + j] - '0');
                    qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 4);
                }
            }

            ncComplete += qrx->m_nBlockLength[i];
        }
        else if (qrx->m_byBlockMode[i] == QR_MODE_ALPHABET)
        {
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 2, 4);
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (uint32_t)qrx->m_nBlockLength[i], nIndicatorLenAlphabet[nVerGroup]);

            for (j = 0; j < qrx->m_nBlockLength[i]; j += 2)
            {
                if (j < qrx->m_nBlockLength[i] - 1)
                {
                    wBinCode = (uint32_t)((AlphabetToBinaly(lpsSource[ncComplete + j]) * 45) +
                                          AlphabetToBinaly(lpsSource[ncComplete + j + 1]));
                    qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 11);
                }
                else
                {
                    wBinCode = (uint32_t)AlphabetToBinaly(lpsSource[ncComplete + j]);
                    qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 6);
                }
            }

            ncComplete += qrx->m_nBlockLength[i];
        }
        else if (qrx->m_byBlockMode[i] == QR_MODE_8BIT)
        {
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 4, 4);
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (uint32_t)qrx->m_nBlockLength[i], nIndicatorLen8Bit[nVerGroup]);

            for (j = 0; j < qrx->m_nBlockLength[i]; ++j)
            {
                qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (uint32_t)lpsSource[ncComplete + j], 8);
            }

            ncComplete += qrx->m_nBlockLength[i];
        }
        else  //if(qrx->m_byBlockMode[i] == QR_MODE_KANJI)
        {
            /* 日本模式 */
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 8, 4);
            qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (uint32_t)(qrx->m_nBlockLength[i] / 2), nIndicatorLenKanji[nVerGroup]);

            for (j = 0; j < qrx->m_nBlockLength[i] / 2; ++j)
            {
                uint32_t wBinCode = KanjiToBinaly((uint32_t)(((uint8_t)lpsSource[ncComplete + (j * 2)] << 8) + (uint8_t)lpsSource[ncComplete + (j * 2) + 1]));
                qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 13);
            }

            ncComplete += qrx->m_nBlockLength[i];
        }

//         else
//         {
//             /* 中文模式 */
//             qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 8, 4);

//             qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (uint32_t)(qrx->m_nBlockLength[i] / 2), nIndicatorLenKanji[nVerGroup]);

//             for (j = 0; j < qrx->m_nBlockLength[i] / 2; ++j)
//             {
//                 uint32_t wBinCode = ChineseToBinaly((uint32_t)(((uint8_t)lpsSource[ncComplete + (j * 2)] << 8) + (uint8_t)lpsSource[ncComplete + (j * 2) + 1]));

//                 qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 13);
//             }

//             ncComplete += qrx->m_nBlockLength[i];
//         }
    }

    return (qrx->m_ncDataCodeu32Bit != -1);
}

/**
 * @brief       获取bit长度
 * @param       nMode           : 模式
 * @param       ncData          : 源数据
 * @param       nVerGroup       : 版本组
 * @retval      bit长度
 */
int GetBitLength(uint8_t nMode, int ncData, int nVerGroup)
{
    int ncBits = 0;

    switch (nMode)
    {
        case QR_MODE_NUMERAL:
            ncBits = 4 + nIndicatorLenNumeral[nVerGroup] + (10 * (ncData / 3));

            switch (ncData % 3)
            {
                case 1:
                    ncBits += 4;
                    break;

                case 2:
                    ncBits += 7;
                    break;

                default:
                    break;
            }

            break;

        case QR_MODE_ALPHABET:
            ncBits = 4 + nIndicatorLenAlphabet[nVerGroup] + (11 * (ncData / 2)) + (6 * (ncData % 2));
            break;

        case QR_MODE_8BIT:
            ncBits = 4 + nIndicatorLen8Bit[nVerGroup] + (8 * ncData);
            break;

        default:
            //ncBits = 4 + nIndicatorLenKanji[nVerGroup] + (13 * (ncData / 2));/* 本程序写法？ */
            ncBits = 4 + nIndicatorLenKanji[nVerGroup] + (13 * ncData);/* 文档算法 */
            break;
    }

    return ncBits;
}

/**
 * @brief       设置bit流
 * @param       nIndex          : 索引
 * @param       wData           : 数据
 * @param       ncData          : 数据
 * @retval      nIndex + ncData
 */
int SetBitStream(int nIndex, uint32_t wData, int ncData)
{
    int i;

    if (nIndex == -1 || nIndex + ncData > MAX_DATACODEu32 * 8) return -1;

    for (i = 0; i < ncData; ++i)
    {
        if (wData & (1 << (ncData - i - 1)))
        {
            qrx->m_byDataCodeu32[(nIndex + i) / 8] |= 1 << (7 - ((nIndex + i) % 8));
        }
    }

    return nIndex + ncData;
}

/**
 * @brief       是不是数字编码信息
 * @param       c               : 编码
 * @retval      QR_TRUE,  是
 *              QR_FALSE, 不是
 */
uint8_t IsNumeralData(unsigned char c)
{
    if (c >= '0' && c <= '9') return QR_TRUE;

    return QR_FALSE;
}

/**
 * @brief       是否是字符编码信息
 * @param       c               : 编码
 * @retval      QR_TRUE,  是
 *              QR_FALSE, 不是
 */
uint8_t IsAlphabetData(unsigned char c)
{
    if (c >= '0' && c <= '9') return QR_TRUE;

    if (c >= 'A' && c <= 'Z') return QR_TRUE;

    if (c == ' ' || c == '$' || c == '%' || c == '*' || c == '+' || c == '-' || c == '.' || c == '/' || c == ':')return QR_TRUE;

    return QR_FALSE;
}

/**
 * @brief       是否是日文编码
 * @param       c1              : 编码1
 * @param       c2              : 编码2
 * @retval      QR_TRUE,  是
 *              QR_FALSE, 不是
 */
uint8_t IsKanjiData(unsigned char c1, unsigned char c2)
{

    if (((c1 >= 0x81 && c1 <= 0x9f) || (c1 >= 0xe0 && c1 <= 0xeb)) && (c2 >= 0x40))
    {
        if ((c1 == 0x9f && c2 > 0xfc) || (c1 == 0xeb && c2 > 0xbf)) return QR_FALSE;

        return QR_TRUE;
    }

    return QR_FALSE;
}

/**
 * @brief       是否是中文编码
 * @param       c1              : 编码1
 * @param       c2              : 编码2
 * @retval      QR_TRUE,  是
 *              QR_FALSE, 不是
 */
uint8_t IsChineseData(unsigned char c1, unsigned char c2)
{
    /* 中文模式 */
    if ((c1 >= 0xa1 && c1 < 0xaa) || (c1 >= 0xb0 && c1 <= 0xfa))
    {
        if (c2 >= 0xa1 && c2 <= 0xfe)return QR_TRUE;
    }

    return QR_FALSE;
}

/**
 * @brief       字母表信息转化
 * @param       c               : 编码
 * @retval      转换后的值
 */
uint8_t AlphabetToBinaly(unsigned char c)
{
    if (c >= '0' && c <= '9') return (unsigned char)(c - '0');

    if (c >= 'A' && c <= 'Z') return (unsigned char)(c - 'A' + 10);

    if (c == ' ') return 36;

    if (c == '$') return 37;

    if (c == '%') return 38;

    if (c == '*') return 39;

    if (c == '+') return 40;

    if (c == '-') return 41;

    if (c == '.') return 42;

    if (c == '/') return 43;

    return 44;
}

/**
 * @brief       日文中文信息转化
 *  @note       日文和汉字的编码会减去一个值
 *              如：在0X8140 to 0X9FFC中的字符会减去8140，
 *              在0XE040到0XEBBF中的字符要减去0XC140，
 *              然后把结果前两个16进制位拿出来乘以0XC0，
 *              然后再加上后两个16进制位，最后转成13bit的编码。
 * @param       wc              : 编码
 * @retval      转换后的编码
 */
uint32_t KanjiToBinaly(uint32_t wc)
{
    if (wc >= 0x8140 && wc <= 0x9ffc)wc -= 0x8140;
    else wc -= 0xc140;

    return (uint32_t)(((wc >> 8) * 0xc0) + (wc & 0x00ff));
}

/**
 * @brief       中文信息转化
 *  @note       中文汉字的与日文汉字转换步骤相似：
 *              1、对于第一字节为0xA1~0xAA之间,第二字节在0xA1~0xFE之间字符：
 *              a)第一字节减去0xA1；
 *              b)上一步结果乘以0x60;
 *              c）第二字节减去0xA1;
 *              d)将b)步骤的结果加上c步骤的结果;
 *              e)将结果转换为13位二进制串。
 *              1、对于第一字节为0xB0~0xFA之间,第二字节在0xA1~0xFE之间字符：
 *              a)第一字节减去0xA6；
 *              b)上一步结果乘以0x60;
 *              c）第二字节减去0xA1;
 *              d)将b)步骤的结果加上c步骤的结果;
 *              e)将结果转换为13位二进制串
 * @param       wc              : 编码
 * @retval      转换后的编码
 */
uint32_t ChineseToBinaly(uint32_t wc)
{
    if (wc >= 0xa1a1 && wc <= 0xa9fe)
    {
        return (uint32_t)((((wc >> 8) - 0xa1) * 0x60) + ((wc & 0x00ff) - 0xa1));
    }

    if (wc >= 0xb0a1 && wc <= 0xf9fe)
    {
        return (uint32_t)((((wc >> 8) - 0xa6) * 0x60) + ((wc & 0x00ff) - 0xa1));
    }

    return (uint32_t)((((wc >> 8) - 0xa6) * 0x60) + ((wc & 0x00ff) - 0xa1));
}

/* 纠错码 */
void GetRSCodeu32(uint8_t *lpbyRSWork, int ncDataCodeu32, int ncRSCodeu32)
{
    int i, j;

    for (i = 0; i < ncDataCodeu32 ; ++i)
    {
        if (lpbyRSWork[0] != 0)
        {
            uint8_t nExpFirst = byIntToExp[lpbyRSWork[0]];

            for (j = 0; j < ncRSCodeu32; ++j)
            {
                uint8_t nExpElement = (uint8_t)(((int)(byRSExp[ncRSCodeu32][j] + nExpFirst)) % 255);
                lpbyRSWork[j] = (uint8_t)(lpbyRSWork[j + 1] ^ byExpToInt[nExpElement]);
            }

            for (j = ncRSCodeu32; j < ncDataCodeu32 + ncRSCodeu32 - 1; ++j) lpbyRSWork[j] = lpbyRSWork[j + 1];
        }
        else
        {
            for (j = 0; j < ncDataCodeu32 + ncRSCodeu32 - 1; ++j) lpbyRSWork[j] = lpbyRSWork[j + 1];
        }
    }
}

/**
 * @brief       格式化数据存放, 存在于所有的尺寸中
 *  @note       详见http://coolshell.cn/articles/10590.html#jtss-tsina
 * @param       无
 * @retval      无
 */
void FormatModule(void)
{
    int i, j;
    int nMinPenalty, nPenalty;
    memset(qrx->m_byModuleData, 0, sizeof(qrx->m_byModuleData));
    SetFunctionModule();    /* 首先把回字图案花在三个角上 */
    SetCodeu32Pattern(); 

    if (qrx->m_nMaskingNo == -1)
    {
        qrx->m_nMaskingNo = 0;
        SetMaskingPattern(qrx->m_nMaskingNo);
        SetFormatInfoPattern(qrx->m_nMaskingNo);
        nMinPenalty = CountPenalty();

        for (i = 1; i <= 7; ++i)
        {
            SetMaskingPattern(i);
            SetFormatInfoPattern(i);
            nPenalty = CountPenalty();

            if (nPenalty < nMinPenalty)
            {
                nMinPenalty = nPenalty;
                qrx->m_nMaskingNo = i;
            }
        }
    }

    SetMaskingPattern(qrx->m_nMaskingNo);
    SetFormatInfoPattern(qrx->m_nMaskingNo);

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize; ++j)
        {
            qrx->m_byModuleData[i][j] = (uint8_t)((qrx->m_byModuleData[i][j] & 0x11) != 0);
        }
    }
}

/**
 * @brief       设置功能性模块信息
 *  @note       Position Detection Pattern	用于标记二维码举行大小
 *              Separators for Postion Detection Patterns	三个就可以标记一个矩形
 *              Timing Patterns也是用于定位的。原因是二维码有40种尺寸，尺寸过大了后需要有根标准线，不然扫描的时候可能会扫歪了
 *              Alignment Patterns 只有Version 2以上（包括Version2）的二维码需要这个东东，同样是为了定位用的。
 * @param       无
 * @retval      无
 */
void SetFunctionModule(void)
{
    int i, j;

    SetFinderPattern(0, 0); /* 三个回字坐标 */
    SetFinderPattern(qrx->m_nSymbleSize - 7, 0);
    SetFinderPattern(0, qrx->m_nSymbleSize - 7);

    for (i = 0; i < 8; ++i)
    {
        qrx->m_byModuleData[i][7] = qrx->m_byModuleData[7][i] = '\x20';
        qrx->m_byModuleData[qrx->m_nSymbleSize - 8][i] = qrx->m_byModuleData[qrx->m_nSymbleSize - 8 + i][7] = '\x20';
        qrx->m_byModuleData[i][qrx->m_nSymbleSize - 8] = qrx->m_byModuleData[7][qrx->m_nSymbleSize - 8 + i] = '\x20';
    }

    for (i = 0; i < 9; ++i)
    {
        qrx->m_byModuleData[i][8] = qrx->m_byModuleData[8][i] = '\x20';
    }

    for (i = 0; i < 8; ++i)
    {
        qrx->m_byModuleData[qrx->m_nSymbleSize - 8 + i][8] = qrx->m_byModuleData[8][qrx->m_nSymbleSize - 8 + i] = '\x20';
    }

    SetVersionPattern();	/* 版本信息 */

    for (i = 0; i < QR_VersonInfo[qrx->m_nVersion].ncAlignPoint; ++i)
    {
        SetAlignmentPattern(QR_VersonInfo[qrx->m_nVersion].nAlignPoint[i], 6);/* 小回字 */
        SetAlignmentPattern(6, QR_VersonInfo[qrx->m_nVersion].nAlignPoint[i]);

        for (j = 0; j < QR_VersonInfo[qrx->m_nVersion].ncAlignPoint; ++j)
        {
            SetAlignmentPattern(QR_VersonInfo[qrx->m_nVersion].nAlignPoint[i], QR_VersonInfo[qrx->m_nVersion].nAlignPoint[j]);
        }
    }

    for (i = 8; i <= qrx->m_nSymbleSize - 9; ++i)
    {
        qrx->m_byModuleData[i][6] = (i % 2) == 0 ? '\x30' : '\x20';
        qrx->m_byModuleData[6][i] = (i % 2) == 0 ? '\x30' : '\x20';
    }
}

/**
 * @brief       设置大回字信息
 *  @note        O X X X X X X X
 *               O X O O O O O X
 *               O X O X X X O X
 *               O X O X X X O X
 *               O X O X X X O X
 *               O X O O O O O X
 *               O X X X X X X X
 * @param       x,y             : 坐标
 * @retval      无
 */
void SetFinderPattern(int x, int y)
{
    const uint8_t byPattern[] = {0x7f, 0x41, 0x5d, 0x5d, 0x5d, 0x41, 0x7f};
    int i, j;

    for (i = 0; i < 7; ++i)
    {
        for (j = 0; j < 7; ++j)
        {
            qrx->m_byModuleData[x + j][y + i] = (byPattern[i] & (1 << (6 - j))) ? '\x30' : '\x20';
        }
    }
}

/**
 * @brief       设置小回字信息
 *  @note       Alignment Patterns 只有Version 2以上（包括Version2）的二维码需要这个东东，同样是为了定位用的。
 *              Alignment Patterns是除了3个大的回字，较小的回字
 *              O X X X X X X X
 *              O O O X X X X X
 *              O O O X O O O X
 *              O O O X O X O X
 *              O O O X O O O X
 *              O O O X X X X X
 * @param       x,y             : 坐标
 * @retval      无
 */
void SetAlignmentPattern(int x, int y)
{
    const uint8_t byPattern[] = {0x1f, 0x11, 0x15, 0x11, 0x1f};
    int i, j;

    if (qrx->m_byModuleData[x][y] & 0x20)return;

    x -= 2;
    y -= 2;

    for (i = 0; i < 5; ++i)
    {
        for (j = 0; j < 5; ++j)
        {
            qrx->m_byModuleData[x + j][y + i] = (byPattern[i] & (1 << (4 - j))) ? '\x30' : '\x20';
        }
    }
}

/**
 * @brief       设置版本
 *  @note       在 >= Version 7以上，需要预留两块3 x 6的区域存放一些版本信息
 * @param       无
 * @retval      无
 */
void SetVersionPattern(void)
{
    int i, j;
    int nVerData;

    if (qrx->m_nVersion <= 6)return;/* 在 >= Version 7以上，需要预留两块3 x 6的区域存放一些版本信息 */

    nVerData = qrx->m_nVersion << 12;

    for (i = 0; i < 6; ++i)
    {
        if (nVerData & (1 << (17 - i)))
        {
            nVerData ^= (0x1f25 << (5 - i));
        }
    }

    nVerData += qrx->m_nVersion << 12;

    for (i = 0; i < 6; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            qrx->m_byModuleData[qrx->m_nSymbleSize - 11 + j][i] = qrx->m_byModuleData[i][qrx->m_nSymbleSize - 11 + j] =
                        (nVerData & (1 << (i * 3 + j))) ? '\x30' : '\x20';
        }
    }
}

/**
 * @brief       设置分区
 *  @note       如果你以为我们可以开始画图，你就错了
 *              二维码的混乱技术还没有玩完，它还要把数据码和纠错码的各个codeuint32_ts交替放在一起。
 *              如何交替呢，规则如下：
 *              对于数据码：把每个块的第一个codeuint32_ts先拿出来按顺度排列好，
 *              然后再取第一块的第二个，如此类推
 * @param       无
 * @retval      无
 */
void SetCodeu32Pattern(void)
{
    int x = qrx->m_nSymbleSize;
    int y = qrx->m_nSymbleSize - 1;
    int nCoef_x = 1;
    int nCoef_y = 1;
    int i, j;

    for (i = 0; i < qrx->m_ncAllCodeu32; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            do
            {
                x += nCoef_x;
                nCoef_x *= -1;

                if (nCoef_x < 0)
                {
                    y += nCoef_y;

                    if (y < 0 || y == qrx->m_nSymbleSize)
                    {
                        y = (y < 0) ? 0 : qrx->m_nSymbleSize - 1;
                        nCoef_y *= -1;
                        x -= 2;

                        if (x == 6) --x;
                    }
                }
            } while (qrx->m_byModuleData[x][y] & 0x20);

            qrx->m_byModuleData[x][y] = (qrx->m_byAllCodeu32[i] & (1 << (7 - j))) ? '\x02' : '\x00';
        }
    }
}

/**
 * @brief       设置遮蔽信息
 * @param       nPatternNo      : 分区号
 * @retval      无
 */
void SetMaskingPattern(int nPatternNo)
{
    int i, j;
    uint8_t bMask;

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize; ++j)
        {
            if (! (qrx->m_byModuleData[j][i] & 0x20))
            {
                switch (nPatternNo)
                {
                    case 0:
                        bMask = ((i + j) % 2 == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    case 1:
                        bMask = (i % 2 == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    case 2:
                        bMask = (j % 3 == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    case 3:
                        bMask = ((i + j) % 3 == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    case 4:
                        bMask = (((i / 2) + (j / 3)) % 2 == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    case 5:
                        bMask = (((i * j) % 2) + ((i * j) % 3) == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    case 6:
                        bMask = ((((i * j) % 2) + ((i * j) % 3)) % 2 == 0) ? QR_TRUE : QR_FALSE;
                        break;

                    default:
                        bMask = ((((i * j) % 3) + ((i + j) % 2)) % 2 == 0) ? QR_TRUE : QR_FALSE;
                        break;
                }

                qrx->m_byModuleData[j][i] = (uint8_t)((qrx->m_byModuleData[j][i] & 0xfe) | (((qrx->m_byModuleData[j][i] & 0x02) > 1) ^ bMask));
            }
        }
    }
}

/**
 * @brief       设置格式化信息
 * @param       nPatternNo      : 分区号
 * @retval      无
 */
void SetFormatInfoPattern(int nPatternNo)
{
    int nFormatInfo;
    int i;
    int nFormatData;

    switch (qrx->m_nLevel)
    {
        case QR_LEVEL_M:
            nFormatInfo = 0x00;
            break;

        case QR_LEVEL_L:
            nFormatInfo = 0x08;
            break;

        case QR_LEVEL_Q:
            nFormatInfo = 0x18;
            break;

        default:
            nFormatInfo = 0x10;
            break;
    }

    nFormatInfo += nPatternNo;
    nFormatData = nFormatInfo << 10;

    for (i = 0; i < 5; ++i)
    {
        if (nFormatData & (1 << (14 - i)))
        {
            nFormatData ^= (0x0537 << (4 - i));
        }
    }

    nFormatData += nFormatInfo << 10;
    nFormatData ^= 0x5412;

    for (i = 0; i <= 5; ++i)qrx->m_byModuleData[8][i] = (nFormatData & (1 << i)) ? '\x30' : '\x20';

    qrx->m_byModuleData[8][7] = (nFormatData & (1 << 6)) ? '\x30' : '\x20';
    qrx->m_byModuleData[8][8] = (nFormatData & (1 << 7)) ? '\x30' : '\x20';
    qrx->m_byModuleData[7][8] = (nFormatData & (1 << 8)) ? '\x30' : '\x20';

    for (i = 9; i <= 14; ++i)qrx->m_byModuleData[14 - i][8] = (nFormatData & (1 << i)) ? '\x30' : '\x20';

    for (i = 0; i <= 7; ++i)qrx->m_byModuleData[qrx->m_nSymbleSize - 1 - i][8] = (nFormatData & (1 << i)) ? '\x30' : '\x20';

    qrx->m_byModuleData[8][qrx->m_nSymbleSize - 8] = '\x30';

    for (i = 8; i <= 14; ++i)qrx->m_byModuleData[8][qrx->m_nSymbleSize - 15 + i] = (nFormatData & (1 << i)) ? '\x30' : '\x20';
}

int CountPenalty(void)
{
    int nPenalty = 0;
    int i, j, k;
    int nCount = 0, s_nCount;

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize - 4; ++j)
        {
            int nCount = 1;

            for (k = j + 1; k < qrx->m_nSymbleSize; k++)
            {
                if (((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i][k] & 0x11) == 0))
                    ++nCount;
                else break;
            }

            if (nCount >= 5)nPenalty += 3 + (nCount - 5);

            j = k - 1;
        }
    }

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize - 4; ++j)
        {
            int nCount = 1;

            for (k = j + 1; k < qrx->m_nSymbleSize; k++)
            {
                if (((qrx->m_byModuleData[j][i] & 0x11) == 0) == ((qrx->m_byModuleData[k][i] & 0x11) == 0))
                    ++nCount;
                else break;
            }

            if (nCount >= 5)nPenalty += 3 + (nCount - 5);

            j = k - 1;
        }
    }

    for (i = 0; i < qrx->m_nSymbleSize - 1; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize - 1; ++j)
        {
            if ((((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i + 1][j]		& 0x11) == 0)) &&
                    (((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i]	[j + 1] & 0x11) == 0)) &&
                    (((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i + 1][j + 1] & 0x11) == 0)))
            {
                nPenalty += 3;
            }
        }
    }

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize - 6; ++j)
        {
            if (((j == 0) || (! (qrx->m_byModuleData[i][j - 1] & 0x11))) && /*  柧 傑偨偼 僔儞儃儖奜 */
                    (   qrx->m_byModuleData[i][j]     & 0x11)   && /*  埫 - 1 */
                    (! (qrx->m_byModuleData[i][j + 1] & 0x11))  && /*  柧 - 1 */
                    (   qrx->m_byModuleData[i][j + 2] & 0x11)   && /*  埫 劉 */
                    (   qrx->m_byModuleData[i][j + 3] & 0x11)   && /*  埫 劆3 */
                    (   qrx->m_byModuleData[i][j + 4] & 0x11)   && /*  埫 劊 */
                    (! (qrx->m_byModuleData[i][j + 5] & 0x11))  && /*  柧 - 1 */
                    (   qrx->m_byModuleData[i][j + 6] & 0x11)   && /*  埫 - 1 */
                    ((j == qrx->m_nSymbleSize - 7) || (! (qrx->m_byModuleData[i][j + 7] & 0x11))))   /*  柧 傑偨偼 僔儞儃儖奜 */
            {
                if (((j < 2 || ! (qrx->m_byModuleData[i][j - 2] & 0x11)) &&
                        (j < 3 || ! (qrx->m_byModuleData[i][j - 3] & 0x11)) &&
                        (j < 4 || ! (qrx->m_byModuleData[i][j - 4] & 0x11))) ||
                        ((j >= qrx->m_nSymbleSize - 8  || ! (qrx->m_byModuleData[i][j + 8]  & 0x11)) &&
                         (j >= qrx->m_nSymbleSize - 9  || ! (qrx->m_byModuleData[i][j + 9]  & 0x11)) &&
                         (j >= qrx->m_nSymbleSize - 10 || ! (qrx->m_byModuleData[i][j + 10] & 0x11))))
                {
                    nPenalty += 40;
                }
            }
        }
    }

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize - 6; ++j)
        {
            if (((j == 0) || (! (qrx->m_byModuleData[j - 1][i] & 0x11))) && /*  柧 傑偨偼 僔儞儃儖奜 */
                    (   qrx->m_byModuleData[j]    [i] & 0x11)   && /*  埫 - 1 */
                    (! (qrx->m_byModuleData[j + 1][i] & 0x11))  && /*  柧 - 1 */
                    (   qrx->m_byModuleData[j + 2][i] & 0x11)   && /*  埫 劉 */
                    (   qrx->m_byModuleData[j + 3][i] & 0x11)   && /*  埫 劆3 */
                    (   qrx->m_byModuleData[j + 4][i] & 0x11)   && /*  埫 劊 */
                    (! (qrx->m_byModuleData[j + 5][i] & 0x11))  && /*  柧 - 1 */
                    (   qrx->m_byModuleData[j + 6][i] & 0x11)   && /*  埫 - 1 */
                    ((j == qrx->m_nSymbleSize - 7) || (! (qrx->m_byModuleData[j + 7][i] & 0x11))))   /*  柧 傑偨偼 僔儞儃儖奜 */
            {
                if (((j < 2 || ! (qrx->m_byModuleData[j - 2][i] & 0x11)) &&
                        (j < 3 || ! (qrx->m_byModuleData[j - 3][i] & 0x11)) &&
                        (j < 4 || ! (qrx->m_byModuleData[j - 4][i] & 0x11))) ||
                        ((j >= qrx->m_nSymbleSize - 8  || ! (qrx->m_byModuleData[j + 8][i]  & 0x11)) &&
                         (j >= qrx->m_nSymbleSize - 9  || ! (qrx->m_byModuleData[j + 9][i]  & 0x11)) &&
                         (j >= qrx->m_nSymbleSize - 10 || ! (qrx->m_byModuleData[j + 10][i] & 0x11))))
                {
                    nPenalty += 40;
                }
            }
        }
    }

    for (i = 0; i < qrx->m_nSymbleSize; ++i)
    {
        for (j = 0; j < qrx->m_nSymbleSize; ++j)
        {
            if (! (qrx->m_byModuleData[i][j] & 0x11))
            {
                ++nCount;
            }
        }
    }

    if ( (50 - ((nCount * 100) / (qrx->m_nSymbleSize * qrx->m_nSymbleSize))) > 0) s_nCount = 50 - ((nCount * 100) / (qrx->m_nSymbleSize * qrx->m_nSymbleSize));
    else s_nCount = 0 - (50 - ((nCount * 100) / (qrx->m_nSymbleSize * qrx->m_nSymbleSize)));

    nPenalty += (s_nCount / 5) * 10;
    return nPenalty;
}















