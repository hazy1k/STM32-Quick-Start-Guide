#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

#include "stm32f10x.h"
#include "emXGUI.h"



void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ; 
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);  
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  ;
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 

#endif /* __GUI_PICTURE_PORT_H */
