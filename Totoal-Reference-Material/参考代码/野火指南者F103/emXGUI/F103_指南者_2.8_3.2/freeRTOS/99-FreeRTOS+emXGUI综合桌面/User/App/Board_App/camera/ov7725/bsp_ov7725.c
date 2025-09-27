#include "bsp_ov7725.h"
#include "bsp_sccb.h"
#include "board.h"
//#include "bsp_ili9341_lcd.h"

typedef struct Reg
{
	uint8_t Address;			       /*寄存器地址*/
	uint8_t Value;		           /*寄存器值*/
}Reg_Info;

/* 寄存器参数配置 */
const Reg_Info Sensor_Config[] =
{
	{CLKRC,     0x00}, /*clock config*/
	{COM7,      0x46}, /*QVGA RGB565 */
	{HSTART,    0x3f},//0x17
	{HSIZE,     0x50},//0x18//{HSIZE,     0x50},//0x18
	{VSTART,    0x03},//0x19
	{VSIZE,     0x78},//0x1A
	{HREF,      0x00},//0x32
	{HOutSize,  0x50},//0x29
	{VOutSize,  0x78},//0x2C
	{EXHCH,     0x00},//0x2A

	/*DSP control*/
	{TGT_B,     0x7f},
	{FixGain,   0x09},
	{AWB_Ctrl0, 0xe0},
	{DSP_Ctrl1, 0xff},
	{DSP_Ctrl2, 0x20},
	{DSP_Ctrl3,	0x00},
	{DSP_Ctrl4, 0x00},

	/*AGC AEC AWB*/
	{COM8,		  0xf0},
	{COM4,		  0x81}, /*Pll AEC CONFIG*/
	{COM6,		  0xc5},
	{COM9,		  0x21},
	{BDBase,	  0xFF},
	{BDMStep,	  0x01},
	{AEW,		    0x34},
	{AEB,		    0x3c},
	{VPT,		    0xa1},
	{EXHCL,		  0x00},
	{AWBCtrl3,  0xaa},
	{COM8,		  0xff},
	{AWBCtrl1,  0x5d},

	{EDGE1,		  0x0a},
	{DNSOff,	  0x01},
	{EDGE2,		  0x01},
	{EDGE3,		  0x01},

	{MTX1,		  0x5f},
	{MTX2,		  0x53},
	{MTX3,		  0x11},
	{MTX4,		  0x1a},
	{MTX5,		  0x3d},
	{MTX6,		  0x5a},
	{MTX_Ctrl,  0x1e},

	{BRIGHT,	  0x00},
	{CNST,		  0x25},
	{USAT,		  0x65},
	{VSAT,		  0x65},
	{UVADJ0,	  0x81},
	{SDE,		    0x06},
	
    /*GAMMA config*/
	{GAM1,		  0x0c},
	{GAM2,		  0x16},
	{GAM3,		  0x2a},
	{GAM4,		  0x4e},
	{GAM5,		  0x61},
	{GAM6,		  0x6f},
	{GAM7,		  0x7b},
	{GAM8,		  0x86},
	{GAM9,		  0x8e},
	{GAM10,		  0x97},
	{GAM11,		  0xa4},
	{GAM12,		  0xaf},
	{GAM13,		  0xc5},
	{GAM14,		  0xd7},
	{GAM15,		  0xe8},
	{SLOP,		  0x20},

	{HUECOS,	  0x80},
	{HUESIN,	  0x80},
	{DSPAuto,	  0xff},
	{DM_LNL,	  0x00},
	{BDBase,	  0x99},
	{BDMStep,	  0x03},
	{LC_RADI,	  0x00},
	{LC_COEF,	  0x13},
	{LC_XC,		  0x08},
	{LC_COEFB,  0x14},
	{LC_COEFR,  0x17},
	{LC_CTR,	  0x05},
	
	{COM3,		  0xd0},/*Horizontal mirror image*/

	/*night mode auto frame rate control*/
	{COM5,		0xf5},	 /*在夜视环境下，自动降低帧率，保证低照度画面质量*/
	//{COM5,		0x31},	/*夜视环境帧率不变*/
};

u8 OV7725_REG_NUM = sizeof(Sensor_Config)/sizeof(Sensor_Config[0]);	  /*结构体数组成员数目*/

uint8_t Ov7725_vsync = 0;	 /* 帧同步信号标志，在中断函数和main函数里面使用 */


/************************************************
 * 函数名：FIFO_GPIO_Config
 * 描述  ：FIFO GPIO配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static void FIFO_GPIO_Config(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	
		/*开启时钟*/
	  RCC_APB2PeriphClockCmd (OV7725_OE_GPIO_CLK|OV7725_WRST_GPIO_CLK|
															OV7725_RRST_GPIO_CLK|OV7725_RCLK_GPIO_CLK|
															OV7725_WE_GPIO_CLK|OV7725_DATA_GPIO_CLK, ENABLE );
	
		/*(FIFO_OE--FIFO输出使能)*/
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
		GPIO_InitStructure.GPIO_Pin = OV7725_OE_GPIO_PIN;
		GPIO_Init(OV7725_OE_GPIO_PORT, &GPIO_InitStructure);
	
			/*(FIFO_WRST--FIFO写复位)*/
		GPIO_InitStructure.GPIO_Pin = OV7725_WRST_GPIO_PIN;
		GPIO_Init(OV7725_WRST_GPIO_PORT, &GPIO_InitStructure);
	
			/*(FIFO_RRST--FIFO读复位) */
		GPIO_InitStructure.GPIO_Pin = OV7725_RRST_GPIO_PIN;
		GPIO_Init(OV7725_RRST_GPIO_PORT, &GPIO_InitStructure);
		
		/*(FIFO_RCLK-FIFO读时钟)*/
		GPIO_InitStructure.GPIO_Pin = OV7725_RCLK_GPIO_PIN;
		GPIO_Init(OV7725_RCLK_GPIO_PORT, &GPIO_InitStructure);

		/*(FIFO_WE--FIFO写使能)*/
		GPIO_InitStructure.GPIO_Pin = OV7725_WE_GPIO_PIN;	
		GPIO_Init(OV7725_WE_GPIO_PORT, &GPIO_InitStructure);
	

    /*(FIFO_DATA--FIFO输出数据)*/
		GPIO_InitStructure.GPIO_Pin = 	OV7725_DATA_0_GPIO_PIN | OV7725_DATA_1_GPIO_PIN |
																			OV7725_DATA_2_GPIO_PIN | OV7725_DATA_3_GPIO_PIN |
																			OV7725_DATA_4_GPIO_PIN | 	OV7725_DATA_5_GPIO_PIN |
																			OV7725_DATA_6_GPIO_PIN | 	OV7725_DATA_7_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(OV7725_DATA_GPIO_PORT, &GPIO_InitStructure);
		
		
    FIFO_OE_L();	  					/*拉低使FIFO输出使能*/
    FIFO_WE_H();   						/*拉高使FIFO写允许*/

}

void Ov7725_GPIO_Config(void)
{
	SCCB_GPIO_Config();
	FIFO_GPIO_Config();
}

/************************************************
 * 函数名：VSYNC_GPIO_Configuration
 * 描述  ：OV7725 GPIO配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static void VSYNC_GPIO_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;	
	
	  RCC_APB2PeriphClockCmd( OV7725_VSYNC_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE );	  /*PA0---VSYNC*/
    GPIO_InitStructure.GPIO_Pin =  OV7725_VSYNC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(OV7725_VSYNC_GPIO_PORT, &GPIO_InitStructure);
}

/************************************************
 * 函数名：VSYNC_NVIC_Configuration
 * 描述  ：VSYNC中断配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static void VSYNC_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = OV7725_VSYNC_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/************************************************
 * 函数名：VSYNC_EXTI_Configuration
 * 描述  ：OV7725 VSYNC中断管脚配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
/*               ___                            ___
 * VSYNC:     __|   |__________________________|   |__     
 */
static void VSYNC_EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
	
		GPIO_EXTILineConfig(OV7725_VSYNC_EXTI_SOURCE_PORT, OV7725_VSYNC_EXTI_SOURCE_PIN);
    EXTI_InitStructure.EXTI_Line = OV7725_VSYNC_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ; /*上升沿触发*/
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_GenerateSWInterrupt(OV7725_VSYNC_EXTI_LINE);	/*中断挂到 EXTI_Line0  线*/
}

/************************************************
 * 函数名：VSYNC_Init
 * 描述  ：OV7725 VSYNC中断相关配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
void VSYNC_Init(void)
{
    VSYNC_GPIO_Configuration();
    VSYNC_EXTI_Configuration();
    VSYNC_NVIC_Configuration();
}

/************************************************
 * 函数名：Sensor_Init
 * 描述  ：Sensor初始化
 * 输入  ：无
 * 输出  ：返回1成功，返回0失败
 * 注意  ：无
 ************************************************/
ErrorStatus Ov7725_Init(void)
{
	uint16_t i = 0;
	uint8_t Sensor_IDCode = 0;	
	
	//DEBUG("ov7725 Register Config Start......");
	
	if( 0 == SCCB_WriteByte ( 0x12, 0x80 ) ) /*复位sensor */
	{
		//DEBUG("sccb write data error");		
		return ERROR ;
	}	

	if( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, 0x0b ) )	 /* 读取sensor ID号*/
	{
		//DEBUG("read id faild");		
		return ERROR;
	}
	//DEBUG("Sensor ID is 0x%x", Sensor_IDCode);	
	
	if(Sensor_IDCode == OV7725_ID)
	{
		for( i = 0 ; i < OV7725_REG_NUM ; i++ )
		{
			if( 0 == SCCB_WriteByte(Sensor_Config[i].Address, Sensor_Config[i].Value) )
			{                
				//DEBUG("write reg faild", Sensor_Config[i].Address);
				return ERROR;
			}
		}
	}
	else
	{
		return ERROR;
	}
	//DEBUG("ov7725 Register Config Success");
	
	return SUCCESS;
}

void discameraexit(void)
{
	  NVIC_InitTypeDef NVIC_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;
	
	  FIFO_OE_H();	  				
    FIFO_WE_L();   						
    EXTI_InitStructure.EXTI_Line = OV7725_VSYNC_EXTI_LINE;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = OV7725_VSYNC_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void DisableCamera(void)
{
	  NVIC_InitTypeDef NVIC_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		SCL_H;
		SDA_H;		
	    						
    EXTI_InitStructure.EXTI_Line = OV7725_VSYNC_EXTI_LINE;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = OV7725_VSYNC_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
		
		FIFO_WRST_L();
		FIFO_RRST_L();
		FIFO_RCLK_H();			  				
    FIFO_WE_H(); 
		FIFO_OE_H();
	
		/*PB8-PB15(FIFO_DATA--FIFO输出数据)*/
		RCC_APB2PeriphClockCmd ( OV7725_DATA_GPIO_CLK, ENABLE );

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
		                              GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
		GPIO_Init(OV7725_DATA_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
		GPIO_Init(OV7725_DATA_GPIO_PORT, &GPIO_InitStructure);

		RCC_APB2PeriphClockCmd ( OV7725_VSYNC_GPIO_CLK, ENABLE );	  /*PA0---VSYNC*/

    GPIO_InitStructure.GPIO_Pin =  OV7725_VSYNC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(OV7725_VSYNC_GPIO_PORT, &GPIO_InitStructure);
}

/************************************* ov7725 场中断 服务程序 *****************************************************/

extern GUI_SEM *cam_sem;//更新图像同步信号量（二值型）

void OV7725_VSYNC_EXTI_INT_FUNCTION ( void )
{
  uint32_t ulReturn;
  
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
	if ( EXTI_GetITStatus(OV7725_VSYNC_EXTI_LINE) != RESET ) 	//检查EXTI_Line0线路上的中断请求是否发送到了NVIC 
	{
			if( Ov7725_vsync == 0 )
			{
					FIFO_WRST_L(); 	                      //拉低使FIFO写(数据from摄像头)指针复位
					FIFO_WE_H();	                        //拉高使FIFO写允许
					
					Ov7725_vsync = 1;	   	
					FIFO_WE_H();                          //使FIFO写允许
					FIFO_WRST_H();                        //允许使FIFO写(数据from摄像头)指针运动
			}
			else if( Ov7725_vsync == 1 )
			{
					FIFO_WE_L();                          //拉低使FIFO写暂停
          Ov7725_vsync = 2;
					GUI_SemPostISR(cam_sem);
			}
			EXTI_ClearITPendingBit(OV7725_VSYNC_EXTI_LINE);		    //清除EXTI_Line0线路挂起标志位
	}    
	
 /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

