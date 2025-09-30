/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2021  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.22 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf.c
Content     : Display controller configuration for weather forecast demo
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include <stdlib.h>

#include "GUI.h"

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/
//
// Set this define to 1 if you want to use soft layers. The application
// needs to be configured properly, too.
//
#define USE_SOFTLAYER    1

#if (USE_SOFTLAYER == 1) 
  #define COLOR_CONVERSION GUICC_8888

  #define DISPLAY_DRIVER   GUIDRV_WIN32

  #define NUM_BUFFERS      3

  #define NUM_LAYER        2
#endif

/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef struct {
  int NumBuffers;
  const GUI_DEVICE_API * pDriver;
  const LCD_API_COLOR_CONV * pColorConv;
  int xSize, ySize;
  int xPos, yPos;
} INIT_LAYER;

typedef struct {
  int NumLayers;
#if (USE_SOFTLAYER == 1)
  GUI_SOFTLAYER_CONFIG aLayer[NUM_LAYER];
#else
  INIT_LAYER aLayer[GUI_NUM_LAYERS];
#endif
} INIT_APP;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
//
// Array for initializing layers for applications. Do not change order
//
static INIT_APP _InitApp = {
  //
  // WeatherForecast
  //
  NUM_LAYER,  // Number of layers
  {
#if (USE_SOFTLAYER == 1)
    { 0, 0, 800, 480, 1 },
    { 0, 0, 800, 480, 1 },
#else
    { NUM_BUFFERS, GUIDRV_WIN32, GUICC_M565,   800, 480, 0, 0, },
    { NUM_BUFFERS, GUIDRV_WIN32, GUICC_M8888I, 800, 480, 0, 0, },
#endif
  }
};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*/
void LCD_X_Config(void) {
  GUI_DEVICE * pDevice;

#if (USE_SOFTLAYER == 1)
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, _InitApp.aLayer[0].ySize, _InitApp.aLayer[0].xSize);
    LCD_SetVSizeEx(0, _InitApp.aLayer[0].ySize, _InitApp.aLayer[0].xSize);
  } else {
    LCD_SetSizeEx (0, _InitApp.aLayer[0].xSize, _InitApp.aLayer[0].ySize);
    LCD_SetVSizeEx(0, _InitApp.aLayer[0].xSize, _InitApp.aLayer[0].ySize);
  }
  GUI_SOFTLAYER_Enable(_InitApp.aLayer, NUM_LAYER, GUI_DARKBLUE);
#else
  int i;
  //
  // Multiple buffer configuration, should be the first thing here in that routine
  //
  for (i = 0; i < _InitApp.NumLayers; i++) {
    if (_InitApp.aLayer[i].NumBuffers > 1) {
      GUI_MULTIBUF_ConfigEx(i, _InitApp.aLayer[i].NumBuffers);
    }
  }
  //
  // Set display driver and color conversion for layers
  //
  for (i = 0; i < _InitApp.NumLayers; i++) {
    pDevice = GUI_DEVICE_CreateAndLink(_InitApp.aLayer[i].pDriver, _InitApp.aLayer[i].pColorConv, 0, i);
    if (pDevice == NULL) {
      while (1); // Error
    }
    if (LCD_GetSwapXYEx(i)) {
      LCD_SetSizeEx (i, _InitApp.aLayer[i].ySize, _InitApp.aLayer[i].xSize);
      LCD_SetVSizeEx(i, _InitApp.aLayer[i].ySize, _InitApp.aLayer[i].xSize);
    } else {
      LCD_SetSizeEx (i, _InitApp.aLayer[i].xSize, _InitApp.aLayer[i].ySize);
      LCD_SetVSizeEx(i, _InitApp.aLayer[i].xSize, _InitApp.aLayer[i].ySize);
    }
    LCD_SetPosEx(i, _InitApp.aLayer[i].xPos, _InitApp.aLayer[i].yPos);
    LCD_SetVisEx(i, 1);
  }
#endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  return 0;
}
/*************************** End of file ****************************/
