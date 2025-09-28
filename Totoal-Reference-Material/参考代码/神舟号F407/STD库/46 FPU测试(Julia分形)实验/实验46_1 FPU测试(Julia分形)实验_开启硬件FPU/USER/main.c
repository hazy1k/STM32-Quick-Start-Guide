#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "timer.h" 

//ALIENTEK 探索者STM32F407开发板  实验46_1
////FPU测试(Julia分形) 实验  -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

//46_1,本版本为开启硬件FPU版本.

//FPU模式提示
#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On" 
#else
#define SCORE_FPU_MODE                  "FPU Off" 
#endif

#define 	ITERATION			128			//迭代次数
#define	 	REAL_CONSTANT		0.285f		//实部常量
#define	 	IMG_CONSTANT		0.01f		//虚部常量
//颜色表
u16 color_map[ITERATION];
//缩放因子列表
const u16 zoom_ratio[] =
{
	120, 110, 100, 150, 200, 275, 350, 450,
	600, 800, 1000, 1200, 1500, 2000, 1500,
	1200, 1000, 800, 600, 450, 350, 275, 200,
	150, 100, 110,
}; 

//初始化颜色表
//clut:颜色表指针
void InitCLUT(u16 * clut)
{
 	u32 i=0x00;
	u16  red=0,green=0,blue=0;  
	for(i=0;i<ITERATION;i++)//产生颜色表
	{
		//产生RGB颜色值
		red=(i*8*256/ITERATION)%256;
		green=(i*6*256/ITERATION)%256;
		blue=(i*4*256 /ITERATION)%256; 
 		//将RGB888,转换为RGB565
		red=red>>3;
		red=red<<11;
		green=green>>2;
		green=green<<5;
		blue=blue>>3; 
		clut[i]=red+green+blue;
	}
} 

//产生Julia分形图形
//size_x,size_y:屏幕x,y方向的尺寸
//offset_x,offset_y:屏幕x,y方向的偏移
//zoom:缩放因子
void GenerateJulia_fpu(u16 size_x,u16 size_y,u16 offset_x,u16 offset_y,u16 zoom)
{
	u8 i;
	u16 x,y;  
	float tmp1,tmp2;
	float num_real,num_img;
	float radius; 
	for(y=0;y<size_y;y++)
	{
       	for(x=0;x<size_x;x++)
   		{
       		num_real=y-offset_y;
         	num_real=num_real/zoom;
           	num_img=x-offset_x;
          	num_img=num_img/zoom;
           	i=0;
          	radius=0;
           	while((i<ITERATION-1)&&(radius<4))
           	{
           		tmp1=num_real*num_real;
               	tmp2=num_img*num_img;
              	num_img=2*num_real*num_img+IMG_CONSTANT;
             	num_real=tmp1-tmp2+REAL_CONSTANT;
               	radius=tmp1+tmp2;
              	i++;
           	}
			LCD->LCD_RAM=color_map[i];//绘制到屏幕 
       	}
	}
}
u8 timeout;

int main(void)
{ 
	u8 key;
	u8 i=0;
	u8 autorun=0;
	float time; 
	u8 buf[50];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	
 
	
	LED_Init();					//初始化LED
	KEY_Init();
 	LCD_Init();
 
	TIM3_Int_Init(65535,8400-1);//10Khz计数频率,最大计时6.5秒超出
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"FPU TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/7/2");	
	LCD_ShowString(30,130,200,16,16,"KEY0:+    KEY2:-");	//显示提示信息
	LCD_ShowString(30,150,200,16,16,"KEY_UP:AUTO/MANUL");	//显示提示信息	
	delay_ms(1200);
	POINT_COLOR=BLUE;	//设置字体为蓝色 
	InitCLUT(color_map);//初始化颜色表
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES:
				i++;
				if(i>sizeof(zoom_ratio)/2-1)i=0;//限制范围
				break;
			case KEY2_PRES:
				if(i)i--;
				else i=sizeof(zoom_ratio)/2-1;
				break;
			case WKUP_PRES:
				autorun=!autorun;	//自动/手动
				break;
		} 
		if(autorun==1)//自动时,自动设置缩放因子
		{
			i++;
			if(i>sizeof(zoom_ratio)/2-1)i=0;//限制范围
		}
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//设置窗口
		LCD_WriteRAM_Prepare();
		TIM3->CNT=0;//重设TIM3定时器的计数器值
		timeout=0;
		GenerateJulia_fpu(lcddev.width,lcddev.height,lcddev.width/2,lcddev.height/2,zoom_ratio[i]);
		time=TIM3->CNT+(u32)timeout*65536;
		sprintf((char*)buf,"%s: zoom:%d  runtime:%0.1fms\r\n",SCORE_FPU_MODE,zoom_ratio[i],time/10);
		LCD_ShowString(5,lcddev.height-5-12,lcddev.width-5,12,12,buf);//显示当前运行情况	 
		printf("%s",buf);//输出到串口
		LED0=!LED0;
	}
}
 
