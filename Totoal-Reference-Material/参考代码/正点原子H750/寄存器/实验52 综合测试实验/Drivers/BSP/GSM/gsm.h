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

#ifndef __GSM_H
#define __GSM_H

#include "./SYSTEM/sys/sys.h"


#define GSM_MAX_NEWMSG      10      /* 最大10条新消息 */

typedef struct
{
    uint8_t status;     /**
                         * gsm状态
                         * bit7:0,没有找到模块;1,找到模块了
                         * bit6:0,SIM卡不正常;1,SIM卡正常
                         * bit5:0,未获得运营商名字;1,已获得运营商名字
                         * bit4:0,中国移动;1,中国联通
                         * [3:0]:保留
                         */
    
    uint8_t mode;       /**
                         * 当前工作模式
                         * 0,号码输入模式/短信模式
                         * 1,拨号中
                         * 2,通话中
                         * 3,来电响应中
                         */

    volatile uint8_t cmdon;     /**
                                 * 标记是否有指令在发送等待状态
                                 * 0,没有指令在等待回应
                                 * 1,有指令在等待回应
                                 */

    uint8_t csq;                /* 信号质量 */

    volatile uint8_t newmsg;    /* 新消息条数,0,没有新消息;其他,新消息条数 */
    uint8_t newmsgindex[GSM_MAX_NEWMSG];    /* 新短信在SIM卡内的索引,最长记录GSM_MAX_NEWMSG条新短信 */
    uint8_t incallnum[20];      /* 来电号码缓存区,最长20位 */
} __gsmdev;

extern __gsmdev gsmdev;         /* gsm控制器 */


#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8 /* 高低字节交换宏定义 */


uint8_t *gsm_check_cmd(uint8_t *str);
uint8_t gsm_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t waittime);
void gsm_cmd_over(void);
uint8_t gsm_chr2hex(uint8_t chr);
uint8_t gsm_hex2chr(uint8_t hex);
void gsm_unigbk_exchange(uint8_t *src, uint8_t *dst, uint8_t mode);
void gsm_cmsgin_check(void);
void gsm_status_check(void);

#endif





