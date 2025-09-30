/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2022  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_00.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_00.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _aCreate
*/
static APPW_CREATE_ITEM _aCreate[] = {
  { WM_OBJECT_WINDOW_Create,
    ID_SCREEN_00, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_QRCODE_Create,
    ID_QRCODE_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 354, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 194, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      10, 10, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_KEYBOARD_Create,
    ID_KEYBOARD_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 240, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 303, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      320, 120, 0, 0, 0, 0
    },
  },
  { WM_OBJECT_GAUGE_Create,
    ID_GAUGE_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 300, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 57, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      200, 100, 0, 0, 0, 0
    },
  },
};

/*********************************************************************
*
*       _aSetup
*/
static const APPW_SETUP_ITEM _aSetup[] = {
  { ID_BOX_00,      APPW_SET_PROP_COLOR,        { ARG_V(GUI_DARKGRAY) } },
  { ID_KEYBOARD_00, APPW_SET_PROP_SLAYOUT,      { ARG_VP(2396, acSKEYBOARD_ENG_LP) } },
  { ID_KEYBOARD_00, APPW_SET_PROP_COLORS,       { ARG_V(0xffc0c3c0),
                                                  ARG_V(GUI_INVALID_COLOR),
                                                  ARG_V(GUI_INVALID_COLOR) } },
  { ID_GAUGE_00,    APPW_SET_PROP_ALIGNBITMAP,  { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_BOTTOM),
                                                  ARG_V(0),
                                                  ARG_V(4294967286) } },
  { ID_GAUGE_00,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                  ARG_V(1800) } },
  { ID_GAUGE_00,    APPW_SET_PROP_SPAN,         { ARG_V(0),
                                                  ARG_V(999) } },
  { ID_GAUGE_00,    APPW_SET_PROP_RADIUS,       { ARG_V(80) } },
  { ID_GAUGE_00,    APPW_SET_PROP_COLORS,       { ARG_V(0xff2c2c30),
                                                  ARG_V(0xff2777b3),
                                                  ARG_V(GUI_INVALID_COLOR) } },
  { ID_GAUGE_00,    APPW_SET_PROP_VALUES,       { ARG_V(19),
                                                  ARG_V(13) } },
  { ID_GAUGE_00,    APPW_SET_PROP_ROUNDEDVAL,   { ARG_V(0) } },
  { ID_GAUGE_00,    APPW_SET_PROP_ROUNDEDEND,   { ARG_V(0) } },
  { ID_GAUGE_00,    APPW_SET_PROP_VALUE,        { ARG_V(250) } },
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_00_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_00_RootInfo = {
  ID_SCREEN_00,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  NULL,  0,
  cbID_SCREEN_00,
  0
};

/*************************** End of file ****************************/
