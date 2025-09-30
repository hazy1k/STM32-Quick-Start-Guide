/*  pcf.c

    FreeType font driver for pcf fonts

  Copyright 2000-2001, 2003 by
  Francesco Zappa Nardelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#define FT_MAKE_OPTION_SINGLE_OBJECT


#include "ft2build.h"
/*

Copyright 1990, 1994, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/
/* $XFree86: xc/lib/font/util/utilbitmap.c,v 1.3 1999/08/22 08:58:58 dawes Exp $ */

/*
 * Author:  Keith Packard, MIT X Consortium
 */

/* Modified for use with FreeType */


#include "ft2build.h"
#include "pcfutil.h"


  /*
   *  Invert bit order within each BYTE of an array.
   */

  FT_LOCAL_DEF( void )
  BitOrderInvert( unsigned char*  buf,
                  size_t          nbytes )
  {
    for ( ; nbytes > 0; nbytes--, buf++ )
    {
      unsigned int  val = *buf;


      val = ( ( val >> 1 ) & 0x55 ) | ( ( val << 1 ) & 0xAA );
      val = ( ( val >> 2 ) & 0x33 ) | ( ( val << 2 ) & 0xCC );
      val = ( ( val >> 4 ) & 0x0F ) | ( ( val << 4 ) & 0xF0 );

      *buf = (unsigned char)val;
    }
  }


  /*
   *  Invert byte order within each 16-bits of an array.
   */

  FT_LOCAL_DEF( void )
  TwoByteSwap( unsigned char*  buf,
               size_t          nbytes )
  {
    for ( ; nbytes >= 2; nbytes -= 2, buf += 2 )
    {
      unsigned char  c;


      c      = buf[0];
      buf[0] = buf[1];
      buf[1] = c;
    }
  }

  /*
   *  Invert byte order within each 32-bits of an array.
   */

  FT_LOCAL_DEF( void )
  FourByteSwap( unsigned char*  buf,
                size_t          nbytes )
  {
    for ( ; nbytes >= 4; nbytes -= 4, buf += 4 )
    {
      unsigned char  c;


      c      = buf[0];
      buf[0] = buf[3];
      buf[3] = c;

      c      = buf[1];
      buf[1] = buf[2];
      buf[2] = c;
    }
  }


/* END */
/*  pcfread.c

    FreeType font driver for pcf fonts

  Copyright 2000-2010, 2012, 2013 by
  Francesco Zappa Nardelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "ft2build.h"

#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_OBJECTS_H

#include "pcf.h"
#include "pcfread.h"

#include "pcferror.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_pcfread


#ifdef FT_DEBUG_LEVEL_TRACE
  static const char* const  tableNames[] =
  {
    "prop", "accl", "mtrcs", "bmps", "imtrcs",
    "enc", "swidth", "names", "accel"
  };
#endif


  static
  const FT_Frame_Field  pcf_toc_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_TocRec

    FT_FRAME_START( 8 ),
      FT_FRAME_ULONG_LE( version ),
      FT_FRAME_ULONG_LE( count ),
    FT_FRAME_END
  };


  static
  const FT_Frame_Field  pcf_table_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_TableRec

    FT_FRAME_START( 16  ),
      FT_FRAME_ULONG_LE( type ),
      FT_FRAME_ULONG_LE( format ),
      FT_FRAME_ULONG_LE( size ),
      FT_FRAME_ULONG_LE( offset ),
    FT_FRAME_END
  };


  static FT_Error
  pcf_read_TOC( FT_Stream  stream,
                PCF_Face   face )
  {
    FT_Error   error;
    PCF_Toc    toc = &face->toc;
    PCF_Table  tables;

    FT_Memory  memory = FT_FACE( face )->memory;
    FT_UInt    n;


    if ( FT_STREAM_SEEK ( 0 )                          ||
         FT_STREAM_READ_FIELDS ( pcf_toc_header, toc ) )
      return FT_THROW( Cannot_Open_Resource );

    if ( toc->version != PCF_FILE_VERSION                 ||
         toc->count   >  FT_ARRAY_MAX( face->toc.tables ) ||
         toc->count   == 0                                )
      return FT_THROW( Invalid_File_Format );

    if ( FT_NEW_ARRAY( face->toc.tables, toc->count ) )
      return FT_THROW( Out_Of_Memory );

    tables = face->toc.tables;
    for ( n = 0; n < toc->count; n++ )
    {
      if ( FT_STREAM_READ_FIELDS( pcf_table_header, tables ) )
        goto Exit;
      tables++;
    }

    /* Sort tables and check for overlaps.  Because they are almost      */
    /* always ordered already, an in-place bubble sort with simultaneous */
    /* boundary checking seems appropriate.                              */
    tables = face->toc.tables;

    for ( n = 0; n < toc->count - 1; n++ )
    {
      FT_UInt  i, have_change;


      have_change = 0;

      for ( i = 0; i < toc->count - 1 - n; i++ )
      {
        PCF_TableRec  tmp;


        if ( tables[i].offset > tables[i + 1].offset )
        {
          tmp           = tables[i];
          tables[i]     = tables[i + 1];
          tables[i + 1] = tmp;

          have_change = 1;
        }

        if ( ( tables[i].size   > tables[i + 1].offset )                  ||
             ( tables[i].offset > tables[i + 1].offset - tables[i].size ) )
          return FT_THROW( Invalid_Offset );
      }

      if ( !have_change )
        break;
    }

#ifdef FT_DEBUG_LEVEL_TRACE

    {
      FT_UInt      i, j;
      const char*  name = "?";


      FT_TRACE4(( "pcf_read_TOC:\n" ));

      FT_TRACE4(( "  number of tables: %ld\n", face->toc.count ));

      tables = face->toc.tables;
      for ( i = 0; i < toc->count; i++ )
      {
        for ( j = 0; j < sizeof ( tableNames ) / sizeof ( tableNames[0] );
              j++ )
          if ( tables[i].type == (FT_UInt)( 1 << j ) )
            name = tableNames[j];

        FT_TRACE4(( "  %d: type=%s, format=0x%X, "
                    "size=%ld (0x%lX), offset=%ld (0x%lX)\n",
                    i, name,
                    tables[i].format,
                    tables[i].size, tables[i].size,
                    tables[i].offset, tables[i].offset ));
      }
    }

#endif

    return FT_Err_Ok;

  Exit:
    FT_FREE( face->toc.tables );
    return error;
  }


#define PCF_METRIC_SIZE  12

  static
  const FT_Frame_Field  pcf_metric_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_MetricRec

    FT_FRAME_START( PCF_METRIC_SIZE ),
      FT_FRAME_SHORT_LE( leftSideBearing ),
      FT_FRAME_SHORT_LE( rightSideBearing ),
      FT_FRAME_SHORT_LE( characterWidth ),
      FT_FRAME_SHORT_LE( ascent ),
      FT_FRAME_SHORT_LE( descent ),
      FT_FRAME_SHORT_LE( attributes ),
    FT_FRAME_END
  };


  static
  const FT_Frame_Field  pcf_metric_msb_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_MetricRec

    FT_FRAME_START( PCF_METRIC_SIZE ),
      FT_FRAME_SHORT( leftSideBearing ),
      FT_FRAME_SHORT( rightSideBearing ),
      FT_FRAME_SHORT( characterWidth ),
      FT_FRAME_SHORT( ascent ),
      FT_FRAME_SHORT( descent ),
      FT_FRAME_SHORT( attributes ),
    FT_FRAME_END
  };


#define PCF_COMPRESSED_METRIC_SIZE  5

  static
  const FT_Frame_Field  pcf_compressed_metric_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_Compressed_MetricRec

    FT_FRAME_START( PCF_COMPRESSED_METRIC_SIZE ),
      FT_FRAME_BYTE( leftSideBearing ),
      FT_FRAME_BYTE( rightSideBearing ),
      FT_FRAME_BYTE( characterWidth ),
      FT_FRAME_BYTE( ascent ),
      FT_FRAME_BYTE( descent ),
    FT_FRAME_END
  };


  static FT_Error
  pcf_get_metric( FT_Stream   stream,
                  FT_ULong    format,
                  PCF_Metric  metric )
  {
    FT_Error  error = FT_Err_Ok;


    if ( PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
    {
      const FT_Frame_Field*  fields;


      /* parsing normal metrics */
      fields = PCF_BYTE_ORDER( format ) == MSBFirst
               ? pcf_metric_msb_header
               : pcf_metric_header;

      /* the following sets `error' but doesn't return in case of failure */
      (void)FT_STREAM_READ_FIELDS( fields, metric );
    }
    else
    {
      PCF_Compressed_MetricRec  compr;


      /* parsing compressed metrics */
      if ( FT_STREAM_READ_FIELDS( pcf_compressed_metric_header, &compr ) )
        goto Exit;

      metric->leftSideBearing  = (FT_Short)( compr.leftSideBearing  - 0x80 );
      metric->rightSideBearing = (FT_Short)( compr.rightSideBearing - 0x80 );
      metric->characterWidth   = (FT_Short)( compr.characterWidth   - 0x80 );
      metric->ascent           = (FT_Short)( compr.ascent           - 0x80 );
      metric->descent          = (FT_Short)( compr.descent          - 0x80 );
      metric->attributes       = 0;
    }

  Exit:
    return error;
  }


  static FT_Error
  pcf_seek_to_table_type( FT_Stream  stream,
                          PCF_Table  tables,
                          FT_ULong   ntables, /* same as PCF_Toc->count */
                          FT_ULong   type,
                          FT_ULong  *aformat,
                          FT_ULong  *asize )
  {
    FT_Error  error = FT_ERR( Invalid_File_Format );
    FT_ULong  i;


    for ( i = 0; i < ntables; i++ )
      if ( tables[i].type == type )
      {
        if ( stream->pos > tables[i].offset )
        {
          error = FT_THROW( Invalid_Stream_Skip );
          goto Fail;
        }

        if ( FT_STREAM_SKIP( tables[i].offset - stream->pos ) )
        {
          error = FT_THROW( Invalid_Stream_Skip );
          goto Fail;
        }

        *asize   = tables[i].size;
        *aformat = tables[i].format;

        return FT_Err_Ok;
      }

  Fail:
    *asize = 0;
    return error;
  }


  static FT_Bool
  pcf_has_table_type( PCF_Table  tables,
                      FT_ULong   ntables, /* same as PCF_Toc->count */
                      FT_ULong   type )
  {
    FT_ULong  i;


    for ( i = 0; i < ntables; i++ )
      if ( tables[i].type == type )
        return TRUE;

    return FALSE;
  }


#define PCF_PROPERTY_SIZE  9

  static
  const FT_Frame_Field  pcf_property_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_ParsePropertyRec

    FT_FRAME_START( PCF_PROPERTY_SIZE ),
      FT_FRAME_LONG_LE( name ),
      FT_FRAME_BYTE   ( isString ),
      FT_FRAME_LONG_LE( value ),
    FT_FRAME_END
  };


  static
  const FT_Frame_Field  pcf_property_msb_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_ParsePropertyRec

    FT_FRAME_START( PCF_PROPERTY_SIZE ),
      FT_FRAME_LONG( name ),
      FT_FRAME_BYTE( isString ),
      FT_FRAME_LONG( value ),
    FT_FRAME_END
  };


  FT_LOCAL_DEF( PCF_Property )
  pcf_find_property( PCF_Face          face,
                     const FT_String*  prop )
  {
    PCF_Property  properties = face->properties;
    FT_Bool       found      = 0;
    int           i;


    for ( i = 0 ; i < face->nprops && !found; i++ )
    {
      if ( !ft_strcmp( properties[i].name, prop ) )
        found = 1;
    }

    if ( found )
      return properties + i - 1;
    else
      return NULL;
  }


  static FT_Error
  pcf_get_properties( FT_Stream  stream,
                      PCF_Face   face )
  {
    PCF_ParseProperty  props      = 0;
    PCF_Property       properties = NULL;
    FT_ULong           nprops, i;
    FT_ULong           format, size;
    FT_Error           error;
    FT_Memory          memory     = FT_FACE( face )->memory;
    FT_ULong           string_size;
    FT_String*         strings    = 0;


    error = pcf_seek_to_table_type( stream,
                                    face->toc.tables,
                                    face->toc.count,
                                    PCF_PROPERTIES,
                                    &format,
                                    &size );
    if ( error )
      goto Bail;

    if ( FT_READ_ULONG_LE( format ) )
      goto Bail;

    FT_TRACE4(( "pcf_get_properties:\n" ));

    FT_TRACE4(( "  format = %ld\n", format ));

    if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
      goto Bail;

    if ( PCF_BYTE_ORDER( format ) == MSBFirst )
      (void)FT_READ_ULONG( nprops );
    else
      (void)FT_READ_ULONG_LE( nprops );
    if ( error )
      goto Bail;

    FT_TRACE4(( "  nprop = %d (truncate %d props)\n",
                (int)nprops, nprops - (int)nprops ));

    nprops = (int)nprops;

    /* rough estimate */
    if ( nprops > size / PCF_PROPERTY_SIZE )
    {
      error = FT_THROW( Invalid_Table );
      goto Bail;
    }

    face->nprops = (int)nprops;

    if ( FT_NEW_ARRAY( props, nprops ) )
      goto Bail;

    for ( i = 0; i < nprops; i++ )
    {
      if ( PCF_BYTE_ORDER( format ) == MSBFirst )
      {
        if ( FT_STREAM_READ_FIELDS( pcf_property_msb_header, props + i ) )
          goto Bail;
      }
      else
      {
        if ( FT_STREAM_READ_FIELDS( pcf_property_header, props + i ) )
          goto Bail;
      }
    }

    /* pad the property array                                            */
    /*                                                                   */
    /* clever here - nprops is the same as the number of odd-units read, */
    /* as only isStringProp are odd length   (Keith Packard)             */
    /*                                                                   */
    if ( nprops & 3 )
    {
      i = 4 - ( nprops & 3 );
      if ( FT_STREAM_SKIP( i ) )
      {
        error = FT_THROW( Invalid_Stream_Skip );
        goto Bail;
      }
    }

    if ( PCF_BYTE_ORDER( format ) == MSBFirst )
      (void)FT_READ_ULONG( string_size );
    else
      (void)FT_READ_ULONG_LE( string_size );
    if ( error )
      goto Bail;

    FT_TRACE4(( "  string_size = %ld\n", string_size ));

    /* rough estimate */
    if ( string_size > size - nprops * PCF_PROPERTY_SIZE )
    {
      error = FT_THROW( Invalid_Table );
      goto Bail;
    }

    /* allocate one more byte so that we have a final null byte */
    if ( FT_NEW_ARRAY( strings, string_size + 1 ) )
      goto Bail;

    error = FT_Stream_Read( stream, (FT_Byte*)strings, string_size );
    if ( error )
      goto Bail;

    if ( FT_NEW_ARRAY( properties, nprops ) )
      goto Bail;

    face->properties = properties;

    for ( i = 0; i < nprops; i++ )
    {
      FT_Long  name_offset = props[i].name;


      if ( ( name_offset < 0 )                     ||
           ( (FT_ULong)name_offset > string_size ) )
      {
        error = FT_THROW( Invalid_Offset );
        goto Bail;
      }

      if ( FT_STRDUP( properties[i].name, strings + name_offset ) )
        goto Bail;

      FT_TRACE4(( "  %s:", properties[i].name ));

      properties[i].isString = props[i].isString;

      if ( props[i].isString )
      {
        FT_Long  value_offset = props[i].value;


        if ( ( value_offset < 0 )                     ||
             ( (FT_ULong)value_offset > string_size ) )
        {
          error = FT_THROW( Invalid_Offset );
          goto Bail;
        }

        if ( FT_STRDUP( properties[i].value.atom, strings + value_offset ) )
          goto Bail;

        FT_TRACE4(( " `%s'\n", properties[i].value.atom ));
      }
      else
      {
        properties[i].value.l = props[i].value;

        FT_TRACE4(( " %d\n", properties[i].value.l ));
      }
    }

    error = FT_Err_Ok;

  Bail:
    FT_FREE( props );
    FT_FREE( strings );

    return error;
  }


  static FT_Error
  pcf_get_metrics( FT_Stream  stream,
                   PCF_Face   face )
  {
    FT_Error    error;
    FT_Memory   memory  = FT_FACE( face )->memory;
    FT_ULong    format, size;
    PCF_Metric  metrics = 0;
    FT_ULong    nmetrics, i;


    error = pcf_seek_to_table_type( stream,
                                    face->toc.tables,
                                    face->toc.count,
                                    PCF_METRICS,
                                    &format,
                                    &size );
    if ( error )
      return error;

    if ( FT_READ_ULONG_LE( format ) )
      goto Bail;

    if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT )     &&
         !PCF_FORMAT_MATCH( format, PCF_COMPRESSED_METRICS ) )
      return FT_THROW( Invalid_File_Format );

    if ( PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
    {
      if ( PCF_BYTE_ORDER( format ) == MSBFirst )
        (void)FT_READ_ULONG( nmetrics );
      else
        (void)FT_READ_ULONG_LE( nmetrics );
    }
    else
    {
      if ( PCF_BYTE_ORDER( format ) == MSBFirst )
        (void)FT_READ_USHORT( nmetrics );
      else
        (void)FT_READ_USHORT_LE( nmetrics );
    }
    if ( error )
      return FT_THROW( Invalid_File_Format );

    face->nmetrics = nmetrics;

    if ( !nmetrics )
      return FT_THROW( Invalid_Table );

    FT_TRACE4(( "pcf_get_metrics:\n" ));

    FT_TRACE4(( "  number of metrics: %d\n", nmetrics ));

    /* rough estimate */
    if ( PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
    {
      if ( nmetrics > size / PCF_METRIC_SIZE )
        return FT_THROW( Invalid_Table );
    }
    else
    {
      if ( nmetrics > size / PCF_COMPRESSED_METRIC_SIZE )
        return FT_THROW( Invalid_Table );
    }

    if ( FT_NEW_ARRAY( face->metrics, nmetrics ) )
      return FT_THROW( Out_Of_Memory );

    metrics = face->metrics;
    for ( i = 0; i < nmetrics; i++ )
    {
      error = pcf_get_metric( stream, format, metrics + i );

      metrics[i].bits = 0;

      FT_TRACE5(( "  idx %d: width=%d, "
                  "lsb=%d, rsb=%d, ascent=%d, descent=%d, swidth=%d\n",
                  i,
                  ( metrics + i )->characterWidth,
                  ( metrics + i )->leftSideBearing,
                  ( metrics + i )->rightSideBearing,
                  ( metrics + i )->ascent,
                  ( metrics + i )->descent,
                  ( metrics + i )->attributes ));

      if ( error )
        break;
    }

    if ( error )
      FT_FREE( face->metrics );

  Bail:
    return error;
  }


  static FT_Error
  pcf_get_bitmaps( FT_Stream  stream,
                   PCF_Face   face )
  {
    FT_Error   error;
    FT_Memory  memory  = FT_FACE( face )->memory;
    FT_Long*   offsets = NULL;
    FT_Long    bitmapSizes[GLYPHPADOPTIONS];
    FT_ULong   format, size;
    FT_ULong   nbitmaps, i, sizebitmaps = 0;


    error = pcf_seek_to_table_type( stream,
                                    face->toc.tables,
                                    face->toc.count,
                                    PCF_BITMAPS,
                                    &format,
                                    &size );
    if ( error )
      return error;

    error = FT_Stream_EnterFrame( stream, 8 );
    if ( error )
      return error;

    format = FT_GET_ULONG_LE();
    if ( PCF_BYTE_ORDER( format ) == MSBFirst )
      nbitmaps  = FT_GET_ULONG();
    else
      nbitmaps  = FT_GET_ULONG_LE();

    FT_Stream_ExitFrame( stream );

    if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
      return FT_THROW( Invalid_File_Format );

    FT_TRACE4(( "pcf_get_bitmaps:\n" ));

    FT_TRACE4(( "  number of bitmaps: %d\n", nbitmaps ));

    /* XXX: PCF_Face->nmetrics is singed FT_Long, see pcf.h */
    if ( face->nmetrics < 0 || nbitmaps != ( FT_ULong )face->nmetrics )
      return FT_THROW( Invalid_File_Format );

    if ( FT_NEW_ARRAY( offsets, nbitmaps ) )
      return error;

    for ( i = 0; i < nbitmaps; i++ )
    {
      if ( PCF_BYTE_ORDER( format ) == MSBFirst )
        (void)FT_READ_LONG( offsets[i] );
      else
        (void)FT_READ_LONG_LE( offsets[i] );

      FT_TRACE5(( "  bitmap %d: offset %ld (0x%lX)\n",
                  i, offsets[i], offsets[i] ));
    }
    if ( error )
      goto Bail;

    for ( i = 0; i < GLYPHPADOPTIONS; i++ )
    {
      if ( PCF_BYTE_ORDER( format ) == MSBFirst )
        (void)FT_READ_LONG( bitmapSizes[i] );
      else
        (void)FT_READ_LONG_LE( bitmapSizes[i] );
      if ( error )
        goto Bail;

      sizebitmaps = bitmapSizes[PCF_GLYPH_PAD_INDEX( format )];

      FT_TRACE4(( "  padding %d implies a size of %ld\n", i, bitmapSizes[i] ));
    }

    FT_TRACE4(( "  %d bitmaps, padding index %ld\n",
                nbitmaps,
                PCF_GLYPH_PAD_INDEX( format ) ));
    FT_TRACE4(( "  bitmap size = %d\n", sizebitmaps ));

    FT_UNUSED( sizebitmaps );       /* only used for debugging */

    for ( i = 0; i < nbitmaps; i++ )
    {
      /* rough estimate */
      if ( ( offsets[i] < 0 )              ||
           ( (FT_ULong)offsets[i] > size ) )
      {
        FT_TRACE0(( "pcf_get_bitmaps:"
                    " invalid offset to bitmap data of glyph %d\n", i ));
      }
      else
        face->metrics[i].bits = stream->pos + offsets[i];
    }

    face->bitmapsFormat = format;

  Bail:
    FT_FREE( offsets );
    return error;
  }


  static FT_Error
  pcf_get_encodings( FT_Stream  stream,
                     PCF_Face   face )
  {
    FT_Error      error;
    FT_Memory     memory = FT_FACE( face )->memory;
    FT_ULong      format, size;
    int           firstCol, lastCol;
    int           firstRow, lastRow;
    int           nencoding, encodingOffset;
    int           i, j, k;
    PCF_Encoding  encoding = NULL;


    error = pcf_seek_to_table_type( stream,
                                    face->toc.tables,
                                    face->toc.count,
                                    PCF_BDF_ENCODINGS,
                                    &format,
                                    &size );
    if ( error )
      return error;

    error = FT_Stream_EnterFrame( stream, 14 );
    if ( error )
      return error;

    format = FT_GET_ULONG_LE();

    if ( PCF_BYTE_ORDER( format ) == MSBFirst )
    {
      firstCol          = FT_GET_SHORT();
      lastCol           = FT_GET_SHORT();
      firstRow          = FT_GET_SHORT();
      lastRow           = FT_GET_SHORT();
      face->defaultChar = FT_GET_SHORT();
    }
    else
    {
      firstCol          = FT_GET_SHORT_LE();
      lastCol           = FT_GET_SHORT_LE();
      firstRow          = FT_GET_SHORT_LE();
      lastRow           = FT_GET_SHORT_LE();
      face->defaultChar = FT_GET_SHORT_LE();
    }

    FT_Stream_ExitFrame( stream );

    if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
      return FT_THROW( Invalid_File_Format );

    FT_TRACE4(( "pdf_get_encodings:\n" ));

    FT_TRACE4(( "  firstCol %d, lastCol %d, firstRow %d, lastRow %d\n",
                firstCol, lastCol, firstRow, lastRow ));

    nencoding = ( lastCol - firstCol + 1 ) * ( lastRow - firstRow + 1 );

    if ( FT_NEW_ARRAY( encoding, nencoding ) )
      return FT_THROW( Out_Of_Memory );

    error = FT_Stream_EnterFrame( stream, 2 * nencoding );
    if ( error )
      goto Bail;

    k = 0;
    for ( i = firstRow; i <= lastRow; i++ )
    {
      for ( j = firstCol; j <= lastCol; j++ )
      {
        if ( PCF_BYTE_ORDER( format ) == MSBFirst )
          encodingOffset = FT_GET_SHORT();
        else
          encodingOffset = FT_GET_SHORT_LE();

        if ( encodingOffset != -1 )
        {
          encoding[k].enc   = i * 256 + j;
          encoding[k].glyph = (FT_Short)encodingOffset;

          FT_TRACE5(( "  code %d (0x%04X): idx %d\n",
                      encoding[k].enc, encoding[k].enc, encoding[k].glyph ));

          k++;
        }
      }
    }
    FT_Stream_ExitFrame( stream );

    if ( FT_RENEW_ARRAY( encoding, nencoding, k ) )
      goto Bail;

    face->nencodings = k;
    face->encodings  = encoding;

    return error;

  Bail:
    FT_FREE( encoding );
    return error;
  }


  static
  const FT_Frame_Field  pcf_accel_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_AccelRec

    FT_FRAME_START( 20 ),
      FT_FRAME_BYTE      ( noOverlap ),
      FT_FRAME_BYTE      ( constantMetrics ),
      FT_FRAME_BYTE      ( terminalFont ),
      FT_FRAME_BYTE      ( constantWidth ),
      FT_FRAME_BYTE      ( inkInside ),
      FT_FRAME_BYTE      ( inkMetrics ),
      FT_FRAME_BYTE      ( drawDirection ),
      FT_FRAME_SKIP_BYTES( 1 ),
      FT_FRAME_LONG_LE   ( fontAscent ),
      FT_FRAME_LONG_LE   ( fontDescent ),
      FT_FRAME_LONG_LE   ( maxOverlap ),
    FT_FRAME_END
  };


  static
  const FT_Frame_Field  pcf_accel_msb_header[] =
  {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  PCF_AccelRec

    FT_FRAME_START( 20 ),
      FT_FRAME_BYTE      ( noOverlap ),
      FT_FRAME_BYTE      ( constantMetrics ),
      FT_FRAME_BYTE      ( terminalFont ),
      FT_FRAME_BYTE      ( constantWidth ),
      FT_FRAME_BYTE      ( inkInside ),
      FT_FRAME_BYTE      ( inkMetrics ),
      FT_FRAME_BYTE      ( drawDirection ),
      FT_FRAME_SKIP_BYTES( 1 ),
      FT_FRAME_LONG      ( fontAscent ),
      FT_FRAME_LONG      ( fontDescent ),
      FT_FRAME_LONG      ( maxOverlap ),
    FT_FRAME_END
  };


  static FT_Error
  pcf_get_accel( FT_Stream  stream,
                 PCF_Face   face,
                 FT_ULong   type )
  {
    FT_ULong   format, size;
    FT_Error   error;
    PCF_Accel  accel = &face->accel;


    error = pcf_seek_to_table_type( stream,
                                    face->toc.tables,
                                    face->toc.count,
                                    type,
                                    &format,
                                    &size );
    if ( error )
      goto Bail;

    if ( FT_READ_ULONG_LE( format ) )
      goto Bail;

    if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT )    &&
         !PCF_FORMAT_MATCH( format, PCF_ACCEL_W_INKBOUNDS ) )
      goto Bail;

    if ( PCF_BYTE_ORDER( format ) == MSBFirst )
    {
      if ( FT_STREAM_READ_FIELDS( pcf_accel_msb_header, accel ) )
        goto Bail;
    }
    else
    {
      if ( FT_STREAM_READ_FIELDS( pcf_accel_header, accel ) )
        goto Bail;
    }

    error = pcf_get_metric( stream,
                            format & ( ~PCF_FORMAT_MASK ),
                            &(accel->minbounds) );
    if ( error )
      goto Bail;

    error = pcf_get_metric( stream,
                            format & ( ~PCF_FORMAT_MASK ),
                            &(accel->maxbounds) );
    if ( error )
      goto Bail;

    if ( PCF_FORMAT_MATCH( format, PCF_ACCEL_W_INKBOUNDS ) )
    {
      error = pcf_get_metric( stream,
                              format & ( ~PCF_FORMAT_MASK ),
                              &(accel->ink_minbounds) );
      if ( error )
        goto Bail;

      error = pcf_get_metric( stream,
                              format & ( ~PCF_FORMAT_MASK ),
                              &(accel->ink_maxbounds) );
      if ( error )
        goto Bail;
    }
    else
    {
      accel->ink_minbounds = accel->minbounds; /* I'm not sure about this */
      accel->ink_maxbounds = accel->maxbounds;
    }

  Bail:
    return error;
  }


  static FT_Error
  pcf_interpret_style( PCF_Face  pcf )
  {
    FT_Error   error  = FT_Err_Ok;
    FT_Face    face   = FT_FACE( pcf );
    FT_Memory  memory = face->memory;

    PCF_Property  prop;

    size_t  nn, len;
    char*   strings[4] = { NULL, NULL, NULL, NULL };
    size_t  lengths[4];


    face->style_flags = 0;

    prop = pcf_find_property( pcf, "SLANT" );
    if ( prop && prop->isString                                       &&
         ( *(prop->value.atom) == 'O' || *(prop->value.atom) == 'o' ||
           *(prop->value.atom) == 'I' || *(prop->value.atom) == 'i' ) )
    {
      face->style_flags |= FT_STYLE_FLAG_ITALIC;
      strings[2] = ( *(prop->value.atom) == 'O' ||
                     *(prop->value.atom) == 'o' ) ? (char *)"Oblique"
                                                  : (char *)"Italic";
    }

    prop = pcf_find_property( pcf, "WEIGHT_NAME" );
    if ( prop && prop->isString                                       &&
         ( *(prop->value.atom) == 'B' || *(prop->value.atom) == 'b' ) )
    {
      face->style_flags |= FT_STYLE_FLAG_BOLD;
      strings[1] = (char*)"Bold";
    }

    prop = pcf_find_property( pcf, "SETWIDTH_NAME" );
    if ( prop && prop->isString                                        &&
         *(prop->value.atom)                                           &&
         !( *(prop->value.atom) == 'N' || *(prop->value.atom) == 'n' ) )
      strings[3] = (char*)( prop->value.atom );

    prop = pcf_find_property( pcf, "ADD_STYLE_NAME" );
    if ( prop && prop->isString                                        &&
         *(prop->value.atom)                                           &&
         !( *(prop->value.atom) == 'N' || *(prop->value.atom) == 'n' ) )
      strings[0] = (char*)( prop->value.atom );

    for ( len = 0, nn = 0; nn < 4; nn++ )
    {
      lengths[nn] = 0;
      if ( strings[nn] )
      {
        lengths[nn] = ft_strlen( strings[nn] );
        len        += lengths[nn] + 1;
      }
    }

    if ( len == 0 )
    {
      strings[0] = (char*)"Regular";
      lengths[0] = ft_strlen( strings[0] );
      len        = lengths[0] + 1;
    }

    {
      char*  s;


      if ( FT_ALLOC( face->style_name, len ) )
        return error;

      s = face->style_name;

      for ( nn = 0; nn < 4; nn++ )
      {
        char*  src = strings[nn];


        len = lengths[nn];

        if ( src == NULL )
          continue;

        /* separate elements with a space */
        if ( s != face->style_name )
          *s++ = ' ';

        ft_memcpy( s, src, len );

        /* need to convert spaces to dashes for */
        /* add_style_name and setwidth_name     */
        if ( nn == 0 || nn == 3 )
        {
          size_t  mm;


          for ( mm = 0; mm < len; mm++ )
            if ( s[mm] == ' ' )
              s[mm] = '-';
        }

        s += len;
      }
      *s = 0;
    }

    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  pcf_load_font( FT_Stream  stream,
                 PCF_Face   face )
  {
    FT_Error   error;
    FT_Memory  memory = FT_FACE( face )->memory;
    FT_Bool    hasBDFAccelerators;


    error = pcf_read_TOC( stream, face );
    if ( error )
      goto Exit;

    error = pcf_get_properties( stream, face );
    if ( error )
      goto Exit;

    /* Use the old accelerators if no BDF accelerators are in the file. */
    hasBDFAccelerators = pcf_has_table_type( face->toc.tables,
                                             face->toc.count,
                                             PCF_BDF_ACCELERATORS );
    if ( !hasBDFAccelerators )
    {
      error = pcf_get_accel( stream, face, PCF_ACCELERATORS );
      if ( error )
        goto Exit;
    }

    /* metrics */
    error = pcf_get_metrics( stream, face );
    if ( error )
      goto Exit;

    /* bitmaps */
    error = pcf_get_bitmaps( stream, face );
    if ( error )
      goto Exit;

    /* encodings */
    error = pcf_get_encodings( stream, face );
    if ( error )
      goto Exit;

    /* BDF style accelerators (i.e. bounds based on encoded glyphs) */
    if ( hasBDFAccelerators )
    {
      error = pcf_get_accel( stream, face, PCF_BDF_ACCELERATORS );
      if ( error )
        goto Exit;
    }

    /* XXX: TO DO: inkmetrics and glyph_names are missing */

    /* now construct the face object */
    {
      FT_Face       root = FT_FACE( face );
      PCF_Property  prop;


      root->num_faces  = 1;
      root->face_index = 0;

      root->face_flags |= FT_FACE_FLAG_FIXED_SIZES |
                          FT_FACE_FLAG_HORIZONTAL  |
                          FT_FACE_FLAG_FAST_GLYPHS;

      if ( face->accel.constantWidth )
        root->face_flags |= FT_FACE_FLAG_FIXED_WIDTH;

      if ( ( error = pcf_interpret_style( face ) ) != 0 )
         goto Exit;

      prop = pcf_find_property( face, "FAMILY_NAME" );
      if ( prop && prop->isString )
      {
        if ( FT_STRDUP( root->family_name, prop->value.atom ) )
          goto Exit;
      }
      else
        root->family_name = NULL;

      /*
       * Note: We shift all glyph indices by +1 since we must
       * respect the convention that glyph 0 always corresponds
       * to the `missing glyph'.
       *
       * This implies bumping the number of `available' glyphs by 1.
       */
      root->num_glyphs = face->nmetrics + 1;

      root->num_fixed_sizes = 1;
      if ( FT_NEW_ARRAY( root->available_sizes, 1 ) )
        goto Exit;

      {
        FT_Bitmap_Size*  bsize = root->available_sizes;
        FT_Short         resolution_x = 0, resolution_y = 0;


        FT_MEM_ZERO( bsize, sizeof ( FT_Bitmap_Size ) );

#if 0
        bsize->height = face->accel.maxbounds.ascent << 6;
#endif
        bsize->height = (FT_Short)( face->accel.fontAscent +
                                    face->accel.fontDescent );

        prop = pcf_find_property( face, "AVERAGE_WIDTH" );
        if ( prop )
          bsize->width = (FT_Short)( ( prop->value.l + 5 ) / 10 );
        else
          bsize->width = (FT_Short)( bsize->height * 2/3 );

        prop = pcf_find_property( face, "POINT_SIZE" );
        if ( prop )
          /* convert from 722.7 decipoints to 72 points per inch */
          bsize->size =
            (FT_Pos)( ( prop->value.l * 64 * 7200 + 36135L ) / 72270L );

        prop = pcf_find_property( face, "PIXEL_SIZE" );
        if ( prop )
          bsize->y_ppem = (FT_Short)prop->value.l << 6;

        prop = pcf_find_property( face, "RESOLUTION_X" );
        if ( prop )
          resolution_x = (FT_Short)prop->value.l;

        prop = pcf_find_property( face, "RESOLUTION_Y" );
        if ( prop )
          resolution_y = (FT_Short)prop->value.l;

        if ( bsize->y_ppem == 0 )
        {
          bsize->y_ppem = bsize->size;
          if ( resolution_y )
            bsize->y_ppem = bsize->y_ppem * resolution_y / 72;
        }
        if ( resolution_x && resolution_y )
          bsize->x_ppem = bsize->y_ppem * resolution_x / resolution_y;
        else
          bsize->x_ppem = bsize->y_ppem;
      }

      /* set up charset */
      {
        PCF_Property  charset_registry = 0, charset_encoding = 0;


        charset_registry = pcf_find_property( face, "CHARSET_REGISTRY" );
        charset_encoding = pcf_find_property( face, "CHARSET_ENCODING" );

        if ( charset_registry && charset_registry->isString &&
             charset_encoding && charset_encoding->isString )
        {
          if ( FT_STRDUP( face->charset_encoding,
                          charset_encoding->value.atom ) ||
               FT_STRDUP( face->charset_registry,
                          charset_registry->value.atom ) )
            goto Exit;
        }
      }
    }

  Exit:
    if ( error )
    {
      /* This is done to respect the behaviour of the original */
      /* PCF font driver.                                      */
      error = FT_THROW( Invalid_File_Format );
    }

    return error;
  }


/* END */
/*  pcfdrivr.c

    FreeType font driver for pcf files

    Copyright (C) 2000-2004, 2006-2011, 2013 by
    Francesco Zappa Nardelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "ft2build.h"

#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_OBJECTS_H
#include FT_GZIP_H
#include FT_LZW_H
#include FT_BZIP2_H
#include FT_ERRORS_H
#include FT_BDF_H
#include FT_TRUETYPE_IDS_H

#include "pcf.h"
#include "pcfdrivr.h"
#include "pcfread.h"

#include "pcferror.h"
#include "pcfutil.h"

#undef  FT_COMPONENT
#define FT_COMPONENT  trace_pcfread

#include FT_SERVICE_BDF_H
#include FT_SERVICE_XFREE86_NAME_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_pcfdriver


  typedef struct  PCF_CMapRec_
  {
    FT_CMapRec    root;
    FT_UInt       num_encodings;
    PCF_Encoding  encodings;

  } PCF_CMapRec, *PCF_CMap;


  FT_CALLBACK_DEF( FT_Error )
  pcf_cmap_init( FT_CMap     pcfcmap,   /* PCF_CMap */
                 FT_Pointer  init_data )
  {
    PCF_CMap  cmap = (PCF_CMap)pcfcmap;
    PCF_Face  face = (PCF_Face)FT_CMAP_FACE( pcfcmap );

    FT_UNUSED( init_data );


    cmap->num_encodings = (FT_UInt)face->nencodings;
    cmap->encodings     = face->encodings;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( void )
  pcf_cmap_done( FT_CMap  pcfcmap )         /* PCF_CMap */
  {
    PCF_CMap  cmap = (PCF_CMap)pcfcmap;


    cmap->encodings     = NULL;
    cmap->num_encodings = 0;
  }


  FT_CALLBACK_DEF( FT_UInt )
  pcf_cmap_char_index( FT_CMap    pcfcmap,  /* PCF_CMap */
                       FT_UInt32  charcode )
  {
    PCF_CMap      cmap      = (PCF_CMap)pcfcmap;
    PCF_Encoding  encodings = cmap->encodings;
    FT_UInt       min, max, mid;
    FT_UInt       result    = 0;


    min = 0;
    max = cmap->num_encodings;

    while ( min < max )
    {
      FT_ULong  code;


      mid  = ( min + max ) >> 1;
      code = encodings[mid].enc;

      if ( charcode == code )
      {
        result = encodings[mid].glyph + 1;
        break;
      }

      if ( charcode < code )
        max = mid;
      else
        min = mid + 1;
    }

    return result;
  }


  FT_CALLBACK_DEF( FT_UInt )
  pcf_cmap_char_next( FT_CMap    pcfcmap,   /* PCF_CMap */
                      FT_UInt32  *acharcode )
  {
    PCF_CMap      cmap      = (PCF_CMap)pcfcmap;
    PCF_Encoding  encodings = cmap->encodings;
    FT_UInt       min, max, mid;
    FT_ULong      charcode  = *acharcode + 1;
    FT_UInt       result    = 0;


    min = 0;
    max = cmap->num_encodings;

    while ( min < max )
    {
      FT_ULong  code;


      mid  = ( min + max ) >> 1;
      code = encodings[mid].enc;

      if ( charcode == code )
      {
        result = encodings[mid].glyph + 1;
        goto Exit;
      }

      if ( charcode < code )
        max = mid;
      else
        min = mid + 1;
    }

    charcode = 0;
    if ( min < cmap->num_encodings )
    {
      charcode = encodings[min].enc;
      result   = encodings[min].glyph + 1;
    }

  Exit:
    if ( charcode > 0xFFFFFFFFUL )
    {
      FT_TRACE1(( "pcf_cmap_char_next: charcode 0x%x > 32bit API" ));
      *acharcode = 0;
      /* XXX: result should be changed to indicate an overflow error */
    }
    else
      *acharcode = (FT_UInt32)charcode;
    return result;
  }


  FT_CALLBACK_TABLE_DEF
  const FT_CMap_ClassRec  pcf_cmap_class =
  {
    sizeof ( PCF_CMapRec ),
    pcf_cmap_init,
    pcf_cmap_done,
    pcf_cmap_char_index,
    pcf_cmap_char_next,

    NULL, NULL, NULL, NULL, NULL
  };


  FT_CALLBACK_DEF( void )
  PCF_Face_Done( FT_Face  pcfface )         /* PCF_Face */
  {
    PCF_Face   face = (PCF_Face)pcfface;
    FT_Memory  memory;


    if ( !face )
      return;

    memory = FT_FACE_MEMORY( face );

    FT_FREE( face->encodings );
    FT_FREE( face->metrics );

    /* free properties */
    if ( face->properties )
    {
      FT_Int  i;


      for ( i = 0; i < face->nprops; i++ )
      {
        PCF_Property  prop = &face->properties[i];


        if ( prop )
        {
          FT_FREE( prop->name );
          if ( prop->isString )
            FT_FREE( prop->value.atom );
        }
      }

      FT_FREE( face->properties );
    }

    FT_FREE( face->toc.tables );
    FT_FREE( pcfface->family_name );
    FT_FREE( pcfface->style_name );
    FT_FREE( pcfface->available_sizes );
    FT_FREE( face->charset_encoding );
    FT_FREE( face->charset_registry );

    /* close compressed stream if any */
    if ( pcfface->stream == &face->comp_stream )
    {
      FT_Stream_Close( &face->comp_stream );
      pcfface->stream = face->comp_source;
    }
  }


  FT_CALLBACK_DEF( FT_Error )
  PCF_Face_Init( FT_Stream      stream,
                 FT_Face        pcfface,        /* PCF_Face */
                 FT_Int         face_index,
                 FT_Int         num_params,
                 FT_Parameter*  params )
  {
    PCF_Face  face  = (PCF_Face)pcfface;
    FT_Error  error;

    FT_UNUSED( num_params );
    FT_UNUSED( params );


    FT_TRACE2(( "PCF driver\n" ));

    error = pcf_load_font( stream, face );
    if ( error )
    {
      PCF_Face_Done( pcfface );

#if defined( FT_CONFIG_OPTION_USE_ZLIB )  || \
    defined( FT_CONFIG_OPTION_USE_LZW )   || \
    defined( FT_CONFIG_OPTION_USE_BZIP2 )

#ifdef FT_CONFIG_OPTION_USE_ZLIB
      {
        FT_Error  error2;


        /* this didn't work, try gzip support! */
        error2 = FT_Stream_OpenGzip( &face->comp_stream, stream );
        if ( FT_ERR_EQ( error2, Unimplemented_Feature ) )
          goto Fail;

        error = error2;
      }
#endif /* FT_CONFIG_OPTION_USE_ZLIB */

#ifdef FT_CONFIG_OPTION_USE_LZW
      if ( error )
      {
        FT_Error  error3;


        /* this didn't work, try LZW support! */
        error3 = FT_Stream_OpenLZW( &face->comp_stream, stream );
        if ( FT_ERR_EQ( error3, Unimplemented_Feature ) )
          goto Fail;

        error = error3;
      }
#endif /* FT_CONFIG_OPTION_USE_LZW */

#ifdef FT_CONFIG_OPTION_USE_BZIP2
      if ( error )
      {
        FT_Error  error4;


        /* this didn't work, try Bzip2 support! */
        error4 = FT_Stream_OpenBzip2( &face->comp_stream, stream );
        if ( FT_ERR_EQ( error4, Unimplemented_Feature ) )
          goto Fail;

        error = error4;
      }
#endif /* FT_CONFIG_OPTION_USE_BZIP2 */

      if ( error )
        goto Fail;

      face->comp_source = stream;
      pcfface->stream   = &face->comp_stream;

      stream = pcfface->stream;

      error = pcf_load_font( stream, face );
      if ( error )
        goto Fail;

#else /* !(FT_CONFIG_OPTION_USE_ZLIB ||
           FT_CONFIG_OPTION_USE_LZW ||
           FT_CONFIG_OPTION_USE_BZIP2) */

      goto Fail;

#endif
    }

    /* PCF could not have multiple face in single font file.
     * XXX: non-zero face_index is already invalid argument, but
     *      Type1, Type42 driver has a convention to return
     *      an invalid argument error when the font could be
     *      opened by the specified driver.
     */
    if ( face_index > 0 ) {
      FT_ERROR(( "PCF_Face_Init: invalid face index\n" ));
      PCF_Face_Done( pcfface );
      return FT_THROW( Invalid_Argument );
    }

    /* set up charmap */
    {
      FT_String  *charset_registry = face->charset_registry;
      FT_String  *charset_encoding = face->charset_encoding;
      FT_Bool     unicode_charmap  = 0;


      if ( charset_registry && charset_encoding )
      {
        char*  s = charset_registry;


        /* Uh, oh, compare first letters manually to avoid dependency
           on locales. */
        if ( ( s[0] == 'i' || s[0] == 'I' ) &&
             ( s[1] == 's' || s[1] == 'S' ) &&
             ( s[2] == 'o' || s[2] == 'O' ) )
        {
          s += 3;
          if ( !ft_strcmp( s, "10646" )                      ||
               ( !ft_strcmp( s, "8859" ) &&
                 !ft_strcmp( face->charset_encoding, "1" ) ) )
          unicode_charmap = 1;
        }
      }

      {
        FT_CharMapRec  charmap;


        charmap.face        = FT_FACE( face );
        charmap.encoding    = FT_ENCODING_NONE;
        /* initial platform/encoding should indicate unset status? */
        charmap.platform_id = TT_PLATFORM_APPLE_UNICODE;
        charmap.encoding_id = TT_APPLE_ID_DEFAULT;

        if ( unicode_charmap )
        {
          charmap.encoding    = FT_ENCODING_UNICODE;
          charmap.platform_id = TT_PLATFORM_MICROSOFT;
          charmap.encoding_id = TT_MS_ID_UNICODE_CS;
        }

        error = FT_CMap_New( &pcf_cmap_class, NULL, &charmap, NULL );

#if 0
        /* Select default charmap */
        if ( pcfface->num_charmaps )
          pcfface->charmap = pcfface->charmaps[0];
#endif
      }
    }

  Exit:
    return error;

  Fail:
    FT_TRACE2(( "  not a PCF file\n" ));
    PCF_Face_Done( pcfface );
    error = FT_THROW( Unknown_File_Format );  /* error */
    goto Exit;
  }


  FT_CALLBACK_DEF( FT_Error )
  PCF_Size_Select( FT_Size   size,
                   FT_ULong  strike_index )
  {
    PCF_Accel  accel = &( (PCF_Face)size->face )->accel;


    FT_Select_Metrics( size->face, strike_index );

    size->metrics.ascender    =  accel->fontAscent << 6;
    size->metrics.descender   = -accel->fontDescent << 6;
    size->metrics.max_advance =  accel->maxbounds.characterWidth << 6;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_Error )
  PCF_Size_Request( FT_Size          size,
                    FT_Size_Request  req )
  {
    PCF_Face         face  = (PCF_Face)size->face;
    FT_Bitmap_Size*  bsize = size->face->available_sizes;
    FT_Error         error = FT_ERR( Invalid_Pixel_Size );
    FT_Long          height;


    height = FT_REQUEST_HEIGHT( req );
    height = ( height + 32 ) >> 6;

    switch ( req->type )
    {
    case FT_SIZE_REQUEST_TYPE_NOMINAL:
      if ( height == ( ( bsize->y_ppem + 32 ) >> 6 ) )
        error = FT_Err_Ok;
      break;

    case FT_SIZE_REQUEST_TYPE_REAL_DIM:
      if ( height == ( face->accel.fontAscent +
                       face->accel.fontDescent ) )
        error = FT_Err_Ok;
      break;

    default:
      error = FT_THROW( Unimplemented_Feature );
      break;
    }

    if ( error )
      return error;
    else
      return PCF_Size_Select( size, 0 );
  }


  FT_CALLBACK_DEF( FT_Error )
  PCF_Glyph_Load( FT_GlyphSlot  slot,
                  FT_Size       size,
                  FT_UInt       glyph_index,
                  FT_Int32      load_flags )
  {
    PCF_Face    face   = (PCF_Face)FT_SIZE_FACE( size );
    FT_Stream   stream;
    FT_Error    error  = FT_Err_Ok;
    FT_Bitmap*  bitmap = &slot->bitmap;
    PCF_Metric  metric;
    FT_Offset   bytes;

    FT_UNUSED( load_flags );


    FT_TRACE1(( "PCF_Glyph_Load: glyph index %d\n", glyph_index ));

    if ( !face || glyph_index >= (FT_UInt)face->root.num_glyphs )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    stream = face->root.stream;

    if ( glyph_index > 0 )
      glyph_index--;

    metric = face->metrics + glyph_index;

    bitmap->rows       = metric->ascent + metric->descent;
    bitmap->width      = metric->rightSideBearing - metric->leftSideBearing;
    bitmap->num_grays  = 1;
    bitmap->pixel_mode = FT_PIXEL_MODE_MONO;

    FT_TRACE6(( "BIT_ORDER %d ; BYTE_ORDER %d ; GLYPH_PAD %d\n",
                  PCF_BIT_ORDER( face->bitmapsFormat ),
                  PCF_BYTE_ORDER( face->bitmapsFormat ),
                  PCF_GLYPH_PAD( face->bitmapsFormat ) ));

    switch ( PCF_GLYPH_PAD( face->bitmapsFormat ) )
    {
    case 1:
      bitmap->pitch = ( bitmap->width + 7 ) >> 3;
      break;

    case 2:
      bitmap->pitch = ( ( bitmap->width + 15 ) >> 4 ) << 1;
      break;

    case 4:
      bitmap->pitch = ( ( bitmap->width + 31 ) >> 5 ) << 2;
      break;

    case 8:
      bitmap->pitch = ( ( bitmap->width + 63 ) >> 6 ) << 3;
      break;

    default:
      return FT_THROW( Invalid_File_Format );
    }

    /* XXX: to do: are there cases that need repadding the bitmap? */
    bytes = bitmap->pitch * bitmap->rows;

    error = ft_glyphslot_alloc_bitmap( slot, (FT_ULong)bytes );
    if ( error )
      goto Exit;

    if ( FT_STREAM_SEEK( metric->bits )          ||
         FT_STREAM_READ( bitmap->buffer, bytes ) )
      goto Exit;

    if ( PCF_BIT_ORDER( face->bitmapsFormat ) != MSBFirst )
      BitOrderInvert( bitmap->buffer, bytes );

    if ( ( PCF_BYTE_ORDER( face->bitmapsFormat ) !=
           PCF_BIT_ORDER( face->bitmapsFormat )  ) )
    {
      switch ( PCF_SCAN_UNIT( face->bitmapsFormat ) )
      {
      case 1:
        break;

      case 2:
        TwoByteSwap( bitmap->buffer, bytes );
        break;

      case 4:
        FourByteSwap( bitmap->buffer, bytes );
        break;
      }
    }

    slot->format      = FT_GLYPH_FORMAT_BITMAP;
    slot->bitmap_left = metric->leftSideBearing;
    slot->bitmap_top  = metric->ascent;

    slot->metrics.horiAdvance  = metric->characterWidth << 6;
    slot->metrics.horiBearingX = metric->leftSideBearing << 6;
    slot->metrics.horiBearingY = metric->ascent << 6;
    slot->metrics.width        = ( metric->rightSideBearing -
                                   metric->leftSideBearing ) << 6;
    slot->metrics.height       = bitmap->rows << 6;

    ft_synthesize_vertical_metrics( &slot->metrics,
                                    ( face->accel.fontAscent +
                                      face->accel.fontDescent ) << 6 );

  Exit:
    return error;
  }


 /*
  *
  *  BDF SERVICE
  *
  */

  static FT_Error
  pcf_get_bdf_property( PCF_Face          face,
                        const char*       prop_name,
                        BDF_PropertyRec  *aproperty )
  {
    PCF_Property  prop;


    prop = pcf_find_property( face, prop_name );
    if ( prop != NULL )
    {
      if ( prop->isString )
      {
        aproperty->type   = BDF_PROPERTY_TYPE_ATOM;
        aproperty->u.atom = prop->value.atom;
      }
      else
      {
        if ( prop->value.l > 0x7FFFFFFFL || prop->value.l < ( -1 - 0x7FFFFFFFL ) )
        {
          FT_TRACE1(( "pcf_get_bdf_property: " ));
          FT_TRACE1(( "too large integer 0x%x is truncated\n" ));
        }
        /* Apparently, the PCF driver loads all properties as signed integers!
         * This really doesn't seem to be a problem, because this is
         * sufficient for any meaningful values.
         */
        aproperty->type      = BDF_PROPERTY_TYPE_INTEGER;
        aproperty->u.integer = (FT_Int32)prop->value.l;
      }
      return 0;
    }

    return FT_THROW( Invalid_Argument );
  }


  static FT_Error
  pcf_get_charset_id( PCF_Face      face,
                      const char*  *acharset_encoding,
                      const char*  *acharset_registry )
  {
    *acharset_encoding = face->charset_encoding;
    *acharset_registry = face->charset_registry;

    return 0;
  }


  static const FT_Service_BDFRec  pcf_service_bdf =
  {
    (FT_BDF_GetCharsetIdFunc)pcf_get_charset_id,
    (FT_BDF_GetPropertyFunc) pcf_get_bdf_property
  };


 /*
  *
  *  SERVICE LIST
  *
  */

  static const FT_ServiceDescRec  pcf_services[] =
  {
    { FT_SERVICE_ID_BDF,       &pcf_service_bdf },
    { FT_SERVICE_ID_XF86_NAME, FT_XF86_FORMAT_PCF },
    { NULL, NULL }
  };


  FT_CALLBACK_DEF( FT_Module_Interface )
  pcf_driver_requester( FT_Module    module,
                        const char*  name )
  {
    FT_UNUSED( module );

    return ft_service_list_lookup( pcf_services, name );
  }


  FT_CALLBACK_TABLE_DEF
  const FT_Driver_ClassRec  pcf_driver_class =
  {
    {
      FT_MODULE_FONT_DRIVER        |
      FT_MODULE_DRIVER_NO_OUTLINES,
      sizeof ( FT_DriverRec ),

      "pcf",
      0x10000L,
      0x20000L,

      0,

      0,                    /* FT_Module_Constructor */
      0,                    /* FT_Module_Destructor  */
      pcf_driver_requester
    },

    sizeof ( PCF_FaceRec ),
    sizeof ( FT_SizeRec ),
    sizeof ( FT_GlyphSlotRec ),

    PCF_Face_Init,
    PCF_Face_Done,
    0,                      /* FT_Size_InitFunc */
    0,                      /* FT_Size_DoneFunc */
    0,                      /* FT_Slot_InitFunc */
    0,                      /* FT_Slot_DoneFunc */

    PCF_Glyph_Load,

    0,                      /* FT_Face_GetKerningFunc  */
    0,                      /* FT_Face_AttachFunc      */
    0,                      /* FT_Face_GetAdvancesFunc */

    PCF_Size_Request,
    PCF_Size_Select
  };


/* END */

/* END */
