/**
 ****************************************************************************************************
 * @file        smsplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-短信测试 代码
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
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __SMSPLAY_H
#define __SMSPLAY_H

#include "common.h"
#include "./BSP/GSM/gsm.h"


#define SMS_RMEMO_MAXLEN        2000        /* 最多可以输入2K字节内容 */

#define SMS_PNUM_BACK_COLOR     0X42F0      /* 手机号码(联系人/收件人)区域背景色 */
#define SMS_PNUM_NAME_COLOR     WHITE       /* 联系人/收件人字体颜色 */
#define SMS_PNUM_NUM_COLOR      WHITE       /* 号码颜色 */

#define SMS_COM_RIM_COLOR       0X9D36      /* 通用分割线 */

#define SMS_RMSG_BACK_COLOR     0XBE3B      /* 短信接收时间区域背景色 */
#define SMS_RMSG_FONT_COLOR     BLACK       /* 时间字体颜色 */

#define SMS_RMEMO_BACK_COLOR    WHITE       /* 接收的短信MEMO控件背景色 */
#define SMS_RMEMO_FONT_COLOR    BLACK       /* 接收的短信字体颜色 */

#define SMS_SAREA_BACK_COLOR    0XA599      /* 发送区域背景色 */
#define SMS_SMEMO_BACK_COLOR    WHITE       /* 发送短信MEMO控件背景色 */
#define SMS_SMEMO_FONT_COLOR    BLACK       /* 发送短信字体颜色 */

/* 短信链表节点 */
typedef  struct __sms_node
{
    uint8_t *pnum;      /* 电话号码指针 */
    uint8_t *msg;       /* 短信内容指针 */
    uint8_t *time;      /* 接收时间指针 */
    uint8_t *item;      /* 用于展示的指针=pnum+msg+time */
    uint8_t index;      /* 该短信在SIM卡的位置 */
    uint8_t readed;     /* 0,未读短信;1,已读短信 */
    struct __sms_node *next;    /* 指向下一个节点 */
} sms_node;

sms_node *sms_node_creat(void);

void sms_node_free(sms_node *pnode, uint8_t mode);
uint8_t sms_node_insert(sms_node *head, sms_node *pnew, uint16_t i);
uint8_t sms_node_delete(sms_node *head, uint16_t i);
sms_node *sms_node_getnode(sms_node *head, uint16_t i);
uint16_t sms_node_getsize(sms_node *head);
void sms_node_destroy(sms_node *head);

void sms_remind_msg(uint8_t mode);
uint8_t sms_read_sms(sms_node *pnode, uint16_t index, uint8_t mode);
uint8_t sms_delete_sms(sms_node **head, uint16_t index);
uint8_t sms_get_smsnum(uint16_t *num, uint16_t *max);
uint8_t sms_msg_select(uint16_t *sel, uint16_t *top, uint8_t *caption, sms_node *head);
uint16_t sms_read_all_sms(sms_node *head, uint16_t *maxnum);
void sms_send_sms(sms_node *pnode, uint8_t mode);
uint8_t sms_play(void);

#endif



















