#ifndef __MALLOC_H
#define __MALLOC_H

#include "sys.h"

/* 定义3个内存池 */
#define SRAMIN     0  // 内部内存池 
#define SRAMCCM    1  // CCM内存池(此部分SRAM仅仅CPU可以访问!!!) 
#define SRAMEX     2  // 外部内存池
#define SRAMBANK   3  // 定义支持的SRAM块数

/* 定义内存管理表类型,当外扩SDRAM的时候，必须使用uint32_t类型，否则可以定义成uint16_t，以节省内存占用 */
#define MT_TYPE  uint16_t

/* 单块内存，内存管理所占用的全部空间大小计算公式如下：
 * size = MEM1_MAX_SIZE + (MEM1_MAX_SIZE / MEM1_BLOCK_SIZE) * sizeof(MT_TYPE)
 * 以SRAMEX为例，size = 963 * 1024 + (963 * 1024 / 32) * 2 = 1047744 ≈ 1023KB

 * 已知总内存容量(size)，最大内存池的计算公式如下：
 * MEM1_MAX_SIZE = (MEM1_BLOCK_SIZE * size) / (MEM1_BLOCK_SIZE + sizeof(MT_TYPE))
 * 以CCM为例, MEM2_MAX_SIZE = (32 * 64) / (32 + 2) = 60.24KB ≈ 60KB
 */
 
/* mem1内存参数设定.mem1完全处于内部SRAM里面 */
#define MEM1_BLOCK_SIZE         32                              // 内存块大小为32字节
#define MEM1_MAX_SIZE           100 * 1024                      // 最大管理内存 100K
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   // 内存表大小
/* mem2内存参数设定.mem2处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!) */
#define MEM2_BLOCK_SIZE         32                              // 内存块大小为32字节 
#define MEM2_MAX_SIZE           60 * 1024                       // 最大管理内存60K 
#define MEM2_ALLOC_TABLE_SIZE   MEM2_MAX_SIZE/MEM2_BLOCK_SIZE   // 内存表大小 
/* mem3内存参数设定.mem3是外扩SRAM */
#define MEM3_BLOCK_SIZE         32                              // 内存块大小为32字节 
#define MEM3_MAX_SIZE           963 * 1024                      // 最大管理内存963K 
#define MEM3_ALLOC_TABLE_SIZE   MEM3_MAX_SIZE/MEM3_BLOCK_SIZE   // 内存表大小 

/* 如果没有定义NULL, 定义NULL */
#ifndef NULL
#define NULL 0
#endif

/* 内存管理控制器 */
struct _m_mallco_dev
{
    void (*init)(uint8_t);        // 初始化 
    uint16_t (*perused)(uint8_t); // 内存使用率 
    uint8_t *membase[SRAMBANK];   // 内存池 管理SRAMBANK个区域的内存 
    MT_TYPE *memmap[SRAMBANK];    // 内存管理状态表 
    uint8_t  memrdy[SRAMBANK];    // 内存管理是否就绪 
};

extern struct _m_mallco_dev mallco_dev;
void my_mem_init(uint8_t memx);                          // 内存管理初始化函数(外/内部调用) 
uint16_t my_mem_perused(uint8_t memx) ;                  // 获得内存使用率(外/内部调用) 
void my_mem_set(void *s, uint8_t c, uint32_t count);     // 内存设置函数 
void my_mem_copy(void *des, void *src, uint32_t n);      // 内存拷贝函数 
void myfree(uint8_t memx, void *ptr);                    // 内存释放(外部调用) 
void *mymalloc(uint8_t memx, uint32_t size);             // 内存分配(外部调用) 
void *myrealloc(uint8_t memx, void *ptr, uint32_t size); // 重新分配内存(外部调用) 

#endif /* __MALLOC_H */
