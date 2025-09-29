本程序只供学习使用，未经作者许可，不得用于其它任何用途

本人脸识别程序由ALIENTEK提供,我们提供2个LIB(即:ATKFREC_DF.lib和ATKFREC_SF.lib),供大家使用 
ATKFREC_DF.lib:适合具有双精度硬件浮点计算单元的STM32F7系列，比如STM32F767/769等 
ATKFREC_SF.lib:适合具有单精度硬件浮点计算单元的STM32F4/F7系列，比如STM32F407/429/746等
请选择合适的lib进行测试！

功能:利用摄像头,实现人脸识别.
说明:本识别库,需要用到内存管理,内存总占用数在560KB左右(20个人脸).每增加一张人脸,内存占增加10KB左右.
限制:由于本识别库以M3/M4为目标处理器,内存有限,算法上进行了大量阉割,所以,很多功能不太完善,效果也不是
     很好.且没有做识别有效检查(输入错误的人脸,也会有结果输出).所以,次代码,仅供大家参考用.

其他需求:
1,摄像头模块一个.
2,SD卡一张

使用方法:					   
第一步:调用atk_frec_initialization函数,初始化人脸识别库
第二步:调用atk_frec_add_a_face函数,添加人脸模板(如果已经有了,可以忽略次步)
第三步:调用atk_frec_load_data_model函数,加载所有模板到内存里面(仅在添加新模板后需要,如没有添加新模板,则可忽略此步)
第四步:调用atk_frec_recognition_face函数,获取识别结果.
第五步:调用atk_frec_delete_data函数,可以删除一个人脸模板
第六步:如果不想再用识别库,则调用atk_frec_destroy函数,释放所有内存,结束人脸识别.

版本:V1.1
V1.1 20160830
1,针对单精度和双精度硬件浮点，改为2个lib（ATKFREC_DF.lib和ATKFREC_SF.lib）.
2,新增atk_frec_fastmalloc和atk_frec_fastfree函数


正点原子@ALIENTEK
Copyright(C) 广州市星翼电子科技有限公司 2009-2019
All rights reserved													    
版权所有，盗版必究。


		广州市星翼电子科技有限公司
                电话：020-38271790
                传真：020-36773971
	       	购买：http://shop62103354.taobao.com
                      http://shop62057469.taobao.com
               	技术支持论坛：www.openedv.com
                公司网址：www.alientek.com