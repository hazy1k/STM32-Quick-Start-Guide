
#include "appplay_485.h"
#include "usb_app.h"

#define SEND_DATA  		"RS485 Send Data Test!"
#define RS485_TX_EN		PGout(8)					//485模式控制.0,接收;1,发送.

u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
u8 RS485_RX_CNT=0;   	//接收到的数据长度



//485检测app	
u8 RS485_Play(u8* caption)
{
	u8 rval=0;	 
	u16 sx=(lcddev.width-210)/2;
	u16 sy=(lcddev.height-180)/2;
	_window_obj* twin=0;	//窗体
	char strbuf[64]={0};
	u8 t=0,send_count=0,recv_count=0,res;
	
 	twin=window_creat(sx,sy,210,180,0,1<<6|1<<0,16);//创建窗口
	if(twin)
	{	
		twin->caption=caption;					 
	 	twin->windowbkc=APP_WIN_BACK_COLOR;	 								//窗体主色
		window_draw(twin);													//画窗体
	 	app_draw_smooth_line(sx+5,sy+32+1+44,200,2,0Xb1ffc4,0X1600b1);		//画彩线
		
		BACK_COLOR=APP_WIN_BACK_COLOR;
		gui_show_ptstr(sx+8,sy+32+1+3,  		sx+8+56,sy+32+1+3+16,   	0,BLACK,16,"Send:",1);
		memcpy(strbuf, SEND_DATA, sizeof(SEND_DATA));	strbuf[24] = 0;
		gui_show_ptstr(sx+8+16,sy+32+1+3+22,	sx+8+192,sy+32+1+3+22+16,	0,RED,16, (u8*)strbuf,1);
		gui_show_ptstr(sx+8,sy+32+1+3+44,		sx+8+56,sy+32+1+3+44+16,	0,BLACK,16,"Recv:",1);
		
 	}
	else rval=1;
	if(rval==0)
	{
		static u8 init_485=0;
		if(init_485==0)
			RS485_Init(115200);			//初始化RS485串口2
		init_485 = 1;	
		BACK_COLOR=APP_WIN_BACK_COLOR;	//背景色为窗体主色
		POINT_COLOR=RED;				//红色的字
		while(1)
		{
			if(system_task_return) break;//TPAD返回
			
			t++;
			if(t>50) //500ms
			{
				t=0;
				sprintf(strbuf,"%d  ", send_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3,   240,320,16, (u8*)strbuf);
				memcpy(strbuf, SEND_DATA, sizeof(SEND_DATA));
				RS485_Send_Data(SEND_DATA, sizeof(SEND_DATA)); 	//发送数据 
				memset(strbuf,0, 62);
			}
			
			RS485_Receive_Data((u8*)strbuf, &res);
			if(res) //RS485接收到有数据
			{
				LCD_Fill(sx+8+16,sy+32+1+3+66, 			sx+8+16+168,sy+32+1+3+66+64,  APP_WIN_BACK_COLOR);	//清除接收区	
				LCD_ShowString(sx+8+16,sy+32+1+3+66,   	168,64,16, (u8*)strbuf);
			
				sprintf(strbuf,"%d  ", recv_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3+44,    240,320,16, (u8*)strbuf);
			}
	//		delay_ms(10);
		}
	}
	window_delete(twin);
	return rval;
}


void RS485_Init(u32 bound)  //初始化IO 串口2  bound:波特率	
{  	 
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
  //串口2引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	//USART2    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2，PA3
	
	//PG8推挽输出，485模式控制  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PG8
	

   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
	USART_Cmd(USART2, ENABLE);  //使能串口 2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	RS485_TX_EN=0;				//默认为接收模式		
}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			//设置为发送模式
  	for(t=0;t<len;t++)		//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //等待发送结束		
    USART_SendData(USART2,buf[t]); //发送数据
	}	 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //等待发送结束		
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//设置为接收模式	
}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//记录本次数据长度
		RS485_RX_CNT=0;		//清零
	}
}

void USART2_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	 	
		res =USART_ReceiveData(USART2);//;读取接收到的数据USART2->DR
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//记录接收到的值
			RS485_RX_CNT++;						//接收数据增加1 
		} 
	}  		
} 
