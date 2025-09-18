#include "mpu.h"
#include "led.h"

/**
 * @brief       设置某个区域的MPU保护
 * @param       baseaddr: MPU保护区域的基址(首地址)
 * @param       size:MPU保护区域的大小(必须是32的倍数,单位为字节)
 * @param       rnum:MPU保护区编号,范围:0~7,最大支持8个保护区域
 * @param       de:禁止指令访问;0,允许指令访问;1,禁止指令访问
 * @param       ap:访问权限,访问关系如下:
 *   @arg       0,无访问（特权&用户都不可访问）
 *   @arg       1,仅支持特权读写访问
 *   @arg       2,禁止用户写访问（特权可读写访问）
 *   @arg       3,全访问（特权&用户都可访问）
 *   @arg       4,无法预测(禁止设置为4!!!)
 *   @arg       5,仅支持特权读访问
 *   @arg       6,只读（特权&用户都不可以写）
 *   @note      详见:STM32H7编程手册.pdf,4.6.6节,Table 91.
 * @param       sen:是否允许共用;0,不允许;1,允许
 * @param       cen:是否允许cache;0,不允许;1,允许
 * @param       ben:是否允许缓冲;0,不允许;1,允许
 * @retval      0, 成功; 1, 错误;
 */
uint8_t mpu_config(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t de, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben)
{
	MPU_Region_InitTypeDef MPU_InitStruct = {0};
	HAL_MPU_Disable(); // 关闭MPU
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = rnum; // MPU保护区编号,范围:0~7,最大支持8个保护区域
	MPU_InitStruct.BaseAddress = baseaddr; // MPU保护区域的基址(首地址)
	MPU_InitStruct.Size = size; // MPU保护区域的大小(必须是32的倍数,单位为字节)
	MPU_InitStruct.SubRegionDisable = 0x00; // 子区域禁用,0x00,表示不禁用子区域
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0; // 内存类型,详见编程手册
	MPU_InitStruct.AccessPermission = ap; // 访问权限
	MPU_InitStruct.DisableExec = de; // 禁止指令访问;0,允许指令访问;1,禁止指令访问
	MPU_InitStruct.IsShareable = sen; // 是否允许共用;0,不允许;1,允许
	MPU_InitStruct.IsCacheable = cen; // 是否允许cache;0,不允许;1,允许
	MPU_InitStruct.IsBufferable = ben; // 是否允许缓冲;0,不允许;1,允许
	HAL_MPU_ConfigRegion(&MPU_InitStruct); // 配置MPU保护区域
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT); // 使能MPU
	return 0;
}

/**
 * @brief       设置需要保护的存储块
 * @note        必须对部分存储区域进行MPU保护,否则可能导致程序运行异常
 *              比如MCU屏不显示,摄像头采集数据出错等等问题
 * @param       无
 * @retval      nbytes以2为底的指数值
 */
void mpu_mem_init(void)
{
    /* 保护整个DTCM,共128K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    mpu_config(0x20000000, MPU_REGION_SIZE_128KB, MPU_REGION_NUMBER1, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个AXI SRAM,共512K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    mpu_config(0x24000000, MPU_REGION_SIZE_512KB,MPU_REGION_NUMBER2, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个SRAM1~SRAM3,共288K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    mpu_config(0x30000000, MPU_REGION_SIZE_512KB,MPU_REGION_NUMBER3, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个SRAM4,共64K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    mpu_config(0x38000000, MPU_REGION_SIZE_64KB, MPU_REGION_NUMBER4, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护MCU LCD屏所在的FMC区域,,共64M字节,允许指令访问,禁止共用,禁止cache,禁止缓冲 */
    mpu_config(0x60000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER5, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);

    /* 保护SDRAM区域,共64M字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    mpu_config(0XC0000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER6, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个NAND FLASH区域,共256M字节,禁止指令访问,禁止共用,禁止cache,禁止缓冲 */
    mpu_config(0X80000000, MPU_REGION_SIZE_256MB, MPU_REGION_NUMBER7, MPU_INSTRUCTION_ACCESS_DISABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
	HAL_GPIO_WritePin(LED_RED_Port, LED_RED_Pin, RESET); //  MemManage错误处理中断
	HAL_Delay(1000);
	NVIC_SystemReset(); // 复位
}
