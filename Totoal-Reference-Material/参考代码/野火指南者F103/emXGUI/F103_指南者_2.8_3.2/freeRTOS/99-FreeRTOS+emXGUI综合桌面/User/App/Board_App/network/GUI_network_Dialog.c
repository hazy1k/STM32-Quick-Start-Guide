#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include "x_libc.h"
#include <stdlib.h>
#include "GUI_AppDef.h"
#include "./network/GUI_network_Dialog.h"
#include "./network/W5500/w5500_conf.h"
#include "./network/W5500/w5500.h"
#include "./network/W5500/socket.h"

int		number_input_box(int x, int y, int w, int h,
							const WCHAR *pCaption,
							WCHAR *pOut,
							int MaxCount,
							HWND hwndParent);
              
/* 单选框 ID */
#define ID_RB1    (0x1100 | (1<<16))
#define ID_RB2    (0x1101 | (1<<16))
#define ID_RB3    (0x1102 | (1<<16))

TaskHandle_t network_task_handle;

int8_t NetworkTypeSelection = 0;

static HWND Receive_Handle;

static uint8_t netmode=0;    // 0:TCP Clent 1:TCP Server 2:UDP
static uint8_t IsConnect=0;
static uint8_t IsNetInit=0;
static uint8_t comdata[512];
static uint8_t link_flag = 0;

#define TRYCOUNT					10

//eg:text: 192.168.1.10 --> ip[0]=192,ip[1]=168,ip[2]=1,ip[3]=10.
static uint8_t TextToIP(uint8_t *text,uint8_t *ip)
{
	uint8_t i,j,tmp[3]={0};
	int8_t dot[5]={0};
	i=0;
	j=0;
	while((i<15)&&(text[i]!='\0'))
	{
		if(!((text[i]>='0'&&text[i]<='9')||(text[i]=='.')))
			return 1;
		if(text[i]=='.')
		{				
			++j;	
			dot[j]=i;				
			if(j>3||i==0)return 1;
		}			
		++i;		
	}
	if(j<3) return 1;
	dot[4]=i;
	if(((dot[2]-dot[1])<=1)||((dot[3]-dot[2])<=1)||((dot[4]-dot[3])<=1)) return 1;	
	dot[0]=-1;
	for(i=0;i<4;++i)
	{
		j=dot[i+1]-dot[i]-1;
		if(j==1){tmp[0]='0';tmp[1]='0';tmp[2]=text[dot[i]+1];}
		else if(j==2){tmp[0]='0';tmp[1]=text[dot[i]+1];tmp[2]=text[dot[i]+2];}
		else if(j==3){tmp[0]=text[dot[i]+1];tmp[1]=text[dot[i]+2];tmp[2]=text[dot[i]+3];}
		ip[i]=100*(tmp[0]-'0')+10*(tmp[1]-'0')+tmp[2]-'0';
	}
	return 0;
}

/* 设置接收窗口的文本 */
void receive_win_set(const char *data, uint32_t rec_len)
{
  uint32_t WinTexeLen = 0;
  WCHAR *wbuf;
  WCHAR *wbuf_rec;
  
  WinTexeLen = GetWindowTextLength(Receive_Handle) + 1;                       // 获取文本长度
  wbuf = (WCHAR *)GUI_VMEM_Alloc(WinTexeLen*sizeof(WCHAR) + rec_len*sizeof(WCHAR));    // 申请文本长度 + 新消息长度的内存
  wbuf_rec =  (WCHAR *)GUI_VMEM_Alloc((rec_len + 1)*sizeof(WCHAR));    // 申请文本长度 + 新消息长度的内存

  if (wbuf != NULL)
  {
    if (wbuf_rec != NULL)
    {
      GetWindowText(Receive_Handle, wbuf, WinTexeLen+1);                      // 得到原文本
      x_mbstowcs(wbuf_rec, data, rec_len*
      sizeof(WCHAR));
      x_wstrcat(wbuf, wbuf_rec);                                              // 追加新文本
      SetWindowText(Receive_Handle, wbuf);                                    // 设置接收窗口的文本
      GUI_VMEM_Free(wbuf_rec);                                                // 释放申请的内存
    }
    
    GUI_VMEM_Free(wbuf);                                                // 释放申请的内存
  }
}

/**
*@brief		TCP Server回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_server(HWND hwnd)
{	
	uint16 len=0;  
	switch(getSn_SR(SOCK_TCPS))											            	/*获取socket的状态*/
	{
		case SOCK_CLOSED:													                  /*socket处于关闭状态*/
			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*打开socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket已初始化状态*/
			listen(SOCK_TCPS);												                /*socket建立监听*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket处于连接建立状态*/
      
      if (link_flag)
      {
        SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"断开");
        link_flag = 1;
      }
		
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*清除接收中断标志位*/
			}
			len=getSn_RX_RSR(SOCK_TCPS);									            /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPS, comdata, len);								              	/*接收来自Client的数据*/
				comdata[len] = '\0'; 											                  /*添加字符串结束符*/
//				printf("%s\r\n",comdata);
        receive_win_set((char *)comdata, len);
		  }
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket处于等待关闭状态*/
			close(SOCK_TCPS);
		  break;
	}
}

/**
*@brief		UDP测试程序
*@param		无
*@return	无
*/
void do_udp(HWND hwnd)
{                                                              
	uint16 len=0;
	switch(getSn_SR(SOCK_UDPS))                                                /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                                        /*socket处于关闭状态*/
			socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                              /*初始化socket*/
		  break;
		
		case SOCK_UDP:                                                           /*socket初始化完成*/
//			delay_ms(10);
      if (link_flag)
      {
        SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"断开");
        link_flag = 1;
      }
      
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*清接收中断*/
			}
			if((len=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*接收到数据*/
			{
				recvfrom(SOCK_UDPS,comdata, len, remote_ip,&remote_port);               /*W5500接收计算机发送来的数据*/
				comdata[len-8]=0x00;                                                    /*添加字符串结束符*/
//				printf("%s\r\n",comdata);                                               /*打印接收缓存*/ 
				sendto(SOCK_UDPS,comdata,len-8, remote_ip, remote_port);                /*W5500把接收到的数据发送给Remote*/
				comdata[len-8]='\0';
        receive_win_set((char *)comdata, len);
			}
			break;
	}

}
/**
*@brief		TCP Client回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_client(HWND hwnd)
{	
   uint16 len=0;	

	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
      
      if (link_flag)
      {
        SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"断开");
        link_flag = 1;
      }
      
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
			}
		
			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPC,comdata,len); 							   		         /*接收来自Server的数据*/
				comdata[len]=0x00;  											                 /*添加字符串结束符*/
//				printf("%s\r\n",comdata);
        receive_win_set((char *)comdata, len);
			}		  
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
			close(SOCK_TCPC);
		  break;

	}
}

void network_dispose_task(HWND hwnd) 
{
//  netmode=0;//默认是TCP Client
//	IsConnect=0;//默认不启动连接
//	IsNetInit=0;
//	GUI_msleep(10); 
//	gpio_for_w5500_config();	         	/*初始化MCU相关引脚*/
//	reset_w5500();					            /*硬复位W5500*/
//	set_w5500_mac();                    /*配置MAC地址*/
//  IsNetInit=set_w5500_ip();           /*配置IP地址*/
//	
//  socket_buf_init(txsize, rxsize);    /*初始化8个Socket的发送接收缓存大小*/
  
  while(1)
  {
    if(IsConnect && (IsNetInit==0))
		{
			switch (netmode)
			{
				case 0:    //TCP Client
						do_tcp_client(hwnd);
					break;        
				case 1:    //TCP Server
						do_tcp_server(hwnd);
					break;
				case 2:    //UDP
						do_udp(hwnd);
					break;
			}
		}
    GUI_msleep(10);
  }
}

//退出按钮重绘制
static void Ent_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置画笔色
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }

}

// 重绘普通按钮
static void Ent_Button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
  WCHAR wbuf[128];
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;

  SetBrushColor(hdc, MapRGB(hdc, 0, 202, 252));
  EnableAntiAlias(hdc, ENABLE);
  FillRoundRect(hdc, &rc, 5);
  EnableAntiAlias(hdc, DISABLE);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
    OffsetRect(&rc, 1, 1);
		SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
	}
	else
	{ //按钮是弹起状态

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));      //设置画笔色
	}

  GetWindowText(hwnd, wbuf, 128);    // 得到原文本

  SetFont(hdc, defaultFont);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);     // 绘制文字(居中显示)
}

/*
 * @brief  重绘显示亮度的透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void edit_textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillRect(hdc, &rc);

  SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));

  GetWindowText(hwnd, wbuf, 128);    // 得到原文本

  SetFont(hdc, defaultFont);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);     // 绘制文字( 顶部对齐，左对齐，当字符超出矩形边界时,自动换行)
  HLine(hdc, rc.x, rc.y + rc.h - 1, rc.w);

}

/*
 * @brief  重绘显示亮度的透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR *wbuf;
  uint32_t WinTexeLen = 0;

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  SetTextColor(hdc, MapRGB(hdc, 170, 170, 170));

  if (ds->ID == ID_TEXTBOX_Receive)
  {
    DrawText(hdc, L"接收区", -1, &rc, DT_VCENTER|DT_CENTER);  // 绘制文字
  }
  else
  {
    SetPenColor(hdc, MapRGB(hdc, 121, 121, 121));

    EnableAntiAlias(hdc, ENABLE);
    DrawRoundRect(hdc, &rc, 7);
    EnableAntiAlias(hdc, DISABLE);
    DrawText(hdc, L"发送区", -1, &rc, DT_VCENTER|DT_CENTER);  // 绘制文字
  }
  
  OffsetRect(&rc, 2, 2);
  SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
  WinTexeLen = GetWindowTextLength(hwnd) + 1;               // 获取文本长度

  if (WinTexeLen > 0)
  {
    wbuf = (WCHAR *)GUI_VMEM_Alloc(WinTexeLen*sizeof(WCHAR));         // 申请文本长度 + 新消息长度的内存
    
    if (wbuf != NULL)
    {
      GetWindowText(hwnd, wbuf, WinTexeLen+1);            // 得到原文本

      SetFont(hdc, defaultFont);
      DrawText(hdc, wbuf, -1, &rc, DT_TOP|DT_LEFT|DT_WORDBREAK);     // 绘制文字( 顶部对齐，左对齐，当字符超出矩形边界时,自动换行)
      
      GUI_VMEM_Free(wbuf);        // 释放申请的内存
    }
  }
}

/* 分组控件背景重绘回调 */
BOOL group_erase(HDC hdc,const RECT *lprc,HWND hwnd)
{
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillRect(hdc, lprc);
  
  return TRUE;
}

extern void TCP_Echo_Init(void);
extern int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);
static WCHAR I[128];

static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc, rc0, m_rc[4];
      WCHAR mode_text[][10] = {{L"TCPClient"}, {L"TCPServer"}, {L"UDP"}};
      uint32_t mode_id[] = {ID_RB1, ID_RB2, ID_RB3};
      GetClientRect(hwnd, &rc); 

      netmode=0;//默认是TCP Client
      IsConnect=0;//默认不启动连接
      IsNetInit=0;
      GUI_msleep(10); 
      gpio_for_w5500_config();	         	/*初始化MCU相关引脚*/
      reset_w5500();					            /*硬复位W5500*/
      set_w5500_mac();                    /*配置MAC地址*/
      IsNetInit=set_w5500_ip();           /*配置IP地址*/
      
      socket_buf_init(txsize, rxsize);    /*初始化8个Socket的发送接收缓存大小*/
      
      if (IsNetInit)
      {
        SetTimer(hwnd, 10, 20, TMR_START | TMR_SINGLE, NULL);
      } 
      
      HWND Temp_Handle;
      
      xTaskCreate((TaskFunction_t )network_dispose_task,      /* 任务入口函数 */
                  (const char*    )"wifi dispose task",    /* 任务名字 */
                  (uint16_t       )3*1024/4,               /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                  (void*          )hwnd,                   /* 任务入口函数参数 */
                  (UBaseType_t    )5,                      /* 任务的优先级 */
                  (TaskHandle_t*  )&network_task_handle);     /* 任务控制块指针 */
                      
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  286, 4, 23, 23, hwnd, eID_BTN_EXIT, NULL, NULL); 
                  
      CreateWindow(GROUPBOX, L"通讯协议", WS_VISIBLE, 157, 25, 164, 52, hwnd, ID_MODE_GROUP, NULL, NULL);
      SetWindowErase(GetDlgItem(hwnd, ID_MODE_GROUP), group_erase);
                  
      GetClientRect(GetDlgItem(hwnd,ID_MODE_GROUP),&rc);

      rc0.x =1;
      rc0.y =16;
      rc0.w =rc.w;
      rc0.h =rc.h-17;
      MakeMatrixRect(m_rc,&rc0,0,1,2,2);		
      for(uint8_t i=0;i<3;i++)
      {					
        CreateWindow(BUTTON,mode_text[i],WS_VISIBLE|BS_RADIOBOX,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,GetDlgItem(hwnd,ID_MODE_GROUP), mode_id[i], NULL, NULL);    // 创建单选按钮

        if(mode_id[i]==ID_RB1)
        {
          HWND wnd;
          
          wnd	= GetDlgItem(hwnd, ID_MODE_GROUP);
          wnd	= GetDlgItem(wnd, ID_RB1&0xFFFF);
          SendMessage(wnd, BM_SETSTATE, BST_CHECKED, 0);
        }
      }

      rc.x = 262;
      rc.y = 99;
      rc.w = 54;
      rc.h = 22;
      
      CreateWindow(BUTTON, L"连接", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|WS_OWNERDRAW,
                  rc.x,rc.y,rc.w,rc.h, hwnd, eID_BTN_STATE, NULL, NULL);
      
      /* 数据发送文本窗口 */
      rc.w = 161;
      rc.h = 117;
      rc.x = 157;
      rc.y = 122;
      CreateWindow(TEXTBOX, L"你好！这里是野火开发板 ^_^", WS_TRANSPARENT | WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Send, NULL, NULL);

      /* 创建接收窗口 */
      rc.w = 154;
      rc.h = 194;
      rc.x = 1;
      rc.y = 26;
      Receive_Handle = CreateWindow(TEXTBOX, L"", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Receive, NULL, NULL);
      
      rc.x = 281;
      rc.y = 217;
      rc.w = 35;
      rc.h = 20;
      CreateWindow(BUTTON, L"发送", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|WS_OWNERDRAW,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_BTN_SEND, NULL, NULL); 
                         
      rc.x = 98;
      rc.h = 20;
      rc.w = 55;
      rc.y = 217;
      CreateWindow(BUTTON, L"清空接收", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|WS_OWNERDRAW,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_BTN_CLEAR, NULL, NULL); 

      /* IP&端口文本窗口 */
      rc.w = 120;
      rc.h = 22;
      rc.x = 196;
      rc.y = 76;
      Temp_Handle = CreateWindow(TEXTBOX, L"192.168.0.122", WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

//      OffsetRect(&rc, rc.w+3, 0);
//      Temp_Handle = CreateWindow(TEXTBOX, L"168", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP2, NULL, NULL);//
//      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

//      OffsetRect(&rc, rc.w+3, 0);
//      Temp_Handle = CreateWindow(TEXTBOX, L"000", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP3, NULL, NULL);//
//      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

//      OffsetRect(&rc, rc.w+3, 0);
//      Temp_Handle = CreateWindow(TEXTBOX, L"138", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP4, NULL, NULL);//
//      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      rc.w = 55;
      rc.h = 22;
      rc.x = 196;
      rc.y = 99;
      Temp_Handle = CreateWindow(TEXTBOX, L"6000", WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemotePort, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);



      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // 定时器 ID

      if (tmr_id == 10)    // 以连接错误
      {
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] ={L"确认",L"取消"};      //对话框内按钮的文字

        ops.Flag =MB_ICONERROR;
        ops.pButtonText =btn;
        ops.ButtonCount =2;
        RC.w = 160;
        RC.h = 120;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"以太网初始化失败\n请检查连接。", L"错误", &ops);    // 显示错误提示框
        PostCloseMessage(hwnd);                                                  // 发送关闭窗口的消息
      }
      
      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      
      RECT rc =  {0, 0, GUI_XSIZE, GUI_YSIZE};
      // hdc_pointer = CreateMemoryDC(SURF_SCREEN, PANEL_W, PANEL_H);
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
      FillRect(hdc, &rc);

      rc.h = 25;
      GradientFillRect(hdc, &rc, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
      SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
      DrawText(hdc, L"以太网", -1, &rc, DT_VCENTER|DT_CENTER);

      SetPenColor(hdc, MapRGB(hdc, 121, 121, 121));

      rc.x = 1;
      rc.y = 26;
      rc.w = 154;
      rc.h = 213;
      EnableAntiAlias(hdc, ENABLE);
      DrawRoundRect(hdc, &rc, 5);     // 绘制接收区的外框
      EnableAntiAlias(hdc, DISABLE);
      
      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 0x16, 0x9B, 0xD5));

      rc.x = 157;
      rc.y = 74;
      rc.w = 39;
      rc.h = 24;
      DrawText(hdc, L"IP:", -1, &rc, DT_RIGHT|DT_TOP);

      rc.y = 98;
      DrawText(hdc, L"端口:", -1, &rc, DT_RIGHT|DT_TOP);
      
      EndPaint(hwnd, &ps);
      break;
    }
    
    //设置TEXTBOX的背景颜色以及文字颜色
		case	WM_CTLCOLOR:
		{
			/* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
			 * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
			 * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
			 * 将忽略本次操作，继续使用默认的颜色进行绘制.
			 *
			 */
			u16 id;
			id =LOWORD(wParam);
      
			if(id >= ID_MODE_GROUP)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(0, 0, 0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(255, 255, 255);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(10,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;				
			}

      return FALSE;
		}

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_BTN_EXIT:
          {
            Ent_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_TEXTBOX_Send:
          case ID_TEXTBOX_Receive:
          {
            Brigh_Textbox_OwnerDraw(ds);
            return TRUE;   
          }

          case eID_BTN_STATE:
          case eID_BTN_SEND:
          case eID_BTN_CLEAR:
          {
            Ent_Button_OwnerDraw(ds);
            return TRUE;   
          }
          
          case ID_TEXTBOX_RemoteIP:
//          case ID_TEXTBOX_RemoteIP2:
//          case ID_TEXTBOX_RemoteIP3:
//          case ID_TEXTBOX_RemoteIP4:
          case ID_TEXTBOX_RemotePort:
          {
            edit_textbox_OwnerDraw(ds);
            return TRUE;   
          }
       }

       break;
    }

    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型    
      if(code == BN_CLICKED && id == eID_BTN_EXIT)    // 退出按钮按下
      {
        PostCloseMessage(hwnd);    // 发送关闭窗口的消息
        break;
      }
      if(code == BN_CLICKED && id == eID_BTN_CLEAR)    // 清空接收的窗口的按钮被按下
      {
        SetWindowText(Receive_Handle, L"");
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP)    // IP1 编辑框被按下
      {
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP", I, 16, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), I);
        
        char buf[128];

        switch (netmode)
        {
          case 1:     
            x_wcstombs(buf, I, 128);                                       // 将宽字符串转为单字符串
            TextToIP((uint8_t *)buf, local_ip);            
            break;
          
          case 0:
          case 2:
            x_wcstombs(buf, I, 128);                                       // 将宽字符串转为单字符串
            TextToIP((uint8_t *)buf, remote_ip);
            break;
        }
        
        
        break;
      }
      
//      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP2){    // IP2 编辑框被按下
//        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP2", I, 3, hwnd);
//        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP2), I);
//        break;
//      }
//      
//      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP3){    // IP3 编辑框被按下
//        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP3", I, 3, hwnd);
//        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP3), I);
//        break;
//      }
//      
//      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP4){    // IP4 编辑框被按下
//        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP4", I, 3, hwnd);
//        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP4), I);
//        break;
//      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemotePort)    // 端口 编辑框被按下
      {
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"PORT", I, 5, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), I);
        
        char buf[128];
        switch (netmode)
        {
          case 1:     
            x_wcstombs(buf, I, 128);                                       // 将宽字符串转为单字符串
            local_port = x_atoi(buf);
            break;
          
          case 0:
          case 2:
            x_wcstombs(buf, I, 128);                                       // 将宽字符串转为单字符串
            remote_port = x_atoi(buf);
            break;
        }
        
        break;
      }
      
      if(code == BN_CLICKED && id == eID_BTN_STATE)
      {
        if(IsConnect==0)
				{
					IsConnect = 1;//启动连接
          link_flag = 1;
          SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"连接中..");

//          EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), DISABLE);
//          EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), DISABLE);
//          EnableWindow(GetDlgItem(hwnd, ID_MODE_GROUP), DISABLE);
				}
				else
				{
          link_flag = 0;
          SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"连接");
//          if(netmode!=1)
//          {
//            EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), ENABLE);
//          }
//          EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), ENABLE);
//          EnableWindow(GetDlgItem(hwnd, ID_MODE_GROUP), ENABLE);
//					remote_port=5000;//调整远端端口为默认值
//					local_port=6000;//调整本地端口为默认值
					IsConnect=0;//关闭连接
				}
      }
      
      if(code == BN_CLICKED && id == eID_BTN_SEND)
      {
        WCHAR wbuf[128];

        /* 获取发送窗口的字符串 */
        GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_Send), wbuf, 128);
        x_wcstombs((char *)comdata, wbuf, 128);
        
        switch(netmode)
        {
          case 0:
            send(SOCK_TCPC,comdata,strlen((const char *)comdata));								     	         /*向Server发送数据*/
            break;
          case 1:
            send(SOCK_TCPS,comdata,strlen((const char *)comdata));
           break;
          case 2:
            sendto(SOCK_UDPS,comdata,strlen((const char *)comdata), remote_ip, remote_port);    
           break;
        }
      }
      
      /* 单选按钮被按下 */
      if( (id >= (ID_RB1 & ~(1<<16))) && (id <= (ID_RB3 & ~(1<<16))))
      {
        if (code == BN_CLICKED)
        {
          netmode = id & 3;
          WCHAR iptext[20];
          WCHAR portetxt[10];
          
          switch (netmode)
          {
            case 1:
              EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), DISABLE);
              EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), DISABLE);
              x_wsprintf(iptext, L"%d.%d.%d.%d",local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
              SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), iptext);
              x_wsprintf(portetxt, L"%d", local_port);
              SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), portetxt);             
              break;
            
            case 0:
            case 2:
              EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), DISABLE);
              EnableWindow(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), DISABLE);
              x_wsprintf(iptext, L"%d.%d.%d.%d",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
              SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP), iptext);
              x_wsprintf(portetxt, L"%d", local_port);
              SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), portetxt);   
              break;
          }
        }
      }

      break;
    } 

    case WM_DESTROY:
    { 
      IsConnect=0;//关闭连接
      vTaskDelete(network_task_handle);

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void gui_network_dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND hwnd;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"gui network dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);

   //显示主窗口
	ShowWindow(hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


