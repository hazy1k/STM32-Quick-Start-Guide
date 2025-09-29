本二维码&条码识别LIB由ALIENTEK提供，方便大家使用。

该二维码&条码识别LIB功能：
1，支持QR二维码识别。
2，支持CODE128、CODE39、I25、EAN13等四种编码方式的条码识别。
3，支持UTF8-OEM转换输出（需要客户自己提供转换码表）。
	
本LIB移植步骤：	
1，实现atk_qrdecode.c里面的所有函数。
2，堆栈（Stack_Size）设置为0X1000或以上。
 	   
本LIB使用步骤：					   
1，调用atk_qr_init函数，初始化识别程序，返回值为ATK_QR_OK，则初始化成功。
2，调用atk_qr_decode函数，给定参数，对图像进行识别。
3，如果需要不停的识别，则重复第2个步骤即可。
4，调用atk_qr_destroy函数，结束识别，释放所有内存，结束识别。




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