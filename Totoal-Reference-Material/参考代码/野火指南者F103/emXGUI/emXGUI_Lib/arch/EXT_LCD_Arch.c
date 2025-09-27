
#include	"def.h"
#include	"emXGUI.h"
#include	"emXGUI_Arch.h"

/*=========================================================================================*/

#ifdef	EXT_LCD_ARCH

/*=========================================================================================*/

void EXT_LCD_WriteStart(U16 sx,U16 sy,U16 ex,U16 ey)
{
	__lcd_write_start(sx,sy,ex,ey);
}

/*===================================================================================*/

void EXT_LCD_ReadStart(U16 sx,U16 sy,U16 ex,U16 ey)
{
	__lcd_read_start(sx,sy,ex,ey);
}

/*=========================================================================================*/

static void __lcd_write_pixels(u16 c,int count)
{

#ifdef	EXT_LCD_DMA
	if(LCD_DMA_WritePixels(c,count))
	{
		return;
	}
#endif

	switch(count & 0x7)
	{
		case 1: count &= ~0x07; goto out1;
		case 2: count &= ~0x07; goto out2;
		case 3: count &= ~0x07; goto out3;
		case 4: count &= ~0x07; goto out4;
		case 5: count &= ~0x07; goto out5;
		case 6: count &= ~0x07; goto out6;
		case 7: count &= ~0x07; goto out7;

		default: break;
	}

	while(count > 0)
	{
		count -= 8;

		out8: __lcd_write_dat(c);
		out7: __lcd_write_dat(c);
		out6: __lcd_write_dat(c);
		out5: __lcd_write_dat(c);

		out4: __lcd_write_dat(c);
		out3: __lcd_write_dat(c);
		out2: __lcd_write_dat(c);
		out1: __lcd_write_dat(c);

	}
}

static void __lcd_write_bits(u16 *bits,int count)
{

#ifdef	EXT_LCD_DMA
	if(LCD_DMA_WriteBitmap(bits,count))
	{
		return;
	}
#endif

	switch(count & 0x7)
	{
		case 1: count &= ~0x07; goto out1;
		case 2: count &= ~0x07; goto out2;
		case 3: count &= ~0x07; goto out3;
		case 4: count &= ~0x07; goto out4;
		case 5: count &= ~0x07; goto out5;
		case 6: count &= ~0x07; goto out6;
		case 7: count &= ~0x07; goto out7;

		default: break;
	}

	while(count > 0)
	{
		count -= 8;

		out8: __lcd_write_dat(*bits++);
		out7: __lcd_write_dat(*bits++);
		out6: __lcd_write_dat(*bits++);
		out5: __lcd_write_dat(*bits++);

		out4: __lcd_write_dat(*bits++);
		out3: __lcd_write_dat(*bits++);
		out2: __lcd_write_dat(*bits++);
		out1: __lcd_write_dat(*bits++);

	}
}

/*=========================================================================================*/

void	EXT_LCD_WritePixel(U16 c)
{
	__lcd_write_dat(c);
}

/*===================================================================================*/

void EXT_LCD_WriteRGB(U8 r,U8 g,U8 b)
{
	__lcd_write_rgb(r,g,b);
}

/*===================================================================================*/

U16 EXT_LCD_ReadPixel(void)
{
	return	__lcd_read_pixel();
}

/*===================================================================================*/

void EXT_LCD_ReadRGB(U8 *r,U8 *g,U8 *b)
{
	__lcd_read_rgb(r,g,b);
}

/*===================================================================================*/

void	EXT_LCD_FillRect(U16 x,U16 y,U16 w,U16 h,U16 color)
{
	int count;

	__lcd_write_start(x,y,x+w-1,y+h-1);

	count =w*h;
	__lcd_write_pixels(color,count);

}

/*===================================================================================*/

void	EXT_LCD_ReadBits(U16 x,U16 y,U16 w,U16 h,U8 *buf,int width_bytes)
{
	int xx,yy;
	U16 *p;
	////

	xx=0;
	yy=0;
	for(yy=0;yy<h;yy++)
	{
		xx=0;

		p =(U16*)buf;
		for(xx=0;xx<w;xx++)
		{
			__lcd_read_start(x+xx,y+yy,x+w-1,y+h-1);

			*p++ =__lcd_read_pixel();
		}

		buf += width_bytes;
	}

}

/*===================================================================================*/

void	EXT_LCD_WriteBits(U16 x,U16 y,U16 w,U16 h,const U8 *bits,int width_bytes)
{
	int count;
	U16 *p;

	__lcd_write_start(x,y,x+w-1,y+h-1);

	if((width_bytes>>1) != w)
	{
		for(h=y+h;y<h;y++)
		{
			p =(U16*)bits;
			__lcd_write_bits(p,w);
			bits += width_bytes;
		}

	}
	else
	{
		p =(U16*)bits;
		count =w*h;
		__lcd_write_bits(p,count);
	}

}

/*===================================================================================*/

#endif  /* EXT_LCD_ARCH */

