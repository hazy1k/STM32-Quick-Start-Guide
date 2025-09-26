#include "bsp_key.h" 
#include <includes.h>   //使用uC/OS内核机制须包含该头文件



static void Key1_Config ( void );
static void Key2_Config ( void );
//static void Key_Delay   ( volatile uint32_t ulCount );



void Key_Initial ( void )
{
	Key1_Config ();
	Key2_Config ();
}


static void Key1_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	

	RCC_APB2PeriphClockCmd ( macKEY1_GPIO_CLK, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = macKEY1_GPIO_PIN; 

	GPIO_InitStructure.GPIO_Mode = macKEY1_GPIO_Mode; 
	
	GPIO_Init ( macKEY1_GPIO_PORT, & GPIO_InitStructure );
}


static void Key2_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	

	RCC_APB2PeriphClockCmd ( macKEY2_GPIO_CLK, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = macKEY2_GPIO_PIN; 

	GPIO_InitStructure.GPIO_Mode = macKEY2_GPIO_Mode; 
	
	GPIO_Init ( macKEY2_GPIO_PORT, & GPIO_InitStructure );
}


//static void Key_Delay ( volatile uint32_t ulCount )
//{
//	for(; ulCount != 0; ulCount--);
//} 


/**
  * @brief  检测按键是否被单击
  * @param  GPIOx ：按键所在的端口
  * @param  GPIO_Pin ：按键所在的管脚
  * @param  ucPushState ：按键被按下时的电平极性
  *   该参数为以下值之一：
  *     @arg 1 :高电平
  *     @arg 0 :低电平
  * @param  pKeyPress ：用于存放按键是否被按下过，须是静态变量	
  * @retval 按键是否被单击
	*   该返回值为以下值之一：
  *     @arg 1 :按键被单击
  *     @arg 0 :按键未被单击
  */
uint8_t Key_Scan ( GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin, uint8_t ucPushState, uint8_t * pKeyPress )
{			
	uint8_t ucKeyState, ucRet = 0;
	
	OS_ERR      err;
	
	ucKeyState = GPIO_ReadInputDataBit ( GPIOx, GPIO_Pin );
	
	if ( ucKeyState == ucPushState )
	{
//		Key_Delay( 10000 );	         //改成uC/OS的延时函数
		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );
		
		ucKeyState = GPIO_ReadInputDataBit ( GPIOx, GPIO_Pin );
		
		if ( ucKeyState == ucPushState )
			* pKeyPress = 1;
	}

	if ( ( ucKeyState != ucPushState ) && ( * pKeyPress == 1 ) )
	{
		ucRet = 1;
		* pKeyPress = 0;
	}
	
	return ucRet;
	
}


/**
  * @brief  检测按键的状态
  * @param  GPIOx ：按键所在的端口
  * @param  GPIO_Pin ：按键所在的管脚
  * @param  ucPushState ：按键被按下时的电平极性
  *   该参数为以下值之一：
  *     @arg 1 :高电平
  *     @arg 0 :低电平
  * @retval 按键的当前状态
	*   该返回值为以下值之一：
  *     @arg 1 :按键被按下
  *     @arg 0 :按键被释放
  */
uint8_t Key_ReadStatus ( GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin, uint8_t ucPushState )
{			
	uint8_t ucKeyState;
	
	
	ucKeyState = GPIO_ReadInputDataBit ( GPIOx, GPIO_Pin );
	
	if ( ucKeyState == ucPushState )
		return 1;
	else 
		return 0;

}


/*********************************************END OF FILE**********************/
