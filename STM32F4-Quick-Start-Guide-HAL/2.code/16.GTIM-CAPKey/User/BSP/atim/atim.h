#ifndef __ATIM_H
#define __ATIM_H

#include "sys.h"

#define TPAD_GATE_VAL         50      // 触摸检测阈值
#define TPAD_ARR_MAX_VAL      0xFFFFFFFF  // 定时器最大计数值
#define TPAD_DISCHARGE_TIME   10      // 放电时间(ms)
#define TPAD_SAMPLE_SIZE      20      // 初始化采样次数
#define TPAD_TRIM_SAMPLES     10      // 初始化有效样本数

// 函数声明
uint8_t tpad_scan(uint8_t mode);
uint8_t tpad_init(uint16_t psc);
void tim_cap_deinit(void);

#endif /* __ATIM_H */