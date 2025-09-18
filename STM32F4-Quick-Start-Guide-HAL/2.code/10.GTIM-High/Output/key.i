# 1 "../User/BSP/key/key.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 387 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "../User/BSP/key/key.c" 2
# 1 "../User/BSP/key/key.h" 1



# 1 "../User/SYSTEM/sys\\sys.h" 1



# 1 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h" 1
# 133 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h"
# 1 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h" 1
# 66 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h"
typedef enum
{

  NonMaskableInt_IRQn = -14,
  MemoryManagement_IRQn = -12,
  BusFault_IRQn = -11,
  UsageFault_IRQn = -10,
  SVCall_IRQn = -5,
  DebugMonitor_IRQn = -4,
  PendSV_IRQn = -2,
  SysTick_IRQn = -1,

  WWDG_IRQn = 0,
  PVD_IRQn = 1,
  TAMP_STAMP_IRQn = 2,
  RTC_WKUP_IRQn = 3,
  FLASH_IRQn = 4,
  RCC_IRQn = 5,
  EXTI0_IRQn = 6,
  EXTI1_IRQn = 7,
  EXTI2_IRQn = 8,
  EXTI3_IRQn = 9,
  EXTI4_IRQn = 10,
  DMA1_Stream0_IRQn = 11,
  DMA1_Stream1_IRQn = 12,
  DMA1_Stream2_IRQn = 13,
  DMA1_Stream3_IRQn = 14,
  DMA1_Stream4_IRQn = 15,
  DMA1_Stream5_IRQn = 16,
  DMA1_Stream6_IRQn = 17,
  ADC_IRQn = 18,
  CAN1_TX_IRQn = 19,
  CAN1_RX0_IRQn = 20,
  CAN1_RX1_IRQn = 21,
  CAN1_SCE_IRQn = 22,
  EXTI9_5_IRQn = 23,
  TIM1_BRK_TIM9_IRQn = 24,
  TIM1_UP_TIM10_IRQn = 25,
  TIM1_TRG_COM_TIM11_IRQn = 26,
  TIM1_CC_IRQn = 27,
  TIM2_IRQn = 28,
  TIM3_IRQn = 29,
  TIM4_IRQn = 30,
  I2C1_EV_IRQn = 31,
  I2C1_ER_IRQn = 32,
  I2C2_EV_IRQn = 33,
  I2C2_ER_IRQn = 34,
  SPI1_IRQn = 35,
  SPI2_IRQn = 36,
  USART1_IRQn = 37,
  USART2_IRQn = 38,
  USART3_IRQn = 39,
  EXTI15_10_IRQn = 40,
  RTC_Alarm_IRQn = 41,
  OTG_FS_WKUP_IRQn = 42,
  TIM8_BRK_TIM12_IRQn = 43,
  TIM8_UP_TIM13_IRQn = 44,
  TIM8_TRG_COM_TIM14_IRQn = 45,
  TIM8_CC_IRQn = 46,
  DMA1_Stream7_IRQn = 47,
  FSMC_IRQn = 48,
  SDIO_IRQn = 49,
  TIM5_IRQn = 50,
  SPI3_IRQn = 51,
  UART4_IRQn = 52,
  UART5_IRQn = 53,
  TIM6_DAC_IRQn = 54,
  TIM7_IRQn = 55,
  DMA2_Stream0_IRQn = 56,
  DMA2_Stream1_IRQn = 57,
  DMA2_Stream2_IRQn = 58,
  DMA2_Stream3_IRQn = 59,
  DMA2_Stream4_IRQn = 60,
  ETH_IRQn = 61,
  ETH_WKUP_IRQn = 62,
  CAN2_TX_IRQn = 63,
  CAN2_RX0_IRQn = 64,
  CAN2_RX1_IRQn = 65,
  CAN2_SCE_IRQn = 66,
  OTG_FS_IRQn = 67,
  DMA2_Stream5_IRQn = 68,
  DMA2_Stream6_IRQn = 69,
  DMA2_Stream7_IRQn = 70,
  USART6_IRQn = 71,
  I2C3_EV_IRQn = 72,
  I2C3_ER_IRQn = 73,
  OTG_HS_EP1_OUT_IRQn = 74,
  OTG_HS_EP1_IN_IRQn = 75,
  OTG_HS_WKUP_IRQn = 76,
  OTG_HS_IRQn = 77,
  DCMI_IRQn = 78,
  RNG_IRQn = 80,
  FPU_IRQn = 81
} IRQn_Type;







# 1 "../Libraries/CMSIS/Include\\core_cm4.h" 1
# 29 "../Libraries/CMSIS/Include\\core_cm4.h" 3





# 1 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\stdint.h" 1 3
# 56 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\stdint.h" 3
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;


typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;





typedef signed char int_least8_t;
typedef signed short int int_least16_t;
typedef signed int int_least32_t;
typedef signed long long int int_least64_t;


typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long int uint_least64_t;




typedef signed int int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed long long int int_fast64_t;


typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long int uint_fast64_t;






typedef signed int intptr_t;
typedef unsigned int uintptr_t;



typedef signed long long intmax_t;
typedef unsigned long long uintmax_t;
# 35 "../Libraries/CMSIS/Include\\core_cm4.h" 2 3
# 63 "../Libraries/CMSIS/Include\\core_cm4.h" 3
# 1 "../Libraries/CMSIS/Include/cmsis_version.h" 1 3
# 29 "../Libraries/CMSIS/Include/cmsis_version.h" 3
# 64 "../Libraries/CMSIS/Include\\core_cm4.h" 2 3
# 162 "../Libraries/CMSIS/Include\\core_cm4.h" 3
# 1 "../Libraries/CMSIS/Include/cmsis_compiler.h" 1 3
# 41 "../Libraries/CMSIS/Include/cmsis_compiler.h" 3
# 1 "../Libraries/CMSIS/Include/cmsis_armclang.h" 1 3
# 31 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3


# 1 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 1 3






# 1 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 1 3
# 39 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
static __inline__ void __attribute__((__always_inline__, __nodebug__)) __wfi(void) {
  __builtin_arm_wfi();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __wfe(void) {
  __builtin_arm_wfe();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __sev(void) {
  __builtin_arm_sev();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __sevl(void) {
  __builtin_arm_sevl();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __yield(void) {
  __builtin_arm_yield();
}







static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__swp(uint32_t __x, volatile uint32_t *__p) {
  uint32_t v;
  do
    v = __builtin_arm_ldrex(__p);
  while (__builtin_arm_strex(__x, __p));
  return v;
}
# 107 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
static __inline__ void __attribute__((__always_inline__, __nodebug__)) __nop(void) {
  __builtin_arm_nop();
}





static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__ror(uint32_t __x, uint32_t __y) {
  __y %= 32;
  if (__y == 0)
    return __x;
  return (__x >> __y) | (__x << (32 - __y));
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__rorll(uint64_t __x, uint32_t __y) {
  __y %= 64;
  if (__y == 0)
    return __x;
  return (__x >> __y) | (__x << (64 - __y));
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__rorl(unsigned long __x, uint32_t __y) {

  return __ror(__x, __y);



}



static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__clz(uint32_t __t) {
  return __builtin_clz(__t);
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__clzl(unsigned long __t) {
  return __builtin_clzl(__t);
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__clzll(uint64_t __t) {
  return __builtin_clzll(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__cls(uint32_t __t) {
  return __builtin_arm_cls(__t);
}

static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__clsl(unsigned long __t) {

  return __builtin_arm_cls(__t);



}

static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__clsll(uint64_t __t) {
  return __builtin_arm_cls64(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__rev(uint32_t __t) {
  return __builtin_bswap32(__t);
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__revl(unsigned long __t) {

  return __builtin_bswap32(__t);



}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__revll(uint64_t __t) {
  return __builtin_bswap64(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__rev16(uint32_t __t) {
  return __ror(__rev(__t), 16);
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__rev16ll(uint64_t __t) {
  return (((uint64_t)__rev16(__t >> 32)) << 32) | __rev16(__t);
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__rev16l(unsigned long __t) {

    return __rev16(__t);



}


static __inline__ int16_t __attribute__((__always_inline__, __nodebug__))
__revsh(int16_t __t) {
  return __builtin_bswap16(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__rbit(uint32_t __t) {
  return __builtin_arm_rbit(__t);
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__rbitll(uint64_t __t) {

  return (((uint64_t)__builtin_arm_rbit(__t)) << 32) |
         __builtin_arm_rbit(__t >> 32);



}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__rbitl(unsigned long __t) {

  return __rbit(__t);



}





static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulbb(int32_t __a, int32_t __b) {
  return __builtin_arm_smulbb(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulbt(int32_t __a, int32_t __b) {
  return __builtin_arm_smulbt(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smultb(int32_t __a, int32_t __b) {
  return __builtin_arm_smultb(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smultt(int32_t __a, int32_t __b) {
  return __builtin_arm_smultt(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulwb(int32_t __a, int32_t __b) {
  return __builtin_arm_smulwb(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulwt(int32_t __a, int32_t __b) {
  return __builtin_arm_smulwt(__a, __b);
}
# 292 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__qadd(int32_t __t, int32_t __v) {
  return __builtin_arm_qadd(__t, __v);
}

static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__qsub(int32_t __t, int32_t __v) {
  return __builtin_arm_qsub(__t, __v);
}

static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__qdbl(int32_t __t) {
  return __builtin_arm_qadd(__t, __t);
}




static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlabb(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlabb(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlabt(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlabt(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlatb(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlatb(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlatt(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlatt(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlawb(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlawb(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlawt(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlawt(__a, __b, __c);
}
# 345 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
typedef int32_t int8x4_t;
typedef int32_t int16x2_t;
typedef uint32_t uint8x4_t;
typedef uint32_t uint16x2_t;

static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sxtab16(int16x2_t __a, int8x4_t __b) {
  return __builtin_arm_sxtab16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sxtb16(int8x4_t __a) {
  return __builtin_arm_sxtb16(__a);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__uxtab16(int16x2_t __a, int8x4_t __b) {
  return __builtin_arm_uxtab16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__uxtb16(int8x4_t __a) {
  return __builtin_arm_uxtb16(__a);
}




static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__sel(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_sel(__a, __b);
}




static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__qadd8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_qadd8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__qsub8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_qsub8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__sadd8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_sadd8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__shadd8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_shadd8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__shsub8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_shsub8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__ssub8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_ssub8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uadd8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uadd8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uhadd8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uhadd8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uhsub8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uhsub8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uqadd8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uqadd8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uqsub8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uqsub8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__usub8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_usub8(__a, __b);
}




static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__usad8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_usad8(__a, __b);
}
static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__usada8(uint8x4_t __a, uint8x4_t __b, uint32_t __c) {
  return __builtin_arm_usada8(__a, __b, __c);
}




static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qadd16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qadd16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qasx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qasx(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qsax(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qsax(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qsub16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qsub16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sadd16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_sadd16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sasx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_sasx(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shadd16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shadd16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shasx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shasx(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shsax(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shsax(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shsub16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shsub16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__ssax(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_ssax(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__ssub16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_ssub16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uadd16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uadd16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uasx(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uasx(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhadd16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhadd16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhasx(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhasx(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhsax(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhsax(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhsub16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhsub16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqadd16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqadd16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqasx(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqasx(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqsax(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqsax(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqsub16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqsub16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__usax(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_usax(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__usub16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_usub16(__a, __b);
}




static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlad(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smlad(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smladx(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smladx(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlald(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlald(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlaldx(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlaldx(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlsd(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smlsd(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlsdx(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smlsdx(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlsld(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlsld(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlsldx(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlsldx(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smuad(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smuad(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smuadx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smuadx(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smusd(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smusd(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smusdx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smusdx(__a, __b);
}
# 8 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 2 3
# 40 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
__disable_fiq(void) {
  unsigned int cpsr;

  __asm__ __volatile__("mrs %[cpsr], faultmask\n"
                       "cpsid f\n"
                       : [cpsr] "=r"(cpsr));
  return cpsr & 0x1;
# 62 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}


static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
__disable_irq(void) {
  unsigned int cpsr;


  __asm__ __volatile__("mrs %[cpsr], primask\n"
                       "cpsid i\n"
                       : [cpsr] "=r"(cpsr));
  return cpsr & 0x1;
# 89 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}







static __inline__ void __attribute__((__always_inline__, __nodebug__))
__enable_fiq(void) {

  __asm__ __volatile__("cpsie f");
# 109 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}


static __inline__ void __attribute__((__always_inline__, __nodebug__))
__enable_irq(void) {

  __asm__ __volatile__("cpsie i");
# 124 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}

static __inline__ void __attribute__((__always_inline__, __nodebug__)) __force_stores(void) {
    __asm__ __volatile__ ("" : : : "memory", "cc");
}

static void __attribute__((__always_inline__, __nodebug__)) __memory_changed(void) {
    __asm__ __volatile__ ("" : : : "memory", "cc");
}

static void __attribute__((__always_inline__, __nodebug__)) __schedule_barrier(void) {
    __asm__ __volatile__ ("" : : : "memory", "cc");
}

static __inline__ int __attribute__((__always_inline__, __nodebug__))
__semihost(int val, const void *ptr) {
  register int v __asm__("r0") = val;
  register const void *p __asm__("r1") = ptr;
  __asm__ __volatile__(


      "bkpt 0xab"
# 160 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
      : "+r"(v), "+r"(p)
      :
      : "memory", "cc");
  return v;
}


static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
__vfp_status(unsigned int mask, unsigned int flags) {
  unsigned int fpscr;
  __asm__ __volatile__("vmrs %[fpscr], fpscr" : [fpscr] "=r"(fpscr));
  unsigned int set = mask & flags;
  unsigned int clear = mask & ~flags;
  unsigned int toggle = ~mask & flags;
  fpscr |= set;
  fpscr &= ~clear;
  fpscr ^= toggle;
  __asm__ __volatile__("vmsr fpscr, %[fpscr]" : : [fpscr] "r"(fpscr));
  return fpscr;
}
# 34 "../Libraries/CMSIS/Include/cmsis_armclang.h" 2 3
# 68 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed)) T_UINT32 { uint32_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT16_WRITE { uint16_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT16_READ { uint16_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT32_WRITE { uint32_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT32_READ { uint32_t v; };
#pragma clang diagnostic pop
# 142 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_CONTROL(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}
# 172 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_CONTROL(uint32_t control)
{
  __asm volatile ("MSR control, %0" : : "r" (control) : "memory");
}
# 196 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_IPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_APSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_xPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_PSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, psp" : "=r" (result) );
  return(result);
}
# 268 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_PSP(uint32_t topOfProcStack)
{
  __asm volatile ("MSR psp, %0" : : "r" (topOfProcStack) : );
}
# 292 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_MSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, msp" : "=r" (result) );
  return(result);
}
# 322 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_MSP(uint32_t topOfMainStack)
{
  __asm volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}
# 373 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_PRIMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}
# 403 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_PRIMASK(uint32_t priMask)
{
  __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}
# 446 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_BASEPRI(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, basepri" : "=r" (result) );
  return(result);
}
# 476 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_BASEPRI(uint32_t basePri)
{
  __asm volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}
# 501 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_BASEPRI_MAX(uint32_t basePri)
{
  __asm volatile ("MSR basepri_max, %0" : : "r" (basePri) : "memory");
}







__attribute__((always_inline)) static __inline uint32_t __get_FAULTMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}
# 542 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_FAULTMASK(uint32_t faultMask)
{
  __asm volatile ("MSR faultmask, %0" : : "r" (faultMask) : "memory");
}
# 876 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  op2 %= 32U;
  if (op2 == 0U)
  {
    return op1;
  }
  return (op1 >> op2) | (op1 << (32U - op2));
}
# 1021 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __RRX(uint32_t value)
{
  uint32_t result;

  __asm volatile ("rrx %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}
# 1036 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint8_t __LDRBT(volatile uint8_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrbt %0, %1" : "=r" (result) : "Q" (*ptr) );
  return ((uint8_t) result);
}
# 1051 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint16_t __LDRHT(volatile uint16_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrht %0, %1" : "=r" (result) : "Q" (*ptr) );
  return ((uint16_t) result);
}
# 1066 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __LDRT(volatile uint32_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrt %0, %1" : "=r" (result) : "Q" (*ptr) );
  return(result);
}
# 1081 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRBT(uint8_t value, volatile uint8_t *ptr)
{
  __asm volatile ("strbt %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}
# 1093 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRHT(uint16_t value, volatile uint16_t *ptr)
{
  __asm volatile ("strht %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}
# 1105 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRT(uint32_t value, volatile uint32_t *ptr)
{
  __asm volatile ("strt %1, %0" : "=Q" (*ptr) : "r" (value) );
}
# 1324 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __SADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("sadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __QADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("qadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SHADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("shadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UQADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uqadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UHADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uhadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}


__attribute__((always_inline)) static __inline uint32_t __SSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("ssub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __QSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("qsub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SHSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("shsub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __USUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("usub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UQSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uqsub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UHSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uhsub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}


__attribute__((always_inline)) static __inline uint32_t __SADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("sadd16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __QADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("qadd16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SHADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("shadd16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uadd16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UQADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uqadd16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UHADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uhadd16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SSUB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("ssub16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __QSUB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("qsub16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SHSUB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("shsub16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __USUB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("usub16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UQSUB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uqsub16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UHSUB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uhsub16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SASX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("sasx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __QASX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("qasx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SHASX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("shasx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UASX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uasx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UQASX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uqasx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UHASX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uhasx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SSAX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("ssax %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __QSAX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("qsax %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SHSAX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("shsax %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __USAX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("usax %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UQSAX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uqsax %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UHSAX(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uhsax %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __USAD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("usad8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __USADA8(uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;

  __asm volatile ("usada8 %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}
# 1644 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __UXTB16(uint32_t op1)
{
  uint32_t result;

  __asm volatile ("uxtb16 %0, %1" : "=r" (result) : "r" (op1));
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __UXTAB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("uxtab16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SXTB16(uint32_t op1)
{
  uint32_t result;

  __asm volatile ("sxtb16 %0, %1" : "=r" (result) : "r" (op1));
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SXTAB16(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("sxtab16 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMUAD (uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("smuad %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMUADX (uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("smuadx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMLAD (uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;

  __asm volatile ("smlad %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMLADX (uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;

  __asm volatile ("smladx %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}

__attribute__((always_inline)) static __inline uint64_t __SMLALD (uint32_t op1, uint32_t op2, uint64_t acc)
{
  union llreg_u{
    uint32_t w32[2];
    uint64_t w64;
  } llr;
  llr.w64 = acc;


  __asm volatile ("smlald %0, %1, %2, %3" : "=r" (llr.w32[0]), "=r" (llr.w32[1]): "r" (op1), "r" (op2) , "0" (llr.w32[0]), "1" (llr.w32[1]) );




  return(llr.w64);
}

__attribute__((always_inline)) static __inline uint64_t __SMLALDX (uint32_t op1, uint32_t op2, uint64_t acc)
{
  union llreg_u{
    uint32_t w32[2];
    uint64_t w64;
  } llr;
  llr.w64 = acc;


  __asm volatile ("smlaldx %0, %1, %2, %3" : "=r" (llr.w32[0]), "=r" (llr.w32[1]): "r" (op1), "r" (op2) , "0" (llr.w32[0]), "1" (llr.w32[1]) );




  return(llr.w64);
}

__attribute__((always_inline)) static __inline uint32_t __SMUSD (uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("smusd %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMUSDX (uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("smusdx %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMLSD (uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;

  __asm volatile ("smlsd %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}

__attribute__((always_inline)) static __inline uint32_t __SMLSDX (uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;

  __asm volatile ("smlsdx %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}

__attribute__((always_inline)) static __inline uint64_t __SMLSLD (uint32_t op1, uint32_t op2, uint64_t acc)
{
  union llreg_u{
    uint32_t w32[2];
    uint64_t w64;
  } llr;
  llr.w64 = acc;


  __asm volatile ("smlsld %0, %1, %2, %3" : "=r" (llr.w32[0]), "=r" (llr.w32[1]): "r" (op1), "r" (op2) , "0" (llr.w32[0]), "1" (llr.w32[1]) );




  return(llr.w64);
}

__attribute__((always_inline)) static __inline uint64_t __SMLSLDX (uint32_t op1, uint32_t op2, uint64_t acc)
{
  union llreg_u{
    uint32_t w32[2];
    uint64_t w64;
  } llr;
  llr.w64 = acc;


  __asm volatile ("smlsldx %0, %1, %2, %3" : "=r" (llr.w32[0]), "=r" (llr.w32[1]): "r" (op1), "r" (op2) , "0" (llr.w32[0]), "1" (llr.w32[1]) );




  return(llr.w64);
}

__attribute__((always_inline)) static __inline uint32_t __SEL (uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __asm volatile ("sel %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline int32_t __QADD( int32_t op1, int32_t op2)
{
  int32_t result;

  __asm volatile ("qadd %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

__attribute__((always_inline)) static __inline int32_t __QSUB( int32_t op1, int32_t op2)
{
  int32_t result;

  __asm volatile ("qsub %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}
# 1857 "../Libraries/CMSIS/Include/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline int32_t __SMMLA (int32_t op1, int32_t op2, int32_t op3)
{
  int32_t result;

  __asm volatile ("smmla %0, %1, %2, %3" : "=r" (result): "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}
# 42 "../Libraries/CMSIS/Include/cmsis_compiler.h" 2 3
# 163 "../Libraries/CMSIS/Include\\core_cm4.h" 2 3
# 259 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t _reserved0:16;
    uint32_t GE:4;
    uint32_t _reserved1:7;
    uint32_t Q:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
  } b;
  uint32_t w;
} APSR_Type;
# 298 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t ISR:9;
    uint32_t _reserved0:23;
  } b;
  uint32_t w;
} IPSR_Type;
# 316 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t ISR:9;
    uint32_t _reserved0:1;
    uint32_t ICI_IT_1:6;
    uint32_t GE:4;
    uint32_t _reserved1:4;
    uint32_t T:1;
    uint32_t ICI_IT_2:2;
    uint32_t Q:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
  } b;
  uint32_t w;
} xPSR_Type;
# 371 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t nPRIV:1;
    uint32_t SPSEL:1;
    uint32_t FPCA:1;
    uint32_t _reserved0:29;
  } b;
  uint32_t w;
} CONTROL_Type;
# 406 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile uint32_t ISER[8U];
        uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];
        uint32_t RSERVED1[24U];
  volatile uint32_t ISPR[8U];
        uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];
        uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];
        uint32_t RESERVED4[56U];
  volatile uint8_t IP[240U];
        uint32_t RESERVED5[644U];
  volatile uint32_t STIR;
} NVIC_Type;
# 440 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile const uint32_t CPUID;
  volatile uint32_t ICSR;
  volatile uint32_t VTOR;
  volatile uint32_t AIRCR;
  volatile uint32_t SCR;
  volatile uint32_t CCR;
  volatile uint8_t SHP[12U];
  volatile uint32_t SHCSR;
  volatile uint32_t CFSR;
  volatile uint32_t HFSR;
  volatile uint32_t DFSR;
  volatile uint32_t MMFAR;
  volatile uint32_t BFAR;
  volatile uint32_t AFSR;
  volatile const uint32_t PFR[2U];
  volatile const uint32_t DFR;
  volatile const uint32_t ADR;
  volatile const uint32_t MMFR[4U];
  volatile const uint32_t ISAR[5U];
        uint32_t RESERVED0[5U];
  volatile uint32_t CPACR;
} SCB_Type;
# 719 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const uint32_t ICTR;
  volatile uint32_t ACTLR;
} SCnSCB_Type;
# 759 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile uint32_t CTRL;
  volatile uint32_t LOAD;
  volatile uint32_t VAL;
  volatile const uint32_t CALIB;
} SysTick_Type;
# 811 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile union
  {
    volatile uint8_t u8;
    volatile uint16_t u16;
    volatile uint32_t u32;
  } PORT [32U];
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;
        uint32_t RESERVED3[29U];
  volatile uint32_t IWR;
  volatile const uint32_t IRR;
  volatile uint32_t IMCR;
        uint32_t RESERVED4[43U];
  volatile uint32_t LAR;
  volatile const uint32_t LSR;
        uint32_t RESERVED5[6U];
  volatile const uint32_t PID4;
  volatile const uint32_t PID5;
  volatile const uint32_t PID6;
  volatile const uint32_t PID7;
  volatile const uint32_t PID0;
  volatile const uint32_t PID1;
  volatile const uint32_t PID2;
  volatile const uint32_t PID3;
  volatile const uint32_t CID0;
  volatile const uint32_t CID1;
  volatile const uint32_t CID2;
  volatile const uint32_t CID3;
} ITM_Type;
# 914 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile uint32_t CTRL;
  volatile uint32_t CYCCNT;
  volatile uint32_t CPICNT;
  volatile uint32_t EXCCNT;
  volatile uint32_t SLEEPCNT;
  volatile uint32_t LSUCNT;
  volatile uint32_t FOLDCNT;
  volatile const uint32_t PCSR;
  volatile uint32_t COMP0;
  volatile uint32_t MASK0;
  volatile uint32_t FUNCTION0;
        uint32_t RESERVED0[1U];
  volatile uint32_t COMP1;
  volatile uint32_t MASK1;
  volatile uint32_t FUNCTION1;
        uint32_t RESERVED1[1U];
  volatile uint32_t COMP2;
  volatile uint32_t MASK2;
  volatile uint32_t FUNCTION2;
        uint32_t RESERVED2[1U];
  volatile uint32_t COMP3;
  volatile uint32_t MASK3;
  volatile uint32_t FUNCTION3;
} DWT_Type;
# 1061 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile const uint32_t SSPSR;
  volatile uint32_t CSPSR;
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;
        uint32_t RESERVED2[131U];
  volatile const uint32_t FFSR;
  volatile uint32_t FFCR;
  volatile const uint32_t FSCR;
        uint32_t RESERVED3[759U];
  volatile const uint32_t TRIGGER;
  volatile const uint32_t FIFO0;
  volatile const uint32_t ITATBCTR2;
        uint32_t RESERVED4[1U];
  volatile const uint32_t ITATBCTR0;
  volatile const uint32_t FIFO1;
  volatile uint32_t ITCTRL;
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;
  volatile uint32_t CLAIMCLR;
        uint32_t RESERVED7[8U];
  volatile const uint32_t DEVID;
  volatile const uint32_t DEVTYPE;
} TPI_Type;
# 1223 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile const uint32_t TYPE;
  volatile uint32_t CTRL;
  volatile uint32_t RNR;
  volatile uint32_t RBAR;
  volatile uint32_t RASR;
  volatile uint32_t RBAR_A1;
  volatile uint32_t RASR_A1;
  volatile uint32_t RBAR_A2;
  volatile uint32_t RASR_A2;
  volatile uint32_t RBAR_A3;
  volatile uint32_t RASR_A3;
} MPU_Type;
# 1319 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile uint32_t FPCCR;
  volatile uint32_t FPCAR;
  volatile uint32_t FPDSCR;
  volatile const uint32_t MVFR0;
  volatile const uint32_t MVFR1;
} FPU_Type;
# 1425 "../Libraries/CMSIS/Include\\core_cm4.h" 3
typedef struct
{
  volatile uint32_t DHCSR;
  volatile uint32_t DCRSR;
  volatile uint32_t DCRDR;
  volatile uint32_t DEMCR;
} CoreDebug_Type;
# 1657 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);

  reg_value = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));
  reg_value = (reg_value |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U) );
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = reg_value;
}







static __inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}
# 1688 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1705 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1724 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __builtin_arm_dsb(0xF);;
    __builtin_arm_isb(0xF);;
  }
}
# 1743 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1762 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1777 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1794 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1816 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)] = (uint8_t)((priority << (8U - 4U)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 4U)) & (uint32_t)0xFFUL);
  }
}
# 1838 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)] >> (8U - 4U)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 4U)));
  }
}
# 1863 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4U)) ? (uint32_t)(4U) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits = ((PriorityGroupTmp + (uint32_t)(4U)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4U));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority & (uint32_t)((1UL << (SubPriorityBits )) - 1UL)))
         );
}
# 1890 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4U)) ? (uint32_t)(4U) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits = ((PriorityGroupTmp + (uint32_t)(4U)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4U));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority = (Priority ) & (uint32_t)((1UL << (SubPriorityBits )) - 1UL);
}
# 1913 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  vectors[(int32_t)IRQn + 16] = vector;
}
# 1928 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return vectors[(int32_t)IRQn + 16];
}






__attribute__((__noreturn__)) static __inline void __NVIC_SystemReset(void)
{
  __builtin_arm_dsb(0xF);;

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = (uint32_t)((0x5FAUL << 16U) |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U) );
  __builtin_arm_dsb(0xF);;

  for(;;)
  {
    __builtin_arm_nop();
  }
}







# 1 "../Libraries/CMSIS/Include/mpu_armv7.h" 1 3
# 29 "../Libraries/CMSIS/Include/mpu_armv7.h" 3
# 180 "../Libraries/CMSIS/Include/mpu_armv7.h" 3
typedef struct {
  uint32_t RBAR;
  uint32_t RASR;
} ARM_MPU_Region_t;




static __inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  __builtin_arm_dsb(0xF);;
  __builtin_arm_isb(0xF);;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);

}



static __inline void ARM_MPU_Disable(void)
{
  __builtin_arm_dsb(0xF);;
  __builtin_arm_isb(0xF);;

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);

  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL &= ~(1UL );
}




static __inline void ARM_MPU_ClrRegion(uint32_t rnr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0U;
}





static __inline void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}






static __inline void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}






static __inline void orderedCpy(volatile uint32_t* dst, const uint32_t* __restrict src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i)
  {
    dst[i] = src[i];
  }
}





static __inline void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt)
{
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > 4U) {
    orderedCpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), 4U*rowWordSize);
    table += 4U;
    cnt -= 4U;
  }
  orderedCpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), cnt*rowWordSize);
}
# 1961 "../Libraries/CMSIS/Include\\core_cm4.h" 2 3
# 1981 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t SCB_GetFPUType(void)
{
  uint32_t mvfr0;

  mvfr0 = ((FPU_Type *) ((0xE000E000UL) + 0x0F30UL) )->MVFR0;
  if ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x020U)
  {
    return 1U;
  }
  else
  {
    return 0U;
  }
}
# 2022 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD = (uint32_t)(ticks - 1UL);
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 4U) - 1UL);
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL = 0UL;
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL = (1UL << 2U) |
                   (1UL << 1U) |
                   (1UL );
  return (0UL);
}
# 2052 "../Libraries/CMSIS/Include\\core_cm4.h" 3
extern volatile int32_t ITM_RxBuffer;
# 2064 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL ) != 0UL) )
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __builtin_arm_nop();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}
# 2085 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;

  if (ITM_RxBuffer != ((int32_t)0x5AA55AA5U))
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ((int32_t)0x5AA55AA5U);
  }

  return (ch);
}
# 2105 "../Libraries/CMSIS/Include\\core_cm4.h" 3
static __inline int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ((int32_t)0x5AA55AA5U))
  {
    return (0);
  }
  else
  {
    return (1);
  }
}
# 168 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h" 2
# 1 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h" 1
# 74 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h"
extern uint32_t SystemCoreClock;

extern const uint8_t AHBPrescTable[16];
extern const uint8_t APBPrescTable[8];
# 103 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h"
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
# 169 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h" 2
# 179 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h"
typedef struct
{
  volatile uint32_t SR;
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t SMPR1;
  volatile uint32_t SMPR2;
  volatile uint32_t JOFR1;
  volatile uint32_t JOFR2;
  volatile uint32_t JOFR3;
  volatile uint32_t JOFR4;
  volatile uint32_t HTR;
  volatile uint32_t LTR;
  volatile uint32_t SQR1;
  volatile uint32_t SQR2;
  volatile uint32_t SQR3;
  volatile uint32_t JSQR;
  volatile uint32_t JDR1;
  volatile uint32_t JDR2;
  volatile uint32_t JDR3;
  volatile uint32_t JDR4;
  volatile uint32_t DR;
} ADC_TypeDef;

typedef struct
{
  volatile uint32_t CSR;
  volatile uint32_t CCR;
  volatile uint32_t CDR;

} ADC_Common_TypeDef;






typedef struct
{
  volatile uint32_t TIR;
  volatile uint32_t TDTR;
  volatile uint32_t TDLR;
  volatile uint32_t TDHR;
} CAN_TxMailBox_TypeDef;





typedef struct
{
  volatile uint32_t RIR;
  volatile uint32_t RDTR;
  volatile uint32_t RDLR;
  volatile uint32_t RDHR;
} CAN_FIFOMailBox_TypeDef;





typedef struct
{
  volatile uint32_t FR1;
  volatile uint32_t FR2;
} CAN_FilterRegister_TypeDef;





typedef struct
{
  volatile uint32_t MCR;
  volatile uint32_t MSR;
  volatile uint32_t TSR;
  volatile uint32_t RF0R;
  volatile uint32_t RF1R;
  volatile uint32_t IER;
  volatile uint32_t ESR;
  volatile uint32_t BTR;
  uint32_t RESERVED0[88];
  CAN_TxMailBox_TypeDef sTxMailBox[3];
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
  uint32_t RESERVED1[12];
  volatile uint32_t FMR;
  volatile uint32_t FM1R;
  uint32_t RESERVED2;
  volatile uint32_t FS1R;
  uint32_t RESERVED3;
  volatile uint32_t FFA1R;
  uint32_t RESERVED4;
  volatile uint32_t FA1R;
  uint32_t RESERVED5[8];
  CAN_FilterRegister_TypeDef sFilterRegister[28];
} CAN_TypeDef;





typedef struct
{
  volatile uint32_t DR;
  volatile uint8_t IDR;
  uint8_t RESERVED0;
  uint16_t RESERVED1;
  volatile uint32_t CR;
} CRC_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t SWTRIGR;
  volatile uint32_t DHR12R1;
  volatile uint32_t DHR12L1;
  volatile uint32_t DHR8R1;
  volatile uint32_t DHR12R2;
  volatile uint32_t DHR12L2;
  volatile uint32_t DHR8R2;
  volatile uint32_t DHR12RD;
  volatile uint32_t DHR12LD;
  volatile uint32_t DHR8RD;
  volatile uint32_t DOR1;
  volatile uint32_t DOR2;
  volatile uint32_t SR;
} DAC_TypeDef;





typedef struct
{
  volatile uint32_t IDCODE;
  volatile uint32_t CR;
  volatile uint32_t APB1FZ;
  volatile uint32_t APB2FZ;
}DBGMCU_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t SR;
  volatile uint32_t RISR;
  volatile uint32_t IER;
  volatile uint32_t MISR;
  volatile uint32_t ICR;
  volatile uint32_t ESCR;
  volatile uint32_t ESUR;
  volatile uint32_t CWSTRTR;
  volatile uint32_t CWSIZER;
  volatile uint32_t DR;
} DCMI_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t NDTR;
  volatile uint32_t PAR;
  volatile uint32_t M0AR;
  volatile uint32_t M1AR;
  volatile uint32_t FCR;
} DMA_Stream_TypeDef;

typedef struct
{
  volatile uint32_t LISR;
  volatile uint32_t HISR;
  volatile uint32_t LIFCR;
  volatile uint32_t HIFCR;
} DMA_TypeDef;





typedef struct
{
  volatile uint32_t MACCR;
  volatile uint32_t MACFFR;
  volatile uint32_t MACHTHR;
  volatile uint32_t MACHTLR;
  volatile uint32_t MACMIIAR;
  volatile uint32_t MACMIIDR;
  volatile uint32_t MACFCR;
  volatile uint32_t MACVLANTR;
  uint32_t RESERVED0[2];
  volatile uint32_t MACRWUFFR;
  volatile uint32_t MACPMTCSR;
  uint32_t RESERVED1;
  volatile uint32_t MACDBGR;
  volatile uint32_t MACSR;
  volatile uint32_t MACIMR;
  volatile uint32_t MACA0HR;
  volatile uint32_t MACA0LR;
  volatile uint32_t MACA1HR;
  volatile uint32_t MACA1LR;
  volatile uint32_t MACA2HR;
  volatile uint32_t MACA2LR;
  volatile uint32_t MACA3HR;
  volatile uint32_t MACA3LR;
  uint32_t RESERVED2[40];
  volatile uint32_t MMCCR;
  volatile uint32_t MMCRIR;
  volatile uint32_t MMCTIR;
  volatile uint32_t MMCRIMR;
  volatile uint32_t MMCTIMR;
  uint32_t RESERVED3[14];
  volatile uint32_t MMCTGFSCCR;
  volatile uint32_t MMCTGFMSCCR;
  uint32_t RESERVED4[5];
  volatile uint32_t MMCTGFCR;
  uint32_t RESERVED5[10];
  volatile uint32_t MMCRFCECR;
  volatile uint32_t MMCRFAECR;
  uint32_t RESERVED6[10];
  volatile uint32_t MMCRGUFCR;
  uint32_t RESERVED7[334];
  volatile uint32_t PTPTSCR;
  volatile uint32_t PTPSSIR;
  volatile uint32_t PTPTSHR;
  volatile uint32_t PTPTSLR;
  volatile uint32_t PTPTSHUR;
  volatile uint32_t PTPTSLUR;
  volatile uint32_t PTPTSAR;
  volatile uint32_t PTPTTHR;
  volatile uint32_t PTPTTLR;
  volatile uint32_t RESERVED8;
  volatile uint32_t PTPTSSR;
  uint32_t RESERVED9[565];
  volatile uint32_t DMABMR;
  volatile uint32_t DMATPDR;
  volatile uint32_t DMARPDR;
  volatile uint32_t DMARDLAR;
  volatile uint32_t DMATDLAR;
  volatile uint32_t DMASR;
  volatile uint32_t DMAOMR;
  volatile uint32_t DMAIER;
  volatile uint32_t DMAMFBOCR;
  volatile uint32_t DMARSWTR;
  uint32_t RESERVED10[8];
  volatile uint32_t DMACHTDR;
  volatile uint32_t DMACHRDR;
  volatile uint32_t DMACHTBAR;
  volatile uint32_t DMACHRBAR;
} ETH_TypeDef;





typedef struct
{
  volatile uint32_t IMR;
  volatile uint32_t EMR;
  volatile uint32_t RTSR;
  volatile uint32_t FTSR;
  volatile uint32_t SWIER;
  volatile uint32_t PR;
} EXTI_TypeDef;





typedef struct
{
  volatile uint32_t ACR;
  volatile uint32_t KEYR;
  volatile uint32_t OPTKEYR;
  volatile uint32_t SR;
  volatile uint32_t CR;
  volatile uint32_t OPTCR;
  volatile uint32_t OPTCR1;
} FLASH_TypeDef;







typedef struct
{
  volatile uint32_t BTCR[8];
} FSMC_Bank1_TypeDef;





typedef struct
{
  volatile uint32_t BWTR[7];
} FSMC_Bank1E_TypeDef;





typedef struct
{
  volatile uint32_t PCR2;
  volatile uint32_t SR2;
  volatile uint32_t PMEM2;
  volatile uint32_t PATT2;
  uint32_t RESERVED0;
  volatile uint32_t ECCR2;
  uint32_t RESERVED1;
  uint32_t RESERVED2;
  volatile uint32_t PCR3;
  volatile uint32_t SR3;
  volatile uint32_t PMEM3;
  volatile uint32_t PATT3;
  uint32_t RESERVED3;
  volatile uint32_t ECCR3;
} FSMC_Bank2_3_TypeDef;





typedef struct
{
  volatile uint32_t PCR4;
  volatile uint32_t SR4;
  volatile uint32_t PMEM4;
  volatile uint32_t PATT4;
  volatile uint32_t PIO4;
} FSMC_Bank4_TypeDef;





typedef struct
{
  volatile uint32_t MODER;
  volatile uint32_t OTYPER;
  volatile uint32_t OSPEEDR;
  volatile uint32_t PUPDR;
  volatile uint32_t IDR;
  volatile uint32_t ODR;
  volatile uint32_t BSRR;
  volatile uint32_t LCKR;
  volatile uint32_t AFR[2];
} GPIO_TypeDef;





typedef struct
{
  volatile uint32_t MEMRMP;
  volatile uint32_t PMC;
  volatile uint32_t EXTICR[4];
  uint32_t RESERVED[2];
  volatile uint32_t CMPCR;
} SYSCFG_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t OAR1;
  volatile uint32_t OAR2;
  volatile uint32_t DR;
  volatile uint32_t SR1;
  volatile uint32_t SR2;
  volatile uint32_t CCR;
  volatile uint32_t TRISE;
} I2C_TypeDef;





typedef struct
{
  volatile uint32_t KR;
  volatile uint32_t PR;
  volatile uint32_t RLR;
  volatile uint32_t SR;
} IWDG_TypeDef;






typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t CSR;
} PWR_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t PLLCFGR;
  volatile uint32_t CFGR;
  volatile uint32_t CIR;
  volatile uint32_t AHB1RSTR;
  volatile uint32_t AHB2RSTR;
  volatile uint32_t AHB3RSTR;
  uint32_t RESERVED0;
  volatile uint32_t APB1RSTR;
  volatile uint32_t APB2RSTR;
  uint32_t RESERVED1[2];
  volatile uint32_t AHB1ENR;
  volatile uint32_t AHB2ENR;
  volatile uint32_t AHB3ENR;
  uint32_t RESERVED2;
  volatile uint32_t APB1ENR;
  volatile uint32_t APB2ENR;
  uint32_t RESERVED3[2];
  volatile uint32_t AHB1LPENR;
  volatile uint32_t AHB2LPENR;
  volatile uint32_t AHB3LPENR;
  uint32_t RESERVED4;
  volatile uint32_t APB1LPENR;
  volatile uint32_t APB2LPENR;
  uint32_t RESERVED5[2];
  volatile uint32_t BDCR;
  volatile uint32_t CSR;
  uint32_t RESERVED6[2];
  volatile uint32_t SSCGR;
  volatile uint32_t PLLI2SCFGR;
} RCC_TypeDef;





typedef struct
{
  volatile uint32_t TR;
  volatile uint32_t DR;
  volatile uint32_t CR;
  volatile uint32_t ISR;
  volatile uint32_t PRER;
  volatile uint32_t WUTR;
  volatile uint32_t CALIBR;
  volatile uint32_t ALRMAR;
  volatile uint32_t ALRMBR;
  volatile uint32_t WPR;
  volatile uint32_t SSR;
  volatile uint32_t SHIFTR;
  volatile uint32_t TSTR;
  volatile uint32_t TSDR;
  volatile uint32_t TSSSR;
  volatile uint32_t CALR;
  volatile uint32_t TAFCR;
  volatile uint32_t ALRMASSR;
  volatile uint32_t ALRMBSSR;
  uint32_t RESERVED7;
  volatile uint32_t BKP0R;
  volatile uint32_t BKP1R;
  volatile uint32_t BKP2R;
  volatile uint32_t BKP3R;
  volatile uint32_t BKP4R;
  volatile uint32_t BKP5R;
  volatile uint32_t BKP6R;
  volatile uint32_t BKP7R;
  volatile uint32_t BKP8R;
  volatile uint32_t BKP9R;
  volatile uint32_t BKP10R;
  volatile uint32_t BKP11R;
  volatile uint32_t BKP12R;
  volatile uint32_t BKP13R;
  volatile uint32_t BKP14R;
  volatile uint32_t BKP15R;
  volatile uint32_t BKP16R;
  volatile uint32_t BKP17R;
  volatile uint32_t BKP18R;
  volatile uint32_t BKP19R;
} RTC_TypeDef;





typedef struct
{
  volatile uint32_t POWER;
  volatile uint32_t CLKCR;
  volatile uint32_t ARG;
  volatile uint32_t CMD;
  volatile const uint32_t RESPCMD;
  volatile const uint32_t RESP1;
  volatile const uint32_t RESP2;
  volatile const uint32_t RESP3;
  volatile const uint32_t RESP4;
  volatile uint32_t DTIMER;
  volatile uint32_t DLEN;
  volatile uint32_t DCTRL;
  volatile const uint32_t DCOUNT;
  volatile const uint32_t STA;
  volatile uint32_t ICR;
  volatile uint32_t MASK;
  uint32_t RESERVED0[2];
  volatile const uint32_t FIFOCNT;
  uint32_t RESERVED1[13];
  volatile uint32_t FIFO;
} SDIO_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t SR;
  volatile uint32_t DR;
  volatile uint32_t CRCPR;
  volatile uint32_t RXCRCR;
  volatile uint32_t TXCRCR;
  volatile uint32_t I2SCFGR;
  volatile uint32_t I2SPR;
} SPI_TypeDef;






typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t SMCR;
  volatile uint32_t DIER;
  volatile uint32_t SR;
  volatile uint32_t EGR;
  volatile uint32_t CCMR1;
  volatile uint32_t CCMR2;
  volatile uint32_t CCER;
  volatile uint32_t CNT;
  volatile uint32_t PSC;
  volatile uint32_t ARR;
  volatile uint32_t RCR;
  volatile uint32_t CCR1;
  volatile uint32_t CCR2;
  volatile uint32_t CCR3;
  volatile uint32_t CCR4;
  volatile uint32_t BDTR;
  volatile uint32_t DCR;
  volatile uint32_t DMAR;
  volatile uint32_t OR;
} TIM_TypeDef;





typedef struct
{
  volatile uint32_t SR;
  volatile uint32_t DR;
  volatile uint32_t BRR;
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t GTPR;
} USART_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t CFR;
  volatile uint32_t SR;
} WWDG_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t SR;
  volatile uint32_t DR;
} RNG_TypeDef;




typedef struct
{
  volatile uint32_t GOTGCTL;
  volatile uint32_t GOTGINT;
  volatile uint32_t GAHBCFG;
  volatile uint32_t GUSBCFG;
  volatile uint32_t GRSTCTL;
  volatile uint32_t GINTSTS;
  volatile uint32_t GINTMSK;
  volatile uint32_t GRXSTSR;
  volatile uint32_t GRXSTSP;
  volatile uint32_t GRXFSIZ;
  volatile uint32_t DIEPTXF0_HNPTXFSIZ;
  volatile uint32_t HNPTXSTS;
  uint32_t Reserved30[2];
  volatile uint32_t GCCFG;
  volatile uint32_t CID;
  uint32_t Reserved40[48];
  volatile uint32_t HPTXFSIZ;
  volatile uint32_t DIEPTXF[0x0F];
} USB_OTG_GlobalTypeDef;




typedef struct
{
  volatile uint32_t DCFG;
  volatile uint32_t DCTL;
  volatile uint32_t DSTS;
  uint32_t Reserved0C;
  volatile uint32_t DIEPMSK;
  volatile uint32_t DOEPMSK;
  volatile uint32_t DAINT;
  volatile uint32_t DAINTMSK;
  uint32_t Reserved20;
  uint32_t Reserved9;
  volatile uint32_t DVBUSDIS;
  volatile uint32_t DVBUSPULSE;
  volatile uint32_t DTHRCTL;
  volatile uint32_t DIEPEMPMSK;
  volatile uint32_t DEACHINT;
  volatile uint32_t DEACHMSK;
  uint32_t Reserved40;
  volatile uint32_t DINEP1MSK;
  uint32_t Reserved44[15];
  volatile uint32_t DOUTEP1MSK;
} USB_OTG_DeviceTypeDef;




typedef struct
{
  volatile uint32_t DIEPCTL;
  uint32_t Reserved04;
  volatile uint32_t DIEPINT;
  uint32_t Reserved0C;
  volatile uint32_t DIEPTSIZ;
  volatile uint32_t DIEPDMA;
  volatile uint32_t DTXFSTS;
  uint32_t Reserved18;
} USB_OTG_INEndpointTypeDef;




typedef struct
{
  volatile uint32_t DOEPCTL;
  uint32_t Reserved04;
  volatile uint32_t DOEPINT;
  uint32_t Reserved0C;
  volatile uint32_t DOEPTSIZ;
  volatile uint32_t DOEPDMA;
  uint32_t Reserved18[2];
} USB_OTG_OUTEndpointTypeDef;




typedef struct
{
  volatile uint32_t HCFG;
  volatile uint32_t HFIR;
  volatile uint32_t HFNUM;
  uint32_t Reserved40C;
  volatile uint32_t HPTXSTS;
  volatile uint32_t HAINT;
  volatile uint32_t HAINTMSK;
} USB_OTG_HostTypeDef;




typedef struct
{
  volatile uint32_t HCCHAR;
  volatile uint32_t HCSPLT;
  volatile uint32_t HCINT;
  volatile uint32_t HCINTMSK;
  volatile uint32_t HCTSIZ;
  volatile uint32_t HCDMA;
  uint32_t Reserved[2];
} USB_OTG_HostChannelTypeDef;
# 134 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h" 2
# 185 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h"
typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;


typedef enum
{
  SUCCESS = 0U,
  ERROR = !SUCCESS
} ErrorStatus;
# 234 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h" 1
# 30 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h"
# 1 "../User\\stm32f4xx_hal_conf.h" 1
# 204 "../User\\stm32f4xx_hal_conf.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h" 1
# 29 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h" 1
# 30 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"
# 1 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h" 1
# 31 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h" 2
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h" 1
# 32 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h" 2
# 1 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 1 3
# 38 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 3
  typedef signed int ptrdiff_t;
# 53 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 3
    typedef unsigned int size_t;
# 71 "E:\\dianzi\\MDKv5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 3
      typedef unsigned short wchar_t;
# 33 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h" 2






typedef enum
{
  HAL_OK = 0x00U,
  HAL_ERROR = 0x01U,
  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;




typedef enum
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED = 0x01U
} HAL_LockTypeDef;
# 30 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
typedef struct
{
  uint32_t PLLState;


  uint32_t PLLSource;


  uint32_t PLLM;


  uint32_t PLLN;



  uint32_t PLLP;


  uint32_t PLLQ;
# 75 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
}RCC_PLLInitTypeDef;
# 384 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
typedef struct
{





  uint32_t PLLI2SN;




  uint32_t PLLI2SR;



}RCC_PLLI2SInitTypeDef;




typedef struct
{
  uint32_t PeriphClockSelection;


  RCC_PLLI2SInitTypeDef PLLI2S;


  uint32_t RTCClockSelection;





}RCC_PeriphCLKInitTypeDef;
# 6730 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef *PeriphClkInit);

uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);
# 6742 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
HAL_StatusTypeDef HAL_RCCEx_EnablePLLI2S(RCC_PLLI2SInitTypeDef *PLLI2SInit);
HAL_StatusTypeDef HAL_RCCEx_DisablePLLI2S(void);
# 34 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h" 2
# 51 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h"
typedef struct
{
  uint32_t OscillatorType;


  uint32_t HSEState;


  uint32_t LSEState;


  uint32_t HSIState;


  uint32_t HSICalibrationValue;


  uint32_t LSIState;


  RCC_PLLInitTypeDef PLL;
}RCC_OscInitTypeDef;




typedef struct
{
  uint32_t ClockType;


  uint32_t SYSCLKSource;


  uint32_t AHBCLKDivider;


  uint32_t APB1CLKDivider;


  uint32_t APB2CLKDivider;


}RCC_ClkInitTypeDef;
# 1247 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h"
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);
# 1258 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rcc.h"
void HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void HAL_RCC_EnableCSS(void);
void HAL_RCC_DisableCSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
void HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);


void HAL_RCC_NMI_IRQHandler(void);


void HAL_RCC_CSSCallback(void);
# 205 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_exti.h" 1
# 45 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_exti.h"
typedef enum
{
  HAL_EXTI_COMMON_CB_ID = 0x00U
} EXTI_CallbackIDTypeDef;




typedef struct
{
  uint32_t Line;
  void (* PendingCallback)(void);
} EXTI_HandleTypeDef;




typedef struct
{
  uint32_t Line;

  uint32_t Mode;

  uint32_t Trigger;

  uint32_t GPIOSel;


} EXTI_ConfigTypeDef;
# 327 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_exti.h"
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);
# 341 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_exti.h"
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti);
# 209 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_gpio.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_gpio.h"
typedef struct
{
  uint32_t Pin;


  uint32_t Mode;


  uint32_t Pull;


  uint32_t Speed;


  uint32_t Alternate;

}GPIO_InitTypeDef;




typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;
# 215 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_gpio.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h" 1
# 216 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_gpio.h" 2
# 226 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_gpio.h"
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
# 236 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_gpio.h"
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
# 213 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h" 1
# 49 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h"
typedef struct
{
  uint32_t Channel;


  uint32_t Direction;



  uint32_t PeriphInc;


  uint32_t MemInc;


  uint32_t PeriphDataAlignment;


  uint32_t MemDataAlignment;


  uint32_t Mode;




  uint32_t Priority;


  uint32_t FIFOMode;




  uint32_t FIFOThreshold;


  uint32_t MemBurst;





  uint32_t PeriphBurst;




}DMA_InitTypeDef;





typedef enum
{
  HAL_DMA_STATE_RESET = 0x00U,
  HAL_DMA_STATE_READY = 0x01U,
  HAL_DMA_STATE_BUSY = 0x02U,
  HAL_DMA_STATE_TIMEOUT = 0x03U,
  HAL_DMA_STATE_ERROR = 0x04U,
  HAL_DMA_STATE_ABORT = 0x05U,
}HAL_DMA_StateTypeDef;




typedef enum
{
  HAL_DMA_FULL_TRANSFER = 0x00U,
  HAL_DMA_HALF_TRANSFER = 0x01U
}HAL_DMA_LevelCompleteTypeDef;




typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID = 0x00U,
  HAL_DMA_XFER_HALFCPLT_CB_ID = 0x01U,
  HAL_DMA_XFER_M1CPLT_CB_ID = 0x02U,
  HAL_DMA_XFER_M1HALFCPLT_CB_ID = 0x03U,
  HAL_DMA_XFER_ERROR_CB_ID = 0x04U,
  HAL_DMA_XFER_ABORT_CB_ID = 0x05U,
  HAL_DMA_XFER_ALL_CB_ID = 0x06U
}HAL_DMA_CallbackIDTypeDef;




typedef struct __DMA_HandleTypeDef
{
  DMA_Stream_TypeDef *Instance;

  DMA_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_DMA_StateTypeDef State;

  void *Parent;

  void (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);

  void (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);

  void (* XferM1CpltCallback)( struct __DMA_HandleTypeDef * hdma);

  void (* XferM1HalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);

  void (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);

  void (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);

  volatile uint32_t ErrorCode;

  uint32_t StreamBaseAddress;

  uint32_t StreamIndex;

}DMA_HandleTypeDef;
# 640 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h"
typedef enum
{
  MEMORY0 = 0x00U,
  MEMORY1 = 0x01U
}HAL_DMA_MemoryTypeDef;
# 70 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h"
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory(DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory);
# 641 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h" 2
# 653 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h"
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);
# 663 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h"
HAL_StatusTypeDef HAL_DMA_Start (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_CleanCallbacks(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);
# 681 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dma.h"
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma);
# 217 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_cortex.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_cortex.h"
typedef struct
{
  uint8_t Enable;

  uint8_t Number;

  uint32_t BaseAddress;
  uint8_t Size;

  uint8_t SubRegionDisable;

  uint8_t TypeExtField;

  uint8_t AccessPermission;

  uint8_t DisableExec;

  uint8_t IsShareable;

  uint8_t IsCacheable;

  uint8_t IsBufferable;

}MPU_Region_InitTypeDef;
# 262 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_cortex.h"
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup);
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_NVIC_SystemReset(void);
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb);
# 276 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_cortex.h"
uint32_t HAL_NVIC_GetPriorityGrouping(void);
void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn);
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource);
void HAL_SYSTICK_IRQHandler(void);
void HAL_SYSTICK_Callback(void);


void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);
# 221 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h" 1
# 32 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h" 1
# 29 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
# 1 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h" 1
# 30 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h" 2
# 1863 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_DMA_GetRegAddr(ADC_TypeDef *ADCx, uint32_t Register)
{
  uint32_t data_reg_addr = 0UL;

  if (Register == 0x00000000UL)
  {

    data_reg_addr = (uint32_t)&(ADCx->DR);
  }
  else
  {

    data_reg_addr = (uint32_t)&(((((ADC_Common_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x2300UL))))->CDR);
  }

  return data_reg_addr;
}
# 1908 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetCommonClock(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t CommonClock)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x3UL << (16U))))) | (CommonClock))));
}
# 1924 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetCommonClock(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x3UL << (16U)))));
}
# 1957 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x1UL << (23U)) | (0x1UL << (22U))))) | (PathInternal))));
}
# 1978 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x1UL << (23U)) | (0x1UL << (22U)))));
}
# 2004 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetResolution(ADC_TypeDef *ADCx, uint32_t Resolution)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x3UL << (24U))))) | (Resolution))));
}
# 2021 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetResolution(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x3UL << (24U)))));
}
# 2037 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetDataAlignment(ADC_TypeDef *ADCx, uint32_t DataAlignment)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (11U))))) | (DataAlignment))));
}
# 2052 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetDataAlignment(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (11U)))));
}
# 2078 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetSequencersScanMode(ADC_TypeDef *ADCx, uint32_t ScanMode)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (8U))))) | (ScanMode))));
}
# 2103 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetSequencersScanMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (8U)))));
}
# 2147 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{





  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0xFUL << (24U))))) | ((TriggerSource & (0xFUL << (24U)))))));
}
# 2191 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetTriggerSource(ADC_TypeDef *ADCx)
{
  uint32_t TriggerSource = ((ADCx->CR2) & ((0xFUL << (24U)) | (0x3UL << (28U))));



  uint32_t ShiftExten = ((TriggerSource & (0x3UL << (28U))) >> ((28UL) - 2UL));



  return ((TriggerSource
           & ((((0x00000000UL & (0xFUL << (24U))) >> (4UL * 0UL)) | (((0xFUL << (24U))) >> (4UL * 1UL)) | (((0xFUL << (24U))) >> (4UL * 2UL)) | (((0xFUL << (24U))) >> (4UL * 3UL))) << ShiftExten) & (0xFUL << (24U)))
          | (((((0x00000000UL & (0x3UL << (28U))) >> (4UL * 0UL)) | ((((0x1UL << (28U)))) >> (4UL * 1UL)) | ((((0x1UL << (28U)))) >> (4UL * 2UL)) | ((((0x1UL << (28U)))) >> (4UL * 3UL))) << ShiftExten) & (0x3UL << (28U)))
         );
}
# 2218 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR2) & ((0x3UL << (28U)))) == (0x00000000UL & (0x3UL << (28U))));
}
# 2235 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x3UL << (28U)))));
}
# 2297 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  (((ADCx->SQR1)) = ((((((ADCx->SQR1))) & (~((0xFUL << (20U))))) | (SequencerNbRanks))));
}
# 2357 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSequencerLength(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->SQR1) & ((0xFUL << (20U)))));
}
# 2385 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (11U)) | (0x7UL << (13U))))) | (SeqDiscont))));
}
# 2408 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (11U)) | (0x7UL << (13U)))));
}
# 2491 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{




  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SQR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~(((0x1FUL << (0U))) << (Rank & (0x0000001FU))))) | ((Channel & ((0x1FUL << (0U)))) << (Rank & (0x0000001FU))))));


}
# 2588 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SQR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  return (uint32_t) (((*preg) & (((0x1FUL << (0U))) << (Rank & (0x0000001FU))))

                     >> (Rank & (0x0000001FU))
                    );
}
# 2613 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetContinuousMode(ADC_TypeDef *ADCx, uint32_t Continuous)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (1U))))) | (Continuous))));
}
# 2630 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetContinuousMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (1U)))));
}
# 2666 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetDMATransfer(ADC_TypeDef *ADCx, uint32_t DMATransfer)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (8U)) | (0x1UL << (9U))))) | (DMATransfer))));
}
# 2701 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetDMATransfer(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (8U)) | (0x1UL << (9U)))));
}
# 2723 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetFlagEndOfConversion(ADC_TypeDef *ADCx, uint32_t EocSelection)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (10U))))) | (EocSelection))));
}
# 2738 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetFlagEndOfConversion(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (10U)))));
}
# 2782 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{





  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0xFUL << (16U))))) | ((TriggerSource & (0xFUL << (16U)))))));
}
# 2826 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetTriggerSource(ADC_TypeDef *ADCx)
{
  uint32_t TriggerSource = ((ADCx->CR2) & ((0xFUL << (16U)) | (0x3UL << (20U))));



  uint32_t ShiftExten = ((TriggerSource & (0x3UL << (20U))) >> ((20UL) - 2UL));



  return ((TriggerSource
           & ((((0x00000000UL & (0xFUL << (16U))) >> (4UL * 0UL)) | (((0xFUL << (16U))) >> (4UL * 1UL)) | (((0xFUL << (16U))) >> (4UL * 2UL)) | (((0xFUL << (16U))) >> (4UL * 3UL))) << ShiftExten) & (0xFUL << (16U)))
          | (((((0x00000000UL & (0x3UL << (20U))) >> (4UL * 0UL)) | ((((0x1UL << (20U)))) >> (4UL * 1UL)) | ((((0x1UL << (20U)))) >> (4UL * 2UL)) | ((((0x1UL << (20U)))) >> (4UL * 3UL))) << ShiftExten) & (0x3UL << (20U)))
         );
}
# 2853 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR2) & ((0x3UL << (20U)))) == (0x00000000UL & (0x3UL << (20U))));
}
# 2868 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x3UL << (20U)))));
}
# 2896 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((0x3UL << (20U))))) | (SequencerNbRanks))));
}
# 2923 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetSequencerLength(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->JSQR) & ((0x3UL << (20U)))));
}
# 2941 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (12U))))) | (SeqDiscont))));
}
# 2956 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (12U)))));
}
# 3008 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{




  uint32_t tmpreg1 = (((ADCx->JSQR) & ((0x3UL << (20U)))) >> (20U)) + 1UL;

  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~(((0x1FUL << (0U))) << (5UL * (uint8_t)(((Rank) + 3UL) - (tmpreg1)))))) | ((Channel & ((0x1FUL << (0U)))) << (5UL * (uint8_t)(((Rank) + 3UL) - (tmpreg1)))))));


}
# 3077 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank)
{
  uint32_t tmpreg1 = (((ADCx->JSQR) & ((0x3UL << (20U)))) >> (20U)) + 1UL;

  return (uint32_t)(((ADCx->JSQR) & (((0x1FUL << (0U))) << (5UL * (uint8_t)(((Rank) + 3UL) - (tmpreg1)))))

                    >> (5UL * (uint8_t)(((Rank) + 3UL) - (tmpreg1)))
                   );
}
# 3113 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetTrigAuto(ADC_TypeDef *ADCx, uint32_t TrigAuto)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (10U))))) | (TrigAuto))));
}
# 3127 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetTrigAuto(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (10U)))));
}
# 3157 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetOffset(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t OffsetLevel)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JOFR1)) + (((((Rank) & ((0x00000000UL | 0x00001000UL | 0x00002000UL | 0x00003000UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00001000UL | 0x00002000UL | 0x00003000UL))))))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (0U))))) | (OffsetLevel))));


}
# 3184 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetOffset(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JOFR1)) + (((((Rank) & ((0x00000000UL | 0x00001000UL | 0x00002000UL | 0x00003000UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00001000UL | 0x00002000UL | 0x00003000UL))))))) << 2UL))));

  return (uint32_t)(((*preg) & ((0xFFFUL << (0U))))

                   );
}
# 3277 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t SamplingTime)
{




  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SMPR1)) + (((((Channel) & ((0x00000000UL | 0x02000000UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x02000000UL))))))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0x7UL << (0U)) << (((Channel) & (0x01F00000UL)) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit((0x01F00000UL)))))))) | (SamplingTime << (((Channel) & (0x01F00000UL)) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit((0x01F00000UL)))))))));


}
# 3354 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SMPR1)) + (((((Channel) & ((0x00000000UL | 0x02000000UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x02000000UL))))))) << 2UL))));

  return (uint32_t)(((*preg) & ((0x7UL << (0U)) << (((Channel) & (0x01F00000UL)) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit((0x01F00000UL)))))))

                    >> (((Channel) & (0x01F00000UL)) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit((0x01F00000UL)))))
                   );
}
# 3468 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetAnalogWDMonitChannels(ADC_TypeDef *ADCx, uint32_t AWDChannelGroup)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~(((0x1UL << (23U)) | (0x1UL << (22U)) | (0x1UL << (9U)) | (0x1FUL << (0U)))))) | (AWDChannelGroup))));


}
# 3564 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetAnalogWDMonitChannels(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & (((0x1UL << (23U)) | (0x1UL << (22U)) | (0x1UL << (9U)) | (0x1FUL << (0U))))));
}
# 3591 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdsHighLow, uint32_t AWDThresholdValue)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->HTR)) + ((AWDThresholdsHighLow) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (0U))))) | (AWDThresholdValue))));


}
# 3614 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdsHighLow)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->HTR)) + ((AWDThresholdsHighLow) << 2UL))));

  return (uint32_t)(((*preg) & ((0xFFFUL << (0U)))));
}
# 3656 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetMultimode(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t Multimode)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x1FUL << (0U))))) | (Multimode))));
}
# 3686 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetMultimode(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x1FUL << (0U)))));
}
# 3735 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiDMATransfer)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x3UL << (14U)) | (0x1UL << (13U))))) | (MultiDMATransfer))));
}
# 3783 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x3UL << (14U)) | (0x1UL << (13U)))));
}
# 3817 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_SetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiTwoSamplingDelay)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0xFUL << (8U))))) | (MultiTwoSamplingDelay))));
}
# 3845 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0xFUL << (8U)))));
}
# 3868 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_Enable(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) |= ((0x1UL << (0U))));
}







static __inline void LL_ADC_Disable(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) &= ~((0x1UL << (0U))));
}







static __inline uint32_t LL_ADC_IsEnabled(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR2) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}
# 3919 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_StartConversionSWStart(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) |= ((0x1UL << (30U))));
}
# 3940 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_StartConversionExtTrig(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  ((ADCx->CR2) |= (ExternalTriggerEdge));
}
# 3958 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_REG_StopConversionExtTrig(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) &= ~((0x3UL << (28U))));
}
# 3972 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_ReadConversionData32(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 3987 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint16_t LL_ADC_REG_ReadConversionData12(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 4002 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint16_t LL_ADC_REG_ReadConversionData10(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 4017 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint8_t LL_ADC_REG_ReadConversionData8(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 4032 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint8_t LL_ADC_REG_ReadConversionData6(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 4059 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_ReadMultiConversionData32(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t ConversionData)
{
  return (uint32_t)(((ADCxy_COMMON->CDR) & ((0xFFFFUL << (16U))))

                    >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(ConversionData)))
                   );
}
# 4092 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_StartConversionSWStart(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) |= ((0x1UL << (22U))));
}
# 4113 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_StartConversionExtTrig(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  ((ADCx->CR2) |= (ExternalTriggerEdge));
}
# 4131 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_INJ_StopConversionExtTrig(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) &= ~((0x3UL << (20U))));
}
# 4153 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_ReadConversionData32(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  return (uint32_t)(((*preg) & ((0xFFFFUL << (0U))))

                   );
}
# 4180 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint16_t LL_ADC_INJ_ReadConversionData12(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  return (uint16_t)(((*preg) & ((0xFFFFUL << (0U))))

                   );
}
# 4207 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint16_t LL_ADC_INJ_ReadConversionData10(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  return (uint16_t)(((*preg) & ((0xFFFFUL << (0U))))

                   );
}
# 4234 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint8_t LL_ADC_INJ_ReadConversionData8(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  return (uint8_t)(((*preg) & ((0xFFFFUL << (0U))))

                  );
}
# 4261 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint8_t LL_ADC_INJ_ReadConversionData6(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))) >> ((uint8_t)__builtin_clz(__builtin_arm_rbit(((0x00000000UL | 0x00000100UL | 0x00000200UL | 0x00000300UL))))))) << 2UL))));

  return (uint8_t)(((*preg) & ((0xFFFFUL << (0U))))

                  );
}
# 4288 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_EOCS(ADC_TypeDef *ADCx)
{
  return (((ADCx->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}







static __inline uint32_t LL_ADC_IsActiveFlag_OVR(ADC_TypeDef *ADCx)
{
  return (((ADCx->SR) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}
# 4311 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_JEOS(ADC_TypeDef *ADCx)
{




  return (((ADCx->SR) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}







static __inline uint32_t LL_ADC_IsActiveFlag_AWD1(ADC_TypeDef *ADCx)
{
  return (((ADCx->SR) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}
# 4341 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_ClearFlag_EOCS(ADC_TypeDef *ADCx)
{
  ((ADCx->SR) = (~(0x1UL << (1U))));
}







static __inline void LL_ADC_ClearFlag_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->SR) = (~(0x1UL << (5U))));
}
# 4364 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_ClearFlag_JEOS(ADC_TypeDef *ADCx)
{




  ((ADCx->SR) = (~(0x1UL << (2U))));
}







static __inline void LL_ADC_ClearFlag_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->SR) = (~(0x1UL << (0U))));
}
# 4396 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_EOCS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((((ADC_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x2000UL))->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}
# 4412 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV1_EOCS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (9U)))) == ((0x1UL << (9U))));
}
# 4428 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV2_EOCS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (17U)))) == ((0x1UL << (17U))));
}







static __inline uint32_t LL_ADC_IsActiveFlag_MST_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}
# 4451 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV1_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (13U)))) == ((0x1UL << (13U))));
}
# 4463 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV2_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (21U)))) == ((0x1UL << (21U))));
}
# 4476 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{




  return (((ADCxy_COMMON->CSR) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}
# 4492 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV1_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{




  return (((ADCxy_COMMON->CSR) & ((0x1UL << (10U)))) == ((0x1UL << (10U))));
}
# 4508 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV2_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{




  return (((ADCxy_COMMON->CSR) & ((0x1UL << (18U)))) == ((0x1UL << (18U))));
}
# 4524 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}
# 4536 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV1_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}
# 4548 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV2_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
    return (((ADCxy_COMMON->CSR) & ((0x1UL << (16U)))) == ((0x1UL << (16U))));
}
# 4573 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_EnableIT_EOCS(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) |= ((0x1UL << (5U))));
}







static __inline void LL_ADC_EnableIT_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) |= ((0x1UL << (26U))));
}
# 4596 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_EnableIT_JEOS(ADC_TypeDef *ADCx)
{




  ((ADCx->CR1) |= ((0x1UL << (7U))));
}







static __inline void LL_ADC_EnableIT_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) |= ((0x1UL << (6U))));
}
# 4626 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_DisableIT_EOCS(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) &= ~((0x1UL << (5U))));
}







static __inline void LL_ADC_DisableIT_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) &= ~((0x1UL << (26U))));
}
# 4649 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline void LL_ADC_DisableIT_JEOS(ADC_TypeDef *ADCx)
{




  ((ADCx->CR1) &= ~((0x1UL << (7U))));
}







static __inline void LL_ADC_DisableIT_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) &= ~((0x1UL << (6U))));
}
# 4680 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_EOCS(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR1) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}
# 4692 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_OVR(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR1) & ((0x1UL << (26U)))) == ((0x1UL << (26U))));
}
# 4705 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_JEOS(ADC_TypeDef *ADCx)
{




  return (((ADCx->CR1) & ((0x1UL << (7U)))) == ((0x1UL << (7U))));
}
# 4721 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_AWD1(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR1) & ((0x1UL << (6U)))) == ((0x1UL << (6U))));
}
# 33 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h" 2
# 60 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
typedef struct
{
  uint32_t ClockPrescaler;


  uint32_t Resolution;

  uint32_t DataAlign;


  uint32_t ScanConvMode;






  uint32_t EOCSelection;






  FunctionalState ContinuousConvMode;


  uint32_t NbrOfConversion;


  FunctionalState DiscontinuousConvMode;



  uint32_t NbrOfDiscConversion;


  uint32_t ExternalTrigConv;



  uint32_t ExternalTrigConvEdge;


  FunctionalState DMAContinuousRequests;




}ADC_InitTypeDef;
# 118 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
typedef struct
{
  uint32_t Channel;

  uint32_t Rank;

  uint32_t SamplingTime;
# 133 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
  uint32_t Offset;
}ADC_ChannelConfTypeDef;




typedef struct
{
  uint32_t WatchdogMode;

  uint32_t HighThreshold;

  uint32_t LowThreshold;

  uint32_t Channel;


  FunctionalState ITMode;


  uint32_t WatchdogNumber;
}ADC_AnalogWDGConfTypeDef;
# 196 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
typedef struct

{
  ADC_TypeDef *Instance;

  ADC_InitTypeDef Init;

  volatile uint32_t NbrOfCurrentConversionRank;

  DMA_HandleTypeDef *DMA_Handle;

  HAL_LockTypeDef Lock;

  volatile uint32_t State;

  volatile uint32_t ErrorCode;
# 221 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
}ADC_HandleTypeDef;
# 555 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_adc_ex.h" 1
# 56 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_adc_ex.h"
typedef struct
{
  uint32_t InjectedChannel;


  uint32_t InjectedRank;


  uint32_t InjectedSamplingTime;
# 73 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_adc_ex.h"
  uint32_t InjectedOffset;



  uint32_t InjectedNbrOfConversion;




  FunctionalState InjectedDiscontinuousConvMode;






  FunctionalState AutoInjectedConv;







  uint32_t ExternalTrigInjecConv;







  uint32_t ExternalTrigInjecConvEdge;




}ADC_InjectionConfTypeDef;




typedef struct
{
  uint32_t Mode;

  uint32_t DMAAccessMode;

  uint32_t TwoSamplingDelay;

}ADC_MultiModeTypeDef;
# 268 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_adc_ex.h"
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef* hadc);
uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef* hadc, uint32_t InjectedRank);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef* hadc);
uint32_t HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef* hadc);
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc);


HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef* hadc,ADC_InjectionConfTypeDef* sConfigInjected);
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef* hadc, ADC_MultiModeTypeDef* multimode);
# 556 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h" 2
# 566 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);
# 584 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);

HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef* hadc, uint32_t EventType, uint32_t Timeout);

HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef* hadc);

void HAL_ADC_IRQHandler(ADC_HandleTypeDef* hadc);

HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef* hadc);

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef* hadc);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
# 612 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig);
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(ADC_HandleTypeDef* hadc, ADC_AnalogWDGConfTypeDef* AnalogWDGConfig);
# 622 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_adc.h"
uint32_t HAL_ADC_GetState(ADC_HandleTypeDef* hadc);
uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc);
# 225 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
typedef enum
{
  HAL_CAN_STATE_RESET = 0x00U,
  HAL_CAN_STATE_READY = 0x01U,
  HAL_CAN_STATE_LISTENING = 0x02U,
  HAL_CAN_STATE_SLEEP_PENDING = 0x03U,
  HAL_CAN_STATE_SLEEP_ACTIVE = 0x04U,
  HAL_CAN_STATE_ERROR = 0x05U

} HAL_CAN_StateTypeDef;




typedef struct
{
  uint32_t Prescaler;


  uint32_t Mode;


  uint32_t SyncJumpWidth;



  uint32_t TimeSeg1;


  uint32_t TimeSeg2;


  FunctionalState TimeTriggeredMode;


  FunctionalState AutoBusOff;


  FunctionalState AutoWakeUp;


  FunctionalState AutoRetransmission;


  FunctionalState ReceiveFifoLocked;


  FunctionalState TransmitFifoPriority;


} CAN_InitTypeDef;




typedef struct
{
  uint32_t FilterIdHigh;



  uint32_t FilterIdLow;



  uint32_t FilterMaskIdHigh;




  uint32_t FilterMaskIdLow;




  uint32_t FilterFIFOAssignment;


  uint32_t FilterBank;





  uint32_t FilterMode;


  uint32_t FilterScale;


  uint32_t FilterActivation;


  uint32_t SlaveStartFilterBank;






} CAN_FilterTypeDef;




typedef struct
{
  uint32_t StdId;


  uint32_t ExtId;


  uint32_t IDE;


  uint32_t RTR;


  uint32_t DLC;


  FunctionalState TransmitGlobalTime;





} CAN_TxHeaderTypeDef;




typedef struct
{
  uint32_t StdId;


  uint32_t ExtId;


  uint32_t IDE;


  uint32_t RTR;


  uint32_t DLC;


  uint32_t Timestamp;



  uint32_t FilterMatchIndex;


} CAN_RxHeaderTypeDef;




typedef struct __CAN_HandleTypeDef
{
  CAN_TypeDef *Instance;

  CAN_InitTypeDef Init;

  volatile HAL_CAN_StateTypeDef State;

  volatile uint32_t ErrorCode;
# 239 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
} CAN_HandleTypeDef;
# 641 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_DeInit(CAN_HandleTypeDef *hcan);
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan);
void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan);
# 662 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *sFilterConfig);
# 674 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_RequestSleep(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_WakeUp(CAN_HandleTypeDef *hcan);
uint32_t HAL_CAN_IsSleepActive(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox);
HAL_StatusTypeDef HAL_CAN_AbortTxRequest(CAN_HandleTypeDef *hcan, uint32_t TxMailboxes);
uint32_t HAL_CAN_GetTxMailboxesFreeLevel(CAN_HandleTypeDef *hcan);
uint32_t HAL_CAN_IsTxMessagePending(CAN_HandleTypeDef *hcan, uint32_t TxMailboxes);
uint32_t HAL_CAN_GetTxTimestamp(CAN_HandleTypeDef *hcan, uint32_t TxMailbox);
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);
uint32_t HAL_CAN_GetRxFifoFillLevel(CAN_HandleTypeDef *hcan, uint32_t RxFifo);
# 696 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs);
HAL_StatusTypeDef HAL_CAN_DeactivateNotification(CAN_HandleTypeDef *hcan, uint32_t InactiveITs);
void HAL_CAN_IRQHandler(CAN_HandleTypeDef *hcan);
# 710 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_SleepCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);
# 733 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_can.h"
HAL_CAN_StateTypeDef HAL_CAN_GetState(CAN_HandleTypeDef *hcan);
uint32_t HAL_CAN_GetError(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_ResetError(CAN_HandleTypeDef *hcan);
# 229 "../User\\stm32f4xx_hal_conf.h" 2







# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_crc.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_crc.h"
typedef enum
{
  HAL_CRC_STATE_RESET = 0x00U,
  HAL_CRC_STATE_READY = 0x01U,
  HAL_CRC_STATE_BUSY = 0x02U,
  HAL_CRC_STATE_TIMEOUT = 0x03U,
  HAL_CRC_STATE_ERROR = 0x04U
} HAL_CRC_StateTypeDef;





typedef struct
{
  CRC_TypeDef *Instance;

  HAL_LockTypeDef Lock;

  volatile HAL_CRC_StateTypeDef State;

} CRC_HandleTypeDef;
# 139 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_crc.h"
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
HAL_StatusTypeDef HAL_CRC_DeInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc);
# 151 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_crc.h"
uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
# 161 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_crc.h"
HAL_CRC_StateTypeDef HAL_CRC_GetState(CRC_HandleTypeDef *hcrc);
# 237 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_cryp.h" 1
# 241 "../User\\stm32f4xx_hal_conf.h" 2







# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h" 1
# 50 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
typedef enum
{
  HAL_DAC_STATE_RESET = 0x00U,
  HAL_DAC_STATE_READY = 0x01U,
  HAL_DAC_STATE_BUSY = 0x02U,
  HAL_DAC_STATE_TIMEOUT = 0x03U,
  HAL_DAC_STATE_ERROR = 0x04U

} HAL_DAC_StateTypeDef;







typedef struct

{
  DAC_TypeDef *Instance;

  volatile HAL_DAC_StateTypeDef State;

  HAL_LockTypeDef Lock;

  DMA_HandleTypeDef *DMA_Handle1;

  DMA_HandleTypeDef *DMA_Handle2;

  volatile uint32_t ErrorCode;
# 97 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
} DAC_HandleTypeDef;




typedef struct
{
  uint32_t DAC_Trigger;


  uint32_t DAC_OutputBuffer;


} DAC_ChannelConfTypeDef;
# 381 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dac_ex.h" 1
# 147 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dac_ex.h"
HAL_StatusTypeDef HAL_DACEx_TriangleWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Amplitude);
HAL_StatusTypeDef HAL_DACEx_NoiseWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Amplitude);



HAL_StatusTypeDef HAL_DACEx_DualStart(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DACEx_DualStop(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DACEx_DualSetValue(DAC_HandleTypeDef *hdac, uint32_t Alignment, uint32_t Data1, uint32_t Data2);
uint32_t HAL_DACEx_DualGetValue(DAC_HandleTypeDef *hdac);



void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ErrorCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_DMAUnderrunCallbackCh2(DAC_HandleTypeDef *hdac);
# 183 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dac_ex.h"
void DAC_DMAConvCpltCh2(DMA_HandleTypeDef *hdma);
void DAC_DMAErrorCh2(DMA_HandleTypeDef *hdma);
void DAC_DMAHalfConvCpltCh2(DMA_HandleTypeDef *hdma);
# 382 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h" 2
# 393 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DAC_DeInit(DAC_HandleTypeDef *hdac);
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac);
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac);
# 406 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
HAL_StatusTypeDef HAL_DAC_Start(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Stop(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t *pData, uint32_t Length,
                                    uint32_t Alignment);
HAL_StatusTypeDef HAL_DAC_Stop_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel);
void HAL_DAC_IRQHandler(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DAC_SetValue(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Alignment, uint32_t Data);

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_DMAUnderrunCallbackCh1(DAC_HandleTypeDef *hdac);
# 434 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
uint32_t HAL_DAC_GetValue(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef *hdac, DAC_ChannelConfTypeDef *sConfig, uint32_t Channel);
# 444 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
HAL_DAC_StateTypeDef HAL_DAC_GetState(DAC_HandleTypeDef *hdac);
uint32_t HAL_DAC_GetError(DAC_HandleTypeDef *hdac);
# 458 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dac.h"
void DAC_DMAConvCpltCh1(DMA_HandleTypeDef *hdma);
void DAC_DMAErrorCh1(DMA_HandleTypeDef *hdma);
void DAC_DMAHalfConvCpltCh1(DMA_HandleTypeDef *hdma);
# 249 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h" 1
# 36 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dcmi_ex.h" 1
# 52 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dcmi_ex.h"
typedef struct
{
  uint8_t FrameStartCode;
  uint8_t LineStartCode;
  uint8_t LineEndCode;
  uint8_t FrameEndCode;
}DCMI_CodesInitTypeDef;




typedef struct
{
  uint32_t SynchroMode;


  uint32_t PCKPolarity;


  uint32_t VSPolarity;


  uint32_t HSPolarity;


  uint32_t CaptureRate;


  uint32_t ExtendedDataMode;


  DCMI_CodesInitTypeDef SyncroCode;

  uint32_t JPEGMode;
# 101 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dcmi_ex.h"
}DCMI_InitTypeDef;
# 37 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h" 2
# 54 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
typedef struct
{
  uint8_t FrameStartUnmask;
  uint8_t LineStartUnmask;
  uint8_t LineEndUnmask;
  uint8_t FrameEndUnmask;
}DCMI_SyncUnmaskTypeDef;



typedef enum
{
  HAL_DCMI_STATE_RESET = 0x00U,
  HAL_DCMI_STATE_READY = 0x01U,
  HAL_DCMI_STATE_BUSY = 0x02U,
  HAL_DCMI_STATE_TIMEOUT = 0x03U,
  HAL_DCMI_STATE_ERROR = 0x04U,
  HAL_DCMI_STATE_SUSPENDED = 0x05U
}HAL_DCMI_StateTypeDef;




typedef struct __DCMI_HandleTypeDef
{
  DCMI_TypeDef *Instance;

  DCMI_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_DCMI_StateTypeDef State;

  volatile uint32_t XferCount;

  volatile uint32_t XferSize;

  uint32_t XferTransferNumber;

  uint32_t pBuffPtr;

  DMA_HandleTypeDef *DMA_Handle;

  volatile uint32_t ErrorCode;
# 106 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
}DCMI_HandleTypeDef;
# 431 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
HAL_StatusTypeDef HAL_DCMI_Init(DCMI_HandleTypeDef *hdcmi);
HAL_StatusTypeDef HAL_DCMI_DeInit(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi);
void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* hdcmi);
# 449 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
HAL_StatusTypeDef HAL_DCMI_Start_DMA(DCMI_HandleTypeDef* hdcmi, uint32_t DCMI_Mode, uint32_t pData, uint32_t Length);
HAL_StatusTypeDef HAL_DCMI_Stop(DCMI_HandleTypeDef* hdcmi);
HAL_StatusTypeDef HAL_DCMI_Suspend(DCMI_HandleTypeDef* hdcmi);
HAL_StatusTypeDef HAL_DCMI_Resume(DCMI_HandleTypeDef* hdcmi);
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_VsyncCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_HsyncCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_IRQHandler(DCMI_HandleTypeDef *hdcmi);
# 468 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
HAL_StatusTypeDef HAL_DCMI_ConfigCrop(DCMI_HandleTypeDef *hdcmi, uint32_t X0, uint32_t Y0, uint32_t XSize, uint32_t YSize);
HAL_StatusTypeDef HAL_DCMI_EnableCrop(DCMI_HandleTypeDef *hdcmi);
HAL_StatusTypeDef HAL_DCMI_DisableCrop(DCMI_HandleTypeDef *hdcmi);
HAL_StatusTypeDef HAL_DCMI_ConfigSyncUnmask(DCMI_HandleTypeDef *hdcmi, DCMI_SyncUnmaskTypeDef *SyncUnmask);
# 480 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_dcmi.h"
HAL_DCMI_StateTypeDef HAL_DCMI_GetState(DCMI_HandleTypeDef *hdcmi);
uint32_t HAL_DCMI_GetError(DCMI_HandleTypeDef *hdcmi);
# 253 "../User\\stm32f4xx_hal_conf.h" 2







# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h"
typedef enum
{
  FLASH_PROC_NONE = 0U,
  FLASH_PROC_SECTERASE,
  FLASH_PROC_MASSERASE,
  FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;




typedef struct
{
  volatile FLASH_ProcedureTypeDef ProcedureOnGoing;

  volatile uint32_t NbSectorsToErase;

  volatile uint8_t VoltageForErase;

  volatile uint32_t Sector;

  volatile uint32_t Bank;

  volatile uint32_t Address;

  HAL_LockTypeDef Lock;

  volatile uint32_t ErrorCode;

}FLASH_ProcessTypeDef;
# 297 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
typedef struct
{
  uint32_t TypeErase;


  uint32_t Banks;


  uint32_t Sector;


  uint32_t NbSectors;


  uint32_t VoltageRange;


} FLASH_EraseInitTypeDef;




typedef struct
{
  uint32_t OptionType;


  uint32_t WRPState;


  uint32_t WRPSector;


  uint32_t Banks;


  uint32_t RDPLevel;


  uint32_t BORLevel;


  uint8_t USERConfig;

} FLASH_OBProgramInitTypeDef;
# 727 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);
# 1046 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
void FLASH_Erase_Sector(uint32_t Sector, uint8_t VoltageRange);
void FLASH_FlushCaches(void);
# 298 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h" 2
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ramfunc.h" 1
# 299 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h" 2
# 308 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h"
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);

void HAL_FLASH_IRQHandler(void);

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
# 323 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h"
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);

HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);
# 337 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_flash.h"
uint32_t HAL_FLASH_GetError(void);
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
# 261 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h" 1
# 31 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_fsmc.h" 1
# 49 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_fsmc.h"
typedef struct
{
  uint32_t NSBank;


  uint32_t DataAddressMux;



  uint32_t MemoryType;



  uint32_t MemoryDataWidth;


  uint32_t BurstAccessMode;



  uint32_t WaitSignalPolarity;



  uint32_t WrapMode;




  uint32_t WaitSignalActive;




  uint32_t WriteOperation;


  uint32_t WaitSignal;



  uint32_t ExtendedMode;


  uint32_t AsynchronousWait;



  uint32_t WriteBurst;


  uint32_t ContinuousClock;





  uint32_t WriteFifo;





  uint32_t PageSize;

}FSMC_NORSRAM_InitTypeDef;




typedef struct
{
  uint32_t AddressSetupTime;




  uint32_t AddressHoldTime;




  uint32_t DataSetupTime;





  uint32_t BusTurnAroundDuration;




  uint32_t CLKDivision;




  uint32_t DataLatency;







  uint32_t AccessMode;


}FSMC_NORSRAM_TimingTypeDef;





typedef struct
{
  uint32_t NandBank;


  uint32_t Waitfeature;


  uint32_t MemoryDataWidth;


  uint32_t EccComputation;


  uint32_t ECCPageSize;


  uint32_t TCLRSetupTime;



  uint32_t TARSetupTime;



}FSMC_NAND_InitTypeDef;




typedef struct
{
  uint32_t SetupTime;





  uint32_t WaitSetupTime;





  uint32_t HoldSetupTime;






  uint32_t HiZSetupTime;





}FSMC_NAND_PCC_TimingTypeDef;




typedef struct
{
  uint32_t Waitfeature;


  uint32_t TCLRSetupTime;



  uint32_t TARSetupTime;



}FSMC_PCCARD_InitTypeDef;
# 947 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_fsmc.h"
HAL_StatusTypeDef FSMC_NORSRAM_Init(FSMC_Bank1_TypeDef *Device, FSMC_NORSRAM_InitTypeDef *Init);
HAL_StatusTypeDef FSMC_NORSRAM_Timing_Init(FSMC_Bank1_TypeDef *Device, FSMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef FSMC_NORSRAM_Extended_Timing_Init(FSMC_Bank1E_TypeDef *Device, FSMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode);
HAL_StatusTypeDef FSMC_NORSRAM_DeInit(FSMC_Bank1_TypeDef *Device, FSMC_Bank1E_TypeDef *ExDevice, uint32_t Bank);







HAL_StatusTypeDef FSMC_NORSRAM_WriteOperation_Enable(FSMC_Bank1_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef FSMC_NORSRAM_WriteOperation_Disable(FSMC_Bank1_TypeDef *Device, uint32_t Bank);
# 974 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_fsmc.h"
HAL_StatusTypeDef FSMC_NAND_Init(FSMC_Bank2_3_TypeDef *Device, FSMC_NAND_InitTypeDef *Init);
HAL_StatusTypeDef FSMC_NAND_CommonSpace_Timing_Init(FSMC_Bank2_3_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef FSMC_NAND_AttributeSpace_Timing_Init(FSMC_Bank2_3_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef FSMC_NAND_DeInit(FSMC_Bank2_3_TypeDef *Device, uint32_t Bank);







HAL_StatusTypeDef FSMC_NAND_ECC_Enable(FSMC_Bank2_3_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef FSMC_NAND_ECC_Disable(FSMC_Bank2_3_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef FSMC_NAND_GetECC(FSMC_Bank2_3_TypeDef *Device, uint32_t *ECCval, uint32_t Bank, uint32_t Timeout);
# 1001 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_fsmc.h"
HAL_StatusTypeDef FSMC_PCCARD_Init(FSMC_Bank4_TypeDef *Device, FSMC_PCCARD_InitTypeDef *Init);
HAL_StatusTypeDef FSMC_PCCARD_CommonSpace_Timing_Init(FSMC_Bank4_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing);
HAL_StatusTypeDef FSMC_PCCARD_AttributeSpace_Timing_Init(FSMC_Bank4_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing);
HAL_StatusTypeDef FSMC_PCCARD_IOSpace_Timing_Init(FSMC_Bank4_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing);
HAL_StatusTypeDef FSMC_PCCARD_DeInit(FSMC_Bank4_TypeDef *Device);
# 32 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h" 2
# 61 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h"
typedef enum
{
  HAL_SRAM_STATE_RESET = 0x00U,
  HAL_SRAM_STATE_READY = 0x01U,
  HAL_SRAM_STATE_BUSY = 0x02U,
  HAL_SRAM_STATE_ERROR = 0x03U,
  HAL_SRAM_STATE_PROTECTED = 0x04U

} HAL_SRAM_StateTypeDef;







typedef struct

{
  FSMC_Bank1_TypeDef *Instance;

  FSMC_Bank1E_TypeDef *Extended;

  FSMC_NORSRAM_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_SRAM_StateTypeDef State;

  DMA_HandleTypeDef *hdma;







} SRAM_HandleTypeDef;
# 155 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h"
HAL_StatusTypeDef HAL_SRAM_Init(SRAM_HandleTypeDef *hsram, FSMC_NORSRAM_TimingTypeDef *Timing, FSMC_NORSRAM_TimingTypeDef *ExtTiming);
HAL_StatusTypeDef HAL_SRAM_DeInit(SRAM_HandleTypeDef *hsram);
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram);
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram);

void HAL_SRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma);
void HAL_SRAM_DMA_XferErrorCallback(DMA_HandleTypeDef *hdma);
# 170 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h"
HAL_StatusTypeDef HAL_SRAM_Read_8b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_8b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pSrcBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Read_16b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint16_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_16b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint16_t *pSrcBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Read_32b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_32b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pSrcBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Read_DMA(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_DMA(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pSrcBuffer, uint32_t BufferSize);
# 192 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h"
HAL_StatusTypeDef HAL_SRAM_WriteOperation_Enable(SRAM_HandleTypeDef *hsram);
HAL_StatusTypeDef HAL_SRAM_WriteOperation_Disable(SRAM_HandleTypeDef *hsram);
# 202 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sram.h"
HAL_SRAM_StateTypeDef HAL_SRAM_GetState(SRAM_HandleTypeDef *hsram);
# 265 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nor.h" 1
# 60 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nor.h"
typedef enum
{
  HAL_NOR_STATE_RESET = 0x00U,
  HAL_NOR_STATE_READY = 0x01U,
  HAL_NOR_STATE_BUSY = 0x02U,
  HAL_NOR_STATE_ERROR = 0x03U,
  HAL_NOR_STATE_PROTECTED = 0x04U
}HAL_NOR_StateTypeDef;




typedef enum
{
  HAL_NOR_STATUS_SUCCESS = 0U,
  HAL_NOR_STATUS_ONGOING,
  HAL_NOR_STATUS_ERROR,
  HAL_NOR_STATUS_TIMEOUT
}HAL_NOR_StatusTypeDef;




typedef struct
{
  uint16_t Manufacturer_Code;

  uint16_t Device_Code1;

  uint16_t Device_Code2;

  uint16_t Device_Code3;



}NOR_IDTypeDef;




typedef struct
{




  uint16_t CFI_1;

  uint16_t CFI_2;

  uint16_t CFI_3;

  uint16_t CFI_4;
}NOR_CFITypeDef;







typedef struct


{
  FSMC_Bank1_TypeDef *Instance;

  FSMC_Bank1E_TypeDef *Extended;

  FSMC_NORSRAM_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_NOR_StateTypeDef State;





} NOR_HandleTypeDef;
# 191 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nor.h"
HAL_StatusTypeDef HAL_NOR_Init(NOR_HandleTypeDef *hnor, FSMC_NORSRAM_TimingTypeDef *Timing, FSMC_NORSRAM_TimingTypeDef *ExtTiming);
HAL_StatusTypeDef HAL_NOR_DeInit(NOR_HandleTypeDef *hnor);
void HAL_NOR_MspInit(NOR_HandleTypeDef *hnor);
void HAL_NOR_MspDeInit(NOR_HandleTypeDef *hnor);
void HAL_NOR_MspWait(NOR_HandleTypeDef *hnor, uint32_t Timeout);
# 204 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nor.h"
HAL_StatusTypeDef HAL_NOR_Read_ID(NOR_HandleTypeDef *hnor, NOR_IDTypeDef *pNOR_ID);
HAL_StatusTypeDef HAL_NOR_ReturnToReadMode(NOR_HandleTypeDef *hnor);
HAL_StatusTypeDef HAL_NOR_Read(NOR_HandleTypeDef *hnor, uint32_t *pAddress, uint16_t *pData);
HAL_StatusTypeDef HAL_NOR_Program(NOR_HandleTypeDef *hnor, uint32_t *pAddress, uint16_t *pData);

HAL_StatusTypeDef HAL_NOR_ReadBuffer(NOR_HandleTypeDef *hnor, uint32_t uwAddress, uint16_t *pData, uint32_t uwBufferSize);
HAL_StatusTypeDef HAL_NOR_ProgramBuffer(NOR_HandleTypeDef *hnor, uint32_t uwAddress, uint16_t *pData, uint32_t uwBufferSize);

HAL_StatusTypeDef HAL_NOR_Erase_Block(NOR_HandleTypeDef *hnor, uint32_t BlockAddress, uint32_t Address);
HAL_StatusTypeDef HAL_NOR_Erase_Chip(NOR_HandleTypeDef *hnor, uint32_t Address);
HAL_StatusTypeDef HAL_NOR_Read_CFI(NOR_HandleTypeDef *hnor, NOR_CFITypeDef *pNOR_CFI);
# 229 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nor.h"
HAL_StatusTypeDef HAL_NOR_WriteOperation_Enable(NOR_HandleTypeDef *hnor);
HAL_StatusTypeDef HAL_NOR_WriteOperation_Disable(NOR_HandleTypeDef *hnor);
# 239 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nor.h"
HAL_NOR_StateTypeDef HAL_NOR_GetState(NOR_HandleTypeDef *hnor);
HAL_NOR_StatusTypeDef HAL_NOR_GetStatus(NOR_HandleTypeDef *hnor, uint32_t Address, uint32_t Timeout);
# 269 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nand.h" 1
# 60 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nand.h"
typedef enum
{
  HAL_NAND_STATE_RESET = 0x00U,
  HAL_NAND_STATE_READY = 0x01U,
  HAL_NAND_STATE_BUSY = 0x02U,
  HAL_NAND_STATE_ERROR = 0x03U
}HAL_NAND_StateTypeDef;




typedef struct
{


  uint8_t Maker_Id;

  uint8_t Device_Id;

  uint8_t Third_Id;

  uint8_t Fourth_Id;
}NAND_IDTypeDef;




typedef struct
{
  uint16_t Page;

  uint16_t Plane;

  uint16_t Block;

}NAND_AddressTypeDef;




typedef struct
{
  uint32_t PageSize;


  uint32_t SpareAreaSize;


  uint32_t BlockSize;

  uint32_t BlockNbr;

  uint32_t PlaneNbr;

  uint32_t PlaneSize;

  FunctionalState ExtraCommandEnable;





}NAND_DeviceConfigTypeDef;







typedef struct

{
  FSMC_Bank2_3_TypeDef *Instance;

  FSMC_NAND_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_NAND_StateTypeDef State;

  NAND_DeviceConfigTypeDef Config;






} NAND_HandleTypeDef;
# 206 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nand.h"
HAL_StatusTypeDef HAL_NAND_Init(NAND_HandleTypeDef *hnand, FSMC_NAND_PCC_TimingTypeDef *ComSpace_Timing, FSMC_NAND_PCC_TimingTypeDef *AttSpace_Timing);
HAL_StatusTypeDef HAL_NAND_DeInit(NAND_HandleTypeDef *hnand);

HAL_StatusTypeDef HAL_NAND_ConfigDevice(NAND_HandleTypeDef *hnand, NAND_DeviceConfigTypeDef *pDeviceConfig);

HAL_StatusTypeDef HAL_NAND_Read_ID(NAND_HandleTypeDef *hnand, NAND_IDTypeDef *pNAND_ID);

void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand);
void HAL_NAND_MspDeInit(NAND_HandleTypeDef *hnand);
void HAL_NAND_IRQHandler(NAND_HandleTypeDef *hnand);
void HAL_NAND_ITCallback(NAND_HandleTypeDef *hnand);
# 227 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nand.h"
HAL_StatusTypeDef HAL_NAND_Reset(NAND_HandleTypeDef *hnand);

HAL_StatusTypeDef HAL_NAND_Read_Page_8b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToRead);
HAL_StatusTypeDef HAL_NAND_Write_Page_8b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToWrite);
HAL_StatusTypeDef HAL_NAND_Read_SpareArea_8b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaToRead);
HAL_StatusTypeDef HAL_NAND_Write_SpareArea_8b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaTowrite);

HAL_StatusTypeDef HAL_NAND_Read_Page_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumPageToRead);
HAL_StatusTypeDef HAL_NAND_Write_Page_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumPageToWrite);
HAL_StatusTypeDef HAL_NAND_Read_SpareArea_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumSpareAreaToRead);
HAL_StatusTypeDef HAL_NAND_Write_SpareArea_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumSpareAreaTowrite);

HAL_StatusTypeDef HAL_NAND_Erase_Block(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress);

uint32_t HAL_NAND_Read_Status(NAND_HandleTypeDef *hnand);
uint32_t HAL_NAND_Address_Inc(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress);
# 259 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nand.h"
HAL_StatusTypeDef HAL_NAND_ECC_Enable(NAND_HandleTypeDef *hnand);
HAL_StatusTypeDef HAL_NAND_ECC_Disable(NAND_HandleTypeDef *hnand);
HAL_StatusTypeDef HAL_NAND_GetECC(NAND_HandleTypeDef *hnand, uint32_t *ECCval, uint32_t Timeout);
# 271 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_nand.h"
HAL_NAND_StateTypeDef HAL_NAND_GetState(NAND_HandleTypeDef *hnand);
# 273 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pccard.h" 1
# 56 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pccard.h"
typedef enum
{
  HAL_PCCARD_STATE_RESET = 0x00U,
  HAL_PCCARD_STATE_READY = 0x01U,
  HAL_PCCARD_STATE_BUSY = 0x02U,
  HAL_PCCARD_STATE_ERROR = 0x04U
}HAL_PCCARD_StateTypeDef;

typedef enum
{
  HAL_PCCARD_STATUS_SUCCESS = 0U,
  HAL_PCCARD_STATUS_ONGOING,
  HAL_PCCARD_STATUS_ERROR,
  HAL_PCCARD_STATUS_TIMEOUT
}HAL_PCCARD_StatusTypeDef;







typedef struct

{
  FSMC_Bank4_TypeDef *Instance;

  FSMC_PCCARD_InitTypeDef Init;

  volatile HAL_PCCARD_StateTypeDef State;

  HAL_LockTypeDef Lock;






} PCCARD_HandleTypeDef;
# 147 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pccard.h"
HAL_StatusTypeDef HAL_PCCARD_Init(PCCARD_HandleTypeDef *hpccard, FSMC_NAND_PCC_TimingTypeDef *ComSpaceTiming, FSMC_NAND_PCC_TimingTypeDef *AttSpaceTiming, FSMC_NAND_PCC_TimingTypeDef *IOSpaceTiming);
HAL_StatusTypeDef HAL_PCCARD_DeInit(PCCARD_HandleTypeDef *hpccard);
void HAL_PCCARD_MspInit(PCCARD_HandleTypeDef *hpccard);
void HAL_PCCARD_MspDeInit(PCCARD_HandleTypeDef *hpccard);
# 159 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pccard.h"
HAL_StatusTypeDef HAL_PCCARD_Read_ID(PCCARD_HandleTypeDef *hpccard, uint8_t CompactFlash_ID[], uint8_t *pStatus);
HAL_StatusTypeDef HAL_PCCARD_Write_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t *pBuffer, uint16_t SectorAddress, uint8_t *pStatus);
HAL_StatusTypeDef HAL_PCCARD_Read_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t *pBuffer, uint16_t SectorAddress, uint8_t *pStatus);
HAL_StatusTypeDef HAL_PCCARD_Erase_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t SectorAddress, uint8_t *pStatus);
HAL_StatusTypeDef HAL_PCCARD_Reset(PCCARD_HandleTypeDef *hpccard);
void HAL_PCCARD_IRQHandler(PCCARD_HandleTypeDef *hpccard);
void HAL_PCCARD_ITCallback(PCCARD_HandleTypeDef *hpccard);
# 180 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pccard.h"
HAL_PCCARD_StateTypeDef HAL_PCCARD_GetState(PCCARD_HandleTypeDef *hpccard);
HAL_PCCARD_StatusTypeDef HAL_PCCARD_GetStatus(PCCARD_HandleTypeDef *hpccard);
HAL_PCCARD_StatusTypeDef HAL_PCCARD_ReadStatus(PCCARD_HandleTypeDef *hpccard);
# 277 "../User\\stm32f4xx_hal_conf.h" 2







# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hash.h" 1
# 285 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
typedef struct
{
  uint32_t ClockSpeed;


  uint32_t DutyCycle;


  uint32_t OwnAddress1;


  uint32_t AddressingMode;


  uint32_t DualAddressMode;


  uint32_t OwnAddress2;


  uint32_t GeneralCallMode;


  uint32_t NoStretchMode;


} I2C_InitTypeDef;
# 107 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
typedef enum
{
  HAL_I2C_STATE_RESET = 0x00U,
  HAL_I2C_STATE_READY = 0x20U,
  HAL_I2C_STATE_BUSY = 0x24U,
  HAL_I2C_STATE_BUSY_TX = 0x21U,
  HAL_I2C_STATE_BUSY_RX = 0x22U,
  HAL_I2C_STATE_LISTEN = 0x28U,
  HAL_I2C_STATE_BUSY_TX_LISTEN = 0x29U,

  HAL_I2C_STATE_BUSY_RX_LISTEN = 0x2AU,

  HAL_I2C_STATE_ABORT = 0x60U,
  HAL_I2C_STATE_TIMEOUT = 0xA0U,
  HAL_I2C_STATE_ERROR = 0xE0U

} HAL_I2C_StateTypeDef;
# 147 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
typedef enum
{
  HAL_I2C_MODE_NONE = 0x00U,
  HAL_I2C_MODE_MASTER = 0x10U,
  HAL_I2C_MODE_SLAVE = 0x20U,
  HAL_I2C_MODE_MEM = 0x40U

} HAL_I2C_ModeTypeDef;
# 188 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
typedef struct

{
  I2C_TypeDef *Instance;

  I2C_InitTypeDef Init;

  uint8_t *pBuffPtr;

  uint16_t XferSize;

  volatile uint16_t XferCount;

  volatile uint32_t XferOptions;

  volatile uint32_t PreviousState;


  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_I2C_StateTypeDef State;

  volatile HAL_I2C_ModeTypeDef Mode;

  volatile uint32_t ErrorCode;

  volatile uint32_t Devaddress;

  volatile uint32_t Memaddress;

  volatile uint32_t MemaddSize;

  volatile uint32_t EventCount;
# 244 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
} I2C_HandleTypeDef;
# 534 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_i2c_ex.h" 1
# 535 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h" 2
# 545 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);
# 567 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);


HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress);


HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
# 611 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c);
# 631 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2c.h"
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c);
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c);
# 289 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h"
typedef struct
{
  uint32_t Mode;


  uint32_t Standard;


  uint32_t DataFormat;


  uint32_t MCLKOutput;


  uint32_t AudioFreq;


  uint32_t CPOL;


  uint32_t ClockSource;

  uint32_t FullDuplexMode;

} I2S_InitTypeDef;




typedef enum
{
  HAL_I2S_STATE_RESET = 0x00U,
  HAL_I2S_STATE_READY = 0x01U,
  HAL_I2S_STATE_BUSY = 0x02U,
  HAL_I2S_STATE_BUSY_TX = 0x03U,
  HAL_I2S_STATE_BUSY_RX = 0x04U,
  HAL_I2S_STATE_BUSY_TX_RX = 0x05U,
  HAL_I2S_STATE_TIMEOUT = 0x06U,
  HAL_I2S_STATE_ERROR = 0x07U
} HAL_I2S_StateTypeDef;




typedef struct __I2S_HandleTypeDef
{
  SPI_TypeDef *Instance;

  I2S_InitTypeDef Init;

  uint16_t *pTxBuffPtr;

  volatile uint16_t TxXferSize;

  volatile uint16_t TxXferCount;

  uint16_t *pRxBuffPtr;

  volatile uint16_t RxXferSize;

  volatile uint16_t RxXferCount;





  void (*IrqHandlerISR)(struct __I2S_HandleTypeDef *hi2s);

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  volatile HAL_LockTypeDef Lock;

  volatile HAL_I2S_StateTypeDef State;

  volatile uint32_t ErrorCode;
# 138 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h"
} I2S_HandleTypeDef;
# 439 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_i2s_ex.h" 1
# 139 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_i2s_ex.h"
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData,
                                            uint16_t Size, uint32_t Timeout);

HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData,
                                               uint16_t Size);

HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData,
                                                uint16_t Size);

void HAL_I2SEx_FullDuplex_IRQHandler(I2S_HandleTypeDef *hi2s);
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s);
# 440 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h" 2
# 450 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h"
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DeInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s);
# 470 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h"
HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout);


HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s);


HAL_StatusTypeDef HAL_I2S_Transmit_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2S_DMAPause(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAResume(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAStop(I2S_HandleTypeDef *hi2s);


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s);
# 500 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_i2s.h"
HAL_I2S_StateTypeDef HAL_I2S_GetState(I2S_HandleTypeDef *hi2s);
uint32_t HAL_I2S_GetError(I2S_HandleTypeDef *hi2s);
# 293 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_iwdg.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_iwdg.h"
typedef struct
{
  uint32_t Prescaler;


  uint32_t Reload;


} IWDG_InitTypeDef;




typedef struct
{
  IWDG_TypeDef *Instance;

  IWDG_InitTypeDef Init;
} IWDG_HandleTypeDef;
# 128 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_iwdg.h"
HAL_StatusTypeDef HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg);
# 137 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_iwdg.h"
HAL_StatusTypeDef HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg);
# 297 "../User\\stm32f4xx_hal_conf.h" 2







# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pwr.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pwr.h"
typedef struct
{
  uint32_t PVDLevel;


  uint32_t Mode;

}PWR_PVDTypeDef;
# 274 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pwr.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h" 1
# 205 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"
void HAL_PWREx_EnableFlashPowerDown(void);
void HAL_PWREx_DisableFlashPowerDown(void);
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void);
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
# 275 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pwr.h" 2
# 285 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pwr.h"
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);
# 297 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pwr.h"
void HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);


void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinx);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);


void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);


void HAL_PWR_PVD_IRQHandler(void);
void HAL_PWR_PVDCallback(void);


void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);
# 305 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h" 1
# 59 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h"
typedef enum
{
  HAL_RNG_STATE_RESET = 0x00U,
  HAL_RNG_STATE_READY = 0x01U,
  HAL_RNG_STATE_BUSY = 0x02U,
  HAL_RNG_STATE_TIMEOUT = 0x03U,
  HAL_RNG_STATE_ERROR = 0x04U

} HAL_RNG_StateTypeDef;
# 79 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h"
typedef struct

{
  RNG_TypeDef *Instance;

  HAL_LockTypeDef Lock;

  volatile HAL_RNG_StateTypeDef State;

  volatile uint32_t ErrorCode;

  uint32_t RandomNumber;
# 100 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h"
} RNG_HandleTypeDef;
# 280 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h"
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng);
# 301 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h"
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_GetRandomNumber_IT(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef *hrng, uint32_t *random32bit);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_IT(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_ReadLastRandomNumber(RNG_HandleTypeDef *hrng);

void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng);
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng);
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit);
# 318 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rng.h"
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_GetError(RNG_HandleTypeDef *hrng);
# 309 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
typedef enum
{
  HAL_RTC_STATE_RESET = 0x00U,
  HAL_RTC_STATE_READY = 0x01U,
  HAL_RTC_STATE_BUSY = 0x02U,
  HAL_RTC_STATE_TIMEOUT = 0x03U,
  HAL_RTC_STATE_ERROR = 0x04U
}HAL_RTCStateTypeDef;




typedef struct
{
  uint32_t HourFormat;


  uint32_t AsynchPrediv;


  uint32_t SynchPrediv;


  uint32_t OutPut;


  uint32_t OutPutPolarity;


  uint32_t OutPutType;

}RTC_InitTypeDef;




typedef struct
{
  uint8_t Hours;



  uint8_t Minutes;


  uint8_t Seconds;


  uint8_t TimeFormat;


  uint32_t SubSeconds;



  uint32_t SecondFraction;





  uint32_t DayLightSaving;


  uint32_t StoreOperation;

}RTC_TimeTypeDef;




typedef struct
{
  uint8_t WeekDay;


  uint8_t Month;


  uint8_t Date;


  uint8_t Year;


}RTC_DateTypeDef;




typedef struct
{
  RTC_TimeTypeDef AlarmTime;

  uint32_t AlarmMask;


  uint32_t AlarmSubSecondMask;


  uint32_t AlarmDateWeekDaySel;


  uint8_t AlarmDateWeekDay;



  uint32_t Alarm;

}RTC_AlarmTypeDef;







typedef struct

{
  RTC_TypeDef *Instance;

  RTC_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_RTCStateTypeDef State;
# 194 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
}RTC_HandleTypeDef;
# 670 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rtc_ex.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rtc_ex.h"
typedef struct
{
  uint32_t Tamper;


  uint32_t PinSelection;


  uint32_t Trigger;


  uint32_t Filter;


  uint32_t SamplingFrequency;


  uint32_t PrechargeDuration;


  uint32_t TamperPullUp;


  uint32_t TimeStampOnTamperDetection;

}RTC_TamperTypeDef;
# 808 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rtc_ex.h"
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTimeStamp, RTC_DateTypeDef *sTimeStampDate, uint32_t Format);

HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef* sTamper);
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef* sTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef *hrtc, uint32_t Tamper);
void HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc);

void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc);
void HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef *hrtc);
void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
HAL_StatusTypeDef HAL_RTCEx_PollForTamper2Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
# 832 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rtc_ex.h"
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
uint32_t HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc);
uint32_t HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc);
void HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc);
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
# 847 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rtc_ex.h"
void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data);
uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister);

HAL_StatusTypeDef HAL_RTCEx_SetCoarseCalib(RTC_HandleTypeDef *hrtc, uint32_t CalibSign, uint32_t Value);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCoarseCalib(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod, uint32_t SmoothCalibPlusPulses, uint32_t SmouthCalibMinusPulsesValue);
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef *hrtc, uint32_t ShiftAdd1S, uint32_t ShiftSubFS);
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef *hrtc, uint32_t CalibOutput);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef *hrtc);
# 868 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rtc_ex.h"
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
# 671 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h" 2
# 681 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);
# 699 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
void HAL_RTC_DST_Add1Hour(RTC_HandleTypeDef *hrtc);
void HAL_RTC_DST_Sub1Hour(RTC_HandleTypeDef *hrtc);
void HAL_RTC_DST_SetStoreOperation(RTC_HandleTypeDef *hrtc);
void HAL_RTC_DST_ClearStoreOperation(RTC_HandleTypeDef *hrtc);
uint32_t HAL_RTC_DST_ReadStoreOperation(RTC_HandleTypeDef *hrtc);
# 716 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Alarm, uint32_t Format);
void HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
# 731 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
HAL_StatusTypeDef HAL_RTC_WaitForSynchro(RTC_HandleTypeDef* hrtc);
# 740 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
HAL_RTCStateTypeDef HAL_RTC_GetState(RTC_HandleTypeDef *hrtc);
# 857 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_rtc.h"
HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef* hrtc);
uint8_t RTC_ByteToBcd2(uint8_t Value);
uint8_t RTC_Bcd2ToByte(uint8_t Value);
# 313 "../User\\stm32f4xx_hal_conf.h" 2







# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h" 1
# 31 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_sdmmc.h" 1
# 49 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_sdmmc.h"
typedef struct
{
  uint32_t ClockEdge;


  uint32_t ClockBypass;



  uint32_t ClockPowerSave;



  uint32_t BusWide;


  uint32_t HardwareFlowControl;


  uint32_t ClockDiv;


}SDIO_InitTypeDef;





typedef struct
{
  uint32_t Argument;




  uint32_t CmdIndex;


  uint32_t Response;


  uint32_t WaitForInterrupt;



  uint32_t CPSM;


}SDIO_CmdInitTypeDef;





typedef struct
{
  uint32_t DataTimeOut;

  uint32_t DataLength;

  uint32_t DataBlockSize;


  uint32_t TransferDir;



  uint32_t TransferMode;


  uint32_t DPSM;


}SDIO_DataInitTypeDef;
# 1037 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_sdmmc.h"
HAL_StatusTypeDef SDIO_Init(SDIO_TypeDef *SDIOx, SDIO_InitTypeDef Init);
# 1046 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_sdmmc.h"
uint32_t SDIO_ReadFIFO(SDIO_TypeDef *SDIOx);
HAL_StatusTypeDef SDIO_WriteFIFO(SDIO_TypeDef *SDIOx, uint32_t *pWriteData);
# 1056 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_sdmmc.h"
HAL_StatusTypeDef SDIO_PowerState_ON(SDIO_TypeDef *SDIOx);
HAL_StatusTypeDef SDIO_PowerState_OFF(SDIO_TypeDef *SDIOx);
uint32_t SDIO_GetPowerState(SDIO_TypeDef *SDIOx);


HAL_StatusTypeDef SDIO_SendCommand(SDIO_TypeDef *SDIOx, SDIO_CmdInitTypeDef *Command);
uint8_t SDIO_GetCommandResponse(SDIO_TypeDef *SDIOx);
uint32_t SDIO_GetResponse(SDIO_TypeDef *SDIOx, uint32_t Response);


HAL_StatusTypeDef SDIO_ConfigData(SDIO_TypeDef *SDIOx, SDIO_DataInitTypeDef* Data);
uint32_t SDIO_GetDataCounter(SDIO_TypeDef *SDIOx);
uint32_t SDIO_GetFIFOCount(SDIO_TypeDef *SDIOx);


HAL_StatusTypeDef SDIO_SetSDMMCReadWaitMode(SDIO_TypeDef *SDIOx, uint32_t SDIO_ReadWaitMode);


uint32_t SDMMC_CmdBlockLength(SDIO_TypeDef *SDIOx, uint32_t BlockSize);
uint32_t SDMMC_CmdReadSingleBlock(SDIO_TypeDef *SDIOx, uint32_t ReadAdd);
uint32_t SDMMC_CmdReadMultiBlock(SDIO_TypeDef *SDIOx, uint32_t ReadAdd);
uint32_t SDMMC_CmdWriteSingleBlock(SDIO_TypeDef *SDIOx, uint32_t WriteAdd);
uint32_t SDMMC_CmdWriteMultiBlock(SDIO_TypeDef *SDIOx, uint32_t WriteAdd);
uint32_t SDMMC_CmdEraseStartAdd(SDIO_TypeDef *SDIOx, uint32_t StartAdd);
uint32_t SDMMC_CmdSDEraseStartAdd(SDIO_TypeDef *SDIOx, uint32_t StartAdd);
uint32_t SDMMC_CmdEraseEndAdd(SDIO_TypeDef *SDIOx, uint32_t EndAdd);
uint32_t SDMMC_CmdSDEraseEndAdd(SDIO_TypeDef *SDIOx, uint32_t EndAdd);
uint32_t SDMMC_CmdErase(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdStopTransfer(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdSelDesel(SDIO_TypeDef *SDIOx, uint64_t Addr);
uint32_t SDMMC_CmdGoIdleState(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdOperCond(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdAppCommand(SDIO_TypeDef *SDIOx, uint32_t Argument);
uint32_t SDMMC_CmdAppOperCommand(SDIO_TypeDef *SDIOx, uint32_t Argument);
uint32_t SDMMC_CmdBusWidth(SDIO_TypeDef *SDIOx, uint32_t BusWidth);
uint32_t SDMMC_CmdSendSCR(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdSendCID(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdSendCSD(SDIO_TypeDef *SDIOx, uint32_t Argument);
uint32_t SDMMC_CmdSendEXTCSD(SDIO_TypeDef *SDIOx, uint32_t Argument);
uint32_t SDMMC_CmdSetRelAdd(SDIO_TypeDef *SDIOx, uint16_t *pRCA);
uint32_t SDMMC_CmdSendStatus(SDIO_TypeDef *SDIOx, uint32_t Argument);
uint32_t SDMMC_CmdStatusRegister(SDIO_TypeDef *SDIOx);
uint32_t SDMMC_CmdOpCondition(SDIO_TypeDef *SDIOx, uint32_t Argument);
uint32_t SDMMC_CmdSwitch(SDIO_TypeDef *SDIOx, uint32_t Argument);
# 32 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h" 2
# 50 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
typedef enum
{
  HAL_SD_STATE_RESET = 0x00000000U,
  HAL_SD_STATE_READY = 0x00000001U,
  HAL_SD_STATE_TIMEOUT = 0x00000002U,
  HAL_SD_STATE_BUSY = 0x00000003U,
  HAL_SD_STATE_PROGRAMMING = 0x00000004U,
  HAL_SD_STATE_RECEIVING = 0x00000005U,
  HAL_SD_STATE_TRANSFER = 0x00000006U,
  HAL_SD_STATE_ERROR = 0x0000000FU
}HAL_SD_StateTypeDef;







typedef uint32_t HAL_SD_CardStateTypeDef;
# 92 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
typedef struct
{
  uint32_t CardType;

  uint32_t CardVersion;

  uint32_t Class;

  uint32_t RelCardAdd;

  uint32_t BlockNbr;

  uint32_t BlockSize;

  uint32_t LogBlockNbr;

  uint32_t LogBlockSize;

}HAL_SD_CardInfoTypeDef;







typedef struct

{
  SDIO_TypeDef *Instance;

  SDIO_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  uint8_t *pTxBuffPtr;

  uint32_t TxXferSize;

  uint8_t *pRxBuffPtr;

  uint32_t RxXferSize;

  volatile uint32_t Context;

  volatile HAL_SD_StateTypeDef State;

  volatile uint32_t ErrorCode;

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_SD_CardInfoTypeDef SdCard;

  uint32_t CSD[4];

  uint32_t CID[4];
# 160 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
}SD_HandleTypeDef;
# 169 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
typedef struct
{
  volatile uint8_t CSDStruct;
  volatile uint8_t SysSpecVersion;
  volatile uint8_t Reserved1;
  volatile uint8_t TAAC;
  volatile uint8_t NSAC;
  volatile uint8_t MaxBusClkFrec;
  volatile uint16_t CardComdClasses;
  volatile uint8_t RdBlockLen;
  volatile uint8_t PartBlockRead;
  volatile uint8_t WrBlockMisalign;
  volatile uint8_t RdBlockMisalign;
  volatile uint8_t DSRImpl;
  volatile uint8_t Reserved2;
  volatile uint32_t DeviceSize;
  volatile uint8_t MaxRdCurrentVDDMin;
  volatile uint8_t MaxRdCurrentVDDMax;
  volatile uint8_t MaxWrCurrentVDDMin;
  volatile uint8_t MaxWrCurrentVDDMax;
  volatile uint8_t DeviceSizeMul;
  volatile uint8_t EraseGrSize;
  volatile uint8_t EraseGrMul;
  volatile uint8_t WrProtectGrSize;
  volatile uint8_t WrProtectGrEnable;
  volatile uint8_t ManDeflECC;
  volatile uint8_t WrSpeedFact;
  volatile uint8_t MaxWrBlockLen;
  volatile uint8_t WriteBlockPaPartial;
  volatile uint8_t Reserved3;
  volatile uint8_t ContentProtectAppli;
  volatile uint8_t FileFormatGroup;
  volatile uint8_t CopyFlag;
  volatile uint8_t PermWrProtect;
  volatile uint8_t TempWrProtect;
  volatile uint8_t FileFormat;
  volatile uint8_t ECC;
  volatile uint8_t CSD_CRC;
  volatile uint8_t Reserved4;
}HAL_SD_CardCSDTypeDef;







typedef struct
{
  volatile uint8_t ManufacturerID;
  volatile uint16_t OEM_AppliID;
  volatile uint32_t ProdName1;
  volatile uint8_t ProdName2;
  volatile uint8_t ProdRev;
  volatile uint32_t ProdSN;
  volatile uint8_t Reserved1;
  volatile uint16_t ManufactDate;
  volatile uint8_t CID_CRC;
  volatile uint8_t Reserved2;

}HAL_SD_CardCIDTypeDef;







typedef struct
{
  volatile uint8_t DataBusWidth;
  volatile uint8_t SecuredMode;
  volatile uint16_t CardType;
  volatile uint32_t ProtectedAreaSize;
  volatile uint8_t SpeedClass;
  volatile uint8_t PerformanceMove;
  volatile uint8_t AllocationUnitSize;
  volatile uint16_t EraseSize;
  volatile uint8_t EraseTimeout;
  volatile uint8_t EraseOffset;

}HAL_SD_CardStatusTypeDef;
# 596 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
HAL_StatusTypeDef HAL_SD_Init(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef HAL_SD_InitCard(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef HAL_SD_DeInit (SD_HandleTypeDef *hsd);
void HAL_SD_MspInit(SD_HandleTypeDef *hsd);
void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd);
# 609 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
HAL_StatusTypeDef HAL_SD_ReadBlocks(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks, uint32_t Timeout);
HAL_StatusTypeDef HAL_SD_WriteBlocks(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks, uint32_t Timeout);
HAL_StatusTypeDef HAL_SD_Erase(SD_HandleTypeDef *hsd, uint32_t BlockStartAdd, uint32_t BlockEndAdd);

HAL_StatusTypeDef HAL_SD_ReadBlocks_IT(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks);
HAL_StatusTypeDef HAL_SD_WriteBlocks_IT(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks);

HAL_StatusTypeDef HAL_SD_ReadBlocks_DMA(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks);
HAL_StatusTypeDef HAL_SD_WriteBlocks_DMA(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks);

void HAL_SD_IRQHandler(SD_HandleTypeDef *hsd);


void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd);
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd);
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd);
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd);
# 640 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_sd.h"
HAL_StatusTypeDef HAL_SD_ConfigWideBusOperation(SD_HandleTypeDef *hsd, uint32_t WideMode);







HAL_StatusTypeDef HAL_SD_SendSDStatus(SD_HandleTypeDef *hsd, uint32_t *pSDstatus);
HAL_SD_CardStateTypeDef HAL_SD_GetCardState(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef HAL_SD_GetCardCID(SD_HandleTypeDef *hsd, HAL_SD_CardCIDTypeDef *pCID);
HAL_StatusTypeDef HAL_SD_GetCardCSD(SD_HandleTypeDef *hsd, HAL_SD_CardCSDTypeDef *pCSD);
HAL_StatusTypeDef HAL_SD_GetCardStatus(SD_HandleTypeDef *hsd, HAL_SD_CardStatusTypeDef *pStatus);
HAL_StatusTypeDef HAL_SD_GetCardInfo(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypeDef *pCardInfo);







HAL_SD_StateTypeDef HAL_SD_GetState(SD_HandleTypeDef *hsd);
uint32_t HAL_SD_GetError(SD_HandleTypeDef *hsd);







HAL_StatusTypeDef HAL_SD_Abort(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef HAL_SD_Abort_IT(SD_HandleTypeDef *hsd);
# 321 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_spi.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_spi.h"
typedef struct
{
  uint32_t Mode;


  uint32_t Direction;


  uint32_t DataSize;


  uint32_t CLKPolarity;


  uint32_t CLKPhase;


  uint32_t NSS;



  uint32_t BaudRatePrescaler;





  uint32_t FirstBit;


  uint32_t TIMode;


  uint32_t CRCCalculation;


  uint32_t CRCPolynomial;

} SPI_InitTypeDef;




typedef enum
{
  HAL_SPI_STATE_RESET = 0x00U,
  HAL_SPI_STATE_READY = 0x01U,
  HAL_SPI_STATE_BUSY = 0x02U,
  HAL_SPI_STATE_BUSY_TX = 0x03U,
  HAL_SPI_STATE_BUSY_RX = 0x04U,
  HAL_SPI_STATE_BUSY_TX_RX = 0x05U,
  HAL_SPI_STATE_ERROR = 0x06U,
  HAL_SPI_STATE_ABORT = 0x07U
} HAL_SPI_StateTypeDef;




typedef struct __SPI_HandleTypeDef
{
  SPI_TypeDef *Instance;

  SPI_InitTypeDef Init;

  uint8_t *pTxBuffPtr;

  uint16_t TxXferSize;

  volatile uint16_t TxXferCount;

  uint8_t *pRxBuffPtr;

  uint16_t RxXferSize;

  volatile uint16_t RxXferCount;

  void (*RxISR)(struct __SPI_HandleTypeDef *hspi);

  void (*TxISR)(struct __SPI_HandleTypeDef *hspi);

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_SPI_StateTypeDef State;

  volatile uint32_t ErrorCode;
# 150 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_spi.h"
} SPI_HandleTypeDef;
# 652 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_spi.h"
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi);
# 670 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_spi.h"
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,
                                          uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t Size);
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi);

HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi);

void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);
# 706 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_spi.h"
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi);
uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *hspi);
# 325 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
typedef struct
{
  uint32_t Prescaler;


  uint32_t CounterMode;


  uint32_t Period;



  uint32_t ClockDivision;


  uint32_t RepetitionCounter;
# 71 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
  uint32_t AutoReloadPreload;

} TIM_Base_InitTypeDef;




typedef struct
{
  uint32_t OCMode;


  uint32_t Pulse;


  uint32_t OCPolarity;


  uint32_t OCNPolarity;



  uint32_t OCFastMode;




  uint32_t OCIdleState;



  uint32_t OCNIdleState;


} TIM_OC_InitTypeDef;




typedef struct
{
  uint32_t OCMode;


  uint32_t Pulse;


  uint32_t OCPolarity;


  uint32_t OCNPolarity;



  uint32_t OCIdleState;



  uint32_t OCNIdleState;



  uint32_t ICPolarity;


  uint32_t ICSelection;


  uint32_t ICFilter;

} TIM_OnePulse_InitTypeDef;




typedef struct
{
  uint32_t ICPolarity;


  uint32_t ICSelection;


  uint32_t ICPrescaler;


  uint32_t ICFilter;

} TIM_IC_InitTypeDef;




typedef struct
{
  uint32_t EncoderMode;


  uint32_t IC1Polarity;


  uint32_t IC1Selection;


  uint32_t IC1Prescaler;


  uint32_t IC1Filter;


  uint32_t IC2Polarity;


  uint32_t IC2Selection;


  uint32_t IC2Prescaler;


  uint32_t IC2Filter;

} TIM_Encoder_InitTypeDef;




typedef struct
{
  uint32_t ClockSource;

  uint32_t ClockPolarity;

  uint32_t ClockPrescaler;

  uint32_t ClockFilter;

} TIM_ClockConfigTypeDef;




typedef struct
{
  uint32_t ClearInputState;

  uint32_t ClearInputSource;

  uint32_t ClearInputPolarity;

  uint32_t ClearInputPrescaler;

  uint32_t ClearInputFilter;

} TIM_ClearInputConfigTypeDef;




typedef struct
{
  uint32_t MasterOutputTrigger;

  uint32_t MasterSlaveMode;






} TIM_MasterConfigTypeDef;




typedef struct
{
  uint32_t SlaveMode;

  uint32_t InputTrigger;

  uint32_t TriggerPolarity;

  uint32_t TriggerPrescaler;

  uint32_t TriggerFilter;


} TIM_SlaveConfigTypeDef;






typedef struct
{
  uint32_t OffStateRunMode;

  uint32_t OffStateIDLEMode;

  uint32_t LockLevel;

  uint32_t DeadTime;

  uint32_t BreakState;

  uint32_t BreakPolarity;

  uint32_t BreakFilter;

  uint32_t AutomaticOutput;

} TIM_BreakDeadTimeConfigTypeDef;




typedef enum
{
  HAL_TIM_STATE_RESET = 0x00U,
  HAL_TIM_STATE_READY = 0x01U,
  HAL_TIM_STATE_BUSY = 0x02U,
  HAL_TIM_STATE_TIMEOUT = 0x03U,
  HAL_TIM_STATE_ERROR = 0x04U
} HAL_TIM_StateTypeDef;




typedef enum
{
  HAL_TIM_CHANNEL_STATE_RESET = 0x00U,
  HAL_TIM_CHANNEL_STATE_READY = 0x01U,
  HAL_TIM_CHANNEL_STATE_BUSY = 0x02U,
} HAL_TIM_ChannelStateTypeDef;




typedef enum
{
  HAL_DMA_BURST_STATE_RESET = 0x00U,
  HAL_DMA_BURST_STATE_READY = 0x01U,
  HAL_DMA_BURST_STATE_BUSY = 0x02U,
} HAL_TIM_DMABurstStateTypeDef;




typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1 = 0x01U,
  HAL_TIM_ACTIVE_CHANNEL_2 = 0x02U,
  HAL_TIM_ACTIVE_CHANNEL_3 = 0x04U,
  HAL_TIM_ACTIVE_CHANNEL_4 = 0x08U,
  HAL_TIM_ACTIVE_CHANNEL_CLEARED = 0x00U
} HAL_TIM_ActiveChannel;







typedef struct

{
  TIM_TypeDef *Instance;
  TIM_Base_InitTypeDef Init;
  HAL_TIM_ActiveChannel Channel;
  DMA_HandleTypeDef *hdma[7];

  HAL_LockTypeDef Lock;
  volatile HAL_TIM_StateTypeDef State;
  volatile HAL_TIM_ChannelStateTypeDef ChannelState[4];
  volatile HAL_TIM_ChannelStateTypeDef ChannelNState[4];
  volatile HAL_TIM_DMABurstStateTypeDef DMABurstState;
# 378 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
} TIM_HandleTypeDef;
# 1851 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
typedef struct
{
  uint32_t IC1Polarity;


  uint32_t IC1Prescaler;


  uint32_t IC1Filter;


  uint32_t Commutation_Delay;

} TIM_HallSensor_InitTypeDef;
# 209 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);


HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);
# 234 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);


HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);


HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 254 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);


HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 273 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);


HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
# 288 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t InputTrigger,
                                              uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t InputTrigger,
                                                 uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t InputTrigger,
                                                  uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
                                                        TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
                                                TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);
# 308 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
# 320 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(TIM_HandleTypeDef *htim, uint32_t ChannelN);
# 335 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);
# 1852 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h" 2
# 1863 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);
# 1885 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 1907 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 1929 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 1951 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim, uint32_t OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
# 1970 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef *htim, TIM_Encoder_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 1993 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);
# 2003 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_IC_InitTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
                                                 uint32_t OutputChannel, uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim, TIM_ClearInputConfigTypeDef *sClearInputConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, TIM_ClockConfigTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                              uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength,
                                                   uint32_t DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                             uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                  uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength,
                                                  uint32_t DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim, uint32_t EventSource);
uint32_t HAL_TIM_ReadCapturedValue(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2037 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);
# 2064 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(TIM_HandleTypeDef *htim);


HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(TIM_HandleTypeDef *htim);
# 2088 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_tim.h"
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_ETR_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ExtTRGPrescaler,
                       uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

void TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_DMAError(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_CCxChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelState);
# 329 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
typedef struct
{
  uint32_t BaudRate;





  uint32_t WordLength;


  uint32_t StopBits;


  uint32_t Parity;






  uint32_t Mode;


  uint32_t HwFlowCtl;


  uint32_t OverSampling;

} UART_InitTypeDef;
# 117 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
typedef enum
{
  HAL_UART_STATE_RESET = 0x00U,

  HAL_UART_STATE_READY = 0x20U,

  HAL_UART_STATE_BUSY = 0x24U,

  HAL_UART_STATE_BUSY_TX = 0x21U,

  HAL_UART_STATE_BUSY_RX = 0x22U,

  HAL_UART_STATE_BUSY_TX_RX = 0x23U,


  HAL_UART_STATE_TIMEOUT = 0xA0U,

  HAL_UART_STATE_ERROR = 0xE0U

} HAL_UART_StateTypeDef;
# 145 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
typedef uint32_t HAL_UART_RxTypeTypeDef;




typedef struct __UART_HandleTypeDef
{
  USART_TypeDef *Instance;

  UART_InitTypeDef Init;

  uint8_t *pTxBuffPtr;

  uint16_t TxXferSize;

  volatile uint16_t TxXferCount;

  uint8_t *pRxBuffPtr;

  uint16_t RxXferSize;

  volatile uint16_t RxXferCount;

  volatile HAL_UART_RxTypeTypeDef ReceptionType;

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_UART_StateTypeDef gState;



  volatile HAL_UART_StateTypeDef RxState;


  volatile uint32_t ErrorCode;
# 201 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
} UART_HandleTypeDef;
# 695 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart, uint32_t BreakDetectLength);
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethod);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);
# 721 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint16_t *RxLen, uint32_t Timeout);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);


HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart);

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
# 763 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart);
# 776 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart);
uint32_t HAL_UART_GetError(UART_HandleTypeDef *huart);
# 860 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_uart.h"
HAL_StatusTypeDef UART_Start_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef UART_Start_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
# 333 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_usart.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_usart.h"
typedef struct
{
  uint32_t BaudRate;




  uint32_t WordLength;


  uint32_t StopBits;


  uint32_t Parity;






  uint32_t Mode;


  uint32_t CLKPolarity;


  uint32_t CLKPhase;


  uint32_t CLKLastBit;


} USART_InitTypeDef;




typedef enum
{
  HAL_USART_STATE_RESET = 0x00U,
  HAL_USART_STATE_READY = 0x01U,
  HAL_USART_STATE_BUSY = 0x02U,
  HAL_USART_STATE_BUSY_TX = 0x12U,
  HAL_USART_STATE_BUSY_RX = 0x22U,
  HAL_USART_STATE_BUSY_TX_RX = 0x32U,
  HAL_USART_STATE_TIMEOUT = 0x03U,
  HAL_USART_STATE_ERROR = 0x04U
} HAL_USART_StateTypeDef;




typedef struct __USART_HandleTypeDef
{
  USART_TypeDef *Instance;

  USART_InitTypeDef Init;

  uint8_t *pTxBuffPtr;

  uint16_t TxXferSize;

  volatile uint16_t TxXferCount;

  uint8_t *pRxBuffPtr;

  uint16_t RxXferSize;

  volatile uint16_t RxXferCount;

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_USART_StateTypeDef State;

  volatile uint32_t ErrorCode;
# 140 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_usart.h"
} USART_HandleTypeDef;
# 492 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_usart.h"
HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart);
void HAL_USART_MspInit(USART_HandleTypeDef *husart);
void HAL_USART_MspDeInit(USART_HandleTypeDef *husart);
# 511 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_usart.h"
HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart);

HAL_StatusTypeDef HAL_USART_Abort(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_Abort_IT(USART_HandleTypeDef *husart);

void HAL_USART_IRQHandler(USART_HandleTypeDef *husart);
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart);
void HAL_USART_AbortCpltCallback(USART_HandleTypeDef *husart);
# 543 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_usart.h"
HAL_USART_StateTypeDef HAL_USART_GetState(USART_HandleTypeDef *husart);
uint32_t HAL_USART_GetError(USART_HandleTypeDef *husart);
# 337 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h" 1
# 46 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h"
typedef struct
{
  uint32_t BaudRate;




  uint32_t WordLength;


  uint32_t Parity;






  uint32_t Mode;


  uint8_t Prescaler;




  uint32_t IrDAMode;

} IRDA_InitTypeDef;
# 114 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h"
typedef enum
{
  HAL_IRDA_STATE_RESET = 0x00U,

  HAL_IRDA_STATE_READY = 0x20U,

  HAL_IRDA_STATE_BUSY = 0x24U,

  HAL_IRDA_STATE_BUSY_TX = 0x21U,

  HAL_IRDA_STATE_BUSY_RX = 0x22U,

  HAL_IRDA_STATE_BUSY_TX_RX = 0x23U,


  HAL_IRDA_STATE_TIMEOUT = 0xA0U,

  HAL_IRDA_STATE_ERROR = 0xE0U

} HAL_IRDA_StateTypeDef;







typedef struct

{
  USART_TypeDef *Instance;

  IRDA_InitTypeDef Init;

  uint8_t *pTxBuffPtr;

  uint16_t TxXferSize;

  volatile uint16_t TxXferCount;

  uint8_t *pRxBuffPtr;

  uint16_t RxXferSize;

  volatile uint16_t RxXferCount;

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_IRDA_StateTypeDef gState;



  volatile HAL_IRDA_StateTypeDef RxState;


  volatile uint32_t ErrorCode;
# 198 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h"
} IRDA_HandleTypeDef;
# 544 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h"
HAL_StatusTypeDef HAL_IRDA_Init(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_DeInit(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_MspInit(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_MspDeInit(IRDA_HandleTypeDef *hirda);
# 563 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h"
HAL_StatusTypeDef HAL_IRDA_Transmit(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_IRDA_Receive(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_IRDA_Transmit_IT(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_Receive_IT(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_Transmit_DMA(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_Receive_DMA(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_DMAPause(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_DMAResume(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_DMAStop(IRDA_HandleTypeDef *hirda);

HAL_StatusTypeDef HAL_IRDA_Abort(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortTransmit(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortReceive(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_Abort_IT(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortTransmit_IT(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortReceive_IT(IRDA_HandleTypeDef *hirda);

void HAL_IRDA_IRQHandler(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_TxCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_RxCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_TxHalfCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_RxHalfCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_ErrorCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_AbortCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_AbortTransmitCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_AbortReceiveCpltCallback(IRDA_HandleTypeDef *hirda);
# 597 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_irda.h"
HAL_IRDA_StateTypeDef HAL_IRDA_GetState(IRDA_HandleTypeDef *hirda);
uint32_t HAL_IRDA_GetError(IRDA_HandleTypeDef *hirda);
# 341 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h"
typedef struct
{
  uint32_t BaudRate;




  uint32_t WordLength;


  uint32_t StopBits;


  uint32_t Parity;






  uint32_t Mode;


  uint32_t CLKPolarity;


  uint32_t CLKPhase;


  uint32_t CLKLastBit;



  uint32_t Prescaler;




  uint32_t GuardTime;

  uint32_t NACKState;

}SMARTCARD_InitTypeDef;
# 130 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h"
typedef enum
{
  HAL_SMARTCARD_STATE_RESET = 0x00U,

  HAL_SMARTCARD_STATE_READY = 0x20U,

  HAL_SMARTCARD_STATE_BUSY = 0x24U,

  HAL_SMARTCARD_STATE_BUSY_TX = 0x21U,

  HAL_SMARTCARD_STATE_BUSY_RX = 0x22U,

  HAL_SMARTCARD_STATE_BUSY_TX_RX = 0x23U,


  HAL_SMARTCARD_STATE_TIMEOUT = 0xA0U,

  HAL_SMARTCARD_STATE_ERROR = 0xE0U

}HAL_SMARTCARD_StateTypeDef;




typedef struct __SMARTCARD_HandleTypeDef
{
  USART_TypeDef *Instance;

  SMARTCARD_InitTypeDef Init;

  uint8_t *pTxBuffPtr;

  uint16_t TxXferSize;

  volatile uint16_t TxXferCount;

  uint8_t *pRxBuffPtr;

  uint16_t RxXferSize;

  volatile uint16_t RxXferCount;

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_SMARTCARD_StateTypeDef gState;



  volatile HAL_SMARTCARD_StateTypeDef RxState;


  volatile uint32_t ErrorCode;
# 205 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h"
} SMARTCARD_HandleTypeDef;
# 628 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h"
HAL_StatusTypeDef HAL_SMARTCARD_Init(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_ReInit(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_DeInit(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsc);
# 646 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h"
HAL_StatusTypeDef HAL_SMARTCARD_Transmit(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SMARTCARD_Receive(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_DMA(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SMARTCARD_Receive_DMA(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_SMARTCARD_Abort(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortTransmit(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortReceive(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_Abort_IT(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortTransmit_IT(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortReceive_IT(SMARTCARD_HandleTypeDef *hsc);

void HAL_SMARTCARD_IRQHandler(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_AbortCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_AbortTransmitCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_AbortReceiveCpltCallback(SMARTCARD_HandleTypeDef *hsc);
# 675 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_smartcard.h"
HAL_SMARTCARD_StateTypeDef HAL_SMARTCARD_GetState(SMARTCARD_HandleTypeDef *hsc);
uint32_t HAL_SMARTCARD_GetError(SMARTCARD_HandleTypeDef *hsc);
# 345 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_wwdg.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_wwdg.h"
typedef struct
{
  uint32_t Prescaler;


  uint32_t Window;


  uint32_t Counter;


  uint32_t EWIMode ;


} WWDG_InitTypeDef;







typedef struct

{
  WWDG_TypeDef *Instance;

  WWDG_InitTypeDef Init;






} WWDG_HandleTypeDef;
# 259 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_wwdg.h"
HAL_StatusTypeDef HAL_WWDG_Init(WWDG_HandleTypeDef *hwwdg);
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg);
# 276 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_wwdg.h"
HAL_StatusTypeDef HAL_WWDG_Refresh(WWDG_HandleTypeDef *hwwdg);
void HAL_WWDG_IRQHandler(WWDG_HandleTypeDef *hwwdg);
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg);
# 349 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h" 1
# 29 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_usb.h" 1
# 47 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_usb.h"
typedef enum
{
  USB_DEVICE_MODE = 0,
  USB_HOST_MODE = 1,
  USB_DRD_MODE = 2
} USB_OTG_ModeTypeDef;




typedef enum
{
  URB_IDLE = 0,
  URB_DONE,
  URB_NOTREADY,
  URB_NYET,
  URB_ERROR,
  URB_STALL
} USB_OTG_URBStateTypeDef;




typedef enum
{
  HC_IDLE = 0,
  HC_XFRC,
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,
  HC_BBLERR,
  HC_DATATGLERR
} USB_OTG_HCStateTypeDef;




typedef struct
{
  uint32_t dev_endpoints;



  uint32_t Host_channels;



  uint32_t speed;


  uint32_t dma_enable;

  uint32_t ep0_mps;

  uint32_t phy_itface;


  uint32_t Sof_enable;

  uint32_t low_power_enable;

  uint32_t lpm_enable;

  uint32_t battery_charging_enable;

  uint32_t vbus_sensing_enable;

  uint32_t use_dedicated_ep1;

  uint32_t use_external_vbus;

} USB_OTG_CfgTypeDef;

typedef struct
{
  uint8_t num;


  uint8_t is_in;


  uint8_t is_stall;


  uint8_t type;


  uint8_t data_pid_start;


  uint8_t even_odd_frame;


  uint16_t tx_fifo_num;


  uint32_t maxpacket;


  uint8_t *xfer_buff;

  uint32_t dma_addr;

  uint32_t xfer_len;

  uint32_t xfer_count;
} USB_OTG_EPTypeDef;

typedef struct
{
  uint8_t dev_addr;


  uint8_t ch_num;


  uint8_t ep_num;


  uint8_t ep_is_in;


  uint8_t speed;


  uint8_t do_ping;

  uint8_t process_ping;

  uint8_t ep_type;


  uint16_t max_packet;


  uint8_t data_pid;


  uint8_t *xfer_buff;

  uint32_t XferSize;

  uint32_t xfer_len;

  uint32_t xfer_count;

  uint8_t toggle_in;


  uint8_t toggle_out;


  uint32_t dma_addr;

  uint32_t ErrCnt;

  USB_OTG_URBStateTypeDef urb_state;


  USB_OTG_HCStateTypeDef state;

} USB_OTG_HCTypeDef;
# 433 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_usb.h"
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_DevInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DisableGlobalInt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_SetTurnaroundTime(USB_OTG_GlobalTypeDef *USBx, uint32_t hclk, uint8_t speed);
HAL_StatusTypeDef USB_SetCurrentMode(USB_OTG_GlobalTypeDef *USBx, USB_OTG_ModeTypeDef mode);
HAL_StatusTypeDef USB_SetDevSpeed(USB_OTG_GlobalTypeDef *USBx, uint8_t speed);
HAL_StatusTypeDef USB_FlushRxFifo(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_FlushTxFifo(USB_OTG_GlobalTypeDef *USBx, uint32_t num);
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_ActivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma);
HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma);
HAL_StatusTypeDef USB_WritePacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *src,
                                  uint8_t ch_ep_num, uint16_t len, uint8_t dma);

void *USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *dest, uint16_t len);
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_SetDevAddress(USB_OTG_GlobalTypeDef *USBx, uint8_t address);
HAL_StatusTypeDef USB_DevConnect(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DevDisconnect(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_ActivateSetup(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint8_t dma, uint8_t *psetup);
uint8_t USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_GetMode(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadInterrupts(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadDevAllOutEpInterrupt(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadDevOutEPInterrupt(USB_OTG_GlobalTypeDef *USBx, uint8_t epnum);
uint32_t USB_ReadDevAllInEpInterrupt(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadDevInEPInterrupt(USB_OTG_GlobalTypeDef *USBx, uint8_t epnum);
void USB_ClearInterrupts(USB_OTG_GlobalTypeDef *USBx, uint32_t interrupt);

HAL_StatusTypeDef USB_HostInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_InitFSLSPClkSel(USB_OTG_GlobalTypeDef *USBx, uint8_t freq);
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DriveVbus(USB_OTG_GlobalTypeDef *USBx, uint8_t state);
uint32_t USB_GetHostSpeed(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_GetCurrentFrame(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_HC_Init(USB_OTG_GlobalTypeDef *USBx, uint8_t ch_num,
                              uint8_t epnum, uint8_t dev_address, uint8_t speed,
                              uint8_t ep_type, uint16_t mps);
HAL_StatusTypeDef USB_HC_StartXfer(USB_OTG_GlobalTypeDef *USBx,
                                   USB_OTG_HCTypeDef *hc, uint8_t dma);

uint32_t USB_HC_ReadInterrupt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_HC_Halt(USB_OTG_GlobalTypeDef *USBx, uint8_t hc_num);
HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint8_t ch_num);
HAL_StatusTypeDef USB_StopHost(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_ActivateRemoteWakeup(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DeActivateRemoteWakeup(USB_OTG_GlobalTypeDef *USBx);
# 30 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h" 2
# 49 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
typedef enum
{
  HAL_PCD_STATE_RESET = 0x00,
  HAL_PCD_STATE_READY = 0x01,
  HAL_PCD_STATE_ERROR = 0x02,
  HAL_PCD_STATE_BUSY = 0x03,
  HAL_PCD_STATE_TIMEOUT = 0x04
} PCD_StateTypeDef;


typedef enum
{
  LPM_L0 = 0x00,
  LPM_L1 = 0x01,
  LPM_L2 = 0x02,
  LPM_L3 = 0x03,
} PCD_LPM_StateTypeDef;

typedef enum
{
  PCD_LPM_L0_ACTIVE = 0x00,
  PCD_LPM_L1_ACTIVE = 0x01,
} PCD_LPM_MsgTypeDef;

typedef enum
{
  PCD_BCD_ERROR = 0xFF,
  PCD_BCD_CONTACT_DETECTION = 0xFE,
  PCD_BCD_STD_DOWNSTREAM_PORT = 0xFD,
  PCD_BCD_CHARGING_DOWNSTREAM_PORT = 0xFC,
  PCD_BCD_DEDICATED_CHARGING_PORT = 0xFB,
  PCD_BCD_DISCOVERY_COMPLETED = 0x00,

} PCD_BCD_MsgTypeDef;


typedef USB_OTG_GlobalTypeDef PCD_TypeDef;
typedef USB_OTG_CfgTypeDef PCD_InitTypeDef;
typedef USB_OTG_EPTypeDef PCD_EPTypeDef;
# 96 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
typedef struct

{
  PCD_TypeDef *Instance;
  PCD_InitTypeDef Init;
  volatile uint8_t USB_Address;
  PCD_EPTypeDef IN_ep[16];
  PCD_EPTypeDef OUT_ep[16];
  HAL_LockTypeDef Lock;
  volatile PCD_StateTypeDef State;
  volatile uint32_t ErrorCode;
  uint32_t Setup[12];
  PCD_LPM_StateTypeDef LPM_State;
  uint32_t BESL;


  uint32_t lpm_active;


  uint32_t battery_charging_active;

  void *pData;
# 138 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
} PCD_HandleTypeDef;






# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd_ex.h" 1
# 51 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd_ex.h"
HAL_StatusTypeDef HAL_PCDEx_SetTxFiFo(PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size);
HAL_StatusTypeDef HAL_PCDEx_SetRxFiFo(PCD_HandleTypeDef *hpcd, uint16_t size);
# 64 "../Libraries/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd_ex.h"
void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg);
void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg);
# 146 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h" 2
# 244 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);
# 335 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
void HAL_PCD_WKUP_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
# 360 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type);

HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                     uint8_t *pBuf, uint32_t len);

HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                      uint8_t *pBuf, uint32_t len);


HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);

uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
# 389 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_pcd.h"
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd);
# 353 "../User\\stm32f4xx_hal_conf.h" 2



# 1 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h" 1
# 48 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
typedef enum
{
  HAL_HCD_STATE_RESET = 0x00,
  HAL_HCD_STATE_READY = 0x01,
  HAL_HCD_STATE_ERROR = 0x02,
  HAL_HCD_STATE_BUSY = 0x03,
  HAL_HCD_STATE_TIMEOUT = 0x04
} HCD_StateTypeDef;

typedef USB_OTG_GlobalTypeDef HCD_TypeDef;
typedef USB_OTG_CfgTypeDef HCD_InitTypeDef;
typedef USB_OTG_HCTypeDef HCD_HCTypeDef;
typedef USB_OTG_URBStateTypeDef HCD_URBStateTypeDef;
typedef USB_OTG_HCStateTypeDef HCD_HCStateTypeDef;
# 72 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
typedef struct

{
  HCD_TypeDef *Instance;
  HCD_InitTypeDef Init;
  HCD_HCTypeDef hc[16];
  HAL_LockTypeDef Lock;
  volatile HCD_StateTypeDef State;
  volatile uint32_t ErrorCode;
  void *pData;
# 94 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
} HCD_HandleTypeDef;
# 177 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd, uint8_t ch_num,
                                  uint8_t epnum, uint8_t dev_address,
                                  uint8_t speed, uint8_t ep_type, uint16_t mps);

HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num);
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd);
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd);
# 241 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd, uint8_t ch_num,
                                           uint8_t direction, uint8_t ep_type,
                                           uint8_t token, uint8_t *pbuff,
                                           uint16_t length, uint8_t do_ping);


void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd);
void HAL_HCD_WKUP_IRQHandler(HCD_HandleTypeDef *hhcd);
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum,
                                         HCD_URBStateTypeDef urb_state);
# 264 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *hhcd);
# 275 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal_hcd.h"
HCD_StateTypeDef HAL_HCD_GetState(HCD_HandleTypeDef *hhcd);
HCD_URBStateTypeDef HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
HCD_HCStateTypeDef HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd);
uint32_t HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd);
# 357 "../User\\stm32f4xx_hal_conf.h" 2
# 31 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h" 2
# 50 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h"
typedef enum
{
  HAL_TICK_FREQ_10HZ = 100U,
  HAL_TICK_FREQ_100HZ = 10U,
  HAL_TICK_FREQ_1KHZ = 1U,
  HAL_TICK_FREQ_DEFAULT = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;
# 205 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h"
extern volatile uint32_t uwTick;
extern uint32_t uwTickPrio;
extern HAL_TickFreqTypeDef uwTickFreq;
# 220 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h"
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority);
# 233 "../Libraries/STM32F4xx_HAL_Driver/Inc\\stm32f4xx_hal.h"
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_DBGMCU_EnableDBGStandbyMode(void);
void HAL_DBGMCU_DisableDBGStandbyMode(void);
void HAL_EnableCompensationCell(void);
void HAL_DisableCompensationCell(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);
# 235 "../Libraries/CMSIS/Device/ST/STM32F4xx/Include\\stm32f4xx.h" 2
# 5 "../User/SYSTEM/sys\\sys.h" 2
# 1 "../Libraries/CMSIS/Include\\core_cm4.h" 1
# 29 "../Libraries/CMSIS/Include\\core_cm4.h" 3
# 6 "../User/SYSTEM/sys\\sys.h" 2





void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);
void sys_standby(void);
void sys_soft_reset(void);
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);


void sys_wfi_set(void);
void sys_intx_disable(void);
void sys_intx_enable(void);
void sys_msr_msp(uint32_t addr);
# 5 "../User/BSP/key/key.h" 2
# 1 "../User/SYSTEM/delay\\delay.h" 1
# 32 "../User/SYSTEM/delay\\delay.h"
void delay_init(uint16_t sysclk);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);


    void HAL_Delay(uint32_t Delay);
# 6 "../User/BSP/key/key.h" 2
# 29 "../User/BSP/key/key.h"
void bsp_key_init(void);
uint8_t key_scan(uint8_t mode);
# 2 "../User/BSP/key/key.c" 2

void bsp_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    do { volatile uint32_t tmpreg = 0x00U; ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) |= ((0x1UL << (4U)))); tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) & ((0x1UL << (4U)))); (void)tmpreg; } while(0U);
    do { volatile uint32_t tmpreg = 0x00U; ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) |= ((0x1UL << (0U)))); tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) & ((0x1UL << (0U)))); (void)tmpreg; } while(0U);

    GPIO_InitStructure.Pin = ((uint16_t)0x0010);
    GPIO_InitStructure.Mode = 0x00000000U;
    GPIO_InitStructure.Pull = 0x00000001U;
    GPIO_InitStructure.Speed = 0x00000002U;
    HAL_GPIO_Init(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), &GPIO_InitStructure);

    GPIO_InitStructure.Pin = ((uint16_t)0x0008);
    GPIO_InitStructure.Mode = 0x00000000U;
    GPIO_InitStructure.Pull = 0x00000001U;
    GPIO_InitStructure.Speed = 0x00000002U;
    HAL_GPIO_Init(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), &GPIO_InitStructure);

    GPIO_InitStructure.Pin = ((uint16_t)0x0004);
    GPIO_InitStructure.Mode = 0x00000000U;
    GPIO_InitStructure.Pull = 0x00000001U;
    GPIO_InitStructure.Speed = 0x00000002U;
    HAL_GPIO_Init(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), &GPIO_InitStructure);

    GPIO_InitStructure.Pin = ((uint16_t)0x0001);
    GPIO_InitStructure.Mode = 0x00000000U;
    GPIO_InitStructure.Pull = 0x00000002U;
    GPIO_InitStructure.Speed = 0x00000002U;
    HAL_GPIO_Init(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x0000UL)), &GPIO_InitStructure);
}

uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;
    uint8_t key_val = 0;
    if(mode)
        key_up = 1;

    if(key_up && (HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0010)) == 0 || HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0008)) == 0 || HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0004)) == 0 || HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x0000UL)), ((uint16_t)0x0001)) == 1))
    {
        delay_ms(10);
        key_up = 0;
        if(HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0010)) == 0)
            return 1;
        if(HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0008)) == 0)
            return 2;
        if(HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0004)) == 0)
            return 3;
        if(HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x0000UL)), ((uint16_t)0x0001)) == 1)
            return 4;
    }
    else if(HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0010)) == 1 && HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0008)) == 1 && HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x1000UL)), ((uint16_t)0x0004)) == 1 && HAL_GPIO_ReadPin(((GPIO_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x0000UL)), ((uint16_t)0x0001)) == 0)
    {
        key_up = 1;
        return 0;
    }
    return 0;
}
