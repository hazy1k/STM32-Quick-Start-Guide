#include "xpt2046.h"
#include "ili9341.h"
#include "led.h"
#include "fonts.h"
#include "flash.h"
#include "palette.h"
#include <stdio.h> 
#include <string.h>

static void XPT2046_DelayUS(__IO uint32_t ulCount);
static void XPT2046_WriteCMD(uint8_t ucCmd);
static uint16_t XPT2046_ReadCMD(void);
static uint16_t XPT2046_ReadAdc(uint8_t ucChannel);
static void XPT2046_ReadAdc_XY(int16_t *sX_Ad, int16_t *sY_Ad);
static uint8_t XPT2046_ReadAdc_Smooth_XY(strType_XPT2046_Coordinate *pScreenCoordinate);
static uint8_t XPT2046_Calculate_CalibrationFactor(strType_XPT2046_Coordinate *pDisplayCoordinate, strType_XPT2046_Coordinate *pScreenSample, strType_XPT2046_Calibration *pCalibrationFactor);
static void ILI9341_DrawCross(uint16_t usX, uint16_t usY);
// 默认触摸参数，不同的屏幕稍有差异，可重新调用触摸校准函数获取
strType_XPT2046_TouchPara strXPT2046_TouchPara[] = { 	
 -0.006464,   -0.073259,  280.358032,    0.074878,    0.002052,   -6.545977,  // 扫描方式0
	0.086314,    0.001891,  -12.836658,   -0.003722,   -0.065799,  254.715714,// 扫描方式1
	0.002782,    0.061522,  -11.595689,    0.083393,    0.005159,  -15.650089,// 扫描方式2
	0.089743,   -0.000289,  -20.612209,   -0.001374,    0.064451,  -16.054003,// 扫描方式3
	0.000767,   -0.068258,  250.891769,   -0.085559,   -0.000195,  334.747650,// 扫描方式4
 -0.084744,    0.000047,  323.163147,   -0.002109,   -0.066371,  260.985809,  // 扫描方式5
 -0.001848,    0.066984,  -12.807136,   -0.084858,   -0.000805,  333.395386,  // 扫描方式6
 -0.085470,   -0.000876,  334.023163,   -0.003390,    0.064725,   -6.211169,  // 扫描方式7
};

volatile uint8_t ucXPT2046_TouchFlag = 0; // 触摸标志位

void XPT2046_Init(void) // 初始化XPT2046
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE);
  /* 模拟SPI GPIO初始化 */       
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CLK_PIN; // 时钟引脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN; // 串行数据输入引脚
  GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN; // 串行数据输出引脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN; // 片选引脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure); 
  /* 拉低片选，选择XPT2046 */
  XPT2046_CS_DISABLE();			
  // 触摸屏触摸信号指示引脚，不使用中断
  GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
  GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);		
}
	
static void XPT2046_DelayUS(__IO uint32_t ulCount) // 简单延时函数
{
	uint32_t i;
	for(i = 0; i < ulCount; i ++)
	{
		uint8_t uc = 12;     
		while ( uc -- );   
	}
}
// 向XPT2046发送命令，函数参数：0x90 :通道Y+的选择控制字，0xd0 :通道X+的选择控制字
static void XPT2046_WriteCMD(uint8_t ucCmd) 
{
	uint8_t i;
	XPT2046_MOSI_0(); // 拉低MOSI
	XPT2046_CLK_LOW(); // 拉低时钟
	for(i = 0; i < 8; i ++) // 发送命令
	{
		((ucCmd >> (7 - i)) & 0x01 ) ? XPT2046_MOSI_1() : XPT2046_MOSI_0();
		XPT2046_DelayUS(5);
		XPT2046_CLK_HIGH();
		XPT2046_DelayUS(5);
		XPT2046_CLK_LOW();
	}
}
// 读取XPT2046的命令
static uint16_t XPT2046_ReadCMD ( void ) 
{
	uint8_t i;
	uint16_t usBuf=0, usTemp;
	XPT2046_MOSI_0();
	XPT2046_CLK_HIGH();
	for(i = 0; i < 12; i++) 
	{
		XPT2046_CLK_LOW();    
		usTemp = XPT2046_MISO();
		usBuf |= usTemp << ( 11 - i );
		XPT2046_CLK_HIGH();
	}
	return usBuf;
}
// 对 XPT2046 选择一个模拟通道后，启动ADC，并返回ADC采样结果
static uint16_t XPT2046_ReadAdc(uint8_t ucChannel)
{
	XPT2046_WriteCMD(ucChannel);
	return XPT2046_ReadCMD();
}
// 读取 XPT2046 的X通道和Y通道的AD值（12 bit，最大是4096）
static void XPT2046_ReadAdc_XY(int16_t * sX_Ad, int16_t * sY_Ad)  
{ 
	int16_t sX_Ad_Temp, sY_Ad_Temp; 
	sX_Ad_Temp = XPT2046_ReadAdc(XPT2046_CHANNEL_X);
	XPT2046_DelayUS(1); 
	sY_Ad_Temp = XPT2046_ReadAdc ( XPT2046_CHANNEL_Y ); 
	* sX_Ad = sX_Ad_Temp; 
	* sY_Ad = sY_Ad_Temp; 
}
// 在触摸 XPT2046 屏幕时获取一组坐标的AD值，并对该坐标进行滤波
// 注意：画板应用实例专用,不是很精准，但是简单，速度比较快   
static uint8_t XPT2046_ReadAdc_Smooth_XY ( strType_XPT2046_Coordinate * pScreenCoordinate )
{
	uint8_t ucCount = 0, i;
	int16_t sAD_X, sAD_Y;
	int16_t sBufferArray[2][10] = {{0},{0}}; // 坐标X和Y进行多次采样
	// 存储采样中的最小值、最大值
	int32_t lX_Min, lX_Max, lY_Min, lY_Max;
	/* 循环采样10次 */ 
	do					       				
	{		  
		XPT2046_ReadAdc_XY(&sAD_X, &sAD_Y);  
		sBufferArray[0][ucCount] = sAD_X;  
		sBufferArray[1][ucCount] = sAD_Y;
		ucCount ++;  
	}while((XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)&&(ucCount < 10)); // 用户点击触摸屏时即TP_INT_IN信号为低 并且 ucCount<10
	/* 如果触笔弹起 */
	if(XPT2046_PENIRQ_Read() != XPT2046_PENIRQ_ActiveLevel)
		ucXPT2046_TouchFlag = 0; // 中断标志复位
	/* 如果成功采样10个样本 */
	if(ucCount == 10)		 					
	{
		lX_Max = lX_Min = sBufferArray[0][0];
		lY_Max = lY_Min = sBufferArray[1][0];       
		for(i = 1; i < 10; i++)
		{
			if(sBufferArray[0][i] < lX_Min)
				lX_Min = sBufferArray[0][i];
			else if ( sBufferArray[0][i] > lX_Max)
				lX_Max = sBufferArray[0][i];
		}
		for(i = 1; i < 10; i ++)
		{
			if(sBufferArray[1][i] < lY_Min)
				lY_Min = sBufferArray[1][i];
			else if(sBufferArray[1][i] > lY_Max)
				lY_Max = sBufferArray[1][i];
		}
		/*去除最小值和最大值之后求平均值*/
		pScreenCoordinate ->x =  ( sBufferArray [ 0 ] [ 0 ] + sBufferArray [ 0 ] [ 1 ] + sBufferArray [ 0 ] [ 2 ] + sBufferArray [ 0 ] [ 3 ] + sBufferArray [ 0 ] [ 4 ] + 
		                           sBufferArray [ 0 ] [ 5 ] + sBufferArray [ 0 ] [ 6 ] + sBufferArray [ 0 ] [ 7 ] + sBufferArray [ 0 ] [ 8 ] + sBufferArray [ 0 ] [ 9 ] - lX_Min-lX_Max ) >> 3;
		pScreenCoordinate ->y =  ( sBufferArray [ 1 ] [ 0 ] + sBufferArray [ 1 ] [ 1 ] + sBufferArray [ 1 ] [ 2 ] + sBufferArray [ 1 ] [ 3 ] + sBufferArray [ 1 ] [ 4 ] + 
		                           sBufferArray [ 1 ] [ 5 ] + sBufferArray [ 1 ] [ 6 ] + sBufferArray [ 1 ] [ 7 ] + sBufferArray [ 1 ] [ 8 ] + sBufferArray [ 1 ] [ 9 ] - lY_Min-lY_Max ) >> 3; 
		return 1;		
	}   	
	return 0;    	
}
static uint8_t XPT2046_Calculate_CalibrationFactor(strType_XPT2046_Coordinate *pDisplayCoordinate, strType_XPT2046_Coordinate *pScreenSample, strType_XPT2046_Calibration *pCalibrationFactor) {
    uint8_t ucRet = 1;
    pCalibrationFactor->Divider = ((pScreenSample[0].x - pScreenSample[2].x) * (pScreenSample[1].y - pScreenSample[2].y)) - ((pScreenSample[1].x - pScreenSample[2].x) * (pScreenSample[0].y - pScreenSample[2].y));
    if (pCalibrationFactor->Divider == 0)
        ucRet = 0;
    else {
        pCalibrationFactor->An = ((pDisplayCoordinate[0].x - pDisplayCoordinate[2].x) * (pScreenSample[1].y - pScreenSample[2].y)) - ((pDisplayCoordinate[1].x - pDisplayCoordinate[2].x) * (pScreenSample[0].y - pScreenSample[2].y));
        pCalibrationFactor->Bn = ((pScreenSample[0].x - pScreenSample[2].x) * (pDisplayCoordinate[1].x - pDisplayCoordinate[2].x)) - ((pDisplayCoordinate[0].x - pDisplayCoordinate[2].x) * (pScreenSample[1].x - pScreenSample[2].x));
        pCalibrationFactor->Cn = (pScreenSample[2].x * pDisplayCoordinate[1].x - pScreenSample[1].x * pDisplayCoordinate[2].x) * pScreenSample[0].y +
                                 (pScreenSample[0].x * pDisplayCoordinate[2].x - pScreenSample[2].x * pDisplayCoordinate[0].x) * pScreenSample[1].y +
                                 (pScreenSample[1].x * pDisplayCoordinate[0].x - pScreenSample[0].x * pDisplayCoordinate[1].x) * pScreenSample[2].y;
        pCalibrationFactor->Dn = ((pDisplayCoordinate[0].y - pDisplayCoordinate[2].y) * (pScreenSample[1].y - pScreenSample[2].y)) - ((pDisplayCoordinate[1].y - pDisplayCoordinate[2].y) * (pScreenSample[0].y - pScreenSample[2].y));
        pCalibrationFactor->En = ((pScreenSample[0].x - pScreenSample[2].x) * (pDisplayCoordinate[1].y - pDisplayCoordinate[2].y)) - ((pDisplayCoordinate[0].y - pDisplayCoordinate[2].y) * (pScreenSample[1].x - pScreenSample[2].x));
        pCalibrationFactor->Fn = (pScreenSample[2].x * pDisplayCoordinate[1].y - pScreenSample[1].x * pDisplayCoordinate[2].y) * pScreenSample[0].y +
                                 (pScreenSample[0].x * pDisplayCoordinate[2].y - pScreenSample[2].x * pDisplayCoordinate[0].y) * pScreenSample[1].y +
                                 (pScreenSample[1].x * pDisplayCoordinate[0].y - pScreenSample[0].x * pDisplayCoordinate[1].y) * pScreenSample[2].y;
    }
    return ucRet;
}
// 在 ILI9341 上显示校正触摸时需要的十字
static void ILI9341_DrawCross(uint16_t usX, uint16_t usY)
{
	ILI9341_DrawLine(usX-10,usY,usX+10,usY);
	ILI9341_DrawLine(usX, usY - 10, usX, usY+10);	
}
// XPT2046 触摸校准函数，LCD_Mode 为当前使用的LCD模式
uint8_t XPT2046_Touch_Calibrate(uint8_t LCD_Mode) {
    uint8_t i;
    char cStr[100];
    uint16_t usTest_x = 0, usTest_y = 0, usGap_x = 0, usGap_y = 0;
    char *pStr = 0;
    strType_XPT2046_Coordinate strCrossCoordinate[4], strScreenSample[4];
    strType_XPT2046_Calibration CalibrationFactor;
    LCD_SetFont(&Font8x16);
    LCD_SetColors(BLUE, BLACK);
    ILI9341_GramScan(LCD_Mode);
    // 设置“十”字交叉点的坐标
    strCrossCoordinate[0].x = LCD_X_LENGTH >> 2;
    strCrossCoordinate[0].y = LCD_Y_LENGTH >> 2;
    strCrossCoordinate[1].x = strCrossCoordinate[0].x;
    strCrossCoordinate[1].y = (LCD_Y_LENGTH * 3) >> 2;
    strCrossCoordinate[2].x = (LCD_X_LENGTH * 3) >> 2;
    strCrossCoordinate[2].y = strCrossCoordinate[1].y;
    strCrossCoordinate[3].x = strCrossCoordinate[2].x;
    strCrossCoordinate[3].y = strCrossCoordinate[0].y;
    for (i = 0; i < 4; i++) {
        ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
        pStr = "Touch Calibrate ......";
        sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
        ILI9341_DispStringLine_EN(LCD_Y_LENGTH >> 1, cStr);
        sprintf(cStr, "%*c%d", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - 1) / 2, ' ', i + 1);
        ILI9341_DispStringLine_EN((LCD_Y_LENGTH >> 1) - (((sFONT *)LCD_GetFont())->Height), cStr);
        XPT2046_DelayUS(300000); // 适当的延时很有必要
        ILI9341_DrawCross(strCrossCoordinate[i].x, strCrossCoordinate[i].y);
        while (!XPT2046_ReadAdc_Smooth_XY(&strScreenSample[i])); // 读取XPT2046数据到变量pCoordinate
    }
    XPT2046_Calculate_CalibrationFactor(strCrossCoordinate, strScreenSample, &CalibrationFactor);
    if (CalibrationFactor.Divider == 0) goto Failure;
    usTest_x = ((CalibrationFactor.An * strScreenSample[3].x) + (CalibrationFactor.Bn * strScreenSample[3].y) + CalibrationFactor.Cn) / CalibrationFactor.Divider;
    usTest_y = ((CalibrationFactor.Dn * strScreenSample[3].x) + (CalibrationFactor.En * strScreenSample[3].y) + CalibrationFactor.Fn) / CalibrationFactor.Divider;
    usGap_x = (usTest_x > strCrossCoordinate[3].x) ? (usTest_x - strCrossCoordinate[3].x) : (strCrossCoordinate[3].x - usTest_x);
    usGap_y = (usTest_y > strCrossCoordinate[3].y) ? (usTest_y - strCrossCoordinate[3].y) : (strCrossCoordinate[3].y - usTest_y);
    if ((usGap_x > 15) || (usGap_y > 15)) goto Failure; // 可以通过修改这两个值的大小来调整精度
    // 校准系数为全局变量
    strXPT2046_TouchPara[LCD_Mode].dX_X = (CalibrationFactor.An * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dX_Y = (CalibrationFactor.Bn * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dX = (CalibrationFactor.Cn * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dY_X = (CalibrationFactor.Dn * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dY_Y = (CalibrationFactor.En * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dY = (CalibrationFactor.Fn * 1.0) / CalibrationFactor.Divider;
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    LCD_SetTextColor(GREEN);
    pStr = "Calibrate Succed";
    sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
    ILI9341_DispStringLine_EN(LCD_Y_LENGTH >> 1, cStr);
    XPT2046_DelayUS(1000000);
    return 1;

Failure:
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    LCD_SetTextColor(RED);
    pStr = "Calibrate fail";
    sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
    ILI9341_DispStringLine_EN(LCD_Y_LENGTH >> 1, cStr);
    pStr = "try again";
    sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
    ILI9341_DispStringLine_EN((LCD_Y_LENGTH >> 1) + (((sFONT *)LCD_GetFont())->Height), cStr);
    XPT2046_DelayUS(1000000);
    return 0;
}
// 从FLASH中获取 或 重新校正触摸参数（校正后会写入到SPI FLASH中）
void Calibrate_or_Get_TouchParaWithFlash(uint8_t LCD_Mode,uint8_t forceCal)
{
	uint8_t para_flag=0;
	
	//初始化FLASH
	SPI_FLASH_Init();
	
	//读取触摸参数标志
	SPI_FLASH_BufferRead(&para_flag,FLASH_TOUCH_PARA_FLAG_ADDR,1);

	//若不存在标志或florceCal=1时，重新校正参数
	if(para_flag != FLASH_TOUCH_PARA_FLAG_VALUE | forceCal ==1)
	{ 		
		//若标志存在，说明原本FLASH内有触摸参数，
		//先读回所有LCD模式的参数值，以便稍后强制更新时只更新指定LCD模式的参数,其它模式的不变
		if(  para_flag == FLASH_TOUCH_PARA_FLAG_VALUE && forceCal == 1)
		{
			SPI_FLASH_BufferRead((uint8_t *)&strXPT2046_TouchPara,FLASH_TOUCH_PARA_ADDR,4*6*8);	
		}
		
		//等待触摸屏校正完毕,更新指定LCD模式的触摸参数值
		while( ! XPT2046_Touch_Calibrate (LCD_Mode) );     

		//擦除扇区
		SPI_FLASH_SectorErase(0);
		
		//设置触摸参数标志
		para_flag = FLASH_TOUCH_PARA_FLAG_VALUE;
		//写入触摸参数标志
		SPI_FLASH_BufferWrite(&para_flag,FLASH_TOUCH_PARA_FLAG_ADDR,1);
		//写入最新的触摸参数
		SPI_FLASH_BufferWrite((uint8_t *)&strXPT2046_TouchPara,FLASH_TOUCH_PARA_ADDR,4*6*8);
 
	}
	else	//若标志存在且不强制校正，则直接从FLASH中读取
	{
		SPI_FLASH_BufferRead((uint8_t *)&strXPT2046_TouchPara,FLASH_TOUCH_PARA_ADDR,4*6*8);	
	}
}
// 获取 XPT2046 触摸点（校准后）的坐标
uint8_t XPT2046_Get_TouchedPoint(strType_XPT2046_Coordinate *pDisplayCoordinate, strType_XPT2046_TouchPara *pTouchPara) {
    uint8_t ucRet = 1; // 若正常，则返回1
    strType_XPT2046_Coordinate strScreenCoordinate;
    if (XPT2046_ReadAdc_Smooth_XY(&strScreenCoordinate)) {
        pDisplayCoordinate->x = (pTouchPara[LCD_SCAN_MODE].dX_X * strScreenCoordinate.x) + (pTouchPara[LCD_SCAN_MODE].dX_Y * strScreenCoordinate.y) + pTouchPara[LCD_SCAN_MODE].dX;
        pDisplayCoordinate->y = (pTouchPara[LCD_SCAN_MODE].dY_X * strScreenCoordinate.x) + (pTouchPara[LCD_SCAN_MODE].dY_Y * strScreenCoordinate.y) + pTouchPara[LCD_SCAN_MODE].dY;
    } else {
        ucRet = 0; // 如果获取的触点信息有误，则返回0
    }
    return ucRet;
}
// 触摸屏检测状态机
uint8_t XPT2046_TouchDetect(void)
{ 
	static enumTouchState touch_state = XPT2046_STATE_RELEASE;
	static uint32_t i;
	uint8_t detectResult = TOUCH_NOT_PRESSED;
	switch(touch_state)
	{
		case XPT2046_STATE_RELEASE:	
		if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) //第一次出现触摸信号
		{
			touch_state = XPT2046_STATE_WAITING;
			detectResult =TOUCH_NOT_PRESSED;
		}
		else	//无触摸
		{
			touch_state = XPT2046_STATE_RELEASE;
			detectResult =TOUCH_NOT_PRESSED;
		}
		break;		
		case XPT2046_STATE_WAITING:
			if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)
			{
				i++;
				// 等待时间大于阈值则认为触摸被按下
				// 消抖时间 = DURIATION_TIME * 本函数被调用的时间间隔
				// 如在定时器中调用，每10ms调用一次，则消抖时间为：DURIATION_TIME*10ms
				if(i > DURIATION_TIME)		
				{
					i = 0;
					touch_state = XPT2046_STATE_PRESSED;
					detectResult = TOUCH_PRESSED;
				}
				else // 等待时间累加
				{
					touch_state = XPT2046_STATE_WAITING;
					detectResult =	 TOUCH_NOT_PRESSED;					
				}
			}
			else // 等待时间值未达到阈值就为无效电平，当成抖动处理					
			{
				i = 0;
            	touch_state = XPT2046_STATE_RELEASE; 
				detectResult = TOUCH_NOT_PRESSED;
			}
			break;
		case XPT2046_STATE_PRESSED:	
			if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)		//触摸持续按下
			{
				touch_state = XPT2046_STATE_PRESSED;
				detectResult = TOUCH_PRESSED;
			}
			else // 触摸释放
			{
				touch_state = XPT2046_STATE_RELEASE;
				detectResult = TOUCH_NOT_PRESSED;
			}
			break;			
		default:
			touch_state = XPT2046_STATE_RELEASE;
			detectResult = TOUCH_NOT_PRESSED;
			break;
	}		
	return detectResult;
}
// 触摸屏被按下的时候会调用本函数
void XPT2046_TouchDown(strType_XPT2046_Coordinate * touch)
{
	// 若为负值表示之前已处理过
	if(touch->pre_x == -1 && touch->pre_x == -1)
		return;  
	/* 处理触摸画板的选择按钮 */
  	Touch_Button_Down(touch->x,touch->y);
  	/* 处理描绘轨迹 */
  	Draw_Trail(touch->pre_x,touch->pre_y,touch->x,touch->y,&brush);	
}
// 触摸屏释放的时候会调用本函数
void XPT2046_TouchUp(strType_XPT2046_Coordinate * touch) 
{
	if(touch->pre_x == -1 && touch->pre_x == -1)
		return;
	Touch_Button_Up(touch->pre_x,touch->pre_y);	
}
// 检测到触摸中断时调用的处理函数,通过它调用tp_down 和tp_up汇报触摸点
void XPT2046_TouchEvenHandler(void )
{
	static strType_XPT2046_Coordinate cinfo={-1,-1,-1,-1};
	if(XPT2046_TouchDetect() == TOUCH_PRESSED)
	{
		LED_GREEN;	
		// 获取触摸坐标
		XPT2046_Get_TouchedPoint(&cinfo,strXPT2046_TouchPara);
		// 输出调试信息到串口
		XPT2046_DEBUG("x=%d,y=%d",cinfo.x,cinfo.y);
		// 调用触摸被按下时的处理函数，可在该函数编写自己的触摸按下处理过程
		XPT2046_TouchDown(&cinfo);
		/* 更新触摸信息到pre xy */
		cinfo.pre_x = cinfo.x; cinfo.pre_y = cinfo.y;  
	}
	else
	{
		LED_BLUE;
		// 调用触摸被释放时的处理函数，可在该函数编写自己的触摸释放处理过程
		XPT2046_TouchUp(&cinfo);
		/* 触笔释放，把 xy 重置为负 */
		cinfo.x = -1;
		cinfo.y = -1; 
		cinfo.pre_x = -1;
		cinfo.pre_y = -1;
	}
}