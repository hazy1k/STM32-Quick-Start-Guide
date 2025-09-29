本数字字母识别程序由ALIENTEK提供，我们提供2个LIB供大家使用。
ATKNCR_M_Vx.x.lib和ATKNCR_N_Vx.x.lib的唯一区别是是否使用动态内存分配。
其中：M，代表需要用到malloc的版本，必须实现alientek_ncr_malloc和alientek_ncr_free两个函数。
          N，代表普通版本，不需要实现alientek_ncr_malloc和alientek_ncr_free两个函数。
          Vx.x，代表当前识别程序的版本。	
	 	  
功能：支持数字/小写字母/大写字母/混合四种识别模式。
		  
本识别程序使用起来相当简单。					   
第一步：调用alientek_ncr_init函数,初始化识别程序。
第二步：获取输入的点阵数据（必须有2个及以上的不同点阵数据输入）。
第三步：调用alientek_ncr函数,得到识别结果。
第四步：如果不需要再识别，则调用alientek_ncr_stop函数，终止识别。如果还需要继续，则重复2，3步即可。	   
当前版本：V2.0




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