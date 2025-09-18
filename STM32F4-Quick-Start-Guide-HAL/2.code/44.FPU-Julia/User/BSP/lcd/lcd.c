#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h" 
#include "delay.h" 

// SRAM句柄，用于控制LCD
SRAM_HandleTypeDef TFTSRAM_Handler;    

// LCD的画笔颜色和背景色    
u32 POINT_COLOR = 0xFF000000; // 画笔颜色，默认黑色
u32 BACK_COLOR  = 0xFFFFFFFF; // 背景色，默认为白色

// 管理LCD重要参数，默认为竖屏
_lcd_dev lcddev;

// 写LCD寄存器指令
// regval: 寄存器地址
void LCD_WR_REG(vu16 regval)
{   
    regval = regval; // 防止编译器优化
    LCD->LCD_REG = regval; // 写入要写的寄存器序号
}

// 写LCD数据
// data: 要写入的值
void LCD_WR_DATA(vu16 data)
{   
    data = data; // 防止编译器优化
    LCD->LCD_RAM = data;
}

// 读LCD数据
// 返回值: 读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram; // 防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}    

// 写寄存器
// LCD_Reg: 寄存器地址
// LCD_RegValue: 要写入的数据
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;     // 写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue; // 写入数据     
}    

// 读寄存器
// LCD_Reg: 寄存器地址
// 返回值: 读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{    
    LCD_WR_REG(LCD_Reg); // 写入要读的寄存器序号
    delay_us(5);         // 延时等待数据稳定
    return LCD_RD_DATA(); // 返回读到的值
}   

// 准备写入GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;   
}

// LCD写入GRAM
// RGB_Code: 颜色值
void LCD_WriteRAM(u16 RGB_Code)
{     
    LCD->LCD_RAM = RGB_Code; // 写16位GRAM数据
}

// BGR格式颜色转换为RGB格式
// c: BGR格式的颜色值
// 返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
    u16  r,g,b,rgb;   
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return(rgb);
}

// 编译器优化时的延时函数
void opt_delay(u8 i)
{
    while(i--);
}

// 读取指定点的颜色值
// x,y: 坐标
// 返回值: 此点的颜色
u32 LCD_ReadPoint(u16 x,u16 y)
{
    u16 r = 0,g = 0,b = 0;
    if(x >= lcddev.width || y >= lcddev.height) return 0; // 超过范围，直接返回    
    
    LCD_SetCursor(x,y);     
    LCD_WR_REG(0X2E); // 9486发送读GRAM指令
    
    r = LCD_RD_DATA(); // dummy Read    
    opt_delay(2);   
    r = LCD_RD_DATA(); // 实际坐标颜色
    opt_delay(2);   
    b = LCD_RD_DATA();
    g = r & 0XFF; 
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); 
}

// 开启LCD显示
void LCD_DisplayOn(void)
{    
    LCD_WR_REG(0X29); // 开启显示指令
    printf("LCD Display ON (0x29).\r\n"); 
}

// 关闭LCD显示
void LCD_DisplayOff(void)
{    
    LCD_WR_REG(0X28); // 关闭显示指令
    printf("LCD Display OFF (0x28).\r\n"); 
}   

// 设置光标位置 (对于GRAM写入操作的起始点)
// Xpos: 横坐标
// Ypos: 纵坐标
// 注意：这个函数设置的是从 (Xpos, Ypos) 到 (lcddev.width-1, lcddev.height-1) 的写入窗口
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    // 参数校验：确保设置的坐标在当前显示尺寸内
    if(Xpos >= lcddev.width || Ypos >= lcddev.height) return;

    // SetColumnAddress (0x2A) 设置X起始和结束地址
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8); 
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_DATA((lcddev.width - 1) >> 8); // 结束X坐标为当前逻辑宽度最大值
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    
    // SetPageAddress (0x2B) 设置Y起始和结束地址
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8); 
    LCD_WR_DATA(Ypos & 0XFF);
    LCD_WR_DATA((lcddev.height - 1) >> 8); // 结束Y坐标为当前逻辑高度最大值
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

// 设置LCD的自动扫描方向
// dir: 0~7, 代表8个方向 (具体定义见lcd.h)
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;  

    switch(dir)
    {
        case L2R_U2D: // 从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U: // 从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D: // 从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U: // 从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R: // 从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L: // 从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R: // 从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L: // 从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }
    dirreg = 0X36; 
    regval |= 0X08; // 9486通常需要BGR设置 (这是你原始代码的设置，保留)
    LCD_WriteReg(dirreg,regval);
    
    if(regval & 0X20) // 如果设置了行列交换 (MV位为1)
    {
        if(lcddev.width < lcddev.height) 
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else // 未设置行列交换 (MV位为0)
    {
        if(lcddev.width > lcddev.height) 
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
      
    // 重新设置显示区域的X和Y范围 (很重要，这里使用更新后的 lcddev.width/height)
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

// 画点
// x,y: 坐标
// POINT_COLOR: 此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
    // 参数校验
    if(x >= lcddev.width || y >= lcddev.height) return;

    LCD_SetCursor(x,y);     // 设置光标位置
    LCD_WriteRAM_Prepare(); // 准备写入GRAM
    LCD->LCD_RAM = POINT_COLOR;  
}

// 快速画点 (关键修改在这里！)
// x,y: 坐标
// color: 颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u32 color)
{    
    // 参数校验
    if(x >= lcddev.width || y >= lcddev.height) return;

    // 针对单个像素点写入，设置精确的1x1像素窗口，而不是从当前点到右下角
    LCD_WR_REG(lcddev.setxcmd); // Column Address Set (0x2A)
    LCD_WR_DATA(x >> 8); 
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_DATA(x >> 8); // 结束X坐标与起始X相同，形成1像素宽的窗口
    LCD_WR_DATA(x & 0XFF);
    
    LCD_WR_REG(lcddev.setycmd); // Page Address Set (0x2B)
    LCD_WR_DATA(y >> 8); 
    LCD_WR_DATA(y & 0XFF);
    LCD_WR_DATA(y >> 8); // 结束Y坐标与起始Y相同，形成1像素高的窗口
    LCD_WR_DATA(y & 0XFF);

    LCD->LCD_REG = lcddev.wramcmd; // 准备写入GRAM (Write Memory Start)
    LCD->LCD_RAM = color; // 写入颜色

    // !!! 每次单点写入后，立即恢复全屏窗口 !!!
    // 这是为了确保后续的字符绘制（它们会调用此函数）不会因为之前设置的1x1窗口而受限。
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); 
}

// 设置LCD显示方向
// dir: 0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    printf("LCD_Display_Dir: Changing to direction %d.\r\n", dir); 

    if(dir == 0) // 竖屏
    {
        lcddev.dir = 0; // 竖屏模式
        lcddev.width = LCD_WIDTH_PX; 
        lcddev.height = LCD_HEIGHT_PX;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;  
        LCD_Scan_Dir(DFT_SCAN_DIR); // 应用默认扫描方向 (L2R_U2D)
    }
    else // 横屏
    {   
        lcddev.dir = 1; // 横屏模式
        lcddev.width = LCD_HEIGHT_PX; // 宽度变为竖屏时的高度
        lcddev.height = LCD_WIDTH_PX; // 高度变为竖屏时的宽度
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;  
        LCD_Scan_Dir(U2D_L2R); // 横屏通常需要行列交换
    }
    // 每次切换方向后，都确保窗口被重置为全屏
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
    printf("LCD_Display_Dir completed, current W:H = %d:%d.\r\n", lcddev.width, lcddev.height); 
}
// 设置窗口
// sx,sy: 窗口起始坐标(左上角)
// width,height: 窗口宽度和高度, 必须大于0!!
// 窗体大小: width * height
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{     
    u16 twidth = sx + width - 1;
    u16 theight = sy + height - 1;

    // 参数校验，确保窗口范围不超出屏幕逻辑尺寸
    if(sx >= lcddev.width || sy >= lcddev.height) return;
    if(twidth >= lcddev.width) twidth = lcddev.width - 1;
    if(theight >= lcddev.height) theight = lcddev.height - 1;
    if(width == 0 || height == 0 || twidth < sx || theight < sy) return; // 避免无效窗口
    
    // 9486窗口设置 (Column Address Set)
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);  
    
    // 9486窗口设置 (Page Address Set)
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);

    // printf("Set_Window: (%d,%d) to (%d,%d) for W:%d H:%d.\r\n", sx, sy, twidth, theight, lcddev.width, lcddev.height); 
}

// SRAM底层驱动，时钟使能，引脚分配
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
// 初始化LCD 
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

// 清屏函数
void LCD_Clear(u32 color)
{
    u32 totalpoint = (u32)lcddev.width * lcddev.height; 
    // 在清屏前设置全屏窗口，确保覆盖整个屏幕
    LCD_Set_Window(0,0,lcddev.width,lcddev.height); // <-- 确保这里设置了全屏窗口
    LCD_WriteRAM_Prepare();     
    for(u32 index = 0; index < totalpoint; index++) 
    {
        LCD->LCD_RAM = color; 
    }
    // 清屏后，窗口仍然保持全屏状态，无需额外恢复
    printf("Screen cleared to color 0x%X (W:%d, H:%d).\r\n", color, lcddev.width, lcddev.height); 
}  

// 在指定区域内填充单个颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{          
    if(sx >= lcddev.width || sy >= lcddev.height || sx > ex || sy > ey) return;
    if(ex >= lcddev.width) ex = lcddev.width - 1;
    if(ey >= lcddev.height) ey = lcddev.height - 1;

    u32 total_pixels = (u32)(ex - sx + 1) * (ey - sy + 1);
    if(total_pixels == 0) return;

    LCD_Set_Window(sx,sy,(ex-sx+1),(ey-sy+1)); // 设置填充区域的窗口
    LCD_WriteRAM_Prepare();    
    for(u32 i = 0; i < total_pixels; i++) 
    {
        LCD->LCD_RAM = color;     
    }
    // 填充后恢复全屏窗口
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // <-- 关键修复：填充后恢复全屏窗口
    printf("Filled rectangle (%d,%d) to (%d,%d) with color 0x%X.\r\n", sx, sy, ex, ey, color); 
}  

// 在指定区域内填充指定颜色块
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
    // 填充后恢复全屏窗口
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // <-- 关键修复：填充后恢复全屏窗口
    printf("Filled color block (%d,%d) to (%d,%d).\r\n", sx, sy, ex, ey); 
}  

// 画线
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
// 画矩形   
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1,y1,x2,y1); 
    LCD_DrawLine(x1,y1,x1,y2); 
    LCD_DrawLine(x1,y2,x2,y2); 
    LCD_DrawLine(x2,y1,x2,y2); 
}
// 在指定位置画一个指定大小的圆
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
// 在指定位置显示一个字符
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{     
    u8 temp,t1; // 删除了未使用的 't'
    u16 y0 = y;
    u16 char_width_pixels = size / 2; // 字符的像素宽度，通常是高度的一半
    
    if(num < ' ' || num > '~') return; 
    // 字符起始位置和整体是否在屏幕范围内
    if(x >= lcddev.width || y >= lcddev.height || (x + char_width_pixels) > lcddev.width || (y + size) > lcddev.height) return;

    u16 csize = ((size / 8) + ((size % 8) ? 1 : 0)) * char_width_pixels; 
    num = num - ' '; 

    // 字符点阵是按列存储，每8行一个字节。
    for(u16 t_byte_offset = 0; t_byte_offset < csize; t_byte_offset++) // 遍历字模的每个字节
    {   
        if(size == 12) temp = asc2_1206[num][t_byte_offset]; 
        else if(size == 16) temp = asc2_1608[num][t_byte_offset]; 
        else if(size == 24) temp = asc2_2412[num][t_byte_offset]; 
        else if(size == 32) temp = asc2_3216[num][t_byte_offset]; 
        else return; 

        for(t1 = 0; t1 < 8; t1++) // 遍历每个字节的8个位（像素）
        {     
            // 计算当前像素的绝对X坐标 (基于当前字节属于哪一列)
            u16 current_pixel_x = x + (t_byte_offset / (size / 8 + ((size % 8) ? 1 : 0))); // 计算当前字节属于的列
            
            // 计算当前像素的绝对Y坐标 (基于当前字节是哪一行，以及字节内的位)
            u16 current_pixel_y = y0 + (t_byte_offset % (size / 8 + ((size % 8) ? 1 : 0))) * 8 + t1;
            
            // 如果计算出来的像素超出屏幕范围，则不绘制并返回
            if(current_pixel_x >= lcddev.width || current_pixel_y >= lcddev.height) return;
            
            if(temp & 0x80) LCD_Fast_DrawPoint(current_pixel_x,current_pixel_y,POINT_COLOR);
            else if(mode == 0) LCD_Fast_DrawPoint(current_pixel_x,current_pixel_y,BACK_COLOR);
            
            temp <<= 1; 
        }  
    }              
}   
// m的n次方函数
u32 LCD_Pow(u8 m,u8 n)
{
    u32 result = 1;
    while(n--) result *= m;    
    return result;
}
// 显示数字,高位为0,则不显示
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
// 显示数字,高位为0,仍显示
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
// 显示字符串
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
    u16 x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' ')) // 判断是不是合法ASCII字符
    {       
        if(x >= width) { x = x0; y += size; } // 超出区域宽度，换行
        if(y >= height) break;                // 超出区域高度，退出
        
        LCD_ShowChar(x, y, *p, size, 0); // 显示当前字符
        x += size / 2;                   // 移动到下一个字符的X坐标
        p++;                             // 指向下一个字符
    }  
}
