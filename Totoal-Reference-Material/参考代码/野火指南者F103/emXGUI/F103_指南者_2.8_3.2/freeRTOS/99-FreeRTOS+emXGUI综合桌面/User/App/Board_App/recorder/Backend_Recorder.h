#ifndef __RECORD_H__
#define __RECORD_H__

#include <inttypes.h>
#include "emXGUI_Arch.h"
#include  "VS1053.h"

/* RECBUFFER_SIZE决定两个缓存区大小，实际每个缓存区占用RAM空间字节数为：RECBUFFER_SIZE*2
 * 对于选择I2S_AudioFreq_16k采样率使用RECBUFFER_SIZE为1024*4，就可保证录音及放音效果流畅，
 * 如果选择其他更高采样率需要适当调整用RECBUFFER_SIZE大小，比如当选择择I2S_AudioFreq_44k
 * 是设置RECBUFFER_SIZE为1024*8
 * 另外，SD卡本身的读写速度也在一定性能上影响录音效果
 */
#define RECBUFFER_SIZE  1024*8

/* 录音文件存放路径 */
#define RECORDERDIR	"0:/recorder"

extern uint8_t Replay;
extern uint8_t RecordFlag;
extern _recorder_obj recset;

void RecorderDemo(void);
void StartRecord(const char *filename);
void recorder(const TCHAR *recfile);
void record_play(uint8_t *recfile,HWND hWin);

#endif  /* __RECORD_H__   */

