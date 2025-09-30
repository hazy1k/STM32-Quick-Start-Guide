/***************************************************************************/
/*                                                                         */
/*  ftbase.c                                                               */
/*                                                                         */
/*    Single object library component (body only).                         */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2006, 2007, 2008, 2009 by       */
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

#define  FT_MAKE_OPTION_SINGLE_OBJECT

/***************************************************************************/
/*                                                                         */
/*  ftpic.c                                                                */
/*                                                                         */
/*    The FreeType position independent code services (body).              */
/*                                                                         */
/*  Copyright 2009, 2013 by                                                */
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
#include "basepic.h"

#ifdef FT_CONFIG_OPTION_PIC

  /* documentation is in ftpic.h */

  FT_BASE_DEF( FT_Error )
  ft_pic_container_init( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Error           error;


    FT_MEM_SET( pic_container, 0, sizeof ( *pic_container ) );

    error = ft_base_pic_init( library );
    if ( error )
      return error;

    return FT_Err_Ok;
  }


  /* Destroy the contents of the container. */
  FT_BASE_DEF( void )
  ft_pic_container_destroy( FT_Library  library )
  {
    ft_base_pic_free( library );
  }

#endif /* FT_CONFIG_OPTION_PIC */


/* END */
/***************************************************************************/
/*                                                                         */
/*  basepic.c                                                              */
/*                                                                         */
/*    The FreeType position independent code services for base.            */
/*                                                                         */
/*  Copyright 2009, 2012, 2013 by                                          */
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
#include "basepic.h"


#ifdef FT_CONFIG_OPTION_PIC

  /* forward declaration of PIC init functions from ftglyph.c */
  void
  FT_Init_Class_ft_outline_glyph_class( FT_Glyph_Class*  clazz );

  void
  FT_Init_Class_ft_bitmap_glyph_class( FT_Glyph_Class*  clazz );

#ifdef FT_CONFIG_OPTION_MAC_FONTS
  /* forward declaration of PIC init function from ftrfork.c */
  /* (not modularized)                                       */
  void
  FT_Init_Table_ft_raccess_guess_table( ft_raccess_guess_rec*  record );
#endif

  /* forward declaration of PIC init functions from ftinit.c */
  FT_Error
  ft_create_default_module_classes( FT_Library  library );

  void
  ft_destroy_default_module_classes( FT_Library  library );


  void
  ft_base_pic_free( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Memory          memory        = library->memory;


    if ( pic_container->base )
    {
      /* destroy default module classes            */
      /* (in case FT_Add_Default_Modules was used) */
      ft_destroy_default_module_classes( library );

      FT_FREE( pic_container->base );
      pic_container->base = NULL;
    }
  }


  FT_Error
  ft_base_pic_init( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Error           error         = FT_Err_Ok;
    BasePIC*           container     = NULL;
    FT_Memory          memory        = library->memory;


    /* allocate pointer, clear and set global container pointer */
    if ( FT_ALLOC( container, sizeof ( *container ) ) )
      return error;
    FT_MEM_SET( container, 0, sizeof ( *container ) );
    pic_container->base = container;

    /* initialize default modules list and pointers */
    error = ft_create_default_module_classes( library );
    if ( error )
      goto Exit;

    /* initialize pointer table -                       */
    /* this is how the module usually expects this data */
    FT_Init_Class_ft_outline_glyph_class(
      &container->ft_outline_glyph_class );
    FT_Init_Class_ft_bitmap_glyph_class(
      &container->ft_bitmap_glyph_class );
#ifdef FT_CONFIG_OPTION_MAC_FONTS
    FT_Init_Table_ft_raccess_guess_table(
      (ft_raccess_guess_rec*)&container->ft_raccess_guess_table );
#endif

  Exit:
    if ( error )
      ft_base_pic_free( library );
    return error;
  }

#endif /* FT_CONFIG_OPTION_PIC */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftadvanc.c                                                             */
/*                                                                         */
/*    Quick computation of advance widths (body).                          */
/*                                                                         */
/*  Copyright 2008, 2009, 2011, 2013 by                                    */
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
#include FT_INTERNAL_DEBUG_H

#include FT_ADVANCES_H
#include FT_INTERNAL_OBJECTS_H


  static FT_Error
  _ft_face_scale_advances( FT_Face    face,
                           FT_Fixed*  advances,
                           FT_UInt    count,
                           FT_Int32   flags )
  {
    FT_Fixed  scale;
    FT_UInt   nn;


    if ( flags & FT_LOAD_NO_SCALE )
      return FT_Err_Ok;

    if ( face->size == NULL )
      return FT_THROW( Invalid_Size_Handle );

    if ( flags & FT_LOAD_VERTICAL_LAYOUT )
      scale = face->size->metrics.y_scale;
    else
      scale = face->size->metrics.x_scale;

    /* this must be the same scaling as to get linear{Hori,Vert}Advance */
    /* (see `FT_Load_Glyph' implementation in src/base/ftobjs.c)        */

    for ( nn = 0; nn < count; nn++ )
      advances[nn] = FT_MulDiv( advances[nn], scale, 64 );

    return FT_Err_Ok;
  }


   /* at the moment, we can perform fast advance retrieval only in */
   /* the following cases:                                         */
   /*                                                              */
   /*  - unscaled load                                             */
   /*  - unhinted load                                             */
   /*  - light-hinted load                                         */

#define LOAD_ADVANCE_FAST_CHECK( flags )                            \
          ( flags & ( FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING )    || \
            FT_LOAD_TARGET_MODE( flags ) == FT_RENDER_MODE_LIGHT )


  /* documentation is in ftadvanc.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_Advance( FT_Face    face,
                  FT_UInt    gindex,
                  FT_Int32   flags,
                  FT_Fixed  *padvance )
  {
    FT_Face_GetAdvancesFunc  func;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( gindex >= (FT_UInt)face->num_glyphs )
      return FT_THROW( Invalid_Glyph_Index );

    func = face->driver->clazz->get_advances;
    if ( func && LOAD_ADVANCE_FAST_CHECK( flags ) )
    {
      FT_Error  error;


      error = func( face, gindex, 1, flags, padvance );
      if ( !error )
        return _ft_face_scale_advances( face, padvance, 1, flags );

      if ( FT_ERR_NEQ( error, Unimplemented_Feature ) )
        return error;
    }

    return FT_Get_Advances( face, gindex, 1, flags, padvance );
  }


  /* documentation is in ftadvanc.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_Advances( FT_Face    face,
                   FT_UInt    start,
                   FT_UInt    count,
                   FT_Int32   flags,
                   FT_Fixed  *padvances )
  {
    FT_Face_GetAdvancesFunc  func;
    FT_UInt                  num, end, nn;
    FT_Error                 error = FT_Err_Ok;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    num = (FT_UInt)face->num_glyphs;
    end = start + count;
    if ( start >= num || end < start || end > num )
      return FT_THROW( Invalid_Glyph_Index );

    if ( count == 0 )
      return FT_Err_Ok;

    func = face->driver->clazz->get_advances;
    if ( func && LOAD_ADVANCE_FAST_CHECK( flags ) )
    {
      error = func( face, start, count, flags, padvances );
      if ( !error )
        return _ft_face_scale_advances( face, padvances, count, flags );

      if ( FT_ERR_NEQ( error, Unimplemented_Feature ) )
        return error;
    }

    error = FT_Err_Ok;

    if ( flags & FT_ADVANCE_FLAG_FAST_ONLY )
      return FT_THROW( Unimplemented_Feature );

    flags |= (FT_UInt32)FT_LOAD_ADVANCE_ONLY;
    for ( nn = 0; nn < count; nn++ )
    {
      error = FT_Load_Glyph( face, start + nn, flags );
      if ( error )
        break;

      /* scale from 26.6 to 16.16 */
      padvances[nn] = ( flags & FT_LOAD_VERTICAL_LAYOUT )
                      ? face->glyph->advance.y << 10
                      : face->glyph->advance.x << 10;
    }

    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftcalc.c                                                               */
/*                                                                         */
/*    Arithmetic computations (body).                                      */
/*                                                                         */
/*  Copyright 1996-2006, 2008, 2012-2014 by                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* Support for 1-complement arithmetic has been totally dropped in this  */
  /* release.  You can still write your own code if you need it.           */
  /*                                                                       */
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* Implementing basic computation routines.                              */
  /*                                                                       */
  /* FT_MulDiv(), FT_MulFix(), FT_DivFix(), FT_RoundFix(), FT_CeilFix(),   */
  /* and FT_FloorFix() are declared in freetype.h.                         */
  /*                                                                       */
  /*************************************************************************/


#include "ft2build.h"
#include FT_GLYPH_H
#include FT_TRIGONOMETRY_H
#include FT_INTERNAL_CALC_H
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_OBJECTS_H

#ifdef FT_MULFIX_INLINED
#undef FT_MulFix
#endif

/* we need to emulate a 64-bit data type if a real one isn't available */

#ifndef FT_LONG64

  typedef struct  FT_Int64_
  {
    FT_UInt32  lo;
    FT_UInt32  hi;

  } FT_Int64;

#endif /* !FT_LONG64 */


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_calc


  /* The following three functions are available regardless of whether */
  /* FT_LONG64 is defined.                                             */

  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_RoundFix( FT_Fixed  a )
  {
    return ( a >= 0 ) ?   ( a + 0x8000L ) & ~0xFFFFL
                      : -((-a + 0x8000L ) & ~0xFFFFL );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_CeilFix( FT_Fixed  a )
  {
    return ( a >= 0 ) ?   ( a + 0xFFFFL ) & ~0xFFFFL
                      : -((-a + 0xFFFFL ) & ~0xFFFFL );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_FloorFix( FT_Fixed  a )
  {
    return ( a >= 0 ) ?   a & ~0xFFFFL
                      : -((-a) & ~0xFFFFL );
  }


  FT_BASE_DEF ( FT_Int )
  FT_MSB( FT_UInt32 z )
  {
    FT_Int shift = 0;

    /* determine msb bit index in `shift' */
    if ( z >= ( 1L << 16 ) )
    {
      z     >>= 16;
      shift  += 16;
    }
    if ( z >= ( 1L << 8 ) )
    {
      z     >>= 8;
      shift  += 8;
    }
    if ( z >= ( 1L << 4 ) )
    {
      z     >>= 4;
      shift  += 4;
    }
    if ( z >= ( 1L << 2 ) )
    {
      z     >>= 2;
      shift  += 2;
    }
    if ( z >= ( 1L << 1 ) )
    {
   /* z     >>= 1; */
      shift  += 1;
    }

    return shift;
  }


  /* documentation is in ftcalc.h */

  FT_BASE_DEF( FT_Fixed )
  FT_Hypot( FT_Fixed  x,
            FT_Fixed  y )
  {
    FT_Vector  v;


    v.x = x;
    v.y = y;

    return FT_Vector_Length( &v );
  }


#ifdef FT_LONG64


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Long )
  FT_MulDiv( FT_Long  a,
             FT_Long  b,
             FT_Long  c )
  {
    FT_Int   s;
    FT_Long  d;


    s = 1;
    if ( a < 0 ) { a = -a; s = -1; }
    if ( b < 0 ) { b = -b; s = -s; }
    if ( c < 0 ) { c = -c; s = -s; }

    d = (FT_Long)( c > 0 ? ( (FT_Int64)a * b + ( c >> 1 ) ) / c
                         : 0x7FFFFFFFL );

    return ( s > 0 ) ? d : -d;
  }


  /* documentation is in ftcalc.h */

  FT_BASE_DEF( FT_Long )
  FT_MulDiv_No_Round( FT_Long  a,
                      FT_Long  b,
                      FT_Long  c )
  {
    FT_Int   s;
    FT_Long  d;


    s = 1;
    if ( a < 0 ) { a = -a; s = -1; }
    if ( b < 0 ) { b = -b; s = -s; }
    if ( c < 0 ) { c = -c; s = -s; }

    d = (FT_Long)( c > 0 ? (FT_Int64)a * b / c
                         : 0x7FFFFFFFL );

    return ( s > 0 ) ? d : -d;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Long )
  FT_MulFix( FT_Long  a,
             FT_Long  b )
  {
#ifdef FT_MULFIX_ASSEMBLER

    return FT_MULFIX_ASSEMBLER( a, b );

#else

    FT_Int   s = 1;
    FT_Long  c;


    if ( a < 0 )
    {
      a = -a;
      s = -1;
    }

    if ( b < 0 )
    {
      b = -b;
      s = -s;
    }

    c = (FT_Long)( ( (FT_Int64)a * b + 0x8000L ) >> 16 );

    return ( s > 0 ) ? c : -c;

#endif /* FT_MULFIX_ASSEMBLER */
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Long )
  FT_DivFix( FT_Long  a,
             FT_Long  b )
  {
    FT_Int32   s;
    FT_UInt32  q;


    s = 1;
    if ( a < 0 )
    {
      a = -a;
      s = -1;
    }
    if ( b < 0 )
    {
      b = -b;
      s = -s;
    }

    if ( b == 0 )
      /* check for division by 0 */
      q = 0x7FFFFFFFL;
    else
      /* compute result directly */
      q = (FT_UInt32)( ( ( (FT_UInt64)a << 16 ) + ( b >> 1 ) ) / b );

    return ( s < 0 ? -(FT_Long)q : (FT_Long)q );
  }


#else /* !FT_LONG64 */


  static void
  ft_multo64( FT_UInt32  x,
              FT_UInt32  y,
              FT_Int64  *z )
  {
    FT_UInt32  lo1, hi1, lo2, hi2, lo, hi, i1, i2;


    lo1 = x & 0x0000FFFFU;  hi1 = x >> 16;
    lo2 = y & 0x0000FFFFU;  hi2 = y >> 16;

    lo = lo1 * lo2;
    i1 = lo1 * hi2;
    i2 = lo2 * hi1;
    hi = hi1 * hi2;

    /* Check carry overflow of i1 + i2 */
    i1 += i2;
    hi += (FT_UInt32)( i1 < i2 ) << 16;

    hi += i1 >> 16;
    i1  = i1 << 16;

    /* Check carry overflow of i1 + lo */
    lo += i1;
    hi += ( lo < i1 );

    z->lo = lo;
    z->hi = hi;
  }


  static FT_UInt32
  ft_div64by32( FT_UInt32  hi,
                FT_UInt32  lo,
                FT_UInt32  y )
  {
    FT_UInt32  r, q;
    FT_Int     i;


    q = 0;
    r = hi;

    if ( r >= y )
      return (FT_UInt32)0x7FFFFFFFL;

    i = 32;
    do
    {
      r <<= 1;
      q <<= 1;
      r  |= lo >> 31;

      if ( r >= y )
      {
        r -= y;
        q |= 1;
      }
      lo <<= 1;
    } while ( --i );

    return q;
  }


  static void
  FT_Add64( FT_Int64*  x,
            FT_Int64*  y,
            FT_Int64  *z )
  {
    register FT_UInt32  lo, hi;


    lo = x->lo + y->lo;
    hi = x->hi + y->hi + ( lo < x->lo );

    z->lo = lo;
    z->hi = hi;
  }


  /* documentation is in freetype.h */

  /* The FT_MulDiv function has been optimized thanks to ideas from      */
  /* Graham Asher.  The trick is to optimize computation when everything */
  /* fits within 32-bits (a rather common case).                         */
  /*                                                                     */
  /*  we compute 'a*b+c/2', then divide it by 'c'. (positive values)     */
  /*                                                                     */
  /*  46340 is FLOOR(SQRT(2^31-1)).                                      */
  /*                                                                     */
  /*  if ( a <= 46340 && b <= 46340 ) then ( a*b <= 0x7FFEA810 )         */
  /*                                                                     */
  /*  0x7FFFFFFF - 0x7FFEA810 = 0x157F0                                  */
  /*                                                                     */
  /*  if ( c < 0x157F0*2 ) then ( a*b+c/2 <= 0x7FFFFFFF )                */
  /*                                                                     */
  /*  and 2*0x157F0 = 176096                                             */
  /*                                                                     */

  FT_EXPORT_DEF( FT_Long )
  FT_MulDiv( FT_Long  a,
             FT_Long  b,
             FT_Long  c )
  {
    long  s;


    /* XXX: this function does not allow 64-bit arguments */
    if ( a == 0 || b == c )
      return a;

    s  = a; a = FT_ABS( a );
    s ^= b; b = FT_ABS( b );
    s ^= c; c = FT_ABS( c );

    if ( a <= 46340L && b <= 46340L && c <= 176095L && c > 0 )
      a = ( a * b + ( c >> 1 ) ) / c;

    else if ( (FT_Int32)c > 0 )
    {
      FT_Int64  temp, temp2;


      ft_multo64( (FT_Int32)a, (FT_Int32)b, &temp );

      temp2.hi = 0;
      temp2.lo = (FT_UInt32)(c >> 1);
      FT_Add64( &temp, &temp2, &temp );
      a = ft_div64by32( temp.hi, temp.lo, (FT_Int32)c );
    }
    else
      a = 0x7FFFFFFFL;

    return ( s < 0 ? -a : a );
  }


  FT_BASE_DEF( FT_Long )
  FT_MulDiv_No_Round( FT_Long  a,
                      FT_Long  b,
                      FT_Long  c )
  {
    long  s;


    if ( a == 0 || b == c )
      return a;

    s  = a; a = FT_ABS( a );
    s ^= b; b = FT_ABS( b );
    s ^= c; c = FT_ABS( c );

    if ( a <= 46340L && b <= 46340L && c > 0 )
      a = a * b / c;

    else if ( (FT_Int32)c > 0 )
    {
      FT_Int64  temp;


      ft_multo64( (FT_Int32)a, (FT_Int32)b, &temp );
      a = ft_div64by32( temp.hi, temp.lo, (FT_Int32)c );
    }
    else
      a = 0x7FFFFFFFL;

    return ( s < 0 ? -a : a );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Long )
  FT_MulFix( FT_Long  a,
             FT_Long  b )
  {
#ifdef FT_MULFIX_ASSEMBLER

    return FT_MULFIX_ASSEMBLER( a, b );

#elif 0

    /*
     *  This code is nonportable.  See comment below.
     *
     *  However, on a platform where right-shift of a signed quantity fills
     *  the leftmost bits by copying the sign bit, it might be faster.
     */

    FT_Long   sa, sb;
    FT_ULong  ua, ub;


    if ( a == 0 || b == 0x10000L )
      return a;

    /*
     *  This is a clever way of converting a signed number `a' into its
     *  absolute value (stored back into `a') and its sign.  The sign is
     *  stored in `sa'; 0 means `a' was positive or zero, and -1 means `a'
     *  was negative.  (Similarly for `b' and `sb').
     *
     *  Unfortunately, it doesn't work (at least not portably).
     *
     *  It makes the assumption that right-shift on a negative signed value
     *  fills the leftmost bits by copying the sign bit.  This is wrong.
     *  According to K&R 2nd ed, section `A7.8 Shift Operators' on page 206,
     *  the result of right-shift of a negative signed value is
     *  implementation-defined.  At least one implementation fills the
     *  leftmost bits with 0s (i.e., it is exactly the same as an unsigned
     *  right shift).  This means that when `a' is negative, `sa' ends up
     *  with the value 1 rather than -1.  After that, everything else goes
     *  wrong.
     */
    sa = ( a >> ( sizeof ( a ) * 8 - 1 ) );
    a  = ( a ^ sa ) - sa;
    sb = ( b >> ( sizeof ( b ) * 8 - 1 ) );
    b  = ( b ^ sb ) - sb;

    ua = (FT_ULong)a;
    ub = (FT_ULong)b;

    if ( ua <= 2048 && ub <= 1048576L )
      ua = ( ua * ub + 0x8000U ) >> 16;
    else
    {
      FT_ULong  al = ua & 0xFFFFU;


      ua = ( ua >> 16 ) * ub +  al * ( ub >> 16 ) +
           ( ( al * ( ub & 0xFFFFU ) + 0x8000U ) >> 16 );
    }

    sa ^= sb,
    ua  = (FT_ULong)(( ua ^ sa ) - sa);

    return (FT_Long)ua;

#else /* 0 */

    FT_Long   s;
    FT_ULong  ua, ub;


    if ( a == 0 || b == 0x10000L )
      return a;

    s  = a; a = FT_ABS( a );
    s ^= b; b = FT_ABS( b );

    ua = (FT_ULong)a;
    ub = (FT_ULong)b;

    if ( ua <= 2048 && ub <= 1048576L )
      ua = ( ua * ub + 0x8000UL ) >> 16;
    else
    {
      FT_ULong  al = ua & 0xFFFFUL;


      ua = ( ua >> 16 ) * ub +  al * ( ub >> 16 ) +
           ( ( al * ( ub & 0xFFFFUL ) + 0x8000UL ) >> 16 );
    }

    return ( s < 0 ? -(FT_Long)ua : (FT_Long)ua );

#endif /* 0 */

  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Long )
  FT_DivFix( FT_Long  a,
             FT_Long  b )
  {
    FT_Int32   s;
    FT_UInt32  q;


    /* XXX: this function does not allow 64-bit arguments */
    s  = (FT_Int32)a; a = FT_ABS( a );
    s ^= (FT_Int32)b; b = FT_ABS( b );

    if ( (FT_UInt32)b == 0 )
    {
      /* check for division by 0 */
      q = (FT_UInt32)0x7FFFFFFFL;
    }
    else if ( ( a >> 16 ) == 0 )
    {
      /* compute result directly */
      q = (FT_UInt32)( ( (FT_ULong)a << 16 ) + ( b >> 1 ) ) / (FT_UInt32)b;
    }
    else
    {
      /* we need more bits; we have to do it by hand */
      FT_Int64  temp, temp2;


      temp.hi  = (FT_Int32)( a >> 16 );
      temp.lo  = (FT_UInt32)a << 16;
      temp2.hi = 0;
      temp2.lo = (FT_UInt32)( b >> 1 );
      FT_Add64( &temp, &temp2, &temp );
      q = ft_div64by32( temp.hi, temp.lo, (FT_Int32)b );
    }

    return ( s < 0 ? -(FT_Int32)q : (FT_Int32)q );
  }


#if 0

  /* documentation is in ftcalc.h */

  FT_EXPORT_DEF( void )
  FT_MulTo64( FT_Int32   x,
              FT_Int32   y,
              FT_Int64  *z )
  {
    FT_Int32  s;


    s  = x; x = FT_ABS( x );
    s ^= y; y = FT_ABS( y );

    ft_multo64( x, y, z );

    if ( s < 0 )
    {
      z->lo = (FT_UInt32)-(FT_Int32)z->lo;
      z->hi = ~z->hi + !( z->lo );
    }
  }


  /* apparently, the second version of this code is not compiled correctly */
  /* on Mac machines with the MPW C compiler..  tsk, tsk, tsk...           */

#if 1

  FT_EXPORT_DEF( FT_Int32 )
  FT_Div64by32( FT_Int64*  x,
                FT_Int32   y )
  {
    FT_Int32   s;
    FT_UInt32  q, r, i, lo;


    s  = x->hi;
    if ( s < 0 )
    {
      x->lo = (FT_UInt32)-(FT_Int32)x->lo;
      x->hi = ~x->hi + !x->lo;
    }
    s ^= y;  y = FT_ABS( y );

    /* Shortcut */
    if ( x->hi == 0 )
    {
      if ( y > 0 )
        q = x->lo / y;
      else
        q = 0x7FFFFFFFL;

      return ( s < 0 ? -(FT_Int32)q : (FT_Int32)q );
    }

    r  = x->hi;
    lo = x->lo;

    if ( r >= (FT_UInt32)y ) /* we know y is to be treated as unsigned here */
      return ( s < 0 ? 0x80000001UL : 0x7FFFFFFFUL );
                             /* Return Max/Min Int32 if division overflow. */
                             /* This includes division by zero!            */
    q = 0;
    for ( i = 0; i < 32; i++ )
    {
      r <<= 1;
      q <<= 1;
      r  |= lo >> 31;

      if ( r >= (FT_UInt32)y )
      {
        r -= y;
        q |= 1;
      }
      lo <<= 1;
    }

    return ( s < 0 ? -(FT_Int32)q : (FT_Int32)q );
  }

#else /* 0 */

  FT_EXPORT_DEF( FT_Int32 )
  FT_Div64by32( FT_Int64*  x,
                FT_Int32   y )
  {
    FT_Int32   s;
    FT_UInt32  q;


    s  = x->hi;
    if ( s < 0 )
    {
      x->lo = (FT_UInt32)-(FT_Int32)x->lo;
      x->hi = ~x->hi + !x->lo;
    }
    s ^= y;  y = FT_ABS( y );

    /* Shortcut */
    if ( x->hi == 0 )
    {
      if ( y > 0 )
        q = ( x->lo + ( y >> 1 ) ) / y;
      else
        q = 0x7FFFFFFFL;

      return ( s < 0 ? -(FT_Int32)q : (FT_Int32)q );
    }

    q = ft_div64by32( x->hi, x->lo, y );

    return ( s < 0 ? -(FT_Int32)q : (FT_Int32)q );
  }

#endif /* 0 */

#endif /* 0 */


#endif /* FT_LONG64 */


  /* documentation is in ftglyph.h */

  FT_EXPORT_DEF( void )
  FT_Matrix_Multiply( const FT_Matrix*  a,
                      FT_Matrix        *b )
  {
    FT_Fixed  xx, xy, yx, yy;


    if ( !a || !b )
      return;

    xx = FT_MulFix( a->xx, b->xx ) + FT_MulFix( a->xy, b->yx );
    xy = FT_MulFix( a->xx, b->xy ) + FT_MulFix( a->xy, b->yy );
    yx = FT_MulFix( a->yx, b->xx ) + FT_MulFix( a->yy, b->yx );
    yy = FT_MulFix( a->yx, b->xy ) + FT_MulFix( a->yy, b->yy );

    b->xx = xx;  b->xy = xy;
    b->yx = yx;  b->yy = yy;
  }


  /* documentation is in ftglyph.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Matrix_Invert( FT_Matrix*  matrix )
  {
    FT_Pos  delta, xx, yy;


    if ( !matrix )
      return FT_THROW( Invalid_Argument );

    /* compute discriminant */
    delta = FT_MulFix( matrix->xx, matrix->yy ) -
            FT_MulFix( matrix->xy, matrix->yx );

    if ( !delta )
      return FT_THROW( Invalid_Argument );  /* matrix can't be inverted */

    matrix->xy = - FT_DivFix( matrix->xy, delta );
    matrix->yx = - FT_DivFix( matrix->yx, delta );

    xx = matrix->xx;
    yy = matrix->yy;

    matrix->xx = FT_DivFix( yy, delta );
    matrix->yy = FT_DivFix( xx, delta );

    return FT_Err_Ok;
  }


  /* documentation is in ftcalc.h */

  FT_BASE_DEF( void )
  FT_Matrix_Multiply_Scaled( const FT_Matrix*  a,
                             FT_Matrix        *b,
                             FT_Long           scaling )
  {
    FT_Fixed  xx, xy, yx, yy;

    FT_Long   val = 0x10000L * scaling;


    if ( !a || !b )
      return;

    xx = FT_MulDiv( a->xx, b->xx, val ) + FT_MulDiv( a->xy, b->yx, val );
    xy = FT_MulDiv( a->xx, b->xy, val ) + FT_MulDiv( a->xy, b->yy, val );
    yx = FT_MulDiv( a->yx, b->xx, val ) + FT_MulDiv( a->yy, b->yx, val );
    yy = FT_MulDiv( a->yx, b->xy, val ) + FT_MulDiv( a->yy, b->yy, val );

    b->xx = xx;  b->xy = xy;
    b->yx = yx;  b->yy = yy;
  }


  /* documentation is in ftcalc.h */

  FT_BASE_DEF( void )
  FT_Vector_Transform_Scaled( FT_Vector*        vector,
                              const FT_Matrix*  matrix,
                              FT_Long           scaling )
  {
    FT_Pos   xz, yz;

    FT_Long  val = 0x10000L * scaling;


    if ( !vector || !matrix )
      return;

    xz = FT_MulDiv( vector->x, matrix->xx, val ) +
         FT_MulDiv( vector->y, matrix->xy, val );

    yz = FT_MulDiv( vector->x, matrix->yx, val ) +
         FT_MulDiv( vector->y, matrix->yy, val );

    vector->x = xz;
    vector->y = yz;
  }


#if 0

  /* documentation is in ftcalc.h */

  FT_BASE_DEF( FT_Int32 )
  FT_SqrtFixed( FT_Int32  x )
  {
    FT_UInt32  root, rem_hi, rem_lo, test_div;
    FT_Int     count;


    root = 0;

    if ( x > 0 )
    {
      rem_hi = 0;
      rem_lo = x;
      count  = 24;
      do
      {
        rem_hi   = ( rem_hi << 2 ) | ( rem_lo >> 30 );
        rem_lo <<= 2;
        root   <<= 1;
        test_div = ( root << 1 ) + 1;

        if ( rem_hi >= test_div )
        {
          rem_hi -= test_div;
          root   += 1;
        }
      } while ( --count );
    }

    return (FT_Int32)root;
  }

#endif /* 0 */


  /* documentation is in ftcalc.h */

  FT_BASE_DEF( FT_Int )
  ft_corner_orientation( FT_Pos  in_x,
                         FT_Pos  in_y,
                         FT_Pos  out_x,
                         FT_Pos  out_y )
  {
    FT_Long  result; /* avoid overflow on 16-bit system */


    /* deal with the trivial cases quickly */
    if ( in_y == 0 )
    {
      if ( in_x >= 0 )
        result = out_y;
      else
        result = -out_y;
    }
    else if ( in_x == 0 )
    {
      if ( in_y >= 0 )
        result = -out_x;
      else
        result = out_x;
    }
    else if ( out_y == 0 )
    {
      if ( out_x >= 0 )
        result = in_y;
      else
        result = -in_y;
    }
    else if ( out_x == 0 )
    {
      if ( out_y >= 0 )
        result = -in_x;
      else
        result =  in_x;
    }
    else /* general case */
    {
#ifdef FT_LONG64

      FT_Int64  delta = (FT_Int64)in_x * out_y - (FT_Int64)in_y * out_x;


      if ( delta == 0 )
        result = 0;
      else
        result = 1 - 2 * ( delta < 0 );

#else

      FT_Int64  z1, z2;


      /* XXX: this function does not allow 64-bit arguments */
      ft_multo64( (FT_Int32)in_x, (FT_Int32)out_y, &z1 );
      ft_multo64( (FT_Int32)in_y, (FT_Int32)out_x, &z2 );

      if ( z1.hi > z2.hi )
        result = +1;
      else if ( z1.hi < z2.hi )
        result = -1;
      else if ( z1.lo > z2.lo )
        result = +1;
      else if ( z1.lo < z2.lo )
        result = -1;
      else
        result = 0;

#endif
    }

    /* XXX: only the sign of return value, +1/0/-1 must be used */
    return (FT_Int)result;
  }


  /* documentation is in ftcalc.h */

  FT_BASE_DEF( FT_Int )
  ft_corner_is_flat( FT_Pos  in_x,
                     FT_Pos  in_y,
                     FT_Pos  out_x,
                     FT_Pos  out_y )
  {
    FT_Pos  ax = in_x;
    FT_Pos  ay = in_y;

    FT_Pos  d_in, d_out, d_corner;


    /* We approximate the Euclidean metric (sqrt(x^2 + y^2)) with */
    /* the Taxicab metric (|x| + |y|), which can be computed much */
    /* faster.  If one of the two vectors is much longer than the */
    /* other one, the direction of the shorter vector doesn't     */
    /* influence the result any more.                             */
    /*                                                            */
    /*                 corner                                     */
    /*       x---------------------------x                        */
    /*        \                      /                            */
    /*         \                /                                 */
    /*      in  \          /  out                                 */
    /*           \    /                                           */
    /*            o                                               */
    /*              Point                                         */
    /*                                                            */

    if ( ax < 0 )
      ax = -ax;
    if ( ay < 0 )
      ay = -ay;
    d_in = ax + ay;  /* d_in = || in || */

    ax = out_x;
    if ( ax < 0 )
      ax = -ax;
    ay = out_y;
    if ( ay < 0 )
      ay = -ay;
    d_out = ax + ay;  /* d_out = || out || */

    ax = out_x + in_x;
    if ( ax < 0 )
      ax = -ax;
    ay = out_y + in_y;
    if ( ay < 0 )
      ay = -ay;
    d_corner = ax + ay;  /* d_corner = || in + out || */

    /* now do a simple length comparison: */
    /*                                    */
    /*   d_in + d_out < 17/16 d_corner    */

    return ( d_in + d_out - d_corner ) < ( d_corner >> 4 );
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftdbgmem.c                                                             */
/*                                                                         */
/*    Memory debugger (body).                                              */
/*                                                                         */
/*  Copyright 2001-2006, 2009, 2013 by                                     */
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
#include FT_CONFIG_CONFIG_H
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_MEMORY_H
#include FT_SYSTEM_H
#include FT_ERRORS_H
#include FT_TYPES_H


#ifdef FT_DEBUG_MEMORY

#define  KEEPALIVE /* `Keep alive' means that freed blocks aren't released
                    * to the heap.  This is useful to detect double-frees
                    * or weird heap corruption, but it uses large amounts of
                    * memory, however.
                    */

#include FT_CONFIG_STANDARD_LIBRARY_H

  FT_BASE_DEF( const char* )  _ft_debug_file   = 0;
  FT_BASE_DEF( long )         _ft_debug_lineno = 0;

  extern void
  FT_DumpMemory( FT_Memory  memory );


  typedef struct FT_MemSourceRec_*  FT_MemSource;
  typedef struct FT_MemNodeRec_*    FT_MemNode;
  typedef struct FT_MemTableRec_*   FT_MemTable;


#define FT_MEM_VAL( addr )  ((FT_PtrDist)(FT_Pointer)( addr ))

  /*
   *  This structure holds statistics for a single allocation/release
   *  site.  This is useful to know where memory operations happen the
   *  most.
   */
  typedef struct  FT_MemSourceRec_
  {
    const char*   file_name;
    long          line_no;

    FT_Long       cur_blocks;   /* current number of allocated blocks */
    FT_Long       max_blocks;   /* max. number of allocated blocks    */
    FT_Long       all_blocks;   /* total number of blocks allocated   */

    FT_Long       cur_size;     /* current cumulative allocated size */
    FT_Long       max_size;     /* maximum cumulative allocated size */
    FT_Long       all_size;     /* total cumulative allocated size   */

    FT_Long       cur_max;      /* current maximum allocated size */

    FT_UInt32     hash;
    FT_MemSource  link;

  } FT_MemSourceRec;


  /*
   *  We don't need a resizable array for the memory sources, because
   *  their number is pretty limited within FreeType.
   */
#define FT_MEM_SOURCE_BUCKETS  128

  /*
   *  This structure holds information related to a single allocated
   *  memory block.  If KEEPALIVE is defined, blocks that are freed by
   *  FreeType are never released to the system.  Instead, their `size'
   *  field is set to -size.  This is mainly useful to detect double frees,
   *  at the price of large memory footprint during execution.
   */
  typedef struct  FT_MemNodeRec_
  {
    FT_Byte*      address;
    FT_Long       size;     /* < 0 if the block was freed */

    FT_MemSource  source;

#ifdef KEEPALIVE
    const char*   free_file_name;
    FT_Long       free_line_no;
#endif

    FT_MemNode    link;

  } FT_MemNodeRec;


  /*
   *  The global structure, containing compound statistics and all hash
   *  tables.
   */
  typedef struct  FT_MemTableRec_
  {
    FT_ULong         size;
    FT_ULong         nodes;
    FT_MemNode*      buckets;

    FT_ULong         alloc_total;
    FT_ULong         alloc_current;
    FT_ULong         alloc_max;
    FT_ULong         alloc_count;

    FT_Bool          bound_total;
    FT_ULong         alloc_total_max;

    FT_Bool          bound_count;
    FT_ULong         alloc_count_max;

    FT_MemSource     sources[FT_MEM_SOURCE_BUCKETS];

    FT_Bool          keep_alive;

    FT_Memory        memory;
    FT_Pointer       memory_user;
    FT_Alloc_Func    alloc;
    FT_Free_Func     free;
    FT_Realloc_Func  realloc;

  } FT_MemTableRec;


#define FT_MEM_SIZE_MIN  7
#define FT_MEM_SIZE_MAX  13845163

#define FT_FILENAME( x )  ((x) ? (x) : "unknown file")


  /*
   *  Prime numbers are ugly to handle.  It would be better to implement
   *  L-Hashing, which is 10% faster and doesn't require divisions.
   */
  static const FT_UInt  ft_mem_primes[] =
  {
    7,
    11,
    19,
    37,
    73,
    109,
    163,
    251,
    367,
    557,
    823,
    1237,
    1861,
    2777,
    4177,
    6247,
    9371,
    14057,
    21089,
    31627,
    47431,
    71143,
    106721,
    160073,
    240101,
    360163,
    540217,
    810343,
    1215497,
    1823231,
    2734867,
    4102283,
    6153409,
    9230113,
    13845163,
  };


  static FT_ULong
  ft_mem_closest_prime( FT_ULong  num )
  {
    FT_UInt  i;


    for ( i = 0;
          i < sizeof ( ft_mem_primes ) / sizeof ( ft_mem_primes[0] ); i++ )
      if ( ft_mem_primes[i] > num )
        return ft_mem_primes[i];

    return FT_MEM_SIZE_MAX;
  }


  extern void
  ft_mem_debug_panic( const char*  fmt,
                      ... )
  {
    va_list  ap;


    printf( "FreeType.Debug: " );

    va_start( ap, fmt );
    vprintf( fmt, ap );
    va_end( ap );

    printf( "\n" );
    exit( EXIT_FAILURE );
  }


  static FT_Pointer
  ft_mem_table_alloc( FT_MemTable  table,
                      FT_Long      size )
  {
    FT_Memory   memory = table->memory;
    FT_Pointer  block;


    memory->user = table->memory_user;
    block = table->alloc( memory, size );
    memory->user = table;

    return block;
  }


  static void
  ft_mem_table_free( FT_MemTable  table,
                     FT_Pointer   block )
  {
    FT_Memory  memory = table->memory;


    memory->user = table->memory_user;
    table->free( memory, block );
    memory->user = table;
  }


  static void
  ft_mem_table_resize( FT_MemTable  table )
  {
    FT_ULong  new_size;


    new_size = ft_mem_closest_prime( table->nodes );
    if ( new_size != table->size )
    {
      FT_MemNode*  new_buckets;
      FT_ULong     i;


      new_buckets = (FT_MemNode *)
                      ft_mem_table_alloc( table,
                                          new_size * sizeof ( FT_MemNode ) );
      if ( new_buckets == NULL )
        return;

      FT_ARRAY_ZERO( new_buckets, new_size );

      for ( i = 0; i < table->size; i++ )
      {
        FT_MemNode  node, next, *pnode;
        FT_PtrDist  hash;


        node = table->buckets[i];
        while ( node )
        {
          next  = node->link;
          hash  = FT_MEM_VAL( node->address ) % new_size;
          pnode = new_buckets + hash;

          node->link = pnode[0];
          pnode[0]   = node;

          node = next;
        }
      }

      if ( table->buckets )
        ft_mem_table_free( table, table->buckets );

      table->buckets = new_buckets;
      table->size    = new_size;
    }
  }


  static FT_MemTable
  ft_mem_table_new( FT_Memory  memory )
  {
    FT_MemTable  table;


    table = (FT_MemTable)memory->alloc( memory, sizeof ( *table ) );
    if ( table == NULL )
      goto Exit;

    FT_ZERO( table );

    table->size  = FT_MEM_SIZE_MIN;
    table->nodes = 0;

    table->memory = memory;

    table->memory_user = memory->user;

    table->alloc   = memory->alloc;
    table->realloc = memory->realloc;
    table->free    = memory->free;

    table->buckets = (FT_MemNode *)
                       memory->alloc( memory,
                                      table->size * sizeof ( FT_MemNode ) );
    if ( table->buckets )
      FT_ARRAY_ZERO( table->buckets, table->size );
    else
    {
      memory->free( memory, table );
      table = NULL;
    }

  Exit:
    return table;
  }


  static void
  ft_mem_table_destroy( FT_MemTable  table )
  {
    FT_ULong  i;
    FT_Long   leak_count = 0;
    FT_ULong  leaks      = 0;


    FT_DumpMemory( table->memory );

    /* remove all blocks from the table, revealing leaked ones */
    for ( i = 0; i < table->size; i++ )
    {
      FT_MemNode  *pnode = table->buckets + i, next, node = *pnode;


      while ( node )
      {
        next       = node->link;
        node->link = 0;

        if ( node->size > 0 )
        {
          printf(
            "leaked memory block at address %p, size %8ld in (%s:%ld)\n",
            node->address, node->size,
            FT_FILENAME( node->source->file_name ),
            node->source->line_no );

          leak_count++;
          leaks += node->size;

          ft_mem_table_free( table, node->address );
        }

        node->address = NULL;
        node->size    = 0;

        ft_mem_table_free( table, node );
        node = next;
      }
      table->buckets[i] = 0;
    }

    ft_mem_table_free( table, table->buckets );
    table->buckets = NULL;

    table->size  = 0;
    table->nodes = 0;

    /* remove all sources */
    for ( i = 0; i < FT_MEM_SOURCE_BUCKETS; i++ )
    {
      FT_MemSource  source, next;


      for ( source = table->sources[i]; source != NULL; source = next )
      {
        next = source->link;
        ft_mem_table_free( table, source );
      }

      table->sources[i] = NULL;
    }

    printf( "FreeType: total memory allocations = %ld\n",
            table->alloc_total );
    printf( "FreeType: maximum memory footprint = %ld\n",
            table->alloc_max );

    ft_mem_table_free( table, table );

    if ( leak_count > 0 )
      ft_mem_debug_panic(
        "FreeType: %ld bytes of memory leaked in %ld blocks\n",
        leaks, leak_count );

    printf( "FreeType: no memory leaks detected\n" );
  }


  static FT_MemNode*
  ft_mem_table_get_nodep( FT_MemTable  table,
                          FT_Byte*     address )
  {
    FT_PtrDist   hash;
    FT_MemNode  *pnode, node;


    hash  = FT_MEM_VAL( address );
    pnode = table->buckets + ( hash % table->size );

    for (;;)
    {
      node = pnode[0];
      if ( !node )
        break;

      if ( node->address == address )
        break;

      pnode = &node->link;
    }
    return pnode;
  }


  static FT_MemSource
  ft_mem_table_get_source( FT_MemTable  table )
  {
    FT_UInt32     hash;
    FT_MemSource  node, *pnode;


    /* cast to FT_PtrDist first since void* can be larger */
    /* than FT_UInt32 and GCC 4.1.1 emits a warning       */
    hash  = (FT_UInt32)(FT_PtrDist)(void*)_ft_debug_file +
              (FT_UInt32)( 5 * _ft_debug_lineno );
    pnode = &table->sources[hash % FT_MEM_SOURCE_BUCKETS];

    for ( ;; )
    {
      node = *pnode;
      if ( node == NULL )
        break;

      if ( node->file_name == _ft_debug_file &&
           node->line_no   == _ft_debug_lineno   )
        goto Exit;

      pnode = &node->link;
    }

    node = (FT_MemSource)ft_mem_table_alloc( table, sizeof ( *node ) );
    if ( node == NULL )
      ft_mem_debug_panic(
        "not enough memory to perform memory debugging\n" );

    node->file_name = _ft_debug_file;
    node->line_no   = _ft_debug_lineno;

    node->cur_blocks = 0;
    node->max_blocks = 0;
    node->all_blocks = 0;

    node->cur_size   = 0;
    node->max_size   = 0;
    node->all_size   = 0;

    node->cur_max    = 0;

    node->link = NULL;
    node->hash = hash;
    *pnode     = node;

  Exit:
    return node;
  }


  static void
  ft_mem_table_set( FT_MemTable  table,
                    FT_Byte*     address,
                    FT_ULong     size,
                    FT_Long      delta )
  {
    FT_MemNode  *pnode, node;


    if ( table )
    {
      FT_MemSource  source;


      pnode = ft_mem_table_get_nodep( table, address );
      node  = *pnode;
      if ( node )
      {
        if ( node->size < 0 )
        {
          /* This block was already freed.  Our memory is now completely */
          /* corrupted!                                                  */
          /* This can only happen in keep-alive mode.                    */
          ft_mem_debug_panic(
            "memory heap corrupted (allocating freed block)" );
        }
        else
        {
          /* This block was already allocated.  This means that our memory */
          /* is also corrupted!                                            */
          ft_mem_debug_panic(
            "memory heap corrupted (re-allocating allocated block at"
            " %p, of size %ld)\n"
            "org=%s:%d new=%s:%d\n",
            node->address, node->size,
            FT_FILENAME( node->source->file_name ), node->source->line_no,
            FT_FILENAME( _ft_debug_file ), _ft_debug_lineno );
        }
      }

      /* we need to create a new node in this table */
      node = (FT_MemNode)ft_mem_table_alloc( table, sizeof ( *node ) );
      if ( node == NULL )
        ft_mem_debug_panic( "not enough memory to run memory tests" );

      node->address = address;
      node->size    = size;
      node->source  = source = ft_mem_table_get_source( table );

      if ( delta == 0 )
      {
        /* this is an allocation */
        source->all_blocks++;
        source->cur_blocks++;
        if ( source->cur_blocks > source->max_blocks )
          source->max_blocks = source->cur_blocks;
      }

      if ( size > (FT_ULong)source->cur_max )
        source->cur_max = size;

      if ( delta != 0 )
      {
        /* we are growing or shrinking a reallocated block */
        source->cur_size     += delta;
        table->alloc_current += delta;
      }
      else
      {
        /* we are allocating a new block */
        source->cur_size     += size;
        table->alloc_current += size;
      }

      source->all_size += size;

      if ( source->cur_size > source->max_size )
        source->max_size = source->cur_size;

      node->free_file_name = NULL;
      node->free_line_no   = 0;

      node->link = pnode[0];

      pnode[0] = node;
      table->nodes++;

      table->alloc_total += size;

      if ( table->alloc_current > table->alloc_max )
        table->alloc_max = table->alloc_current;

      if ( table->nodes * 3 < table->size  ||
           table->size  * 3 < table->nodes )
        ft_mem_table_resize( table );
    }
  }


  static void
  ft_mem_table_remove( FT_MemTable  table,
                       FT_Byte*     address,
                       FT_Long      delta )
  {
    if ( table )
    {
      FT_MemNode  *pnode, node;


      pnode = ft_mem_table_get_nodep( table, address );
      node  = *pnode;
      if ( node )
      {
        FT_MemSource  source;


        if ( node->size < 0 )
          ft_mem_debug_panic(
            "freeing memory block at %p more than once at (%s:%ld)\n"
            "block allocated at (%s:%ld) and released at (%s:%ld)",
            address,
            FT_FILENAME( _ft_debug_file ), _ft_debug_lineno,
            FT_FILENAME( node->source->file_name ), node->source->line_no,
            FT_FILENAME( node->free_file_name ), node->free_line_no );

        /* scramble the node's content for additional safety */
        FT_MEM_SET( address, 0xF3, node->size );

        if ( delta == 0 )
        {
          source = node->source;

          source->cur_blocks--;
          source->cur_size -= node->size;

          table->alloc_current -= node->size;
        }

        if ( table->keep_alive )
        {
          /* we simply invert the node's size to indicate that the node */
          /* was freed.                                                 */
          node->size           = -node->size;
          node->free_file_name = _ft_debug_file;
          node->free_line_no   = _ft_debug_lineno;
        }
        else
        {
          table->nodes--;

          *pnode = node->link;

          node->size   = 0;
          node->source = NULL;

          ft_mem_table_free( table, node );

          if ( table->nodes * 3 < table->size  ||
               table->size  * 3 < table->nodes )
            ft_mem_table_resize( table );
        }
      }
      else
        ft_mem_debug_panic(
          "trying to free unknown block at %p in (%s:%ld)\n",
          address,
          FT_FILENAME( _ft_debug_file ), _ft_debug_lineno );
    }
  }


  extern FT_Pointer
  ft_mem_debug_alloc( FT_Memory  memory,
                      FT_Long    size )
  {
    FT_MemTable  table = (FT_MemTable)memory->user;
    FT_Byte*     block;


    if ( size <= 0 )
      ft_mem_debug_panic( "negative block size allocation (%ld)", size );

    /* return NULL if the maximum number of allocations was reached */
    if ( table->bound_count                           &&
         table->alloc_count >= table->alloc_count_max )
      return NULL;

    /* return NULL if this allocation would overflow the maximum heap size */
    if ( table->bound_total                                             &&
         table->alloc_total_max - table->alloc_current > (FT_ULong)size )
      return NULL;

    block = (FT_Byte *)ft_mem_table_alloc( table, size );
    if ( block )
    {
      ft_mem_table_set( table, block, (FT_ULong)size, 0 );

      table->alloc_count++;
    }

    _ft_debug_file   = "<unknown>";
    _ft_debug_lineno = 0;

    return (FT_Pointer)block;
  }


  extern void
  ft_mem_debug_free( FT_Memory   memory,
                     FT_Pointer  block )
  {
    FT_MemTable  table = (FT_MemTable)memory->user;


    if ( block == NULL )
      ft_mem_debug_panic( "trying to free NULL in (%s:%ld)",
                          FT_FILENAME( _ft_debug_file ),
                          _ft_debug_lineno );

    ft_mem_table_remove( table, (FT_Byte*)block, 0 );

    if ( !table->keep_alive )
      ft_mem_table_free( table, block );

    table->alloc_count--;

    _ft_debug_file   = "<unknown>";
    _ft_debug_lineno = 0;
  }


  extern FT_Pointer
  ft_mem_debug_realloc( FT_Memory   memory,
                        FT_Long     cur_size,
                        FT_Long     new_size,
                        FT_Pointer  block )
  {
    FT_MemTable  table = (FT_MemTable)memory->user;
    FT_MemNode   node, *pnode;
    FT_Pointer   new_block;
    FT_Long      delta;

    const char*  file_name = FT_FILENAME( _ft_debug_file );
    FT_Long      line_no   = _ft_debug_lineno;


    /* unlikely, but possible */
    if ( new_size == cur_size )
      return block;

    /* the following is valid according to ANSI C */
#if 0
    if ( block == NULL || cur_size == 0 )
      ft_mem_debug_panic( "trying to reallocate NULL in (%s:%ld)",
                          file_name, line_no );
#endif

    /* while the following is allowed in ANSI C also, we abort since */
    /* such case should be handled by FreeType.                      */
    if ( new_size <= 0 )
      ft_mem_debug_panic(
        "trying to reallocate %p to size 0 (current is %ld) in (%s:%ld)",
        block, cur_size, file_name, line_no );

    /* check `cur_size' value */
    pnode = ft_mem_table_get_nodep( table, (FT_Byte*)block );
    node  = *pnode;
    if ( !node )
      ft_mem_debug_panic(
        "trying to reallocate unknown block at %p in (%s:%ld)",
        block, file_name, line_no );

    if ( node->size <= 0 )
      ft_mem_debug_panic(
        "trying to reallocate freed block at %p in (%s:%ld)",
        block, file_name, line_no );

    if ( node->size != cur_size )
      ft_mem_debug_panic( "invalid ft_realloc request for %p. cur_size is "
                          "%ld instead of %ld in (%s:%ld)",
                          block, cur_size, node->size, file_name, line_no );

    /* return NULL if the maximum number of allocations was reached */
    if ( table->bound_count                           &&
         table->alloc_count >= table->alloc_count_max )
      return NULL;

    delta = (FT_Long)( new_size - cur_size );

    /* return NULL if this allocation would overflow the maximum heap size */
    if ( delta > 0                                                       &&
         table->bound_total                                              &&
         table->alloc_current + (FT_ULong)delta > table->alloc_total_max )
      return NULL;

    new_block = (FT_Byte *)ft_mem_table_alloc( table, new_size );
    if ( new_block == NULL )
      return NULL;

    ft_mem_table_set( table, (FT_Byte*)new_block, new_size, delta );

    ft_memcpy( new_block, block, cur_size < new_size ? cur_size : new_size );

    ft_mem_table_remove( table, (FT_Byte*)block, delta );

    _ft_debug_file   = "<unknown>";
    _ft_debug_lineno = 0;

    if ( !table->keep_alive )
      ft_mem_table_free( table, block );

    return new_block;
  }


  extern FT_Int
  ft_mem_debug_init( FT_Memory  memory )
  {
    FT_MemTable  table;
    FT_Int       result = 0;


    if ( getenv( "FT2_DEBUG_MEMORY" ) )
    {
      table = ft_mem_table_new( memory );
      if ( table )
      {
        const char*  p;


        memory->user    = table;
        memory->alloc   = ft_mem_debug_alloc;
        memory->realloc = ft_mem_debug_realloc;
        memory->free    = ft_mem_debug_free;

        p = getenv( "FT2_ALLOC_TOTAL_MAX" );
        if ( p != NULL )
        {
          FT_Long   total_max = ft_atol( p );


          if ( total_max > 0 )
          {
            table->bound_total     = 1;
            table->alloc_total_max = (FT_ULong)total_max;
          }
        }

        p = getenv( "FT2_ALLOC_COUNT_MAX" );
        if ( p != NULL )
        {
          FT_Long  total_count = ft_atol( p );


          if ( total_count > 0 )
          {
            table->bound_count     = 1;
            table->alloc_count_max = (FT_ULong)total_count;
          }
        }

        p = getenv( "FT2_KEEP_ALIVE" );
        if ( p != NULL )
        {
          FT_Long  keep_alive = ft_atol( p );


          if ( keep_alive > 0 )
            table->keep_alive = 1;
        }

        result = 1;
      }
    }
    return result;
  }


  extern void
  ft_mem_debug_done( FT_Memory  memory )
  {
    FT_MemTable  table = (FT_MemTable)memory->user;


    if ( table )
    {
      memory->free    = table->free;
      memory->realloc = table->realloc;
      memory->alloc   = table->alloc;

      ft_mem_table_destroy( table );
      memory->user = NULL;
    }
  }



  static int
  ft_mem_source_compare( const void*  p1,
                         const void*  p2 )
  {
    FT_MemSource  s1 = *(FT_MemSource*)p1;
    FT_MemSource  s2 = *(FT_MemSource*)p2;


    if ( s2->max_size > s1->max_size )
      return 1;
    else if ( s2->max_size < s1->max_size )
      return -1;
    else
      return 0;
  }


  extern void
  FT_DumpMemory( FT_Memory  memory )
  {
    FT_MemTable  table = (FT_MemTable)memory->user;


    if ( table )
    {
      FT_MemSource*  bucket = table->sources;
      FT_MemSource*  limit  = bucket + FT_MEM_SOURCE_BUCKETS;
      FT_MemSource*  sources;
      FT_UInt        nn, count;
      const char*    fmt;


      count = 0;
      for ( ; bucket < limit; bucket++ )
      {
        FT_MemSource  source = *bucket;


        for ( ; source; source = source->link )
          count++;
      }

      sources = (FT_MemSource*)ft_mem_table_alloc(
                                 table, sizeof ( *sources ) * count );

      count = 0;
      for ( bucket = table->sources; bucket < limit; bucket++ )
      {
        FT_MemSource  source = *bucket;


        for ( ; source; source = source->link )
          sources[count++] = source;
      }

      ft_qsort( sources, count, sizeof ( *sources ), ft_mem_source_compare );

      printf( "FreeType Memory Dump: "
              "current=%ld max=%ld total=%ld count=%ld\n",
              table->alloc_current, table->alloc_max,
              table->alloc_total, table->alloc_count );
      printf( " block  block    sizes    sizes    sizes   source\n" );
      printf( " count   high      sum  highsum      max   location\n" );
      printf( "-------------------------------------------------\n" );

      fmt = "%6ld %6ld %8ld %8ld %8ld %s:%d\n";

      for ( nn = 0; nn < count; nn++ )
      {
        FT_MemSource  source = sources[nn];


        printf( fmt,
                source->cur_blocks, source->max_blocks,
                source->cur_size, source->max_size, source->cur_max,
                FT_FILENAME( source->file_name ),
                source->line_no );
      }
      printf( "------------------------------------------------\n" );

      ft_mem_table_free( table, sources );
    }
  }

#else  /* !FT_DEBUG_MEMORY */

  /* ANSI C doesn't like empty source files */
  typedef int  _debug_mem_dummy;

#endif /* !FT_DEBUG_MEMORY */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftgloadr.c                                                             */
/*                                                                         */
/*    The FreeType glyph loader (body).                                    */
/*                                                                         */
/*  Copyright 2002-2006, 2010, 2013 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg                       */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "ft2build.h"
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_GLYPH_LOADER_H
#include FT_INTERNAL_MEMORY_H
#include FT_INTERNAL_OBJECTS_H

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gloader


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                                                               *****/
  /*****                    G L Y P H   L O A D E R                    *****/
  /*****                                                               *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* The glyph loader is a simple object which is used to load a set of    */
  /* glyphs easily.  It is critical for the correct loading of composites. */
  /*                                                                       */
  /* Ideally, one can see it as a stack of abstract `glyph' objects.       */
  /*                                                                       */
  /*   loader.base     Is really the bottom of the stack.  It describes a  */
  /*                   single glyph image made of the juxtaposition of     */
  /*                   several glyphs (those `in the stack').              */
  /*                                                                       */
  /*   loader.current  Describes the top of the stack, on which a new      */
  /*                   glyph can be loaded.                                */
  /*                                                                       */
  /*   Rewind          Clears the stack.                                   */
  /*   Prepare         Set up `loader.current' for addition of a new glyph */
  /*                   image.                                              */
  /*   Add             Add the `current' glyph image to the `base' one,    */
  /*                   and prepare for another one.                        */
  /*                                                                       */
  /* The glyph loader is now a base object.  Each driver used to           */
  /* re-implement it in one way or the other, which wasted code and        */
  /* energy.                                                               */
  /*                                                                       */
  /*************************************************************************/


  /* create a new glyph loader */
  FT_BASE_DEF( FT_Error )
  FT_GlyphLoader_New( FT_Memory        memory,
                      FT_GlyphLoader  *aloader )
  {
    FT_GlyphLoader  loader = NULL;
    FT_Error        error;


    if ( !FT_NEW( loader ) )
    {
      loader->memory = memory;
      *aloader       = loader;
    }
    return error;
  }


  /* rewind the glyph loader - reset counters to 0 */
  FT_BASE_DEF( void )
  FT_GlyphLoader_Rewind( FT_GlyphLoader  loader )
  {
    FT_GlyphLoad  base    = &loader->base;
    FT_GlyphLoad  current = &loader->current;


    base->outline.n_points   = 0;
    base->outline.n_contours = 0;
    base->num_subglyphs      = 0;

    *current = *base;
  }


  /* reset the glyph loader, frees all allocated tables */
  /* and starts from zero                               */
  FT_BASE_DEF( void )
  FT_GlyphLoader_Reset( FT_GlyphLoader  loader )
  {
    FT_Memory memory = loader->memory;


    FT_FREE( loader->base.outline.points );
    FT_FREE( loader->base.outline.tags );
    FT_FREE( loader->base.outline.contours );
    FT_FREE( loader->base.extra_points );
    FT_FREE( loader->base.subglyphs );

    loader->base.extra_points2 = NULL;

    loader->max_points    = 0;
    loader->max_contours  = 0;
    loader->max_subglyphs = 0;

    FT_GlyphLoader_Rewind( loader );
  }


  /* delete a glyph loader */
  FT_BASE_DEF( void )
  FT_GlyphLoader_Done( FT_GlyphLoader  loader )
  {
    if ( loader )
    {
      FT_Memory memory = loader->memory;


      FT_GlyphLoader_Reset( loader );
      FT_FREE( loader );
    }
  }


  /* re-adjust the `current' outline fields */
  static void
  FT_GlyphLoader_Adjust_Points( FT_GlyphLoader  loader )
  {
    FT_Outline*  base    = &loader->base.outline;
    FT_Outline*  current = &loader->current.outline;


    current->points   = base->points   + base->n_points;
    current->tags     = base->tags     + base->n_points;
    current->contours = base->contours + base->n_contours;

    /* handle extra points table - if any */
    if ( loader->use_extra )
    {
      loader->current.extra_points  = loader->base.extra_points +
                                      base->n_points;

      loader->current.extra_points2 = loader->base.extra_points2 +
                                      base->n_points;
    }
  }


  FT_BASE_DEF( FT_Error )
  FT_GlyphLoader_CreateExtra( FT_GlyphLoader  loader )
  {
    FT_Error   error;
    FT_Memory  memory = loader->memory;


    if ( !FT_NEW_ARRAY( loader->base.extra_points, 2 * loader->max_points ) )
    {
      loader->use_extra          = 1;
      loader->base.extra_points2 = loader->base.extra_points +
                                   loader->max_points;

      FT_GlyphLoader_Adjust_Points( loader );
    }
    return error;
  }


  /* re-adjust the `current' subglyphs field */
  static void
  FT_GlyphLoader_Adjust_Subglyphs( FT_GlyphLoader  loader )
  {
    FT_GlyphLoad  base    = &loader->base;
    FT_GlyphLoad  current = &loader->current;


    current->subglyphs = base->subglyphs + base->num_subglyphs;
  }


  /* Ensure that we can add `n_points' and `n_contours' to our glyph.      */
  /* This function reallocates its outline tables if necessary.  Note that */
  /* it DOESN'T change the number of points within the loader!             */
  /*                                                                       */
  FT_BASE_DEF( FT_Error )
  FT_GlyphLoader_CheckPoints( FT_GlyphLoader  loader,
                              FT_UInt         n_points,
                              FT_UInt         n_contours )
  {
    FT_Memory    memory  = loader->memory;
    FT_Error     error   = FT_Err_Ok;
    FT_Outline*  base    = &loader->base.outline;
    FT_Outline*  current = &loader->current.outline;
    FT_Bool      adjust  = 0;

    FT_UInt      new_max, old_max;


    /* check points & tags */
    new_max = base->n_points + current->n_points + n_points;
    old_max = loader->max_points;

    if ( new_max > old_max )
    {
      new_max = FT_PAD_CEIL( new_max, 8 );

      if ( new_max > FT_OUTLINE_POINTS_MAX )
        return FT_THROW( Array_Too_Large );

      if ( FT_RENEW_ARRAY( base->points, old_max, new_max ) ||
           FT_RENEW_ARRAY( base->tags,   old_max, new_max ) )
        goto Exit;

      if ( loader->use_extra )
      {
        if ( FT_RENEW_ARRAY( loader->base.extra_points,
                             old_max * 2, new_max * 2 ) )
          goto Exit;

        FT_ARRAY_MOVE( loader->base.extra_points + new_max,
                       loader->base.extra_points + old_max,
                       old_max );

        loader->base.extra_points2 = loader->base.extra_points + new_max;
      }

      adjust = 1;
      loader->max_points = new_max;
    }

    /* check contours */
    old_max = loader->max_contours;
    new_max = base->n_contours + current->n_contours +
              n_contours;
    if ( new_max > old_max )
    {
      new_max = FT_PAD_CEIL( new_max, 4 );

      if ( new_max > FT_OUTLINE_CONTOURS_MAX )
        return FT_THROW( Array_Too_Large );

      if ( FT_RENEW_ARRAY( base->contours, old_max, new_max ) )
        goto Exit;

      adjust = 1;
      loader->max_contours = new_max;
    }

    if ( adjust )
      FT_GlyphLoader_Adjust_Points( loader );

  Exit:
    if ( error )
      FT_GlyphLoader_Reset( loader );

    return error;
  }


  /* Ensure that we can add `n_subglyphs' to our glyph. this function */
  /* reallocates its subglyphs table if necessary.  Note that it DOES */
  /* NOT change the number of subglyphs within the loader!            */
  /*                                                                  */
  FT_BASE_DEF( FT_Error )
  FT_GlyphLoader_CheckSubGlyphs( FT_GlyphLoader  loader,
                                 FT_UInt         n_subs )
  {
    FT_Memory     memory = loader->memory;
    FT_Error      error  = FT_Err_Ok;
    FT_UInt       new_max, old_max;

    FT_GlyphLoad  base    = &loader->base;
    FT_GlyphLoad  current = &loader->current;


    new_max = base->num_subglyphs + current->num_subglyphs + n_subs;
    old_max = loader->max_subglyphs;
    if ( new_max > old_max )
    {
      new_max = FT_PAD_CEIL( new_max, 2 );
      if ( FT_RENEW_ARRAY( base->subglyphs, old_max, new_max ) )
        goto Exit;

      loader->max_subglyphs = new_max;

      FT_GlyphLoader_Adjust_Subglyphs( loader );
    }

  Exit:
    return error;
  }


  /* prepare loader for the addition of a new glyph on top of the base one */
  FT_BASE_DEF( void )
  FT_GlyphLoader_Prepare( FT_GlyphLoader  loader )
  {
    FT_GlyphLoad  current = &loader->current;


    current->outline.n_points   = 0;
    current->outline.n_contours = 0;
    current->num_subglyphs      = 0;

    FT_GlyphLoader_Adjust_Points   ( loader );
    FT_GlyphLoader_Adjust_Subglyphs( loader );
  }


  /* add current glyph to the base image -- and prepare for another */
  FT_BASE_DEF( void )
  FT_GlyphLoader_Add( FT_GlyphLoader  loader )
  {
    FT_GlyphLoad  base;
    FT_GlyphLoad  current;

    FT_UInt       n_curr_contours;
    FT_UInt       n_base_points;
    FT_UInt       n;


    if ( !loader )
      return;

    base    = &loader->base;
    current = &loader->current;

    n_curr_contours = current->outline.n_contours;
    n_base_points   = base->outline.n_points;

    base->outline.n_points =
      (short)( base->outline.n_points + current->outline.n_points );
    base->outline.n_contours =
      (short)( base->outline.n_contours + current->outline.n_contours );

    base->num_subglyphs += current->num_subglyphs;

    /* adjust contours count in newest outline */
    for ( n = 0; n < n_curr_contours; n++ )
      current->outline.contours[n] =
        (short)( current->outline.contours[n] + n_base_points );

    /* prepare for another new glyph image */
    FT_GlyphLoader_Prepare( loader );
  }


  FT_BASE_DEF( FT_Error )
  FT_GlyphLoader_CopyPoints( FT_GlyphLoader  target,
                             FT_GlyphLoader  source )
  {
    FT_Error  error;
    FT_UInt   num_points   = source->base.outline.n_points;
    FT_UInt   num_contours = source->base.outline.n_contours;


    error = FT_GlyphLoader_CheckPoints( target, num_points, num_contours );
    if ( !error )
    {
      FT_Outline*  out = &target->base.outline;
      FT_Outline*  in  = &source->base.outline;


      FT_ARRAY_COPY( out->points, in->points,
                     num_points );
      FT_ARRAY_COPY( out->tags, in->tags,
                     num_points );
      FT_ARRAY_COPY( out->contours, in->contours,
                     num_contours );

      /* do we need to copy the extra points? */
      if ( target->use_extra && source->use_extra )
      {
        FT_ARRAY_COPY( target->base.extra_points, source->base.extra_points,
                       num_points );
        FT_ARRAY_COPY( target->base.extra_points2, source->base.extra_points2,
                       num_points );
      }

      out->n_points   = (short)num_points;
      out->n_contours = (short)num_contours;

      FT_GlyphLoader_Adjust_Points( target );
    }

    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftobjs.c                                                               */
/*                                                                         */
/*    The FreeType private base classes (body).                            */
/*                                                                         */
/*  Copyright 1996-2014 by                                                 */
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
#include FT_LIST_H
#include FT_OUTLINE_H
#include FT_INTERNAL_VALIDATE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_RFORK_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_SFNT_H    /* for SFNT_Load_Table_Func */
#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_TAGS_H
#include FT_TRUETYPE_IDS_H

#include FT_SERVICE_PROPERTIES_H
#include FT_SERVICE_SFNT_H
#include FT_SERVICE_POSTSCRIPT_NAME_H
#include FT_SERVICE_GLYPH_DICT_H
#include FT_SERVICE_TT_CMAP_H
#include FT_SERVICE_KERNING_H
#include FT_SERVICE_TRUETYPE_ENGINE_H

#ifdef FT_CONFIG_OPTION_MAC_FONTS
#include "ftbase.h"
#endif


#ifdef FT_DEBUG_LEVEL_TRACE

#include FT_BITMAP_H

#if defined( _MSC_VER )      /* Visual C++ (and Intel C++)   */
  /* We disable the warning `conversion from XXX to YYY,     */
  /* possible loss of data' in order to compile cleanly with */
  /* the maximum level of warnings: `md5.c' is non-FreeType  */
  /* code, and it gets used during development builds only.  */
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif /* _MSC_VER */

  /* it's easiest to include `md5.c' directly */
/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * (This is a heavily cut-down "BSD license".)
 *
 * This differs from Colin Plumb's older public domain implementation in that
 * no exactly 32-bit integer data type is required (any 32-bit or wider
 * unsigned integer data type will do), there's no compile-time endianness
 * configuration, and the function prototypes match OpenSSL's.  No code from
 * Colin Plumb's implementation has been reused; this comment merely compares
 * the properties of the two independent implementations.
 *
 * The primary goals of this implementation are portability and ease of use.
 * It is meant to be fast, but not as fast as possible.  Some known
 * optimizations are not included to reduce source code size and avoid
 * compile-time configuration.
 */

#ifndef HAVE_OPENSSL

#include <string.h>

#include "md5.h"

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, just like in Colin Plumb's
 * implementation.
 */
#define F(x, y, z)			((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)			((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)			(((x) ^ (y)) ^ (z))
#define H2(x, y, z)			((x) ^ ((y) ^ (z)))
#define I(x, y, z)			((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP(f, a, b, c, d, x, t, s) \
	(a) += f((b), (c), (d)) + (x) + (t); \
	(a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
	(a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned
 * memory accesses is just an optimization.  Nothing will break if it
 * doesn't work.
 */
#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define SET(n) \
	(*(MD5_u32plus *)&ptr[(n) * 4])
#define GET(n) \
	SET(n)
#else
#define SET(n) \
	(ctx->block[(n)] = \
	(MD5_u32plus)ptr[(n) * 4] | \
	((MD5_u32plus)ptr[(n) * 4 + 1] << 8) | \
	((MD5_u32plus)ptr[(n) * 4 + 2] << 16) | \
	((MD5_u32plus)ptr[(n) * 4 + 3] << 24))
#define GET(n) \
	(ctx->block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters.  There are no alignment requirements.
 */
static const void *body(MD5_CTX *ctx, const void *data, unsigned long size)
{
	const unsigned char *ptr;
	MD5_u32plus a, b, c, d;
	MD5_u32plus saved_a, saved_b, saved_c, saved_d;

	ptr = (const unsigned char *)data;

	a = ctx->a;
	b = ctx->b;
	c = ctx->c;
	d = ctx->d;

	do {
		saved_a = a;
		saved_b = b;
		saved_c = c;
		saved_d = d;

/* Round 1 */
		STEP(F, a, b, c, d, SET(0), 0xd76aa478, 7)
		STEP(F, d, a, b, c, SET(1), 0xe8c7b756, 12)
		STEP(F, c, d, a, b, SET(2), 0x242070db, 17)
		STEP(F, b, c, d, a, SET(3), 0xc1bdceee, 22)
		STEP(F, a, b, c, d, SET(4), 0xf57c0faf, 7)
		STEP(F, d, a, b, c, SET(5), 0x4787c62a, 12)
		STEP(F, c, d, a, b, SET(6), 0xa8304613, 17)
		STEP(F, b, c, d, a, SET(7), 0xfd469501, 22)
		STEP(F, a, b, c, d, SET(8), 0x698098d8, 7)
		STEP(F, d, a, b, c, SET(9), 0x8b44f7af, 12)
		STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17)
		STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
		STEP(F, a, b, c, d, SET(12), 0x6b901122, 7)
		STEP(F, d, a, b, c, SET(13), 0xfd987193, 12)
		STEP(F, c, d, a, b, SET(14), 0xa679438e, 17)
		STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)

/* Round 2 */
		STEP(G, a, b, c, d, GET(1), 0xf61e2562, 5)
		STEP(G, d, a, b, c, GET(6), 0xc040b340, 9)
		STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14)
		STEP(G, b, c, d, a, GET(0), 0xe9b6c7aa, 20)
		STEP(G, a, b, c, d, GET(5), 0xd62f105d, 5)
		STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
		STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14)
		STEP(G, b, c, d, a, GET(4), 0xe7d3fbc8, 20)
		STEP(G, a, b, c, d, GET(9), 0x21e1cde6, 5)
		STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9)
		STEP(G, c, d, a, b, GET(3), 0xf4d50d87, 14)
		STEP(G, b, c, d, a, GET(8), 0x455a14ed, 20)
		STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5)
		STEP(G, d, a, b, c, GET(2), 0xfcefa3f8, 9)
		STEP(G, c, d, a, b, GET(7), 0x676f02d9, 14)
		STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)

/* Round 3 */
		STEP(H, a, b, c, d, GET(5), 0xfffa3942, 4)
		STEP(H2, d, a, b, c, GET(8), 0x8771f681, 11)
		STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16)
		STEP(H2, b, c, d, a, GET(14), 0xfde5380c, 23)
		STEP(H, a, b, c, d, GET(1), 0xa4beea44, 4)
		STEP(H2, d, a, b, c, GET(4), 0x4bdecfa9, 11)
		STEP(H, c, d, a, b, GET(7), 0xf6bb4b60, 16)
		STEP(H2, b, c, d, a, GET(10), 0xbebfbc70, 23)
		STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4)
		STEP(H2, d, a, b, c, GET(0), 0xeaa127fa, 11)
		STEP(H, c, d, a, b, GET(3), 0xd4ef3085, 16)
		STEP(H2, b, c, d, a, GET(6), 0x04881d05, 23)
		STEP(H, a, b, c, d, GET(9), 0xd9d4d039, 4)
		STEP(H2, d, a, b, c, GET(12), 0xe6db99e5, 11)
		STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16)
		STEP(H2, b, c, d, a, GET(2), 0xc4ac5665, 23)

/* Round 4 */
		STEP(I, a, b, c, d, GET(0), 0xf4292244, 6)
		STEP(I, d, a, b, c, GET(7), 0x432aff97, 10)
		STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15)
		STEP(I, b, c, d, a, GET(5), 0xfc93a039, 21)
		STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6)
		STEP(I, d, a, b, c, GET(3), 0x8f0ccc92, 10)
		STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15)
		STEP(I, b, c, d, a, GET(1), 0x85845dd1, 21)
		STEP(I, a, b, c, d, GET(8), 0x6fa87e4f, 6)
		STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10)
		STEP(I, c, d, a, b, GET(6), 0xa3014314, 15)
		STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
		STEP(I, a, b, c, d, GET(4), 0xf7537e82, 6)
		STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10)
		STEP(I, c, d, a, b, GET(2), 0x2ad7d2bb, 15)
		STEP(I, b, c, d, a, GET(9), 0xeb86d391, 21)

		a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;

		ptr += 64;
	} while (size -= 64);

	ctx->a = a;
	ctx->b = b;
	ctx->c = c;
	ctx->d = d;

	return ptr;
}

void MD5_Init(MD5_CTX *ctx)
{
	ctx->a = 0x67452301;
	ctx->b = 0xefcdab89;
	ctx->c = 0x98badcfe;
	ctx->d = 0x10325476;

	ctx->lo = 0;
	ctx->hi = 0;
}

void MD5_Update(MD5_CTX *ctx, const void *data, unsigned long size)
{
	MD5_u32plus saved_lo;
	unsigned long used, available;

	saved_lo = ctx->lo;
	if ((ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
		ctx->hi++;
	ctx->hi += size >> 29;

	used = saved_lo & 0x3f;

	if (used) {
		available = 64 - used;

		if (size < available) {
			memcpy(&ctx->buffer[used], data, size);
			return;
		}

		memcpy(&ctx->buffer[used], data, available);
		data = (const unsigned char *)data + available;
		size -= available;
		body(ctx, ctx->buffer, 64);
	}

	if (size >= 64) {
		data = body(ctx, data, size & ~(unsigned long)0x3f);
		size &= 0x3f;
	}

	memcpy(ctx->buffer, data, size);
}

void MD5_Final(unsigned char *result, MD5_CTX *ctx)
{
	unsigned long used, available;

	used = ctx->lo & 0x3f;

	ctx->buffer[used++] = 0x80;

	available = 64 - used;

	if (available < 8) {
		memset(&ctx->buffer[used], 0, available);
		body(ctx, ctx->buffer, 64);
		used = 0;
		available = 64;
	}

	memset(&ctx->buffer[used], 0, available - 8);

	ctx->lo <<= 3;
	ctx->buffer[56] = ctx->lo;
	ctx->buffer[57] = ctx->lo >> 8;
	ctx->buffer[58] = ctx->lo >> 16;
	ctx->buffer[59] = ctx->lo >> 24;
	ctx->buffer[60] = ctx->hi;
	ctx->buffer[61] = ctx->hi >> 8;
	ctx->buffer[62] = ctx->hi >> 16;
	ctx->buffer[63] = ctx->hi >> 24;

	body(ctx, ctx->buffer, 64);

	result[0] = ctx->a;
	result[1] = ctx->a >> 8;
	result[2] = ctx->a >> 16;
	result[3] = ctx->a >> 24;
	result[4] = ctx->b;
	result[5] = ctx->b >> 8;
	result[6] = ctx->b >> 16;
	result[7] = ctx->b >> 24;
	result[8] = ctx->c;
	result[9] = ctx->c >> 8;
	result[10] = ctx->c >> 16;
	result[11] = ctx->c >> 24;
	result[12] = ctx->d;
	result[13] = ctx->d >> 8;
	result[14] = ctx->d >> 16;
	result[15] = ctx->d >> 24;

	memset(ctx, 0, sizeof(*ctx));
}

#endif

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

#endif /* FT_DEBUG_LEVEL_TRACE */


#define GRID_FIT_METRICS


  FT_BASE_DEF( FT_Pointer )
  ft_service_list_lookup( FT_ServiceDesc  service_descriptors,
                          const char*     service_id )
  {
    FT_Pointer      result = NULL;
    FT_ServiceDesc  desc   = service_descriptors;


    if ( desc && service_id )
    {
      for ( ; desc->serv_id != NULL; desc++ )
      {
        if ( ft_strcmp( desc->serv_id, service_id ) == 0 )
        {
          result = (FT_Pointer)desc->serv_data;
          break;
        }
      }
    }

    return result;
  }


  FT_BASE_DEF( void )
  ft_validator_init( FT_Validator        valid,
                     const FT_Byte*      base,
                     const FT_Byte*      limit,
                     FT_ValidationLevel  level )
  {
    valid->base  = base;
    valid->limit = limit;
    valid->level = level;
    valid->error = FT_Err_Ok;
  }


  FT_BASE_DEF( FT_Int )
  ft_validator_run( FT_Validator  valid )
  {
    /* This function doesn't work!  None should call it. */
    FT_UNUSED( valid );

    return -1;
  }


  FT_BASE_DEF( void )
  ft_validator_error( FT_Validator  valid,
                      FT_Error      error )
  {
    /* since the cast below also disables the compiler's */
    /* type check, we introduce a dummy variable, which  */
    /* will be optimized away                            */
    volatile ft_jmp_buf* jump_buffer = &valid->jump_buffer;


    valid->error = error;

    /* throw away volatileness; use `jump_buffer' or the  */
    /* compiler may warn about an unused local variable   */
    ft_longjmp( *(ft_jmp_buf*) jump_buffer, 1 );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                           S T R E A M                           ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  /* create a new input stream from an FT_Open_Args structure */
  /*                                                          */
  FT_BASE_DEF( FT_Error )
  FT_Stream_New( FT_Library           library,
                 const FT_Open_Args*  args,
                 FT_Stream           *astream )
  {
    FT_Error   error;
    FT_Memory  memory;
    FT_Stream  stream = NULL;


    *astream = 0;

    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( !args )
      return FT_THROW( Invalid_Argument );

    memory = library->memory;

    if ( FT_NEW( stream ) )
      goto Exit;

    stream->memory = memory;

    if ( args->flags & FT_OPEN_MEMORY )
    {
      /* create a memory-based stream */
      FT_Stream_OpenMemory( stream,
                            (const FT_Byte*)args->memory_base,
                            args->memory_size );
    }

#ifndef FT_CONFIG_OPTION_DISABLE_STREAM_SUPPORT

    else if ( args->flags & FT_OPEN_PATHNAME )
    {
      /* create a normal system stream */
      error = FT_Stream_Open( stream, args->pathname );
      stream->pathname.pointer = args->pathname;
    }
    else if ( ( args->flags & FT_OPEN_STREAM ) && args->stream )
    {
      /* use an existing, user-provided stream */

      /* in this case, we do not need to allocate a new stream object */
      /* since the caller is responsible for closing it himself       */
      FT_FREE( stream );
      stream = args->stream;
    }

#endif

    else
      error = FT_THROW( Invalid_Argument );

    if ( error )
      FT_FREE( stream );
    else
      stream->memory = memory;  /* just to be certain */

    *astream = stream;

  Exit:
    return error;
  }


  FT_BASE_DEF( void )
  FT_Stream_Free( FT_Stream  stream,
                  FT_Int     external )
  {
    if ( stream )
    {
      FT_Memory  memory = stream->memory;


      FT_Stream_Close( stream );

      if ( !external )
        FT_FREE( stream );
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_objs


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****               FACE, SIZE & GLYPH SLOT OBJECTS                   ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  static FT_Error
  ft_glyphslot_init( FT_GlyphSlot  slot )
  {
    FT_Driver         driver   = slot->face->driver;
    FT_Driver_Class   clazz    = driver->clazz;
    FT_Memory         memory   = driver->root.memory;
    FT_Error          error    = FT_Err_Ok;
    FT_Slot_Internal  internal = NULL;


    slot->library = driver->root.library;

    if ( FT_NEW( internal ) )
      goto Exit;

    slot->internal = internal;

    if ( FT_DRIVER_USES_OUTLINES( driver ) )
      error = FT_GlyphLoader_New( memory, &internal->loader );

    if ( !error && clazz->init_slot )
      error = clazz->init_slot( slot );

  Exit:
    return error;
  }


  FT_BASE_DEF( void )
  ft_glyphslot_free_bitmap( FT_GlyphSlot  slot )
  {
    if ( slot->internal && ( slot->internal->flags & FT_GLYPH_OWN_BITMAP ) )
    {
      FT_Memory  memory = FT_FACE_MEMORY( slot->face );


      FT_FREE( slot->bitmap.buffer );
      slot->internal->flags &= ~FT_GLYPH_OWN_BITMAP;
    }
    else
    {
      /* assume that the bitmap buffer was stolen or not */
      /* allocated from the heap                         */
      slot->bitmap.buffer = NULL;
    }
  }


  FT_BASE_DEF( void )
  ft_glyphslot_set_bitmap( FT_GlyphSlot  slot,
                           FT_Byte*      buffer )
  {
    ft_glyphslot_free_bitmap( slot );

    slot->bitmap.buffer = buffer;

    FT_ASSERT( (slot->internal->flags & FT_GLYPH_OWN_BITMAP) == 0 );
  }


  FT_BASE_DEF( FT_Error )
  ft_glyphslot_alloc_bitmap( FT_GlyphSlot  slot,
                             FT_ULong      size )
  {
    FT_Memory  memory = FT_FACE_MEMORY( slot->face );
    FT_Error   error;


    if ( slot->internal->flags & FT_GLYPH_OWN_BITMAP )
      FT_FREE( slot->bitmap.buffer );
    else
      slot->internal->flags |= FT_GLYPH_OWN_BITMAP;

    (void)FT_ALLOC( slot->bitmap.buffer, size );
    return error;
  }


  static void
  ft_glyphslot_clear( FT_GlyphSlot  slot )
  {
    /* free bitmap if needed */
    ft_glyphslot_free_bitmap( slot );

    /* clear all public fields in the glyph slot */
    FT_ZERO( &slot->metrics );
    FT_ZERO( &slot->outline );

    slot->bitmap.width      = 0;
    slot->bitmap.rows       = 0;
    slot->bitmap.pitch      = 0;
    slot->bitmap.pixel_mode = 0;
    /* `slot->bitmap.buffer' has been handled by ft_glyphslot_free_bitmap */

    slot->bitmap_left   = 0;
    slot->bitmap_top    = 0;
    slot->num_subglyphs = 0;
    slot->subglyphs     = 0;
    slot->control_data  = 0;
    slot->control_len   = 0;
    slot->other         = 0;
    slot->format        = FT_GLYPH_FORMAT_NONE;

    slot->linearHoriAdvance = 0;
    slot->linearVertAdvance = 0;
    slot->lsb_delta         = 0;
    slot->rsb_delta         = 0;
  }


  static void
  ft_glyphslot_done( FT_GlyphSlot  slot )
  {
    FT_Driver        driver = slot->face->driver;
    FT_Driver_Class  clazz  = driver->clazz;
    FT_Memory        memory = driver->root.memory;


    if ( clazz->done_slot )
      clazz->done_slot( slot );

    /* free bitmap buffer if needed */
    ft_glyphslot_free_bitmap( slot );

    /* slot->internal might be NULL in out-of-memory situations */
    if ( slot->internal )
    {
      /* free glyph loader */
      if ( FT_DRIVER_USES_OUTLINES( driver ) )
      {
        FT_GlyphLoader_Done( slot->internal->loader );
        slot->internal->loader = 0;
      }

      FT_FREE( slot->internal );
    }
  }


  /* documentation is in ftobjs.h */

  FT_BASE_DEF( FT_Error )
  FT_New_GlyphSlot( FT_Face        face,
                    FT_GlyphSlot  *aslot )
  {
    FT_Error         error;
    FT_Driver        driver;
    FT_Driver_Class  clazz;
    FT_Memory        memory;
    FT_GlyphSlot     slot = NULL;


    if ( !face || !face->driver )
      return FT_THROW( Invalid_Argument );

    driver = face->driver;
    clazz  = driver->clazz;
    memory = driver->root.memory;

    FT_TRACE4(( "FT_New_GlyphSlot: Creating new slot object\n" ));
    if ( !FT_ALLOC( slot, clazz->slot_object_size ) )
    {
      slot->face = face;

      error = ft_glyphslot_init( slot );
      if ( error )
      {
        ft_glyphslot_done( slot );
        FT_FREE( slot );
        goto Exit;
      }

      slot->next  = face->glyph;
      face->glyph = slot;

      if ( aslot )
        *aslot = slot;
    }
    else if ( aslot )
      *aslot = 0;


  Exit:
    FT_TRACE4(( "FT_New_GlyphSlot: Return %d\n", error ));
    return error;
  }


  /* documentation is in ftobjs.h */

  FT_BASE_DEF( void )
  FT_Done_GlyphSlot( FT_GlyphSlot  slot )
  {
    if ( slot )
    {
      FT_Driver     driver = slot->face->driver;
      FT_Memory     memory = driver->root.memory;
      FT_GlyphSlot  prev;
      FT_GlyphSlot  cur;


      /* Remove slot from its parent face's list */
      prev = NULL;
      cur  = slot->face->glyph;

      while ( cur )
      {
        if ( cur == slot )
        {
          if ( !prev )
            slot->face->glyph = cur->next;
          else
            prev->next = cur->next;

          /* finalize client-specific data */
          if ( slot->generic.finalizer )
            slot->generic.finalizer( slot );

          ft_glyphslot_done( slot );
          FT_FREE( slot );
          break;
        }
        prev = cur;
        cur  = cur->next;
      }
    }
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( void )
  FT_Set_Transform( FT_Face     face,
                    FT_Matrix*  matrix,
                    FT_Vector*  delta )
  {
    FT_Face_Internal  internal;


    if ( !face )
      return;

    internal = face->internal;

    internal->transform_flags = 0;

    if ( !matrix )
    {
      internal->transform_matrix.xx = 0x10000L;
      internal->transform_matrix.xy = 0;
      internal->transform_matrix.yx = 0;
      internal->transform_matrix.yy = 0x10000L;
      matrix = &internal->transform_matrix;
    }
    else
      internal->transform_matrix = *matrix;

    /* set transform_flags bit flag 0 if `matrix' isn't the identity */
    if ( ( matrix->xy | matrix->yx ) ||
         matrix->xx != 0x10000L      ||
         matrix->yy != 0x10000L      )
      internal->transform_flags |= 1;

    if ( !delta )
    {
      internal->transform_delta.x = 0;
      internal->transform_delta.y = 0;
      delta = &internal->transform_delta;
    }
    else
      internal->transform_delta = *delta;

    /* set transform_flags bit flag 1 if `delta' isn't the null vector */
    if ( delta->x | delta->y )
      internal->transform_flags |= 2;
  }


  static FT_Renderer
  ft_lookup_glyph_renderer( FT_GlyphSlot  slot );


#ifdef GRID_FIT_METRICS
  static void
  ft_glyphslot_grid_fit_metrics( FT_GlyphSlot  slot,
                                 FT_Bool       vertical )
  {
    FT_Glyph_Metrics*  metrics = &slot->metrics;
    FT_Pos             right, bottom;


    if ( vertical )
    {
      metrics->horiBearingX = FT_PIX_FLOOR( metrics->horiBearingX );
      metrics->horiBearingY = FT_PIX_CEIL ( metrics->horiBearingY );

      right  = FT_PIX_CEIL( metrics->vertBearingX + metrics->width );
      bottom = FT_PIX_CEIL( metrics->vertBearingY + metrics->height );

      metrics->vertBearingX = FT_PIX_FLOOR( metrics->vertBearingX );
      metrics->vertBearingY = FT_PIX_FLOOR( metrics->vertBearingY );

      metrics->width  = right - metrics->vertBearingX;
      metrics->height = bottom - metrics->vertBearingY;
    }
    else
    {
      metrics->vertBearingX = FT_PIX_FLOOR( metrics->vertBearingX );
      metrics->vertBearingY = FT_PIX_FLOOR( metrics->vertBearingY );

      right  = FT_PIX_CEIL ( metrics->horiBearingX + metrics->width );
      bottom = FT_PIX_FLOOR( metrics->horiBearingY - metrics->height );

      metrics->horiBearingX = FT_PIX_FLOOR( metrics->horiBearingX );
      metrics->horiBearingY = FT_PIX_CEIL ( metrics->horiBearingY );

      metrics->width  = right - metrics->horiBearingX;
      metrics->height = metrics->horiBearingY - bottom;
    }

    metrics->horiAdvance = FT_PIX_ROUND( metrics->horiAdvance );
    metrics->vertAdvance = FT_PIX_ROUND( metrics->vertAdvance );
  }
#endif /* GRID_FIT_METRICS */


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Load_Glyph( FT_Face   face,
                 FT_UInt   glyph_index,
                 FT_Int32  load_flags )
  {
    FT_Error      error;
    FT_Driver     driver;
    FT_GlyphSlot  slot;
    FT_Library    library;
    FT_Bool       autohint = FALSE;
    FT_Module     hinter;
    TT_Face       ttface = (TT_Face)face;


    if ( !face || !face->size || !face->glyph )
      return FT_THROW( Invalid_Face_Handle );

    /* The validity test for `glyph_index' is performed by the */
    /* font drivers.                                           */

    slot = face->glyph;
    ft_glyphslot_clear( slot );

    driver  = face->driver;
    library = driver->root.library;
    hinter  = library->auto_hinter;

    /* resolve load flags dependencies */

    if ( load_flags & FT_LOAD_NO_RECURSE )
      load_flags |= FT_LOAD_NO_SCALE         |
                    FT_LOAD_IGNORE_TRANSFORM;

    if ( load_flags & FT_LOAD_NO_SCALE )
    {
      load_flags |= FT_LOAD_NO_HINTING |
                    FT_LOAD_NO_BITMAP;

      load_flags &= ~FT_LOAD_RENDER;
    }

    /*
     * Determine whether we need to auto-hint or not.
     * The general rules are:
     *
     * - Do only auto-hinting if we have a hinter module, a scalable font
     *   format dealing with outlines, and no transforms except simple
     *   slants and/or rotations by integer multiples of 90 degrees.
     *
     * - Then, auto-hint if FT_LOAD_FORCE_AUTOHINT is set or if we don't
     *   have a native font hinter.
     *
     * - Otherwise, auto-hint for LIGHT hinting mode or if there isn't
     *   any hinting bytecode in the TrueType/OpenType font.
     *
     * - Exception: The font is `tricky' and requires the native hinter to
     *   load properly.
     */

    if ( hinter                                           &&
         !( load_flags & FT_LOAD_NO_HINTING )             &&
         !( load_flags & FT_LOAD_NO_AUTOHINT )            &&
         FT_DRIVER_IS_SCALABLE( driver )                  &&
         FT_DRIVER_USES_OUTLINES( driver )                &&
         !FT_IS_TRICKY( face )                            &&
         ( ( load_flags & FT_LOAD_IGNORE_TRANSFORM )    ||
           ( face->internal->transform_matrix.yx == 0 &&
             face->internal->transform_matrix.xx != 0 ) ||
           ( face->internal->transform_matrix.xx == 0 &&
             face->internal->transform_matrix.yx != 0 ) ) )
    {
      if ( ( load_flags & FT_LOAD_FORCE_AUTOHINT ) ||
           !FT_DRIVER_HAS_HINTER( driver )         )
        autohint = TRUE;
      else
      {
        FT_Render_Mode  mode = FT_LOAD_TARGET_MODE( load_flags );


        /* the check for `num_locations' assures that we actually    */
        /* test for instructions in a TTF and not in a CFF-based OTF */
        /*                                                           */
        /* since `maxSizeOfInstructions' might be unreliable, we     */
        /* check the size of the `fpgm' and `prep' tables, too --    */
        /* the assumption is that there don't exist real TTFs where  */
        /* both `fpgm' and `prep' tables are missing                 */
        if ( mode == FT_RENDER_MODE_LIGHT                       ||
             face->internal->ignore_unpatented_hinter           ||
             ( FT_IS_SFNT( face )                             &&
               ttface->num_locations                          &&
               ttface->max_profile.maxSizeOfInstructions == 0 &&
               ttface->font_program_size == 0                 &&
               ttface->cvt_program_size == 0                  ) )
          autohint = TRUE;
      }
    }

    if ( autohint )
    {
      FT_AutoHinter_Interface  hinting;


      /* try to load embedded bitmaps first if available            */
      /*                                                            */
      /* XXX: This is really a temporary hack that should disappear */
      /*      promptly with FreeType 2.1!                           */
      /*                                                            */
      if ( FT_HAS_FIXED_SIZES( face )             &&
          ( load_flags & FT_LOAD_NO_BITMAP ) == 0 )
      {
        error = driver->clazz->load_glyph( slot, face->size,
                                           glyph_index,
                                           load_flags | FT_LOAD_SBITS_ONLY );

        if ( !error && slot->format == FT_GLYPH_FORMAT_BITMAP )
          goto Load_Ok;
      }

      {
        FT_Face_Internal  internal        = face->internal;
        FT_Int            transform_flags = internal->transform_flags;


        /* since the auto-hinter calls FT_Load_Glyph by itself, */
        /* make sure that glyphs aren't transformed             */
        internal->transform_flags = 0;

        /* load auto-hinted outline */
        hinting = (FT_AutoHinter_Interface)hinter->clazz->module_interface;

        error   = hinting->load_glyph( (FT_AutoHinter)hinter,
                                       slot, face->size,
                                       glyph_index, load_flags );

        internal->transform_flags = transform_flags;
      }
    }
    else
    {
      error = driver->clazz->load_glyph( slot,
                                         face->size,
                                         glyph_index,
                                         load_flags );
      if ( error )
        goto Exit;

      if ( slot->format == FT_GLYPH_FORMAT_OUTLINE )
      {
        /* check that the loaded outline is correct */
        error = FT_Outline_Check( &slot->outline );
        if ( error )
          goto Exit;

#ifdef GRID_FIT_METRICS
        if ( !( load_flags & FT_LOAD_NO_HINTING ) )
          ft_glyphslot_grid_fit_metrics( slot,
              FT_BOOL( load_flags & FT_LOAD_VERTICAL_LAYOUT ) );
#endif
      }
    }

  Load_Ok:
    /* compute the advance */
    if ( load_flags & FT_LOAD_VERTICAL_LAYOUT )
    {
      slot->advance.x = 0;
      slot->advance.y = slot->metrics.vertAdvance;
    }
    else
    {
      slot->advance.x = slot->metrics.horiAdvance;
      slot->advance.y = 0;
    }

    /* compute the linear advance in 16.16 pixels */
    if ( ( load_flags & FT_LOAD_LINEAR_DESIGN ) == 0 &&
         ( FT_IS_SCALABLE( face ) )                  )
    {
      FT_Size_Metrics*  metrics = &face->size->metrics;


      /* it's tricky! */
      slot->linearHoriAdvance = FT_MulDiv( slot->linearHoriAdvance,
                                           metrics->x_scale, 64 );

      slot->linearVertAdvance = FT_MulDiv( slot->linearVertAdvance,
                                           metrics->y_scale, 64 );
    }

    if ( ( load_flags & FT_LOAD_IGNORE_TRANSFORM ) == 0 )
    {
      FT_Face_Internal  internal = face->internal;


      /* now, transform the glyph image if needed */
      if ( internal->transform_flags )
      {
        /* get renderer */
        FT_Renderer  renderer = ft_lookup_glyph_renderer( slot );


        if ( renderer )
          error = renderer->clazz->transform_glyph(
                                     renderer, slot,
                                     &internal->transform_matrix,
                                     &internal->transform_delta );
        else if ( slot->format == FT_GLYPH_FORMAT_OUTLINE )
        {
          /* apply `standard' transformation if no renderer is available */
          if ( internal->transform_flags & 1 )
            FT_Outline_Transform( &slot->outline,
                                  &internal->transform_matrix );

          if ( internal->transform_flags & 2 )
            FT_Outline_Translate( &slot->outline,
                                  internal->transform_delta.x,
                                  internal->transform_delta.y );
        }

        /* transform advance */
        FT_Vector_Transform( &slot->advance, &internal->transform_matrix );
      }
    }

    FT_TRACE5(( "  x advance: %d\n" , slot->advance.x ));
    FT_TRACE5(( "  y advance: %d\n" , slot->advance.y ));

    FT_TRACE5(( "  linear x advance: %d\n" , slot->linearHoriAdvance ));
    FT_TRACE5(( "  linear y advance: %d\n" , slot->linearVertAdvance ));

    /* do we need to render the image now? */
    if ( !error                                    &&
         slot->format != FT_GLYPH_FORMAT_BITMAP    &&
         slot->format != FT_GLYPH_FORMAT_COMPOSITE &&
         load_flags & FT_LOAD_RENDER )
    {
      FT_Render_Mode  mode = FT_LOAD_TARGET_MODE( load_flags );


      if ( mode == FT_RENDER_MODE_NORMAL      &&
           (load_flags & FT_LOAD_MONOCHROME ) )
        mode = FT_RENDER_MODE_MONO;

      error = FT_Render_Glyph( slot, mode );
    }

  Exit:
    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Load_Char( FT_Face   face,
                FT_ULong  char_code,
                FT_Int32  load_flags )
  {
    FT_UInt  glyph_index;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    glyph_index = (FT_UInt)char_code;
    if ( face->charmap )
      glyph_index = FT_Get_Char_Index( face, char_code );

    return FT_Load_Glyph( face, glyph_index, load_flags );
  }


  /* destructor for sizes list */
  static void
  destroy_size( FT_Memory  memory,
                FT_Size    size,
                FT_Driver  driver )
  {
    /* finalize client-specific data */
    if ( size->generic.finalizer )
      size->generic.finalizer( size );

    /* finalize format-specific stuff */
    if ( driver->clazz->done_size )
      driver->clazz->done_size( size );

    FT_FREE( size->internal );
    FT_FREE( size );
  }


  static void
  ft_cmap_done_internal( FT_CMap  cmap );


  static void
  destroy_charmaps( FT_Face    face,
                    FT_Memory  memory )
  {
    FT_Int  n;


    if ( !face )
      return;

    for ( n = 0; n < face->num_charmaps; n++ )
    {
      FT_CMap  cmap = FT_CMAP( face->charmaps[n] );


      ft_cmap_done_internal( cmap );

      face->charmaps[n] = NULL;
    }

    FT_FREE( face->charmaps );
    face->num_charmaps = 0;
  }


  /* destructor for faces list */
  static void
  destroy_face( FT_Memory  memory,
                FT_Face    face,
                FT_Driver  driver )
  {
    FT_Driver_Class  clazz = driver->clazz;


    /* discard auto-hinting data */
    if ( face->autohint.finalizer )
      face->autohint.finalizer( face->autohint.data );

    /* Discard glyph slots for this face.                           */
    /* Beware!  FT_Done_GlyphSlot() changes the field `face->glyph' */
    while ( face->glyph )
      FT_Done_GlyphSlot( face->glyph );

    /* discard all sizes for this face */
    FT_List_Finalize( &face->sizes_list,
                      (FT_List_Destructor)destroy_size,
                      memory,
                      driver );
    face->size = 0;

    /* now discard client data */
    if ( face->generic.finalizer )
      face->generic.finalizer( face );

    /* discard charmaps */
    destroy_charmaps( face, memory );

    /* finalize format-specific stuff */
    if ( clazz->done_face )
      clazz->done_face( face );

    /* close the stream for this face if needed */
    FT_Stream_Free(
      face->stream,
      ( face->face_flags & FT_FACE_FLAG_EXTERNAL_STREAM ) != 0 );

    face->stream = 0;

    /* get rid of it */
    if ( face->internal )
    {
      FT_FREE( face->internal );
    }
    FT_FREE( face );
  }


  static void
  Destroy_Driver( FT_Driver  driver )
  {
    FT_List_Finalize( &driver->faces_list,
                      (FT_List_Destructor)destroy_face,
                      driver->root.memory,
                      driver );

    /* check whether we need to drop the driver's glyph loader */
    if ( FT_DRIVER_USES_OUTLINES( driver ) )
      FT_GlyphLoader_Done( driver->glyph_loader );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    find_unicode_charmap                                               */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This function finds a Unicode charmap, if there is one.            */
  /*    And if there is more than one, it tries to favour the more         */
  /*    extensive one, i.e., one that supports UCS-4 against those which   */
  /*    are limited to the BMP (said UCS-2 encoding.)                      */
  /*                                                                       */
  /*    This function is called from open_face() (just below), and also    */
  /*    from FT_Select_Charmap( ..., FT_ENCODING_UNICODE ).                */
  /*                                                                       */
  static FT_Error
  find_unicode_charmap( FT_Face  face )
  {
    FT_CharMap*  first;
    FT_CharMap*  cur;


    /* caller should have already checked that `face' is valid */
    FT_ASSERT( face );

    first = face->charmaps;

    if ( !first )
      return FT_THROW( Invalid_CharMap_Handle );

    /*
     *  The original TrueType specification(s) only specified charmap
     *  formats that are capable of mapping 8 or 16 bit character codes to
     *  glyph indices.
     *
     *  However, recent updates to the Apple and OpenType specifications
     *  introduced new formats that are capable of mapping 32-bit character
     *  codes as well.  And these are already used on some fonts, mainly to
     *  map non-BMP Asian ideographs as defined in Unicode.
     *
     *  For compatibility purposes, these fonts generally come with
     *  *several* Unicode charmaps:
     *
     *   - One of them in the "old" 16-bit format, that cannot access
     *     all glyphs in the font.
     *
     *   - Another one in the "new" 32-bit format, that can access all
     *     the glyphs.
     *
     *  This function has been written to always favor a 32-bit charmap
     *  when found.  Otherwise, a 16-bit one is returned when found.
     */

    /* Since the `interesting' table, with IDs (3,10), is normally the */
    /* last one, we loop backwards.  This loses with type1 fonts with  */
    /* non-BMP characters (<.0001%), this wins with .ttf with non-BMP  */
    /* chars (.01% ?), and this is the same about 99.99% of the time!  */

    cur = first + face->num_charmaps;  /* points after the last one */

    for ( ; --cur >= first; )
    {
      if ( cur[0]->encoding == FT_ENCODING_UNICODE )
      {
        /* XXX If some new encodings to represent UCS-4 are added, */
        /*     they should be added here.                          */
        if ( ( cur[0]->platform_id == TT_PLATFORM_MICROSOFT &&
               cur[0]->encoding_id == TT_MS_ID_UCS_4        )     ||
             ( cur[0]->platform_id == TT_PLATFORM_APPLE_UNICODE &&
               cur[0]->encoding_id == TT_APPLE_ID_UNICODE_32    ) )
        {
#ifdef FT_MAX_CHARMAP_CACHEABLE
          if ( cur - first > FT_MAX_CHARMAP_CACHEABLE )
          {
            FT_ERROR(( "find_unicode_charmap: UCS-4 cmap is found "
                       "at too late position (%d)\n", cur - first ));
            continue;
          }
#endif
          face->charmap = cur[0];
          return FT_Err_Ok;
        }
      }
    }

    /* We do not have any UCS-4 charmap.                */
    /* Do the loop again and search for UCS-2 charmaps. */
    cur = first + face->num_charmaps;

    for ( ; --cur >= first; )
    {
      if ( cur[0]->encoding == FT_ENCODING_UNICODE )
      {
#ifdef FT_MAX_CHARMAP_CACHEABLE
        if ( cur - first > FT_MAX_CHARMAP_CACHEABLE )
        {
          FT_ERROR(( "find_unicode_charmap: UCS-2 cmap is found "
                     "at too late position (%d)\n", cur - first ));
          continue;
        }
#endif
        face->charmap = cur[0];
        return FT_Err_Ok;
      }
    }

    return FT_THROW( Invalid_CharMap_Handle );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    find_variant_selector_charmap                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This function finds the variant selector charmap, if there is one. */
  /*    There can only be one (platform=0, specific=5, format=14).         */
  /*                                                                       */
  static FT_CharMap
  find_variant_selector_charmap( FT_Face  face )
  {
    FT_CharMap*  first;
    FT_CharMap*  end;
    FT_CharMap*  cur;


    /* caller should have already checked that `face' is valid */
    FT_ASSERT( face );

    first = face->charmaps;

    if ( !first )
      return NULL;

    end = first + face->num_charmaps;  /* points after the last one */

    for ( cur = first; cur < end; ++cur )
    {
      if ( cur[0]->platform_id == TT_PLATFORM_APPLE_UNICODE    &&
           cur[0]->encoding_id == TT_APPLE_ID_VARIANT_SELECTOR &&
           FT_Get_CMap_Format( cur[0] ) == 14                  )
      {
#ifdef FT_MAX_CHARMAP_CACHEABLE
        if ( cur - first > FT_MAX_CHARMAP_CACHEABLE )
        {
          FT_ERROR(( "find_unicode_charmap: UVS cmap is found "
                     "at too late position (%d)\n", cur - first ));
          continue;
        }
#endif
        return cur[0];
      }
    }

    return NULL;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    open_face                                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This function does some work for FT_Open_Face().                   */
  /*                                                                       */
  static FT_Error
  open_face( FT_Driver      driver,
             FT_Stream      *astream,
             FT_Bool        external_stream,
             FT_Long        face_index,
             FT_Int         num_params,
             FT_Parameter*  params,
             FT_Face       *aface )
  {
    FT_Memory         memory;
    FT_Driver_Class   clazz;
    FT_Face           face     = NULL;
    FT_Face_Internal  internal = NULL;

    FT_Error          error, error2;


    clazz  = driver->clazz;
    memory = driver->root.memory;

    /* allocate the face object and perform basic initialization */
    if ( FT_ALLOC( face, clazz->face_object_size ) )
      goto Fail;

    face->driver = driver;
    face->memory = memory;
    face->stream = *astream;

    /* set the FT_FACE_FLAG_EXTERNAL_STREAM bit for FT_Done_Face */
    if ( external_stream )
      face->face_flags |= FT_FACE_FLAG_EXTERNAL_STREAM;

    if ( FT_NEW( internal ) )
      goto Fail;

    face->internal = internal;

#ifdef FT_CONFIG_OPTION_INCREMENTAL
    {
      int  i;


      face->internal->incremental_interface = 0;
      for ( i = 0; i < num_params && !face->internal->incremental_interface;
            i++ )
        if ( params[i].tag == FT_PARAM_TAG_INCREMENTAL )
          face->internal->incremental_interface =
            (FT_Incremental_Interface)params[i].data;
    }
#endif

    if ( clazz->init_face )
      error = clazz->init_face( *astream,
                                face,
                                (FT_Int)face_index,
                                num_params,
                                params );
    *astream = face->stream; /* Stream may have been changed. */
    if ( error )
      goto Fail;

    /* select Unicode charmap by default */
    error2 = find_unicode_charmap( face );

    /* if no Unicode charmap can be found, FT_Err_Invalid_CharMap_Handle */
    /* is returned.                                                      */

    /* no error should happen, but we want to play safe */
    if ( error2 && FT_ERR_NEQ( error2, Invalid_CharMap_Handle ) )
    {
      error = error2;
      goto Fail;
    }

    *aface = face;

  Fail:
    if ( error )
    {
      destroy_charmaps( face, memory );
      if ( clazz->done_face )
        clazz->done_face( face );
      FT_FREE( internal );
      FT_FREE( face );
      *aface = 0;
    }

    return error;
  }


  /* there's a Mac-specific extended implementation of FT_New_Face() */
  /* in src/base/ftmac.c                                             */

#ifndef FT_MACINTOSH

  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_New_Face( FT_Library   library,
               const char*  pathname,
               FT_Long      face_index,
               FT_Face     *aface )
  {
    FT_Open_Args  args;


    /* test for valid `library' and `aface' delayed to FT_Open_Face() */
    if ( !pathname )
      return FT_THROW( Invalid_Argument );

    args.flags    = FT_OPEN_PATHNAME;
    args.pathname = (char*)pathname;
    args.stream   = NULL;

    return FT_Open_Face( library, &args, face_index, aface );
  }

#endif


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_New_Memory_Face( FT_Library      library,
                      const FT_Byte*  file_base,
                      FT_Long         file_size,
                      FT_Long         face_index,
                      FT_Face        *aface )
  {
    FT_Open_Args  args;


    /* test for valid `library' and `face' delayed to FT_Open_Face() */
    if ( !file_base )
      return FT_THROW( Invalid_Argument );

    args.flags       = FT_OPEN_MEMORY;
    args.memory_base = file_base;
    args.memory_size = file_size;
    args.stream      = NULL;

    return FT_Open_Face( library, &args, face_index, aface );
  }


#ifdef FT_CONFIG_OPTION_MAC_FONTS

  /* The behavior here is very similar to that in base/ftmac.c, but it     */
  /* is designed to work on non-mac systems, so no mac specific calls.     */
  /*                                                                       */
  /* We look at the file and determine if it is a mac dfont file or a mac  */
  /* resource file, or a macbinary file containing a mac resource file.    */
  /*                                                                       */
  /* Unlike ftmac I'm not going to look at a `FOND'.  I don't really see   */
  /* the point, especially since there may be multiple `FOND' resources.   */
  /* Instead I'll just look for `sfnt' and `POST' resources, ordered as    */
  /* they occur in the file.                                               */
  /*                                                                       */
  /* Note that multiple `POST' resources do not mean multiple postscript   */
  /* fonts; they all get jammed together to make what is essentially a     */
  /* pfb file.                                                             */
  /*                                                                       */
  /* We aren't interested in `NFNT' or `FONT' bitmap resources.            */
  /*                                                                       */
  /* As soon as we get an `sfnt' load it into memory and pass it off to    */
  /* FT_Open_Face.                                                         */
  /*                                                                       */
  /* If we have a (set of) `POST' resources, massage them into a (memory)  */
  /* pfb file and pass that to FT_Open_Face.  (As with ftmac.c I'm not     */
  /* going to try to save the kerning info.  After all that lives in the   */
  /* `FOND' which isn't in the file containing the `POST' resources so     */
  /* we don't really have access to it.                                    */


  /* Finalizer for a memory stream; gets called by FT_Done_Face(). */
  /* It frees the memory it uses.                                  */
  /* From ftmac.c.                                                 */
  static void
  memory_stream_close( FT_Stream  stream )
  {
    FT_Memory  memory = stream->memory;


    FT_FREE( stream->base );

    stream->size  = 0;
    stream->base  = 0;
    stream->close = 0;
  }


  /* Create a new memory stream from a buffer and a size. */
  /* From ftmac.c.                                        */
  static FT_Error
  new_memory_stream( FT_Library           library,
                     FT_Byte*             base,
                     FT_ULong             size,
                     FT_Stream_CloseFunc  close,
                     FT_Stream           *astream )
  {
    FT_Error   error;
    FT_Memory  memory;
    FT_Stream  stream = NULL;


    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( !base )
      return FT_THROW( Invalid_Argument );

    *astream = 0;
    memory = library->memory;
    if ( FT_NEW( stream ) )
      goto Exit;

    FT_Stream_OpenMemory( stream, base, size );

    stream->close = close;

    *astream = stream;

  Exit:
    return error;
  }


  /* Create a new FT_Face given a buffer and a driver name. */
  /* from ftmac.c */
  FT_LOCAL_DEF( FT_Error )
  open_face_from_buffer( FT_Library   library,
                         FT_Byte*     base,
                         FT_ULong     size,
                         FT_Long      face_index,
                         const char*  driver_name,
                         FT_Face     *aface )
  {
    FT_Open_Args  args;
    FT_Error      error;
    FT_Stream     stream = NULL;
    FT_Memory     memory = library->memory;


    error = new_memory_stream( library,
                               base,
                               size,
                               memory_stream_close,
                               &stream );
    if ( error )
    {
      FT_FREE( base );
      return error;
    }

    args.flags = FT_OPEN_STREAM;
    args.stream = stream;
    if ( driver_name )
    {
      args.flags = args.flags | FT_OPEN_DRIVER;
      args.driver = FT_Get_Module( library, driver_name );
    }

#ifdef FT_MACINTOSH
    /* At this point, face_index has served its purpose;      */
    /* whoever calls this function has already used it to     */
    /* locate the correct font data.  We should not propagate */
    /* this index to FT_Open_Face() (unless it is negative).  */

    if ( face_index > 0 )
      face_index = 0;
#endif

    error = FT_Open_Face( library, &args, face_index, aface );

    if ( error == FT_Err_Ok )
      (*aface)->face_flags &= ~FT_FACE_FLAG_EXTERNAL_STREAM;
    else
#ifdef FT_MACINTOSH
      FT_Stream_Free( stream, 0 );
#else
    {
      FT_Stream_Close( stream );
      FT_FREE( stream );
    }
#endif

    return error;
  }


  /* Look up `TYP1' or `CID ' table from sfnt table directory.       */
  /* `offset' and `length' must exclude the binary header in tables. */

  /* Type 1 and CID-keyed font drivers should recognize sfnt-wrapped */
  /* format too.  Here, since we can't expect that the TrueType font */
  /* driver is loaded unconditially, we must parse the font by       */
  /* ourselves.  We are only interested in the name of the table and */
  /* the offset.                                                     */

  static FT_Error
  ft_lookup_PS_in_sfnt_stream( FT_Stream  stream,
                               FT_Long    face_index,
                               FT_ULong*  offset,
                               FT_ULong*  length,
                               FT_Bool*   is_sfnt_cid )
  {
    FT_Error   error;
    FT_UShort  numTables;
    FT_Long    pstable_index;
    FT_ULong   tag;
    int        i;


    *offset = 0;
    *length = 0;
    *is_sfnt_cid = FALSE;

    /* TODO: support for sfnt-wrapped PS/CID in TTC format */

    /* version check for 'typ1' (should be ignored?) */
    if ( FT_READ_ULONG( tag ) )
      return error;
    if ( tag != TTAG_typ1 )
      return FT_THROW( Unknown_File_Format );

    if ( FT_READ_USHORT( numTables ) )
      return error;
    if ( FT_STREAM_SKIP( 2 * 3 ) ) /* skip binary search header */
      return error;

    pstable_index = -1;
    *is_sfnt_cid  = FALSE;

    for ( i = 0; i < numTables; i++ )
    {
      if ( FT_READ_ULONG( tag )     || FT_STREAM_SKIP( 4 )      ||
           FT_READ_ULONG( *offset ) || FT_READ_ULONG( *length ) )
        return error;

      if ( tag == TTAG_CID )
      {
        pstable_index++;
        *offset += 22;
        *length -= 22;
        *is_sfnt_cid = TRUE;
        if ( face_index < 0 )
          return FT_Err_Ok;
      }
      else if ( tag == TTAG_TYP1 )
      {
        pstable_index++;
        *offset += 24;
        *length -= 24;
        *is_sfnt_cid = FALSE;
        if ( face_index < 0 )
          return FT_Err_Ok;
      }
      if ( face_index >= 0 && pstable_index == face_index )
        return FT_Err_Ok;
    }
    return FT_THROW( Table_Missing );
  }


  FT_LOCAL_DEF( FT_Error )
  open_face_PS_from_sfnt_stream( FT_Library     library,
                                 FT_Stream      stream,
                                 FT_Long        face_index,
                                 FT_Int         num_params,
                                 FT_Parameter  *params,
                                 FT_Face       *aface )
  {
    FT_Error   error;
    FT_Memory  memory = library->memory;
    FT_ULong   offset, length;
    FT_Long    pos;
    FT_Bool    is_sfnt_cid;
    FT_Byte*   sfnt_ps = NULL;

    FT_UNUSED( num_params );
    FT_UNUSED( params );


    pos = FT_Stream_Pos( stream );

    error = ft_lookup_PS_in_sfnt_stream( stream,
                                         face_index,
                                         &offset,
                                         &length,
                                         &is_sfnt_cid );
    if ( error )
      goto Exit;

    if ( FT_Stream_Seek( stream, pos + offset ) )
      goto Exit;

    if ( FT_ALLOC( sfnt_ps, (FT_Long)length ) )
      goto Exit;

    error = FT_Stream_Read( stream, (FT_Byte *)sfnt_ps, length );
    if ( error )
      goto Exit;

    error = open_face_from_buffer( library,
                                   sfnt_ps,
                                   length,
                                   FT_MIN( face_index, 0 ),
                                   is_sfnt_cid ? "cid" : "type1",
                                   aface );
  Exit:
    {
      FT_Error  error1;


      if ( FT_ERR_EQ( error, Unknown_File_Format ) )
      {
        error1 = FT_Stream_Seek( stream, pos );
        if ( error1 )
          return error1;
      }

      return error;
    }
  }


#ifndef FT_MACINTOSH

  /* The resource header says we've got resource_cnt `POST' (type1) */
  /* resources in this file.  They all need to be coalesced into    */
  /* one lump which gets passed on to the type1 driver.             */
  /* Here can be only one PostScript font in a file so face_index   */
  /* must be 0 (or -1).                                             */
  /*                                                                */
  static FT_Error
  Mac_Read_POST_Resource( FT_Library  library,
                          FT_Stream   stream,
                          FT_Long    *offsets,
                          FT_Long     resource_cnt,
                          FT_Long     face_index,
                          FT_Face    *aface )
  {
    FT_Error   error  = FT_ERR( Cannot_Open_Resource );
    FT_Memory  memory = library->memory;
    FT_Byte*   pfb_data = NULL;
    int        i, type, flags;
    FT_Long    len;
    FT_Long    pfb_len, pfb_pos, pfb_lenpos;
    FT_Long    rlen, temp;


    if ( face_index == -1 )
      face_index = 0;
    if ( face_index != 0 )
      return error;

    /* Find the length of all the POST resources, concatenated.  Assume */
    /* worst case (each resource in its own section).                   */
    pfb_len = 0;
    for ( i = 0; i < resource_cnt; ++i )
    {
      error = FT_Stream_Seek( stream, offsets[i] );
      if ( error )
        goto Exit;
      if ( FT_READ_LONG( temp ) )
        goto Exit;
      pfb_len += temp + 6;
    }

    if ( FT_ALLOC( pfb_data, (FT_Long)pfb_len + 2 ) )
      goto Exit;

    pfb_data[0] = 0x80;
    pfb_data[1] = 1;            /* Ascii section */
    pfb_data[2] = 0;            /* 4-byte length, fill in later */
    pfb_data[3] = 0;
    pfb_data[4] = 0;
    pfb_data[5] = 0;
    pfb_pos     = 6;
    pfb_lenpos  = 2;

    len = 0;
    type = 1;
    for ( i = 0; i < resource_cnt; ++i )
    {
      error = FT_Stream_Seek( stream, offsets[i] );
      if ( error )
        goto Exit2;
      if ( FT_READ_LONG( rlen ) )
        goto Exit;
      if ( FT_READ_USHORT( flags ) )
        goto Exit;
      FT_TRACE3(( "POST fragment[%d]: offsets=0x%08x, rlen=0x%08x, flags=0x%04x\n",
                   i, offsets[i], rlen, flags ));

      /* postpone the check of rlen longer than buffer until FT_Stream_Read() */
      if ( ( flags >> 8 ) == 0 )        /* Comment, should not be loaded */
        continue;

      /* the flags are part of the resource, so rlen >= 2.  */
      /* but some fonts declare rlen = 0 for empty fragment */
      if ( rlen > 2 )
        rlen -= 2;
      else
        rlen = 0;

      if ( ( flags >> 8 ) == type )
        len += rlen;
      else
      {
        if ( pfb_lenpos + 3 > pfb_len + 2 )
          goto Exit2;
        pfb_data[pfb_lenpos    ] = (FT_Byte)( len );
        pfb_data[pfb_lenpos + 1] = (FT_Byte)( len >> 8 );
        pfb_data[pfb_lenpos + 2] = (FT_Byte)( len >> 16 );
        pfb_data[pfb_lenpos + 3] = (FT_Byte)( len >> 24 );

        if ( ( flags >> 8 ) == 5 )      /* End of font mark */
          break;

        if ( pfb_pos + 6 > pfb_len + 2 )
          goto Exit2;
        pfb_data[pfb_pos++] = 0x80;

        type = flags >> 8;
        len = rlen;

        pfb_data[pfb_pos++] = (FT_Byte)type;
        pfb_lenpos          = pfb_pos;
        pfb_data[pfb_pos++] = 0;        /* 4-byte length, fill in later */
        pfb_data[pfb_pos++] = 0;
        pfb_data[pfb_pos++] = 0;
        pfb_data[pfb_pos++] = 0;
      }

      error = FT_ERR( Cannot_Open_Resource );
      if ( pfb_pos > pfb_len || pfb_pos + rlen > pfb_len )
        goto Exit2;

      error = FT_Stream_Read( stream, (FT_Byte *)pfb_data + pfb_pos, rlen );
      if ( error )
        goto Exit2;
      pfb_pos += rlen;
    }

    if ( pfb_pos + 2 > pfb_len + 2 )
      goto Exit2;
    pfb_data[pfb_pos++] = 0x80;
    pfb_data[pfb_pos++] = 3;

    if ( pfb_lenpos + 3 > pfb_len + 2 )
      goto Exit2;
    pfb_data[pfb_lenpos    ] = (FT_Byte)( len );
    pfb_data[pfb_lenpos + 1] = (FT_Byte)( len >> 8 );
    pfb_data[pfb_lenpos + 2] = (FT_Byte)( len >> 16 );
    pfb_data[pfb_lenpos + 3] = (FT_Byte)( len >> 24 );

    return open_face_from_buffer( library,
                                  pfb_data,
                                  pfb_pos,
                                  face_index,
                                  "type1",
                                  aface );

  Exit2:
    FT_FREE( pfb_data );

  Exit:
    return error;
  }


  /* The resource header says we've got resource_cnt `sfnt'      */
  /* (TrueType/OpenType) resources in this file.  Look through   */
  /* them for the one indicated by face_index, load it into mem, */
  /* pass it on the the truetype driver and return it.           */
  /*                                                             */
  static FT_Error
  Mac_Read_sfnt_Resource( FT_Library  library,
                          FT_Stream   stream,
                          FT_Long    *offsets,
                          FT_Long     resource_cnt,
                          FT_Long     face_index,
                          FT_Face    *aface )
  {
    FT_Memory  memory = library->memory;
    FT_Byte*   sfnt_data = NULL;
    FT_Error   error;
    FT_Long    flag_offset;
    FT_Long    rlen;
    int        is_cff;
    FT_Long    face_index_in_resource = 0;


    if ( face_index == -1 )
      face_index = 0;
    if ( face_index >= resource_cnt )
      return FT_THROW( Cannot_Open_Resource );

    flag_offset = offsets[face_index];
    error = FT_Stream_Seek( stream, flag_offset );
    if ( error )
      goto Exit;

    if ( FT_READ_LONG( rlen ) )
      goto Exit;
    if ( rlen == -1 )
      return FT_THROW( Cannot_Open_Resource );

    error = open_face_PS_from_sfnt_stream( library,
                                           stream,
                                           face_index,
                                           0, NULL,
                                           aface );
    if ( !error )
      goto Exit;

    /* rewind sfnt stream before open_face_PS_from_sfnt_stream() */
    if ( FT_Stream_Seek( stream, flag_offset + 4 ) )
      goto Exit;

    if ( FT_ALLOC( sfnt_data, (FT_Long)rlen ) )
      return error;
    error = FT_Stream_Read( stream, (FT_Byte *)sfnt_data, rlen );
    if ( error )
      goto Exit;

    is_cff = rlen > 4 && !ft_memcmp( sfnt_data, "OTTO", 4 );
    error = open_face_from_buffer( library,
                                   sfnt_data,
                                   rlen,
                                   face_index_in_resource,
                                   is_cff ? "cff" : "truetype",
                                   aface );

  Exit:
    return error;
  }


  /* Check for a valid resource fork header, or a valid dfont    */
  /* header.  In a resource fork the first 16 bytes are repeated */
  /* at the location specified by bytes 4-7.  In a dfont bytes   */
  /* 4-7 point to 16 bytes of zeroes instead.                    */
  /*                                                             */
  static FT_Error
  IsMacResource( FT_Library  library,
                 FT_Stream   stream,
                 FT_Long     resource_offset,
                 FT_Long     face_index,
                 FT_Face    *aface )
  {
    FT_Memory  memory = library->memory;
    FT_Error   error;
    FT_Long    map_offset, rdara_pos;
    FT_Long    *data_offsets;
    FT_Long    count;


    error = FT_Raccess_Get_HeaderInfo( library, stream, resource_offset,
                                       &map_offset, &rdara_pos );
    if ( error )
      return error;

    /* POST resources must be sorted to concatenate properly */
    error = FT_Raccess_Get_DataOffsets( library, stream,
                                        map_offset, rdara_pos,
                                        TTAG_POST, TRUE,
                                        &data_offsets, &count );
    if ( !error )
    {
      error = Mac_Read_POST_Resource( library, stream, data_offsets, count,
                                      face_index, aface );
      FT_FREE( data_offsets );
      /* POST exists in an LWFN providing a single face */
      if ( !error )
        (*aface)->num_faces = 1;
      return error;
    }

    /* sfnt resources should not be sorted to preserve the face order by
       QuickDraw API */
    error = FT_Raccess_Get_DataOffsets( library, stream,
                                        map_offset, rdara_pos,
                                        TTAG_sfnt, FALSE,
                                        &data_offsets, &count );
    if ( !error )
    {
      FT_Long  face_index_internal = face_index % count;


      error = Mac_Read_sfnt_Resource( library, stream, data_offsets, count,
                                      face_index_internal, aface );
      FT_FREE( data_offsets );
      if ( !error )
        (*aface)->num_faces = count;
    }

    return error;
  }


  /* Check for a valid macbinary header, and if we find one   */
  /* check that the (flattened) resource fork in it is valid. */
  /*                                                          */
  static FT_Error
  IsMacBinary( FT_Library  library,
               FT_Stream   stream,
               FT_Long     face_index,
               FT_Face    *aface )
  {
    unsigned char  header[128];
    FT_Error       error;
    FT_Long        dlen, offset;


    if ( NULL == stream )
      return FT_THROW( Invalid_Stream_Operation );

    error = FT_Stream_Seek( stream, 0 );
    if ( error )
      goto Exit;

    error = FT_Stream_Read( stream, (FT_Byte*)header, 128 );
    if ( error )
      goto Exit;

    if (            header[ 0] !=  0 ||
                    header[74] !=  0 ||
                    header[82] !=  0 ||
                    header[ 1] ==  0 ||
                    header[ 1] >  33 ||
                    header[63] !=  0 ||
         header[2 + header[1]] !=  0 )
      return FT_THROW( Unknown_File_Format );

    dlen = ( header[0x53] << 24 ) |
           ( header[0x54] << 16 ) |
           ( header[0x55] <<  8 ) |
             header[0x56];
#if 0
    rlen = ( header[0x57] << 24 ) |
           ( header[0x58] << 16 ) |
           ( header[0x59] <<  8 ) |
             header[0x5a];
#endif /* 0 */
    offset = 128 + ( ( dlen + 127 ) & ~127 );

    return IsMacResource( library, stream, offset, face_index, aface );

  Exit:
    return error;
  }


  static FT_Error
  load_face_in_embedded_rfork( FT_Library           library,
                               FT_Stream            stream,
                               FT_Long              face_index,
                               FT_Face             *aface,
                               const FT_Open_Args  *args )
  {

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_raccess

    FT_Memory  memory = library->memory;
    FT_Error   error  = FT_ERR( Unknown_File_Format );
    int        i;

    char *     file_names[FT_RACCESS_N_RULES];
    FT_Long    offsets[FT_RACCESS_N_RULES];
    FT_Error   errors[FT_RACCESS_N_RULES];
    FT_Bool    is_darwin_vfs, vfs_rfork_has_no_font = FALSE; /* not tested */

    FT_Open_Args  args2;
    FT_Stream     stream2 = 0;


    FT_Raccess_Guess( library, stream,
                      args->pathname, file_names, offsets, errors );

    for ( i = 0; i < FT_RACCESS_N_RULES; i++ )
    {
      is_darwin_vfs = ft_raccess_rule_by_darwin_vfs( library, i );
      if ( is_darwin_vfs && vfs_rfork_has_no_font )
      {
        FT_TRACE3(( "Skip rule %d: darwin vfs resource fork"
                    " is already checked and"
                    " no font is found\n", i ));
        continue;
      }

      if ( errors[i] )
      {
        FT_TRACE3(( "Error[%d] has occurred in rule %d\n", errors[i], i ));
        continue;
      }

      args2.flags    = FT_OPEN_PATHNAME;
      args2.pathname = file_names[i] ? file_names[i] : args->pathname;

      FT_TRACE3(( "Try rule %d: %s (offset=%d) ...",
                  i, args2.pathname, offsets[i] ));

      error = FT_Stream_New( library, &args2, &stream2 );
      if ( is_darwin_vfs && FT_ERR_EQ( error, Cannot_Open_Stream ) )
        vfs_rfork_has_no_font = TRUE;

      if ( error )
      {
        FT_TRACE3(( "failed\n" ));
        continue;
      }

      error = IsMacResource( library, stream2, offsets[i],
                             face_index, aface );
      FT_Stream_Free( stream2, 0 );

      FT_TRACE3(( "%s\n", error ? "failed": "successful" ));

      if ( !error )
          break;
      else if ( is_darwin_vfs )
          vfs_rfork_has_no_font = TRUE;
    }

    for (i = 0; i < FT_RACCESS_N_RULES; i++)
    {
      if ( file_names[i] )
        FT_FREE( file_names[i] );
    }

    /* Caller (load_mac_face) requires FT_Err_Unknown_File_Format. */
    if ( error )
      error = FT_ERR( Unknown_File_Format );

    return error;

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_objs

  }


  /* Check for some macintosh formats without Carbon framework.    */
  /* Is this a macbinary file?  If so look at the resource fork.   */
  /* Is this a mac dfont file?                                     */
  /* Is this an old style resource fork? (in data)                 */
  /* Else call load_face_in_embedded_rfork to try extra rules      */
  /* (defined in `ftrfork.c').                                     */
  /*                                                               */
  static FT_Error
  load_mac_face( FT_Library           library,
                 FT_Stream            stream,
                 FT_Long              face_index,
                 FT_Face             *aface,
                 const FT_Open_Args  *args )
  {
    FT_Error error;
    FT_UNUSED( args );


    error = IsMacBinary( library, stream, face_index, aface );
    if ( FT_ERR_EQ( error, Unknown_File_Format ) )
    {

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_raccess

      FT_TRACE3(( "Try as dfont: %s ...", args->pathname ));

      error = IsMacResource( library, stream, 0, face_index, aface );

      FT_TRACE3(( "%s\n", error ? "failed" : "successful" ));

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_objs

    }

    if ( ( FT_ERR_EQ( error, Unknown_File_Format )      ||
           FT_ERR_EQ( error, Invalid_Stream_Operation ) ) &&
         ( args->flags & FT_OPEN_PATHNAME )               )
      error = load_face_in_embedded_rfork( library, stream,
                                           face_index, aface, args );
    return error;
  }
#endif

#endif  /* !FT_MACINTOSH && FT_CONFIG_OPTION_MAC_FONTS */


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Open_Face( FT_Library           library,
                const FT_Open_Args*  args,
                FT_Long              face_index,
                FT_Face             *aface )
  {
    FT_Error     error;
    FT_Driver    driver = NULL;
    FT_Memory    memory = NULL;
    FT_Stream    stream = NULL;
    FT_Face      face   = NULL;
    FT_ListNode  node   = NULL;
    FT_Bool      external_stream;
    FT_Module*   cur;
    FT_Module*   limit;


    /* test for valid `library' delayed to */
    /* FT_Stream_New()                     */

    if ( ( !aface && face_index >= 0 ) || !args )
      return FT_THROW( Invalid_Argument );

    external_stream = FT_BOOL( ( args->flags & FT_OPEN_STREAM ) &&
                               args->stream                     );

    /* create input stream */
    error = FT_Stream_New( library, args, &stream );
    if ( error )
      goto Fail3;

    memory = library->memory;

    /* If the font driver is specified in the `args' structure, use */
    /* it.  Otherwise, we scan the list of registered drivers.      */
    if ( ( args->flags & FT_OPEN_DRIVER ) && args->driver )
    {
      driver = FT_DRIVER( args->driver );

      /* not all modules are drivers, so check... */
      if ( FT_MODULE_IS_DRIVER( driver ) )
      {
        FT_Int         num_params = 0;
        FT_Parameter*  params     = 0;


        if ( args->flags & FT_OPEN_PARAMS )
        {
          num_params = args->num_params;
          params     = args->params;
        }

        error = open_face( driver, &stream, external_stream, face_index,
                           num_params, params, &face );
        if ( !error )
          goto Success;
      }
      else
        error = FT_THROW( Invalid_Handle );

      FT_Stream_Free( stream, external_stream );
      goto Fail;
    }
    else
    {
      error = FT_ERR( Missing_Module );

      /* check each font driver for an appropriate format */
      cur   = library->modules;
      limit = cur + library->num_modules;

      for ( ; cur < limit; cur++ )
      {
        /* not all modules are font drivers, so check... */
        if ( FT_MODULE_IS_DRIVER( cur[0] ) )
        {
          FT_Int         num_params = 0;
          FT_Parameter*  params     = 0;


          driver = FT_DRIVER( cur[0] );

          if ( args->flags & FT_OPEN_PARAMS )
          {
            num_params = args->num_params;
            params     = args->params;
          }

          error = open_face( driver, &stream, external_stream, face_index,
                             num_params, params, &face );
          if ( !error )
            goto Success;

#ifdef FT_CONFIG_OPTION_MAC_FONTS
          if ( ft_strcmp( cur[0]->clazz->module_name, "truetype" ) == 0 &&
               FT_ERR_EQ( error, Table_Missing )                        )
          {
            /* TrueType but essential tables are missing */
            if ( FT_Stream_Seek( stream, 0 ) )
              break;

            error = open_face_PS_from_sfnt_stream( library,
                                                   stream,
                                                   face_index,
                                                   num_params,
                                                   params,
                                                   aface );
            if ( !error )
            {
              FT_Stream_Free( stream, external_stream );
              return error;
            }
          }
#endif

          if ( FT_ERR_NEQ( error, Unknown_File_Format ) )
            goto Fail3;
        }
      }

    Fail3:
      /* If we are on the mac, and we get an                          */
      /* FT_Err_Invalid_Stream_Operation it may be because we have an */
      /* empty data fork, so we need to check the resource fork.      */
      if ( FT_ERR_NEQ( error, Cannot_Open_Stream )       &&
           FT_ERR_NEQ( error, Unknown_File_Format )      &&
           FT_ERR_NEQ( error, Invalid_Stream_Operation ) )
        goto Fail2;

#if !defined( FT_MACINTOSH ) && defined( FT_CONFIG_OPTION_MAC_FONTS )
      error = load_mac_face( library, stream, face_index, aface, args );
      if ( !error )
      {
        /* We don't want to go to Success here.  We've already done that. */
        /* On the other hand, if we succeeded we still need to close this */
        /* stream (we opened a different stream which extracted the       */
        /* interesting information out of this stream here.  That stream  */
        /* will still be open and the face will point to it).             */
        FT_Stream_Free( stream, external_stream );
        return error;
      }

      if ( FT_ERR_NEQ( error, Unknown_File_Format ) )
        goto Fail2;
#endif  /* !FT_MACINTOSH && FT_CONFIG_OPTION_MAC_FONTS */

      /* no driver is able to handle this format */
      error = FT_THROW( Unknown_File_Format );

  Fail2:
      FT_Stream_Free( stream, external_stream );
      goto Fail;
    }

  Success:
    FT_TRACE4(( "FT_Open_Face: New face object, adding to list\n" ));

    /* add the face object to its driver's list */
    if ( FT_NEW( node ) )
      goto Fail;

    node->data = face;
    /* don't assume driver is the same as face->driver, so use */
    /* face->driver instead.                                   */
    FT_List_Add( &face->driver->faces_list, node );

    /* now allocate a glyph slot object for the face */
    FT_TRACE4(( "FT_Open_Face: Creating glyph slot\n" ));

    if ( face_index >= 0 )
    {
      error = FT_New_GlyphSlot( face, NULL );
      if ( error )
        goto Fail;

      /* finally, allocate a size object for the face */
      {
        FT_Size  size;


        FT_TRACE4(( "FT_Open_Face: Creating size object\n" ));

        error = FT_New_Size( face, &size );
        if ( error )
          goto Fail;

        face->size = size;
      }
    }

    /* some checks */

    if ( FT_IS_SCALABLE( face ) )
    {
      if ( face->height < 0 )
        face->height = (FT_Short)-face->height;

      if ( !FT_HAS_VERTICAL( face ) )
        face->max_advance_height = (FT_Short)face->height;
    }

    if ( FT_HAS_FIXED_SIZES( face ) )
    {
      FT_Int  i;


      for ( i = 0; i < face->num_fixed_sizes; i++ )
      {
        FT_Bitmap_Size*  bsize = face->available_sizes + i;


        if ( bsize->height < 0 )
          bsize->height = (FT_Short)-bsize->height;
        if ( bsize->x_ppem < 0 )
          bsize->x_ppem = (FT_Short)-bsize->x_ppem;
        if ( bsize->y_ppem < 0 )
          bsize->y_ppem = -bsize->y_ppem;
      }
    }

    /* initialize internal face data */
    {
      FT_Face_Internal  internal = face->internal;


      internal->transform_matrix.xx = 0x10000L;
      internal->transform_matrix.xy = 0;
      internal->transform_matrix.yx = 0;
      internal->transform_matrix.yy = 0x10000L;

      internal->transform_delta.x = 0;
      internal->transform_delta.y = 0;

      internal->refcount = 1;
    }

    if ( aface )
      *aface = face;
    else
      FT_Done_Face( face );

    goto Exit;

  Fail:
    if ( node )
      FT_Done_Face( face );    /* face must be in the driver's list */
    else if ( face )
      destroy_face( memory, face, driver );

  Exit:
    FT_TRACE4(( "FT_Open_Face: Return %d\n", error ));

    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Attach_File( FT_Face      face,
                  const char*  filepathname )
  {
    FT_Open_Args  open;


    /* test for valid `face' delayed to FT_Attach_Stream() */

    if ( !filepathname )
      return FT_THROW( Invalid_Argument );

    open.stream   = NULL;
    open.flags    = FT_OPEN_PATHNAME;
    open.pathname = (char*)filepathname;

    return FT_Attach_Stream( face, &open );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Attach_Stream( FT_Face        face,
                    FT_Open_Args*  parameters )
  {
    FT_Stream  stream;
    FT_Error   error;
    FT_Driver  driver;

    FT_Driver_Class  clazz;


    /* test for valid `parameters' delayed to FT_Stream_New() */

    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    driver = face->driver;
    if ( !driver )
      return FT_THROW( Invalid_Driver_Handle );

    error = FT_Stream_New( driver->root.library, parameters, &stream );
    if ( error )
      goto Exit;

    /* we implement FT_Attach_Stream in each driver through the */
    /* `attach_file' interface                                  */

    error = FT_ERR( Unimplemented_Feature );
    clazz = driver->clazz;
    if ( clazz->attach_file )
      error = clazz->attach_file( face, stream );

    /* close the attached stream */
    FT_Stream_Free( stream,
                    (FT_Bool)( parameters->stream &&
                               ( parameters->flags & FT_OPEN_STREAM ) ) );

  Exit:
    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Reference_Face( FT_Face  face )
  {
    face->internal->refcount++;

    return FT_Err_Ok;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Done_Face( FT_Face  face )
  {
    FT_Error     error;
    FT_Driver    driver;
    FT_Memory    memory;
    FT_ListNode  node;


    error = FT_ERR( Invalid_Face_Handle );
    if ( face && face->driver )
    {
      face->internal->refcount--;
      if ( face->internal->refcount > 0 )
        error = FT_Err_Ok;
      else
      {
        driver = face->driver;
        memory = driver->root.memory;

        /* find face in driver's list */
        node = FT_List_Find( &driver->faces_list, face );
        if ( node )
        {
          /* remove face object from the driver's list */
          FT_List_Remove( &driver->faces_list, node );
          FT_FREE( node );

          /* now destroy the object proper */
          destroy_face( memory, face, driver );
          error = FT_Err_Ok;
        }
      }
    }

    return error;
  }


  /* documentation is in ftobjs.h */

  FT_EXPORT_DEF( FT_Error )
  FT_New_Size( FT_Face   face,
               FT_Size  *asize )
  {
    FT_Error         error;
    FT_Memory        memory;
    FT_Driver        driver;
    FT_Driver_Class  clazz;

    FT_Size          size = 0;
    FT_ListNode      node = 0;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( !asize )
      return FT_THROW( Invalid_Size_Handle );

    if ( !face->driver )
      return FT_THROW( Invalid_Driver_Handle );

    *asize = 0;

    driver = face->driver;
    clazz  = driver->clazz;
    memory = face->memory;

    /* Allocate new size object and perform basic initialisation */
    if ( FT_ALLOC( size, clazz->size_object_size ) || FT_NEW( node ) )
      goto Exit;

    size->face = face;

    /* for now, do not use any internal fields in size objects */
    size->internal = 0;

    if ( clazz->init_size )
      error = clazz->init_size( size );

    /* in case of success, add to the face's list */
    if ( !error )
    {
      *asize     = size;
      node->data = size;
      FT_List_Add( &face->sizes_list, node );
    }

  Exit:
    if ( error )
    {
      FT_FREE( node );
      FT_FREE( size );
    }

    return error;
  }


  /* documentation is in ftobjs.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Done_Size( FT_Size  size )
  {
    FT_Error     error;
    FT_Driver    driver;
    FT_Memory    memory;
    FT_Face      face;
    FT_ListNode  node;


    if ( !size )
      return FT_THROW( Invalid_Size_Handle );

    face = size->face;
    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    driver = face->driver;
    if ( !driver )
      return FT_THROW( Invalid_Driver_Handle );

    memory = driver->root.memory;

    error = FT_Err_Ok;
    node  = FT_List_Find( &face->sizes_list, size );
    if ( node )
    {
      FT_List_Remove( &face->sizes_list, node );
      FT_FREE( node );

      if ( face->size == size )
      {
        face->size = 0;
        if ( face->sizes_list.head )
          face->size = (FT_Size)(face->sizes_list.head->data);
      }

      destroy_size( memory, size, driver );
    }
    else
      error = FT_THROW( Invalid_Size_Handle );

    return error;
  }


  /* documentation is in ftobjs.h */

  FT_BASE_DEF( FT_Error )
  FT_Match_Size( FT_Face          face,
                 FT_Size_Request  req,
                 FT_Bool          ignore_width,
                 FT_ULong*        size_index )
  {
    FT_Int   i;
    FT_Long  w, h;


    if ( !FT_HAS_FIXED_SIZES( face ) )
      return FT_THROW( Invalid_Face_Handle );

    /* FT_Bitmap_Size doesn't provide enough info... */
    if ( req->type != FT_SIZE_REQUEST_TYPE_NOMINAL )
      return FT_THROW( Unimplemented_Feature );

    w = FT_REQUEST_WIDTH ( req );
    h = FT_REQUEST_HEIGHT( req );

    if ( req->width && !req->height )
      h = w;
    else if ( !req->width && req->height )
      w = h;

    w = FT_PIX_ROUND( w );
    h = FT_PIX_ROUND( h );

    for ( i = 0; i < face->num_fixed_sizes; i++ )
    {
      FT_Bitmap_Size*  bsize = face->available_sizes + i;


      if ( h != FT_PIX_ROUND( bsize->y_ppem ) )
        continue;

      if ( w == FT_PIX_ROUND( bsize->x_ppem ) || ignore_width )
      {
        FT_TRACE3(( "FT_Match_Size: bitmap strike %d matches\n", i ));

        if ( size_index )
          *size_index = (FT_ULong)i;

        return FT_Err_Ok;
      }
    }

    return FT_THROW( Invalid_Pixel_Size );
  }


  /* documentation is in ftobjs.h */

  FT_BASE_DEF( void )
  ft_synthesize_vertical_metrics( FT_Glyph_Metrics*  metrics,
                                  FT_Pos             advance )
  {
    FT_Pos  height = metrics->height;


    /* compensate for glyph with bbox above/below the baseline */
    if ( metrics->horiBearingY < 0 )
    {
      if ( height < metrics->horiBearingY )
        height = metrics->horiBearingY;
    }
    else if ( metrics->horiBearingY > 0 )
      height -= metrics->horiBearingY;

    /* the factor 1.2 is a heuristical value */
    if ( !advance )
      advance = height * 12 / 10;

    metrics->vertBearingX = metrics->horiBearingX - metrics->horiAdvance / 2;
    metrics->vertBearingY = ( advance - height ) / 2;
    metrics->vertAdvance  = advance;
  }


  static void
  ft_recompute_scaled_metrics( FT_Face           face,
                               FT_Size_Metrics*  metrics )
  {
    /* Compute root ascender, descender, test height, and max_advance */

#ifdef GRID_FIT_METRICS
    metrics->ascender    = FT_PIX_CEIL( FT_MulFix( face->ascender,
                                                   metrics->y_scale ) );

    metrics->descender   = FT_PIX_FLOOR( FT_MulFix( face->descender,
                                                    metrics->y_scale ) );

    metrics->height      = FT_PIX_ROUND( FT_MulFix( face->height,
                                                    metrics->y_scale ) );

    metrics->max_advance = FT_PIX_ROUND( FT_MulFix( face->max_advance_width,
                                                    metrics->x_scale ) );
#else /* !GRID_FIT_METRICS */
    metrics->ascender    = FT_MulFix( face->ascender,
                                      metrics->y_scale );

    metrics->descender   = FT_MulFix( face->descender,
                                      metrics->y_scale );

    metrics->height      = FT_MulFix( face->height,
                                      metrics->y_scale );

    metrics->max_advance = FT_MulFix( face->max_advance_width,
                                      metrics->x_scale );
#endif /* !GRID_FIT_METRICS */
  }


  FT_BASE_DEF( void )
  FT_Select_Metrics( FT_Face   face,
                     FT_ULong  strike_index )
  {
    FT_Size_Metrics*  metrics;
    FT_Bitmap_Size*   bsize;


    metrics = &face->size->metrics;
    bsize   = face->available_sizes + strike_index;

    metrics->x_ppem = (FT_UShort)( ( bsize->x_ppem + 32 ) >> 6 );
    metrics->y_ppem = (FT_UShort)( ( bsize->y_ppem + 32 ) >> 6 );

    if ( FT_IS_SCALABLE( face ) )
    {
      metrics->x_scale = FT_DivFix( bsize->x_ppem,
                                    face->units_per_EM );
      metrics->y_scale = FT_DivFix( bsize->y_ppem,
                                    face->units_per_EM );

      ft_recompute_scaled_metrics( face, metrics );
    }
    else
    {
      metrics->x_scale     = 1L << 16;
      metrics->y_scale     = 1L << 16;
      metrics->ascender    = bsize->y_ppem;
      metrics->descender   = 0;
      metrics->height      = bsize->height << 6;
      metrics->max_advance = bsize->x_ppem;
    }

    FT_TRACE5(( "FT_Select_Metrics:\n" ));
    FT_TRACE5(( "  x scale: %d (%f)\n",
                metrics->x_scale, metrics->x_scale / 65536.0 ));
    FT_TRACE5(( "  y scale: %d (%f)\n",
                metrics->y_scale, metrics->y_scale / 65536.0 ));
    FT_TRACE5(( "  ascender: %f\n",    metrics->ascender / 64.0 ));
    FT_TRACE5(( "  descender: %f\n",   metrics->descender / 64.0 ));
    FT_TRACE5(( "  height: %f\n",      metrics->height / 64.0 ));
    FT_TRACE5(( "  max advance: %f\n", metrics->max_advance / 64.0 ));
    FT_TRACE5(( "  x ppem: %d\n",      metrics->x_ppem ));
    FT_TRACE5(( "  y ppem: %d\n",      metrics->y_ppem ));
  }


  FT_BASE_DEF( void )
  FT_Request_Metrics( FT_Face          face,
                      FT_Size_Request  req )
  {
    FT_Size_Metrics*  metrics;


    metrics = &face->size->metrics;

    if ( FT_IS_SCALABLE( face ) )
    {
      FT_Long  w = 0, h = 0, scaled_w = 0, scaled_h = 0;


      switch ( req->type )
      {
      case FT_SIZE_REQUEST_TYPE_NOMINAL:
        w = h = face->units_per_EM;
        break;

      case FT_SIZE_REQUEST_TYPE_REAL_DIM:
        w = h = face->ascender - face->descender;
        break;

      case FT_SIZE_REQUEST_TYPE_BBOX:
        w = face->bbox.xMax - face->bbox.xMin;
        h = face->bbox.yMax - face->bbox.yMin;
        break;

      case FT_SIZE_REQUEST_TYPE_CELL:
        w = face->max_advance_width;
        h = face->ascender - face->descender;
        break;

      case FT_SIZE_REQUEST_TYPE_SCALES:
        metrics->x_scale = (FT_Fixed)req->width;
        metrics->y_scale = (FT_Fixed)req->height;
        if ( !metrics->x_scale )
          metrics->x_scale = metrics->y_scale;
        else if ( !metrics->y_scale )
          metrics->y_scale = metrics->x_scale;
        goto Calculate_Ppem;

      case FT_SIZE_REQUEST_TYPE_MAX:
        break;
      }

      /* to be on the safe side */
      if ( w < 0 )
        w = -w;

      if ( h < 0 )
        h = -h;

      scaled_w = FT_REQUEST_WIDTH ( req );
      scaled_h = FT_REQUEST_HEIGHT( req );

      /* determine scales */
      if ( req->width )
      {
        metrics->x_scale = FT_DivFix( scaled_w, w );

        if ( req->height )
        {
          metrics->y_scale = FT_DivFix( scaled_h, h );

          if ( req->type == FT_SIZE_REQUEST_TYPE_CELL )
          {
            if ( metrics->y_scale > metrics->x_scale )
              metrics->y_scale = metrics->x_scale;
            else
              metrics->x_scale = metrics->y_scale;
          }
        }
        else
        {
          metrics->y_scale = metrics->x_scale;
          scaled_h = FT_MulDiv( scaled_w, h, w );
        }
      }
      else
      {
        metrics->x_scale = metrics->y_scale = FT_DivFix( scaled_h, h );
        scaled_w = FT_MulDiv( scaled_h, w, h );
      }

  Calculate_Ppem:
      /* calculate the ppems */
      if ( req->type != FT_SIZE_REQUEST_TYPE_NOMINAL )
      {
        scaled_w = FT_MulFix( face->units_per_EM, metrics->x_scale );
        scaled_h = FT_MulFix( face->units_per_EM, metrics->y_scale );
      }

      metrics->x_ppem = (FT_UShort)( ( scaled_w + 32 ) >> 6 );
      metrics->y_ppem = (FT_UShort)( ( scaled_h + 32 ) >> 6 );

      ft_recompute_scaled_metrics( face, metrics );
    }
    else
    {
      FT_ZERO( metrics );
      metrics->x_scale = 1L << 16;
      metrics->y_scale = 1L << 16;
    }

    FT_TRACE5(( "FT_Request_Metrics:\n" ));
    FT_TRACE5(( "  x scale: %d (%f)\n",
                metrics->x_scale, metrics->x_scale / 65536.0 ));
    FT_TRACE5(( "  y scale: %d (%f)\n",
                metrics->y_scale, metrics->y_scale / 65536.0 ));
    FT_TRACE5(( "  ascender: %f\n",    metrics->ascender / 64.0 ));
    FT_TRACE5(( "  descender: %f\n",   metrics->descender / 64.0 ));
    FT_TRACE5(( "  height: %f\n",      metrics->height / 64.0 ));
    FT_TRACE5(( "  max advance: %f\n", metrics->max_advance / 64.0 ));
    FT_TRACE5(( "  x ppem: %d\n",      metrics->x_ppem ));
    FT_TRACE5(( "  y ppem: %d\n",      metrics->y_ppem ));
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Select_Size( FT_Face  face,
                  FT_Int   strike_index )
  {
    FT_Driver_Class  clazz;


    if ( !face || !FT_HAS_FIXED_SIZES( face ) )
      return FT_THROW( Invalid_Face_Handle );

    if ( strike_index < 0 || strike_index >= face->num_fixed_sizes )
      return FT_THROW( Invalid_Argument );

    clazz = face->driver->clazz;

    if ( clazz->select_size )
    {
      FT_Error  error;


      error = clazz->select_size( face->size, (FT_ULong)strike_index );

#ifdef FT_DEBUG_LEVEL_TRACE
      {
        FT_Size_Metrics*  metrics = &face->size->metrics;


        FT_TRACE5(( "FT_Select_Size (font driver's `select_size'):\n" ));
        FT_TRACE5(( "  x scale: %d (%f)\n",
                    metrics->x_scale, metrics->x_scale / 65536.0 ));
        FT_TRACE5(( "  y scale: %d (%f)\n",
                    metrics->y_scale, metrics->y_scale / 65536.0 ));
        FT_TRACE5(( "  ascender: %f\n",    metrics->ascender / 64.0 ));
        FT_TRACE5(( "  descender: %f\n",   metrics->descender / 64.0 ));
        FT_TRACE5(( "  height: %f\n",      metrics->height / 64.0 ));
        FT_TRACE5(( "  max advance: %f\n", metrics->max_advance / 64.0 ));
        FT_TRACE5(( "  x ppem: %d\n",      metrics->x_ppem ));
        FT_TRACE5(( "  y ppem: %d\n",      metrics->y_ppem ));
      }
#endif

      return error;
    }

    FT_Select_Metrics( face, (FT_ULong)strike_index );

    return FT_Err_Ok;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Request_Size( FT_Face          face,
                   FT_Size_Request  req )
  {
    FT_Driver_Class  clazz;
    FT_ULong         strike_index;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( !req || req->width < 0 || req->height < 0 ||
         req->type >= FT_SIZE_REQUEST_TYPE_MAX )
      return FT_THROW( Invalid_Argument );

    clazz = face->driver->clazz;

    if ( clazz->request_size )
    {
      FT_Error  error;


      error = clazz->request_size( face->size, req );

#ifdef FT_DEBUG_LEVEL_TRACE
      {
        FT_Size_Metrics*  metrics = &face->size->metrics;


        FT_TRACE5(( "FT_Request_Size (font driver's `request_size'):\n" ));
        FT_TRACE5(( "  x scale: %d (%f)\n",
                    metrics->x_scale, metrics->x_scale / 65536.0 ));
        FT_TRACE5(( "  y scale: %d (%f)\n",
                    metrics->y_scale, metrics->y_scale / 65536.0 ));
        FT_TRACE5(( "  ascender: %f\n",    metrics->ascender / 64.0 ));
        FT_TRACE5(( "  descender: %f\n",   metrics->descender / 64.0 ));
        FT_TRACE5(( "  height: %f\n",      metrics->height / 64.0 ));
        FT_TRACE5(( "  max advance: %f\n", metrics->max_advance / 64.0 ));
        FT_TRACE5(( "  x ppem: %d\n",      metrics->x_ppem ));
        FT_TRACE5(( "  y ppem: %d\n",      metrics->y_ppem ));
      }
#endif

      return error;
    }

    /*
     * The reason that a driver doesn't have `request_size' defined is
     * either that the scaling here suffices or that the supported formats
     * are bitmap-only and size matching is not implemented.
     *
     * In the latter case, a simple size matching is done.
     */
    if ( !FT_IS_SCALABLE( face ) && FT_HAS_FIXED_SIZES( face ) )
    {
      FT_Error  error;


      error = FT_Match_Size( face, req, 0, &strike_index );
      if ( error )
        return error;

      return FT_Select_Size( face, (FT_Int)strike_index );
    }

    FT_Request_Metrics( face, req );

    return FT_Err_Ok;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Set_Char_Size( FT_Face     face,
                    FT_F26Dot6  char_width,
                    FT_F26Dot6  char_height,
                    FT_UInt     horz_resolution,
                    FT_UInt     vert_resolution )
  {
    FT_Size_RequestRec  req;


    if ( !char_width )
      char_width = char_height;
    else if ( !char_height )
      char_height = char_width;

    if ( !horz_resolution )
      horz_resolution = vert_resolution;
    else if ( !vert_resolution )
      vert_resolution = horz_resolution;

    if ( char_width  < 1 * 64 )
      char_width  = 1 * 64;
    if ( char_height < 1 * 64 )
      char_height = 1 * 64;

    if ( !horz_resolution )
      horz_resolution = vert_resolution = 72;

    req.type           = FT_SIZE_REQUEST_TYPE_NOMINAL;
    req.width          = char_width;
    req.height         = char_height;
    req.horiResolution = horz_resolution;
    req.vertResolution = vert_resolution;

    return FT_Request_Size( face, &req );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Set_Pixel_Sizes( FT_Face  face,
                      FT_UInt  pixel_width,
                      FT_UInt  pixel_height )
  {
    FT_Size_RequestRec  req;


    if ( pixel_width == 0 )
      pixel_width = pixel_height;
    else if ( pixel_height == 0 )
      pixel_height = pixel_width;

    if ( pixel_width  < 1 )
      pixel_width  = 1;
    if ( pixel_height < 1 )
      pixel_height = 1;

    /* use `>=' to avoid potential compiler warning on 16bit platforms */
    if ( pixel_width  >= 0xFFFFU )
      pixel_width  = 0xFFFFU;
    if ( pixel_height >= 0xFFFFU )
      pixel_height = 0xFFFFU;

    req.type           = FT_SIZE_REQUEST_TYPE_NOMINAL;
    req.width          = pixel_width << 6;
    req.height         = pixel_height << 6;
    req.horiResolution = 0;
    req.vertResolution = 0;

    return FT_Request_Size( face, &req );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_Kerning( FT_Face     face,
                  FT_UInt     left_glyph,
                  FT_UInt     right_glyph,
                  FT_UInt     kern_mode,
                  FT_Vector  *akerning )
  {
    FT_Error   error = FT_Err_Ok;
    FT_Driver  driver;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( !akerning )
      return FT_THROW( Invalid_Argument );

    driver = face->driver;

    akerning->x = 0;
    akerning->y = 0;

    if ( driver->clazz->get_kerning )
    {
      error = driver->clazz->get_kerning( face,
                                          left_glyph,
                                          right_glyph,
                                          akerning );
      if ( !error )
      {
        if ( kern_mode != FT_KERNING_UNSCALED )
        {
          akerning->x = FT_MulFix( akerning->x, face->size->metrics.x_scale );
          akerning->y = FT_MulFix( akerning->y, face->size->metrics.y_scale );

          if ( kern_mode != FT_KERNING_UNFITTED )
          {
            /* we scale down kerning values for small ppem values */
            /* to avoid that rounding makes them too big.         */
            /* `25' has been determined heuristically.            */
            if ( face->size->metrics.x_ppem < 25 )
              akerning->x = FT_MulDiv( akerning->x,
                                       face->size->metrics.x_ppem, 25 );
            if ( face->size->metrics.y_ppem < 25 )
              akerning->y = FT_MulDiv( akerning->y,
                                       face->size->metrics.y_ppem, 25 );

            akerning->x = FT_PIX_ROUND( akerning->x );
            akerning->y = FT_PIX_ROUND( akerning->y );
          }
        }
      }
    }

    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_Track_Kerning( FT_Face    face,
                        FT_Fixed   point_size,
                        FT_Int     degree,
                        FT_Fixed*  akerning )
  {
    FT_Service_Kerning  service;
    FT_Error            error = FT_Err_Ok;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( !akerning )
      return FT_THROW( Invalid_Argument );

    FT_FACE_FIND_SERVICE( face, service, KERNING );
    if ( !service )
      return FT_THROW( Unimplemented_Feature );

    error = service->get_track( face,
                                point_size,
                                degree,
                                akerning );

    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Select_Charmap( FT_Face      face,
                     FT_Encoding  encoding )
  {
    FT_CharMap*  cur;
    FT_CharMap*  limit;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( encoding == FT_ENCODING_NONE )
      return FT_THROW( Invalid_Argument );

    /* FT_ENCODING_UNICODE is special.  We try to find the `best' Unicode */
    /* charmap available, i.e., one with UCS-4 characters, if possible.   */
    /*                                                                    */
    /* This is done by find_unicode_charmap() above, to share code.       */
    if ( encoding == FT_ENCODING_UNICODE )
      return find_unicode_charmap( face );

    cur = face->charmaps;
    if ( !cur )
      return FT_THROW( Invalid_CharMap_Handle );

    limit = cur + face->num_charmaps;

    for ( ; cur < limit; cur++ )
    {
      if ( cur[0]->encoding == encoding )
      {
#ifdef FT_MAX_CHARMAP_CACHEABLE
        if ( cur - face->charmaps > FT_MAX_CHARMAP_CACHEABLE )
        {
          FT_ERROR(( "FT_Select_Charmap: requested charmap is found (%d), "
                     "but in too late position to cache\n",
                     cur - face->charmaps ));
          continue;
        }
#endif
        face->charmap = cur[0];
        return 0;
      }
    }

    return FT_THROW( Invalid_Argument );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Set_Charmap( FT_Face     face,
                  FT_CharMap  charmap )
  {
    FT_CharMap*  cur;
    FT_CharMap*  limit;


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    cur = face->charmaps;
    if ( !cur )
      return FT_THROW( Invalid_CharMap_Handle );
    if ( FT_Get_CMap_Format( charmap ) == 14 )
      return FT_THROW( Invalid_Argument );

    limit = cur + face->num_charmaps;

    for ( ; cur < limit; cur++ )
    {
      if ( cur[0] == charmap )
      {
#ifdef FT_MAX_CHARMAP_CACHEABLE
        if ( cur - face->charmaps > FT_MAX_CHARMAP_CACHEABLE )
        {
          FT_ERROR(( "FT_Set_Charmap: requested charmap is found (%d), "
                     "but in too late position to cache\n",
                     cur - face->charmaps ));
          continue;
        }
#endif
        face->charmap = cur[0];
        return 0;
      }
    }
    return FT_THROW( Invalid_Argument );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Int )
  FT_Get_Charmap_Index( FT_CharMap  charmap )
  {
    FT_Int  i;


    if ( !charmap || !charmap->face )
      return -1;

    for ( i = 0; i < charmap->face->num_charmaps; i++ )
      if ( charmap->face->charmaps[i] == charmap )
        break;

    FT_ASSERT( i < charmap->face->num_charmaps );

#ifdef FT_MAX_CHARMAP_CACHEABLE
    if ( i > FT_MAX_CHARMAP_CACHEABLE )
    {
      FT_ERROR(( "FT_Get_Charmap_Index: requested charmap is found (%d), "
                 "but in too late position to cache\n",
                 i ));
      return -i;
    }
#endif
    return i;
  }


  static void
  ft_cmap_done_internal( FT_CMap  cmap )
  {
    FT_CMap_Class  clazz  = cmap->clazz;
    FT_Face        face   = cmap->charmap.face;
    FT_Memory      memory = FT_FACE_MEMORY( face );


    if ( clazz->done )
      clazz->done( cmap );

    FT_FREE( cmap );
  }


  FT_BASE_DEF( void )
  FT_CMap_Done( FT_CMap  cmap )
  {
    if ( cmap )
    {
      FT_Face    face   = cmap->charmap.face;
      FT_Memory  memory = FT_FACE_MEMORY( face );
      FT_Error   error;
      FT_Int     i, j;


      for ( i = 0; i < face->num_charmaps; i++ )
      {
        if ( (FT_CMap)face->charmaps[i] == cmap )
        {
          FT_CharMap  last_charmap = face->charmaps[face->num_charmaps - 1];


          if ( FT_RENEW_ARRAY( face->charmaps,
                               face->num_charmaps,
                               face->num_charmaps - 1 ) )
            return;

          /* remove it from our list of charmaps */
          for ( j = i + 1; j < face->num_charmaps; j++ )
          {
            if ( j == face->num_charmaps - 1 )
              face->charmaps[j - 1] = last_charmap;
            else
              face->charmaps[j - 1] = face->charmaps[j];
          }

          face->num_charmaps--;

          if ( (FT_CMap)face->charmap == cmap )
            face->charmap = NULL;

          ft_cmap_done_internal( cmap );

          break;
        }
      }
    }
  }


  FT_BASE_DEF( FT_Error )
  FT_CMap_New( FT_CMap_Class  clazz,
               FT_Pointer     init_data,
               FT_CharMap     charmap,
               FT_CMap       *acmap )
  {
    FT_Error   error = FT_Err_Ok;
    FT_Face    face;
    FT_Memory  memory;
    FT_CMap    cmap = NULL;


    if ( clazz == NULL || charmap == NULL || charmap->face == NULL )
      return FT_THROW( Invalid_Argument );

    face   = charmap->face;
    memory = FT_FACE_MEMORY( face );

    if ( !FT_ALLOC( cmap, clazz->size ) )
    {
      cmap->charmap = *charmap;
      cmap->clazz   = clazz;

      if ( clazz->init )
      {
        error = clazz->init( cmap, init_data );
        if ( error )
          goto Fail;
      }

      /* add it to our list of charmaps */
      if ( FT_RENEW_ARRAY( face->charmaps,
                           face->num_charmaps,
                           face->num_charmaps + 1 ) )
        goto Fail;

      face->charmaps[face->num_charmaps++] = (FT_CharMap)cmap;
    }

  Exit:
    if ( acmap )
      *acmap = cmap;

    return error;

  Fail:
    ft_cmap_done_internal( cmap );
    cmap = NULL;
    goto Exit;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_UInt )
  FT_Get_Char_Index( FT_Face   face,
                     FT_ULong  charcode )
  {
    FT_UInt  result = 0;


    if ( face && face->charmap )
    {
      FT_CMap  cmap = FT_CMAP( face->charmap );


      if ( charcode > 0xFFFFFFFFUL )
      {
        FT_TRACE1(( "FT_Get_Char_Index: too large charcode" ));
        FT_TRACE1(( " 0x%x is truncated\n", charcode ));
      }
      result = cmap->clazz->char_index( cmap, (FT_UInt32)charcode );
    }
    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_ULong )
  FT_Get_First_Char( FT_Face   face,
                     FT_UInt  *agindex )
  {
    FT_ULong  result = 0;
    FT_UInt   gindex = 0;


    /* only do something if we have a charmap, and we have glyphs at all */
    if ( face && face->charmap && face->num_glyphs )
    {
      gindex = FT_Get_Char_Index( face, 0 );
      if ( gindex == 0 || gindex >= (FT_UInt)face->num_glyphs )
        result = FT_Get_Next_Char( face, 0, &gindex );
    }

    if ( agindex )
      *agindex = gindex;

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_ULong )
  FT_Get_Next_Char( FT_Face   face,
                    FT_ULong  charcode,
                    FT_UInt  *agindex )
  {
    FT_ULong  result = 0;
    FT_UInt   gindex = 0;


    if ( face && face->charmap && face->num_glyphs )
    {
      FT_UInt32  code = (FT_UInt32)charcode;
      FT_CMap    cmap = FT_CMAP( face->charmap );


      do
      {
        gindex = cmap->clazz->char_next( cmap, &code );

      } while ( gindex >= (FT_UInt)face->num_glyphs );

      result = ( gindex == 0 ) ? 0 : code;
    }

    if ( agindex )
      *agindex = gindex;

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_UInt )
  FT_Face_GetCharVariantIndex( FT_Face   face,
                               FT_ULong  charcode,
                               FT_ULong  variantSelector )
  {
    FT_UInt  result = 0;


    if ( face && face->charmap &&
        face->charmap->encoding == FT_ENCODING_UNICODE )
    {
      FT_CharMap  charmap = find_variant_selector_charmap( face );
      FT_CMap     ucmap = FT_CMAP( face->charmap );


      if ( charmap != NULL )
      {
        FT_CMap  vcmap = FT_CMAP( charmap );


        if ( charcode > 0xFFFFFFFFUL )
        {
          FT_TRACE1(( "FT_Get_Char_Index: too large charcode" ));
          FT_TRACE1(( " 0x%x is truncated\n", charcode ));
        }
        if ( variantSelector > 0xFFFFFFFFUL )
        {
          FT_TRACE1(( "FT_Get_Char_Index: too large variantSelector" ));
          FT_TRACE1(( " 0x%x is truncated\n", variantSelector ));
        }

        result = vcmap->clazz->char_var_index( vcmap, ucmap,
                                               (FT_UInt32)charcode,
                                               (FT_UInt32)variantSelector );
      }
    }

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Int )
  FT_Face_GetCharVariantIsDefault( FT_Face   face,
                                   FT_ULong  charcode,
                                   FT_ULong  variantSelector )
  {
    FT_Int  result = -1;


    if ( face )
    {
      FT_CharMap  charmap = find_variant_selector_charmap( face );


      if ( charmap != NULL )
      {
        FT_CMap  vcmap = FT_CMAP( charmap );


        if ( charcode > 0xFFFFFFFFUL )
        {
          FT_TRACE1(( "FT_Get_Char_Index: too large charcode" ));
          FT_TRACE1(( " 0x%x is truncated\n", charcode ));
        }
        if ( variantSelector > 0xFFFFFFFFUL )
        {
          FT_TRACE1(( "FT_Get_Char_Index: too large variantSelector" ));
          FT_TRACE1(( " 0x%x is truncated\n", variantSelector ));
        }

        result = vcmap->clazz->char_var_default( vcmap,
                                                 (FT_UInt32)charcode,
                                                 (FT_UInt32)variantSelector );
      }
    }

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_UInt32* )
  FT_Face_GetVariantSelectors( FT_Face  face )
  {
    FT_UInt32  *result = NULL;


    if ( face )
    {
      FT_CharMap  charmap = find_variant_selector_charmap( face );


      if ( charmap != NULL )
      {
        FT_CMap    vcmap  = FT_CMAP( charmap );
        FT_Memory  memory = FT_FACE_MEMORY( face );


        result = vcmap->clazz->variant_list( vcmap, memory );
      }
    }

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_UInt32* )
  FT_Face_GetVariantsOfChar( FT_Face   face,
                             FT_ULong  charcode )
  {
    FT_UInt32  *result = NULL;


    if ( face )
    {
      FT_CharMap  charmap = find_variant_selector_charmap( face );


      if ( charmap != NULL )
      {
        FT_CMap    vcmap  = FT_CMAP( charmap );
        FT_Memory  memory = FT_FACE_MEMORY( face );


        if ( charcode > 0xFFFFFFFFUL )
        {
          FT_TRACE1(( "FT_Get_Char_Index: too large charcode" ));
          FT_TRACE1(( " 0x%x is truncated\n", charcode ));
        }

        result = vcmap->clazz->charvariant_list( vcmap, memory,
                                                 (FT_UInt32)charcode );
      }
    }
    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_UInt32* )
  FT_Face_GetCharsOfVariant( FT_Face   face,
                             FT_ULong  variantSelector )
  {
    FT_UInt32  *result = NULL;


    if ( face )
    {
      FT_CharMap  charmap = find_variant_selector_charmap( face );


      if ( charmap != NULL )
      {
        FT_CMap    vcmap  = FT_CMAP( charmap );
        FT_Memory  memory = FT_FACE_MEMORY( face );


        if ( variantSelector > 0xFFFFFFFFUL )
        {
          FT_TRACE1(( "FT_Get_Char_Index: too large variantSelector" ));
          FT_TRACE1(( " 0x%x is truncated\n", variantSelector ));
        }

        result = vcmap->clazz->variantchar_list( vcmap, memory,
                                                 (FT_UInt32)variantSelector );
      }
    }

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_UInt )
  FT_Get_Name_Index( FT_Face     face,
                     FT_String*  glyph_name )
  {
    FT_UInt  result = 0;


    if ( face && FT_HAS_GLYPH_NAMES( face ) )
    {
      FT_Service_GlyphDict  service;


      FT_FACE_LOOKUP_SERVICE( face,
                              service,
                              GLYPH_DICT );

      if ( service && service->name_index )
        result = service->name_index( face, glyph_name );
    }

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_Glyph_Name( FT_Face     face,
                     FT_UInt     glyph_index,
                     FT_Pointer  buffer,
                     FT_UInt     buffer_max )
  {
    FT_Error  error = FT_ERR( Invalid_Argument );


    /* clean up buffer */
    if ( buffer && buffer_max > 0 )
      ((FT_Byte*)buffer)[0] = 0;

    if ( face                                     &&
         (FT_Long)glyph_index <= face->num_glyphs &&
         FT_HAS_GLYPH_NAMES( face )               )
    {
      FT_Service_GlyphDict  service;


      FT_FACE_LOOKUP_SERVICE( face,
                              service,
                              GLYPH_DICT );

      if ( service && service->get_name )
        error = service->get_name( face, glyph_index, buffer, buffer_max );
    }

    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( const char* )
  FT_Get_Postscript_Name( FT_Face  face )
  {
    const char*  result = NULL;


    if ( !face )
      goto Exit;

    if ( !result )
    {
      FT_Service_PsFontName  service;


      FT_FACE_LOOKUP_SERVICE( face,
                              service,
                              POSTSCRIPT_FONT_NAME );

      if ( service && service->get_ps_font_name )
        result = service->get_ps_font_name( face );
    }

  Exit:
    return result;
  }


  /* documentation is in tttables.h */

  FT_EXPORT_DEF( void* )
  FT_Get_Sfnt_Table( FT_Face      face,
                     FT_Sfnt_Tag  tag )
  {
    void*                  table = 0;
    FT_Service_SFNT_Table  service;


    if ( face && FT_IS_SFNT( face ) )
    {
      FT_FACE_FIND_SERVICE( face, service, SFNT_TABLE );
      if ( service != NULL )
        table = service->get_table( face, tag );
    }

    return table;
  }


  /* documentation is in tttables.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Load_Sfnt_Table( FT_Face    face,
                      FT_ULong   tag,
                      FT_Long    offset,
                      FT_Byte*   buffer,
                      FT_ULong*  length )
  {
    FT_Service_SFNT_Table  service;


    if ( !face || !FT_IS_SFNT( face ) )
      return FT_THROW( Invalid_Face_Handle );

    FT_FACE_FIND_SERVICE( face, service, SFNT_TABLE );
    if ( service == NULL )
      return FT_THROW( Unimplemented_Feature );

    return service->load_table( face, tag, offset, buffer, length );
  }


  /* documentation is in tttables.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Sfnt_Table_Info( FT_Face    face,
                      FT_UInt    table_index,
                      FT_ULong  *tag,
                      FT_ULong  *length )
  {
    FT_Service_SFNT_Table  service;
    FT_ULong               offset;


    if ( !face || !FT_IS_SFNT( face ) )
      return FT_THROW( Invalid_Face_Handle );

    FT_FACE_FIND_SERVICE( face, service, SFNT_TABLE );
    if ( service == NULL )
      return FT_THROW( Unimplemented_Feature );

    return service->table_info( face, table_index, tag, &offset, length );
  }


  /* documentation is in tttables.h */

  FT_EXPORT_DEF( FT_ULong )
  FT_Get_CMap_Language_ID( FT_CharMap  charmap )
  {
    FT_Service_TTCMaps  service;
    FT_Face             face;
    TT_CMapInfo         cmap_info;


    if ( !charmap || !charmap->face )
      return 0;

    face = charmap->face;
    FT_FACE_FIND_SERVICE( face, service, TT_CMAP );
    if ( service == NULL )
      return 0;
    if ( service->get_cmap_info( charmap, &cmap_info ))
      return 0;

    return cmap_info.language;
  }


  /* documentation is in tttables.h */

  FT_EXPORT_DEF( FT_Long )
  FT_Get_CMap_Format( FT_CharMap  charmap )
  {
    FT_Service_TTCMaps  service;
    FT_Face             face;
    TT_CMapInfo         cmap_info;


    if ( !charmap || !charmap->face )
      return -1;

    face = charmap->face;
    FT_FACE_FIND_SERVICE( face, service, TT_CMAP );
    if ( service == NULL )
      return -1;
    if ( service->get_cmap_info( charmap, &cmap_info ))
      return -1;

    return cmap_info.format;
  }


  /* documentation is in ftsizes.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Activate_Size( FT_Size  size )
  {
    FT_Face  face;


    if ( size == NULL )
      return FT_THROW( Invalid_Argument );

    face = size->face;
    if ( face == NULL || face->driver == NULL )
      return FT_THROW( Invalid_Argument );

    /* we don't need anything more complex than that; all size objects */
    /* are already listed by the face                                  */
    face->size = size;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                        R E N D E R E R S                        ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

  /* lookup a renderer by glyph format in the library's list */
  FT_BASE_DEF( FT_Renderer )
  FT_Lookup_Renderer( FT_Library       library,
                      FT_Glyph_Format  format,
                      FT_ListNode*     node )
  {
    FT_ListNode  cur;
    FT_Renderer  result = 0;


    if ( !library )
      goto Exit;

    cur = library->renderers.head;

    if ( node )
    {
      if ( *node )
        cur = (*node)->next;
      *node = 0;
    }

    while ( cur )
    {
      FT_Renderer  renderer = FT_RENDERER( cur->data );


      if ( renderer->glyph_format == format )
      {
        if ( node )
          *node = cur;

        result = renderer;
        break;
      }
      cur = cur->next;
    }

  Exit:
    return result;
  }


  static FT_Renderer
  ft_lookup_glyph_renderer( FT_GlyphSlot  slot )
  {
    FT_Face      face    = slot->face;
    FT_Library   library = FT_FACE_LIBRARY( face );
    FT_Renderer  result  = library->cur_renderer;


    if ( !result || result->glyph_format != slot->format )
      result = FT_Lookup_Renderer( library, slot->format, 0 );

    return result;
  }


  static void
  ft_set_current_renderer( FT_Library  library )
  {
    FT_Renderer  renderer;


    renderer = FT_Lookup_Renderer( library, FT_GLYPH_FORMAT_OUTLINE, 0 );
    library->cur_renderer = renderer;
  }


  static FT_Error
  ft_add_renderer( FT_Module  module )
  {
    FT_Library   library = module->library;
    FT_Memory    memory  = library->memory;
    FT_Error     error;
    FT_ListNode  node    = NULL;


    if ( FT_NEW( node ) )
      goto Exit;

    {
      FT_Renderer         render = FT_RENDERER( module );
      FT_Renderer_Class*  clazz  = (FT_Renderer_Class*)module->clazz;


      render->clazz        = clazz;
      render->glyph_format = clazz->glyph_format;

      /* allocate raster object if needed */
      if ( clazz->glyph_format == FT_GLYPH_FORMAT_OUTLINE &&
           clazz->raster_class->raster_new                )
      {
        error = clazz->raster_class->raster_new( memory, &render->raster );
        if ( error )
          goto Fail;

        render->raster_render = clazz->raster_class->raster_render;
        render->render        = clazz->render_glyph;
      }

      /* add to list */
      node->data = module;
      FT_List_Add( &library->renderers, node );

      ft_set_current_renderer( library );
    }

  Fail:
    if ( error )
      FT_FREE( node );

  Exit:
    return error;
  }


  static void
  ft_remove_renderer( FT_Module  module )
  {
    FT_Library   library;
    FT_Memory    memory;
    FT_ListNode  node;


    library = module->library;
    if ( !library )
      return;

    memory = library->memory;

    node = FT_List_Find( &library->renderers, module );
    if ( node )
    {
      FT_Renderer  render = FT_RENDERER( module );


      /* release raster object, if any */
      if ( render->clazz->glyph_format == FT_GLYPH_FORMAT_OUTLINE &&
           render->raster                                         )
        render->clazz->raster_class->raster_done( render->raster );

      /* remove from list */
      FT_List_Remove( &library->renderers, node );
      FT_FREE( node );

      ft_set_current_renderer( library );
    }
  }


  /* documentation is in ftrender.h */

  FT_EXPORT_DEF( FT_Renderer )
  FT_Get_Renderer( FT_Library       library,
                   FT_Glyph_Format  format )
  {
    /* test for valid `library' delayed to FT_Lookup_Renderer() */

    return FT_Lookup_Renderer( library, format, 0 );
  }


  /* documentation is in ftrender.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Set_Renderer( FT_Library     library,
                   FT_Renderer    renderer,
                   FT_UInt        num_params,
                   FT_Parameter*  parameters )
  {
    FT_ListNode  node;
    FT_Error     error = FT_Err_Ok;


    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( !renderer )
      return FT_THROW( Invalid_Argument );

    node = FT_List_Find( &library->renderers, renderer );
    if ( !node )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    FT_List_Up( &library->renderers, node );

    if ( renderer->glyph_format == FT_GLYPH_FORMAT_OUTLINE )
      library->cur_renderer = renderer;

    if ( num_params > 0 )
    {
      FT_Renderer_SetModeFunc  set_mode = renderer->clazz->set_mode;


      for ( ; num_params > 0; num_params-- )
      {
        error = set_mode( renderer, parameters->tag, parameters->data );
        if ( error )
          break;
        parameters++;
      }
    }

  Exit:
    return error;
  }


  FT_BASE_DEF( FT_Error )
  FT_Render_Glyph_Internal( FT_Library      library,
                            FT_GlyphSlot    slot,
                            FT_Render_Mode  render_mode )
  {
    FT_Error     error = FT_Err_Ok;
    FT_Renderer  renderer;


    /* if it is already a bitmap, no need to do anything */
    switch ( slot->format )
    {
    case FT_GLYPH_FORMAT_BITMAP:   /* already a bitmap, don't do anything */
      break;

    default:
      {
        FT_ListNode  node   = 0;
        FT_Bool      update = 0;


        /* small shortcut for the very common case */
        if ( slot->format == FT_GLYPH_FORMAT_OUTLINE )
        {
          renderer = library->cur_renderer;
          node     = library->renderers.head;
        }
        else
          renderer = FT_Lookup_Renderer( library, slot->format, &node );

        error = FT_ERR( Unimplemented_Feature );
        while ( renderer )
        {
          error = renderer->render( renderer, slot, render_mode, NULL );
          if ( !error                                   ||
               FT_ERR_NEQ( error, Cannot_Render_Glyph ) )
            break;

          /* FT_Err_Cannot_Render_Glyph is returned if the render mode   */
          /* is unsupported by the current renderer for this glyph image */
          /* format.                                                     */

          /* now, look for another renderer that supports the same */
          /* format.                                               */
          renderer = FT_Lookup_Renderer( library, slot->format, &node );
          update   = 1;
        }

        /* if we changed the current renderer for the glyph image format */
        /* we need to select it as the next current one                  */
        if ( !error && update && renderer )
          FT_Set_Renderer( library, renderer, 0, 0 );
      }
    }

#ifdef FT_DEBUG_LEVEL_TRACE

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_bitmap

    /* we convert to a single bitmap format for computing the checksum */
    {
      FT_Bitmap  bitmap;
      FT_Error   err;


      FT_Bitmap_New( &bitmap );

      err = FT_Bitmap_Convert( library, &slot->bitmap, &bitmap, 1 );
      if ( !err )
      {
        MD5_CTX        ctx;
        unsigned char  md5[16];
        int            i;


        MD5_Init( &ctx);
        MD5_Update( &ctx, bitmap.buffer, bitmap.rows * bitmap.pitch );
        MD5_Final( md5, &ctx );

        FT_TRACE3(( "MD5 checksum for %dx%d bitmap:\n"
                    "  ",
                    bitmap.rows, bitmap.pitch ));
        for ( i = 0; i < 16; i++ )
          FT_TRACE3(( "%02X", md5[i] ));
        FT_TRACE3(( "\n" ));
      }

      FT_Bitmap_Done( library, &bitmap );
    }

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_objs

#endif /* FT_DEBUG_LEVEL_TRACE */

    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Render_Glyph( FT_GlyphSlot    slot,
                   FT_Render_Mode  render_mode )
  {
    FT_Library  library;


    if ( !slot || !slot->face )
      return FT_THROW( Invalid_Argument );

    library = FT_FACE_LIBRARY( slot->face );

    return FT_Render_Glyph_Internal( library, slot, render_mode );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                         M O D U L E S                           ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Destroy_Module                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Destroys a given module object.  For drivers, this also destroys   */
  /*    all child faces.                                                   */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    module :: A handle to the target driver object.                    */
  /*                                                                       */
  /* <Note>                                                                */
  /*    The driver _must_ be LOCKED!                                       */
  /*                                                                       */
  static void
  Destroy_Module( FT_Module  module )
  {
    FT_Memory         memory  = module->memory;
    FT_Module_Class*  clazz   = module->clazz;
    FT_Library        library = module->library;


    if ( library && library->auto_hinter == module )
      library->auto_hinter = 0;

    /* if the module is a renderer */
    if ( FT_MODULE_IS_RENDERER( module ) )
      ft_remove_renderer( module );

    /* if the module is a font driver, add some steps */
    if ( FT_MODULE_IS_DRIVER( module ) )
      Destroy_Driver( FT_DRIVER( module ) );

    /* finalize the module object */
    if ( clazz->module_done )
      clazz->module_done( module );

    /* discard it */
    FT_FREE( module );
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Add_Module( FT_Library              library,
                 const FT_Module_Class*  clazz )
  {
    FT_Error   error;
    FT_Memory  memory;
    FT_Module  module;
    FT_UInt    nn;


#define FREETYPE_VER_FIXED  ( ( (FT_Long)FREETYPE_MAJOR << 16 ) | \
                                FREETYPE_MINOR                  )

    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( !clazz )
      return FT_THROW( Invalid_Argument );

    /* check freetype version */
    if ( clazz->module_requires > FREETYPE_VER_FIXED )
      return FT_THROW( Invalid_Version );

    /* look for a module with the same name in the library's table */
    for ( nn = 0; nn < library->num_modules; nn++ )
    {
      module = library->modules[nn];
      if ( ft_strcmp( module->clazz->module_name, clazz->module_name ) == 0 )
      {
        /* this installed module has the same name, compare their versions */
        if ( clazz->module_version <= module->clazz->module_version )
          return FT_THROW( Lower_Module_Version );

        /* remove the module from our list, then exit the loop to replace */
        /* it by our new version..                                        */
        FT_Remove_Module( library, module );
        break;
      }
    }

    memory = library->memory;
    error  = FT_Err_Ok;

    if ( library->num_modules >= FT_MAX_MODULES )
    {
      error = FT_THROW( Too_Many_Drivers );
      goto Exit;
    }

    /* allocate module object */
    if ( FT_ALLOC( module, clazz->module_size ) )
      goto Exit;

    /* base initialization */
    module->library = library;
    module->memory  = memory;
    module->clazz   = (FT_Module_Class*)clazz;

    /* check whether the module is a renderer - this must be performed */
    /* before the normal module initialization                         */
    if ( FT_MODULE_IS_RENDERER( module ) )
    {
      /* add to the renderers list */
      error = ft_add_renderer( module );
      if ( error )
        goto Fail;
    }

    /* is the module a auto-hinter? */
    if ( FT_MODULE_IS_HINTER( module ) )
      library->auto_hinter = module;

    /* if the module is a font driver */
    if ( FT_MODULE_IS_DRIVER( module ) )
    {
      /* allocate glyph loader if needed */
      FT_Driver  driver = FT_DRIVER( module );


      driver->clazz = (FT_Driver_Class)module->clazz;
      if ( FT_DRIVER_USES_OUTLINES( driver ) )
      {
        error = FT_GlyphLoader_New( memory, &driver->glyph_loader );
        if ( error )
          goto Fail;
      }
    }

    if ( clazz->module_init )
    {
      error = clazz->module_init( module );
      if ( error )
        goto Fail;
    }

    /* add module to the library's table */
    library->modules[library->num_modules++] = module;

  Exit:
    return error;

  Fail:
    if ( FT_MODULE_IS_DRIVER( module ) )
    {
      FT_Driver  driver = FT_DRIVER( module );


      if ( FT_DRIVER_USES_OUTLINES( driver ) )
        FT_GlyphLoader_Done( driver->glyph_loader );
    }

    if ( FT_MODULE_IS_RENDERER( module ) )
    {
      FT_Renderer  renderer = FT_RENDERER( module );


      if ( renderer->clazz                                          &&
           renderer->clazz->glyph_format == FT_GLYPH_FORMAT_OUTLINE &&
           renderer->raster                                         )
        renderer->clazz->raster_class->raster_done( renderer->raster );
    }

    FT_FREE( module );
    goto Exit;
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Module )
  FT_Get_Module( FT_Library   library,
                 const char*  module_name )
  {
    FT_Module   result = 0;
    FT_Module*  cur;
    FT_Module*  limit;


    if ( !library || !module_name )
      return result;

    cur   = library->modules;
    limit = cur + library->num_modules;

    for ( ; cur < limit; cur++ )
      if ( ft_strcmp( cur[0]->clazz->module_name, module_name ) == 0 )
      {
        result = cur[0];
        break;
      }

    return result;
  }


  /* documentation is in ftobjs.h */

  FT_BASE_DEF( const void* )
  FT_Get_Module_Interface( FT_Library   library,
                           const char*  mod_name )
  {
    FT_Module  module;


    /* test for valid `library' delayed to FT_Get_Module() */

    module = FT_Get_Module( library, mod_name );

    return module ? module->clazz->module_interface : 0;
  }


  FT_BASE_DEF( FT_Pointer )
  ft_module_get_service( FT_Module    module,
                         const char*  service_id )
  {
    FT_Pointer  result = NULL;


    if ( module )
    {
      FT_ASSERT( module->clazz && module->clazz->get_interface );

      /* first, look for the service in the module */
      if ( module->clazz->get_interface )
        result = module->clazz->get_interface( module, service_id );

      if ( result == NULL )
      {
        /* we didn't find it, look in all other modules then */
        FT_Library  library = module->library;
        FT_Module*  cur     = library->modules;
        FT_Module*  limit   = cur + library->num_modules;


        for ( ; cur < limit; cur++ )
        {
          if ( cur[0] != module )
          {
            FT_ASSERT( cur[0]->clazz );

            if ( cur[0]->clazz->get_interface )
            {
              result = cur[0]->clazz->get_interface( cur[0], service_id );
              if ( result != NULL )
                break;
            }
          }
        }
      }
    }

    return result;
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Remove_Module( FT_Library  library,
                    FT_Module   module )
  {
    /* try to find the module from the table, then remove it from there */

    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( module )
    {
      FT_Module*  cur   = library->modules;
      FT_Module*  limit = cur + library->num_modules;


      for ( ; cur < limit; cur++ )
      {
        if ( cur[0] == module )
        {
          /* remove it from the table */
          library->num_modules--;
          limit--;
          while ( cur < limit )
          {
            cur[0] = cur[1];
            cur++;
          }
          limit[0] = 0;

          /* destroy the module */
          Destroy_Module( module );

          return FT_Err_Ok;
        }
      }
    }
    return FT_THROW( Invalid_Driver_Handle );
  }


  static FT_Error
  ft_property_do( FT_Library        library,
                  const FT_String*  module_name,
                  const FT_String*  property_name,
                  void*             value,
                  FT_Bool           set )
  {
    FT_Module*           cur;
    FT_Module*           limit;
    FT_Module_Interface  interface;

    FT_Service_Properties  service;

#ifdef FT_DEBUG_LEVEL_ERROR
    const FT_String*  set_name  = "FT_Property_Set";
    const FT_String*  get_name  = "FT_Property_Get";
    const FT_String*  func_name = set ? set_name : get_name;
#endif

    FT_Bool  missing_func;


    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( !module_name || !property_name || !value )
      return FT_THROW( Invalid_Argument );

    cur   = library->modules;
    limit = cur + library->num_modules;

    /* search module */
    for ( ; cur < limit; cur++ )
      if ( !ft_strcmp( cur[0]->clazz->module_name, module_name ) )
        break;

    if ( cur == limit )
    {
      FT_ERROR(( "%s: can't find module `%s'\n",
                 func_name, module_name ));
      return FT_THROW( Missing_Module );
    }

    /* check whether we have a service interface */
    if ( !cur[0]->clazz->get_interface )
    {
      FT_ERROR(( "%s: module `%s' doesn't support properties\n",
                 func_name, module_name ));
      return FT_THROW( Unimplemented_Feature );
    }

    /* search property service */
    interface = cur[0]->clazz->get_interface( cur[0],
                                              FT_SERVICE_ID_PROPERTIES );
    if ( !interface )
    {
      FT_ERROR(( "%s: module `%s' doesn't support properties\n",
                 func_name, module_name ));
      return FT_THROW( Unimplemented_Feature );
    }

    service = (FT_Service_Properties)interface;

    if ( set )
      missing_func = (FT_Bool)( !service->set_property );
    else
      missing_func = (FT_Bool)( !service->get_property );

    if ( missing_func )
    {
      FT_ERROR(( "%s: property service of module `%s' is broken\n",
                 func_name, module_name ));
      return FT_THROW( Unimplemented_Feature );
    }

    return set ? service->set_property( cur[0], property_name, value )
               : service->get_property( cur[0], property_name, value );
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Property_Set( FT_Library        library,
                   const FT_String*  module_name,
                   const FT_String*  property_name,
                   const void*       value )
  {
    return ft_property_do( library,
                           module_name,
                           property_name,
                           (void*)value,
                           TRUE );
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Property_Get( FT_Library        library,
                   const FT_String*  module_name,
                   const FT_String*  property_name,
                   void*             value )
  {
    return ft_property_do( library,
                           module_name,
                           property_name,
                           value,
                           FALSE );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                         L I B R A R Y                           ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Reference_Library( FT_Library  library )
  {
    library->refcount++;

    return FT_Err_Ok;
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_New_Library( FT_Memory    memory,
                  FT_Library  *alibrary )
  {
    FT_Library  library = NULL;
    FT_Error    error;


    if ( !memory )
      return FT_THROW( Invalid_Argument );

#ifdef FT_DEBUG_LEVEL_ERROR
    /* init debugging support */
    ft_debug_init();
#endif

    /* first of all, allocate the library object */
    if ( FT_NEW( library ) )
      return error;

    library->memory = memory;

#ifdef FT_CONFIG_OPTION_PIC
    /* initialize position independent code containers */
    error = ft_pic_container_init( library );
    if ( error )
      goto Fail;
#endif

    /* allocate the render pool */
    library->raster_pool_size = FT_RENDER_POOL_SIZE;
#if FT_RENDER_POOL_SIZE > 0
    if ( FT_ALLOC( library->raster_pool, FT_RENDER_POOL_SIZE ) )
      goto Fail;
#endif

    library->version_major = FREETYPE_MAJOR;
    library->version_minor = FREETYPE_MINOR;
    library->version_patch = FREETYPE_PATCH;

    library->refcount = 1;

    /* That's ok now */
    *alibrary = library;

    return FT_Err_Ok;

  Fail:
#ifdef FT_CONFIG_OPTION_PIC
    ft_pic_container_destroy( library );
#endif
    FT_FREE( library );
    return error;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( void )
  FT_Library_Version( FT_Library   library,
                      FT_Int      *amajor,
                      FT_Int      *aminor,
                      FT_Int      *apatch )
  {
    FT_Int  major = 0;
    FT_Int  minor = 0;
    FT_Int  patch = 0;


    if ( library )
    {
      major = library->version_major;
      minor = library->version_minor;
      patch = library->version_patch;
    }

    if ( amajor )
      *amajor = major;

    if ( aminor )
      *aminor = minor;

    if ( apatch )
      *apatch = patch;
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Done_Library( FT_Library  library )
  {
    FT_Memory  memory;


    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    library->refcount--;
    if ( library->refcount > 0 )
      goto Exit;

    memory = library->memory;

    /*
     * Close all faces in the library.  If we don't do this, we can have
     * some subtle memory leaks.
     *
     * Example:
     *
     *  - the cff font driver uses the pshinter module in cff_size_done
     *  - if the pshinter module is destroyed before the cff font driver,
     *    opened FT_Face objects managed by the driver are not properly
     *    destroyed, resulting in a memory leak
     *
     * Some faces are dependent on other faces, like Type42 faces that
     * depend on TrueType faces synthesized internally.
     *
     * The order of drivers should be specified in driver_name[].
     */
    {
      FT_UInt      m, n;
      const char*  driver_name[] = { "type42", NULL };


      for ( m = 0;
            m < sizeof ( driver_name ) / sizeof ( driver_name[0] );
            m++ )
      {
        for ( n = 0; n < library->num_modules; n++ )
        {
          FT_Module    module      = library->modules[n];
          const char*  module_name = module->clazz->module_name;
          FT_List      faces;


          if ( driver_name[m]                                &&
               ft_strcmp( module_name, driver_name[m] ) != 0 )
            continue;

          if ( ( module->clazz->module_flags & FT_MODULE_FONT_DRIVER ) == 0 )
            continue;

          FT_TRACE7(( "FT_Done_Library: close faces for %s\n", module_name ));

          faces = &FT_DRIVER( module )->faces_list;
          while ( faces->head )
          {
            FT_Done_Face( FT_FACE( faces->head->data ) );
            if ( faces->head )
              FT_TRACE0(( "FT_Done_Library: failed to free some faces\n" ));
          }
        }
      }
    }

    /* Close all other modules in the library */
#if 1
    /* XXX Modules are removed in the reversed order so that  */
    /* type42 module is removed before truetype module.  This */
    /* avoids double free in some occasions.  It is a hack.   */
    while ( library->num_modules > 0 )
      FT_Remove_Module( library,
                        library->modules[library->num_modules - 1] );
#else
    {
      FT_UInt  n;


      for ( n = 0; n < library->num_modules; n++ )
      {
        FT_Module  module = library->modules[n];


        if ( module )
        {
          Destroy_Module( module );
          library->modules[n] = 0;
        }
      }
    }
#endif

    /* Destroy raster objects */
    FT_FREE( library->raster_pool );
    library->raster_pool_size = 0;

#ifdef FT_CONFIG_OPTION_PIC
    /* Destroy pic container contents */
    ft_pic_container_destroy( library );
#endif

    FT_FREE( library );

  Exit:
    return FT_Err_Ok;
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( void )
  FT_Set_Debug_Hook( FT_Library         library,
                     FT_UInt            hook_index,
                     FT_DebugHook_Func  debug_hook )
  {
    if ( library && debug_hook &&
         hook_index <
           ( sizeof ( library->debug_hooks ) / sizeof ( void* ) ) )
      library->debug_hooks[hook_index] = debug_hook;
  }


  /* documentation is in ftmodapi.h */

  FT_EXPORT_DEF( FT_TrueTypeEngineType )
  FT_Get_TrueType_Engine_Type( FT_Library  library )
  {
    FT_TrueTypeEngineType  result = FT_TRUETYPE_ENGINE_TYPE_NONE;


    if ( library )
    {
      FT_Module  module = FT_Get_Module( library, "truetype" );


      if ( module )
      {
        FT_Service_TrueTypeEngine  service;


        service = (FT_Service_TrueTypeEngine)
                    ft_module_get_service( module,
                                           FT_SERVICE_ID_TRUETYPE_ENGINE );
        if ( service )
          result = service->engine_type;
      }
    }

    return result;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_SubGlyph_Info( FT_GlyphSlot  glyph,
                        FT_UInt       sub_index,
                        FT_Int       *p_index,
                        FT_UInt      *p_flags,
                        FT_Int       *p_arg1,
                        FT_Int       *p_arg2,
                        FT_Matrix    *p_transform )
  {
    FT_Error  error = FT_ERR( Invalid_Argument );


    if ( glyph                                      &&
         glyph->subglyphs                           &&
         glyph->format == FT_GLYPH_FORMAT_COMPOSITE &&
         sub_index < glyph->num_subglyphs           )
    {
      FT_SubGlyph  subg = glyph->subglyphs + sub_index;


      *p_index     = subg->index;
      *p_flags     = subg->flags;
      *p_arg1      = subg->arg1;
      *p_arg2      = subg->arg2;
      *p_transform = subg->transform;
    }

    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftoutln.c                                                              */
/*                                                                         */
/*    FreeType outline management (body).                                  */
/*                                                                         */
/*  Copyright 1996-2008, 2010, 2012-2014 by                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* All functions are declared in freetype.h.                             */
  /*                                                                       */
  /*************************************************************************/


#include "ft2build.h"
#include FT_OUTLINE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_CALC_H
#include FT_INTERNAL_DEBUG_H
#include FT_TRIGONOMETRY_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_outline


  static
  const FT_Outline  null_outline = { 0, 0, 0, 0, 0, 0 };


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Decompose( FT_Outline*              outline,
                        const FT_Outline_Funcs*  func_interface,
                        void*                    user )
  {
#undef SCALED
#define SCALED( x )  ( ( (x) << shift ) - delta )

    FT_Vector   v_last;
    FT_Vector   v_control;
    FT_Vector   v_start;

    FT_Vector*  point;
    FT_Vector*  limit;
    char*       tags;

    FT_Error    error;

    FT_Int   n;         /* index of contour in outline     */
    FT_UInt  first;     /* index of first point in contour */
    FT_Int   tag;       /* current point's state           */

    FT_Int   shift;
    FT_Pos   delta;


    if ( !outline || !func_interface )
      return FT_THROW( Invalid_Argument );

    shift = func_interface->shift;
    delta = func_interface->delta;
    first = 0;

    for ( n = 0; n < outline->n_contours; n++ )
    {
      FT_Int  last;  /* index of last point in contour */


      FT_TRACE5(( "FT_Outline_Decompose: Outline %d\n", n ));

      last = outline->contours[n];
      if ( last < 0 )
        goto Invalid_Outline;
      limit = outline->points + last;

      v_start   = outline->points[first];
      v_start.x = SCALED( v_start.x );
      v_start.y = SCALED( v_start.y );

      v_last   = outline->points[last];
      v_last.x = SCALED( v_last.x );
      v_last.y = SCALED( v_last.y );

      v_control = v_start;

      point = outline->points + first;
      tags  = outline->tags   + first;
      tag   = FT_CURVE_TAG( tags[0] );

      /* A contour cannot start with a cubic control point! */
      if ( tag == FT_CURVE_TAG_CUBIC )
        goto Invalid_Outline;

      /* check first point to determine origin */
      if ( tag == FT_CURVE_TAG_CONIC )
      {
        /* first point is conic control.  Yes, this happens. */
        if ( FT_CURVE_TAG( outline->tags[last] ) == FT_CURVE_TAG_ON )
        {
          /* start at last point if it is on the curve */
          v_start = v_last;
          limit--;
        }
        else
        {
          /* if both first and last points are conic,         */
          /* start at their middle and record its position    */
          /* for closure                                      */
          v_start.x = ( v_start.x + v_last.x ) / 2;
          v_start.y = ( v_start.y + v_last.y ) / 2;

       /* v_last = v_start; */
        }
        point--;
        tags--;
      }

      FT_TRACE5(( "  move to (%.2f, %.2f)\n",
                  v_start.x / 64.0, v_start.y / 64.0 ));
      error = func_interface->move_to( &v_start, user );
      if ( error )
        goto Exit;

      while ( point < limit )
      {
        point++;
        tags++;

        tag = FT_CURVE_TAG( tags[0] );
        switch ( tag )
        {
        case FT_CURVE_TAG_ON:  /* emit a single line_to */
          {
            FT_Vector  vec;


            vec.x = SCALED( point->x );
            vec.y = SCALED( point->y );

            FT_TRACE5(( "  line to (%.2f, %.2f)\n",
                        vec.x / 64.0, vec.y / 64.0 ));
            error = func_interface->line_to( &vec, user );
            if ( error )
              goto Exit;
            continue;
          }

        case FT_CURVE_TAG_CONIC:  /* consume conic arcs */
          v_control.x = SCALED( point->x );
          v_control.y = SCALED( point->y );

        Do_Conic:
          if ( point < limit )
          {
            FT_Vector  vec;
            FT_Vector  v_middle;


            point++;
            tags++;
            tag = FT_CURVE_TAG( tags[0] );

            vec.x = SCALED( point->x );
            vec.y = SCALED( point->y );

            if ( tag == FT_CURVE_TAG_ON )
            {
              FT_TRACE5(( "  conic to (%.2f, %.2f)"
                          " with control (%.2f, %.2f)\n",
                          vec.x / 64.0, vec.y / 64.0,
                          v_control.x / 64.0, v_control.y / 64.0 ));
              error = func_interface->conic_to( &v_control, &vec, user );
              if ( error )
                goto Exit;
              continue;
            }

            if ( tag != FT_CURVE_TAG_CONIC )
              goto Invalid_Outline;

            v_middle.x = ( v_control.x + vec.x ) / 2;
            v_middle.y = ( v_control.y + vec.y ) / 2;

            FT_TRACE5(( "  conic to (%.2f, %.2f)"
                        " with control (%.2f, %.2f)\n",
                        v_middle.x / 64.0, v_middle.y / 64.0,
                        v_control.x / 64.0, v_control.y / 64.0 ));
            error = func_interface->conic_to( &v_control, &v_middle, user );
            if ( error )
              goto Exit;

            v_control = vec;
            goto Do_Conic;
          }

          FT_TRACE5(( "  conic to (%.2f, %.2f)"
                      " with control (%.2f, %.2f)\n",
                      v_start.x / 64.0, v_start.y / 64.0,
                      v_control.x / 64.0, v_control.y / 64.0 ));
          error = func_interface->conic_to( &v_control, &v_start, user );
          goto Close;

        default:  /* FT_CURVE_TAG_CUBIC */
          {
            FT_Vector  vec1, vec2;


            if ( point + 1 > limit                             ||
                 FT_CURVE_TAG( tags[1] ) != FT_CURVE_TAG_CUBIC )
              goto Invalid_Outline;

            point += 2;
            tags  += 2;

            vec1.x = SCALED( point[-2].x );
            vec1.y = SCALED( point[-2].y );

            vec2.x = SCALED( point[-1].x );
            vec2.y = SCALED( point[-1].y );

            if ( point <= limit )
            {
              FT_Vector  vec;


              vec.x = SCALED( point->x );
              vec.y = SCALED( point->y );

              FT_TRACE5(( "  cubic to (%.2f, %.2f)"
                          " with controls (%.2f, %.2f) and (%.2f, %.2f)\n",
                          vec.x / 64.0, vec.y / 64.0,
                          vec1.x / 64.0, vec1.y / 64.0,
                          vec2.x / 64.0, vec2.y / 64.0 ));
              error = func_interface->cubic_to( &vec1, &vec2, &vec, user );
              if ( error )
                goto Exit;
              continue;
            }

            FT_TRACE5(( "  cubic to (%.2f, %.2f)"
                        " with controls (%.2f, %.2f) and (%.2f, %.2f)\n",
                        v_start.x / 64.0, v_start.y / 64.0,
                        vec1.x / 64.0, vec1.y / 64.0,
                        vec2.x / 64.0, vec2.y / 64.0 ));
            error = func_interface->cubic_to( &vec1, &vec2, &v_start, user );
            goto Close;
          }
        }
      }

      /* close the contour with a line segment */
      FT_TRACE5(( "  line to (%.2f, %.2f)\n",
                  v_start.x / 64.0, v_start.y / 64.0 ));
      error = func_interface->line_to( &v_start, user );

    Close:
      if ( error )
        goto Exit;

      first = last + 1;
    }

    FT_TRACE5(( "FT_Outline_Decompose: Done\n", n ));
    return FT_Err_Ok;

  Exit:
    FT_TRACE5(( "FT_Outline_Decompose: Error %d\n", error ));
    return error;

  Invalid_Outline:
    return FT_THROW( Invalid_Outline );
  }


  FT_EXPORT_DEF( FT_Error )
  FT_Outline_New_Internal( FT_Memory    memory,
                           FT_UInt      numPoints,
                           FT_Int       numContours,
                           FT_Outline  *anoutline )
  {
    FT_Error  error;


    if ( !anoutline || !memory )
      return FT_THROW( Invalid_Argument );

    *anoutline = null_outline;

    if ( numContours < 0                  ||
         (FT_UInt)numContours > numPoints )
      return FT_THROW( Invalid_Argument );

    if ( numPoints > FT_OUTLINE_POINTS_MAX )
      return FT_THROW( Array_Too_Large );

    if ( FT_NEW_ARRAY( anoutline->points,   numPoints   ) ||
         FT_NEW_ARRAY( anoutline->tags,     numPoints   ) ||
         FT_NEW_ARRAY( anoutline->contours, numContours ) )
      goto Fail;

    anoutline->n_points    = (FT_UShort)numPoints;
    anoutline->n_contours  = (FT_Short)numContours;
    anoutline->flags      |= FT_OUTLINE_OWNER;

    return FT_Err_Ok;

  Fail:
    anoutline->flags |= FT_OUTLINE_OWNER;
    FT_Outline_Done_Internal( memory, anoutline );

    return error;
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_New( FT_Library   library,
                  FT_UInt      numPoints,
                  FT_Int       numContours,
                  FT_Outline  *anoutline )
  {
    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    return FT_Outline_New_Internal( library->memory, numPoints,
                                    numContours, anoutline );
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Check( FT_Outline*  outline )
  {
    if ( outline )
    {
      FT_Int  n_points   = outline->n_points;
      FT_Int  n_contours = outline->n_contours;
      FT_Int  end0, end;
      FT_Int  n;


      /* empty glyph? */
      if ( n_points == 0 && n_contours == 0 )
        return 0;

      /* check point and contour counts */
      if ( n_points <= 0 || n_contours <= 0 )
        goto Bad;

      end0 = end = -1;
      for ( n = 0; n < n_contours; n++ )
      {
        end = outline->contours[n];

        /* note that we don't accept empty contours */
        if ( end <= end0 || end >= n_points )
          goto Bad;

        end0 = end;
      }

      if ( end != n_points - 1 )
        goto Bad;

      /* XXX: check the tags array */
      return 0;
    }

  Bad:
    return FT_THROW( Invalid_Argument );
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Copy( const FT_Outline*  source,
                   FT_Outline        *target )
  {
    FT_Int  is_owner;


    if ( !source            || !target            ||
         source->n_points   != target->n_points   ||
         source->n_contours != target->n_contours )
      return FT_THROW( Invalid_Argument );

    if ( source == target )
      return FT_Err_Ok;

    FT_ARRAY_COPY( target->points, source->points, source->n_points );

    FT_ARRAY_COPY( target->tags, source->tags, source->n_points );

    FT_ARRAY_COPY( target->contours, source->contours, source->n_contours );

    /* copy all flags, except the `FT_OUTLINE_OWNER' one */
    is_owner      = target->flags & FT_OUTLINE_OWNER;
    target->flags = source->flags;

    target->flags &= ~FT_OUTLINE_OWNER;
    target->flags |= is_owner;

    return FT_Err_Ok;
  }


  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Done_Internal( FT_Memory    memory,
                            FT_Outline*  outline )
  {
    if ( memory && outline )
    {
      if ( outline->flags & FT_OUTLINE_OWNER )
      {
        FT_FREE( outline->points   );
        FT_FREE( outline->tags     );
        FT_FREE( outline->contours );
      }
      *outline = null_outline;

      return FT_Err_Ok;
    }
    else
      return FT_THROW( Invalid_Argument );
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Done( FT_Library   library,
                   FT_Outline*  outline )
  {
    /* check for valid `outline' in FT_Outline_Done_Internal() */

    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    return FT_Outline_Done_Internal( library->memory, outline );
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( void )
  FT_Outline_Get_CBox( const FT_Outline*  outline,
                       FT_BBox           *acbox )
  {
    FT_Pos  xMin, yMin, xMax, yMax;


    if ( outline && acbox )
    {
      if ( outline->n_points == 0 )
      {
        xMin = 0;
        yMin = 0;
        xMax = 0;
        yMax = 0;
      }
      else
      {
        FT_Vector*  vec   = outline->points;
        FT_Vector*  limit = vec + outline->n_points;


        xMin = xMax = vec->x;
        yMin = yMax = vec->y;
        vec++;

        for ( ; vec < limit; vec++ )
        {
          FT_Pos  x, y;


          x = vec->x;
          if ( x < xMin ) xMin = x;
          if ( x > xMax ) xMax = x;

          y = vec->y;
          if ( y < yMin ) yMin = y;
          if ( y > yMax ) yMax = y;
        }
      }
      acbox->xMin = xMin;
      acbox->xMax = xMax;
      acbox->yMin = yMin;
      acbox->yMax = yMax;
    }
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( void )
  FT_Outline_Translate( const FT_Outline*  outline,
                        FT_Pos             xOffset,
                        FT_Pos             yOffset )
  {
    FT_UShort   n;
    FT_Vector*  vec;


    if ( !outline )
      return;

    vec = outline->points;

    for ( n = 0; n < outline->n_points; n++ )
    {
      vec->x += xOffset;
      vec->y += yOffset;
      vec++;
    }
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( void )
  FT_Outline_Reverse( FT_Outline*  outline )
  {
    FT_UShort  n;
    FT_Int     first, last;


    if ( !outline )
      return;

    first = 0;

    for ( n = 0; n < outline->n_contours; n++ )
    {
      last  = outline->contours[n];

      /* reverse point table */
      {
        FT_Vector*  p = outline->points + first;
        FT_Vector*  q = outline->points + last;
        FT_Vector   swap;


        while ( p < q )
        {
          swap = *p;
          *p   = *q;
          *q   = swap;
          p++;
          q--;
        }
      }

      /* reverse tags table */
      {
        char*  p = outline->tags + first;
        char*  q = outline->tags + last;


        while ( p < q )
        {
          char  swap;


          swap = *p;
          *p   = *q;
          *q   = swap;
          p++;
          q--;
        }
      }

      first = last + 1;
    }

    outline->flags ^= FT_OUTLINE_REVERSE_FILL;
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Render( FT_Library         library,
                     FT_Outline*        outline,
                     FT_Raster_Params*  params )
  {
    FT_Error     error;
    FT_Bool      update = FALSE;
    FT_Renderer  renderer;
    FT_ListNode  node;


    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    if ( !outline || !params )
      return FT_THROW( Invalid_Argument );

    renderer = library->cur_renderer;
    node     = library->renderers.head;

    params->source = (void*)outline;

    error = FT_ERR( Cannot_Render_Glyph );
    while ( renderer )
    {
      error = renderer->raster_render( renderer->raster, params );
      if ( !error || FT_ERR_NEQ( error, Cannot_Render_Glyph ) )
        break;

      /* FT_Err_Cannot_Render_Glyph is returned if the render mode   */
      /* is unsupported by the current renderer for this glyph image */
      /* format                                                      */

      /* now, look for another renderer that supports the same */
      /* format                                                */
      renderer = FT_Lookup_Renderer( library, FT_GLYPH_FORMAT_OUTLINE,
                                     &node );
      update   = TRUE;
    }

    /* if we changed the current renderer for the glyph image format */
    /* we need to select it as the next current one                  */
    if ( !error && update && renderer )
      FT_Set_Renderer( library, renderer, 0, 0 );

    return error;
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Get_Bitmap( FT_Library        library,
                         FT_Outline*       outline,
                         const FT_Bitmap  *abitmap )
  {
    FT_Raster_Params  params;


    if ( !abitmap )
      return FT_THROW( Invalid_Argument );

    /* other checks are delayed to FT_Outline_Render() */

    params.target = abitmap;
    params.flags  = 0;

    if ( abitmap->pixel_mode == FT_PIXEL_MODE_GRAY  ||
         abitmap->pixel_mode == FT_PIXEL_MODE_LCD   ||
         abitmap->pixel_mode == FT_PIXEL_MODE_LCD_V )
      params.flags |= FT_RASTER_FLAG_AA;

    return FT_Outline_Render( library, outline, &params );
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( void )
  FT_Vector_Transform( FT_Vector*        vector,
                       const FT_Matrix*  matrix )
  {
    FT_Pos  xz, yz;


    if ( !vector || !matrix )
      return;

    xz = FT_MulFix( vector->x, matrix->xx ) +
         FT_MulFix( vector->y, matrix->xy );

    yz = FT_MulFix( vector->x, matrix->yx ) +
         FT_MulFix( vector->y, matrix->yy );

    vector->x = xz;
    vector->y = yz;
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( void )
  FT_Outline_Transform( const FT_Outline*  outline,
                        const FT_Matrix*   matrix )
  {
    FT_Vector*  vec;
    FT_Vector*  limit;


    if ( !outline || !matrix )
      return;

    vec   = outline->points;
    limit = vec + outline->n_points;

    for ( ; vec < limit; vec++ )
      FT_Vector_Transform( vec, matrix );
  }


#if 0

#define FT_OUTLINE_GET_CONTOUR( outline, c, first, last )  \
  do                                                       \
  {                                                        \
    (first) = ( c > 0 ) ? (outline)->points +              \
                            (outline)->contours[c - 1] + 1 \
                        : (outline)->points;               \
    (last) = (outline)->points + (outline)->contours[c];   \
  } while ( 0 )


  /* Is a point in some contour?                     */
  /*                                                 */
  /* We treat every point of the contour as if it    */
  /* it were ON.  That is, we allow false positives, */
  /* but disallow false negatives.  (XXX really?)    */
  static FT_Bool
  ft_contour_has( FT_Outline*  outline,
                  FT_Short     c,
                  FT_Vector*   point )
  {
    FT_Vector*  first;
    FT_Vector*  last;
    FT_Vector*  a;
    FT_Vector*  b;
    FT_UInt     n = 0;


    FT_OUTLINE_GET_CONTOUR( outline, c, first, last );

    for ( a = first; a <= last; a++ )
    {
      FT_Pos  x;
      FT_Int  intersect;


      b = ( a == last ) ? first : a + 1;

      intersect = ( a->y - point->y ) ^ ( b->y - point->y );

      /* a and b are on the same side */
      if ( intersect >= 0 )
      {
        if ( intersect == 0 && a->y == point->y )
        {
          if ( ( a->x <= point->x && b->x >= point->x ) ||
               ( a->x >= point->x && b->x <= point->x ) )
            return 1;
        }

        continue;
      }

      x = a->x + ( b->x - a->x ) * (point->y - a->y ) / ( b->y - a->y );

      if ( x < point->x )
        n++;
      else if ( x == point->x )
        return 1;
    }

    return n & 1;
  }


  static FT_Bool
  ft_contour_enclosed( FT_Outline*  outline,
                       FT_UShort    c )
  {
    FT_Vector*  first;
    FT_Vector*  last;
    FT_Short    i;


    FT_OUTLINE_GET_CONTOUR( outline, c, first, last );

    for ( i = 0; i < outline->n_contours; i++ )
    {
      if ( i != c && ft_contour_has( outline, i, first ) )
      {
        FT_Vector*  pt;


        for ( pt = first + 1; pt <= last; pt++ )
          if ( !ft_contour_has( outline, i, pt ) )
            return 0;

        return 1;
      }
    }

    return 0;
  }


  /* This version differs from the public one in that each */
  /* part (contour not enclosed in another contour) of the */
  /* outline is checked for orientation.  This is          */
  /* necessary for some buggy CJK fonts.                   */
  static FT_Orientation
  ft_outline_get_orientation( FT_Outline*  outline )
  {
    FT_Short        i;
    FT_Vector*      first;
    FT_Vector*      last;
    FT_Orientation  orient = FT_ORIENTATION_NONE;


    first = outline->points;
    for ( i = 0; i < outline->n_contours; i++, first = last + 1 )
    {
      FT_Vector*  point;
      FT_Vector*  xmin_point;
      FT_Pos      xmin;


      last = outline->points + outline->contours[i];

      /* skip degenerate contours */
      if ( last < first + 2 )
        continue;

      if ( ft_contour_enclosed( outline, i ) )
        continue;

      xmin       = first->x;
      xmin_point = first;

      for ( point = first + 1; point <= last; point++ )
      {
        if ( point->x < xmin )
        {
          xmin       = point->x;
          xmin_point = point;
        }
      }

      /* check the orientation of the contour */
      {
        FT_Vector*      prev;
        FT_Vector*      next;
        FT_Orientation  o;


        prev = ( xmin_point == first ) ? last : xmin_point - 1;
        next = ( xmin_point == last ) ? first : xmin_point + 1;

        if ( FT_Atan2( prev->x - xmin_point->x, prev->y - xmin_point->y ) >
             FT_Atan2( next->x - xmin_point->x, next->y - xmin_point->y ) )
          o = FT_ORIENTATION_POSTSCRIPT;
        else
          o = FT_ORIENTATION_TRUETYPE;

        if ( orient == FT_ORIENTATION_NONE )
          orient = o;
        else if ( orient != o )
          return FT_ORIENTATION_NONE;
      }
    }

    return orient;
  }

#endif /* 0 */


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_Embolden( FT_Outline*  outline,
                       FT_Pos       strength )
  {
    return FT_Outline_EmboldenXY( outline, strength, strength );
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Outline_EmboldenXY( FT_Outline*  outline,
                         FT_Pos       xstrength,
                         FT_Pos       ystrength )
  {
    FT_Vector*  points;
    FT_Vector   v_prev, v_first, v_next, v_cur;
    FT_Int      c, n, first;
    FT_Int      orientation;


    if ( !outline )
      return FT_THROW( Invalid_Argument );

    xstrength /= 2;
    ystrength /= 2;
    if ( xstrength == 0 && ystrength == 0 )
      return FT_Err_Ok;

    orientation = FT_Outline_Get_Orientation( outline );
    if ( orientation == FT_ORIENTATION_NONE )
    {
      if ( outline->n_contours )
        return FT_THROW( Invalid_Argument );
      else
        return FT_Err_Ok;
    }

    points = outline->points;

    first = 0;
    for ( c = 0; c < outline->n_contours; c++ )
    {
      FT_Vector  in, out, shift;
      FT_Fixed   l_in, l_out, l, q, d;
      int        last = outline->contours[c];


      v_first = points[first];
      v_prev  = points[last];
      v_cur   = v_first;

      /* compute incoming normalized vector */
      in.x = v_cur.x - v_prev.x;
      in.y = v_cur.y - v_prev.y;
      l_in = FT_Vector_Length( &in );
      if ( l_in )
      {
        in.x = FT_DivFix( in.x, l_in );
        in.y = FT_DivFix( in.y, l_in );
      }

      for ( n = first; n <= last; n++ )
      {
        if ( n < last )
          v_next = points[n + 1];
        else
          v_next = v_first;

        /* compute outgoing normalized vector */
        out.x = v_next.x - v_cur.x;
        out.y = v_next.y - v_cur.y;
        l_out = FT_Vector_Length( &out );
        if ( l_out )
        {
          out.x = FT_DivFix( out.x, l_out );
          out.y = FT_DivFix( out.y, l_out );
        }

        d = FT_MulFix( in.x, out.x ) + FT_MulFix( in.y, out.y );

        /* shift only if turn is less than ~160 degrees */
        if ( d > -0xF000L )
        {
          d = d + 0x10000L;

          /* shift components are aligned along lateral bisector */
          /* and directed according to the outline orientation.  */
          shift.x = in.y + out.y;
          shift.y = in.x + out.x;

          if ( orientation == FT_ORIENTATION_TRUETYPE )
            shift.x = -shift.x;
          else
            shift.y = -shift.y;

          /* restrict shift magnitude to better handle collapsing segments */
          q = FT_MulFix( out.x, in.y ) - FT_MulFix( out.y, in.x );
          if ( orientation == FT_ORIENTATION_TRUETYPE )
            q = -q;

          l = FT_MIN( l_in, l_out );

          /* non-strict inequalities avoid divide-by-zero when q == l == 0 */
          if ( FT_MulFix( xstrength, q ) <= FT_MulFix( d, l ) )
            shift.x = FT_MulDiv( shift.x, xstrength, d );
          else
            shift.x = FT_MulDiv( shift.x, l, q );


          if ( FT_MulFix( ystrength, q ) <= FT_MulFix( d, l ) )
            shift.y = FT_MulDiv( shift.y, ystrength, d );
          else
            shift.y = FT_MulDiv( shift.y, l, q );
        }
        else
          shift.x = shift.y = 0;

        outline->points[n].x = v_cur.x + xstrength + shift.x;
        outline->points[n].y = v_cur.y + ystrength + shift.y;

        in    = out;
        l_in  = l_out;
        v_cur = v_next;
      }

      first = last + 1;
    }

    return FT_Err_Ok;
  }


  /* documentation is in ftoutln.h */

  FT_EXPORT_DEF( FT_Orientation )
  FT_Outline_Get_Orientation( FT_Outline*  outline )
  {
    FT_BBox     cbox;
    FT_Int      xshift, yshift;
    FT_Vector*  points;
    FT_Vector   v_prev, v_cur;
    FT_Int      c, n, first;
    FT_Pos      area = 0;


    if ( !outline || outline->n_points <= 0 )
      return FT_ORIENTATION_TRUETYPE;

    /* We use the nonzero winding rule to find the orientation.       */
    /* Since glyph outlines behave much more `regular' than arbitrary */
    /* cubic or quadratic curves, this test deals with the polygon    */
    /* only which is spanned up by the control points.                */

    FT_Outline_Get_CBox( outline, &cbox );

    xshift = FT_MSB( FT_ABS( cbox.xMax ) | FT_ABS( cbox.xMin ) ) - 14;
    xshift = FT_MAX( xshift, 0 );

    yshift = FT_MSB( cbox.yMax - cbox.yMin ) - 14;
    yshift = FT_MAX( yshift, 0 );

    points = outline->points;

    first = 0;
    for ( c = 0; c < outline->n_contours; c++ )
    {
      FT_Int  last = outline->contours[c];


      v_prev = points[last];

      for ( n = first; n <= last; n++ )
      {
        v_cur = points[n];
        area += ( ( v_cur.y - v_prev.y ) >> yshift ) *
                ( ( v_cur.x + v_prev.x ) >> xshift );
        v_prev = v_cur;
      }

      first = last + 1;
    }

    if ( area > 0 )
      return FT_ORIENTATION_POSTSCRIPT;
    else if ( area < 0 )
      return FT_ORIENTATION_TRUETYPE;
    else
      return FT_ORIENTATION_NONE;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftrfork.c                                                              */
/*                                                                         */
/*    Embedded resource forks accessor (body).                             */
/*                                                                         */
/*  Copyright 2004-2010, 2013, 2014 by                                     */
/*  Masatake YAMATO and Redhat K.K.                                        */
/*                                                                         */
/*  FT_Raccess_Get_HeaderInfo() and raccess_guess_darwin_hfsplus() are     */
/*  derived from ftobjs.c.                                                 */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* Development of the code in this file is support of                      */
/* Information-technology Promotion Agency, Japan.                         */
/***************************************************************************/


#include "ft2build.h"
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_RFORK_H
#include "basepic.h"
#include "ftbase.h"

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_raccess


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****               Resource fork directory access                    ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

  FT_BASE_DEF( FT_Error )
  FT_Raccess_Get_HeaderInfo( FT_Library  library,
                             FT_Stream   stream,
                             FT_Long     rfork_offset,
                             FT_Long    *map_offset,
                             FT_Long    *rdata_pos )
  {
    FT_Error       error;
    unsigned char  head[16], head2[16];
    FT_Long        map_pos, rdata_len;
    int            allzeros, allmatch, i;
    FT_Long        type_list;

    FT_UNUSED( library );


    error = FT_Stream_Seek( stream, rfork_offset );
    if ( error )
      return error;

    error = FT_Stream_Read( stream, (FT_Byte *)head, 16 );
    if ( error )
      return error;

    *rdata_pos = rfork_offset + ( ( head[0] << 24 ) |
                                  ( head[1] << 16 ) |
                                  ( head[2] <<  8 ) |
                                    head[3]         );
    map_pos    = rfork_offset + ( ( head[4] << 24 ) |
                                  ( head[5] << 16 ) |
                                  ( head[6] <<  8 ) |
                                    head[7]         );
    rdata_len = ( head[ 8] << 24 ) |
                ( head[ 9] << 16 ) |
                ( head[10] <<  8 ) |
                  head[11];

    /* map_len = head[12] .. head[15] */

    if ( *rdata_pos + rdata_len != map_pos || map_pos == rfork_offset )
      return FT_THROW( Unknown_File_Format );

    error = FT_Stream_Seek( stream, map_pos );
    if ( error )
      return error;

    head2[15] = (FT_Byte)( head[15] + 1 );       /* make it be different */

    error = FT_Stream_Read( stream, (FT_Byte*)head2, 16 );
    if ( error )
      return error;

    allzeros = 1;
    allmatch = 1;
    for ( i = 0; i < 16; ++i )
    {
      if ( head2[i] != 0 )
        allzeros = 0;
      if ( head2[i] != head[i] )
        allmatch = 0;
    }
    if ( !allzeros && !allmatch )
      return FT_THROW( Unknown_File_Format );

    /* If we have reached this point then it is probably a mac resource */
    /* file.  Now, does it contain any interesting resources?           */
    /* Skip handle to next resource map, the file resource number, and  */
    /* attributes.                                                      */
    (void)FT_STREAM_SKIP( 4        /* skip handle to next resource map */
                          + 2      /* skip file resource number */
                          + 2 );   /* skip attributes */

    if ( FT_READ_USHORT( type_list ) )
      return error;
    if ( type_list == -1 )
      return FT_THROW( Unknown_File_Format );

    error = FT_Stream_Seek( stream, map_pos + type_list );
    if ( error )
      return error;

    *map_offset = map_pos + type_list;
    return FT_Err_Ok;
  }


  static int
  ft_raccess_sort_ref_by_id( FT_RFork_Ref*  a,
                             FT_RFork_Ref*  b )
  {
    if ( a->res_id < b->res_id )
      return -1;
    else if ( a->res_id > b->res_id )
      return 1;
    else
      return 0;
  }


  FT_BASE_DEF( FT_Error )
  FT_Raccess_Get_DataOffsets( FT_Library  library,
                              FT_Stream   stream,
                              FT_Long     map_offset,
                              FT_Long     rdata_pos,
                              FT_Long     tag,
                              FT_Bool     sort_by_res_id,
                              FT_Long   **offsets,
                              FT_Long    *count )
  {
    FT_Error      error;
    int           i, j, cnt, subcnt;
    FT_Long       tag_internal, rpos;
    FT_Memory     memory = library->memory;
    FT_Long       temp;
    FT_Long       *offsets_internal = NULL;
    FT_RFork_Ref  *ref = NULL;


    FT_TRACE3(( "\n" ));
    error = FT_Stream_Seek( stream, map_offset );
    if ( error )
      return error;

    if ( FT_READ_USHORT( cnt ) )
      return error;
    cnt++;

    for ( i = 0; i < cnt; ++i )
    {
      if ( FT_READ_LONG( tag_internal ) ||
           FT_READ_USHORT( subcnt )     ||
           FT_READ_USHORT( rpos )       )
        return error;

      FT_TRACE2(( "Resource tags: %c%c%c%c\n",
                  (char)( 0xff & ( tag_internal >> 24 ) ),
                  (char)( 0xff & ( tag_internal >> 16 ) ),
                  (char)( 0xff & ( tag_internal >>  8 ) ),
                  (char)( 0xff & ( tag_internal >>  0 ) ) ));
      FT_TRACE3(( "             : subcount=%d, suboffset=0x%04x\n",
                  subcnt, rpos ));

      if ( tag_internal == tag )
      {
        *count = subcnt + 1;
        rpos  += map_offset;

        error = FT_Stream_Seek( stream, rpos );
        if ( error )
          return error;

        if ( FT_NEW_ARRAY( ref, *count ) )
          return error;

        for ( j = 0; j < *count; ++j )
        {
          if ( FT_READ_USHORT( ref[j].res_id ) )
            goto Exit;
          if ( FT_STREAM_SKIP( 2 ) ) /* resource name */
            goto Exit;
          if ( FT_READ_LONG( temp ) )
            goto Exit;
          if ( FT_STREAM_SKIP( 4 ) ) /* mbz */
            goto Exit;

          ref[j].offset = temp & 0xFFFFFFL;
          FT_TRACE3(( "             [%d]:"
                      " resource_id=0x%04x, offset=0x%08x\n",
                      j, ref[j].res_id, ref[j].offset ));
        }

        if (sort_by_res_id)
        {
          ft_qsort( ref, *count, sizeof ( FT_RFork_Ref ),
                    ( int(*)(const void*, const void*) )
                    ft_raccess_sort_ref_by_id );

          FT_TRACE3(( "             -- sort resources by their ids --\n" ));
          for ( j = 0; j < *count; ++ j ) {
            FT_TRACE3(( "             [%d]:"
                        " resource_id=0x%04x, offset=0x%08x\n",
                        j, ref[j].res_id, ref[j].offset ));
          }
        }

        if ( FT_NEW_ARRAY( offsets_internal, *count ) )
          goto Exit;

        /* XXX: duplicated reference ID,
         *      gap between reference IDs are acceptable?
         *      further investigation on Apple implementation is needed.
         */
        for ( j = 0; j < *count; ++j )
          offsets_internal[j] = rdata_pos + ref[j].offset;

        *offsets = offsets_internal;
        error    = FT_Err_Ok;

      Exit:
        FT_FREE( ref );
        return error;
      }
    }

    return FT_THROW( Cannot_Open_Resource );
  }


#ifdef FT_CONFIG_OPTION_GUESSING_EMBEDDED_RFORK

  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                     Guessing functions                          ****/
  /****                                                                 ****/
  /****            When you add a new guessing function,                ****/
  /****           update FT_RACCESS_N_RULES in ftrfork.h.               ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

  static FT_Error
  raccess_guess_apple_double( FT_Library  library,
                              FT_Stream   stream,
                              char       *base_file_name,
                              char      **result_file_name,
                              FT_Long    *result_offset );

  static FT_Error
  raccess_guess_apple_single( FT_Library  library,
                              FT_Stream   stream,
                              char       *base_file_name,
                              char      **result_file_name,
                              FT_Long    *result_offset );

  static FT_Error
  raccess_guess_darwin_ufs_export( FT_Library  library,
                                   FT_Stream   stream,
                                   char       *base_file_name,
                                   char      **result_file_name,
                                   FT_Long    *result_offset );

  static FT_Error
  raccess_guess_darwin_newvfs( FT_Library  library,
                               FT_Stream   stream,
                               char       *base_file_name,
                               char      **result_file_name,
                               FT_Long    *result_offset );

  static FT_Error
  raccess_guess_darwin_hfsplus( FT_Library  library,
                                FT_Stream   stream,
                                char       *base_file_name,
                                char      **result_file_name,
                                FT_Long    *result_offset );

  static FT_Error
  raccess_guess_vfat( FT_Library  library,
                      FT_Stream   stream,
                      char       *base_file_name,
                      char      **result_file_name,
                      FT_Long    *result_offset );

  static FT_Error
  raccess_guess_linux_cap( FT_Library  library,
                           FT_Stream   stream,
                           char       *base_file_name,
                           char      **result_file_name,
                           FT_Long    *result_offset );

  static FT_Error
  raccess_guess_linux_double( FT_Library  library,
                              FT_Stream   stream,
                              char       *base_file_name,
                              char      **result_file_name,
                              FT_Long    *result_offset );

  static FT_Error
  raccess_guess_linux_netatalk( FT_Library  library,
                                FT_Stream   stream,
                                char       *base_file_name,
                                char      **result_file_name,
                                FT_Long    *result_offset );


  CONST_FT_RFORK_RULE_ARRAY_BEGIN(ft_raccess_guess_table,
                                  ft_raccess_guess_rec)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(apple_double,      apple_double)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(apple_single,      apple_single)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(darwin_ufs_export, darwin_ufs_export)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(darwin_newvfs,     darwin_newvfs)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(darwin_hfsplus,    darwin_hfsplus)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(vfat,              vfat)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(linux_cap,         linux_cap)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(linux_double,      linux_double)
  CONST_FT_RFORK_RULE_ARRAY_ENTRY(linux_netatalk,    linux_netatalk)
  CONST_FT_RFORK_RULE_ARRAY_END


  /*************************************************************************/
  /****                                                                 ****/
  /****                       Helper functions                          ****/
  /****                                                                 ****/
  /*************************************************************************/

  static FT_Error
  raccess_guess_apple_generic( FT_Library  library,
                               FT_Stream   stream,
                               char       *base_file_name,
                               FT_Int32    magic,
                               FT_Long    *result_offset );

  static FT_Error
  raccess_guess_linux_double_from_file_name( FT_Library  library,
                                             char *      file_name,
                                             FT_Long    *result_offset );

  static char *
  raccess_make_file_name( FT_Memory    memory,
                          const char  *original_name,
                          const char  *insertion );

  FT_BASE_DEF( void )
  FT_Raccess_Guess( FT_Library  library,
                    FT_Stream   stream,
                    char*       base_name,
                    char      **new_names,
                    FT_Long    *offsets,
                    FT_Error   *errors )
  {
    FT_Int  i;


    for ( i = 0; i < FT_RACCESS_N_RULES; i++ )
    {
      new_names[i] = NULL;
      if ( NULL != stream )
        errors[i] = FT_Stream_Seek( stream, 0 );
      else
        errors[i] = FT_Err_Ok;

      if ( errors[i] )
        continue ;

      errors[i] = (FT_RACCESS_GUESS_TABLE_GET[i].func)( library,
                                                 stream, base_name,
                                                 &(new_names[i]),
                                                 &(offsets[i]) );
    }

    return;
  }


#ifndef FT_MACINTOSH
  static FT_RFork_Rule
  raccess_get_rule_type_from_rule_index( FT_Library  library,
                                         FT_UInt     rule_index )
  {
    FT_UNUSED( library );

    if ( rule_index >= FT_RACCESS_N_RULES )
      return FT_RFork_Rule_invalid;

    return FT_RACCESS_GUESS_TABLE_GET[rule_index].type;
  }


  /*
   * For this function, refer ftbase.h.
   */
  FT_LOCAL_DEF( FT_Bool )
  ft_raccess_rule_by_darwin_vfs( FT_Library  library,
                                 FT_UInt     rule_index )
  {
    switch( raccess_get_rule_type_from_rule_index( library, rule_index ) )
    {
      case FT_RFork_Rule_darwin_newvfs:
      case FT_RFork_Rule_darwin_hfsplus:
        return TRUE;

      default:
        return FALSE;
    }
  }
#endif


  static FT_Error
  raccess_guess_apple_double( FT_Library  library,
                              FT_Stream   stream,
                              char       *base_file_name,
                              char      **result_file_name,
                              FT_Long    *result_offset )
  {
    FT_Int32  magic = ( 0x00 << 24 ) |
                      ( 0x05 << 16 ) |
                      ( 0x16 <<  8 ) |
                        0x07;


    *result_file_name = NULL;
    if ( NULL == stream )
      return FT_THROW( Cannot_Open_Stream );

    return raccess_guess_apple_generic( library, stream, base_file_name,
                                        magic, result_offset );
  }


  static FT_Error
  raccess_guess_apple_single( FT_Library  library,
                              FT_Stream   stream,
                              char       *base_file_name,
                              char      **result_file_name,
                              FT_Long    *result_offset )
  {
    FT_Int32  magic = ( 0x00 << 24 ) |
                      ( 0x05 << 16 ) |
                      ( 0x16 <<  8 ) |
                        0x00;


    *result_file_name = NULL;
    if ( NULL == stream )
      return FT_THROW( Cannot_Open_Stream );

    return raccess_guess_apple_generic( library, stream, base_file_name,
                                        magic, result_offset );
  }


  static FT_Error
  raccess_guess_darwin_ufs_export( FT_Library  library,
                                   FT_Stream   stream,
                                   char       *base_file_name,
                                   char      **result_file_name,
                                   FT_Long    *result_offset )
  {
    char*      newpath;
    FT_Error   error;
    FT_Memory  memory;

    FT_UNUSED( stream );


    memory  = library->memory;
    newpath = raccess_make_file_name( memory, base_file_name, "._" );
    if ( !newpath )
      return FT_THROW( Out_Of_Memory );

    error = raccess_guess_linux_double_from_file_name( library, newpath,
                                                       result_offset );
    if ( !error )
      *result_file_name = newpath;
    else
      FT_FREE( newpath );

    return error;
  }


  static FT_Error
  raccess_guess_darwin_hfsplus( FT_Library  library,
                                FT_Stream   stream,
                                char       *base_file_name,
                                char      **result_file_name,
                                FT_Long    *result_offset )
  {
    /*
      Only meaningful on systems with hfs+ drivers (or Macs).
     */
    FT_Error   error;
    char*      newpath = NULL;
    FT_Memory  memory;
    FT_Long    base_file_len = (FT_Long)ft_strlen( base_file_name );

    FT_UNUSED( stream );


    memory = library->memory;

    if ( base_file_len + 6 > FT_INT_MAX )
      return FT_THROW( Array_Too_Large );

    if ( FT_ALLOC( newpath, base_file_len + 6 ) )
      return error;

    FT_MEM_COPY( newpath, base_file_name, base_file_len );
    FT_MEM_COPY( newpath + base_file_len, "/rsrc", 6 );

    *result_file_name = newpath;
    *result_offset    = 0;

    return FT_Err_Ok;
  }


  static FT_Error
  raccess_guess_darwin_newvfs( FT_Library  library,
                               FT_Stream   stream,
                               char       *base_file_name,
                               char      **result_file_name,
                               FT_Long    *result_offset )
  {
    /*
      Only meaningful on systems with Mac OS X (> 10.1).
     */
    FT_Error   error;
    char*      newpath = NULL;
    FT_Memory  memory;
    FT_Long    base_file_len = (FT_Long)ft_strlen( base_file_name );

    FT_UNUSED( stream );


    memory = library->memory;

    if ( base_file_len + 18 > FT_INT_MAX )
      return FT_THROW( Array_Too_Large );

    if ( FT_ALLOC( newpath, base_file_len + 18 ) )
      return error;

    FT_MEM_COPY( newpath, base_file_name, base_file_len );
    FT_MEM_COPY( newpath + base_file_len, "/..namedfork/rsrc", 18 );

    *result_file_name = newpath;
    *result_offset    = 0;

    return FT_Err_Ok;
  }


  static FT_Error
  raccess_guess_vfat( FT_Library  library,
                      FT_Stream   stream,
                      char       *base_file_name,
                      char      **result_file_name,
                      FT_Long    *result_offset )
  {
    char*      newpath;
    FT_Memory  memory;

    FT_UNUSED( stream );


    memory = library->memory;

    newpath = raccess_make_file_name( memory, base_file_name,
                                      "resource.frk/" );
    if ( !newpath )
      return FT_THROW( Out_Of_Memory );

    *result_file_name = newpath;
    *result_offset    = 0;

    return FT_Err_Ok;
  }


  static FT_Error
  raccess_guess_linux_cap( FT_Library  library,
                           FT_Stream   stream,
                           char       *base_file_name,
                           char      **result_file_name,
                           FT_Long    *result_offset )
  {
    char*      newpath;
    FT_Memory  memory;

    FT_UNUSED( stream );


    memory = library->memory;

    newpath = raccess_make_file_name( memory, base_file_name, ".resource/" );
    if ( !newpath )
      return FT_THROW( Out_Of_Memory );

    *result_file_name = newpath;
    *result_offset    = 0;

    return FT_Err_Ok;
  }


  static FT_Error
  raccess_guess_linux_double( FT_Library  library,
                              FT_Stream   stream,
                              char       *base_file_name,
                              char      **result_file_name,
                              FT_Long    *result_offset )
  {
    char*      newpath;
    FT_Error   error;
    FT_Memory  memory;

    FT_UNUSED( stream );


    memory = library->memory;

    newpath = raccess_make_file_name( memory, base_file_name, "%" );
    if ( !newpath )
      return FT_THROW( Out_Of_Memory );

    error = raccess_guess_linux_double_from_file_name( library, newpath,
                                                       result_offset );
    if ( !error )
      *result_file_name = newpath;
    else
      FT_FREE( newpath );

    return error;
  }


  static FT_Error
  raccess_guess_linux_netatalk( FT_Library  library,
                                FT_Stream   stream,
                                char       *base_file_name,
                                char      **result_file_name,
                                FT_Long    *result_offset )
  {
    char*      newpath;
    FT_Error   error;
    FT_Memory  memory;

    FT_UNUSED( stream );


    memory = library->memory;

    newpath = raccess_make_file_name( memory, base_file_name,
                                      ".AppleDouble/" );
    if ( !newpath )
      return FT_THROW( Out_Of_Memory );

    error = raccess_guess_linux_double_from_file_name( library, newpath,
                                                       result_offset );
    if ( !error )
      *result_file_name = newpath;
    else
      FT_FREE( newpath );

    return error;
  }


  static FT_Error
  raccess_guess_apple_generic( FT_Library  library,
                               FT_Stream   stream,
                               char       *base_file_name,
                               FT_Int32    magic,
                               FT_Long    *result_offset )
  {
    FT_Int32   magic_from_stream;
    FT_Error   error;
    FT_Int32   version_number = 0;
    FT_UShort  n_of_entries;

    int        i;
    FT_UInt32  entry_id, entry_offset, entry_length = 0;

    const FT_UInt32  resource_fork_entry_id = 0x2;

    FT_UNUSED( library );
    FT_UNUSED( base_file_name );
    FT_UNUSED( version_number );
    FT_UNUSED( entry_length   );


    if ( FT_READ_LONG( magic_from_stream ) )
      return error;
    if ( magic_from_stream != magic )
      return FT_THROW( Unknown_File_Format );

    if ( FT_READ_LONG( version_number ) )
      return error;

    /* filler */
    error = FT_Stream_Skip( stream, 16 );
    if ( error )
      return error;

    if ( FT_READ_USHORT( n_of_entries ) )
      return error;
    if ( n_of_entries == 0 )
      return FT_THROW( Unknown_File_Format );

    for ( i = 0; i < n_of_entries; i++ )
    {
      if ( FT_READ_LONG( entry_id ) )
        return error;
      if ( entry_id == resource_fork_entry_id )
      {
        if ( FT_READ_LONG( entry_offset ) ||
             FT_READ_LONG( entry_length ) )
          continue;
        *result_offset = entry_offset;

        return FT_Err_Ok;
      }
      else
      {
        error = FT_Stream_Skip( stream, 4 + 4 );    /* offset + length */
        if ( error )
          return error;
      }
    }

    return FT_THROW( Unknown_File_Format );
  }


  static FT_Error
  raccess_guess_linux_double_from_file_name( FT_Library  library,
                                             char       *file_name,
                                             FT_Long    *result_offset )
  {
    FT_Open_Args  args2;
    FT_Stream     stream2;
    char *        nouse = NULL;
    FT_Error      error;


    args2.flags    = FT_OPEN_PATHNAME;
    args2.pathname = file_name;
    error = FT_Stream_New( library, &args2, &stream2 );
    if ( error )
      return error;

    error = raccess_guess_apple_double( library, stream2, file_name,
                                        &nouse, result_offset );

    FT_Stream_Free( stream2, 0 );

    return error;
  }


  static char*
  raccess_make_file_name( FT_Memory    memory,
                          const char  *original_name,
                          const char  *insertion )
  {
    char*        new_name = NULL;
    const char*  tmp;
    const char*  slash;
    size_t       new_length;
    FT_Error     error = FT_Err_Ok;

    FT_UNUSED( error );


    new_length = ft_strlen( original_name ) + ft_strlen( insertion );
    if ( FT_ALLOC( new_name, new_length + 1 ) )
      return NULL;

    tmp = ft_strrchr( original_name, '/' );
    if ( tmp )
    {
      ft_strncpy( new_name, original_name, tmp - original_name + 1 );
      new_name[tmp - original_name + 1] = '\0';
      slash = tmp + 1;
    }
    else
    {
      slash       = original_name;
      new_name[0] = '\0';
    }

    ft_strcat( new_name, insertion );
    ft_strcat( new_name, slash );

    return new_name;
  }


#else   /* !FT_CONFIG_OPTION_GUESSING_EMBEDDED_RFORK */


  /*************************************************************************/
  /*                  Dummy function; just sets errors                     */
  /*************************************************************************/

  FT_BASE_DEF( void )
  FT_Raccess_Guess( FT_Library  library,
                    FT_Stream   stream,
                    char       *base_name,
                    char      **new_names,
                    FT_Long    *offsets,
                    FT_Error   *errors )
  {
    FT_Int  i;

    FT_UNUSED( library );
    FT_UNUSED( stream );
    FT_UNUSED( base_name );


    for ( i = 0; i < FT_RACCESS_N_RULES; i++ )
    {
      new_names[i] = NULL;
      offsets[i]   = 0;
      errors[i]    = FT_ERR( Unimplemented_Feature );
    }
  }


#endif  /* !FT_CONFIG_OPTION_GUESSING_EMBEDDED_RFORK */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftsnames.c                                                             */
/*                                                                         */
/*    Simple interface to access SFNT name tables (which are used          */
/*    to hold font names, copyright info, notices, etc.) (body).           */
/*                                                                         */
/*    This is _not_ used to retrieve glyph names!                          */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2009 by                                     */
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
#include FT_SFNT_NAMES_H
#include FT_INTERNAL_TRUETYPE_TYPES_H
#include FT_INTERNAL_STREAM_H


#ifdef TT_CONFIG_OPTION_SFNT_NAMES


  /* documentation is in ftsnames.h */

  FT_EXPORT_DEF( FT_UInt )
  FT_Get_Sfnt_Name_Count( FT_Face  face )
  {
    return ( face && FT_IS_SFNT( face ) ) ? ((TT_Face)face)->num_names : 0;
  }


  /* documentation is in ftsnames.h */

  FT_EXPORT_DEF( FT_Error )
  FT_Get_Sfnt_Name( FT_Face       face,
                    FT_UInt       idx,
                    FT_SfntName  *aname )
  {
    FT_Error  error = FT_ERR( Invalid_Argument );


    if ( aname && face && FT_IS_SFNT( face ) )
    {
      TT_Face  ttface = (TT_Face)face;


      if ( idx < (FT_UInt)ttface->num_names )
      {
        TT_NameEntryRec*  entry = ttface->name_table.names + idx;


        /* load name on demand */
        if ( entry->stringLength > 0 && entry->string == NULL )
        {
          FT_Memory  memory = face->memory;
          FT_Stream  stream = face->stream;


          if ( FT_NEW_ARRAY  ( entry->string, entry->stringLength ) ||
               FT_STREAM_SEEK( entry->stringOffset )                ||
               FT_STREAM_READ( entry->string, entry->stringLength ) )
          {
            FT_FREE( entry->string );
            entry->stringLength = 0;
          }
        }

        aname->platform_id = entry->platformID;
        aname->encoding_id = entry->encodingID;
        aname->language_id = entry->languageID;
        aname->name_id     = entry->nameID;
        aname->string      = (FT_Byte*)entry->string;
        aname->string_len  = entry->stringLength;

        error = FT_Err_Ok;
      }
    }

    return error;
  }


#endif /* TT_CONFIG_OPTION_SFNT_NAMES */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftstream.c                                                             */
/*                                                                         */
/*    I/O stream support (body).                                           */
/*                                                                         */
/*  Copyright 2000-2002, 2004-2006, 2008-2011, 2013 by                     */
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
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_DEBUG_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_stream


  FT_BASE_DEF( void )
  FT_Stream_OpenMemory( FT_Stream       stream,
                        const FT_Byte*  base,
                        FT_ULong        size )
  {
    stream->base   = (FT_Byte*) base;
    stream->size   = size;
    stream->pos    = 0;
    stream->cursor = 0;
    stream->read   = 0;
    stream->close  = 0;
  }


  FT_BASE_DEF( void )
  FT_Stream_Close( FT_Stream  stream )
  {
    if ( stream && stream->close )
      stream->close( stream );
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_Seek( FT_Stream  stream,
                  FT_ULong   pos )
  {
    FT_Error  error = FT_Err_Ok;


    if ( stream->read )
    {
      if ( stream->read( stream, pos, 0, 0 ) )
      {
        FT_ERROR(( "FT_Stream_Seek:"
                   " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
                   pos, stream->size ));

        error = FT_THROW( Invalid_Stream_Operation );
      }
    }
    /* note that seeking to the first position after the file is valid */
    else if ( pos > stream->size )
    {
      FT_ERROR(( "FT_Stream_Seek:"
                 " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
                 pos, stream->size ));

      error = FT_THROW( Invalid_Stream_Operation );
    }

    if ( !error )
      stream->pos = pos;

    return error;
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_Skip( FT_Stream  stream,
                  FT_Long    distance )
  {
    if ( distance < 0 )
      return FT_THROW( Invalid_Stream_Operation );

    return FT_Stream_Seek( stream, (FT_ULong)( stream->pos + distance ) );
  }


  FT_BASE_DEF( FT_Long )
  FT_Stream_Pos( FT_Stream  stream )
  {
    return stream->pos;
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_Read( FT_Stream  stream,
                  FT_Byte*   buffer,
                  FT_ULong   count )
  {
    return FT_Stream_ReadAt( stream, stream->pos, buffer, count );
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_ReadAt( FT_Stream  stream,
                    FT_ULong   pos,
                    FT_Byte*   buffer,
                    FT_ULong   count )
  {
    FT_Error  error = FT_Err_Ok;
    FT_ULong  read_bytes;


    if ( pos >= stream->size )
    {
      FT_ERROR(( "FT_Stream_ReadAt:"
                 " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
                 pos, stream->size ));

      return FT_THROW( Invalid_Stream_Operation );
    }

    if ( stream->read )
      read_bytes = stream->read( stream, pos, buffer, count );
    else
    {
      read_bytes = stream->size - pos;
      if ( read_bytes > count )
        read_bytes = count;

      FT_MEM_COPY( buffer, stream->base + pos, read_bytes );
    }

    stream->pos = pos + read_bytes;

    if ( read_bytes < count )
    {
      FT_ERROR(( "FT_Stream_ReadAt:"
                 " invalid read; expected %lu bytes, got %lu\n",
                 count, read_bytes ));

      error = FT_THROW( Invalid_Stream_Operation );
    }

    return error;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_TryRead( FT_Stream  stream,
                     FT_Byte*   buffer,
                     FT_ULong   count )
  {
    FT_ULong  read_bytes = 0;


    if ( stream->pos >= stream->size )
      goto Exit;

    if ( stream->read )
      read_bytes = stream->read( stream, stream->pos, buffer, count );
    else
    {
      read_bytes = stream->size - stream->pos;
      if ( read_bytes > count )
        read_bytes = count;

      FT_MEM_COPY( buffer, stream->base + stream->pos, read_bytes );
    }

    stream->pos += read_bytes;

  Exit:
    return read_bytes;
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_ExtractFrame( FT_Stream  stream,
                          FT_ULong   count,
                          FT_Byte**  pbytes )
  {
    FT_Error  error;


    error = FT_Stream_EnterFrame( stream, count );
    if ( !error )
    {
      *pbytes = (FT_Byte*)stream->cursor;

      /* equivalent to FT_Stream_ExitFrame(), with no memory block release */
      stream->cursor = 0;
      stream->limit  = 0;
    }

    return error;
  }


  FT_BASE_DEF( void )
  FT_Stream_ReleaseFrame( FT_Stream  stream,
                          FT_Byte**  pbytes )
  {
    if ( stream && stream->read )
    {
      FT_Memory  memory = stream->memory;

#ifdef FT_DEBUG_MEMORY
      ft_mem_free( memory, *pbytes );
      *pbytes = NULL;
#else
      FT_FREE( *pbytes );
#endif
    }
    *pbytes = 0;
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_EnterFrame( FT_Stream  stream,
                        FT_ULong   count )
  {
    FT_Error  error = FT_Err_Ok;
    FT_ULong  read_bytes;


    /* check for nested frame access */
    FT_ASSERT( stream && stream->cursor == 0 );

    if ( stream->read )
    {
      /* allocate the frame in memory */
      FT_Memory  memory = stream->memory;


      /* simple sanity check */
      if ( count > stream->size )
      {
        FT_ERROR(( "FT_Stream_EnterFrame:"
                   " frame size (%lu) larger than stream size (%lu)\n",
                   count, stream->size ));

        error = FT_THROW( Invalid_Stream_Operation );
        goto Exit;
      }

#ifdef FT_DEBUG_MEMORY
      /* assume _ft_debug_file and _ft_debug_lineno are already set */
      stream->base = (unsigned char*)ft_mem_qalloc( memory, count, &error );
      if ( error )
        goto Exit;
#else
      if ( FT_QALLOC( stream->base, count ) )
        goto Exit;
#endif
      /* read it */
      read_bytes = stream->read( stream, stream->pos,
                                 stream->base, count );
      if ( read_bytes < count )
      {
        FT_ERROR(( "FT_Stream_EnterFrame:"
                   " invalid read; expected %lu bytes, got %lu\n",
                   count, read_bytes ));

        FT_FREE( stream->base );
        error = FT_THROW( Invalid_Stream_Operation );
      }
      stream->cursor = stream->base;
      stream->limit  = stream->cursor + count;
      stream->pos   += read_bytes;
    }
    else
    {
      /* check current and new position */
      if ( stream->pos >= stream->size        ||
           stream->size - stream->pos < count )
      {
        FT_ERROR(( "FT_Stream_EnterFrame:"
                   " invalid i/o; pos = 0x%lx, count = %lu, size = 0x%lx\n",
                   stream->pos, count, stream->size ));

        error = FT_THROW( Invalid_Stream_Operation );
        goto Exit;
      }

      /* set cursor */
      stream->cursor = stream->base + stream->pos;
      stream->limit  = stream->cursor + count;
      stream->pos   += count;
    }

  Exit:
    return error;
  }


  FT_BASE_DEF( void )
  FT_Stream_ExitFrame( FT_Stream  stream )
  {
    /* IMPORTANT: The assertion stream->cursor != 0 was removed, given    */
    /*            that it is possible to access a frame of length 0 in    */
    /*            some weird fonts (usually, when accessing an array of   */
    /*            0 records, like in some strange kern tables).           */
    /*                                                                    */
    /*  In this case, the loader code handles the 0-length table          */
    /*  gracefully; however, stream.cursor is really set to 0 by the      */
    /*  FT_Stream_EnterFrame() call, and this is not an error.            */
    /*                                                                    */
    FT_ASSERT( stream );

    if ( stream->read )
    {
      FT_Memory  memory = stream->memory;

#ifdef FT_DEBUG_MEMORY
      ft_mem_free( memory, stream->base );
      stream->base = NULL;
#else
      FT_FREE( stream->base );
#endif
    }
    stream->cursor = 0;
    stream->limit  = 0;
  }


  FT_BASE_DEF( FT_Char )
  FT_Stream_GetChar( FT_Stream  stream )
  {
    FT_Char  result;


    FT_ASSERT( stream && stream->cursor );

    result = 0;
    if ( stream->cursor < stream->limit )
      result = *stream->cursor++;

    return result;
  }


  FT_BASE_DEF( FT_UShort )
  FT_Stream_GetUShort( FT_Stream  stream )
  {
    FT_Byte*  p;
    FT_Short  result;


    FT_ASSERT( stream && stream->cursor );

    result         = 0;
    p              = stream->cursor;
    if ( p + 1 < stream->limit )
      result       = FT_NEXT_USHORT( p );
    stream->cursor = p;

    return result;
  }


  FT_BASE_DEF( FT_UShort )
  FT_Stream_GetUShortLE( FT_Stream  stream )
  {
    FT_Byte*  p;
    FT_Short  result;


    FT_ASSERT( stream && stream->cursor );

    result         = 0;
    p              = stream->cursor;
    if ( p + 1 < stream->limit )
      result       = FT_NEXT_USHORT_LE( p );
    stream->cursor = p;

    return result;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_GetUOffset( FT_Stream  stream )
  {
    FT_Byte*  p;
    FT_Long   result;


    FT_ASSERT( stream && stream->cursor );

    result         = 0;
    p              = stream->cursor;
    if ( p + 2 < stream->limit )
      result       = FT_NEXT_UOFF3( p );
    stream->cursor = p;
    return result;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_GetULong( FT_Stream  stream )
  {
    FT_Byte*  p;
    FT_Long   result;


    FT_ASSERT( stream && stream->cursor );

    result         = 0;
    p              = stream->cursor;
    if ( p + 3 < stream->limit )
      result       = FT_NEXT_ULONG( p );
    stream->cursor = p;
    return result;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_GetULongLE( FT_Stream  stream )
  {
    FT_Byte*  p;
    FT_Long   result;


    FT_ASSERT( stream && stream->cursor );

    result         = 0;
    p              = stream->cursor;
    if ( p + 3 < stream->limit )
      result       = FT_NEXT_ULONG_LE( p );
    stream->cursor = p;
    return result;
  }


  FT_BASE_DEF( FT_Char )
  FT_Stream_ReadChar( FT_Stream  stream,
                      FT_Error*  error )
  {
    FT_Byte  result = 0;


    FT_ASSERT( stream );

    *error = FT_Err_Ok;

    if ( stream->read )
    {
      if ( stream->read( stream, stream->pos, &result, 1L ) != 1L )
        goto Fail;
    }
    else
    {
      if ( stream->pos < stream->size )
        result = stream->base[stream->pos];
      else
        goto Fail;
    }
    stream->pos++;

    return result;

  Fail:
    *error = FT_THROW( Invalid_Stream_Operation );
    FT_ERROR(( "FT_Stream_ReadChar:"
               " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
               stream->pos, stream->size ));

    return 0;
  }


  FT_BASE_DEF( FT_UShort )
  FT_Stream_ReadUShort( FT_Stream  stream,
                       FT_Error*  error )
  {
    FT_Byte   reads[2];
    FT_Byte*  p = 0;
    FT_Short  result = 0;


    FT_ASSERT( stream );

    *error = FT_Err_Ok;

    if ( stream->pos + 1 < stream->size )
    {
      if ( stream->read )
      {
        if ( stream->read( stream, stream->pos, reads, 2L ) != 2L )
          goto Fail;

        p = reads;
      }
      else
      {
        p = stream->base + stream->pos;
      }

      if ( p )
        result = FT_NEXT_USHORT( p );
    }
    else
      goto Fail;

    stream->pos += 2;

    return result;

  Fail:
    *error = FT_THROW( Invalid_Stream_Operation );
    FT_ERROR(( "FT_Stream_ReadUShort:"
               " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
               stream->pos, stream->size ));

    return 0;
  }


  FT_BASE_DEF( FT_UShort )
  FT_Stream_ReadUShortLE( FT_Stream  stream,
                         FT_Error*  error )
  {
    FT_Byte   reads[2];
    FT_Byte*  p = 0;
    FT_Short  result = 0;


    FT_ASSERT( stream );

    *error = FT_Err_Ok;

    if ( stream->pos + 1 < stream->size )
    {
      if ( stream->read )
      {
        if ( stream->read( stream, stream->pos, reads, 2L ) != 2L )
          goto Fail;

        p = reads;
      }
      else
      {
        p = stream->base + stream->pos;
      }

      if ( p )
        result = FT_NEXT_USHORT_LE( p );
    }
    else
      goto Fail;

    stream->pos += 2;

    return result;

  Fail:
    *error = FT_THROW( Invalid_Stream_Operation );
    FT_ERROR(( "FT_Stream_ReadUShortLE:"
               " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
               stream->pos, stream->size ));

    return 0;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_ReadUOffset( FT_Stream  stream,
                        FT_Error*  error )
  {
    FT_Byte   reads[3];
    FT_Byte*  p = 0;
    FT_Long   result = 0;


    FT_ASSERT( stream );

    *error = FT_Err_Ok;

    if ( stream->pos + 2 < stream->size )
    {
      if ( stream->read )
      {
        if (stream->read( stream, stream->pos, reads, 3L ) != 3L )
          goto Fail;

        p = reads;
      }
      else
      {
        p = stream->base + stream->pos;
      }

      if ( p )
        result = FT_NEXT_UOFF3( p );
    }
    else
      goto Fail;

    stream->pos += 3;

    return result;

  Fail:
    *error = FT_THROW( Invalid_Stream_Operation );
    FT_ERROR(( "FT_Stream_ReadUOffset:"
               " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
               stream->pos, stream->size ));

    return 0;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_ReadULong( FT_Stream  stream,
                      FT_Error*  error )
  {
    FT_Byte   reads[4];
    FT_Byte*  p = 0;
    FT_Long   result = 0;


    FT_ASSERT( stream );

    *error = FT_Err_Ok;

    if ( stream->pos + 3 < stream->size )
    {
      if ( stream->read )
      {
        if ( stream->read( stream, stream->pos, reads, 4L ) != 4L )
          goto Fail;

        p = reads;
      }
      else
      {
        p = stream->base + stream->pos;
      }

      if ( p )
        result = FT_NEXT_ULONG( p );
    }
    else
      goto Fail;

    stream->pos += 4;

    return result;

  Fail:
    *error = FT_THROW( Invalid_Stream_Operation );
    FT_ERROR(( "FT_Stream_ReadULong:"
               " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
               stream->pos, stream->size ));

    return 0;
  }


  FT_BASE_DEF( FT_ULong )
  FT_Stream_ReadULongLE( FT_Stream  stream,
                        FT_Error*  error )
  {
    FT_Byte   reads[4];
    FT_Byte*  p = 0;
    FT_Long   result = 0;


    FT_ASSERT( stream );

    *error = FT_Err_Ok;

    if ( stream->pos + 3 < stream->size )
    {
      if ( stream->read )
      {
        if ( stream->read( stream, stream->pos, reads, 4L ) != 4L )
          goto Fail;

        p = reads;
      }
      else
      {
        p = stream->base + stream->pos;
      }

      if ( p )
        result = FT_NEXT_ULONG_LE( p );
    }
    else
      goto Fail;

    stream->pos += 4;

    return result;

  Fail:
    *error = FT_THROW( Invalid_Stream_Operation );
    FT_ERROR(( "FT_Stream_ReadULongLE:"
               " invalid i/o; pos = 0x%lx, size = 0x%lx\n",
               stream->pos, stream->size ));

    return 0;
  }


  FT_BASE_DEF( FT_Error )
  FT_Stream_ReadFields( FT_Stream              stream,
                        const FT_Frame_Field*  fields,
                        void*                  structure )
  {
    FT_Error  error;
    FT_Bool   frame_accessed = 0;
    FT_Byte*  cursor;


    if ( !fields || !stream )
      return FT_THROW( Invalid_Argument );

    cursor = stream->cursor;

    error = FT_Err_Ok;
    do
    {
      FT_ULong  value;
      FT_Int    sign_shift;
      FT_Byte*  p;


      switch ( fields->value )
      {
      case ft_frame_start:  /* access a new frame */
        error = FT_Stream_EnterFrame( stream, fields->offset );
        if ( error )
          goto Exit;

        frame_accessed = 1;
        cursor         = stream->cursor;
        fields++;
        continue;  /* loop! */

      case ft_frame_bytes:  /* read a byte sequence */
      case ft_frame_skip:   /* skip some bytes      */
        {
          FT_UInt  len = fields->size;


          if ( cursor + len > stream->limit )
          {
            error = FT_THROW( Invalid_Stream_Operation );
            goto Exit;
          }

          if ( fields->value == ft_frame_bytes )
          {
            p = (FT_Byte*)structure + fields->offset;
            FT_MEM_COPY( p, cursor, len );
          }
          cursor += len;
          fields++;
          continue;
        }

      case ft_frame_byte:
      case ft_frame_schar:  /* read a single byte */
        value = FT_NEXT_BYTE( cursor );
        sign_shift = 24;
        break;

      case ft_frame_short_be:
      case ft_frame_ushort_be:  /* read a 2-byte big-endian short */
        value = FT_NEXT_USHORT( cursor) ;
        sign_shift = 16;
        break;

      case ft_frame_short_le:
      case ft_frame_ushort_le:  /* read a 2-byte little-endian short */
        value = FT_NEXT_USHORT_LE( cursor );
        sign_shift = 16;
        break;

      case ft_frame_long_be:
      case ft_frame_ulong_be:  /* read a 4-byte big-endian long */
        value = FT_NEXT_ULONG( cursor );
        sign_shift = 0;
        break;

      case ft_frame_long_le:
      case ft_frame_ulong_le:  /* read a 4-byte little-endian long */
        value = FT_NEXT_ULONG_LE( cursor );
        sign_shift = 0;
        break;

      case ft_frame_off3_be:
      case ft_frame_uoff3_be:  /* read a 3-byte big-endian long */
        value = FT_NEXT_UOFF3( cursor );
        sign_shift = 8;
        break;

      case ft_frame_off3_le:
      case ft_frame_uoff3_le:  /* read a 3-byte little-endian long */
        value = FT_NEXT_UOFF3_LE( cursor );
        sign_shift = 8;
        break;

      default:
        /* otherwise, exit the loop */
        stream->cursor = cursor;
        goto Exit;
      }

      /* now, compute the signed value is necessary */
      if ( fields->value & FT_FRAME_OP_SIGNED )
        value = (FT_ULong)( (FT_Int32)( value << sign_shift ) >> sign_shift );

      /* finally, store the value in the object */

      p = (FT_Byte*)structure + fields->offset;
      switch ( fields->size )
      {
      case ( 8 / FT_CHAR_BIT ):
        *(FT_Byte*)p = (FT_Byte)value;
        break;

      case ( 16 / FT_CHAR_BIT ):
        *(FT_UShort*)p = (FT_UShort)value;
        break;

      case ( 32 / FT_CHAR_BIT ):
        *(FT_UInt32*)p = (FT_UInt32)value;
        break;

      default:  /* for 64-bit systems */
        *(FT_ULong*)p = (FT_ULong)value;
      }

      /* go to next field */
      fields++;
    }
    while ( 1 );

  Exit:
    /* close the frame if it was opened by this read */
    if ( frame_accessed )
      FT_Stream_ExitFrame( stream );

    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  fttrigon.c                                                             */
/*                                                                         */
/*    FreeType trigonometric functions (body).                             */
/*                                                                         */
/*  Copyright 2001-2005, 2012-2013 by                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* This is a fixed-point CORDIC implementation of trigonometric          */
  /* functions as well as transformations between Cartesian and polar      */
  /* coordinates.  The angles are represented as 16.16 fixed-point values  */
  /* in degrees, i.e., the angular resolution is 2^-16 degrees.  Note that */
  /* only vectors longer than 2^16*180/pi (or at least 22 bits) on a       */
  /* discrete Cartesian grid can have the same or better angular           */
  /* resolution.  Therefore, to maintain this precision, some functions    */
  /* require an interim upscaling of the vectors, whereas others operate   */
  /* with 24-bit long vectors directly.                                    */
  /*                                                                       */
  /*************************************************************************/

#include "ft2build.h"
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_CALC_H
#include FT_TRIGONOMETRY_H


  /* the Cordic shrink factor 0.858785336480436 * 2^32 */
#define FT_TRIG_SCALE      0xDBD95B16UL

  /* the highest bit in overflow-safe vector components, */
  /* MSB of 0.858785336480436 * sqrt(0.5) * 2^30         */
#define FT_TRIG_SAFE_MSB   29

  /* this table was generated for FT_PI = 180L << 16, i.e. degrees */
#define FT_TRIG_MAX_ITERS  23

  static const FT_Fixed
  ft_trig_arctan_table[] =
  {
    1740967L, 919879L, 466945L, 234379L, 117304L, 58666L, 29335L,
    14668L, 7334L, 3667L, 1833L, 917L, 458L, 229L, 115L,
    57L, 29L, 14L, 7L, 4L, 2L, 1L
  };


#ifdef FT_LONG64

  /* multiply a given value by the CORDIC shrink factor */
  static FT_Fixed
  ft_trig_downscale( FT_Fixed  val )
  {
    FT_Fixed  s;
    FT_Int64  v;


    s   = val;
    val = FT_ABS( val );

    v   = ( val * (FT_Int64)FT_TRIG_SCALE ) + 0x100000000UL;
    val = (FT_Fixed)( v >> 32 );

    return ( s >= 0 ) ? val : -val;
  }

#else /* !FT_LONG64 */

  /* multiply a given value by the CORDIC shrink factor */
  static FT_Fixed
  ft_trig_downscale( FT_Fixed  val )
  {
    FT_Fixed   s;
    FT_UInt32  v1, v2, k1, k2, hi, lo1, lo2, lo3;


    s   = val;
    val = FT_ABS( val );

    v1 = (FT_UInt32)val >> 16;
    v2 = (FT_UInt32)( val & 0xFFFFL );

    k1 = (FT_UInt32)FT_TRIG_SCALE >> 16;           /* constant */
    k2 = (FT_UInt32)( FT_TRIG_SCALE & 0xFFFFL );   /* constant */

    hi   = k1 * v1;
    lo1  = k1 * v2 + k2 * v1;       /* can't overflow */

    lo2  = ( k2 * v2 ) >> 16;
    lo3  = FT_MAX( lo1, lo2 );
    lo1 += lo2;

    hi  += lo1 >> 16;
    if ( lo1 < lo3 )
      hi += (FT_UInt32)0x10000UL;

    val  = (FT_Fixed)hi;

    return ( s >= 0 ) ? val : -val;
  }

#endif /* !FT_LONG64 */


  static FT_Int
  ft_trig_prenorm( FT_Vector*  vec )
  {
    FT_Pos  x, y;
    FT_Int  shift;


    x = vec->x;
    y = vec->y;

    shift = FT_MSB( FT_ABS( x ) | FT_ABS( y ) );

    if ( shift <= FT_TRIG_SAFE_MSB )
    {
      shift  = FT_TRIG_SAFE_MSB - shift;
      vec->x = (FT_Pos)( (FT_ULong)x << shift );
      vec->y = (FT_Pos)( (FT_ULong)y << shift );
    }
    else
    {
      shift -= FT_TRIG_SAFE_MSB;
      vec->x = x >> shift;
      vec->y = y >> shift;
      shift  = -shift;
    }

    return shift;
  }


  static void
  ft_trig_pseudo_rotate( FT_Vector*  vec,
                         FT_Angle    theta )
  {
    FT_Int           i;
    FT_Fixed         x, y, xtemp, b;
    const FT_Fixed  *arctanptr;


    x = vec->x;
    y = vec->y;

    /* Rotate inside [-PI/4,PI/4] sector */
    while ( theta < -FT_ANGLE_PI4 )
    {
      xtemp  =  y;
      y      = -x;
      x      =  xtemp;
      theta +=  FT_ANGLE_PI2;
    }

    while ( theta > FT_ANGLE_PI4 )
    {
      xtemp  = -y;
      y      =  x;
      x      =  xtemp;
      theta -=  FT_ANGLE_PI2;
    }

    arctanptr = ft_trig_arctan_table;

    /* Pseudorotations, with right shifts */
    for ( i = 1, b = 1; i < FT_TRIG_MAX_ITERS; b <<= 1, i++ )
    {
      if ( theta < 0 )
      {
        xtemp  = x + ( ( y + b ) >> i );
        y      = y - ( ( x + b ) >> i );
        x      = xtemp;
        theta += *arctanptr++;
      }
      else
      {
        xtemp  = x - ( ( y + b ) >> i );
        y      = y + ( ( x + b ) >> i );
        x      = xtemp;
        theta -= *arctanptr++;
      }
    }

    vec->x = x;
    vec->y = y;
  }


  static void
  ft_trig_pseudo_polarize( FT_Vector*  vec )
  {
    FT_Angle         theta;
    FT_Int           i;
    FT_Fixed         x, y, xtemp, b;
    const FT_Fixed  *arctanptr;


    x = vec->x;
    y = vec->y;

    /* Get the vector into [-PI/4,PI/4] sector */
    if ( y > x )
    {
      if ( y > -x )
      {
        theta =  FT_ANGLE_PI2;
        xtemp =  y;
        y     = -x;
        x     =  xtemp;
      }
      else
      {
        theta =  y > 0 ? FT_ANGLE_PI : -FT_ANGLE_PI;
        x     = -x;
        y     = -y;
      }
    }
    else
    {
      if ( y < -x )
      {
        theta = -FT_ANGLE_PI2;
        xtemp = -y;
        y     =  x;
        x     =  xtemp;
      }
      else
      {
        theta = 0;
      }
    }

    arctanptr = ft_trig_arctan_table;

    /* Pseudorotations, with right shifts */
    for ( i = 1, b = 1; i < FT_TRIG_MAX_ITERS; b <<= 1, i++ )
    {
      if ( y > 0 )
      {
        xtemp  = x + ( ( y + b ) >> i );
        y      = y - ( ( x + b ) >> i );
        x      = xtemp;
        theta += *arctanptr++;
      }
      else
      {
        xtemp  = x - ( ( y + b ) >> i );
        y      = y + ( ( x + b ) >> i );
        x      = xtemp;
        theta -= *arctanptr++;
      }
    }

    /* round theta */
    if ( theta >= 0 )
      theta = FT_PAD_ROUND( theta, 32 );
    else
      theta = -FT_PAD_ROUND( -theta, 32 );

    vec->x = x;
    vec->y = theta;
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_Cos( FT_Angle  angle )
  {
    FT_Vector  v;


    v.x = FT_TRIG_SCALE >> 8;
    v.y = 0;
    ft_trig_pseudo_rotate( &v, angle );

    return ( v.x + 0x80L ) >> 8;
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_Sin( FT_Angle  angle )
  {
    return FT_Cos( FT_ANGLE_PI2 - angle );
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_Tan( FT_Angle  angle )
  {
    FT_Vector  v;


    v.x = FT_TRIG_SCALE >> 8;
    v.y = 0;
    ft_trig_pseudo_rotate( &v, angle );

    return FT_DivFix( v.y, v.x );
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( FT_Angle )
  FT_Atan2( FT_Fixed  dx,
            FT_Fixed  dy )
  {
    FT_Vector  v;


    if ( dx == 0 && dy == 0 )
      return 0;

    v.x = dx;
    v.y = dy;
    ft_trig_prenorm( &v );
    ft_trig_pseudo_polarize( &v );

    return v.y;
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( void )
  FT_Vector_Unit( FT_Vector*  vec,
                  FT_Angle    angle )
  {
    vec->x = FT_TRIG_SCALE >> 8;
    vec->y = 0;
    ft_trig_pseudo_rotate( vec, angle );
    vec->x = ( vec->x + 0x80L ) >> 8;
    vec->y = ( vec->y + 0x80L ) >> 8;
  }


  /* these macros return 0 for positive numbers,
     and -1 for negative ones */
#define FT_SIGN_LONG( x )   ( (x) >> ( FT_SIZEOF_LONG * 8 - 1 ) )
#define FT_SIGN_INT( x )    ( (x) >> ( FT_SIZEOF_INT * 8 - 1 ) )
#define FT_SIGN_INT32( x )  ( (x) >> 31 )
#define FT_SIGN_INT16( x )  ( (x) >> 15 )


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( void )
  FT_Vector_Rotate( FT_Vector*  vec,
                    FT_Angle    angle )
  {
    FT_Int     shift;
    FT_Vector  v;


    v.x   = vec->x;
    v.y   = vec->y;

    if ( angle && ( v.x != 0 || v.y != 0 ) )
    {
      shift = ft_trig_prenorm( &v );
      ft_trig_pseudo_rotate( &v, angle );
      v.x = ft_trig_downscale( v.x );
      v.y = ft_trig_downscale( v.y );

      if ( shift > 0 )
      {
        FT_Int32  half = (FT_Int32)1L << ( shift - 1 );


        vec->x = ( v.x + half + FT_SIGN_LONG( v.x ) ) >> shift;
        vec->y = ( v.y + half + FT_SIGN_LONG( v.y ) ) >> shift;
      }
      else
      {
        shift  = -shift;
        vec->x = (FT_Pos)( (FT_ULong)v.x << shift );
        vec->y = (FT_Pos)( (FT_ULong)v.y << shift );
      }
    }
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( FT_Fixed )
  FT_Vector_Length( FT_Vector*  vec )
  {
    FT_Int     shift;
    FT_Vector  v;


    v = *vec;

    /* handle trivial cases */
    if ( v.x == 0 )
    {
      return FT_ABS( v.y );
    }
    else if ( v.y == 0 )
    {
      return FT_ABS( v.x );
    }

    /* general case */
    shift = ft_trig_prenorm( &v );
    ft_trig_pseudo_polarize( &v );

    v.x = ft_trig_downscale( v.x );

    if ( shift > 0 )
      return ( v.x + ( 1 << ( shift - 1 ) ) ) >> shift;

    return (FT_Fixed)( (FT_UInt32)v.x << -shift );
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( void )
  FT_Vector_Polarize( FT_Vector*  vec,
                      FT_Fixed   *length,
                      FT_Angle   *angle )
  {
    FT_Int     shift;
    FT_Vector  v;


    v = *vec;

    if ( v.x == 0 && v.y == 0 )
      return;

    shift = ft_trig_prenorm( &v );
    ft_trig_pseudo_polarize( &v );

    v.x = ft_trig_downscale( v.x );

    *length = ( shift >= 0 ) ?                      ( v.x >>  shift )
                             : (FT_Fixed)( (FT_UInt32)v.x << -shift );
    *angle  = v.y;
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( void )
  FT_Vector_From_Polar( FT_Vector*  vec,
                        FT_Fixed    length,
                        FT_Angle    angle )
  {
    vec->x = length;
    vec->y = 0;

    FT_Vector_Rotate( vec, angle );
  }


  /* documentation is in fttrigon.h */

  FT_EXPORT_DEF( FT_Angle )
  FT_Angle_Diff( FT_Angle  angle1,
                 FT_Angle  angle2 )
  {
    FT_Angle  delta = angle2 - angle1;


    delta %= FT_ANGLE_2PI;
    if ( delta < 0 )
      delta += FT_ANGLE_2PI;

    if ( delta > FT_ANGLE_PI )
      delta -= FT_ANGLE_2PI;

    return delta;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ftutil.c                                                               */
/*                                                                         */
/*    FreeType utility file for memory and list management (body).         */
/*                                                                         */
/*  Copyright 2002, 2004-2007, 2013 by                                     */
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
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_MEMORY_H
#include FT_INTERNAL_OBJECTS_H
#include FT_LIST_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_memory


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                                                               *****/
  /*****               M E M O R Y   M A N A G E M E N T               *****/
  /*****                                                               *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  FT_BASE_DEF( FT_Pointer )
  ft_mem_alloc( FT_Memory  memory,
                FT_Long    size,
                FT_Error  *p_error )
  {
    FT_Error    error;
    FT_Pointer  block = ft_mem_qalloc( memory, size, &error );

    if ( !error && size > 0 )
      FT_MEM_ZERO( block, size );

    *p_error = error;
    return block;
  }


  FT_BASE_DEF( FT_Pointer )
  ft_mem_qalloc( FT_Memory  memory,
                 FT_Long    size,
                 FT_Error  *p_error )
  {
    FT_Error    error = FT_Err_Ok;
    FT_Pointer  block = NULL;


    if ( size > 0 )
    {
      block = memory->alloc( memory, size );
      if ( block == NULL )
        error = FT_THROW( Out_Of_Memory );
    }
    else if ( size < 0 )
    {
      /* may help catch/prevent security issues */
      error = FT_THROW( Invalid_Argument );
    }

    *p_error = error;
    return block;
  }


  FT_BASE_DEF( FT_Pointer )
  ft_mem_realloc( FT_Memory  memory,
                  FT_Long    item_size,
                  FT_Long    cur_count,
                  FT_Long    new_count,
                  void*      block,
                  FT_Error  *p_error )
  {
    FT_Error  error = FT_Err_Ok;


    block = ft_mem_qrealloc( memory, item_size,
                             cur_count, new_count, block, &error );
    if ( !error && new_count > cur_count )
      FT_MEM_ZERO( (char*)block + cur_count * item_size,
                   ( new_count - cur_count ) * item_size );

    *p_error = error;
    return block;
  }


  FT_BASE_DEF( FT_Pointer )
  ft_mem_qrealloc( FT_Memory  memory,
                   FT_Long    item_size,
                   FT_Long    cur_count,
                   FT_Long    new_count,
                   void*      block,
                   FT_Error  *p_error )
  {
    FT_Error  error = FT_Err_Ok;


    /* Note that we now accept `item_size == 0' as a valid parameter, in
     * order to cover very weird cases where an ALLOC_MULT macro would be
     * called.
     */
    if ( cur_count < 0 || new_count < 0 || item_size < 0 )
    {
      /* may help catch/prevent nasty security issues */
      error = FT_THROW( Invalid_Argument );
    }
    else if ( new_count == 0 || item_size == 0 )
    {
      ft_mem_free( memory, block );
      block = NULL;
    }
    else if ( new_count > FT_INT_MAX/item_size )
    {
      error = FT_THROW( Array_Too_Large );
    }
    else if ( cur_count == 0 )
    {
      FT_ASSERT( block == NULL );

      block = ft_mem_alloc( memory, new_count*item_size, &error );
    }
    else
    {
      FT_Pointer  block2;
      FT_Long     cur_size = cur_count*item_size;
      FT_Long     new_size = new_count*item_size;


      block2 = memory->realloc( memory, cur_size, new_size, block );
      if ( block2 == NULL )
        error = FT_THROW( Out_Of_Memory );
      else
        block = block2;
    }

    *p_error = error;
    return block;
  }


  FT_BASE_DEF( void )
  ft_mem_free( FT_Memory   memory,
               const void *P )
  {
    if ( P )
      memory->free( memory, (void*)P );
  }


  FT_BASE_DEF( FT_Pointer )
  ft_mem_dup( FT_Memory    memory,
              const void*  address,
              FT_ULong     size,
              FT_Error    *p_error )
  {
    FT_Error    error;
    FT_Pointer  p = ft_mem_qalloc( memory, size, &error );


    if ( !error && address )
      ft_memcpy( p, address, size );

    *p_error = error;
    return p;
  }


  FT_BASE_DEF( FT_Pointer )
  ft_mem_strdup( FT_Memory    memory,
                 const char*  str,
                 FT_Error    *p_error )
  {
    FT_ULong  len = str ? (FT_ULong)ft_strlen( str ) + 1
                        : 0;


    return ft_mem_dup( memory, str, len, p_error );
  }


  FT_BASE_DEF( FT_Int )
  ft_mem_strcpyn( char*        dst,
                  const char*  src,
                  FT_ULong     size )
  {
    while ( size > 1 && *src != 0 )
    {
      *dst++ = *src++;
      size--;
    }

    *dst = 0;  /* always zero-terminate */

    return *src != 0;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                                                               *****/
  /*****            D O U B L Y   L I N K E D   L I S T S              *****/
  /*****                                                               *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_list

  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( FT_ListNode )
  FT_List_Find( FT_List  list,
                void*    data )
  {
    FT_ListNode  cur;


    cur = list->head;
    while ( cur )
    {
      if ( cur->data == data )
        return cur;

      cur = cur->next;
    }

    return (FT_ListNode)0;
  }


  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( void )
  FT_List_Add( FT_List      list,
               FT_ListNode  node )
  {
    FT_ListNode  before = list->tail;


    node->next = 0;
    node->prev = before;

    if ( before )
      before->next = node;
    else
      list->head = node;

    list->tail = node;
  }


  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( void )
  FT_List_Insert( FT_List      list,
                  FT_ListNode  node )
  {
    FT_ListNode  after = list->head;


    node->next = after;
    node->prev = 0;

    if ( !after )
      list->tail = node;
    else
      after->prev = node;

    list->head = node;
  }


  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( void )
  FT_List_Remove( FT_List      list,
                  FT_ListNode  node )
  {
    FT_ListNode  before, after;


    before = node->prev;
    after  = node->next;

    if ( before )
      before->next = after;
    else
      list->head = after;

    if ( after )
      after->prev = before;
    else
      list->tail = before;
  }


  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( void )
  FT_List_Up( FT_List      list,
              FT_ListNode  node )
  {
    FT_ListNode  before, after;


    before = node->prev;
    after  = node->next;

    /* check whether we are already on top of the list */
    if ( !before )
      return;

    before->next = after;

    if ( after )
      after->prev = before;
    else
      list->tail = before;

    node->prev       = 0;
    node->next       = list->head;
    list->head->prev = node;
    list->head       = node;
  }


  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( FT_Error )
  FT_List_Iterate( FT_List            list,
                   FT_List_Iterator   iterator,
                   void*              user )
  {
    FT_ListNode  cur   = list->head;
    FT_Error     error = FT_Err_Ok;


    while ( cur )
    {
      FT_ListNode  next = cur->next;


      error = iterator( cur, user );
      if ( error )
        break;

      cur = next;
    }

    return error;
  }


  /* documentation is in ftlist.h */

  FT_EXPORT_DEF( void )
  FT_List_Finalize( FT_List             list,
                    FT_List_Destructor  destroy,
                    FT_Memory           memory,
                    void*               user )
  {
    FT_ListNode  cur;


    cur = list->head;
    while ( cur )
    {
      FT_ListNode  next = cur->next;
      void*        data = cur->data;


      if ( destroy )
        destroy( memory, data, user );

      FT_FREE( cur );
      cur = next;
    }

    list->head = 0;
    list->tail = 0;
  }


  FT_BASE_DEF( FT_UInt32 )
  ft_highpow2( FT_UInt32  value )
  {
    FT_UInt32  value2;


    /*
     *  We simply clear the lowest bit in each iteration.  When
     *  we reach 0, we know that the previous value was our result.
     */
    for ( ;; )
    {
      value2 = value & (value - 1);  /* clear lowest bit */
      if ( value2 == 0 )
        break;

      value = value2;
    }
    return value;
  }


/* END */

#ifdef FT_MACINTOSH
/***************************************************************************/
/*                                                                         */
/*  ftmac.c                                                                */
/*                                                                         */
/*    Mac FOND support.  Written by just@letterror.com.                    */
/*  Heavily modified by mpsuzuki, George Williams, and Sean McBride.       */
/*                                                                         */
/*  This file is for Mac OS X only; see builds/mac/ftoldmac.c for          */
/*  classic platforms built by MPW.                                        */
/*                                                                         */
/*  Copyright 1996-2009, 2013 by                                           */
/*  Just van Rossum, David Turner, Robert Wilhelm, and Werner Lemberg.     */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*
    Notes

    Mac suitcase files can (and often do!) contain multiple fonts.  To
    support this I use the face_index argument of FT_(Open|New)_Face()
    functions, and pretend the suitcase file is a collection.

    Warning: fbit and NFNT bitmap resources are not supported yet.  In old
    sfnt fonts, bitmap glyph data for each size is stored in each `NFNT'
    resources instead of the `bdat' table in the sfnt resource.  Therefore,
    face->num_fixed_sizes is set to 0, because bitmap data in `NFNT'
    resource is unavailable at present.

    The Mac FOND support works roughly like this:

    - Check whether the offered stream points to a Mac suitcase file.  This
      is done by checking the file type: it has to be 'FFIL' or 'tfil'.  The
      stream that gets passed to our init_face() routine is a stdio stream,
      which isn't usable for us, since the FOND resources live in the
      resource fork.  So we just grab the stream->pathname field.

    - Read the FOND resource into memory, then check whether there is a
      TrueType font and/or(!) a Type 1 font available.

    - If there is a Type 1 font available (as a separate `LWFN' file), read
      its data into memory, massage it slightly so it becomes PFB data, wrap
      it into a memory stream, load the Type 1 driver and delegate the rest
      of the work to it by calling FT_Open_Face().  (XXX TODO: after this
      has been done, the kerning data from the FOND resource should be
      appended to the face: On the Mac there are usually no AFM files
      available.  However, this is tricky since we need to map Mac char
      codes to ps glyph names to glyph ID's...)

    - If there is a TrueType font (an `sfnt' resource), read it into memory,
      wrap it into a memory stream, load the TrueType driver and delegate
      the rest of the work to it, by calling FT_Open_Face().

    - Some suitcase fonts (notably Onyx) might point the `LWFN' file to
      itself, even though it doesn't contains `POST' resources.  To handle
      this special case without opening the file an extra time, we just
      ignore errors from the `LWFN' and fallback to the `sfnt' if both are
      available.
  */


#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_TRUETYPE_TAGS_H
#include FT_INTERNAL_STREAM_H
#include "ftbase.h"

  /* This is for Mac OS X.  Without redefinition, OS_INLINE */
  /* expands to `static inline' which doesn't survive the   */
  /* -ansi compilation flag of GCC.                         */
#if !HAVE_ANSI_OS_INLINE
#undef  OS_INLINE
#define OS_INLINE  static __inline__
#endif

  /* `configure' checks the availability of `ResourceIndex' strictly */
  /* and sets HAVE_TYPE_RESOURCE_INDEX 1 or 0 always.  If it is      */
  /* not set (e.g., a build without `configure'), the availability   */
  /* is guessed from the SDK version.                                */
#ifndef HAVE_TYPE_RESOURCE_INDEX
#if !defined( MAC_OS_X_VERSION_10_5 ) || \
    ( MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5 )
#define HAVE_TYPE_RESOURCE_INDEX 0
#else
#define HAVE_TYPE_RESOURCE_INDEX 1
#endif
#endif /* !HAVE_TYPE_RESOURCE_INDEX */

#if ( HAVE_TYPE_RESOURCE_INDEX == 0 )
  typedef short  ResourceIndex;
#endif

#include <CoreServices/CoreServices.h>
#include <ApplicationServices/ApplicationServices.h>
#include <sys/syslimits.h> /* PATH_MAX */

  /* Don't want warnings about our own use of deprecated functions. */
#define FT_DEPRECATED_ATTRIBUTE

#include FT_MAC_H

#ifndef kATSOptionFlagsUnRestrictedScope /* since Mac OS X 10.1 */
#define kATSOptionFlagsUnRestrictedScope kATSOptionFlagsDefault
#endif


  /* Set PREFER_LWFN to 1 if LWFN (Type 1) is preferred over
     TrueType in case *both* are available (this is not common,
     but it *is* possible). */
#ifndef PREFER_LWFN
#define PREFER_LWFN  1
#endif


#ifdef FT_MACINTOSH

  /* This function is deprecated because FSSpec is deprecated in Mac OS X  */
  FT_EXPORT_DEF( FT_Error )
  FT_GetFile_From_Mac_Name( const char*  fontName,
                            FSSpec*      pathSpec,
                            FT_Long*     face_index )
  {
    FT_UNUSED( fontName );
    FT_UNUSED( pathSpec );
    FT_UNUSED( face_index );

    return FT_THROW( Unimplemented_Feature );
  }


  /* Private function.                                         */
  /* The FSSpec type has been discouraged for a long time,     */
  /* unfortunately an FSRef replacement API for                */
  /* ATSFontGetFileSpecification() is only available in        */
  /* Mac OS X 10.5 and later.                                  */
  static OSStatus
  FT_ATSFontGetFileReference( ATSFontRef  ats_font_id,
                              FSRef*      ats_font_ref )
  {
#if defined( MAC_OS_X_VERSION_10_5 ) && \
    ( MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 )

    OSStatus  err;

    err = ATSFontGetFileReference( ats_font_id, ats_font_ref );

    return err;
#elif __LP64__ /* No 64bit Carbon API on legacy platforms */
    FT_UNUSED( ats_font_id );
    FT_UNUSED( ats_font_ref );


    return fnfErr;
#else /* 32bit Carbon API on legacy platforms */
    OSStatus  err;
    FSSpec    spec;


    err = ATSFontGetFileSpecification( ats_font_id, &spec );
    if ( noErr == err )
      err = FSpMakeFSRef( &spec, ats_font_ref );

    return err;
#endif
  }


  static FT_Error
  FT_GetFileRef_From_Mac_ATS_Name( const char*  fontName,
                                   FSRef*       ats_font_ref,
                                   FT_Long*     face_index )
  {
    CFStringRef  cf_fontName;
    ATSFontRef   ats_font_id;


    *face_index = 0;

    cf_fontName = CFStringCreateWithCString( NULL, fontName,
                                             kCFStringEncodingMacRoman );
    ats_font_id = ATSFontFindFromName( cf_fontName,
                                       kATSOptionFlagsUnRestrictedScope );
    CFRelease( cf_fontName );

    if ( ats_font_id == 0 || ats_font_id == 0xFFFFFFFFUL )
      return FT_THROW( Unknown_File_Format );

    if ( noErr != FT_ATSFontGetFileReference( ats_font_id, ats_font_ref ) )
      return FT_THROW( Unknown_File_Format );

    /* face_index calculation by searching preceding fontIDs */
    /* with same FSRef                                       */
    {
      ATSFontRef  id2 = ats_font_id - 1;
      FSRef       ref2;


      while ( id2 > 0 )
      {
        if ( noErr != FT_ATSFontGetFileReference( id2, &ref2 ) )
          break;
        if ( noErr != FSCompareFSRefs( ats_font_ref, &ref2 ) )
          break;

        id2 --;
      }
      *face_index = ats_font_id - ( id2 + 1 );
    }

    return FT_Err_Ok;
  }


  FT_EXPORT_DEF( FT_Error )
  FT_GetFilePath_From_Mac_ATS_Name( const char*  fontName,
                                    UInt8*       path,
                                    UInt32       maxPathSize,
                                    FT_Long*     face_index )
  {
    FSRef     ref;
    FT_Error  err;


    err = FT_GetFileRef_From_Mac_ATS_Name( fontName, &ref, face_index );
    if ( err )
      return err;

    if ( noErr != FSRefMakePath( &ref, path, maxPathSize ) )
      return FT_THROW( Unknown_File_Format );

    return FT_Err_Ok;
  }


  /* This function is deprecated because FSSpec is deprecated in Mac OS X  */
  FT_EXPORT_DEF( FT_Error )
  FT_GetFile_From_Mac_ATS_Name( const char*  fontName,
                                FSSpec*      pathSpec,
                                FT_Long*     face_index )
  {
#if ( __LP64__ ) || ( defined( MAC_OS_X_VERSION_10_5 ) && \
      ( MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 ) )
    FT_UNUSED( fontName );
    FT_UNUSED( pathSpec );
    FT_UNUSED( face_index );

    return FT_THROW( Unimplemented_Feature );
#else
    FSRef     ref;
    FT_Error  err;


    err = FT_GetFileRef_From_Mac_ATS_Name( fontName, &ref, face_index );
    if ( err )
      return err;

    if ( noErr != FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, NULL,
                                    pathSpec, NULL ) )
      return FT_THROW( Unknown_File_Format );

    return FT_Err_Ok;
#endif
  }


  static OSErr
  FT_FSPathMakeRes( const UInt8*    pathname,
                    ResFileRefNum*  res )
  {
    OSErr  err;
    FSRef  ref;


    if ( noErr != FSPathMakeRef( pathname, &ref, FALSE ) )
      return FT_THROW( Cannot_Open_Resource );

    /* at present, no support for dfont format */
    err = FSOpenResourceFile( &ref, 0, NULL, fsRdPerm, res );
    if ( noErr == err )
      return err;

    /* fallback to original resource-fork font */
    *res = FSOpenResFile( &ref, fsRdPerm );
    err  = ResError();

    return err;
  }


  /* Return the file type for given pathname */
  static OSType
  get_file_type_from_path( const UInt8*  pathname )
  {
    FSRef          ref;
    FSCatalogInfo  info;


    if ( noErr != FSPathMakeRef( pathname, &ref, FALSE ) )
      return ( OSType ) 0;

    if ( noErr != FSGetCatalogInfo( &ref, kFSCatInfoFinderInfo, &info,
                                    NULL, NULL, NULL ) )
      return ( OSType ) 0;

    return ((FInfo *)(info.finderInfo))->fdType;
  }


  /* Given a PostScript font name, create the Macintosh LWFN file name. */
  static void
  create_lwfn_name( char*   ps_name,
                    Str255  lwfn_file_name )
  {
    int       max = 5, count = 0;
    FT_Byte*  p = lwfn_file_name;
    FT_Byte*  q = (FT_Byte*)ps_name;


    lwfn_file_name[0] = 0;

    while ( *q )
    {
      if ( ft_isupper( *q ) )
      {
        if ( count )
          max = 3;
        count = 0;
      }
      if ( count < max && ( ft_isalnum( *q ) || *q == '_' ) )
      {
        *++p = *q;
        lwfn_file_name[0]++;
        count++;
      }
      q++;
    }
  }


  static short
  count_faces_sfnt( char*  fond_data )
  {
    /* The count is 1 greater than the value in the FOND.  */
    /* Isn't that cute? :-)                                */

    return EndianS16_BtoN( *( (short*)( fond_data +
                                        sizeof ( FamRec ) ) ) ) + 1;
  }


  static short
  count_faces_scalable( char*  fond_data )
  {
    AsscEntry*  assoc;
    short       i, face, face_all;


    face_all = EndianS16_BtoN( *( (short *)( fond_data +
                                             sizeof ( FamRec ) ) ) ) + 1;
    assoc    = (AsscEntry*)( fond_data + sizeof ( FamRec ) + 2 );
    face     = 0;

    for ( i = 0; i < face_all; i++ )
    {
      if ( 0 == EndianS16_BtoN( assoc[i].fontSize ) )
        face++;
    }
    return face;
  }


  /* Look inside the FOND data, answer whether there should be an SFNT
     resource, and answer the name of a possible LWFN Type 1 file.

     Thanks to Paul Miller (paulm@profoundeffects.com) for the fix
     to load a face OTHER than the first one in the FOND!
  */


  static void
  parse_fond( char*   fond_data,
              short*  have_sfnt,
              ResID*  sfnt_id,
              Str255  lwfn_file_name,
              short   face_index )
  {
    AsscEntry*  assoc;
    AsscEntry*  base_assoc;
    FamRec*     fond;


    *sfnt_id          = 0;
    *have_sfnt        = 0;
    lwfn_file_name[0] = 0;

    fond       = (FamRec*)fond_data;
    assoc      = (AsscEntry*)( fond_data + sizeof ( FamRec ) + 2 );
    base_assoc = assoc;

    /* the maximum faces in a FOND is 48, size of StyleTable.indexes[] */
    if ( 47 < face_index )
      return;

    /* Let's do a little range checking before we get too excited here */
    if ( face_index < count_faces_sfnt( fond_data ) )
    {
      assoc += face_index;        /* add on the face_index! */

      /* if the face at this index is not scalable,
         fall back to the first one (old behavior) */
      if ( EndianS16_BtoN( assoc->fontSize ) == 0 )
      {
        *have_sfnt = 1;
        *sfnt_id   = EndianS16_BtoN( assoc->fontID );
      }
      else if ( base_assoc->fontSize == 0 )
      {
        *have_sfnt = 1;
        *sfnt_id   = EndianS16_BtoN( base_assoc->fontID );
      }
    }

    if ( EndianS32_BtoN( fond->ffStylOff ) )
    {
      unsigned char*  p = (unsigned char*)fond_data;
      StyleTable*     style;
      unsigned short  string_count;
      char            ps_name[256];
      unsigned char*  names[64];
      int             i;


      p += EndianS32_BtoN( fond->ffStylOff );
      style = (StyleTable*)p;
      p += sizeof ( StyleTable );
      string_count = EndianS16_BtoN( *(short*)(p) );
      p += sizeof ( short );

      for ( i = 0; i < string_count && i < 64; i++ )
      {
        names[i] = p;
        p       += names[i][0];
        p++;
      }

      {
        size_t  ps_name_len = (size_t)names[0][0];


        if ( ps_name_len != 0 )
        {
          ft_memcpy(ps_name, names[0] + 1, ps_name_len);
          ps_name[ps_name_len] = 0;
        }
        if ( style->indexes[face_index] > 1 &&
             style->indexes[face_index] <= FT_MIN( string_count, 64 ) )
        {
          unsigned char*  suffixes = names[style->indexes[face_index] - 1];


          for ( i = 1; i <= suffixes[0]; i++ )
          {
            unsigned char*  s;
            size_t          j = suffixes[i] - 1;


            if ( j < string_count && ( s = names[j] ) != NULL )
            {
              size_t  s_len = (size_t)s[0];


              if ( s_len != 0 && ps_name_len + s_len < sizeof ( ps_name ) )
              {
                ft_memcpy( ps_name + ps_name_len, s + 1, s_len );
                ps_name_len += s_len;
                ps_name[ps_name_len] = 0;
              }
            }
          }
        }
      }

      create_lwfn_name( ps_name, lwfn_file_name );
    }
  }


  static  FT_Error
  lookup_lwfn_by_fond( const UInt8*      path_fond,
                       ConstStr255Param  base_lwfn,
                       UInt8*            path_lwfn,
                       size_t            path_size )
  {
    FSRef   ref, par_ref;
    size_t  dirname_len;


    /* Pathname for FSRef can be in various formats: HFS, HFS+, and POSIX. */
    /* We should not extract parent directory by string manipulation.      */

    if ( noErr != FSPathMakeRef( path_fond, &ref, FALSE ) )
      return FT_THROW( Invalid_Argument );

    if ( noErr != FSGetCatalogInfo( &ref, kFSCatInfoNone,
                                    NULL, NULL, NULL, &par_ref ) )
      return FT_THROW( Invalid_Argument );

    if ( noErr != FSRefMakePath( &par_ref, path_lwfn, path_size ) )
      return FT_THROW( Invalid_Argument );

    if ( ft_strlen( (char *)path_lwfn ) + 1 + base_lwfn[0] > path_size )
      return FT_THROW( Invalid_Argument );

    /* now we have absolute dirname in path_lwfn */
    ft_strcat( (char *)path_lwfn, "/" );
    dirname_len = ft_strlen( (char *)path_lwfn );
    ft_strcat( (char *)path_lwfn, (char *)base_lwfn + 1 );
    path_lwfn[dirname_len + base_lwfn[0]] = '\0';

    if ( noErr != FSPathMakeRef( path_lwfn, &ref, FALSE ) )
      return FT_THROW( Cannot_Open_Resource );

    if ( noErr != FSGetCatalogInfo( &ref, kFSCatInfoNone,
                                    NULL, NULL, NULL, NULL ) )
      return FT_THROW( Cannot_Open_Resource );

    return FT_Err_Ok;
  }


  static short
  count_faces( Handle        fond,
               const UInt8*  pathname )
  {
    ResID     sfnt_id;
    short     have_sfnt, have_lwfn;
    Str255    lwfn_file_name;
    UInt8     buff[PATH_MAX];
    FT_Error  err;
    short     num_faces;


    have_sfnt = have_lwfn = 0;

    parse_fond( *fond, &have_sfnt, &sfnt_id, lwfn_file_name, 0 );

    if ( lwfn_file_name[0] )
    {
      err = lookup_lwfn_by_fond( pathname, lwfn_file_name,
                                 buff, sizeof ( buff )  );
      if ( !err )
        have_lwfn = 1;
    }

    if ( have_lwfn && ( !have_sfnt || PREFER_LWFN ) )
      num_faces = 1;
    else
      num_faces = count_faces_scalable( *fond );

    return num_faces;
  }


  /* Read Type 1 data from the POST resources inside the LWFN file,
     return a PFB buffer.  This is somewhat convoluted because the FT2
     PFB parser wants the ASCII header as one chunk, and the LWFN
     chunks are often not organized that way, so we glue chunks
     of the same type together. */
  static FT_Error
  read_lwfn( FT_Memory      memory,
             ResFileRefNum  res,
             FT_Byte**      pfb_data,
             FT_ULong*      size )
  {
    FT_Error       error = FT_Err_Ok;
    ResID          res_id;
    unsigned char  *buffer, *p, *size_p = NULL;
    FT_ULong       total_size = 0;
    FT_ULong       old_total_size = 0;
    FT_ULong       post_size, pfb_chunk_size;
    Handle         post_data;
    char           code, last_code;


    UseResFile( res );

    /* First pass: load all POST resources, and determine the size of */
    /* the output buffer.                                             */
    res_id    = 501;
    last_code = -1;

    for (;;)
    {
      post_data = Get1Resource( TTAG_POST, res_id++ );
      if ( post_data == NULL )
        break;  /* we are done */

      code = (*post_data)[0];

      if ( code != last_code )
      {
        if ( code == 5 )
          total_size += 2; /* just the end code */
        else
          total_size += 6; /* code + 4 bytes chunk length */
      }

      total_size += GetHandleSize( post_data ) - 2;
      last_code = code;

      /* detect integer overflows */
      if ( total_size < old_total_size )
      {
        error = FT_THROW( Array_Too_Large );
        goto Error;
      }

      old_total_size = total_size;
    }

    if ( FT_ALLOC( buffer, (FT_Long)total_size ) )
      goto Error;

    /* Second pass: append all POST data to the buffer, add PFB fields. */
    /* Glue all consecutive chunks of the same type together.           */
    p              = buffer;
    res_id         = 501;
    last_code      = -1;
    pfb_chunk_size = 0;

    for (;;)
    {
      post_data = Get1Resource( TTAG_POST, res_id++ );
      if ( post_data == NULL )
        break;  /* we are done */

      post_size = (FT_ULong)GetHandleSize( post_data ) - 2;
      code = (*post_data)[0];

      if ( code != last_code )
      {
        if ( last_code != -1 )
        {
          /* we are done adding a chunk, fill in the size field */
          if ( size_p != NULL )
          {
            *size_p++ = (FT_Byte)(   pfb_chunk_size         & 0xFF );
            *size_p++ = (FT_Byte)( ( pfb_chunk_size >> 8  ) & 0xFF );
            *size_p++ = (FT_Byte)( ( pfb_chunk_size >> 16 ) & 0xFF );
            *size_p++ = (FT_Byte)( ( pfb_chunk_size >> 24 ) & 0xFF );
          }
          pfb_chunk_size = 0;
        }

        *p++ = 0x80;
        if ( code == 5 )
          *p++ = 0x03;  /* the end */
        else if ( code == 2 )
          *p++ = 0x02;  /* binary segment */
        else
          *p++ = 0x01;  /* ASCII segment */

        if ( code != 5 )
        {
          size_p = p;   /* save for later */
          p += 4;       /* make space for size field */
        }
      }

      ft_memcpy( p, *post_data + 2, post_size );
      pfb_chunk_size += post_size;
      p += post_size;
      last_code = code;
    }

    *pfb_data = buffer;
    *size = total_size;

  Error:
    CloseResFile( res );
    return error;
  }


  /* Create a new FT_Face from a file path to an LWFN file. */
  static FT_Error
  FT_New_Face_From_LWFN( FT_Library    library,
                         const UInt8*  pathname,
                         FT_Long       face_index,
                         FT_Face*      aface )
  {
    FT_Byte*       pfb_data;
    FT_ULong       pfb_size;
    FT_Error       error;
    ResFileRefNum  res;


    if ( noErr != FT_FSPathMakeRes( pathname, &res ) )
      return FT_THROW( Cannot_Open_Resource );

    pfb_data = NULL;
    pfb_size = 0;
    error = read_lwfn( library->memory, res, &pfb_data, &pfb_size );
    CloseResFile( res ); /* PFB is already loaded, useless anymore */
    if ( error )
      return error;

    return open_face_from_buffer( library,
                                  pfb_data,
                                  pfb_size,
                                  face_index,
                                  "type1",
                                  aface );
  }


  /* Create a new FT_Face from an SFNT resource, specified by res ID. */
  static FT_Error
  FT_New_Face_From_SFNT( FT_Library  library,
                         ResID       sfnt_id,
                         FT_Long     face_index,
                         FT_Face*    aface )
  {
    Handle     sfnt = NULL;
    FT_Byte*   sfnt_data;
    size_t     sfnt_size;
    FT_Error   error  = FT_Err_Ok;
    FT_Memory  memory = library->memory;
    int        is_cff, is_sfnt_ps;


    sfnt = GetResource( TTAG_sfnt, sfnt_id );
    if ( sfnt == NULL )
      return FT_THROW( Invalid_Handle );

    sfnt_size = (FT_ULong)GetHandleSize( sfnt );
    if ( FT_ALLOC( sfnt_data, (FT_Long)sfnt_size ) )
    {
      ReleaseResource( sfnt );
      return error;
    }

    ft_memcpy( sfnt_data, *sfnt, sfnt_size );
    ReleaseResource( sfnt );

    is_cff     = sfnt_size > 4 && !ft_memcmp( sfnt_data, "OTTO", 4 );
    is_sfnt_ps = sfnt_size > 4 && !ft_memcmp( sfnt_data, "typ1", 4 );

    if ( is_sfnt_ps )
    {
      FT_Stream  stream;


      if ( FT_NEW( stream ) )
        goto Try_OpenType;

      FT_Stream_OpenMemory( stream, sfnt_data, sfnt_size );
      if ( !open_face_PS_from_sfnt_stream( library,
                                           stream,
                                           face_index,
                                           0, NULL,
                                           aface ) )
      {
        FT_Stream_Close( stream );
        FT_FREE( stream );
        FT_FREE( sfnt_data );
        goto Exit;
      }

      FT_FREE( stream );
    }
  Try_OpenType:
    error = open_face_from_buffer( library,
                                   sfnt_data,
                                   sfnt_size,
                                   face_index,
                                   is_cff ? "cff" : "truetype",
                                   aface );
  Exit:
    return error;
  }


  /* Create a new FT_Face from a file path to a suitcase file. */
  static FT_Error
  FT_New_Face_From_Suitcase( FT_Library    library,
                             const UInt8*  pathname,
                             FT_Long       face_index,
                             FT_Face*      aface )
  {
    FT_Error       error = FT_ERR( Cannot_Open_Resource );
    ResFileRefNum  res_ref;
    ResourceIndex  res_index;
    Handle         fond;
    short          num_faces_in_res;


    if ( noErr != FT_FSPathMakeRes( pathname, &res_ref ) )
      return FT_THROW( Cannot_Open_Resource );

    UseResFile( res_ref );
    if ( ResError() )
      return FT_THROW( Cannot_Open_Resource );

    num_faces_in_res = 0;
    for ( res_index = 1; ; ++res_index )
    {
      short  num_faces_in_fond;


      fond = Get1IndResource( TTAG_FOND, res_index );
      if ( ResError() )
        break;

      num_faces_in_fond  = count_faces( fond, pathname );
      num_faces_in_res  += num_faces_in_fond;

      if ( 0 <= face_index && face_index < num_faces_in_fond && error )
        error = FT_New_Face_From_FOND( library, fond, face_index, aface );

      face_index -= num_faces_in_fond;
    }

    CloseResFile( res_ref );
    if ( !error && aface && *aface )
      (*aface)->num_faces = num_faces_in_res;
    return error;
  }


  /* documentation is in ftmac.h */

  FT_EXPORT_DEF( FT_Error )
  FT_New_Face_From_FOND( FT_Library  library,
                         Handle      fond,
                         FT_Long     face_index,
                         FT_Face*    aface )
  {
    short     have_sfnt, have_lwfn = 0;
    ResID     sfnt_id, fond_id;
    OSType    fond_type;
    Str255    fond_name;
    Str255    lwfn_file_name;
    UInt8     path_lwfn[PATH_MAX];
    OSErr     err;
    FT_Error  error = FT_Err_Ok;


    GetResInfo( fond, &fond_id, &fond_type, fond_name );
    if ( ResError() != noErr || fond_type != TTAG_FOND )
      return FT_THROW( Invalid_File_Format );

    parse_fond( *fond, &have_sfnt, &sfnt_id, lwfn_file_name, face_index );

    if ( lwfn_file_name[0] )
    {
      ResFileRefNum  res;


      res = HomeResFile( fond );
      if ( noErr != ResError() )
        goto found_no_lwfn_file;

      {
        UInt8  path_fond[PATH_MAX];
        FSRef  ref;


        err = FSGetForkCBInfo( res, kFSInvalidVolumeRefNum,
                               NULL, NULL, NULL, &ref, NULL );
        if ( noErr != err )
          goto found_no_lwfn_file;

        err = FSRefMakePath( &ref, path_fond, sizeof ( path_fond ) );
        if ( noErr != err )
          goto found_no_lwfn_file;

        error = lookup_lwfn_by_fond( path_fond, lwfn_file_name,
                                     path_lwfn, sizeof ( path_lwfn ) );
        if ( !error )
          have_lwfn = 1;
      }
    }

    if ( have_lwfn && ( !have_sfnt || PREFER_LWFN ) )
      error = FT_New_Face_From_LWFN( library,
                                     path_lwfn,
                                     face_index,
                                     aface );
    else
      error = FT_THROW( Unknown_File_Format );

  found_no_lwfn_file:
    if ( have_sfnt && error )
      error = FT_New_Face_From_SFNT( library,
                                     sfnt_id,
                                     face_index,
                                     aface );

    return error;
  }


  /* Common function to load a new FT_Face from a resource file. */
  static FT_Error
  FT_New_Face_From_Resource( FT_Library    library,
                             const UInt8*  pathname,
                             FT_Long       face_index,
                             FT_Face*      aface )
  {
    OSType    file_type;
    FT_Error  error;


    /* LWFN is a (very) specific file format, check for it explicitly */
    file_type = get_file_type_from_path( pathname );
    if ( file_type == TTAG_LWFN )
      return FT_New_Face_From_LWFN( library, pathname, face_index, aface );

    /* Otherwise the file type doesn't matter (there are more than  */
    /* `FFIL' and `tfil').  Just try opening it as a font suitcase; */
    /* if it works, fine.                                           */

    error = FT_New_Face_From_Suitcase( library, pathname, face_index, aface );
    if ( error == 0 )
      return error;

    /* let it fall through to normal loader (.ttf, .otf, etc.); */
    /* we signal this by returning no error and no FT_Face      */
    *aface = NULL;
    return 0;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_New_Face                                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This is the Mac-specific implementation of FT_New_Face.  In        */
  /*    addition to the standard FT_New_Face() functionality, it also      */
  /*    accepts pathnames to Mac suitcase files.  For further              */
  /*    documentation see the original FT_New_Face() in freetype.h.        */
  /*                                                                       */
  FT_EXPORT_DEF( FT_Error )
  FT_New_Face( FT_Library   library,
               const char*  pathname,
               FT_Long      face_index,
               FT_Face*     aface )
  {
    FT_Open_Args  args;
    FT_Error      error;


    /* test for valid `library' and `aface' delayed to FT_Open_Face() */
    if ( !pathname )
      return FT_THROW( Invalid_Argument );

    *aface = NULL;

    /* try resourcefork based font: LWFN, FFIL */
    error = FT_New_Face_From_Resource( library, (UInt8 *)pathname,
                                       face_index, aface );
    if ( error != 0 || *aface != NULL )
      return error;

    /* let it fall through to normal loader (.ttf, .otf, etc.) */
    args.flags    = FT_OPEN_PATHNAME;
    args.pathname = (char*)pathname;
    return FT_Open_Face( library, &args, face_index, aface );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_New_Face_From_FSRef                                             */
  /*                                                                       */
  /* <Description>                                                         */
  /*    FT_New_Face_From_FSRef is identical to FT_New_Face except it       */
  /*    accepts an FSRef instead of a path.                                */
  /*                                                                       */
  /* This function is deprecated because Carbon data types (FSRef)         */
  /* are not cross-platform, and thus not suitable for the freetype API.   */
  FT_EXPORT_DEF( FT_Error )
  FT_New_Face_From_FSRef( FT_Library    library,
                          const FSRef*  ref,
                          FT_Long       face_index,
                          FT_Face*      aface )
  {
    FT_Error      error;
    FT_Open_Args  args;
    OSErr   err;
    UInt8   pathname[PATH_MAX];


    if ( !ref )
      return FT_THROW( Invalid_Argument );

    err = FSRefMakePath( ref, pathname, sizeof ( pathname ) );
    if ( err )
      error = FT_THROW( Cannot_Open_Resource );

    error = FT_New_Face_From_Resource( library, pathname, face_index, aface );
    if ( error != 0 || *aface != NULL )
      return error;

    /* fallback to datafork font */
    args.flags    = FT_OPEN_PATHNAME;
    args.pathname = (char*)pathname;
    return FT_Open_Face( library, &args, face_index, aface );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_New_Face_From_FSSpec                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    FT_New_Face_From_FSSpec is identical to FT_New_Face except it      */
  /*    accepts an FSSpec instead of a path.                               */
  /*                                                                       */
  /* This function is deprecated because FSSpec is deprecated in Mac OS X  */
  FT_EXPORT_DEF( FT_Error )
  FT_New_Face_From_FSSpec( FT_Library     library,
                           const FSSpec*  spec,
                           FT_Long        face_index,
                           FT_Face*       aface )
  {
#if ( __LP64__ ) || ( defined( MAC_OS_X_VERSION_10_5 ) && \
      ( MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 ) )
    FT_UNUSED( library );
    FT_UNUSED( spec );
    FT_UNUSED( face_index );
    FT_UNUSED( aface );

    return FT_THROW( Unimplemented_Feature );
#else
    FSRef  ref;


    if ( !spec || FSpMakeFSRef( spec, &ref ) != noErr )
      return FT_THROW( Invalid_Argument );
    else
      return FT_New_Face_From_FSRef( library, &ref, face_index, aface );
#endif
  }

#endif /* FT_MACINTOSH */


/* END */
#endif

/* END */
