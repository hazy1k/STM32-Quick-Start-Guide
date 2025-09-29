#line 1 "..\\..\\Drivers\\SYSTEM\\sys\\sys.c"

























 


#line 1 "..\\..\\Drivers\\./SYSTEM/sys/sys.h"

























 





#line 1 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h7xx.h"



























 



 



 










 



 







 

#line 78 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h7xx.h"



 










 
   




 
#line 107 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h7xx.h"



 



 

#line 1 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"























 



 



 










 




 
typedef enum
{
 
  NonMaskableInt_IRQn         = -14,     
  HardFault_IRQn              = -13,     
  MemoryManagement_IRQn       = -12,     
  BusFault_IRQn               = -11,     
  UsageFault_IRQn             = -10,     
  SVCall_IRQn                 = -5,      
  DebugMonitor_IRQn           = -4,      
  PendSV_IRQn                 = -2,      
  SysTick_IRQn                = -1,      
 
  WWDG_IRQn                   = 0,       
  PVD_AVD_IRQn                = 1,       
  TAMP_STAMP_IRQn             = 2,       
  RTC_WKUP_IRQn               = 3,       
  FLASH_IRQn                  = 4,       
  RCC_IRQn                    = 5,       
  EXTI0_IRQn                  = 6,       
  EXTI1_IRQn                  = 7,       
  EXTI2_IRQn                  = 8,       
  EXTI3_IRQn                  = 9,       
  EXTI4_IRQn                  = 10,      
  DMA1_Stream0_IRQn           = 11,      
  DMA1_Stream1_IRQn           = 12,      
  DMA1_Stream2_IRQn           = 13,      
  DMA1_Stream3_IRQn           = 14,      
  DMA1_Stream4_IRQn           = 15,      
  DMA1_Stream5_IRQn           = 16,      
  DMA1_Stream6_IRQn           = 17,      
  ADC_IRQn                    = 18,      
  FDCAN1_IT0_IRQn             = 19,      
  FDCAN2_IT0_IRQn             = 20,      
  FDCAN1_IT1_IRQn             = 21,      
  FDCAN2_IT1_IRQn             = 22,      
  EXTI9_5_IRQn                = 23,      
  TIM1_BRK_IRQn               = 24,      
  TIM1_UP_IRQn                = 25,      
  TIM1_TRG_COM_IRQn           = 26,      
  TIM1_CC_IRQn                = 27,      
  TIM2_IRQn                   = 28,      
  TIM3_IRQn                   = 29,      
  TIM4_IRQn                   = 30,      
  I2C1_EV_IRQn                = 31,      
  I2C1_ER_IRQn                = 32,      
  I2C2_EV_IRQn                = 33,      
  I2C2_ER_IRQn                = 34,      
  SPI1_IRQn                   = 35,      
  SPI2_IRQn                   = 36,      
  USART1_IRQn                 = 37,      
  USART2_IRQn                 = 38,      
  USART3_IRQn                 = 39,      
  EXTI15_10_IRQn              = 40,      
  RTC_Alarm_IRQn              = 41,      
  TIM8_BRK_TIM12_IRQn         = 43,      
  TIM8_UP_TIM13_IRQn          = 44,      
  TIM8_TRG_COM_TIM14_IRQn     = 45,      
  TIM8_CC_IRQn                = 46,      
  DMA1_Stream7_IRQn           = 47,      
  FMC_IRQn                    = 48,      
  SDMMC1_IRQn                 = 49,      
  TIM5_IRQn                   = 50,      
  SPI3_IRQn                   = 51,      
  UART4_IRQn                  = 52,      
  UART5_IRQn                  = 53,      
  TIM6_DAC_IRQn               = 54,      
  TIM7_IRQn                   = 55,      
  DMA2_Stream0_IRQn           = 56,      
  DMA2_Stream1_IRQn           = 57,      
  DMA2_Stream2_IRQn           = 58,      
  DMA2_Stream3_IRQn           = 59,      
  DMA2_Stream4_IRQn           = 60,      
  ETH_IRQn                    = 61,      
  ETH_WKUP_IRQn               = 62,      
  FDCAN_CAL_IRQn              = 63,      
  DMA2_Stream5_IRQn           = 68,      
  DMA2_Stream6_IRQn           = 69,      
  DMA2_Stream7_IRQn           = 70,      
  USART6_IRQn                 = 71,      
  I2C3_EV_IRQn                = 72,      
  I2C3_ER_IRQn                = 73,      
  OTG_HS_EP1_OUT_IRQn         = 74,      
  OTG_HS_EP1_IN_IRQn          = 75,      
  OTG_HS_WKUP_IRQn            = 76,      
  OTG_HS_IRQn                 = 77,      
  DCMI_IRQn                   = 78,      
  CRYP_IRQn                   = 79,      
  HASH_RNG_IRQn               = 80,      
  FPU_IRQn                    = 81,      
  UART7_IRQn                  = 82,      
  UART8_IRQn                  = 83,      
  SPI4_IRQn                   = 84,      
  SPI5_IRQn                   = 85,      
  SPI6_IRQn                   = 86,      
  SAI1_IRQn                   = 87,      
  LTDC_IRQn                   = 88,      
  LTDC_ER_IRQn                = 89,      
  DMA2D_IRQn                  = 90,      
  SAI2_IRQn                   = 91,      
  QUADSPI_IRQn                = 92,      
  LPTIM1_IRQn                 = 93,      
  CEC_IRQn                    = 94,      
  I2C4_EV_IRQn                = 95,      
  I2C4_ER_IRQn                = 96,      
  SPDIF_RX_IRQn               = 97,      
  OTG_FS_EP1_OUT_IRQn         = 98,      
  OTG_FS_EP1_IN_IRQn          = 99,      
  OTG_FS_WKUP_IRQn            = 100,     
  OTG_FS_IRQn                 = 101,     
  DMAMUX1_OVR_IRQn            = 102,     
  HRTIM1_Master_IRQn          = 103,     
  HRTIM1_TIMA_IRQn            = 104,     
  HRTIM1_TIMB_IRQn            = 105,     
  HRTIM1_TIMC_IRQn            = 106,     
  HRTIM1_TIMD_IRQn            = 107,     
  HRTIM1_TIME_IRQn            = 108,     
  HRTIM1_FLT_IRQn             = 109,     
  DFSDM1_FLT0_IRQn            = 110,     
  DFSDM1_FLT1_IRQn            = 111,     
  DFSDM1_FLT2_IRQn            = 112,     
  DFSDM1_FLT3_IRQn            = 113,     
  SAI3_IRQn                   = 114,     
  SWPMI1_IRQn                 = 115,     
  TIM15_IRQn                  = 116,     
  TIM16_IRQn                  = 117,     
  TIM17_IRQn                  = 118,     
  MDIOS_WKUP_IRQn             = 119,     
  MDIOS_IRQn                  = 120,     
  JPEG_IRQn                   = 121,     
  MDMA_IRQn                   = 122,     
  SDMMC2_IRQn                 = 124,     
  HSEM1_IRQn                  = 125,     
  ADC3_IRQn                   = 127,     
  DMAMUX2_OVR_IRQn            = 128,     
  BDMA_Channel0_IRQn          = 129,     
  BDMA_Channel1_IRQn          = 130,     
  BDMA_Channel2_IRQn          = 131,     
  BDMA_Channel3_IRQn          = 132,     
  BDMA_Channel4_IRQn          = 133,     
  BDMA_Channel5_IRQn          = 134,     
  BDMA_Channel6_IRQn          = 135,     
  BDMA_Channel7_IRQn          = 136,     
  COMP_IRQn                   = 137 ,    
  LPTIM2_IRQn                 = 138,     
  LPTIM3_IRQn                 = 139,     
  LPTIM4_IRQn                 = 140,     
  LPTIM5_IRQn                 = 141,     
  LPUART1_IRQn                = 142,     
  CRS_IRQn                    = 144,     
  ECC_IRQn                    = 145,     
  SAI4_IRQn                   = 146,     
  WAKEUP_PIN_IRQn             = 149,     
} IRQn_Type;



 



 






 
#line 1 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"
 




 
















 










#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"
 
 





 









     
#line 27 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"
     











#line 46 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"





 

     

     
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

     
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;

     

     
     
typedef   signed          char int_least8_t;
typedef   signed short     int int_least16_t;
typedef   signed           int int_least32_t;
typedef   signed       __int64 int_least64_t;

     
typedef unsigned          char uint_least8_t;
typedef unsigned short     int uint_least16_t;
typedef unsigned           int uint_least32_t;
typedef unsigned       __int64 uint_least64_t;

     

     
typedef   signed           int int_fast8_t;
typedef   signed           int int_fast16_t;
typedef   signed           int int_fast32_t;
typedef   signed       __int64 int_fast64_t;

     
typedef unsigned           int uint_fast8_t;
typedef unsigned           int uint_fast16_t;
typedef unsigned           int uint_fast32_t;
typedef unsigned       __int64 uint_fast64_t;

     




typedef   signed           int intptr_t;
typedef unsigned           int uintptr_t;


     
typedef   signed     long long intmax_t;
typedef unsigned     long long uintmax_t;




     

     





     





     





     

     





     





     





     

     





     





     





     

     






     






     






     

     


     


     


     

     
#line 216 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"

     



     






     
    
 



#line 241 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"

     







     










     











#line 305 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"






 
#line 35 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

















 




 



 

#line 1 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_version.h"
 




 
















 










 
#line 64 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

 









 
#line 87 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

#line 161 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

#line 1 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_compiler.h"
 




 
















 




#line 29 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_compiler.h"



 
#line 1 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_armcc.h"
 




 
















 









 













   
   


 
#line 103 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_armcc.h"

 



 





 
 






 
 





 
static __inline uint32_t __get_CONTROL(void)
{
  register uint32_t __regControl         __asm("control");
  return(__regControl);
}






 
static __inline void __set_CONTROL(uint32_t control)
{
  register uint32_t __regControl         __asm("control");
  __regControl = control;
}






 
static __inline uint32_t __get_IPSR(void)
{
  register uint32_t __regIPSR          __asm("ipsr");
  return(__regIPSR);
}






 
static __inline uint32_t __get_APSR(void)
{
  register uint32_t __regAPSR          __asm("apsr");
  return(__regAPSR);
}






 
static __inline uint32_t __get_xPSR(void)
{
  register uint32_t __regXPSR          __asm("xpsr");
  return(__regXPSR);
}






 
static __inline uint32_t __get_PSP(void)
{
  register uint32_t __regProcessStackPointer  __asm("psp");
  return(__regProcessStackPointer);
}






 
static __inline void __set_PSP(uint32_t topOfProcStack)
{
  register uint32_t __regProcessStackPointer  __asm("psp");
  __regProcessStackPointer = topOfProcStack;
}






 
static __inline uint32_t __get_MSP(void)
{
  register uint32_t __regMainStackPointer     __asm("msp");
  return(__regMainStackPointer);
}






 
static __inline void __set_MSP(uint32_t topOfMainStack)
{
  register uint32_t __regMainStackPointer     __asm("msp");
  __regMainStackPointer = topOfMainStack;
}






 
static __inline uint32_t __get_PRIMASK(void)
{
  register uint32_t __regPriMask         __asm("primask");
  return(__regPriMask);
}






 
static __inline void __set_PRIMASK(uint32_t priMask)
{
  register uint32_t __regPriMask         __asm("primask");
  __regPriMask = (priMask);
}









 







 







 
static __inline uint32_t  __get_BASEPRI(void)
{
  register uint32_t __regBasePri         __asm("basepri");
  return(__regBasePri);
}






 
static __inline void __set_BASEPRI(uint32_t basePri)
{
  register uint32_t __regBasePri         __asm("basepri");
  __regBasePri = (basePri & 0xFFU);
}







 
static __inline void __set_BASEPRI_MAX(uint32_t basePri)
{
  register uint32_t __regBasePriMax      __asm("basepri_max");
  __regBasePriMax = (basePri & 0xFFU);
}






 
static __inline uint32_t __get_FAULTMASK(void)
{
  register uint32_t __regFaultMask       __asm("faultmask");
  return(__regFaultMask);
}






 
static __inline void __set_FAULTMASK(uint32_t faultMask)
{
  register uint32_t __regFaultMask       __asm("faultmask");
  __regFaultMask = (faultMask & (uint32_t)1U);
}









 
static __inline uint32_t __get_FPSCR(void)
{


  register uint32_t __regfpscr         __asm("fpscr");
  return(__regfpscr);



}






 
static __inline void __set_FPSCR(uint32_t fpscr)
{


  register uint32_t __regfpscr         __asm("fpscr");
  __regfpscr = (fpscr);



}


 


 



 




 






 







 






 








 










 










 






                  





 








 

__attribute__((section(".rev16_text"))) static __inline __asm uint32_t __REV16(uint32_t value)
{
  rev16 r0, r0
  bx lr
}








 

__attribute__((section(".revsh_text"))) static __inline __asm int16_t __REVSH(int16_t value)
{
  revsh r0, r0
  bx lr
}









 









 








 
#line 532 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_armcc.h"







 











 












 












 














 














 














 










 









 









 









 

__attribute__((section(".rrx_text"))) static __inline __asm uint32_t __RRX(uint32_t value)
{
  rrx r0, r0
  bx lr
}








 








 








 








 








 








 


#line 780 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_armcc.h"

   


 



 



#line 851 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_armcc.h"











 


#line 35 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_compiler.h"




 
#line 263 "..\\..\\Drivers\\CMSIS\\Include\\cmsis_compiler.h"




#line 163 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

















 
#line 222 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

 






 
#line 238 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

 




 













 



 






 



 
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

 





















 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:23;               
  } b;                                    
  uint32_t w;                             
} IPSR_Type;

 






 
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
  volatile uint8_t  IP[240U];                
        uint32_t RESERVED5[644U];
  volatile  uint32_t STIR;                    
}  NVIC_Type;

 



 







 



 
typedef struct
{
  volatile const  uint32_t CPUID;                   
  volatile uint32_t ICSR;                    
  volatile uint32_t VTOR;                    
  volatile uint32_t AIRCR;                   
  volatile uint32_t SCR;                     
  volatile uint32_t CCR;                     
  volatile uint8_t  SHPR[12U];               
  volatile uint32_t SHCSR;                   
  volatile uint32_t CFSR;                    
  volatile uint32_t HFSR;                    
  volatile uint32_t DFSR;                    
  volatile uint32_t MMFAR;                   
  volatile uint32_t BFAR;                    
  volatile uint32_t AFSR;                    
  volatile const  uint32_t ID_PFR[2U];              
  volatile const  uint32_t ID_DFR;                  
  volatile const  uint32_t ID_AFR;                  
  volatile const  uint32_t ID_MFR[4U];              
  volatile const  uint32_t ID_ISAR[5U];             
        uint32_t RESERVED0[1U];
  volatile const  uint32_t CLIDR;                   
  volatile const  uint32_t CTR;                     
  volatile const  uint32_t CCSIDR;                  
  volatile uint32_t CSSELR;                  
  volatile uint32_t CPACR;                   
        uint32_t RESERVED3[93U];
  volatile  uint32_t STIR;                    
        uint32_t RESERVED4[15U];
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
        uint32_t RESERVED5[1U];
  volatile  uint32_t ICIALLU;                 
        uint32_t RESERVED6[1U];
  volatile  uint32_t ICIMVAU;                 
  volatile  uint32_t DCIMVAC;                 
  volatile  uint32_t DCISW;                   
  volatile  uint32_t DCCMVAU;                 
  volatile  uint32_t DCCMVAC;                 
  volatile  uint32_t DCCSW;                   
  volatile  uint32_t DCCIMVAC;                
  volatile  uint32_t DCCISW;                  
        uint32_t RESERVED7[6U];
  volatile uint32_t ITCMCR;                  
  volatile uint32_t DTCMCR;                  
  volatile uint32_t AHBPCR;                  
  volatile uint32_t CACR;                    
  volatile uint32_t AHBSCR;                  
        uint32_t RESERVED8[1U];
  volatile uint32_t ABFSR;                   
} SCB_Type;

 















 






























 



 





















 









 



























 










































 









 


















 





















 


















 









 















 






 















 





















 






 



 






 






 






 












 












 






 









 









 


















 







 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const  uint32_t ICTR;                    
  volatile uint32_t ACTLR;                   
} SCnSCB_Type;

 



 















 







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t LOAD;                    
  volatile uint32_t VAL;                     
  volatile const  uint32_t CALIB;                   
} SysTick_Type;

 












 



 



 









 







 



 
typedef struct
{
  volatile  union
  {
    volatile  uint8_t    u8;                  
    volatile  uint16_t   u16;                 
    volatile  uint32_t   u32;                 
  }  PORT [32U];                          
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;                     
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;                     
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;                     
        uint32_t RESERVED3[29U];
  volatile  uint32_t IWR;                     
  volatile const  uint32_t IRR;                     
  volatile uint32_t IMCR;                    
        uint32_t RESERVED4[43U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
        uint32_t RESERVED5[6U];
  volatile const  uint32_t PID4;                    
  volatile const  uint32_t PID5;                    
  volatile const  uint32_t PID6;                    
  volatile const  uint32_t PID7;                    
  volatile const  uint32_t PID0;                    
  volatile const  uint32_t PID1;                    
  volatile const  uint32_t PID2;                    
  volatile const  uint32_t PID3;                    
  volatile const  uint32_t CID0;                    
  volatile const  uint32_t CID1;                    
  volatile const  uint32_t CID2;                    
  volatile const  uint32_t CID3;                    
} ITM_Type;

 



 



























 



 



 



 









   







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t CYCCNT;                  
  volatile uint32_t CPICNT;                  
  volatile uint32_t EXCCNT;                  
  volatile uint32_t SLEEPCNT;                
  volatile uint32_t LSUCNT;                  
  volatile uint32_t FOLDCNT;                 
  volatile const  uint32_t PCSR;                    
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
        uint32_t RESERVED3[981U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
} DWT_Type;

 






















































 



 



 



 



 



 



 



























   







 



 
typedef struct
{
  volatile const  uint32_t SSPSR;                   
  volatile uint32_t CSPSR;                   
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;                    
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;                    
        uint32_t RESERVED2[131U];
  volatile const  uint32_t FFSR;                    
  volatile uint32_t FFCR;                    
  volatile const  uint32_t FSCR;                    
        uint32_t RESERVED3[759U];
  volatile const  uint32_t TRIGGER;                 
  volatile const  uint32_t FIFO0;                   
  volatile const  uint32_t ITATBCTR2;               
        uint32_t RESERVED4[1U];
  volatile const  uint32_t ITATBCTR0;               
  volatile const  uint32_t FIFO1;                   
  volatile uint32_t ITCTRL;                  
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;                
  volatile uint32_t CLAIMCLR;                
        uint32_t RESERVED7[8U];
  volatile const  uint32_t DEVID;                   
  volatile const  uint32_t DEVTYPE;                 
} TPI_Type;

 



 



 












 






 



 





















 






 





















 






 



 


















 






   








 



 
typedef struct
{
  volatile const  uint32_t TYPE;                    
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



 









 









 



 









 






























 








 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile uint32_t FPCCR;                   
  volatile uint32_t FPCAR;                   
  volatile uint32_t FPDSCR;                  
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
} FPU_Type;

 



























 



 












 
























 












 

 







 



 
typedef struct
{
  volatile uint32_t DHCSR;                   
  volatile  uint32_t DCRSR;                   
  volatile uint32_t DCRDR;                   
  volatile uint32_t DEMCR;                   
} CoreDebug_Type;

 




































 






 







































 







 






 







 


 







 

 
#line 1770 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

#line 1779 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"









 










 


 



 





 

#line 1833 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"

#line 1843 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"




 
#line 1854 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"










 
static __inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);              

  reg_value  =  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;                                                    
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));  
  reg_value  =  (reg_value                                   |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U)  );               
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR =  reg_value;
}






 
static __inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}







 
static __inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}









 
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







 
static __inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);
  }
}









 
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







 
static __inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}







 
static __inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}









 
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










 
static __inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]                = (uint8_t)((priority << (8U - 4)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 4)) & (uint32_t)0xFFUL);
  }
}










 
static __inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]                >> (8U - 4)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 4)));
  }
}












 
static __inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority     & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL)))
         );
}












 
static __inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority     = (Priority                   ) & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL);
}










 
static __inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  vectors[(int32_t)IRQn + 16] = vector;
}









 
static __inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return vectors[(int32_t)IRQn + 16];
}





 
__declspec(noreturn) static __inline void __NVIC_SystemReset(void)
{
  do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);                                                          
 
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR  = (uint32_t)((0x5FAUL << 16U)    |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U)    );          
  do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);                                                           

  for(;;)                                                            
  {
    __nop();
  }
}

 

 



#line 1 "..\\..\\Drivers\\CMSIS\\Include\\mpu_armv7.h"





 
















 
 





 



#line 62 "..\\..\\Drivers\\CMSIS\\Include\\mpu_armv7.h"

#line 69 "..\\..\\Drivers\\CMSIS\\Include\\mpu_armv7.h"





 












   














 




  











                          









  










  












  




 




 




 




 





 
typedef struct {
  uint32_t RBAR; 
  uint32_t RASR; 
} ARM_MPU_Region_t;
    


 
static __inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
  do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);

}


 
static __inline void ARM_MPU_Disable(void)
{
  do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
  do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);

  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL  &= ~(1UL );
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

#line 2169 "..\\..\\Drivers\\CMSIS\\Include\\core_cm7.h"



 





 








 
static __inline uint32_t SCB_GetFPUType(void)
{
  uint32_t mvfr0;

  mvfr0 = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->MVFR0;
  if      ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x220U)
  {
    return 2U;            
  }
  else if ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x020U)
  {
    return 1U;            
  }
  else
  {
    return 0U;            
  }
}


 



 





 

 







 
static __inline void SCB_EnableICache (void)
{

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;                      
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR |=  (uint32_t)(1UL << 17U);   
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_DisableICache (void)
{

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR &= ~(uint32_t)(1UL << 17U);   
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;                      
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_InvalidateICache (void)
{

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_EnableDCache (void)
{

    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );

          __schedule_barrier();

      } while (ways-- != 0U);
    } while(sets-- != 0U);
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR |=  (uint32_t)(1UL << 16U);   

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_DisableDCache (void)
{

    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR &= ~(uint32_t)(1UL << 16U);   
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                       ((ways << 30U) & (3UL << 30U))  );

          __schedule_barrier();

      } while (ways-- != 0U);
    } while(sets-- != 0U);

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_InvalidateDCache (void)
{

    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );

          __schedule_barrier();

      } while (ways-- != 0U);
    } while(sets-- != 0U);

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_CleanDCache (void)
{

    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

     ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
   do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCSW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );

          __schedule_barrier();

      } while (ways-- != 0U);
    } while(sets-- != 0U);

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}





 
static __inline void SCB_CleanInvalidateDCache (void)
{

    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                       ((ways << 30U) & (3UL << 30U))  );

          __schedule_barrier();

      } while (ways-- != 0U);
    } while(sets-- != 0U);

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}







 
static __inline void SCB_InvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize)
{

     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t)addr;
     int32_t linesize = 32;                 

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    while (op_size > 0) {
      ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCIMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}







 
static __inline void SCB_CleanDCache_by_Addr (uint32_t *addr, int32_t dsize)
{

     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 32;                 

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    while (op_size > 0) {
      ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}







 
static __inline void SCB_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize)
{

     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 32;                 

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);

    while (op_size > 0) {
      ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCIMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    do { __schedule_barrier(); __dsb(0xF); __schedule_barrier(); } while (0U);
    do { __schedule_barrier(); __isb(0xF); __schedule_barrier(); } while (0U);

}


 



 





 













 
static __inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);                                                    
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD  = (uint32_t)(ticks - 1UL);                          
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 4) - 1UL);  
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL   = 0UL;                                              
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL  = (1UL << 2U) |
                   (1UL << 1U)   |
                   (1UL );                          
  return (0UL);                                                      
}



 



 





 

extern volatile int32_t ITM_RxBuffer;                               










 
static __inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&       
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL               ) != 0UL)   )      
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __nop();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}







 
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

 










#line 226 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 




#line 1 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\system_stm32h7xx.h"

















 



 



 



 









 



 




 
  






 
extern uint32_t SystemCoreClock;              
extern uint32_t SystemD2Clock;                
extern const  uint8_t D1CorePrescTable[16] ;  



 



 



 



 



 



 

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);


 









 



 
 
#line 235 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
#line 236 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 



 

typedef struct
{
  volatile uint32_t ISR;               
  volatile uint32_t IER;               
  volatile uint32_t CR;                
  volatile uint32_t CFGR;              
  volatile uint32_t CFGR2;             
  volatile uint32_t SMPR1;             
  volatile uint32_t SMPR2;             
  volatile uint32_t PCSEL;             
  volatile uint32_t LTR1;              
  volatile uint32_t HTR1;              
  uint32_t      RESERVED1;         
  uint32_t      RESERVED2;         
  volatile uint32_t SQR1;              
  volatile uint32_t SQR2;              
  volatile uint32_t SQR3;              
  volatile uint32_t SQR4;              
  volatile uint32_t DR;                
  uint32_t      RESERVED3;         
  uint32_t      RESERVED4;         
  volatile uint32_t JSQR;              
  uint32_t      RESERVED5[4];      
  volatile uint32_t OFR1;              
  volatile uint32_t OFR2;              
  volatile uint32_t OFR3;              
  volatile uint32_t OFR4;              
  uint32_t      RESERVED6[4];      
  volatile uint32_t JDR1;              
  volatile uint32_t JDR2;              
  volatile uint32_t JDR3;              
  volatile uint32_t JDR4;              
  uint32_t      RESERVED7[4];      
  volatile uint32_t AWD2CR;            
  volatile uint32_t AWD3CR;            
  uint32_t      RESERVED8;         
  uint32_t      RESERVED9;         
  volatile uint32_t LTR2;              
  volatile uint32_t HTR2;              
  volatile uint32_t LTR3;              
  volatile uint32_t HTR3;              
  volatile uint32_t DIFSEL;            
  volatile uint32_t CALFACT;           
  volatile uint32_t CALFACT2;          
} ADC_TypeDef;


typedef struct
{
volatile uint32_t CSR;  
uint32_t RESERVED;  
volatile uint32_t CCR;  
volatile uint32_t CDR;  
volatile uint32_t CDR2;  

} ADC_Common_TypeDef;




 

typedef struct
{
  volatile uint32_t CSR;          
  volatile uint32_t CCR;          
} VREFBUF_TypeDef;




 

typedef struct
{
  volatile uint32_t CREL;          
  volatile uint32_t ENDN;          
  volatile uint32_t RESERVED1;     
  volatile uint32_t DBTP;          
  volatile uint32_t TEST;          
  volatile uint32_t RWD;           
  volatile uint32_t CCCR;          
  volatile uint32_t NBTP;          
  volatile uint32_t TSCC;          
  volatile uint32_t TSCV;          
  volatile uint32_t TOCC;          
  volatile uint32_t TOCV;          
  volatile uint32_t RESERVED2[4];  
  volatile uint32_t ECR;           
  volatile uint32_t PSR;           
  volatile uint32_t TDCR;          
  volatile uint32_t RESERVED3;     
  volatile uint32_t IR;            
  volatile uint32_t IE;            
  volatile uint32_t ILS;           
  volatile uint32_t ILE;           
  volatile uint32_t RESERVED4[8];  
  volatile uint32_t GFC;           
  volatile uint32_t SIDFC;         
  volatile uint32_t XIDFC;         
  volatile uint32_t RESERVED5;     
  volatile uint32_t XIDAM;         
  volatile uint32_t HPMS;          
  volatile uint32_t NDAT1;         
  volatile uint32_t NDAT2;         
  volatile uint32_t RXF0C;         
  volatile uint32_t RXF0S;         
  volatile uint32_t RXF0A;         
  volatile uint32_t RXBC;          
  volatile uint32_t RXF1C;         
  volatile uint32_t RXF1S;         
  volatile uint32_t RXF1A;         
  volatile uint32_t RXESC;         
  volatile uint32_t TXBC;          
  volatile uint32_t TXFQS;         
  volatile uint32_t TXESC;         
  volatile uint32_t TXBRP;         
  volatile uint32_t TXBAR;         
  volatile uint32_t TXBCR;         
  volatile uint32_t TXBTO;         
  volatile uint32_t TXBCF;         
  volatile uint32_t TXBTIE;        
  volatile uint32_t TXBCIE;        
  volatile uint32_t RESERVED6[2];  
  volatile uint32_t TXEFC;         
  volatile uint32_t TXEFS;         
  volatile uint32_t TXEFA;         
  volatile uint32_t RESERVED7;     
} FDCAN_GlobalTypeDef;



 

typedef struct
{
  volatile uint32_t TTTMC;           
  volatile uint32_t TTRMC;           
  volatile uint32_t TTOCF;           
  volatile uint32_t TTMLM;           
  volatile uint32_t TURCF;           
  volatile uint32_t TTOCN;           
  volatile uint32_t TTGTP;           
  volatile uint32_t TTTMK;           
  volatile uint32_t TTIR;            
  volatile uint32_t TTIE;            
  volatile uint32_t TTILS;           
  volatile uint32_t TTOST;           
  volatile uint32_t TURNA;           
  volatile uint32_t TTLGT;           
  volatile uint32_t TTCTC;           
  volatile uint32_t TTCPT;           
  volatile uint32_t TTCSM;           
  volatile uint32_t RESERVED1[111];  
  volatile uint32_t TTTS;            
} TTCAN_TypeDef;



 

typedef struct
{
  volatile uint32_t CREL;   
  volatile uint32_t CCFG;   
  volatile uint32_t CSTAT;  
  volatile uint32_t CWD;    
  volatile uint32_t IR;     
  volatile uint32_t IE;     
} FDCAN_ClockCalibrationUnit_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;            
  volatile uint32_t CFGR;          
  volatile uint32_t TXDR;          
  volatile uint32_t RXDR;          
  volatile uint32_t ISR;           
  volatile uint32_t IER;           
}CEC_TypeDef;



 

typedef struct
{
  volatile uint32_t DR;           
  volatile uint32_t IDR;          
  volatile uint32_t CR;           
  uint32_t      RESERVED2;    
  volatile uint32_t INIT;         
  volatile uint32_t POL;          
} CRC_TypeDef;




 
typedef struct
{
volatile uint32_t CR;             
volatile uint32_t CFGR;           
volatile uint32_t ISR;            
volatile uint32_t ICR;            
} CRS_TypeDef;




 

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
  volatile uint32_t CCR;       
  volatile uint32_t MCR;       
  volatile uint32_t SHSR1;     
  volatile uint32_t SHSR2;     
  volatile uint32_t SHHR;      
  volatile uint32_t SHRR;      
} DAC_TypeDef;



 
typedef struct
{
  volatile uint32_t FLTCR1;           
  volatile uint32_t FLTCR2;           
  volatile uint32_t FLTISR;           
  volatile uint32_t FLTICR;           
  volatile uint32_t FLTJCHGR;         
  volatile uint32_t FLTFCR;           
  volatile uint32_t FLTJDATAR;        
  volatile uint32_t FLTRDATAR;        
  volatile uint32_t FLTAWHTR;         
  volatile uint32_t FLTAWLTR;         
  volatile uint32_t FLTAWSR;          
  volatile uint32_t FLTAWCFR;         
  volatile uint32_t FLTEXMAX;         
  volatile uint32_t FLTEXMIN;         
  volatile uint32_t FLTCNVTIMR;       
} DFSDM_Filter_TypeDef;



 
typedef struct
{
  volatile uint32_t CHCFGR1;       
  volatile uint32_t CHCFGR2;       
  volatile uint32_t CHAWSCDR;     
 
  volatile uint32_t CHWDATAR;      
  volatile uint32_t CHDATINR;      
} DFSDM_Channel_TypeDef;



 
typedef struct
{
  volatile uint32_t IDCODE;         
  volatile uint32_t CR;             
  uint32_t RESERVED4[11];       
  volatile uint32_t APB3FZ1;      
  uint32_t RESERVED5;           
  volatile uint32_t APB1LFZ1;     
  uint32_t RESERVED6;           
  volatile uint32_t APB1HFZ1;     
  uint32_t RESERVED7;           
  volatile uint32_t APB2FZ1;      
  uint32_t RESERVED8;           
  volatile uint32_t APB4FZ1;      
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
  volatile uint32_t CCR;           
  volatile uint32_t CNDTR;         
  volatile uint32_t CPAR;          
  volatile uint32_t CM0AR;         
  volatile uint32_t CM1AR;         
} BDMA_Channel_TypeDef;

typedef struct
{
  volatile uint32_t ISR;           
  volatile uint32_t IFCR;          
} BDMA_TypeDef;

typedef struct
{
  volatile uint32_t  CCR;         
}DMAMUX_Channel_TypeDef;

typedef struct
{
  volatile uint32_t  CSR;       
  volatile uint32_t  CFR;       
}DMAMUX_ChannelStatus_TypeDef;

typedef struct
{
  volatile uint32_t  RGCR;         
}DMAMUX_RequestGen_TypeDef;

typedef struct
{
  volatile uint32_t  RGSR;         
  volatile uint32_t  RGCFR;        
}DMAMUX_RequestGenStatus_TypeDef;



 
typedef struct
{
  volatile uint32_t  GISR0;    
}MDMA_TypeDef;

typedef struct
{
  volatile uint32_t  CISR;       
  volatile uint32_t  CIFCR;      
  volatile uint32_t  CESR;       
  volatile uint32_t  CCR;        
  volatile uint32_t  CTCR;       
  volatile uint32_t  CBNDTR;     
  volatile uint32_t  CSAR;       
  volatile uint32_t  CDAR;       
  volatile uint32_t  CBRUR;      
  volatile uint32_t  CLAR;       
  volatile uint32_t  CTBR;       
  uint32_t       RESERVED0;  
  volatile uint32_t  CMAR;       
  volatile uint32_t  CMDR;       
}MDMA_Channel_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t ISR;            
  volatile uint32_t IFCR;           
  volatile uint32_t FGMAR;          
  volatile uint32_t FGOR;           
  volatile uint32_t BGMAR;          
  volatile uint32_t BGOR;           
  volatile uint32_t FGPFCCR;        
  volatile uint32_t FGCOLR;         
  volatile uint32_t BGPFCCR;        
  volatile uint32_t BGCOLR;         
  volatile uint32_t FGCMAR;         
  volatile uint32_t BGCMAR;         
  volatile uint32_t OPFCCR;         
  volatile uint32_t OCOLR;          
  volatile uint32_t OMAR;           
  volatile uint32_t OOR;            
  volatile uint32_t NLR;            
  volatile uint32_t LWR;            
  volatile uint32_t AMTCR;          
  uint32_t      RESERVED[236];  
  volatile uint32_t FGCLUT[256];    
  volatile uint32_t BGCLUT[256];    
} DMA2D_TypeDef;




 
typedef struct
{
  volatile uint32_t MACCR;
  volatile uint32_t MACECR;
  volatile uint32_t MACPFR;
  volatile uint32_t MACWTR;
  volatile uint32_t MACHT0R;
  volatile uint32_t MACHT1R;
  uint32_t      RESERVED1[14];
  volatile uint32_t MACVTR;
  uint32_t      RESERVED2;
  volatile uint32_t MACVHTR;
  uint32_t      RESERVED3;
  volatile uint32_t MACVIR;
  volatile uint32_t MACIVIR;
  uint32_t      RESERVED4[2];
  volatile uint32_t MACTFCR;
  uint32_t      RESERVED5[7];
  volatile uint32_t MACRFCR;
  uint32_t      RESERVED6[7];
  volatile uint32_t MACISR;
  volatile uint32_t MACIER;
  volatile uint32_t MACRXTXSR;
  uint32_t      RESERVED7;
  volatile uint32_t MACPCSR;
  volatile uint32_t MACRWKPFR;
  uint32_t      RESERVED8[2];
  volatile uint32_t MACLCSR;
  volatile uint32_t MACLTCR;
  volatile uint32_t MACLETR;
  volatile uint32_t MAC1USTCR;
  uint32_t      RESERVED9[12];
  volatile uint32_t MACVR;
  volatile uint32_t MACDR;
  uint32_t      RESERVED10;
  volatile uint32_t MACHWF0R;
  volatile uint32_t MACHWF1R;
  volatile uint32_t MACHWF2R;
  uint32_t      RESERVED11[54];
  volatile uint32_t MACMDIOAR;
  volatile uint32_t MACMDIODR;
  uint32_t      RESERVED12[2];
  volatile uint32_t MACARPAR;
  uint32_t      RESERVED13[59];
  volatile uint32_t MACA0HR;
  volatile uint32_t MACA0LR;
  volatile uint32_t MACA1HR;
  volatile uint32_t MACA1LR;
  volatile uint32_t MACA2HR;
  volatile uint32_t MACA2LR;
  volatile uint32_t MACA3HR;
  volatile uint32_t MACA3LR;
  uint32_t      RESERVED14[248];
  volatile uint32_t MMCCR;
  volatile uint32_t MMCRIR;
  volatile uint32_t MMCTIR;
  volatile uint32_t MMCRIMR;
  volatile uint32_t MMCTIMR;
  uint32_t      RESERVED15[14];
  volatile uint32_t MMCTSCGPR;
  volatile uint32_t MMCTMCGPR;
  uint32_t      RESERVED16[5];
  volatile uint32_t MMCTPCGR;
  uint32_t      RESERVED17[10];
  volatile uint32_t MMCRCRCEPR;
  volatile uint32_t MMCRAEPR;
  uint32_t      RESERVED18[10];
  volatile uint32_t MMCRUPGR;
  uint32_t      RESERVED19[9];
  volatile uint32_t MMCTLPIMSTR;
  volatile uint32_t MMCTLPITCR;
  volatile uint32_t MMCRLPIMSTR;
  volatile uint32_t MMCRLPITCR;
  uint32_t      RESERVED20[65];
  volatile uint32_t MACL3L4C0R;
  volatile uint32_t MACL4A0R;
  uint32_t      RESERVED21[2];
  volatile uint32_t MACL3A0R0R;
  volatile uint32_t MACL3A1R0R;
  volatile uint32_t MACL3A2R0R;
  volatile uint32_t MACL3A3R0R;
  uint32_t      RESERVED22[4];
  volatile uint32_t MACL3L4C1R;
  volatile uint32_t MACL4A1R;
  uint32_t      RESERVED23[2];
  volatile uint32_t MACL3A0R1R;
  volatile uint32_t MACL3A1R1R;
  volatile uint32_t MACL3A2R1R;
  volatile uint32_t MACL3A3R1R;
  uint32_t      RESERVED24[108];
  volatile uint32_t MACTSCR;
  volatile uint32_t MACSSIR;
  volatile uint32_t MACSTSR;
  volatile uint32_t MACSTNR;
  volatile uint32_t MACSTSUR;
  volatile uint32_t MACSTNUR;
  volatile uint32_t MACTSAR;
  uint32_t      RESERVED25;
  volatile uint32_t MACTSSR;
  uint32_t      RESERVED26[3];
  volatile uint32_t MACTTSSNR;
  volatile uint32_t MACTTSSSR;
  uint32_t      RESERVED27[2];
  volatile uint32_t MACACR;
  uint32_t      RESERVED28;
  volatile uint32_t MACATSNR;
  volatile uint32_t MACATSSR;
  volatile uint32_t MACTSIACR;
  volatile uint32_t MACTSEACR;
  volatile uint32_t MACTSICNR;
  volatile uint32_t MACTSECNR;
  uint32_t      RESERVED29[4];
  volatile uint32_t MACPPSCR;
  uint32_t      RESERVED30[3];
  volatile uint32_t MACPPSTTSR;
  volatile uint32_t MACPPSTTNR;
  volatile uint32_t MACPPSIR;
  volatile uint32_t MACPPSWR;
  uint32_t      RESERVED31[12];
  volatile uint32_t MACPOCR;
  volatile uint32_t MACSPI0R;
  volatile uint32_t MACSPI1R;
  volatile uint32_t MACSPI2R;
  volatile uint32_t MACLMIR;
  uint32_t      RESERVED32[11];
  volatile uint32_t MTLOMR;
  uint32_t      RESERVED33[7];
  volatile uint32_t MTLISR;
  uint32_t      RESERVED34[55];
  volatile uint32_t MTLTQOMR;
  volatile uint32_t MTLTQUR;
  volatile uint32_t MTLTQDR;
  uint32_t      RESERVED35[8];
  volatile uint32_t MTLQICSR;
  volatile uint32_t MTLRQOMR;
  volatile uint32_t MTLRQMPOCR;
  volatile uint32_t MTLRQDR;
  uint32_t      RESERVED36[177];
  volatile uint32_t DMAMR;
  volatile uint32_t DMASBMR;
  volatile uint32_t DMAISR;
  volatile uint32_t DMADSR;
  uint32_t      RESERVED37[60];
  volatile uint32_t DMACCR;
  volatile uint32_t DMACTCR;
  volatile uint32_t DMACRCR;
  uint32_t      RESERVED38[2];
  volatile uint32_t DMACTDLAR;
  uint32_t      RESERVED39;
  volatile uint32_t DMACRDLAR;
  volatile uint32_t DMACTDTPR;
  uint32_t      RESERVED40;
  volatile uint32_t DMACRDTPR;
  volatile uint32_t DMACTDRLR;
  volatile uint32_t DMACRDRLR;
  volatile uint32_t DMACIER;
  volatile uint32_t DMACRIWTR;
volatile uint32_t DMACSFCSR;
  uint32_t      RESERVED41;
  volatile uint32_t DMACCATDR;
  uint32_t      RESERVED42;
  volatile uint32_t DMACCARDR;
  uint32_t      RESERVED43;
  volatile uint32_t DMACCATBR;
  uint32_t      RESERVED44;
  volatile uint32_t DMACCARBR;
  volatile uint32_t DMACSR;
uint32_t      RESERVED45[2];
volatile uint32_t DMACMFCR;
}ETH_TypeDef;


 

typedef struct
{
volatile uint32_t RTSR1;                
volatile uint32_t FTSR1;                
volatile uint32_t SWIER1;               
volatile uint32_t D3PMR1;               
volatile uint32_t D3PCR1L;              
volatile uint32_t D3PCR1H;              
uint32_t      RESERVED1[2];         
volatile uint32_t RTSR2;                
volatile uint32_t FTSR2;                
volatile uint32_t SWIER2;               
volatile uint32_t D3PMR2;               
volatile uint32_t D3PCR2L;              
volatile uint32_t D3PCR2H;              
uint32_t      RESERVED2[2];         
volatile uint32_t RTSR3;                
volatile uint32_t FTSR3;                
volatile uint32_t SWIER3;               
volatile uint32_t D3PMR3;               
volatile uint32_t D3PCR3L;              
volatile uint32_t D3PCR3H;              
uint32_t      RESERVED3[10];        
volatile uint32_t IMR1;                 
volatile uint32_t EMR1;                 
volatile uint32_t PR1;                  
uint32_t      RESERVED4;            
volatile uint32_t IMR2;                 
volatile uint32_t EMR2;                 
volatile uint32_t PR2;                  
uint32_t      RESERVED5;            
volatile uint32_t IMR3;                 
volatile uint32_t EMR3;                 
volatile uint32_t PR3;                  
}EXTI_TypeDef;

typedef struct
{
volatile uint32_t IMR1;                 
volatile uint32_t EMR1;                 
volatile uint32_t PR1;                  
uint32_t      RESERVED1;            
volatile uint32_t IMR2;                 
volatile uint32_t EMR2;                 
volatile uint32_t PR2;                  
uint32_t      RESERVED2;            
volatile uint32_t IMR3;                 
volatile uint32_t EMR3;                 
volatile uint32_t PR3;                  
}EXTI_Core_TypeDef;




 

typedef struct
{
  volatile uint32_t ACR;              
  volatile uint32_t KEYR1;            
  volatile uint32_t OPTKEYR;          
  volatile uint32_t CR1;              
  volatile uint32_t SR1;              
  volatile uint32_t CCR1;             
  volatile uint32_t OPTCR;            
  volatile uint32_t OPTSR_CUR;        
  volatile uint32_t OPTSR_PRG;        
  volatile uint32_t OPTCCR;           
  volatile uint32_t PRAR_CUR1;        
  volatile uint32_t PRAR_PRG1;        
  volatile uint32_t SCAR_CUR1;        
  volatile uint32_t SCAR_PRG1;        
  volatile uint32_t WPSN_CUR1;        
  volatile uint32_t WPSN_PRG1;        
  volatile uint32_t BOOT_CUR;         
  volatile uint32_t BOOT_PRG;         
  uint32_t      RESERVED0[2];     
  volatile uint32_t CRCCR1;           
  volatile uint32_t CRCSADD1;         
  volatile uint32_t CRCEADD1;         
  volatile uint32_t CRCDATA;          
  volatile uint32_t ECC_FA1;          
  uint32_t      RESERVED1[40];    
  volatile uint32_t KEYR2;            
  uint32_t      RESERVED2;        
  volatile uint32_t CR2;              
  volatile uint32_t SR2;              
  volatile uint32_t CCR2;             
  uint32_t      RESERVED3[4];     
  volatile uint32_t PRAR_CUR2;        
  volatile uint32_t PRAR_PRG2;        
  volatile uint32_t SCAR_CUR2;        
  volatile uint32_t SCAR_PRG2;        
  volatile uint32_t WPSN_CUR2;        
  volatile uint32_t WPSN_PRG2;        
  uint32_t      RESERVED4[4];     
  volatile uint32_t CRCCR2;           
  volatile uint32_t CRCSADD2;         
  volatile uint32_t CRCEADD2;         
  volatile uint32_t CRCDATA2;         
  volatile uint32_t ECC_FA2;          
} FLASH_TypeDef;



 

typedef struct
{
  volatile uint32_t BTCR[8];     
} FMC_Bank1_TypeDef;



 

typedef struct
{
  volatile uint32_t BWTR[7];     
} FMC_Bank1E_TypeDef;



 

typedef struct
{
  volatile uint32_t PCR2;        
  volatile uint32_t SR2;         
  volatile uint32_t PMEM2;       
  volatile uint32_t PATT2;       
  uint32_t      RESERVED0;   
  volatile uint32_t ECCR2;       
} FMC_Bank2_TypeDef;



 

typedef struct
{
  volatile uint32_t PCR;        
  volatile uint32_t SR;         
  volatile uint32_t PMEM;       
  volatile uint32_t PATT;       
  uint32_t      RESERVED;   
  volatile uint32_t ECCR;       
} FMC_Bank3_TypeDef;



 


typedef struct
{
  volatile uint32_t SDCR[2];         
  volatile uint32_t SDTR[2];         
  volatile uint32_t SDCMR;        
  volatile uint32_t SDRTR;        
  volatile uint32_t SDSR;         
} FMC_Bank5_6_TypeDef;



 

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
  volatile uint32_t CSR;           
  volatile uint32_t OTR;           
  volatile uint32_t HSOTR;         
} OPAMP_TypeDef;



 

typedef struct
{
 uint32_t RESERVED1;            
 volatile uint32_t PMCR;            
 volatile uint32_t EXTICR[4];       
 volatile uint32_t CFGR;            
 uint32_t RESERVED2;            
 volatile uint32_t CCCSR;           
 volatile uint32_t CCVR;            
 volatile uint32_t CCCR;            
 volatile uint32_t PWRCR;           
  uint32_t     RESERVED3[61];   
  volatile uint32_t PKGR;           
  uint32_t     RESERVED4[118];  
 volatile uint32_t UR0;             
 volatile uint32_t UR1;             
 volatile uint32_t UR2;             
 volatile uint32_t UR3;             
 volatile uint32_t UR4;             
 volatile uint32_t UR5;             
 volatile uint32_t UR6;             
 volatile uint32_t UR7;             
 volatile uint32_t UR8;             
 volatile uint32_t UR9;             
 volatile uint32_t UR10;            
 volatile uint32_t UR11;            
 volatile uint32_t UR12;            
 volatile uint32_t UR13;            
 volatile uint32_t UR14;            
 volatile uint32_t UR15;            
 volatile uint32_t UR16;            
 volatile uint32_t UR17;            

} SYSCFG_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;       
  volatile uint32_t CR2;       
  volatile uint32_t OAR1;      
  volatile uint32_t OAR2;      
  volatile uint32_t TIMINGR;   
  volatile uint32_t TIMEOUTR;  
  volatile uint32_t ISR;       
  volatile uint32_t ICR;       
  volatile uint32_t PECR;      
  volatile uint32_t RXDR;      
  volatile uint32_t TXDR;      
} I2C_TypeDef;



 

typedef struct
{
  volatile uint32_t KR;    
  volatile uint32_t PR;    
  volatile uint32_t RLR;   
  volatile uint32_t SR;    
  volatile uint32_t WINR;  
} IWDG_TypeDef;




 
typedef struct
{
  volatile uint32_t CONFR0;           
  volatile uint32_t CONFR1;           
  volatile uint32_t CONFR2;           
  volatile uint32_t CONFR3;           
  volatile uint32_t CONFR4;           
  volatile uint32_t CONFR5;           
  volatile uint32_t CONFR6;           
  volatile uint32_t CONFR7;           
  uint32_t  Reserved20[4];        
  volatile uint32_t CR;               
  volatile uint32_t SR;               
  volatile uint32_t CFR;              
  uint32_t  Reserved3c;           
  volatile uint32_t DIR;              
  volatile uint32_t DOR;              
  uint32_t  Reserved48[2];        
  volatile uint32_t QMEM0[16];        
  volatile uint32_t QMEM1[16];        
  volatile uint32_t QMEM2[16];        
  volatile uint32_t QMEM3[16];        
  volatile uint32_t HUFFMIN[16];      
  volatile uint32_t HUFFBASE[32];     
  volatile uint32_t HUFFSYMB[84];     
  volatile uint32_t DHTMEM[103];      
  uint32_t  Reserved4FC;          
  volatile uint32_t HUFFENC_AC0[88];  
  volatile uint32_t HUFFENC_AC1[88];  
  volatile uint32_t HUFFENC_DC0[8];   
  volatile uint32_t HUFFENC_DC1[8];   

} JPEG_TypeDef;



 

typedef struct
{
  uint32_t      RESERVED0[2];   
  volatile uint32_t SSCR;           
  volatile uint32_t BPCR;           
  volatile uint32_t AWCR;           
  volatile uint32_t TWCR;           
  volatile uint32_t GCR;            
  uint32_t      RESERVED1[2];   
  volatile uint32_t SRCR;           
  uint32_t      RESERVED2[1];   
  volatile uint32_t BCCR;           
  uint32_t      RESERVED3[1];   
  volatile uint32_t IER;            
  volatile uint32_t ISR;            
  volatile uint32_t ICR;            
  volatile uint32_t LIPCR;          
  volatile uint32_t CPSR;           
  volatile uint32_t CDSR;          
} LTDC_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t WHPCR;          
  volatile uint32_t WVPCR;          
  volatile uint32_t CKCR;           
  volatile uint32_t PFCR;           
  volatile uint32_t CACR;           
  volatile uint32_t DCCR;           
  volatile uint32_t BFCR;           
  uint32_t      RESERVED0[2];   
  volatile uint32_t CFBAR;          
  volatile uint32_t CFBLR;          
  volatile uint32_t CFBLNR;         
  uint32_t      RESERVED1[3];   
  volatile uint32_t CLUTWR;          

} LTDC_Layer_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;        
  volatile uint32_t CSR1;       
  volatile uint32_t CR2;        
  volatile uint32_t CR3;        
  volatile uint32_t CPUCR;      
       uint32_t RESERVED0;  
  volatile uint32_t D3CR;       
       uint32_t RESERVED1;  
  volatile uint32_t WKUPCR;     
  volatile uint32_t WKUPFR;     
  volatile uint32_t WKUPEPR;    
} PWR_TypeDef;



 

typedef struct
{
 volatile uint32_t CR;              
 volatile uint32_t HSICFGR;         
 volatile uint32_t CRRCR;           
 volatile uint32_t CSICFGR;         
 volatile uint32_t CFGR;            
 uint32_t     RESERVED1;        
 volatile uint32_t D1CFGR;          
 volatile uint32_t D2CFGR;          
 volatile uint32_t D3CFGR;          
 uint32_t     RESERVED2;        
 volatile uint32_t PLLCKSELR;       
 volatile uint32_t PLLCFGR;         
 volatile uint32_t PLL1DIVR;        
 volatile uint32_t PLL1FRACR;       
 volatile uint32_t PLL2DIVR;        
 volatile uint32_t PLL2FRACR;       
 volatile uint32_t PLL3DIVR;        
 volatile uint32_t PLL3FRACR;       
 uint32_t      RESERVED3;       
 volatile uint32_t  D1CCIPR;        
 volatile uint32_t  D2CCIP1R;       
 volatile uint32_t  D2CCIP2R;       
 volatile uint32_t  D3CCIPR;        
 uint32_t      RESERVED4;       
 volatile uint32_t  CIER;           
 volatile uint32_t  CIFR;           
 volatile uint32_t  CICR;           
 uint32_t     RESERVED5;        
 volatile uint32_t  BDCR;           
 volatile uint32_t  CSR;            
 uint32_t     RESERVED6;        
 volatile uint32_t AHB3RSTR;        
 volatile uint32_t AHB1RSTR;        
 volatile uint32_t AHB2RSTR;        
 volatile uint32_t AHB4RSTR;        
 volatile uint32_t APB3RSTR;        
 volatile uint32_t APB1LRSTR;       
 volatile uint32_t APB1HRSTR;       
 volatile uint32_t APB2RSTR;        
 volatile uint32_t APB4RSTR;        
 volatile uint32_t GCR;             
 uint32_t     RESERVED8;        
 volatile uint32_t D3AMR;           
 uint32_t     RESERVED11[9];     
 volatile uint32_t RSR;             
 volatile uint32_t AHB3ENR;         
 volatile uint32_t AHB1ENR;         
 volatile uint32_t AHB2ENR;         
 volatile uint32_t AHB4ENR;         
 volatile uint32_t APB3ENR;         
 volatile uint32_t APB1LENR;        
 volatile uint32_t APB1HENR;        
 volatile uint32_t APB2ENR;         
 volatile uint32_t APB4ENR;         
 uint32_t      RESERVED12;       
 volatile uint32_t AHB3LPENR;       
 volatile uint32_t AHB1LPENR;       
 volatile uint32_t AHB2LPENR;       
 volatile uint32_t AHB4LPENR;       
 volatile uint32_t APB3LPENR;       
 volatile uint32_t APB1LLPENR;      
 volatile uint32_t APB1HLPENR;      
 volatile uint32_t APB2LPENR;       
 volatile uint32_t APB4LPENR;       
 uint32_t     RESERVED13[4];    

} RCC_TypeDef;




 
typedef struct
{
  volatile uint32_t TR;          
  volatile uint32_t DR;          
  volatile uint32_t CR;          
  volatile uint32_t ISR;         
  volatile uint32_t PRER;        
  volatile uint32_t WUTR;        
       uint32_t RESERVED;    
  volatile uint32_t ALRMAR;      
  volatile uint32_t ALRMBR;      
  volatile uint32_t WPR;         
  volatile uint32_t SSR;         
  volatile uint32_t SHIFTR;      
  volatile uint32_t TSTR;        
  volatile uint32_t TSDR;        
  volatile uint32_t TSSSR;       
  volatile uint32_t CALR;        
  volatile uint32_t TAMPCR;      
  volatile uint32_t ALRMASSR;    
  volatile uint32_t ALRMBSSR;    
  volatile uint32_t OR;          
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
  volatile uint32_t BKP20R;      
  volatile uint32_t BKP21R;      
  volatile uint32_t BKP22R;      
  volatile uint32_t BKP23R;      
  volatile uint32_t BKP24R;      
  volatile uint32_t BKP25R;      
  volatile uint32_t BKP26R;      
  volatile uint32_t BKP27R;      
  volatile uint32_t BKP28R;      
  volatile uint32_t BKP29R;      
  volatile uint32_t BKP30R;      
  volatile uint32_t BKP31R;      
} RTC_TypeDef;



 

typedef struct
{
  volatile uint32_t GCR;            
  uint32_t      RESERVED0[16];  
  volatile uint32_t PDMCR;          
  volatile uint32_t PDMDLY;         
} SAI_TypeDef;

typedef struct
{
  volatile uint32_t CR1;       
  volatile uint32_t CR2;       
  volatile uint32_t FRCR;      
  volatile uint32_t SLOTR;     
  volatile uint32_t IMR;       
  volatile uint32_t SR;        
  volatile uint32_t CLRFR;     
  volatile uint32_t DR;        
} SAI_Block_TypeDef;



 

typedef struct
{
  volatile uint32_t   CR;            
  volatile uint32_t   IMR;           
  volatile uint32_t   SR;            
  volatile uint32_t   IFCR;          
  volatile uint32_t   DR;            
  volatile uint32_t   CSR;           
  volatile uint32_t   DIR;           
  uint32_t        RESERVED2;     
} SPDIFRX_TypeDef;




 

typedef struct
{
  volatile uint32_t POWER;           
  volatile uint32_t CLKCR;           
  volatile uint32_t ARG;             
  volatile uint32_t CMD;             
  volatile const uint32_t  RESPCMD;         
  volatile const uint32_t  RESP1;           
  volatile const uint32_t  RESP2;           
  volatile const uint32_t  RESP3;           
  volatile const uint32_t  RESP4;           
  volatile uint32_t DTIMER;          
  volatile uint32_t DLEN;            
  volatile uint32_t DCTRL;           
  volatile const uint32_t  DCOUNT;          
  volatile const uint32_t  STA;             
  volatile uint32_t ICR;             
  volatile uint32_t MASK;            
  volatile uint32_t ACKTIME;         
  uint32_t      RESERVED0[3];    
  volatile uint32_t IDMACTRL;        
  volatile uint32_t IDMABSIZE;       
  volatile uint32_t IDMABASE0;       
  volatile uint32_t IDMABASE1;       
  uint32_t      RESERVED1[8];    
  volatile uint32_t FIFO;            
  uint32_t      RESERVED2[222];  
  volatile uint32_t IPVR;            
} SDMMC_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;           
  volatile uint32_t CFGR;         
} DLYB_TypeDef;



 

typedef struct
{
  volatile uint32_t R[32];       
  volatile uint32_t RLR[32];     
  volatile uint32_t C1IER;       
  volatile uint32_t C1ICR;       
  volatile uint32_t C1ISR;       
  volatile uint32_t C1MISR;      
  uint32_t  Reserved[12];    
  volatile uint32_t CR;          
  volatile uint32_t KEYR;        

} HSEM_TypeDef;

typedef struct
{
  volatile uint32_t IER;         
  volatile uint32_t ICR;         
  volatile uint32_t ISR;         
  volatile uint32_t MISR;        
} HSEM_Common_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;            
  volatile uint32_t CR2;            
  volatile uint32_t CFG1;           
  volatile uint32_t CFG2;           
  volatile uint32_t IER;            
  volatile uint32_t SR;             
  volatile uint32_t IFCR;           
  uint32_t      RESERVED0;      
  volatile uint32_t TXDR;           
  uint32_t      RESERVED1[3];   
  volatile uint32_t RXDR;           
  uint32_t      RESERVED2[3];   
  volatile uint32_t CRCPOLY;        
  volatile uint32_t TXCRC;          
  volatile uint32_t RXCRC;          
  volatile uint32_t UDRDR;          
  volatile uint32_t I2SCFGR;        

} SPI_TypeDef;


 

typedef struct
{
  volatile uint32_t CR;        
  volatile uint32_t DCR;       
  volatile uint32_t SR;        
  volatile uint32_t FCR;       
  volatile uint32_t DLR;       
  volatile uint32_t CCR;       
  volatile uint32_t AR;        
  volatile uint32_t ABR;       
  volatile uint32_t DR;        
  volatile uint32_t PSMKR;     
  volatile uint32_t PSMAR;     
  volatile uint32_t PIR;       
  volatile uint32_t LPTR;      
} QUADSPI_TypeDef;



 

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
  uint32_t      RESERVED1;    
  volatile uint32_t CCMR3;        
  volatile uint32_t CCR5;         
  volatile uint32_t CCR6;         
  volatile uint32_t AF1;          
  volatile uint32_t AF2;          
  volatile uint32_t TISEL;        
} TIM_TypeDef;



 
typedef struct
{
  volatile uint32_t ISR;       
  volatile uint32_t ICR;       
  volatile uint32_t IER;       
  volatile uint32_t CFGR;      
  volatile uint32_t CR;        
  volatile uint32_t CMP;       
  volatile uint32_t ARR;       
  volatile uint32_t CNT;       
  uint32_t  RESERVED1;     
  volatile uint32_t CFGR2;     
} LPTIM_TypeDef;



 
typedef struct
{
  volatile uint32_t SR;         
  volatile uint32_t ICFR;       
  volatile uint32_t OR;         
} COMPOPT_TypeDef;

typedef struct
{
  volatile uint32_t CFGR;       
} COMP_TypeDef;

typedef struct
{
  volatile uint32_t CFGR;        
} COMP_Common_TypeDef;


 

typedef struct
{
  volatile uint32_t CR1;     
  volatile uint32_t CR2;     
  volatile uint32_t CR3;     
  volatile uint32_t BRR;     
  volatile uint32_t GTPR;    
  volatile uint32_t RTOR;    
  volatile uint32_t RQR;     
  volatile uint32_t ISR;     
  volatile uint32_t ICR;     
  volatile uint32_t RDR;     
  volatile uint32_t TDR;     
  volatile uint32_t PRESC;   
} USART_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;           
  volatile uint32_t BRR;          
    uint32_t  RESERVED1;      
  volatile uint32_t ISR;          
  volatile uint32_t ICR;          
  volatile uint32_t IER;          
  volatile uint32_t RFL;          
  volatile uint32_t TDR;          
  volatile uint32_t RDR;          
  volatile uint32_t OR;           
} SWPMI_TypeDef;



 

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
  volatile uint32_t FAR;           
  volatile uint32_t FDRL;          
  volatile uint32_t FDRH;          
  volatile uint32_t FECR;          
} RAMECC_MonitorTypeDef;

typedef struct
{
  volatile uint32_t IER;           
} RAMECC_TypeDef;


 




 

typedef struct
{
  volatile uint32_t CR;          
  volatile uint32_t SR;          
  volatile uint32_t DIN;          
  volatile uint32_t DOUT;        
  volatile uint32_t DMACR;       
  volatile uint32_t IMSCR;       
  volatile uint32_t RISR;        
  volatile uint32_t MISR;        
  volatile uint32_t K0LR;        
  volatile uint32_t K0RR;        
  volatile uint32_t K1LR;        
  volatile uint32_t K1RR;        
  volatile uint32_t K2LR;        
  volatile uint32_t K2RR;        
  volatile uint32_t K3LR;        
  volatile uint32_t K3RR;        
  volatile uint32_t IV0LR;       
  volatile uint32_t IV0RR;       
  volatile uint32_t IV1LR;       
  volatile uint32_t IV1RR;       
  volatile uint32_t CSGCMCCM0R;  
  volatile uint32_t CSGCMCCM1R;  
  volatile uint32_t CSGCMCCM2R;  
  volatile uint32_t CSGCMCCM3R;  
  volatile uint32_t CSGCMCCM4R;  
  volatile uint32_t CSGCMCCM5R;  
  volatile uint32_t CSGCMCCM6R;  
  volatile uint32_t CSGCMCCM7R;  
  volatile uint32_t CSGCM0R;     
  volatile uint32_t CSGCM1R;     
  volatile uint32_t CSGCM2R;     
  volatile uint32_t CSGCM3R;     
  volatile uint32_t CSGCM4R;     
  volatile uint32_t CSGCM5R;     
  volatile uint32_t CSGCM6R;     
  volatile uint32_t CSGCM7R;     
} CRYP_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;                
  volatile uint32_t DIN;               
  volatile uint32_t STR;               
  volatile uint32_t HR[5];             
  volatile uint32_t IMR;               
  volatile uint32_t SR;                
       uint32_t RESERVED[52];      
  volatile uint32_t CSR[54];           
} HASH_TypeDef;



 

typedef struct
{
  volatile uint32_t HR[8];      
} HASH_DIGEST_TypeDef;




 
 
typedef struct
{
  volatile uint32_t MCR;             
  volatile uint32_t MISR;            
  volatile uint32_t MICR;            
  volatile uint32_t MDIER;           
  volatile uint32_t MCNTR;           
  volatile uint32_t MPER;            
  volatile uint32_t MREP;            
  volatile uint32_t MCMP1R;          
  uint32_t      RESERVED0;      
  volatile uint32_t MCMP2R;          
  volatile uint32_t MCMP3R;          
  volatile uint32_t MCMP4R;          
  uint32_t      RESERVED1[20];   
}HRTIM_Master_TypeDef;

 
typedef struct
{
  volatile uint32_t TIMxCR;      
  volatile uint32_t TIMxISR;     
  volatile uint32_t TIMxICR;     
  volatile uint32_t TIMxDIER;    
  volatile uint32_t CNTxR;       
  volatile uint32_t PERxR;       
  volatile uint32_t REPxR;       
  volatile uint32_t CMP1xR;      
  volatile uint32_t CMP1CxR;     
  volatile uint32_t CMP2xR;      
  volatile uint32_t CMP3xR;      
  volatile uint32_t CMP4xR;      
  volatile uint32_t CPT1xR;      
  volatile uint32_t CPT2xR;      
  volatile uint32_t DTxR;        
  volatile uint32_t SETx1R;      
  volatile uint32_t RSTx1R;      
  volatile uint32_t SETx2R;      
  volatile uint32_t RSTx2R;      
  volatile uint32_t EEFxR1;      
  volatile uint32_t EEFxR2;      
  volatile uint32_t RSTxR;       
  volatile uint32_t CHPxR;       
  volatile uint32_t CPT1xCR;     
  volatile uint32_t CPT2xCR;     
  volatile uint32_t OUTxR;       
  volatile uint32_t FLTxR;       
  uint32_t      RESERVED0[5];   
}HRTIM_Timerx_TypeDef;

 
typedef struct
{
  volatile uint32_t CR1;         
  volatile uint32_t CR2;         
  volatile uint32_t ISR;         
  volatile uint32_t ICR;         
  volatile uint32_t IER;         
  volatile uint32_t OENR;        
  volatile uint32_t ODISR;       
  volatile uint32_t ODSR;        
  volatile uint32_t BMCR;        
  volatile uint32_t BMTRGR;      
  volatile uint32_t BMCMPR;      
  volatile uint32_t BMPER;       
  volatile uint32_t EECR1;       
  volatile uint32_t EECR2;       
  volatile uint32_t EECR3;       
  volatile uint32_t ADC1R;       
  volatile uint32_t ADC2R;       
  volatile uint32_t ADC3R;       
  volatile uint32_t ADC4R;       
  volatile uint32_t RESERVED0;   
  volatile uint32_t FLTINR1;     
  volatile uint32_t FLTINR2;     
  volatile uint32_t BDMUPR;      
  volatile uint32_t BDTAUPR;     
  volatile uint32_t BDTBUPR;     
  volatile uint32_t BDTCUPR;     
  volatile uint32_t BDTDUPR;     
  volatile uint32_t BDTEUPR;     
  volatile uint32_t BDMADR;      
}HRTIM_Common_TypeDef;

 
typedef struct {
  HRTIM_Master_TypeDef sMasterRegs;
  HRTIM_Timerx_TypeDef sTimerxRegs[5];
  uint32_t             RESERVED0[32];
  HRTIM_Common_TypeDef sCommonRegs;
}HRTIM_TypeDef;


 

typedef struct
{
  volatile uint32_t CR;   
  volatile uint32_t SR;   
  volatile uint32_t DR;   
} RNG_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t WRFR;
  volatile uint32_t CWRFR;
  volatile uint32_t RDFR;
  volatile uint32_t CRDFR;
  volatile uint32_t SR;
  volatile uint32_t CLRFR;
  uint32_t RESERVED[57];
  volatile uint32_t DINR0;
  volatile uint32_t DINR1;
  volatile uint32_t DINR2;
  volatile uint32_t DINR3;
  volatile uint32_t DINR4;
  volatile uint32_t DINR5;
  volatile uint32_t DINR6;
  volatile uint32_t DINR7;
  volatile uint32_t DINR8;
  volatile uint32_t DINR9;
  volatile uint32_t DINR10;
  volatile uint32_t DINR11;
  volatile uint32_t DINR12;
  volatile uint32_t DINR13;
  volatile uint32_t DINR14;
  volatile uint32_t DINR15;
  volatile uint32_t DINR16;
  volatile uint32_t DINR17;
  volatile uint32_t DINR18;
  volatile uint32_t DINR19;
  volatile uint32_t DINR20;
  volatile uint32_t DINR21;
  volatile uint32_t DINR22;
  volatile uint32_t DINR23;
  volatile uint32_t DINR24;
  volatile uint32_t DINR25;
  volatile uint32_t DINR26;
  volatile uint32_t DINR27;
  volatile uint32_t DINR28;
  volatile uint32_t DINR29;
  volatile uint32_t DINR30;
  volatile uint32_t DINR31;
  volatile uint32_t DOUTR0;
  volatile uint32_t DOUTR1;
  volatile uint32_t DOUTR2;
  volatile uint32_t DOUTR3;
  volatile uint32_t DOUTR4;
  volatile uint32_t DOUTR5;
  volatile uint32_t DOUTR6;
  volatile uint32_t DOUTR7;
  volatile uint32_t DOUTR8;
  volatile uint32_t DOUTR9;
  volatile uint32_t DOUTR10;
  volatile uint32_t DOUTR11;
  volatile uint32_t DOUTR12;
  volatile uint32_t DOUTR13;
  volatile uint32_t DOUTR14;
  volatile uint32_t DOUTR15;
  volatile uint32_t DOUTR16;
  volatile uint32_t DOUTR17;
  volatile uint32_t DOUTR18;
  volatile uint32_t DOUTR19;
  volatile uint32_t DOUTR20;
  volatile uint32_t DOUTR21;
  volatile uint32_t DOUTR22;
  volatile uint32_t DOUTR23;
  volatile uint32_t DOUTR24;
  volatile uint32_t DOUTR25;
  volatile uint32_t DOUTR26;
  volatile uint32_t DOUTR27;
  volatile uint32_t DOUTR28;
  volatile uint32_t DOUTR29;
  volatile uint32_t DOUTR30;
  volatile uint32_t DOUTR31;
} MDIOS_TypeDef;




 
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
  volatile uint32_t GSNPSID;               
  volatile uint32_t GHWCFG1;               
  volatile uint32_t GHWCFG2;               
  volatile uint32_t GHWCFG3;               
  uint32_t  Reserved6;                 
  volatile uint32_t GLPMCFG;               
  volatile uint32_t GPWRDN;                
  volatile uint32_t GDFIFOCFG;             
   volatile uint32_t GADPCTL;              
    uint32_t  Reserved43[39];          
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
  uint32_t  Reserved20;           
  uint32_t Reserved9;             
  volatile uint32_t DVBUSDIS;         
  volatile uint32_t DVBUSPULSE;       
  volatile uint32_t DTHRCTL;          
  volatile uint32_t DIEPEMPMSK;       
  volatile uint32_t DEACHINT;         
  volatile uint32_t DEACHMSK;         
  uint32_t Reserved40;            
  volatile uint32_t DINEP1MSK;        
  uint32_t  Reserved44[15];       
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


 




 
#line 2045 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"














 


 




 











 






 

#line 2098 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 2109 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 2125 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 2136 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 2158 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 
#line 2176 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 2202 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 2244 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 2264 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"









#line 2281 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2290 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2299 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"




#line 2311 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2320 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2337 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2346 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"




 






 


#line 2376 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



















 



 
#line 2411 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 2444 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2453 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 2485 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2504 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2516 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 2529 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2539 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2546 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2553 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 2567 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 2577 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"




#line 2590 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2600 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 2619 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2628 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





























#line 2675 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 







 



 

  

 

 
 
 

 
 
 
 
 
 

 
#line 2743 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 2778 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 2836 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 






#line 2850 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2859 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 2875 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 2886 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2905 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2914 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 2926 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2934 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2941 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2954 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2968 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2976 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 2984 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2991 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 2998 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3005 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3012 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3019 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3026 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3033 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3040 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3047 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3055 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3062 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3069 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3076 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3083 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3090 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3097 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3104 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3111 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3118 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3143 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 





 
#line 3163 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3172 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3181 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3190 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3199 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3209 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3218 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3227 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3236 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3245 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3255 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3264 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3273 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3282 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3291 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3301 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3310 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 




 






#line 3330 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 3345 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3354 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3363 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3372 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3403 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3412 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
#line 3448 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3457 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
#line 3493 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3502 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
#line 3538 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3547 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
#line 3589 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3626 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3663 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3700 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3725 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3750 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3775 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3805 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3840 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
#line 3909 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 3919 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3927 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"














#line 3948 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 3958 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 








 





 
 
 
 
 
 
#line 3992 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 4003 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
 
 
 
 
 
 
#line 4034 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 4056 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4067 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4075 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4119 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4133 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4141 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 4157 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 4176 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4211 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4219 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4305 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4397 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4489 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4497 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4511 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4519 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4527 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 4546 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4644 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4742 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4756 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4773 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 4797 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4814 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 4830 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4844 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4858 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 




 
#line 4909 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4926 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 4939 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4950 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4982 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 4996 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5007 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5048 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5056 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5067 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5126 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5185 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5244 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5288 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 5301 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5309 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5317 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 5330 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 

 
#line 5356 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5376 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5387 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5395 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5403 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5411 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 

 
#line 5428 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5457 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 5508 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5549 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 




 




 
#line 5582 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
 
 
 

 
#line 5623 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5631 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5638 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"











 
#line 5677 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5691 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 




#line 5736 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5753 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5763 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5780 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 5787 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 5794 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 5801 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 5808 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 5821 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5829 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"








#line 5844 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5854 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5861 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5869 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"








#line 5884 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5894 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5902 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 
#line 5940 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5948 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5956 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 5977 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 5987 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 5995 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6003 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 6010 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 6028 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6036 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 6091 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6102 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6119 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6136 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 6154 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 6172 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 6187 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6201 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6209 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6217 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6231 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 

 

 
#line 6282 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6290 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6306 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 6319 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

 
#line 6357 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 6373 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6402 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6431 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6445 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 6464 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6472 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6483 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6491 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6499 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6507 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6515 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6523 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6531 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
 
 
 
 

 
#line 6640 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6738 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6764 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



















#line 6792 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 
 
 
 
 
 
#line 6916 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6936 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 6990 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7013 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 7093 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7134 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7170 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7203 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7211 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7240 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7260 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7283 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7312 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7352 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7360 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 7378 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7402 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7480 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7524 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7544 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7625 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7633 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7646 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7665 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7684 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7703 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7728 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 7756 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7773 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7790 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7807 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7824 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 




 




 




 
#line 7912 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 7920 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 
#line 7993 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8001 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 
#line 8024 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 8055 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8063 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 8085 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 
#line 8126 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 8139 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 8172 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 
#line 8198 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8209 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8217 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8236 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8244 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8271 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8285 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8318 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8332 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8366 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8374 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8402 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8416 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8427 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8473 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8486 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8506 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8526 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 
#line 8597 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 
#line 8669 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8677 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 8750 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8771 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8790 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8852 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8914 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 8976 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9038 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 
 
 
 
 
 
#line 9101 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9151 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9201 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9230 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9256 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9282 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 

 

#line 9327 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 9348 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 9369 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 





 





 





 

#line 9431 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 9443 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 9476 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 9488 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 





 

#line 9518 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

 

#line 9535 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9546 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9560 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9574 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 





 

#line 9595 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 

#line 9610 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 

 


 
 
 
 
 
 
#line 9692 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9763 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9831 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9893 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9943 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9957 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9968 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9979 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 9987 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10016 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 10026 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 10047 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 10063 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10080 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10097 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10198 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10296 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10367 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10465 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10563 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10574 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10645 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 10720 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10737 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
 
 
 
 


 
#line 10751 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10777 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10847 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10897 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10935 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10952 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 10997 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 11013 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 11024 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 11037 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 11066 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 
 
 
 
 
 
#line 11099 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"










 
#line 11116 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"













#line 11153 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11160 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 11173 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11181 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11193 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11201 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11209 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11217 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







 
#line 11232 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11240 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11252 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11260 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







 
#line 11274 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"











#line 11292 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11300 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11307 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 11330 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 11343 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11355 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11367 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11379 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 11392 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11404 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11416 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11428 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
















































 
#line 11491 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11499 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11507 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11514 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11521 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11528 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 11535 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 11543 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"









#line 11559 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 












 
















 
 
 
 
 
 
































































































 
#line 11745 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
































































































 
































































































 
#line 11989 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12039 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12137 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12190 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12248 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12258 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12316 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12326 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 12342 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12353 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12451 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12549 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12647 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12745 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12753 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
 
 
 
 
 
#line 12800 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12813 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12821 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12835 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
 
 
 
 
 
#line 12901 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12936 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12947 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12980 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 12997 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13014 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13067 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13096 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 
 
 
 
 
 




 
#line 13129 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 13145 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
 
 
 
 
 




 
#line 13186 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 13230 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13264 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13298 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13332 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13361 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13384 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13392 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
 
 
 
 

 

#line 13417 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13426 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13435 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13444 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13474 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 

#line 13484 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13496 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13511 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13526 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13541 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 

#line 13556 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13571 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13583 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13592 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13601 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13613 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 





 

#line 13640 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 13649 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 

#line 13664 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 

#line 13685 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 13740 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13760 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13777 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13797 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13834 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13900 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 13914 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 13932 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 13948 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
 
 
 
 
 
#line 13971 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"













#line 13990 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 14004 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14014 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14022 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"













#line 14041 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 14055 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14065 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14073 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"













#line 14092 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 14106 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14116 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14124 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14132 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14140 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14148 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14156 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14164 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 


 
#line 14205 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14229 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14241 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14257 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14280 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14306 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14335 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 14346 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14366 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14386 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14457 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 


 
#line 14483 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14502 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14509 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14522 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 14542 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14550 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
#line 14568 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14580 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 

 
#line 14598 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
 
#line 14613 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14624 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
#line 14633 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
#line 14646 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"














 
#line 14670 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 14688 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14696 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14704 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14711 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
#line 14721 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 14748 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14756 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14770 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14778 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14804 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
#line 14813 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14827 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 14835 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14849 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
#line 14858 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14872 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





#line 14889 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14899 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14909 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14919 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 14935 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14949 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14963 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14973 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14983 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 14993 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 15008 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 15027 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 15046 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 15071 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15079 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15086 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15093 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15100 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





















 
#line 15128 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15135 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

























#line 15166 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15174 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 15187 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15194 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15201 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 15208 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 15221 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 15252 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15287 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15322 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15333 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 15346 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 15359 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 15366 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 15387 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15419 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15427 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 15454 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15465 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15515 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15523 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 15556 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 15600 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 15621 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15668 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15706 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 15727 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 15748 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 15770 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15817 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 




 

#line 15898 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 15919 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 15966 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16001 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 




 
#line 16051 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 16059 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 16093 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 16126 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16134 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16160 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16171 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16221 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 16233 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 16266 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


#line 16310 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 16331 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16378 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16416 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 16449 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
 
 
 
 
 
#line 16470 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16487 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 16535 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16579 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16649 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16705 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16713 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 16788 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16858 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 16876 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16919 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 16949 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 16977 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17049 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17061 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17073 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17081 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 
 
 
 
 
 
#line 17298 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17321 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17353 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17367 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17387 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17407 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17415 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17426 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17434 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17442 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
 
 
 
 
 


 












 












#line 17493 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17500 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 17522 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17532 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"









 


 
#line 17551 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17564 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17574 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"











 
#line 17597 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17608 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17618 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 
#line 17631 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 17645 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 17672 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17695 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17702 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17725 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 










#line 17754 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17762 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17769 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17776 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17783 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17790 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17797 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17804 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17811 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 17832 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 17840 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 17864 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 17880 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 17908 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 




 
#line 17961 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17969 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 17988 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 18082 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18144 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18182 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18189 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18196 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18218 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 18239 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 


 


 
 
 
 
 
 
#line 18263 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 18273 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18284 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18300 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
 
 
 
 
 
#line 18341 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18349 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18359 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18367 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"














#line 18387 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18396 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 18407 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18416 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18424 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"











#line 18441 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18457 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 18467 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 18506 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18558 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18587 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 
#line 18665 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
 
 
 
 
 
#line 18733 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18752 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18781 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18795 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 18865 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 
 
 
 
 

 
#line 18935 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 18958 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 18972 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 18986 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19001 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19015 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19030 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19044 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19058 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19072 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19086 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19101 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19115 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19130 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19145 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19160 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 
#line 19175 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19189 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19203 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19217 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19231 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
#line 19245 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19280 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19294 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19302 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19310 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 




 




 
#line 19327 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19337 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 




   





 
#line 19355 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19363 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19371 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19379 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19387 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19395 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19403 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 
#line 19417 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 19435 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 
 
 
 
 


 
#line 19465 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





















 
#line 19496 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19503 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19534 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19542 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19551 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19560 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 19572 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 19585 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19632 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19682 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19711 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 






#line 19726 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19734 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"











#line 19751 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19759 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 







#line 19779 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 19793 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 






#line 19807 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19815 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"











#line 19832 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 19840 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 







#line 19860 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 19874 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 19933 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 19940 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 




 




 




 




 




 




 




 
#line 19988 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 20006 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 20031 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20038 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20045 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20055 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20064 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 20077 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20085 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 20096 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20104 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"




 
#line 20130 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20138 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20161 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20170 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20178 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20186 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20194 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20217 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20225 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 20247 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20256 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20265 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20274 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20297 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20320 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20343 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 20371 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20394 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20417 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 






















#line 20447 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 20454 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 20479 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20496 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 




 




 




 
#line 20524 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 

 
#line 20544 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 20551 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 20585 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20633 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"


 
 
 
 
 
 
#line 20726 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 20796 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20881 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20889 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20897 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20905 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 20922 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21008 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21055 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 21074 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 

 
#line 21106 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 21149 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21175 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21207 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 




 





 
#line 21232 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 21249 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 21265 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





#line 21276 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 
 
 
 
 

 
#line 21296 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21325 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 21370 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 21394 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21417 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
 
 
 
 
 
#line 21429 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21439 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21461 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21480 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 21493 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







 
#line 21522 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21545 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21568 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21590 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 
#line 21633 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21646 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21653 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21664 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21689 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21698 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21706 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21768 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21812 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 21856 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 21899 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 




 




 




 




 




 




 
#line 21999 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22022 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22038 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22066 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22097 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 22124 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22140 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22168 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22199 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"






 
#line 22227 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22243 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22271 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22302 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 22329 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22345 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22373 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22404 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 
#line 22420 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22431 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22442 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22453 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22464 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22476 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22487 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22498 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22509 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22520 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22531 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22547 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22578 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22588 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22598 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22608 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22618 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22627 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22634 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22642 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22680 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22693 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22706 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22719 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22732 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22745 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22783 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22796 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22809 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22822 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22835 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22848 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22870 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22883 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 22904 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22924 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 22968 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23006 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23029 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23052 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23075 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23107 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23139 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23171 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23217 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23315 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 23343 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23360 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23377 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23394 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23411 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23426 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23440 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23454 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23468 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23482 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23524 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23622 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23720 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23818 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23916 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 23937 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23957 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23977 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 23997 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24023 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24055 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24120 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
 
 
 
 
 
#line 24144 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24158 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24169 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 




 
 
 
 
 
 
#line 24219 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24230 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24241 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
 
 
 
 
 
#line 24302 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 24313 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 24324 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24335 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"













 
#line 24358 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24378 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24392 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24414 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 24427 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




#line 24444 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




#line 24467 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 24485 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24532 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24549 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24564 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24590 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




#line 24607 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24619 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 24662 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24748 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24834 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24842 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 24861 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24869 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 24882 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







#line 24896 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24904 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24912 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 
#line 24936 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




#line 24958 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24969 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 24977 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 24993 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25009 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 25022 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25054 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25062 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25070 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 
#line 25122 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25151 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25180 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25189 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25197 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







 
#line 25238 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25246 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25260 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25269 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25295 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




#line 25314 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"













#line 25346 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 25359 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25370 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

#line 25382 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25417 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25458 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25493 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 25505 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 25520 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 




 




 




 
#line 25543 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 25584 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25622 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 

#line 25631 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"







 
#line 25648 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"



 



 



 

 









 




 



 


 

 


 



 





#line 25706 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"
 
#line 25719 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25745 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25755 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25781 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25791 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25809 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25827 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 25845 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 
#line 25866 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 





 
#line 25888 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 
 


 














 




 


 




 




 


 


 


 



 


 
#line 25952 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"





 


 






 



 
#line 25986 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26000 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26010 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26018 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26026 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 



 



 

 
#line 26048 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26061 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26072 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26081 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26089 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26097 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26106 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 






 
#line 26124 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26133 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 26148 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 






 
#line 26163 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 
#line 26228 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26236 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 



 
#line 26261 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26270 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26278 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26290 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26302 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26311 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26320 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26330 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26340 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 




 




 
#line 26365 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 





 





 
#line 26390 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26400 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26410 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26421 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26432 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26443 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26453 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26464 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 
#line 26474 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 





 


 

 


 

 


 


 
#line 26506 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h750xx.h"

 


 





 



 



 
 
 
 
 
 
 

 








 






 




 



 



 







 
#line 122 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h7xx.h"
#line 147 "..\\..\\Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include\\stm32h7xx.h"



 



 
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;


typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;



 




 



















 













 



 




 
#line 33 "..\\..\\Drivers\\./SYSTEM/sys/sys.h"






 



 




 


















 
#line 84 "..\\..\\Drivers\\./SYSTEM/sys/sys.h"


 
 
static void sys_nvic_priority_group_config(uint8_t group);                       


 
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);              
void sys_nvic_init(uint8_t pprio, uint8_t sprio, uint8_t ch, uint8_t group);     
void sys_nvic_ex_config(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t tmode);    
void sys_gpio_af_set(GPIO_TypeDef *gpiox, uint16_t pinx, uint8_t afx);           
void sys_gpio_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint32_t mode, 
                  uint32_t otype, uint32_t ospeed, uint32_t pupd);               
void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status);     
uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx);                  
void sys_standby(void);          
void sys_soft_reset(void);       
void sys_cache_enable(void);     
uint8_t sys_clock_set(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);       
void sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);   
void sys_qspi_enable_memmapmode(uint8_t ftype);  

 
void sys_wfi_set(void);              
void sys_intx_disable(void);         
void sys_intx_enable(void);          
void sys_msr_msp(uint32_t addr);     













#line 30 "..\\..\\Drivers\\SYSTEM\\sys\\sys.c"







 
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
     
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}





 
static void sys_nvic_priority_group_config(uint8_t group)
{
    uint32_t temp, temp1;
    temp1 = (~group) & 0x07; 
    temp1 <<= 8;
    temp = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;       
    temp &= 0X0000F8FF;      
    temp |= 0X05FA0000;      
    temp |= temp1;
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = temp;       
}














 
void sys_nvic_init(uint8_t pprio, uint8_t sprio, uint8_t ch, uint8_t group)
{
    uint32_t temp;
    sys_nvic_priority_group_config(group);   
    temp = pprio << (4 - group);
    temp |= sprio & (0x0f >> group);
    temp &= 0xf;                             
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[ch / 32] |= 1 << (ch % 32);   
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[ch] |= temp << 4;               
}












 
void sys_nvic_ex_config(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t tmode)
{
    uint8_t offset;
    uint32_t gpio_num = 0;       
    uint32_t pinpos = 0, pos = 0, curpin = 0;

    gpio_num = ((uint32_t)p_gpiox - (uint32_t)((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0000UL))) / 0X400 ; 
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->APB4ENR |= 1 << 1;      

    for (pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;       
        curpin = pinx & pos;     

        if (curpin == pos)       
        {
            offset = (pinpos % 4) * 4;
            ((SYSCFG_TypeDef *) (((0x40000000UL) + 0x18000000UL) + 0x0400UL))->EXTICR[pinpos / 4] &= ~(0x000F << offset);   
            ((SYSCFG_TypeDef *) (((0x40000000UL) + 0x18000000UL) + 0x0400UL))->EXTICR[pinpos / 4] |= gpio_num << offset;    

            ((EXTI_Core_TypeDef *) ((((0x40000000UL) + 0x18000000UL) + 0x0000UL) + 0x0080UL))->IMR1 |= 1 << pinpos;    

            if (tmode & 0x01) ((EXTI_TypeDef *) (((0x40000000UL) + 0x18000000UL) + 0x0000UL))->FTSR1 |= 1 << pinpos;        
            if (tmode & 0x02) ((EXTI_TypeDef *) (((0x40000000UL) + 0x18000000UL) + 0x0000UL))->RTSR1 |= 1 << pinpos;        
        }
    }
}













 
void sys_gpio_af_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t afx)
{
    uint32_t pinpos = 0, pos = 0, curpin = 0;;

    for (pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;       
        curpin = pinx & pos;     

        if (curpin == pos)       
        {
            p_gpiox->AFR[pinpos >> 3] &= ~(0X0F << ((pinpos & 0X07) * 4));
            p_gpiox->AFR[pinpos >> 3] |= (uint32_t)afx << ((pinpos & 0X07) * 4);
        }
    }
}































 
void sys_gpio_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint32_t mode, uint32_t otype, uint32_t ospeed, uint32_t pupd)
{
    uint32_t pinpos = 0, pos = 0, curpin = 0;

    for (pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;       
        curpin = pinx & pos;     

        if (curpin == pos)       
        {
            p_gpiox->MODER &= ~(3 << (pinpos * 2));  
            p_gpiox->MODER |= mode << (pinpos * 2);  

            if ((mode == 0X01) || (mode == 0X02))    
            {
                p_gpiox->OSPEEDR &= ~(3 << (pinpos * 2));        
                p_gpiox->OSPEEDR |= (ospeed << (pinpos * 2));    
                p_gpiox->OTYPER &= ~(1 << pinpos) ;              
                p_gpiox->OTYPER |= otype << pinpos;              
            }

            p_gpiox->PUPDR &= ~(3 << (pinpos * 2));  
            p_gpiox->PUPDR |= pupd << (pinpos * 2);  
        }
    }
}










 
void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status)
{
    if (status & 0X01)
    {
        p_gpiox->BSRR |= pinx;               
    }
    else
    {
        p_gpiox->BSRR |= (uint32_t)pinx << 16;    
    }
}







 
uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx)
{
    if (p_gpiox->IDR & pinx)
    {
        return 1;    
    }
    else
    {
        return 0;    
    }
}





 
void sys_wfi_set(void)
{
    __asm volatile("wfi");
}





 
void sys_intx_disable(void)
{
    __asm volatile("cpsid i");
}





 
void sys_intx_enable(void)
{
    __asm volatile("cpsie i");
}






 
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);     
}





 
void sys_standby(void)
{
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->WKUPEPR &= ~(1 << 0);   
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->WKUPEPR |= 1 << 0;      
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->WKUPEPR &= ~(1 << 8);   
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->WKUPEPR &= ~(3 << 16);  
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->WKUPEPR |= 2 << 16;     
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->WKUPCR |= 0X3F << 0;    
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->CPUCR |= 7 << 0;        
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR |= 1 << 2;          
    sys_wfi_set();               
}





 
void sys_soft_reset(void)
{
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = 0X05FA0000 | (uint32_t)0x04;
}





 
void sys_cache_enable(void)
{
    SCB_EnableICache();  
    SCB_EnableDCache();  
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CACR |= 1 << 2; 
}
































 
uint8_t sys_clock_set(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    uint32_t retry = 0;
    uint8_t retval = 0;
    uint8_t swsval = 0;
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->CR3 &= ~(1 << 2);       
    ((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->D3CR |= 3 << 14;        
     
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->APB4ENR |= 1 << 1;      
    ((SYSCFG_TypeDef *) (((0x40000000UL) + 0x18000000UL) + 0x0400UL))->PWRCR |= 1 << 0;     

    while ((((PWR_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4800UL))->D3CR & (1 << 13)) == 0);    

    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR |= 1 << 16;  

    while (((((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR & (1 << 17)) == 0) && (retry < 0X7FFF))
    {
        retry++;         
    }

    if (retry == 0X7FFF)
    {
        retval = 1;      
    }
    else
    {
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCKSELR |= 2 << 0;            
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCKSELR |= pllm << 4;         
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL1DIVR |= (plln - 1) << 0;    
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL1DIVR |= (pllp - 1) << 9;    
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL1DIVR |= (pllq - 1) << 16;   
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL1DIVR |= 1 << 24;            
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 2 << 2;              
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 0 << 1;              
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 3 << 16;             
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR |= 1 << 24;                  
        retry = 0;

        while ((((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR & (1 << 25)) == 0)    
        {
            retry++;

            if (retry > 0X1FFFFF)
            {
                retval = 2;  
                break;
            }
        }

         
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCKSELR |= 8 << 12;           
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL2DIVR |= (440 - 1) << 0;     
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL2DIVR |= (2 - 1) << 9;       
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLL2DIVR |= (2 - 1) << 24;      
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 0 << 6;              
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 0 << 5;              
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 1 << 19;             
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR |= 1 << 21;             
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CCIPR &= ~(3 << 0);           
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CCIPR |= 2 << 0;              
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR |= 1 << 26;                  
        retry = 0;

        while ((((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR & (1 << 27)) == 0)   
        {
            retry++;

            if (retry > 0X1FFFFF)
            {
                retval = 3;  
                break;
            }
        }

        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CFGR |= 8 << 0;               
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CFGR |= 0 << 8;               
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CFGR |= 3 << 0;                 
        retry = 0;

        while (swsval != 3)                  
        {
            swsval = (((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CFGR & (7 << 3)) >> 3;    
            retry++;

            if (retry > 0X1FFFFF)
            {
                retval = 4;  
                break;
            }
        }

        ((FLASH_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x2000UL))->ACR |= 2 << 0;                
        ((FLASH_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x2000UL))->ACR |= 2 << 4;                
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CFGR |= 4 << 4;               
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D2CFGR |= 4 << 4;               
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D2CFGR |= 4 << 8;               
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D3CFGR |= 4 << 4;               
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR |= 1 << 7;                   
        ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->APB4ENR |= 1 << 1;              
        ((SYSCFG_TypeDef *) (((0x40000000UL) + 0x18000000UL) + 0x0400UL))->CCCSR |= 1 << 0;             
    }

    return retval;
}








 
void sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CR = 0x00000001;            
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CFGR = 0x00000000;          
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CFGR = 0x00000000;        
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D2CFGR = 0x00000000;        
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D3CFGR = 0x00000000;        
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCKSELR = 0x00000000;     
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->PLLCFGR = 0x00000000;       
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->CIER = 0x00000000;          

     
     
    *((volatile uint32_t *)0x51008108) = 0x00000001; 

    sys_clock_set(plln, pllm, pllp, pllq);   
    sys_qspi_enable_memmapmode(0);           
    sys_cache_enable();                      

     



    sys_nvic_set_vector_table((0x08000000UL), 0x0);

}








 
void sys_qspi_enable_memmapmode(uint8_t ftype)
{
    uint32_t tempreg = 0;
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->AHB4ENR |= 1 << 1;      
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->AHB4ENR |= 1 << 3;      
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->AHB4ENR |= 1 << 4;      
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->AHB3ENR |= 1 << 14;     

    sys_gpio_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0400UL)), 1<<2,
                 2, 0, 3, 1);  

    sys_gpio_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0400UL)), 1<<6,
                 2, 0, 3, 1);  

    sys_gpio_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0C00UL)), 1<<11 | 1<<12 | 1<<13,
                 2, 0, 3, 1);  

    sys_gpio_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x1000UL)), 1<<2,
                 2, 0, 3, 1);  

    sys_gpio_af_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0400UL)), 1<<2,  9);   
    sys_gpio_af_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0400UL)), 1<<6,  10);  
    sys_gpio_af_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0C00UL)), 1<<11, 9);   
    sys_gpio_af_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0C00UL)), 1<<12, 9);   
    sys_gpio_af_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x0C00UL)), 1<<13, 9);   
    sys_gpio_af_set(((GPIO_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x1000UL)), 1<<2,  9);   

     
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->AHB3RSTR |= 1 << 14;        
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->AHB3RSTR &= ~(1 << 14);     

    while (((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->SR & (1 << 5));  

    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CCIPR &= ~(3 << 4);       
    ((RCC_TypeDef *) (((0x40000000UL) + 0x18020000UL) + 0x4400UL))->D1CCIPR |= 2 << 4;          
    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->CR = 0X01000310;        
    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->DCR = 0X00180201;       
    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->CR |= 1 << 0;           

    








 

     
    while (((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->SR & (1 << 5));  

    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->CCR = 0X00000106;       

    while ((((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->SR & (1 << 1)) == 0); 

    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->FCR |= 1 << 1;          

     
    while (((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->SR & (1 << 5));  

    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->ABR = 0;                
    tempreg = 0XEB;                  
    tempreg |= 1 << 8;               
    tempreg |= 3 << 10;              
    tempreg |= (2 + ftype) << 12;    
    tempreg |= 3 << 14;              
    tempreg |= 0 << 16;              
    tempreg |= 4 << 18;              
    tempreg |= 3 << 24;              
    tempreg |= 3 << 26;              
    ((QUADSPI_TypeDef *) (((0x40000000UL) + 0x12000000UL) + 0x5000UL))->CCR = tempreg;          

     
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1 << 16);        
    ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL &= ~(1 << 0);          
    ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = 0;                    
    ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = 0X90000000;          
    ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0X0303002D;          
    ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = (1 << 2) | (1 << 0); 
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= 1 << 16;           
}

























