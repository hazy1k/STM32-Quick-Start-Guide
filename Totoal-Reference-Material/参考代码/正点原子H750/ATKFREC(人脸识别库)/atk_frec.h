#ifndef __ATK_FREC_H
#define __ATK_FREC_H
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//本人脸识别程序由ALIENTEK提供,我们提供2个LIB(即:ATKFREC_DF.lib和ATKFREC_SF.lib),供大家使用 
//ATKFREC_DF.lib:适合具有双精度硬件浮点计算单元的STM32F7系列，比如STM32F767/769等 
//ATKFREC_SF.lib:适合具有单精度硬件浮点计算单元的STM32F4/F7系列，比如STM32F407/429/746等
//请选择合适的lib进行测试！

//功能:利用摄像头,实现人脸识别.
//说明:本识别库,需要用到内存管理,内存总占用数在560KB左右(20个人脸).每增加一张人脸,内存占增加10KB左右.
//限制:由于本识别库以M3/M4/M7为目标处理器,内存有限,算法上进行了大量阉割,所以,很多功能不太完善,效果也不
//     是很好.且没有做识别有效检查(输入错误的人脸,也会有结果输出).所以,次代码,仅供大家参考用.
//
//其他需求:
//1,摄像头模块一个.
//2,SD卡一张
//
//使用方法:					   
//第一步:调用atk_frec_initialization函数,初始化人脸识别库
//第二步:调用atk_frec_add_a_face函数,添加人脸模板(如果已经有了,可以忽略次步)
//第三步:调用atk_frec_load_data_model函数,加载所有模板到内存里面(仅在添加新模板后需要,如没有添加新模板,则可忽略此步)
//第四步:调用atk_frec_recognition_face函数,获取识别结果.
//第五步:调用atk_frec_delete_data函数,可以删除一个人脸模板
//第六步:如果不想再用识别库,则调用atk_frec_destroy函数,释放所有内存,结束人脸识别.
//
//版本:V1.1
//正点原子@ALIENTEK
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved													    								  
//版权所有，盗版必究。
//****************************************************************************************************************
//升级说明
//V1.1 20160830
//1,针对单精度和双精度硬件浮点，改为2个lib（ATKFREC_DF.lib和ATKFREC_SF.lib）.
//2,新增atk_frec_fastmalloc和atk_frec_fastfree函数
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//以下三个宏,用户可以自己修改.
#define MAX_LEBEL_NUM 				20								//定义最大可以支持多少张脸识别,每增加1,内存占用增加10KB
#define ATK_FREC_DATA_PDIR 			"0:ATK_FREC"					//数据模型存储文件夹
#define ATK_FREC_DATA_PNAME 		"0:ATK_FREC/MODEL_%03d.fea"		//数据模型存储文件夹+文件名


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//以下宏定义,用户无需修改
//错误代码
#define ATK_FREC_OK					0								//执行正常
#define ATK_FREC_MEMORY_ERR			1								//内存错误
#define ATK_FREC_FACE_FULL_ERR		2  								//人脸模板数据已满
#define ATK_FREC_READ_WRITE_ERR		3 								//数据存储/读取失败
#define ATK_FREC_MODEL_DATA_ERR		4								//没有可用的模板,需要先添加人脸模板 
#define ATK_FREC_UNREC_FACE_ERR		5								//无法识别该人脸
//定义人脸识别用于识别的图像尺寸(此值请不要修改!!)
#define ATK_GABOR_IMG_WID			30								//用于识别的图像宽度
#define ATK_GABOR_IMG_HEI			40								//用于识别的图像高度

//人脸识别模型结构体
typedef struct RecogModelStruct
{
	unsigned char count;
	unsigned char *validSample;
	float *feature;
	float **models;
}RecogModel;
extern RecogModel model;					//识别模型
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//以下5个函数,存放在.lib里面,用户只需直接调用即可.

//该函数用于初始化人脸识别,返回0表示正常,其他错误代码.
unsigned char atk_frec_initialization(void);										//初始化人脸识别

//该函数用于结束人脸识别,释放所有内存.
void atk_frec_destroy(void);														//结束人脸识别,释放所有内存

//该函数添加一个人脸模板
//data:图像原始数据,RGB565格式,尺寸必须是:ATK_GABOR_IMG_WID*ATK_GABOR_IMG_HEI
//     像素颜色值存放顺序为:从左到右(ATK_GABOR_IMG_WID),从上到下(ATK_GABOR_IMG_HEI)
//index:存储的头像索引,0~MAX_LEBEL_NUM-1,正常;0XFF,存储失败
//返回值:0,正常
//    其他,错误代码 
unsigned char atk_frec_add_a_face(unsigned short *data,unsigned char *index);		//添加一个人脸

//该函数用于识别一个人脸
//data:图像原始数据,RGB565格式,尺寸必须是:ATK_GABOR_IMG_WID*ATK_GABOR_IMG_HEI
//     像素颜色值存放顺序为:从左到右(ATK_GABOR_IMG_WID),从上到下(ATK_GABOR_IMG_HEI)
//findex:识别出的人脸索引编号(范围:0~MAX_LEBEL_NUM-1,0XFF,表示失败)
//返回值:0,正常
//    其他,错误代码
unsigned char atk_frec_recognition_face(unsigned short *data,unsigned char *findex);//识别一个人脸

//该函数用于加载人脸数据模型
//在添加了新人脸以后,通过该函数可以加载到内存
void atk_frec_load_data_model(void);												//加载人脸识别数据模型

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//以下6个函数,需要用户自己实现
void atk_frec_memset(char *p,char c,unsigned long len);	//内存设置
void *atk_frec_fastmalloc(unsigned int size);			//快速访问内存申请
void atk_frec_fastfree(void *ptr);						//快速访问内存释放
void *atk_frec_malloc(unsigned int size);				//内存申请
void atk_frec_free(void *ptr);							//内存释放
unsigned char atk_frec_save_data(unsigned char index,unsigned char* buf,unsigned int size);	//保存数据
unsigned char atk_frec_read_data(unsigned char index,unsigned char* buf,unsigned int size);	//读取数据
unsigned char atk_frec_delete_data(unsigned char index);									//删除一个人脸数据

#endif


























