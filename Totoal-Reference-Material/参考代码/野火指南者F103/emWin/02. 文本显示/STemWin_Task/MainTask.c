/**
  *********************************************************************
  * @file    MainTask.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 工程模版
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/*******************************************************************************
 * 包含的头文件
 ******************************************************************************/
#include "MainTask.h"

/*******************************************************************************
 * 全局变量
 ******************************************************************************/
char acText[] = "This example demostrates text wrapping";
GUI_RECT rect = {10, 150, 75, 270};
GUI_WRAPMODE aWm[] = {GUI_WRAPMODE_NONE, GUI_WRAPMODE_CHAR, GUI_WRAPMODE_WORD};

/*******************************************************************************
 * 函数
 ******************************************************************************/
/**
  * @brief GUI主任务
  * @note 无
  * @param 无
  * @retval 无
  */
void MainTask(void)
{
	U8 i;
	
	/* 设置背景色 */
	GUI_SetBkColor(GUI_BLUE);	
	GUI_Clear();
	
//	/* 开启光标 */
//	GUI_CURSOR_Show();
	
	/* 设置字体大小 */
	GUI_SetFont(GUI_FONT_16_1);
	GUI_DispStringAt("STemWIN@EmbedFire STM32F103", 10, 10);
	
	/* 画线 */
	GUI_SetPenSize(8);
	GUI_SetColor(GUI_RED);
	GUI_DrawLine(10, 40, 230, 120);
	GUI_DrawLine(10, 120, 230, 40);
	
	/* 绘制文本 */
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_16B_ASCII);
	/* 正常模式 */
	GUI_SetTextMode(GUI_TM_NORMAL);
	GUI_DispStringHCenterAt("GUI_TM_NORMAL" , 120, 40);
	/* 反转显示 */
	GUI_SetTextMode(GUI_TM_REV);
	GUI_DispStringHCenterAt("GUI_TM_REV" , 120, 40 + 16);
	/* 透明文本 */
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt("GUI_TM_TRANS" , 120, 40 + 16 * 2);
	/* 异或文本 */
	GUI_SetTextMode(GUI_TM_XOR);
	GUI_DispStringHCenterAt("GUI_TM_XOR" , 120, 40 + 16 * 3);
	/* 透明反转文本 */
	GUI_SetTextMode(GUI_TM_TRANS | GUI_TM_REV);
	GUI_DispStringHCenterAt("GUI_TM_TRANS | GUI_TM_REV", 120, 40 + 16 * 4);
	
	/* 在矩形区域内显示文本 */
	GUI_SetFont(GUI_FONT_16B_ASCII);
	GUI_SetTextMode(GUI_TM_TRANS);
	for(i = 0;i < 3;i++)
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRectEx(&rect);
		GUI_SetColor(GUI_RED);
		GUI_DispStringInRectWrap(acText, &rect, GUI_TA_LEFT, aWm[i]);
		rect.x0 += 75;
		rect.x1 += 75;
	}
	
	while(1)
	{
		GUI_Delay(100);
	}
}
