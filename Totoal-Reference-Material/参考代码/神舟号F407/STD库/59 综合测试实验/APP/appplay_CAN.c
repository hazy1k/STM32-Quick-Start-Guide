
#include "appplay_CAN.h"
#include "usb_app.h"


//CAN检测app	
u8 CAN_Play(u8* caption)
{
	u8 rval=0;	 
	u16 sx=(lcddev.width-138)/2;
	u16 sy=(lcddev.height-180)/2;
	_window_obj* twin=0;	//窗体
	u8 strbuf[32]={0};
	u8 t=0,send_count=0,recv_count=0,res;
	
 	twin=window_creat(sx,sy,138,180,0,1<<6|1<<0,16);//创建窗口
	if(twin)
	{	
		twin->caption=caption;					 
	 	twin->windowbkc=APP_WIN_BACK_COLOR;	 								//窗体主色
		window_draw(twin);													//画窗体
	 	app_draw_smooth_line(sx+5,sy+32+1+44,123,2,0Xb1ffc4,0X1600b1);		//画彩线
		
		BACK_COLOR=APP_WIN_BACK_COLOR;
		gui_show_ptstr(sx+8,sy+32+1+3,  		sx+8+56,sy+32+1+3+16,   	0,BLACK,16,"Send:",1);
		gui_show_ptstr(sx+8,sy+32+1+3+44,		sx+8+56,sy+32+1+3+44+16,	0,BLACK,16,"Recv:",1);	
 	}
	else rval=1;
	if(rval==0)
	{
		BACK_COLOR=APP_WIN_BACK_COLOR;	//背景色为窗体主色
		POINT_COLOR=RED;				//红色的字
		
		OSTaskSuspend(3);   			//挂起watch_task
		usbapp_mode_stop();				//先停止当前USB工作模式
		RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, DISABLE);	//使能USB OTG时钟
		
		CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal); 	 //CAN初始化普通模式,波特率500Kbps  
		
		while(1)
		{
			u8 i,str[6],sbuf[8];
			res=KEY_Scan(0);
			if(res) break;	//返回
			
			t++;
			if(t>50) //500ms
			{
				t=0;  LED0 = !LED0;
				sprintf((char*)strbuf,"%d  ", send_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3,   240,320,16, strbuf);
				
				for(i=0;i<8;i++) sbuf[i] = send_count+i;
				res = CAN1_Send_Msg(sbuf,8);//发送8个字节 
				if(res)	memcpy(strbuf, "CAN Send Failed", sizeof("CAN Send Failed"));		//提示发送失败
				else 	memcpy(strbuf, "CAN Send OK    ", sizeof("CAN Send OK    "));		//提示发送成功
				gui_show_ptstr(sx+8+16,sy+32+1+3+22,	sx+8+192,sy+32+1+3+22+16,	0,RED,16, strbuf,1);
			}

			res = CAN1_Receive_Msg((u8*)sbuf);
			if(res) //CAN接收到有数据
			{
				memset(strbuf,0, 30);
				for(i=0;i<res;i++)
				{
					sprintf((char*)str, "%0.2X ", sbuf[i]);
					strcat((char*)strbuf, (char*)str);
				}
				LCD_Fill(sx+8+16,sy+32+1+3+66, 			sx+8+16+96,sy+32+1+3+66+64,  APP_WIN_BACK_COLOR);	//清除接收区	
				LCD_ShowString(sx+8+16,sy+32+1+3+66,   	96,64,16, strbuf);
				
				sprintf((char*)strbuf,"%d  ", recv_count++);
				LCD_ShowString(sx+8+48,sy+32+1+3+44,    240,320,16, strbuf);
			}
			
			delay_ms(10);
		}
	}
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, DISABLE);//使能CAN1时钟	
	usbapp_mode_set(0);  //设置为U盘模式
	OSTaskResume(3);  //恢复watch_task 
	
	window_delete(twin);
	return rval;
}

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
  	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
  	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;

    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
	return 0;
}   

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;	 // 标准标识符为0
	TxMessage.ExtId=0x12;	 // 设置扩展标示符（29位）
	TxMessage.IDE=0;		  // 使用扩展标识符
	TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
	TxMessage.DLC=len;							 // 发送两帧信息
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // 第一帧信息          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;		

}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

