#ifndef __GTIME_H__
#define __GTIME_H__ 

#include "sys.h"

// 输入捕获状态机状态定义
#define CAP_IDLE       0x00  // 空闲状态
#define CAP_RISING     0x40  // 已捕获上升沿
#define CAP_COMPLETE   0x80  // 捕获完成
#define CAP_OVF_MASK   0x3F  // 溢出计数器掩码(最大63次)

// 使用结构体优化状态管理
typedef struct {
    uint8_t status;     // 状态标志 (CAP_IDLE/CAP_RISING/CAP_COMPLETE + 溢出计数)
    uint16_t cap_val;   // 捕获值
    uint32_t timestamp; // 上次处理时间(用于超时检测)
} CapState_t;

void TIM5_CH1_Init(uint32_t arr, uint16_t psc);

#endif /* __GTIME_H__ */
