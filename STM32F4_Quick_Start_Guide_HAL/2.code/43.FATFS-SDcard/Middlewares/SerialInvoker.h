#ifndef __SERIAL_INVOKER_H
#define __SERIAL_INVOKER_H

#include "usart.h"

/******************************************************************************************/
/* �û����ò��� */
#define MAX_FNAME_LEN                   30      /* ��������󳤶� */
#define MAX_PARM                        10      /* ���������� */
#define PARM_LEN                        200     /* �����ܳ������� */
#define SERIAL_INVOKER_ENTIMX_SCAN      1       /* ��ʱ��ɨ�迪�� */
#define SERIAL_INVOKER_USE_HELP         1       /* �������ܿ��� */
#define SERIAL_INVOKER_USE_WRFUNS       1       /* ��д���ܿ��� */
#define SERIAL_INVOKER_PRINTF           printf  /* ����ض��� */
/******************************************************************************************/
/* ��ʱ������ */
#if SERIAL_INVOKER_ENTIMX_SCAN == 1
#define SERIAL_INVOKER_TIMX             TIM4
#define SERIAL_INVOKER_TIMX_IRQn        TIM4_IRQn
#define SERIAL_INVOKER_TIMX_IRQHandler  TIM4_IRQHandler
#define SERIAL_INVOKER_TIMX_CLK_ENABLE() do{ __HAL_RCC_TIM4_CLK_ENABLE(); }while(0)
#endif
/******************************************************************************************/
/* ��׼���Ͷ��� */
#ifndef _STDINT_H
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
#endif
#define SERIAL_INVOKER_OK           0   /* �޴��� */
#define SERIAL_INVOKER_FUNCERR      1   /* �������� */
#define SERIAL_INVOKER_PARMERR      2   /* �������� */
#define SERIAL_INVOKER_PARMOVER     3   /* ������� */
#define SERIAL_INVOKER_NOFUNCFIND   4   /* δ�ҵ����� */
#define SP_TYPE_DEC                 0   /* 10������ʾ */
#define SP_TYPE_HEX                 1   /* 16������ʾ */
/* �������б� */
struct _m_serial_invoker_nametab {
    void *func;             /* ����ָ�� */
    const char *name;       /* ������(���Ҵ�) */
};

/* ���ƹ����� */
struct _m_serial_invoker_dev {
    struct _m_serial_invoker_nametab *funs;  /* ������ָ�� */
    void (*init)(uint16_t tclk);             /* ��ʼ�� */
    uint8_t (*cmd_rec)(char *str);           /* ʶ������������ */
    void (*exe)(void);                       /* ִ�� */
    void (*scan)(void);                      /* ɨ�� */
    uint8_t fnum;                            /* �������� */
    uint8_t pnum;                            /* �������� */
    uint8_t id;                              /* ����id */
    uint8_t sptype;                          /* ������ʾ���� */
    uint16_t parmtype;                       /* �������� */
    uint8_t  plentbl[MAX_PARM];              /* ���������ݴ�� */
    uint8_t  parm[PARM_LEN];                 /* �������� */
    uint8_t runtimeflag;                     /* ����ʱͳ�Ʊ�־ */
    uint32_t runtime;                        /* ����ʱ��(0.1ms) */
};

/* ================ �ַ�������㺯������ ================ */
uint8_t serial_invoker_get_parmpos(uint8_t num);
uint8_t serial_invoker_strcmp(char *str1, char *str2);
uint32_t serial_invoker_pow(uint8_t m, uint8_t n);
uint8_t serial_invoker_str2num(char *str, uint32_t *res);
uint8_t serial_invoker_get_cmdname(char *str, char *cmdname, uint8_t *nlen, uint8_t maxlen);
uint8_t serial_invoker_get_fname(char *str, char *fname, uint8_t *pnum, uint8_t *rval);
uint8_t serial_invoker_get_aparm(char *str, char *fparm, uint8_t *ptype);
uint8_t serial_invoker_get_fparam(char *str, uint8_t *parn);

/* ================ Ӳ����ֲ�㺯������ ================ */
char *serial_invoker_get_input_string(void);
void serial_invoker_timx_reset_time(void);
uint32_t serial_invoker_timx_get_time(void);
void serial_invoker_timx_init(uint16_t arr, uint16_t psc);

/* ================ ���Ĺ��ܲ㺯������ ================ */
extern struct _m_serial_invoker_nametab serial_invoker_nametab[];
extern struct _m_serial_invoker_dev serial_invoker_dev;
void serial_invoker_init(uint16_t tclk);
uint8_t serial_invoker_cmd_rec(char *str);
void serial_invoker_exe(void);
void serial_invoker_scan(void);
uint32_t read_addr(uint32_t addr);
void write_addr(uint32_t addr, uint32_t val);

#endif /* __SERIAL_INVOKER_H */
