#ifndef __STM324xG_USB_AUDIOCODEC_H
#define __STM324xG_USB_AUDIOCODEC_H

#include "stm32f4xx.h" 
 
 
/*-----------------------------------
                        Audio Codec User defines
                                     -----------------------------------------*/
/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0 


/* Exported functions ------------------------------------------------------- */
/*-----------------------------------
                           Generic functions 
                                    ------------------------------------------*/
uint32_t EVAL_AUDIO_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
uint32_t EVAL_AUDIO_DeInit(void);
uint32_t EVAL_AUDIO_Play(uint16_t* pBuffer, uint32_t Size);
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size);
uint32_t EVAL_AUDIO_Stop(uint32_t CodecPowerDown_Mode);
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t Volume);
uint32_t EVAL_AUDIO_Mute(uint32_t Command);
uint32_t Codec_SwitchOutput(uint8_t Output);
 

/*-----------------------------------
                   MAL (Media Access Layer) functions 
                                    ------------------------------------------*/
void     Audio_MAL_Init(void);
void     Audio_MAL_DeInit(void);
void     Audio_MAL_Play(uint32_t Addr, uint32_t Size);
void     Audio_MAL_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size);
void     Audio_MAL_Stop(void);
 
 
#endif 






















