/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：串口IAP 实验-IAP Bootloader V1.0_ITCM
 * 实验平台：正点原子 Mini Pro H750开发板
 * 实验目的：学习STM32的IAP功能,掌握IAP的使用

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 RGB灯
     RED   : LED0 - PB4
 * 2 独立按键
     KEY0  - PA1
     KEY1  - PA15
     WK_UP  - PA0
 * 3 串口1 (PA9/PA10连接在板载USB转串口芯片CH340上面)
 * 4 ALIENTEK 2.8/3.5/4.3/7/10寸TFTLCD模块(仅限MCU屏，16位8080并口驱动)
 * 5 QSPI(PB2/PB6/PD11/PD12/PD13/PE2)
 * 6 norflash(QSPI FLASH芯片,连接在QSPI上)

 ***************************************************************************************************
 * 实验现象
 * 1 IAP部分,包括两份代码:
     IAP Bootloader V1.0_FLASH, 用于下载到目标开发板,执行IAP BootLoader.
     IAP Bootloader V1.0_ITCM, 用于生成具体的IAP Bootloader动作,实现IAP过程.

     IAP Bootloader V1.0_ITCM工程生成.bin文件需要通过ATK-C2B软件转成数组形式,然后把数组拷贝到IAP 
     Bootloader V1.0_FLASH的bl_itcm.c,替换里面的acApp2数组内容,然后编译IAP Bootloader V1.0_FLASH,
     并下载到STM32开发板上.

     IAP Bootloader V1.0_FLASH工程实际上只是完成了将IAP Bootloader V1.0_ITCM拷贝到ITCM并运行的工作.
     IAP Bootloader V1.0_ITCM工程生成的BIN文件转成数组后由IAP Bootloader V1.0_FLASH工程存储到外部
     QSPI FLASH.

     本实验开机的时候先显示提示信息，然后等待串口输入接收APP程序（无校验，一次性接收），在串口接收
     到APP程序之后，即可执行IAP。

     1,对于SRAM APP，通过按下KEY0即可执行这个收到的SRAM APP程序。

     2,对于FLASH APP，则需要按下KEY1按键，将串口接收到的APP程序存放到STM32的FLASH，之后会自动
     执行这个FLASH APP程序。

     3,对于FLASH&QSPI APP,则会生成两个.bin文件(ER_m_qspiflash和ER_m_stmflash),先接收ER_m_qspiflash固
     件,按KEY_UP按键，将串口接收到的ER_m_qspiflash存放到外部的QSPI FLASH(BY25Q128)，然后再接收
     ER_m_stmflash,按KEY1将串口接收到的ER_m_qspiflash存放到STM32的内部FLASH,并执行这个FLASH&QSPI APP程序.


 ***************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 P5的PA9/PA10必须通过跳线帽连接在RXD/TXD上
 * 6 本例程仅支持MCU屏，不支持RGB屏
 * 7 本实验下载成功后,需用串口调试助手发送SRAM APP/FLASH APP等APP代码(.bin文件)验证IAP功能
 * 8 SRAM APP代码的起始地址必须是：0X24001000，FLASH APP代码的起始地址必须是：0x08004000
 * 9 在创建APP代码的时候，切记要设置中断向量偏移量
 
 ***********************************************************************************************************
 * 公司名称：广州市星翼电子科技有限公司（正点原子）
 * 电话号码：020-38271790
 * 传真号码：020-36773971
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 最新资料：www.openedv.com/docs/index.html
 *
 * 在线视频：www.yuanzige.com
 * B 站视频：space.bilibili.com/394620890
 * 公 众 号：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * 抖    音：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */