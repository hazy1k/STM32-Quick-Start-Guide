#ifndef __USMART_H
#define __USMART_H

#include "usmart_port.h"

#define USMART_OK               0       /* 无错误 */
#define USMART_FUNCERR          1       /* 函数错误 */
#define USMART_PARMERR          2       /* 参数错误 */
#define USMART_PARMOVER         3       /* 参数溢出 */
#define USMART_NOFUNCFIND       4       /* 未找到匹配函数 */

#define SP_TYPE_DEC             0       /* 10进制参数显示 */
#define SP_TYPE_HEX             1       /* 16进制参数显示 */


/* 函数名列表 */
struct _m_usmart_nametab
{
    void *func;             /* 函数指针 */
    const char *name;       /* 函数名(查找串) */
};

/* usmart控制管理器 */
struct _m_usmart_dev
{
    struct _m_usmart_nametab *funs;     /* 函数名指针 */

    void (*init)(uint16_t tclk);        /* 初始化 */
    uint8_t (*cmd_rec)(char *str);   /* 识别函数名及参数 */
    void (*exe)(void);                  /* 执行  */
    void (*scan)(void);                 /* 扫描 */
    uint8_t fnum;                       /* 函数数量 */
    uint8_t pnum;                       /* 参数数量 */
    uint8_t id;                         /* 函数id */
    uint8_t sptype;                     /* 参数显示类型(非字符串参数):0,10进制;1,16进制; */
    uint16_t parmtype;                  /* 参数的类型 */
    uint8_t  plentbl[MAX_PARM];         /* 每个参数的长度暂存表 */
    uint8_t  parm[PARM_LEN];            /* 函数的参数 */
    uint8_t runtimeflag;                /* 0,不统计函数执行时间;1,统计函数执行时间,注意:此功能必须在USMART_ENTIMX_SCAN使能的时候,才有用 */
    uint32_t runtime;                   /* 运行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms */
};

extern struct _m_usmart_nametab usmart_nametab[];   /* 在usmart_config.c里面定义 */
extern struct _m_usmart_dev usmart_dev;             /* 在usmart_config.c里面定义 */


void usmart_init(uint16_t tclk);        /* 初始化 */
uint8_t usmart_cmd_rec(char*str);    	/* 识别 */
void usmart_exe(void);                  /* 执行 */
void usmart_scan(void);                 /* 扫描 */
uint32_t read_addr(uint32_t addr);      /* 读取指定地址的值 */
void write_addr(uint32_t addr,uint32_t val);/* 在指定地址写入指定的值 */

#endif






























