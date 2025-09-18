#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h" 
#include "delay.h" 

// SRAM��������ڿ���LCD
SRAM_HandleTypeDef TFTSRAM_Handler;    

// LCD�Ļ�����ɫ�ͱ���ɫ    
u32 POINT_COLOR = 0xFF000000; // ������ɫ��Ĭ�Ϻ�ɫ
u32 BACK_COLOR  = 0xFFFFFFFF; // ����ɫ��Ĭ��Ϊ��ɫ

// ����LCD��Ҫ������Ĭ��Ϊ����
_lcd_dev lcddev;

// дLCD�Ĵ���ָ��
// regval: �Ĵ�����ַ
void LCD_WR_REG(vu16 regval)
{   
    regval = regval; // ��ֹ�������Ż�
    LCD->LCD_REG = regval; // д��Ҫд�ļĴ������
}

// дLCD����
// data: Ҫд���ֵ
void LCD_WR_DATA(vu16 data)
{   
    data = data; // ��ֹ�������Ż�
    LCD->LCD_RAM = data;
}

// ��LCD����
// ����ֵ: ������ֵ
u16 LCD_RD_DATA(void)
{
    vu16 ram; // ��ֹ���Ż�
    ram = LCD->LCD_RAM;
    return ram;
}    

// д�Ĵ���
// LCD_Reg: �Ĵ�����ַ
// LCD_RegValue: Ҫд�������
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;     // д��Ҫд�ļĴ������
    LCD->LCD_RAM = LCD_RegValue; // д������     
}    

// ���Ĵ���
// LCD_Reg: �Ĵ�����ַ
// ����ֵ: ����������
u16 LCD_ReadReg(u16 LCD_Reg)
{    
    LCD_WR_REG(LCD_Reg); // д��Ҫ���ļĴ������
    delay_us(5);         // ��ʱ�ȴ������ȶ�
    return LCD_RD_DATA(); // ���ض�����ֵ
}   

// ׼��д��GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;   
}

// LCDд��GRAM
// RGB_Code: ��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{     
    LCD->LCD_RAM = RGB_Code; // д16λGRAM����
}

// BGR��ʽ��ɫת��ΪRGB��ʽ
// c: BGR��ʽ����ɫֵ
// ����ֵ��RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
    u16  r,g,b,rgb;   
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return(rgb);
}

// �������Ż�ʱ����ʱ����
void opt_delay(u8 i)
{
    while(i--);
}

// ��ȡָ�������ɫֵ
// x,y: ����
// ����ֵ: �˵����ɫ
u32 LCD_ReadPoint(u16 x,u16 y)
{
    u16 r = 0,g = 0,b = 0;
    if(x >= lcddev.width || y >= lcddev.height) return 0; // ������Χ��ֱ�ӷ���    
    
    LCD_SetCursor(x,y);     
    LCD_WR_REG(0X2E); // 9486���Ͷ�GRAMָ��
    
    r = LCD_RD_DATA(); // dummy Read    
    opt_delay(2);   
    r = LCD_RD_DATA(); // ʵ��������ɫ
    opt_delay(2);   
    b = LCD_RD_DATA();
    g = r & 0XFF; 
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); 
}

// ����LCD��ʾ
void LCD_DisplayOn(void)
{    
    LCD_WR_REG(0X29); // ������ʾָ��
    printf("LCD Display ON (0x29).\r\n"); 
}

// �ر�LCD��ʾ
void LCD_DisplayOff(void)
{    
    LCD_WR_REG(0X28); // �ر���ʾָ��
    printf("LCD Display OFF (0x28).\r\n"); 
}   

// ���ù��λ�� (����GRAMд���������ʼ��)
// Xpos: ������
// Ypos: ������
// ע�⣺����������õ��Ǵ� (Xpos, Ypos) �� (lcddev.width-1, lcddev.height-1) ��д�봰��
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    // ����У�飺ȷ�����õ������ڵ�ǰ��ʾ�ߴ���
    if(Xpos >= lcddev.width || Ypos >= lcddev.height) return;

    // SetColumnAddress (0x2A) ����X��ʼ�ͽ�����ַ
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8); 
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_DATA((lcddev.width - 1) >> 8); // ����X����Ϊ��ǰ�߼�������ֵ
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    
    // SetPageAddress (0x2B) ����Y��ʼ�ͽ�����ַ
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8); 
    LCD_WR_DATA(Ypos & 0XFF);
    LCD_WR_DATA((lcddev.height - 1) >> 8); // ����Y����Ϊ��ǰ�߼��߶����ֵ
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

// ����LCD���Զ�ɨ�跽��
// dir: 0~7, ����8������ (���嶨���lcd.h)
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;  

    switch(dir)
    {
        case L2R_U2D: // ������,���ϵ���
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U: // ������,���µ���
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D: // ���ҵ���,���ϵ���
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U: // ���ҵ���,���µ���
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R: // ���ϵ���,������
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L: // ���ϵ���,���ҵ���
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R: // ���µ���,������
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L: // ���µ���,���ҵ���
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }
    dirreg = 0X36; 
    regval |= 0X08; // 9486ͨ����ҪBGR���� (������ԭʼ��������ã�����)
    LCD_WriteReg(dirreg,regval);
    
    if(regval & 0X20) // ������������н��� (MVλΪ1)
    {
        if(lcddev.width < lcddev.height) 
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else // δ�������н��� (MVλΪ0)
    {
        if(lcddev.width > lcddev.height) 
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
      
    // ����������ʾ�����X��Y��Χ (����Ҫ������ʹ�ø��º�� lcddev.width/height)
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);  

    printf("Scan_Dir: 0x36 set to 0x%X (dir=%d), Current W:H = %d:%d.\r\n", regval, dir, lcddev.width, lcddev.height); 
}     

// ����
// x,y: ����
// POINT_COLOR: �˵����ɫ
void LCD_DrawPoint(u16 x,u16 y)
{
    // ����У��
    if(x >= lcddev.width || y >= lcddev.height) return;

    LCD_SetCursor(x,y);     // ���ù��λ��
    LCD_WriteRAM_Prepare(); // ׼��д��GRAM
    LCD->LCD_RAM = POINT_COLOR;  
}

// ���ٻ��� (�ؼ��޸������)
// x,y: ����
// color: ��ɫ
void LCD_Fast_DrawPoint(u16 x,u16 y,u32 color)
{    
    // ����У��
    if(x >= lcddev.width || y >= lcddev.height) return;

    // ��Ե������ص�д�룬���þ�ȷ��1x1���ش��ڣ������Ǵӵ�ǰ�㵽���½�
    LCD_WR_REG(lcddev.setxcmd); // Column Address Set (0x2A)
    LCD_WR_DATA(x >> 8); 
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_DATA(x >> 8); // ����X��������ʼX��ͬ���γ�1���ؿ�Ĵ���
    LCD_WR_DATA(x & 0XFF);
    
    LCD_WR_REG(lcddev.setycmd); // Page Address Set (0x2B)
    LCD_WR_DATA(y >> 8); 
    LCD_WR_DATA(y & 0XFF);
    LCD_WR_DATA(y >> 8); // ����Y��������ʼY��ͬ���γ�1���ظߵĴ���
    LCD_WR_DATA(y & 0XFF);

    LCD->LCD_REG = lcddev.wramcmd; // ׼��д��GRAM (Write Memory Start)
    LCD->LCD_RAM = color; // д����ɫ

    // !!! ÿ�ε���д��������ָ�ȫ������ !!!
    // ����Ϊ��ȷ���������ַ����ƣ����ǻ���ô˺�����������Ϊ֮ǰ���õ�1x1���ڶ����ޡ�
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); 
}

// ����LCD��ʾ����
// dir: 0,������1,����
void LCD_Display_Dir(u8 dir)
{
    printf("LCD_Display_Dir: Changing to direction %d.\r\n", dir); 

    if(dir == 0) // ����
    {
        lcddev.dir = 0; // ����ģʽ
        lcddev.width = LCD_WIDTH_PX; 
        lcddev.height = LCD_HEIGHT_PX;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;  
        LCD_Scan_Dir(DFT_SCAN_DIR); // Ӧ��Ĭ��ɨ�跽�� (L2R_U2D)
    }
    else // ����
    {   
        lcddev.dir = 1; // ����ģʽ
        lcddev.width = LCD_HEIGHT_PX; // ��ȱ�Ϊ����ʱ�ĸ߶�
        lcddev.height = LCD_WIDTH_PX; // �߶ȱ�Ϊ����ʱ�Ŀ��
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;  
        LCD_Scan_Dir(U2D_L2R); // ����ͨ����Ҫ���н���
    }
    // ÿ���л�����󣬶�ȷ�����ڱ�����Ϊȫ��
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
    printf("LCD_Display_Dir completed, current W:H = %d:%d.\r\n", lcddev.width, lcddev.height); 
}
// ���ô���
// sx,sy: ������ʼ����(���Ͻ�)
// width,height: ���ڿ�Ⱥ͸߶�, �������0!!
// �����С: width * height
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{     
    u16 twidth = sx + width - 1;
    u16 theight = sy + height - 1;

    // ����У�飬ȷ�����ڷ�Χ��������Ļ�߼��ߴ�
    if(sx >= lcddev.width || sy >= lcddev.height) return;
    if(twidth >= lcddev.width) twidth = lcddev.width - 1;
    if(theight >= lcddev.height) theight = lcddev.height - 1;
    if(width == 0 || height == 0 || twidth < sx || theight < sy) return; // ������Ч����
    
    // 9486�������� (Column Address Set)
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);  
    
    // 9486�������� (Page Address Set)
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);

    // printf("Set_Window: (%d,%d) to (%d,%d) for W:%d H:%d.\r\n", sx, sy, twidth, theight, lcddev.width, lcddev.height); 
}

// SRAM�ײ�������ʱ��ʹ�ܣ����ŷ���
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_FSMC_CLK_ENABLE(); 
    __HAL_RCC_GPIOD_CLK_ENABLE(); 
    __HAL_RCC_GPIOE_CLK_ENABLE(); 
    __HAL_RCC_GPIOF_CLK_ENABLE(); 
    __HAL_RCC_GPIOG_CLK_ENABLE(); 
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | \
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;   
    GPIO_Initure.Pull = GPIO_PULLUP;       
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;  
    GPIO_Initure.Alternate = GPIO_AF12_FSMC; 
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                       GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    GPIO_Initure.Pin = GPIO_PIN_12; 
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    GPIO_Initure.Pin = GPIO_PIN_12; 
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    printf("HAL_SRAM_MspInit: FSMC GPIOs Configured.\r\n"); 
}
// ��ʼ��LCD 
void LCD_Init(void)
{   
    GPIO_InitTypeDef GPIO_Initure;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
    FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
    
    __HAL_RCC_GPIOB_CLK_ENABLE(); 
    GPIO_Initure.Pin = GPIO_PIN_15;           
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, RESET);
    printf("LCD_Init: Starting LCD Initialization...\r\n"); 

    TFTSRAM_Handler.Instance = FSMC_NORSRAM_DEVICE;                
    TFTSRAM_Handler.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;    
    TFTSRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK4;                  
    TFTSRAM_Handler.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; 
    TFTSRAM_Handler.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;          
    TFTSRAM_Handler.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; 
    TFTSRAM_Handler.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; 
    TFTSRAM_Handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; 
    TFTSRAM_Handler.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;   
    TFTSRAM_Handler.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;    
    TFTSRAM_Handler.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;           
    TFTSRAM_Handler.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;        
    TFTSRAM_Handler.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; 
    TFTSRAM_Handler.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;           
    TFTSRAM_Handler.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC; 
    
    FSMC_ReadWriteTim.AddressSetupTime = 0x0F;        
    FSMC_ReadWriteTim.AddressHoldTime = 0;
    FSMC_ReadWriteTim.DataSetupTime = 60;             
    FSMC_ReadWriteTim.AccessMode = FSMC_ACCESS_MODE_A; 
    
    FSMC_WriteTim.BusTurnAroundDuration = 0;          
    FSMC_WriteTim.AddressSetupTime = 9;               
    FSMC_WriteTim.AddressHoldTime = 0;
    FSMC_WriteTim.DataSetupTime = 17;                 
    FSMC_WriteTim.AccessMode = FSMC_ACCESS_MODE_A;    
    
    if (HAL_SRAM_Init(&TFTSRAM_Handler,&FSMC_ReadWriteTim,&FSMC_WriteTim) != HAL_OK) 
    {
        printf("HAL_SRAM_Init FAILED! Program halted.\r\n");
        while(1); 
    }
    delay_ms(50); 
    
    LCD_WR_REG(0XD3);    
    LCD_RD_DATA(); 
    LCD_RD_DATA(); 
    u16 temp_id_high = LCD_RD_DATA();    
    u16 temp_id_low = LCD_RD_DATA();    
    lcddev.id = (temp_id_high << 8) | temp_id_low; 
    lcddev.id = 0x9486; 
    
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); 
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); 
    FSMC_Bank1E->BWTR[6] |= 3 << 0;      
    FSMC_Bank1E->BWTR[6] |= 2 << 8;      
    printf("LCD ID:%x\r\n",lcddev.id); 
    
    printf("Starting 9486 Initialization Sequence...\r\n"); 
    LCD_WR_REG(0XF2);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0xA3);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0XB2);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0x00);
    
    LCD_WR_REG(0XF8); 
    LCD_WR_DATA(0x21); 
    LCD_WR_DATA(0x04);
    
    LCD_WR_REG(0XF9); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x08);
    
    LCD_WR_REG(0X36); 
    LCD_WR_DATA(0X08); 
    printf("Sent Memory Access Control (0x36) to 0x08.\r\n"); 
    
    LCD_WR_REG(0X3A); 
    LCD_WR_DATA(0x05); 
    printf("Sent Pixel Format Set (0x3A) to 0x05 (RGB565).\r\n"); 
    
    LCD_WR_REG(0XB4); 
    LCD_WR_DATA(0x01); 
    
    LCD_WR_REG(0XB6); 
    LCD_WR_DATA(0X02); 
    LCD_WR_DATA(0x22); 
    
    LCD_WR_REG(0XC1); 
    LCD_WR_DATA(0X41); 
    
    LCD_WR_REG(0XC5); 
    LCD_WR_DATA(0X00); 
    LCD_WR_DATA(0x07); 
    
    LCD_WR_REG(0XE0); 
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x1F);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0X0F);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0X98);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0X0D);
    LCD_WR_DATA(0x00);
    
    LCD_WR_REG(0XE1); 
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x2E);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0X0D);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x47);
    LCD_WR_DATA(0X75);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0X20);
    LCD_WR_DATA(0x00);
    printf("Sent Gamma Correction commands.\r\n"); 
    
    LCD_WR_REG(0x11); // Exit Sleep mode
    delay_ms(120);    
    printf("Sent Sleep Out (0x11).\r\n"); 
    
    LCD_WR_REG(0x29); // Turn on Display
    printf("Sent Display On (0x29).\r\n"); 
    
    LCD_Display_Dir(0); 
    printf("LCD_Display_Dir(0) called for initial setup.\r\n"); 

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, SET);       
    printf("LCD_LED set to HIGH (Backlight ON).\r\n"); 

    LCD_Clear(WHITE);   
    printf("LCD Clear to WHITE after init.\r\n"); 
    printf("LCD_Init: Initialization sequence completed.\r\n"); 
}  

// ��������
void LCD_Clear(u32 color)
{
    u32 totalpoint = (u32)lcddev.width * lcddev.height; 
    // ������ǰ����ȫ�����ڣ�ȷ������������Ļ
    LCD_Set_Window(0,0,lcddev.width,lcddev.height); // <-- ȷ������������ȫ������
    LCD_WriteRAM_Prepare();     
    for(u32 index = 0; index < totalpoint; index++) 
    {
        LCD->LCD_RAM = color; 
    }
    // �����󣬴�����Ȼ����ȫ��״̬���������ָ�
    printf("Screen cleared to color 0x%X (W:%d, H:%d).\r\n", color, lcddev.width, lcddev.height); 
}  

// ��ָ����������䵥����ɫ
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{          
    if(sx >= lcddev.width || sy >= lcddev.height || sx > ex || sy > ey) return;
    if(ex >= lcddev.width) ex = lcddev.width - 1;
    if(ey >= lcddev.height) ey = lcddev.height - 1;

    u32 total_pixels = (u32)(ex - sx + 1) * (ey - sy + 1);
    if(total_pixels == 0) return;

    LCD_Set_Window(sx,sy,(ex-sx+1),(ey-sy+1)); // �����������Ĵ���
    LCD_WriteRAM_Prepare();    
    for(u32 i = 0; i < total_pixels; i++) 
    {
        LCD->LCD_RAM = color;     
    }
    // ����ָ�ȫ������
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // <-- �ؼ��޸�������ָ�ȫ������
    printf("Filled rectangle (%d,%d) to (%d,%d) with color 0x%X.\r\n", sx, sy, ex, ey, color); 
}  

// ��ָ�����������ָ����ɫ��
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
    if(sx >= lcddev.width || sy >= lcddev.height || sx > ex || sy > ey) return;
    if(ex >= lcddev.width) ex = lcddev.width - 1;
    if(ey >= lcddev.height) ey = lcddev.height - 1;

    u32 total_pixels = (u32)(ex - sx + 1) * (ey - sy + 1);
    if(total_pixels == 0) return;

    LCD_Set_Window(sx,sy,(ex-sx+1),(ey-sy+1));    
    LCD_WriteRAM_Prepare();      
    for(u32 k = 0; k < total_pixels; k++) 
    {
        LCD->LCD_RAM = color[k]; 
    }
    // ����ָ�ȫ������
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // <-- �ؼ��޸�������ָ�ȫ������
    printf("Filled color block (%d,%d) to (%d,%d).\r\n", sx, sy, ex, ey); 
}  

// ����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; 
    delta_y = y2 - y1; 
    uRow = x1;
    uCol = y1;
    if(delta_x > 0) incx = 1;        
    else if(delta_x == 0) incx = 0;  
    else { incx = -1; delta_x = -delta_x; }
    if(delta_y > 0) incy = 1;        
    else if(delta_y == 0) incy = 0;  
    else { incy = -1; delta_y = -delta_y; }
    if(delta_x > delta_y) distance = delta_x; 
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++) 
    {  
        LCD_DrawPoint(uRow,uCol); 
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }  
}    
// ������   
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1,y1,x2,y1); 
    LCD_DrawLine(x1,y1,x1,y2); 
    LCD_DrawLine(x1,y2,x2,y2); 
    LCD_DrawLine(x2,y1,x2,y2); 
}
// ��ָ��λ�û�һ��ָ����С��Բ
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
    int a,b;
    int di;
    a=0;b=r;   
    di=3-(r<<1); 
    while(a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);             
        LCD_DrawPoint(x0 + b, y0 - a);             
        LCD_DrawPoint(x0 + b, y0 + a);             
        LCD_DrawPoint(x0 + a, y0 + b);             
        LCD_DrawPoint(x0 - a, y0 + b);             
        LCD_DrawPoint(x0 - b, y0 + a);             
        LCD_DrawPoint(x0 - a, y0 - b);             
        LCD_DrawPoint(x0 - b, y0 - a);             
        
        a++;
        if(di < 0) di += 4*a + 6;   
        else
        {
            di += 10 + 4*(a - b);   
            b--;
        }     
    }
}   
// ��ָ��λ����ʾһ���ַ�
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{     
    u8 temp,t1; // ɾ����δʹ�õ� 't'
    u16 y0 = y;
    u16 char_width_pixels = size / 2; // �ַ������ؿ�ȣ�ͨ���Ǹ߶ȵ�һ��
    
    if(num < ' ' || num > '~') return; 
    // �ַ���ʼλ�ú������Ƿ�����Ļ��Χ��
    if(x >= lcddev.width || y >= lcddev.height || (x + char_width_pixels) > lcddev.width || (y + size) > lcddev.height) return;

    u16 csize = ((size / 8) + ((size % 8) ? 1 : 0)) * char_width_pixels; 
    num = num - ' '; 

    // �ַ������ǰ��д洢��ÿ8��һ���ֽڡ�
    for(u16 t_byte_offset = 0; t_byte_offset < csize; t_byte_offset++) // ������ģ��ÿ���ֽ�
    {   
        if(size == 12) temp = asc2_1206[num][t_byte_offset]; 
        else if(size == 16) temp = asc2_1608[num][t_byte_offset]; 
        else if(size == 24) temp = asc2_2412[num][t_byte_offset]; 
        else if(size == 32) temp = asc2_3216[num][t_byte_offset]; 
        else return; 

        for(t1 = 0; t1 < 8; t1++) // ����ÿ���ֽڵ�8��λ�����أ�
        {     
            // ���㵱ǰ���صľ���X���� (���ڵ�ǰ�ֽ�������һ��)
            u16 current_pixel_x = x + (t_byte_offset / (size / 8 + ((size % 8) ? 1 : 0))); // ���㵱ǰ�ֽ����ڵ���
            
            // ���㵱ǰ���صľ���Y���� (���ڵ�ǰ�ֽ�����һ�У��Լ��ֽ��ڵ�λ)
            u16 current_pixel_y = y0 + (t_byte_offset % (size / 8 + ((size % 8) ? 1 : 0))) * 8 + t1;
            
            // ���������������س�����Ļ��Χ���򲻻��Ʋ�����
            if(current_pixel_x >= lcddev.width || current_pixel_y >= lcddev.height) return;
            
            if(temp & 0x80) LCD_Fast_DrawPoint(current_pixel_x,current_pixel_y,POINT_COLOR);
            else if(mode == 0) LCD_Fast_DrawPoint(current_pixel_x,current_pixel_y,BACK_COLOR);
            
            temp <<= 1; 
        }  
    }              
}   
// m��n�η�����
u32 LCD_Pow(u8 m,u8 n)
{
    u32 result = 1;
    while(n--) result *= m;    
    return result;
}
// ��ʾ����,��λΪ0,����ʾ
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         
    u8 t,temp;
    u8 enshow = 0;    
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}
// ��ʾ����,��λΪ0,����ʾ
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
    u8 t,temp;
    u8 enshow = 0;    
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode & 0X80) LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);  
                else LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);  
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
    }
}
// ��ʾ�ַ���
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
    u16 x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' ')) // �ж��ǲ��ǺϷ�ASCII�ַ�
    {       
        if(x >= width) { x = x0; y += size; } // ���������ȣ�����
        if(y >= height) break;                // ��������߶ȣ��˳�
        
        LCD_ShowChar(x, y, *p, size, 0); // ��ʾ��ǰ�ַ�
        x += size / 2;                   // �ƶ�����һ���ַ���X����
        p++;                             // ָ����һ���ַ�
    }  
}
