/**
 ****************************************************************************************************
 * @file        facereco.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-人脸识别 代码
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

#ifndef __FACEFRECO_H
#define __FACEFRECO_H

#include "common.h"


/* 人脸链表节点 */
typedef  struct __frec_node
{
    uint8_t *pname;             /* 姓名指针 */
    uint8_t *pnum;              /* 手机号码指针 */
    uint8_t *sex;               /* 性别字符串指针 */
    uint8_t index;              /* 该人脸,保存的位置,也就是识别ID. */
    struct __frec_node *next;   /* 指向下一个节点 */
} frec_node;

/* 人脸识别控制结构体 */
typedef __PACKED_STRUCT
{
    uint16_t xoff;              /* 摄像头在LCD上面显示,x方向的偏移 */
    uint16_t yoff;              /* 摄像头在LCD上面显示,y方向的偏移 */
    uint16_t width;             /* 摄像头在LCD上面显示的宽度 */
    uint16_t height;            /* 摄像头在LCD上面显示的高度 */
    uint16_t *databuf;          /* 图像数据缓存区 */
    frec_node *face;            /* 人脸信息 */
} _frec_obj;

extern _frec_obj frec_dev;      /* 人脸识别控制结构体 */
extern uint16_t frec_curline;   /* 摄像头输出数据,当前行编号,人脸识别用 */
extern volatile uint8_t memshow_flag;   /* 人脸识别运行中标志 */

frec_node *frec_node_creat(void);

void frec_node_free(frec_node *pnode, uint8_t mode);
uint8_t frec_node_insert(frec_node *head, frec_node *pnew, uint16_t i);
uint8_t frec_node_delete(frec_node *head, uint16_t i);
frec_node *frec_node_getnode(frec_node *head, uint16_t i);
uint16_t frec_node_getsize(frec_node *head);
void frec_node_destroy(frec_node *head);


void frec_set_image_center(_frec_obj *frecdev);
void frec_get_image_data(uint16_t *dbuf, uint16_t xoff, uint16_t yoff, uint16_t xsize, uint16_t width);

uint8_t frec_add_a_face_info(_frec_obj *frecdev);

uint8_t appplay_frec_read_a_face(frec_node *pnode, _frec_obj *frecdev, uint8_t index);

uint8_t appplay_frec_add_a_face(frec_node *pnode, _frec_obj *frecdev, uint8_t mode);

void frec_show_picture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
uint8_t frec_play(void);

#endif













