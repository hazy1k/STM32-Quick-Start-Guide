#include "ili9341.h"
#include "fonts.h"	

uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;
uint8_t LCD_SCAN_MODE = 6;
uint16_t lcdid = LCDID_UNKNOWN;

static sFONT *LCD_Currentfonts = &Font8x16; // 英文字体
static uint16_t CurrentTextColor = BLACK;   // 前景色
static uint16_t CurrentBackColor = WHITE;   // 背景色

void ILI9341_Write_Cmd(uint16_t usCmd);
void ILI9341_Write_Data(uint16_t usData);
uint16_t ILI9341_Read_Data(void);
static void ILI9341_Delay(__IO uint32_t nCount);
static void ILI9341_GPIO_Config(void);
static void ILI9341_FSMC_Config(void);
static void ILI9341_REG_Config(void);
static void ILI9341_SetCursor(uint16_t usX, uint16_t usY);
static void ILI9341_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
static uint16_t ILI9341_Read_PixelData(void);

void ILI9341_Write_Cmd(uint16_t usCmd)
{
    *(__IO uint16_t *)(FSMC_Addr_ILI9341_CMD) = usCmd;  // 写命令
}

void ILI9341_Write_Data(uint16_t usData)
{
    *(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA) = usData;  // 写数据
}

uint16_t ILI9341_Read_Data(void)
{
    return *(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA);  // 读取数据
}

static void ILI9341_Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);  // 延时
}

static void ILI9341_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(ILI9341_CS_CLK | ILI9341_DC_CLK | ILI9341_WR_CLK |
                           ILI9341_RD_CLK | ILI9341_BK_CLK | ILI9341_RST_CLK |
                           ILI9341_D0_CLK | ILI9341_D1_CLK | ILI9341_D2_CLK |
                           ILI9341_D3_CLK | ILI9341_D4_CLK | ILI9341_D5_CLK |
                           ILI9341_D6_CLK | ILI9341_D7_CLK | ILI9341_D8_CLK |
                           ILI9341_D9_CLK | ILI9341_D10_CLK | ILI9341_D11_CLK |
                           ILI9341_D12_CLK | ILI9341_D13_CLK | ILI9341_D14_CLK |
                           ILI9341_D15_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
    GPIO_Init(ILI9341_D0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
    GPIO_Init(ILI9341_D1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D2_PIN;
    GPIO_Init(ILI9341_D2_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D3_PIN;
    GPIO_Init(ILI9341_D3_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D4_PIN;
    GPIO_Init(ILI9341_D4_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D5_PIN;
    GPIO_Init(ILI9341_D5_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D6_PIN;
    GPIO_Init(ILI9341_D6_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D7_PIN;
    GPIO_Init(ILI9341_D7_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D8_PIN;
    GPIO_Init(ILI9341_D8_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D9_PIN;
    GPIO_Init(ILI9341_D9_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D10_PIN;
    GPIO_Init(ILI9341_D10_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D11_PIN;
    GPIO_Init(ILI9341_D11_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D12_PIN;
    GPIO_Init(ILI9341_D12_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D13_PIN;
    GPIO_Init(ILI9341_D13_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D14_PIN;
    GPIO_Init(ILI9341_D14_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_D15_PIN;
    GPIO_Init(ILI9341_D15_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN;
    GPIO_Init(ILI9341_RD_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN;
    GPIO_Init(ILI9341_WR_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
    GPIO_Init(ILI9341_CS_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
    GPIO_Init(ILI9341_DC_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
    GPIO_Init(ILI9341_RST_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
    GPIO_Init(ILI9341_BK_PORT, &GPIO_InitStructure);
}

static void ILI9341_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure={0};
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming={0}; 	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	readWriteTiming.FSMC_AddressSetupTime      = 0x01;	 
	readWriteTiming.FSMC_DataSetupTime         = 0x04;
	readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B;	
	readWriteTiming.FSMC_AddressHoldTime       = 0x00;	
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision           = 0x00;	
	readWriteTiming.FSMC_DataLatency           = 0x00;	
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming;  
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  
}

static void ILI9341_REG_Config(void)
{
    lcdid = ILI9341_ReadID();

    if (lcdid == LCDID_ILI9341) {
        // Power control B (CFh)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xCF);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x81);
        ILI9341_Write_Data(0x30);
        // Power on sequence control (EDh)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xED);
        ILI9341_Write_Data(0x64);
        ILI9341_Write_Data(0x03);
        ILI9341_Write_Data(0x12);
        ILI9341_Write_Data(0x81);
        // Driver timing control A (E8h)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xE8);
        ILI9341_Write_Data(0x85);
        ILI9341_Write_Data(0x10);
        ILI9341_Write_Data(0x78);
        // Power control A (CBh)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xCB);
        ILI9341_Write_Data(0x39);
        ILI9341_Write_Data(0x2C);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x34);
        ILI9341_Write_Data(0x06);  // Prevent stripes during white screen
        // Pump ratio control (F7h)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xF7);
        ILI9341_Write_Data(0x20);
        // Driver timing control B (EA)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xEA);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x00);
        // Frame Rate Control (B1h)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xB1);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x1B);
        // Display Function Control (B6h)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xB6);
        ILI9341_Write_Data(0x0A);
        ILI9341_Write_Data(0xA2);
        // Power Control 1 (C0h)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xC0);
        ILI9341_Write_Data(0x35);
        // Power Control 2 (C1h)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xC1);
        ILI9341_Write_Data(0x11);
        // VCOM Control 1 (C5h)
        ILI9341_Write_Cmd(0xC5);
        ILI9341_Write_Data(0x45);
        ILI9341_Write_Data(0x45);
        // VCOM Control 2 (C7h)
        ILI9341_Write_Cmd(0xC7);
        ILI9341_Write_Data(0xA2);
        // Enable 3G (F2h)
        ILI9341_Write_Cmd(0xF2);
        ILI9341_Write_Data(0x00);
        // Gamma Set (26h)
        ILI9341_Write_Cmd(0x26);
        ILI9341_Write_Data(0x01);
        DEBUG_DELAY();
        // Positive Gamma Correction (E0h)
        ILI9341_Write_Cmd(0xE0);
        ILI9341_Write_Data(0x0F);
        ILI9341_Write_Data(0x26);
        ILI9341_Write_Data(0x24);
        ILI9341_Write_Data(0x0B);
        ILI9341_Write_Data(0x0E);
        ILI9341_Write_Data(0x09);
        ILI9341_Write_Data(0x54);
        ILI9341_Write_Data(0xA8);
        ILI9341_Write_Data(0x46);
        ILI9341_Write_Data(0x0C);
        ILI9341_Write_Data(0x17);
        ILI9341_Write_Data(0x09);
        ILI9341_Write_Data(0x0F);
        ILI9341_Write_Data(0x07);
        ILI9341_Write_Data(0x00);
        // Negative Gamma Correction (E1h)
        ILI9341_Write_Cmd(0xE1);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x19);
        ILI9341_Write_Data(0x1B);
        ILI9341_Write_Data(0x04);
        ILI9341_Write_Data(0x10);
        ILI9341_Write_Data(0x07);
        ILI9341_Write_Data(0x2A);
        ILI9341_Write_Data(0x47);
        ILI9341_Write_Data(0x39);
        ILI9341_Write_Data(0x03);
        ILI9341_Write_Data(0x06);
        ILI9341_Write_Data(0x06);
        ILI9341_Write_Data(0x30);
        ILI9341_Write_Data(0x38);
        ILI9341_Write_Data(0x0F);
        // Memory Access Control (36h) for portrait mode
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0x36);
        ILI9341_Write_Data(0xC8);  // Portrait orientation: top-left to bottom-right
        DEBUG_DELAY();
        // Column address control (Set X coordinates)
        ILI9341_Write_Cmd(CMD_SetCoordinateX);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0xEF);
        // Page address control (Set Y coordinates)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(CMD_SetCoordinateY);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0x01);
        ILI9341_Write_Data(0x3F);
        // Pixel Format (3Ah)
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0x3A);
        ILI9341_Write_Data(0x55);
        // Sleep Out (11h)
        ILI9341_Write_Cmd(0x11);
        ILI9341_Delay(0xAFFf << 2);
        DEBUG_DELAY();
        // Display ON (29h)
        ILI9341_Write_Cmd(0x29);
    } else if (lcdid == LCDID_ST7789V) {
        // Similar configuration for ST7789V controller, but with different parameters
        // The process is similar, with necessary adjustments for this display
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0xCF);
        ILI9341_Write_Data(0x00);
        ILI9341_Write_Data(0xC1);
        ILI9341_Write_Data(0x30);
        // Power on sequence, timings, gamma corrections, etc. are similar to the ILI9341 setup above.
        ILI9341_Write_Cmd(0x11);  // Exit Sleep
        ILI9341_Delay(0xAFFf << 2);
        DEBUG_DELAY();
        ILI9341_Write_Cmd(0x29);  // Display ON
        ILI9341_Write_Cmd(0x2C);  // Begin writing to display
    }
}
void ILI9341_Init(void)
{
	ILI9341_GPIO_Config();
	ILI9341_FSMC_Config();	
	ILI9341_BackLed_Control(ENABLE);
	ILI9341_Rst();
	ILI9341_REG_Config();  
	ILI9341_GramScan(LCD_SCAN_MODE);
}
void ILI9341_BackLed_Control(FunctionalState enumState)
{
    if (enumState)
        GPIO_ResetBits(ILI9341_BK_PORT, ILI9341_BK_PIN);
    else
        GPIO_SetBits(ILI9341_BK_PORT, ILI9341_BK_PIN);
}
uint16_t ILI9341_ReadID(void)
{
    uint16_t id = 0;

    ILI9341_Write_Cmd(0x04);
    ILI9341_Read_Data();
    ILI9341_Read_Data();
    id = ILI9341_Read_Data();
    id = (id << 8) | ILI9341_Read_Data();
    if (id == LCDID_ST7789V)
        return id;
    ILI9341_Write_Cmd(0xD3);
    ILI9341_Read_Data();
    ILI9341_Read_Data();
    id = ILI9341_Read_Data();
    id = (id << 8) | ILI9341_Read_Data();
    return (id == LCDID_ILI9341) ? id : LCDID_UNKNOWN;
}
void ILI9341_Rst(void)
{
    GPIO_ResetBits(ILI9341_RST_PORT, ILI9341_RST_PIN);
    ILI9341_Delay(0xAFF);
    GPIO_SetBits(ILI9341_RST_PORT, ILI9341_RST_PIN);
    ILI9341_Delay(0xAFF);
}
void ILI9341_GramScan(uint8_t ucOption)
{
    if (ucOption > 7) return;
    LCD_SCAN_MODE = ucOption;
    if (ucOption % 2 == 0) {
        LCD_X_LENGTH = ILI9341_LESS_PIXEL;
        LCD_Y_LENGTH = ILI9341_MORE_PIXEL;
    } else {
        LCD_X_LENGTH = ILI9341_MORE_PIXEL;
        LCD_Y_LENGTH = ILI9341_LESS_PIXEL;
    }
    ILI9341_Write_Cmd(0x36);
    if (lcdid == LCDID_ILI9341) {
        ILI9341_Write_Data(0x08 | (ucOption << 5));
    } else if (lcdid == LCDID_ST7789V) {
        ILI9341_Write_Data(0x00 | (ucOption << 5));
    }
    ILI9341_Write_Cmd(CMD_SetCoordinateX);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(((LCD_X_LENGTH - 1) >> 8) & 0xFF);
    ILI9341_Write_Data((LCD_X_LENGTH - 1) & 0xFF);
    ILI9341_Write_Cmd(CMD_SetCoordinateY);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(((LCD_Y_LENGTH - 1) >> 8) & 0xFF);
    ILI9341_Write_Data((LCD_Y_LENGTH - 1) & 0xFF);
    ILI9341_Write_Cmd(CMD_SetPixel);
}
void ILI9341_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
    ILI9341_Write_Cmd(CMD_SetCoordinateX);
    ILI9341_Write_Data(usX >> 8);
    ILI9341_Write_Data(usX & 0xFF);
    ILI9341_Write_Data((usX + usWidth - 1) >> 8);
    ILI9341_Write_Data((usX + usWidth - 1) & 0xFF);
    ILI9341_Write_Cmd(CMD_SetCoordinateY);
    ILI9341_Write_Data(usY >> 8);
    ILI9341_Write_Data(usY & 0xFF);
    ILI9341_Write_Data((usY + usHeight - 1) >> 8);
    ILI9341_Write_Data((usY + usHeight - 1) & 0xFF);
}
static void ILI9341_SetCursor(uint16_t usX, uint16_t usY)
{
    ILI9341_OpenWindow(usX, usY, 1, 1);
}
static void ILI9341_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
    uint32_t i;
    ILI9341_Write_Cmd(CMD_SetPixel);
    for (i = 0; i < ulAmout_Point; i++)
        ILI9341_Write_Data(usColor);
}
void ILI9341_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
    ILI9341_OpenWindow(usX, usY, usWidth, usHeight);
    ILI9341_FillColor(usWidth * usHeight, CurrentBackColor);
}
void ILI9341_SetPointPixel(uint16_t usX, uint16_t usY)
{
    if ((usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH)) {
        ILI9341_SetCursor(usX, usY);
        ILI9341_FillColor(1, CurrentTextColor);
    }
}
static uint16_t ILI9341_Read_PixelData(void)
{
    uint16_t usRG = 0, usB = 0;
    ILI9341_Write_Cmd(0x2E);
    ILI9341_Read_Data(); 
    usRG = ILI9341_Read_Data(); 
    usB = ILI9341_Read_Data();
    return ((usRG & 0xF800) | ((usRG << 3) & 0x7E0) | (usB >> 11));
}
uint16_t ILI9341_GetPointPixel(uint16_t usX, uint16_t usY)
{
    uint16_t usPixelData;
    ILI9341_SetCursor(usX, usY);
    usPixelData = ILI9341_Read_PixelData();
    return usPixelData;
}
void ILI9341_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
    uint16_t us;
    uint16_t usX_Current, usY_Current;
    int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance;
    int32_t lIncrease_X, lIncrease_Y;
    lDelta_X = usX2 - usX1;
    lDelta_Y = usY2 - usY1;
    usX_Current = usX1;
    usY_Current = usY1;
    if (lDelta_X > 0)
        lIncrease_X = 1;
    else if (lDelta_X == 0)
        lIncrease_X = 0;
    else {
        lIncrease_X = -1;
        lDelta_X = -lDelta_X;
    }
    if (lDelta_Y > 0)
        lIncrease_Y = 1;
    else if (lDelta_Y == 0)
        lIncrease_Y = 0;
    else {
        lIncrease_Y = -1;
        lDelta_Y = -lDelta_Y;
    }
    lDistance = (lDelta_X > lDelta_Y) ? lDelta_X : lDelta_Y;
    for (us = 0; us <= lDistance + 1; us++) {
        ILI9341_SetPointPixel(usX_Current, usY_Current);
        lError_X += lDelta_X;
        lError_Y += lDelta_Y;
        if (lError_X > lDistance) {
            lError_X -= lDistance;
            usX_Current += lIncrease_X;
        }
        if (lError_Y > lDistance) {
            lError_Y -= lDistance;
            usY_Current += lIncrease_Y;
        }
    }
}
void ILI9341_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
	if (ucFilled)
	{
		ILI9341_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
		ILI9341_FillColor(usWidth * usHeight, CurrentTextColor);
	}
	else
	{
		ILI9341_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
		ILI9341_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
		ILI9341_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
		ILI9341_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
	}
}
void ILI9341_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled)
{
	int16_t sCurrentX = 0, sCurrentY = usRadius, sError = 3 - (usRadius << 1);
	while (sCurrentX <= sCurrentY)
	{
		int16_t sCountY;
		if (ucFilled)
			for (sCountY = sCurrentX; sCountY <= sCurrentY; sCountY++)
			{
				ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCountY);
				ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCountY);
				ILI9341_SetPointPixel(usX_Center - sCountY, usY_Center + sCurrentX);
				ILI9341_SetPointPixel(usX_Center - sCountY, usY_Center - sCurrentX);
				ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCountY);
				ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCountY);
				ILI9341_SetPointPixel(usX_Center + sCountY, usY_Center - sCurrentX);
				ILI9341_SetPointPixel(usX_Center + sCountY, usY_Center + sCurrentX);
			}
		else
		{
			ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCurrentY);
			ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCurrentY);
			ILI9341_SetPointPixel(usX_Center - sCurrentY, usY_Center + sCurrentX);
			ILI9341_SetPointPixel(usX_Center - sCurrentY, usY_Center - sCurrentX);
			ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCurrentY);
			ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCurrentY);
			ILI9341_SetPointPixel(usX_Center + sCurrentY, usY_Center - sCurrentX);
			ILI9341_SetPointPixel(usX_Center + sCurrentY, usY_Center + sCurrentX);
		}
		sCurrentX++;
		if (sError < 0)
			sError += 4 * sCurrentX + 6;
		else
		{
			sError += 10 + 4 * (sCurrentX - sCurrentY);
			sCurrentY--;
		}
	}
}
void ILI9341_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
	uint8_t byteCount, bitCount, fontLength;
	uint16_t ucRelativePositon = cChar - ' ';
	fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;
	uint8_t *Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	ILI9341_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
	ILI9341_Write_Cmd(CMD_SetPixel);
	for (byteCount = 0; byteCount < fontLength; byteCount++)
	{
		for (bitCount = 0; bitCount < 8; bitCount++)
		{
			if (Pfont[byteCount] & (0x80 >> bitCount))
				ILI9341_Write_Data(CurrentTextColor);
			else
				ILI9341_Write_Data(CurrentBackColor);
		}
	}
}
void ILI9341_DispStringLine_EN(uint16_t line, char *pStr) {
    uint16_t usX = 0;
    while (*pStr != '\0') {
        if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            line += LCD_Currentfonts->Height;
        }
        if ((line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            line = ILI9341_DispWindow_Y_Star;
        }
        ILI9341_DispChar_EN(usX, line, *pStr);
        pStr++;
        usX += LCD_Currentfonts->Width;
    }
}
void ILI9341_DispString_EN(uint16_t usX, uint16_t usY, char *pStr) {
    while (*pStr != '\0') {
        if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            usY += LCD_Currentfonts->Height;
        }
        if ((usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            usY = ILI9341_DispWindow_Y_Star;
        }
        ILI9341_DispChar_EN(usX, usY, *pStr);
        pStr++;
        usX += LCD_Currentfonts->Width;
    }
}
void ILI9341_DispString_EN_YDir(uint16_t usX, uint16_t usY, char *pStr) {
    while (*pStr != '\0') {
        if ((usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
            usY = ILI9341_DispWindow_Y_Star;
            usX += LCD_Currentfonts->Width;
        }
        if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            usY = ILI9341_DispWindow_Y_Star;
        }
        ILI9341_DispChar_EN(usX, usY, *pStr);
        pStr++;
        usY += LCD_Currentfonts->Height;
    }
}
void ILI9341_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar) {
    uint8_t ucBuffer[WIDTH_CH_CHAR * HEIGHT_CH_CHAR / 8];
    uint16_t usTemp;
    ILI9341_OpenWindow(usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR);
    ILI9341_Write_Cmd(CMD_SetPixel);
    GetGBKCode(ucBuffer, usChar);
    for (uint8_t rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++) {
        usTemp = ucBuffer[rowCount * 2] << 8 | ucBuffer[rowCount * 2 + 1];
        for (uint8_t bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount++) {
            ILI9341_Write_Data((usTemp & (0x8000 >> bitCount)) ? CurrentTextColor : CurrentBackColor);
        }
    }
}
void ILI9341_DispString_CH(uint16_t usX, uint16_t usY, char *pStr) {
    while (*pStr != '\0') {
        if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            usY += HEIGHT_CH_CHAR;
        }
        if ((usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            usY = ILI9341_DispWindow_Y_Star;
        }
        uint16_t usCh = *((uint16_t *)pStr);
        usCh = (usCh << 8) | (usCh >> 8);
        ILI9341_DispChar_CH(usX, usY, usCh);
        usX += WIDTH_CH_CHAR;
        pStr += 2;
    }
}
void ILI9341_DispStringLine_EN_CH(uint16_t line, char *pStr)
{
    uint16_t usCh, usX = 0;
    while (*pStr != '\0') {
        if (*pStr <= 126) {
            if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                line += LCD_Currentfonts->Height;
            }
            if ((line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                line = ILI9341_DispWindow_Y_Star;
            }
            ILI9341_DispChar_EN(usX, line, *pStr);
            usX += LCD_Currentfonts->Width;
            pStr++;
        } else {
            if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                line += HEIGHT_CH_CHAR;
            }
            if ((line - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                line = ILI9341_DispWindow_Y_Star;
            }
            usCh = *(uint16_t *)pStr;
            usCh = (usCh << 8) + (usCh >> 8);
            ILI9341_DispChar_CH(usX, line, usCh);
            usX += WIDTH_CH_CHAR;
            pStr += 2;
        }
    }
}
void ILI9341_DispString_EN_CH(uint16_t usX, uint16_t usY, char *pStr)
{
    uint16_t usCh;
    while (*pStr != '\0') {
        if (*pStr <= 126) {
            if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                usY += LCD_Currentfonts->Height;
            }
            if ((usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                usY = ILI9341_DispWindow_Y_Star;
            }
            ILI9341_DispChar_EN(usX, usY, *pStr);
            usX += LCD_Currentfonts->Width;
            pStr++;
        } else {
            if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                usY += HEIGHT_CH_CHAR;
            }
            if ((usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH) {
                usX = ILI9341_DispWindow_X_Star;
                usY = ILI9341_DispWindow_Y_Star;
            }
            usCh = *(uint16_t *)pStr;
            usCh = (usCh << 8) + (usCh >> 8);
            ILI9341_DispChar_CH(usX, usY, usCh);
            usX += WIDTH_CH_CHAR;
            pStr += 2;
        }
    }
}
void ILI9341_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY, char *pStr)
{
    uint16_t usCh;
    while (*pStr != '\0') {
        if ((usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH) {
            usY = ILI9341_DispWindow_Y_Star;
            usX += WIDTH_CH_CHAR;
        }
        if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH) {
            usX = ILI9341_DispWindow_X_Star;
            usY = ILI9341_DispWindow_Y_Star;
        }
        if (*pStr <= 126) {
            ILI9341_DispChar_EN(usX, usY, *pStr);
            pStr++;
            usY += HEIGHT_CH_CHAR;
        } else {
            usCh = *(uint16_t *)pStr;
            usCh = (usCh << 8) + (usCh >> 8);
            ILI9341_DispChar_CH(usX, usY, usCh);
            usY += HEIGHT_CH_CHAR;
            pStr += 2;
        }
    }
}

#define ZOOMMAXBUFF 16384
uint8_t zoomBuff[ZOOMMAXBUFF] = {0};
uint8_t zoomTempBuff[1024] = {0};

void ILI9341_zoomChar(uint16_t in_width, uint16_t in_heig, uint16_t out_width, uint16_t out_heig, uint8_t *in_ptr, uint8_t *out_ptr, uint8_t en_cn) {
    uint8_t *pts, *ots;
    unsigned int xrIntFloat_16 = (in_width << 16) / out_width + 1;
    unsigned int yrIntFloat_16 = (in_heig << 16) / out_heig + 1;
    unsigned int srcy_16 = 0, y, x;
    uint8_t *pSrcLine;
    uint16_t byteCount, bitCount;
    if (in_width >= 32 || in_width * in_heig == 0 || in_width * in_heig >= 1024 || out_width * out_heig == 0 || out_width * out_heig >= ZOOMMAXBUFF) return;
    pts = (uint8_t*)&zoomTempBuff;
    for (byteCount = 0; byteCount < in_heig * in_width / 8; byteCount++) {
        for (bitCount = 0; bitCount < 8; bitCount++) {
            *pts++ = (in_ptr[byteCount] & (0x80 >> bitCount)) ? 1 : 0;
        }
    }
    pts = (uint8_t*)&zoomTempBuff;
    ots = (uint8_t*)&zoomBuff;
    for (y = 0; y < out_heig; y++) {
        unsigned int srcx_16 = 0;
        pSrcLine = pts + in_width * (srcy_16 >> 16);
        for (x = 0; x < out_width; x++) {
            ots[x] = pSrcLine[srcx_16 >> 16];
            srcx_16 += xrIntFloat_16;
        }
        srcy_16 += yrIntFloat_16;
        ots += out_width;
    }
    out_ptr = (uint8_t*)&zoomBuff;
	out_ptr++; // 防止warning
}
void ILI9341_DrawChar_Ex(uint16_t usX, uint16_t usY, uint16_t Font_width, uint16_t Font_Height, uint8_t *c, uint16_t DrawModel) {
    uint32_t index = 0, counter = 0;
    ILI9341_OpenWindow(usX, usY, Font_width, Font_Height);
    ILI9341_Write_Cmd(CMD_SetPixel);
    for (index = 0; index < Font_Height; index++) {
        for (counter = 0; counter < Font_width; counter++) {
            if (*c++ == DrawModel) ILI9341_Write_Data(CurrentBackColor);
            else ILI9341_Write_Data(CurrentTextColor);
        }
    }
}
void ILI9341_DisplayStringEx(uint16_t x, uint16_t y, uint16_t Font_width, uint16_t Font_Height, uint8_t *ptr, uint16_t DrawModel)
{
    uint16_t Charwidth = Font_width;
    uint8_t *psr;
    uint8_t Ascii;
    uint16_t usCh;
    uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];

    while (*ptr != '\0')
    {
        if ((x + Charwidth) > LCD_X_LENGTH)
        {
            x = ILI9341_DispWindow_X_Star;
            y += Font_Height;
        }
        if ((y + Font_Height) > LCD_Y_LENGTH)
        {
            x = ILI9341_DispWindow_X_Star;
            y = ILI9341_DispWindow_Y_Star;
        }
        if (*ptr > 0x80)
        {
            usCh = *(uint16_t *)ptr;
            usCh = (usCh << 8) + (usCh >> 8);
            GetGBKCode(ucBuffer, usCh);
            ILI9341_zoomChar(WIDTH_CH_CHAR, HEIGHT_CH_CHAR, Charwidth, Font_Height, ucBuffer, psr, 1);
            ILI9341_DrawChar_Ex(x, y, Charwidth, Font_Height, (uint8_t *)&zoomBuff, DrawModel);
            x += Charwidth;
            ptr += 2;
        }
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32;
            ILI9341_zoomChar(16, 24, Charwidth, Font_Height, (uint8_t *)&Font16x24.table[Ascii * Font16x24.Height * Font16x24.Width / 8], psr, 0);
            ILI9341_DrawChar_Ex(x, y, Charwidth, Font_Height, (uint8_t *)&zoomBuff, DrawModel);
            x += Charwidth;
            ptr++;
        }
    }
}
void ILI9341_DisplayStringEx_YDir(uint16_t x, uint16_t y, uint16_t Font_width, uint16_t Font_Height, uint8_t *ptr, uint16_t DrawModel)
{
    uint16_t Charwidth = Font_width;
    uint8_t *psr;
    uint8_t Ascii;
    uint16_t usCh;
    uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];

    while (*ptr != '\0')
    {
        if ((y + Font_width) > LCD_X_LENGTH)
        {
            y = ILI9341_DispWindow_X_Star;
            x += Font_width;
        }
        if ((x + Font_Height) > LCD_Y_LENGTH)
        {
            y = ILI9341_DispWindow_X_Star;
            x = ILI9341_DispWindow_Y_Star;
        }
        if (*ptr > 0x80)
        {
            usCh = *(uint16_t *)ptr;
            usCh = (usCh << 8) + (usCh >> 8);
            GetGBKCode(ucBuffer, usCh);
            ILI9341_zoomChar(WIDTH_CH_CHAR, HEIGHT_CH_CHAR, Charwidth, Font_Height, ucBuffer, psr, 1);
            ILI9341_DrawChar_Ex(x, y, Charwidth, Font_Height, (uint8_t *)&zoomBuff, DrawModel);
            y += Font_Height;
            ptr += 2;
        }
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32;
            ILI9341_zoomChar(16, 24, Charwidth, Font_Height, (uint8_t *)&Font16x24.table[Ascii * Font16x24.Height * Font16x24.Width / 8], psr, 0);
            ILI9341_DrawChar_Ex(x, y, Charwidth, Font_Height, (uint8_t *)&zoomBuff, DrawModel);
            y += Font_Height;
            ptr++;
        }
    }
}
void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}
sFONT *LCD_GetFont(void)
{
    return LCD_Currentfonts;
}
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor)
{
    CurrentTextColor = TextColor;
    CurrentBackColor = BackColor;
}
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
    *TextColor = CurrentTextColor;
    *BackColor = CurrentBackColor;
}
void LCD_SetTextColor(uint16_t Color)
{
    CurrentTextColor = Color;
}
void LCD_SetBackColor(uint16_t Color)
{
    CurrentBackColor = Color;
}
void LCD_ClearLine(uint16_t Line)
{
    ILI9341_Clear(0, Line, LCD_X_LENGTH, ((sFONT *)LCD_GetFont())->Height);
}
