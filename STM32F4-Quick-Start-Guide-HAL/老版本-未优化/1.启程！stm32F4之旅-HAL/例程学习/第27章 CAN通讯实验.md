# 第二十七章 CAN通讯实验

## 1. 硬件设计

本章要用到的硬件资源如下：

- 指示灯DS0
- KEY0和KEY_UP按键
- TFTLCD模块
- CAN
- CAN收发芯片JTA1050

前面 3 个之前都已经详细介绍过了，这里我们介绍 STM32F4 与 TJA1050 连接关系，如图：

![屏幕截图 2024-10-21 225324.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-22-53-35-屏幕截图%202024-10-21%20225324.png)

从上图可以看出： STM32F4 的 CAN 通过 P11 的设置，连接到 TJA1050 收发芯片，然后通过接线端子（CAN）同外部的 CAN 总线连接。图中可以看出，在探索者 STM32F4 开发板上面是带有 120Ω的终端电阻的，如果我们的开发板不是作为 CAN 的终端的话，需要把这个电阻去掉，以免影响通信。另外，需要注意： CAN1 和 USB 共用了 PA11 和 PA12，所以他们不能同时使用。

这里还要注意，我们要设置好开发板上 P11 排针的连接，通过跳线帽将 PA11 和 PA12 分别连接到 CRX（CAN_RX）和 CTX（CAN_TX）上面，如图：

![屏幕截图 2024-10-21 225417.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-22-54-20-屏幕截图%202024-10-21%20225417.png)

最后，我们用 2 根导线将两个开发板 CAN 端子的 CAN_L 和 CAN_L， CAN_H 和 CAN_H连接起来。

## 2. 软件设计

### 2.1 CAN初始化

```c
CAN_HandleTypeDef    CAN1_Handler; // CAN1句柄
CAN_TxHeaderTypeDef    TxHeader;   // 发送
CAN_RxHeaderTypeDef    RxHeader;   // 接收
// CAN初始化
// tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1TQ~CAN_SJW_4TQ
// tbs2:时间段2的时间单元.   范围:CAN_BS2_1TQ~CAN_BS2_8TQ;
// tbs1:时间段1的时间单元.   范围:CAN_BS1_1TQ~CAN_BS1_16TQ
// brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
// 波特率=Fpclk1/((tbs1+tbs2+1)*brp); 其中tbs1和tbs2我们只用关注标识符上标志的序号，例如CAN_BS2_1TQ，我们就认为tbs2=1来计算即可。
// mode:CAN_MODE_NORMAL,普通模式;CAN_MODE_LOOPBACK,回环模式;
// Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_MODE_LOOPBACK);
// 则波特率为:42M/((6+7+1)*6)=500Kbps
// 返回值:0,初始化OK;
// 其他,初始化失败; 
u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
  CAN_InitTypeDef    CAN1_InitConf; // CAN1初始化配置结构体
  CAN1_Handler.Instance = CAN1;  // CAN1设备句柄
    CAN1_Handler.Init = CAN1_InitConf;// CAN1初始化配置结构体
  CAN1_Handler.Init.Prescaler=brp;    // 分频系数(Fdiv)为brp+1
  CAN1_Handler.Init.Mode=mode;            // 模式设置 
  CAN1_Handler.Init.SyncJumpWidth=tsjw;// 重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
  CAN1_Handler.Init.TimeSeg1=tbs1;         // tbs1范围CAN_BS1_1TQ~CAN_BS1_16TQ
  CAN1_Handler.Init.TimeSeg2=tbs2;         // tbs2范围CAN_BS2_1TQ~CAN_BS2_8TQ
  CAN1_Handler.Init.TimeTriggeredMode=DISABLE;// 非时间触发通信模式 
  CAN1_Handler.Init.AutoBusOff=DISABLE;              // 软件自动离线管理
  CAN1_Handler.Init.AutoWakeUp=DISABLE;              // 睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  CAN1_Handler.Init.AutoRetransmission=ENABLE;// 禁止报文自动传送 
  CAN1_Handler.Init.ReceiveFifoLocked=DISABLE;// 报文不锁定,新的覆盖旧的 
  CAN1_Handler.Init.TransmitFifoPriority=DISABLE;// 优先级由报文标识符决定 
  if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK)            // 初始化
        return 1;
  return 0;
}
```

### 2.2 CAN GPIO配置

```c
// CAN底层驱动，引脚配置，时钟配置，中断配置
// 此函数会被HAL_CAN_Init()调用
// hcan:CAN句柄
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_CAN1_CLK_ENABLE();          // 使能CAN1时钟
  __HAL_RCC_GPIOA_CLK_ENABLE();                // 开启GPIOA时钟
  GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;// PA11,12
  GPIO_Initure.Mode=GPIO_MODE_AF_PP;       // 推挽复用
  GPIO_Initure.Pull=GPIO_PULLUP;           // 上拉
  GPIO_Initure.Speed=GPIO_SPEED_FAST;      // 快速
  GPIO_Initure.Alternate=GPIO_AF9_CAN1;    // 复用为CAN1
  HAL_GPIO_Init(GPIOA,&GPIO_Initure);      // 初始化
}
```

### 2.3 CAN配置函数

```c
// CAN配置函数
void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;
  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0; 
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;
  if (HAL_CAN_ConfigFilter(&CAN1_Handler, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    while(1)
      {
      }
  }
  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&CAN1_Handler) != HAL_OK)
  {
    /* Start Error */
    while(1)
      {
      }
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&CAN1_Handler, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    while(1)
      {
      }
  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
}
```

### 2.4 CAN发送数据函数

```c
// can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)    
// len:数据长度(最大为8)                     
// msg:数据指针,最大为8个字节.
// 返回值:0,成功;
//         其他,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{    
  u8 i=0;
    u32 TxMailbox;
    u8 message[8];
  TxHeader.StdId = 0X12;       // 标准标识符
  TxHeader.ExtId = 0x12;       // 扩展标识符(29位)
  TxHeader.IDE = CAN_ID_STD;   // 使用标准帧
  TxHeader.RTR = CAN_RTR_DATA; // 数据帧
  TxHeader.DLC = len; // 数据长度                
  for(i=0;i<len;i++)
  {
        message[i]=msg[i];
    }
  if(HAL_CAN_AddTxMessage(&CAN1_Handler, &TxHeader, message, &TxMailbox) != HAL_OK)//发送
    {
        return 1;
    }
    while(HAL_CAN_GetTxMailboxesFreeLevel(&CAN1_Handler) != 3) {}
    return 0;
}
```

### 2.5 CAN接收数据查询

```c
// can口接收数据查询
// buf:数据缓存区;     
// 返回值:0,无数据被收到;
//         其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{
     u32 i;
    u8    RxData[8];
    if(HAL_CAN_GetRxFifoFillLevel(&CAN1_Handler, CAN_RX_FIFO0) != 1)
    {
        return 0xF1;
    }
    if(HAL_CAN_GetRxMessage(&CAN1_Handler, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
        return 0xF2;
    }
    for(i=0;i<RxHeader.DLC;i++)
        buf[i]=RxData[i];
    return RxHeader.DLC;
}
```

### 2.6 主函数

```c
int main(void)
{
       u8 key;
    u8 i=0,t=0;
    u8 cnt=0;
    u8 canbuf[8];
    u8 res;
    u8 mode=1;     
    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
    delay_init(168);            // 初始化延时函数
    uart_init(115200);          // 初始化USART
    usmart_dev.init(84);         // 初始化USMART
    LED_Init();                    // 初始化LED    
    KEY_Init();                    // 初始化KEY
     LCD_Init();                   // 初始化LCD
     CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_7TQ,6,CAN_MODE_LOOPBACK); // CAN初始化,波特率500Kbps     
    CAN_Config(); // 配置CAN1
    POINT_COLOR=RED;
    LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");    
    LCD_ShowString(30,70,200,16,16,"CAN TEST");             
    LCD_ShowString(30,130,200,16,16,"LoopBack Mode");     
    LCD_ShowString(30,150,200,16,16,"KEY0:Send WK_UP:Mode");//显示提示信息        
      POINT_COLOR=BLUE;//设置字体为蓝色      
    LCD_ShowString(30,170,200,16,16,"Count:");         // 显示当前计数值    
    LCD_ShowString(30,190,200,16,16,"Send Data:");     // 提示发送的数据    
    LCD_ShowString(30,250,200,16,16,"Receive Data:");// 提示接收到的数据        
    while(1)
    {
        key=KEY_Scan(0);
        if(key==KEY0_PRES)//KEY0按下,发送一次数据
        {
            for(i=0;i<8;i++)
            {
                canbuf[i]=cnt+i;//填充发送缓冲区
                if(i<4)LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);    //显示数据
                else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);    //显示数据
             }
            res=CAN1_Send_Msg(canbuf,8);// 发送8个字节 
            if(res)LCD_ShowString(30+80,190,200,16,16,"Failed");        //提示发送失败
            else LCD_ShowString(30+80,190,200,16,16,"OK    ");             //提示发送成功                                   
        }else if(key==WKUP_PRES)//WK_UP按下，改变CAN的工作模式
        {       
            mode=!mode;
            if(mode==0)  CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_7TQ,6,CAN_MODE_NORMAL);        //回环模式,波特率500Kbps
            else if(mode==1) CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_7TQ,6,CAN_MODE_LOOPBACK);  //回环模式,波特率500Kbps
            CAN_Config();
              POINT_COLOR=RED;//设置字体为红色 
            if(mode==0)//普通模式，需要2个开发板
            {
                LCD_ShowString(30,130,200,16,16,"Nnormal Mode ");        
            }else //回环模式,一个开发板就可以测试了.
            {
                 LCD_ShowString(30,130,200,16,16,"LoopBack Mode");
            }
             POINT_COLOR=BLUE;//设置字体为蓝色 
        }         
        key=CAN1_Receive_Msg(canbuf);
        if(key < 9)//接收到有数据
        {            
            LCD_Fill(30,270,160,310,WHITE);//清除之前的显示
             for(i=0;i<key;i++)
            {                                        
                if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);    //显示数据
                else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);    //显示数据
             }
        }
        t++; 
        delay_ms(10);
        if(t==20)
        {
            LED0=!LED0;//提示系统正在运行    
            t=0;
            cnt++;
            LCD_ShowxNum(30+48,170,cnt,3,16,0X80);    //显示数据
        }           
    }     
}
```

## 3. 小结

### 实验目的

1. 理解CAN总线通信原理。
2. 实现CAN数据的发送与接收。
3. 使用LCD模块显示接收到的数据。

### 硬件连接

1. **CAN接口连接**：
   
   - STM32F4开发板的CAN_TX连接到MCP2551的TXD引脚。
   - STM32F4开发板的CAN_RX连接到MCP2551的RXD引脚。
   - MCP2551的VCC和GND连接至开发板的电源和接地。

2. **LCD模块连接**：
   
   - 按照LCD模块的引脚说明连接到STM32F4的GPIO引脚。

3. **按键连接**：
   
   - KEY_UP连接到某个GPIO引脚（例如PA0）。
   - KEY0连接到另一个GPIO引脚（例如PA1）。

### 实验步骤

#### 1. 项目初始化

使用STM32CubeMX配置项目：

- 启用CAN模块（如CAN1）。
- 配置波特率为500 kbps。
- 配置GPIO引脚（TX、RX和LCD控制引脚）。

#### 2. 编写CAN初始化代码

```c
CAN_HandleTypeDef hcan1;

void MX_CAN1_Init(void) {
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 16;
    hcan1.Init.Mode = CAN_MODE_NORMAL; // 初始模式
    hcan1.Init.SJW = CAN_SJW_1TQ;
    hcan1.Init.BS1 = CAN_BS1_8TQ;
    hcan1.Init.BS2 = CAN_BS2_3TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    HAL_CAN_Init(&hcan1);
}
```

#### 3. 按键处理

- 使用外部中断处理按键输入。
- KEY_UP用于切换工作模式（环回或正常模式）。
- KEY0用于发送数据。

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == KEY_UP_PIN) {
        // 切换工作模式
        if (hcan1.Init.Mode == CAN_MODE_NORMAL) {
            hcan1.Init.Mode = CAN_MODE_LOOPBACK;
        } else {
            hcan1.Init.Mode = CAN_MODE_NORMAL;
        }
        HAL_CAN_Init(&hcan1); // 重新初始化
    } else if (GPIO_Pin == KEY0_PIN) {
        // 发送数据
        uint8_t data[] = "Hello CAN!";
        CAN_TxHeaderTypeDef txHeader;
        txHeader.StdId = 0x321; 
        txHeader.ExtId = 0x01; 
        txHeader.RTR = CAN_RTR_DATA;
        txHeader.DLC = sizeof(data);
        HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &txMailbox);
    }
}
```

#### 4. 接收数据

设置CAN接收功能，并在LCD上显示接收到的数据。

```c
void CAN_Receive(void) {
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];
    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
        // 将接收到的数据通过LCD显示
        LCD_Display(rxData, rxHeader.DLC);
    }
}
```

#### 5. LCD显示函数

实现一个函数，用于在LCD上显示接收到的数据。

```c
void LCD_Display(uint8_t* data, uint8_t length) {
    LCD_Clear(); // 清空LCD
    for (int i = 0; i < length; i++) {
        LCD_SendData(data[i]); // 发送数据到LCD
    }
}
```

#### 6. 主函数

在主函数中初始化系统和CAN，并进入主循环。

```c
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_CAN1_Init();
    LCD_Init();

    while (1) {
        CAN_Receive(); // 接收数据
        HAL_Delay(100); // 延迟
    }
}
```
