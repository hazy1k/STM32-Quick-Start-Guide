    该文件夹下面存放驱动层代码，包括：
    1，BSP：正点原子提供的板级支持包驱动代码（原HARDWARE文件夹下的代码），如：LED、BEEP、KEY、EXTI、TIMER、WDG...等
    2，CMSISI：ARM提供的CMSIS代码（主要包括各种头文件和启动文件（.s文件），为了方便使用，减少占用空间，我们进行了精简）	      3，STM32H7xx_HAL_Driver：ST提供的H7 HAL库驱动代码
    4，SYSTEM：正点原子提供的系统级核心驱动代码，包括sys、delay和usart三个子文件夹，分别提供时钟配置，延时和串口驱动等关键代码，方便大家构建工程

 
 

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