/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: #ifdef jail to whip a few platforms into the UNIX ideal.

 ********************************************************************/
#ifndef _OS_TYPES_H
#define _OS_TYPES_H

#include "sys.h"


#define  _LOW_ACCURACY_
#define inline __inline
#define BYTE_ORDER  LITTLE_ENDIAN

#ifdef _LOW_ACCURACY_
#  define X(n) (((((n)>>22)+1)>>1) - ((((n)>>22)+1)>>9))
#  define LOOKUP_T const unsigned char
#else
#  define X(n) (n)
#  define LOOKUP_T const ogg_int32_t
#endif

#include "config_types.h"

extern void* ogg_malloc(u32 size);
extern void* ogg_calloc(u32 count,u32 size);
extern void *ogg_realloc(void *ptr,u32 size);
extern void ogg_free(void *ptr);
/* make it easy on the folks that want to compile the libs with a
   different malloc than stdlib */
#define _ogg_malloc  ogg_malloc
#define _ogg_calloc  ogg_calloc
#define _ogg_realloc ogg_realloc
#define _ogg_free    ogg_free

extern void* alloca(u32 size);

#endif  /* _OS_TYPES_H */
