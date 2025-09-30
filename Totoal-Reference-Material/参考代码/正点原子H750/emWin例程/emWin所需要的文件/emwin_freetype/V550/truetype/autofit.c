/***************************************************************************/
/*                                                                         */
/*  autofit.c                                                              */
/*                                                                         */
/*    Auto-fitter module (body).                                           */
/*                                                                         */
/*  Copyright 2003-2007, 2011, 2013 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#define FT_MAKE_OPTION_SINGLE_OBJECT
#include "ft2build.h"
/***************************************************************************/
/*                                                                         */
/*  afpic.c                                                                */
/*                                                                         */
/*    The FreeType position independent code services for autofit module.  */
/*                                                                         */
/*  Copyright 2009-2014 by                                                 */
/*  Oran Agra and Mickey Gabel.                                            */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_INTERNAL_OBJECTS_H
#include "afpic.h"
#include "afglobal.h"
#include "aferrors.h"


#ifdef FT_CONFIG_OPTION_PIC

  /* forward declaration of PIC init functions from afmodule.c */
  FT_Error
  FT_Create_Class_af_services( FT_Library           library,
                               FT_ServiceDescRec**  output_class );

  void
  FT_Destroy_Class_af_services( FT_Library          library,
                                FT_ServiceDescRec*  clazz );

  void
  FT_Init_Class_af_service_properties( FT_Service_PropertiesRec*  clazz );

  void FT_Init_Class_af_autofitter_interface(
    FT_Library                   library,
    FT_AutoHinter_InterfaceRec*  clazz );


  /* forward declaration of PIC init functions from writing system classes */
#undef  WRITING_SYSTEM
#define WRITING_SYSTEM( ws, WS )  /* empty */

#include "afwrtsys.h"


  void
  autofit_module_class_pic_free( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Memory          memory        = library->memory;


    if ( pic_container->autofit )
    {
      AFModulePIC*  container = (AFModulePIC*)pic_container->autofit;


      if ( container->af_services )
        FT_Destroy_Class_af_services( library,
                                      container->af_services );
      container->af_services = NULL;

      FT_FREE( container );
      pic_container->autofit = NULL;
    }
  }


  FT_Error
  autofit_module_class_pic_init( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_UInt            ss;
    FT_Error           error         = FT_Err_Ok;
    AFModulePIC*       container     = NULL;
    FT_Memory          memory        = library->memory;


    /* allocate pointer, clear and set global container pointer */
    if ( FT_ALLOC ( container, sizeof ( *container ) ) )
      return error;
    FT_MEM_SET( container, 0, sizeof ( *container ) );
    pic_container->autofit = container;

    /* initialize pointer table -                       */
    /* this is how the module usually expects this data */
    error = FT_Create_Class_af_services( library,
                                         &container->af_services );
    if ( error )
      goto Exit;

    FT_Init_Class_af_service_properties( &container->af_service_properties );

    for ( ss = 0; ss < AF_WRITING_SYSTEM_MAX; ss++ )
      container->af_writing_system_classes[ss] =
        &container->af_writing_system_classes_rec[ss];
    container->af_writing_system_classes[AF_WRITING_SYSTEM_MAX] = NULL;

    for ( ss = 0; ss < AF_SCRIPT_MAX; ss++ )
      container->af_script_classes[ss] =
        &container->af_script_classes_rec[ss];
    container->af_script_classes[AF_SCRIPT_MAX] = NULL;

    for ( ss = 0; ss < AF_STYLE_MAX; ss++ )
      container->af_style_classes[ss] =
        &container->af_style_classes_rec[ss];
    container->af_style_classes[AF_STYLE_MAX] = NULL;

#undef  WRITING_SYSTEM
#define WRITING_SYSTEM( ws, WS )                             \
        FT_Init_Class_af_ ## ws ## _writing_system_class(    \
          &container->af_writing_system_classes_rec[ss++] );

    ss = 0;
#include "afwrtsys.h"

#undef  SCRIPT
#define SCRIPT( s, S, d, h, sc1, sc2, sc3 )          \
        FT_Init_Class_af_ ## s ## _script_class(     \
          &container->af_script_classes_rec[ss++] );

    ss = 0;
#include "afscript.h"

#undef  STYLE
#define STYLE( s, S, d, ws, sc, bss, c )            \
        FT_Init_Class_af_ ## s ## _style_class(     \
          &container->af_style_classes_rec[ss++] );

    ss = 0;
#include "afstyles.h"

    FT_Init_Class_af_autofitter_interface(
      library, &container->af_autofitter_interface );

  Exit:
    if ( error )
      autofit_module_class_pic_free( library );
    return error;
  }

#endif /* FT_CONFIG_OPTION_PIC */


/* END */
/***************************************************************************/
/*                                                                         */
/*  afangles.c                                                             */
/*                                                                         */
/*    Routines used to compute vector angles with limited accuracy         */
/*    and very high speed.  It also contains sorting routines (body).      */
/*                                                                         */
/*  Copyright 2003-2006, 2011-2012 by                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "aftypes.h"


#if 0

  FT_LOCAL_DEF( FT_Int )
  af_corner_is_flat( FT_Pos  x_in,
                     FT_Pos  y_in,
                     FT_Pos  x_out,
                     FT_Pos  y_out )
  {
    FT_Pos  ax = x_in;
    FT_Pos  ay = y_in;

    FT_Pos  d_in, d_out, d_corner;


    if ( ax < 0 )
      ax = -ax;
    if ( ay < 0 )
      ay = -ay;
    d_in = ax + ay;

    ax = x_out;
    if ( ax < 0 )
      ax = -ax;
    ay = y_out;
    if ( ay < 0 )
      ay = -ay;
    d_out = ax + ay;

    ax = x_out + x_in;
    if ( ax < 0 )
      ax = -ax;
    ay = y_out + y_in;
    if ( ay < 0 )
      ay = -ay;
    d_corner = ax + ay;

    return ( d_in + d_out - d_corner ) < ( d_corner >> 4 );
  }


  FT_LOCAL_DEF( FT_Int )
  af_corner_orientation( FT_Pos  x_in,
                         FT_Pos  y_in,
                         FT_Pos  x_out,
                         FT_Pos  y_out )
  {
    FT_Pos  delta;


    delta = x_in * y_out - y_in * x_out;

    if ( delta == 0 )
      return 0;
    else
      return 1 - 2 * ( delta < 0 );
  }

#endif /* 0 */


  /*
   *  We are not using `af_angle_atan' anymore, but we keep the source
   *  code below just in case...
   */


#if 0


  /*
   *  The trick here is to realize that we don't need a very accurate angle
   *  approximation.  We are going to use the result of `af_angle_atan' to
   *  only compare the sign of angle differences, or check whether its
   *  magnitude is very small.
   *
   *  The approximation
   *
   *    dy * PI / (|dx|+|dy|)
   *
   *  should be enough, and much faster to compute.
   */
  FT_LOCAL_DEF( AF_Angle )
  af_angle_atan( FT_Fixed  dx,
                 FT_Fixed  dy )
  {
    AF_Angle  angle;
    FT_Fixed  ax = dx;
    FT_Fixed  ay = dy;


    if ( ax < 0 )
      ax = -ax;
    if ( ay < 0 )
      ay = -ay;

    ax += ay;

    if ( ax == 0 )
      angle = 0;
    else
    {
      angle = ( AF_ANGLE_PI2 * dy ) / ( ax + ay );
      if ( dx < 0 )
      {
        if ( angle >= 0 )
          angle = AF_ANGLE_PI - angle;
        else
          angle = -AF_ANGLE_PI - angle;
      }
    }

    return angle;
  }


#elif 0


  /* the following table has been automatically generated with */
  /* the `mather.py' Python script                             */

#define AF_ATAN_BITS  8

  static const FT_Byte  af_arctan[1L << AF_ATAN_BITS] =
  {
     0,  0,  1,  1,  1,  2,  2,  2,
     3,  3,  3,  3,  4,  4,  4,  5,
     5,  5,  6,  6,  6,  7,  7,  7,
     8,  8,  8,  9,  9,  9, 10, 10,
    10, 10, 11, 11, 11, 12, 12, 12,
    13, 13, 13, 14, 14, 14, 14, 15,
    15, 15, 16, 16, 16, 17, 17, 17,
    18, 18, 18, 18, 19, 19, 19, 20,
    20, 20, 21, 21, 21, 21, 22, 22,
    22, 23, 23, 23, 24, 24, 24, 24,
    25, 25, 25, 26, 26, 26, 26, 27,
    27, 27, 28, 28, 28, 28, 29, 29,
    29, 30, 30, 30, 30, 31, 31, 31,
    31, 32, 32, 32, 33, 33, 33, 33,
    34, 34, 34, 34, 35, 35, 35, 35,
    36, 36, 36, 36, 37, 37, 37, 38,
    38, 38, 38, 39, 39, 39, 39, 40,
    40, 40, 40, 41, 41, 41, 41, 42,
    42, 42, 42, 42, 43, 43, 43, 43,
    44, 44, 44, 44, 45, 45, 45, 45,
    46, 46, 46, 46, 46, 47, 47, 47,
    47, 48, 48, 48, 48, 48, 49, 49,
    49, 49, 50, 50, 50, 50, 50, 51,
    51, 51, 51, 51, 52, 52, 52, 52,
    52, 53, 53, 53, 53, 53, 54, 54,
    54, 54, 54, 55, 55, 55, 55, 55,
    56, 56, 56, 56, 56, 57, 57, 57,
    57, 57, 57, 58, 58, 58, 58, 58,
    59, 59, 59, 59, 59, 59, 60, 60,
    60, 60, 60, 61, 61, 61, 61, 61,
    61, 62, 62, 62, 62, 62, 62, 63,
    63, 63, 63, 63, 63, 64, 64, 64
  };


  FT_LOCAL_DEF( AF_Angle )
  af_angle_atan( FT_Fixed  dx,
                 FT_Fixed  dy )
  {
    AF_Angle  angle;


    /* check trivial cases */
    if ( dy == 0 )
    {
      angle = 0;
      if ( dx < 0 )
        angle = AF_ANGLE_PI;
      return angle;
    }
    else if ( dx == 0 )
    {
      angle = AF_ANGLE_PI2;
      if ( dy < 0 )
        angle = -AF_ANGLE_PI2;
      return angle;
    }

    angle = 0;
    if ( dx < 0 )
    {
      dx = -dx;
      dy = -dy;
      angle = AF_ANGLE_PI;
    }

    if ( dy < 0 )
    {
      FT_Pos  tmp;


      tmp = dx;
      dx  = -dy;
      dy  = tmp;
      angle -= AF_ANGLE_PI2;
    }

    if ( dx == 0 && dy == 0 )
      return 0;

    if ( dx == dy )
      angle += AF_ANGLE_PI4;
    else if ( dx > dy )
      angle += af_arctan[FT_DivFix( dy, dx ) >> ( 16 - AF_ATAN_BITS )];
    else
      angle += AF_ANGLE_PI2 -
               af_arctan[FT_DivFix( dx, dy ) >> ( 16 - AF_ATAN_BITS )];

    if ( angle > AF_ANGLE_PI )
      angle -= AF_ANGLE_2PI;

    return angle;
  }


#endif /* 0 */


  FT_LOCAL_DEF( void )
  af_sort_pos( FT_UInt  count,
               FT_Pos*  table )
  {
    FT_UInt  i, j;
    FT_Pos   swap;


    for ( i = 1; i < count; i++ )
    {
      for ( j = i; j > 0; j-- )
      {
        if ( table[j] >= table[j - 1] )
          break;

        swap         = table[j];
        table[j]     = table[j - 1];
        table[j - 1] = swap;
      }
    }
  }


  FT_LOCAL_DEF( void )
  af_sort_and_quantize_widths( FT_UInt*  count,
                               AF_Width  table,
                               FT_Pos    threshold )
  {
    FT_UInt      i, j;
    FT_UInt      cur_idx;
    FT_Pos       cur_val;
    FT_Pos       sum;
    AF_WidthRec  swap;


    if ( *count == 1 )
      return;

    /* sort */
    for ( i = 1; i < *count; i++ )
    {
      for ( j = i; j > 0; j-- )
      {
        if ( table[j].org >= table[j - 1].org )
          break;

        swap         = table[j];
        table[j]     = table[j - 1];
        table[j - 1] = swap;
      }
    }

    cur_idx = 0;
    cur_val = table[cur_idx].org;

    /* compute and use mean values for clusters not larger than  */
    /* `threshold'; this is very primitive and might not yield   */
    /* the best result, but normally, using reference character  */
    /* `o', `*count' is 2, so the code below is fully sufficient */
    for ( i = 1; i < *count; i++ )
    {
      if ( table[i].org - cur_val > threshold ||
           i == *count - 1                    )
      {
        sum = 0;

        /* fix loop for end of array */
        if ( table[i].org - cur_val <= threshold &&
             i == *count - 1                     )
          i++;

        for ( j = cur_idx; j < i; j++ )
        {
          sum         += table[j].org;
          table[j].org = 0;
        }
        table[cur_idx].org = sum / j;

        if ( i < *count - 1 )
        {
          cur_idx = i + 1;
          cur_val = table[cur_idx].org;
        }
      }
    }

    cur_idx = 1;

    /* compress array to remove zero values */
    for ( i = 1; i < *count; i++ )
    {
      if ( table[i].org )
        table[cur_idx++] = table[i];
    }

    *count = cur_idx;
  }


/* END */
/* This file has been generated by the Perl script `afblue.pl', */
/* using data from file `afblue.dat'.                           */

/***************************************************************************/
/*                                                                         */
/*  afblue.c                                                               */
/*                                                                         */
/*    Auto-fitter data for blue strings (body).                            */
/*                                                                         */
/*  Copyright 2013 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "aftypes.h"


  FT_LOCAL_ARRAY_DEF( char )
  af_blue_strings[] =
  {
    /* */
    'T', 'H', 'E', 'Z', 'O', 'C', 'Q', 'S',  /* THEZOCQS */
    '\0',
    'H', 'E', 'Z', 'L', 'O', 'C', 'U', 'S',  /* HEZLOCUS */
    '\0',
    'f', 'i', 'j', 'k', 'd', 'b', 'h',  /* fijkdbh */
    '\0',
    'x', 'z', 'r', 'o', 'e', 's', 'c',  /* xzroesc */
    '\0',
    'p', 'q', 'g', 'j', 'y',  /* pqgjy */
    '\0',
    '\xCE', '\x93', '\xCE', '\x92', '\xCE', '\x95', '\xCE', '\x96', '\xCE', '\x98', '\xCE', '\x9F', '\xCE', '\xA9',  /* ΓΒΕΖΘΟΩ */
    '\0',
    '\xCE', '\x92', '\xCE', '\x94', '\xCE', '\x96', '\xCE', '\x9E', '\xCE', '\x98', '\xCE', '\x9F',  /* ΒΔΖΞΘΟ */
    '\0',
    '\xCE', '\xB2', '\xCE', '\xB8', '\xCE', '\xB4', '\xCE', '\xB6', '\xCE', '\xBB', '\xCE', '\xBE',  /* βθδζλξ */
    '\0',
    '\xCE', '\xB1', '\xCE', '\xB5', '\xCE', '\xB9', '\xCE', '\xBF', '\xCF', '\x80', '\xCF', '\x83', '\xCF', '\x84', '\xCF', '\x89',  /* αειοπστω */
    '\0',
    '\xCE', '\xB2', '\xCE', '\xB3', '\xCE', '\xB7', '\xCE', '\xBC', '\xCF', '\x81', '\xCF', '\x86', '\xCF', '\x87', '\xCF', '\x88',  /* βγημρφχψ */
    '\0',
    '\xD0', '\x91', '\xD0', '\x92', '\xD0', '\x95', '\xD0', '\x9F', '\xD0', '\x97', '\xD0', '\x9E', '\xD0', '\xA1', '\xD0', '\xAD',  /* БВЕПЗОСЭ */
    '\0',
    '\xD0', '\x91', '\xD0', '\x92', '\xD0', '\x95', '\xD0', '\xA8', '\xD0', '\x97', '\xD0', '\x9E', '\xD0', '\xA1', '\xD0', '\xAD',  /* БВЕШЗОСЭ */
    '\0',
    '\xD1', '\x85', '\xD0', '\xBF', '\xD0', '\xBD', '\xD1', '\x88', '\xD0', '\xB5', '\xD0', '\xB7', '\xD0', '\xBE', '\xD1', '\x81',  /* хпншезос */
    '\0',
    '\xD1', '\x80', '\xD1', '\x83', '\xD1', '\x84',  /* руф */
    '\0',
    '\xD7', '\x91', '\xD7', '\x93', '\xD7', '\x94', '\xD7', '\x97', '\xD7', '\x9A', '\xD7', '\x9B', '\xD7', '\x9D', '\xD7', '\xA1',  /* בדהחךכםס */
    '\0',
    '\xD7', '\x91', '\xD7', '\x98', '\xD7', '\x9B', '\xD7', '\x9D', '\xD7', '\xA1', '\xD7', '\xA6',  /* בטכםסצ */
    '\0',
    '\xD7', '\xA7', '\xD7', '\x9A', '\xD7', '\x9F', '\xD7', '\xA3', '\xD7', '\xA5',  /* קךןףץ */
#ifdef AF_CONFIG_OPTION_CJK
    '\0',
    '\xE4', '\xBB', '\x96', '\xE4', '\xBB', '\xAC', '\xE4', '\xBD', '\xA0', '\xE4', '\xBE', '\x86', '\xE5', '\x80', '\x91', '\xE5', '\x88', '\xB0', '\xE5', '\x92', '\x8C', '\xE5', '\x9C', '\xB0',  /* 他们你來們到和地 */
    '\xE5', '\xAF', '\xB9', '\xE5', '\xB0', '\x8D', '\xE5', '\xB0', '\xB1', '\xE5', '\xB8', '\xAD', '\xE6', '\x88', '\x91', '\xE6', '\x97', '\xB6', '\xE6', '\x99', '\x82', '\xE6', '\x9C', '\x83',  /* 对對就席我时時會 */
    '\xE6', '\x9D', '\xA5', '\xE7', '\x82', '\xBA', '\xE8', '\x83', '\xBD', '\xE8', '\x88', '\xB0', '\xE8', '\xAA', '\xAA', '\xE8', '\xAF', '\xB4', '\xE8', '\xBF', '\x99', '\xE9', '\x80', '\x99',  /* 来為能舰說说这這 */
    '\xE9', '\xBD', '\x8A',  /* 齊 */
    '\0',
    '\xE5', '\x86', '\x9B', '\xE5', '\x90', '\x8C', '\xE5', '\xB7', '\xB2', '\xE6', '\x84', '\xBF', '\xE6', '\x97', '\xA2', '\xE6', '\x98', '\x9F', '\xE6', '\x98', '\xAF', '\xE6', '\x99', '\xAF',  /* 军同已愿既星是景 */
    '\xE6', '\xB0', '\x91', '\xE7', '\x85', '\xA7', '\xE7', '\x8E', '\xB0', '\xE7', '\x8F', '\xBE', '\xE7', '\x90', '\x86', '\xE7', '\x94', '\xA8', '\xE7', '\xBD', '\xAE', '\xE8', '\xA6', '\x81',  /* 民照现現理用置要 */
    '\xE8', '\xBB', '\x8D', '\xE9', '\x82', '\xA3', '\xE9', '\x85', '\x8D', '\xE9', '\x87', '\x8C', '\xE9', '\x96', '\x8B', '\xE9', '\x9B', '\xB7', '\xE9', '\x9C', '\xB2', '\xE9', '\x9D', '\xA2',  /* 軍那配里開雷露面 */
    '\xE9', '\xA1', '\xBE',  /* 顾 */
    '\0',
    '\xE4', '\xB8', '\xAA', '\xE4', '\xB8', '\xBA', '\xE4', '\xBA', '\xBA', '\xE4', '\xBB', '\x96', '\xE4', '\xBB', '\xA5', '\xE4', '\xBB', '\xAC', '\xE4', '\xBD', '\xA0', '\xE4', '\xBE', '\x86',  /* 个为人他以们你來 */
    '\xE5', '\x80', '\x8B', '\xE5', '\x80', '\x91', '\xE5', '\x88', '\xB0', '\xE5', '\x92', '\x8C', '\xE5', '\xA4', '\xA7', '\xE5', '\xAF', '\xB9', '\xE5', '\xB0', '\x8D', '\xE5', '\xB0', '\xB1',  /* 個們到和大对對就 */
    '\xE6', '\x88', '\x91', '\xE6', '\x97', '\xB6', '\xE6', '\x99', '\x82', '\xE6', '\x9C', '\x89', '\xE6', '\x9D', '\xA5', '\xE7', '\x82', '\xBA', '\xE8', '\xA6', '\x81', '\xE8', '\xAA', '\xAA',  /* 我时時有来為要說 */
    '\xE8', '\xAF', '\xB4',  /* 说 */
    '\0',
    '\xE4', '\xB8', '\xBB', '\xE4', '\xBA', '\x9B', '\xE5', '\x9B', '\xA0', '\xE5', '\xAE', '\x83', '\xE6', '\x83', '\xB3', '\xE6', '\x84', '\x8F', '\xE7', '\x90', '\x86', '\xE7', '\x94', '\x9F',  /* 主些因它想意理生 */
    '\xE7', '\x95', '\xB6', '\xE7', '\x9C', '\x8B', '\xE7', '\x9D', '\x80', '\xE7', '\xBD', '\xAE', '\xE8', '\x80', '\x85', '\xE8', '\x87', '\xAA', '\xE8', '\x91', '\x97', '\xE8', '\xA3', '\xA1',  /* 當看着置者自著裡 */
    '\xE8', '\xBF', '\x87', '\xE8', '\xBF', '\x98', '\xE8', '\xBF', '\x9B', '\xE9', '\x80', '\xB2', '\xE9', '\x81', '\x8E', '\xE9', '\x81', '\x93', '\xE9', '\x82', '\x84', '\xE9', '\x87', '\x8C',  /* 过还进進過道還里 */
    '\xE9', '\x9D', '\xA2',  /* 面 */
#ifdef AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT
    '\0',
    '\xE4', '\xBA', '\x9B', '\xE4', '\xBB', '\xAC', '\xE4', '\xBD', '\xA0', '\xE4', '\xBE', '\x86', '\xE5', '\x80', '\x91', '\xE5', '\x88', '\xB0', '\xE5', '\x92', '\x8C', '\xE5', '\x9C', '\xB0',  /* 些们你來們到和地 */
    '\xE5', '\xA5', '\xB9', '\xE5', '\xB0', '\x86', '\xE5', '\xB0', '\x87', '\xE5', '\xB0', '\xB1', '\xE5', '\xB9', '\xB4', '\xE5', '\xBE', '\x97', '\xE6', '\x83', '\x85', '\xE6', '\x9C', '\x80',  /* 她将將就年得情最 */
    '\xE6', '\xA0', '\xB7', '\xE6', '\xA8', '\xA3', '\xE7', '\x90', '\x86', '\xE8', '\x83', '\xBD', '\xE8', '\xAA', '\xAA', '\xE8', '\xAF', '\xB4', '\xE8', '\xBF', '\x99', '\xE9', '\x80', '\x99',  /* 样樣理能說说这這 */
    '\xE9', '\x80', '\x9A',  /* 通 */
    '\0',
    '\xE5', '\x8D', '\xB3', '\xE5', '\x90', '\x97', '\xE5', '\x90', '\xA7', '\xE5', '\x90', '\xAC', '\xE5', '\x91', '\xA2', '\xE5', '\x93', '\x81', '\xE5', '\x93', '\x8D', '\xE5', '\x97', '\x8E',  /* 即吗吧听呢品响嗎 */
    '\xE5', '\xB8', '\x88', '\xE5', '\xB8', '\xAB', '\xE6', '\x94', '\xB6', '\xE6', '\x96', '\xAD', '\xE6', '\x96', '\xB7', '\xE6', '\x98', '\x8E', '\xE7', '\x9C', '\xBC', '\xE9', '\x96', '\x93',  /* 师師收断斷明眼間 */
    '\xE9', '\x97', '\xB4', '\xE9', '\x99', '\x85', '\xE9', '\x99', '\x88', '\xE9', '\x99', '\x90', '\xE9', '\x99', '\xA4', '\xE9', '\x99', '\xB3', '\xE9', '\x9A', '\x8F', '\xE9', '\x9A', '\x9B',  /* 间际陈限除陳随際 */
    '\xE9', '\x9A', '\xA8',  /* 隨 */
    '\0',
    '\xE4', '\xBA', '\x8B', '\xE5', '\x89', '\x8D', '\xE5', '\xAD', '\xB8', '\xE5', '\xB0', '\x86', '\xE5', '\xB0', '\x87', '\xE6', '\x83', '\x85', '\xE6', '\x83', '\xB3', '\xE6', '\x88', '\x96',  /* 事前學将將情想或 */
    '\xE6', '\x94', '\xBF', '\xE6', '\x96', '\xAF', '\xE6', '\x96', '\xB0', '\xE6', '\xA0', '\xB7', '\xE6', '\xA8', '\xA3', '\xE6', '\xB0', '\x91', '\xE6', '\xB2', '\x92', '\xE6', '\xB2', '\xA1',  /* 政斯新样樣民沒没 */
    '\xE7', '\x84', '\xB6', '\xE7', '\x89', '\xB9', '\xE7', '\x8E', '\xB0', '\xE7', '\x8F', '\xBE', '\xE7', '\x90', '\x83', '\xE7', '\xAC', '\xAC', '\xE7', '\xB6', '\x93', '\xE8', '\xB0', '\x81',  /* 然特现現球第經谁 */
    '\xE8', '\xB5', '\xB7',  /* 起 */
    '\0',
    '\xE4', '\xBE', '\x8B', '\xE5', '\x88', '\xA5', '\xE5', '\x88', '\xAB', '\xE5', '\x88', '\xB6', '\xE5', '\x8A', '\xA8', '\xE5', '\x8B', '\x95', '\xE5', '\x90', '\x97', '\xE5', '\x97', '\x8E',  /* 例別别制动動吗嗎 */
    '\xE5', '\xA2', '\x9E', '\xE6', '\x8C', '\x87', '\xE6', '\x98', '\x8E', '\xE6', '\x9C', '\x9D', '\xE6', '\x9C', '\x9F', '\xE6', '\x9E', '\x84', '\xE7', '\x89', '\xA9', '\xE7', '\xA1', '\xAE',  /* 增指明朝期构物确 */
    '\xE7', '\xA7', '\x8D', '\xE8', '\xAA', '\xBF', '\xE8', '\xB0', '\x83', '\xE8', '\xB2', '\xBB', '\xE8', '\xB4', '\xB9', '\xE9', '\x82', '\xA3', '\xE9', '\x83', '\xBD', '\xE9', '\x96', '\x93',  /* 种調调費费那都間 */
    '\xE9', '\x97', '\xB4',  /* 间 */
#endif /* AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT */
#endif /* AF_CONFIG_OPTION_CJK                */
    '\0',

  };


  /* stringsets are specific to styles */
  FT_LOCAL_ARRAY_DEF( AF_Blue_StringRec )
  af_blue_stringsets[] =
  {
    /* */
    { AF_BLUE_STRING_LATIN_CAPITAL_TOP,     AF_BLUE_PROPERTY_LATIN_TOP        },
    { AF_BLUE_STRING_LATIN_CAPITAL_BOTTOM,  0                                 },
    { AF_BLUE_STRING_LATIN_SMALL_F_TOP,     AF_BLUE_PROPERTY_LATIN_TOP        },
    { AF_BLUE_STRING_LATIN_SMALL,           AF_BLUE_PROPERTY_LATIN_TOP      |
                                            AF_BLUE_PROPERTY_LATIN_X_HEIGHT   },
    { AF_BLUE_STRING_LATIN_SMALL,           0                                 },
    { AF_BLUE_STRING_LATIN_SMALL_DESCENDER, 0                                 },
    { AF_BLUE_STRING_MAX,                   0                                 },
    { AF_BLUE_STRING_GREEK_CAPITAL_TOP,     AF_BLUE_PROPERTY_LATIN_TOP        },
    { AF_BLUE_STRING_GREEK_CAPITAL_BOTTOM,  0                                 },
    { AF_BLUE_STRING_GREEK_SMALL_BETA_TOP,  AF_BLUE_PROPERTY_LATIN_TOP        },
    { AF_BLUE_STRING_GREEK_SMALL,           AF_BLUE_PROPERTY_LATIN_TOP      |
                                            AF_BLUE_PROPERTY_LATIN_X_HEIGHT   },
    { AF_BLUE_STRING_GREEK_SMALL,           0                                 },
    { AF_BLUE_STRING_GREEK_SMALL_DESCENDER, 0                                 },
    { AF_BLUE_STRING_MAX,                   0                                 },
    { AF_BLUE_STRING_CYRILLIC_CAPITAL_TOP,     AF_BLUE_PROPERTY_LATIN_TOP        },
    { AF_BLUE_STRING_CYRILLIC_CAPITAL_BOTTOM,  0                                 },
    { AF_BLUE_STRING_CYRILLIC_SMALL,           AF_BLUE_PROPERTY_LATIN_TOP      |
                                               AF_BLUE_PROPERTY_LATIN_X_HEIGHT   },
    { AF_BLUE_STRING_CYRILLIC_SMALL,           0                                 },
    { AF_BLUE_STRING_CYRILLIC_SMALL_DESCENDER, 0                                 },
    { AF_BLUE_STRING_MAX,                      0                                 },
    { AF_BLUE_STRING_HEBREW_TOP,       AF_BLUE_PROPERTY_LATIN_TOP  |
                                       AF_BLUE_PROPERTY_LATIN_LONG   },
    { AF_BLUE_STRING_HEBREW_BOTTOM,    0                             },
    { AF_BLUE_STRING_HEBREW_DESCENDER, 0                             },
    { AF_BLUE_STRING_MAX,              0                             },
#ifdef AF_CONFIG_OPTION_CJK
    { AF_BLUE_STRING_CJK_TOP_FILL,      AF_BLUE_PROPERTY_CJK_TOP |
                                        AF_BLUE_PROPERTY_CJK_FILL    },
    { AF_BLUE_STRING_CJK_TOP_UNFILL,    AF_BLUE_PROPERTY_CJK_TOP     },
    { AF_BLUE_STRING_CJK_BOTTOM_FILL,   AF_BLUE_PROPERTY_CJK_FILL    },
    { AF_BLUE_STRING_CJK_BOTTOM_UNFILL, 0                            },
#ifdef AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT
    { AF_BLUE_STRING_CJK_LEFT_FILL,     AF_BLUE_PROPERTY_CJK_HORIZ |
                                        AF_BLUE_PROPERTY_CJK_FILL    },
    { AF_BLUE_STRING_CJK_LEFT_UNFILL,   AF_BLUE_PROPERTY_CJK_HORIZ   },
    { AF_BLUE_STRING_CJK_RIGHT_FILL,    AF_BLUE_PROPERTY_CJK_HORIZ |
                                        AF_BLUE_PROPERTY_CJK_RIGHT |
                                        AF_BLUE_PROPERTY_CJK_FILL    },
    { AF_BLUE_STRING_CJK_RIGHT_UNFILL,  AF_BLUE_PROPERTY_CJK_HORIZ |
                                        AF_BLUE_PROPERTY_CJK_RIGHT   },
#endif /* AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT */
    { AF_BLUE_STRING_MAX,               0                            },
#endif /* AF_CONFIG_OPTION_CJK                */

  };


/* END */
/***************************************************************************/
/*                                                                         */
/*  afglobal.c                                                             */
/*                                                                         */
/*    Auto-fitter routines to compute global hinting values (body).        */
/*                                                                         */
/*  Copyright 2003-2014 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "afglobal.h"
#include "afranges.h"
#include "hbshim.h"
#include FT_INTERNAL_DEBUG_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_afglobal


  /* get writing system specific header files */
#undef  WRITING_SYSTEM
#define WRITING_SYSTEM( ws, WS )  /* empty */
#include "afwrtsys.h"

#include "aferrors.h"
#include "afpic.h"


#undef  SCRIPT
#define SCRIPT( s, S, d, h, sc1, sc2, sc3 ) \
          AF_DEFINE_SCRIPT_CLASS(           \
            af_ ## s ## _script_class,      \
            AF_SCRIPT_ ## S,                \
            af_ ## s ## _uniranges,         \
            sc1, sc2, sc3 )

#include "afscript.h"


#undef  STYLE
#define STYLE( s, S, d, ws, sc, ss, c )  \
          AF_DEFINE_STYLE_CLASS(         \
            af_ ## s ## _style_class,    \
            AF_STYLE_ ## S,              \
            ws,                          \
            sc,                          \
            ss,                          \
            c )

#include "afstyles.h"


#ifndef FT_CONFIG_OPTION_PIC

#undef  WRITING_SYSTEM
#define WRITING_SYSTEM( ws, WS )               \
          &af_ ## ws ## _writing_system_class,

  FT_LOCAL_ARRAY_DEF( AF_WritingSystemClass )
  af_writing_system_classes[] =
  {

#include "afwrtsys.h"

    NULL  /* do not remove */
  };


#undef  SCRIPT
#define SCRIPT( s, S, d, h, sc1, sc2, sc3 ) \
          &af_ ## s ## _script_class,

  FT_LOCAL_ARRAY_DEF( AF_ScriptClass )
  af_script_classes[] =
  {

#include "afscript.h"

    NULL  /* do not remove */
  };


#undef  STYLE
#define STYLE( s, S, d, ws, sc, ss, c ) \
          &af_ ## s ## _style_class,

  FT_LOCAL_ARRAY_DEF( AF_StyleClass )
  af_style_classes[] =
  {

#include "afstyles.h"

    NULL  /* do not remove */
  };

#endif /* !FT_CONFIG_OPTION_PIC */


#ifdef FT_DEBUG_LEVEL_TRACE

#undef  STYLE
#define STYLE( s, S, d, ws, sc, ss, c )  #s,

  FT_LOCAL_ARRAY_DEF( char* )
  af_style_names[] =
  {

#include "afstyles.h"

  };

#endif /* FT_DEBUG_LEVEL_TRACE */


  /* Compute the style index of each glyph within a given face. */

  static FT_Error
  af_face_globals_compute_style_coverage( AF_FaceGlobals  globals )
  {
    FT_Error    error;
    FT_Face     face        = globals->face;
    FT_CharMap  old_charmap = face->charmap;
    FT_Byte*    gstyles     = globals->glyph_styles;
    FT_UInt     ss;
    FT_UInt     i;
    FT_UInt     dflt        = -1;


    /* the value AF_STYLE_UNASSIGNED means `uncovered glyph' */
    FT_MEM_SET( globals->glyph_styles,
                AF_STYLE_UNASSIGNED,
                globals->glyph_count );

    error = FT_Select_Charmap( face, FT_ENCODING_UNICODE );
    if ( error )
    {
      /*
       * Ignore this error; we simply use the fallback style.
       * XXX: Shouldn't we rather disable hinting?
       */
      error = FT_Err_Ok;
      goto Exit;
    }

    /* scan each style in a Unicode charmap */
    for ( ss = 0; AF_STYLE_CLASSES_GET[ss]; ss++ )
    {
      AF_StyleClass       style_class =
                            AF_STYLE_CLASSES_GET[ss];
      AF_ScriptClass      script_class =
                            AF_SCRIPT_CLASSES_GET[style_class->script];
      AF_Script_UniRange  range;


      if ( script_class->script_uni_ranges == NULL )
        continue;

      /*
       *  Scan all Unicode points in the range and set the corresponding
       *  glyph style index.
       */
      if ( style_class->coverage == AF_COVERAGE_DEFAULT )
      {
        if ( (int)style_class->script == (int)globals->module->default_script )
          dflt = ss;

        for ( range = script_class->script_uni_ranges;
              range->first != 0;
              range++ )
        {
          FT_ULong  charcode = range->first;
          FT_UInt   gindex;


          gindex = FT_Get_Char_Index( face, charcode );

          if ( gindex != 0                             &&
               gindex < (FT_ULong)globals->glyph_count &&
               gstyles[gindex] == AF_STYLE_UNASSIGNED  )
            gstyles[gindex] = (FT_Byte)ss;

          for (;;)
          {
            charcode = FT_Get_Next_Char( face, charcode, &gindex );

            if ( gindex == 0 || charcode > range->last )
              break;

            if ( gindex < (FT_ULong)globals->glyph_count &&
                 gstyles[gindex] == AF_STYLE_UNASSIGNED  )
              gstyles[gindex] = (FT_Byte)ss;
          }
        }
      }
      else
      {
        /* get glyphs not directly addressable by cmap */
        af_get_coverage( globals, style_class, gstyles );
      }
    }

    /* handle the default OpenType features of the default script ... */
    af_get_coverage( globals, AF_STYLE_CLASSES_GET[dflt], gstyles );

    /* ... and the remaining default OpenType features */
    for ( ss = 0; AF_STYLE_CLASSES_GET[ss]; ss++ )
    {
      AF_StyleClass  style_class = AF_STYLE_CLASSES_GET[ss];


      if ( ss != dflt && style_class->coverage == AF_COVERAGE_DEFAULT )
        af_get_coverage( globals, style_class, gstyles );
    }

    /* mark ASCII digits */
    for ( i = 0x30; i <= 0x39; i++ )
    {
      FT_UInt  gindex = FT_Get_Char_Index( face, i );


      if ( gindex != 0 && gindex < (FT_ULong)globals->glyph_count )
        gstyles[gindex] |= AF_DIGIT;
    }

  Exit:
    /*
     *  By default, all uncovered glyphs are set to the fallback style.
     *  XXX: Shouldn't we disable hinting or do something similar?
     */
    if ( globals->module->fallback_style != AF_STYLE_UNASSIGNED )
    {
      FT_Long  nn;


      for ( nn = 0; nn < globals->glyph_count; nn++ )
      {
        if ( ( gstyles[nn] & ~AF_DIGIT ) == AF_STYLE_UNASSIGNED )
        {
          gstyles[nn] &= ~AF_STYLE_UNASSIGNED;
          gstyles[nn] |= globals->module->fallback_style;
        }
      }
    }

#ifdef FT_DEBUG_LEVEL_TRACE

    FT_TRACE4(( "\n"
                "style coverage\n"
                "==============\n"
                "\n" ));

    for ( ss = 0; AF_STYLE_CLASSES_GET[ss]; ss++ )
    {
      AF_StyleClass  style_class = AF_STYLE_CLASSES_GET[ss];
      FT_UInt        count       = 0;
      FT_Long        idx;


      FT_TRACE4(( "%s:\n", af_style_names[style_class->style] ));

      for ( idx = 0; idx < globals->glyph_count; idx++ )
      {
        if ( ( gstyles[idx] & ~AF_DIGIT ) == style_class->style )
        {
          if ( !( count % 10 ) )
            FT_TRACE4(( " " ));

          FT_TRACE4(( " %d", idx ));
          count++;

          if ( !( count % 10 ) )
            FT_TRACE4(( "\n" ));
        }
      }

      if ( !count )
        FT_TRACE4(( "  (none)\n" ));
      if ( count % 10 )
        FT_TRACE4(( "\n" ));
    }

#endif /* FT_DEBUG_LEVEL_TRACE */

    FT_Set_Charmap( face, old_charmap );
    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  af_face_globals_new( FT_Face          face,
                       AF_FaceGlobals  *aglobals,
                       AF_Module        module )
  {
    FT_Error        error;
    FT_Memory       memory;
    AF_FaceGlobals  globals = NULL;


    memory = face->memory;

    if ( FT_ALLOC( globals, sizeof ( *globals ) +
                            face->num_glyphs * sizeof ( FT_Byte ) ) )
      goto Exit;

    globals->face         = face;
    globals->glyph_count  = face->num_glyphs;
    globals->glyph_styles = (FT_Byte*)( globals + 1 );
    globals->module       = module;

#ifdef FT_CONFIG_OPTION_USE_HARFBUZZ
    globals->hb_font = hb_ft_font_create( face, NULL );
#endif

    error = af_face_globals_compute_style_coverage( globals );
    if ( error )
    {
      af_face_globals_free( globals );
      globals = NULL;
    }

    globals->increase_x_height = AF_PROP_INCREASE_X_HEIGHT_MAX;

  Exit:
    *aglobals = globals;
    return error;
  }


  FT_LOCAL_DEF( void )
  af_face_globals_free( AF_FaceGlobals  globals )
  {
    if ( globals )
    {
      FT_Memory  memory = globals->face->memory;
      FT_UInt    nn;


      for ( nn = 0; nn < AF_STYLE_MAX; nn++ )
      {
        if ( globals->metrics[nn] )
        {
          AF_StyleClass          style_class =
            AF_STYLE_CLASSES_GET[nn];
          AF_WritingSystemClass  writing_system_class =
            AF_WRITING_SYSTEM_CLASSES_GET[style_class->writing_system];


          if ( writing_system_class->style_metrics_done )
            writing_system_class->style_metrics_done( globals->metrics[nn] );

          FT_FREE( globals->metrics[nn] );
        }
      }

#ifdef FT_CONFIG_OPTION_USE_HARFBUZZ
      hb_font_destroy( globals->hb_font );
      globals->hb_font = NULL;
#endif

      globals->glyph_count  = 0;
      globals->glyph_styles = NULL;  /* no need to free this one! */
      globals->face         = NULL;

      FT_FREE( globals );
    }
  }


  FT_LOCAL_DEF( FT_Error )
  af_face_globals_get_metrics( AF_FaceGlobals    globals,
                               FT_UInt           gindex,
                               FT_UInt           options,
                               AF_StyleMetrics  *ametrics )
  {
    AF_StyleMetrics  metrics = NULL;

    AF_Style               style = (AF_Style)options;
    AF_WritingSystemClass  writing_system_class;
    AF_StyleClass          style_class;

    FT_Error  error = FT_Err_Ok;


    if ( gindex >= (FT_ULong)globals->glyph_count )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    /* if we have a forced style (via `options'), use it, */
    /* otherwise look into `glyph_styles' array           */
    if ( style == AF_STYLE_NONE_DFLT || style + 1 >= AF_STYLE_MAX )
      style = (AF_Style)( globals->glyph_styles[gindex] &
                          AF_STYLE_UNASSIGNED           );

    style_class          = AF_STYLE_CLASSES_GET[style];
    writing_system_class = AF_WRITING_SYSTEM_CLASSES_GET
                             [style_class->writing_system];

    metrics = globals->metrics[style];
    if ( metrics == NULL )
    {
      /* create the global metrics object if necessary */
      FT_Memory  memory = globals->face->memory;


      if ( FT_ALLOC( metrics, writing_system_class->style_metrics_size ) )
        goto Exit;

      metrics->style_class = style_class;
      metrics->globals     = globals;

      if ( writing_system_class->style_metrics_init )
      {
        error = writing_system_class->style_metrics_init( metrics,
                                                          globals->face );
        if ( error )
        {
          if ( writing_system_class->style_metrics_done )
            writing_system_class->style_metrics_done( metrics );

          FT_FREE( metrics );
          goto Exit;
        }
      }

      globals->metrics[style] = metrics;
    }

  Exit:
    *ametrics = metrics;

    return error;
  }


  FT_LOCAL_DEF( FT_Bool )
  af_face_globals_is_digit( AF_FaceGlobals  globals,
                            FT_UInt         gindex )
  {
    if ( gindex < (FT_ULong)globals->glyph_count )
      return (FT_Bool)( globals->glyph_styles[gindex] & AF_DIGIT );

    return (FT_Bool)0;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  afhints.c                                                              */
/*                                                                         */
/*    Auto-fitter hinting routines (body).                                 */
/*                                                                         */
/*  Copyright 2003-2007, 2009-2014 by                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "afhints.h"
#include "aferrors.h"
#include FT_INTERNAL_CALC_H
#include FT_INTERNAL_DEBUG_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_afhints


  /* Get new segment for given axis. */

  FT_LOCAL_DEF( FT_Error )
  af_axis_hints_new_segment( AF_AxisHints  axis,
                             FT_Memory     memory,
                             AF_Segment   *asegment )
  {
    FT_Error    error   = FT_Err_Ok;
    AF_Segment  segment = NULL;


    if ( axis->num_segments >= axis->max_segments )
    {
      FT_Int  old_max = axis->max_segments;
      FT_Int  new_max = old_max;
      FT_Int  big_max = (FT_Int)( FT_INT_MAX / sizeof ( *segment ) );


      if ( old_max >= big_max )
      {
        error = FT_THROW( Out_Of_Memory );
        goto Exit;
      }

      new_max += ( new_max >> 2 ) + 4;
      if ( new_max < old_max || new_max > big_max )
        new_max = big_max;

      if ( FT_RENEW_ARRAY( axis->segments, old_max, new_max ) )
        goto Exit;

      axis->max_segments = new_max;
    }

    segment = axis->segments + axis->num_segments++;

  Exit:
    *asegment = segment;
    return error;
  }


  /* Get new edge for given axis, direction, and position. */

  FT_LOCAL( FT_Error )
  af_axis_hints_new_edge( AF_AxisHints  axis,
                          FT_Int        fpos,
                          AF_Direction  dir,
                          FT_Memory     memory,
                          AF_Edge      *anedge )
  {
    FT_Error  error = FT_Err_Ok;
    AF_Edge   edge  = NULL;
    AF_Edge   edges;


    if ( axis->num_edges >= axis->max_edges )
    {
      FT_Int  old_max = axis->max_edges;
      FT_Int  new_max = old_max;
      FT_Int  big_max = (FT_Int)( FT_INT_MAX / sizeof ( *edge ) );


      if ( old_max >= big_max )
      {
        error = FT_THROW( Out_Of_Memory );
        goto Exit;
      }

      new_max += ( new_max >> 2 ) + 4;
      if ( new_max < old_max || new_max > big_max )
        new_max = big_max;

      if ( FT_RENEW_ARRAY( axis->edges, old_max, new_max ) )
        goto Exit;

      axis->max_edges = new_max;
    }

    edges = axis->edges;
    edge  = edges + axis->num_edges;

    while ( edge > edges )
    {
      if ( edge[-1].fpos < fpos )
        break;

      /* we want the edge with same position and minor direction */
      /* to appear before those in the major one in the list     */
      if ( edge[-1].fpos == fpos && dir == axis->major_dir )
        break;

      edge[0] = edge[-1];
      edge--;
    }

    axis->num_edges++;

    FT_ZERO( edge );
    edge->fpos = (FT_Short)fpos;
    edge->dir  = (FT_Char)dir;

  Exit:
    *anedge = edge;
    return error;
  }


#ifdef FT_DEBUG_AUTOFIT

#include FT_CONFIG_STANDARD_LIBRARY_H

  /* The dump functions are used in the `ftgrid' demo program, too. */
#define AF_DUMP( varformat )          \
          do                          \
          {                           \
            if ( to_stdout )          \
              printf varformat;       \
            else                      \
              FT_TRACE7( varformat ); \
          } while ( 0 )


  static const char*
  af_dir_str( AF_Direction  dir )
  {
    const char*  result;


    switch ( dir )
    {
    case AF_DIR_UP:
      result = "up";
      break;
    case AF_DIR_DOWN:
      result = "down";
      break;
    case AF_DIR_LEFT:
      result = "left";
      break;
    case AF_DIR_RIGHT:
      result = "right";
      break;
    default:
      result = "none";
    }

    return result;
  }


#define AF_INDEX_NUM( ptr, base )  ( (ptr) ? ( (ptr) - (base) ) : -1 )


#ifdef __cplusplus
  extern "C" {
#endif
  void
  af_glyph_hints_dump_points( AF_GlyphHints  hints,
                              FT_Bool        to_stdout )
  {
    AF_Point  points = hints->points;
    AF_Point  limit  = points + hints->num_points;
    AF_Point  point;


    AF_DUMP(( "Table of points:\n"
              "  [ index |  xorg |  yorg | xscale | yscale"
              " |  xfit |  yfit |  flags ]\n" ));

    for ( point = points; point < limit; point++ )
      AF_DUMP(( "  [ %5d | %5d | %5d | %6.2f | %6.2f"
                " | %5.2f | %5.2f | %c%c%c%c%c%c ]\n",
                point - points,
                point->fx,
                point->fy,
                point->ox / 64.0,
                point->oy / 64.0,
                point->x / 64.0,
                point->y / 64.0,
                ( point->flags & AF_FLAG_WEAK_INTERPOLATION ) ? 'w' : ' ',
                ( point->flags & AF_FLAG_INFLECTION )         ? 'i' : ' ',
                ( point->flags & AF_FLAG_EXTREMA_X )          ? '<' : ' ',
                ( point->flags & AF_FLAG_EXTREMA_Y )          ? 'v' : ' ',
                ( point->flags & AF_FLAG_ROUND_X )            ? '(' : ' ',
                ( point->flags & AF_FLAG_ROUND_Y )            ? 'u' : ' '));
    AF_DUMP(( "\n" ));
  }
#ifdef __cplusplus
  }
#endif


  static const char*
  af_edge_flags_to_string( AF_Edge_Flags  flags )
  {
    static char  temp[32];
    int          pos = 0;


    if ( flags & AF_EDGE_ROUND )
    {
      ft_memcpy( temp + pos, "round", 5 );
      pos += 5;
    }
    if ( flags & AF_EDGE_SERIF )
    {
      if ( pos > 0 )
        temp[pos++] = ' ';
      ft_memcpy( temp + pos, "serif", 5 );
      pos += 5;
    }
    if ( pos == 0 )
      return "normal";

    temp[pos] = '\0';

    return temp;
  }


  /* Dump the array of linked segments. */

#ifdef __cplusplus
  extern "C" {
#endif
  void
  af_glyph_hints_dump_segments( AF_GlyphHints  hints,
                                FT_Bool        to_stdout )
  {
    FT_Int  dimension;


    for ( dimension = 1; dimension >= 0; dimension-- )
    {
      AF_AxisHints  axis     = &hints->axis[dimension];
      AF_Point      points   = hints->points;
      AF_Edge       edges    = axis->edges;
      AF_Segment    segments = axis->segments;
      AF_Segment    limit    = segments + axis->num_segments;
      AF_Segment    seg;


      AF_DUMP(( "Table of %s segments:\n",
                dimension == AF_DIMENSION_HORZ ? "vertical"
                                               : "horizontal" ));
      if ( axis->num_segments )
        AF_DUMP(( "  [ index |  pos  |  dir  | from"
                  " |  to  | link | serif | edge"
                  " | height | extra |    flags    ]\n" ));
      else
        AF_DUMP(( "  (none)\n" ));

      for ( seg = segments; seg < limit; seg++ )
        AF_DUMP(( "  [ %5d | %5.2g | %5s | %4d"
                  " | %4d | %4d | %5d | %4d"
                  " | %6d | %5d | %11s ]\n",
                  seg - segments,
                  dimension == AF_DIMENSION_HORZ
                               ? (int)seg->first->ox / 64.0
                               : (int)seg->first->oy / 64.0,
                  af_dir_str( (AF_Direction)seg->dir ),
                  AF_INDEX_NUM( seg->first, points ),
                  AF_INDEX_NUM( seg->last, points ),
                  AF_INDEX_NUM( seg->link, segments ),
                  AF_INDEX_NUM( seg->serif, segments ),
                  AF_INDEX_NUM( seg->edge, edges ),
                  seg->height,
                  seg->height - ( seg->max_coord - seg->min_coord ),
                  af_edge_flags_to_string( (AF_Edge_Flags)seg->flags ) ));
      AF_DUMP(( "\n" ));
    }
  }
#ifdef __cplusplus
  }
#endif


  /* Fetch number of segments. */

#ifdef __cplusplus
  extern "C" {
#endif
  FT_Error
  af_glyph_hints_get_num_segments( AF_GlyphHints  hints,
                                   FT_Int         dimension,
                                   FT_Int*        num_segments )
  {
    AF_Dimension  dim;
    AF_AxisHints  axis;


    dim = ( dimension == 0 ) ? AF_DIMENSION_HORZ : AF_DIMENSION_VERT;

    axis          = &hints->axis[dim];
    *num_segments = axis->num_segments;

    return FT_Err_Ok;
  }
#ifdef __cplusplus
  }
#endif


  /* Fetch offset of segments into user supplied offset array. */

#ifdef __cplusplus
  extern "C" {
#endif
  FT_Error
  af_glyph_hints_get_segment_offset( AF_GlyphHints  hints,
                                     FT_Int         dimension,
                                     FT_Int         idx,
                                     FT_Pos        *offset,
                                     FT_Bool       *is_blue,
                                     FT_Pos        *blue_offset )
  {
    AF_Dimension  dim;
    AF_AxisHints  axis;
    AF_Segment    seg;


    if ( !offset )
      return FT_THROW( Invalid_Argument );

    dim = ( dimension == 0 ) ? AF_DIMENSION_HORZ : AF_DIMENSION_VERT;

    axis = &hints->axis[dim];

    if ( idx < 0 || idx >= axis->num_segments )
      return FT_THROW( Invalid_Argument );

    seg      = &axis->segments[idx];
    *offset  = ( dim == AF_DIMENSION_HORZ ) ? seg->first->ox
                                            : seg->first->oy;
    if ( seg->edge )
      *is_blue = (FT_Bool)( seg->edge->blue_edge != 0 );
    else
      *is_blue = FALSE;

    if ( *is_blue )
      *blue_offset = seg->edge->blue_edge->cur;
    else
      *blue_offset = 0;

    return FT_Err_Ok;
  }
#ifdef __cplusplus
  }
#endif


  /* Dump the array of linked edges. */

#ifdef __cplusplus
  extern "C" {
#endif
  void
  af_glyph_hints_dump_edges( AF_GlyphHints  hints,
                             FT_Bool        to_stdout )
  {
    FT_Int  dimension;


    for ( dimension = 1; dimension >= 0; dimension-- )
    {
      AF_AxisHints  axis  = &hints->axis[dimension];
      AF_Edge       edges = axis->edges;
      AF_Edge       limit = edges + axis->num_edges;
      AF_Edge       edge;


      /*
       *  note: AF_DIMENSION_HORZ corresponds to _vertical_ edges
       *        since they have a constant X coordinate.
       */
      AF_DUMP(( "Table of %s edges:\n",
                dimension == AF_DIMENSION_HORZ ? "vertical"
                                               : "horizontal" ));
      if ( axis->num_edges )
        AF_DUMP(( "  [ index |  pos  |  dir  | link"
                  " | serif | blue | opos  |  pos  |    flags    ]\n" ));
      else
        AF_DUMP(( "  (none)\n" ));

      for ( edge = edges; edge < limit; edge++ )
        AF_DUMP(( "  [ %5d | %5.2g | %5s | %4d"
                  " | %5d |   %c  | %5.2f | %5.2f | %11s ]\n",
                  edge - edges,
                  (int)edge->opos / 64.0,
                  af_dir_str( (AF_Direction)edge->dir ),
                  AF_INDEX_NUM( edge->link, edges ),
                  AF_INDEX_NUM( edge->serif, edges ),
                  edge->blue_edge ? 'y' : 'n',
                  edge->opos / 64.0,
                  edge->pos / 64.0,
                  af_edge_flags_to_string( (AF_Edge_Flags)edge->flags ) ));
      AF_DUMP(( "\n" ));
    }
  }
#ifdef __cplusplus
  }
#endif

#undef AF_DUMP

#endif /* !FT_DEBUG_AUTOFIT */


  /* Compute the direction value of a given vector. */

  FT_LOCAL_DEF( AF_Direction )
  af_direction_compute( FT_Pos  dx,
                        FT_Pos  dy )
  {
    FT_Pos        ll, ss;  /* long and short arm lengths */
    AF_Direction  dir;     /* candidate direction        */


    if ( dy >= dx )
    {
      if ( dy >= -dx )
      {
        dir = AF_DIR_UP;
        ll  = dy;
        ss  = dx;
      }
      else
      {
        dir = AF_DIR_LEFT;
        ll  = -dx;
        ss  = dy;
      }
    }
    else /* dy < dx */
    {
      if ( dy >= -dx )
      {
        dir = AF_DIR_RIGHT;
        ll  = dx;
        ss  = dy;
      }
      else
      {
        dir = AF_DIR_DOWN;
        ll  = dy;
        ss  = dx;
      }
    }

    /* return no direction if arm lengths differ too much            */
    /* (value 14 is heuristic, corresponding to approx. 4.1 degrees) */
    ss *= 14;
    if ( FT_ABS( ll ) <= FT_ABS( ss ) )
      dir = AF_DIR_NONE;

    return dir;
  }


  FT_LOCAL_DEF( void )
  af_glyph_hints_init( AF_GlyphHints  hints,
                       FT_Memory      memory )
  {
    FT_ZERO( hints );
    hints->memory = memory;
  }


  FT_LOCAL_DEF( void )
  af_glyph_hints_done( AF_GlyphHints  hints )
  {
    FT_Memory  memory = hints->memory;
    int        dim;


    if ( !( hints && hints->memory ) )
      return;

    /*
     *  note that we don't need to free the segment and edge
     *  buffers since they are really within the hints->points array
     */
    for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
    {
      AF_AxisHints  axis = &hints->axis[dim];


      axis->num_segments = 0;
      axis->max_segments = 0;
      FT_FREE( axis->segments );

      axis->num_edges = 0;
      axis->max_edges = 0;
      FT_FREE( axis->edges );
    }

    FT_FREE( hints->contours );
    hints->max_contours = 0;
    hints->num_contours = 0;

    FT_FREE( hints->points );
    hints->num_points = 0;
    hints->max_points = 0;

    hints->memory = NULL;
  }


  /* Reset metrics. */

  FT_LOCAL_DEF( void )
  af_glyph_hints_rescale( AF_GlyphHints    hints,
                          AF_StyleMetrics  metrics )
  {
    hints->metrics      = metrics;
    hints->scaler_flags = metrics->scaler.flags;
  }


  /* Recompute all AF_Point in AF_GlyphHints from the definitions */
  /* in a source outline.                                         */

  FT_LOCAL_DEF( FT_Error )
  af_glyph_hints_reload( AF_GlyphHints  hints,
                         FT_Outline*    outline )
  {
    FT_Error   error   = FT_Err_Ok;
    AF_Point   points;
    FT_UInt    old_max, new_max;
    FT_Fixed   x_scale = hints->x_scale;
    FT_Fixed   y_scale = hints->y_scale;
    FT_Pos     x_delta = hints->x_delta;
    FT_Pos     y_delta = hints->y_delta;
    FT_Memory  memory  = hints->memory;


    hints->num_points   = 0;
    hints->num_contours = 0;

    hints->axis[0].num_segments = 0;
    hints->axis[0].num_edges    = 0;
    hints->axis[1].num_segments = 0;
    hints->axis[1].num_edges    = 0;

    /* first of all, reallocate the contours array if necessary */
    new_max = (FT_UInt)outline->n_contours;
    old_max = hints->max_contours;
    if ( new_max > old_max )
    {
      new_max = ( new_max + 3 ) & ~3; /* round up to a multiple of 4 */

      if ( FT_RENEW_ARRAY( hints->contours, old_max, new_max ) )
        goto Exit;

      hints->max_contours = new_max;
    }

    /*
     *  then reallocate the points arrays if necessary --
     *  note that we reserve two additional point positions, used to
     *  hint metrics appropriately
     */
    new_max = (FT_UInt)( outline->n_points + 2 );
    old_max = hints->max_points;
    if ( new_max > old_max )
    {
      new_max = ( new_max + 2 + 7 ) & ~7; /* round up to a multiple of 8 */

      if ( FT_RENEW_ARRAY( hints->points, old_max, new_max ) )
        goto Exit;

      hints->max_points = new_max;
    }

    hints->num_points   = outline->n_points;
    hints->num_contours = outline->n_contours;

    /* We can't rely on the value of `FT_Outline.flags' to know the fill   */
    /* direction used for a glyph, given that some fonts are broken (e.g., */
    /* the Arphic ones).  We thus recompute it each time we need to.       */
    /*                                                                     */
    hints->axis[AF_DIMENSION_HORZ].major_dir = AF_DIR_UP;
    hints->axis[AF_DIMENSION_VERT].major_dir = AF_DIR_LEFT;

    if ( FT_Outline_Get_Orientation( outline ) == FT_ORIENTATION_POSTSCRIPT )
    {
      hints->axis[AF_DIMENSION_HORZ].major_dir = AF_DIR_DOWN;
      hints->axis[AF_DIMENSION_VERT].major_dir = AF_DIR_RIGHT;
    }

    hints->x_scale = x_scale;
    hints->y_scale = y_scale;
    hints->x_delta = x_delta;
    hints->y_delta = y_delta;

    hints->xmin_delta = 0;
    hints->xmax_delta = 0;

    points = hints->points;
    if ( hints->num_points == 0 )
      goto Exit;

    {
      AF_Point  point;
      AF_Point  point_limit = points + hints->num_points;


      /* compute coordinates & Bezier flags, next and prev */
      {
        FT_Vector*  vec           = outline->points;
        char*       tag           = outline->tags;
        AF_Point    end           = points + outline->contours[0];
        AF_Point    prev          = end;
        FT_Int      contour_index = 0;


        for ( point = points; point < point_limit; point++, vec++, tag++ )
        {
          point->fx = (FT_Short)vec->x;
          point->fy = (FT_Short)vec->y;
          point->ox = point->x = FT_MulFix( vec->x, x_scale ) + x_delta;
          point->oy = point->y = FT_MulFix( vec->y, y_scale ) + y_delta;

          switch ( FT_CURVE_TAG( *tag ) )
          {
          case FT_CURVE_TAG_CONIC:
            point->flags = AF_FLAG_CONIC;
            break;
          case FT_CURVE_TAG_CUBIC:
            point->flags = AF_FLAG_CUBIC;
            break;
          default:
            point->flags = AF_FLAG_NONE;
          }

          point->prev = prev;
          prev->next  = point;
          prev        = point;

          if ( point == end )
          {
            if ( ++contour_index < outline->n_contours )
            {
              end  = points + outline->contours[contour_index];
              prev = end;
            }
          }
        }
      }

      /* set up the contours array */
      {
        AF_Point*  contour       = hints->contours;
        AF_Point*  contour_limit = contour + hints->num_contours;
        short*     end           = outline->contours;
        short      idx           = 0;


        for ( ; contour < contour_limit; contour++, end++ )
        {
          contour[0] = points + idx;
          idx        = (short)( end[0] + 1 );
        }
      }

      /* compute directions of in & out vectors */
      {
        AF_Point      first  = points;
        AF_Point      prev   = NULL;
        FT_Pos        in_x   = 0;
        FT_Pos        in_y   = 0;
        AF_Direction  in_dir = AF_DIR_NONE;

        FT_Pos  last_good_in_x = 0;
        FT_Pos  last_good_in_y = 0;

        FT_UInt  units_per_em = hints->metrics->scaler.face->units_per_EM;
        FT_Int   near_limit   = 20 * units_per_em / 2048;


        for ( point = points; point < point_limit; point++ )
        {
          AF_Point  next;
          FT_Pos    out_x, out_y;


          if ( point == first )
          {
            prev = first->prev;

            in_x = first->fx - prev->fx;
            in_y = first->fy - prev->fy;

            last_good_in_x = in_x;
            last_good_in_y = in_y;

            if ( FT_ABS( in_x ) + FT_ABS( in_y ) < near_limit )
            {
              /* search first non-near point to get a good `in_dir' value */

              AF_Point  point_ = prev;


              while ( point_ != first )
              {
                AF_Point  prev_ = point_->prev;

                FT_Pos  in_x_ = point_->fx - prev_->fx;
                FT_Pos  in_y_ = point_->fy - prev_->fy;


                if ( FT_ABS( in_x_ ) + FT_ABS( in_y_ ) >= near_limit )
                {
                  last_good_in_x = in_x_;
                  last_good_in_y = in_y_;

                  break;
                }

                point_ = prev_;
              }
            }

            in_dir = af_direction_compute( in_x, in_y );
            first  = prev + 1;
          }

          point->in_dir = (FT_Char)in_dir;

          /* check whether the current point is near to the previous one */
          /* (value 20 in `near_limit' is heuristic; we use Taxicab      */
          /* metrics for the test)                                       */

          if ( FT_ABS( in_x ) + FT_ABS( in_y ) < near_limit )
            point->flags |= AF_FLAG_NEAR;
          else
          {
            last_good_in_x = in_x;
            last_good_in_y = in_y;
          }

          next  = point->next;
          out_x = next->fx - point->fx;
          out_y = next->fy - point->fy;

          in_dir         = af_direction_compute( out_x, out_y );
          point->out_dir = (FT_Char)in_dir;

          /* Check for weak points.  The remaining points not collected */
          /* in edges are then implicitly classified as strong points.  */

          if ( point->flags & AF_FLAG_CONTROL )
          {
            /* control points are always weak */
          Is_Weak_Point:
            point->flags |= AF_FLAG_WEAK_INTERPOLATION;
          }
          else if ( point->out_dir == point->in_dir )
          {
            if ( point->out_dir != AF_DIR_NONE )
            {
              /* current point lies on a horizontal or          */
              /* vertical segment (but doesn't start or end it) */
              goto Is_Weak_Point;
            }

            /* test whether `in' and `out' direction is approximately */
            /* the same (and use the last good `in' vector in case    */
            /* the current point is near to the previous one)         */
            if ( ft_corner_is_flat(
                   point->flags & AF_FLAG_NEAR ? last_good_in_x : in_x,
                   point->flags & AF_FLAG_NEAR ? last_good_in_y : in_y,
                   out_x,
                   out_y ) )
            {
              /* current point lies on a straight, diagonal line */
              /* (more or less)                                  */
              goto Is_Weak_Point;
            }
          }
          else if ( point->in_dir == -point->out_dir )
          {
            /* current point forms a spike */
            goto Is_Weak_Point;
          }

          in_x = out_x;
          in_y = out_y;
        }
      }
    }

  Exit:
    return error;
  }


  /* Store the hinted outline in an FT_Outline structure. */

  FT_LOCAL_DEF( void )
  af_glyph_hints_save( AF_GlyphHints  hints,
                       FT_Outline*    outline )
  {
    AF_Point    point = hints->points;
    AF_Point    limit = point + hints->num_points;
    FT_Vector*  vec   = outline->points;
    char*       tag   = outline->tags;


    for ( ; point < limit; point++, vec++, tag++ )
    {
      vec->x = point->x;
      vec->y = point->y;

      if ( point->flags & AF_FLAG_CONIC )
        tag[0] = FT_CURVE_TAG_CONIC;
      else if ( point->flags & AF_FLAG_CUBIC )
        tag[0] = FT_CURVE_TAG_CUBIC;
      else
        tag[0] = FT_CURVE_TAG_ON;
    }
  }


  /****************************************************************
   *
   *                     EDGE POINT GRID-FITTING
   *
   ****************************************************************/


  /* Align all points of an edge to the same coordinate value, */
  /* either horizontally or vertically.                        */

  FT_LOCAL_DEF( void )
  af_glyph_hints_align_edge_points( AF_GlyphHints  hints,
                                    AF_Dimension   dim )
  {
    AF_AxisHints  axis          = & hints->axis[dim];
    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    AF_Segment    seg;


    if ( dim == AF_DIMENSION_HORZ )
    {
      for ( seg = segments; seg < segment_limit; seg++ )
      {
        AF_Edge   edge = seg->edge;
        AF_Point  point, first, last;


        if ( edge == NULL )
          continue;

        first = seg->first;
        last  = seg->last;
        point = first;
        for (;;)
        {
          point->x      = edge->pos;
          point->flags |= AF_FLAG_TOUCH_X;

          if ( point == last )
            break;

          point = point->next;
        }
      }
    }
    else
    {
      for ( seg = segments; seg < segment_limit; seg++ )
      {
        AF_Edge   edge = seg->edge;
        AF_Point  point, first, last;


        if ( edge == NULL )
          continue;

        first = seg->first;
        last  = seg->last;
        point = first;
        for (;;)
        {
          point->y      = edge->pos;
          point->flags |= AF_FLAG_TOUCH_Y;

          if ( point == last )
            break;

          point = point->next;
        }
      }
    }
  }


  /****************************************************************
   *
   *                    STRONG POINT INTERPOLATION
   *
   ****************************************************************/


  /* Hint the strong points -- this is equivalent to the TrueType `IP' */
  /* hinting instruction.                                              */

  FT_LOCAL_DEF( void )
  af_glyph_hints_align_strong_points( AF_GlyphHints  hints,
                                      AF_Dimension   dim )
  {
    AF_Point      points      = hints->points;
    AF_Point      point_limit = points + hints->num_points;
    AF_AxisHints  axis        = &hints->axis[dim];
    AF_Edge       edges       = axis->edges;
    AF_Edge       edge_limit  = edges + axis->num_edges;
    AF_Flags      touch_flag;


    if ( dim == AF_DIMENSION_HORZ )
      touch_flag = AF_FLAG_TOUCH_X;
    else
      touch_flag  = AF_FLAG_TOUCH_Y;

    if ( edges < edge_limit )
    {
      AF_Point  point;
      AF_Edge   edge;


      for ( point = points; point < point_limit; point++ )
      {
        FT_Pos  u, ou, fu;  /* point position */
        FT_Pos  delta;


        if ( point->flags & touch_flag )
          continue;

        /* if this point is candidate to weak interpolation, we       */
        /* interpolate it after all strong points have been processed */

        if (  ( point->flags & AF_FLAG_WEAK_INTERPOLATION ) &&
             !( point->flags & AF_FLAG_INFLECTION )         )
          continue;

        if ( dim == AF_DIMENSION_VERT )
        {
          u  = point->fy;
          ou = point->oy;
        }
        else
        {
          u  = point->fx;
          ou = point->ox;
        }

        fu = u;

        /* is the point before the first edge? */
        edge  = edges;
        delta = edge->fpos - u;
        if ( delta >= 0 )
        {
          u = edge->pos - ( edge->opos - ou );
          goto Store_Point;
        }

        /* is the point after the last edge? */
        edge  = edge_limit - 1;
        delta = u - edge->fpos;
        if ( delta >= 0 )
        {
          u = edge->pos + ( ou - edge->opos );
          goto Store_Point;
        }

        {
          FT_PtrDist  min, max, mid;
          FT_Pos      fpos;


          /* find enclosing edges */
          min = 0;
          max = edge_limit - edges;

#if 1
          /* for a small number of edges, a linear search is better */
          if ( max <= 8 )
          {
            FT_PtrDist  nn;


            for ( nn = 0; nn < max; nn++ )
              if ( edges[nn].fpos >= u )
                break;

            if ( edges[nn].fpos == u )
            {
              u = edges[nn].pos;
              goto Store_Point;
            }
            min = nn;
          }
          else
#endif
          while ( min < max )
          {
            mid  = ( max + min ) >> 1;
            edge = edges + mid;
            fpos = edge->fpos;

            if ( u < fpos )
              max = mid;
            else if ( u > fpos )
              min = mid + 1;
            else
            {
              /* we are on the edge */
              u = edge->pos;
              goto Store_Point;
            }
          }

          /* point is not on an edge */
          {
            AF_Edge  before = edges + min - 1;
            AF_Edge  after  = edges + min + 0;


            /* assert( before && after && before != after ) */
            if ( before->scale == 0 )
              before->scale = FT_DivFix( after->pos - before->pos,
                                         after->fpos - before->fpos );

            u = before->pos + FT_MulFix( fu - before->fpos,
                                         before->scale );
          }
        }

      Store_Point:
        /* save the point position */
        if ( dim == AF_DIMENSION_HORZ )
          point->x = u;
        else
          point->y = u;

        point->flags |= touch_flag;
      }
    }
  }


  /****************************************************************
   *
   *                    WEAK POINT INTERPOLATION
   *
   ****************************************************************/


  /* Shift the original coordinates of all points between `p1' and */
  /* `p2' to get hinted coordinates, using the same difference as  */
  /* given by `ref'.                                               */

  static void
  af_iup_shift( AF_Point  p1,
                AF_Point  p2,
                AF_Point  ref )
  {
    AF_Point  p;
    FT_Pos    delta = ref->u - ref->v;


    if ( delta == 0 )
      return;

    for ( p = p1; p < ref; p++ )
      p->u = p->v + delta;

    for ( p = ref + 1; p <= p2; p++ )
      p->u = p->v + delta;
  }


  /* Interpolate the original coordinates of all points between `p1' and  */
  /* `p2' to get hinted coordinates, using `ref1' and `ref2' as the       */
  /* reference points.  The `u' and `v' members are the current and       */
  /* original coordinate values, respectively.                            */
  /*                                                                      */
  /* Details can be found in the TrueType bytecode specification.         */

  static void
  af_iup_interp( AF_Point  p1,
                 AF_Point  p2,
                 AF_Point  ref1,
                 AF_Point  ref2 )
  {
    AF_Point  p;
    FT_Pos    u;
    FT_Pos    v1 = ref1->v;
    FT_Pos    v2 = ref2->v;
    FT_Pos    d1 = ref1->u - v1;
    FT_Pos    d2 = ref2->u - v2;


    if ( p1 > p2 )
      return;

    if ( v1 == v2 )
    {
      for ( p = p1; p <= p2; p++ )
      {
        u = p->v;

        if ( u <= v1 )
          u += d1;
        else
          u += d2;

        p->u = u;
      }
      return;
    }

    if ( v1 < v2 )
    {
      for ( p = p1; p <= p2; p++ )
      {
        u = p->v;

        if ( u <= v1 )
          u += d1;
        else if ( u >= v2 )
          u += d2;
        else
          u = ref1->u + FT_MulDiv( u - v1, ref2->u - ref1->u, v2 - v1 );

        p->u = u;
      }
    }
    else
    {
      for ( p = p1; p <= p2; p++ )
      {
        u = p->v;

        if ( u <= v2 )
          u += d2;
        else if ( u >= v1 )
          u += d1;
        else
          u = ref1->u + FT_MulDiv( u - v1, ref2->u - ref1->u, v2 - v1 );

        p->u = u;
      }
    }
  }


  /* Hint the weak points -- this is equivalent to the TrueType `IUP' */
  /* hinting instruction.                                             */

  FT_LOCAL_DEF( void )
  af_glyph_hints_align_weak_points( AF_GlyphHints  hints,
                                    AF_Dimension   dim )
  {
    AF_Point   points        = hints->points;
    AF_Point   point_limit   = points + hints->num_points;
    AF_Point*  contour       = hints->contours;
    AF_Point*  contour_limit = contour + hints->num_contours;
    AF_Flags   touch_flag;
    AF_Point   point;
    AF_Point   end_point;
    AF_Point   first_point;


    /* PASS 1: Move segment points to edge positions */

    if ( dim == AF_DIMENSION_HORZ )
    {
      touch_flag = AF_FLAG_TOUCH_X;

      for ( point = points; point < point_limit; point++ )
      {
        point->u = point->x;
        point->v = point->ox;
      }
    }
    else
    {
      touch_flag = AF_FLAG_TOUCH_Y;

      for ( point = points; point < point_limit; point++ )
      {
        point->u = point->y;
        point->v = point->oy;
      }
    }

    for ( ; contour < contour_limit; contour++ )
    {
      AF_Point  first_touched, last_touched;


      point       = *contour;
      end_point   = point->prev;
      first_point = point;

      /* find first touched point */
      for (;;)
      {
        if ( point > end_point )  /* no touched point in contour */
          goto NextContour;

        if ( point->flags & touch_flag )
          break;

        point++;
      }

      first_touched = point;

      for (;;)
      {
        FT_ASSERT( point <= end_point                 &&
                   ( point->flags & touch_flag ) != 0 );

        /* skip any touched neighbours */
        while ( point < end_point                    &&
                ( point[1].flags & touch_flag ) != 0 )
          point++;

        last_touched = point;

        /* find the next touched point, if any */
        point++;
        for (;;)
        {
          if ( point > end_point )
            goto EndContour;

          if ( ( point->flags & touch_flag ) != 0 )
            break;

          point++;
        }

        /* interpolate between last_touched and point */
        af_iup_interp( last_touched + 1, point - 1,
                       last_touched, point );
      }

    EndContour:
      /* special case: only one point was touched */
      if ( last_touched == first_touched )
        af_iup_shift( first_point, end_point, first_touched );

      else /* interpolate the last part */
      {
        if ( last_touched < end_point )
          af_iup_interp( last_touched + 1, end_point,
                         last_touched, first_touched );

        if ( first_touched > points )
          af_iup_interp( first_point, first_touched - 1,
                         last_touched, first_touched );
      }

    NextContour:
      ;
    }

    /* now save the interpolated values back to x/y */
    if ( dim == AF_DIMENSION_HORZ )
    {
      for ( point = points; point < point_limit; point++ )
        point->x = point->u;
    }
    else
    {
      for ( point = points; point < point_limit; point++ )
        point->y = point->u;
    }
  }


#ifdef AF_CONFIG_OPTION_USE_WARPER

  /* Apply (small) warp scale and warp delta for given dimension. */

  FT_LOCAL_DEF( void )
  af_glyph_hints_scale_dim( AF_GlyphHints  hints,
                            AF_Dimension   dim,
                            FT_Fixed       scale,
                            FT_Pos         delta )
  {
    AF_Point  points       = hints->points;
    AF_Point  points_limit = points + hints->num_points;
    AF_Point  point;


    if ( dim == AF_DIMENSION_HORZ )
    {
      for ( point = points; point < points_limit; point++ )
        point->x = FT_MulFix( point->fx, scale ) + delta;
    }
    else
    {
      for ( point = points; point < points_limit; point++ )
        point->y = FT_MulFix( point->fy, scale ) + delta;
    }
  }

#endif /* AF_CONFIG_OPTION_USE_WARPER */

/* END */

/***************************************************************************/
/*                                                                         */
/*  afranges.c                                                             */
/*                                                                         */
/*    Auto-fitter Unicode script ranges (body).                            */
/*                                                                         */
/*  Copyright 2013, 2014 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "afranges.h"


  const AF_Script_UniRangeRec  af_cyrl_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0400UL,  0x04FFUL ),  /* Cyrillic */
    AF_UNIRANGE_REC(  0x0500UL,  0x052FUL ),  /* Cyrillic Supplement */
    AF_UNIRANGE_REC(  0x2DE0UL,  0x2DFFUL ),  /* Cyrillic Extended-A */
    AF_UNIRANGE_REC(  0xA640UL,  0xA69FUL ),  /* Cyrillic Extended-B */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_grek_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0370UL,  0x03FFUL ),  /* Greek and Coptic */
    AF_UNIRANGE_REC(  0x1F00UL,  0x1FFFUL ),  /* Greek Extended */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_hebr_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0590UL,  0x05FFUL ),  /* Hebrew */
    AF_UNIRANGE_REC(  0xFB1DUL,  0xFB4FUL ),  /* Alphab. Present. Forms (Hebrew) */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_latn_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0020UL,  0x007FUL ),  /* Basic Latin (no control chars) */
    AF_UNIRANGE_REC(  0x00A0UL,  0x00FFUL ),  /* Latin-1 Supplement (no control chars) */
    AF_UNIRANGE_REC(  0x0100UL,  0x017FUL ),  /* Latin Extended-A */
    AF_UNIRANGE_REC(  0x0180UL,  0x024FUL ),  /* Latin Extended-B */
    AF_UNIRANGE_REC(  0x0250UL,  0x02AFUL ),  /* IPA Extensions */
    AF_UNIRANGE_REC(  0x02B0UL,  0x02FFUL ),  /* Spacing Modifier Letters */
    AF_UNIRANGE_REC(  0x0300UL,  0x036FUL ),  /* Combining Diacritical Marks */
    AF_UNIRANGE_REC(  0x1D00UL,  0x1D7FUL ),  /* Phonetic Extensions */
    AF_UNIRANGE_REC(  0x1D80UL,  0x1DBFUL ),  /* Phonetic Extensions Supplement */
    AF_UNIRANGE_REC(  0x1DC0UL,  0x1DFFUL ),  /* Combining Diacritical Marks Supplement */
    AF_UNIRANGE_REC(  0x1E00UL,  0x1EFFUL ),  /* Latin Extended Additional */
    AF_UNIRANGE_REC(  0x2000UL,  0x206FUL ),  /* General Punctuation */
    AF_UNIRANGE_REC(  0x2070UL,  0x209FUL ),  /* Superscripts and Subscripts */
    AF_UNIRANGE_REC(  0x20A0UL,  0x20CFUL ),  /* Currency Symbols */
    AF_UNIRANGE_REC(  0x2150UL,  0x218FUL ),  /* Number Forms */
    AF_UNIRANGE_REC(  0x2460UL,  0x24FFUL ),  /* Enclosed Alphanumerics */
    AF_UNIRANGE_REC(  0x2C60UL,  0x2C7FUL ),  /* Latin Extended-C */
    AF_UNIRANGE_REC(  0x2E00UL,  0x2E7FUL ),  /* Supplemental Punctuation */
    AF_UNIRANGE_REC(  0xA720UL,  0xA7FFUL ),  /* Latin Extended-D */
    AF_UNIRANGE_REC(  0xFB00UL,  0xFB06UL ),  /* Alphab. Present. Forms (Latin Ligs) */
    AF_UNIRANGE_REC( 0x1D400UL, 0x1D7FFUL ),  /* Mathematical Alphanumeric Symbols */
    AF_UNIRANGE_REC( 0x1F100UL, 0x1F1FFUL ),  /* Enclosed Alphanumeric Supplement */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_none_uniranges[] =
  {
    AF_UNIRANGE_REC( 0UL, 0UL )
  };

#ifdef AF_CONFIG_OPTION_INDIC

  const AF_Script_UniRangeRec  af_beng_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0980UL,  0x09FFUL ),  /* Bengali */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_deva_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0900UL,  0x097FUL ),  /* Devanagari */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_gujr_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0A80UL,  0x0AFFUL ),  /* Gujarati */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_guru_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0A00UL,  0x0A7FUL ),  /* Gurmukhi */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_knda_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0C80UL,  0x0CFFUL ),  /* Kannada */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_limb_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x1900UL,  0x194FUL ),  /* Limbu */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_mlym_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0D00UL,  0x0D7FUL ),  /* Malayalam */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_orya_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0B00UL,  0x0B7FUL ),  /* Oriya */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_sinh_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0D80UL,  0x0DFFUL ),  /* Sinhala */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_sund_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x1B80UL,  0x1BBFUL ),  /* Sundanese */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_sylo_uniranges[] =
  {
    AF_UNIRANGE_REC(  0xA800UL,  0xA82FUL ),  /* Syloti Nagri */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_taml_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0B80UL,  0x0BFFUL ),  /* Tamil */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_telu_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0C00UL,  0x0C7FUL ),  /* Telugu */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

  const AF_Script_UniRangeRec  af_tibt_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x0F00UL,  0x0FFFUL ),  /* Tibetan */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

#endif /* !AF_CONFIG_OPTION_INDIC */

#ifdef AF_CONFIG_OPTION_CJK

  /* this corresponds to Unicode 6.0 */

  const AF_Script_UniRangeRec  af_hani_uniranges[] =
  {
    AF_UNIRANGE_REC(  0x1100UL,  0x11FFUL ),  /* Hangul Jamo                             */
    AF_UNIRANGE_REC(  0x2E80UL,  0x2EFFUL ),  /* CJK Radicals Supplement                 */
    AF_UNIRANGE_REC(  0x2F00UL,  0x2FDFUL ),  /* Kangxi Radicals                         */
    AF_UNIRANGE_REC(  0x2FF0UL,  0x2FFFUL ),  /* Ideographic Description Characters      */
    AF_UNIRANGE_REC(  0x3000UL,  0x303FUL ),  /* CJK Symbols and Punctuation             */
    AF_UNIRANGE_REC(  0x3040UL,  0x309FUL ),  /* Hiragana                                */
    AF_UNIRANGE_REC(  0x30A0UL,  0x30FFUL ),  /* Katakana                                */
    AF_UNIRANGE_REC(  0x3100UL,  0x312FUL ),  /* Bopomofo                                */
    AF_UNIRANGE_REC(  0x3130UL,  0x318FUL ),  /* Hangul Compatibility Jamo               */
    AF_UNIRANGE_REC(  0x3190UL,  0x319FUL ),  /* Kanbun                                  */
    AF_UNIRANGE_REC(  0x31A0UL,  0x31BFUL ),  /* Bopomofo Extended                       */
    AF_UNIRANGE_REC(  0x31C0UL,  0x31EFUL ),  /* CJK Strokes                             */
    AF_UNIRANGE_REC(  0x31F0UL,  0x31FFUL ),  /* Katakana Phonetic Extensions            */
    AF_UNIRANGE_REC(  0x3200UL,  0x32FFUL ),  /* Enclosed CJK Letters and Months         */
    AF_UNIRANGE_REC(  0x3300UL,  0x33FFUL ),  /* CJK Compatibility                       */
    AF_UNIRANGE_REC(  0x3400UL,  0x4DBFUL ),  /* CJK Unified Ideographs Extension A      */
    AF_UNIRANGE_REC(  0x4DC0UL,  0x4DFFUL ),  /* Yijing Hexagram Symbols                 */
    AF_UNIRANGE_REC(  0x4E00UL,  0x9FFFUL ),  /* CJK Unified Ideographs                  */
    AF_UNIRANGE_REC(  0xA960UL,  0xA97FUL ),  /* Hangul Jamo Extended-A                  */
    AF_UNIRANGE_REC(  0xAC00UL,  0xD7AFUL ),  /* Hangul Syllables                        */
    AF_UNIRANGE_REC(  0xD7B0UL,  0xD7FFUL ),  /* Hangul Jamo Extended-B                  */
    AF_UNIRANGE_REC(  0xF900UL,  0xFAFFUL ),  /* CJK Compatibility Ideographs            */
    AF_UNIRANGE_REC(  0xFE10UL,  0xFE1FUL ),  /* Vertical forms                          */
    AF_UNIRANGE_REC(  0xFE30UL,  0xFE4FUL ),  /* CJK Compatibility Forms                 */
    AF_UNIRANGE_REC(  0xFF00UL,  0xFFEFUL ),  /* Halfwidth and Fullwidth Forms           */
    AF_UNIRANGE_REC( 0x1B000UL, 0x1B0FFUL ),  /* Kana Supplement                         */
    AF_UNIRANGE_REC( 0x1D300UL, 0x1D35FUL ),  /* Tai Xuan Hing Symbols                   */
    AF_UNIRANGE_REC( 0x1F200UL, 0x1F2FFUL ),  /* Enclosed Ideographic Supplement         */
    AF_UNIRANGE_REC( 0x20000UL, 0x2A6DFUL ),  /* CJK Unified Ideographs Extension B      */
    AF_UNIRANGE_REC( 0x2A700UL, 0x2B73FUL ),  /* CJK Unified Ideographs Extension C      */
    AF_UNIRANGE_REC( 0x2B740UL, 0x2B81FUL ),  /* CJK Unified Ideographs Extension D      */
    AF_UNIRANGE_REC( 0x2F800UL, 0x2FA1FUL ),  /* CJK Compatibility Ideographs Supplement */
    AF_UNIRANGE_REC(       0UL,       0UL )
  };

#endif /* !AF_CONFIG_OPTION_CJK */

/* END */

/***************************************************************************/
/*                                                                         */
/*  afdummy.c                                                              */
/*                                                                         */
/*    Auto-fitter dummy routines to be used if no hinting should be        */
/*    performed (body).                                                    */
/*                                                                         */
/*  Copyright 2003-2005, 2011, 2013 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "afdummy.h"
#include "afhints.h"
#include "aferrors.h"


  static FT_Error
  af_dummy_hints_init( AF_GlyphHints    hints,
                       AF_StyleMetrics  metrics )
  {
    af_glyph_hints_rescale( hints, metrics );

    hints->x_scale = metrics->scaler.x_scale;
    hints->y_scale = metrics->scaler.y_scale;
    hints->x_delta = metrics->scaler.x_delta;
    hints->y_delta = metrics->scaler.y_delta;

    return FT_Err_Ok;
  }


  static FT_Error
  af_dummy_hints_apply( AF_GlyphHints  hints,
                        FT_Outline*    outline )
  {
    FT_Error  error;


    error = af_glyph_hints_reload( hints, outline );
    if ( !error )
      af_glyph_hints_save( hints, outline );

    return error;
  }


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_dummy_writing_system_class,

    AF_WRITING_SYSTEM_DUMMY,

    sizeof ( AF_StyleMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) NULL,
    (AF_WritingSystem_ScaleMetricsFunc)NULL,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   af_dummy_hints_init,
    (AF_WritingSystem_ApplyHintsFunc)  af_dummy_hints_apply
  )


/* END */
/***************************************************************************/
/*                                                                         */
/*  aflatin.c                                                              */
/*                                                                         */
/*    Auto-fitter hinting routines for latin writing system (body).        */
/*                                                                         */
/*  Copyright 2003-2014 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "ft2build.h"
#include FT_ADVANCES_H
#include FT_INTERNAL_DEBUG_H

#include "afglobal.h"
#include "afpic.h"
#include "aflatin.h"
#include "aferrors.h"


#ifdef AF_CONFIG_OPTION_USE_WARPER
#include "afwarp.h"
#endif


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_aflatin


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****            L A T I N   G L O B A L   M E T R I C S            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /* Find segments and links, compute all stem widths, and initialize */
  /* standard width and height for the glyph with given charcode.     */

  FT_LOCAL_DEF( void )
  af_latin_metrics_init_widths( AF_LatinMetrics  metrics,
                                FT_Face          face )
  {
    /* scan the array of segments in each direction */
    AF_GlyphHintsRec  hints[1];


    FT_TRACE5(( "\n"
                "latin standard widths computation (style `%s')\n"
                "=====================================================\n"
                "\n",
                af_style_names[metrics->root.style_class->style] ));

    af_glyph_hints_init( hints, face->memory );

    metrics->axis[AF_DIMENSION_HORZ].width_count = 0;
    metrics->axis[AF_DIMENSION_VERT].width_count = 0;

    {
      FT_Error            error;
      FT_ULong            glyph_index;
      FT_Long             y_offset;
      int                 dim;
      AF_LatinMetricsRec  dummy[1];
      AF_Scaler           scaler = &dummy->root.scaler;

#ifdef FT_CONFIG_OPTION_PIC
      AF_FaceGlobals  globals = metrics->root.globals;
#endif

      AF_StyleClass   style_class  = metrics->root.style_class;
      AF_ScriptClass  script_class = AF_SCRIPT_CLASSES_GET
                                       [style_class->script];

      FT_UInt32  standard_char;


      /*
       * We check more than a single standard character to catch features
       * like `c2sc' (small caps from caps) that don't contain lowercase
       * letters by definition, or other features that mainly operate on
       * numerals.
       */

      standard_char = script_class->standard_char1;
      af_get_char_index( &metrics->root,
                         standard_char,
                         &glyph_index,
                         &y_offset );
      if ( !glyph_index )
      {
        if ( script_class->standard_char2 )
        {
          standard_char = script_class->standard_char2;
          af_get_char_index( &metrics->root,
                             standard_char,
                             &glyph_index,
                             &y_offset );
          if ( !glyph_index )
          {
            if ( script_class->standard_char3 )
            {
              standard_char = script_class->standard_char3;
              af_get_char_index( &metrics->root,
                                 standard_char,
                                 &glyph_index,
                                 &y_offset );
              if ( !glyph_index )
                goto Exit;
            }
            else
              goto Exit;
          }
        }
        else
          goto Exit;
      }

      FT_TRACE5(( "standard character: U+%04lX (glyph index %d)\n",
                  standard_char, glyph_index ));

      error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
      if ( error || face->glyph->outline.n_points <= 0 )
        goto Exit;

      FT_ZERO( dummy );

      dummy->units_per_em = metrics->units_per_em;

      scaler->x_scale = 0x10000L;
      scaler->y_scale = 0x10000L;
      scaler->x_delta = 0;
      scaler->y_delta = 0;

      scaler->face        = face;
      scaler->render_mode = FT_RENDER_MODE_NORMAL;
      scaler->flags       = 0;

      af_glyph_hints_rescale( hints, (AF_StyleMetrics)dummy );

      error = af_glyph_hints_reload( hints, &face->glyph->outline );
      if ( error )
        goto Exit;

      for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
      {
        AF_LatinAxis  axis    = &metrics->axis[dim];
        AF_AxisHints  axhints = &hints->axis[dim];
        AF_Segment    seg, limit, link;
        FT_UInt       num_widths = 0;


        error = af_latin_hints_compute_segments( hints,
                                                 (AF_Dimension)dim );
        if ( error )
          goto Exit;

        af_latin_hints_link_segments( hints,
                                      (AF_Dimension)dim );

        seg   = axhints->segments;
        limit = seg + axhints->num_segments;

        for ( ; seg < limit; seg++ )
        {
          link = seg->link;

          /* we only consider stem segments there! */
          if ( link && link->link == seg && link > seg )
          {
            FT_Pos  dist;


            dist = seg->pos - link->pos;
            if ( dist < 0 )
              dist = -dist;

            if ( num_widths < AF_LATIN_MAX_WIDTHS )
              axis->widths[num_widths++].org = dist;
          }
        }

        /* this also replaces multiple almost identical stem widths */
        /* with a single one (the value 100 is heuristic)           */
        af_sort_and_quantize_widths( &num_widths, axis->widths,
                                     dummy->units_per_em / 100 );
        axis->width_count = num_widths;
      }

    Exit:
      for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
      {
        AF_LatinAxis  axis = &metrics->axis[dim];
        FT_Pos        stdw;


        stdw = ( axis->width_count > 0 ) ? axis->widths[0].org
                                         : AF_LATIN_CONSTANT( metrics, 50 );

        /* let's try 20% of the smallest width */
        axis->edge_distance_threshold = stdw / 5;
        axis->standard_width          = stdw;
        axis->extra_light             = 0;

#ifdef FT_DEBUG_LEVEL_TRACE
        {
          FT_UInt  i;


          FT_TRACE5(( "%s widths:\n",
                      dim == AF_DIMENSION_VERT ? "horizontal"
                                               : "vertical" ));

          FT_TRACE5(( "  %d (standard)", axis->standard_width ));
          for ( i = 1; i < axis->width_count; i++ )
            FT_TRACE5(( " %d", axis->widths[i].org ));

          FT_TRACE5(( "\n" ));
        }
#endif
      }
    }

    FT_TRACE5(( "\n" ));

    af_glyph_hints_done( hints );
  }


  /* Find all blue zones.  Flat segments give the reference points, */
  /* round segments the overshoot positions.                        */

  static void
  af_latin_metrics_init_blues( AF_LatinMetrics  metrics,
                               FT_Face          face )
  {
    FT_Pos        flats [AF_BLUE_STRING_MAX_LEN];
    FT_Pos        rounds[AF_BLUE_STRING_MAX_LEN];

    FT_Int        num_flats;
    FT_Int        num_rounds;

    AF_LatinBlue  blue;
    FT_Error      error;
    AF_LatinAxis  axis = &metrics->axis[AF_DIMENSION_VERT];
    FT_Outline    outline;

    AF_StyleClass  sc = metrics->root.style_class;

    AF_Blue_Stringset         bss = sc->blue_stringset;
    const AF_Blue_StringRec*  bs  = &af_blue_stringsets[bss];


    /* we walk over the blue character strings as specified in the */
    /* style's entry in the `af_blue_stringset' array              */

    FT_TRACE5(( "latin blue zones computation\n"
                "============================\n"
                "\n" ));

    for ( ; bs->string != AF_BLUE_STRING_MAX; bs++ )
    {
      const char*  p = &af_blue_strings[bs->string];
      FT_Pos*      blue_ref;
      FT_Pos*      blue_shoot;


#ifdef FT_DEBUG_LEVEL_TRACE
      {
        FT_Bool  have_flag = 0;


        FT_TRACE5(( "blue zone %d", axis->blue_count ));

        if ( bs->properties )
        {
          FT_TRACE5(( " (" ));

          if ( AF_LATIN_IS_TOP_BLUE( bs ) )
          {
            FT_TRACE5(( "top" ));
            have_flag = 1;
          }

          if ( AF_LATIN_IS_X_HEIGHT_BLUE( bs ) )
          {
            if ( have_flag )
              FT_TRACE5(( ", " ));
            FT_TRACE5(( "small top" ));
            have_flag = 1;
          }

          if ( AF_LATIN_IS_LONG_BLUE( bs ) )
          {
            if ( have_flag )
              FT_TRACE5(( ", " ));
            FT_TRACE5(( "long" ));
          }

          FT_TRACE5(( ")" ));
        }

        FT_TRACE5(( ":\n" ));
      }
#endif /* FT_DEBUG_LEVEL_TRACE */

      num_flats  = 0;
      num_rounds = 0;

      while ( *p )
      {
        FT_ULong    ch;
        FT_ULong    glyph_index;
        FT_Long     y_offset;
        FT_Pos      best_y;                            /* same as points.y */
        FT_Int      best_point, best_contour_first, best_contour_last;
        FT_Vector*  points;
        FT_Bool     round = 0;


        GET_UTF8_CHAR( ch, p );

        /* load the character in the face -- skip unknown or empty ones */
        af_get_char_index( &metrics->root, ch, &glyph_index, &y_offset );
        if ( glyph_index == 0 )
        {
          FT_TRACE5(( "  U+%04lX unavailable\n", ch ));
          continue;
        }

        error   = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
        outline = face->glyph->outline;
        if ( error || outline.n_points <= 0 )
        {
          FT_TRACE5(( "  U+%04lX contains no outlines\n", ch ));
          continue;
        }

        /* now compute min or max point indices and coordinates */
        points             = outline.points;
        best_point         = -1;
        best_y             = 0;  /* make compiler happy */
        best_contour_first = 0;  /* ditto */
        best_contour_last  = 0;  /* ditto */

        {
          FT_Int  nn;
          FT_Int  first = 0;
          FT_Int  last  = -1;


          for ( nn = 0; nn < outline.n_contours; first = last + 1, nn++ )
          {
            FT_Int  old_best_point = best_point;
            FT_Int  pp;


            last = outline.contours[nn];

            /* Avoid single-point contours since they are never rasterized. */
            /* In some fonts, they correspond to mark attachment points     */
            /* that are way outside of the glyph's real outline.            */
            if ( last <= first )
              continue;

            if ( AF_LATIN_IS_TOP_BLUE( bs ) )
            {
              for ( pp = first; pp <= last; pp++ )
                if ( best_point < 0 || points[pp].y > best_y )
                {
                  best_point = pp;
                  best_y     = points[pp].y;
                }
            }
            else
            {
              for ( pp = first; pp <= last; pp++ )
                if ( best_point < 0 || points[pp].y < best_y )
                {
                  best_point = pp;
                  best_y     = points[pp].y;
                }
            }

            if ( best_point != old_best_point )
            {
              best_contour_first = first;
              best_contour_last  = last;
            }
          }
        }

        /* now check whether the point belongs to a straight or round   */
        /* segment; we first need to find in which contour the extremum */
        /* lies, then inspect its previous and next points              */
        if ( best_point >= 0 )
        {
          FT_Pos  best_x = points[best_point].x;
          FT_Int  prev, next;
          FT_Int  best_segment_first, best_segment_last;
          FT_Int  best_on_point_first, best_on_point_last;
          FT_Pos  dist;


          best_segment_first = best_point;
          best_segment_last  = best_point;

          if ( FT_CURVE_TAG( outline.tags[best_point] ) == FT_CURVE_TAG_ON )
          {
            best_on_point_first = best_point;
            best_on_point_last  = best_point;
          }
          else
          {
            best_on_point_first = -1;
            best_on_point_last  = -1;
          }

          /* look for the previous and next points on the contour  */
          /* that are not on the same Y coordinate, then threshold */
          /* the `closeness'...                                    */
          prev = best_point;
          next = prev;

          do
          {
            if ( prev > best_contour_first )
              prev--;
            else
              prev = best_contour_last;

            dist = FT_ABS( points[prev].y - best_y );
            /* accept a small distance or a small angle (both values are */
            /* heuristic; value 20 corresponds to approx. 2.9 degrees)   */
            if ( dist > 5 )
              if ( FT_ABS( points[prev].x - best_x ) <= 20 * dist )
                break;

            best_segment_first = prev;

            if ( FT_CURVE_TAG( outline.tags[prev] ) == FT_CURVE_TAG_ON )
            {
              best_on_point_first = prev;
              if ( best_on_point_last < 0 )
                best_on_point_last = prev;
            }

          } while ( prev != best_point );

          do
          {
            if ( next < best_contour_last )
              next++;
            else
              next = best_contour_first;

            dist = FT_ABS( points[next].y - best_y );
            if ( dist > 5 )
              if ( FT_ABS( points[next].x - best_x ) <= 20 * dist )
                break;

            best_segment_last = next;

            if ( FT_CURVE_TAG( outline.tags[next] ) == FT_CURVE_TAG_ON )
            {
              best_on_point_last = next;
              if ( best_on_point_first < 0 )
                best_on_point_first = next;
            }

          } while ( next != best_point );

          if ( AF_LATIN_IS_LONG_BLUE( bs ) )
          {
            /* If this flag is set, we have an additional constraint to  */
            /* get the blue zone distance: Find a segment of the topmost */
            /* (or bottommost) contour that is longer than a heuristic   */
            /* threshold.  This ensures that small bumps in the outline  */
            /* are ignored (for example, the `vertical serifs' found in  */
            /* many Hebrew glyph designs).                               */

            /* If this segment is long enough, we are done.  Otherwise,  */
            /* search the segment next to the extremum that is long      */
            /* enough, has the same direction, and a not too large       */
            /* vertical distance from the extremum.  Note that the       */
            /* algorithm doesn't check whether the found segment is      */
            /* actually the one (vertically) nearest to the extremum.    */

            /* heuristic threshold value */
            FT_Pos  length_threshold = metrics->units_per_em / 25;


            dist = FT_ABS( points[best_segment_last].x -
                             points[best_segment_first].x );

            if ( dist < length_threshold                       &&
                 best_segment_last - best_segment_first + 2 <=
                   best_contour_last - best_contour_first      )
            {
              /* heuristic threshold value */
              FT_Pos  height_threshold = metrics->units_per_em / 4;

              FT_Int   first;
              FT_Int   last;
              FT_Bool  hit;

              FT_Bool  left2right;


              /* compute direction */
              prev = best_point;

              do
              {
                if ( prev > best_contour_first )
                  prev--;
                else
                  prev = best_contour_last;

                if ( points[prev].x != best_x )
                  break;

              } while ( prev != best_point );

              /* skip glyph for the degenerate case */
              if ( prev == best_point )
                continue;

              left2right = FT_BOOL( points[prev].x < points[best_point].x );

              first = best_segment_last;
              last  = first;
              hit   = 0;

              do
              {
                FT_Bool  l2r;
                FT_Pos   d;
                FT_Int   p_first, p_last;


                if ( !hit )
                {
                  /* no hit; adjust first point */
                  first = last;

                  /* also adjust first and last on point */
                  if ( FT_CURVE_TAG( outline.tags[first] ) ==
                         FT_CURVE_TAG_ON )
                  {
                    p_first = first;
                    p_last  = first;
                  }
                  else
                  {
                    p_first = -1;
                    p_last  = -1;
                  }

                  hit = 1;
                }

                if ( last < best_contour_last )
                  last++;
                else
                  last = best_contour_first;

                if ( FT_ABS( best_y - points[first].y ) > height_threshold )
                {
                  /* vertical distance too large */
                  hit = 0;
                  continue;
                }

                /* same test as above */
                dist = FT_ABS( points[last].y - points[first].y );
                if ( dist > 5 )
                  if ( FT_ABS( points[last].x - points[first].x ) <=
                         20 * dist )
                  {
                    hit = 0;
                    continue;
                  }

                if ( FT_CURVE_TAG( outline.tags[last] ) == FT_CURVE_TAG_ON )
                {
                  p_last = last;
                  if ( p_first < 0 )
                    p_first = last;
                }

                l2r = FT_BOOL( points[first].x < points[last].x );
                d   = FT_ABS( points[last].x - points[first].x );

                if ( l2r == left2right     &&
                     d >= length_threshold )
                {
                  /* all constraints are met; update segment after finding */
                  /* its end                                               */
                  do
                  {
                    if ( last < best_contour_last )
                      last++;
                    else
                      last = best_contour_first;

                    d = FT_ABS( points[last].y - points[first].y );
                    if ( d > 5 )
                      if ( FT_ABS( points[next].x - points[first].x ) <=
                             20 * dist )
                      {
                        if ( last > best_contour_first )
                          last--;
                        else
                          last = best_contour_last;
                        break;
                      }

                    p_last = last;

                    if ( FT_CURVE_TAG( outline.tags[last] ) ==
                           FT_CURVE_TAG_ON )
                    {
                      p_last = last;
                      if ( p_first < 0 )
                        p_first = last;
                    }

                  } while ( last != best_segment_first );

                  best_y = points[first].y;

                  best_segment_first = first;
                  best_segment_last  = last;

                  best_on_point_first = p_first;
                  best_on_point_last  = p_last;

                  break;
                }

              } while ( last != best_segment_first );
            }
          }

          /* for computing blue zones, we add the y offset as returned */
          /* by the currently used OpenType feature -- for example,    */
          /* superscript glyphs might be identical to subscript glyphs */
          /* with a vertical shift                                     */
          best_y += y_offset;

          FT_TRACE5(( "  U+%04lX: best_y = %5ld", ch, best_y ));

          /* now set the `round' flag depending on the segment's kind: */
          /*                                                           */
          /* - if the horizontal distance between the first and last   */
          /*   `on' point is larger than upem/8 (value 8 is heuristic) */
          /*   we have a flat segment                                  */
          /* - if either the first or the last point of the segment is */
          /*   an `off' point, the segment is round, otherwise it is   */
          /*   flat                                                    */
          if ( best_on_point_first >= 0                               &&
               best_on_point_last >= 0                                &&
               (FT_UInt)( FT_ABS( points[best_on_point_last].x -
                                  points[best_on_point_first].x ) ) >
                 metrics->units_per_em / 8                            )
            round = 0;
          else
            round = FT_BOOL(
                      FT_CURVE_TAG( outline.tags[best_segment_first] ) !=
                        FT_CURVE_TAG_ON                                   ||
                      FT_CURVE_TAG( outline.tags[best_segment_last]  ) !=
                        FT_CURVE_TAG_ON                                   );

          FT_TRACE5(( " (%s)\n", round ? "round" : "flat" ));
        }

        if ( round )
          rounds[num_rounds++] = best_y;
        else
          flats[num_flats++]   = best_y;
      }

      if ( num_flats == 0 && num_rounds == 0 )
      {
        /*
         *  we couldn't find a single glyph to compute this blue zone,
         *  we will simply ignore it then
         */
        FT_TRACE5(( "  empty\n" ));
        continue;
      }

      /* we have computed the contents of the `rounds' and `flats' tables, */
      /* now determine the reference and overshoot position of the blue -- */
      /* we simply take the median value after a simple sort               */
      af_sort_pos( num_rounds, rounds );
      af_sort_pos( num_flats,  flats );

      blue       = &axis->blues[axis->blue_count];
      blue_ref   = &blue->ref.org;
      blue_shoot = &blue->shoot.org;

      axis->blue_count++;

      if ( num_flats == 0 )
      {
        *blue_ref   =
        *blue_shoot = rounds[num_rounds / 2];
      }
      else if ( num_rounds == 0 )
      {
        *blue_ref   =
        *blue_shoot = flats[num_flats / 2];
      }
      else
      {
        *blue_ref   = flats [num_flats  / 2];
        *blue_shoot = rounds[num_rounds / 2];
      }

      /* there are sometimes problems: if the overshoot position of top     */
      /* zones is under its reference position, or the opposite for bottom  */
      /* zones.  We must thus check everything there and correct the errors */
      if ( *blue_shoot != *blue_ref )
      {
        FT_Pos   ref      = *blue_ref;
        FT_Pos   shoot    = *blue_shoot;
        FT_Bool  over_ref = FT_BOOL( shoot > ref );


        if ( AF_LATIN_IS_TOP_BLUE( bs ) ^ over_ref )
        {
          *blue_ref   =
          *blue_shoot = ( shoot + ref ) / 2;

          FT_TRACE5(( "  [overshoot smaller than reference,"
                      " taking mean value]\n" ));
        }
      }

      blue->flags = 0;
      if ( AF_LATIN_IS_TOP_BLUE( bs ) )
        blue->flags |= AF_LATIN_BLUE_TOP;

      /*
       * The following flag is used later to adjust the y and x scales
       * in order to optimize the pixel grid alignment of the top of small
       * letters.
       */
      if ( AF_LATIN_IS_X_HEIGHT_BLUE( bs ) )
        blue->flags |= AF_LATIN_BLUE_ADJUSTMENT;

      FT_TRACE5(( "    -> reference = %ld\n"
                  "       overshoot = %ld\n",
                  *blue_ref, *blue_shoot ));
    }

    FT_TRACE5(( "\n" ));

    return;
  }


  /* Check whether all ASCII digits have the same advance width. */

  FT_LOCAL_DEF( void )
  af_latin_metrics_check_digits( AF_LatinMetrics  metrics,
                                 FT_Face          face )
  {
    FT_UInt   i;
    FT_Bool   started = 0, same_width = 1;
    FT_Fixed  advance, old_advance = 0;


    /* digit `0' is 0x30 in all supported charmaps */
    for ( i = 0x30; i <= 0x39; i++ )
    {
      FT_ULong  glyph_index;
      FT_Long   y_offset;


      af_get_char_index( &metrics->root, i, &glyph_index, &y_offset );
      if ( glyph_index == 0 )
        continue;

      if ( FT_Get_Advance( face, glyph_index,
                           FT_LOAD_NO_SCALE         |
                           FT_LOAD_NO_HINTING       |
                           FT_LOAD_IGNORE_TRANSFORM,
                           &advance ) )
        continue;

      if ( started )
      {
        if ( advance != old_advance )
        {
          same_width = 0;
          break;
        }
      }
      else
      {
        old_advance = advance;
        started     = 1;
      }
    }

    metrics->root.digits_have_same_width = same_width;
  }


  /* Initialize global metrics. */

  FT_LOCAL_DEF( FT_Error )
  af_latin_metrics_init( AF_LatinMetrics  metrics,
                         FT_Face          face )
  {
    FT_CharMap  oldmap = face->charmap;


    metrics->units_per_em = face->units_per_EM;

    if ( !FT_Select_Charmap( face, FT_ENCODING_UNICODE ) )
    {
      af_latin_metrics_init_widths( metrics, face );
      af_latin_metrics_init_blues( metrics, face );
      af_latin_metrics_check_digits( metrics, face );
    }

    FT_Set_Charmap( face, oldmap );
    return FT_Err_Ok;
  }


  /* Adjust scaling value, then scale and shift widths   */
  /* and blue zones (if applicable) for given dimension. */

  static void
  af_latin_metrics_scale_dim( AF_LatinMetrics  metrics,
                              AF_Scaler        scaler,
                              AF_Dimension     dim )
  {
    FT_Fixed      scale;
    FT_Pos        delta;
    AF_LatinAxis  axis;
    FT_UInt       nn;


    if ( dim == AF_DIMENSION_HORZ )
    {
      scale = scaler->x_scale;
      delta = scaler->x_delta;
    }
    else
    {
      scale = scaler->y_scale;
      delta = scaler->y_delta;
    }

    axis = &metrics->axis[dim];

    if ( axis->org_scale == scale && axis->org_delta == delta )
      return;

    axis->org_scale = scale;
    axis->org_delta = delta;

    /*
     * correct X and Y scale to optimize the alignment of the top of small
     * letters to the pixel grid
     */
    {
      AF_LatinAxis  Axis = &metrics->axis[AF_DIMENSION_VERT];
      AF_LatinBlue  blue = NULL;


      for ( nn = 0; nn < Axis->blue_count; nn++ )
      {
        if ( Axis->blues[nn].flags & AF_LATIN_BLUE_ADJUSTMENT )
        {
          blue = &Axis->blues[nn];
          break;
        }
      }

      if ( blue )
      {
        FT_Pos   scaled;
        FT_Pos   threshold;
        FT_Pos   fitted;
        FT_UInt  limit;
        FT_UInt  ppem;


        scaled    = FT_MulFix( blue->shoot.org, scaler->y_scale );
        ppem      = metrics->root.scaler.face->size->metrics.x_ppem;
        limit     = metrics->root.globals->increase_x_height;
        threshold = 40;

        /* if the `increase-x-height' property is active, */
        /* we round up much more often                    */
        if ( limit                                 &&
             ppem <= limit                         &&
             ppem >= AF_PROP_INCREASE_X_HEIGHT_MIN )
          threshold = 52;

        fitted = ( scaled + threshold ) & ~63;

        if ( scaled != fitted )
        {
#if 0
          if ( dim == AF_DIMENSION_HORZ )
          {
            if ( fitted < scaled )
              scale -= scale / 50;  /* scale *= 0.98 */
          }
          else
#endif
          if ( dim == AF_DIMENSION_VERT )
          {
            scale = FT_MulDiv( scale, fitted, scaled );

            FT_TRACE5((
              "af_latin_metrics_scale_dim:"
              " x height alignment (style `%s'):\n"
              "                           "
              " vertical scaling changed from %.4f to %.4f (by %d%%)\n"
              "\n",
              af_style_names[metrics->root.style_class->style],
              axis->org_scale / 65536.0,
              scale / 65536.0,
              ( fitted - scaled ) * 100 / scaled ));
          }
        }
      }
    }

    axis->scale = scale;
    axis->delta = delta;

    if ( dim == AF_DIMENSION_HORZ )
    {
      metrics->root.scaler.x_scale = scale;
      metrics->root.scaler.x_delta = delta;
    }
    else
    {
      metrics->root.scaler.y_scale = scale;
      metrics->root.scaler.y_delta = delta;
    }

    FT_TRACE5(( "%s widths (style `%s')\n",
                dim == AF_DIMENSION_HORZ ? "horizontal" : "vertical",
                af_style_names[metrics->root.style_class->style] ));

    /* scale the widths */
    for ( nn = 0; nn < axis->width_count; nn++ )
    {
      AF_Width  width = axis->widths + nn;


      width->cur = FT_MulFix( width->org, scale );
      width->fit = width->cur;

      FT_TRACE5(( "  %d scaled to %.2f\n",
                  width->org,
                  width->cur / 64.0 ));
    }

    FT_TRACE5(( "\n" ));

    /* an extra-light axis corresponds to a standard width that is */
    /* smaller than 5/8 pixels                                     */
    axis->extra_light =
      (FT_Bool)( FT_MulFix( axis->standard_width, scale ) < 32 + 8 );

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( axis->extra_light )
      FT_TRACE5(( "`%s' style is extra light (at current resolution)\n"
                  "\n",
                  af_style_names[metrics->root.style_class->style] ));
#endif

    if ( dim == AF_DIMENSION_VERT )
    {
      FT_TRACE5(( "blue zones (style `%s')\n",
                  af_style_names[metrics->root.style_class->style] ));

      /* scale the blue zones */
      for ( nn = 0; nn < axis->blue_count; nn++ )
      {
        AF_LatinBlue  blue = &axis->blues[nn];
        FT_Pos        dist;


        blue->ref.cur   = FT_MulFix( blue->ref.org, scale ) + delta;
        blue->ref.fit   = blue->ref.cur;
        blue->shoot.cur = FT_MulFix( blue->shoot.org, scale ) + delta;
        blue->shoot.fit = blue->shoot.cur;
        blue->flags    &= ~AF_LATIN_BLUE_ACTIVE;

        /* a blue zone is only active if it is less than 3/4 pixels tall */
        dist = FT_MulFix( blue->ref.org - blue->shoot.org, scale );
        if ( dist <= 48 && dist >= -48 )
        {
#if 0
          FT_Pos  delta1;
#endif
          FT_Pos  delta2;


          /* use discrete values for blue zone widths */

#if 0

          /* generic, original code */
          delta1 = blue->shoot.org - blue->ref.org;
          delta2 = delta1;
          if ( delta1 < 0 )
            delta2 = -delta2;

          delta2 = FT_MulFix( delta2, scale );

          if ( delta2 < 32 )
            delta2 = 0;
          else if ( delta2 < 64 )
            delta2 = 32 + ( ( ( delta2 - 32 ) + 16 ) & ~31 );
          else
            delta2 = FT_PIX_ROUND( delta2 );

          if ( delta1 < 0 )
            delta2 = -delta2;

          blue->ref.fit   = FT_PIX_ROUND( blue->ref.cur );
          blue->shoot.fit = blue->ref.fit + delta2;

#else

          /* simplified version due to abs(dist) <= 48 */
          delta2 = dist;
          if ( dist < 0 )
            delta2 = -delta2;

          if ( delta2 < 32 )
            delta2 = 0;
          else if ( delta2 < 48 )
            delta2 = 32;
          else
            delta2 = 64;

          if ( dist < 0 )
            delta2 = -delta2;

          blue->ref.fit   = FT_PIX_ROUND( blue->ref.cur );
          blue->shoot.fit = blue->ref.fit - delta2;

#endif

          blue->flags |= AF_LATIN_BLUE_ACTIVE;

          FT_TRACE5(( "  reference %d: %d scaled to %.2f%s\n"
                      "  overshoot %d: %d scaled to %.2f%s\n",
                      nn,
                      blue->ref.org,
                      blue->ref.fit / 64.0,
                      blue->flags & AF_LATIN_BLUE_ACTIVE ? ""
                                                         : " (inactive)",
                      nn,
                      blue->shoot.org,
                      blue->shoot.fit / 64.0,
                      blue->flags & AF_LATIN_BLUE_ACTIVE ? ""
                                                         : " (inactive)" ));
        }
      }
    }
  }


  /* Scale global values in both directions. */

  FT_LOCAL_DEF( void )
  af_latin_metrics_scale( AF_LatinMetrics  metrics,
                          AF_Scaler        scaler )
  {
    metrics->root.scaler.render_mode = scaler->render_mode;
    metrics->root.scaler.face        = scaler->face;
    metrics->root.scaler.flags       = scaler->flags;

    af_latin_metrics_scale_dim( metrics, scaler, AF_DIMENSION_HORZ );
    af_latin_metrics_scale_dim( metrics, scaler, AF_DIMENSION_VERT );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****           L A T I N   G L Y P H   A N A L Y S I S             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /* Walk over all contours and compute its segments. */

  FT_LOCAL_DEF( FT_Error )
  af_latin_hints_compute_segments( AF_GlyphHints  hints,
                                   AF_Dimension   dim )
  {
    AF_AxisHints   axis          = &hints->axis[dim];
    FT_Memory      memory        = hints->memory;
    FT_Error       error         = FT_Err_Ok;
    AF_Segment     segment       = NULL;
    AF_SegmentRec  seg0;
    AF_Point*      contour       = hints->contours;
    AF_Point*      contour_limit = contour + hints->num_contours;
    AF_Direction   major_dir, segment_dir;


    FT_ZERO( &seg0 );
    seg0.score = 32000;
    seg0.flags = AF_EDGE_NORMAL;

    major_dir   = (AF_Direction)FT_ABS( axis->major_dir );
    segment_dir = major_dir;

    axis->num_segments = 0;

    /* set up (u,v) in each point */
    if ( dim == AF_DIMENSION_HORZ )
    {
      AF_Point  point = hints->points;
      AF_Point  limit = point + hints->num_points;


      for ( ; point < limit; point++ )
      {
        point->u = point->fx;
        point->v = point->fy;
      }
    }
    else
    {
      AF_Point  point = hints->points;
      AF_Point  limit = point + hints->num_points;


      for ( ; point < limit; point++ )
      {
        point->u = point->fy;
        point->v = point->fx;
      }
    }

    /* do each contour separately */
    for ( ; contour < contour_limit; contour++ )
    {
      AF_Point  point   =  contour[0];
      AF_Point  last    =  point->prev;
      int       on_edge =  0;
      FT_Pos    min_pos =  32000;  /* minimum segment pos != min_coord */
      FT_Pos    max_pos = -32000;  /* maximum segment pos != max_coord */
      FT_Bool   passed;


      if ( point == last )  /* skip singletons -- just in case */
        continue;

      if ( FT_ABS( last->out_dir )  == major_dir &&
           FT_ABS( point->out_dir ) == major_dir )
      {
        /* we are already on an edge, try to locate its start */
        last = point;

        for (;;)
        {
          point = point->prev;
          if ( FT_ABS( point->out_dir ) != major_dir )
          {
            point = point->next;
            break;
          }
          if ( point == last )
            break;
        }
      }

      last   = point;
      passed = 0;

      for (;;)
      {
        FT_Pos  u, v;


        if ( on_edge )
        {
          u = point->u;
          if ( u < min_pos )
            min_pos = u;
          if ( u > max_pos )
            max_pos = u;

          if ( point->out_dir != segment_dir || point == last )
          {
            /* we are just leaving an edge; record a new segment! */
            segment->last = point;
            segment->pos  = (FT_Short)( ( min_pos + max_pos ) >> 1 );

            /* a segment is round if either its first or last point */
            /* is a control point                                   */
            if ( ( segment->first->flags | point->flags ) &
                 AF_FLAG_CONTROL                          )
              segment->flags |= AF_EDGE_ROUND;

            /* compute segment size */
            min_pos = max_pos = point->v;

            v = segment->first->v;
            if ( v < min_pos )
              min_pos = v;
            if ( v > max_pos )
              max_pos = v;

            segment->min_coord = (FT_Short)min_pos;
            segment->max_coord = (FT_Short)max_pos;
            segment->height    = (FT_Short)( segment->max_coord -
                                             segment->min_coord );

            on_edge = 0;
            segment = NULL;
            /* fall through */
          }
        }

        /* now exit if we are at the start/end point */
        if ( point == last )
        {
          if ( passed )
            break;
          passed = 1;
        }

        if ( !on_edge && FT_ABS( point->out_dir ) == major_dir )
        {
          /* this is the start of a new segment! */
          segment_dir = (AF_Direction)point->out_dir;

          /* clear all segment fields */
          error = af_axis_hints_new_segment( axis, memory, &segment );
          if ( error )
            goto Exit;

          segment[0]        = seg0;
          segment->dir      = (FT_Char)segment_dir;
          min_pos = max_pos = point->u;
          segment->first    = point;
          segment->last     = point;
          on_edge           = 1;
        }

        point = point->next;
      }

    } /* contours */


    /* now slightly increase the height of segments if this makes */
    /* sense -- this is used to better detect and ignore serifs   */
    {
      AF_Segment  segments     = axis->segments;
      AF_Segment  segments_end = segments + axis->num_segments;


      for ( segment = segments; segment < segments_end; segment++ )
      {
        AF_Point  first   = segment->first;
        AF_Point  last    = segment->last;
        FT_Pos    first_v = first->v;
        FT_Pos    last_v  = last->v;


        if ( first == last )
          continue;

        if ( first_v < last_v )
        {
          AF_Point  p;


          p = first->prev;
          if ( p->v < first_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( first_v - p->v ) >> 1 ) );

          p = last->next;
          if ( p->v > last_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( p->v - last_v ) >> 1 ) );
        }
        else
        {
          AF_Point  p;


          p = first->prev;
          if ( p->v > first_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( p->v - first_v ) >> 1 ) );

          p = last->next;
          if ( p->v < last_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( last_v - p->v ) >> 1 ) );
        }
      }
    }

  Exit:
    return error;
  }


  /* Link segments to form stems and serifs. */

  FT_LOCAL_DEF( void )
  af_latin_hints_link_segments( AF_GlyphHints  hints,
                                AF_Dimension   dim )
  {
    AF_AxisHints  axis          = &hints->axis[dim];
    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    FT_Pos        len_threshold, len_score;
    AF_Segment    seg1, seg2;


    len_threshold = AF_LATIN_CONSTANT( hints->metrics, 8 );
    if ( len_threshold == 0 )
      len_threshold = 1;

    len_score = AF_LATIN_CONSTANT( hints->metrics, 6000 );

    /* now compare each segment to the others */
    for ( seg1 = segments; seg1 < segment_limit; seg1++ )
    {
      /* the fake segments are introduced to hint the metrics -- */
      /* we must never link them to anything                     */
      if ( seg1->dir != axis->major_dir || seg1->first == seg1->last )
        continue;

      /* search for stems having opposite directions, */
      /* with seg1 to the `left' of seg2              */
      for ( seg2 = segments; seg2 < segment_limit; seg2++ )
      {
        FT_Pos  pos1 = seg1->pos;
        FT_Pos  pos2 = seg2->pos;


        if ( seg1->dir + seg2->dir == 0 && pos2 > pos1 )
        {
          /* compute distance between the two segments */
          FT_Pos  dist = pos2 - pos1;
          FT_Pos  min  = seg1->min_coord;
          FT_Pos  max  = seg1->max_coord;
          FT_Pos  len, score;


          if ( min < seg2->min_coord )
            min = seg2->min_coord;

          if ( max > seg2->max_coord )
            max = seg2->max_coord;

          /* compute maximum coordinate difference of the two segments */
          len = max - min;
          if ( len >= len_threshold )
          {
            /* small coordinate differences cause a higher score, and     */
            /* segments with a greater distance cause a higher score also */
            score = dist + len_score / len;

            /* and we search for the smallest score */
            /* of the sum of the two values         */
            if ( score < seg1->score )
            {
              seg1->score = score;
              seg1->link  = seg2;
            }

            if ( score < seg2->score )
            {
              seg2->score = score;
              seg2->link  = seg1;
            }
          }
        }
      }
    }

    /* now compute the `serif' segments, cf. explanations in `afhints.h' */
    for ( seg1 = segments; seg1 < segment_limit; seg1++ )
    {
      seg2 = seg1->link;

      if ( seg2 )
      {
        if ( seg2->link != seg1 )
        {
          seg1->link  = 0;
          seg1->serif = seg2->link;
        }
      }
    }
  }


  /* Link segments to edges, using feature analysis for selection. */

  FT_LOCAL_DEF( FT_Error )
  af_latin_hints_compute_edges( AF_GlyphHints  hints,
                                AF_Dimension   dim )
  {
    AF_AxisHints  axis   = &hints->axis[dim];
    FT_Error      error  = FT_Err_Ok;
    FT_Memory     memory = hints->memory;
    AF_LatinAxis  laxis  = &((AF_LatinMetrics)hints->metrics)->axis[dim];

    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    AF_Segment    seg;

#if 0
    AF_Direction  up_dir;
#endif
    FT_Fixed      scale;
    FT_Pos        edge_distance_threshold;
    FT_Pos        segment_length_threshold;


    axis->num_edges = 0;

    scale = ( dim == AF_DIMENSION_HORZ ) ? hints->x_scale
                                         : hints->y_scale;

#if 0
    up_dir = ( dim == AF_DIMENSION_HORZ ) ? AF_DIR_UP
                                          : AF_DIR_RIGHT;
#endif

    /*
     *  We ignore all segments that are less than 1 pixel in length
     *  to avoid many problems with serif fonts.  We compute the
     *  corresponding threshold in font units.
     */
    if ( dim == AF_DIMENSION_HORZ )
        segment_length_threshold = FT_DivFix( 64, hints->y_scale );
    else
        segment_length_threshold = 0;

    /*********************************************************************/
    /*                                                                   */
    /* We begin by generating a sorted table of edges for the current    */
    /* direction.  To do so, we simply scan each segment and try to find */
    /* an edge in our table that corresponds to its position.            */
    /*                                                                   */
    /* If no edge is found, we create and insert a new edge in the       */
    /* sorted table.  Otherwise, we simply add the segment to the edge's */
    /* list which gets processed in the second step to compute the       */
    /* edge's properties.                                                */
    /*                                                                   */
    /* Note that the table of edges is sorted along the segment/edge     */
    /* position.                                                         */
    /*                                                                   */
    /*********************************************************************/

    /* assure that edge distance threshold is at most 0.25px */
    edge_distance_threshold = FT_MulFix( laxis->edge_distance_threshold,
                                         scale );
    if ( edge_distance_threshold > 64 / 4 )
      edge_distance_threshold = 64 / 4;

    edge_distance_threshold = FT_DivFix( edge_distance_threshold,
                                         scale );

    for ( seg = segments; seg < segment_limit; seg++ )
    {
      AF_Edge  found = NULL;
      FT_Int   ee;


      if ( seg->height < segment_length_threshold )
        continue;

      /* A special case for serif edges: If they are smaller than */
      /* 1.5 pixels we ignore them.                               */
      if ( seg->serif                                     &&
           2 * seg->height < 3 * segment_length_threshold )
        continue;

      /* look for an edge corresponding to the segment */
      for ( ee = 0; ee < axis->num_edges; ee++ )
      {
        AF_Edge  edge = axis->edges + ee;
        FT_Pos   dist;


        dist = seg->pos - edge->fpos;
        if ( dist < 0 )
          dist = -dist;

        if ( dist < edge_distance_threshold && edge->dir == seg->dir )
        {
          found = edge;
          break;
        }
      }

      if ( !found )
      {
        AF_Edge  edge;


        /* insert a new edge in the list and */
        /* sort according to the position    */
        error = af_axis_hints_new_edge( axis, seg->pos,
                                        (AF_Direction)seg->dir,
                                        memory, &edge );
        if ( error )
          goto Exit;

        /* add the segment to the new edge's list */
        FT_ZERO( edge );

        edge->first    = seg;
        edge->last     = seg;
        edge->dir      = seg->dir;
        edge->fpos     = seg->pos;
        edge->opos     = FT_MulFix( seg->pos, scale );
        edge->pos      = edge->opos;
        seg->edge_next = seg;
      }
      else
      {
        /* if an edge was found, simply add the segment to the edge's */
        /* list                                                       */
        seg->edge_next         = found->first;
        found->last->edge_next = seg;
        found->last            = seg;
      }
    }


    /******************************************************************/
    /*                                                                */
    /* Good, we now compute each edge's properties according to the   */
    /* segments found on its position.  Basically, these are          */
    /*                                                                */
    /*  - the edge's main direction                                   */
    /*  - stem edge, serif edge or both (which defaults to stem then) */
    /*  - rounded edge, straight or both (which defaults to straight) */
    /*  - link for edge                                               */
    /*                                                                */
    /******************************************************************/

    /* first of all, set the `edge' field in each segment -- this is */
    /* required in order to compute edge links                       */

    /*
     * Note that removing this loop and setting the `edge' field of each
     * segment directly in the code above slows down execution speed for
     * some reasons on platforms like the Sun.
     */
    {
      AF_Edge  edges      = axis->edges;
      AF_Edge  edge_limit = edges + axis->num_edges;
      AF_Edge  edge;


      for ( edge = edges; edge < edge_limit; edge++ )
      {
        seg = edge->first;
        if ( seg )
          do
          {
            seg->edge = edge;
            seg       = seg->edge_next;

          } while ( seg != edge->first );
      }

      /* now compute each edge properties */
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        FT_Int  is_round    = 0;  /* does it contain round segments?    */
        FT_Int  is_straight = 0;  /* does it contain straight segments? */
#if 0
        FT_Pos  ups         = 0;  /* number of upwards segments         */
        FT_Pos  downs       = 0;  /* number of downwards segments       */
#endif


        seg = edge->first;

        do
        {
          FT_Bool  is_serif;


          /* check for roundness of segment */
          if ( seg->flags & AF_EDGE_ROUND )
            is_round++;
          else
            is_straight++;

#if 0
          /* check for segment direction */
          if ( seg->dir == up_dir )
            ups   += seg->max_coord - seg->min_coord;
          else
            downs += seg->max_coord - seg->min_coord;
#endif

          /* check for links -- if seg->serif is set, then seg->link must */
          /* be ignored                                                   */
          is_serif = (FT_Bool)( seg->serif               &&
                                seg->serif->edge         &&
                                seg->serif->edge != edge );

          if ( ( seg->link && seg->link->edge != NULL ) || is_serif )
          {
            AF_Edge     edge2;
            AF_Segment  seg2;


            edge2 = edge->link;
            seg2  = seg->link;

            if ( is_serif )
            {
              seg2  = seg->serif;
              edge2 = edge->serif;
            }

            if ( edge2 )
            {
              FT_Pos  edge_delta;
              FT_Pos  seg_delta;


              edge_delta = edge->fpos - edge2->fpos;
              if ( edge_delta < 0 )
                edge_delta = -edge_delta;

              seg_delta = seg->pos - seg2->pos;
              if ( seg_delta < 0 )
                seg_delta = -seg_delta;

              if ( seg_delta < edge_delta )
                edge2 = seg2->edge;
            }
            else
              edge2 = seg2->edge;

            if ( is_serif )
            {
              edge->serif   = edge2;
              edge2->flags |= AF_EDGE_SERIF;
            }
            else
              edge->link  = edge2;
          }

          seg = seg->edge_next;

        } while ( seg != edge->first );

        /* set the round/straight flags */
        edge->flags = AF_EDGE_NORMAL;

        if ( is_round > 0 && is_round >= is_straight )
          edge->flags |= AF_EDGE_ROUND;

#if 0
        /* set the edge's main direction */
        edge->dir = AF_DIR_NONE;

        if ( ups > downs )
          edge->dir = (FT_Char)up_dir;

        else if ( ups < downs )
          edge->dir = (FT_Char)-up_dir;

        else if ( ups == downs )
          edge->dir = 0;  /* both up and down! */
#endif

        /* get rid of serifs if link is set                 */
        /* XXX: This gets rid of many unpleasant artefacts! */
        /*      Example: the `c' in cour.pfa at size 13     */

        if ( edge->serif && edge->link )
          edge->serif = 0;
      }
    }

  Exit:
    return error;
  }


  /* Detect segments and edges for given dimension. */

  FT_LOCAL_DEF( FT_Error )
  af_latin_hints_detect_features( AF_GlyphHints  hints,
                                  AF_Dimension   dim )
  {
    FT_Error  error;


    error = af_latin_hints_compute_segments( hints, dim );
    if ( !error )
    {
      af_latin_hints_link_segments( hints, dim );

      error = af_latin_hints_compute_edges( hints, dim );
    }

    return error;
  }


  /* Compute all edges which lie within blue zones. */

  FT_LOCAL_DEF( void )
  af_latin_hints_compute_blue_edges( AF_GlyphHints    hints,
                                     AF_LatinMetrics  metrics )
  {
    AF_AxisHints  axis       = &hints->axis[AF_DIMENSION_VERT];
    AF_Edge       edge       = axis->edges;
    AF_Edge       edge_limit = edge + axis->num_edges;
    AF_LatinAxis  latin      = &metrics->axis[AF_DIMENSION_VERT];
    FT_Fixed      scale      = latin->scale;


    /* compute which blue zones are active, i.e. have their scaled */
    /* size < 3/4 pixels                                           */

    /* for each horizontal edge search the blue zone which is closest */
    for ( ; edge < edge_limit; edge++ )
    {
      FT_UInt   bb;
      AF_Width  best_blue = NULL;
      FT_Pos    best_dist;  /* initial threshold */


      /* compute the initial threshold as a fraction of the EM size */
      /* (the value 40 is heuristic)                                */
      best_dist = FT_MulFix( metrics->units_per_em / 40, scale );

      /* assure a minimum distance of 0.5px */
      if ( best_dist > 64 / 2 )
        best_dist = 64 / 2;

      for ( bb = 0; bb < latin->blue_count; bb++ )
      {
        AF_LatinBlue  blue = latin->blues + bb;
        FT_Bool       is_top_blue, is_major_dir;


        /* skip inactive blue zones (i.e., those that are too large) */
        if ( !( blue->flags & AF_LATIN_BLUE_ACTIVE ) )
          continue;

        /* if it is a top zone, check for right edges -- if it is a bottom */
        /* zone, check for left edges                                      */
        /*                                                                 */
        /* of course, that's for TrueType                                  */
        is_top_blue  = (FT_Byte)( ( blue->flags & AF_LATIN_BLUE_TOP ) != 0 );
        is_major_dir = FT_BOOL( edge->dir == axis->major_dir );

        /* if it is a top zone, the edge must be against the major    */
        /* direction; if it is a bottom zone, it must be in the major */
        /* direction                                                  */
        if ( is_top_blue ^ is_major_dir )
        {
          FT_Pos  dist;


          /* first of all, compare it to the reference position */
          dist = edge->fpos - blue->ref.org;
          if ( dist < 0 )
            dist = -dist;

          dist = FT_MulFix( dist, scale );
          if ( dist < best_dist )
          {
            best_dist = dist;
            best_blue = &blue->ref;
          }

          /* now compare it to the overshoot position and check whether */
          /* the edge is rounded, and whether the edge is over the      */
          /* reference position of a top zone, or under the reference   */
          /* position of a bottom zone                                  */
          if ( edge->flags & AF_EDGE_ROUND && dist != 0 )
          {
            FT_Bool  is_under_ref = FT_BOOL( edge->fpos < blue->ref.org );


            if ( is_top_blue ^ is_under_ref )
            {
              dist = edge->fpos - blue->shoot.org;
              if ( dist < 0 )
                dist = -dist;

              dist = FT_MulFix( dist, scale );
              if ( dist < best_dist )
              {
                best_dist = dist;
                best_blue = &blue->shoot;
              }
            }
          }
        }
      }

      if ( best_blue )
        edge->blue_edge = best_blue;
    }
  }


  /* Initalize hinting engine. */

  static FT_Error
  af_latin_hints_init( AF_GlyphHints    hints,
                       AF_LatinMetrics  metrics )
  {
    FT_Render_Mode  mode;
    FT_UInt32       scaler_flags, other_flags;
    FT_Face         face = metrics->root.scaler.face;


    af_glyph_hints_rescale( hints, (AF_StyleMetrics)metrics );

    /*
     *  correct x_scale and y_scale if needed, since they may have
     *  been modified by `af_latin_metrics_scale_dim' above
     */
    hints->x_scale = metrics->axis[AF_DIMENSION_HORZ].scale;
    hints->x_delta = metrics->axis[AF_DIMENSION_HORZ].delta;
    hints->y_scale = metrics->axis[AF_DIMENSION_VERT].scale;
    hints->y_delta = metrics->axis[AF_DIMENSION_VERT].delta;

    /* compute flags depending on render mode, etc. */
    mode = metrics->root.scaler.render_mode;

#if 0 /* #ifdef AF_CONFIG_OPTION_USE_WARPER */
    if ( mode == FT_RENDER_MODE_LCD || mode == FT_RENDER_MODE_LCD_V )
      metrics->root.scaler.render_mode = mode = FT_RENDER_MODE_NORMAL;
#endif

    scaler_flags = hints->scaler_flags;
    other_flags  = 0;

    /*
     *  We snap the width of vertical stems for the monochrome and
     *  horizontal LCD rendering targets only.
     */
    if ( mode == FT_RENDER_MODE_MONO || mode == FT_RENDER_MODE_LCD )
      other_flags |= AF_LATIN_HINTS_HORZ_SNAP;

    /*
     *  We snap the width of horizontal stems for the monochrome and
     *  vertical LCD rendering targets only.
     */
    if ( mode == FT_RENDER_MODE_MONO || mode == FT_RENDER_MODE_LCD_V )
      other_flags |= AF_LATIN_HINTS_VERT_SNAP;

    /*
     *  We adjust stems to full pixels only if we don't use the `light' mode.
     */
    if ( mode != FT_RENDER_MODE_LIGHT )
      other_flags |= AF_LATIN_HINTS_STEM_ADJUST;

    if ( mode == FT_RENDER_MODE_MONO )
      other_flags |= AF_LATIN_HINTS_MONO;

    /*
     *  In `light' hinting mode we disable horizontal hinting completely.
     *  We also do it if the face is italic.
     */
    if ( mode == FT_RENDER_MODE_LIGHT                      ||
         ( face->style_flags & FT_STYLE_FLAG_ITALIC ) != 0 )
      scaler_flags |= AF_SCALER_FLAG_NO_HORIZONTAL;

    hints->scaler_flags = scaler_flags;
    hints->other_flags  = other_flags;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****        L A T I N   G L Y P H   G R I D - F I T T I N G        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* Snap a given width in scaled coordinates to one of the */
  /* current standard widths.                               */

  static FT_Pos
  af_latin_snap_width( AF_Width  widths,
                       FT_Int    count,
                       FT_Pos    width )
  {
    int     n;
    FT_Pos  best      = 64 + 32 + 2;
    FT_Pos  reference = width;
    FT_Pos  scaled;


    for ( n = 0; n < count; n++ )
    {
      FT_Pos  w;
      FT_Pos  dist;


      w = widths[n].cur;
      dist = width - w;
      if ( dist < 0 )
        dist = -dist;
      if ( dist < best )
      {
        best      = dist;
        reference = w;
      }
    }

    scaled = FT_PIX_ROUND( reference );

    if ( width >= reference )
    {
      if ( width < scaled + 48 )
        width = reference;
    }
    else
    {
      if ( width > scaled - 48 )
        width = reference;
    }

    return width;
  }


  /* Compute the snapped width of a given stem, ignoring very thin ones. */
  /* There is a lot of voodoo in this function; changing the hard-coded  */
  /* parameters influence the whole hinting process.                     */

  static FT_Pos
  af_latin_compute_stem_width( AF_GlyphHints  hints,
                               AF_Dimension   dim,
                               FT_Pos         width,
                               AF_Edge_Flags  base_flags,
                               AF_Edge_Flags  stem_flags )
  {
    AF_LatinMetrics  metrics  = (AF_LatinMetrics)hints->metrics;
    AF_LatinAxis     axis     = &metrics->axis[dim];
    FT_Pos           dist     = width;
    FT_Int           sign     = 0;
    FT_Int           vertical = ( dim == AF_DIMENSION_VERT );


    if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) ||
         axis->extra_light                       )
      return width;

    if ( dist < 0 )
    {
      dist = -width;
      sign = 1;
    }

    if ( (  vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) ) ||
         ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) )
    {
      /* smooth hinting process: very lightly quantize the stem width */

      /* leave the widths of serifs alone */
      if ( ( stem_flags & AF_EDGE_SERIF ) &&
           vertical                       &&
           ( dist < 3 * 64 )              )
        goto Done_Width;

      else if ( base_flags & AF_EDGE_ROUND )
      {
        if ( dist < 80 )
          dist = 64;
      }
      else if ( dist < 56 )
        dist = 56;

      if ( axis->width_count > 0 )
      {
        FT_Pos  delta;


        /* compare to standard width */
        delta = dist - axis->widths[0].cur;

        if ( delta < 0 )
          delta = -delta;

        if ( delta < 40 )
        {
          dist = axis->widths[0].cur;
          if ( dist < 48 )
            dist = 48;

          goto Done_Width;
        }

        if ( dist < 3 * 64 )
        {
          delta  = dist & 63;
          dist  &= -64;

          if ( delta < 10 )
            dist += delta;

          else if ( delta < 32 )
            dist += 10;

          else if ( delta < 54 )
            dist += 54;

          else
            dist += delta;
        }
        else
          dist = ( dist + 32 ) & ~63;
      }
    }
    else
    {
      /* strong hinting process: snap the stem width to integer pixels */

      FT_Pos  org_dist = dist;


      dist = af_latin_snap_width( axis->widths, axis->width_count, dist );

      if ( vertical )
      {
        /* in the case of vertical hinting, always round */
        /* the stem heights to integer pixels            */

        if ( dist >= 64 )
          dist = ( dist + 16 ) & ~63;
        else
          dist = 64;
      }
      else
      {
        if ( AF_LATIN_HINTS_DO_MONO( hints ) )
        {
          /* monochrome horizontal hinting: snap widths to integer pixels */
          /* with a different threshold                                   */

          if ( dist < 64 )
            dist = 64;
          else
            dist = ( dist + 32 ) & ~63;
        }
        else
        {
          /* for horizontal anti-aliased hinting, we adopt a more subtle */
          /* approach: we strengthen small stems, round stems whose size */
          /* is between 1 and 2 pixels to an integer, otherwise nothing  */

          if ( dist < 48 )
            dist = ( dist + 64 ) >> 1;

          else if ( dist < 128 )
          {
            /* We only round to an integer width if the corresponding */
            /* distortion is less than 1/4 pixel.  Otherwise this     */
            /* makes everything worse since the diagonals, which are  */
            /* not hinted, appear a lot bolder or thinner than the    */
            /* vertical stems.                                        */

            FT_Pos  delta;


            dist = ( dist + 22 ) & ~63;
            delta = dist - org_dist;
            if ( delta < 0 )
              delta = -delta;

            if ( delta >= 16 )
            {
              dist = org_dist;
              if ( dist < 48 )
                dist = ( dist + 64 ) >> 1;
            }
          }
          else
            /* round otherwise to prevent color fringes in LCD mode */
            dist = ( dist + 32 ) & ~63;
        }
      }
    }

  Done_Width:
    if ( sign )
      dist = -dist;

    return dist;
  }


  /* Align one stem edge relative to the previous stem edge. */

  static void
  af_latin_align_linked_edge( AF_GlyphHints  hints,
                              AF_Dimension   dim,
                              AF_Edge        base_edge,
                              AF_Edge        stem_edge )
  {
    FT_Pos  dist = stem_edge->opos - base_edge->opos;

    FT_Pos  fitted_width = af_latin_compute_stem_width(
                             hints, dim, dist,
                             (AF_Edge_Flags)base_edge->flags,
                             (AF_Edge_Flags)stem_edge->flags );


    stem_edge->pos = base_edge->pos + fitted_width;

    FT_TRACE5(( "  LINK: edge %d (opos=%.2f) linked to %.2f,"
                " dist was %.2f, now %.2f\n",
                stem_edge-hints->axis[dim].edges, stem_edge->opos / 64.0,
                stem_edge->pos / 64.0, dist / 64.0, fitted_width / 64.0 ));
  }


  /* Shift the coordinates of the `serif' edge by the same amount */
  /* as the corresponding `base' edge has been moved already.     */

  static void
  af_latin_align_serif_edge( AF_GlyphHints  hints,
                             AF_Edge        base,
                             AF_Edge        serif )
  {
    FT_UNUSED( hints );

    serif->pos = base->pos + ( serif->opos - base->opos );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                    E D G E   H I N T I N G                      ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  /* The main grid-fitting routine. */

  FT_LOCAL_DEF( void )
  af_latin_hint_edges( AF_GlyphHints  hints,
                       AF_Dimension   dim )
  {
    AF_AxisHints  axis       = &hints->axis[dim];
    AF_Edge       edges      = axis->edges;
    AF_Edge       edge_limit = edges + axis->num_edges;
    FT_PtrDist    n_edges;
    AF_Edge       edge;
    AF_Edge       anchor     = NULL;
    FT_Int        has_serifs = 0;

#ifdef FT_DEBUG_LEVEL_TRACE
    FT_UInt       num_actions = 0;
#endif


    FT_TRACE5(( "latin %s edge hinting (style `%s')\n",
                dim == AF_DIMENSION_VERT ? "horizontal" : "vertical",
                af_style_names[hints->metrics->style_class->style] ));

    /* we begin by aligning all stems relative to the blue zone */
    /* if needed -- that's only for horizontal edges            */

    if ( dim == AF_DIMENSION_VERT && AF_HINTS_DO_BLUES( hints ) )
    {
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        AF_Width  blue;
        AF_Edge   edge1, edge2; /* these edges form the stem to check */


        if ( edge->flags & AF_EDGE_DONE )
          continue;

        blue  = edge->blue_edge;
        edge1 = NULL;
        edge2 = edge->link;

        if ( blue )
          edge1 = edge;

        /* flip edges if the other stem is aligned to a blue zone */
        else if ( edge2 && edge2->blue_edge )
        {
          blue  = edge2->blue_edge;
          edge1 = edge2;
          edge2 = edge;
        }

        if ( !edge1 )
          continue;

#ifdef FT_DEBUG_LEVEL_TRACE
        if ( !anchor )
          FT_TRACE5(( "  BLUE_ANCHOR: edge %d (opos=%.2f) snapped to %.2f,"
                      " was %.2f (anchor=edge %d)\n",
                      edge1 - edges, edge1->opos / 64.0, blue->fit / 64.0,
                      edge1->pos / 64.0, edge - edges ));
        else
          FT_TRACE5(( "  BLUE: edge %d (opos=%.2f) snapped to %.2f,"
                      " was %.2f\n",
                      edge1 - edges, edge1->opos / 64.0, blue->fit / 64.0,
                      edge1->pos / 64.0 ));

        num_actions++;
#endif

        edge1->pos    = blue->fit;
        edge1->flags |= AF_EDGE_DONE;

        if ( edge2 && !edge2->blue_edge )
        {
          af_latin_align_linked_edge( hints, dim, edge1, edge2 );
          edge2->flags |= AF_EDGE_DONE;

#ifdef FT_DEBUG_LEVEL_TRACE
          num_actions++;
#endif
        }

        if ( !anchor )
          anchor = edge;
      }
    }

    /* now we align all other stem edges, trying to maintain the */
    /* relative order of stems in the glyph                      */
    for ( edge = edges; edge < edge_limit; edge++ )
    {
      AF_Edge  edge2;


      if ( edge->flags & AF_EDGE_DONE )
        continue;

      /* skip all non-stem edges */
      edge2 = edge->link;
      if ( !edge2 )
      {
        has_serifs++;
        continue;
      }

      /* now align the stem */

      /* this should not happen, but it's better to be safe */
      if ( edge2->blue_edge )
      {
        FT_TRACE5(( "  ASSERTION FAILED for edge %d\n", edge2-edges ));

        af_latin_align_linked_edge( hints, dim, edge2, edge );
        edge->flags |= AF_EDGE_DONE;

#ifdef FT_DEBUG_LEVEL_TRACE
        num_actions++;
#endif
        continue;
      }

      if ( !anchor )
      {
        /* if we reach this if clause, no stem has been aligned yet */

        FT_Pos  org_len, org_center, cur_len;
        FT_Pos  cur_pos1, error1, error2, u_off, d_off;


        org_len = edge2->opos - edge->opos;
        cur_len = af_latin_compute_stem_width(
                    hints, dim, org_len,
                    (AF_Edge_Flags)edge->flags,
                    (AF_Edge_Flags)edge2->flags );

        /* some voodoo to specially round edges for small stem widths; */
        /* the idea is to align the center of a stem, then shifting    */
        /* the stem edges to suitable positions                        */
        if ( cur_len <= 64 )
        {
          /* width <= 1px */
          u_off = 32;
          d_off = 32;
        }
        else
        {
          /* 1px < width < 1.5px */
          u_off = 38;
          d_off = 26;
        }

        if ( cur_len < 96 )
        {
          org_center = edge->opos + ( org_len >> 1 );
          cur_pos1   = FT_PIX_ROUND( org_center );

          error1 = org_center - ( cur_pos1 - u_off );
          if ( error1 < 0 )
            error1 = -error1;

          error2 = org_center - ( cur_pos1 + d_off );
          if ( error2 < 0 )
            error2 = -error2;

          if ( error1 < error2 )
            cur_pos1 -= u_off;
          else
            cur_pos1 += d_off;

          edge->pos  = cur_pos1 - cur_len / 2;
          edge2->pos = edge->pos + cur_len;
        }
        else
          edge->pos = FT_PIX_ROUND( edge->opos );

        anchor       = edge;
        edge->flags |= AF_EDGE_DONE;

        FT_TRACE5(( "  ANCHOR: edge %d (opos=%.2f) and %d (opos=%.2f)"
                    " snapped to %.2f and %.2f\n",
                    edge - edges, edge->opos / 64.0,
                    edge2 - edges, edge2->opos / 64.0,
                    edge->pos / 64.0, edge2->pos / 64.0 ));

        af_latin_align_linked_edge( hints, dim, edge, edge2 );

#ifdef FT_DEBUG_LEVEL_TRACE
        num_actions += 2;
#endif
      }
      else
      {
        FT_Pos  org_pos, org_len, org_center, cur_len;
        FT_Pos  cur_pos1, cur_pos2, delta1, delta2;


        org_pos    = anchor->pos + ( edge->opos - anchor->opos );
        org_len    = edge2->opos - edge->opos;
        org_center = org_pos + ( org_len >> 1 );

        cur_len = af_latin_compute_stem_width(
                    hints, dim, org_len,
                    (AF_Edge_Flags)edge->flags,
                    (AF_Edge_Flags)edge2->flags );

        if ( edge2->flags & AF_EDGE_DONE )
        {
          FT_TRACE5(( "  ADJUST: edge %d (pos=%.2f) moved to %.2f\n",
                      edge - edges, edge->pos / 64.0,
                      ( edge2->pos - cur_len ) / 64.0 ));

          edge->pos = edge2->pos - cur_len;
        }

        else if ( cur_len < 96 )
        {
          FT_Pos  u_off, d_off;


          cur_pos1 = FT_PIX_ROUND( org_center );

          if ( cur_len <= 64 )
          {
            u_off = 32;
            d_off = 32;
          }
          else
          {
            u_off = 38;
            d_off = 26;
          }

          delta1 = org_center - ( cur_pos1 - u_off );
          if ( delta1 < 0 )
            delta1 = -delta1;

          delta2 = org_center - ( cur_pos1 + d_off );
          if ( delta2 < 0 )
            delta2 = -delta2;

          if ( delta1 < delta2 )
            cur_pos1 -= u_off;
          else
            cur_pos1 += d_off;

          edge->pos  = cur_pos1 - cur_len / 2;
          edge2->pos = cur_pos1 + cur_len / 2;

          FT_TRACE5(( "  STEM: edge %d (opos=%.2f) linked to %d (opos=%.2f)"
                      " snapped to %.2f and %.2f\n",
                      edge - edges, edge->opos / 64.0,
                      edge2 - edges, edge2->opos / 64.0,
                      edge->pos / 64.0, edge2->pos / 64.0 ));
        }

        else
        {
          org_pos    = anchor->pos + ( edge->opos - anchor->opos );
          org_len    = edge2->opos - edge->opos;
          org_center = org_pos + ( org_len >> 1 );

          cur_len    = af_latin_compute_stem_width(
                         hints, dim, org_len,
                         (AF_Edge_Flags)edge->flags,
                         (AF_Edge_Flags)edge2->flags );

          cur_pos1 = FT_PIX_ROUND( org_pos );
          delta1   = cur_pos1 + ( cur_len >> 1 ) - org_center;
          if ( delta1 < 0 )
            delta1 = -delta1;

          cur_pos2 = FT_PIX_ROUND( org_pos + org_len ) - cur_len;
          delta2   = cur_pos2 + ( cur_len >> 1 ) - org_center;
          if ( delta2 < 0 )
            delta2 = -delta2;

          edge->pos  = ( delta1 < delta2 ) ? cur_pos1 : cur_pos2;
          edge2->pos = edge->pos + cur_len;

          FT_TRACE5(( "  STEM: edge %d (opos=%.2f) linked to %d (opos=%.2f)"
                      " snapped to %.2f and %.2f\n",
                      edge - edges, edge->opos / 64.0,
                      edge2 - edges, edge2->opos / 64.0,
                      edge->pos / 64.0, edge2->pos / 64.0 ));
        }

#ifdef FT_DEBUG_LEVEL_TRACE
        num_actions++;
#endif

        edge->flags  |= AF_EDGE_DONE;
        edge2->flags |= AF_EDGE_DONE;

        if ( edge > edges && edge->pos < edge[-1].pos )
        {
#ifdef FT_DEBUG_LEVEL_TRACE
          FT_TRACE5(( "  BOUND: edge %d (pos=%.2f) moved to %.2f\n",
                      edge - edges, edge->pos / 64.0, edge[-1].pos / 64.0 ));

          num_actions++;
#endif

          edge->pos = edge[-1].pos;
        }
      }
    }

    /* make sure that lowercase m's maintain their symmetry */

    /* In general, lowercase m's have six vertical edges if they are sans */
    /* serif, or twelve if they are with serifs.  This implementation is  */
    /* based on that assumption, and seems to work very well with most    */
    /* faces.  However, if for a certain face this assumption is not      */
    /* true, the m is just rendered like before.  In addition, any stem   */
    /* correction will only be applied to symmetrical glyphs (even if the */
    /* glyph is not an m), so the potential for unwanted distortion is    */
    /* relatively low.                                                    */

    /* We don't handle horizontal edges since we can't easily assure that */
    /* the third (lowest) stem aligns with the base line; it might end up */
    /* one pixel higher or lower.                                         */

    n_edges = edge_limit - edges;
    if ( dim == AF_DIMENSION_HORZ && ( n_edges == 6 || n_edges == 12 ) )
    {
      AF_Edge  edge1, edge2, edge3;
      FT_Pos   dist1, dist2, span, delta;


      if ( n_edges == 6 )
      {
        edge1 = edges;
        edge2 = edges + 2;
        edge3 = edges + 4;
      }
      else
      {
        edge1 = edges + 1;
        edge2 = edges + 5;
        edge3 = edges + 9;
      }

      dist1 = edge2->opos - edge1->opos;
      dist2 = edge3->opos - edge2->opos;

      span = dist1 - dist2;
      if ( span < 0 )
        span = -span;

      if ( span < 8 )
      {
        delta = edge3->pos - ( 2 * edge2->pos - edge1->pos );
        edge3->pos -= delta;
        if ( edge3->link )
          edge3->link->pos -= delta;

        /* move the serifs along with the stem */
        if ( n_edges == 12 )
        {
          ( edges + 8 )->pos -= delta;
          ( edges + 11 )->pos -= delta;
        }

        edge3->flags |= AF_EDGE_DONE;
        if ( edge3->link )
          edge3->link->flags |= AF_EDGE_DONE;
      }
    }

    if ( has_serifs || !anchor )
    {
      /*
       *  now hint the remaining edges (serifs and single) in order
       *  to complete our processing
       */
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        FT_Pos  delta;


        if ( edge->flags & AF_EDGE_DONE )
          continue;

        delta = 1000;

        if ( edge->serif )
        {
          delta = edge->serif->opos - edge->opos;
          if ( delta < 0 )
            delta = -delta;
        }

        if ( delta < 64 + 16 )
        {
          af_latin_align_serif_edge( hints, edge->serif, edge );
          FT_TRACE5(( "  SERIF: edge %d (opos=%.2f) serif to %d (opos=%.2f)"
                      " aligned to %.2f\n",
                      edge - edges, edge->opos / 64.0,
                      edge->serif - edges, edge->serif->opos / 64.0,
                      edge->pos / 64.0 ));
        }
        else if ( !anchor )
        {
          edge->pos = FT_PIX_ROUND( edge->opos );
          anchor    = edge;
          FT_TRACE5(( "  SERIF_ANCHOR: edge %d (opos=%.2f)"
                      " snapped to %.2f\n",
                      edge-edges, edge->opos / 64.0, edge->pos / 64.0 ));
        }
        else
        {
          AF_Edge  before, after;


          for ( before = edge - 1; before >= edges; before-- )
            if ( before->flags & AF_EDGE_DONE )
              break;

          for ( after = edge + 1; after < edge_limit; after++ )
            if ( after->flags & AF_EDGE_DONE )
              break;

          if ( before >= edges && before < edge   &&
               after < edge_limit && after > edge )
          {
            if ( after->opos == before->opos )
              edge->pos = before->pos;
            else
              edge->pos = before->pos +
                          FT_MulDiv( edge->opos - before->opos,
                                     after->pos - before->pos,
                                     after->opos - before->opos );

            FT_TRACE5(( "  SERIF_LINK1: edge %d (opos=%.2f) snapped to %.2f"
                        " from %d (opos=%.2f)\n",
                        edge - edges, edge->opos / 64.0,
                        edge->pos / 64.0,
                        before - edges, before->opos / 64.0 ));
          }
          else
          {
            edge->pos = anchor->pos +
                        ( ( edge->opos - anchor->opos + 16 ) & ~31 );
            FT_TRACE5(( "  SERIF_LINK2: edge %d (opos=%.2f)"
                        " snapped to %.2f\n",
                        edge - edges, edge->opos / 64.0, edge->pos / 64.0 ));
          }
        }

#ifdef FT_DEBUG_LEVEL_TRACE
        num_actions++;
#endif
        edge->flags |= AF_EDGE_DONE;

        if ( edge > edges && edge->pos < edge[-1].pos )
        {
#ifdef FT_DEBUG_LEVEL_TRACE
          FT_TRACE5(( "  BOUND: edge %d (pos=%.2f) moved to %.2f\n",
                      edge - edges, edge->pos / 64.0, edge[-1].pos / 64.0 ));

          num_actions++;
#endif
          edge->pos = edge[-1].pos;
        }

        if ( edge + 1 < edge_limit        &&
             edge[1].flags & AF_EDGE_DONE &&
             edge->pos > edge[1].pos      )
        {
#ifdef FT_DEBUG_LEVEL_TRACE
          FT_TRACE5(( "  BOUND: edge %d (pos=%.2f) moved to %.2f\n",
                      edge - edges, edge->pos / 64.0, edge[1].pos / 64.0 ));

          num_actions++;
#endif

          edge->pos = edge[1].pos;
        }
      }
    }

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( !num_actions )
      FT_TRACE5(( "  (none)\n" ));
    FT_TRACE5(( "\n" ));
#endif
  }


  /* Apply the complete hinting algorithm to a latin glyph. */

  static FT_Error
  af_latin_hints_apply( AF_GlyphHints    hints,
                        FT_Outline*      outline,
                        AF_LatinMetrics  metrics )
  {
    FT_Error  error;
    int       dim;


    error = af_glyph_hints_reload( hints, outline );
    if ( error )
      goto Exit;

    /* analyze glyph outline */
#ifdef AF_CONFIG_OPTION_USE_WARPER
    if ( metrics->root.scaler.render_mode == FT_RENDER_MODE_LIGHT ||
         AF_HINTS_DO_HORIZONTAL( hints )                          )
#else
    if ( AF_HINTS_DO_HORIZONTAL( hints ) )
#endif
    {
      error = af_latin_hints_detect_features( hints, AF_DIMENSION_HORZ );
      if ( error )
        goto Exit;
    }

    if ( AF_HINTS_DO_VERTICAL( hints ) )
    {
      error = af_latin_hints_detect_features( hints, AF_DIMENSION_VERT );
      if ( error )
        goto Exit;

      af_latin_hints_compute_blue_edges( hints, metrics );
    }

    /* grid-fit the outline */
    for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
    {
#ifdef AF_CONFIG_OPTION_USE_WARPER
      if ( dim == AF_DIMENSION_HORZ                                 &&
           metrics->root.scaler.render_mode == FT_RENDER_MODE_LIGHT )
      {
        AF_WarperRec  warper;
        FT_Fixed      scale;
        FT_Pos        delta;


        af_warper_compute( &warper, hints, (AF_Dimension)dim,
                           &scale, &delta );
        af_glyph_hints_scale_dim( hints, (AF_Dimension)dim,
                                  scale, delta );
        continue;
      }
#endif

      if ( ( dim == AF_DIMENSION_HORZ && AF_HINTS_DO_HORIZONTAL( hints ) ) ||
           ( dim == AF_DIMENSION_VERT && AF_HINTS_DO_VERTICAL( hints ) )   )
      {
        af_latin_hint_edges( hints, (AF_Dimension)dim );
        af_glyph_hints_align_edge_points( hints, (AF_Dimension)dim );
        af_glyph_hints_align_strong_points( hints, (AF_Dimension)dim );
        af_glyph_hints_align_weak_points( hints, (AF_Dimension)dim );
      }
    }

    af_glyph_hints_save( hints, outline );

  Exit:
    return error;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****              L A T I N   S C R I P T   C L A S S              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_latin_writing_system_class,

    AF_WRITING_SYSTEM_LATIN,

    sizeof ( AF_LatinMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) af_latin_metrics_init,
    (AF_WritingSystem_ScaleMetricsFunc)af_latin_metrics_scale,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   af_latin_hints_init,
    (AF_WritingSystem_ApplyHintsFunc)  af_latin_hints_apply
  )


/* END */
#ifdef FT_OPTION_AUTOFIT2
/***************************************************************************/
/*                                                                         */
/*  aflatin2.c                                                             */
/*                                                                         */
/*    Auto-fitter hinting routines for latin writing system (body).        */
/*                                                                         */
/*  Copyright 2003-2013 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include FT_ADVANCES_H

#include "afglobal.h"
#include "aflatin.h"
#include "aflatin2.h"
#include "aferrors.h"


#ifdef AF_CONFIG_OPTION_USE_WARPER
#include "afwarp.h"
#endif


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_aflatin2


  FT_LOCAL_DEF( FT_Error )
  af_latin2_hints_compute_segments( AF_GlyphHints  hints,
                                    AF_Dimension   dim );

  FT_LOCAL_DEF( void )
  af_latin2_hints_link_segments( AF_GlyphHints  hints,
                                 AF_Dimension   dim );

  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****            L A T I N   G L O B A L   M E T R I C S            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  af_latin2_metrics_init_widths( AF_LatinMetrics  metrics,
                                 FT_Face          face )
  {
    /* scan the array of segments in each direction */
    AF_GlyphHintsRec  hints[1];


    af_glyph_hints_init( hints, face->memory );

    metrics->axis[AF_DIMENSION_HORZ].width_count = 0;
    metrics->axis[AF_DIMENSION_VERT].width_count = 0;

    {
      FT_Error             error;
      FT_UInt              glyph_index;
      int                  dim;
      AF_LatinMetricsRec   dummy[1];
      AF_Scaler            scaler = &dummy->root.scaler;


      glyph_index = FT_Get_Char_Index(
                      face,
                      metrics->root.style_class->standard_char );
      if ( glyph_index == 0 )
        goto Exit;

      error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
      if ( error || face->glyph->outline.n_points <= 0 )
        goto Exit;

      FT_ZERO( dummy );

      dummy->units_per_em = metrics->units_per_em;
      scaler->x_scale     = scaler->y_scale = 0x10000L;
      scaler->x_delta     = scaler->y_delta = 0;
      scaler->face        = face;
      scaler->render_mode = FT_RENDER_MODE_NORMAL;
      scaler->flags       = 0;

      af_glyph_hints_rescale( hints, (AF_StyleMetrics)dummy );

      error = af_glyph_hints_reload( hints, &face->glyph->outline );
      if ( error )
        goto Exit;

      for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
      {
        AF_LatinAxis  axis    = &metrics->axis[dim];
        AF_AxisHints  axhints = &hints->axis[dim];
        AF_Segment    seg, limit, link;
        FT_UInt       num_widths = 0;


        error = af_latin2_hints_compute_segments( hints,
                                                 (AF_Dimension)dim );
        if ( error )
          goto Exit;

        af_latin2_hints_link_segments( hints,
                                      (AF_Dimension)dim );

        seg   = axhints->segments;
        limit = seg + axhints->num_segments;

        for ( ; seg < limit; seg++ )
        {
          link = seg->link;

          /* we only consider stem segments there! */
          if ( link && link->link == seg && link > seg )
          {
            FT_Pos  dist;


            dist = seg->pos - link->pos;
            if ( dist < 0 )
              dist = -dist;

            if ( num_widths < AF_LATIN_MAX_WIDTHS )
              axis->widths[num_widths++].org = dist;
          }
        }

        af_sort_widths( num_widths, axis->widths );
        axis->width_count = num_widths;
      }

  Exit:
      for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
      {
        AF_LatinAxis  axis = &metrics->axis[dim];
        FT_Pos        stdw;


        stdw = ( axis->width_count > 0 )
                 ? axis->widths[0].org
                 : AF_LATIN_CONSTANT( metrics, 50 );

        /* let's try 20% of the smallest width */
        axis->edge_distance_threshold = stdw / 5;
        axis->standard_width          = stdw;
        axis->extra_light             = 0;
      }
    }

    af_glyph_hints_done( hints );
  }



#define AF_LATIN_MAX_TEST_CHARACTERS  12


  static const char af_latin2_blue_chars[AF_LATIN_MAX_BLUES]
                                        [AF_LATIN_MAX_TEST_CHARACTERS+1] =
  {
    "THEZOCQS",
    "HEZLOCUS",
    "fijkdbh",
    "xzroesc",
    "xzroesc",
    "pqgjy"
  };


  static void
  af_latin2_metrics_init_blues( AF_LatinMetrics  metrics,
                                FT_Face          face )
  {
    FT_Pos        flats [AF_LATIN_MAX_TEST_CHARACTERS];
    FT_Pos        rounds[AF_LATIN_MAX_TEST_CHARACTERS];
    FT_Int        num_flats;
    FT_Int        num_rounds;
    FT_Int        bb;
    AF_LatinBlue  blue;
    FT_Error      error;
    AF_LatinAxis  axis  = &metrics->axis[AF_DIMENSION_VERT];
    FT_GlyphSlot  glyph = face->glyph;


    /* we compute the blues simply by loading each character from the     */
    /* 'af_latin2_blue_chars[blues]' string, then compute its top-most or */
    /* bottom-most points (depending on `AF_IS_TOP_BLUE')                 */

    FT_TRACE5(( "blue zones computation\n"
                "======================\n\n" ));

    for ( bb = 0; bb < AF_LATIN_BLUE_MAX; bb++ )
    {
      const char*  p     = af_latin2_blue_chars[bb];
      const char*  limit = p + AF_LATIN_MAX_TEST_CHARACTERS;
      FT_Pos*      blue_ref;
      FT_Pos*      blue_shoot;


      FT_TRACE5(( "blue zone %d:\n", bb ));

      num_flats  = 0;
      num_rounds = 0;

      for ( ; p < limit && *p; p++ )
      {
        FT_UInt     glyph_index;
        FT_Int      best_point, best_y, best_first, best_last;
        FT_Vector*  points;
        FT_Bool     round;


        /* load the character in the face -- skip unknown or empty ones */
        glyph_index = FT_Get_Char_Index( face, (FT_UInt)*p );
        if ( glyph_index == 0 )
          continue;

        error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
        if ( error || glyph->outline.n_points <= 0 )
          continue;

        /* now compute min or max point indices and coordinates */
        points      = glyph->outline.points;
        best_point  = -1;
        best_y      = 0;  /* make compiler happy */
        best_first  = 0;  /* ditto */
        best_last   = 0;  /* ditto */

        {
          FT_Int  nn;
          FT_Int  first = 0;
          FT_Int  last  = -1;


          for ( nn = 0; nn < glyph->outline.n_contours; first = last+1, nn++ )
          {
            FT_Int  old_best_point = best_point;
            FT_Int  pp;


            last = glyph->outline.contours[nn];

            /* Avoid single-point contours since they are never rasterized. */
            /* In some fonts, they correspond to mark attachment points     */
            /* which are way outside of the glyph's real outline.           */
            if ( last == first )
                continue;

            if ( AF_LATIN_IS_TOP_BLUE( bb ) )
            {
              for ( pp = first; pp <= last; pp++ )
                if ( best_point < 0 || points[pp].y > best_y )
                {
                  best_point = pp;
                  best_y     = points[pp].y;
                }
            }
            else
            {
              for ( pp = first; pp <= last; pp++ )
                if ( best_point < 0 || points[pp].y < best_y )
                {
                  best_point = pp;
                  best_y     = points[pp].y;
                }
            }

            if ( best_point != old_best_point )
            {
              best_first = first;
              best_last  = last;
            }
          }
          FT_TRACE5(( "  %c  %d", *p, best_y ));
        }

        /* now check whether the point belongs to a straight or round   */
        /* segment; we first need to find in which contour the extremum */
        /* lies, then inspect its previous and next points              */
        {
          FT_Pos  best_x = points[best_point].x;
          FT_Int  start, end, prev, next;
          FT_Pos  dist;


          /* now look for the previous and next points that are not on the */
          /* same Y coordinate.  Threshold the `closeness'...              */
          start = end = best_point;

          do
          {
            prev = start - 1;
            if ( prev < best_first )
              prev = best_last;

            dist = FT_ABS( points[prev].y - best_y );
            /* accept a small distance or a small angle (both values are */
            /* heuristic; value 20 corresponds to approx. 2.9 degrees)   */
            if ( dist > 5 )
              if ( FT_ABS( points[prev].x - best_x ) <= 20 * dist )
                break;

            start = prev;

          } while ( start != best_point );

          do
          {
            next = end + 1;
            if ( next > best_last )
              next = best_first;

            dist = FT_ABS( points[next].y - best_y );
            if ( dist > 5 )
              if ( FT_ABS( points[next].x - best_x ) <= 20 * dist )
                break;

            end = next;

          } while ( end != best_point );

          /* now, set the `round' flag depending on the segment's kind */
          round = FT_BOOL(
            FT_CURVE_TAG( glyph->outline.tags[start] ) != FT_CURVE_TAG_ON ||
            FT_CURVE_TAG( glyph->outline.tags[ end ] ) != FT_CURVE_TAG_ON );

          FT_TRACE5(( " (%s)\n", round ? "round" : "flat" ));
        }

        if ( round )
          rounds[num_rounds++] = best_y;
        else
          flats[num_flats++]   = best_y;
      }

      if ( num_flats == 0 && num_rounds == 0 )
      {
        /*
         *  we couldn't find a single glyph to compute this blue zone,
         *  we will simply ignore it then
         */
        FT_TRACE5(( "  empty\n" ));
        continue;
      }

      /* we have computed the contents of the `rounds' and `flats' tables, */
      /* now determine the reference and overshoot position of the blue -- */
      /* we simply take the median value after a simple sort               */
      af_sort_pos( num_rounds, rounds );
      af_sort_pos( num_flats,  flats );

      blue       = & axis->blues[axis->blue_count];
      blue_ref   = & blue->ref.org;
      blue_shoot = & blue->shoot.org;

      axis->blue_count++;

      if ( num_flats == 0 )
      {
        *blue_ref   =
        *blue_shoot = rounds[num_rounds / 2];
      }
      else if ( num_rounds == 0 )
      {
        *blue_ref   =
        *blue_shoot = flats[num_flats / 2];
      }
      else
      {
        *blue_ref   = flats[num_flats / 2];
        *blue_shoot = rounds[num_rounds / 2];
      }

      /* there are sometimes problems: if the overshoot position of top     */
      /* zones is under its reference position, or the opposite for bottom  */
      /* zones.  We must thus check everything there and correct the errors */
      if ( *blue_shoot != *blue_ref )
      {
        FT_Pos   ref      = *blue_ref;
        FT_Pos   shoot    = *blue_shoot;
        FT_Bool  over_ref = FT_BOOL( shoot > ref );


        if ( AF_LATIN_IS_TOP_BLUE( bb ) ^ over_ref )
        {
          *blue_ref   =
          *blue_shoot = ( shoot + ref ) / 2;

          FT_TRACE5(( "  [overshoot smaller than reference,"
                      " taking mean value]\n" ));
        }
      }

      blue->flags = 0;
      if ( AF_LATIN_IS_TOP_BLUE( bb ) )
        blue->flags |= AF_LATIN_BLUE_TOP;

      /*
       * The following flag is used later to adjust the y and x scales
       * in order to optimize the pixel grid alignment of the top of small
       * letters.
       */
      if ( AF_LATIN_IS_X_HEIGHT_BLUE( bb ) )
        blue->flags |= AF_LATIN_BLUE_ADJUSTMENT;

      FT_TRACE5(( "    -> reference = %ld\n"
                  "       overshoot = %ld\n",
                  *blue_ref, *blue_shoot ));
    }

    return;
  }


  FT_LOCAL_DEF( void )
  af_latin2_metrics_check_digits( AF_LatinMetrics  metrics,
                                  FT_Face          face )
  {
    FT_UInt   i;
    FT_Bool   started = 0, same_width = 1;
    FT_Fixed  advance, old_advance = 0;


    /* check whether all ASCII digits have the same advance width; */
    /* digit `0' is 0x30 in all supported charmaps                 */
    for ( i = 0x30; i <= 0x39; i++ )
    {
      FT_UInt  glyph_index;


      glyph_index = FT_Get_Char_Index( face, i );
      if ( glyph_index == 0 )
        continue;

      if ( FT_Get_Advance( face, glyph_index,
                           FT_LOAD_NO_SCALE         |
                           FT_LOAD_NO_HINTING       |
                           FT_LOAD_IGNORE_TRANSFORM,
                           &advance ) )
        continue;

      if ( started )
      {
        if ( advance != old_advance )
        {
          same_width = 0;
          break;
        }
      }
      else
      {
        old_advance = advance;
        started     = 1;
      }
    }

    metrics->root.digits_have_same_width = same_width;
  }


  FT_LOCAL_DEF( FT_Error )
  af_latin2_metrics_init( AF_LatinMetrics  metrics,
                          FT_Face          face )
  {
    FT_Error    error  = FT_Err_Ok;
    FT_CharMap  oldmap = face->charmap;
    FT_UInt     ee;

    static const FT_Encoding  latin_encodings[] =
    {
      FT_ENCODING_UNICODE,
      FT_ENCODING_APPLE_ROMAN,
      FT_ENCODING_ADOBE_STANDARD,
      FT_ENCODING_ADOBE_LATIN_1,
      FT_ENCODING_NONE  /* end of list */
    };


    metrics->units_per_em = face->units_per_EM;

    /* do we have a latin charmap in there? */
    for ( ee = 0; latin_encodings[ee] != FT_ENCODING_NONE; ee++ )
    {
      error = FT_Select_Charmap( face, latin_encodings[ee] );
      if ( !error )
        break;
    }

    if ( !error )
    {
      af_latin2_metrics_init_widths( metrics, face );
      af_latin2_metrics_init_blues( metrics, face );
      af_latin2_metrics_check_digits( metrics, face );
    }

    FT_Set_Charmap( face, oldmap );
    return FT_Err_Ok;
  }


  static void
  af_latin2_metrics_scale_dim( AF_LatinMetrics  metrics,
                               AF_Scaler        scaler,
                               AF_Dimension     dim )
  {
    FT_Fixed      scale;
    FT_Pos        delta;
    AF_LatinAxis  axis;
    FT_UInt       nn;


    if ( dim == AF_DIMENSION_HORZ )
    {
      scale = scaler->x_scale;
      delta = scaler->x_delta;
    }
    else
    {
      scale = scaler->y_scale;
      delta = scaler->y_delta;
    }

    axis = &metrics->axis[dim];

    if ( axis->org_scale == scale && axis->org_delta == delta )
      return;

    axis->org_scale = scale;
    axis->org_delta = delta;

    /*
     * correct Y scale to optimize the alignment of the top of small
     * letters to the pixel grid
     */
    if ( dim == AF_DIMENSION_VERT )
    {
      AF_LatinAxis  vaxis = &metrics->axis[AF_DIMENSION_VERT];
      AF_LatinBlue  blue = NULL;


      for ( nn = 0; nn < vaxis->blue_count; nn++ )
      {
        if ( vaxis->blues[nn].flags & AF_LATIN_BLUE_ADJUSTMENT )
        {
          blue = &vaxis->blues[nn];
          break;
        }
      }

      if ( blue )
      {
        FT_Pos   scaled;
        FT_Pos   threshold;
        FT_Pos   fitted;
        FT_UInt  limit;
        FT_UInt  ppem;


        scaled    = FT_MulFix( blue->shoot.org, scaler->y_scale );
        ppem      = metrics->root.scaler.face->size->metrics.x_ppem;
        limit     = metrics->root.globals->increase_x_height;
        threshold = 40;

        /* if the `increase-x-height' property is active, */
        /* we round up much more often                    */
        if ( limit                                 &&
             ppem <= limit                         &&
             ppem >= AF_PROP_INCREASE_X_HEIGHT_MIN )
          threshold = 52;

        fitted = ( scaled + threshold ) & ~63;

#if 1
        if ( scaled != fitted )
        {
          scale = FT_MulDiv( scale, fitted, scaled );
          FT_TRACE5(( "== scaled x-top = %.2g"
                      "  fitted = %.2g, scaling = %.4g\n",
                      scaled / 64.0, fitted / 64.0,
                      ( fitted * 1.0 ) / scaled ));
        }
#endif
      }
    }

    axis->scale = scale;
    axis->delta = delta;

    if ( dim == AF_DIMENSION_HORZ )
    {
      metrics->root.scaler.x_scale = scale;
      metrics->root.scaler.x_delta = delta;
    }
    else
    {
      metrics->root.scaler.y_scale = scale;
      metrics->root.scaler.y_delta = delta;
    }

    /* scale the standard widths */
    for ( nn = 0; nn < axis->width_count; nn++ )
    {
      AF_Width  width = axis->widths + nn;


      width->cur = FT_MulFix( width->org, scale );
      width->fit = width->cur;
    }

    /* an extra-light axis corresponds to a standard width that is */
    /* smaller than 5/8 pixels                                     */
    axis->extra_light =
      (FT_Bool)( FT_MulFix( axis->standard_width, scale ) < 32 + 8 );

    if ( dim == AF_DIMENSION_VERT )
    {
      /* scale the blue zones */
      for ( nn = 0; nn < axis->blue_count; nn++ )
      {
        AF_LatinBlue  blue = &axis->blues[nn];
        FT_Pos        dist;


        blue->ref.cur   = FT_MulFix( blue->ref.org, scale ) + delta;
        blue->ref.fit   = blue->ref.cur;
        blue->shoot.cur = FT_MulFix( blue->shoot.org, scale ) + delta;
        blue->shoot.fit = blue->shoot.cur;
        blue->flags    &= ~AF_LATIN_BLUE_ACTIVE;

        /* a blue zone is only active if it is less than 3/4 pixels tall */
        dist = FT_MulFix( blue->ref.org - blue->shoot.org, scale );
        if ( dist <= 48 && dist >= -48 )
        {
          FT_Pos  delta1, delta2;

          delta1 = blue->shoot.org - blue->ref.org;
          delta2 = delta1;
          if ( delta1 < 0 )
            delta2 = -delta2;

          delta2 = FT_MulFix( delta2, scale );

          if ( delta2 < 32 )
            delta2 = 0;
          else if ( delta2 < 64 )
            delta2 = 32 + ( ( ( delta2 - 32 ) + 16 ) & ~31 );
          else
            delta2 = FT_PIX_ROUND( delta2 );

          if ( delta1 < 0 )
            delta2 = -delta2;

          blue->ref.fit   = FT_PIX_ROUND( blue->ref.cur );
          blue->shoot.fit = blue->ref.fit + delta2;

          FT_TRACE5(( ">> activating blue zone %d:"
                      "  ref.cur=%.2g ref.fit=%.2g"
                      "  shoot.cur=%.2g shoot.fit=%.2g\n",
                      nn, blue->ref.cur / 64.0, blue->ref.fit / 64.0,
                      blue->shoot.cur / 64.0, blue->shoot.fit / 64.0 ));

          blue->flags |= AF_LATIN_BLUE_ACTIVE;
        }
      }
    }
  }


  FT_LOCAL_DEF( void )
  af_latin2_metrics_scale( AF_LatinMetrics  metrics,
                           AF_Scaler        scaler )
  {
    metrics->root.scaler.render_mode = scaler->render_mode;
    metrics->root.scaler.face        = scaler->face;
    metrics->root.scaler.flags       = scaler->flags;

    af_latin2_metrics_scale_dim( metrics, scaler, AF_DIMENSION_HORZ );
    af_latin2_metrics_scale_dim( metrics, scaler, AF_DIMENSION_VERT );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****           L A T I N   G L Y P H   A N A L Y S I S             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define  SORT_SEGMENTS

  FT_LOCAL_DEF( FT_Error )
  af_latin2_hints_compute_segments( AF_GlyphHints  hints,
                                    AF_Dimension   dim )
  {
    AF_AxisHints  axis          = &hints->axis[dim];
    FT_Memory     memory        = hints->memory;
    FT_Error      error         = FT_Err_Ok;
    AF_Segment    segment       = NULL;
    AF_SegmentRec seg0;
    AF_Point*     contour       = hints->contours;
    AF_Point*     contour_limit = contour + hints->num_contours;
    AF_Direction  major_dir, segment_dir;


    FT_ZERO( &seg0 );
    seg0.score = 32000;
    seg0.flags = AF_EDGE_NORMAL;

    major_dir   = (AF_Direction)FT_ABS( axis->major_dir );
    segment_dir = major_dir;

    axis->num_segments = 0;

    /* set up (u,v) in each point */
    if ( dim == AF_DIMENSION_HORZ )
    {
      AF_Point  point = hints->points;
      AF_Point  limit = point + hints->num_points;


      for ( ; point < limit; point++ )
      {
        point->u = point->fx;
        point->v = point->fy;
      }
    }
    else
    {
      AF_Point  point = hints->points;
      AF_Point  limit = point + hints->num_points;


      for ( ; point < limit; point++ )
      {
        point->u = point->fy;
        point->v = point->fx;
      }
    }

    /* do each contour separately */
    for ( ; contour < contour_limit; contour++ )
    {
      AF_Point  point   =  contour[0];
      AF_Point  start   =  point;
      AF_Point  last    =  point->prev;


      if ( point == last )  /* skip singletons -- just in case */
        continue;

      /* already on an edge ?, backtrack to find its start */
      if ( FT_ABS( point->in_dir ) == major_dir )
      {
        point = point->prev;

        while ( point->in_dir == start->in_dir )
          point = point->prev;
      }
      else  /* otherwise, find first segment start, if any */
      {
        while ( FT_ABS( point->out_dir ) != major_dir )
        {
          point = point->next;

          if ( point == start )
            goto NextContour;
        }
      }

      start = point;

      for  (;;)
      {
        AF_Point  first;
        FT_Pos    min_u, min_v, max_u, max_v;

        /* we're at the start of a new segment */
        FT_ASSERT( FT_ABS( point->out_dir ) == major_dir &&
                           point->in_dir != point->out_dir );
        first = point;

        min_u = max_u = point->u;
        min_v = max_v = point->v;

        point = point->next;

        while ( point->out_dir == first->out_dir )
        {
          point = point->next;

          if ( point->u < min_u )
            min_u = point->u;

          if ( point->u > max_u )
            max_u = point->u;
        }

        if ( point->v < min_v )
          min_v = point->v;

        if ( point->v > max_v )
          max_v = point->v;

        /* record new segment */
        error = af_axis_hints_new_segment( axis, memory, &segment );
        if ( error )
          goto Exit;

        segment[0]         = seg0;
        segment->dir       = first->out_dir;
        segment->first     = first;
        segment->last      = point;
        segment->pos       = (FT_Short)( ( min_u + max_u ) >> 1 );
        segment->min_coord = (FT_Short) min_v;
        segment->max_coord = (FT_Short) max_v;
        segment->height    = (FT_Short)( max_v - min_v );

        /* a segment is round if it doesn't have successive */
        /* on-curve points.                                 */
        {
          AF_Point  pt   = first;
          AF_Point  last = point;
          AF_Flags  f0   = (AF_Flags)( pt->flags & AF_FLAG_CONTROL );
          AF_Flags  f1;


          segment->flags &= ~AF_EDGE_ROUND;

          for ( ; pt != last; f0 = f1 )
          {
            pt = pt->next;
            f1 = (AF_Flags)( pt->flags & AF_FLAG_CONTROL );

            if ( !f0 && !f1 )
              break;

            if ( pt == last )
              segment->flags |= AF_EDGE_ROUND;
          }
        }

       /* this can happen in the case of a degenerate contour
        * e.g. a 2-point vertical contour
        */
        if ( point == start )
          break;

        /* jump to the start of the next segment, if any */
        while ( FT_ABS( point->out_dir ) != major_dir )
        {
          point = point->next;

          if ( point == start )
            goto NextContour;
        }
      }

    NextContour:
      ;
    } /* contours */

    /* now slightly increase the height of segments when this makes */
    /* sense -- this is used to better detect and ignore serifs     */
    {
      AF_Segment  segments     = axis->segments;
      AF_Segment  segments_end = segments + axis->num_segments;


      for ( segment = segments; segment < segments_end; segment++ )
      {
        AF_Point  first   = segment->first;
        AF_Point  last    = segment->last;
        AF_Point  p;
        FT_Pos    first_v = first->v;
        FT_Pos    last_v  = last->v;


        if ( first == last )
          continue;

        if ( first_v < last_v )
        {
          p = first->prev;
          if ( p->v < first_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( first_v - p->v ) >> 1 ) );

          p = last->next;
          if ( p->v > last_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( p->v - last_v ) >> 1 ) );
        }
        else
        {
          p = first->prev;
          if ( p->v > first_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( p->v - first_v ) >> 1 ) );

          p = last->next;
          if ( p->v < last_v )
            segment->height = (FT_Short)( segment->height +
                                          ( ( last_v - p->v ) >> 1 ) );
        }
      }
    }

#ifdef AF_SORT_SEGMENTS
   /* place all segments with a negative direction to the start
    * of the array, used to speed up segment linking later...
    */
    {
      AF_Segment  segments = axis->segments;
      FT_UInt     count    = axis->num_segments;
      FT_UInt     ii, jj;

      for ( ii = 0; ii < count; ii++ )
      {
        if ( segments[ii].dir > 0 )
        {
          for ( jj = ii + 1; jj < count; jj++ )
          {
            if ( segments[jj].dir < 0 )
            {
              AF_SegmentRec  tmp;


              tmp          = segments[ii];
              segments[ii] = segments[jj];
              segments[jj] = tmp;

              break;
            }
          }

          if ( jj == count )
            break;
        }
      }
      axis->mid_segments = ii;
    }
#endif

  Exit:
    return error;
  }


  FT_LOCAL_DEF( void )
  af_latin2_hints_link_segments( AF_GlyphHints  hints,
                                 AF_Dimension   dim )
  {
    AF_AxisHints  axis          = &hints->axis[dim];
    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
#ifdef AF_SORT_SEGMENTS
    AF_Segment    segment_mid   = segments + axis->mid_segments;
#endif
    FT_Pos        len_threshold, len_score;
    AF_Segment    seg1, seg2;


    len_threshold = AF_LATIN_CONSTANT( hints->metrics, 8 );
    if ( len_threshold == 0 )
      len_threshold = 1;

    len_score = AF_LATIN_CONSTANT( hints->metrics, 6000 );

#ifdef AF_SORT_SEGMENTS
    for ( seg1 = segments; seg1 < segment_mid; seg1++ )
    {
      if ( seg1->dir != axis->major_dir || seg1->first == seg1->last )
        continue;

      for ( seg2 = segment_mid; seg2 < segment_limit; seg2++ )
#else
    /* now compare each segment to the others */
    for ( seg1 = segments; seg1 < segment_limit; seg1++ )
    {
      /* the fake segments are introduced to hint the metrics -- */
      /* we must never link them to anything                     */
      if ( seg1->dir != axis->major_dir || seg1->first == seg1->last )
        continue;

      for ( seg2 = segments; seg2 < segment_limit; seg2++ )
        if ( seg1->dir + seg2->dir == 0 && seg2->pos > seg1->pos )
#endif
        {
          FT_Pos  pos1 = seg1->pos;
          FT_Pos  pos2 = seg2->pos;
          FT_Pos  dist = pos2 - pos1;


          if ( dist < 0 )
            continue;

          {
            FT_Pos  min = seg1->min_coord;
            FT_Pos  max = seg1->max_coord;
            FT_Pos  len, score;


            if ( min < seg2->min_coord )
              min = seg2->min_coord;

            if ( max > seg2->max_coord )
              max = seg2->max_coord;

            len = max - min;
            if ( len >= len_threshold )
            {
              score = dist + len_score / len;
              if ( score < seg1->score )
              {
                seg1->score = score;
                seg1->link  = seg2;
              }

              if ( score < seg2->score )
              {
                seg2->score = score;
                seg2->link  = seg1;
              }
            }
          }
        }
    }
#if 0
    }
#endif

    /* now, compute the `serif' segments */
    for ( seg1 = segments; seg1 < segment_limit; seg1++ )
    {
      seg2 = seg1->link;

      if ( seg2 )
      {
        if ( seg2->link != seg1 )
        {
          seg1->link  = 0;
          seg1->serif = seg2->link;
        }
      }
    }
  }


  FT_LOCAL_DEF( FT_Error )
  af_latin2_hints_compute_edges( AF_GlyphHints  hints,
                                 AF_Dimension   dim )
  {
    AF_AxisHints  axis   = &hints->axis[dim];
    FT_Error      error  = FT_Err_Ok;
    FT_Memory     memory = hints->memory;
    AF_LatinAxis  laxis  = &((AF_LatinMetrics)hints->metrics)->axis[dim];

    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    AF_Segment    seg;

    AF_Direction  up_dir;
    FT_Fixed      scale;
    FT_Pos        edge_distance_threshold;
    FT_Pos        segment_length_threshold;


    axis->num_edges = 0;

    scale = ( dim == AF_DIMENSION_HORZ ) ? hints->x_scale
                                         : hints->y_scale;

    up_dir = ( dim == AF_DIMENSION_HORZ ) ? AF_DIR_UP
                                          : AF_DIR_RIGHT;

    /*
     *  We want to ignore very small (mostly serif) segments, we do that
     *  by ignoring those that whose length is less than a given fraction
     *  of the standard width. If there is no standard width, we ignore
     *  those that are less than a given size in pixels
     *
     *  also, unlink serif segments that are linked to segments farther
     *  than 50% of the standard width
     */
    if ( dim == AF_DIMENSION_HORZ )
    {
      if ( laxis->width_count > 0 )
        segment_length_threshold = ( laxis->standard_width * 10 ) >> 4;
      else
        segment_length_threshold = FT_DivFix( 64, hints->y_scale );
    }
    else
      segment_length_threshold = 0;

    /*********************************************************************/
    /*                                                                   */
    /* We will begin by generating a sorted table of edges for the       */
    /* current direction.  To do so, we simply scan each segment and try */
    /* to find an edge in our table that corresponds to its position.    */
    /*                                                                   */
    /* If no edge is found, we create and insert a new edge in the       */
    /* sorted table.  Otherwise, we simply add the segment to the edge's */
    /* list which will be processed in the second step to compute the    */
    /* edge's properties.                                                */
    /*                                                                   */
    /* Note that the edges table is sorted along the segment/edge        */
    /* position.                                                         */
    /*                                                                   */
    /*********************************************************************/

    edge_distance_threshold = FT_MulFix( laxis->edge_distance_threshold,
                                         scale );
    if ( edge_distance_threshold > 64 / 4 )
      edge_distance_threshold = 64 / 4;

    edge_distance_threshold = FT_DivFix( edge_distance_threshold,
                                         scale );

    for ( seg = segments; seg < segment_limit; seg++ )
    {
      AF_Edge  found = 0;
      FT_Int   ee;


      if ( seg->height < segment_length_threshold )
        continue;

      /* A special case for serif edges: If they are smaller than */
      /* 1.5 pixels we ignore them.                               */
      if ( seg->serif )
      {
        FT_Pos  dist = seg->serif->pos - seg->pos;


        if ( dist < 0 )
          dist = -dist;

        if ( dist >= laxis->standard_width >> 1 )
        {
          /* unlink this serif, it is too distant from its reference stem */
          seg->serif = NULL;
        }
        else if ( 2*seg->height < 3 * segment_length_threshold )
          continue;
      }

      /* look for an edge corresponding to the segment */
      for ( ee = 0; ee < axis->num_edges; ee++ )
      {
        AF_Edge  edge = axis->edges + ee;
        FT_Pos   dist;


        dist = seg->pos - edge->fpos;
        if ( dist < 0 )
          dist = -dist;

        if ( dist < edge_distance_threshold && edge->dir == seg->dir )
        {
          found = edge;
          break;
        }
      }

      if ( !found )
      {
        AF_Edge   edge;


        /* insert a new edge in the list and */
        /* sort according to the position    */
        error = af_axis_hints_new_edge( axis, seg->pos, seg->dir,
                                        memory, &edge );
        if ( error )
          goto Exit;

        /* add the segment to the new edge's list */
        FT_ZERO( edge );

        edge->first    = seg;
        edge->last     = seg;
        edge->fpos     = seg->pos;
        edge->dir      = seg->dir;
        edge->opos     = edge->pos = FT_MulFix( seg->pos, scale );
        seg->edge_next = seg;
      }
      else
      {
        /* if an edge was found, simply add the segment to the edge's */
        /* list                                                       */
        seg->edge_next         = found->first;
        found->last->edge_next = seg;
        found->last            = seg;
      }
    }


    /*********************************************************************/
    /*                                                                   */
    /* Good, we will now compute each edge's properties according to     */
    /* segments found on its position.  Basically, these are:            */
    /*                                                                   */
    /*  - edge's main direction                                          */
    /*  - stem edge, serif edge or both (which defaults to stem then)    */
    /*  - rounded edge, straight or both (which defaults to straight)    */
    /*  - link for edge                                                  */
    /*                                                                   */
    /*********************************************************************/

    /* first of all, set the `edge' field in each segment -- this is */
    /* required in order to compute edge links                       */

    /*
     * Note that removing this loop and setting the `edge' field of each
     * segment directly in the code above slows down execution speed for
     * some reasons on platforms like the Sun.
     */
    {
      AF_Edge  edges      = axis->edges;
      AF_Edge  edge_limit = edges + axis->num_edges;
      AF_Edge  edge;


      for ( edge = edges; edge < edge_limit; edge++ )
      {
        seg = edge->first;
        if ( seg )
          do
          {
            seg->edge = edge;
            seg       = seg->edge_next;

          } while ( seg != edge->first );
      }

      /* now, compute each edge properties */
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        FT_Int  is_round    = 0;  /* does it contain round segments?    */
        FT_Int  is_straight = 0;  /* does it contain straight segments? */
#if 0
        FT_Pos  ups         = 0;  /* number of upwards segments         */
        FT_Pos  downs       = 0;  /* number of downwards segments       */
#endif


        seg = edge->first;

        do
        {
          FT_Bool  is_serif;


          /* check for roundness of segment */
          if ( seg->flags & AF_EDGE_ROUND )
            is_round++;
          else
            is_straight++;

#if 0
          /* check for segment direction */
          if ( seg->dir == up_dir )
            ups   += seg->max_coord-seg->min_coord;
          else
            downs += seg->max_coord-seg->min_coord;
#endif

          /* check for links -- if seg->serif is set, then seg->link must */
          /* be ignored                                                   */
          is_serif = (FT_Bool)( seg->serif               &&
                                seg->serif->edge         &&
                                seg->serif->edge != edge );

          if ( ( seg->link && seg->link->edge != NULL ) || is_serif )
          {
            AF_Edge     edge2;
            AF_Segment  seg2;


            edge2 = edge->link;
            seg2  = seg->link;

            if ( is_serif )
            {
              seg2  = seg->serif;
              edge2 = edge->serif;
            }

            if ( edge2 )
            {
              FT_Pos  edge_delta;
              FT_Pos  seg_delta;


              edge_delta = edge->fpos - edge2->fpos;
              if ( edge_delta < 0 )
                edge_delta = -edge_delta;

              seg_delta = seg->pos - seg2->pos;
              if ( seg_delta < 0 )
                seg_delta = -seg_delta;

              if ( seg_delta < edge_delta )
                edge2 = seg2->edge;
            }
            else
              edge2 = seg2->edge;

            if ( is_serif )
            {
              edge->serif   = edge2;
              edge2->flags |= AF_EDGE_SERIF;
            }
            else
              edge->link  = edge2;
          }

          seg = seg->edge_next;

        } while ( seg != edge->first );

        /* set the round/straight flags */
        edge->flags = AF_EDGE_NORMAL;

        if ( is_round > 0 && is_round >= is_straight )
          edge->flags |= AF_EDGE_ROUND;

#if 0
        /* set the edge's main direction */
        edge->dir = AF_DIR_NONE;

        if ( ups > downs )
          edge->dir = (FT_Char)up_dir;

        else if ( ups < downs )
          edge->dir = (FT_Char)-up_dir;

        else if ( ups == downs )
          edge->dir = 0;  /* both up and down! */
#endif

        /* gets rid of serifs if link is set                */
        /* XXX: This gets rid of many unpleasant artefacts! */
        /*      Example: the `c' in cour.pfa at size 13     */

        if ( edge->serif && edge->link )
          edge->serif = 0;
      }
    }

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  af_latin2_hints_detect_features( AF_GlyphHints  hints,
                                   AF_Dimension   dim )
  {
    FT_Error  error;


    error = af_latin2_hints_compute_segments( hints, dim );
    if ( !error )
    {
      af_latin2_hints_link_segments( hints, dim );

      error = af_latin2_hints_compute_edges( hints, dim );
    }
    return error;
  }


  FT_LOCAL_DEF( void )
  af_latin2_hints_compute_blue_edges( AF_GlyphHints    hints,
                                      AF_LatinMetrics  metrics )
  {
    AF_AxisHints  axis       = &hints->axis[AF_DIMENSION_VERT];
    AF_Edge       edge       = axis->edges;
    AF_Edge       edge_limit = edge + axis->num_edges;
    AF_LatinAxis  latin      = &metrics->axis[AF_DIMENSION_VERT];
    FT_Fixed      scale      = latin->scale;
    FT_Pos        best_dist0;  /* initial threshold */


    /* compute the initial threshold as a fraction of the EM size */
    best_dist0 = FT_MulFix( metrics->units_per_em / 40, scale );

    if ( best_dist0 > 64 / 2 )
      best_dist0 = 64 / 2;

    /* compute which blue zones are active, i.e. have their scaled */
    /* size < 3/4 pixels                                           */

    /* for each horizontal edge search the blue zone which is closest */
    for ( ; edge < edge_limit; edge++ )
    {
      FT_Int    bb;
      AF_Width  best_blue = NULL;
      FT_Pos    best_dist = best_dist0;

      for ( bb = 0; bb < AF_LATIN_BLUE_MAX; bb++ )
      {
        AF_LatinBlue  blue = latin->blues + bb;
        FT_Bool       is_top_blue, is_major_dir;


        /* skip inactive blue zones (i.e., those that are too small) */
        if ( !( blue->flags & AF_LATIN_BLUE_ACTIVE ) )
          continue;

        /* if it is a top zone, check for right edges -- if it is a bottom */
        /* zone, check for left edges                                      */
        /*                                                                 */
        /* of course, that's for TrueType                                  */
        is_top_blue  = (FT_Byte)( ( blue->flags & AF_LATIN_BLUE_TOP ) != 0 );
        is_major_dir = FT_BOOL( edge->dir == axis->major_dir );

        /* if it is a top zone, the edge must be against the major    */
        /* direction; if it is a bottom zone, it must be in the major */
        /* direction                                                  */
        if ( is_top_blue ^ is_major_dir )
        {
          FT_Pos     dist;
          AF_Width   compare;


          /* if it's a rounded edge, compare it to the overshoot position */
          /* if it's a flat edge, compare it to the reference position    */
          if ( edge->flags & AF_EDGE_ROUND )
            compare = &blue->shoot;
          else
            compare = &blue->ref;

          dist = edge->fpos - compare->org;
          if ( dist < 0 )
            dist = -dist;

          dist = FT_MulFix( dist, scale );
          if ( dist < best_dist )
          {
            best_dist = dist;
            best_blue = compare;
          }

#if 0
          /* now, compare it to the overshoot position if the edge is     */
          /* rounded, and if the edge is over the reference position of a */
          /* top zone, or under the reference position of a bottom zone   */
          if ( edge->flags & AF_EDGE_ROUND && dist != 0 )
          {
            FT_Bool  is_under_ref = FT_BOOL( edge->fpos < blue->ref.org );


            if ( is_top_blue ^ is_under_ref )
            {
              blue = latin->blues + bb;
              dist = edge->fpos - blue->shoot.org;
              if ( dist < 0 )
                dist = -dist;

              dist = FT_MulFix( dist, scale );
              if ( dist < best_dist )
              {
                best_dist = dist;
                best_blue = & blue->shoot;
              }
            }
          }
#endif
        }
      }

      if ( best_blue )
        edge->blue_edge = best_blue;
    }
  }


  static FT_Error
  af_latin2_hints_init( AF_GlyphHints    hints,
                        AF_LatinMetrics  metrics )
  {
    FT_Render_Mode  mode;
    FT_UInt32       scaler_flags, other_flags;
    FT_Face         face = metrics->root.scaler.face;


    af_glyph_hints_rescale( hints, (AF_StyleMetrics)metrics );

    /*
     *  correct x_scale and y_scale if needed, since they may have
     *  been modified `af_latin2_metrics_scale_dim' above
     */
    hints->x_scale = metrics->axis[AF_DIMENSION_HORZ].scale;
    hints->x_delta = metrics->axis[AF_DIMENSION_HORZ].delta;
    hints->y_scale = metrics->axis[AF_DIMENSION_VERT].scale;
    hints->y_delta = metrics->axis[AF_DIMENSION_VERT].delta;

    /* compute flags depending on render mode, etc. */
    mode = metrics->root.scaler.render_mode;

#if 0 /* #ifdef AF_CONFIG_OPTION_USE_WARPER */
    if ( mode == FT_RENDER_MODE_LCD || mode == FT_RENDER_MODE_LCD_V )
    {
      metrics->root.scaler.render_mode = mode = FT_RENDER_MODE_NORMAL;
    }
#endif

    scaler_flags = hints->scaler_flags;
    other_flags  = 0;

    /*
     *  We snap the width of vertical stems for the monochrome and
     *  horizontal LCD rendering targets only.
     */
    if ( mode == FT_RENDER_MODE_MONO || mode == FT_RENDER_MODE_LCD )
      other_flags |= AF_LATIN_HINTS_HORZ_SNAP;

    /*
     *  We snap the width of horizontal stems for the monochrome and
     *  vertical LCD rendering targets only.
     */
    if ( mode == FT_RENDER_MODE_MONO || mode == FT_RENDER_MODE_LCD_V )
      other_flags |= AF_LATIN_HINTS_VERT_SNAP;

    /*
     *  We adjust stems to full pixels only if we don't use the `light' mode.
     */
    if ( mode != FT_RENDER_MODE_LIGHT )
      other_flags |= AF_LATIN_HINTS_STEM_ADJUST;

    if ( mode == FT_RENDER_MODE_MONO )
      other_flags |= AF_LATIN_HINTS_MONO;

    /*
     *  In `light' hinting mode we disable horizontal hinting completely.
     *  We also do it if the face is italic.
     */
    if ( mode == FT_RENDER_MODE_LIGHT                      ||
         ( face->style_flags & FT_STYLE_FLAG_ITALIC ) != 0 )
      scaler_flags |= AF_SCALER_FLAG_NO_HORIZONTAL;

    hints->scaler_flags = scaler_flags;
    hints->other_flags  = other_flags;

    return 0;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****        L A T I N   G L Y P H   G R I D - F I T T I N G        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* snap a given width in scaled coordinates to one of the */
  /* current standard widths                                */

  static FT_Pos
  af_latin2_snap_width( AF_Width  widths,
                        FT_Int    count,
                        FT_Pos    width )
  {
    int     n;
    FT_Pos  best      = 64 + 32 + 2;
    FT_Pos  reference = width;
    FT_Pos  scaled;


    for ( n = 0; n < count; n++ )
    {
      FT_Pos  w;
      FT_Pos  dist;


      w = widths[n].cur;
      dist = width - w;
      if ( dist < 0 )
        dist = -dist;
      if ( dist < best )
      {
        best      = dist;
        reference = w;
      }
    }

    scaled = FT_PIX_ROUND( reference );

    if ( width >= reference )
    {
      if ( width < scaled + 48 )
        width = reference;
    }
    else
    {
      if ( width > scaled - 48 )
        width = reference;
    }

    return width;
  }


  /* compute the snapped width of a given stem */

  static FT_Pos
  af_latin2_compute_stem_width( AF_GlyphHints  hints,
                                AF_Dimension   dim,
                                FT_Pos         width,
                                AF_Edge_Flags  base_flags,
                                AF_Edge_Flags  stem_flags )
  {
    AF_LatinMetrics  metrics  = (AF_LatinMetrics) hints->metrics;
    AF_LatinAxis     axis     = & metrics->axis[dim];
    FT_Pos           dist     = width;
    FT_Int           sign     = 0;
    FT_Int           vertical = ( dim == AF_DIMENSION_VERT );

    FT_UNUSED( base_flags );


    if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) ||
          axis->extra_light                      )
      return width;

    if ( dist < 0 )
    {
      dist = -width;
      sign = 1;
    }

    if ( (  vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) ) ||
         ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) )
    {
      /* smooth hinting process: very lightly quantize the stem width */

      /* leave the widths of serifs alone */

      if ( ( stem_flags & AF_EDGE_SERIF ) && vertical && ( dist < 3 * 64 ) )
        goto Done_Width;

#if 0
      else if ( ( base_flags & AF_EDGE_ROUND ) )
      {
        if ( dist < 80 )
          dist = 64;
      }
      else if ( dist < 56 )
        dist = 56;
#endif
      if ( axis->width_count > 0 )
      {
        FT_Pos  delta;


        /* compare to standard width */
        if ( axis->width_count > 0 )
        {
          delta = dist - axis->widths[0].cur;

          if ( delta < 0 )
            delta = -delta;

          if ( delta < 40 )
          {
            dist = axis->widths[0].cur;
            if ( dist < 48 )
              dist = 48;

            goto Done_Width;
          }
        }

        if ( dist < 3 * 64 )
        {
          delta  = dist & 63;
          dist  &= -64;

          if ( delta < 10 )
            dist += delta;

          else if ( delta < 32 )
            dist += 10;

          else if ( delta < 54 )
            dist += 54;

          else
            dist += delta;
        }
        else
          dist = ( dist + 32 ) & ~63;
      }
    }
    else
    {
      /* strong hinting process: snap the stem width to integer pixels */
      FT_Pos  org_dist = dist;


      dist = af_latin2_snap_width( axis->widths, axis->width_count, dist );

      if ( vertical )
      {
        /* in the case of vertical hinting, always round */
        /* the stem heights to integer pixels            */

        if ( dist >= 64 )
          dist = ( dist + 16 ) & ~63;
        else
          dist = 64;
      }
      else
      {
        if ( AF_LATIN_HINTS_DO_MONO( hints ) )
        {
          /* monochrome horizontal hinting: snap widths to integer pixels */
          /* with a different threshold                                   */

          if ( dist < 64 )
            dist = 64;
          else
            dist = ( dist + 32 ) & ~63;
        }
        else
        {
          /* for horizontal anti-aliased hinting, we adopt a more subtle */
          /* approach: we strengthen small stems, round stems whose size */
          /* is between 1 and 2 pixels to an integer, otherwise nothing  */

          if ( dist < 48 )
            dist = ( dist + 64 ) >> 1;

          else if ( dist < 128 )
          {
            /* We only round to an integer width if the corresponding */
            /* distortion is less than 1/4 pixel.  Otherwise this     */
            /* makes everything worse since the diagonals, which are  */
            /* not hinted, appear a lot bolder or thinner than the    */
            /* vertical stems.                                        */

            FT_Int  delta;


            dist = ( dist + 22 ) & ~63;
            delta = dist - org_dist;
            if ( delta < 0 )
              delta = -delta;

            if ( delta >= 16 )
            {
              dist = org_dist;
              if ( dist < 48 )
                dist = ( dist + 64 ) >> 1;
            }
          }
          else
            /* round otherwise to prevent color fringes in LCD mode */
            dist = ( dist + 32 ) & ~63;
        }
      }
    }

  Done_Width:
    if ( sign )
      dist = -dist;

    return dist;
  }


  /* align one stem edge relative to the previous stem edge */

  static void
  af_latin2_align_linked_edge( AF_GlyphHints  hints,
                               AF_Dimension   dim,
                               AF_Edge        base_edge,
                               AF_Edge        stem_edge )
  {
    FT_Pos  dist = stem_edge->opos - base_edge->opos;

    FT_Pos  fitted_width = af_latin2_compute_stem_width(
                             hints, dim, dist,
                             (AF_Edge_Flags)base_edge->flags,
                             (AF_Edge_Flags)stem_edge->flags );


    stem_edge->pos = base_edge->pos + fitted_width;

    FT_TRACE5(( "LINK: edge %d (opos=%.2f) linked to (%.2f), "
                "dist was %.2f, now %.2f\n",
                stem_edge-hints->axis[dim].edges, stem_edge->opos / 64.0,
                stem_edge->pos / 64.0, dist / 64.0, fitted_width / 64.0 ));
  }


  static void
  af_latin2_align_serif_edge( AF_GlyphHints  hints,
                              AF_Edge        base,
                              AF_Edge        serif )
  {
    FT_UNUSED( hints );

    serif->pos = base->pos + ( serif->opos - base->opos );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                    E D G E   H I N T I N G                      ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  FT_LOCAL_DEF( void )
  af_latin2_hint_edges( AF_GlyphHints  hints,
                        AF_Dimension   dim )
  {
    AF_AxisHints  axis       = &hints->axis[dim];
    AF_Edge       edges      = axis->edges;
    AF_Edge       edge_limit = edges + axis->num_edges;
    AF_Edge       edge;
    AF_Edge       anchor     = 0;
    FT_Int        has_serifs = 0;
    FT_Pos        anchor_drift = 0;



    FT_TRACE5(( "==== hinting %s edges =====\n",
                dim == AF_DIMENSION_HORZ ? "vertical" : "horizontal" ));

    /* we begin by aligning all stems relative to the blue zone */
    /* if needed -- that's only for horizontal edges            */

    if ( dim == AF_DIMENSION_VERT && AF_HINTS_DO_BLUES( hints ) )
    {
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        AF_Width  blue;
        AF_Edge   edge1, edge2;


        if ( edge->flags & AF_EDGE_DONE )
          continue;

        blue  = edge->blue_edge;
        edge1 = NULL;
        edge2 = edge->link;

        if ( blue )
        {
          edge1 = edge;
        }
        else if ( edge2 && edge2->blue_edge )
        {
          blue  = edge2->blue_edge;
          edge1 = edge2;
          edge2 = edge;
        }

        if ( !edge1 )
          continue;

        FT_TRACE5(( "BLUE: edge %d (opos=%.2f) snapped to (%.2f), "
                    "was (%.2f)\n",
                    edge1-edges, edge1->opos / 64.0, blue->fit / 64.0,
                    edge1->pos / 64.0 ));

        edge1->pos    = blue->fit;
        edge1->flags |= AF_EDGE_DONE;

        if ( edge2 && !edge2->blue_edge )
        {
          af_latin2_align_linked_edge( hints, dim, edge1, edge2 );
          edge2->flags |= AF_EDGE_DONE;
        }

        if ( !anchor )
        {
          anchor = edge;

          anchor_drift = ( anchor->pos - anchor->opos );
          if ( edge2 )
            anchor_drift = ( anchor_drift +
                             ( edge2->pos - edge2->opos ) ) >> 1;
        }
      }
    }

    /* now we will align all stem edges, trying to maintain the */
    /* relative order of stems in the glyph                     */
    for ( edge = edges; edge < edge_limit; edge++ )
    {
      AF_Edge  edge2;


      if ( edge->flags & AF_EDGE_DONE )
        continue;

      /* skip all non-stem edges */
      edge2 = edge->link;
      if ( !edge2 )
      {
        has_serifs++;
        continue;
      }

      /* now align the stem */

      /* this should not happen, but it's better to be safe */
      if ( edge2->blue_edge )
      {
        FT_TRACE5(( "ASSERTION FAILED for edge %d\n", edge2-edges ));

        af_latin2_align_linked_edge( hints, dim, edge2, edge );
        edge->flags |= AF_EDGE_DONE;
        continue;
      }

      if ( !anchor )
      {
        FT_Pos  org_len, org_center, cur_len;
        FT_Pos  cur_pos1, error1, error2, u_off, d_off;


        org_len = edge2->opos - edge->opos;
        cur_len = af_latin2_compute_stem_width(
                    hints, dim, org_len,
                    (AF_Edge_Flags)edge->flags,
                    (AF_Edge_Flags)edge2->flags );
        if ( cur_len <= 64 )
          u_off = d_off = 32;
        else
        {
          u_off = 38;
          d_off = 26;
        }

        if ( cur_len < 96 )
        {
          org_center = edge->opos + ( org_len >> 1 );

          cur_pos1   = FT_PIX_ROUND( org_center );

          error1 = org_center - ( cur_pos1 - u_off );
          if ( error1 < 0 )
            error1 = -error1;

          error2 = org_center - ( cur_pos1 + d_off );
          if ( error2 < 0 )
            error2 = -error2;

          if ( error1 < error2 )
            cur_pos1 -= u_off;
          else
            cur_pos1 += d_off;

          edge->pos  = cur_pos1 - cur_len / 2;
          edge2->pos = edge->pos + cur_len;
        }
        else
          edge->pos = FT_PIX_ROUND( edge->opos );

        FT_TRACE5(( "ANCHOR: edge %d (opos=%.2f) and %d (opos=%.2f)"
                    " snapped to (%.2f) (%.2f)\n",
                    edge-edges, edge->opos / 64.0,
                    edge2-edges, edge2->opos / 64.0,
                    edge->pos / 64.0, edge2->pos / 64.0 ));
        anchor = edge;

        edge->flags |= AF_EDGE_DONE;

        af_latin2_align_linked_edge( hints, dim, edge, edge2 );

        edge2->flags |= AF_EDGE_DONE;

        anchor_drift = ( ( anchor->pos - anchor->opos ) +
                         ( edge2->pos - edge2->opos ) ) >> 1;

        FT_TRACE5(( "DRIFT: %.2f\n", anchor_drift/64.0 ));
      }
      else
      {
        FT_Pos   org_pos, org_len, org_center, cur_center, cur_len;
        FT_Pos   org_left, org_right;


        org_pos    = edge->opos + anchor_drift;
        org_len    = edge2->opos - edge->opos;
        org_center = org_pos + ( org_len >> 1 );

        cur_len = af_latin2_compute_stem_width(
                   hints, dim, org_len,
                   (AF_Edge_Flags)edge->flags,
                   (AF_Edge_Flags)edge2->flags );

        org_left  = org_pos + ( ( org_len - cur_len ) >> 1 );
        org_right = org_pos + ( ( org_len + cur_len ) >> 1 );

        FT_TRACE5(( "ALIGN: left=%.2f right=%.2f ",
                    org_left / 64.0, org_right / 64.0 ));
        cur_center = org_center;

        if ( edge2->flags & AF_EDGE_DONE )
        {
          FT_TRACE5(( "\n" ));
          edge->pos = edge2->pos - cur_len;
        }
        else
        {
         /* we want to compare several displacement, and choose
          * the one that increases fitness while minimizing
          * distortion as well
          */
          FT_Pos   displacements[6], scores[6], org, fit, delta;
          FT_UInt  count = 0;

          /* note: don't even try to fit tiny stems */
          if ( cur_len < 32 )
          {
            FT_TRACE5(( "tiny stem\n" ));
            goto AlignStem;
          }

          /* if the span is within a single pixel, don't touch it */
          if ( FT_PIX_FLOOR( org_left ) == FT_PIX_CEIL( org_right ) )
          {
            FT_TRACE5(( "single pixel stem\n" ));
            goto AlignStem;
          }

          if ( cur_len <= 96 )
          {
           /* we want to avoid the absolute worst case which is
            * when the left and right edges of the span each represent
            * about 50% of the gray. we'd better want to change this
            * to 25/75%, since this is much more pleasant to the eye with
            * very acceptable distortion
            */
            FT_Pos  frac_left  = org_left  & 63;
            FT_Pos  frac_right = org_right & 63;

            if ( frac_left  >= 22 && frac_left  <= 42 &&
                 frac_right >= 22 && frac_right <= 42 )
            {
              org = frac_left;
              fit = ( org <= 32 ) ? 16 : 48;
              delta = FT_ABS( fit - org );
              displacements[count] = fit - org;
              scores[count++]      = delta;
              FT_TRACE5(( "dispA=%.2f (%d) ", ( fit - org ) / 64.0, delta ));

              org = frac_right;
              fit = ( org <= 32 ) ? 16 : 48;
              delta = FT_ABS( fit - org );
              displacements[count] = fit - org;
              scores[count++]     = delta;
              FT_TRACE5(( "dispB=%.2f (%d) ", ( fit - org ) / 64.0, delta ));
            }
          }

          /* snapping the left edge to the grid */
          org   = org_left;
          fit   = FT_PIX_ROUND( org );
          delta = FT_ABS( fit - org );
          displacements[count] = fit - org;
          scores[count++]      = delta;
          FT_TRACE5(( "dispC=%.2f (%d) ", ( fit - org ) / 64.0, delta ));

          /* snapping the right edge to the grid */
          org   = org_right;
          fit   = FT_PIX_ROUND( org );
          delta = FT_ABS( fit - org );
          displacements[count] = fit - org;
          scores[count++]      = delta;
          FT_TRACE5(( "dispD=%.2f (%d) ", ( fit - org ) / 64.0, delta ));

          /* now find the best displacement */
          {
            FT_Pos  best_score = scores[0];
            FT_Pos  best_disp  = displacements[0];
            FT_UInt nn;

            for ( nn = 1; nn < count; nn++ )
            {
              if ( scores[nn] < best_score )
              {
                best_score = scores[nn];
                best_disp  = displacements[nn];
              }
            }

            cur_center = org_center + best_disp;
          }
          FT_TRACE5(( "\n" ));
        }

      AlignStem:
        edge->pos  = cur_center - ( cur_len >> 1 );
        edge2->pos = edge->pos + cur_len;

        FT_TRACE5(( "STEM1: %d (opos=%.2f) to %d (opos=%.2f)"
                    " snapped to (%.2f) and (%.2f),"
                    " org_len=%.2f cur_len=%.2f\n",
                    edge-edges, edge->opos / 64.0,
                    edge2-edges, edge2->opos / 64.0,
                    edge->pos / 64.0, edge2->pos / 64.0,
                    org_len / 64.0, cur_len / 64.0 ));

        edge->flags  |= AF_EDGE_DONE;
        edge2->flags |= AF_EDGE_DONE;

        if ( edge > edges && edge->pos < edge[-1].pos )
        {
          FT_TRACE5(( "BOUND: %d (pos=%.2f) to (%.2f)\n",
                      edge-edges, edge->pos / 64.0, edge[-1].pos / 64.0 ));
          edge->pos = edge[-1].pos;
        }
      }
    }

    /* make sure that lowercase m's maintain their symmetry */

    /* In general, lowercase m's have six vertical edges if they are sans */
    /* serif, or twelve if they are with serifs.  This implementation is  */
    /* based on that assumption, and seems to work very well with most    */
    /* faces.  However, if for a certain face this assumption is not      */
    /* true, the m is just rendered like before.  In addition, any stem   */
    /* correction will only be applied to symmetrical glyphs (even if the */
    /* glyph is not an m), so the potential for unwanted distortion is    */
    /* relatively low.                                                    */

    /* We don't handle horizontal edges since we can't easily assure that */
    /* the third (lowest) stem aligns with the base line; it might end up */
    /* one pixel higher or lower.                                         */

#if 0
    {
      FT_Int  n_edges = edge_limit - edges;


      if ( dim == AF_DIMENSION_HORZ && ( n_edges == 6 || n_edges == 12 ) )
      {
        AF_Edge  edge1, edge2, edge3;
        FT_Pos   dist1, dist2, span, delta;


        if ( n_edges == 6 )
        {
          edge1 = edges;
          edge2 = edges + 2;
          edge3 = edges + 4;
        }
        else
        {
          edge1 = edges + 1;
          edge2 = edges + 5;
          edge3 = edges + 9;
        }

        dist1 = edge2->opos - edge1->opos;
        dist2 = edge3->opos - edge2->opos;

        span = dist1 - dist2;
        if ( span < 0 )
          span = -span;

        if ( span < 8 )
        {
          delta = edge3->pos - ( 2 * edge2->pos - edge1->pos );
          edge3->pos -= delta;
          if ( edge3->link )
            edge3->link->pos -= delta;

          /* move the serifs along with the stem */
          if ( n_edges == 12 )
          {
            ( edges + 8 )->pos -= delta;
            ( edges + 11 )->pos -= delta;
          }

          edge3->flags |= AF_EDGE_DONE;
          if ( edge3->link )
            edge3->link->flags |= AF_EDGE_DONE;
        }
      }
    }
#endif

    if ( has_serifs || !anchor )
    {
      /*
       *  now hint the remaining edges (serifs and single) in order
       *  to complete our processing
       */
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        FT_Pos  delta;


        if ( edge->flags & AF_EDGE_DONE )
          continue;

        delta = 1000;

        if ( edge->serif )
        {
          delta = edge->serif->opos - edge->opos;
          if ( delta < 0 )
            delta = -delta;
        }

        if ( delta < 64 + 16 )
        {
          af_latin2_align_serif_edge( hints, edge->serif, edge );
          FT_TRACE5(( "SERIF: edge %d (opos=%.2f) serif to %d (opos=%.2f)"
                      " aligned to (%.2f)\n",
                      edge-edges, edge->opos / 64.0,
                      edge->serif - edges, edge->serif->opos / 64.0,
                      edge->pos / 64.0 ));
        }
        else if ( !anchor )
        {
          FT_TRACE5(( "SERIF_ANCHOR: edge %d (opos=%.2f)"
                      " snapped to (%.2f)\n",
                      edge-edges, edge->opos / 64.0, edge->pos / 64.0 ));
          edge->pos = FT_PIX_ROUND( edge->opos );
          anchor    = edge;
        }
        else
        {
          AF_Edge  before, after;


          for ( before = edge - 1; before >= edges; before-- )
            if ( before->flags & AF_EDGE_DONE )
              break;

          for ( after = edge + 1; after < edge_limit; after++ )
            if ( after->flags & AF_EDGE_DONE )
              break;

          if ( before >= edges && before < edge   &&
               after < edge_limit && after > edge )
          {
            if ( after->opos == before->opos )
              edge->pos = before->pos;
            else
              edge->pos = before->pos +
                          FT_MulDiv( edge->opos - before->opos,
                                     after->pos - before->pos,
                                     after->opos - before->opos );
            FT_TRACE5(( "SERIF_LINK1: edge %d (opos=%.2f) snapped to (%.2f)"
                        " from %d (opos=%.2f)\n",
                        edge-edges, edge->opos / 64.0, edge->pos / 64.0,
                        before - edges, before->opos / 64.0 ));
          }
          else
          {
            edge->pos = anchor->pos +
                        ( ( edge->opos - anchor->opos + 16 ) & ~31 );

            FT_TRACE5(( "SERIF_LINK2: edge %d (opos=%.2f)"
                        " snapped to (%.2f)\n",
                        edge-edges, edge->opos / 64.0, edge->pos / 64.0 ));
          }
        }

        edge->flags |= AF_EDGE_DONE;

        if ( edge > edges && edge->pos < edge[-1].pos )
          edge->pos = edge[-1].pos;

        if ( edge + 1 < edge_limit        &&
             edge[1].flags & AF_EDGE_DONE &&
             edge->pos > edge[1].pos      )
          edge->pos = edge[1].pos;
      }
    }
  }


  static FT_Error
  af_latin2_hints_apply( AF_GlyphHints    hints,
                         FT_Outline*      outline,
                         AF_LatinMetrics  metrics )
  {
    FT_Error  error;
    int       dim;


    error = af_glyph_hints_reload( hints, outline );
    if ( error )
      goto Exit;

    /* analyze glyph outline */
#ifdef AF_CONFIG_OPTION_USE_WARPER
    if ( metrics->root.scaler.render_mode == FT_RENDER_MODE_LIGHT ||
         AF_HINTS_DO_HORIZONTAL( hints ) )
#else
    if ( AF_HINTS_DO_HORIZONTAL( hints ) )
#endif
    {
      error = af_latin2_hints_detect_features( hints, AF_DIMENSION_HORZ );
      if ( error )
        goto Exit;
    }

    if ( AF_HINTS_DO_VERTICAL( hints ) )
    {
      error = af_latin2_hints_detect_features( hints, AF_DIMENSION_VERT );
      if ( error )
        goto Exit;

      af_latin2_hints_compute_blue_edges( hints, metrics );
    }

    /* grid-fit the outline */
    for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
    {
#ifdef AF_CONFIG_OPTION_USE_WARPER
      if ( ( dim == AF_DIMENSION_HORZ &&
             metrics->root.scaler.render_mode == FT_RENDER_MODE_LIGHT ) )
      {
        AF_WarperRec  warper;
        FT_Fixed      scale;
        FT_Pos        delta;


        af_warper_compute( &warper, hints, dim, &scale, &delta );
        af_glyph_hints_scale_dim( hints, dim, scale, delta );
        continue;
      }
#endif

      if ( ( dim == AF_DIMENSION_HORZ && AF_HINTS_DO_HORIZONTAL( hints ) ) ||
           ( dim == AF_DIMENSION_VERT && AF_HINTS_DO_VERTICAL( hints ) )   )
      {
        af_latin2_hint_edges( hints, (AF_Dimension)dim );
        af_glyph_hints_align_edge_points( hints, (AF_Dimension)dim );
        af_glyph_hints_align_strong_points( hints, (AF_Dimension)dim );
        af_glyph_hints_align_weak_points( hints, (AF_Dimension)dim );
      }
    }
    af_glyph_hints_save( hints, outline );

  Exit:
    return error;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****              L A T I N   S C R I P T   C L A S S              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_latin2_writing_system_class,

    AF_WRITING_SYSTEM_LATIN2,

    sizeof ( AF_LatinMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) af_latin2_metrics_init,
    (AF_WritingSystem_ScaleMetricsFunc)af_latin2_metrics_scale,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   af_latin2_hints_init,
    (AF_WritingSystem_ApplyHintsFunc)  af_latin2_hints_apply
  )


/* END */
#endif
/***************************************************************************/
/*                                                                         */
/*  afcjk.c                                                                */
/*                                                                         */
/*    Auto-fitter hinting routines for CJK writing system (body).          */
/*                                                                         */
/*  Copyright 2006-2013 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

  /*
   *  The algorithm is based on akito's autohint patch, available here:
   *
   *  http://www.kde.gr.jp/~akito/patch/freetype2/
   *
   */

#include "ft2build.h"
#include FT_ADVANCES_H
#include FT_INTERNAL_DEBUG_H

#include "afglobal.h"
#include "afpic.h"
#include "aflatin.h"


#ifdef AF_CONFIG_OPTION_CJK

#undef AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT

#include "afcjk.h"
#include "aferrors.h"


#ifdef AF_CONFIG_OPTION_USE_WARPER
#include "afwarp.h"
#endif


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_afcjk


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****              C J K   G L O B A L   M E T R I C S              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /* Basically the Latin version with AF_CJKMetrics */
  /* to replace AF_LatinMetrics.                    */

  FT_LOCAL_DEF( void )
  af_cjk_metrics_init_widths( AF_CJKMetrics  metrics,
                              FT_Face        face )
  {
    /* scan the array of segments in each direction */
    AF_GlyphHintsRec  hints[1];


    FT_TRACE5(( "\n"
                "cjk standard widths computation (style `%s')\n"
                "===================================================\n"
                "\n",
                af_style_names[metrics->root.style_class->style] ));

    af_glyph_hints_init( hints, face->memory );

    metrics->axis[AF_DIMENSION_HORZ].width_count = 0;
    metrics->axis[AF_DIMENSION_VERT].width_count = 0;

    {
      FT_Error          error;
      FT_ULong          glyph_index;
      FT_Long           y_offset;
      int               dim;
      AF_CJKMetricsRec  dummy[1];
      AF_Scaler         scaler = &dummy->root.scaler;

#ifdef FT_CONFIG_OPTION_PIC
      AF_FaceGlobals  globals = metrics->root.globals;
#endif

      AF_StyleClass   style_class  = metrics->root.style_class;
      AF_ScriptClass  script_class = AF_SCRIPT_CLASSES_GET
                                       [style_class->script];

      FT_UInt32  standard_char;


      standard_char = script_class->standard_char1;
      af_get_char_index( &metrics->root,
                         standard_char,
                         &glyph_index,
                         &y_offset );
      if ( !glyph_index )
      {
        if ( script_class->standard_char2 )
        {
          standard_char = script_class->standard_char2;
          af_get_char_index( &metrics->root,
                             standard_char,
                             &glyph_index,
                             &y_offset );
          if ( !glyph_index )
          {
            if ( script_class->standard_char3 )
            {
              standard_char = script_class->standard_char3;
              af_get_char_index( &metrics->root,
                                 standard_char,
                                 &glyph_index,
                                 &y_offset );
              if ( !glyph_index )
                goto Exit;
            }
            else
              goto Exit;
          }
        }
        else
          goto Exit;
      }

      FT_TRACE5(( "standard character: U+%04lX (glyph index %d)\n",
                  standard_char, glyph_index ));

      error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
      if ( error || face->glyph->outline.n_points <= 0 )
        goto Exit;

      FT_ZERO( dummy );

      dummy->units_per_em = metrics->units_per_em;

      scaler->x_scale = 0x10000L;
      scaler->y_scale = 0x10000L;
      scaler->x_delta = 0;
      scaler->y_delta = 0;

      scaler->face        = face;
      scaler->render_mode = FT_RENDER_MODE_NORMAL;
      scaler->flags       = 0;

      af_glyph_hints_rescale( hints, (AF_StyleMetrics)dummy );

      error = af_glyph_hints_reload( hints, &face->glyph->outline );
      if ( error )
        goto Exit;

      for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
      {
        AF_CJKAxis    axis    = &metrics->axis[dim];
        AF_AxisHints  axhints = &hints->axis[dim];
        AF_Segment    seg, limit, link;
        FT_UInt       num_widths = 0;


        error = af_latin_hints_compute_segments( hints,
                                                 (AF_Dimension)dim );
        if ( error )
          goto Exit;

        af_latin_hints_link_segments( hints,
                                      (AF_Dimension)dim );

        seg   = axhints->segments;
        limit = seg + axhints->num_segments;

        for ( ; seg < limit; seg++ )
        {
          link = seg->link;

          /* we only consider stem segments there! */
          if ( link && link->link == seg && link > seg )
          {
            FT_Pos  dist;


            dist = seg->pos - link->pos;
            if ( dist < 0 )
              dist = -dist;

            if ( num_widths < AF_CJK_MAX_WIDTHS )
              axis->widths[num_widths++].org = dist;
          }
        }

        /* this also replaces multiple almost identical stem widths */
        /* with a single one (the value 100 is heuristic)           */
        af_sort_and_quantize_widths( &num_widths, axis->widths,
                                     dummy->units_per_em / 100 );
        axis->width_count = num_widths;
      }

    Exit:
      for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
      {
        AF_CJKAxis  axis = &metrics->axis[dim];
        FT_Pos      stdw;


        stdw = ( axis->width_count > 0 ) ? axis->widths[0].org
                                         : AF_LATIN_CONSTANT( metrics, 50 );

        /* let's try 20% of the smallest width */
        axis->edge_distance_threshold = stdw / 5;
        axis->standard_width          = stdw;
        axis->extra_light             = 0;

#ifdef FT_DEBUG_LEVEL_TRACE
        {
          FT_UInt  i;


          FT_TRACE5(( "%s widths:\n",
                      dim == AF_DIMENSION_VERT ? "horizontal"
                                               : "vertical" ));

          FT_TRACE5(( "  %d (standard)", axis->standard_width ));
          for ( i = 1; i < axis->width_count; i++ )
            FT_TRACE5(( " %d", axis->widths[i].org ));

          FT_TRACE5(( "\n" ));
        }
#endif
      }
    }

    FT_TRACE5(( "\n" ));

    af_glyph_hints_done( hints );
  }


  /* Find all blue zones. */

  static void
  af_cjk_metrics_init_blues( AF_CJKMetrics  metrics,
                             FT_Face        face )
  {
    FT_Pos      fills[AF_BLUE_STRING_MAX_LEN];
    FT_Pos      flats[AF_BLUE_STRING_MAX_LEN];

    FT_Int      num_fills;
    FT_Int      num_flats;

    AF_CJKBlue  blue;
    FT_Error    error;
    AF_CJKAxis  axis;
    FT_Outline  outline;

    AF_StyleClass  sc = metrics->root.style_class;

    AF_Blue_Stringset         bss = sc->blue_stringset;
    const AF_Blue_StringRec*  bs  = &af_blue_stringsets[bss];

#ifdef FT_DEBUG_LEVEL_TRACE
    FT_String*  cjk_blue_name[4] =
    {
      (FT_String*)"bottom",    /* --   , --  */
      (FT_String*)"top",       /* --   , TOP */
      (FT_String*)"left",      /* HORIZ, --  */
      (FT_String*)"right"      /* HORIZ, TOP */
    };

    FT_String*  cjk_blue_type_name[2] =
    {
      (FT_String*)"unfilled",  /* --   */
      (FT_String*)"filled"     /* FILL */
    };
#endif


    /* we walk over the blue character strings as specified in the   */
    /* style's entry in the `af_blue_stringset' array, computing its */
    /* extremum points (depending on the string properties)          */

    FT_TRACE5(( "cjk blue zones computation\n"
                "==========================\n"
                "\n" ));

    for ( ; bs->string != AF_BLUE_STRING_MAX; bs++ )
    {
      const char*  p = &af_blue_strings[bs->string];
      FT_Pos*      blue_ref;
      FT_Pos*      blue_shoot;


      if ( AF_CJK_IS_HORIZ_BLUE( bs ) )
        axis = &metrics->axis[AF_DIMENSION_HORZ];
      else
        axis = &metrics->axis[AF_DIMENSION_VERT];

      FT_TRACE5(( "blue zone %d:\n", axis->blue_count ));

      num_fills = 0;
      num_flats = 0;

      FT_TRACE5(( "  cjk blue %s/%s\n",
                  cjk_blue_name[AF_CJK_IS_HORIZ_BLUE( bs ) |
                                AF_CJK_IS_TOP_BLUE( bs )   ],
                  cjk_blue_type_name[!!AF_CJK_IS_FILLED_BLUE( bs )] ));

      while ( *p )
      {
        FT_ULong    ch;
        FT_ULong    glyph_index;
        FT_Long     y_offset;
        FT_Pos      best_pos;       /* same as points.y or points.x, resp. */
        FT_Int      best_point;
        FT_Vector*  points;


        GET_UTF8_CHAR( ch, p );

        /* load the character in the face -- skip unknown or empty ones */
        af_get_char_index( &metrics->root, ch, &glyph_index, &y_offset );
        if ( glyph_index == 0 )
        {
          FT_TRACE5(( "  U+%04lX unavailable\n", ch ));
          continue;
        }

        error   = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
        outline = face->glyph->outline;
        if ( error || outline.n_points <= 0 )
        {
          FT_TRACE5(( "  U+%04lX contains no outlines\n", ch ));
          continue;
        }

        /* now compute min or max point indices and coordinates */
        points     = outline.points;
        best_point = -1;
        best_pos   = 0;  /* make compiler happy */

        {
          FT_Int  nn;
          FT_Int  first = 0;
          FT_Int  last  = -1;


          for ( nn = 0; nn < outline.n_contours; first = last + 1, nn++ )
          {
            FT_Int  pp;


            last = outline.contours[nn];

            /* Avoid single-point contours since they are never rasterized. */
            /* In some fonts, they correspond to mark attachment points     */
            /* which are way outside of the glyph's real outline.           */
            if ( last <= first )
              continue;

            if ( AF_CJK_IS_HORIZ_BLUE( bs ) )
            {
              if ( AF_CJK_IS_RIGHT_BLUE( bs ) )
              {
                for ( pp = first; pp <= last; pp++ )
                  if ( best_point < 0 || points[pp].x > best_pos )
                  {
                    best_point = pp;
                    best_pos   = points[pp].x;
                  }
              }
              else
              {
                for ( pp = first; pp <= last; pp++ )
                  if ( best_point < 0 || points[pp].x < best_pos )
                  {
                    best_point = pp;
                    best_pos   = points[pp].x;
                  }
              }
            }
            else
            {
              if ( AF_CJK_IS_TOP_BLUE( bs ) )
              {
                for ( pp = first; pp <= last; pp++ )
                  if ( best_point < 0 || points[pp].y > best_pos )
                  {
                    best_point = pp;
                    best_pos   = points[pp].y;
                  }
              }
              else
              {
                for ( pp = first; pp <= last; pp++ )
                  if ( best_point < 0 || points[pp].y < best_pos )
                  {
                    best_point = pp;
                    best_pos   = points[pp].y;
                  }
              }
            }
          }

          FT_TRACE5(( "  U+%04lX: best_pos = %5ld\n", ch, best_pos ));
        }

        if ( AF_CJK_IS_FILLED_BLUE( bs ) )
          fills[num_fills++] = best_pos;
        else
          flats[num_flats++] = best_pos;
      }

      if ( num_flats == 0 && num_fills == 0 )
      {
        /*
         *  we couldn't find a single glyph to compute this blue zone,
         *  we will simply ignore it then
         */
        FT_TRACE5(( "    empty\n" ));
        continue;
      }

      /* we have computed the contents of the `fill' and `flats' tables, */
      /* now determine the reference position of the blue zone --        */
      /* we simply take the median value after a simple sort             */
      af_sort_pos( num_flats, flats );
      af_sort_pos( num_fills, fills );

      blue       = &axis->blues[axis->blue_count];
      blue_ref   = &blue->ref.org;
      blue_shoot = &blue->shoot.org;

      axis->blue_count++;

      if ( num_flats == 0 )
      {
        *blue_ref   =
        *blue_shoot = fills[num_fills / 2];
      }
      else if ( num_fills == 0 )
      {
        *blue_ref   =
        *blue_shoot = flats[num_flats / 2];
      }
      else
      {
        *blue_ref   = fills[num_fills / 2];
        *blue_shoot = flats[num_flats / 2];
      }

      /* make sure blue_ref >= blue_shoot for top/right or */
      /* vice versa for bottom/left                        */
      if ( *blue_shoot != *blue_ref )
      {
        FT_Pos   ref       = *blue_ref;
        FT_Pos   shoot     = *blue_shoot;
        FT_Bool  under_ref = FT_BOOL( shoot < ref );


        /* AF_CJK_IS_TOP_BLUE covers `right' and `top' */
        if ( AF_CJK_IS_TOP_BLUE( bs ) ^ under_ref )
        {
          *blue_ref   =
          *blue_shoot = ( shoot + ref ) / 2;

          FT_TRACE5(( "  [overshoot smaller than reference,"
                      " taking mean value]\n" ));
        }
      }

      blue->flags = 0;
      if ( AF_CJK_IS_TOP_BLUE( bs ) )
        blue->flags |= AF_CJK_BLUE_TOP;

      FT_TRACE5(( "    -> reference = %ld\n"
                  "       overshoot = %ld\n",
                  *blue_ref, *blue_shoot ));
    }

    FT_TRACE5(( "\n" ));

    return;
  }


  /* Basically the Latin version with type AF_CJKMetrics for metrics. */

  FT_LOCAL_DEF( void )
  af_cjk_metrics_check_digits( AF_CJKMetrics  metrics,
                               FT_Face        face )
  {
    FT_UInt   i;
    FT_Bool   started = 0, same_width = 1;
    FT_Fixed  advance, old_advance = 0;


    /* digit `0' is 0x30 in all supported charmaps */
    for ( i = 0x30; i <= 0x39; i++ )
    {
      FT_ULong  glyph_index;
      FT_Long   y_offset;


      af_get_char_index( &metrics->root, i, &glyph_index, &y_offset );
      if ( glyph_index == 0 )
        continue;

      if ( FT_Get_Advance( face, glyph_index,
                           FT_LOAD_NO_SCALE         |
                           FT_LOAD_NO_HINTING       |
                           FT_LOAD_IGNORE_TRANSFORM,
                           &advance ) )
        continue;

      if ( started )
      {
        if ( advance != old_advance )
        {
          same_width = 0;
          break;
        }
      }
      else
      {
        old_advance = advance;
        started     = 1;
      }
    }

    metrics->root.digits_have_same_width = same_width;
  }


  /* Initialize global metrics. */

  FT_LOCAL_DEF( FT_Error )
  af_cjk_metrics_init( AF_CJKMetrics  metrics,
                       FT_Face        face )
  {
    FT_CharMap  oldmap = face->charmap;


    metrics->units_per_em = face->units_per_EM;

    if ( !FT_Select_Charmap( face, FT_ENCODING_UNICODE ) )
    {
      af_cjk_metrics_init_widths( metrics, face );
      af_cjk_metrics_init_blues( metrics, face );
      af_cjk_metrics_check_digits( metrics, face );
    }

    FT_Set_Charmap( face, oldmap );
    return FT_Err_Ok;
  }


  /* Adjust scaling value, then scale and shift widths   */
  /* and blue zones (if applicable) for given dimension. */

  static void
  af_cjk_metrics_scale_dim( AF_CJKMetrics  metrics,
                            AF_Scaler      scaler,
                            AF_Dimension   dim )
  {
    FT_Fixed    scale;
    FT_Pos      delta;
    AF_CJKAxis  axis;
    FT_UInt     nn;


    if ( dim == AF_DIMENSION_HORZ )
    {
      scale = scaler->x_scale;
      delta = scaler->x_delta;
    }
    else
    {
      scale = scaler->y_scale;
      delta = scaler->y_delta;
    }

    axis = &metrics->axis[dim];

    if ( axis->org_scale == scale && axis->org_delta == delta )
      return;

    axis->org_scale = scale;
    axis->org_delta = delta;

    axis->scale = scale;
    axis->delta = delta;

    /* scale the blue zones */
    for ( nn = 0; nn < axis->blue_count; nn++ )
    {
      AF_CJKBlue  blue = &axis->blues[nn];
      FT_Pos      dist;


      blue->ref.cur   = FT_MulFix( blue->ref.org, scale ) + delta;
      blue->ref.fit   = blue->ref.cur;
      blue->shoot.cur = FT_MulFix( blue->shoot.org, scale ) + delta;
      blue->shoot.fit = blue->shoot.cur;
      blue->flags    &= ~AF_CJK_BLUE_ACTIVE;

      /* a blue zone is only active if it is less than 3/4 pixels tall */
      dist = FT_MulFix( blue->ref.org - blue->shoot.org, scale );
      if ( dist <= 48 && dist >= -48 )
      {
        FT_Pos  delta1, delta2;


        blue->ref.fit  = FT_PIX_ROUND( blue->ref.cur );

        /* shoot is under shoot for cjk */
        delta1 = FT_DivFix( blue->ref.fit, scale ) - blue->shoot.org;
        delta2 = delta1;
        if ( delta1 < 0 )
          delta2 = -delta2;

        delta2 = FT_MulFix( delta2, scale );

        FT_TRACE5(( "delta: %d", delta1 ));
        if ( delta2 < 32 )
          delta2 = 0;
#if 0
        else if ( delta2 < 64 )
          delta2 = 32 + ( ( ( delta2 - 32 ) + 16 ) & ~31 );
#endif
        else
          delta2 = FT_PIX_ROUND( delta2 );
        FT_TRACE5(( "/%d\n", delta2 ));

        if ( delta1 < 0 )
          delta2 = -delta2;

        blue->shoot.fit = blue->ref.fit - delta2;

        FT_TRACE5(( ">> active cjk blue zone %c%d[%ld/%ld]:\n"
                    "     ref:   cur=%.2f fit=%.2f\n"
                    "     shoot: cur=%.2f fit=%.2f\n",
                    ( dim == AF_DIMENSION_HORZ ) ? 'H' : 'V',
                    nn, blue->ref.org, blue->shoot.org,
                    blue->ref.cur / 64.0, blue->ref.fit / 64.0,
                    blue->shoot.cur / 64.0, blue->shoot.fit / 64.0 ));

        blue->flags |= AF_CJK_BLUE_ACTIVE;
      }
    }
  }


  /* Scale global values in both directions. */

  FT_LOCAL_DEF( void )
  af_cjk_metrics_scale( AF_CJKMetrics  metrics,
                        AF_Scaler      scaler )
  {
    /* we copy the whole structure since the x and y scaling values */
    /* are not modified, contrary to e.g. the `latin' auto-hinter   */
    metrics->root.scaler = *scaler;

    af_cjk_metrics_scale_dim( metrics, scaler, AF_DIMENSION_HORZ );
    af_cjk_metrics_scale_dim( metrics, scaler, AF_DIMENSION_VERT );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****              C J K   G L Y P H   A N A L Y S I S              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /* Walk over all contours and compute its segments. */

  static FT_Error
  af_cjk_hints_compute_segments( AF_GlyphHints  hints,
                                 AF_Dimension   dim )
  {
    AF_AxisHints  axis          = &hints->axis[dim];
    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    FT_Error      error;
    AF_Segment    seg;


    error = af_latin_hints_compute_segments( hints, dim );
    if ( error )
      return error;

    /* a segment is round if it doesn't have successive */
    /* on-curve points.                                 */
    for ( seg = segments; seg < segment_limit; seg++ )
    {
      AF_Point  pt   = seg->first;
      AF_Point  last = seg->last;
      AF_Flags  f0   = (AF_Flags)( pt->flags & AF_FLAG_CONTROL );
      AF_Flags  f1;


      seg->flags &= ~AF_EDGE_ROUND;

      for ( ; pt != last; f0 = f1 )
      {
        pt = pt->next;
        f1 = (AF_Flags)( pt->flags & AF_FLAG_CONTROL );

        if ( !f0 && !f1 )
          break;

        if ( pt == last )
          seg->flags |= AF_EDGE_ROUND;
      }
    }

    return FT_Err_Ok;
  }


  static void
  af_cjk_hints_link_segments( AF_GlyphHints  hints,
                              AF_Dimension   dim )
  {
    AF_AxisHints  axis          = &hints->axis[dim];
    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    AF_Direction  major_dir     = axis->major_dir;
    AF_Segment    seg1, seg2;
    FT_Pos        len_threshold;
    FT_Pos        dist_threshold;


    len_threshold = AF_LATIN_CONSTANT( hints->metrics, 8 );

    dist_threshold = ( dim == AF_DIMENSION_HORZ ) ? hints->x_scale
                                                  : hints->y_scale;
    dist_threshold = FT_DivFix( 64 * 3, dist_threshold );

    /* now compare each segment to the others */
    for ( seg1 = segments; seg1 < segment_limit; seg1++ )
    {
      /* the fake segments are for metrics hinting only */
      if ( seg1->first == seg1->last )
        continue;

      if ( seg1->dir != major_dir )
        continue;

      for ( seg2 = segments; seg2 < segment_limit; seg2++ )
        if ( seg2 != seg1 && seg1->dir + seg2->dir == 0 )
        {
          FT_Pos  dist = seg2->pos - seg1->pos;


          if ( dist < 0 )
            continue;

          {
            FT_Pos  min = seg1->min_coord;
            FT_Pos  max = seg1->max_coord;
            FT_Pos  len;


            if ( min < seg2->min_coord )
              min = seg2->min_coord;

            if ( max > seg2->max_coord )
              max = seg2->max_coord;

            len = max - min;
            if ( len >= len_threshold )
            {
              if ( dist * 8 < seg1->score * 9                        &&
                   ( dist * 8 < seg1->score * 7 || seg1->len < len ) )
              {
                seg1->score = dist;
                seg1->len   = len;
                seg1->link  = seg2;
              }

              if ( dist * 8 < seg2->score * 9                        &&
                   ( dist * 8 < seg2->score * 7 || seg2->len < len ) )
              {
                seg2->score = dist;
                seg2->len   = len;
                seg2->link  = seg1;
              }
            }
          }
        }
    }

    /*
     *  now compute the `serif' segments
     *
     *  In Hanzi, some strokes are wider on one or both of the ends.
     *  We either identify the stems on the ends as serifs or remove
     *  the linkage, depending on the length of the stems.
     *
     */

    {
      AF_Segment  link1, link2;


      for ( seg1 = segments; seg1 < segment_limit; seg1++ )
      {
        link1 = seg1->link;
        if ( !link1 || link1->link != seg1 || link1->pos <= seg1->pos )
          continue;

        if ( seg1->score >= dist_threshold )
          continue;

        for ( seg2 = segments; seg2 < segment_limit; seg2++ )
        {
          if ( seg2->pos > seg1->pos || seg1 == seg2 )
            continue;

          link2 = seg2->link;
          if ( !link2 || link2->link != seg2 || link2->pos < link1->pos )
            continue;

          if ( seg1->pos == seg2->pos && link1->pos == link2->pos )
            continue;

          if ( seg2->score <= seg1->score || seg1->score * 4 <= seg2->score )
            continue;

          /* seg2 < seg1 < link1 < link2 */

          if ( seg1->len >= seg2->len * 3 )
          {
            AF_Segment  seg;


            for ( seg = segments; seg < segment_limit; seg++ )
            {
              AF_Segment  link = seg->link;


              if ( link == seg2 )
              {
                seg->link  = 0;
                seg->serif = link1;
              }
              else if ( link == link2 )
              {
                seg->link  = 0;
                seg->serif = seg1;
              }
            }
          }
          else
          {
            seg1->link = link1->link = 0;

            break;
          }
        }
      }
    }

    for ( seg1 = segments; seg1 < segment_limit; seg1++ )
    {
      seg2 = seg1->link;

      if ( seg2 )
      {
        seg2->num_linked++;
        if ( seg2->link != seg1 )
        {
          seg1->link = 0;

          if ( seg2->score < dist_threshold || seg1->score < seg2->score * 4 )
            seg1->serif = seg2->link;
          else
            seg2->num_linked--;
        }
      }
    }
  }


  static FT_Error
  af_cjk_hints_compute_edges( AF_GlyphHints  hints,
                              AF_Dimension   dim )
  {
    AF_AxisHints  axis   = &hints->axis[dim];
    FT_Error      error  = FT_Err_Ok;
    FT_Memory     memory = hints->memory;
    AF_CJKAxis    laxis  = &((AF_CJKMetrics)hints->metrics)->axis[dim];

    AF_Segment    segments      = axis->segments;
    AF_Segment    segment_limit = segments + axis->num_segments;
    AF_Segment    seg;

    FT_Fixed      scale;
    FT_Pos        edge_distance_threshold;


    axis->num_edges = 0;

    scale = ( dim == AF_DIMENSION_HORZ ) ? hints->x_scale
                                         : hints->y_scale;

    /*********************************************************************/
    /*                                                                   */
    /* We begin by generating a sorted table of edges for the current    */
    /* direction.  To do so, we simply scan each segment and try to find */
    /* an edge in our table that corresponds to its position.            */
    /*                                                                   */
    /* If no edge is found, we create and insert a new edge in the       */
    /* sorted table.  Otherwise, we simply add the segment to the edge's */
    /* list which is then processed in the second step to compute the    */
    /* edge's properties.                                                */
    /*                                                                   */
    /* Note that the edges table is sorted along the segment/edge        */
    /* position.                                                         */
    /*                                                                   */
    /*********************************************************************/

    edge_distance_threshold = FT_MulFix( laxis->edge_distance_threshold,
                                         scale );
    if ( edge_distance_threshold > 64 / 4 )
      edge_distance_threshold = FT_DivFix( 64 / 4, scale );
    else
      edge_distance_threshold = laxis->edge_distance_threshold;

    for ( seg = segments; seg < segment_limit; seg++ )
    {
      AF_Edge  found = NULL;
      FT_Pos   best  = 0xFFFFU;
      FT_Int   ee;


      /* look for an edge corresponding to the segment */
      for ( ee = 0; ee < axis->num_edges; ee++ )
      {
        AF_Edge  edge = axis->edges + ee;
        FT_Pos   dist;


        if ( edge->dir != seg->dir )
          continue;

        dist = seg->pos - edge->fpos;
        if ( dist < 0 )
          dist = -dist;

        if ( dist < edge_distance_threshold && dist < best )
        {
          AF_Segment  link = seg->link;


          /* check whether all linked segments of the candidate edge */
          /* can make a single edge.                                 */
          if ( link )
          {
            AF_Segment  seg1  = edge->first;
            FT_Pos      dist2 = 0;


            do
            {
              AF_Segment  link1 = seg1->link;


              if ( link1 )
              {
                dist2 = AF_SEGMENT_DIST( link, link1 );
                if ( dist2 >= edge_distance_threshold )
                  break;
              }

            } while ( ( seg1 = seg1->edge_next ) != edge->first );

            if ( dist2 >= edge_distance_threshold )
              continue;
          }

          best  = dist;
          found = edge;
        }
      }

      if ( !found )
      {
        AF_Edge  edge;


        /* insert a new edge in the list and */
        /* sort according to the position    */
        error = af_axis_hints_new_edge( axis, seg->pos,
                                        (AF_Direction)seg->dir,
                                        memory, &edge );
        if ( error )
          goto Exit;

        /* add the segment to the new edge's list */
        FT_ZERO( edge );

        edge->first    = seg;
        edge->last     = seg;
        edge->fpos     = seg->pos;
        edge->opos     = edge->pos = FT_MulFix( seg->pos, scale );
        seg->edge_next = seg;
        edge->dir      = seg->dir;
      }
      else
      {
        /* if an edge was found, simply add the segment to the edge's */
        /* list                                                       */
        seg->edge_next         = found->first;
        found->last->edge_next = seg;
        found->last            = seg;
      }
    }

    /******************************************************************/
    /*                                                                */
    /* Good, we now compute each edge's properties according to the   */
    /* segments found on its position.  Basically, these are          */
    /*                                                                */
    /*  - the edge's main direction                                   */
    /*  - stem edge, serif edge or both (which defaults to stem then) */
    /*  - rounded edge, straight or both (which defaults to straight) */
    /*  - link for edge                                               */
    /*                                                                */
    /******************************************************************/

    /* first of all, set the `edge' field in each segment -- this is */
    /* required in order to compute edge links                       */

    /*
     * Note that removing this loop and setting the `edge' field of each
     * segment directly in the code above slows down execution speed for
     * some reasons on platforms like the Sun.
     */
    {
      AF_Edge  edges      = axis->edges;
      AF_Edge  edge_limit = edges + axis->num_edges;
      AF_Edge  edge;


      for ( edge = edges; edge < edge_limit; edge++ )
      {
        seg = edge->first;
        if ( seg )
          do
          {
            seg->edge = edge;
            seg       = seg->edge_next;

          } while ( seg != edge->first );
      }

      /* now compute each edge properties */
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        FT_Int  is_round    = 0;  /* does it contain round segments?    */
        FT_Int  is_straight = 0;  /* does it contain straight segments? */


        seg = edge->first;

        do
        {
          FT_Bool  is_serif;


          /* check for roundness of segment */
          if ( seg->flags & AF_EDGE_ROUND )
            is_round++;
          else
            is_straight++;

          /* check for links -- if seg->serif is set, then seg->link must */
          /* be ignored                                                   */
          is_serif = (FT_Bool)( seg->serif && seg->serif->edge != edge );

          if ( seg->link || is_serif )
          {
            AF_Edge     edge2;
            AF_Segment  seg2;


            edge2 = edge->link;
            seg2  = seg->link;

            if ( is_serif )
            {
              seg2  = seg->serif;
              edge2 = edge->serif;
            }

            if ( edge2 )
            {
              FT_Pos  edge_delta;
              FT_Pos  seg_delta;


              edge_delta = edge->fpos - edge2->fpos;
              if ( edge_delta < 0 )
                edge_delta = -edge_delta;

              seg_delta = AF_SEGMENT_DIST( seg, seg2 );

              if ( seg_delta < edge_delta )
                edge2 = seg2->edge;
            }
            else
              edge2 = seg2->edge;

            if ( is_serif )
            {
              edge->serif   = edge2;
              edge2->flags |= AF_EDGE_SERIF;
            }
            else
              edge->link  = edge2;
          }

          seg = seg->edge_next;

        } while ( seg != edge->first );

        /* set the round/straight flags */
        edge->flags = AF_EDGE_NORMAL;

        if ( is_round > 0 && is_round >= is_straight )
          edge->flags |= AF_EDGE_ROUND;

        /* get rid of serifs if link is set                 */
        /* XXX: This gets rid of many unpleasant artefacts! */
        /*      Example: the `c' in cour.pfa at size 13     */

        if ( edge->serif && edge->link )
          edge->serif = 0;
      }
    }

  Exit:
    return error;
  }


  /* Detect segments and edges for given dimension. */

  static FT_Error
  af_cjk_hints_detect_features( AF_GlyphHints  hints,
                                AF_Dimension   dim )
  {
    FT_Error  error;


    error = af_cjk_hints_compute_segments( hints, dim );
    if ( !error )
    {
      af_cjk_hints_link_segments( hints, dim );

      error = af_cjk_hints_compute_edges( hints, dim );
    }
    return error;
  }


  /* Compute all edges which lie within blue zones. */

  FT_LOCAL_DEF( void )
  af_cjk_hints_compute_blue_edges( AF_GlyphHints  hints,
                                   AF_CJKMetrics  metrics,
                                   AF_Dimension   dim )
  {
    AF_AxisHints  axis       = &hints->axis[dim];
    AF_Edge       edge       = axis->edges;
    AF_Edge       edge_limit = edge + axis->num_edges;
    AF_CJKAxis    cjk        = &metrics->axis[dim];
    FT_Fixed      scale      = cjk->scale;
    FT_Pos        best_dist0;  /* initial threshold */


    /* compute the initial threshold as a fraction of the EM size */
    best_dist0 = FT_MulFix( metrics->units_per_em / 40, scale );

    if ( best_dist0 > 64 / 2 ) /* maximum 1/2 pixel */
      best_dist0 = 64 / 2;

    /* compute which blue zones are active, i.e. have their scaled */
    /* size < 3/4 pixels                                           */

    /* If the distant between an edge and a blue zone is shorter than */
    /* best_dist0, set the blue zone for the edge.  Then search for   */
    /* the blue zone with the smallest best_dist to the edge.         */

    for ( ; edge < edge_limit; edge++ )
    {
      FT_UInt   bb;
      AF_Width  best_blue = NULL;
      FT_Pos    best_dist = best_dist0;


      for ( bb = 0; bb < cjk->blue_count; bb++ )
      {
        AF_CJKBlue  blue = cjk->blues + bb;
        FT_Bool     is_top_right_blue, is_major_dir;


        /* skip inactive blue zones (i.e., those that are too small) */
        if ( !( blue->flags & AF_CJK_BLUE_ACTIVE ) )
          continue;

        /* if it is a top zone, check for right edges -- if it is a bottom */
        /* zone, check for left edges                                      */
        /*                                                                 */
        /* of course, that's for TrueType                                  */
        is_top_right_blue = FT_BOOL( blue->flags & AF_CJK_BLUE_TOP );
        is_major_dir      = FT_BOOL( edge->dir == axis->major_dir );

        /* if it is a top zone, the edge must be against the major    */
        /* direction; if it is a bottom zone, it must be in the major */
        /* direction                                                  */
        if ( is_top_right_blue ^ is_major_dir )
        {
          FT_Pos    dist;
          AF_Width  compare;


          /* Compare the edge to the closest blue zone type */
          if ( FT_ABS( edge->fpos - blue->ref.org ) >
               FT_ABS( edge->fpos - blue->shoot.org ) )
            compare = &blue->shoot;
          else
            compare = &blue->ref;

          dist = edge->fpos - compare->org;
          if ( dist < 0 )
            dist = -dist;

          dist = FT_MulFix( dist, scale );
          if ( dist < best_dist )
          {
            best_dist = dist;
            best_blue = compare;
          }
        }
      }

      if ( best_blue )
        edge->blue_edge = best_blue;
    }
  }


  /* Initalize hinting engine. */

  FT_LOCAL_DEF( FT_Error )
  af_cjk_hints_init( AF_GlyphHints  hints,
                     AF_CJKMetrics  metrics )
  {
    FT_Render_Mode  mode;
    FT_UInt32       scaler_flags, other_flags;


    af_glyph_hints_rescale( hints, (AF_StyleMetrics)metrics );

    /*
     *  correct x_scale and y_scale when needed, since they may have
     *  been modified af_cjk_scale_dim above
     */
    hints->x_scale = metrics->axis[AF_DIMENSION_HORZ].scale;
    hints->x_delta = metrics->axis[AF_DIMENSION_HORZ].delta;
    hints->y_scale = metrics->axis[AF_DIMENSION_VERT].scale;
    hints->y_delta = metrics->axis[AF_DIMENSION_VERT].delta;

    /* compute flags depending on render mode, etc. */
    mode = metrics->root.scaler.render_mode;

#ifdef AF_CONFIG_OPTION_USE_WARPER
    if ( mode == FT_RENDER_MODE_LCD || mode == FT_RENDER_MODE_LCD_V )
      metrics->root.scaler.render_mode = mode = FT_RENDER_MODE_NORMAL;
#endif

    scaler_flags = hints->scaler_flags;
    other_flags  = 0;

    /*
     *  We snap the width of vertical stems for the monochrome and
     *  horizontal LCD rendering targets only.
     */
    if ( mode == FT_RENDER_MODE_MONO || mode == FT_RENDER_MODE_LCD )
      other_flags |= AF_LATIN_HINTS_HORZ_SNAP;

    /*
     *  We snap the width of horizontal stems for the monochrome and
     *  vertical LCD rendering targets only.
     */
    if ( mode == FT_RENDER_MODE_MONO || mode == FT_RENDER_MODE_LCD_V )
      other_flags |= AF_LATIN_HINTS_VERT_SNAP;

    /*
     *  We adjust stems to full pixels only if we don't use the `light' mode.
     */
    if ( mode != FT_RENDER_MODE_LIGHT )
      other_flags |= AF_LATIN_HINTS_STEM_ADJUST;

    if ( mode == FT_RENDER_MODE_MONO )
      other_flags |= AF_LATIN_HINTS_MONO;

    scaler_flags |= AF_SCALER_FLAG_NO_ADVANCE;

    hints->scaler_flags = scaler_flags;
    hints->other_flags  = other_flags;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****          C J K   G L Y P H   G R I D - F I T T I N G          *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* Snap a given width in scaled coordinates to one of the */
  /* current standard widths.                               */

  static FT_Pos
  af_cjk_snap_width( AF_Width  widths,
                     FT_Int    count,
                     FT_Pos    width )
  {
    int     n;
    FT_Pos  best      = 64 + 32 + 2;
    FT_Pos  reference = width;
    FT_Pos  scaled;


    for ( n = 0; n < count; n++ )
    {
      FT_Pos  w;
      FT_Pos  dist;


      w = widths[n].cur;
      dist = width - w;
      if ( dist < 0 )
        dist = -dist;
      if ( dist < best )
      {
        best      = dist;
        reference = w;
      }
    }

    scaled = FT_PIX_ROUND( reference );

    if ( width >= reference )
    {
      if ( width < scaled + 48 )
        width = reference;
    }
    else
    {
      if ( width > scaled - 48 )
        width = reference;
    }

    return width;
  }


  /* Compute the snapped width of a given stem.                          */
  /* There is a lot of voodoo in this function; changing the hard-coded  */
  /* parameters influence the whole hinting process.                     */

  static FT_Pos
  af_cjk_compute_stem_width( AF_GlyphHints  hints,
                             AF_Dimension   dim,
                             FT_Pos         width,
                             AF_Edge_Flags  base_flags,
                             AF_Edge_Flags  stem_flags )
  {
    AF_CJKMetrics  metrics  = (AF_CJKMetrics)hints->metrics;
    AF_CJKAxis     axis     = &metrics->axis[dim];
    FT_Pos         dist     = width;
    FT_Int         sign     = 0;
    FT_Bool        vertical = FT_BOOL( dim == AF_DIMENSION_VERT );

    FT_UNUSED( base_flags );
    FT_UNUSED( stem_flags );


    if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) )
      return width;

    if ( dist < 0 )
    {
      dist = -width;
      sign = 1;
    }

    if ( (  vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) ) ||
         ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) )
    {
      /* smooth hinting process: very lightly quantize the stem width */

      if ( axis->width_count > 0 )
      {
        if ( FT_ABS( dist - axis->widths[0].cur ) < 40 )
        {
          dist = axis->widths[0].cur;
          if ( dist < 48 )
            dist = 48;

          goto Done_Width;
        }
      }

      if ( dist < 54 )
        dist += ( 54 - dist ) / 2 ;
      else if ( dist < 3 * 64 )
      {
        FT_Pos  delta;


        delta  = dist & 63;
        dist  &= -64;

        if ( delta < 10 )
          dist += delta;
        else if ( delta < 22 )
          dist += 10;
        else if ( delta < 42 )
          dist += delta;
        else if ( delta < 54 )
          dist += 54;
        else
          dist += delta;
      }
    }
    else
    {
      /* strong hinting process: snap the stem width to integer pixels */

      dist = af_cjk_snap_width( axis->widths, axis->width_count, dist );

      if ( vertical )
      {
        /* in the case of vertical hinting, always round */
        /* the stem heights to integer pixels            */

        if ( dist >= 64 )
          dist = ( dist + 16 ) & ~63;
        else
          dist = 64;
      }
      else
      {
        if ( AF_LATIN_HINTS_DO_MONO( hints ) )
        {
          /* monochrome horizontal hinting: snap widths to integer pixels */
          /* with a different threshold                                   */

          if ( dist < 64 )
            dist = 64;
          else
            dist = ( dist + 32 ) & ~63;
        }
        else
        {
          /* for horizontal anti-aliased hinting, we adopt a more subtle */
          /* approach: we strengthen small stems, round stems whose size */
          /* is between 1 and 2 pixels to an integer, otherwise nothing  */

          if ( dist < 48 )
            dist = ( dist + 64 ) >> 1;

          else if ( dist < 128 )
            dist = ( dist + 22 ) & ~63;
          else
            /* round otherwise to prevent color fringes in LCD mode */
            dist = ( dist + 32 ) & ~63;
        }
      }
    }

  Done_Width:
    if ( sign )
      dist = -dist;

    return dist;
  }


  /* Align one stem edge relative to the previous stem edge. */

  static void
  af_cjk_align_linked_edge( AF_GlyphHints  hints,
                            AF_Dimension   dim,
                            AF_Edge        base_edge,
                            AF_Edge        stem_edge )
  {
    FT_Pos  dist = stem_edge->opos - base_edge->opos;

    FT_Pos  fitted_width = af_cjk_compute_stem_width(
                             hints, dim, dist,
                             (AF_Edge_Flags)base_edge->flags,
                             (AF_Edge_Flags)stem_edge->flags );


    stem_edge->pos = base_edge->pos + fitted_width;
  }


  /* Shift the coordinates of the `serif' edge by the same amount */
  /* as the corresponding `base' edge has been moved already.     */

  static void
  af_cjk_align_serif_edge( AF_GlyphHints  hints,
                           AF_Edge        base,
                           AF_Edge        serif )
  {
    FT_UNUSED( hints );

    serif->pos = base->pos + ( serif->opos - base->opos );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                    E D G E   H I N T I N G                      ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


#define AF_LIGHT_MODE_MAX_HORZ_GAP    9
#define AF_LIGHT_MODE_MAX_VERT_GAP   15
#define AF_LIGHT_MODE_MAX_DELTA_ABS  14


  static FT_Pos
  af_hint_normal_stem( AF_GlyphHints  hints,
                       AF_Edge        edge,
                       AF_Edge        edge2,
                       FT_Pos         anchor,
                       AF_Dimension   dim )
  {
    FT_Pos  org_len, cur_len, org_center;
    FT_Pos  cur_pos1, cur_pos2;
    FT_Pos  d_off1, u_off1, d_off2, u_off2, delta;
    FT_Pos  offset;
    FT_Pos  threshold = 64;


    if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) )
    {
      if ( ( edge->flags  & AF_EDGE_ROUND ) &&
           ( edge2->flags & AF_EDGE_ROUND ) )
      {
        if ( dim == AF_DIMENSION_VERT )
          threshold = 64 - AF_LIGHT_MODE_MAX_HORZ_GAP;
        else
          threshold = 64 - AF_LIGHT_MODE_MAX_VERT_GAP;
      }
      else
      {
        if ( dim == AF_DIMENSION_VERT )
          threshold = 64 - AF_LIGHT_MODE_MAX_HORZ_GAP / 3;
        else
          threshold = 64 - AF_LIGHT_MODE_MAX_VERT_GAP / 3;
      }
    }

    org_len    = edge2->opos - edge->opos;
    cur_len    = af_cjk_compute_stem_width( hints, dim, org_len,
                                            (AF_Edge_Flags)edge->flags,
                                            (AF_Edge_Flags)edge2->flags );

    org_center = ( edge->opos + edge2->opos ) / 2 + anchor;
    cur_pos1   = org_center - cur_len / 2;
    cur_pos2   = cur_pos1 + cur_len;
    d_off1     = cur_pos1 - FT_PIX_FLOOR( cur_pos1 );
    d_off2     = cur_pos2 - FT_PIX_FLOOR( cur_pos2 );
    u_off1     = 64 - d_off1;
    u_off2     = 64 - d_off2;
    delta      = 0;


    if ( d_off1 == 0 || d_off2 == 0 )
      goto Exit;

    if ( cur_len <= threshold )
    {
      if ( d_off2 < cur_len )
      {
        if ( u_off1 <= d_off2 )
          delta =  u_off1;
        else
          delta = -d_off2;
      }

      goto Exit;
    }

    if ( threshold < 64 )
    {
      if ( d_off1 >= threshold || u_off1 >= threshold ||
           d_off2 >= threshold || u_off2 >= threshold )
        goto Exit;
    }

    offset = cur_len & 63;

    if ( offset < 32 )
    {
      if ( u_off1 <= offset || d_off2 <= offset )
        goto Exit;
    }
    else
      offset = 64 - threshold;

    d_off1 = threshold - u_off1;
    u_off1 = u_off1    - offset;
    u_off2 = threshold - d_off2;
    d_off2 = d_off2    - offset;

    if ( d_off1 <= u_off1 )
      u_off1 = -d_off1;

    if ( d_off2 <= u_off2 )
      u_off2 = -d_off2;

    if ( FT_ABS( u_off1 ) <= FT_ABS( u_off2 ) )
      delta = u_off1;
    else
      delta = u_off2;

  Exit:

#if 1
    if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) )
    {
      if ( delta > AF_LIGHT_MODE_MAX_DELTA_ABS )
        delta = AF_LIGHT_MODE_MAX_DELTA_ABS;
      else if ( delta < -AF_LIGHT_MODE_MAX_DELTA_ABS )
        delta = -AF_LIGHT_MODE_MAX_DELTA_ABS;
    }
#endif

    cur_pos1 += delta;

    if ( edge->opos < edge2->opos )
    {
      edge->pos  = cur_pos1;
      edge2->pos = cur_pos1 + cur_len;
    }
    else
    {
      edge->pos  = cur_pos1 + cur_len;
      edge2->pos = cur_pos1;
    }

    return delta;
  }


  /* The main grid-fitting routine. */

  static void
  af_cjk_hint_edges( AF_GlyphHints  hints,
                     AF_Dimension   dim )
  {
    AF_AxisHints  axis       = &hints->axis[dim];
    AF_Edge       edges      = axis->edges;
    AF_Edge       edge_limit = edges + axis->num_edges;
    FT_PtrDist    n_edges;
    AF_Edge       edge;
    AF_Edge       anchor   = 0;
    FT_Pos        delta    = 0;
    FT_Int        skipped  = 0;
    FT_Bool       has_last_stem = FALSE;
    FT_Pos        last_stem_pos = 0;

#ifdef FT_DEBUG_LEVEL_TRACE
    FT_UInt       num_actions = 0;
#endif


    FT_TRACE5(( "cjk %s edge hinting (style `%s')\n",
                dim == AF_DIMENSION_VERT ? "horizontal" : "vertical",
                af_style_names[hints->metrics->style_class->style] ));

    /* we begin by aligning all stems relative to the blue zone */

    if ( AF_HINTS_DO_BLUES( hints ) )
    {
      for ( edge = edges; edge < edge_limit; edge++ )
      {
        AF_Width  blue;
        AF_Edge   edge1, edge2;


        if ( edge->flags & AF_EDGE_DONE )
          continue;

        blue  = edge->blue_edge;
        edge1 = NULL;
        edge2 = edge->link;

        if ( blue )
        {
          edge1 = edge;
        }
        else if ( edge2 && edge2->blue_edge )
        {
          blue  = edge2->blue_edge;
          edge1 = edge2;
          edge2 = edge;
        }

        if ( !edge1 )
          continue;

#ifdef FT_DEBUG_LEVEL_TRACE
        FT_TRACE5(( "  CJKBLUE: edge %d @%d (opos=%.2f) snapped to %.2f,"
                    " was %.2f\n",
                    edge1 - edges, edge1->fpos, edge1->opos / 64.0,
                    blue->fit / 64.0, edge1->pos / 64.0 ));

        num_actions++;
#endif

        edge1->pos    = blue->fit;
        edge1->flags |= AF_EDGE_DONE;

        if ( edge2 && !edge2->blue_edge )
        {
          af_cjk_align_linked_edge( hints, dim, edge1, edge2 );
          edge2->flags |= AF_EDGE_DONE;

#ifdef FT_DEBUG_LEVEL_TRACE
          num_actions++;
#endif
        }

        if ( !anchor )
          anchor = edge;
      }
    }

    /* now we align all stem edges. */
    for ( edge = edges; edge < edge_limit; edge++ )
    {
      AF_Edge  edge2;


      if ( edge->flags & AF_EDGE_DONE )
        continue;

      /* skip all non-stem edges */
      edge2 = edge->link;
      if ( !edge2 )
      {
        skipped++;
        continue;
      }

      /* Some CJK characters have so many stems that
       * the hinter is likely to merge two adjacent ones.
       * To solve this problem, if either edge of a stem
       * is too close to the previous one, we avoid
       * aligning the two edges, but rather interpolate
       * their locations at the end of this function in
       * order to preserve the space between the stems.
       */
      if ( has_last_stem                       &&
           ( edge->pos  < last_stem_pos + 64 ||
             edge2->pos < last_stem_pos + 64 ) )
      {
        skipped++;
        continue;
      }

      /* now align the stem */

      /* this should not happen, but it's better to be safe */
      if ( edge2->blue_edge )
      {
        FT_TRACE5(( "ASSERTION FAILED for edge %d\n", edge2-edges ));

        af_cjk_align_linked_edge( hints, dim, edge2, edge );
        edge->flags |= AF_EDGE_DONE;

#ifdef FT_DEBUG_LEVEL_TRACE
        num_actions++;
#endif

        continue;
      }

      if ( edge2 < edge )
      {
        af_cjk_align_linked_edge( hints, dim, edge2, edge );
        edge->flags |= AF_EDGE_DONE;

#ifdef FT_DEBUG_LEVEL_TRACE
        num_actions++;
#endif

        /* We rarely reaches here it seems;
         * usually the two edges belonging
         * to one stem are marked as DONE together
         */
        has_last_stem = TRUE;
        last_stem_pos = edge->pos;
        continue;
      }

      if ( dim != AF_DIMENSION_VERT && !anchor )
      {

#if 0
        if ( fixedpitch )
        {
          AF_Edge     left  = edge;
          AF_Edge     right = edge_limit - 1;
          AF_EdgeRec  left1, left2, right1, right2;
          FT_Pos      target, center1, center2;
          FT_Pos      delta1, delta2, d1, d2;


          while ( right > left && !right->link )
            right--;

          left1  = *left;
          left2  = *left->link;
          right1 = *right->link;
          right2 = *right;

          delta  = ( ( ( hinter->pp2.x + 32 ) & -64 ) - hinter->pp2.x ) / 2;
          target = left->opos + ( right->opos - left->opos ) / 2 + delta - 16;

          delta1  = delta;
          delta1 += af_hint_normal_stem( hints, left, left->link,
                                         delta1, 0 );

          if ( left->link != right )
            af_hint_normal_stem( hints, right->link, right, delta1, 0 );

          center1 = left->pos + ( right->pos - left->pos ) / 2;

          if ( center1 >= target )
            delta2 = delta - 32;
          else
            delta2 = delta + 32;

          delta2 += af_hint_normal_stem( hints, &left1, &left2, delta2, 0 );

          if ( delta1 != delta2 )
          {
            if ( left->link != right )
              af_hint_normal_stem( hints, &right1, &right2, delta2, 0 );

            center2 = left1.pos + ( right2.pos - left1.pos ) / 2;

            d1 = center1 - target;
            d2 = center2 - target;

            if ( FT_ABS( d2 ) < FT_ABS( d1 ) )
            {
              left->pos       = left1.pos;
              left->link->pos = left2.pos;

              if ( left->link != right )
              {
                right->link->pos = right1.pos;
                right->pos       = right2.pos;
              }

              delta1 = delta2;
            }
          }

          delta               = delta1;
          right->link->flags |= AF_EDGE_DONE;
          right->flags       |= AF_EDGE_DONE;
        }
        else

#endif /* 0 */

          delta = af_hint_normal_stem( hints, edge, edge2, 0,
                                       AF_DIMENSION_HORZ );
      }
      else
        af_hint_normal_stem( hints, edge, edge2, delta, dim );

#if 0
      printf( "stem (%d,%d) adjusted (%.1f,%.1f)\n",
               edge - edges, edge2 - edges,
               ( edge->pos - edge->opos ) / 64.0,
               ( edge2->pos - edge2->opos ) / 64.0 );
#endif

      anchor = edge;
      edge->flags  |= AF_EDGE_DONE;
      edge2->flags |= AF_EDGE_DONE;
      has_last_stem = TRUE;
      last_stem_pos = edge2->pos;
    }

    /* make sure that lowercase m's maintain their symmetry */

    /* In general, lowercase m's have six vertical edges if they are sans */
    /* serif, or twelve if they are with serifs.  This implementation is  */
    /* based on that assumption, and seems to work very well with most    */
    /* faces.  However, if for a certain face this assumption is not      */
    /* true, the m is just rendered like before.  In addition, any stem   */
    /* correction will only be applied to symmetrical glyphs (even if the */
    /* glyph is not an m), so the potential for unwanted distortion is    */
    /* relatively low.                                                    */

    /* We don't handle horizontal edges since we can't easily assure that */
    /* the third (lowest) stem aligns with the base line; it might end up */
    /* one pixel higher or lower.                                         */

    n_edges = edge_limit - edges;
    if ( dim == AF_DIMENSION_HORZ && ( n_edges == 6 || n_edges == 12 ) )
    {
      AF_Edge  edge1, edge2, edge3;
      FT_Pos   dist1, dist2, span;


      if ( n_edges == 6 )
      {
        edge1 = edges;
        edge2 = edges + 2;
        edge3 = edges + 4;
      }
      else
      {
        edge1 = edges + 1;
        edge2 = edges + 5;
        edge3 = edges + 9;
      }

      dist1 = edge2->opos - edge1->opos;
      dist2 = edge3->opos - edge2->opos;

      span = dist1 - dist2;
      if ( span < 0 )
        span = -span;

      if ( edge1->link == edge1 + 1 &&
           edge2->link == edge2 + 1 &&
           edge3->link == edge3 + 1 && span < 8 )
      {
        delta = edge3->pos - ( 2 * edge2->pos - edge1->pos );
        edge3->pos -= delta;
        if ( edge3->link )
          edge3->link->pos -= delta;

        /* move the serifs along with the stem */
        if ( n_edges == 12 )
        {
          ( edges + 8 )->pos -= delta;
          ( edges + 11 )->pos -= delta;
        }

        edge3->flags |= AF_EDGE_DONE;
        if ( edge3->link )
          edge3->link->flags |= AF_EDGE_DONE;
      }
    }

    if ( !skipped )
      goto Exit;

    /*
     *  now hint the remaining edges (serifs and single) in order
     *  to complete our processing
     */
    for ( edge = edges; edge < edge_limit; edge++ )
    {
      if ( edge->flags & AF_EDGE_DONE )
        continue;

      if ( edge->serif )
      {
        af_cjk_align_serif_edge( hints, edge->serif, edge );
        edge->flags |= AF_EDGE_DONE;
        skipped--;
      }
    }

    if ( !skipped )
      goto Exit;

    for ( edge = edges; edge < edge_limit; edge++ )
    {
      AF_Edge  before, after;


      if ( edge->flags & AF_EDGE_DONE )
        continue;

      before = after = edge;

      while ( --before >= edges )
        if ( before->flags & AF_EDGE_DONE )
          break;

      while ( ++after < edge_limit )
        if ( after->flags & AF_EDGE_DONE )
          break;

      if ( before >= edges || after < edge_limit )
      {
        if ( before < edges )
          af_cjk_align_serif_edge( hints, after, edge );
        else if ( after >= edge_limit )
          af_cjk_align_serif_edge( hints, before, edge );
        else
        {
          if ( after->fpos == before->fpos )
            edge->pos = before->pos;
          else
            edge->pos = before->pos +
                        FT_MulDiv( edge->fpos - before->fpos,
                                   after->pos - before->pos,
                                   after->fpos - before->fpos );
        }
      }
    }

  Exit:

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( !num_actions )
      FT_TRACE5(( "  (none)\n" ));
    FT_TRACE5(( "\n" ));
#endif

    return;
  }


  static void
  af_cjk_align_edge_points( AF_GlyphHints  hints,
                            AF_Dimension   dim )
  {
    AF_AxisHints  axis       = & hints->axis[dim];
    AF_Edge       edges      = axis->edges;
    AF_Edge       edge_limit = edges + axis->num_edges;
    AF_Edge       edge;
    FT_Bool       snapping;


    snapping = FT_BOOL( ( dim == AF_DIMENSION_HORZ             &&
                          AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) )  ||
                        ( dim == AF_DIMENSION_VERT             &&
                          AF_LATIN_HINTS_DO_VERT_SNAP( hints ) )  );

    for ( edge = edges; edge < edge_limit; edge++ )
    {
      /* move the points of each segment     */
      /* in each edge to the edge's position */
      AF_Segment  seg = edge->first;


      if ( snapping )
      {
        do
        {
          AF_Point  point = seg->first;


          for (;;)
          {
            if ( dim == AF_DIMENSION_HORZ )
            {
              point->x      = edge->pos;
              point->flags |= AF_FLAG_TOUCH_X;
            }
            else
            {
              point->y      = edge->pos;
              point->flags |= AF_FLAG_TOUCH_Y;
            }

            if ( point == seg->last )
              break;

            point = point->next;
          }

          seg = seg->edge_next;

        } while ( seg != edge->first );
      }
      else
      {
        FT_Pos  delta = edge->pos - edge->opos;


        do
        {
          AF_Point  point = seg->first;


          for (;;)
          {
            if ( dim == AF_DIMENSION_HORZ )
            {
              point->x     += delta;
              point->flags |= AF_FLAG_TOUCH_X;
            }
            else
            {
              point->y     += delta;
              point->flags |= AF_FLAG_TOUCH_Y;
            }

            if ( point == seg->last )
              break;

            point = point->next;
          }

          seg = seg->edge_next;

        } while ( seg != edge->first );
      }
    }
  }


  /* Apply the complete hinting algorithm to a CJK glyph. */

  FT_LOCAL_DEF( FT_Error )
  af_cjk_hints_apply( AF_GlyphHints  hints,
                      FT_Outline*    outline,
                      AF_CJKMetrics  metrics )
  {
    FT_Error  error;
    int       dim;

    FT_UNUSED( metrics );


    error = af_glyph_hints_reload( hints, outline );
    if ( error )
      goto Exit;

    /* analyze glyph outline */
    if ( AF_HINTS_DO_HORIZONTAL( hints ) )
    {
      error = af_cjk_hints_detect_features( hints, AF_DIMENSION_HORZ );
      if ( error )
        goto Exit;

      af_cjk_hints_compute_blue_edges( hints, metrics, AF_DIMENSION_HORZ );
    }

    if ( AF_HINTS_DO_VERTICAL( hints ) )
    {
      error = af_cjk_hints_detect_features( hints, AF_DIMENSION_VERT );
      if ( error )
        goto Exit;

      af_cjk_hints_compute_blue_edges( hints, metrics, AF_DIMENSION_VERT );
    }

    /* grid-fit the outline */
    for ( dim = 0; dim < AF_DIMENSION_MAX; dim++ )
    {
      if ( ( dim == AF_DIMENSION_HORZ && AF_HINTS_DO_HORIZONTAL( hints ) ) ||
           ( dim == AF_DIMENSION_VERT && AF_HINTS_DO_VERTICAL( hints ) )   )
      {

#ifdef AF_CONFIG_OPTION_USE_WARPER
        if ( dim == AF_DIMENSION_HORZ                                  &&
             metrics->root.scaler.render_mode == FT_RENDER_MODE_NORMAL )
        {
          AF_WarperRec  warper;
          FT_Fixed      scale;
          FT_Pos        delta;


          af_warper_compute( &warper, hints, (AF_Dimension)dim,
                             &scale, &delta );
          af_glyph_hints_scale_dim( hints, (AF_Dimension)dim,
                                    scale, delta );
          continue;
        }
#endif /* AF_CONFIG_OPTION_USE_WARPER */

        af_cjk_hint_edges( hints, (AF_Dimension)dim );
        af_cjk_align_edge_points( hints, (AF_Dimension)dim );
        af_glyph_hints_align_strong_points( hints, (AF_Dimension)dim );
        af_glyph_hints_align_weak_points( hints, (AF_Dimension)dim );
      }
    }

#if 0
    af_glyph_hints_dump_points( hints );
    af_glyph_hints_dump_segments( hints );
    af_glyph_hints_dump_edges( hints );
#endif

    af_glyph_hints_save( hints, outline );

  Exit:
    return error;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                C J K   S C R I P T   C L A S S                *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_cjk_writing_system_class,

    AF_WRITING_SYSTEM_CJK,

    sizeof ( AF_CJKMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) af_cjk_metrics_init,
    (AF_WritingSystem_ScaleMetricsFunc)af_cjk_metrics_scale,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   af_cjk_hints_init,
    (AF_WritingSystem_ApplyHintsFunc)  af_cjk_hints_apply
  )


#else /* !AF_CONFIG_OPTION_CJK */


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_cjk_writing_system_class,

    AF_WRITING_SYSTEM_CJK,

    sizeof ( AF_CJKMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) NULL,
    (AF_WritingSystem_ScaleMetricsFunc)NULL,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   NULL,
    (AF_WritingSystem_ApplyHintsFunc)  NULL
  )


#endif /* !AF_CONFIG_OPTION_CJK */


/* END */
/***************************************************************************/
/*                                                                         */
/*  afindic.c                                                              */
/*                                                                         */
/*    Auto-fitter hinting routines for Indic writing system (body).        */
/*                                                                         */
/*  Copyright 2007, 2011-2013 by                                           */
/*  Rahul Bhalerao <rahul.bhalerao@redhat.com>, <b.rahul.pm@gmail.com>.    */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "aftypes.h"
#include "aflatin.h"


#ifdef AF_CONFIG_OPTION_INDIC

#include "afindic.h"
#include "aferrors.h"
#include "afcjk.h"


#ifdef AF_CONFIG_OPTION_USE_WARPER
#include "afwarp.h"
#endif


  static FT_Error
  af_indic_metrics_init( AF_CJKMetrics  metrics,
                         FT_Face        face )
  {
    /* skip blue zone init in CJK routines */
    FT_CharMap  oldmap = face->charmap;


    metrics->units_per_em = face->units_per_EM;

    if ( FT_Select_Charmap( face, FT_ENCODING_UNICODE ) )
      face->charmap = NULL;
    else
    {
      af_cjk_metrics_init_widths( metrics, face );
#if 0
      /* either need indic specific blue_chars[] or just skip blue zones */
      af_cjk_metrics_init_blues( metrics, face, af_cjk_blue_chars );
#endif
      af_cjk_metrics_check_digits( metrics, face );
    }

    FT_Set_Charmap( face, oldmap );

    return FT_Err_Ok;
  }


  static void
  af_indic_metrics_scale( AF_CJKMetrics  metrics,
                          AF_Scaler      scaler )
  {
    /* use CJK routines */
    af_cjk_metrics_scale( metrics, scaler );
  }


  static FT_Error
  af_indic_hints_init( AF_GlyphHints  hints,
                       AF_CJKMetrics  metrics )
  {
    /* use CJK routines */
    return af_cjk_hints_init( hints, metrics );
  }


  static FT_Error
  af_indic_hints_apply( AF_GlyphHints  hints,
                        FT_Outline*    outline,
                        AF_CJKMetrics  metrics )
  {
    /* use CJK routines */
    return af_cjk_hints_apply( hints, outline, metrics );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                I N D I C   S C R I P T   C L A S S            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_indic_writing_system_class,

    AF_WRITING_SYSTEM_INDIC,

    sizeof ( AF_CJKMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) af_indic_metrics_init,
    (AF_WritingSystem_ScaleMetricsFunc)af_indic_metrics_scale,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   af_indic_hints_init,
    (AF_WritingSystem_ApplyHintsFunc)  af_indic_hints_apply
  )


#else /* !AF_CONFIG_OPTION_INDIC */


  AF_DEFINE_WRITING_SYSTEM_CLASS(
    af_indic_writing_system_class,

    AF_WRITING_SYSTEM_INDIC,

    sizeof ( AF_CJKMetricsRec ),

    (AF_WritingSystem_InitMetricsFunc) NULL,
    (AF_WritingSystem_ScaleMetricsFunc)NULL,
    (AF_WritingSystem_DoneMetricsFunc) NULL,

    (AF_WritingSystem_InitHintsFunc)   NULL,
    (AF_WritingSystem_ApplyHintsFunc)  NULL
  )


#endif /* !AF_CONFIG_OPTION_INDIC */


/* END */

/***************************************************************************/
/*                                                                         */
/*  hbshim.c                                                               */
/*                                                                         */
/*    HarfBuzz interface for accessing OpenType features (body).           */
/*                                                                         */
/*  Copyright 2013, 2014 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "ft2build.h"
#include FT_FREETYPE_H
#include "afglobal.h"
#include "aftypes.h"
#include "hbshim.h"

#ifdef FT_CONFIG_OPTION_USE_HARFBUZZ


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_afharfbuzz


  /*
   * We use `sets' (in the HarfBuzz sense, which comes quite near to the
   * usual mathematical meaning) to manage both lookups and glyph indices.
   *
   * 1. For each coverage, collect lookup IDs in a set.  Note that an
   *    auto-hinter `coverage' is represented by one `feature', and a
   *    feature consists of an arbitrary number of (font specific) `lookup's
   *    that actually do the mapping job.  Please check the OpenType
   *    specification for more details on features and lookups.
   *
   * 2. Create glyph ID sets from the corresponding lookup sets.
   *
   * 3. The glyph set corresponding to AF_COVERAGE_DEFAULT is computed
   *    with all lookups specific to the OpenType script activated.  It
   *    relies on the order of AF_DEFINE_STYLE_CLASS entries so that
   *    special coverages (like `oldstyle figures') don't get overwritten.
   *
   */


  /* load coverage tags */
#undef  COVERAGE
#define COVERAGE( name, NAME, description,             \
                  tag1, tag2, tag3, tag4 )             \
          static const hb_tag_t  name ## _coverage[] = \
          {                                            \
            HB_TAG( tag1, tag2, tag3, tag4 ),          \
            HB_TAG_NONE                                \
          };


#include "afcover.h"


  /* define mapping between coverage tags and AF_Coverage */
#undef  COVERAGE
#define COVERAGE( name, NAME, description, \
                  tag1, tag2, tag3, tag4 ) \
          name ## _coverage,


  static const hb_tag_t*  coverages[] =
  {
#include "afcover.h"

    NULL /* AF_COVERAGE_DEFAULT */
  };


  /* load HarfBuzz script tags */
#undef  SCRIPT
#define SCRIPT( s, S, d, h, sc1, sc2, sc3 )  h,


  static const hb_script_t  scripts[] =
  {
#include "afscript.h"
  };


  FT_Error
  af_get_coverage( AF_FaceGlobals  globals,
                   AF_StyleClass   style_class,
                   FT_Byte*        gstyles )
  {
    hb_face_t*  face;

    hb_set_t*  gsub_lookups;  /* GSUB lookups for a given script */
    hb_set_t*  gsub_glyphs;   /* glyphs covered by GSUB lookups  */
    hb_set_t*  gpos_lookups;  /* GPOS lookups for a given script */
    hb_set_t*  gpos_glyphs;   /* glyphs covered by GPOS lookups  */

    hb_script_t      script;
    const hb_tag_t*  coverage_tags;
    hb_tag_t         script_tags[] = { HB_TAG_NONE,
                                       HB_TAG_NONE,
                                       HB_TAG_NONE,
                                       HB_TAG_NONE };

    hb_codepoint_t  idx;
#ifdef FT_DEBUG_LEVEL_TRACE
    int             count;
#endif


    if ( !globals || !style_class || !gstyles )
      return FT_THROW( Invalid_Argument );

    face = hb_font_get_face( globals->hb_font );

    gsub_lookups = hb_set_create();
    gsub_glyphs  = hb_set_create();
    gpos_lookups = hb_set_create();
    gpos_glyphs  = hb_set_create();

    coverage_tags = coverages[style_class->coverage];
    script        = scripts[style_class->script];

    /* Convert a HarfBuzz script tag into the corresponding OpenType */
    /* tag or tags -- some Indic scripts like Devanagari have an old */
    /* and a new set of features.                                    */
    hb_ot_tags_from_script( script,
                            &script_tags[0],
                            &script_tags[1] );

    /* `hb_ot_tags_from_script' usually returns HB_OT_TAG_DEFAULT_SCRIPT */
    /* as the second tag.  We change that to HB_TAG_NONE except for the  */
    /* default script.                                                   */
    if ( style_class->script == globals->module->default_script &&
         style_class->coverage == AF_COVERAGE_DEFAULT           )
    {
      if ( script_tags[0] == HB_TAG_NONE )
        script_tags[0] = HB_OT_TAG_DEFAULT_SCRIPT;
      else
      {
        if ( script_tags[1] == HB_TAG_NONE )
          script_tags[1] = HB_OT_TAG_DEFAULT_SCRIPT;
        else if ( script_tags[1] != HB_OT_TAG_DEFAULT_SCRIPT )
          script_tags[2] = HB_OT_TAG_DEFAULT_SCRIPT;
      }
    }
    else
    {
      if ( script_tags[1] == HB_OT_TAG_DEFAULT_SCRIPT )
        script_tags[1] = HB_TAG_NONE;
    }

    hb_ot_layout_collect_lookups( face,
                                  HB_OT_TAG_GSUB,
                                  script_tags,
                                  NULL,
                                  coverage_tags,
                                  gsub_lookups );

    if ( hb_set_is_empty( gsub_lookups ) )
      goto Exit; /* nothing to do */

    hb_ot_layout_collect_lookups( face,
                                  HB_OT_TAG_GPOS,
                                  script_tags,
                                  NULL,
                                  coverage_tags,
                                  gpos_lookups );

    FT_TRACE4(( "GSUB lookups (style `%s'):\n"
                " ",
                af_style_names[style_class->style] ));

#ifdef FT_DEBUG_LEVEL_TRACE
    count = 0;
#endif

    for ( idx = -1; hb_set_next( gsub_lookups, &idx ); )
    {
#ifdef FT_DEBUG_LEVEL_TRACE
      FT_TRACE4(( " %d", idx ));
      count++;
#endif

      /* get output coverage of GSUB feature */
      hb_ot_layout_lookup_collect_glyphs( face,
                                          HB_OT_TAG_GSUB,
                                          idx,
                                          NULL,
                                          NULL,
                                          NULL,
                                          gsub_glyphs );
    }

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( !count )
      FT_TRACE4(( " (none)" ));
    FT_TRACE4(( "\n\n" ));
#endif

    FT_TRACE4(( "GPOS lookups (style `%s'):\n"
                " ",
                af_style_names[style_class->style] ));

#ifdef FT_DEBUG_LEVEL_TRACE
    count = 0;
#endif

    for ( idx = -1; hb_set_next( gpos_lookups, &idx ); )
    {
#ifdef FT_DEBUG_LEVEL_TRACE
      FT_TRACE4(( " %d", idx ));
      count++;
#endif

      /* get input coverage of GPOS feature */
      hb_ot_layout_lookup_collect_glyphs( face,
                                          HB_OT_TAG_GPOS,
                                          idx,
                                          NULL,
                                          gpos_glyphs,
                                          NULL,
                                          NULL );
    }

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( !count )
      FT_TRACE4(( " (none)" ));
    FT_TRACE4(( "\n\n" ));
#endif

    /*
     * We now check whether we can construct blue zones, using glyphs
     * covered by the feature only.  In case there is not a single zone
     * (this is, not a single character is covered), we skip this coverage.
     *
     */
    {
      AF_Blue_Stringset         bss = style_class->blue_stringset;
      const AF_Blue_StringRec*  bs  = &af_blue_stringsets[bss];

      FT_Bool  found = 0;


      for ( ; bs->string != AF_BLUE_STRING_MAX; bs++ )
      {
        const char*  p = &af_blue_strings[bs->string];


        while ( *p )
        {
          hb_codepoint_t  ch;


          GET_UTF8_CHAR( ch, p );

          for ( idx = -1; hb_set_next( gsub_lookups, &idx ); )
          {
            hb_codepoint_t  gidx = FT_Get_Char_Index( globals->face, ch );


            if ( hb_ot_layout_lookup_would_substitute( face, idx,
                                                       &gidx, 1, 1 ) )
            {
              found = 1;
              break;
            }
          }
        }
      }

      if ( !found )
      {
        FT_TRACE4(( "  no blue characters found; style skipped\n" ));
        goto Exit;
      }
    }

    /*
     * Various OpenType features might use the same glyphs at different
     * vertical positions; for example, superscript and subscript glyphs
     * could be the same.  However, the auto-hinter is completely
     * agnostic of OpenType features after the feature analysis has been
     * completed: The engine then simply receives a glyph index and returns a
     * hinted and usually rendered glyph.
     *
     * Consider the superscript feature of font `pala.ttf': Some of the
     * glyphs are `real', this is, they have a zero vertical offset, but
     * most of them are small caps glyphs shifted up to the superscript
     * position (this is, the `sups' feature is present in both the GSUB and
     * GPOS tables).  The code for blue zones computation actually uses a
     * feature's y offset so that the `real' glyphs get correct hints.  But
     * later on it is impossible to decide whether a glyph index belongs to,
     * say, the small caps or superscript feature.
     *
     * For this reason, we don't assign a style to a glyph if the current
     * feature covers the glyph in both the GSUB and the GPOS tables.  This
     * is quite a broad condition, assuming that
     *
     *   (a) glyphs that get used in multiple features are present in a
     *       feature without vertical shift,
     *
     * and
     *
     *   (b) a feature's GPOS data really moves the glyph vertically.
     *
     * Not fulfilling condition (a) makes a font larger; it would also
     * reduce the number of glyphs that could be addressed directly without
     * using OpenType features, so this assumption is rather strong.
     *
     * Condition (b) is much weaker, and there might be glyphs which get
     * missed.  However, the OpenType features we are going to handle are
     * primarily located in GSUB, and HarfBuzz doesn't provide an API to
     * directly get the necessary information from the GPOS table.  A
     * possible solution might be to directly parse the GPOS table to find
     * out whether a glyph gets shifted vertically, but this is something I
     * would like to avoid if not really necessary.
     *
     */
    hb_set_subtract( gsub_glyphs, gpos_glyphs );

#ifdef FT_DEBUG_LEVEL_TRACE
    FT_TRACE4(( "  glyphs without GPOS data (`*' means already assigned)" ));
    count = 0;
#endif

    for ( idx = -1; hb_set_next( gsub_glyphs, &idx ); )
    {
#ifdef FT_DEBUG_LEVEL_TRACE
      if ( !( count % 10 ) )
        FT_TRACE4(( "\n"
                    "   " ));

      FT_TRACE4(( " %d", idx ));
      count++;
#endif

      if ( gstyles[idx] == AF_STYLE_UNASSIGNED )
        gstyles[idx] = (FT_Byte)style_class->style;
#ifdef FT_DEBUG_LEVEL_TRACE
      else
        FT_TRACE4(( "*" ));
#endif
    }

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( !count )
      FT_TRACE4(( "\n"
                  "    (none)" ));
    FT_TRACE4(( "\n\n" ));
#endif

  Exit:
    hb_set_destroy( gsub_lookups );
    hb_set_destroy( gsub_glyphs  );
    hb_set_destroy( gpos_lookups );
    hb_set_destroy( gpos_glyphs  );

    return FT_Err_Ok;
  }


  /* construct HarfBuzz features */
#undef  COVERAGE
#define COVERAGE( name, NAME, description,                \
                  tag1, tag2, tag3, tag4 )                \
          static const hb_feature_t  name ## _feature[] = \
          {                                               \
            {                                             \
              HB_TAG( tag1, tag2, tag3, tag4 ),           \
              1, 0, (unsigned int)-1                      \
            }                                             \
          };


#include "afcover.h"


  /* define mapping between HarfBuzz features and AF_Coverage */
#undef  COVERAGE
#define COVERAGE( name, NAME, description, \
                  tag1, tag2, tag3, tag4 ) \
          name ## _feature,


  static const hb_feature_t*  features[] =
  {
#include "afcover.h"

    NULL /* AF_COVERAGE_DEFAULT */
  };


  FT_Error
  af_get_char_index( AF_StyleMetrics  metrics,
                     FT_ULong         charcode,
                     FT_ULong        *codepoint,
                     FT_Long         *y_offset )
  {
    AF_StyleClass  style_class;

    const hb_feature_t*  feature;

    FT_ULong  in_idx, out_idx;


    if ( !metrics )
      return FT_THROW( Invalid_Argument );

    in_idx = FT_Get_Char_Index( metrics->globals->face, charcode );

    style_class = metrics->style_class;

    feature = features[style_class->coverage];

    if ( feature )
    {
      FT_UInt  upem = metrics->globals->face->units_per_EM;

      hb_font_t*    font = metrics->globals->hb_font;
      hb_buffer_t*  buf  = hb_buffer_create();

      uint32_t  c = (uint32_t)charcode;

      hb_glyph_info_t*      ginfo;
      hb_glyph_position_t*  gpos;
      unsigned int          gcount;


      /* we shape at a size of units per EM; this means font units */
      hb_font_set_scale( font, upem, upem );

      /* XXX: is this sufficient for a single character of any script? */
      hb_buffer_set_direction( buf, HB_DIRECTION_LTR );
      hb_buffer_set_script( buf, scripts[style_class->script] );

      /* we add one character to `buf' ... */
      hb_buffer_add_utf32( buf, &c, 1, 0, 1 );

      /* ... and apply one feature */
      hb_shape( font, buf, feature, 1 );

      ginfo = hb_buffer_get_glyph_infos( buf, &gcount );
      gpos  = hb_buffer_get_glyph_positions( buf, &gcount );

      out_idx = ginfo[0].codepoint;

      /* getting the same index indicates no substitution,         */
      /* which means that the glyph isn't available in the feature */
      if ( in_idx == out_idx )
      {
        *codepoint = 0;
        *y_offset  = 0;
      }
      else
      {
        *codepoint = out_idx;
        *y_offset  = gpos[0].y_offset;
      }

      hb_buffer_destroy( buf );

#ifdef FT_DEBUG_LEVEL_TRACE
      if ( gcount > 1 )
        FT_TRACE1(( "af_get_char_index:"
                    " input character mapped to multiple glyphs\n" ));
#endif
    }
    else
    {
      *codepoint = in_idx;
      *y_offset  = 0;
    }

    return FT_Err_Ok;
  }


#else /* !FT_CONFIG_OPTION_USE_HARFBUZZ */


  FT_Error
  af_get_coverage( AF_FaceGlobals  globals,
                   AF_StyleClass   style_class,
                   FT_Byte*        gstyles )
  {
    FT_UNUSED( globals );
    FT_UNUSED( style_class );
    FT_UNUSED( gstyles );

    return FT_Err_Ok;
  }


  FT_Error
  af_get_char_index( AF_StyleMetrics  metrics,
                     FT_ULong         charcode,
                     FT_ULong        *codepoint,
                     FT_Long         *y_offset )
  {
    FT_Face  face;


    if ( !metrics )
      return FT_THROW( Invalid_Argument );

    face = metrics->globals->face;

    *codepoint = FT_Get_Char_Index( face, charcode );
    *y_offset  = 0;

    return FT_Err_Ok;
  }


#endif /* !FT_CONFIG_OPTION_USE_HARFBUZZ */


/* END */

/***************************************************************************/
/*                                                                         */
/*  afloader.c                                                             */
/*                                                                         */
/*    Auto-fitter glyph loading routines (body).                           */
/*                                                                         */
/*  Copyright 2003-2009, 2011-2014 by                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "afglobal.h"
#include "afloader.h"
#include "afhints.h"
#include "aferrors.h"
#include "afmodule.h"
#include "afpic.h"


  /* Initialize glyph loader. */

  FT_LOCAL_DEF( FT_Error )
  af_loader_init( AF_Module  module )
  {
    AF_Loader  loader = module->loader;
    FT_Memory  memory = module->root.library->memory;


    FT_ZERO( loader );

    af_glyph_hints_init( &loader->hints, memory );
#ifdef FT_DEBUG_AUTOFIT
    _af_debug_hints = &loader->hints;
#endif
    return FT_GlyphLoader_New( memory, &loader->gloader );
  }


  /* Reset glyph loader and compute globals if necessary. */

  FT_LOCAL_DEF( FT_Error )
  af_loader_reset( AF_Module  module,
                   FT_Face    face )
  {
    FT_Error   error  = FT_Err_Ok;
    AF_Loader  loader = module->loader;


    loader->face    = face;
    loader->globals = (AF_FaceGlobals)face->autohint.data;

    FT_GlyphLoader_Rewind( loader->gloader );

    if ( loader->globals == NULL )
    {
      error = af_face_globals_new( face, &loader->globals, module );
      if ( !error )
      {
        face->autohint.data =
          (FT_Pointer)loader->globals;
        face->autohint.finalizer =
          (FT_Generic_Finalizer)af_face_globals_free;
      }
    }

    return error;
  }


  /* Finalize glyph loader. */

  FT_LOCAL_DEF( void )
  af_loader_done( AF_Module  module )
  {
    AF_Loader  loader = module->loader;


    af_glyph_hints_done( &loader->hints );

    loader->face    = NULL;
    loader->globals = NULL;

#ifdef FT_DEBUG_AUTOFIT
    _af_debug_hints = NULL;
#endif
    FT_GlyphLoader_Done( loader->gloader );
    loader->gloader = NULL;
  }


  /* Load a single glyph component.  This routine calls itself */
  /* recursively, if necessary, and does the main work of      */
  /* `af_loader_load_glyph.'                                   */

  static FT_Error
  af_loader_load_g( AF_Loader  loader,
                    AF_Scaler  scaler,
                    FT_UInt    glyph_index,
                    FT_Int32   load_flags,
                    FT_UInt    depth )
  {
    FT_Error          error;
    FT_Face           face     = loader->face;
    FT_GlyphLoader    gloader  = loader->gloader;
    AF_StyleMetrics   metrics  = loader->metrics;
    AF_GlyphHints     hints    = &loader->hints;
    FT_GlyphSlot      slot     = face->glyph;
    FT_Slot_Internal  internal = slot->internal;
    FT_Int32          flags;


    flags = load_flags | FT_LOAD_LINEAR_DESIGN;
    error = FT_Load_Glyph( face, glyph_index, flags );
    if ( error )
      goto Exit;

    loader->transformed = internal->glyph_transformed;
    if ( loader->transformed )
    {
      FT_Matrix  inverse;


      loader->trans_matrix = internal->glyph_matrix;
      loader->trans_delta  = internal->glyph_delta;

      inverse = loader->trans_matrix;
      FT_Matrix_Invert( &inverse );
      FT_Vector_Transform( &loader->trans_delta, &inverse );
    }

    switch ( slot->format )
    {
    case FT_GLYPH_FORMAT_OUTLINE:
      /* translate the loaded glyph when an internal transform is needed */
      if ( loader->transformed )
        FT_Outline_Translate( &slot->outline,
                              loader->trans_delta.x,
                              loader->trans_delta.y );

      /* copy the outline points in the loader's current                */
      /* extra points which are used to keep original glyph coordinates */
      error = FT_GLYPHLOADER_CHECK_POINTS( gloader,
                                           slot->outline.n_points + 4,
                                           slot->outline.n_contours );
      if ( error )
        goto Exit;

      FT_ARRAY_COPY( gloader->current.outline.points,
                     slot->outline.points,
                     slot->outline.n_points );

      FT_ARRAY_COPY( gloader->current.outline.contours,
                     slot->outline.contours,
                     slot->outline.n_contours );

      FT_ARRAY_COPY( gloader->current.outline.tags,
                     slot->outline.tags,
                     slot->outline.n_points );

      gloader->current.outline.n_points   = slot->outline.n_points;
      gloader->current.outline.n_contours = slot->outline.n_contours;

      /* compute original horizontal phantom points (and ignore */
      /* vertical ones)                                         */
      loader->pp1.x = hints->x_delta;
      loader->pp1.y = hints->y_delta;
      loader->pp2.x = FT_MulFix( slot->metrics.horiAdvance,
                                 hints->x_scale ) + hints->x_delta;
      loader->pp2.y = hints->y_delta;

      /* be sure to check for spacing glyphs */
      if ( slot->outline.n_points == 0 )
        goto Hint_Metrics;

      /* now load the slot image into the auto-outline and run the */
      /* automatic hinting process                                 */
      {
#ifdef FT_CONFIG_OPTION_PIC
        AF_FaceGlobals         globals = loader->globals;
#endif
        AF_StyleClass          style_class = metrics->style_class;
        AF_WritingSystemClass  writing_system_class =
          AF_WRITING_SYSTEM_CLASSES_GET[style_class->writing_system];


        if ( writing_system_class->style_hints_apply )
          writing_system_class->style_hints_apply( hints,
                                                   &gloader->current.outline,
                                                   metrics );
      }

      /* we now need to adjust the metrics according to the change in */
      /* width/positioning that occurred during the hinting process   */
      if ( scaler->render_mode != FT_RENDER_MODE_LIGHT )
      {
        FT_Pos        old_rsb, old_lsb, new_lsb;
        FT_Pos        pp1x_uh, pp2x_uh;
        AF_AxisHints  axis  = &hints->axis[AF_DIMENSION_HORZ];
        AF_Edge       edge1 = axis->edges;         /* leftmost edge  */
        AF_Edge       edge2 = edge1 +
                              axis->num_edges - 1; /* rightmost edge */


        if ( axis->num_edges > 1 && AF_HINTS_DO_ADVANCE( hints ) )
        {
          old_rsb = loader->pp2.x - edge2->opos;
          old_lsb = edge1->opos;
          new_lsb = edge1->pos;

          /* remember unhinted values to later account */
          /* for rounding errors                       */

          pp1x_uh = new_lsb    - old_lsb;
          pp2x_uh = edge2->pos + old_rsb;

          /* prefer too much space over too little space */
          /* for very small sizes                        */

          if ( old_lsb < 24 )
            pp1x_uh -= 8;

          if ( old_rsb < 24 )
            pp2x_uh += 8;

          loader->pp1.x = FT_PIX_ROUND( pp1x_uh );
          loader->pp2.x = FT_PIX_ROUND( pp2x_uh );

          if ( loader->pp1.x >= new_lsb && old_lsb > 0 )
            loader->pp1.x -= 64;

          if ( loader->pp2.x <= edge2->pos && old_rsb > 0 )
            loader->pp2.x += 64;

          slot->lsb_delta = loader->pp1.x - pp1x_uh;
          slot->rsb_delta = loader->pp2.x - pp2x_uh;
        }
        else
        {
          FT_Pos  pp1x = loader->pp1.x;
          FT_Pos  pp2x = loader->pp2.x;


          loader->pp1.x = FT_PIX_ROUND( pp1x );
          loader->pp2.x = FT_PIX_ROUND( pp2x );

          slot->lsb_delta = loader->pp1.x - pp1x;
          slot->rsb_delta = loader->pp2.x - pp2x;
        }
      }
      else
      {
        FT_Pos  pp1x = loader->pp1.x;
        FT_Pos  pp2x = loader->pp2.x;


        loader->pp1.x = FT_PIX_ROUND( pp1x + hints->xmin_delta );
        loader->pp2.x = FT_PIX_ROUND( pp2x + hints->xmax_delta );

        slot->lsb_delta = loader->pp1.x - pp1x;
        slot->rsb_delta = loader->pp2.x - pp2x;
      }

      /* good, we simply add the glyph to our loader's base */
      FT_GlyphLoader_Add( gloader );
      break;

    case FT_GLYPH_FORMAT_COMPOSITE:
      {
        FT_UInt      nn, num_subglyphs = slot->num_subglyphs;
        FT_UInt      num_base_subgs, start_point;
        FT_SubGlyph  subglyph;


        start_point = gloader->base.outline.n_points;

        /* first of all, copy the subglyph descriptors in the glyph loader */
        error = FT_GlyphLoader_CheckSubGlyphs( gloader, num_subglyphs );
        if ( error )
          goto Exit;

        FT_ARRAY_COPY( gloader->current.subglyphs,
                       slot->subglyphs,
                       num_subglyphs );

        gloader->current.num_subglyphs = num_subglyphs;
        num_base_subgs                 = gloader->base.num_subglyphs;

        /* now read each subglyph independently */
        for ( nn = 0; nn < num_subglyphs; nn++ )
        {
          FT_Vector  pp1, pp2;
          FT_Pos     x, y;
          FT_UInt    num_points, num_new_points, num_base_points;


          /* gloader.current.subglyphs can change during glyph loading due */
          /* to re-allocation -- we must recompute the current subglyph on */
          /* each iteration                                                */
          subglyph = gloader->base.subglyphs + num_base_subgs + nn;

          pp1 = loader->pp1;
          pp2 = loader->pp2;

          num_base_points = gloader->base.outline.n_points;

          error = af_loader_load_g( loader, scaler, subglyph->index,
                                    load_flags, depth + 1 );
          if ( error )
            goto Exit;

          /* recompute subglyph pointer */
          subglyph = gloader->base.subglyphs + num_base_subgs + nn;

          if ( !( subglyph->flags & FT_SUBGLYPH_FLAG_USE_MY_METRICS ) )
          {
            loader->pp1 = pp1;
            loader->pp2 = pp2;
          }

          num_points     = gloader->base.outline.n_points;
          num_new_points = num_points - num_base_points;

          /* now perform the transformation required for this subglyph */

          if ( subglyph->flags & ( FT_SUBGLYPH_FLAG_SCALE    |
                                   FT_SUBGLYPH_FLAG_XY_SCALE |
                                   FT_SUBGLYPH_FLAG_2X2      ) )
          {
            FT_Vector*  cur   = gloader->base.outline.points +
                                num_base_points;
            FT_Vector*  limit = cur + num_new_points;


            for ( ; cur < limit; cur++ )
              FT_Vector_Transform( cur, &subglyph->transform );
          }

          /* apply offset */

          if ( !( subglyph->flags & FT_SUBGLYPH_FLAG_ARGS_ARE_XY_VALUES ) )
          {
            FT_Int      k = subglyph->arg1;
            FT_UInt     l = subglyph->arg2;
            FT_Vector*  p1;
            FT_Vector*  p2;


            if ( start_point + k >= num_base_points         ||
                               l >= (FT_UInt)num_new_points )
            {
              error = FT_THROW( Invalid_Composite );
              goto Exit;
            }

            l += num_base_points;

            /* for now, only use the current point coordinates; */
            /* we eventually may consider another approach      */
            p1 = gloader->base.outline.points + start_point + k;
            p2 = gloader->base.outline.points + start_point + l;

            x = p1->x - p2->x;
            y = p1->y - p2->y;
          }
          else
          {
            x = FT_MulFix( subglyph->arg1, hints->x_scale ) + hints->x_delta;
            y = FT_MulFix( subglyph->arg2, hints->y_scale ) + hints->y_delta;

            x = FT_PIX_ROUND( x );
            y = FT_PIX_ROUND( y );
          }

          {
            FT_Outline  dummy = gloader->base.outline;


            dummy.points  += num_base_points;
            dummy.n_points = (short)num_new_points;

            FT_Outline_Translate( &dummy, x, y );
          }
        }
      }
      break;

    default:
      /* we don't support other formats (yet?) */
      error = FT_THROW( Unimplemented_Feature );
    }

  Hint_Metrics:
    if ( depth == 0 )
    {
      FT_BBox    bbox;
      FT_Vector  vvector;


      vvector.x = slot->metrics.vertBearingX - slot->metrics.horiBearingX;
      vvector.y = slot->metrics.vertBearingY - slot->metrics.horiBearingY;
      vvector.x = FT_MulFix( vvector.x, metrics->scaler.x_scale );
      vvector.y = FT_MulFix( vvector.y, metrics->scaler.y_scale );

      /* transform the hinted outline if needed */
      if ( loader->transformed )
      {
        FT_Outline_Transform( &gloader->base.outline, &loader->trans_matrix );
        FT_Vector_Transform( &vvector, &loader->trans_matrix );
      }
#if 1
      /* we must translate our final outline by -pp1.x and compute */
      /* the new metrics                                           */
      if ( loader->pp1.x )
        FT_Outline_Translate( &gloader->base.outline, -loader->pp1.x, 0 );
#endif
      FT_Outline_Get_CBox( &gloader->base.outline, &bbox );

      bbox.xMin = FT_PIX_FLOOR( bbox.xMin );
      bbox.yMin = FT_PIX_FLOOR( bbox.yMin );
      bbox.xMax = FT_PIX_CEIL(  bbox.xMax );
      bbox.yMax = FT_PIX_CEIL(  bbox.yMax );

      slot->metrics.width        = bbox.xMax - bbox.xMin;
      slot->metrics.height       = bbox.yMax - bbox.yMin;
      slot->metrics.horiBearingX = bbox.xMin;
      slot->metrics.horiBearingY = bbox.yMax;

      slot->metrics.vertBearingX = FT_PIX_FLOOR( bbox.xMin + vvector.x );
      slot->metrics.vertBearingY = FT_PIX_FLOOR( bbox.yMax + vvector.y );

      /* for mono-width fonts (like Andale, Courier, etc.) we need */
      /* to keep the original rounded advance width; ditto for     */
      /* digits if all have the same advance width                 */
#if 0
      if ( !FT_IS_FIXED_WIDTH( slot->face ) )
        slot->metrics.horiAdvance = loader->pp2.x - loader->pp1.x;
      else
        slot->metrics.horiAdvance = FT_MulFix( slot->metrics.horiAdvance,
                                               x_scale );
#else
      if ( scaler->render_mode != FT_RENDER_MODE_LIGHT                      &&
           ( FT_IS_FIXED_WIDTH( slot->face )                              ||
             ( af_face_globals_is_digit( loader->globals, glyph_index ) &&
               metrics->digits_have_same_width                          ) ) )
      {
        slot->metrics.horiAdvance = FT_MulFix( slot->metrics.horiAdvance,
                                               metrics->scaler.x_scale );

        /* Set delta values to 0.  Otherwise code that uses them is */
        /* going to ruin the fixed advance width.                   */
        slot->lsb_delta = 0;
        slot->rsb_delta = 0;
      }
      else
      {
        /* non-spacing glyphs must stay as-is */
        if ( slot->metrics.horiAdvance )
          slot->metrics.horiAdvance = loader->pp2.x - loader->pp1.x;
      }
#endif

      slot->metrics.vertAdvance = FT_MulFix( slot->metrics.vertAdvance,
                                             metrics->scaler.y_scale );

      slot->metrics.horiAdvance = FT_PIX_ROUND( slot->metrics.horiAdvance );
      slot->metrics.vertAdvance = FT_PIX_ROUND( slot->metrics.vertAdvance );

      /* now copy outline into glyph slot */
      FT_GlyphLoader_Rewind( internal->loader );
      error = FT_GlyphLoader_CopyPoints( internal->loader, gloader );
      if ( error )
        goto Exit;

      /* reassign all outline fields except flags to protect them */
      slot->outline.n_contours = internal->loader->base.outline.n_contours;
      slot->outline.n_points   = internal->loader->base.outline.n_points;
      slot->outline.points     = internal->loader->base.outline.points;
      slot->outline.tags       = internal->loader->base.outline.tags;
      slot->outline.contours   = internal->loader->base.outline.contours;

      slot->format  = FT_GLYPH_FORMAT_OUTLINE;
    }

  Exit:
    return error;
  }


  /* Load a glyph. */

  FT_LOCAL_DEF( FT_Error )
  af_loader_load_glyph( AF_Module  module,
                        FT_Face    face,
                        FT_UInt    gindex,
                        FT_Int32   load_flags )
  {
    FT_Error      error;
    FT_Size       size   = face->size;
    AF_Loader     loader = module->loader;
    AF_ScalerRec  scaler;


    if ( !size )
      return FT_THROW( Invalid_Argument );

    FT_ZERO( &scaler );

    scaler.face    = face;
    scaler.x_scale = size->metrics.x_scale;
    scaler.x_delta = 0;  /* XXX: TODO: add support for sub-pixel hinting */
    scaler.y_scale = size->metrics.y_scale;
    scaler.y_delta = 0;  /* XXX: TODO: add support for sub-pixel hinting */

    scaler.render_mode = FT_LOAD_TARGET_MODE( load_flags );
    scaler.flags       = 0;  /* XXX: fix this */

    error = af_loader_reset( module, face );
    if ( !error )
    {
      AF_StyleMetrics  metrics;
      FT_UInt          options = AF_STYLE_NONE_DFLT;


#ifdef FT_OPTION_AUTOFIT2
      /* XXX: undocumented hook to activate the latin2 writing system */
      if ( load_flags & ( 1UL << 20 ) )
        options = AF_STYLE_LTN2_DFLT;
#endif

      error = af_face_globals_get_metrics( loader->globals, gindex,
                                           options, &metrics );
      if ( !error )
      {
#ifdef FT_CONFIG_OPTION_PIC
        AF_FaceGlobals         globals = loader->globals;
#endif
        AF_StyleClass          style_class = metrics->style_class;
        AF_WritingSystemClass  writing_system_class =
          AF_WRITING_SYSTEM_CLASSES_GET[style_class->writing_system];


        loader->metrics = metrics;

        if ( writing_system_class->style_metrics_scale )
          writing_system_class->style_metrics_scale( metrics, &scaler );
        else
          metrics->scaler = scaler;

        load_flags |=  FT_LOAD_NO_SCALE | FT_LOAD_IGNORE_TRANSFORM;
        load_flags &= ~FT_LOAD_RENDER;

        if ( writing_system_class->style_hints_init )
        {
          error = writing_system_class->style_hints_init( &loader->hints,
                                                          metrics );
          if ( error )
            goto Exit;
        }

        error = af_loader_load_g( loader, &scaler, gindex, load_flags, 0 );
      }
    }
  Exit:
    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  afmodule.c                                                             */
/*                                                                         */
/*    Auto-fitter module implementation (body).                            */
/*                                                                         */
/*  Copyright 2003-2006, 2009, 2011-2014 by                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "afglobal.h"
#include "afmodule.h"
#include "afloader.h"
#include "aferrors.h"
#include "afpic.h"

#ifdef FT_DEBUG_AUTOFIT
  int    _af_debug_disable_horz_hints;
  int    _af_debug_disable_vert_hints;
  int    _af_debug_disable_blue_hints;
  void*  _af_debug_hints;
#endif

#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_DEBUG_H
#include FT_AUTOHINTER_H
#include FT_SERVICE_PROPERTIES_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_afmodule


  static FT_Error
  af_property_get_face_globals( FT_Face          face,
                                AF_FaceGlobals*  aglobals,
                                AF_Module        module )
  {
    FT_Error        error = FT_Err_Ok;
    AF_FaceGlobals  globals;


    if ( !face )
      return FT_THROW( Invalid_Argument );

    globals = (AF_FaceGlobals)face->autohint.data;
    if ( !globals )
    {
      /* trigger computation of the global style data */
      /* in case it hasn't been done yet              */
      error = af_face_globals_new( face, &globals, module );
      if ( !error )
      {
        face->autohint.data =
          (FT_Pointer)globals;
        face->autohint.finalizer =
          (FT_Generic_Finalizer)af_face_globals_free;
      }
    }

    if ( !error )
      *aglobals = globals;

    return error;
  }


  static FT_Error
  af_property_set( FT_Module    ft_module,
                   const char*  property_name,
                   const void*  value )
  {
    FT_Error   error  = FT_Err_Ok;
    AF_Module  module = (AF_Module)ft_module;


    if ( !ft_strcmp( property_name, "fallback-script" ) )
    {
      FT_UInt*  fallback_script = (FT_UInt*)value;

      FT_UInt  ss;


      /* We translate the fallback script to a fallback style that uses */
      /* `fallback-script' as its script and `AF_COVERAGE_NONE' as its  */
      /* coverage value.                                                */
      for ( ss = 0; AF_STYLE_CLASSES_GET[ss]; ss++ )
      {
        AF_StyleClass  style_class = AF_STYLE_CLASSES_GET[ss];


        if ( (int)style_class->script == (int)(*fallback_script)      &&
             style_class->coverage == AF_COVERAGE_DEFAULT )
        {
          module->fallback_style = ss;
          break;
        }
      }

      if ( !AF_STYLE_CLASSES_GET[ss] )
      {
        FT_TRACE0(( "af_property_set: Invalid value %d for property `%s'\n",
                    fallback_script, property_name ));
        return FT_THROW( Invalid_Argument );
      }

      return error;
    }
    else if ( !ft_strcmp( property_name, "default-script" ) )
    {
      FT_UInt*  default_script = (FT_UInt*)value;


      module->default_script = *default_script;

      return error;
    }
    else if ( !ft_strcmp( property_name, "increase-x-height" ) )
    {
      FT_Prop_IncreaseXHeight*  prop = (FT_Prop_IncreaseXHeight*)value;
      AF_FaceGlobals            globals;


      error = af_property_get_face_globals( prop->face, &globals, module );
      if ( !error )
        globals->increase_x_height = prop->limit;

      return error;
    }

    FT_TRACE0(( "af_property_set: missing property `%s'\n",
                property_name ));
    return FT_THROW( Missing_Property );
  }


  static FT_Error
  af_property_get( FT_Module    ft_module,
                   const char*  property_name,
                   void*        value )
  {
    FT_Error   error          = FT_Err_Ok;
    AF_Module  module         = (AF_Module)ft_module;
    FT_UInt    fallback_style = module->fallback_style;
    FT_UInt    default_script = module->default_script;


    if ( !ft_strcmp( property_name, "glyph-to-script-map" ) )
    {
      FT_Prop_GlyphToScriptMap*  prop = (FT_Prop_GlyphToScriptMap*)value;
      AF_FaceGlobals             globals;


      error = af_property_get_face_globals( prop->face, &globals, module );
      if ( !error )
        prop->map = globals->glyph_styles;

      return error;
    }
    else if ( !ft_strcmp( property_name, "fallback-script" ) )
    {
      FT_UInt*  val = (FT_UInt*)value;

      AF_StyleClass  style_class = AF_STYLE_CLASSES_GET[fallback_style];


      *val = style_class->script;

      return error;
    }
    else if ( !ft_strcmp( property_name, "default-script" ) )
    {
      FT_UInt*  val = (FT_UInt*)value;


      *val = default_script;

      return error;
    }
    else if ( !ft_strcmp( property_name, "increase-x-height" ) )
    {
      FT_Prop_IncreaseXHeight*  prop = (FT_Prop_IncreaseXHeight*)value;
      AF_FaceGlobals            globals;


      error = af_property_get_face_globals( prop->face, &globals, module );
      if ( !error )
        prop->limit = globals->increase_x_height;

      return error;
    }


    FT_TRACE0(( "af_property_get: missing property `%s'\n",
                property_name ));
    return FT_THROW( Missing_Property );
  }


  FT_DEFINE_SERVICE_PROPERTIESREC(
    af_service_properties,
    (FT_Properties_SetFunc)af_property_set,
    (FT_Properties_GetFunc)af_property_get )


  FT_DEFINE_SERVICEDESCREC1(
    af_services,
    FT_SERVICE_ID_PROPERTIES, &AF_SERVICE_PROPERTIES_GET )


  FT_CALLBACK_DEF( FT_Module_Interface )
  af_get_interface( FT_Module    module,
                    const char*  module_interface )
  {
    /* AF_SERVICES_GET derefers `library' in PIC mode */
#ifdef FT_CONFIG_OPTION_PIC
    FT_Library  library;


    if ( !module )
      return NULL;
    library = module->library;
    if ( !library )
      return NULL;
#else
    FT_UNUSED( module );
#endif

    return ft_service_list_lookup( AF_SERVICES_GET, module_interface );
  }


  FT_CALLBACK_DEF( FT_Error )
  af_autofitter_init( FT_Module  ft_module )      /* AF_Module */
  {
    AF_Module  module = (AF_Module)ft_module;


    module->fallback_style = AF_STYLE_FALLBACK;
    module->default_script = AF_SCRIPT_DEFAULT;

    return af_loader_init( module );
  }


  FT_CALLBACK_DEF( void )
  af_autofitter_done( FT_Module  ft_module )      /* AF_Module */
  {
    AF_Module  module = (AF_Module)ft_module;


    af_loader_done( module );
  }


  FT_CALLBACK_DEF( FT_Error )
  af_autofitter_load_glyph( AF_Module     module,
                            FT_GlyphSlot  slot,
                            FT_Size       size,
                            FT_UInt       glyph_index,
                            FT_Int32      load_flags )
  {
    FT_UNUSED( size );

    return af_loader_load_glyph( module, slot->face,
                                 glyph_index, load_flags );
  }


  FT_DEFINE_AUTOHINTER_INTERFACE(
    af_autofitter_interface,
    NULL,                                                    /* reset_face */
    NULL,                                              /* get_global_hints */
    NULL,                                             /* done_global_hints */
    (FT_AutoHinter_GlyphLoadFunc)af_autofitter_load_glyph )  /* load_glyph */


  FT_DEFINE_MODULE(
    autofit_module_class,

    FT_MODULE_HINTER,
    sizeof ( AF_ModuleRec ),

    "autofitter",
    0x10000L,   /* version 1.0 of the autofitter  */
    0x20000L,   /* requires FreeType 2.0 or above */

    (const void*)&AF_INTERFACE_GET,

    (FT_Module_Constructor)af_autofitter_init,
    (FT_Module_Destructor) af_autofitter_done,
    (FT_Module_Requester)  af_get_interface )


/* END */

#ifdef AF_CONFIG_OPTION_USE_WARPER
/***************************************************************************/
/*                                                                         */
/*  afwarp.c                                                               */
/*                                                                         */
/*    Auto-fitter warping algorithm (body).                                */
/*                                                                         */
/*  Copyright 2006, 2007, 2011 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*
   *  The idea of the warping code is to slightly scale and shift a glyph
   *  within a single dimension so that as much of its segments are aligned
   *  (more or less) on the grid.  To find out the optimal scaling and
   *  shifting value, various parameter combinations are tried and scored.
   */

#include "afwarp.h"

#ifdef AF_CONFIG_OPTION_USE_WARPER

  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_afwarp


  /* The weights cover the range 0/64 - 63/64 of a pixel.  Obviously, */
  /* values around a half pixel (which means exactly between two grid */
  /* lines) gets the worst weight.                                    */
#if 1
  static const AF_WarpScore
  af_warper_weights[64] =
  {
    35, 32, 30, 25, 20, 15, 12, 10,  5,  1,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0, -1, -2, -5, -8,-10,-10,-20,-20,-30,-30,

   -30,-30,-20,-20,-10,-10, -8, -5, -2, -1,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  1,  5, 10, 12, 15, 20, 25, 30, 32,
  };
#else
  static const AF_WarpScore
  af_warper_weights[64] =
  {
    30, 20, 10,  5,  4,  4,  3,  2,  1,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0, -1, -2, -2, -5, -5,-10,-10,-15,-20,

   -20,-15,-15,-10,-10, -5, -5, -2, -2, -1,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  4,  5, 10, 20,
  };
#endif


  /* Score segments for a given `scale' and `delta' in the range */
  /* `xx1' to `xx2', and store the best result in `warper'.  If  */
  /* the new best score is equal to the old one, prefer the      */
  /* value with a smaller distortion (around `base_distort').    */

  static void
  af_warper_compute_line_best( AF_Warper     warper,
                               FT_Fixed      scale,
                               FT_Pos        delta,
                               FT_Pos        xx1,
                               FT_Pos        xx2,
                               AF_WarpScore  base_distort,
                               AF_Segment    segments,
                               FT_UInt       num_segments )
  {
    FT_Int        idx_min, idx_max, idx0;
    FT_UInt       nn;
    AF_WarpScore  scores[65];


    for ( nn = 0; nn < 65; nn++ )
      scores[nn] = 0;

    idx0 = xx1 - warper->t1;

    /* compute minimum and maximum indices */
    {
      FT_Pos  xx1min = warper->x1min;
      FT_Pos  xx1max = warper->x1max;
      FT_Pos  w      = xx2 - xx1;


      if ( xx1min + w < warper->x2min )
        xx1min = warper->x2min - w;

      xx1max = warper->x1max;
      if ( xx1max + w > warper->x2max )
        xx1max = warper->x2max - w;

      idx_min = xx1min - warper->t1;
      idx_max = xx1max - warper->t1;

      if ( idx_min < 0 || idx_min > idx_max || idx_max > 64 )
      {
        FT_TRACE5(( "invalid indices:\n"
                    "  min=%d max=%d, xx1=%ld xx2=%ld,\n"
                    "  x1min=%ld x1max=%ld, x2min=%ld x2max=%ld\n",
                    idx_min, idx_max, xx1, xx2,
                    warper->x1min, warper->x1max,
                    warper->x2min, warper->x2max ));
        return;
      }
    }

    for ( nn = 0; nn < num_segments; nn++ )
    {
      FT_Pos  len = segments[nn].max_coord - segments[nn].min_coord;
      FT_Pos  y0  = FT_MulFix( segments[nn].pos, scale ) + delta;
      FT_Pos  y   = y0 + ( idx_min - idx0 );
      FT_Int  idx;


      /* score the length of the segments for the given range */
      for ( idx = idx_min; idx <= idx_max; idx++, y++ )
        scores[idx] += af_warper_weights[y & 63] * len;
    }

    /* find best score */
    {
      FT_Int  idx;


      for ( idx = idx_min; idx <= idx_max; idx++ )
      {
        AF_WarpScore  score = scores[idx];
        AF_WarpScore  distort = base_distort + ( idx - idx0 );


        if ( score > warper->best_score         ||
             ( score == warper->best_score    &&
               distort < warper->best_distort ) )
        {
          warper->best_score   = score;
          warper->best_distort = distort;
          warper->best_scale   = scale;
          warper->best_delta   = delta + ( idx - idx0 );
        }
      }
    }
  }


  /* Compute optimal scaling and delta values for a given glyph and */
  /* dimension.                                                     */

  FT_LOCAL_DEF( void )
  af_warper_compute( AF_Warper      warper,
                     AF_GlyphHints  hints,
                     AF_Dimension   dim,
                     FT_Fixed      *a_scale,
                     FT_Pos        *a_delta )
  {
    AF_AxisHints  axis;
    AF_Point      points;

    FT_Fixed      org_scale;
    FT_Pos        org_delta;

    FT_UInt       nn, num_points, num_segments;
    FT_Int        X1, X2;
    FT_Int        w;

    AF_WarpScore  base_distort;
    AF_Segment    segments;


    /* get original scaling transformation */
    if ( dim == AF_DIMENSION_VERT )
    {
      org_scale = hints->y_scale;
      org_delta = hints->y_delta;
    }
    else
    {
      org_scale = hints->x_scale;
      org_delta = hints->x_delta;
    }

    warper->best_scale   = org_scale;
    warper->best_delta   = org_delta;
    warper->best_score   = INT_MIN;
    warper->best_distort = 0;

    axis         = &hints->axis[dim];
    segments     = axis->segments;
    num_segments = axis->num_segments;
    points       = hints->points;
    num_points   = hints->num_points;

    *a_scale = org_scale;
    *a_delta = org_delta;

    /* get X1 and X2, minimum and maximum in original coordinates */
    if ( num_segments < 1 )
      return;

#if 1
    X1 = X2 = points[0].fx;
    for ( nn = 1; nn < num_points; nn++ )
    {
      FT_Int  X = points[nn].fx;


      if ( X < X1 )
        X1 = X;
      if ( X > X2 )
        X2 = X;
    }
#else
    X1 = X2 = segments[0].pos;
    for ( nn = 1; nn < num_segments; nn++ )
    {
      FT_Int  X = segments[nn].pos;


      if ( X < X1 )
        X1 = X;
      if ( X > X2 )
        X2 = X;
    }
#endif

    if ( X1 >= X2 )
      return;

    warper->x1 = FT_MulFix( X1, org_scale ) + org_delta;
    warper->x2 = FT_MulFix( X2, org_scale ) + org_delta;

    warper->t1 = AF_WARPER_FLOOR( warper->x1 );
    warper->t2 = AF_WARPER_CEIL( warper->x2 );

    /* examine a half pixel wide range around the maximum coordinates */
    warper->x1min = warper->x1 & ~31;
    warper->x1max = warper->x1min + 32;
    warper->x2min = warper->x2 & ~31;
    warper->x2max = warper->x2min + 32;

    if ( warper->x1max > warper->x2 )
      warper->x1max = warper->x2;

    if ( warper->x2min < warper->x1 )
      warper->x2min = warper->x1;

    warper->w0 = warper->x2 - warper->x1;

    if ( warper->w0 <= 64 )
    {
      warper->x1max = warper->x1;
      warper->x2min = warper->x2;
    }

    /* examine (at most) a pixel wide range around the natural width */
    warper->wmin = warper->x2min - warper->x1max;
    warper->wmax = warper->x2max - warper->x1min;

#if 1
    /* some heuristics to reduce the number of widths to be examined */
    {
      int  margin = 16;


      if ( warper->w0 <= 128 )
      {
         margin = 8;
         if ( warper->w0 <= 96 )
           margin = 4;
      }

      if ( warper->wmin < warper->w0 - margin )
        warper->wmin = warper->w0 - margin;

      if ( warper->wmax > warper->w0 + margin )
        warper->wmax = warper->w0 + margin;
    }

    if ( warper->wmin < warper->w0 * 3 / 4 )
      warper->wmin = warper->w0 * 3 / 4;

    if ( warper->wmax > warper->w0 * 5 / 4 )
      warper->wmax = warper->w0 * 5 / 4;
#else
    /* no scaling, just translation */
    warper->wmin = warper->wmax = warper->w0;
#endif

    for ( w = warper->wmin; w <= warper->wmax; w++ )
    {
      FT_Fixed  new_scale;
      FT_Pos    new_delta;
      FT_Pos    xx1, xx2;


      /* compute min and max positions for given width,       */
      /* assuring that they stay within the coordinate ranges */
      xx1 = warper->x1;
      xx2 = warper->x2;
      if ( w >= warper->w0 )
      {
        xx1 -= w - warper->w0;
        if ( xx1 < warper->x1min )
        {
          xx2 += warper->x1min - xx1;
          xx1  = warper->x1min;
        }
      }
      else
      {
        xx1 -= w - warper->w0;
        if ( xx1 > warper->x1max )
        {
          xx2 -= xx1 - warper->x1max;
          xx1  = warper->x1max;
        }
      }

      if ( xx1 < warper->x1 )
        base_distort = warper->x1 - xx1;
      else
        base_distort = xx1 - warper->x1;

      if ( xx2 < warper->x2 )
        base_distort += warper->x2 - xx2;
      else
        base_distort += xx2 - warper->x2;

      /* give base distortion a greater weight while scoring */
      base_distort *= 10;

      new_scale = org_scale + FT_DivFix( w - warper->w0, X2 - X1 );
      new_delta = xx1 - FT_MulFix( X1, new_scale );

      af_warper_compute_line_best( warper, new_scale, new_delta, xx1, xx2,
                                   base_distort,
                                   segments, num_segments );
    }

    {
      FT_Fixed  best_scale = warper->best_scale;
      FT_Pos    best_delta = warper->best_delta;


      hints->xmin_delta = FT_MulFix( X1, best_scale - org_scale )
                          + best_delta;
      hints->xmax_delta = FT_MulFix( X2, best_scale - org_scale )
                          + best_delta;

      *a_scale = best_scale;
      *a_delta = best_delta;
    }
  }

#else /* !AF_CONFIG_OPTION_USE_WARPER */

  /* ANSI C doesn't like empty source files */
  typedef int  _af_warp_dummy;

#endif /* !AF_CONFIG_OPTION_USE_WARPER */

/* END */
#endif

/* END */
