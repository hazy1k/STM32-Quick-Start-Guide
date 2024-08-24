
#include "appplay_CAN.h"
#include "usb_app.h"


//CAN���app	
u8 CAN_Play(u8* caption)
{
	u8 rval=0;	 
	u16 sx=(lcddev.width-138)/2;
	u16 sy=(lcddev.height-180)/2;
	_window_obj* twin=0;	//����
	u8 strbuf[32]={0};
	u8 t=0,send_count=0,recv_count=0,res;
	
 	twin=window_creat(sx,sy,138,180,0,1<<6|1<<0,16);//��������
	if(twin)
	{	
		twin->caption=caption;					 
	 	twin->windowbkc=APP_WIN_BACK_COLOR;	 								//������ɫ
		window_draw(twin);													//������
	 	app_draw_smooth_line(sx+5,sy+32+1+44,123,2,0Xb1ffc4,0X1600b1);		//������
		
		BACK_COLOR=APP_WIN_BACK_COLOR;
		gui_show_ptstr(sx+8,sy+32+1+3,  		sx+8+56,sy+32+1+3+16,   	0,BLACK,16,"Send:",1);
		gui_show_ptstr(sx+8,sy+32+1+3+44,		sx+8+56,sy+32+1+3+44+16,	0,BLACK,16,"Recv:",1);	
 	}
	else rval=1;
	if(rval==0)
	{
		BACK_COLOR=APP_WIN_BACK_COLOR;	//����ɫΪ������ɫ
		POINT_COLOR=RED;				//��ɫ����
		
		OSTaskSuspend(3);   			//����watch_task
		usbapp_mode_stop();				//��ֹͣ��ǰUSB����ģʽ
		RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, DISABLE);	//ʹ��USB OTGʱ��
		
		CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal); 	 //CAN��ʼ����ͨģʽ,������500Kbps  
		
		while(1)
		{
			u8 i,str[6],sbuf[8];
			res=KEY_Scan(0);
			if(res) break;	//����
			
			t++;
			if(t>50) //500ms
			{
				t=0;  LED0 = !LED0;
				sprintf((char*)strbuf,"%d  ", send_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3,   240,320,16, strbuf);
				
				for(i=0;i<8;i++) sbuf[i] = send_count+i;
				res = CAN1_Send_Msg(sbuf,8);//����8���ֽ� 
				if(res)	memcpy(strbuf, "CAN Send Failed", sizeof("CAN Send Failed"));		//��ʾ����ʧ��
				else 	memcpy(strbuf, "CAN Send OK    ", sizeof("CAN Send OK    "));		//��ʾ���ͳɹ�
				gui_show_ptstr(sx+8+16,sy+32+1+3+22,	sx+8+192,sy+32+1+3+22+16,	0,RED,16, strbuf,1);
			}

			res = CAN1_Receive_Msg((u8*)sbuf);
			if(res) //CAN���յ�������
			{
				memset(strbuf,0, 30);
				for(i=0;i<res;i++)
				{
					sprintf((char*)str, "%0.2X ", sbuf[i]);
					strcat((char*)strbuf, (char*)str);
				}
				LCD_Fill(sx+8+16,sy+32+1+3+66, 			sx+8+16+96,sy+32+1+3+66+64,  APP_WIN_BACK_COLOR);	//���������	
				LCD_ShowString(sx+8+16,sy+32+1+3+66,   	96,64,16, strbuf);
				
				sprintf((char*)strbuf,"%d  ", recv_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3+44,    240,320,16, strbuf);
			}
			
			delay_ms(10);
		}
	}
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, DISABLE);//ʹ��CAN1ʱ��	
	usbapp_mode_set(0);  //����ΪU��ģʽ
	OSTaskResume(3);  //�ָ�watch_task 
	
	window_delete(twin);
	return rval;
}

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
  	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
  	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;

    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	return 0;
}   

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;	 // ��׼��ʶ��Ϊ0
	TxMessage.ExtId=0x12;	 // ������չ��ʾ����29λ��
	TxMessage.IDE=0;		  // ʹ����չ��ʶ��
	TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=len;							 // ������֡��Ϣ
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)return 1;
	return 0;		

}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

