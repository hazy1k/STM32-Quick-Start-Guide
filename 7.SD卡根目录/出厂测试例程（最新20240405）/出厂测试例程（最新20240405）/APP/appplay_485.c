
#include "appplay_485.h"
#include "usb_app.h"

#define SEND_DATA  		"RS485 Send Data Test!"
#define RS485_TX_EN		PGout(8)					//485ģʽ����.0,����;1,����.

u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
u8 RS485_RX_CNT=0;   	//���յ������ݳ���



//485���app	
u8 RS485_Play(u8* caption)
{
	u8 rval=0;	 
	u16 sx=(lcddev.width-210)/2;
	u16 sy=(lcddev.height-180)/2;
	_window_obj* twin=0;	//����
	char strbuf[64]={0};
	u8 t=0,send_count=0,recv_count=0,res;
	
 	twin=window_creat(sx,sy,210,180,0,1<<6|1<<0,16);//��������
	if(twin)
	{	
		twin->caption=caption;					 
	 	twin->windowbkc=APP_WIN_BACK_COLOR;	 								//������ɫ
		window_draw(twin);													//������
	 	app_draw_smooth_line(sx+5,sy+32+1+44,200,2,0Xb1ffc4,0X1600b1);		//������
		
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
			RS485_Init(115200);			//��ʼ��RS485����2
		init_485 = 1;	
		BACK_COLOR=APP_WIN_BACK_COLOR;	//����ɫΪ������ɫ
		POINT_COLOR=RED;				//��ɫ����
		while(1)
		{
			if(system_task_return) break;//TPAD����
			
			t++;
			if(t>50) //500ms
			{
				t=0;
				sprintf(strbuf,"%d  ", send_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3,   240,320,16, (u8*)strbuf);
				memcpy(strbuf, SEND_DATA, sizeof(SEND_DATA));
				RS485_Send_Data(SEND_DATA, sizeof(SEND_DATA)); 	//�������� 
				memset(strbuf,0, 62);
			}
			
			RS485_Receive_Data((u8*)strbuf, &res);
			if(res) //RS485���յ�������
			{
				LCD_Fill(sx+8+16,sy+32+1+3+66, 			sx+8+16+168,sy+32+1+3+66+64,  APP_WIN_BACK_COLOR);	//���������	
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


void RS485_Init(u32 bound)  //��ʼ��IO ����2  bound:������	
{  	 
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	
  //����2���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3
	
	//PG8���������485ģʽ����  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��PG8
	

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ��� 2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	RS485_TX_EN=0;				//Ĭ��Ϊ����ģʽ		
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			//����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //�ȴ����ͽ���		
    USART_SendData(USART2,buf[t]); //��������
	}	 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //�ȴ����ͽ���		
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
	}
}

void USART2_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 	
		res =USART_ReceiveData(USART2);//;��ȡ���յ�������USART2->DR
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
			RS485_RX_CNT++;						//������������1 
		} 
	}  		
} 
