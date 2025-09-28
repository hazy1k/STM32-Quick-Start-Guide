
#include "appplay_key.h"
#include "lsens.h"


//按键检测app	
u8 KEY_Play(u8* caption)
{
	u8 rval=0;
	u8 key;		 
	u16 sx=(lcddev.width-120)/2;
	u16 sy=(lcddev.height-146)/2;
	u8 key_num[4]={0,0,0,0};
	u8 t=0;
	
	_window_obj* twin=0;	//窗体
 	twin=window_creat(sx,sy,120,146,0,1<<6|1<<0,16);//创建窗口
	if(twin)
	{	
		twin->caption=caption;					 
	 	twin->windowbkc=APP_WIN_BACK_COLOR;	 								//窗体主色
		window_draw(twin);													//画窗体
	 	app_draw_smooth_line(sx+5,sy+32+1+22,110,1,0Xb1ffc4,0X1600b1);		//画彩线
	 	app_draw_smooth_line(sx+5,sy+32+1+44,110,1,0Xb1ffc4,0X1600b1);		//画彩线
		app_draw_smooth_line(sx+5,sy+32+1+66,110,1,0Xb1ffc4,0X1600b1);		//画彩线
		app_draw_smooth_line(sx+5,sy+32+1+88,110,1,0Xb1ffc4,0X1600b1);		//画彩线
		BACK_COLOR=APP_WIN_BACK_COLOR;
	
		gui_show_ptstr(sx+8,sy+32+1+3,   sx+8+56,sy+32+1+3+16,   0,BLACK,16," WKUP: 0 ",1);
		gui_show_ptstr(sx+8,sy+32+1+3+22,sx+8+56,sy+32+1+3+22+16,0,BLACK,16," KEY2: 0 ",1);
		gui_show_ptstr(sx+8,sy+32+1+3+44,sx+8+56,sy+32+1+3+44+16,0,BLACK,16," KEY1: 0 ",1);
		gui_show_ptstr(sx+8,sy+32+1+3+66,sx+8+56,sy+32+1+3+66+16,0,BLACK,16," KEY0: 0 ",1);
		gui_show_ptstr(sx+8,sy+32+1+3+88,sx+8+56,sy+32+1+3+88+16,0,BLUE,16,	" LENS:   ",1);
		
 	}else rval=1;
	if(rval==0)
	{
		BACK_COLOR=APP_WIN_BACK_COLOR;	//背景色为窗体主色
		POINT_COLOR=RED;				//红色的字
		while(1)
		{
			u8 str[8];
			
			t++;
			if(t>10)
			{
				t=0;
				key = Lsens_Get_Val();
				sprintf((char*)str,"%d%%  ", key);
				LCD_ShowString(sx+8+56,sy+32+1+3+88,240,320,16, str);
			}
			
			key=KEY_Scan(0);
			if(system_task_return) break;//TPAD返回
			switch(key)
			{
				case WKUP_PRES:	
				{
					sprintf((char*)str,"%d  ", key_num[0]++);
					LCD_ShowString(sx+8+56,sy+32+1+3,   240,320,16, str);
				} break;
				
				case KEY2_PRES:	
				{
					sprintf((char*)str,"%d  ", key_num[1]++);
					LCD_ShowString(sx+8+56,sy+32+1+3+22,240,320,16, str);
				} break;
				
				case KEY1_PRES:	
				{
					sprintf((char*)str,"%d  ", key_num[2]++);
					LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16, str);
				} break;
				
				case KEY0_PRES:	
				{
					sprintf((char*)str,"%d  ", key_num[3]++);
					LCD_ShowString(sx+8+56,sy+32+1+3+66,240,320,16, str);
				} break;
			}
			if(key!=0)  
			{
				BEEP=1;	delay_ms(100);	BEEP=0;	
			}
			delay_ms(10);
		}
	}
	window_delete(twin);
	return rval;
}

