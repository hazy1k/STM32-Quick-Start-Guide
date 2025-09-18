#ifndef __SERIAL_INVOKER_H
#define __SERIAL_INVOKER_H

#include "usart.h"

/******************************************************************************************/
/* 用户配置参数 */
#define MAX_FNAME_LEN                   30      /* 函数名最大长度 */
#define MAX_PARM                        10      /* 最大参数数量 */
#define PARM_LEN                        200     /* 参数总长度限制 */
#define SERIAL_INVOKER_ENTIMX_SCAN      1       /* 定时器扫描开关 */
#define SERIAL_INVOKER_USE_HELP         1       /* 帮助功能开关 */
#define SERIAL_INVOKER_USE_WRFUNS       1       /* 读写功能开关 */
#define SERIAL_INVOKER_PRINTF           printf  /* 输出重定向 */
/******************************************************************************************/
/* 定时器定义 */
#if SERIAL_INVOKER_ENTIMX_SCAN == 1
#define SERIAL_INVOKER_TIMX             TIM4
#define SERIAL_INVOKER_TIMX_IRQn        TIM4_IRQn
#define SERIAL_INVOKER_TIMX_IRQHandler  TIM4_IRQHandler
#define SERIAL_INVOKER_TIMX_CLK_ENABLE() do{ __HAL_RCC_TIM4_CLK_ENABLE(); }while(0)
#endif
/******************************************************************************************/
/* 标准类型定义 */
#ifndef _STDINT_H
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
#endif
#define SERIAL_INVOKER_OK           0   /* 无错误 */
#define SERIAL_INVOKER_FUNCERR      1   /* 函数错误 */
#define SERIAL_INVOKER_PARMERR      2   /* 参数错误 */
#define SERIAL_INVOKER_PARMOVER     3   /* 参数溢出 */
#define SERIAL_INVOKER_NOFUNCFIND   4   /* 未找到函数 */
#define SP_TYPE_DEC                 0   /* 10进制显示 */
#define SP_TYPE_HEX                 1   /* 16进制显示 */
/* 函数名列表 */
struct _m_serial_invoker_nametab {
    void *func;             /* 函数指针 */
    const char *name;       /* 函数名(查找串) */
};

/* 控制管理器 */
struct _m_serial_invoker_dev {
    struct _m_serial_invoker_nametab *funs;  /* 函数名指针 */
    void (*init)(uint16_t tclk);             /* 初始化 */
    uint8_t (*cmd_rec)(char *str);           /* 识别函数名及参数 */
    void (*exe)(void);                       /* 执行 */
    void (*scan)(void);                      /* 扫描 */
    uint8_t fnum;                            /* 函数数量 */
    uint8_t pnum;                            /* 参数数量 */
    uint8_t id;                              /* 函数id */
    uint8_t sptype;                          /* 参数显示类型 */
    uint16_t parmtype;                       /* 参数类型 */
    uint8_t  plentbl[MAX_PARM];              /* 参数长度暂存表 */
    uint8_t  parm[PARM_LEN];                 /* 函数参数 */
    uint8_t runtimeflag;                     /* 运行时统计标志 */
    uint32_t runtime;                        /* 运行时间(0.1ms) */
};

/* ================ 字符串处理层函数声明 ================ */
uint8_t serial_invoker_get_parmpos(uint8_t num);
uint8_t serial_invoker_strcmp(char *str1, char *str2);
uint32_t serial_invoker_pow(uint8_t m, uint8_t n);
uint8_t serial_invoker_str2num(char *str, uint32_t *res);
uint8_t serial_invoker_get_cmdname(char *str, char *cmdname, uint8_t *nlen, uint8_t maxlen);
uint8_t serial_invoker_get_fname(char *str, char *fname, uint8_t *pnum, uint8_t *rval);
uint8_t serial_invoker_get_aparm(char *str, char *fparm, uint8_t *ptype);
uint8_t serial_invoker_get_fparam(char *str, uint8_t *parn);

/* ================ 硬件移植层函数声明 ================ */
char *serial_invoker_get_input_string(void);
void serial_invoker_timx_reset_time(void);
uint32_t serial_invoker_timx_get_time(void);
void serial_invoker_timx_init(uint16_t arr, uint16_t psc);

/* ================ 核心功能层函数声明 ================ */
extern struct _m_serial_invoker_nametab serial_invoker_nametab[];
extern struct _m_serial_invoker_dev serial_invoker_dev;
void serial_invoker_init(uint16_t tclk);
uint8_t serial_invoker_cmd_rec(char *str);
void serial_invoker_exe(void);
void serial_invoker_scan(void);
uint32_t read_addr(uint32_t addr);
void write_addr(uint32_t addr, uint32_t val);

#endif /* __SERIAL_INVOKER_H */
