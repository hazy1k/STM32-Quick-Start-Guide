#include "rtc.h"

// RTC初始化
void RTC_Config(void) {
    // 启用PWR和BKP时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // 允许访问BKP区域
    PWR_BackupAccessCmd(ENABLE);
    // 复位备份区域
    BKP_DeInit();
    // 启用LSE时钟（32.768KHz）
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    // 选择LSE作为RTC时钟源
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    
    // 等待RTC寄存器同步
    RTC_WaitForSynchro();
    
    // 等待上一次操作完成
    RTC_WaitForLastTask();
    
    // 设置RTC分频器：32768Hz/(32767+1) = 1Hz
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    
    // 检查是否首次配置
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        RTC_SetCounter(0);  // 从1970-01-01 00:00:00开始计数
        RTC_WaitForLastTask();
        
        // 设置备份寄存器标志
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
}


// 将Unix时间戳转换为日期时间
void ConvertTimestamp(uint32_t timestamp, Custom_DateTypeDef* date, Custom_TimeTypeDef* time) {
    // 简化的转换逻辑（实际项目需使用完整算法）
    time->second = timestamp % 60;
    timestamp /= 60;
    time->minute = timestamp % 60;
    timestamp /= 60;
    time->hour = timestamp % 24;
    timestamp /= 24;
    
    // 简化日期计算（忽略闰年等细节）
    date->year = 1970 + timestamp / 365;
    timestamp %= 365;
    date->month = 1 + timestamp / 30;
    date->day = 1 + timestamp % 30;
}
