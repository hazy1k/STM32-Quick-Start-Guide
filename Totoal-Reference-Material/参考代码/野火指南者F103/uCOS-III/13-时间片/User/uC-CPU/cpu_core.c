/*
************************************************************************************************************************
*                                                 包含的头文件
************************************************************************************************************************
*/
#define    CPU_CORE_MODULE
#include  "cpu_core.h"
#include  "lib_def.h"



/*
*********************************************************************************************************
*                                             寄存器定义
*********************************************************************************************************
*/
/*
 在Cortex-M内核里面有一个外设叫DWT(Data Watchpoint and Trace)，该外设有一个32位的寄存器叫CYCCNT，
 它是一个向上的 计数器，记录的是内核时钟运行的个数，当CYCCNT溢出之后，会清0重新开始向上计数。
 使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 */
#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)

/*
*********************************************************************************************************
*                                            寄存器位定义
*********************************************************************************************************
*/

#define  BSP_DBGMCU_CR_TRACE_IOEN_MASK                   0x10
#define  BSP_DBGMCU_CR_TRACE_MODE_ASYNC                  0x00
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_01                0x40
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_02                0x80
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04                0xC0
#define  BSP_DBGMCU_CR_TRACE_MODE_MASK                   0xC0

#define  BSP_BIT_DEM_CR_TRCENA                          (1<<24)

#define  BSP_BIT_DWT_CR_CYCCNTENA                       (1<<0)

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

#ifndef   CPU_CFG_LEAD_ZEROS_ASM_PRESENT
static  const  CPU_INT08U  CPU_CntLeadZerosTbl[256] = {                             /* Data vals :                      */
/*   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F   */
    8u,  7u,  6u,  6u,  5u,  5u,  5u,  5u,  4u,  4u,  4u,  4u,  4u,  4u,  4u,  4u,  /*   0x00 to 0x0F                   */
    3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  /*   0x10 to 0x1F                   */
    2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  /*   0x20 to 0x2F                   */
    2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  /*   0x30 to 0x3F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x40 to 0x4F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x50 to 0x5F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x60 to 0x6F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x70 to 0x7F                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0x80 to 0x8F                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0x90 to 0x9F                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xA0 to 0xAF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xB0 to 0xBF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xC0 to 0xCF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xD0 to 0xDF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xE0 to 0xEF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u   /*   0xF0 to 0xFF                   */
};
#endif

/*
*********************************************************************************************************
*                                            函数实现
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            CPU初始化函数
*********************************************************************************************************
*/
/* CPU初始化函数 */
void  CPU_Init (void)
{
/* CPU初始化函数中总共做了三件事
   1、初始化时间戳
   2、初始化中断失能时间测量
   3、初始化CPU名字
   这里只讲时间戳功能，剩下两个的初始化代码则删除不讲 */
	
#if ((CPU_CFG_TS_EN     == DEF_ENABLED) || \
     (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
    CPU_TS_Init();
#endif
	
}

/*
*********************************************************************************************************
*                                            时间戳相关函数
*********************************************************************************************************
*/
/* 时间戳初始化函数 */
#if ((CPU_CFG_TS_EN     == DEF_ENABLED) || \
     (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
static  void  CPU_TS_Init (void)
{
                                                              
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
    CPU_TS_TmrFreq_Hz   = 0u;                                  
    CPU_TS_TmrInit();                                          
#endif

}
#endif

/* 时间戳定时器初始化 */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    /* 使能DWT外设 */
	BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;
	/* DWT CYCCNT寄存器计数清0 */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
/* 注意：当使用软件仿真全速运行的时候，会先停在这里，
         就好像在这里设置了一个断点一样，需要手动运行才能跳过， 
         当使用硬件仿真的时候却不会 */
	/* 使能Cortex-M3 DWT CYCCNT寄存器 */
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif


/* 获取CPU的HCLK时钟 
这个是跟硬件相关的，目前我们是软件仿真，我们暂时把跟硬件相关的代码屏蔽掉，
直接手动设置CPU的HCLK时钟*/
CPU_INT32U  BSP_CPU_ClkFreq (void)
{
#if 0 
	RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
#else
	CPU_INT32U    CPU_HCLK;
	
	
	/* 目前软件仿真我们使用25M的系统时钟 */
	CPU_HCLK = 25000000;
	
	return CPU_HCLK;
#endif
}

/* 初始化CPU_TS_TmrFreq_Hz，这个就是系统的时钟，单位为HZ */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrFreqSet (CPU_TS_TMR_FREQ  freq_hz)
{
    CPU_TS_TmrFreq_Hz = freq_hz;
}
#endif



#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;

                                                                
    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif

/*
*********************************************************************************************************
*                                            前导0计算函数
*********************************************************************************************************
*/
#ifndef   CPU_CFG_LEAD_ZEROS_ASM_PRESENT
CPU_DATA  CPU_CntLeadZeros (CPU_DATA  val)
{
    CPU_DATA    nbr_lead_zeros;
    CPU_INT08U  ix;


#if   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_08)                                   /* ----------  8-BIT DATA VAL --------- */
                                                                                /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
    ix             = (CPU_INT08U)(val >>  0u);                                  /* .. lookup tbl ix  = 'val' >>  0 bits */
    nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);               /* .. plus nbr msb lead zeros =  0 bits.*/


#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)                                   /* ---------- 16-BIT DATA VAL --------- */
    if (val > 0x00FFu) {                                                        /* Chk bits [15:08] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
        ix             = (CPU_INT08U)(val >>  8u);                              /* .. lookup tbl ix  = 'val' >>  8 bits */
        nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);           /* .. plus nbr msb lead zeros =  0 bits.*/

    } else {                                                                    /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
        ix             = (CPU_INT08U)(val >>  0u);                              /* .. lookup tbl ix  = 'val' >>  0 bits */
        nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  8u);           /* .. plus nbr msb lead zeros =  8 bits.*/
    }


#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32)                                   /* ---------- 32-BIT DATA VAL --------- */
    if (val > 0x0000FFFFu) {
        if (val > 0x00FFFFFFu) {                                                /* Chk bits [31:24] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >> 24u);                          /* .. lookup tbl ix  = 'val' >> 24 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);       /* .. plus nbr msb lead zeros =  0 bits.*/

        } else {                                                                /* Chk bits [23:16] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >> 16u);                          /* .. lookup tbl ix  = 'val' >> 16 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  8u);       /* .. plus nbr msb lead zeros =  8 bits.*/
        }

    } else {
        if (val > 0x000000FFu) {                                                /* Chk bits [15:08] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >>  8u);                          /* .. lookup tbl ix  = 'val' >>  8 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 16u);       /* .. plus nbr msb lead zeros = 16 bits.*/

        } else {                                                                /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >>  0u);                          /* .. lookup tbl ix  = 'val' >>  0 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 24u);       /* .. plus nbr msb lead zeros = 24 bits.*/
        }
    }

/*$PAGE*/
#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_64)                                   /* ---------- 64-BIT DATA VAL --------- */
    if (val > 0x00000000FFFFFFFFu) {
        if (val > 0x0000FFFFFFFFFFFFu) {
            if (val > 0x00FFFFFFFFFFFFFFu) {                                    /* Chk bits [63:56] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 56u);                      /* .. lookup tbl ix  = 'val' >> 56 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);   /* .. plus nbr msb lead zeros =  0 bits.*/

            } else {                                                            /* Chk bits [55:48] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 48u);                      /* .. lookup tbl ix  = 'val' >> 48 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  8u);   /* .. plus nbr msb lead zeros =  8 bits.*/
            }

        } else {
            if (val > 0x000000FFFFFFFFFFu) {                                    /* Chk bits [47:40] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 40u);                      /* .. lookup tbl ix  = 'val' >> 40 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 16u);   /* .. plus nbr msb lead zeros = 16 bits.*/

            } else {                                                            /* Chk bits [39:32] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 32u);                      /* .. lookup tbl ix  = 'val' >> 32 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 24u);   /* .. plus nbr msb lead zeros = 24 bits.*/
            }
        }

    } else {
        if (val > 0x000000000000FFFFu) {
            if (val > 0x0000000000FFFFFFu) {                                    /* Chk bits [31:24] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 24u);                      /* .. lookup tbl ix  = 'val' >> 24 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 32u);   /* .. plus nbr msb lead zeros = 32 bits.*/

            } else {                                                            /* Chk bits [23:16] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 16u);                      /* .. lookup tbl ix  = 'val' >> 16 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 40u);   /* .. plus nbr msb lead zeros = 40 bits.*/
            }

        } else {
            if (val > 0x00000000000000FFu) {                                    /* Chk bits [15:08] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >>  8u);                      /* .. lookup tbl ix  = 'val' >>  8 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 48u);   /* .. plus nbr msb lead zeros = 48 bits.*/

            } else {                                                            /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >>  0u);                      /* .. lookup tbl ix  = 'val' >>  0 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 56u);   /* .. plus nbr msb lead zeros = 56 bits.*/
            }
        }
    }


#else                                                                           /* See Note #1a.                        */
   (void)&ix;
    nbr_lead_zeros = 0u;
#endif

    return (nbr_lead_zeros);
}
#endif




