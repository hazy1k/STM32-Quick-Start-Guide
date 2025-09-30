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
File        : MainTask_MoviePlayer.c
Purpose     : Simple single layer movie player.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef SKIP_TEST

#include "DIALOG.h"

#ifdef WIN32
  #include <windows.h>
#else
  #include "BSP.h"
  #include "FS.h"
  #include "RTOS.h"
#endif

#include "Resource.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef _MAX_PATH
  #define _MAX_PATH 256
#else
  #if (_MAX_PATH > 256)
    #undef  _MAX_PATH
    #define _MAX_PATH 256
  #endif
#endif

#ifndef _MAX_EXT
  #define _MAX_EXT  256
#endif

#define X_FRAME 4
//#define Y_SIZE_INDICATOR 6
#define Y_SIZE_INDICATOR (((_apBm[0]->YSize) * 3) / 4)
#define Y_FRAME_INDICATOR 2

#define INDEX_STOP   0
#define INDEX_BACK   1
#define INDEX_PLAY   2
#define INDEX_NEXT   3
#define INDEX_EJECT  4
#define INDEX_PAUSE  5

#define APP_SETPOS1000 (WM_USER +  0)
#define APP_SETFRAME   (WM_USER +  1)
#define APP_SETPLAY    (WM_USER +  2)
#define APP_SETPAUSE   (WM_USER +  3)
#define APP_SETSTOP    (WM_USER +  4)
#define APP_SETEMPTY   (WM_USER +  5)
#define APP_SETSKIP    (WM_USER +  6)
#define APP_CLEARSKIP  (WM_USER +  7)
#define APP_SLIDER_ON  (WM_USER +  8)
#define APP_SLIDER_OFF (WM_USER +  9)
#define APP_CONTINUE   (WM_USER + 10)

#define MODE_STOP  0
#define MODE_PLAY  1
#define MODE_PAUSE 2

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static struct {
  U32 Mask;
  char c;
} _aAttrib[] = {
#ifdef WIN32
  { FILE_ATTRIBUTE_READONLY , 'R' },
  { FILE_ATTRIBUTE_HIDDEN   , 'H' },
  { FILE_ATTRIBUTE_SYSTEM   , 'S' },
  { FILE_ATTRIBUTE_DIRECTORY, 'D' },
  { FILE_ATTRIBUTE_ARCHIVE  , 'A' }
#else
  { FS_ATTR_READ_ONLY, 'R' },
  { FS_ATTR_HIDDEN   , 'H' },
  { FS_ATTR_SYSTEM   , 'S' },
  { FS_ATTR_DIRECTORY, 'D' },
  { FS_ATTR_ARCHIVE  , 'A' },
#endif
};

static const GUI_BITMAP * _apBm[] = {
  &bmStop,
  &bmBack,
  &bmPlay,
  &bmNext,
  &bmEject,
  &bmPause,
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetExt
*/
#ifndef WIN32
static void _GetExt(char * pFile, char * pExt) {
  int Len;
  int i;
  int j;

  //
  // Search beginning of extension
  //
  Len = strlen(pFile);
  for (i = Len; i > 0; i--) {
    if (*(pFile + i) == '.') {
      *(pFile + i) = '\0';     // Cut extension from filename
      break;
    }
  }
  //
  // Copy extension
  //
  j = 0;
  while (*(pFile + ++i) != '\0') {
    *(pExt + j) = *(pFile + i);
    j++;
  }
  *(pExt + j) = '\0';          // Set end of string
}
#endif

/*********************************************************************
*
*       _GetChooseFileData
*/
static int _GetChooseFileData(CHOOSEFILE_INFO * pInfo) {
#ifdef WIN32
  static HANDLE hFind;
  static int NewDir;
  static char acDrive [_MAX_DRIVE];
  static char acDir   [_MAX_DIR];
  static char acName  [_MAX_FNAME];
  static char acExt   [_MAX_EXT];
  static char acMask  [_MAX_PATH];
  static char acPath  [_MAX_PATH];
  static char acAttrib[10] = {0};
  WIN32_FIND_DATA Context;
  int i, r;
  char c;

  switch (pInfo->Cmd) {
  case CHOOSEFILE_FINDFIRST:
    if (hFind != 0) {
      FindClose(hFind);
    }
    //
    // Split path into drive and directory
    //
    _splitpath(pInfo->pRoot, acDrive, acDir, NULL, NULL);
    NewDir = 1;
    //
    // Do not 'break' here...
    //
  case CHOOSEFILE_FINDNEXT:
    if (NewDir) {
      _makepath(acMask, acDrive, acDir, NULL, NULL);
      strcat(acMask, pInfo->pMask);
      hFind = FindFirstFile(acMask, &Context);
      if (hFind == INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        hFind = 0;
        return 1;
      }
    } else {
      r = FindNextFile(hFind, &Context);
      if (r == 0) {
        FindClose(hFind);
        hFind = 0;
        return 1;
      }
    }
    NewDir = 0;
    //
    // Generate attribute string (pInfo->pAttrib)
    //
    for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) {
      c = (Context.dwFileAttributes & _aAttrib[i].Mask) ? _aAttrib[i].c : '-';
      acAttrib[i] = c;
    }
    //
    // Make name and extension (pInfo->pName, pInfo->pExt)
    //
    if ((Context.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
      _splitpath(Context.cFileName, NULL, NULL, acName, acExt);
    } else {
      strcpy(acName, Context.cFileName);
      acExt[0] = 0;
    }
    //
    // Pass data to dialog
    //
    pInfo->pAttrib = acAttrib;
    pInfo->pName   = acName;
    pInfo->pExt    = acExt;
    pInfo->SizeL   = Context.nFileSizeLow;
    pInfo->SizeH   = Context.nFileSizeHigh;
    pInfo->Flags   = (Context.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                     ? CHOOSEFILE_FLAG_DIRECTORY : 0;
  }
  return 0;
#else
  static FS_FIND_DATA FindData;
  static char         acAttrib[10]         = { 0 };
  static char         acFile  [_MAX_PATH];
  static char         acExt   [_MAX_EXT];
  char                c;
  int                 i;
  int                 r;

  switch (pInfo->Cmd) {
  case CHOOSEFILE_FINDFIRST:
    r = FS_FindFirstFile(&FindData, pInfo->pRoot, acFile, sizeof(acFile));
    break;
  case CHOOSEFILE_FINDNEXT:
    r = FS_FindNextFile(&FindData) ^ 1;
    break;
  }
  if (r == 0) {
    pInfo->Flags = (FindData.Attributes & _aAttrib[3].Mask) ? CHOOSEFILE_FLAG_DIRECTORY : 0;
    for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) {
      if (FindData.Attributes & _aAttrib[i].Mask) {
        c = _aAttrib[i].c;
      } else {
        c = '-';
      }
      acAttrib[i] = c;
    }
    _GetExt(acFile, acExt);
    pInfo->pAttrib = acAttrib;
    pInfo->pName   = acFile;
    pInfo->pExt    = acExt;
    pInfo->SizeL   = FindData.FileSize;
    pInfo->SizeH   = 0;
  }
  return r;
#endif
}

/*********************************************************************
*
*       _ChooseFile
*/
static char * _ChooseFile(void) {
  const char             acMask[]            = "*.*";
  static CHOOSEFILE_INFO Info                = { 0 };
  WM_HWIN                hWin;
  int                    xSize;
  int                    ySize;
  int                    Cnt                 = 0;
#ifdef WIN32
  char                   acDirLocal[]        = "C:\\";
  char                   acDirWin[_MAX_PATH] = "\0";
  char                   acDirTmp[_MAX_PATH] = "\0";
  char                   acDirSys[_MAX_PATH] = "\0";
  char                 * apDrives[4]         = { acDirLocal, acDirWin, acDirTmp, acDirSys };
#else
  char                   acDir[_MAX_PATH]    = "mmc:";
  char const           * apDrives[1]         = { acDir };
#endif

#ifdef WIN32
  GetWindowsDirectory(acDirWin, sizeof(acDirWin));
  GetTempPath        (sizeof(acDirTmp), acDirTmp);
  GetSystemDirectory (acDirSys, sizeof(acDirSys));
#endif
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  Info.pfGetData = _GetChooseFileData;
  Info.pMask     = acMask;
  hWin = CHOOSEFILE_Create(WM_HBKWIN, 0, 0, xSize, ySize, apDrives, GUI_COUNTOF(apDrives), 0, "Please choose movie file...", 0, &Info);
  FRAMEWIN_SetClientColor(hWin, COLOR_BRIGHT);
  WM_MakeModal(hWin);
  do {
    GUI_Delay(10);
  } while (WM_IsWindow(hWin));
  if (Info.Id == GUI_ID_OK) {
    return Info.pRoot;
  } else {
    return NULL;
  }
}

/*********************************************************************
*
*       _cbNotify
*
* Function description
*   Callback function for movie player. Uses multiple buffering if
*   available to avoid tearing effects.
*/
void _cbNotify(GUI_HMEM hMem, int Notification, U32 CurrentFrame) {
  WM_HWIN hWin;
  WM_MESSAGE Msg;

  switch (Notification) {
  case GUI_MOVIE_NOTIFICATION_PREDRAW:
    GUI_MULTIBUF_Begin();
    break;
  case GUI_MOVIE_NOTIFICATION_POSTDRAW:
    GUI_MULTIBUF_End();
    hWin = WM_GetFirstChild(WM_HBKWIN);
    GUI_ZEROFILL(&Msg, sizeof(Msg));
    Msg.Data.v = CurrentFrame;
    Msg.MsgId = APP_SETFRAME;
    WM_SendMessage(hWin, &Msg);
    break;
  case GUI_MOVIE_NOTIFICATION_STOP:
    break;
  }
}

/*********************************************************************
*
*       _GetData
*
* Function description
*   Reading data directly from file system
*/
int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {
  U32 NumBytesRead;
#ifdef WIN32
  HANDLE hFile;

  hFile = *(HANDLE *)p;
  SetFilePointer(hFile, Off, 0, FILE_BEGIN);
  ReadFile(hFile, (U8 *)*ppData, NumBytes, &NumBytesRead, NULL);
#else
  FS_FILE * pFile;

  pFile = (FS_FILE *)p;
  FS_SetFilePos(pFile, Off, FS_FILE_BEGIN);
  NumBytesRead = FS_Read(pFile, (U8 *)*ppData, NumBytes);
#endif
  return NumBytesRead;
}

/*********************************************************************
*
*       _cbButton
*/
static void _cbButton(WM_MESSAGE * pMsg) {
  GUI_BITMAP * pBm;
  WM_HWIN hWin;
  int IsPressed;
  int IsEnabled;
  GUI_COLOR Color;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_USER_DATA:
    WM_InvalidateWindow(hWin);
    break;
  case WM_PAINT:
    BUTTON_GetUserData(hWin, &pBm, sizeof(pBm));
    IsPressed = BUTTON_IsPressed(hWin);
    IsEnabled = WM_IsEnabled(hWin);
    if (IsEnabled) {
      if (IsPressed) {
        Color = COLOR_LIME;
      } else {
        Color = GUI_DARKGRAY;
      }
    } else {
      Color = GUI_LIGHTGRAY;
    }
    GUI_SetColor(Color);
    GUI_DrawBitmap(pBm, 0,  0);
    break;
  default:
    BUTTON_Callback(pMsg);
    break;
  }
}

/*********************************************************************
*
*       _cbSlider
*/
static void _cbSlider(WM_MESSAGE * pMsg) {
  static int xSizeWindow, ySizeWindow;
  static int ReactOnTouch;
  WM_HWIN hWin, hParent;
  int * pPos1000;
  int xPos;
  GUI_PID_STATE * pState;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case APP_SLIDER_ON:
    ReactOnTouch = 1;
    WM_InvalidateWindow(hWin);
    break;
  case APP_SLIDER_OFF:
    ReactOnTouch = 0;
    WM_InvalidateWindow(hWin);
    break;
  case WM_CREATE:
    xSizeWindow = WM_GetXSize(hWin);
    ySizeWindow = WM_GetYSize(hWin);
    break;
  case WM_PAINT:
    WM_GetUserData(hWin, &pPos1000, sizeof(pPos1000));
    if (ReactOnTouch) {
      xPos = (xSizeWindow * *pPos1000) / 1000;
    } else {
      xPos = 0;
    }
    GUI_SetColor(COLOR_LIME);
    GUI_FillRect(0, 0, xPos - 1, ySizeWindow - 1);
    GUI_SetColor(GUI_DARKGRAY);
    GUI_FillRect(xPos, 0, xSizeWindow - 1, ySizeWindow - 1);
    break;
  case WM_TOUCH:
    pState = (GUI_PID_STATE *)pMsg->Data.p;
    if (pState) {
      hParent = WM_GetParent(hWin);
      if (pState->Pressed) {
        WM_GetUserData(hWin, &pPos1000, sizeof(pPos1000));
        WM_SetCapture(hWin, 1);
        *pPos1000 = (pState->x * 1000) / xSizeWindow;
        WM_InvalidateWindow(hWin);
        WM_SendMessageNoPara(hParent, APP_SETPOS1000);
      } else {
        WM_SendMessageNoPara(hParent, APP_CONTINUE);
      }
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       _EnableNavigation
*/
static void _EnableNavigation(WM_HWIN hWin, int OnOff) {
  int aId[4] = { INDEX_BACK, INDEX_NEXT };
  int i;
  WM_HWIN hItem;
  
  for (i = 0; i < GUI_COUNTOF(aId); i++) {
    hItem = WM_GetDialogItem(hWin, GUI_ID_BUTTON0 + aId[i]);
    if (OnOff) {
      WM_EnableWindow(hItem);
    } else {
      WM_DisableWindow(hItem);
    }
  }
}

/*********************************************************************
*
*       _EnablePlayStop
*/
static void _EnablePlayStop(WM_HWIN hWin, int OnOff, WM_HWIN hWinSlider) {
  int aId[4] = { INDEX_STOP, INDEX_PLAY };
  int i;
  WM_HWIN hItem;
  
  for (i = 0; i < GUI_COUNTOF(aId); i++) {
    hItem = WM_GetDialogItem(hWin, GUI_ID_BUTTON0 + aId[i]);
    if (OnOff) {
      WM_EnableWindow(hItem);
    } else {
      WM_DisableWindow(hItem);
    }
  }
  if (OnOff) {
    WM_SendMessageNoPara(hWinSlider, APP_SLIDER_ON);
  } else {
    WM_SendMessageNoPara(hWinSlider, APP_SLIDER_OFF);
  }
}

/*********************************************************************
*
*       _SetPlayButtonBitmap
*/
static void _SetPlayButtonBitmap(WM_HWIN hWin, int Index) {
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(hWin, GUI_ID_BUTTON0 + INDEX_PLAY);
  BUTTON_SetUserData(hItem, &_apBm[Index], sizeof(void *));
}

/*********************************************************************
*
*       _cbPlayer
*/
static void _cbPlayer(WM_MESSAGE * pMsg) {
#ifdef WIN32
  static HANDLE    hFile;
  #define PARAM    &hFile
#else
  static FS_FILE * pFile;
  #define PARAM    pFile
#endif
  static GUI_RECT RectMovie;
  static int Pos1000;
  static WM_HWIN hWinSlider;
  static int Mode;
  static xPosMovie, yPosMovie;
  static int xSizeWindow, ySizeWindow;
  int * pPos1000;
  WM_HWIN hWin;
  WM_HWIN hItem;
  int yPosWindow;
  int xPos, yPos;
  int i, OldMode;
  int Id, NCode;
  U32 Frame;
  const char * pFileName;
  static GUI_MOVIE_INFO   Info;
  static GUI_MOVIE_HANDLE hMovie;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case APP_CONTINUE:
    if (hMovie && (Mode == MODE_PLAY)) {
      GUI_MOVIE_Play(hMovie);
    }
    break;
  case APP_SETPLAY:
    _SetPlayButtonBitmap(hWin, INDEX_PAUSE);
    _EnableNavigation(hWin, 0);
    _EnablePlayStop(hWin, 1, hWinSlider);
    Mode = MODE_PLAY;
    break;
  case APP_SETPAUSE:
    _SetPlayButtonBitmap(hWin, INDEX_PLAY);
    _EnableNavigation(hWin, 1);
    Mode = MODE_PAUSE;
    break;
  case APP_SETSTOP:
    _SetPlayButtonBitmap(hWin, INDEX_PLAY);
    _EnableNavigation(hWin, 0);
    Mode = MODE_STOP;
    break;
  case APP_SETPOS1000:
    if (hMovie) {
      Frame = (Pos1000 * Info.NumFrames) / 1000;
      GUI_MOVIE_GotoFrame(hMovie, Frame);
      if (Mode != MODE_PLAY) {
        GUI_MOVIE_DrawFrame(hMovie, Frame, xPosMovie, yPosMovie);
      }
      GUI_MOVIE_Pause(hMovie);
    }
    break;
  case APP_SETFRAME:
    Frame = pMsg->Data.v;
    GUI_MOVIE_GotoFrame(hMovie, Frame);
    Pos1000 = (Frame * 1000) / Info.NumFrames;
    WM_InvalidateWindow(hWinSlider);
    break;
  case WM_CREATE:
    xSizeWindow = WM_GetXSize(hWin);
    ySizeWindow = WM_GetYSize(hWin);
    yPosWindow = WM_GetWindowOrgY(hWin);
    RectMovie.x1 = LCD_GetXSize();
    RectMovie.y1 = yPosWindow - 1;
    xPos = X_FRAME;
    yPos = Y_FRAME_INDICATOR;
    for (i = 0; i < GUI_COUNTOF(_apBm) - 1; i++) {
      hItem = BUTTON_CreateUser(xPos, yPos, _apBm[i]->XSize, _apBm[i]->YSize, pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, GUI_ID_BUTTON0 + i, sizeof(void *));
      BUTTON_SetUserData(hItem, &_apBm[i], sizeof(_apBm[i]));
      WM_SetCallback(hItem, _cbButton);
      xPos += _apBm[i]->XSize + X_FRAME;
    }
    xPos += X_FRAME;
    hWinSlider = WM_CreateWindowAsChild(xPos, (ySizeWindow - Y_SIZE_INDICATOR) / 2, xSizeWindow - xPos - X_FRAME * 2, Y_SIZE_INDICATOR, hWin, WM_CF_SHOW, _cbSlider, sizeof(void *));
    pPos1000 = &Pos1000;
    WM_SetUserData(hWinSlider, &pPos1000, sizeof(pPos1000));
    WM_SendMessageNoPara(hWin, APP_SETSTOP);
    _EnablePlayStop(hWin, 0, hWinSlider);
    break;
  case WM_TIMER:
    break;
  case WM_TOUCH:
    break;
  case WM_PID_STATE_CHANGED:
    break;
  case WM_NOTIFY_PARENT:
    Id = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    if (NCode == WM_NOTIFICATION_RELEASED) {
      switch (Id - GUI_ID_BUTTON0) {
      case INDEX_NEXT:
        Frame = GUI_MOVIE_GetFrameIndex(hMovie);
        if (Frame < Info.NumFrames - 1) {
          Frame++;
          GUI_MOVIE_GotoFrame(hMovie, Frame);
        }
        GUI_MOVIE_DrawFrame(hMovie, Frame, xPosMovie, yPosMovie);
        break;
      case INDEX_BACK:
        Frame = GUI_MOVIE_GetFrameIndex(hMovie);
        if (Frame > 0) {
          Frame--;
          GUI_MOVIE_GotoFrame(hMovie, Frame);
        }
        GUI_MOVIE_DrawFrame(hMovie, Frame, xPosMovie, yPosMovie);
        break;
      case INDEX_STOP:
        if (hMovie) {
          GUI_MOVIE_Delete(hMovie);
          hMovie = 0;
          Pos1000 = 0;
          _EnablePlayStop(hWin, 0, hWinSlider);
          WM_SendMessageNoPara(WM_HBKWIN, APP_CLEARSKIP);
          WM_SendMessageNoPara(hWin, APP_SETSTOP);
        }
        break;
      case INDEX_PLAY:
        switch (Mode) {
        case MODE_PLAY:
          GUI_MOVIE_Pause(hMovie);
          WM_SendMessageNoPara(hWin, APP_SETPAUSE);
          break;
        case MODE_STOP:
        case MODE_PAUSE:
          GUI_MOVIE_Play(hMovie);
          WM_SendMessageNoPara(hWin, APP_SETPLAY);
          break;
        }
        break;
      case INDEX_EJECT:
        OldMode = Mode;
        if (OldMode == MODE_PLAY) {
          GUI_MOVIE_Pause(hMovie);
          WM_SendMessageNoPara(hWin, APP_SETPAUSE);
        }
        pFileName = _ChooseFile();
        if (pFileName) {
          if (hMovie) {
            GUI_MOVIE_Delete(hMovie);
            hMovie = 0;
            _EnablePlayStop(hWin, 0, hWinSlider);
          }
          #ifdef WIN32
            if (hFile) {
              CloseHandle(hFile);
            }
            hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
          #else
            if (pFile) {
              FS_FClose(pFile);
            }
            pFile = FS_FOpen(pFileName, "r");
          #endif
          if (GUI_MOVIE_GetInfoEx(_GetData, PARAM, &Info) == 0) {
            if ((Info.xSize <= RectMovie.x1) && (Info.ySize <= RectMovie.y1)) {
              hMovie = GUI_MOVIE_CreateEx(_GetData, PARAM, _cbNotify);
              if (hMovie) {
                xPosMovie = (RectMovie.x1 - Info.xSize + 1) / 2;
                yPosMovie = (RectMovie.y1 - Info.ySize + 1) / 2;
                GUI_MOVIE_Show(hMovie, xPosMovie, yPosMovie, 1);
                WM_SendMessageNoPara(WM_HBKWIN, APP_SETSKIP);
              }
              WM_SendMessageNoPara(hWin, APP_SETPLAY);
            }
          } else {
            Pos1000 = 0;
            _EnablePlayStop(hWin, 0, hWinSlider);
            WM_SendMessageNoPara(WM_HBKWIN, APP_CLEARSKIP);
            WM_SendMessageNoPara(hWin, APP_SETSTOP);
          }
        } else {
          if (hMovie) {
            if (OldMode == MODE_PLAY) {
              GUI_MOVIE_Play(hMovie);
              WM_SendMessageNoPara(hWin, APP_SETPLAY);
            } else {
              Frame = GUI_MOVIE_GetFrameIndex(hMovie);
              GUI_MOVIE_DrawFrame(hMovie, Frame, xPosMovie, yPosMovie);
            }
          }
        }
        break;
      }
    }
    break;
  case WM_PAINT:
    GUI_SetBkColor(COLOR_WHITE);
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_DrawHLine(0, 0, xSizeWindow - 1);
    GUI_DrawHLine(1, 0, xSizeWindow - 1);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _CreatePlayerWindow
*/
static void _CreatePlayerWindow(void) {
  int xSizeDisplay, ySizeDisplay;
  int xSizePlayer, ySizePlayer;
  int xPos, yPos;

  xSizeDisplay = LCD_GetXSize();
  ySizeDisplay = LCD_GetYSize();
  xSizePlayer = xSizeDisplay;
  ySizePlayer = _apBm[0]->YSize + Y_FRAME_INDICATOR * 2;
  xPos = (xSizeDisplay - xSizePlayer) / 2;
  yPos = (ySizeDisplay - ySizePlayer);
  WM_CreateWindowAsChild(xPos, yPos, xSizePlayer, ySizePlayer, WM_HBKWIN, WM_CF_SHOW, _cbPlayer, 0);
}

/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  static int Skip;
  static int xSize, ySize;
  const GUI_BITMAP * pBm;
  WM_HWIN hWin;
  GUI_RECT Rect;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_SET_CALLBACK:
    xSize = LCD_GetXSize();
    ySize = LCD_GetYSize();
    ySize -= (_apBm[0]->YSize + Y_FRAME_INDICATOR * 2);
    break;
  case APP_SETSKIP:
    Skip = 1;
    WM_InvalidateWindow(hWin);
    break;
  case APP_CLEARSKIP:
    Skip = 0;
    WM_InvalidateWindow(hWin);
    break;
  case WM_PAINT:
    if (Skip == 0) {
      Rect.x0 = 0;
      Rect.y0 = 20;
      Rect.x1 = xSize - 1;
      Rect.y1 = Rect.y0 + 40 - 1;
      pBm = &bmSeggerLogo_120x60;
      GUI_SetBkColor(COLOR_LIGHTGRAY);
      GUI_Clear();
      GUI_SetColor(COLOR_DARKBLUE);
      GUI_FillRectEx(&Rect);
      GUI_DrawBitmap(pBm, 10, 10);
      GUI_SetFont(&GUI_Font24_AA4);
      Rect.x0 += 10 + pBm->XSize;
      GUI_SetTextMode(GUI_TM_TRANS);
      GUI_SetColor(COLOR_LIGHTGRAY);
      GUI_DispStringInRect("emWin, it simply works!", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
      Rect.x0 = 0;
      Rect.y0 = 10 + pBm->YSize;
      Rect.x1 = xSize - 1;
      Rect.y1 = ySize;
      GUI_SetColor(COLOR_DARKBLUE);
      GUI_DispStringInRect("Please press 'eject'\nto select movie file", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    } else {
      GUI_SetBkColor(GUI_DARKGRAY);
      GUI_Clear();
    }
    break;
  }
}

/*********************************************************************
*
*       _FRAMEWIN_Skin
*/
static int _FRAMEWIN_Skin(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_GET_RADIUS:
    return 0;
  case WIDGET_ITEM_DRAW_BACKGROUND:
    GUI_SetBkColor(COLOR_BRIGHT);
    GUI_Clear();
    break;
  default:
    return FRAMEWIN_DrawSkinFlex(pDrawItemInfo);
  }
  return 0;
}

/*********************************************************************
*
*       _SetSkinning
*/
static void _SetSkinning(void) {
  FRAMEWIN_SetDefaultSkin(_FRAMEWIN_Skin);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask_MoviePlayer
*/
void MainTask_MoviePlayer(void) {
  GUI_Init();
#ifndef WIN32
  FS_Init();
  FS_FAT_SupportLFN();
  GUI_SPY_StartServer();
  GUI_VNC_X_StartServerFT(0, 0);
#endif
  _SetSkinning();
  WM_MULTIBUF_Enable(1);
  FRAMEWIN_SetDefaultFont(GUI_FONT_20_ASCII);
  FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);
  WM_SetCallback(WM_HBKWIN, _cbBk);
  _CreatePlayerWindow();
  while (1) {
    GUI_Delay(100);
  }
}

#endif

/*************************** End of file ****************************/
