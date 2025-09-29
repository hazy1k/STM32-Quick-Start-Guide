/*----------------------------------------------*/
/* TJpgDec System Configurations R0.03          */
/*----------------------------------------------*/

#define	JD_SZBUF        1024
/* Specifies size of stream input buffer */

#define JD_FORMAT       1
/* Specifies output pixel format.
/  0: RGB888 (24-bit/pix)
/  1: RGB565 (16-bit/pix)
/  2: Grayscale (8-bit/pix)
*/

#define	JD_USE_SCALE    1
/* Switches output descaling feature.
/  0: Disable
/  1: Enable
*/

#define JD_TBLCLIP      1
/* Use table conversion for saturation arithmetic. A bit faster, but increases 1 KB of code size.
/  0: Disable
/  1: Enable
*/

#define JD_FASTDECODE   2
/* Optimization level
/  0: Basic optimization. Suitable for 8/16-bit MCUs.
/  1: + 32-bit barrel shifter. Suitable for 32-bit MCUs.
/  2: + Table conversion for huffman decoding (wants 6 << HUFF_BIT bytes of RAM)
*/

/******************************************************************************************/
/* 用户配置区(正点原子团队添加) */
/* R0.03相较于 R0.01C 速度提升巨大, 在F4上, R0.01C 解码800*480图片需要950ms左右 
 * R0.03 设置JD_FASTDECODE = 2 时, 解码同一张图片, 只需要620ms, 提升35%.
 * 再开启-O2优化, 解码同一张图片, 只需要504ms, 提升47%.
 * H750由于tjpg.c是存放在外部SPI FLASH,导致里面的加速数组也是放在外部SPI FLASH，所以性能几乎没有提升，甚至还下降了
 * 如果想提升，可以把tjpgd.o放到内部FLASH（修改分散加载文件），则速度会有提升
 */

#define JPEG_USE_MALLOC     1               /* 定义是否使用malloc,这里我们选择使用malloc */
#define JPEG_WBUF_SIZE      6144 + 4096     /* 定义工作区数组大小,最少应不小于3092字节, 如果JD_FASTDECODE==2, 则需要增加 6144字节内存 */

/******************************************************************************************/










