#include "SerialInvoker.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ================ 字符串处理层 ================ */
uint8_t serial_invoker_strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) return 1;
        if (*str1 == '\0') break;
        str1++; str2++;
    }
    return 0;
}
void serial_invoker_strcopy(char *src, char *dst) {
    while (1) {
        *dst = *src;
        if (*src == '\0') break;
        src++; dst++;
    }
}
uint8_t serial_invoker_strlen(char *str) {
    uint8_t len = 0;
    while (1) {
        if (*str == '\0') break;
        len++; str++;
    }
    return len;
}
uint32_t serial_invoker_pow(uint8_t m, uint8_t n) {
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}
uint8_t serial_invoker_str2num(char *str, uint32_t *res) {
    uint32_t t; int tnum;
    uint8_t bnum = 0, hexdec = 10, flag = 0;
    char *p = str;
    *res = 0;
    
    while (1) {
        if ((*p <= '9' && *p >= '0') || ((*str == '-' || *str == '+') && bnum == 0) || 
            (*p <= 'F' && *p >= 'A') || (*p == 'X' && bnum == 1)) {
            if (*p >= 'A') hexdec = 16;
            if (*str == '-') { flag = 2; str++; }
            else if (*str == '+') { flag = 1; str++; }
            else bnum++;
        }
        else if (*p == '\0') break;
        else return 1;
        p++;
    }
    
    p = str;
    if (hexdec == 16) {
        if (bnum < 3) return 2;
        if (*p == '0' && (*(p + 1) == 'X')) {
            p += 2; bnum -= 2;
        }
        else return 3;
    }
    else if (bnum == 0) return 4;
    
    while (1) {
        if (bnum) bnum--;
        if (*p <= '9' && *p >= '0') t = *p - '0';
        else t = *p - 'A' + 10;
        *res += t * serial_invoker_pow(hexdec, bnum);
        p++;
        if (*p == '\0') break;
    }
    
    if (flag == 2) { tnum = -*res; *res = tnum; }
    return 0;
}
uint8_t serial_invoker_get_cmdname(char *str, char *cmdname, uint8_t *nlen, uint8_t maxlen) {
    *nlen = 0;
    while (*str != ' ' && *str != '\0') {
        *cmdname = *str;
        str++; cmdname++;
        (*nlen)++;
        if (*nlen >= maxlen) return 1;
    }
    *cmdname = '\0';
    return 0;
}
uint8_t serial_invoker_search_nextc(char *str) {
    str++;
    while (*str == ' ' && str != 0) str++;
    return *str;
}
uint8_t serial_invoker_get_fname(char *str, char *fname, uint8_t *pnum, uint8_t *rval) {
    uint8_t res = 0, fover = 0, offset = 0, parmnum = 0, temp = 1;
    uint8_t fplcnt = 0, pcnt = 0, nchar;
    char fpname[6], *strtemp = str;
    
    // 检查返回值类型
    while (*strtemp != '\0') {
        if (*strtemp != ' ' && (pcnt & 0X7F) < 5) {
            if (pcnt == 0) pcnt |= 0X80;
            if (((pcnt & 0x7f) == 4) && (*strtemp != '*')) break;
            fpname[pcnt & 0x7f] = *strtemp;
            pcnt++;
        }
        else if (pcnt == 0X85) break;
        strtemp++;
    }
    
    if (pcnt) {
        fpname[pcnt & 0x7f] = '\0';
        *rval = (serial_invoker_strcmp(fpname, "void") == 0) ? 0 : 1;
        pcnt = 0;
    }
    
    // 找到函数名起始位置
    strtemp = str;
    while (*strtemp != '(' && *strtemp != '\0') {
        strtemp++;
        res++;
        if (*strtemp == ' ' || *strtemp == '*') {
            nchar = serial_invoker_search_nextc(strtemp);
            if (nchar != '(' && nchar != '*') offset = res;
        }
    }
    
    strtemp = str;
    if (offset) strtemp += offset + 1;
    res = 0; nchar = 0;
    
    // 解析函数名和参数
    while (1) {
        if (*strtemp == 0) { res = SERIAL_INVOKER_FUNCERR; break; }
        else if (*strtemp == '(' && nchar == 0) fover++;
        else if (*strtemp == ')' && nchar == 0) {
            if (fover) fover--;
            else { res = SERIAL_INVOKER_FUNCERR; break; }
            if (fover == 0) break;
        }
        else if (*strtemp == '"') nchar = !nchar;
        
        if (fover == 0) {
            if (*strtemp != ' ') {
                *fname = *strtemp;
                fname++;
            }
        }
        else {
            if (*strtemp == ',') { temp = 1; pcnt++; }
            else if (*strtemp != ' ' && *strtemp != '(') {
                if (pcnt == 0 && fplcnt < 5) {
                    fpname[fplcnt] = *strtemp;
                    fplcnt++;
                }
                temp++;
            }
            if (fover == 1 && temp == 2) {
                temp++; parmnum++;
            }
        }
        strtemp++;
    }
    
    if (parmnum == 1) {
        fpname[fplcnt] = '\0';
        if (serial_invoker_strcmp(fpname, "void") == 0) parmnum = 0;
    }
    
    *pnum = parmnum;
    *fname = '\0';
    return res;
}
uint8_t serial_invoker_get_aparm(char *str, char *fparm, uint8_t *ptype) {
    uint8_t i = 0, enout = 0, type = 0, string = 0;
    
    while (1) {
        if (*str == ',' && string == 0) enout = 1;
        if ((*str == ')' || *str == '\0') && string == 0) break;
        
        if (type == 0) {
            if ((*str >= '0' && *str <= '9') || *str == '-' || *str == '+' || 
                (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F') || 
                *str == 'X' || *str == 'x') {
                if (enout) break;
                *fparm = (*str >= 'a') ? *str - 0X20 : *str;
                fparm++;
            }
            else if (*str == '"') {
                if (enout) break;
                type = 1; string = 1;
            }
            else if (*str != ' ' && *str != ',') {
                type = 0XFF; break;
            }
        }
        else {
            if (*str == '"') string = 0;
            if (enout) break;
            if (string) {
                if (*str == '\\') { str++; i++; }
                *fparm = *str;
                fparm++;
            }
        }
        i++; str++;
    }
    
    *fparm = '\0';
    *ptype = type;
    return i;
}
uint8_t serial_invoker_get_parmpos(uint8_t num) {
    uint8_t temp = 0;
    for (uint8_t i = 0; i < num; i++) {
        temp += serial_invoker_dev.plentbl[i];
    }
    return temp;
}
uint8_t serial_invoker_get_fparam(char *str, uint8_t *parn) {
    uint8_t i, type, n = 0, len;
    uint32_t res;
    char tstr[PARM_LEN + 1];
    
    for (i = 0; i < MAX_PARM; i++) {
        serial_invoker_dev.plentbl[i] = 0;
    }
    
    while (*str != '(') {
        str++;
        if (*str == '\0') return SERIAL_INVOKER_FUNCERR;
    }
    str++;
    
    while (1) {
        i = serial_invoker_get_aparm(str, tstr, &type);
        str += i;
        
        switch (type) {
            case 0:
                if (tstr[0] != '\0') {
                    i = serial_invoker_str2num(tstr, &res);
                    if (i) return SERIAL_INVOKER_PARMERR;
                    *(uint32_t *)(serial_invoker_dev.parm + serial_invoker_get_parmpos(n)) = res;
                    serial_invoker_dev.parmtype &= ~(1 << n);
                    serial_invoker_dev.plentbl[n] = 4;
                    n++;
                    if (n > MAX_PARM) return SERIAL_INVOKER_PARMOVER;
                }
                break;
                
            case 1:
                len = serial_invoker_strlen(tstr) + 1;
                serial_invoker_strcopy(tstr, (char *)&serial_invoker_dev.parm[serial_invoker_get_parmpos(n)]);
                serial_invoker_dev.parmtype |= 1 << n;
                serial_invoker_dev.plentbl[n] = len;
                n++;
                if (n > MAX_PARM) return SERIAL_INVOKER_PARMOVER;
                break;
                
            case 0XFF:
                return SERIAL_INVOKER_PARMERR;
        }
        
        if (*str == ')' || *str == '\0') break;
    }
    
    *parn = n;
    return SERIAL_INVOKER_OK;
}
/* ================ 核心功能层 ================ */
/* 系统命令 */
char *sys_cmd_tab[] = {
    "?", "help", "list", "id", "hex", "dec", "runtime"
};
uint8_t serial_invoker_sys_cmd_exe(char *str) {
    uint8_t i;
    char sfname[MAX_FNAME_LEN];
    uint8_t pnum, rval;
    uint32_t res;
    
    res = serial_invoker_get_cmdname(str, sfname, &i, MAX_FNAME_LEN);
    if (res) return SERIAL_INVOKER_FUNCERR;
    str += i;
    
    for (i = 0; i < sizeof(sys_cmd_tab) / 4; i++) {
        if (serial_invoker_strcmp(sfname, sys_cmd_tab[i]) == 0) break;
    }
    
    switch (i) {
        case 0:
        case 1:
            SERIAL_INVOKER_PRINTF("\r\n");
#if SERIAL_INVOKER_USE_HELP
            SERIAL_INVOKER_PRINTF("------------------------SerialInvoker V1.0------------------------ \r\n");
            SERIAL_INVOKER_PRINTF("SerialInvoker是由Hazy1k开发的灵巧串口调试交互组件,通过它你可以通过串口助手调用程序中的任何函数并执行.\r\n");
            SERIAL_INVOKER_PRINTF("因此,你可以随意更改函数输入参数(支持数字/字符串/函数指针)\r\n");
            SERIAL_INVOKER_PRINTF("单个函数最多支持10个输入参数,并支持函数返回值显示.支持参数显示进制设置及转换功能.\r\n");
            SERIAL_INVOKER_PRINTF("技术支持:https://www.cnblogs.com/hazy1k\r\n");
            SERIAL_INVOKER_PRINTF("SerialInvoker有7个系统命令(必须小写):\r\n");
            SERIAL_INVOKER_PRINTF("?:      获取帮助信息\r\n");
            SERIAL_INVOKER_PRINTF("help:   获取帮助信息\r\n");
            SERIAL_INVOKER_PRINTF("list:   可用的函数列表\r\n");
            SERIAL_INVOKER_PRINTF("id:     可用函数的ID列表\r\n");
            SERIAL_INVOKER_PRINTF("hex:    参数16进制显示,后跟数字即执行进制转换\r\n");
            SERIAL_INVOKER_PRINTF("dec:    参数10进制显示,后跟数字即执行进制转换\r\n");
            SERIAL_INVOKER_PRINTF("runtime:1,开启函数运行计时;0,关闭函数运行计时;\r\n");
            SERIAL_INVOKER_PRINTF("请按照函数格式输入函数名及参数并以回车键结束.\r\n");
            SERIAL_INVOKER_PRINTF("-------------------------------------------------------------- \r\n");
#else
            SERIAL_INVOKER_PRINTF("指令失效\r\n");
#endif
            break;
            
        case 2:
            SERIAL_INVOKER_PRINTF("\r\n");
            SERIAL_INVOKER_PRINTF("-------------------------函数清单--------------------------- \r\n");
            for (i = 0; i < serial_invoker_dev.fnum; i++) {
                SERIAL_INVOKER_PRINTF("%s\r\n", serial_invoker_dev.funs[i].name);
            }
            SERIAL_INVOKER_PRINTF("\r\n");
            break;
            
        case 3:
            SERIAL_INVOKER_PRINTF("\r\n");
            SERIAL_INVOKER_PRINTF("-------------------------函数 ID --------------------------- \r\n");
            for (i = 0; i < serial_invoker_dev.fnum; i++) {
                serial_invoker_get_fname((char *)serial_invoker_dev.funs[i].name, sfname, &pnum, &rval);
                SERIAL_INVOKER_PRINTF("%s id is:\r\n0X%08X\r\n", sfname, (unsigned int)serial_invoker_dev.funs[i].func);
            }
            SERIAL_INVOKER_PRINTF("\r\n");
            break;
            
        case 4:
            SERIAL_INVOKER_PRINTF("\r\n");
            serial_invoker_get_aparm(str, sfname, &i);
            if (i == 0) {
                i = serial_invoker_str2num(sfname, &res);
                if (i == 0) {
                    SERIAL_INVOKER_PRINTF("HEX:0X%X\r\n", res);
                }
                else if (i != 4) return SERIAL_INVOKER_PARMERR;
                else {
                    SERIAL_INVOKER_PRINTF("16进制参数显示!\r\n");
                    serial_invoker_dev.sptype = SP_TYPE_HEX;
                }
            }
            else return SERIAL_INVOKER_PARMERR;
            SERIAL_INVOKER_PRINTF("\r\n");
            break;
            
        case 5:
            SERIAL_INVOKER_PRINTF("\r\n");
            serial_invoker_get_aparm(str, sfname, &i);
            if (i == 0) {
                i = serial_invoker_str2num(sfname, &res);
                if (i == 0) {
                    SERIAL_INVOKER_PRINTF("DEC:%lu\r\n", (unsigned long)res);
                }
                else if (i != 4) return SERIAL_INVOKER_PARMERR;
                else {
                    SERIAL_INVOKER_PRINTF("10进制参数显示!\r\n");
                    serial_invoker_dev.sptype = SP_TYPE_DEC;
                }
            }
            else return SERIAL_INVOKER_PARMERR;
            SERIAL_INVOKER_PRINTF("\r\n");
            break;
            
        case 6:
            SERIAL_INVOKER_PRINTF("\r\n");
            serial_invoker_get_aparm(str, sfname, &i);
            if (i == 0) {
                i = serial_invoker_str2num(sfname, &res);
                if (i == 0) {
                    if (SERIAL_INVOKER_ENTIMX_SCAN == 0) {
                        SERIAL_INVOKER_PRINTF("\r\nError! \r\nTo EN RunTime function,Please set SERIAL_INVOKER_ENTIMX_SCAN = 1 first!\r\n");
                    }
                    else {
                        serial_invoker_dev.runtimeflag = res;
                        SERIAL_INVOKER_PRINTF("Run Time Calculation %s\r\n", res ? "ON" : "OFF");
                    }
                }
                else return SERIAL_INVOKER_PARMERR;
            }
            else return SERIAL_INVOKER_PARMERR;
            SERIAL_INVOKER_PRINTF("\r\n");
            break;
            
        default:
            return SERIAL_INVOKER_FUNCERR;
    }
    return 0;
}
void serial_invoker_init(uint16_t tclk) {
#if SERIAL_INVOKER_ENTIMX_SCAN == 1
    serial_invoker_timx_init(1000, tclk * 100 - 1);
#endif
    serial_invoker_dev.sptype = 1;  // 默认16进制显示
}
uint8_t serial_invoker_cmd_rec(char *str) {
    uint8_t sta, i, rval;
    uint8_t rpnum, spnum;
    char rfname[MAX_FNAME_LEN];
    char sfname[MAX_FNAME_LEN];
    
    sta = serial_invoker_get_fname(str, rfname, &rpnum, &rval);
    if (sta) return sta;
    
    for (i = 0; i < serial_invoker_dev.fnum; i++) {
        sta = serial_invoker_get_fname((char *)serial_invoker_dev.funs[i].name, sfname, &spnum, &rval);
        if (sta) return sta;
        if (serial_invoker_strcmp(sfname, rfname) == 0) {
            if (spnum > rpnum) return SERIAL_INVOKER_PARMERR;
            serial_invoker_dev.id = i;
            break;
        }
    }
    
    if (i == serial_invoker_dev.fnum) return SERIAL_INVOKER_NOFUNCFIND;
    sta = serial_invoker_get_fparam(str, &i);
    if (sta) return sta;
    serial_invoker_dev.pnum = i;
    return SERIAL_INVOKER_OK;
}
void serial_invoker_exe(void) {
    uint8_t id, i;
    uint32_t res;
    uint32_t temp[MAX_PARM];
    char sfname[MAX_FNAME_LEN];
    uint8_t pnum, rval;
    
    id = serial_invoker_dev.id;
    if (id >= serial_invoker_dev.fnum) return;
    
    serial_invoker_get_fname((char *)serial_invoker_dev.funs[id].name, sfname, &pnum, &rval);
    SERIAL_INVOKER_PRINTF("\r\n%s(", sfname);
    
    for (i = 0; i < pnum; i++) {
        if (serial_invoker_dev.parmtype & (1 << i)) {
            SERIAL_INVOKER_PRINTF("\"%s\"", serial_invoker_dev.parm + serial_invoker_get_parmpos(i));
            temp[i] = (uint32_t)&(serial_invoker_dev.parm[serial_invoker_get_parmpos(i)]);
        }
        else {
            temp[i] = *(uint32_t *)(serial_invoker_dev.parm + serial_invoker_get_parmpos(i));
            if (serial_invoker_dev.sptype == SP_TYPE_DEC) {
                SERIAL_INVOKER_PRINTF("%ld", (long)temp[i]);
            }
            else {
                SERIAL_INVOKER_PRINTF("0X%X", temp[i]);
            }
        }
        if (i != pnum - 1) SERIAL_INVOKER_PRINTF(",");
    }
    
    SERIAL_INVOKER_PRINTF(")");
    
#if SERIAL_INVOKER_ENTIMX_SCAN == 1
    serial_invoker_timx_reset_time();
#endif
    
    switch (serial_invoker_dev.pnum) {
        case 0: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(); break;
        case 1: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0]); break;
        case 2: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1]); break;
        case 3: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2]); break;
        case 4: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3]); break;
        case 5: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4]); break;
        case 6: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]); break;
        case 7: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6]); break;
        case 8: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7]); break;
        case 9: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8]); break;
        case 10: res = (*(uint32_t(*)())serial_invoker_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8], temp[9]); break;
    }
    
#if SERIAL_INVOKER_ENTIMX_SCAN == 1
    serial_invoker_timx_get_time();
#endif
    
    if (rval == 1) {
        if (serial_invoker_dev.sptype == SP_TYPE_DEC) {
            SERIAL_INVOKER_PRINTF("=%lu;\r\n", (unsigned long)res);
        }
        else {
            SERIAL_INVOKER_PRINTF("=0X%X;\r\n", res);
        }
    }
    else {
        SERIAL_INVOKER_PRINTF(";\r\n");
    }
    
    if (serial_invoker_dev.runtimeflag) {
        SERIAL_INVOKER_PRINTF("Function Run Time:%d.%1dms\r\n", 
            serial_invoker_dev.runtime / 10, serial_invoker_dev.runtime % 10);
    }
}
void serial_invoker_scan(void) {
    uint8_t sta, len;
    char *pbuf = serial_invoker_get_input_string();
    if (pbuf == 0) return;
    
    sta = serial_invoker_dev.cmd_rec(pbuf);
    if (sta == 0) {
        serial_invoker_dev.exe();
    }
    else {
        len = serial_invoker_sys_cmd_exe(pbuf);
        if (len != SERIAL_INVOKER_FUNCERR) sta = len;
        if (sta) {
            switch (sta) {
                case SERIAL_INVOKER_FUNCERR: SERIAL_INVOKER_PRINTF("函数错误!\r\n"); break;
                case SERIAL_INVOKER_PARMERR: SERIAL_INVOKER_PRINTF("参数错误!\r\n"); break;
                case SERIAL_INVOKER_PARMOVER: SERIAL_INVOKER_PRINTF("参数太多!\r\n"); break;
                case SERIAL_INVOKER_NOFUNCFIND: SERIAL_INVOKER_PRINTF("未找到匹配的函数!\r\n"); break;
            }
        }
    }
}
#if SERIAL_INVOKER_USE_WRFUNS == 1
uint32_t read_addr(uint32_t addr) {
    return *(uint32_t *)addr;
}
void write_addr(uint32_t addr, uint32_t val) {
    *(uint32_t *)addr = val;
}
#endif

/* ================ 硬件移植层 ================ */
TIM_HandleTypeDef g_timx_serial_invoker_handle;
char *serial_invoker_get_input_string(void) {
    uint8_t len;
    char *pbuf = 0;
    if (g_usart_rx_sta & 0x8000) {
        len = g_usart_rx_sta & 0x3fff;
        g_usart_rx_buf[len] = '\0';
        pbuf = (char*)g_usart_rx_buf;
        g_usart_rx_sta = 0;
    }
    return pbuf;
}
#if SERIAL_INVOKER_ENTIMX_SCAN == 1
void serial_invoker_timx_reset_time(void) {
    __HAL_TIM_CLEAR_FLAG(&g_timx_serial_invoker_handle, TIM_FLAG_UPDATE);
    __HAL_TIM_SET_AUTORELOAD(&g_timx_serial_invoker_handle, 0XFFFF);
    __HAL_TIM_SET_COUNTER(&g_timx_serial_invoker_handle, 0);
    serial_invoker_dev.runtime = 0;
}
uint32_t serial_invoker_timx_get_time(void) {
    if (__HAL_TIM_GET_FLAG(&g_timx_serial_invoker_handle, TIM_FLAG_UPDATE) == SET) {
        serial_invoker_dev.runtime += 0XFFFF;
    }
    serial_invoker_dev.runtime += __HAL_TIM_GET_COUNTER(&g_timx_serial_invoker_handle);
    return serial_invoker_dev.runtime;
}
void serial_invoker_timx_init(uint16_t arr, uint16_t psc) {
    SERIAL_INVOKER_TIMX_CLK_ENABLE();
    g_timx_serial_invoker_handle.Instance = SERIAL_INVOKER_TIMX;
    g_timx_serial_invoker_handle.Init.Prescaler = psc;
    g_timx_serial_invoker_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_serial_invoker_handle.Init.Period = arr;
    g_timx_serial_invoker_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&g_timx_serial_invoker_handle);
    HAL_TIM_Base_Start_IT(&g_timx_serial_invoker_handle);
    HAL_NVIC_SetPriority(SERIAL_INVOKER_TIMX_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(SERIAL_INVOKER_TIMX_IRQn);
}
void SERIAL_INVOKER_TIMX_IRQHandler(void) {
    if (__HAL_TIM_GET_IT_SOURCE(&g_timx_serial_invoker_handle, TIM_IT_UPDATE) == SET) {
        serial_invoker_dev.scan();
        __HAL_TIM_SET_COUNTER(&g_timx_serial_invoker_handle, 0);
        __HAL_TIM_SET_AUTORELOAD(&g_timx_serial_invoker_handle, 100);
    }
    
    __HAL_TIM_CLEAR_IT(&g_timx_serial_invoker_handle, TIM_IT_UPDATE);
}

#endif
