/**
 ****************************************************************************************************
 * @file        gsm.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-27
 * @brief       GSM模块 驱动代码
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
 * V1.0 20220527
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "./BSP/GSM/gsm.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./MALLOC/malloc.h"
#include "./TEXT/text.h"
#include "./BSP/USART2/usart2.h"
#include "./FATFS/source/ff.h"
#include "ucos_ii.h"


__gsmdev gsmdev;    /* gsm控制器 */


/**
 * @brief       gsm发送命令后,检测接收到的应答
 * @param       str     : 期待的应答结果
 * @retval      0, 没有得到期待的应答结果
 *              其他, 期待应答结果的位置(str的位置)
 */
uint8_t *gsm_check_cmd(uint8_t *str)
{
    char *strx = 0;

    if (USART2_RX_STA & 0X8000) /* 接收到一次数据了 */
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0;  /* 添加结束符 */
        strx = strstr((const char *)USART2_RX_BUF, (const char *)str);
    }

    return (uint8_t *)strx;
}

/**
 * @brief       向gsm发送命令
 * @param       cmd     : 发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
 * @param       cmd     : 期待的应答结果,如果为空,则表示不需要等待应答
 * @param       waittime: 等待时间(单位:10ms)
 * @retval      0,发送成功(得到了期待的应答结果)
 *              1,收到非预期结果
 *              2,没收到任何回复
 */
uint8_t gsm_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t waittime)
{
    uint8_t res = 0;
    USART2_RX_STA = 0;
    gsmdev.cmdon = 1;   /* 进入指令等待状态 */

    if ((uint32_t)cmd <= 0XFF)
    {
        while ((USART2->ISR & 0X40) == 0);  /* 等待上一次数据发送完成 */

        USART2->TDR = (uint32_t)cmd;
    }
    else usart2_printf("%s\r\n", cmd);      /* 发送命令 */

    if (ack && waittime)        /* 需要等待应答 */
    {
        while (--waittime)      /* 等待倒计时 */
        {
            delay_ms(10);

            if (USART2_RX_STA & 0X8000)     /* 是否接收到期待的应答结果 */
            {
                if (gsm_check_cmd(ack))res = 0; /* 收到期待的结果了 */
                else res = 1; /* 不是期待的结果 */

                break;
            }
        }

        if (waittime == 0)res = 2;
    }

    return res;
}

/**
 * @brief       命令处理完时调用,与gsm_send_cmd成对使用/多个与gsm_send_cmd后调用.
 * @param       无
 * @retval      无
 */
void gsm_cmd_over(void)
{
    USART2_RX_STA = 0;
    gsmdev.cmdon = 0; /* 退出指令等待状态 */
}

/**
 * @brief       将1个字符转换为16进制数字
 * @param       chr     : 字符,0~9/A~F/a~F
 * @retval      chr对应的16进制数值
 */
uint8_t gsm_chr2hex(uint8_t chr)
{
    if (chr >= '0' && chr <= '9')return chr - '0';

    if (chr >= 'A' && chr <= 'F')return (chr - 'A' + 10);

    if (chr >= 'a' && chr <= 'f')return (chr - 'a' + 10);

    return 0;
}

/**
 * @brief       将1个16进制数字转换为字符
 * @param       hex     : 16进制数字,0~15;
 * @retval      字符
 */
uint8_t gsm_hex2chr(uint8_t hex)
{
    if (hex <= 9)return hex + '0';

    if (hex >= 10 && hex <= 15)return (hex - 10 + 'A');

    return '0';
}

/**
 * @brief       unicode gbk 转换函数
 * @param       src     : 输入字符串
 * @param       dst     : 输出(uni2gbk时为gbk内码,gbk2uni时,为unicode字符串)
 * @param       mode    : 0,unicode到gbk转换;
 *                        1,gbk到unicode转换;
 * @retval      无
 */
void gsm_unigbk_exchange(uint8_t *src, uint8_t *dst, uint8_t mode)
{
    OS_CPU_SR cpu_sr = 0;
    uint16_t temp;
    uint8_t buf[2];
    OS_ENTER_CRITICAL();/* 进入临界区(无法被中断打断) */

    if (mode) /* gbk 2 unicode */
    {
        while (*src != 0)
        {
            if (*src < 0X81)    /* 非汉字 */
            {
                temp = (uint16_t)ff_oem2uni((WCHAR) * src, 0);
                src++;
            }
            else    /* 汉字,占2个字节 */
            {
                buf[1] = *src++;
                buf[0] = *src++;
                temp = (uint16_t)ff_oem2uni((WCHAR) * (uint16_t *)buf, 0);
            }

            *dst++ = gsm_hex2chr((temp >> 12) & 0X0F);
            *dst++ = gsm_hex2chr((temp >> 8) & 0X0F);
            *dst++ = gsm_hex2chr((temp >> 4) & 0X0F);
            *dst++ = gsm_hex2chr(temp & 0X0F);
        }
    }
    else    /* unicode 2 gbk */
    {
        while (*src != 0)
        {
            buf[1] = gsm_chr2hex(*src++) * 16;
            buf[1] += gsm_chr2hex(*src++);
            buf[0] = gsm_chr2hex(*src++) * 16;
            buf[0] += gsm_chr2hex(*src++);
            temp = (uint16_t)ff_uni2oem((WCHAR) * (uint16_t *)buf, 0);

            if (temp < 0X80)
            {
                *dst = temp;
                dst++;
            }
            else
            {
                *(uint16_t *)dst = swap16(temp);
                dst += 2;
            }
        }
    }

    *dst = 0; /* 添加结束符 */
    OS_EXIT_CRITICAL();	/* 退出临界区(可以被中断打断) */
}

extern void sms_remind_msg(uint8_t mode);
extern void phone_incall_task_creat(void);

/**
 * @brief       电话呼入/检测到短信 检测
 * @param       无
 * @retval      无
 */
void gsm_cmsgin_check(void)
{
    uint8_t *p1, *p2;
    uint8_t num;

    if (gsmdev.cmdon == 0 && gsmdev.mode == 0) /* 非指令等待状态,.拨号/短信模式,才检测数据 */
    {
        if (USART2_RX_STA & 0X8000) /* 收到数据了 */
        {
            if (gsm_check_cmd("+CLIP:")) /* 接收到来电? */
            {
                p1 = gsm_check_cmd("+CLIP:");
                p1 += 8;
                p2 = (uint8_t *)strstr((const char *)p1, "\"");
                p2[0] = 0; /* 添加结束符 */
                strcpy((char *)gsmdev.incallnum, (char *)p1); /* 拷贝号码 */
                gsmdev.mode = 3;            /* 标记来电了 */
                phone_incall_task_creat();  /* 创建来电任务 */
            }

            if (gsm_check_cmd("+CMGS:"))    /* 短信发送成功 */
            {
                sms_remind_msg(1);          /* 提示发送信息成功 */
            }

            if (gsm_check_cmd("+CMTI:"))    /* 收到新消息 */
            {
                if (gsmdev.newmsg < GSM_MAX_NEWMSG)
                {
                    p1 = (uint8_t *)strstr((const char *)(USART2_RX_BUF), ",");
                    p2 = (uint8_t *)strstr((const char *)(p1 + 1), "\r\n");

                    if ((p2 - p1) == 2)num = p1[1] - '0'; /* 1位 */
                    else if ((p2 - p1) == 3)num = (p1[1] - '0') * 10 + p1[2] - '0'; /* 2位 */
                    else if ((p2 - p1) == 4)num = (p1[1] - '0') * 100 + (p1[2] - '0') * 10 + p1[2] - '0'; /* 3位 */

                    gsmdev.newmsgindex[gsmdev.newmsg] = num;
                    gsmdev.newmsg++;
                }

                sms_remind_msg(0);/* 提示收到新消息 */
            }

            USART2_RX_STA = 0;
            printf("rev:%s\r\n", USART2_RX_BUF);
        }
    }
}

/**
 * @brief       gsm状态监测
 * @param       无
 * @retval      无
 */
void gsm_status_check(void)
{
    uint8_t *p1;

    if (gsmdev.cmdon == 0 && gsmdev.mode == 0 && USART2_RX_STA == 0) /* 非指令等待状态.拨号/短信模式/且没有收到任何数据,才允许查询 */
    {
        if (gsm_send_cmd("AT+CSQ", "OK", 25) == 0) /* 查询信号质量,顺便检测GSM模块状态 */
        {
            p1 = (uint8_t *)strstr((const char *)(USART2_RX_BUF), ":");
            p1 += 2;
            gsmdev.csq = (p1[0] - '0') * 10 + p1[1] - '0'; /* 信号质量 */

            if (gsmdev.csq > 30)gsmdev.csq = 30;

            gsmdev.status |= 1 << 7;    /* 查询GSM模块是否在位? */
        }
        else
        {
            gsmdev.csq = 0;
            gsmdev.status = 0;          /* 重新查找 */
        }

        if ((gsmdev.status & 0XC0) == 0X80) /* CPIN状态,未获取? */
        {
            gsm_send_cmd("ATE0", "OK", 100); /* 不回显(必须关闭,否则接收数据可能异常) */

            if (gsm_send_cmd("AT+CPIN?", "OK", 25) == 0)gsmdev.status |= 1 << 6; /* SIM卡在位 */
            else gsmdev.status &= ~(1 << 6); /* SIM卡错误 */
        }

        if ((gsmdev.status & 0XE0) == 0XC0) /* 运营商名字,未获取? */
        {
            if (gsm_send_cmd("AT+COPS?", "OK", 25) == 0) /* 查询运营商名字 */
            {
                p1 = (uint8_t *)strstr((const char *)(USART2_RX_BUF), "MOBILE"); /* 查找MOBILE,看看是不是中国移动? */

                if (p1)gsmdev.status &= ~(1 << 4); /* 中国移动 */
                else
                {
                    p1 = (uint8_t *)strstr((const char *)(USART2_RX_BUF), "UNICOM"); /* 查找UNICOM,看看是不是中国联通? */

                    if (p1)gsmdev.status |= 1 << 4;	/* 中国联通 */
                }

                if (p1)
                {
                    gsmdev.status |= 1 << 5;    /* 得到运营商名字了 */
                    /* phone部分通用设置 */
                    gsm_send_cmd("AT+CLIP=1", "OK", 100);   /* 设置来电显示 */
                    gsm_send_cmd("AT+COLP=1", "OK", 100);   /* 设置被叫号码显示 */
                    /* sms部分通用设置 */
                    gsm_send_cmd("AT+CMGF=1", "OK", 100);   /* 设置文本模式 */
                    gsm_send_cmd("AT+CSCS=\"UCS2\"", "OK", 100);    /* 设置TE字符集为UCS2 */
                    gsm_send_cmd("AT+CSMP=17,0,2,25", "OK", 100);   /* 设置短消息文本模式参数 */
                }
            }
            else gsmdev.status &= ~(1 << 5);    /* 未获得运营商名字 */
        }

        gsm_cmd_over();                         /* 处理完毕 */
    }
}























