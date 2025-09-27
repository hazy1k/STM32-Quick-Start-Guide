
#include	"BSP.h"
#include	"emXGUI.h"

#include	"GUI_Drv.h"

#include 	"stm32f10x_rcc.h"
#include 	"stm32f10x_gpio.h"
#include 	"stm32f10x_fsmc.h"

/*=========================================================================================*/
/*
 * 野火STM32F103 3.2寸LCD (ILI9341)
 */

//#define	LCD_CMD_ADDR	EXT_LCD_BASE
//#define	LCD_DAT_ADDR	(EXT_LCD_BASE+(0x00020000))
#define	LCD_CMD_ADDR	( ( uint32_t ) 0x60000000 )
#define	LCD_DAT_ADDR	( ( uint32_t ) 0x60020000 )

#define	LCD_CMD	(*(volatile u16*)(LCD_CMD_ADDR))
#define	LCD_DAT	(*(volatile u16*)(LCD_DAT_ADDR))


/*=========================================================================================*/

static void delay_ms(unsigned int t)
{
	volatile int i;
	while(t-- > 0)
	{
		for(i=200;i>0;i--);
	}
}

static	INLINE void write_cmd(u8 a)
{
	LCD_CMD =a;
}

static	INLINE void write_dat(u8 a)
{
 	LCD_DAT =a;
}

/*=========================================================================================*/

#define	__lcd_write_cmd(cmd)    LCD_CMD =cmd
#define	__lcd_write_dat(dat) 	LCD_DAT =dat
#define	__lcd_read_dat()		LCD_DAT

#define	__lcd_map_rgb(r,g,b)	RGB565(r>>3,g>>2,b>>3)


void __lcd_write_start(u16 sx,u16 sy,u16 ex,u16 ey)
{
	__lcd_write_cmd(0x2A);
	__lcd_write_dat(sx>>8);
	__lcd_write_dat(sx);
	__lcd_write_dat(ex>>8);
	__lcd_write_dat(ex);

	__lcd_write_cmd(0x2B);
	__lcd_write_dat(sy>>8);
	__lcd_write_dat(sy);
	__lcd_write_dat(ey>>8);
	__lcd_write_dat(ey);

	__lcd_write_cmd(0x2C);
}

void __lcd_read_start(u16 sx,u16 sy,u16 ex,u16 ey)
{
	volatile u16 c;

	__lcd_write_cmd(0x2A);
	__lcd_write_dat(sx>>8);
	__lcd_write_dat(sx);
	__lcd_write_dat(ex>>8);
	__lcd_write_dat(ex);

	__lcd_write_cmd(0x2B);
	__lcd_write_dat(sy>>8);
	__lcd_write_dat(sy);
	__lcd_write_dat(ey>>8);
	__lcd_write_dat(ey);

	__lcd_write_cmd(0x2E);
	c =__lcd_read_dat(); //DummyRead
	for(c=0;c<2;c++);
}

void __lcd_write_rgb(U8 r,U8 g,U8 b)
{
	__lcd_write_dat(__lcd_map_rgb(r,g,b));
}

u16 __lcd_read_pixel(void)
{

	u16 c1,c2;

	c1 =__lcd_read_dat(); //RG
	c2 =__lcd_read_dat(); //BX

	return	__lcd_map_rgb(c1>>8,c1,c2>>8);
}

void __lcd_read_rgb(u8 *r,u8 *g,u8 *b)
{
	u16 c1,c2;

	c1 =__lcd_read_dat(); //RG
	c2 =__lcd_read_dat(); //BX

	*r =c1>>8;
	*g =c1;
	*b =c2>>8;
}

/*=========================================================================================*/

BOOL LCD_SetRotate(int rotate)
{

	switch(rotate)
	{
		case ROTATE_0:
			write_cmd(0x36); // Memory Access Control
    write_dat( 0x08 |(3<<5));//write_dat((0<<7)|(1<<6)|(0<<5)|(1<<3)|(0<<2));	// 0
			return TRUE;
			////

		case ROTATE_90:
			write_cmd(0x36); // Memory Access Control
			write_dat((0<<7)|(0<<6)|(1<<5)|(1<<3)|(0<<2));	// 90
			return TRUE;
			////

		case ROTATE_180:
			write_cmd(0x36); // Memory Access Control
			write_dat((1<<7)|(0<<6)|(0<<5)|(1<<3)|(0<<2));	// 180
			return TRUE;
			////

		case ROTATE_270:
			write_cmd(0x36); // Memory Access Control
			write_dat((1<<7)|(1<<6)|(1<<5)|(1<<3)|(0<<2));	// 270
			return TRUE;
			////

		default:
			return FALSE;

	}
}

/*=========================================================================================*/


static void macDEBUG_DELAY(void)
{
	delay_ms(50);
}

static void ILI9341_Init ( void )
{

#define	ILI9341_Write_Cmd write_cmd
#define	ILI9341_Write_Data	write_dat


	/******************************* ¶¨Òå ILI934 ³£ÓÃÃüÁî ********************************/
	#define      macCMD_SetCoordinateX		 		    0x2A	     //ÉèÖÃX×ø±ê
	#define      macCMD_SetCoordinateY		 		    0x2B	     //ÉèÖÃY×ø±ê
	#define      macCMD_SetPixel		 		        0x2C	     //Ìî³äÏñËØ

	/*  Power control B (CFh)  */
	macDEBUG_DELAY  ();
	ILI9341_Write_Cmd ( 0xCF  );
	ILI9341_Write_Data ( 0x00  );
	ILI9341_Write_Data ( 0x81  );
	ILI9341_Write_Data ( 0x30  );

	/*  Power on sequence control (EDh) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xED );
	ILI9341_Write_Data ( 0x64 );
	ILI9341_Write_Data ( 0x03 );
	ILI9341_Write_Data ( 0x12 );
	ILI9341_Write_Data ( 0x81 );

	/*  Driver timing control A (E8h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xE8 );
	ILI9341_Write_Data ( 0x85 );
	ILI9341_Write_Data ( 0x10 );
	ILI9341_Write_Data ( 0x78 );

	/*  Power control A (CBh) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xCB );
	ILI9341_Write_Data ( 0x39 );
	ILI9341_Write_Data ( 0x2C );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x34 );
	ILI9341_Write_Data ( 0x02 );

	/* Pump ratio control (F7h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xF7 );
	ILI9341_Write_Data ( 0x20 );

	/* Driver timing control B */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xEA );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );


	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xB1 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x1B );

	/*  Display Function Control (B6h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xB6 );
	ILI9341_Write_Data ( 0x0A );
	ILI9341_Write_Data ( 0xA2 );


	/* Power Control 1 (C0h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xC0 );
	ILI9341_Write_Data ( 0x25 ); //0x35

	/* Power Control 2 (C1h) */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0xC1 );
	ILI9341_Write_Data ( 0x10 ); //0x11

	/* VCOM Control 1 (C5h) */
	ILI9341_Write_Cmd ( 0xC5 );
	ILI9341_Write_Data ( 0x45 );
	ILI9341_Write_Data ( 0x45 );

	/*  VCOM Control 2 (C7h)  */
	ILI9341_Write_Cmd ( 0xC7 );
	ILI9341_Write_Data ( 0xA2 );

	/* Enable 3G (F2h) */
	ILI9341_Write_Cmd ( 0xF2 );
	ILI9341_Write_Data ( 0x00 );

	/* Gamma Set (26h) */
	ILI9341_Write_Cmd ( 0x26 );
	ILI9341_Write_Data ( 0x01 );
	macDEBUG_DELAY ();

	/* Positive Gamma Correction */
	ILI9341_Write_Cmd ( 0xE0 ); //Set Gamma
	ILI9341_Write_Data ( 0x0F );
	ILI9341_Write_Data ( 0x26 );
	ILI9341_Write_Data ( 0x24 );
	ILI9341_Write_Data ( 0x0B );
	ILI9341_Write_Data ( 0x0E );
	ILI9341_Write_Data ( 0x09 );
	ILI9341_Write_Data ( 0x54 );
	ILI9341_Write_Data ( 0xA8 );
	ILI9341_Write_Data ( 0x46 );
	ILI9341_Write_Data ( 0x0C );
	ILI9341_Write_Data ( 0x17 );
	ILI9341_Write_Data ( 0x09 );
	ILI9341_Write_Data ( 0x0F );
	ILI9341_Write_Data ( 0x07 );
	ILI9341_Write_Data ( 0x00 );

	/* Negative Gamma Correction (E1h) */
	ILI9341_Write_Cmd ( 0XE1 ); //Set Gamma
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x19 );
	ILI9341_Write_Data ( 0x1B );
	ILI9341_Write_Data ( 0x04 );
	ILI9341_Write_Data ( 0x10 );
	ILI9341_Write_Data ( 0x07 );
	ILI9341_Write_Data ( 0x2A );
	ILI9341_Write_Data ( 0x47 );
	ILI9341_Write_Data ( 0x39 );
	ILI9341_Write_Data ( 0x03 );
	ILI9341_Write_Data ( 0x06 );
	ILI9341_Write_Data ( 0x06 );
	ILI9341_Write_Data ( 0x30 );
	ILI9341_Write_Data ( 0x38 );
	ILI9341_Write_Data ( 0x0F );

	/* memory access control set */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0x36 );
	ILI9341_Write_Data ( 0xC8 );    /*ÊúÆÁ  ×óÉÏ½Çµ½ (Æðµã)µ½ÓÒÏÂ½Ç (ÖÕµã)É¨Ãè·½Ê½*/
	macDEBUG_DELAY ();

	/* column address control set */
	ILI9341_Write_Cmd ( macCMD_SetCoordinateX );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0xEF );

	/* page address control set */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( macCMD_SetCoordinateY );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x01 );
	ILI9341_Write_Data ( 0x3F );

	/*  Pixel Format Set (3Ah)  */
	macDEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0x3a );
	ILI9341_Write_Data ( 0x55 );

	/* Sleep Out (11h)  */
	ILI9341_Write_Cmd ( 0x11 );
	//ILI9341_Delay ( 0xAFFf<<2 );
	macDEBUG_DELAY ();

	/* Display ON (29h) */
	ILI9341_Write_Cmd ( 0x29 );
}

/*=========================================================================================*/

#if 1

#define		EXT_LCD_ARCH
//#define		EXT_LCD_DMA
#include 	"EXT_LCD_Arch.c"
//#ifdef	EXT_LCD_ARCH

#endif

#define	LCD_RST_PIN	GPIOE,GPIO_Pin_1
#define	LCD_BL_PIN	GPIOD,GPIO_Pin_12

/*=========================================================================================*/


void	LCD_Init(void)
{
  GPIO_InitTypeDef gpio_init;

	gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP; //复用输出
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz; //50MHz

	//PG6 - LCD BackLight: 0:ON; 1:OFF
	gpio_init.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOD, &gpio_init); //初始化

	//PG11 - LCD_nRST:
	gpio_init.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOE, &gpio_init); //初始化

	GPIO_SetBits(LCD_RST_PIN);
	GUI_msleep(50);
	GPIO_ResetBits(LCD_RST_PIN);
	GUI_msleep(50);
	GPIO_SetBits(LCD_RST_PIN);
	GUI_msleep(50);
  
	ILI9341_Init();
	LCD_SetRotate(LCD_ROTATE);

#ifdef	EXT_LCD_DMA
	LCD_DMA_Init(LCD_DAT_ADDR);
#endif

}

void LCD_BackLed_Control(int on)
{
	if(on)
	{
		GPIO_ResetBits(LCD_BL_PIN);
	}
	else
	{
		GPIO_SetBits(LCD_BL_PIN);
	}
}

/*=========================================================================================*/
