/***************************************************************************/
/*                                                                         */
/*  sfnt.c                                                                 */
/*                                                                         */
/*    Single object library component.                                     */
/*                                                                         */
/*  Copyright 1996-2006, 2013 by                                           */
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
/*  sfntpic.c                                                              */
/*                                                                         */
/*    The FreeType position independent code services for sfnt module.     */
/*                                                                         */
/*  Copyright 2009, 2010, 2012, 2013 by                                    */
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
#include "sfntpic.h"
#include "sferrors.h"


#ifdef FT_CONFIG_OPTION_PIC

  /* forward declaration of PIC init functions from sfdriver.c */
  FT_Error
  FT_Create_Class_sfnt_services( FT_Library           library,
                                 FT_ServiceDescRec**  output_class );
  void
  FT_Destroy_Class_sfnt_services( FT_Library          library,
                                  FT_ServiceDescRec*  clazz );
  void
  FT_Init_Class_sfnt_service_bdf( FT_Service_BDFRec*  clazz );
  void
  FT_Init_Class_sfnt_interface( FT_Library       library,
                                SFNT_Interface*  clazz );
  void
  FT_Init_Class_sfnt_service_glyph_dict(
    FT_Library                library,
    FT_Service_GlyphDictRec*  clazz );
  void
  FT_Init_Class_sfnt_service_ps_name(
    FT_Library                 library,
    FT_Service_PsFontNameRec*  clazz );
  void
  FT_Init_Class_tt_service_get_cmap_info(
    FT_Library              library,
    FT_Service_TTCMapsRec*  clazz );
  void
  FT_Init_Class_sfnt_service_sfnt_table(
    FT_Service_SFNT_TableRec*  clazz );


  /* forward declaration of PIC init functions from ttcmap.c */
  FT_Error
  FT_Create_Class_tt_cmap_classes( FT_Library       library,
                                   TT_CMap_Class**  output_class );
  void
  FT_Destroy_Class_tt_cmap_classes( FT_Library      library,
                                    TT_CMap_Class*  clazz );


  void
  sfnt_module_class_pic_free( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Memory          memory        = library->memory;


    if ( pic_container->sfnt )
    {
      sfntModulePIC*  container = (sfntModulePIC*)pic_container->sfnt;


      if ( container->sfnt_services )
        FT_Destroy_Class_sfnt_services( library,
                                        container->sfnt_services );
      container->sfnt_services = NULL;

      if ( container->tt_cmap_classes )
        FT_Destroy_Class_tt_cmap_classes( library,
                                          container->tt_cmap_classes );
      container->tt_cmap_classes = NULL;

      FT_FREE( container );
      pic_container->sfnt = NULL;
    }
  }


  FT_Error
  sfnt_module_class_pic_init( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Error           error         = FT_Err_Ok;
    sfntModulePIC*     container     = NULL;
    FT_Memory          memory        = library->memory;


    /* allocate pointer, clear and set global container pointer */
    if ( FT_ALLOC( container, sizeof ( *container ) ) )
      return error;
    FT_MEM_SET( container, 0, sizeof ( *container ) );
    pic_container->sfnt = container;

    /* initialize pointer table -                       */
    /* this is how the module usually expects this data */
    error = FT_Create_Class_sfnt_services( library,
                                           &container->sfnt_services );
    if ( error )
      goto Exit;

    error = FT_Create_Class_tt_cmap_classes( library,
                                             &container->tt_cmap_classes );
    if ( error )
      goto Exit;

    FT_Init_Class_sfnt_service_glyph_dict(
      library, &container->sfnt_service_glyph_dict );
    FT_Init_Class_sfnt_service_ps_name(
      library, &container->sfnt_service_ps_name );
    FT_Init_Class_tt_service_get_cmap_info(
      library, &container->tt_service_get_cmap_info );
    FT_Init_Class_sfnt_service_sfnt_table(
      &container->sfnt_service_sfnt_table );
#ifdef TT_CONFIG_OPTION_BDF
    FT_Init_Class_sfnt_service_bdf( &container->sfnt_service_bdf );
#endif
    FT_Init_Class_sfnt_interface( library, &container->sfnt_interface );

  Exit:
    if ( error )
      sfnt_module_class_pic_free( library );
    return error;
  }

#endif /* FT_CONFIG_OPTION_PIC */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttload.c                                                               */
/*                                                                         */
/*    Load the basic TrueType tables, i.e., tables that can be either in   */
/*    TTF or OTF fonts (body).                                             */
/*                                                                         */
/*  Copyright 1996-2010, 2012, 2013 by                                     */
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
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include "ttload.h"

#include "sferrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttload


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_lookup_table                                               */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Looks for a TrueType table by name.                                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A face object handle.                                      */
  /*                                                                       */
  /*    tag  :: The searched tag.                                          */
  /*                                                                       */
  /* <Return>                                                              */
  /*    A pointer to the table directory entry.  0 if not found.           */
  /*                                                                       */
  FT_LOCAL_DEF( TT_Table  )
  tt_face_lookup_table( TT_Face   face,
                        FT_ULong  tag  )
  {
    TT_Table  entry;
    TT_Table  limit;
#ifdef FT_DEBUG_LEVEL_TRACE
    FT_Bool   zero_length = FALSE;
#endif


    FT_TRACE4(( "tt_face_lookup_table: %08p, `%c%c%c%c' -- ",
                face,
                (FT_Char)( tag >> 24 ),
                (FT_Char)( tag >> 16 ),
                (FT_Char)( tag >> 8  ),
                (FT_Char)( tag       ) ));

    entry = face->dir_tables;
    limit = entry + face->num_tables;

    for ( ; entry < limit; entry++ )
    {
      /* For compatibility with Windows, we consider    */
      /* zero-length tables the same as missing tables. */
      if ( entry->Tag == tag )
      {
        if ( entry->Length != 0 )
        {
          FT_TRACE4(( "found table.\n" ));
          return entry;
        }
#ifdef FT_DEBUG_LEVEL_TRACE
        zero_length = TRUE;
#endif
      }
    }

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( zero_length )
      FT_TRACE4(( "ignoring empty table\n" ));
    else
      FT_TRACE4(( "could not find table\n" ));
#endif

    return NULL;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_goto_table                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Looks for a TrueType table by name, then seek a stream to it.      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A face object handle.                                    */
  /*                                                                       */
  /*    tag    :: The searched tag.                                        */
  /*                                                                       */
  /*    stream :: The stream to seek when the table is found.              */
  /*                                                                       */
  /* <Output>                                                              */
  /*    length :: The length of the table if found, undefined otherwise.   */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_goto_table( TT_Face    face,
                      FT_ULong   tag,
                      FT_Stream  stream,
                      FT_ULong*  length )
  {
    TT_Table  table;
    FT_Error  error;


    table = tt_face_lookup_table( face, tag );
    if ( table )
    {
      if ( length )
        *length = table->Length;

      if ( FT_STREAM_SEEK( table->Offset ) )
        goto Exit;
    }
    else
      error = FT_THROW( Table_Missing );

  Exit:
    return error;
  }


  /* Here, we                                                         */
  /*                                                                  */
  /* - check that `num_tables' is valid (and adjust it if necessary)  */
  /*                                                                  */
  /* - look for a `head' table, check its size, and parse it to check */
  /*   whether its `magic' field is correctly set                     */
  /*                                                                  */
  /* - errors (except errors returned by stream handling)             */
  /*                                                                  */
  /*     SFNT_Err_Unknown_File_Format:                                */
  /*       no table is defined in directory, it is not sfnt-wrapped   */
  /*       data                                                       */
  /*     SFNT_Err_Table_Missing:                                      */
  /*       table directory is valid, but essential tables             */
  /*       (head/bhed/SING) are missing                               */
  /*                                                                  */
  static FT_Error
  check_table_dir( SFNT_Header  sfnt,
                   FT_Stream    stream )
  {
    FT_Error   error;
    FT_UShort  nn, valid_entries = 0;
    FT_UInt    has_head = 0, has_sing = 0, has_meta = 0;
    FT_ULong   offset = sfnt->offset + 12;

    static const FT_Frame_Field  table_dir_entry_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_TableRec

      FT_FRAME_START( 16 ),
        FT_FRAME_ULONG( Tag ),
        FT_FRAME_ULONG( CheckSum ),
        FT_FRAME_ULONG( Offset ),
        FT_FRAME_ULONG( Length ),
      FT_FRAME_END
    };


    if ( FT_STREAM_SEEK( offset ) )
      goto Exit;

    for ( nn = 0; nn < sfnt->num_tables; nn++ )
    {
      TT_TableRec  table;


      if ( FT_STREAM_READ_FIELDS( table_dir_entry_fields, &table ) )
      {
        nn--;
        FT_TRACE2(( "check_table_dir:"
                    " can read only %d table%s in font (instead of %d)\n",
                    nn, nn == 1 ? "" : "s", sfnt->num_tables ));
        sfnt->num_tables = nn;
        break;
      }

      /* we ignore invalid tables */
      if ( table.Offset + table.Length > stream->size )
      {
        FT_TRACE2(( "check_table_dir: table entry %d invalid\n", nn ));
        continue;
      }
      else
        valid_entries++;

      if ( table.Tag == TTAG_head || table.Tag == TTAG_bhed )
      {
        FT_UInt32  magic;


#ifndef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
        if ( table.Tag == TTAG_head )
#endif
          has_head = 1;

        /*
         * The table length should be 0x36, but certain font tools make it
         * 0x38, so we will just check that it is greater.
         *
         * Note that according to the specification, the table must be
         * padded to 32-bit lengths, but this doesn't apply to the value of
         * its `Length' field!
         *
         */
        if ( table.Length < 0x36 )
        {
          FT_TRACE2(( "check_table_dir:"
                      " `head' or `bhed' table too small\n" ));
          error = FT_THROW( Table_Missing );
          goto Exit;
        }

        if ( FT_STREAM_SEEK( table.Offset + 12 ) ||
             FT_READ_ULONG( magic )              )
          goto Exit;

        if ( magic != 0x5F0F3CF5UL )
          FT_TRACE2(( "check_table_dir:"
                      " invalid magic number in `head' or `bhed' table\n"));

        if ( FT_STREAM_SEEK( offset + ( nn + 1 ) * 16 ) )
          goto Exit;
      }
      else if ( table.Tag == TTAG_SING )
        has_sing = 1;
      else if ( table.Tag == TTAG_META )
        has_meta = 1;
    }

    sfnt->num_tables = valid_entries;

    if ( sfnt->num_tables == 0 )
    {
      FT_TRACE2(( "check_table_dir: no tables found\n" ));
      error = FT_THROW( Unknown_File_Format );
      goto Exit;
    }

    /* if `sing' and `meta' tables are present, there is no `head' table */
    if ( has_head || ( has_sing && has_meta ) )
    {
      error = FT_Err_Ok;
      goto Exit;
    }
    else
    {
      FT_TRACE2(( "check_table_dir:" ));
#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
      FT_TRACE2(( " neither `head', `bhed', nor `sing' table found\n" ));
#else
      FT_TRACE2(( " neither `head' nor `sing' table found\n" ));
#endif
      error = FT_THROW( Table_Missing );
    }

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_font_dir                                              */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the header of a SFNT font file.                              */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face       :: A handle to the target face object.                  */
  /*                                                                       */
  /*    stream     :: The input stream.                                    */
  /*                                                                       */
  /* <Output>                                                              */
  /*    sfnt       :: The SFNT header.                                     */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    The stream cursor must be at the beginning of the font directory.  */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_font_dir( TT_Face    face,
                         FT_Stream  stream )
  {
    SFNT_HeaderRec  sfnt;
    FT_Error        error;
    FT_Memory       memory = stream->memory;
    TT_TableRec*    entry;
    FT_Int          nn;

    static const FT_Frame_Field  offset_table_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  SFNT_HeaderRec

      FT_FRAME_START( 8 ),
        FT_FRAME_USHORT( num_tables ),
        FT_FRAME_USHORT( search_range ),
        FT_FRAME_USHORT( entry_selector ),
        FT_FRAME_USHORT( range_shift ),
      FT_FRAME_END
    };


    FT_TRACE2(( "tt_face_load_font_dir: %08p\n", face ));

    /* read the offset table */

    sfnt.offset = FT_STREAM_POS();

    if ( FT_READ_ULONG( sfnt.format_tag )                    ||
         FT_STREAM_READ_FIELDS( offset_table_fields, &sfnt ) )
      goto Exit;

    /* many fonts don't have these fields set correctly */
#if 0
    if ( sfnt.search_range != 1 << ( sfnt.entry_selector + 4 )        ||
         sfnt.search_range + sfnt.range_shift != sfnt.num_tables << 4 )
      return FT_THROW( Unknown_File_Format );
#endif

    /* load the table directory */

    FT_TRACE2(( "-- Number of tables: %10u\n",    sfnt.num_tables ));
    FT_TRACE2(( "-- Format version:   0x%08lx\n", sfnt.format_tag ));

    if ( sfnt.format_tag != TTAG_OTTO )
    {
      /* check first */
      error = check_table_dir( &sfnt, stream );
      if ( error )
      {
        FT_TRACE2(( "tt_face_load_font_dir:"
                    " invalid table directory for TrueType\n" ));

        goto Exit;
      }
    }

    face->num_tables = sfnt.num_tables;
    face->format_tag = sfnt.format_tag;

    if ( FT_QNEW_ARRAY( face->dir_tables, face->num_tables ) )
      goto Exit;

    if ( FT_STREAM_SEEK( sfnt.offset + 12 )       ||
         FT_FRAME_ENTER( face->num_tables * 16L ) )
      goto Exit;

    entry = face->dir_tables;

    FT_TRACE2(( "\n"
                "  tag    offset    length   checksum\n"
                "  ----------------------------------\n" ));

    for ( nn = 0; nn < sfnt.num_tables; nn++ )
    {
      entry->Tag      = FT_GET_TAG4();
      entry->CheckSum = FT_GET_ULONG();
      entry->Offset   = FT_GET_ULONG();
      entry->Length   = FT_GET_ULONG();

      /* ignore invalid tables */
      if ( entry->Offset + entry->Length > stream->size )
        continue;
      else
      {
        FT_TRACE2(( "  %c%c%c%c  %08lx  %08lx  %08lx\n",
                    (FT_Char)( entry->Tag >> 24 ),
                    (FT_Char)( entry->Tag >> 16 ),
                    (FT_Char)( entry->Tag >> 8  ),
                    (FT_Char)( entry->Tag       ),
                    entry->Offset,
                    entry->Length,
                    entry->CheckSum ));
        entry++;
      }
    }

    FT_FRAME_EXIT();

    FT_TRACE2(( "table directory loaded\n\n" ));

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_any                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads any font table into client memory.                           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: The face object to look for.                             */
  /*                                                                       */
  /*    tag    :: The tag of table to load.  Use the value 0 if you want   */
  /*              to access the whole font file, else set this parameter   */
  /*              to a valid TrueType table tag that you can forge with    */
  /*              the MAKE_TT_TAG macro.                                   */
  /*                                                                       */
  /*    offset :: The starting offset in the table (or the file if         */
  /*              tag == 0).                                               */
  /*                                                                       */
  /*    length :: The address of the decision variable:                    */
  /*                                                                       */
  /*                If length == NULL:                                     */
  /*                  Loads the whole table.  Returns an error if          */
  /*                  `offset' == 0!                                       */
  /*                                                                       */
  /*                If *length == 0:                                       */
  /*                  Exits immediately; returning the length of the given */
  /*                  table or of the font file, depending on the value of */
  /*                  `tag'.                                               */
  /*                                                                       */
  /*                If *length != 0:                                       */
  /*                  Loads the next `length' bytes of table or font,      */
  /*                  starting at offset `offset' (in table or font too).  */
  /*                                                                       */
  /* <Output>                                                              */
  /*    buffer :: The address of target buffer.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_any( TT_Face    face,
                    FT_ULong   tag,
                    FT_Long    offset,
                    FT_Byte*   buffer,
                    FT_ULong*  length )
  {
    FT_Error   error;
    FT_Stream  stream;
    TT_Table   table;
    FT_ULong   size;


    if ( tag != 0 )
    {
      /* look for tag in font directory */
      table = tt_face_lookup_table( face, tag );
      if ( !table )
      {
        error = FT_THROW( Table_Missing );
        goto Exit;
      }

      offset += table->Offset;
      size    = table->Length;
    }
    else
      /* tag == 0 -- the user wants to access the font file directly */
      size = face->root.stream->size;

    if ( length && *length == 0 )
    {
      *length = size;

      return FT_Err_Ok;
    }

    if ( length )
      size = *length;

    stream = face->root.stream;
    /* the `if' is syntactic sugar for picky compilers */
    if ( FT_STREAM_READ_AT( offset, buffer, size ) )
      goto Exit;

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_generic_header                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the TrueType table `head' or `bhed'.                         */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: The input stream.                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  static FT_Error
  tt_face_load_generic_header( TT_Face    face,
                               FT_Stream  stream,
                               FT_ULong   tag )
  {
    FT_Error    error;
    TT_Header*  header;

    static const FT_Frame_Field  header_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_Header

      FT_FRAME_START( 54 ),
        FT_FRAME_ULONG ( Table_Version ),
        FT_FRAME_ULONG ( Font_Revision ),
        FT_FRAME_LONG  ( CheckSum_Adjust ),
        FT_FRAME_LONG  ( Magic_Number ),
        FT_FRAME_USHORT( Flags ),
        FT_FRAME_USHORT( Units_Per_EM ),
        FT_FRAME_LONG  ( Created[0] ),
        FT_FRAME_LONG  ( Created[1] ),
        FT_FRAME_LONG  ( Modified[0] ),
        FT_FRAME_LONG  ( Modified[1] ),
        FT_FRAME_SHORT ( xMin ),
        FT_FRAME_SHORT ( yMin ),
        FT_FRAME_SHORT ( xMax ),
        FT_FRAME_SHORT ( yMax ),
        FT_FRAME_USHORT( Mac_Style ),
        FT_FRAME_USHORT( Lowest_Rec_PPEM ),
        FT_FRAME_SHORT ( Font_Direction ),
        FT_FRAME_SHORT ( Index_To_Loc_Format ),
        FT_FRAME_SHORT ( Glyph_Data_Format ),
      FT_FRAME_END
    };


    error = face->goto_table( face, tag, stream, 0 );
    if ( error )
      goto Exit;

    header = &face->header;

    if ( FT_STREAM_READ_FIELDS( header_fields, header ) )
      goto Exit;

    FT_TRACE3(( "Units per EM: %4u\n", header->Units_Per_EM ));
    FT_TRACE3(( "IndexToLoc:   %4d\n", header->Index_To_Loc_Format ));

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  tt_face_load_head( TT_Face    face,
                     FT_Stream  stream )
  {
    return tt_face_load_generic_header( face, stream, TTAG_head );
  }


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

  FT_LOCAL_DEF( FT_Error )
  tt_face_load_bhed( TT_Face    face,
                     FT_Stream  stream )
  {
    return tt_face_load_generic_header( face, stream, TTAG_bhed );
  }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_max_profile                                           */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the maximum profile into a face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: The input stream.                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_maxp( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error        error;
    TT_MaxProfile*  maxProfile = &face->max_profile;

    static const FT_Frame_Field  maxp_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_MaxProfile

      FT_FRAME_START( 6 ),
        FT_FRAME_LONG  ( version ),
        FT_FRAME_USHORT( numGlyphs ),
      FT_FRAME_END
    };

    static const FT_Frame_Field  maxp_fields_extra[] =
    {
      FT_FRAME_START( 26 ),
        FT_FRAME_USHORT( maxPoints ),
        FT_FRAME_USHORT( maxContours ),
        FT_FRAME_USHORT( maxCompositePoints ),
        FT_FRAME_USHORT( maxCompositeContours ),
        FT_FRAME_USHORT( maxZones ),
        FT_FRAME_USHORT( maxTwilightPoints ),
        FT_FRAME_USHORT( maxStorage ),
        FT_FRAME_USHORT( maxFunctionDefs ),
        FT_FRAME_USHORT( maxInstructionDefs ),
        FT_FRAME_USHORT( maxStackElements ),
        FT_FRAME_USHORT( maxSizeOfInstructions ),
        FT_FRAME_USHORT( maxComponentElements ),
        FT_FRAME_USHORT( maxComponentDepth ),
      FT_FRAME_END
    };


    error = face->goto_table( face, TTAG_maxp, stream, 0 );
    if ( error )
      goto Exit;

    if ( FT_STREAM_READ_FIELDS( maxp_fields, maxProfile ) )
      goto Exit;

    maxProfile->maxPoints             = 0;
    maxProfile->maxContours           = 0;
    maxProfile->maxCompositePoints    = 0;
    maxProfile->maxCompositeContours  = 0;
    maxProfile->maxZones              = 0;
    maxProfile->maxTwilightPoints     = 0;
    maxProfile->maxStorage            = 0;
    maxProfile->maxFunctionDefs       = 0;
    maxProfile->maxInstructionDefs    = 0;
    maxProfile->maxStackElements      = 0;
    maxProfile->maxSizeOfInstructions = 0;
    maxProfile->maxComponentElements  = 0;
    maxProfile->maxComponentDepth     = 0;

    if ( maxProfile->version >= 0x10000L )
    {
      if ( FT_STREAM_READ_FIELDS( maxp_fields_extra, maxProfile ) )
        goto Exit;

      /* XXX: an adjustment that is necessary to load certain */
      /*      broken fonts like `Keystrokes MT' :-(           */
      /*                                                      */
      /*   We allocate 64 function entries by default when    */
      /*   the maxFunctionDefs value is smaller.              */

      if ( maxProfile->maxFunctionDefs < 64 )
        maxProfile->maxFunctionDefs = 64;

      /* we add 4 phantom points later */
      if ( maxProfile->maxTwilightPoints > ( 0xFFFFU - 4 ) )
      {
        FT_TRACE0(( "tt_face_load_maxp:"
                    " too much twilight points in `maxp' table;\n"
                    "                  "
                    " some glyphs might be rendered incorrectly\n" ));

        maxProfile->maxTwilightPoints = 0xFFFFU - 4;
      }

      /* we arbitrarily limit recursion to avoid stack exhaustion */
      if ( maxProfile->maxComponentDepth > 100 )
      {
        FT_TRACE0(( "tt_face_load_maxp:"
                    " abnormally large component depth (%d) set to 100\n",
                    maxProfile->maxComponentDepth ));
        maxProfile->maxComponentDepth = 100;
      }
    }

    FT_TRACE3(( "numGlyphs: %u\n", maxProfile->numGlyphs ));

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_name                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the name records.                                            */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: The input stream.                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_name( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error      error;
    FT_Memory     memory = stream->memory;
    FT_ULong      table_pos, table_len;
    FT_ULong      storage_start, storage_limit;
    FT_UInt       count;
    TT_NameTable  table;

    static const FT_Frame_Field  name_table_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_NameTableRec

      FT_FRAME_START( 6 ),
        FT_FRAME_USHORT( format ),
        FT_FRAME_USHORT( numNameRecords ),
        FT_FRAME_USHORT( storageOffset ),
      FT_FRAME_END
    };

    static const FT_Frame_Field  name_record_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_NameEntryRec

      /* no FT_FRAME_START */
        FT_FRAME_USHORT( platformID ),
        FT_FRAME_USHORT( encodingID ),
        FT_FRAME_USHORT( languageID ),
        FT_FRAME_USHORT( nameID ),
        FT_FRAME_USHORT( stringLength ),
        FT_FRAME_USHORT( stringOffset ),
      FT_FRAME_END
    };


    table         = &face->name_table;
    table->stream = stream;

    error = face->goto_table( face, TTAG_name, stream, &table_len );
    if ( error )
      goto Exit;

    table_pos = FT_STREAM_POS();


    if ( FT_STREAM_READ_FIELDS( name_table_fields, table ) )
      goto Exit;

    /* Some popular Asian fonts have an invalid `storageOffset' value   */
    /* (it should be at least "6 + 12*num_names").  However, the string */
    /* offsets, computed as "storageOffset + entry->stringOffset", are  */
    /* valid pointers within the name table...                          */
    /*                                                                  */
    /* We thus can't check `storageOffset' right now.                   */
    /*                                                                  */
    storage_start = table_pos + 6 + 12*table->numNameRecords;
    storage_limit = table_pos + table_len;

    if ( storage_start > storage_limit )
    {
      FT_ERROR(( "tt_face_load_name: invalid `name' table\n" ));
      error = FT_THROW( Name_Table_Missing );
      goto Exit;
    }

    /* Allocate the array of name records. */
    count                 = table->numNameRecords;
    table->numNameRecords = 0;

    if ( FT_NEW_ARRAY( table->names, count ) ||
         FT_FRAME_ENTER( count * 12 )        )
      goto Exit;

    /* Load the name records and determine how much storage is needed */
    /* to hold the strings themselves.                                */
    {
      TT_NameEntryRec*  entry = table->names;


      for ( ; count > 0; count-- )
      {
        if ( FT_STREAM_READ_FIELDS( name_record_fields, entry ) )
          continue;

        /* check that the name is not empty */
        if ( entry->stringLength == 0 )
          continue;

        /* check that the name string is within the table */
        entry->stringOffset += table_pos + table->storageOffset;
        if ( entry->stringOffset                       < storage_start ||
             entry->stringOffset + entry->stringLength > storage_limit )
        {
          /* invalid entry - ignore it */
          entry->stringOffset = 0;
          entry->stringLength = 0;
          continue;
        }

        entry++;
      }

      table->numNameRecords = (FT_UInt)( entry - table->names );
    }

    FT_FRAME_EXIT();

    /* everything went well, update face->num_names */
    face->num_names = (FT_UShort) table->numNameRecords;

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_free_names                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Frees the name records.                                            */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A handle to the target face object.                        */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_face_free_name( TT_Face  face )
  {
    FT_Memory     memory = face->root.driver->root.memory;
    TT_NameTable  table  = &face->name_table;
    TT_NameEntry  entry  = table->names;
    FT_UInt       count  = table->numNameRecords;


    if ( table->names )
    {
      for ( ; count > 0; count--, entry++ )
      {
        FT_FREE( entry->string );
        entry->stringLength = 0;
      }

      /* free strings table */
      FT_FREE( table->names );
    }

    table->numNameRecords = 0;
    table->format         = 0;
    table->storageOffset  = 0;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_cmap                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the cmap directory in a face object.  The cmaps themselves   */
  /*    are loaded on demand in the `ttcmap.c' module.                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */

  FT_LOCAL_DEF( FT_Error )
  tt_face_load_cmap( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error  error;


    error = face->goto_table( face, TTAG_cmap, stream, &face->cmap_size );
    if ( error )
      goto Exit;

    if ( FT_FRAME_EXTRACT( face->cmap_size, face->cmap_table ) )
      face->cmap_size = 0;

  Exit:
    return error;
  }



  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_os2                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the OS2 table.                                               */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_os2( TT_Face    face,
                    FT_Stream  stream )
  {
    FT_Error  error;
    TT_OS2*   os2;

    static const FT_Frame_Field  os2_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_OS2

      FT_FRAME_START( 78 ),
        FT_FRAME_USHORT( version ),
        FT_FRAME_SHORT ( xAvgCharWidth ),
        FT_FRAME_USHORT( usWeightClass ),
        FT_FRAME_USHORT( usWidthClass ),
        FT_FRAME_SHORT ( fsType ),
        FT_FRAME_SHORT ( ySubscriptXSize ),
        FT_FRAME_SHORT ( ySubscriptYSize ),
        FT_FRAME_SHORT ( ySubscriptXOffset ),
        FT_FRAME_SHORT ( ySubscriptYOffset ),
        FT_FRAME_SHORT ( ySuperscriptXSize ),
        FT_FRAME_SHORT ( ySuperscriptYSize ),
        FT_FRAME_SHORT ( ySuperscriptXOffset ),
        FT_FRAME_SHORT ( ySuperscriptYOffset ),
        FT_FRAME_SHORT ( yStrikeoutSize ),
        FT_FRAME_SHORT ( yStrikeoutPosition ),
        FT_FRAME_SHORT ( sFamilyClass ),
        FT_FRAME_BYTE  ( panose[0] ),
        FT_FRAME_BYTE  ( panose[1] ),
        FT_FRAME_BYTE  ( panose[2] ),
        FT_FRAME_BYTE  ( panose[3] ),
        FT_FRAME_BYTE  ( panose[4] ),
        FT_FRAME_BYTE  ( panose[5] ),
        FT_FRAME_BYTE  ( panose[6] ),
        FT_FRAME_BYTE  ( panose[7] ),
        FT_FRAME_BYTE  ( panose[8] ),
        FT_FRAME_BYTE  ( panose[9] ),
        FT_FRAME_ULONG ( ulUnicodeRange1 ),
        FT_FRAME_ULONG ( ulUnicodeRange2 ),
        FT_FRAME_ULONG ( ulUnicodeRange3 ),
        FT_FRAME_ULONG ( ulUnicodeRange4 ),
        FT_FRAME_BYTE  ( achVendID[0] ),
        FT_FRAME_BYTE  ( achVendID[1] ),
        FT_FRAME_BYTE  ( achVendID[2] ),
        FT_FRAME_BYTE  ( achVendID[3] ),

        FT_FRAME_USHORT( fsSelection ),
        FT_FRAME_USHORT( usFirstCharIndex ),
        FT_FRAME_USHORT( usLastCharIndex ),
        FT_FRAME_SHORT ( sTypoAscender ),
        FT_FRAME_SHORT ( sTypoDescender ),
        FT_FRAME_SHORT ( sTypoLineGap ),
        FT_FRAME_USHORT( usWinAscent ),
        FT_FRAME_USHORT( usWinDescent ),
      FT_FRAME_END
    };

    /* `OS/2' version 1 and newer */
    static const FT_Frame_Field  os2_fields_extra1[] =
    {
      FT_FRAME_START( 8 ),
        FT_FRAME_ULONG( ulCodePageRange1 ),
        FT_FRAME_ULONG( ulCodePageRange2 ),
      FT_FRAME_END
    };

    /* `OS/2' version 2 and newer */
    static const FT_Frame_Field  os2_fields_extra2[] =
    {
      FT_FRAME_START( 10 ),
        FT_FRAME_SHORT ( sxHeight ),
        FT_FRAME_SHORT ( sCapHeight ),
        FT_FRAME_USHORT( usDefaultChar ),
        FT_FRAME_USHORT( usBreakChar ),
        FT_FRAME_USHORT( usMaxContext ),
      FT_FRAME_END
    };

    /* `OS/2' version 5 and newer */
    static const FT_Frame_Field  os2_fields_extra5[] =
    {
      FT_FRAME_START( 4 ),
        FT_FRAME_USHORT( usLowerOpticalPointSize ),
        FT_FRAME_USHORT( usUpperOpticalPointSize ),
      FT_FRAME_END
    };


    /* We now support old Mac fonts where the OS/2 table doesn't  */
    /* exist.  Simply put, we set the `version' field to 0xFFFF   */
    /* and test this value each time we need to access the table. */
    error = face->goto_table( face, TTAG_OS2, stream, 0 );
    if ( error )
      goto Exit;

    os2 = &face->os2;

    if ( FT_STREAM_READ_FIELDS( os2_fields, os2 ) )
      goto Exit;

    os2->ulCodePageRange1        = 0;
    os2->ulCodePageRange2        = 0;
    os2->sxHeight                = 0;
    os2->sCapHeight              = 0;
    os2->usDefaultChar           = 0;
    os2->usBreakChar             = 0;
    os2->usMaxContext            = 0;
    os2->usLowerOpticalPointSize = 0;
    os2->usUpperOpticalPointSize = 0xFFFF;

    if ( os2->version >= 0x0001 )
    {
      /* only version 1 tables */
      if ( FT_STREAM_READ_FIELDS( os2_fields_extra1, os2 ) )
        goto Exit;

      if ( os2->version >= 0x0002 )
      {
        /* only version 2 tables */
        if ( FT_STREAM_READ_FIELDS( os2_fields_extra2, os2 ) )
          goto Exit;

        if ( os2->version >= 0x0005 )
        {
          /* only version 5 tables */
          if ( FT_STREAM_READ_FIELDS( os2_fields_extra5, os2 ) )
            goto Exit;
        }
      }
    }

    FT_TRACE3(( "sTypoAscender:  %4d\n",   os2->sTypoAscender ));
    FT_TRACE3(( "sTypoDescender: %4d\n",   os2->sTypoDescender ));
    FT_TRACE3(( "usWinAscent:    %4u\n",   os2->usWinAscent ));
    FT_TRACE3(( "usWinDescent:   %4u\n",   os2->usWinDescent ));
    FT_TRACE3(( "fsSelection:    0x%2x\n", os2->fsSelection ));

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_postscript                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the Postscript table.                                        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_post( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error        error;
    TT_Postscript*  post = &face->postscript;

    static const FT_Frame_Field  post_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_Postscript

      FT_FRAME_START( 32 ),
        FT_FRAME_ULONG( FormatType ),
        FT_FRAME_ULONG( italicAngle ),
        FT_FRAME_SHORT( underlinePosition ),
        FT_FRAME_SHORT( underlineThickness ),
        FT_FRAME_ULONG( isFixedPitch ),
        FT_FRAME_ULONG( minMemType42 ),
        FT_FRAME_ULONG( maxMemType42 ),
        FT_FRAME_ULONG( minMemType1 ),
        FT_FRAME_ULONG( maxMemType1 ),
      FT_FRAME_END
    };


    error = face->goto_table( face, TTAG_post, stream, 0 );
    if ( error )
      return error;

    if ( FT_STREAM_READ_FIELDS( post_fields, post ) )
      return error;

    /* we don't load the glyph names, we do that in another */
    /* module (ttpost).                                     */

    FT_TRACE3(( "FormatType:   0x%x\n", post->FormatType ));
    FT_TRACE3(( "isFixedPitch:   %s\n", post->isFixedPitch
                                        ? "  yes" : "   no" ));

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_pclt                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the PCL 5 Table.                                             */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_pclt( TT_Face    face,
                     FT_Stream  stream )
  {
    static const FT_Frame_Field  pclt_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_PCLT

      FT_FRAME_START( 54 ),
        FT_FRAME_ULONG ( Version ),
        FT_FRAME_ULONG ( FontNumber ),
        FT_FRAME_USHORT( Pitch ),
        FT_FRAME_USHORT( xHeight ),
        FT_FRAME_USHORT( Style ),
        FT_FRAME_USHORT( TypeFamily ),
        FT_FRAME_USHORT( CapHeight ),
        FT_FRAME_USHORT( SymbolSet ),
        FT_FRAME_BYTES ( TypeFace, 16 ),
        FT_FRAME_BYTES ( CharacterComplement, 8 ),
        FT_FRAME_BYTES ( FileName, 6 ),
        FT_FRAME_CHAR  ( StrokeWeight ),
        FT_FRAME_CHAR  ( WidthType ),
        FT_FRAME_BYTE  ( SerifStyle ),
        FT_FRAME_BYTE  ( Reserved ),
      FT_FRAME_END
    };

    FT_Error  error;
    TT_PCLT*  pclt = &face->pclt;


    /* optional table */
    error = face->goto_table( face, TTAG_PCLT, stream, 0 );
    if ( error )
      goto Exit;

    if ( FT_STREAM_READ_FIELDS( pclt_fields, pclt ) )
      goto Exit;

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_gasp                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the `gasp' table into a face object.                         */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /*    stream :: The input stream.                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_gasp( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error   error;
    FT_Memory  memory = stream->memory;

    FT_UInt        j,num_ranges;
    TT_GaspRange   gaspranges = NULL;


    /* the gasp table is optional */
    error = face->goto_table( face, TTAG_gasp, stream, 0 );
    if ( error )
      goto Exit;

    if ( FT_FRAME_ENTER( 4L ) )
      goto Exit;

    face->gasp.version   = FT_GET_USHORT();
    face->gasp.numRanges = FT_GET_USHORT();

    FT_FRAME_EXIT();

    /* only support versions 0 and 1 of the table */
    if ( face->gasp.version >= 2 )
    {
      face->gasp.numRanges = 0;
      error = FT_THROW( Invalid_Table );
      goto Exit;
    }

    num_ranges = face->gasp.numRanges;
    FT_TRACE3(( "numRanges: %u\n", num_ranges ));

    if ( FT_QNEW_ARRAY( face->gasp.gaspRanges, num_ranges ) ||
         FT_FRAME_ENTER( num_ranges * 4L )                  )
      goto Exit;

    gaspranges = face->gasp.gaspRanges;

    for ( j = 0; j < num_ranges; j++ )
    {
      gaspranges[j].maxPPEM  = FT_GET_USHORT();
      gaspranges[j].gaspFlag = FT_GET_USHORT();

      FT_TRACE3(( "gaspRange %d: rangeMaxPPEM %5d, rangeGaspBehavior 0x%x\n",
                  j,
                  gaspranges[j].maxPPEM,
                  gaspranges[j].gaspFlag ));
    }

    FT_FRAME_EXIT();

  Exit:
    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttmtx.c                                                                */
/*                                                                         */
/*    Load the metrics tables common to TTF and OTF fonts (body).          */
/*                                                                         */
/*  Copyright 2006-2009, 2011-2013 by                                      */
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
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include "ttmtx.h"

#include "sferrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttmtx


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_hmtx                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the `hmtx' or `vmtx' table into a face object.                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face     :: A handle to the target face object.                    */
  /*                                                                       */
  /*    stream   :: The input stream.                                      */
  /*                                                                       */
  /*    vertical :: A boolean flag.  If set, load `vmtx'.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_hmtx( TT_Face    face,
                     FT_Stream  stream,
                     FT_Bool    vertical )
  {
    FT_Error   error;
    FT_ULong   tag, table_size;
    FT_ULong*  ptable_offset;
    FT_ULong*  ptable_size;


    if ( vertical )
    {
      tag           = TTAG_vmtx;
      ptable_offset = &face->vert_metrics_offset;
      ptable_size   = &face->vert_metrics_size;
    }
    else
    {
      tag           = TTAG_hmtx;
      ptable_offset = &face->horz_metrics_offset;
      ptable_size   = &face->horz_metrics_size;
    }

    error = face->goto_table( face, tag, stream, &table_size );
    if ( error )
      goto Fail;

    *ptable_size   = table_size;
    *ptable_offset = FT_STREAM_POS();

  Fail:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_hhea                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the `hhea' or 'vhea' table into a face object.                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face     :: A handle to the target face object.                    */
  /*                                                                       */
  /*    stream   :: The input stream.                                      */
  /*                                                                       */
  /*    vertical :: A boolean flag.  If set, load `vhea'.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_hhea( TT_Face    face,
                     FT_Stream  stream,
                     FT_Bool    vertical )
  {
    FT_Error        error;
    TT_HoriHeader*  header;

    static const FT_Frame_Field  metrics_header_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_HoriHeader

      FT_FRAME_START( 36 ),
        FT_FRAME_ULONG ( Version ),
        FT_FRAME_SHORT ( Ascender ),
        FT_FRAME_SHORT ( Descender ),
        FT_FRAME_SHORT ( Line_Gap ),
        FT_FRAME_USHORT( advance_Width_Max ),
        FT_FRAME_SHORT ( min_Left_Side_Bearing ),
        FT_FRAME_SHORT ( min_Right_Side_Bearing ),
        FT_FRAME_SHORT ( xMax_Extent ),
        FT_FRAME_SHORT ( caret_Slope_Rise ),
        FT_FRAME_SHORT ( caret_Slope_Run ),
        FT_FRAME_SHORT ( caret_Offset ),
        FT_FRAME_SHORT ( Reserved[0] ),
        FT_FRAME_SHORT ( Reserved[1] ),
        FT_FRAME_SHORT ( Reserved[2] ),
        FT_FRAME_SHORT ( Reserved[3] ),
        FT_FRAME_SHORT ( metric_Data_Format ),
        FT_FRAME_USHORT( number_Of_HMetrics ),
      FT_FRAME_END
    };


    if ( vertical )
    {
      void  *v = &face->vertical;


      error = face->goto_table( face, TTAG_vhea, stream, 0 );
      if ( error )
        goto Fail;

      header = (TT_HoriHeader*)v;
    }
    else
    {
      error = face->goto_table( face, TTAG_hhea, stream, 0 );
      if ( error )
        goto Fail;

      header = &face->horizontal;
    }

    if ( FT_STREAM_READ_FIELDS( metrics_header_fields, header ) )
      goto Fail;

    FT_TRACE3(( "Ascender:          %5d\n", header->Ascender ));
    FT_TRACE3(( "Descender:         %5d\n", header->Descender ));
    FT_TRACE3(( "number_Of_Metrics: %5u\n", header->number_Of_HMetrics ));

    header->long_metrics  = NULL;
    header->short_metrics = NULL;

  Fail:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_get_metrics                                                */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Return the horizontal or vertical metrics in font units for a      */
  /*    given glyph.  The values are the left side bearing (top side       */
  /*    bearing for vertical metrics) and advance width (advance height    */
  /*    for vertical metrics).                                             */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face     :: A pointer to the TrueType face structure.              */
  /*                                                                       */
  /*    vertical :: If set to TRUE, get vertical metrics.                  */
  /*                                                                       */
  /*    gindex   :: The glyph index.                                       */
  /*                                                                       */
  /* <Output>                                                              */
  /*    abearing :: The bearing, either left side or top side.             */
  /*                                                                       */
  /*    aadvance :: The advance width or advance height, depending on      */
  /*                the `vertical' flag.                                   */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_get_metrics( TT_Face     face,
                       FT_Bool     vertical,
                       FT_UInt     gindex,
                       FT_Short   *abearing,
                       FT_UShort  *aadvance )
  {
    FT_Error        error;
    FT_Stream       stream = face->root.stream;
    TT_HoriHeader*  header;
    FT_ULong        table_pos, table_size, table_end;
    FT_UShort       k;


    if ( vertical )
    {
      void*  v = &face->vertical;


      header     = (TT_HoriHeader*)v;
      table_pos  = face->vert_metrics_offset;
      table_size = face->vert_metrics_size;
    }
    else
    {
      header     = &face->horizontal;
      table_pos  = face->horz_metrics_offset;
      table_size = face->horz_metrics_size;
    }

    table_end = table_pos + table_size;

    k = header->number_Of_HMetrics;

    if ( k > 0 )
    {
      if ( gindex < (FT_UInt)k )
      {
        table_pos += 4 * gindex;
        if ( table_pos + 4 > table_end )
          goto NoData;

        if ( FT_STREAM_SEEK( table_pos ) ||
             FT_READ_USHORT( *aadvance ) ||
             FT_READ_SHORT( *abearing )  )
          goto NoData;
      }
      else
      {
        table_pos += 4 * ( k - 1 );
        if ( table_pos + 4 > table_end )
          goto NoData;

        if ( FT_STREAM_SEEK( table_pos ) ||
             FT_READ_USHORT( *aadvance ) )
          goto NoData;

        table_pos += 4 + 2 * ( gindex - k );
        if ( table_pos + 2 > table_end )
          *abearing = 0;
        else
        {
          if ( !FT_STREAM_SEEK( table_pos ) )
            (void)FT_READ_SHORT( *abearing );
        }
      }
    }
    else
    {
    NoData:
      *abearing = 0;
      *aadvance = 0;
    }

    return FT_Err_Ok;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttcmap.c                                                               */
/*                                                                         */
/*    TrueType character mapping table (cmap) support (body).              */
/*                                                                         */
/*  Copyright 2002-2010, 2012-2014 by                                      */
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

#include "sferrors.h"           /* must come before FT_INTERNAL_VALIDATE_H */

#include FT_INTERNAL_VALIDATE_H
#include FT_INTERNAL_STREAM_H
#include "ttload.h"
#include "ttcmap.h"
#include "sfntpic.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttcmap


#define TT_PEEK_SHORT   FT_PEEK_SHORT
#define TT_PEEK_USHORT  FT_PEEK_USHORT
#define TT_PEEK_UINT24  FT_PEEK_UOFF3
#define TT_PEEK_LONG    FT_PEEK_LONG
#define TT_PEEK_ULONG   FT_PEEK_ULONG

#define TT_NEXT_SHORT   FT_NEXT_SHORT
#define TT_NEXT_USHORT  FT_NEXT_USHORT
#define TT_NEXT_UINT24  FT_NEXT_UOFF3
#define TT_NEXT_LONG    FT_NEXT_LONG
#define TT_NEXT_ULONG   FT_NEXT_ULONG


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap_init( TT_CMap   cmap,
                FT_Byte*  table )
  {
    cmap->data = table;
    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                           FORMAT 0                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME        OFFSET         TYPE          DESCRIPTION                */
  /*                                                                       */
  /*   format      0              USHORT        must be 0                  */
  /*   length      2              USHORT        table length in bytes      */
  /*   language    4              USHORT        Mac language code          */
  /*   glyph_ids   6              BYTE[256]     array of glyph indices     */
  /*               262                                                     */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_0

  FT_CALLBACK_DEF( FT_Error )
  tt_cmap0_validate( FT_Byte*      table,
                     FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_UInt   length;


    if ( table + 2 + 2 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p      = table + 2;           /* skip format */
    length = TT_NEXT_USHORT( p );

    if ( table + length > valid->limit || length < 262 )
      FT_INVALID_TOO_SHORT;

    /* check glyph indices whenever necessary */
    if ( valid->level >= FT_VALIDATE_TIGHT )
    {
      FT_UInt  n, idx;


      p = table + 6;
      for ( n = 0; n < 256; n++ )
      {
        idx = *p++;
        if ( idx >= TT_VALID_GLYPH_COUNT( valid ) )
          FT_INVALID_GLYPH_ID;
      }
    }

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap0_char_index( TT_CMap    cmap,
                       FT_UInt32  char_code )
  {
    FT_Byte*  table = cmap->data;


    return char_code < 256 ? table[6 + char_code] : 0;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap0_char_next( TT_CMap     cmap,
                      FT_UInt32  *pchar_code )
  {
    FT_Byte*   table    = cmap->data;
    FT_UInt32  charcode = *pchar_code;
    FT_UInt32  result   = 0;
    FT_UInt    gindex   = 0;


    table += 6;  /* go to glyph IDs */
    while ( ++charcode < 256 )
    {
      gindex = table[charcode];
      if ( gindex != 0 )
      {
        result = charcode;
        break;
      }
    }

    *pchar_code = result;
    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap0_get_info( TT_CMap       cmap,
                     TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 4;


    cmap_info->format   = 0;
    cmap_info->language = (FT_ULong)TT_PEEK_USHORT( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap0_class_rec,
    sizeof ( TT_CMapRec ),

    (FT_CMap_InitFunc)     tt_cmap_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap0_char_index,
    (FT_CMap_CharNextFunc) tt_cmap0_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    0,
    (TT_CMap_ValidateFunc)tt_cmap0_validate,
    (TT_CMap_Info_GetFunc)tt_cmap0_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_0 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          FORMAT 2                             *****/
  /*****                                                               *****/
  /***** This is used for certain CJK encodings that encode text in a  *****/
  /***** mixed 8/16 bits encoding along the following lines:           *****/
  /*****                                                               *****/
  /***** * Certain byte values correspond to an 8-bit character code   *****/
  /*****   (typically in the range 0..127 for ASCII compatibility).    *****/
  /*****                                                               *****/
  /***** * Certain byte values signal the first byte of a 2-byte       *****/
  /*****   character code (but these values are also valid as the      *****/
  /*****   second byte of a 2-byte character).                         *****/
  /*****                                                               *****/
  /***** The following charmap lookup and iteration functions all      *****/
  /***** assume that the value "charcode" correspond to following:     *****/
  /*****                                                               *****/
  /*****   - For one byte characters, "charcode" is simply the         *****/
  /*****     character code.                                           *****/
  /*****                                                               *****/
  /*****   - For two byte characters, "charcode" is the 2-byte         *****/
  /*****     character code in big endian format.  More exactly:       *****/
  /*****                                                               *****/
  /*****       (charcode >> 8)    is the first byte value              *****/
  /*****       (charcode & 0xFF)  is the second byte value             *****/
  /*****                                                               *****/
  /***** Note that not all values of "charcode" are valid according    *****/
  /***** to these rules, and the function moderately check the         *****/
  /***** arguments.                                                    *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME        OFFSET         TYPE            DESCRIPTION              */
  /*                                                                       */
  /*   format      0              USHORT          must be 2                */
  /*   length      2              USHORT          table length in bytes    */
  /*   language    4              USHORT          Mac language code        */
  /*   keys        6              USHORT[256]     sub-header keys          */
  /*   subs        518            SUBHEAD[NSUBS]  sub-headers array        */
  /*   glyph_ids   518+NSUB*8     USHORT[]        glyph ID array           */
  /*                                                                       */
  /* The `keys' table is used to map charcode high-bytes to sub-headers.   */
  /* The value of `NSUBS' is the number of sub-headers defined in the      */
  /* table and is computed by finding the maximum of the `keys' table.     */
  /*                                                                       */
  /* Note that for any n, `keys[n]' is a byte offset within the `subs'     */
  /* table, i.e., it is the corresponding sub-header index multiplied      */
  /* by 8.                                                                 */
  /*                                                                       */
  /* Each sub-header has the following format:                             */
  /*                                                                       */
  /*   NAME        OFFSET      TYPE            DESCRIPTION                 */
  /*                                                                       */
  /*   first       0           USHORT          first valid low-byte        */
  /*   count       2           USHORT          number of valid low-bytes   */
  /*   delta       4           SHORT           see below                   */
  /*   offset      6           USHORT          see below                   */
  /*                                                                       */
  /* A sub-header defines, for each high-byte, the range of valid          */
  /* low-bytes within the charmap.  Note that the range defined by `first' */
  /* and `count' must be completely included in the interval [0..255]      */
  /* according to the specification.                                       */
  /*                                                                       */
  /* If a character code is contained within a given sub-header, then      */
  /* mapping it to a glyph index is done as follows:                       */
  /*                                                                       */
  /* * The value of `offset' is read.  This is a _byte_ distance from the  */
  /*   location of the `offset' field itself into a slice of the           */
  /*   `glyph_ids' table.  Let's call it `slice' (it is a USHORT[] too).   */
  /*                                                                       */
  /* * The value `slice[char.lo - first]' is read.  If it is 0, there is   */
  /*   no glyph for the charcode.  Otherwise, the value of `delta' is      */
  /*   added to it (modulo 65536) to form a new glyph index.               */
  /*                                                                       */
  /* It is up to the validation routine to check that all offsets fall     */
  /* within the glyph IDs table (and not within the `subs' table itself or */
  /* outside of the CMap).                                                 */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_2

  FT_CALLBACK_DEF( FT_Error )
  tt_cmap2_validate( FT_Byte*      table,
                     FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_UInt   length;

    FT_UInt   n, max_subs;
    FT_Byte*  keys;        /* keys table     */
    FT_Byte*  subs;        /* sub-headers    */
    FT_Byte*  glyph_ids;   /* glyph ID array */


    if ( table + 2 + 2 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p      = table + 2;           /* skip format */
    length = TT_NEXT_USHORT( p );

    if ( table + length > valid->limit || length < 6 + 512 )
      FT_INVALID_TOO_SHORT;

    keys = table + 6;

    /* parse keys to compute sub-headers count */
    p        = keys;
    max_subs = 0;
    for ( n = 0; n < 256; n++ )
    {
      FT_UInt  idx = TT_NEXT_USHORT( p );


      /* value must be multiple of 8 */
      if ( valid->level >= FT_VALIDATE_PARANOID && ( idx & 7 ) != 0 )
        FT_INVALID_DATA;

      idx >>= 3;

      if ( idx > max_subs )
        max_subs = idx;
    }

    FT_ASSERT( p == table + 518 );

    subs      = p;
    glyph_ids = subs + (max_subs + 1) * 8;
    if ( glyph_ids > valid->limit )
      FT_INVALID_TOO_SHORT;

    /* parse sub-headers */
    for ( n = 0; n <= max_subs; n++ )
    {
      FT_UInt  first_code, code_count, offset;
      FT_Int   delta;


      first_code = TT_NEXT_USHORT( p );
      code_count = TT_NEXT_USHORT( p );
      delta      = TT_NEXT_SHORT( p );
      offset     = TT_NEXT_USHORT( p );

      /* many Dynalab fonts have empty sub-headers */
      if ( code_count == 0 )
        continue;

      /* check range within 0..255 */
      if ( valid->level >= FT_VALIDATE_PARANOID )
      {
        if ( first_code >= 256 || first_code + code_count > 256 )
          FT_INVALID_DATA;
      }

      /* check offset */
      if ( offset != 0 )
      {
        FT_Byte*  ids;


        ids = p - 2 + offset;
        if ( ids < glyph_ids || ids + code_count*2 > table + length )
          FT_INVALID_OFFSET;

        /* check glyph IDs */
        if ( valid->level >= FT_VALIDATE_TIGHT )
        {
          FT_Byte*  limit = p + code_count * 2;
          FT_UInt   idx;


          for ( ; p < limit; )
          {
            idx = TT_NEXT_USHORT( p );
            if ( idx != 0 )
            {
              idx = ( idx + delta ) & 0xFFFFU;
              if ( idx >= TT_VALID_GLYPH_COUNT( valid ) )
                FT_INVALID_GLYPH_ID;
            }
          }
        }
      }
    }

    return FT_Err_Ok;
  }


  /* return sub header corresponding to a given character code */
  /* NULL on invalid charcode                                  */
  static FT_Byte*
  tt_cmap2_get_subheader( FT_Byte*   table,
                          FT_UInt32  char_code )
  {
    FT_Byte*  result = NULL;


    if ( char_code < 0x10000UL )
    {
      FT_UInt   char_lo = (FT_UInt)( char_code & 0xFF );
      FT_UInt   char_hi = (FT_UInt)( char_code >> 8 );
      FT_Byte*  p       = table + 6;    /* keys table */
      FT_Byte*  subs    = table + 518;  /* subheaders table */
      FT_Byte*  sub;


      if ( char_hi == 0 )
      {
        /* an 8-bit character code -- we use subHeader 0 in this case */
        /* to test whether the character code is in the charmap       */
        /*                                                            */
        sub = subs;  /* jump to first sub-header */

        /* check that the sub-header for this byte is 0, which */
        /* indicates that it is really a valid one-byte value  */
        /* Otherwise, return 0                                 */
        /*                                                     */
        p += char_lo * 2;
        if ( TT_PEEK_USHORT( p ) != 0 )
          goto Exit;
      }
      else
      {
        /* a 16-bit character code */

        /* jump to key entry  */
        p  += char_hi * 2;
        /* jump to sub-header */
        sub = subs + ( FT_PAD_FLOOR( TT_PEEK_USHORT( p ), 8 ) );

        /* check that the high byte isn't a valid one-byte value */
        if ( sub == subs )
          goto Exit;
      }
      result = sub;
    }
  Exit:
    return result;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap2_char_index( TT_CMap    cmap,
                       FT_UInt32  char_code )
  {
    FT_Byte*  table   = cmap->data;
    FT_UInt   result  = 0;
    FT_Byte*  subheader;


    subheader = tt_cmap2_get_subheader( table, char_code );
    if ( subheader )
    {
      FT_Byte*  p   = subheader;
      FT_UInt   idx = (FT_UInt)(char_code & 0xFF);
      FT_UInt   start, count;
      FT_Int    delta;
      FT_UInt   offset;


      start  = TT_NEXT_USHORT( p );
      count  = TT_NEXT_USHORT( p );
      delta  = TT_NEXT_SHORT ( p );
      offset = TT_PEEK_USHORT( p );

      idx -= start;
      if ( idx < count && offset != 0 )
      {
        p  += offset + 2 * idx;
        idx = TT_PEEK_USHORT( p );

        if ( idx != 0 )
          result = (FT_UInt)( idx + delta ) & 0xFFFFU;
      }
    }
    return result;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap2_char_next( TT_CMap     cmap,
                      FT_UInt32  *pcharcode )
  {
    FT_Byte*   table    = cmap->data;
    FT_UInt    gindex   = 0;
    FT_UInt32  result   = 0;
    FT_UInt32  charcode = *pcharcode + 1;
    FT_Byte*   subheader;


    while ( charcode < 0x10000UL )
    {
      subheader = tt_cmap2_get_subheader( table, charcode );
      if ( subheader )
      {
        FT_Byte*  p       = subheader;
        FT_UInt   start   = TT_NEXT_USHORT( p );
        FT_UInt   count   = TT_NEXT_USHORT( p );
        FT_Int    delta   = TT_NEXT_SHORT ( p );
        FT_UInt   offset  = TT_PEEK_USHORT( p );
        FT_UInt   char_lo = (FT_UInt)( charcode & 0xFF );
        FT_UInt   pos, idx;


        if ( offset == 0 )
          goto Next_SubHeader;

        if ( char_lo < start )
        {
          char_lo = start;
          pos     = 0;
        }
        else
          pos = (FT_UInt)( char_lo - start );

        p       += offset + pos * 2;
        charcode = FT_PAD_FLOOR( charcode, 256 ) + char_lo;

        for ( ; pos < count; pos++, charcode++ )
        {
          idx = TT_NEXT_USHORT( p );

          if ( idx != 0 )
          {
            gindex = ( idx + delta ) & 0xFFFFU;
            if ( gindex != 0 )
            {
              result = charcode;
              goto Exit;
            }
          }
        }
      }

      /* jump to next sub-header, i.e. higher byte value */
    Next_SubHeader:
      charcode = FT_PAD_FLOOR( charcode, 256 ) + 256;
    }

  Exit:
    *pcharcode = result;

    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap2_get_info( TT_CMap       cmap,
                     TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 4;


    cmap_info->format   = 2;
    cmap_info->language = (FT_ULong)TT_PEEK_USHORT( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap2_class_rec,
    sizeof ( TT_CMapRec ),

    (FT_CMap_InitFunc)     tt_cmap_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap2_char_index,
    (FT_CMap_CharNextFunc) tt_cmap2_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    2,
    (TT_CMap_ValidateFunc)tt_cmap2_validate,
    (TT_CMap_Info_GetFunc)tt_cmap2_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_2 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                           FORMAT 4                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME          OFFSET         TYPE              DESCRIPTION          */
  /*                                                                       */
  /*   format        0              USHORT            must be 4            */
  /*   length        2              USHORT            table length         */
  /*                                                  in bytes             */
  /*   language      4              USHORT            Mac language code    */
  /*                                                                       */
  /*   segCountX2    6              USHORT            2*NUM_SEGS           */
  /*   searchRange   8              USHORT            2*(1 << LOG_SEGS)    */
  /*   entrySelector 10             USHORT            LOG_SEGS             */
  /*   rangeShift    12             USHORT            segCountX2 -         */
  /*                                                    searchRange        */
  /*                                                                       */
  /*   endCount      14             USHORT[NUM_SEGS]  end charcode for     */
  /*                                                  each segment; last   */
  /*                                                  is 0xFFFF            */
  /*                                                                       */
  /*   pad           14+NUM_SEGS*2  USHORT            padding              */
  /*                                                                       */
  /*   startCount    16+NUM_SEGS*2  USHORT[NUM_SEGS]  first charcode for   */
  /*                                                  each segment         */
  /*                                                                       */
  /*   idDelta       16+NUM_SEGS*4  SHORT[NUM_SEGS]   delta for each       */
  /*                                                  segment              */
  /*   idOffset      16+NUM_SEGS*6  SHORT[NUM_SEGS]   range offset for     */
  /*                                                  each segment; can be */
  /*                                                  zero                 */
  /*                                                                       */
  /*   glyphIds      16+NUM_SEGS*8  USHORT[]          array of glyph ID    */
  /*                                                  ranges               */
  /*                                                                       */
  /* Character codes are modelled by a series of ordered (increasing)      */
  /* intervals called segments.  Each segment has start and end codes,     */
  /* provided by the `startCount' and `endCount' arrays.  Segments must    */
  /* not overlap, and the last segment should always contain the value     */
  /* 0xFFFF for `endCount'.                                                */
  /*                                                                       */
  /* The fields `searchRange', `entrySelector' and `rangeShift' are better */
  /* ignored (they are traces of over-engineering in the TrueType          */
  /* specification).                                                       */
  /*                                                                       */
  /* Each segment also has a signed `delta', as well as an optional offset */
  /* within the `glyphIds' table.                                          */
  /*                                                                       */
  /* If a segment's idOffset is 0, the glyph index corresponding to any    */
  /* charcode within the segment is obtained by adding the value of        */
  /* `idDelta' directly to the charcode, modulo 65536.                     */
  /*                                                                       */
  /* Otherwise, a glyph index is taken from the glyph IDs sub-array for    */
  /* the segment, and the value of `idDelta' is added to it.               */
  /*                                                                       */
  /*                                                                       */
  /* Finally, note that a lot of fonts contain an invalid last segment,    */
  /* where `start' and `end' are correctly set to 0xFFFF but both `delta'  */
  /* and `offset' are incorrect (e.g., `opens___.ttf' which comes with     */
  /* OpenOffice.org).  We need special code to deal with them correctly.   */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_4

  typedef struct  TT_CMap4Rec_
  {
    TT_CMapRec  cmap;
    FT_UInt32   cur_charcode;   /* current charcode */
    FT_UInt     cur_gindex;     /* current glyph index */

    FT_UInt     num_ranges;
    FT_UInt     cur_range;
    FT_UInt     cur_start;
    FT_UInt     cur_end;
    FT_Int      cur_delta;
    FT_Byte*    cur_values;

  } TT_CMap4Rec, *TT_CMap4;


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap4_init( TT_CMap4  cmap,
                 FT_Byte*  table )
  {
    FT_Byte*  p;


    cmap->cmap.data    = table;

    p                  = table + 6;
    cmap->num_ranges   = FT_PEEK_USHORT( p ) >> 1;
    cmap->cur_charcode = (FT_UInt32)0xFFFFFFFFUL;
    cmap->cur_gindex   = 0;

    return FT_Err_Ok;
  }


  static FT_Int
  tt_cmap4_set_range( TT_CMap4  cmap,
                      FT_UInt   range_index )
  {
    FT_Byte*  table = cmap->cmap.data;
    FT_Byte*  p;
    FT_UInt   num_ranges = cmap->num_ranges;


    while ( range_index < num_ranges )
    {
      FT_UInt  offset;


      p             = table + 14 + range_index * 2;
      cmap->cur_end = FT_PEEK_USHORT( p );

      p              += 2 + num_ranges * 2;
      cmap->cur_start = FT_PEEK_USHORT( p );

      p              += num_ranges * 2;
      cmap->cur_delta = FT_PEEK_SHORT( p );

      p     += num_ranges * 2;
      offset = FT_PEEK_USHORT( p );

      /* some fonts have an incorrect last segment; */
      /* we have to catch it                        */
      if ( range_index     >= num_ranges - 1 &&
           cmap->cur_start == 0xFFFFU        &&
           cmap->cur_end   == 0xFFFFU        )
      {
        TT_Face   face  = (TT_Face)cmap->cmap.cmap.charmap.face;
        FT_Byte*  limit = face->cmap_table + face->cmap_size;


        if ( offset && p + offset + 2 > limit )
        {
          cmap->cur_delta = 1;
          offset          = 0;
        }
      }

      if ( offset != 0xFFFFU )
      {
        cmap->cur_values = offset ? p + offset : NULL;
        cmap->cur_range  = range_index;
        return 0;
      }

      /* we skip empty segments */
      range_index++;
    }

    return -1;
  }


  /* search the index of the charcode next to cmap->cur_charcode; */
  /* caller should call tt_cmap4_set_range with proper range      */
  /* before calling this function                                 */
  /*                                                              */
  static void
  tt_cmap4_next( TT_CMap4  cmap )
  {
    FT_UInt  charcode;


    if ( cmap->cur_charcode >= 0xFFFFUL )
      goto Fail;

    charcode = (FT_UInt)cmap->cur_charcode + 1;

    if ( charcode < cmap->cur_start )
      charcode = cmap->cur_start;

    for ( ;; )
    {
      FT_Byte*  values = cmap->cur_values;
      FT_UInt   end    = cmap->cur_end;
      FT_Int    delta  = cmap->cur_delta;


      if ( charcode <= end )
      {
        if ( values )
        {
          FT_Byte*  p = values + 2 * ( charcode - cmap->cur_start );


          do
          {
            FT_UInt  gindex = FT_NEXT_USHORT( p );


            if ( gindex != 0 )
            {
              gindex = (FT_UInt)( ( gindex + delta ) & 0xFFFFU );
              if ( gindex != 0 )
              {
                cmap->cur_charcode = charcode;
                cmap->cur_gindex   = gindex;
                return;
              }
            }
          } while ( ++charcode <= end );
        }
        else
        {
          do
          {
            FT_UInt  gindex = (FT_UInt)( ( charcode + delta ) & 0xFFFFU );


            if ( gindex != 0 )
            {
              cmap->cur_charcode = charcode;
              cmap->cur_gindex   = gindex;
              return;
            }
          } while ( ++charcode <= end );
        }
      }

      /* we need to find another range */
      if ( tt_cmap4_set_range( cmap, cmap->cur_range + 1 ) < 0 )
        break;

      if ( charcode < cmap->cur_start )
        charcode = cmap->cur_start;
    }

  Fail:
    cmap->cur_charcode = (FT_UInt32)0xFFFFFFFFUL;
    cmap->cur_gindex   = 0;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap4_validate( FT_Byte*      table,
                     FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_UInt   length;

    FT_Byte   *ends, *starts, *offsets, *deltas, *glyph_ids;
    FT_UInt   num_segs;
    FT_Error  error = FT_Err_Ok;


    if ( table + 2 + 2 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p      = table + 2;           /* skip format */
    length = TT_NEXT_USHORT( p );

    if ( length < 16 )
      FT_INVALID_TOO_SHORT;

    /* in certain fonts, the `length' field is invalid and goes */
    /* out of bound.  We try to correct this here...            */
    if ( table + length > valid->limit )
    {
      if ( valid->level >= FT_VALIDATE_TIGHT )
        FT_INVALID_TOO_SHORT;

      length = (FT_UInt)( valid->limit - table );
    }

    p        = table + 6;
    num_segs = TT_NEXT_USHORT( p );   /* read segCountX2 */

    if ( valid->level >= FT_VALIDATE_PARANOID )
    {
      /* check that we have an even value here */
      if ( num_segs & 1 )
        FT_INVALID_DATA;
    }

    num_segs /= 2;

    if ( length < 16 + num_segs * 2 * 4 )
      FT_INVALID_TOO_SHORT;

    /* check the search parameters - even though we never use them */
    /*                                                             */
    if ( valid->level >= FT_VALIDATE_PARANOID )
    {
      /* check the values of `searchRange', `entrySelector', `rangeShift' */
      FT_UInt  search_range   = TT_NEXT_USHORT( p );
      FT_UInt  entry_selector = TT_NEXT_USHORT( p );
      FT_UInt  range_shift    = TT_NEXT_USHORT( p );


      if ( ( search_range | range_shift ) & 1 )  /* must be even values */
        FT_INVALID_DATA;

      search_range /= 2;
      range_shift  /= 2;

      /* `search range' is the greatest power of 2 that is <= num_segs */

      if ( search_range                > num_segs                 ||
           search_range * 2            < num_segs                 ||
           search_range + range_shift != num_segs                 ||
           search_range               != ( 1U << entry_selector ) )
        FT_INVALID_DATA;
    }

    ends      = table   + 14;
    starts    = table   + 16 + num_segs * 2;
    deltas    = starts  + num_segs * 2;
    offsets   = deltas  + num_segs * 2;
    glyph_ids = offsets + num_segs * 2;

    /* check last segment; its end count value must be 0xFFFF */
    if ( valid->level >= FT_VALIDATE_PARANOID )
    {
      p = ends + ( num_segs - 1 ) * 2;
      if ( TT_PEEK_USHORT( p ) != 0xFFFFU )
        FT_INVALID_DATA;
    }

    {
      FT_UInt   start, end, offset, n;
      FT_UInt   last_start = 0, last_end = 0;
      FT_Int    delta;
      FT_Byte*  p_start   = starts;
      FT_Byte*  p_end     = ends;
      FT_Byte*  p_delta   = deltas;
      FT_Byte*  p_offset  = offsets;


      for ( n = 0; n < num_segs; n++ )
      {
        p      = p_offset;
        start  = TT_NEXT_USHORT( p_start );
        end    = TT_NEXT_USHORT( p_end );
        delta  = TT_NEXT_SHORT( p_delta );
        offset = TT_NEXT_USHORT( p_offset );

        if ( start > end )
          FT_INVALID_DATA;

        /* this test should be performed at default validation level; */
        /* unfortunately, some popular Asian fonts have overlapping   */
        /* ranges in their charmaps                                   */
        /*                                                            */
        if ( start <= last_end && n > 0 )
        {
          if ( valid->level >= FT_VALIDATE_TIGHT )
            FT_INVALID_DATA;
          else
          {
            /* allow overlapping segments, provided their start points */
            /* and end points, respectively, are in ascending order    */
            /*                                                         */
            if ( last_start > start || last_end > end )
              error |= TT_CMAP_FLAG_UNSORTED;
            else
              error |= TT_CMAP_FLAG_OVERLAPPING;
          }
        }

        if ( offset && offset != 0xFFFFU )
        {
          p += offset;  /* start of glyph ID array */

          /* check that we point within the glyph IDs table only */
          if ( valid->level >= FT_VALIDATE_TIGHT )
          {
            if ( p < glyph_ids                                ||
                 p + ( end - start + 1 ) * 2 > table + length )
              FT_INVALID_DATA;
          }
          /* Some fonts handle the last segment incorrectly.  In */
          /* theory, 0xFFFF might point to an ordinary glyph --  */
          /* a cmap 4 is versatile and could be used for any     */
          /* encoding, not only Unicode.  However, reality shows */
          /* that far too many fonts are sloppy and incorrectly  */
          /* set all fields but `start' and `end' for the last   */
          /* segment if it contains only a single character.     */
          /*                                                     */
          /* We thus omit the test here, delaying it to the      */
          /* routines which actually access the cmap.            */
          else if ( n != num_segs - 1                       ||
                    !( start == 0xFFFFU && end == 0xFFFFU ) )
          {
            if ( p < glyph_ids                              ||
                 p + ( end - start + 1 ) * 2 > valid->limit )
              FT_INVALID_DATA;
          }

          /* check glyph indices within the segment range */
          if ( valid->level >= FT_VALIDATE_TIGHT )
          {
            FT_UInt  i, idx;


            for ( i = start; i < end; i++ )
            {
              idx = FT_NEXT_USHORT( p );
              if ( idx != 0 )
              {
                idx = (FT_UInt)( idx + delta ) & 0xFFFFU;

                if ( idx >= TT_VALID_GLYPH_COUNT( valid ) )
                  FT_INVALID_GLYPH_ID;
              }
            }
          }
        }
        else if ( offset == 0xFFFFU )
        {
          /* some fonts (erroneously?) use a range offset of 0xFFFF */
          /* to mean missing glyph in cmap table                    */
          /*                                                        */
          if ( valid->level >= FT_VALIDATE_PARANOID    ||
               n != num_segs - 1                       ||
               !( start == 0xFFFFU && end == 0xFFFFU ) )
            FT_INVALID_DATA;
        }

        last_start = start;
        last_end   = end;
      }
    }

    return error;
  }


  static FT_UInt
  tt_cmap4_char_map_linear( TT_CMap     cmap,
                            FT_UInt32*  pcharcode,
                            FT_Bool     next )
  {
    FT_UInt    num_segs2, start, end, offset;
    FT_Int     delta;
    FT_UInt    i, num_segs;
    FT_UInt32  charcode = *pcharcode;
    FT_UInt    gindex   = 0;
    FT_Byte*   p;


    p = cmap->data + 6;
    num_segs2 = FT_PAD_FLOOR( TT_PEEK_USHORT( p ), 2 );

    num_segs = num_segs2 >> 1;

    if ( !num_segs )
      return 0;

    if ( next )
      charcode++;

    /* linear search */
    for ( ; charcode <= 0xFFFFU; charcode++ )
    {
      FT_Byte*  q;


      p = cmap->data + 14;               /* ends table   */
      q = cmap->data + 16 + num_segs2;   /* starts table */

      for ( i = 0; i < num_segs; i++ )
      {
        end   = TT_NEXT_USHORT( p );
        start = TT_NEXT_USHORT( q );

        if ( charcode >= start && charcode <= end )
        {
          p       = q - 2 + num_segs2;
          delta   = TT_PEEK_SHORT( p );
          p      += num_segs2;
          offset  = TT_PEEK_USHORT( p );

          /* some fonts have an incorrect last segment; */
          /* we have to catch it                        */
          if ( i >= num_segs - 1                  &&
               start == 0xFFFFU && end == 0xFFFFU )
          {
            TT_Face   face  = (TT_Face)cmap->cmap.charmap.face;
            FT_Byte*  limit = face->cmap_table + face->cmap_size;


            if ( offset && p + offset + 2 > limit )
            {
              delta  = 1;
              offset = 0;
            }
          }

          if ( offset == 0xFFFFU )
            continue;

          if ( offset )
          {
            p += offset + ( charcode - start ) * 2;
            gindex = TT_PEEK_USHORT( p );
            if ( gindex != 0 )
              gindex = (FT_UInt)( gindex + delta ) & 0xFFFFU;
          }
          else
            gindex = (FT_UInt)( charcode + delta ) & 0xFFFFU;

          break;
        }
      }

      if ( !next || gindex )
        break;
    }

    if ( next && gindex )
      *pcharcode = charcode;

    return gindex;
  }


  static FT_UInt
  tt_cmap4_char_map_binary( TT_CMap     cmap,
                            FT_UInt32*  pcharcode,
                            FT_Bool     next )
  {
    FT_UInt   num_segs2, start, end, offset;
    FT_Int    delta;
    FT_UInt   max, min, mid, num_segs;
    FT_UInt   charcode = (FT_UInt)*pcharcode;
    FT_UInt   gindex   = 0;
    FT_Byte*  p;


    p = cmap->data + 6;
    num_segs2 = FT_PAD_FLOOR( TT_PEEK_USHORT( p ), 2 );

    if ( !num_segs2 )
      return 0;

    num_segs = num_segs2 >> 1;

    /* make compiler happy */
    mid = num_segs;
    end = 0xFFFFU;

    if ( next )
      charcode++;

    min = 0;
    max = num_segs;

    /* binary search */
    while ( min < max )
    {
      mid    = ( min + max ) >> 1;
      p      = cmap->data + 14 + mid * 2;
      end    = TT_PEEK_USHORT( p );
      p     += 2 + num_segs2;
      start  = TT_PEEK_USHORT( p );

      if ( charcode < start )
        max = mid;
      else if ( charcode > end )
        min = mid + 1;
      else
      {
        p     += num_segs2;
        delta  = TT_PEEK_SHORT( p );
        p     += num_segs2;
        offset = TT_PEEK_USHORT( p );

        /* some fonts have an incorrect last segment; */
        /* we have to catch it                        */
        if ( mid >= num_segs - 1                &&
             start == 0xFFFFU && end == 0xFFFFU )
        {
          TT_Face   face  = (TT_Face)cmap->cmap.charmap.face;
          FT_Byte*  limit = face->cmap_table + face->cmap_size;


          if ( offset && p + offset + 2 > limit )
          {
            delta  = 1;
            offset = 0;
          }
        }

        /* search the first segment containing `charcode' */
        if ( cmap->flags & TT_CMAP_FLAG_OVERLAPPING )
        {
          FT_UInt  i;


          /* call the current segment `max' */
          max = mid;

          if ( offset == 0xFFFFU )
            mid = max + 1;

          /* search in segments before the current segment */
          for ( i = max ; i > 0; i-- )
          {
            FT_UInt   prev_end;
            FT_Byte*  old_p;


            old_p    = p;
            p        = cmap->data + 14 + ( i - 1 ) * 2;
            prev_end = TT_PEEK_USHORT( p );

            if ( charcode > prev_end )
            {
              p = old_p;
              break;
            }

            end    = prev_end;
            p     += 2 + num_segs2;
            start  = TT_PEEK_USHORT( p );
            p     += num_segs2;
            delta  = TT_PEEK_SHORT( p );
            p     += num_segs2;
            offset = TT_PEEK_USHORT( p );

            if ( offset != 0xFFFFU )
              mid = i - 1;
          }

          /* no luck */
          if ( mid == max + 1 )
          {
            if ( i != max )
            {
              p      = cmap->data + 14 + max * 2;
              end    = TT_PEEK_USHORT( p );
              p     += 2 + num_segs2;
              start  = TT_PEEK_USHORT( p );
              p     += num_segs2;
              delta  = TT_PEEK_SHORT( p );
              p     += num_segs2;
              offset = TT_PEEK_USHORT( p );
            }

            mid = max;

            /* search in segments after the current segment */
            for ( i = max + 1; i < num_segs; i++ )
            {
              FT_UInt  next_end, next_start;


              p          = cmap->data + 14 + i * 2;
              next_end   = TT_PEEK_USHORT( p );
              p         += 2 + num_segs2;
              next_start = TT_PEEK_USHORT( p );

              if ( charcode < next_start )
                break;

              end    = next_end;
              start  = next_start;
              p     += num_segs2;
              delta  = TT_PEEK_SHORT( p );
              p     += num_segs2;
              offset = TT_PEEK_USHORT( p );

              if ( offset != 0xFFFFU )
                mid = i;
            }
            i--;

            /* still no luck */
            if ( mid == max )
            {
              mid = i;

              break;
            }
          }

          /* end, start, delta, and offset are for the i'th segment */
          if ( mid != i )
          {
            p      = cmap->data + 14 + mid * 2;
            end    = TT_PEEK_USHORT( p );
            p     += 2 + num_segs2;
            start  = TT_PEEK_USHORT( p );
            p     += num_segs2;
            delta  = TT_PEEK_SHORT( p );
            p     += num_segs2;
            offset = TT_PEEK_USHORT( p );
          }
        }
        else
        {
          if ( offset == 0xFFFFU )
            break;
        }

        if ( offset )
        {
          p += offset + ( charcode - start ) * 2;
          gindex = TT_PEEK_USHORT( p );
          if ( gindex != 0 )
            gindex = (FT_UInt)( gindex + delta ) & 0xFFFFU;
        }
        else
          gindex = (FT_UInt)( charcode + delta ) & 0xFFFFU;

        break;
      }
    }

    if ( next )
    {
      TT_CMap4  cmap4 = (TT_CMap4)cmap;


      /* if `charcode' is not in any segment, then `mid' is */
      /* the segment nearest to `charcode'                  */
      /*                                                    */

      if ( charcode > end )
      {
        mid++;
        if ( mid == num_segs )
          return 0;
      }

      if ( tt_cmap4_set_range( cmap4, mid ) )
      {
        if ( gindex )
          *pcharcode = charcode;
      }
      else
      {
        cmap4->cur_charcode = charcode;

        if ( gindex )
          cmap4->cur_gindex = gindex;
        else
        {
          cmap4->cur_charcode = charcode;
          tt_cmap4_next( cmap4 );
          gindex = cmap4->cur_gindex;
        }

        if ( gindex )
          *pcharcode = cmap4->cur_charcode;
      }
    }

    return gindex;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap4_char_index( TT_CMap    cmap,
                       FT_UInt32  char_code )
  {
    if ( char_code >= 0x10000UL )
      return 0;

    if ( cmap->flags & TT_CMAP_FLAG_UNSORTED )
      return tt_cmap4_char_map_linear( cmap, &char_code, 0 );
    else
      return tt_cmap4_char_map_binary( cmap, &char_code, 0 );
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap4_char_next( TT_CMap     cmap,
                      FT_UInt32  *pchar_code )
  {
    FT_UInt  gindex;


    if ( *pchar_code >= 0xFFFFU )
      return 0;

    if ( cmap->flags & TT_CMAP_FLAG_UNSORTED )
      gindex = tt_cmap4_char_map_linear( cmap, pchar_code, 1 );
    else
    {
      TT_CMap4  cmap4 = (TT_CMap4)cmap;


      /* no need to search */
      if ( *pchar_code == cmap4->cur_charcode )
      {
        tt_cmap4_next( cmap4 );
        gindex = cmap4->cur_gindex;
        if ( gindex )
          *pchar_code = cmap4->cur_charcode;
      }
      else
        gindex = tt_cmap4_char_map_binary( cmap, pchar_code, 1 );
    }

    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap4_get_info( TT_CMap       cmap,
                     TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 4;


    cmap_info->format   = 4;
    cmap_info->language = (FT_ULong)TT_PEEK_USHORT( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap4_class_rec,
    sizeof ( TT_CMap4Rec ),
    (FT_CMap_InitFunc)     tt_cmap4_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap4_char_index,
    (FT_CMap_CharNextFunc) tt_cmap4_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    4,
    (TT_CMap_ValidateFunc)tt_cmap4_validate,
    (TT_CMap_Info_GetFunc)tt_cmap4_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_4 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          FORMAT 6                             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME        OFFSET          TYPE             DESCRIPTION            */
  /*                                                                       */
  /*   format       0              USHORT           must be 4              */
  /*   length       2              USHORT           table length in bytes  */
  /*   language     4              USHORT           Mac language code      */
  /*                                                                       */
  /*   first        6              USHORT           first segment code     */
  /*   count        8              USHORT           segment size in chars  */
  /*   glyphIds     10             USHORT[count]    glyph IDs              */
  /*                                                                       */
  /* A very simplified segment mapping.                                    */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_6

  FT_CALLBACK_DEF( FT_Error )
  tt_cmap6_validate( FT_Byte*      table,
                     FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_UInt   length, count;


    if ( table + 10 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p      = table + 2;
    length = TT_NEXT_USHORT( p );

    p      = table + 8;             /* skip language and start index */
    count  = TT_NEXT_USHORT( p );

    if ( table + length > valid->limit || length < 10 + count * 2 )
      FT_INVALID_TOO_SHORT;

    /* check glyph indices */
    if ( valid->level >= FT_VALIDATE_TIGHT )
    {
      FT_UInt  gindex;


      for ( ; count > 0; count-- )
      {
        gindex = TT_NEXT_USHORT( p );
        if ( gindex >= TT_VALID_GLYPH_COUNT( valid ) )
          FT_INVALID_GLYPH_ID;
      }
    }

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap6_char_index( TT_CMap    cmap,
                       FT_UInt32  char_code )
  {
    FT_Byte*  table  = cmap->data;
    FT_UInt   result = 0;
    FT_Byte*  p      = table + 6;
    FT_UInt   start  = TT_NEXT_USHORT( p );
    FT_UInt   count  = TT_NEXT_USHORT( p );
    FT_UInt   idx    = (FT_UInt)( char_code - start );


    if ( idx < count )
    {
      p += 2 * idx;
      result = TT_PEEK_USHORT( p );
    }
    return result;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap6_char_next( TT_CMap     cmap,
                      FT_UInt32  *pchar_code )
  {
    FT_Byte*   table     = cmap->data;
    FT_UInt32  result    = 0;
    FT_UInt32  char_code = *pchar_code + 1;
    FT_UInt    gindex    = 0;

    FT_Byte*   p         = table + 6;
    FT_UInt    start     = TT_NEXT_USHORT( p );
    FT_UInt    count     = TT_NEXT_USHORT( p );
    FT_UInt    idx;


    if ( char_code >= 0x10000UL )
      goto Exit;

    if ( char_code < start )
      char_code = start;

    idx = (FT_UInt)( char_code - start );
    p  += 2 * idx;

    for ( ; idx < count; idx++ )
    {
      gindex = TT_NEXT_USHORT( p );
      if ( gindex != 0 )
      {
        result = char_code;
        break;
      }
      char_code++;
    }

  Exit:
    *pchar_code = result;
    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap6_get_info( TT_CMap       cmap,
                     TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 4;


    cmap_info->format   = 6;
    cmap_info->language = (FT_ULong)TT_PEEK_USHORT( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap6_class_rec,
    sizeof ( TT_CMapRec ),

    (FT_CMap_InitFunc)     tt_cmap_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap6_char_index,
    (FT_CMap_CharNextFunc) tt_cmap6_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    6,
    (TT_CMap_ValidateFunc)tt_cmap6_validate,
    (TT_CMap_Info_GetFunc)tt_cmap6_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_6 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          FORMAT 8                             *****/
  /*****                                                               *****/
  /***** It is hard to completely understand what the OpenType spec    *****/
  /***** says about this format, but here is my conclusion.            *****/
  /*****                                                               *****/
  /***** The purpose of this format is to easily map UTF-16 text to    *****/
  /***** glyph indices.  Basically, the `char_code' must be in one of  *****/
  /***** the following formats:                                        *****/
  /*****                                                               *****/
  /*****   - A 16-bit value that isn't part of the Unicode Surrogates  *****/
  /*****     Area (i.e. U+D800-U+DFFF).                                *****/
  /*****                                                               *****/
  /*****   - A 32-bit value, made of two surrogate values, i.e.. if    *****/
  /*****     `char_code = (char_hi << 16) | char_lo', then both        *****/
  /*****     `char_hi' and `char_lo' must be in the Surrogates Area.   *****/
  /*****      Area.                                                    *****/
  /*****                                                               *****/
  /***** The `is32' table embedded in the charmap indicates whether a  *****/
  /***** given 16-bit value is in the surrogates area or not.          *****/
  /*****                                                               *****/
  /***** So, for any given `char_code', we can assert the following:   *****/
  /*****                                                               *****/
  /*****   If `char_hi == 0' then we must have `is32[char_lo] == 0'.   *****/
  /*****                                                               *****/
  /*****   If `char_hi != 0' then we must have both                    *****/
  /*****   `is32[char_hi] != 0' and `is32[char_lo] != 0'.              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME        OFFSET         TYPE        DESCRIPTION                  */
  /*                                                                       */
  /*   format      0              USHORT      must be 8                    */
  /*   reserved    2              USHORT      reserved                     */
  /*   length      4              ULONG       length in bytes              */
  /*   language    8              ULONG       Mac language code            */
  /*   is32        12             BYTE[8192]  32-bitness bitmap            */
  /*   count       8204           ULONG       number of groups             */
  /*                                                                       */
  /* This header is followed by `count' groups of the following format:    */
  /*                                                                       */
  /*   start       0              ULONG       first charcode               */
  /*   end         4              ULONG       last charcode                */
  /*   startId     8              ULONG       start glyph ID for the group */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_8

  FT_CALLBACK_DEF( FT_Error )
  tt_cmap8_validate( FT_Byte*      table,
                     FT_Validator  valid )
  {
    FT_Byte*   p = table + 4;
    FT_Byte*   is32;
    FT_UInt32  length;
    FT_UInt32  num_groups;


    if ( table + 16 + 8192 > valid->limit )
      FT_INVALID_TOO_SHORT;

    length = TT_NEXT_ULONG( p );
    if ( length > (FT_UInt32)( valid->limit - table ) || length < 8192 + 16 )
      FT_INVALID_TOO_SHORT;

    is32       = table + 12;
    p          = is32  + 8192;          /* skip `is32' array */
    num_groups = TT_NEXT_ULONG( p );

    if ( p + num_groups * 12 > valid->limit )
      FT_INVALID_TOO_SHORT;

    /* check groups, they must be in increasing order */
    {
      FT_UInt32  n, start, end, start_id, count, last = 0;


      for ( n = 0; n < num_groups; n++ )
      {
        FT_UInt   hi, lo;


        start    = TT_NEXT_ULONG( p );
        end      = TT_NEXT_ULONG( p );
        start_id = TT_NEXT_ULONG( p );

        if ( start > end )
          FT_INVALID_DATA;

        if ( n > 0 && start <= last )
          FT_INVALID_DATA;

        if ( valid->level >= FT_VALIDATE_TIGHT )
        {
          if ( start_id + end - start >= TT_VALID_GLYPH_COUNT( valid ) )
            FT_INVALID_GLYPH_ID;

          count = (FT_UInt32)( end - start + 1 );

          if ( start & ~0xFFFFU )
          {
            /* start_hi != 0; check that is32[i] is 1 for each i in */
            /* the `hi' and `lo' of the range [start..end]          */
            for ( ; count > 0; count--, start++ )
            {
              hi = (FT_UInt)( start >> 16 );
              lo = (FT_UInt)( start & 0xFFFFU );

              if ( (is32[hi >> 3] & ( 0x80 >> ( hi & 7 ) ) ) == 0 )
                FT_INVALID_DATA;

              if ( (is32[lo >> 3] & ( 0x80 >> ( lo & 7 ) ) ) == 0 )
                FT_INVALID_DATA;
            }
          }
          else
          {
            /* start_hi == 0; check that is32[i] is 0 for each i in */
            /* the range [start..end]                               */

            /* end_hi cannot be != 0! */
            if ( end & ~0xFFFFU )
              FT_INVALID_DATA;

            for ( ; count > 0; count--, start++ )
            {
              lo = (FT_UInt)( start & 0xFFFFU );

              if ( (is32[lo >> 3] & ( 0x80 >> ( lo & 7 ) ) ) != 0 )
                FT_INVALID_DATA;
            }
          }
        }

        last = end;
      }
    }

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap8_char_index( TT_CMap    cmap,
                       FT_UInt32  char_code )
  {
    FT_Byte*   table      = cmap->data;
    FT_UInt    result     = 0;
    FT_Byte*   p          = table + 8204;
    FT_UInt32  num_groups = TT_NEXT_ULONG( p );
    FT_UInt32  start, end, start_id;


    for ( ; num_groups > 0; num_groups-- )
    {
      start    = TT_NEXT_ULONG( p );
      end      = TT_NEXT_ULONG( p );
      start_id = TT_NEXT_ULONG( p );

      if ( char_code < start )
        break;

      if ( char_code <= end )
      {
        result = (FT_UInt)( start_id + char_code - start );
        break;
      }
    }
    return result;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap8_char_next( TT_CMap     cmap,
                      FT_UInt32  *pchar_code )
  {
    FT_UInt32  result     = 0;
    FT_UInt32  char_code  = *pchar_code + 1;
    FT_UInt    gindex     = 0;
    FT_Byte*   table      = cmap->data;
    FT_Byte*   p          = table + 8204;
    FT_UInt32  num_groups = TT_NEXT_ULONG( p );
    FT_UInt32  start, end, start_id;


    p = table + 8208;

    for ( ; num_groups > 0; num_groups-- )
    {
      start    = TT_NEXT_ULONG( p );
      end      = TT_NEXT_ULONG( p );
      start_id = TT_NEXT_ULONG( p );

      if ( char_code < start )
        char_code = start;

      if ( char_code <= end )
      {
        gindex = (FT_UInt)( char_code - start + start_id );
        if ( gindex != 0 )
        {
          result = char_code;
          goto Exit;
        }
      }
    }

  Exit:
    *pchar_code = result;
    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap8_get_info( TT_CMap       cmap,
                     TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 8;


    cmap_info->format   = 8;
    cmap_info->language = (FT_ULong)TT_PEEK_ULONG( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap8_class_rec,
    sizeof ( TT_CMapRec ),

    (FT_CMap_InitFunc)     tt_cmap_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap8_char_index,
    (FT_CMap_CharNextFunc) tt_cmap8_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    8,
    (TT_CMap_ValidateFunc)tt_cmap8_validate,
    (TT_CMap_Info_GetFunc)tt_cmap8_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_8 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          FORMAT 10                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME      OFFSET  TYPE               DESCRIPTION                    */
  /*                                                                       */
  /*   format     0      USHORT             must be 10                     */
  /*   reserved   2      USHORT             reserved                       */
  /*   length     4      ULONG              length in bytes                */
  /*   language   8      ULONG              Mac language code              */
  /*                                                                       */
  /*   start     12      ULONG              first char in range            */
  /*   count     16      ULONG              number of chars in range       */
  /*   glyphIds  20      USHORT[count]      glyph indices covered          */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_10

  FT_CALLBACK_DEF( FT_Error )
  tt_cmap10_validate( FT_Byte*      table,
                      FT_Validator  valid )
  {
    FT_Byte*  p = table + 4;
    FT_ULong  length, count;


    if ( table + 20 > valid->limit )
      FT_INVALID_TOO_SHORT;

    length = TT_NEXT_ULONG( p );
    p      = table + 16;
    count  = TT_NEXT_ULONG( p );

    if ( length > (FT_ULong)( valid->limit - table ) ||
         length < 20 + count * 2                     )
      FT_INVALID_TOO_SHORT;

    /* check glyph indices */
    if ( valid->level >= FT_VALIDATE_TIGHT )
    {
      FT_UInt  gindex;


      for ( ; count > 0; count-- )
      {
        gindex = TT_NEXT_USHORT( p );
        if ( gindex >= TT_VALID_GLYPH_COUNT( valid ) )
          FT_INVALID_GLYPH_ID;
      }
    }

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap10_char_index( TT_CMap    cmap,
                        FT_UInt32  char_code )
  {
    FT_Byte*   table  = cmap->data;
    FT_UInt    result = 0;
    FT_Byte*   p      = table + 12;
    FT_UInt32  start  = TT_NEXT_ULONG( p );
    FT_UInt32  count  = TT_NEXT_ULONG( p );
    FT_UInt32  idx    = (FT_ULong)( char_code - start );


    if ( idx < count )
    {
      p     += 2 * idx;
      result = TT_PEEK_USHORT( p );
    }
    return result;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap10_char_next( TT_CMap     cmap,
                       FT_UInt32  *pchar_code )
  {
    FT_Byte*   table     = cmap->data;
    FT_UInt32  char_code = *pchar_code + 1;
    FT_UInt    gindex    = 0;
    FT_Byte*   p         = table + 12;
    FT_UInt32  start     = TT_NEXT_ULONG( p );
    FT_UInt32  count     = TT_NEXT_ULONG( p );
    FT_UInt32  idx;


    if ( char_code < start )
      char_code = start;

    idx = (FT_UInt32)( char_code - start );
    p  += 2 * idx;

    for ( ; idx < count; idx++ )
    {
      gindex = TT_NEXT_USHORT( p );
      if ( gindex != 0 )
        break;
      char_code++;
    }

    *pchar_code = char_code;
    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap10_get_info( TT_CMap       cmap,
                      TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 8;


    cmap_info->format   = 10;
    cmap_info->language = (FT_ULong)TT_PEEK_ULONG( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap10_class_rec,
    sizeof ( TT_CMapRec ),

    (FT_CMap_InitFunc)     tt_cmap_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap10_char_index,
    (FT_CMap_CharNextFunc) tt_cmap10_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    10,
    (TT_CMap_ValidateFunc)tt_cmap10_validate,
    (TT_CMap_Info_GetFunc)tt_cmap10_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_10 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          FORMAT 12                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME        OFFSET     TYPE       DESCRIPTION                       */
  /*                                                                       */
  /*   format      0          USHORT     must be 12                        */
  /*   reserved    2          USHORT     reserved                          */
  /*   length      4          ULONG      length in bytes                   */
  /*   language    8          ULONG      Mac language code                 */
  /*   count       12         ULONG      number of groups                  */
  /*               16                                                      */
  /*                                                                       */
  /* This header is followed by `count' groups of the following format:    */
  /*                                                                       */
  /*   start       0          ULONG      first charcode                    */
  /*   end         4          ULONG      last charcode                     */
  /*   startId     8          ULONG      start glyph ID for the group      */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_12

  typedef struct  TT_CMap12Rec_
  {
    TT_CMapRec  cmap;
    FT_Bool     valid;
    FT_ULong    cur_charcode;
    FT_UInt     cur_gindex;
    FT_ULong    cur_group;
    FT_ULong    num_groups;

  } TT_CMap12Rec, *TT_CMap12;


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap12_init( TT_CMap12  cmap,
                  FT_Byte*   table )
  {
    cmap->cmap.data  = table;

    table           += 12;
    cmap->num_groups = FT_PEEK_ULONG( table );

    cmap->valid      = 0;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap12_validate( FT_Byte*      table,
                      FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_ULong  length;
    FT_ULong  num_groups;


    if ( table + 16 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p      = table + 4;
    length = TT_NEXT_ULONG( p );

    p          = table + 12;
    num_groups = TT_NEXT_ULONG( p );

    if ( length > (FT_ULong)( valid->limit - table ) ||
         length < 16 + 12 * num_groups               )
      FT_INVALID_TOO_SHORT;

    /* check groups, they must be in increasing order */
    {
      FT_ULong  n, start, end, start_id, last = 0;


      for ( n = 0; n < num_groups; n++ )
      {
        start    = TT_NEXT_ULONG( p );
        end      = TT_NEXT_ULONG( p );
        start_id = TT_NEXT_ULONG( p );

        if ( start > end )
          FT_INVALID_DATA;

        if ( n > 0 && start <= last )
          FT_INVALID_DATA;

        if ( valid->level >= FT_VALIDATE_TIGHT )
        {
          if ( start_id + end - start >= TT_VALID_GLYPH_COUNT( valid ) )
            FT_INVALID_GLYPH_ID;
        }

        last = end;
      }
    }

    return FT_Err_Ok;
  }


  /* search the index of the charcode next to cmap->cur_charcode */
  /* cmap->cur_group should be set up properly by caller         */
  /*                                                             */
  static void
  tt_cmap12_next( TT_CMap12  cmap )
  {
    FT_Byte*  p;
    FT_ULong  start, end, start_id, char_code;
    FT_ULong  n;
    FT_UInt   gindex;


    if ( cmap->cur_charcode >= 0xFFFFFFFFUL )
      goto Fail;

    char_code = cmap->cur_charcode + 1;

    for ( n = cmap->cur_group; n < cmap->num_groups; n++ )
    {
      p        = cmap->cmap.data + 16 + 12 * n;
      start    = TT_NEXT_ULONG( p );
      end      = TT_NEXT_ULONG( p );
      start_id = TT_PEEK_ULONG( p );

      if ( char_code < start )
        char_code = start;

      for ( ; char_code <= end; char_code++ )
      {
        gindex = (FT_UInt)( start_id + char_code - start );

        if ( gindex )
        {
          cmap->cur_charcode = char_code;;
          cmap->cur_gindex   = gindex;
          cmap->cur_group    = n;

          return;
        }
      }
    }

  Fail:
    cmap->valid = 0;
  }


  static FT_UInt
  tt_cmap12_char_map_binary( TT_CMap     cmap,
                             FT_UInt32*  pchar_code,
                             FT_Bool     next )
  {
    FT_UInt    gindex     = 0;
    FT_Byte*   p          = cmap->data + 12;
    FT_UInt32  num_groups = TT_PEEK_ULONG( p );
    FT_UInt32  char_code  = *pchar_code;
    FT_UInt32  start, end, start_id;
    FT_UInt32  max, min, mid;


    if ( !num_groups )
      return 0;

    /* make compiler happy */
    mid = num_groups;
    end = 0xFFFFFFFFUL;

    if ( next )
      char_code++;

    min = 0;
    max = num_groups;

    /* binary search */
    while ( min < max )
    {
      mid = ( min + max ) >> 1;
      p   = cmap->data + 16 + 12 * mid;

      start = TT_NEXT_ULONG( p );
      end   = TT_NEXT_ULONG( p );

      if ( char_code < start )
        max = mid;
      else if ( char_code > end )
        min = mid + 1;
      else
      {
        start_id = TT_PEEK_ULONG( p );
        gindex = (FT_UInt)( start_id + char_code - start );

        break;
      }
    }

    if ( next )
    {
      TT_CMap12  cmap12 = (TT_CMap12)cmap;


      /* if `char_code' is not in any group, then `mid' is */
      /* the group nearest to `char_code'                  */
      /*                                                   */

      if ( char_code > end )
      {
        mid++;
        if ( mid == num_groups )
          return 0;
      }

      cmap12->valid        = 1;
      cmap12->cur_charcode = char_code;
      cmap12->cur_group    = mid;

      if ( !gindex )
      {
        tt_cmap12_next( cmap12 );

        if ( cmap12->valid )
          gindex = cmap12->cur_gindex;
      }
      else
        cmap12->cur_gindex = gindex;

      if ( gindex )
        *pchar_code = cmap12->cur_charcode;
    }

    return gindex;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap12_char_index( TT_CMap    cmap,
                        FT_UInt32  char_code )
  {
    return tt_cmap12_char_map_binary( cmap, &char_code, 0 );
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap12_char_next( TT_CMap     cmap,
                       FT_UInt32  *pchar_code )
  {
    TT_CMap12  cmap12 = (TT_CMap12)cmap;
    FT_ULong   gindex;


    if ( cmap12->cur_charcode >= 0xFFFFFFFFUL )
      return 0;

    /* no need to search */
    if ( cmap12->valid && cmap12->cur_charcode == *pchar_code )
    {
      tt_cmap12_next( cmap12 );
      if ( cmap12->valid )
      {
        gindex = cmap12->cur_gindex;

        /* XXX: check cur_charcode overflow is expected */
        if ( gindex )
          *pchar_code = (FT_UInt32)cmap12->cur_charcode;
      }
      else
        gindex = 0;
    }
    else
      gindex = tt_cmap12_char_map_binary( cmap, pchar_code, 1 );

    /* XXX: check gindex overflow is expected */
    return (FT_UInt32)gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap12_get_info( TT_CMap       cmap,
                      TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 8;


    cmap_info->format   = 12;
    cmap_info->language = (FT_ULong)TT_PEEK_ULONG( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap12_class_rec,
    sizeof ( TT_CMap12Rec ),

    (FT_CMap_InitFunc)     tt_cmap12_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap12_char_index,
    (FT_CMap_CharNextFunc) tt_cmap12_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    12,
    (TT_CMap_ValidateFunc)tt_cmap12_validate,
    (TT_CMap_Info_GetFunc)tt_cmap12_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_12 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          FORMAT 13                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME        OFFSET     TYPE       DESCRIPTION                       */
  /*                                                                       */
  /*   format      0          USHORT     must be 13                        */
  /*   reserved    2          USHORT     reserved                          */
  /*   length      4          ULONG      length in bytes                   */
  /*   language    8          ULONG      Mac language code                 */
  /*   count       12         ULONG      number of groups                  */
  /*               16                                                      */
  /*                                                                       */
  /* This header is followed by `count' groups of the following format:    */
  /*                                                                       */
  /*   start       0          ULONG      first charcode                    */
  /*   end         4          ULONG      last charcode                     */
  /*   glyphId     8          ULONG      glyph ID for the whole group      */
  /*                                                                       */

#ifdef TT_CONFIG_CMAP_FORMAT_13

  typedef struct  TT_CMap13Rec_
  {
    TT_CMapRec  cmap;
    FT_Bool     valid;
    FT_ULong    cur_charcode;
    FT_UInt     cur_gindex;
    FT_ULong    cur_group;
    FT_ULong    num_groups;

  } TT_CMap13Rec, *TT_CMap13;


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap13_init( TT_CMap13  cmap,
                  FT_Byte*   table )
  {
    cmap->cmap.data  = table;

    table           += 12;
    cmap->num_groups = FT_PEEK_ULONG( table );

    cmap->valid      = 0;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap13_validate( FT_Byte*      table,
                      FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_ULong  length;
    FT_ULong  num_groups;


    if ( table + 16 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p      = table + 4;
    length = TT_NEXT_ULONG( p );

    p          = table + 12;
    num_groups = TT_NEXT_ULONG( p );

    if ( length > (FT_ULong)( valid->limit - table ) ||
         length < 16 + 12 * num_groups               )
      FT_INVALID_TOO_SHORT;

    /* check groups, they must be in increasing order */
    {
      FT_ULong  n, start, end, glyph_id, last = 0;


      for ( n = 0; n < num_groups; n++ )
      {
        start    = TT_NEXT_ULONG( p );
        end      = TT_NEXT_ULONG( p );
        glyph_id = TT_NEXT_ULONG( p );

        if ( start > end )
          FT_INVALID_DATA;

        if ( n > 0 && start <= last )
          FT_INVALID_DATA;

        if ( valid->level >= FT_VALIDATE_TIGHT )
        {
          if ( glyph_id >= TT_VALID_GLYPH_COUNT( valid ) )
            FT_INVALID_GLYPH_ID;
        }

        last = end;
      }
    }

    return FT_Err_Ok;
  }


  /* search the index of the charcode next to cmap->cur_charcode */
  /* cmap->cur_group should be set up properly by caller         */
  /*                                                             */
  static void
  tt_cmap13_next( TT_CMap13  cmap )
  {
    FT_Byte*  p;
    FT_ULong  start, end, glyph_id, char_code;
    FT_ULong  n;
    FT_UInt   gindex;


    if ( cmap->cur_charcode >= 0xFFFFFFFFUL )
      goto Fail;

    char_code = cmap->cur_charcode + 1;

    for ( n = cmap->cur_group; n < cmap->num_groups; n++ )
    {
      p        = cmap->cmap.data + 16 + 12 * n;
      start    = TT_NEXT_ULONG( p );
      end      = TT_NEXT_ULONG( p );
      glyph_id = TT_PEEK_ULONG( p );

      if ( char_code < start )
        char_code = start;

      if ( char_code <= end )
      {
        gindex = (FT_UInt)glyph_id;

        if ( gindex )
        {
          cmap->cur_charcode = char_code;;
          cmap->cur_gindex   = gindex;
          cmap->cur_group    = n;

          return;
        }
      }
    }

  Fail:
    cmap->valid = 0;
  }


  static FT_UInt
  tt_cmap13_char_map_binary( TT_CMap     cmap,
                             FT_UInt32*  pchar_code,
                             FT_Bool     next )
  {
    FT_UInt    gindex     = 0;
    FT_Byte*   p          = cmap->data + 12;
    FT_UInt32  num_groups = TT_PEEK_ULONG( p );
    FT_UInt32  char_code  = *pchar_code;
    FT_UInt32  start, end;
    FT_UInt32  max, min, mid;


    if ( !num_groups )
      return 0;

    /* make compiler happy */
    mid = num_groups;
    end = 0xFFFFFFFFUL;

    if ( next )
      char_code++;

    min = 0;
    max = num_groups;

    /* binary search */
    while ( min < max )
    {
      mid = ( min + max ) >> 1;
      p   = cmap->data + 16 + 12 * mid;

      start = TT_NEXT_ULONG( p );
      end   = TT_NEXT_ULONG( p );

      if ( char_code < start )
        max = mid;
      else if ( char_code > end )
        min = mid + 1;
      else
      {
        gindex = (FT_UInt)TT_PEEK_ULONG( p );

        break;
      }
    }

    if ( next )
    {
      TT_CMap13  cmap13 = (TT_CMap13)cmap;


      /* if `char_code' is not in any group, then `mid' is */
      /* the group nearest to `char_code'                  */

      if ( char_code > end )
      {
        mid++;
        if ( mid == num_groups )
          return 0;
      }

      cmap13->valid        = 1;
      cmap13->cur_charcode = char_code;
      cmap13->cur_group    = mid;

      if ( !gindex )
      {
        tt_cmap13_next( cmap13 );

        if ( cmap13->valid )
          gindex = cmap13->cur_gindex;
      }
      else
        cmap13->cur_gindex = gindex;

      if ( gindex )
        *pchar_code = cmap13->cur_charcode;
    }

    return gindex;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap13_char_index( TT_CMap    cmap,
                        FT_UInt32  char_code )
  {
    return tt_cmap13_char_map_binary( cmap, &char_code, 0 );
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap13_char_next( TT_CMap     cmap,
                       FT_UInt32  *pchar_code )
  {
    TT_CMap13  cmap13 = (TT_CMap13)cmap;
    FT_UInt    gindex;


    if ( cmap13->cur_charcode >= 0xFFFFFFFFUL )
      return 0;

    /* no need to search */
    if ( cmap13->valid && cmap13->cur_charcode == *pchar_code )
    {
      tt_cmap13_next( cmap13 );
      if ( cmap13->valid )
      {
        gindex = cmap13->cur_gindex;
        if ( gindex )
          *pchar_code = cmap13->cur_charcode;
      }
      else
        gindex = 0;
    }
    else
      gindex = tt_cmap13_char_map_binary( cmap, pchar_code, 1 );

    return gindex;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap13_get_info( TT_CMap       cmap,
                      TT_CMapInfo  *cmap_info )
  {
    FT_Byte*  p = cmap->data + 8;


    cmap_info->format   = 13;
    cmap_info->language = (FT_ULong)TT_PEEK_ULONG( p );

    return FT_Err_Ok;
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap13_class_rec,
    sizeof ( TT_CMap13Rec ),

    (FT_CMap_InitFunc)     tt_cmap13_init,
    (FT_CMap_DoneFunc)     NULL,
    (FT_CMap_CharIndexFunc)tt_cmap13_char_index,
    (FT_CMap_CharNextFunc) tt_cmap13_char_next,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    13,
    (TT_CMap_ValidateFunc)tt_cmap13_validate,
    (TT_CMap_Info_GetFunc)tt_cmap13_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_13 */


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                           FORMAT 14                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* TABLE OVERVIEW                                                        */
  /* --------------                                                        */
  /*                                                                       */
  /*   NAME         OFFSET  TYPE    DESCRIPTION                            */
  /*                                                                       */
  /*   format         0     USHORT  must be 14                             */
  /*   length         2     ULONG   table length in bytes                  */
  /*   numSelector    6     ULONG   number of variation sel. records       */
  /*                                                                       */
  /* Followed by numSelector records, each of which looks like             */
  /*                                                                       */
  /*   varSelector    0     UINT24  Unicode codepoint of sel.              */
  /*   defaultOff     3     ULONG   offset to a default UVS table          */
  /*                                describing any variants to be found in */
  /*                                the normal Unicode subtable.           */
  /*   nonDefOff      7     ULONG   offset to a non-default UVS table      */
  /*                                describing any variants not in the     */
  /*                                standard cmap, with GIDs here          */
  /* (either offset may be 0 NULL)                                         */
  /*                                                                       */
  /* Selectors are sorted by code point.                                   */
  /*                                                                       */
  /* A default Unicode Variation Selector (UVS) subtable is just a list of */
  /* ranges of code points which are to be found in the standard cmap.  No */
  /* glyph IDs (GIDs) here.                                                */
  /*                                                                       */
  /*   numRanges      0     ULONG   number of ranges following             */
  /*                                                                       */
  /* A range looks like                                                    */
  /*                                                                       */
  /*   uniStart       0     UINT24  code point of the first character in   */
  /*                                this range                             */
  /*   additionalCnt  3     UBYTE   count of additional characters in this */
  /*                                range (zero means a range of a single  */
  /*                                character)                             */
  /*                                                                       */
  /* Ranges are sorted by `uniStart'.                                      */
  /*                                                                       */
  /* A non-default Unicode Variation Selector (UVS) subtable is a list of  */
  /* mappings from codepoint to GID.                                       */
  /*                                                                       */
  /*   numMappings    0     ULONG   number of mappings                     */
  /*                                                                       */
  /* A range looks like                                                    */
  /*                                                                       */
  /*   uniStart       0     UINT24  code point of the first character in   */
  /*                                this range                             */
  /*   GID            3     USHORT  and its GID                            */
  /*                                                                       */
  /* Ranges are sorted by `uniStart'.                                      */

#ifdef TT_CONFIG_CMAP_FORMAT_14

  typedef struct  TT_CMap14Rec_
  {
    TT_CMapRec  cmap;
    FT_ULong    num_selectors;

    /* This array is used to store the results of various
     * cmap 14 query functions.  The data is overwritten
     * on each call to these functions.
     */
    FT_UInt32   max_results;
    FT_UInt32*  results;
    FT_Memory   memory;

  } TT_CMap14Rec, *TT_CMap14;


  FT_CALLBACK_DEF( void )
  tt_cmap14_done( TT_CMap14  cmap )
  {
    FT_Memory  memory = cmap->memory;


    cmap->max_results = 0;
    if ( memory != NULL && cmap->results != NULL )
      FT_FREE( cmap->results );
  }


  static FT_Error
  tt_cmap14_ensure( TT_CMap14  cmap,
                    FT_UInt32  num_results,
                    FT_Memory  memory )
  {
    FT_UInt32  old_max = cmap->max_results;
    FT_Error   error   = FT_Err_Ok;


    if ( num_results > cmap->max_results )
    {
       cmap->memory = memory;

       if ( FT_QRENEW_ARRAY( cmap->results, old_max, num_results ) )
         return error;

       cmap->max_results = num_results;
    }

    return error;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap14_init( TT_CMap14  cmap,
                  FT_Byte*   table )
  {
    cmap->cmap.data = table;

    table               += 6;
    cmap->num_selectors  = FT_PEEK_ULONG( table );
    cmap->max_results    = 0;
    cmap->results        = NULL;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap14_validate( FT_Byte*      table,
                      FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_ULong  length;
    FT_ULong  num_selectors;


    if ( table + 2 + 4 + 4 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p             = table + 2;
    length        = TT_NEXT_ULONG( p );
    num_selectors = TT_NEXT_ULONG( p );

    if ( length > (FT_ULong)( valid->limit - table ) ||
         length < 10 + 11 * num_selectors            )
      FT_INVALID_TOO_SHORT;

    /* check selectors, they must be in increasing order */
    {
      /* we start lastVarSel at 1 because a variant selector value of 0
       * isn't valid.
       */
      FT_ULong  n, lastVarSel = 1;


      for ( n = 0; n < num_selectors; n++ )
      {
        FT_ULong  varSel    = TT_NEXT_UINT24( p );
        FT_ULong  defOff    = TT_NEXT_ULONG( p );
        FT_ULong  nondefOff = TT_NEXT_ULONG( p );


        if ( defOff >= length || nondefOff >= length )
          FT_INVALID_TOO_SHORT;

        if ( varSel < lastVarSel )
          FT_INVALID_DATA;

        lastVarSel = varSel + 1;

        /* check the default table (these glyphs should be reached     */
        /* through the normal Unicode cmap, no GIDs, just check order) */
        if ( defOff != 0 )
        {
          FT_Byte*  defp      = table + defOff;
          FT_ULong  numRanges = TT_NEXT_ULONG( defp );
          FT_ULong  i;
          FT_ULong  lastBase  = 0;


          if ( defp + numRanges * 4 > valid->limit )
            FT_INVALID_TOO_SHORT;

          for ( i = 0; i < numRanges; ++i )
          {
            FT_ULong  base = TT_NEXT_UINT24( defp );
            FT_ULong  cnt  = FT_NEXT_BYTE( defp );


            if ( base + cnt >= 0x110000UL )              /* end of Unicode */
              FT_INVALID_DATA;

            if ( base < lastBase )
              FT_INVALID_DATA;

            lastBase = base + cnt + 1U;
          }
        }

        /* and the non-default table (these glyphs are specified here) */
        if ( nondefOff != 0 )
        {
          FT_Byte*  ndp         = table + nondefOff;
          FT_ULong  numMappings = TT_NEXT_ULONG( ndp );
          FT_ULong  i, lastUni  = 0;


          if ( numMappings * 4 > (FT_ULong)( valid->limit - ndp ) )
            FT_INVALID_TOO_SHORT;

          for ( i = 0; i < numMappings; ++i )
          {
            FT_ULong  uni = TT_NEXT_UINT24( ndp );
            FT_ULong  gid = TT_NEXT_USHORT( ndp );


            if ( uni >= 0x110000UL )                     /* end of Unicode */
              FT_INVALID_DATA;

            if ( uni < lastUni )
              FT_INVALID_DATA;

            lastUni = uni + 1U;

            if ( valid->level >= FT_VALIDATE_TIGHT    &&
                 gid >= TT_VALID_GLYPH_COUNT( valid ) )
              FT_INVALID_GLYPH_ID;
          }
        }
      }
    }

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap14_char_index( TT_CMap    cmap,
                        FT_UInt32  char_code )
  {
    FT_UNUSED( cmap );
    FT_UNUSED( char_code );

    /* This can't happen */
    return 0;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  tt_cmap14_char_next( TT_CMap     cmap,
                       FT_UInt32  *pchar_code )
  {
    FT_UNUSED( cmap );

    /* This can't happen */
    *pchar_code = 0;
    return 0;
  }


  FT_CALLBACK_DEF( FT_Error )
  tt_cmap14_get_info( TT_CMap       cmap,
                      TT_CMapInfo  *cmap_info )
  {
    FT_UNUSED( cmap );

    cmap_info->format   = 14;
    /* subtable 14 does not define a language field */
    cmap_info->language = 0xFFFFFFFFUL;

    return FT_Err_Ok;
  }


  static FT_UInt
  tt_cmap14_char_map_def_binary( FT_Byte    *base,
                                 FT_UInt32   char_code )
  {
    FT_UInt32  numRanges = TT_PEEK_ULONG( base );
    FT_UInt32  max, min;


    min = 0;
    max = numRanges;

    base += 4;

    /* binary search */
    while ( min < max )
    {
      FT_UInt32  mid   = ( min + max ) >> 1;
      FT_Byte*   p     = base + 4 * mid;
      FT_ULong   start = TT_NEXT_UINT24( p );
      FT_UInt    cnt   = FT_NEXT_BYTE( p );


      if ( char_code < start )
        max = mid;
      else if ( char_code > start+cnt )
        min = mid + 1;
      else
        return TRUE;
    }

    return FALSE;
  }


  static FT_UInt
  tt_cmap14_char_map_nondef_binary( FT_Byte    *base,
                                    FT_UInt32   char_code )
  {
    FT_UInt32  numMappings = TT_PEEK_ULONG( base );
    FT_UInt32  max, min;


    min = 0;
    max = numMappings;

    base += 4;

    /* binary search */
    while ( min < max )
    {
      FT_UInt32  mid = ( min + max ) >> 1;
      FT_Byte*   p   = base + 5 * mid;
      FT_UInt32  uni = (FT_UInt32)TT_NEXT_UINT24( p );


      if ( char_code < uni )
        max = mid;
      else if ( char_code > uni )
        min = mid + 1;
      else
        return TT_PEEK_USHORT( p );
    }

    return 0;
  }


  static FT_Byte*
  tt_cmap14_find_variant( FT_Byte    *base,
                          FT_UInt32   variantCode )
  {
    FT_UInt32  numVar = TT_PEEK_ULONG( base );
    FT_UInt32  max, min;


    min = 0;
    max = numVar;

    base += 4;

    /* binary search */
    while ( min < max )
    {
      FT_UInt32  mid    = ( min + max ) >> 1;
      FT_Byte*   p      = base + 11 * mid;
      FT_ULong   varSel = TT_NEXT_UINT24( p );


      if ( variantCode < varSel )
        max = mid;
      else if ( variantCode > varSel )
        min = mid + 1;
      else
        return p;
    }

    return NULL;
  }


  FT_CALLBACK_DEF( FT_UInt )
  tt_cmap14_char_var_index( TT_CMap    cmap,
                            TT_CMap    ucmap,
                            FT_UInt32  charcode,
                            FT_UInt32  variantSelector )
  {
    FT_Byte*  p = tt_cmap14_find_variant( cmap->data + 6, variantSelector );
    FT_ULong  defOff;
    FT_ULong  nondefOff;


    if ( !p )
      return 0;

    defOff    = TT_NEXT_ULONG( p );
    nondefOff = TT_PEEK_ULONG( p );

    if ( defOff != 0                                                    &&
         tt_cmap14_char_map_def_binary( cmap->data + defOff, charcode ) )
    {
      /* This is the default variant of this charcode.  GID not stored */
      /* here; stored in the normal Unicode charmap instead.           */
      return ucmap->cmap.clazz->char_index( &ucmap->cmap, charcode );
    }

    if ( nondefOff != 0 )
      return tt_cmap14_char_map_nondef_binary( cmap->data + nondefOff,
                                               charcode );

    return 0;
  }


  FT_CALLBACK_DEF( FT_Int )
  tt_cmap14_char_var_isdefault( TT_CMap    cmap,
                                FT_UInt32  charcode,
                                FT_UInt32  variantSelector )
  {
    FT_Byte*  p = tt_cmap14_find_variant( cmap->data + 6, variantSelector );
    FT_ULong  defOff;
    FT_ULong  nondefOff;


    if ( !p )
      return -1;

    defOff    = TT_NEXT_ULONG( p );
    nondefOff = TT_NEXT_ULONG( p );

    if ( defOff != 0                                                    &&
         tt_cmap14_char_map_def_binary( cmap->data + defOff, charcode ) )
      return 1;

    if ( nondefOff != 0                                            &&
         tt_cmap14_char_map_nondef_binary( cmap->data + nondefOff,
                                           charcode ) != 0         )
      return 0;

    return -1;
  }


  FT_CALLBACK_DEF( FT_UInt32* )
  tt_cmap14_variants( TT_CMap    cmap,
                      FT_Memory  memory )
  {
    TT_CMap14   cmap14 = (TT_CMap14)cmap;
    FT_UInt32   count  = cmap14->num_selectors;
    FT_Byte*    p      = cmap->data + 10;
    FT_UInt32*  result;
    FT_UInt32   i;


    if ( tt_cmap14_ensure( cmap14, ( count + 1 ), memory ) )
      return NULL;

    result = cmap14->results;
    for ( i = 0; i < count; ++i )
    {
      result[i] = (FT_UInt32)TT_NEXT_UINT24( p );
      p        += 8;
    }
    result[i] = 0;

    return result;
  }


  FT_CALLBACK_DEF( FT_UInt32 * )
  tt_cmap14_char_variants( TT_CMap    cmap,
                           FT_Memory  memory,
                           FT_UInt32  charCode )
  {
    TT_CMap14   cmap14 = (TT_CMap14)  cmap;
    FT_UInt32   count  = cmap14->num_selectors;
    FT_Byte*    p      = cmap->data + 10;
    FT_UInt32*  q;


    if ( tt_cmap14_ensure( cmap14, ( count + 1 ), memory ) )
      return NULL;

    for ( q = cmap14->results; count > 0; --count )
    {
      FT_UInt32  varSel    = TT_NEXT_UINT24( p );
      FT_ULong   defOff    = TT_NEXT_ULONG( p );
      FT_ULong   nondefOff = TT_NEXT_ULONG( p );


      if ( ( defOff != 0                                               &&
             tt_cmap14_char_map_def_binary( cmap->data + defOff,
                                            charCode )                 ) ||
           ( nondefOff != 0                                            &&
             tt_cmap14_char_map_nondef_binary( cmap->data + nondefOff,
                                               charCode ) != 0         ) )
      {
        q[0] = varSel;
        q++;
      }
    }
    q[0] = 0;

    return cmap14->results;
  }


  static FT_UInt
  tt_cmap14_def_char_count( FT_Byte  *p )
  {
    FT_UInt32  numRanges = (FT_UInt32)TT_NEXT_ULONG( p );
    FT_UInt    tot       = 0;


    p += 3;  /* point to the first `cnt' field */
    for ( ; numRanges > 0; numRanges-- )
    {
      tot += 1 + p[0];
      p   += 4;
    }

    return tot;
  }


  static FT_UInt32*
  tt_cmap14_get_def_chars( TT_CMap    cmap,
                           FT_Byte*   p,
                           FT_Memory  memory )
  {
    TT_CMap14   cmap14 = (TT_CMap14) cmap;
    FT_UInt32   numRanges;
    FT_UInt     cnt;
    FT_UInt32*  q;


    cnt       = tt_cmap14_def_char_count( p );
    numRanges = (FT_UInt32)TT_NEXT_ULONG( p );

    if ( tt_cmap14_ensure( cmap14, ( cnt + 1 ), memory ) )
      return NULL;

    for ( q = cmap14->results; numRanges > 0; --numRanges )
    {
      FT_UInt32  uni = (FT_UInt32)TT_NEXT_UINT24( p );


      cnt = FT_NEXT_BYTE( p ) + 1;
      do
      {
        q[0]  = uni;
        uni  += 1;
        q    += 1;

      } while ( --cnt != 0 );
    }
    q[0] = 0;

    return cmap14->results;
  }


  static FT_UInt32*
  tt_cmap14_get_nondef_chars( TT_CMap     cmap,
                              FT_Byte    *p,
                              FT_Memory   memory )
  {
    TT_CMap14   cmap14 = (TT_CMap14) cmap;
    FT_UInt32   numMappings;
    FT_UInt     i;
    FT_UInt32  *ret;


    numMappings = (FT_UInt32)TT_NEXT_ULONG( p );

    if ( tt_cmap14_ensure( cmap14, ( numMappings + 1 ), memory ) )
      return NULL;

    ret = cmap14->results;
    for ( i = 0; i < numMappings; ++i )
    {
      ret[i] = (FT_UInt32)TT_NEXT_UINT24( p );
      p += 2;
    }
    ret[i] = 0;

    return ret;
  }


  FT_CALLBACK_DEF( FT_UInt32 * )
  tt_cmap14_variant_chars( TT_CMap    cmap,
                           FT_Memory  memory,
                           FT_UInt32  variantSelector )
  {
    FT_Byte    *p  = tt_cmap14_find_variant( cmap->data + 6,
                                             variantSelector );
    FT_Int      i;
    FT_ULong    defOff;
    FT_ULong    nondefOff;


    if ( !p )
      return NULL;

    defOff    = TT_NEXT_ULONG( p );
    nondefOff = TT_NEXT_ULONG( p );

    if ( defOff == 0 && nondefOff == 0 )
      return NULL;

    if ( defOff == 0 )
      return tt_cmap14_get_nondef_chars( cmap, cmap->data + nondefOff,
                                         memory );
    else if ( nondefOff == 0 )
      return tt_cmap14_get_def_chars( cmap, cmap->data + defOff,
                                      memory );
    else
    {
      /* Both a default and a non-default glyph set?  That's probably not */
      /* good font design, but the spec allows for it...                  */
      TT_CMap14  cmap14 = (TT_CMap14) cmap;
      FT_UInt32  numRanges;
      FT_UInt32  numMappings;
      FT_UInt32  duni;
      FT_UInt32  dcnt;
      FT_UInt32  nuni;
      FT_Byte*   dp;
      FT_UInt    di, ni, k;

      FT_UInt32  *ret;


      p  = cmap->data + nondefOff;
      dp = cmap->data + defOff;

      numMappings = (FT_UInt32)TT_NEXT_ULONG( p );
      dcnt        = tt_cmap14_def_char_count( dp );
      numRanges   = (FT_UInt32)TT_NEXT_ULONG( dp );

      if ( numMappings == 0 )
        return tt_cmap14_get_def_chars( cmap, cmap->data + defOff,
                                        memory );
      if ( dcnt == 0 )
        return tt_cmap14_get_nondef_chars( cmap, cmap->data + nondefOff,
                                           memory );

      if ( tt_cmap14_ensure( cmap14, ( dcnt + numMappings + 1 ), memory ) )
        return NULL;

      ret  = cmap14->results;
      duni = (FT_UInt32)TT_NEXT_UINT24( dp );
      dcnt = FT_NEXT_BYTE( dp );
      di   = 1;
      nuni = (FT_UInt32)TT_NEXT_UINT24( p );
      p   += 2;
      ni   = 1;
      i    = 0;

      for ( ;; )
      {
        if ( nuni > duni + dcnt )
        {
          for ( k = 0; k <= dcnt; ++k )
            ret[i++] = duni + k;

          ++di;

          if ( di > numRanges )
            break;

          duni = (FT_UInt32)TT_NEXT_UINT24( dp );
          dcnt = FT_NEXT_BYTE( dp );
        }
        else
        {
          if ( nuni < duni )
            ret[i++] = nuni;
          /* If it is within the default range then ignore it -- */
          /* that should not have happened                       */
          ++ni;
          if ( ni > numMappings )
            break;

          nuni = (FT_UInt32)TT_NEXT_UINT24( p );
          p += 2;
        }
      }

      if ( ni <= numMappings )
      {
        /* If we get here then we have run out of all default ranges.   */
        /* We have read one non-default mapping which we haven't stored */
        /* and there may be others that need to be read.                */
        ret[i++] = nuni;
        while ( ni < numMappings )
        {
          ret[i++] = (FT_UInt32)TT_NEXT_UINT24( p );
          p += 2;
          ++ni;
        }
      }
      else if ( di <= numRanges )
      {
        /* If we get here then we have run out of all non-default     */
        /* mappings.  We have read one default range which we haven't */
        /* stored and there may be others that need to be read.       */
        for ( k = 0; k <= dcnt; ++k )
          ret[i++] = duni + k;

        while ( di < numRanges )
        {
          duni = (FT_UInt32)TT_NEXT_UINT24( dp );
          dcnt = FT_NEXT_BYTE( dp );

          for ( k = 0; k <= dcnt; ++k )
            ret[i++] = duni + k;
          ++di;
        }
      }

      ret[i] = 0;

      return ret;
    }
  }


  FT_DEFINE_TT_CMAP(
    tt_cmap14_class_rec,
    sizeof ( TT_CMap14Rec ),

    (FT_CMap_InitFunc)     tt_cmap14_init,
    (FT_CMap_DoneFunc)     tt_cmap14_done,
    (FT_CMap_CharIndexFunc)tt_cmap14_char_index,
    (FT_CMap_CharNextFunc) tt_cmap14_char_next,

    /* Format 14 extension functions */
    (FT_CMap_CharVarIndexFunc)    tt_cmap14_char_var_index,
    (FT_CMap_CharVarIsDefaultFunc)tt_cmap14_char_var_isdefault,
    (FT_CMap_VariantListFunc)     tt_cmap14_variants,
    (FT_CMap_CharVariantListFunc) tt_cmap14_char_variants,
    (FT_CMap_VariantCharListFunc) tt_cmap14_variant_chars,

    14,
    (TT_CMap_ValidateFunc)tt_cmap14_validate,
    (TT_CMap_Info_GetFunc)tt_cmap14_get_info )

#endif /* TT_CONFIG_CMAP_FORMAT_14 */


#ifndef FT_CONFIG_OPTION_PIC

  static const TT_CMap_Class  tt_cmap_classes[] =
  {
#define TTCMAPCITEM( a )  &a,
#include "ttcmapc.h"
    NULL,
  };

#else /*FT_CONFIG_OPTION_PIC*/

  void
  FT_Destroy_Class_tt_cmap_classes( FT_Library      library,
                                    TT_CMap_Class*  clazz )
  {
    FT_Memory  memory = library->memory;


    if ( clazz )
      FT_FREE( clazz );
  }


  FT_Error
  FT_Create_Class_tt_cmap_classes( FT_Library       library,
                                   TT_CMap_Class**  output_class )
  {
    TT_CMap_Class*     clazz  = NULL;
    TT_CMap_ClassRec*  recs;
    FT_Error           error;
    FT_Memory          memory = library->memory;

    int  i = 0;


#define TTCMAPCITEM( a ) i++;
#include "ttcmapc.h"

    /* allocate enough space for both the pointers */
    /* plus terminator and the class instances     */
    if ( FT_ALLOC( clazz, sizeof ( *clazz ) * ( i + 1 ) +
                          sizeof ( TT_CMap_ClassRec ) * i ) )
      return error;

    /* the location of the class instances follows the array of pointers */
    recs = (TT_CMap_ClassRec*)( (char*)clazz +
                                sizeof ( *clazz ) * ( i + 1 ) );
    i    = 0;

#undef TTCMAPCITEM
#define  TTCMAPCITEM( a )             \
    FT_Init_Class_ ## a( &recs[i] );  \
    clazz[i] = &recs[i];              \
    i++;
#include "ttcmapc.h"

    clazz[i] = NULL;

    *output_class = clazz;
    return FT_Err_Ok;
  }

#endif /*FT_CONFIG_OPTION_PIC*/


  /* parse the `cmap' table and build the corresponding TT_CMap objects */
  /* in the current face                                                */
  /*                                                                    */
  FT_LOCAL_DEF( FT_Error )
  tt_face_build_cmaps( TT_Face  face )
  {
    FT_Byte*           table = face->cmap_table;
    FT_Byte*           limit = table + face->cmap_size;
    FT_UInt volatile   num_cmaps;
    FT_Byte* volatile  p     = table;
    FT_Library         library = FT_FACE_LIBRARY( face );

    FT_UNUSED( library );


    if ( !p || p + 4 > limit )
      return FT_THROW( Invalid_Table );

    /* only recognize format 0 */
    if ( TT_NEXT_USHORT( p ) != 0 )
    {
      p -= 2;
      FT_ERROR(( "tt_face_build_cmaps:"
                 " unsupported `cmap' table format = %d\n",
                 TT_PEEK_USHORT( p ) ));
      return FT_THROW( Invalid_Table );
    }

    num_cmaps = TT_NEXT_USHORT( p );

#ifdef FT_MAX_CHARMAP_CACHEABLE
    if ( num_cmaps > FT_MAX_CHARMAP_CACHEABLE )
      FT_ERROR(( "tt_face_build_cmaps: too many cmap subtables (%d)\n"
                 "                     subtable #%d and higher are loaded"
                 "                     but cannot be searched\n",
                 num_cmaps, FT_MAX_CHARMAP_CACHEABLE + 1 ));
#endif

    for ( ; num_cmaps > 0 && p + 8 <= limit; num_cmaps-- )
    {
      FT_CharMapRec  charmap;
      FT_UInt32      offset;


      charmap.platform_id = TT_NEXT_USHORT( p );
      charmap.encoding_id = TT_NEXT_USHORT( p );
      charmap.face        = FT_FACE( face );
      charmap.encoding    = FT_ENCODING_NONE;  /* will be filled later */
      offset              = TT_NEXT_ULONG( p );

      if ( offset && offset <= face->cmap_size - 2 )
      {
        FT_Byte* volatile              cmap   = table + offset;
        volatile FT_UInt               format = TT_PEEK_USHORT( cmap );
        const TT_CMap_Class* volatile  pclazz = TT_CMAP_CLASSES_GET;
        TT_CMap_Class volatile         clazz;


        for ( ; *pclazz; pclazz++ )
        {
          clazz = *pclazz;
          if ( clazz->format == format )
          {
            volatile TT_ValidatorRec  valid;
            volatile FT_Error         error = FT_Err_Ok;


            ft_validator_init( FT_VALIDATOR( &valid ), cmap, limit,
                               FT_VALIDATE_DEFAULT );

            valid.num_glyphs = (FT_UInt)face->max_profile.numGlyphs;

            if ( ft_setjmp( FT_VALIDATOR( &valid )->jump_buffer) == 0 )
            {
              /* validate this cmap sub-table */
              error = clazz->validate( cmap, FT_VALIDATOR( &valid ) );
            }

            if ( valid.validator.error == 0 )
            {
              FT_CMap  ttcmap;


              /* It might make sense to store the single variation         */
              /* selector cmap somewhere special.  But it would have to be */
              /* in the public FT_FaceRec, and we can't change that.       */

              if ( !FT_CMap_New( (FT_CMap_Class)clazz,
                                 cmap, &charmap, &ttcmap ) )
              {
                /* it is simpler to directly set `flags' than adding */
                /* a parameter to FT_CMap_New                        */
                ((TT_CMap)ttcmap)->flags = (FT_Int)error;
              }
            }
            else
            {
              FT_TRACE0(( "tt_face_build_cmaps:"
                          " broken cmap sub-table ignored\n" ));
            }
            break;
          }
        }

        if ( *pclazz == NULL )
        {
          FT_TRACE0(( "tt_face_build_cmaps:"
                      " unsupported cmap sub-table ignored\n" ));
        }
      }
    }

    return FT_Err_Ok;
  }


  FT_LOCAL( FT_Error )
  tt_get_cmap_info( FT_CharMap    charmap,
                    TT_CMapInfo  *cmap_info )
  {
    FT_CMap        cmap  = (FT_CMap)charmap;
    TT_CMap_Class  clazz = (TT_CMap_Class)cmap->clazz;


    return clazz->get_cmap_info( charmap, cmap_info );
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttkern.c                                                               */
/*                                                                         */
/*    Load the basic TrueType kerning table.  This doesn't handle          */
/*    kerning data within the GPOS table at the moment.                    */
/*                                                                         */
/*  Copyright 1996-2007, 2009, 2010, 2013 by                               */
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
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include "ttkern.h"

#include "sferrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttkern


#undef  TT_KERN_INDEX
#define TT_KERN_INDEX( g1, g2 )  ( ( (FT_ULong)(g1) << 16 ) | (g2) )


  FT_LOCAL_DEF( FT_Error )
  tt_face_load_kern( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error   error;
    FT_ULong   table_size;
    FT_Byte*   p;
    FT_Byte*   p_limit;
    FT_UInt    nn, num_tables;
    FT_UInt32  avail = 0, ordered = 0;


    /* the kern table is optional; exit silently if it is missing */
    error = face->goto_table( face, TTAG_kern, stream, &table_size );
    if ( error )
      goto Exit;

    if ( table_size < 4 )  /* the case of a malformed table */
    {
      FT_ERROR(( "tt_face_load_kern:"
                 " kerning table is too small - ignored\n" ));
      error = FT_THROW( Table_Missing );
      goto Exit;
    }

    if ( FT_FRAME_EXTRACT( table_size, face->kern_table ) )
    {
      FT_ERROR(( "tt_face_load_kern:"
                 " could not extract kerning table\n" ));
      goto Exit;
    }

    face->kern_table_size = table_size;

    p       = face->kern_table;
    p_limit = p + table_size;

    p         += 2; /* skip version */
    num_tables = FT_NEXT_USHORT( p );

    if ( num_tables > 32 ) /* we only support up to 32 sub-tables */
      num_tables = 32;

    for ( nn = 0; nn < num_tables; nn++ )
    {
      FT_UInt    num_pairs, length, coverage;
      FT_Byte*   p_next;
      FT_UInt32  mask = (FT_UInt32)1UL << nn;


      if ( p + 6 > p_limit )
        break;

      p_next = p;

      p += 2; /* skip version */
      length   = FT_NEXT_USHORT( p );
      coverage = FT_NEXT_USHORT( p );

      if ( length <= 6 )
        break;

      p_next += length;

      if ( p_next > p_limit )  /* handle broken table */
        p_next = p_limit;

      /* only use horizontal kerning tables */
      if ( ( coverage & ~8 ) != 0x0001 ||
           p + 8 > p_limit             )
        goto NextTable;

      num_pairs = FT_NEXT_USHORT( p );
      p        += 6;

      if ( ( p_next - p ) < 6 * (int)num_pairs ) /* handle broken count */
        num_pairs = (FT_UInt)( ( p_next - p ) / 6 );

      avail |= mask;

      /*
       *  Now check whether the pairs in this table are ordered.
       *  We then can use binary search.
       */
      if ( num_pairs > 0 )
      {
        FT_ULong  count;
        FT_ULong  old_pair;


        old_pair = FT_NEXT_ULONG( p );
        p       += 2;

        for ( count = num_pairs - 1; count > 0; count-- )
        {
          FT_UInt32  cur_pair;


          cur_pair = FT_NEXT_ULONG( p );
          if ( cur_pair <= old_pair )
            break;

          p += 2;
          old_pair = cur_pair;
        }

        if ( count == 0 )
          ordered |= mask;
      }

    NextTable:
      p = p_next;
    }

    face->num_kern_tables = nn;
    face->kern_avail_bits = avail;
    face->kern_order_bits = ordered;

  Exit:
    return error;
  }


  FT_LOCAL_DEF( void )
  tt_face_done_kern( TT_Face  face )
  {
    FT_Stream  stream = face->root.stream;


    FT_FRAME_RELEASE( face->kern_table );
    face->kern_table_size = 0;
    face->num_kern_tables = 0;
    face->kern_avail_bits = 0;
    face->kern_order_bits = 0;
  }


  FT_LOCAL_DEF( FT_Int )
  tt_face_get_kerning( TT_Face  face,
                       FT_UInt  left_glyph,
                       FT_UInt  right_glyph )
  {
    FT_Int    result = 0;
    FT_UInt   count, mask;
    FT_Byte*  p       = face->kern_table;
    FT_Byte*  p_limit = p + face->kern_table_size;


    p   += 4;
    mask = 0x0001;

    for ( count = face->num_kern_tables;
          count > 0 && p + 6 <= p_limit;
          count--, mask <<= 1 )
    {
      FT_Byte* base     = p;
      FT_Byte* next;
      FT_UInt  version  = FT_NEXT_USHORT( p );
      FT_UInt  length   = FT_NEXT_USHORT( p );
      FT_UInt  coverage = FT_NEXT_USHORT( p );
      FT_UInt  num_pairs;
      FT_Int   value    = 0;

      FT_UNUSED( version );


      next = base + length;

      if ( next > p_limit )  /* handle broken table */
        next = p_limit;

      if ( ( face->kern_avail_bits & mask ) == 0 )
        goto NextTable;

      if ( p + 8 > next )
        goto NextTable;

      num_pairs = FT_NEXT_USHORT( p );
      p        += 6;

      if ( ( next - p ) < 6 * (int)num_pairs )  /* handle broken count  */
        num_pairs = (FT_UInt)( ( next - p ) / 6 );

      switch ( coverage >> 8 )
      {
      case 0:
        {
          FT_ULong  key0 = TT_KERN_INDEX( left_glyph, right_glyph );


          if ( face->kern_order_bits & mask )   /* binary search */
          {
            FT_UInt   min = 0;
            FT_UInt   max = num_pairs;


            while ( min < max )
            {
              FT_UInt   mid = ( min + max ) >> 1;
              FT_Byte*  q   = p + 6 * mid;
              FT_ULong  key;


              key = FT_NEXT_ULONG( q );

              if ( key == key0 )
              {
                value = FT_PEEK_SHORT( q );
                goto Found;
              }
              if ( key < key0 )
                min = mid + 1;
              else
                max = mid;
            }
          }
          else /* linear search */
          {
            FT_UInt  count2;


            for ( count2 = num_pairs; count2 > 0; count2-- )
            {
              FT_ULong  key = FT_NEXT_ULONG( p );


              if ( key == key0 )
              {
                value = FT_PEEK_SHORT( p );
                goto Found;
              }
              p += 2;
            }
          }
        }
        break;

       /*
        *  We don't support format 2 because we haven't seen a single font
        *  using it in real life...
        */

      default:
        ;
      }

      goto NextTable;

    Found:
      if ( coverage & 8 ) /* override or add */
        result = value;
      else
        result += value;

    NextTable:
      p = next;
    }

    return result;
  }

#undef TT_KERN_INDEX

/* END */
/***************************************************************************/
/*                                                                         */
/*  sfobjs.c                                                               */
/*                                                                         */
/*    SFNT object management (base).                                       */
/*                                                                         */
/*  Copyright 1996-2008, 2010-2013 by                                      */
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
#include "sfobjs.h"
#include "ttload.h"
#include "ttcmap.h"
#include "ttkern.h"
#include FT_INTERNAL_SFNT_H
#include FT_INTERNAL_DEBUG_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TAGS_H
#include FT_SERVICE_POSTSCRIPT_CMAPS_H
#include FT_SFNT_NAMES_H
#include FT_GZIP_H
#include "sferrors.h"

#ifdef TT_CONFIG_OPTION_BDF
#include "ttbdf.h"
#endif


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_sfobjs



  /* convert a UTF-16 name entry to ASCII */
  static FT_String*
  tt_name_entry_ascii_from_utf16( TT_NameEntry  entry,
                                  FT_Memory     memory )
  {
    FT_String*  string = NULL;
    FT_UInt     len, code, n;
    FT_Byte*    read   = (FT_Byte*)entry->string;
    FT_Error    error;


    len = (FT_UInt)entry->stringLength / 2;

    if ( FT_NEW_ARRAY( string, len + 1 ) )
      return NULL;

    for ( n = 0; n < len; n++ )
    {
      code = FT_NEXT_USHORT( read );

      if ( code == 0 )
        break;

      if ( code < 32 || code > 127 )
        code = '?';

      string[n] = (char)code;
    }

    string[n] = 0;

    return string;
  }


  /* convert an Apple Roman or symbol name entry to ASCII */
  static FT_String*
  tt_name_entry_ascii_from_other( TT_NameEntry  entry,
                                  FT_Memory     memory )
  {
    FT_String*  string = NULL;
    FT_UInt     len, code, n;
    FT_Byte*    read   = (FT_Byte*)entry->string;
    FT_Error    error;


    len = (FT_UInt)entry->stringLength;

    if ( FT_NEW_ARRAY( string, len + 1 ) )
      return NULL;

    for ( n = 0; n < len; n++ )
    {
      code = *read++;

      if ( code == 0 )
        break;

      if ( code < 32 || code > 127 )
        code = '?';

      string[n] = (char)code;
    }

    string[n] = 0;

    return string;
  }


  typedef FT_String*  (*TT_NameEntry_ConvertFunc)( TT_NameEntry  entry,
                                                   FT_Memory     memory );


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_get_name                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Returns a given ENGLISH name record in ASCII.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the source face object.                      */
  /*                                                                       */
  /*    nameid :: The name id of the name record to return.                */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    name   :: The address of a string pointer.  NULL if no name is     */
  /*              present.                                                 */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  static FT_Error
  tt_face_get_name( TT_Face      face,
                    FT_UShort    nameid,
                    FT_String**  name )
  {
    FT_Memory         memory = face->root.memory;
    FT_Error          error  = FT_Err_Ok;
    FT_String*        result = NULL;
    FT_UShort         n;
    TT_NameEntryRec*  rec;
    FT_Int            found_apple         = -1;
    FT_Int            found_apple_roman   = -1;
    FT_Int            found_apple_english = -1;
    FT_Int            found_win           = -1;
    FT_Int            found_unicode       = -1;

    FT_Bool           is_english = 0;

    TT_NameEntry_ConvertFunc  convert;


    FT_ASSERT( name );

    rec = face->name_table.names;
    for ( n = 0; n < face->num_names; n++, rec++ )
    {
      /* According to the OpenType 1.3 specification, only Microsoft or  */
      /* Apple platform IDs might be used in the `name' table.  The      */
      /* `Unicode' platform is reserved for the `cmap' table, and the    */
      /* `ISO' one is deprecated.                                        */
      /*                                                                 */
      /* However, the Apple TrueType specification doesn't say the same  */
      /* thing and goes to suggest that all Unicode `name' table entries */
      /* should be coded in UTF-16 (in big-endian format I suppose).     */
      /*                                                                 */
      if ( rec->nameID == nameid && rec->stringLength > 0 )
      {
        switch ( rec->platformID )
        {
        case TT_PLATFORM_APPLE_UNICODE:
        case TT_PLATFORM_ISO:
          /* there is `languageID' to check there.  We should use this */
          /* field only as a last solution when nothing else is        */
          /* available.                                                */
          /*                                                           */
          found_unicode = n;
          break;

        case TT_PLATFORM_MACINTOSH:
          /* This is a bit special because some fonts will use either    */
          /* an English language id, or a Roman encoding id, to indicate */
          /* the English version of its font name.                       */
          /*                                                             */
          if ( rec->languageID == TT_MAC_LANGID_ENGLISH )
            found_apple_english = n;
          else if ( rec->encodingID == TT_MAC_ID_ROMAN )
            found_apple_roman = n;
          break;

        case TT_PLATFORM_MICROSOFT:
          /* we only take a non-English name when there is nothing */
          /* else available in the font                            */
          /*                                                       */
          if ( found_win == -1 || ( rec->languageID & 0x3FF ) == 0x009 )
          {
            switch ( rec->encodingID )
            {
            case TT_MS_ID_SYMBOL_CS:
            case TT_MS_ID_UNICODE_CS:
            case TT_MS_ID_UCS_4:
              is_english = FT_BOOL( ( rec->languageID & 0x3FF ) == 0x009 );
              found_win  = n;
              break;

            default:
              ;
            }
          }
          break;

        default:
          ;
        }
      }
    }

    found_apple = found_apple_roman;
    if ( found_apple_english >= 0 )
      found_apple = found_apple_english;

    /* some fonts contain invalid Unicode or Macintosh formatted entries; */
    /* we will thus favor names encoded in Windows formats if available   */
    /* (provided it is an English name)                                   */
    /*                                                                    */
    convert = NULL;
    if ( found_win >= 0 && !( found_apple >= 0 && !is_english ) )
    {
      rec = face->name_table.names + found_win;
      switch ( rec->encodingID )
      {
        /* all Unicode strings are encoded using UTF-16BE */
      case TT_MS_ID_UNICODE_CS:
      case TT_MS_ID_SYMBOL_CS:
        convert = tt_name_entry_ascii_from_utf16;
        break;

      case TT_MS_ID_UCS_4:
        /* Apparently, if this value is found in a name table entry, it is */
        /* documented as `full Unicode repertoire'.  Experience with the   */
        /* MsGothic font shipped with Windows Vista shows that this really */
        /* means UTF-16 encoded names (UCS-4 values are only used within   */
        /* charmaps).                                                      */
        convert = tt_name_entry_ascii_from_utf16;
        break;

      default:
        ;
      }
    }
    else if ( found_apple >= 0 )
    {
      rec     = face->name_table.names + found_apple;
      convert = tt_name_entry_ascii_from_other;
    }
    else if ( found_unicode >= 0 )
    {
      rec     = face->name_table.names + found_unicode;
      convert = tt_name_entry_ascii_from_utf16;
    }

    if ( rec && convert )
    {
      if ( rec->string == NULL )
      {
        FT_Stream  stream = face->name_table.stream;


        if ( FT_QNEW_ARRAY ( rec->string, rec->stringLength ) ||
             FT_STREAM_SEEK( rec->stringOffset )              ||
             FT_STREAM_READ( rec->string, rec->stringLength ) )
        {
          FT_FREE( rec->string );
          rec->stringLength = 0;
          result            = NULL;
          goto Exit;
        }
      }

      result = convert( rec, memory );
    }

  Exit:
    *name = result;
    return error;
  }


  static FT_Encoding
  sfnt_find_encoding( int  platform_id,
                      int  encoding_id )
  {
    typedef struct  TEncoding_
    {
      int          platform_id;
      int          encoding_id;
      FT_Encoding  encoding;

    } TEncoding;

    static
    const TEncoding  tt_encodings[] =
    {
      { TT_PLATFORM_ISO,           -1,                  FT_ENCODING_UNICODE },

      { TT_PLATFORM_APPLE_UNICODE, -1,                  FT_ENCODING_UNICODE },

      { TT_PLATFORM_MACINTOSH,     TT_MAC_ID_ROMAN,     FT_ENCODING_APPLE_ROMAN },

      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_SYMBOL_CS,  FT_ENCODING_MS_SYMBOL },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_UCS_4,      FT_ENCODING_UNICODE },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_UNICODE_CS, FT_ENCODING_UNICODE },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_SJIS,       FT_ENCODING_SJIS },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_GB2312,     FT_ENCODING_GB2312 },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_BIG_5,      FT_ENCODING_BIG5 },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_WANSUNG,    FT_ENCODING_WANSUNG },
      { TT_PLATFORM_MICROSOFT,     TT_MS_ID_JOHAB,      FT_ENCODING_JOHAB }
    };

    const TEncoding  *cur, *limit;


    cur   = tt_encodings;
    limit = cur + sizeof ( tt_encodings ) / sizeof ( tt_encodings[0] );

    for ( ; cur < limit; cur++ )
    {
      if ( cur->platform_id == platform_id )
      {
        if ( cur->encoding_id == encoding_id ||
             cur->encoding_id == -1          )
          return cur->encoding;
      }
    }

    return FT_ENCODING_NONE;
  }


#define WRITE_BYTE( p, v )     \
          do                   \
          {                    \
            *(p)++ = (v) >> 0; \
                               \
          } while ( 0 )

#define WRITE_USHORT( p, v )   \
          do                   \
          {                    \
            *(p)++ = (v) >> 8; \
            *(p)++ = (v) >> 0; \
                               \
          } while ( 0 )

#define WRITE_ULONG( p, v )     \
          do                    \
          {                     \
            *(p)++ = (FT_Byte)((v) >> 24); \
            *(p)++ = (FT_Byte)((v) >> 16); \
            *(p)++ = (FT_Byte)((v) >>  8); \
            *(p)++ = (FT_Byte)((v) >>  0); \
                                \
          } while ( 0 )


  static void
  sfnt_stream_close( FT_Stream  stream )
  {
    FT_Memory  memory = stream->memory;


    FT_FREE( stream->base );

    stream->size  = 0;
    stream->base  = 0;
    stream->close = 0;
  }


  FT_CALLBACK_DEF( int )
  compare_offsets( const void*  a,
                   const void*  b )
  {
    WOFF_Table  table1 = *(WOFF_Table*)a;
    WOFF_Table  table2 = *(WOFF_Table*)b;

    FT_ULong  offset1 = table1->Offset;
    FT_ULong  offset2 = table2->Offset;


    if ( offset1 > offset2 )
      return 1;
    else if ( offset1 < offset2 )
      return -1;
    else
      return 0;
  }


  /* Replace `face->root.stream' with a stream containing the extracted */
  /* SFNT of a WOFF font.                                               */

  static FT_Error
  woff_open_font( FT_Stream  stream,
                  TT_Face    face )
  {
    FT_Memory       memory = stream->memory;
    FT_Error        error  = FT_Err_Ok;

    WOFF_HeaderRec  woff;
    WOFF_Table      tables  = NULL;
    WOFF_Table*     indices = NULL;

    FT_ULong        woff_offset;

    FT_Byte*        sfnt        = NULL;
    FT_Stream       sfnt_stream = NULL;

    FT_Byte*        sfnt_header;
    FT_ULong        sfnt_offset;

    FT_Int          nn;
    FT_ULong        old_tag = 0;

    static const FT_Frame_Field  woff_header_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  WOFF_HeaderRec

      FT_FRAME_START( 44 ),
        FT_FRAME_ULONG ( signature ),
        FT_FRAME_ULONG ( flavor ),
        FT_FRAME_ULONG ( length ),
        FT_FRAME_USHORT( num_tables ),
        FT_FRAME_USHORT( reserved ),
        FT_FRAME_ULONG ( totalSfntSize ),
        FT_FRAME_USHORT( majorVersion ),
        FT_FRAME_USHORT( minorVersion ),
        FT_FRAME_ULONG ( metaOffset ),
        FT_FRAME_ULONG ( metaLength ),
        FT_FRAME_ULONG ( metaOrigLength ),
        FT_FRAME_ULONG ( privOffset ),
        FT_FRAME_ULONG ( privLength ),
      FT_FRAME_END
    };


    FT_ASSERT( stream == face->root.stream );
    FT_ASSERT( FT_STREAM_POS() == 0 );

    if ( FT_STREAM_READ_FIELDS( woff_header_fields, &woff ) )
      return error;

    /* Make sure we don't recurse back here or hit TTC code. */
    if ( woff.flavor == TTAG_wOFF || woff.flavor == TTAG_ttcf )
      return FT_THROW( Invalid_Table );

    /* Miscellaneous checks. */
    if ( woff.length != stream->size                              ||
         woff.num_tables == 0                                     ||
         44 + woff.num_tables * 20UL >= woff.length               ||
         12 + woff.num_tables * 16UL >= woff.totalSfntSize        ||
         ( woff.totalSfntSize & 3 ) != 0                          ||
         ( woff.metaOffset == 0 && ( woff.metaLength != 0     ||
                                     woff.metaOrigLength != 0 ) ) ||
         ( woff.metaLength != 0 && woff.metaOrigLength == 0 )     ||
         ( woff.privOffset == 0 && woff.privLength != 0 )         )
      return FT_THROW( Invalid_Table );

    if ( FT_ALLOC( sfnt, woff.totalSfntSize ) ||
         FT_NEW( sfnt_stream )                )
      goto Exit;

    sfnt_header = sfnt;

    /* Write sfnt header. */
    {
      FT_UInt  searchRange, entrySelector, rangeShift, x;


      x             = woff.num_tables;
      entrySelector = 0;
      while ( x )
      {
        x            >>= 1;
        entrySelector += 1;
      }
      entrySelector--;

      searchRange = ( 1 << entrySelector ) * 16;
      rangeShift  = woff.num_tables * 16 - searchRange;

      WRITE_ULONG ( sfnt_header, woff.flavor );
      WRITE_USHORT( sfnt_header, woff.num_tables );
      WRITE_USHORT( sfnt_header, searchRange );
      WRITE_USHORT( sfnt_header, entrySelector );
      WRITE_USHORT( sfnt_header, rangeShift );
    }

    /* While the entries in the sfnt header must be sorted by the */
    /* tag value, the tables themselves are not.  We thus have to */
    /* sort them by offset and check that they don't overlap.     */

    if ( FT_NEW_ARRAY( tables, woff.num_tables )  ||
         FT_NEW_ARRAY( indices, woff.num_tables ) )
      goto Exit;

    FT_TRACE2(( "\n"
                "  tag    offset    compLen  origLen  checksum\n"
                "  -------------------------------------------\n" ));

    if ( FT_FRAME_ENTER( 20L * woff.num_tables ) )
      goto Exit;

    for ( nn = 0; nn < woff.num_tables; nn++ )
    {
      WOFF_Table  table = tables + nn;

      table->Tag        = FT_GET_TAG4();
      table->Offset     = FT_GET_ULONG();
      table->CompLength = FT_GET_ULONG();
      table->OrigLength = FT_GET_ULONG();
      table->CheckSum   = FT_GET_ULONG();

      FT_TRACE2(( "  %c%c%c%c  %08lx  %08lx  %08lx  %08lx\n",
                  (FT_Char)( table->Tag >> 24 ),
                  (FT_Char)( table->Tag >> 16 ),
                  (FT_Char)( table->Tag >> 8  ),
                  (FT_Char)( table->Tag       ),
                  table->Offset,
                  table->CompLength,
                  table->OrigLength,
                  table->CheckSum ));

      if ( table->Tag <= old_tag )
      {
        FT_FRAME_EXIT();
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      old_tag     = table->Tag;
      indices[nn] = table;
    }

    FT_FRAME_EXIT();

    /* Sort by offset. */

    ft_qsort( indices,
              woff.num_tables,
              sizeof ( WOFF_Table ),
              compare_offsets );

    /* Check offsets and lengths. */

    woff_offset = 44 + woff.num_tables * 20L;
    sfnt_offset = 12 + woff.num_tables * 16L;

    for ( nn = 0; nn < woff.num_tables; nn++ )
    {
      WOFF_Table  table = indices[nn];


      if ( table->Offset != woff_offset                         ||
           table->Offset + table->CompLength > woff.length      ||
           sfnt_offset + table->OrigLength > woff.totalSfntSize ||
           table->CompLength > table->OrigLength                )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      table->OrigOffset = sfnt_offset;

      /* The offsets must be multiples of 4. */
      woff_offset += ( table->CompLength + 3 ) & ~3;
      sfnt_offset += ( table->OrigLength + 3 ) & ~3;
    }

    /*
     * Final checks!
     *
     * We don't decode and check the metadata block.
     * We don't check table checksums either.
     * But other than those, I think we implement all
     * `MUST' checks from the spec.
     */

    if ( woff.metaOffset )
    {
      if ( woff.metaOffset != woff_offset                  ||
           woff.metaOffset + woff.metaLength > woff.length )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      /* We have padding only ... */
      woff_offset += woff.metaLength;
    }

    if ( woff.privOffset )
    {
      /* ... if it isn't the last block. */
      woff_offset = ( woff_offset + 3 ) & ~3;

      if ( woff.privOffset != woff_offset                  ||
           woff.privOffset + woff.privLength > woff.length )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      /* No padding for the last block. */
      woff_offset += woff.privLength;
    }

    if ( sfnt_offset != woff.totalSfntSize ||
         woff_offset != woff.length        )
    {
      error = FT_THROW( Invalid_Table );
      goto Exit;
    }

    /* Write the tables. */

    for ( nn = 0; nn < woff.num_tables; nn++ )
    {
      WOFF_Table  table = tables + nn;


      /* Write SFNT table entry. */
      WRITE_ULONG( sfnt_header, table->Tag );
      WRITE_ULONG( sfnt_header, table->CheckSum );
      WRITE_ULONG( sfnt_header, table->OrigOffset );
      WRITE_ULONG( sfnt_header, table->OrigLength );

      /* Write table data. */
      if ( FT_STREAM_SEEK( table->Offset )     ||
           FT_FRAME_ENTER( table->CompLength ) )
        goto Exit;

      if ( table->CompLength == table->OrigLength )
      {
        /* Uncompressed data; just copy. */
        ft_memcpy( sfnt + table->OrigOffset,
                   stream->cursor,
                   table->OrigLength );
      }
      else
      {
        /* Uncompress with zlib. */
        FT_ULong  output_len = table->OrigLength;


        error = FT_Gzip_Uncompress( memory,
                                    sfnt + table->OrigOffset, &output_len,
                                    stream->cursor, table->CompLength );
        if ( error )
          goto Exit;
        if ( output_len != table->OrigLength )
        {
          error = FT_THROW( Invalid_Table );
          goto Exit;
        }
      }

      FT_FRAME_EXIT();

      /* We don't check whether the padding bytes in the WOFF file are     */
      /* actually '\0'.  For the output, however, we do set them properly. */
      sfnt_offset = table->OrigOffset + table->OrigLength;
      while ( sfnt_offset & 3 )
      {
        sfnt[sfnt_offset] = '\0';
        sfnt_offset++;
      }
    }

    /* Ok!  Finally ready.  Swap out stream and return. */
    FT_Stream_OpenMemory( sfnt_stream, sfnt, woff.totalSfntSize );
    sfnt_stream->memory = stream->memory;
    sfnt_stream->close  = sfnt_stream_close;

    FT_Stream_Free(
      face->root.stream,
      ( face->root.face_flags & FT_FACE_FLAG_EXTERNAL_STREAM ) != 0 );

    face->root.stream = sfnt_stream;

    face->root.face_flags &= ~FT_FACE_FLAG_EXTERNAL_STREAM;

  Exit:
    FT_FREE( tables );
    FT_FREE( indices );

    if ( error )
    {
      FT_FREE( sfnt );
      FT_Stream_Close( sfnt_stream );
      FT_FREE( sfnt_stream );
    }

    return error;
  }


#undef WRITE_BYTE
#undef WRITE_USHORT
#undef WRITE_ULONG


  /* Fill in face->ttc_header.  If the font is not a TTC, it is */
  /* synthesized into a TTC with one offset table.              */
  static FT_Error
  sfnt_open_font( FT_Stream  stream,
                  TT_Face    face )
  {
    FT_Memory  memory = stream->memory;
    FT_Error   error;
    FT_ULong   tag, offset;

    static const FT_Frame_Field  ttc_header_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TTC_HeaderRec

      FT_FRAME_START( 8 ),
        FT_FRAME_LONG( version ),
        FT_FRAME_LONG( count   ),  /* this is ULong in the specs */
      FT_FRAME_END
    };


    face->ttc_header.tag     = 0;
    face->ttc_header.version = 0;
    face->ttc_header.count   = 0;

  retry:
    offset = FT_STREAM_POS();

    if ( FT_READ_ULONG( tag ) )
      return error;

    if ( tag == TTAG_wOFF )
    {
      FT_TRACE2(( "sfnt_open_font: file is a WOFF; synthesizing SFNT\n" ));

      if ( FT_STREAM_SEEK( offset ) )
        return error;

      error = woff_open_font( stream, face );
      if ( error )
        return error;

      /* Swap out stream and retry! */
      stream = face->root.stream;
      goto retry;
    }

    if ( tag != 0x00010000UL &&
         tag != TTAG_ttcf    &&
         tag != TTAG_OTTO    &&
         tag != TTAG_true    &&
         tag != TTAG_typ1    &&
         tag != 0x00020000UL )
    {
      FT_TRACE2(( "  not a font using the SFNT container format\n" ));
      return FT_THROW( Unknown_File_Format );
    }

    face->ttc_header.tag = TTAG_ttcf;

    if ( tag == TTAG_ttcf )
    {
      FT_Int  n;


      FT_TRACE3(( "sfnt_open_font: file is a collection\n" ));

      if ( FT_STREAM_READ_FIELDS( ttc_header_fields, &face->ttc_header ) )
        return error;

      if ( face->ttc_header.count == 0 )
        return FT_THROW( Invalid_Table );

      /* a rough size estimate: let's conservatively assume that there   */
      /* is just a single table info in each subfont header (12 + 16*1 = */
      /* 28 bytes), thus we have (at least) `12 + 4*count' bytes for the */
      /* size of the TTC header plus `28*count' bytes for all subfont    */
      /* headers                                                         */
      if ( (FT_ULong)face->ttc_header.count > stream->size / ( 28 + 4 ) )
        return FT_THROW( Array_Too_Large );

      /* now read the offsets of each font in the file */
      if ( FT_NEW_ARRAY( face->ttc_header.offsets, face->ttc_header.count ) )
        return error;

      if ( FT_FRAME_ENTER( face->ttc_header.count * 4L ) )
        return error;

      for ( n = 0; n < face->ttc_header.count; n++ )
        face->ttc_header.offsets[n] = FT_GET_ULONG();

      FT_FRAME_EXIT();
    }
    else
    {
      FT_TRACE3(( "sfnt_open_font: synthesize TTC\n" ));

      face->ttc_header.version = 1 << 16;
      face->ttc_header.count   = 1;

      if ( FT_NEW( face->ttc_header.offsets ) )
        return error;

      face->ttc_header.offsets[0] = offset;
    }

    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  sfnt_init_face( FT_Stream      stream,
                  TT_Face        face,
                  FT_Int         face_index,
                  FT_Int         num_params,
                  FT_Parameter*  params )
  {
    FT_Error        error;
    FT_Library      library = face->root.driver->root.library;
    SFNT_Service    sfnt;


    /* for now, parameters are unused */
    FT_UNUSED( num_params );
    FT_UNUSED( params );


    sfnt = (SFNT_Service)face->sfnt;
    if ( !sfnt )
    {
      sfnt = (SFNT_Service)FT_Get_Module_Interface( library, "sfnt" );
      if ( !sfnt )
      {
        FT_ERROR(( "sfnt_init_face: cannot access `sfnt' module\n" ));
        return FT_THROW( Missing_Module );
      }

      face->sfnt       = sfnt;
      face->goto_table = sfnt->goto_table;
    }

    FT_FACE_FIND_GLOBAL_SERVICE( face, face->psnames, POSTSCRIPT_CMAPS );

    FT_TRACE2(( "SFNT driver\n" ));

    error = sfnt_open_font( stream, face );
    if ( error )
      return error;

    /* Stream may have changed in sfnt_open_font. */
    stream = face->root.stream;

    FT_TRACE2(( "sfnt_init_face: %08p, %ld\n", face, face_index ));

    if ( face_index < 0 )
      face_index = 0;

    if ( face_index >= face->ttc_header.count )
      return FT_THROW( Invalid_Argument );

    if ( FT_STREAM_SEEK( face->ttc_header.offsets[face_index] ) )
      return error;

    /* check that we have a valid TrueType file */
    error = sfnt->load_font_dir( face, stream );
    if ( error )
      return error;

    face->root.num_faces  = face->ttc_header.count;
    face->root.face_index = face_index;

    return error;
  }


#define LOAD_( x )                                          \
  do                                                        \
  {                                                         \
    FT_TRACE2(( "`" #x "' " ));                             \
    FT_TRACE3(( "-->\n" ));                                 \
                                                            \
    error = sfnt->load_ ## x( face, stream );               \
                                                            \
    FT_TRACE2(( "%s\n", ( !error )                          \
                        ? "loaded"                          \
                        : FT_ERR_EQ( error, Table_Missing ) \
                          ? "missing"                       \
                          : "failed to load" ));            \
    FT_TRACE3(( "\n" ));                                    \
  } while ( 0 )

#define LOADM_( x, vertical )                               \
  do                                                        \
  {                                                         \
    FT_TRACE2(( "`%s" #x "' ",                              \
                vertical ? "vertical " : "" ));             \
    FT_TRACE3(( "-->\n" ));                                 \
                                                            \
    error = sfnt->load_ ## x( face, stream, vertical );     \
                                                            \
    FT_TRACE2(( "%s\n", ( !error )                          \
                        ? "loaded"                          \
                        : FT_ERR_EQ( error, Table_Missing ) \
                          ? "missing"                       \
                          : "failed to load" ));            \
    FT_TRACE3(( "\n" ));                                    \
  } while ( 0 )

#define GET_NAME( id, field )                                   \
  do                                                            \
  {                                                             \
    error = tt_face_get_name( face, TT_NAME_ID_ ## id, field ); \
    if ( error )                                                \
      goto Exit;                                                \
  } while ( 0 )


  FT_LOCAL_DEF( FT_Error )
  sfnt_load_face( FT_Stream      stream,
                  TT_Face        face,
                  FT_Int         face_index,
                  FT_Int         num_params,
                  FT_Parameter*  params )
  {
    FT_Error      error;
#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
    FT_Error      psnames_error;
#endif
    FT_Bool       has_outline;
    FT_Bool       is_apple_sbit;
    FT_Bool       is_apple_sbix;
    FT_Bool       ignore_preferred_family    = FALSE;
    FT_Bool       ignore_preferred_subfamily = FALSE;

    SFNT_Service  sfnt = (SFNT_Service)face->sfnt;

    FT_UNUSED( face_index );


    /* Check parameters */

    {
      FT_Int  i;


      for ( i = 0; i < num_params; i++ )
      {
        if ( params[i].tag == FT_PARAM_TAG_IGNORE_PREFERRED_FAMILY )
          ignore_preferred_family = TRUE;
        else if ( params[i].tag == FT_PARAM_TAG_IGNORE_PREFERRED_SUBFAMILY )
          ignore_preferred_subfamily = TRUE;
      }
    }

    /* Load tables */

    /* We now support two SFNT-based bitmapped font formats.  They */
    /* are recognized easily as they do not include a `glyf'       */
    /* table.                                                      */
    /*                                                             */
    /* The first format comes from Apple, and uses a table named   */
    /* `bhed' instead of `head' to store the font header (using    */
    /* the same format).  It also doesn't include horizontal and   */
    /* vertical metrics tables (i.e. `hhea' and `vhea' tables are  */
    /* missing).                                                   */
    /*                                                             */
    /* The other format comes from Microsoft, and is used with     */
    /* WinCE/PocketPC.  It looks like a standard TTF, except that  */
    /* it doesn't contain outlines.                                */
    /*                                                             */

    FT_TRACE2(( "sfnt_load_face: %08p\n\n", face ));

    /* do we have outlines in there? */
#ifdef FT_CONFIG_OPTION_INCREMENTAL
    has_outline = FT_BOOL( face->root.internal->incremental_interface != 0 ||
                           tt_face_lookup_table( face, TTAG_glyf )    != 0 ||
                           tt_face_lookup_table( face, TTAG_CFF )     != 0 );
#else
    has_outline = FT_BOOL( tt_face_lookup_table( face, TTAG_glyf ) != 0 ||
                           tt_face_lookup_table( face, TTAG_CFF )  != 0 );
#endif

    is_apple_sbit = 0;
    is_apple_sbix = !face->goto_table( face, TTAG_sbix, stream, 0 );

    /* Apple 'sbix' color bitmaps are rendered scaled and then the 'glyf'
     * outline rendered on top.  We don't support that yet, so just ignore
     * the 'glyf' outline and advertise it as a bitmap-only font. */
    if ( is_apple_sbix )
      has_outline = FALSE;


    /* if this font doesn't contain outlines, we try to load */
    /* a `bhed' table                                        */
    if ( !has_outline && sfnt->load_bhed )
    {
      LOAD_( bhed );
      is_apple_sbit = FT_BOOL( !error );
    }

    /* load the font header (`head' table) if this isn't an Apple */
    /* sbit font file                                             */
    if ( !is_apple_sbit || is_apple_sbix )
    {
      LOAD_( head );
      if ( error )
        goto Exit;
    }

    if ( face->header.Units_Per_EM == 0 )
    {
      error = FT_THROW( Invalid_Table );

      goto Exit;
    }

    /* the following tables are often not present in embedded TrueType */
    /* fonts within PDF documents, so don't check for them.            */
    LOAD_( maxp );
    LOAD_( cmap );

    /* the following tables are optional in PCL fonts -- */
    /* don't check for errors                            */
    LOAD_( name );
    LOAD_( post );

#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
    psnames_error = error;
#endif

    /* do not load the metrics headers and tables if this is an Apple */
    /* sbit font file                                                 */
    if ( !is_apple_sbit )
    {
      /* load the `hhea' and `hmtx' tables */
      LOADM_( hhea, 0 );
      if ( !error )
      {
        LOADM_( hmtx, 0 );
        if ( FT_ERR_EQ( error, Table_Missing ) )
        {
          error = FT_THROW( Hmtx_Table_Missing );

#ifdef FT_CONFIG_OPTION_INCREMENTAL
          /* If this is an incrementally loaded font and there are */
          /* overriding metrics, tolerate a missing `hmtx' table.  */
          if ( face->root.internal->incremental_interface          &&
               face->root.internal->incremental_interface->funcs->
                 get_glyph_metrics                                 )
          {
            face->horizontal.number_Of_HMetrics = 0;
            error                               = FT_Err_Ok;
          }
#endif
        }
      }
      else if ( FT_ERR_EQ( error, Table_Missing ) )
      {
        /* No `hhea' table necessary for SFNT Mac fonts. */
        if ( face->format_tag == TTAG_true )
        {
          FT_TRACE2(( "This is an SFNT Mac font.\n" ));

          has_outline = 0;
          error       = FT_Err_Ok;
        }
        else
        {
          error = FT_THROW( Horiz_Header_Missing );

#ifdef FT_CONFIG_OPTION_INCREMENTAL
          /* If this is an incrementally loaded font and there are */
          /* overriding metrics, tolerate a missing `hhea' table.  */
          if ( face->root.internal->incremental_interface          &&
               face->root.internal->incremental_interface->funcs->
                 get_glyph_metrics                                 )
          {
            face->horizontal.number_Of_HMetrics = 0;
            error                               = FT_Err_Ok;
          }
#endif

        }
      }

      if ( error )
        goto Exit;

      /* try to load the `vhea' and `vmtx' tables */
      LOADM_( hhea, 1 );
      if ( !error )
      {
        LOADM_( hmtx, 1 );
        if ( !error )
          face->vertical_info = 1;
      }

      if ( error && FT_ERR_NEQ( error, Table_Missing ) )
        goto Exit;

      LOAD_( os2 );
      if ( error )
      {
        /* we treat the table as missing if there are any errors */
        face->os2.version = 0xFFFFU;
      }
    }

    /* the optional tables */

    /* embedded bitmap support */
    if ( sfnt->load_eblc )
    {
      LOAD_( eblc );
      if ( error )
      {
        /* a font which contains neither bitmaps nor outlines is */
        /* still valid (although rather useless in most cases);  */
        /* however, you can find such stripped fonts in PDFs     */
        if ( FT_ERR_EQ( error, Table_Missing ) )
          error = FT_Err_Ok;
        else
          goto Exit;
      }
    }

    LOAD_( pclt );
    if ( error )
    {
      if ( FT_ERR_NEQ( error, Table_Missing ) )
        goto Exit;

      face->pclt.Version = 0;
    }

    /* consider the kerning and gasp tables as optional */
    LOAD_( gasp );
    LOAD_( kern );

    face->root.num_glyphs = face->max_profile.numGlyphs;

    /* Bit 8 of the `fsSelection' field in the `OS/2' table denotes  */
    /* a WWS-only font face.  `WWS' stands for `weight', width', and */
    /* `slope', a term used by Microsoft's Windows Presentation      */
    /* Foundation (WPF).  This flag has been introduced in version   */
    /* 1.5 of the OpenType specification (May 2008).                 */

    face->root.family_name = NULL;
    face->root.style_name  = NULL;
    if ( face->os2.version != 0xFFFFU && face->os2.fsSelection & 256 )
    {
      if ( !ignore_preferred_family )
        GET_NAME( PREFERRED_FAMILY, &face->root.family_name );
      if ( !face->root.family_name )
        GET_NAME( FONT_FAMILY, &face->root.family_name );

      if ( !ignore_preferred_subfamily )
        GET_NAME( PREFERRED_SUBFAMILY, &face->root.style_name );
      if ( !face->root.style_name )
        GET_NAME( FONT_SUBFAMILY, &face->root.style_name );
    }
    else
    {
      GET_NAME( WWS_FAMILY, &face->root.family_name );
      if ( !face->root.family_name && !ignore_preferred_family )
        GET_NAME( PREFERRED_FAMILY, &face->root.family_name );
      if ( !face->root.family_name )
        GET_NAME( FONT_FAMILY, &face->root.family_name );

      GET_NAME( WWS_SUBFAMILY, &face->root.style_name );
      if ( !face->root.style_name && !ignore_preferred_subfamily )
        GET_NAME( PREFERRED_SUBFAMILY, &face->root.style_name );
      if ( !face->root.style_name )
        GET_NAME( FONT_SUBFAMILY, &face->root.style_name );
    }

    /* now set up root fields */
    {
      FT_Face  root  = &face->root;
      FT_Long  flags = root->face_flags;


      /*********************************************************************/
      /*                                                                   */
      /* Compute face flags.                                               */
      /*                                                                   */
      if ( face->sbit_table_type == TT_SBIT_TABLE_TYPE_CBLC ||
           face->sbit_table_type == TT_SBIT_TABLE_TYPE_SBIX )
        flags |= FT_FACE_FLAG_COLOR;      /* color glyphs */

      if ( has_outline == TRUE )
        flags |= FT_FACE_FLAG_SCALABLE;   /* scalable outlines */

      /* The sfnt driver only supports bitmap fonts natively, thus we */
      /* don't set FT_FACE_FLAG_HINTER.                               */
      flags |= FT_FACE_FLAG_SFNT       |  /* SFNT file format  */
               FT_FACE_FLAG_HORIZONTAL;   /* horizontal data   */

#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
      if ( !psnames_error                             &&
           face->postscript.FormatType != 0x00030000L )
        flags |= FT_FACE_FLAG_GLYPH_NAMES;
#endif

      /* fixed width font? */
      if ( face->postscript.isFixedPitch )
        flags |= FT_FACE_FLAG_FIXED_WIDTH;

      /* vertical information? */
      if ( face->vertical_info )
        flags |= FT_FACE_FLAG_VERTICAL;

      /* kerning available ? */
      if ( TT_FACE_HAS_KERNING( face ) )
        flags |= FT_FACE_FLAG_KERNING;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
      /* Don't bother to load the tables unless somebody asks for them. */
      /* No need to do work which will (probably) not be used.          */
      if ( tt_face_lookup_table( face, TTAG_glyf ) != 0 &&
           tt_face_lookup_table( face, TTAG_fvar ) != 0 &&
           tt_face_lookup_table( face, TTAG_gvar ) != 0 )
        flags |= FT_FACE_FLAG_MULTIPLE_MASTERS;
#endif

      root->face_flags = flags;

      /*********************************************************************/
      /*                                                                   */
      /* Compute style flags.                                              */
      /*                                                                   */

      flags = 0;
      if ( has_outline == TRUE && face->os2.version != 0xFFFFU )
      {
        /* We have an OS/2 table; use the `fsSelection' field.  Bit 9 */
        /* indicates an oblique font face.  This flag has been        */
        /* introduced in version 1.5 of the OpenType specification.   */

        if ( face->os2.fsSelection & 512 )       /* bit 9 */
          flags |= FT_STYLE_FLAG_ITALIC;
        else if ( face->os2.fsSelection & 1 )    /* bit 0 */
          flags |= FT_STYLE_FLAG_ITALIC;

        if ( face->os2.fsSelection & 32 )        /* bit 5 */
          flags |= FT_STYLE_FLAG_BOLD;
      }
      else
      {
        /* this is an old Mac font, use the header field */

        if ( face->header.Mac_Style & 1 )
          flags |= FT_STYLE_FLAG_BOLD;

        if ( face->header.Mac_Style & 2 )
          flags |= FT_STYLE_FLAG_ITALIC;
      }

      root->style_flags = flags;

      /*********************************************************************/
      /*                                                                   */
      /* Polish the charmaps.                                              */
      /*                                                                   */
      /*   Try to set the charmap encoding according to the platform &     */
      /*   encoding ID of each charmap.                                    */
      /*                                                                   */

      tt_face_build_cmaps( face );  /* ignore errors */


      /* set the encoding fields */
      {
        FT_Int  m;


        for ( m = 0; m < root->num_charmaps; m++ )
        {
          FT_CharMap  charmap = root->charmaps[m];


          charmap->encoding = sfnt_find_encoding( charmap->platform_id,
                                                  charmap->encoding_id );

#if 0
          if ( root->charmap     == NULL &&
               charmap->encoding == FT_ENCODING_UNICODE )
          {
            /* set 'root->charmap' to the first Unicode encoding we find */
            root->charmap = charmap;
          }
#endif
        }
      }

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

      /*
       *  Now allocate the root array of FT_Bitmap_Size records and
       *  populate them.  Unfortunately, it isn't possible to indicate bit
       *  depths in the FT_Bitmap_Size record.  This is a design error.
       */
      {
        FT_UInt  i, count;


        count = face->sbit_num_strikes;

        if ( count > 0 )
        {
          FT_Memory        memory   = face->root.stream->memory;
          FT_UShort        em_size  = face->header.Units_Per_EM;
          FT_Short         avgwidth = face->os2.xAvgCharWidth;
          FT_Size_Metrics  metrics;


          if ( em_size == 0 || face->os2.version == 0xFFFFU )
          {
            avgwidth = 1;
            em_size = 1;
          }

          if ( FT_NEW_ARRAY( root->available_sizes, count ) )
            goto Exit;

          for ( i = 0; i < count; i++ )
          {
            FT_Bitmap_Size*  bsize = root->available_sizes + i;


            error = sfnt->load_strike_metrics( face, i, &metrics );
            if ( error )
              goto Exit;

            bsize->height = (FT_Short)( metrics.height >> 6 );
            bsize->width = (FT_Short)(
                ( avgwidth * metrics.x_ppem + em_size / 2 ) / em_size );

            bsize->x_ppem = metrics.x_ppem << 6;
            bsize->y_ppem = metrics.y_ppem << 6;

            /* assume 72dpi */
            bsize->size   = metrics.y_ppem << 6;
          }

          root->face_flags     |= FT_FACE_FLAG_FIXED_SIZES;
          root->num_fixed_sizes = (FT_Int)count;
        }
      }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */

      /* a font with no bitmaps and no outlines is scalable; */
      /* it has only empty glyphs then                       */
      if ( !FT_HAS_FIXED_SIZES( root ) && !FT_IS_SCALABLE( root ) )
        root->face_flags |= FT_FACE_FLAG_SCALABLE;


      /*********************************************************************/
      /*                                                                   */
      /*  Set up metrics.                                                  */
      /*                                                                   */
      if ( FT_IS_SCALABLE( root ) )
      {
        /* XXX What about if outline header is missing */
        /*     (e.g. sfnt wrapped bitmap)?             */
        root->bbox.xMin    = face->header.xMin;
        root->bbox.yMin    = face->header.yMin;
        root->bbox.xMax    = face->header.xMax;
        root->bbox.yMax    = face->header.yMax;
        root->units_per_EM = face->header.Units_Per_EM;


        /* XXX: Computing the ascender/descender/height is very different */
        /*      from what the specification tells you.  Apparently, we    */
        /*      must be careful because                                   */
        /*                                                                */
        /*      - not all fonts have an OS/2 table; in this case, we take */
        /*        the values in the horizontal header.  However, these    */
        /*        values very often are not reliable.                     */
        /*                                                                */
        /*      - otherwise, the correct typographic values are in the    */
        /*        sTypoAscender, sTypoDescender & sTypoLineGap fields.    */
        /*                                                                */
        /*        However, certain fonts have these fields set to 0.      */
        /*        Rather, they have usWinAscent & usWinDescent correctly  */
        /*        set (but with different values).                        */
        /*                                                                */
        /*      As an example, Arial Narrow is implemented through four   */
        /*      files ARIALN.TTF, ARIALNI.TTF, ARIALNB.TTF & ARIALNBI.TTF */
        /*                                                                */
        /*      Strangely, all fonts have the same values in their        */
        /*      sTypoXXX fields, except ARIALNB which sets them to 0.     */
        /*                                                                */
        /*      On the other hand, they all have different                */
        /*      usWinAscent/Descent values -- as a conclusion, the OS/2   */
        /*      table cannot be used to compute the text height reliably! */
        /*                                                                */

        /* The ascender and descender are taken from the `hhea' table. */
        /* If zero, they are taken from the `OS/2' table.              */

        root->ascender  = face->horizontal.Ascender;
        root->descender = face->horizontal.Descender;

        root->height = (FT_Short)( root->ascender - root->descender +
                                   face->horizontal.Line_Gap );

        if ( !( root->ascender || root->descender ) )
        {
          if ( face->os2.version != 0xFFFFU )
          {
            if ( face->os2.sTypoAscender || face->os2.sTypoDescender )
            {
              root->ascender  = face->os2.sTypoAscender;
              root->descender = face->os2.sTypoDescender;

              root->height = (FT_Short)( root->ascender - root->descender +
                                         face->os2.sTypoLineGap );
            }
            else
            {
              root->ascender  =  (FT_Short)face->os2.usWinAscent;
              root->descender = -(FT_Short)face->os2.usWinDescent;

              root->height = (FT_UShort)( root->ascender - root->descender );
            }
          }
        }

        root->max_advance_width  = face->horizontal.advance_Width_Max;
        root->max_advance_height = (FT_Short)( face->vertical_info
                                     ? face->vertical.advance_Height_Max
                                     : root->height );

        /* See http://www.microsoft.com/OpenType/OTSpec/post.htm -- */
        /* Adjust underline position from top edge to centre of     */
        /* stroke to convert TrueType meaning to FreeType meaning.  */
        root->underline_position  = face->postscript.underlinePosition -
                                    face->postscript.underlineThickness / 2;
        root->underline_thickness = face->postscript.underlineThickness;
      }

    }

  Exit:
    FT_TRACE2(( "sfnt_load_face: done\n" ));

    return error;
  }


#undef LOAD_
#undef LOADM_
#undef GET_NAME


  FT_LOCAL_DEF( void )
  sfnt_done_face( TT_Face  face )
  {
    FT_Memory     memory;
    SFNT_Service  sfnt;


    if ( !face )
      return;

    memory = face->root.memory;
    sfnt   = (SFNT_Service)face->sfnt;

    if ( sfnt )
    {
      /* destroy the postscript names table if it is loaded */
      if ( sfnt->free_psnames )
        sfnt->free_psnames( face );

      /* destroy the embedded bitmaps table if it is loaded */
      if ( sfnt->free_eblc )
        sfnt->free_eblc( face );
    }

#ifdef TT_CONFIG_OPTION_BDF
    /* freeing the embedded BDF properties */
    tt_face_free_bdf_props( face );
#endif

    /* freeing the kerning table */
    tt_face_done_kern( face );

    /* freeing the collection table */
    FT_FREE( face->ttc_header.offsets );
    face->ttc_header.count = 0;

    /* freeing table directory */
    FT_FREE( face->dir_tables );
    face->num_tables = 0;

    {
      FT_Stream  stream = FT_FACE_STREAM( face );


      /* simply release the 'cmap' table frame */
      FT_FRAME_RELEASE( face->cmap_table );
      face->cmap_size = 0;
    }

    /* freeing the horizontal metrics */
    {
      FT_Stream  stream = FT_FACE_STREAM( face );


      FT_FRAME_RELEASE( face->horz_metrics );
      FT_FRAME_RELEASE( face->vert_metrics );
      face->horz_metrics_size = 0;
      face->vert_metrics_size = 0;
    }

    /* freeing the vertical ones, if any */
    if ( face->vertical_info )
    {
      FT_FREE( face->vertical.long_metrics  );
      FT_FREE( face->vertical.short_metrics );
      face->vertical_info = 0;
    }

    /* freeing the gasp table */
    FT_FREE( face->gasp.gaspRanges );
    face->gasp.numRanges = 0;

    /* freeing the name table */
    if ( sfnt )
      sfnt->free_name( face );

    /* freeing family and style name */
    FT_FREE( face->root.family_name );
    FT_FREE( face->root.style_name );

    /* freeing sbit size table */
    FT_FREE( face->root.available_sizes );
    face->root.num_fixed_sizes = 0;

    FT_FREE( face->postscript_name );

    face->sfnt = 0;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  sfdriver.c                                                             */
/*                                                                         */
/*    High-level SFNT driver interface (body).                             */
/*                                                                         */
/*  Copyright 1996-2007, 2009-2014 by                                      */
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
#include FT_INTERNAL_SFNT_H
#include FT_INTERNAL_OBJECTS_H

#include "sfdriver.h"
#include "ttload.h"
#include "sfobjs.h"
#include "sfntpic.h"

#include "sferrors.h"

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
#include "ttsbit.h"
#endif

#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
#include "ttpost.h"
#endif

#ifdef TT_CONFIG_OPTION_BDF
#include "ttbdf.h"
#include FT_SERVICE_BDF_H
#endif

#include "ttcmap.h"
#include "ttkern.h"
#include "ttmtx.h"

#include FT_SERVICE_GLYPH_DICT_H
#include FT_SERVICE_POSTSCRIPT_NAME_H
#include FT_SERVICE_SFNT_H
#include FT_SERVICE_TT_CMAP_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_sfdriver


  /*
   *  SFNT TABLE SERVICE
   *
   */

  static void*
  get_sfnt_table( TT_Face      face,
                  FT_Sfnt_Tag  tag )
  {
    void*  table;


    switch ( tag )
    {
    case ft_sfnt_head:
      table = &face->header;
      break;

    case ft_sfnt_hhea:
      table = &face->horizontal;
      break;

    case ft_sfnt_vhea:
      table = face->vertical_info ? &face->vertical : 0;
      break;

    case ft_sfnt_os2:
      table = face->os2.version == 0xFFFFU ? 0 : &face->os2;
      break;

    case ft_sfnt_post:
      table = &face->postscript;
      break;

    case ft_sfnt_maxp:
      table = &face->max_profile;
      break;

    case ft_sfnt_pclt:
      table = face->pclt.Version ? &face->pclt : 0;
      break;

    default:
      table = 0;
    }

    return table;
  }


  static FT_Error
  sfnt_table_info( TT_Face    face,
                   FT_UInt    idx,
                   FT_ULong  *tag,
                   FT_ULong  *offset,
                   FT_ULong  *length )
  {
    if ( !offset || !length )
      return FT_THROW( Invalid_Argument );

    if ( !tag )
      *length = face->num_tables;
    else
    {
      if ( idx >= face->num_tables )
        return FT_THROW( Table_Missing );

      *tag    = face->dir_tables[idx].Tag;
      *offset = face->dir_tables[idx].Offset;
      *length = face->dir_tables[idx].Length;
    }

    return FT_Err_Ok;
  }


  FT_DEFINE_SERVICE_SFNT_TABLEREC(
    sfnt_service_sfnt_table,
    (FT_SFNT_TableLoadFunc)tt_face_load_any,
    (FT_SFNT_TableGetFunc) get_sfnt_table,
    (FT_SFNT_TableInfoFunc)sfnt_table_info )


#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES

  /*
   *  GLYPH DICT SERVICE
   *
   */

  static FT_Error
  sfnt_get_glyph_name( TT_Face     face,
                       FT_UInt     glyph_index,
                       FT_Pointer  buffer,
                       FT_UInt     buffer_max )
  {
    FT_String*  gname;
    FT_Error    error;


    error = tt_face_get_ps_name( face, glyph_index, &gname );
    if ( !error )
      FT_STRCPYN( buffer, gname, buffer_max );

    return error;
  }


  static FT_UInt
  sfnt_get_name_index( TT_Face     face,
                       FT_String*  glyph_name )
  {
    FT_Face  root = &face->root;

    FT_UInt  i, max_gid = FT_UINT_MAX;


    if ( root->num_glyphs < 0 )
      return 0;
    else if ( (FT_ULong)root->num_glyphs < FT_UINT_MAX )
      max_gid = (FT_UInt)root->num_glyphs;
    else
      FT_TRACE0(( "Ignore glyph names for invalid GID 0x%08x - 0x%08x\n",
                  FT_UINT_MAX, root->num_glyphs ));

    for ( i = 0; i < max_gid; i++ )
    {
      FT_String*  gname;
      FT_Error    error = tt_face_get_ps_name( face, i, &gname );


      if ( error )
        continue;

      if ( !ft_strcmp( glyph_name, gname ) )
        return i;
    }

    return 0;
  }


  FT_DEFINE_SERVICE_GLYPHDICTREC(
    sfnt_service_glyph_dict,
    (FT_GlyphDict_GetNameFunc)  sfnt_get_glyph_name,
    (FT_GlyphDict_NameIndexFunc)sfnt_get_name_index )


#endif /* TT_CONFIG_OPTION_POSTSCRIPT_NAMES */


  /*
   *  POSTSCRIPT NAME SERVICE
   *
   */

  static const char*
  sfnt_get_ps_name( TT_Face  face )
  {
    FT_Int       n, found_win, found_apple;
    const char*  result = NULL;


    /* shouldn't happen, but just in case to avoid memory leaks */
    if ( face->postscript_name )
      return face->postscript_name;

    /* scan the name table to see whether we have a Postscript name here, */
    /* either in Macintosh or Windows platform encodings                  */
    found_win   = -1;
    found_apple = -1;

    for ( n = 0; n < face->num_names; n++ )
    {
      TT_NameEntryRec*  name = face->name_table.names + n;


      if ( name->nameID == 6 && name->stringLength > 0 )
      {
        if ( name->platformID == 3     &&
             name->encodingID == 1     &&
             name->languageID == 0x409 )
          found_win = n;

        if ( name->platformID == 1 &&
             name->encodingID == 0 &&
             name->languageID == 0 )
          found_apple = n;
      }
    }

    if ( found_win != -1 )
    {
      FT_Memory         memory = face->root.memory;
      TT_NameEntryRec*  name   = face->name_table.names + found_win;
      FT_UInt           len    = name->stringLength / 2;
      FT_Error          error  = FT_Err_Ok;

      FT_UNUSED( error );


      if ( !FT_ALLOC( result, name->stringLength + 1 ) )
      {
        FT_Stream   stream = face->name_table.stream;
        FT_String*  r      = (FT_String*)result;
        FT_Byte*    p;


        if ( FT_STREAM_SEEK( name->stringOffset ) ||
             FT_FRAME_ENTER( name->stringLength ) )
        {
          FT_FREE( result );
          name->stringLength = 0;
          name->stringOffset = 0;
          FT_FREE( name->string );

          goto Exit;
        }

        p = (FT_Byte*)stream->cursor;

        for ( ; len > 0; len--, p += 2 )
        {
          if ( p[0] == 0 && p[1] >= 32 && p[1] < 128 )
            *r++ = p[1];
        }
        *r = '\0';

        FT_FRAME_EXIT();
      }
      goto Exit;
    }

    if ( found_apple != -1 )
    {
      FT_Memory         memory = face->root.memory;
      TT_NameEntryRec*  name   = face->name_table.names + found_apple;
      FT_UInt           len    = name->stringLength;
      FT_Error          error  = FT_Err_Ok;

      FT_UNUSED( error );


      if ( !FT_ALLOC( result, len + 1 ) )
      {
        FT_Stream  stream = face->name_table.stream;


        if ( FT_STREAM_SEEK( name->stringOffset ) ||
             FT_STREAM_READ( result, len )        )
        {
          name->stringOffset = 0;
          name->stringLength = 0;
          FT_FREE( name->string );
          FT_FREE( result );
          goto Exit;
        }
        ((char*)result)[len] = '\0';
      }
    }

  Exit:
    face->postscript_name = result;
    return result;
  }


  FT_DEFINE_SERVICE_PSFONTNAMEREC(
    sfnt_service_ps_name,
    (FT_PsName_GetFunc)sfnt_get_ps_name )


  /*
   *  TT CMAP INFO
   */
  FT_DEFINE_SERVICE_TTCMAPSREC(
    tt_service_get_cmap_info,
    (TT_CMap_Info_GetFunc)tt_get_cmap_info )


#ifdef TT_CONFIG_OPTION_BDF

  static FT_Error
  sfnt_get_charset_id( TT_Face       face,
                       const char*  *acharset_encoding,
                       const char*  *acharset_registry )
  {
    BDF_PropertyRec  encoding, registry;
    FT_Error         error;


    /* XXX: I don't know whether this is correct, since
     *      tt_face_find_bdf_prop only returns something correct if we have
     *      previously selected a size that is listed in the BDF table.
     *      Should we change the BDF table format to include single offsets
     *      for `CHARSET_REGISTRY' and `CHARSET_ENCODING'?
     */
    error = tt_face_find_bdf_prop( face, "CHARSET_REGISTRY", &registry );
    if ( !error )
    {
      error = tt_face_find_bdf_prop( face, "CHARSET_ENCODING", &encoding );
      if ( !error )
      {
        if ( registry.type == BDF_PROPERTY_TYPE_ATOM &&
             encoding.type == BDF_PROPERTY_TYPE_ATOM )
        {
          *acharset_encoding = encoding.u.atom;
          *acharset_registry = registry.u.atom;
        }
        else
          error = FT_THROW( Invalid_Argument );
      }
    }

    return error;
  }


  FT_DEFINE_SERVICE_BDFRec(
    sfnt_service_bdf,
    (FT_BDF_GetCharsetIdFunc)sfnt_get_charset_id,
    (FT_BDF_GetPropertyFunc) tt_face_find_bdf_prop )


#endif /* TT_CONFIG_OPTION_BDF */


  /*
   *  SERVICE LIST
   */

#if defined TT_CONFIG_OPTION_POSTSCRIPT_NAMES && defined TT_CONFIG_OPTION_BDF
  FT_DEFINE_SERVICEDESCREC5(
    sfnt_services,
    FT_SERVICE_ID_SFNT_TABLE,           &SFNT_SERVICE_SFNT_TABLE_GET,
    FT_SERVICE_ID_POSTSCRIPT_FONT_NAME, &SFNT_SERVICE_PS_NAME_GET,
    FT_SERVICE_ID_GLYPH_DICT,           &SFNT_SERVICE_GLYPH_DICT_GET,
    FT_SERVICE_ID_BDF,                  &SFNT_SERVICE_BDF_GET,
    FT_SERVICE_ID_TT_CMAP,              &TT_SERVICE_CMAP_INFO_GET )
#elif defined TT_CONFIG_OPTION_POSTSCRIPT_NAMES
  FT_DEFINE_SERVICEDESCREC4(
    sfnt_services,
    FT_SERVICE_ID_SFNT_TABLE,           &SFNT_SERVICE_SFNT_TABLE_GET,
    FT_SERVICE_ID_POSTSCRIPT_FONT_NAME, &SFNT_SERVICE_PS_NAME_GET,
    FT_SERVICE_ID_GLYPH_DICT,           &SFNT_SERVICE_GLYPH_DICT_GET,
    FT_SERVICE_ID_TT_CMAP,              &TT_SERVICE_CMAP_INFO_GET )
#elif defined TT_CONFIG_OPTION_BDF
  FT_DEFINE_SERVICEDESCREC4(
    sfnt_services,
    FT_SERVICE_ID_SFNT_TABLE,           &SFNT_SERVICE_SFNT_TABLE_GET,
    FT_SERVICE_ID_POSTSCRIPT_FONT_NAME, &SFNT_SERVICE_PS_NAME_GET,
    FT_SERVICE_ID_BDF,                  &SFNT_SERVICE_BDF_GET,
    FT_SERVICE_ID_TT_CMAP,              &TT_SERVICE_CMAP_INFO_GET )
#else
  FT_DEFINE_SERVICEDESCREC3(
    sfnt_services,
    FT_SERVICE_ID_SFNT_TABLE,           &SFNT_SERVICE_SFNT_TABLE_GET,
    FT_SERVICE_ID_POSTSCRIPT_FONT_NAME, &SFNT_SERVICE_PS_NAME_GET,
    FT_SERVICE_ID_TT_CMAP,              &TT_SERVICE_CMAP_INFO_GET )
#endif


  FT_CALLBACK_DEF( FT_Module_Interface )
  sfnt_get_interface( FT_Module    module,
                      const char*  module_interface )
  {
    /* SFNT_SERVICES_GET derefers `library' in PIC mode */
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

    return ft_service_list_lookup( SFNT_SERVICES_GET, module_interface );
  }


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
#define PUT_EMBEDDED_BITMAPS( a )  a
#else
#define PUT_EMBEDDED_BITMAPS( a )  NULL
#endif

#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
#define PUT_PS_NAMES( a )  a
#else
#define PUT_PS_NAMES( a )  NULL
#endif

  FT_DEFINE_SFNT_INTERFACE(
    sfnt_interface,
    tt_face_goto_table,

    sfnt_init_face,
    sfnt_load_face,
    sfnt_done_face,
    sfnt_get_interface,

    tt_face_load_any,

    tt_face_load_head,
    tt_face_load_hhea,
    tt_face_load_cmap,
    tt_face_load_maxp,
    tt_face_load_os2,
    tt_face_load_post,

    tt_face_load_name,
    tt_face_free_name,

    tt_face_load_kern,
    tt_face_load_gasp,
    tt_face_load_pclt,

    /* see `ttload.h' */
    PUT_EMBEDDED_BITMAPS( tt_face_load_bhed ),

    PUT_EMBEDDED_BITMAPS( tt_face_load_sbit_image ),

    /* see `ttpost.h' */
    PUT_PS_NAMES( tt_face_get_ps_name   ),
    PUT_PS_NAMES( tt_face_free_ps_names ),

    /* since version 2.1.8 */
    tt_face_get_kerning,

    /* since version 2.2 */
    tt_face_load_font_dir,
    tt_face_load_hmtx,

    /* see `ttsbit.h' and `sfnt.h' */
    PUT_EMBEDDED_BITMAPS( tt_face_load_sbit ),
    PUT_EMBEDDED_BITMAPS( tt_face_free_sbit ),

    PUT_EMBEDDED_BITMAPS( tt_face_set_sbit_strike     ),
    PUT_EMBEDDED_BITMAPS( tt_face_load_strike_metrics ),

    tt_face_get_metrics
  )


  FT_DEFINE_MODULE(
    sfnt_module_class,

    0,  /* not a font driver or renderer */
    sizeof ( FT_ModuleRec ),

    "sfnt",     /* driver name                            */
    0x10000L,   /* driver version 1.0                     */
    0x20000L,   /* driver requires FreeType 2.0 or higher */

    (const void*)&SFNT_INTERFACE_GET,  /* module specific interface */

    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  sfnt_get_interface )


/* END */

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
/***************************************************************************/
/*                                                                         */
/*  pngshim.c                                                              */
/*                                                                         */
/*    PNG Bitmap glyph support.                                            */
/*                                                                         */
/*  Copyright 2013 by Google, Inc.                                         */
/*  Written by Stuart Gill and Behdad Esfahbod.                            */
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
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include FT_CONFIG_STANDARD_LIBRARY_H


#ifdef FT_CONFIG_OPTION_USE_PNG

  /* We always include <stjmp.h>, so make libpng shut up! */
#define PNG_SKIP_SETJMP_CHECK 1
#include <png.h>
#include "pngshim.h"

#include "sferrors.h"


  /* This code is freely based on cairo-png.c.  There's so many ways */
  /* to call libpng, and the way cairo does it is defacto standard.  */

  static int
  multiply_alpha( int  alpha,
                  int  color )
  {
    int  temp = ( alpha * color ) + 0x80;


    return ( temp + ( temp >> 8 ) ) >> 8;
  }


  /* Premultiplies data and converts RGBA bytes => native endian. */
  static void
  premultiply_data( png_structp    png,
                    png_row_infop  row_info,
                    png_bytep      data )
  {
    unsigned int  i;

    FT_UNUSED( png );


    for ( i = 0; i < row_info->rowbytes; i += 4 )
    {
      unsigned char*  base  = &data[i];
      unsigned int    alpha = base[3];


      if ( alpha == 0 )
        base[0] = base[1] = base[2] = base[3] = 0;

      else
      {
        unsigned int  red   = base[0];
        unsigned int  green = base[1];
        unsigned int  blue  = base[2];


        if ( alpha != 0xFF )
        {
          red   = multiply_alpha( alpha, red   );
          green = multiply_alpha( alpha, green );
          blue  = multiply_alpha( alpha, blue  );
        }

        base[0] = blue;
        base[1] = green;
        base[2] = red;
        base[3] = alpha;
      }
    }
  }


  /* Converts RGBx bytes to BGRA. */
  static void
  convert_bytes_to_data( png_structp    png,
                         png_row_infop  row_info,
                         png_bytep      data )
  {
    unsigned int  i;

    FT_UNUSED( png );


    for ( i = 0; i < row_info->rowbytes; i += 4 )
    {
      unsigned char*  base  = &data[i];
      unsigned int    red   = base[0];
      unsigned int    green = base[1];
      unsigned int    blue  = base[2];


      base[0] = blue;
      base[1] = green;
      base[2] = red;
      base[3] = 0xFF;
    }
  }


  /* Use error callback to avoid png writing to stderr. */
  static void
  error_callback( png_structp      png,
                  png_const_charp  error_msg )
  {
    FT_Error*  error = (FT_Error*)png_get_error_ptr( png );

    FT_UNUSED( error_msg );


    *error = FT_THROW( Out_Of_Memory );
#ifdef PNG_SETJMP_SUPPORTED
    longjmp( png_jmpbuf( png ), 1 );
#endif
    /* if we get here, then we have no choice but to abort ... */
  }


  /* Use warning callback to avoid png writing to stderr. */
  static void
  warning_callback( png_structp      png,
                    png_const_charp  error_msg )
  {
    FT_UNUSED( png );
    FT_UNUSED( error_msg );

    /* Just ignore warnings. */
  }


  static void
  read_data_from_FT_Stream( png_structp  png,
                            png_bytep    data,
                            png_size_t   length )
  {
    FT_Error   error;
    png_voidp  p      = png_get_io_ptr( png );
    FT_Stream  stream = (FT_Stream)p;


    if ( FT_FRAME_ENTER( length ) )
    {
      FT_Error*  e = (FT_Error*)png_get_error_ptr( png );


      *e = FT_THROW( Invalid_Stream_Read );
      png_error( png, NULL );

      return;
    }

    memcpy( data, stream->cursor, length );

    FT_FRAME_EXIT();
  }


  FT_LOCAL_DEF( FT_Error )
  Load_SBit_Png( FT_GlyphSlot     slot,
                 FT_Int           x_offset,
                 FT_Int           y_offset,
                 FT_Int           pix_bits,
                 TT_SBit_Metrics  metrics,
                 FT_Memory        memory,
                 FT_Byte*         data,
                 FT_UInt          png_len,
                 FT_Bool          populate_map_and_metrics )
  {
    FT_Bitmap    *map   = &slot->bitmap;
    FT_Error      error = FT_Err_Ok;
    FT_StreamRec  stream;

    png_structp  png;
    png_infop    info;
    png_uint_32  imgWidth, imgHeight;

    int         bitdepth, color_type, interlace;
    FT_Int      i;
    png_byte*  *rows = NULL; /* pacify compiler */


    if ( x_offset < 0 ||
         y_offset < 0 )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    if ( !populate_map_and_metrics                   &&
         ( x_offset + metrics->width  > map->width ||
           y_offset + metrics->height > map->rows  ||
           pix_bits != 32                          ||
           map->pixel_mode != FT_PIXEL_MODE_BGRA   ) )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    FT_Stream_OpenMemory( &stream, data, png_len );

    png = png_create_read_struct( PNG_LIBPNG_VER_STRING,
                                  &error,
                                  error_callback,
                                  warning_callback );
    if ( !png )
    {
      error = FT_THROW( Out_Of_Memory );
      goto Exit;
    }

    info = png_create_info_struct( png );
    if ( !info )
    {
      error = FT_THROW( Out_Of_Memory );
      png_destroy_read_struct( &png, NULL, NULL );
      goto Exit;
    }

    if ( ft_setjmp( png_jmpbuf( png ) ) )
    {
      error = FT_THROW( Invalid_File_Format );
      goto DestroyExit;
    }

    png_set_read_fn( png, &stream, read_data_from_FT_Stream );

    png_read_info( png, info );
    png_get_IHDR( png, info,
                  &imgWidth, &imgHeight,
                  &bitdepth, &color_type, &interlace,
                  NULL, NULL );

    if ( error                                        ||
         ( !populate_map_and_metrics                &&
           ( (FT_Int)imgWidth  != metrics->width  ||
             (FT_Int)imgHeight != metrics->height ) ) )
      goto DestroyExit;

    if ( populate_map_and_metrics )
    {
      FT_Long  size;


      metrics->width  = (FT_Int)imgWidth;
      metrics->height = (FT_Int)imgHeight;

      map->width      = metrics->width;
      map->rows       = metrics->height;
      map->pixel_mode = FT_PIXEL_MODE_BGRA;
      map->pitch      = map->width * 4;
      map->num_grays  = 256;

      size = map->rows * map->pitch;

      error = ft_glyphslot_alloc_bitmap( slot, size );
      if ( error )
        goto DestroyExit;
    }

    /* convert palette/gray image to rgb */
    if ( color_type == PNG_COLOR_TYPE_PALETTE )
      png_set_palette_to_rgb( png );

    /* expand gray bit depth if needed */
    if ( color_type == PNG_COLOR_TYPE_GRAY )
    {
#if PNG_LIBPNG_VER >= 10209
      png_set_expand_gray_1_2_4_to_8( png );
#else
      png_set_gray_1_2_4_to_8( png );
#endif
    }

    /* transform transparency to alpha */
    if ( png_get_valid(png, info, PNG_INFO_tRNS ) )
      png_set_tRNS_to_alpha( png );

    if ( bitdepth == 16 )
      png_set_strip_16( png );

    if ( bitdepth < 8 )
      png_set_packing( png );

    /* convert grayscale to RGB */
    if ( color_type == PNG_COLOR_TYPE_GRAY       ||
         color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
      png_set_gray_to_rgb( png );

    if ( interlace != PNG_INTERLACE_NONE )
      png_set_interlace_handling( png );

    png_set_filler( png, 0xFF, PNG_FILLER_AFTER );

    /* recheck header after setting EXPAND options */
    png_read_update_info(png, info );
    png_get_IHDR( png, info,
                  &imgWidth, &imgHeight,
                  &bitdepth, &color_type, &interlace,
                  NULL, NULL );

    if ( bitdepth != 8                              ||
        !( color_type == PNG_COLOR_TYPE_RGB       ||
           color_type == PNG_COLOR_TYPE_RGB_ALPHA ) )
    {
      error = FT_THROW( Invalid_File_Format );
      goto DestroyExit;
    }

    switch ( color_type )
    {
    default:
      /* Shouldn't happen, but fall through. */

    case PNG_COLOR_TYPE_RGB_ALPHA:
      png_set_read_user_transform_fn( png, premultiply_data );
      break;

    case PNG_COLOR_TYPE_RGB:
      /* Humm, this smells.  Carry on though. */
      png_set_read_user_transform_fn( png, convert_bytes_to_data );
      break;
    }

    if ( FT_NEW_ARRAY( rows, imgHeight ) )
    {
      error = FT_THROW( Out_Of_Memory );
      goto DestroyExit;
    }

    for ( i = 0; i < (FT_Int)imgHeight; i++ )
      rows[i] = map->buffer + ( y_offset + i ) * map->pitch + x_offset * 4;

    png_read_image( png, rows );

    FT_FREE( rows );

    png_read_end( png, info );

  DestroyExit:
    png_destroy_read_struct( &png, &info, NULL );
    FT_Stream_Close( &stream );

  Exit:
    return error;
  }

#endif /* FT_CONFIG_OPTION_USE_PNG */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttsbit.c                                                               */
/*                                                                         */
/*    TrueType and OpenType embedded bitmap support (body).                */
/*                                                                         */
/*  Copyright 2005-2009, 2013, 2014 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  Copyright 2013 by Google, Inc.                                         */
/*  Google Author(s): Behdad Esfahbod.                                     */
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
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include FT_BITMAP_H
#include "ttsbit.h"

#include "sferrors.h"

#include "ttmtx.h"
#include "pngshim.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttsbit


  FT_LOCAL_DEF( FT_Error )
  tt_face_load_sbit( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error  error;
    FT_ULong  table_size;


    face->sbit_table       = NULL;
    face->sbit_table_size  = 0;
    face->sbit_table_type  = TT_SBIT_TABLE_TYPE_NONE;
    face->sbit_num_strikes = 0;

    error = face->goto_table( face, TTAG_CBLC, stream, &table_size );
    if ( !error )
      face->sbit_table_type = TT_SBIT_TABLE_TYPE_CBLC;
    else
    {
      error = face->goto_table( face, TTAG_EBLC, stream, &table_size );
      if ( error )
        error = face->goto_table( face, TTAG_bloc, stream, &table_size );
      if ( !error )
        face->sbit_table_type = TT_SBIT_TABLE_TYPE_EBLC;
    }

    if ( error )
    {
      error = face->goto_table( face, TTAG_sbix, stream, &table_size );
      if ( !error )
        face->sbit_table_type = TT_SBIT_TABLE_TYPE_SBIX;
    }
    if ( error )
      goto Exit;

    if ( table_size < 8 )
    {
      FT_ERROR(( "tt_face_load_sbit_strikes: table too short\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    switch ( (FT_UInt)face->sbit_table_type )
    {
    case TT_SBIT_TABLE_TYPE_EBLC:
    case TT_SBIT_TABLE_TYPE_CBLC:
      {
        FT_Byte*  p;
        FT_Fixed  version;
        FT_ULong  num_strikes;
        FT_UInt   count;


        if ( FT_FRAME_EXTRACT( table_size, face->sbit_table ) )
          goto Exit;

        face->sbit_table_size = table_size;

        p = face->sbit_table;

        version     = FT_NEXT_ULONG( p );
        num_strikes = FT_NEXT_ULONG( p );

        if ( ( version & 0xFFFF0000UL ) != 0x00020000UL )
        {
          error = FT_THROW( Unknown_File_Format );
          goto Exit;
        }

        if ( num_strikes >= 0x10000UL )
        {
          error = FT_THROW( Invalid_File_Format );
          goto Exit;
        }

        /*
         *  Count the number of strikes available in the table.  We are a bit
         *  paranoid there and don't trust the data.
         */
        count = (FT_UInt)num_strikes;
        if ( 8 + 48UL * count > table_size )
          count = (FT_UInt)( ( table_size - 8 ) / 48 );

        face->sbit_num_strikes = count;
      }
      break;

    case TT_SBIT_TABLE_TYPE_SBIX:
      {
        FT_UShort  version;
        FT_UShort  flags;
        FT_ULong   num_strikes;
        FT_UInt    count;


        if ( FT_FRAME_ENTER( 8 ) )
          goto Exit;

        version     = FT_GET_USHORT();
        flags       = FT_GET_USHORT();
        num_strikes = FT_GET_ULONG();

        FT_FRAME_EXIT();

        if ( version < 1 )
        {
          error = FT_THROW( Unknown_File_Format );
          goto Exit;
        }
        if ( flags != 0x0001 || num_strikes >= 0x10000UL )
        {
          error = FT_THROW( Invalid_File_Format );
          goto Exit;
        }

        /*
         *  Count the number of strikes available in the table.  We are a bit
         *  paranoid there and don't trust the data.
         */
        count = (FT_UInt)num_strikes;
        if ( 8 + 4UL * count > table_size )
          count = (FT_UInt)( ( table_size - 8 ) / 4 );

        if ( FT_STREAM_SEEK( FT_STREAM_POS() - 8 ) )
          goto Exit;

        face->sbit_table_size = 8 + count * 4;
        if ( FT_FRAME_EXTRACT( face->sbit_table_size, face->sbit_table ) )
          goto Exit;

        face->sbit_num_strikes = count;
      }
      break;

    default:
      error = FT_THROW( Unknown_File_Format );
      break;
    }

    if ( !error )
      FT_TRACE3(( "sbit_num_strikes: %u\n", face->sbit_num_strikes ));

    return FT_Err_Ok;

  Exit:
    if ( error )
    {
      if ( face->sbit_table )
        FT_FRAME_RELEASE( face->sbit_table );
      face->sbit_table_size = 0;
      face->sbit_table_type = TT_SBIT_TABLE_TYPE_NONE;
    }

    return error;
  }


  FT_LOCAL_DEF( void )
  tt_face_free_sbit( TT_Face  face )
  {
    FT_Stream  stream = face->root.stream;


    FT_FRAME_RELEASE( face->sbit_table );
    face->sbit_table_size  = 0;
    face->sbit_table_type  = TT_SBIT_TABLE_TYPE_NONE;
    face->sbit_num_strikes = 0;
  }


  FT_LOCAL_DEF( FT_Error )
  tt_face_set_sbit_strike( TT_Face          face,
                           FT_Size_Request  req,
                           FT_ULong*        astrike_index )
  {
    return FT_Match_Size( (FT_Face)face, req, 0, astrike_index );
  }


  FT_LOCAL_DEF( FT_Error )
  tt_face_load_strike_metrics( TT_Face           face,
                               FT_ULong          strike_index,
                               FT_Size_Metrics*  metrics )
  {
    if ( strike_index >= (FT_ULong)face->sbit_num_strikes )
      return FT_THROW( Invalid_Argument );

    switch ( (FT_UInt)face->sbit_table_type )
    {
    case TT_SBIT_TABLE_TYPE_EBLC:
    case TT_SBIT_TABLE_TYPE_CBLC:
      {
        FT_Byte*  strike;


        strike = face->sbit_table + 8 + strike_index * 48;

        metrics->x_ppem = (FT_UShort)strike[44];
        metrics->y_ppem = (FT_UShort)strike[45];

        metrics->ascender  = (FT_Char)strike[16] << 6;  /* hori.ascender  */
        metrics->descender = (FT_Char)strike[17] << 6;  /* hori.descender */
        metrics->height    = metrics->ascender - metrics->descender;

        /* Is this correct? */
        metrics->max_advance = ( (FT_Char)strike[22] + /* min_origin_SB  */
                                          strike[18] + /* max_width      */
                                 (FT_Char)strike[23]   /* min_advance_SB */
                                                     ) << 6;
        return FT_Err_Ok;
      }

    case TT_SBIT_TABLE_TYPE_SBIX:
      {
        FT_Stream       stream = face->root.stream;
        FT_UInt         offset, ppem, resolution, upem;
        TT_HoriHeader  *hori;
        FT_ULong        table_size;

        FT_Error  error;
        FT_Byte*  p;


        p      = face->sbit_table + 8 + 4 * strike_index;
        offset = FT_NEXT_ULONG( p );

        error = face->goto_table( face, TTAG_sbix, stream, &table_size );
        if ( error )
          return error;

        if ( offset + 4  > table_size )
          return FT_THROW( Invalid_File_Format );

        if ( FT_STREAM_SEEK( FT_STREAM_POS() + offset ) ||
             FT_FRAME_ENTER( 4 )                        )
          return error;

        ppem       = FT_GET_USHORT();
        resolution = FT_GET_USHORT();

        FT_UNUSED( resolution ); /* What to do with this? */

        FT_FRAME_EXIT();

        upem = face->header.Units_Per_EM;
        hori = &face->horizontal;

        metrics->x_ppem = ppem;
        metrics->y_ppem = ppem;

        metrics->ascender    = ppem * hori->Ascender * 64 / upem;
        metrics->descender   = ppem * hori->Descender * 64 / upem;
        metrics->height      = ppem * ( hori->Ascender -
                                        hori->Descender +
                                        hori->Line_Gap ) * 64 / upem;
        metrics->max_advance = ppem * hori->advance_Width_Max * 64 / upem;

        return error;
      }

    default:
      return FT_THROW( Unknown_File_Format );
    }
  }


  typedef struct  TT_SBitDecoderRec_
  {
    TT_Face          face;
    FT_Stream        stream;
    FT_Bitmap*       bitmap;
    TT_SBit_Metrics  metrics;
    FT_Bool          metrics_loaded;
    FT_Bool          bitmap_allocated;
    FT_Byte          bit_depth;

    FT_ULong         ebdt_start;
    FT_ULong         ebdt_size;

    FT_ULong         strike_index_array;
    FT_ULong         strike_index_count;
    FT_Byte*         eblc_base;
    FT_Byte*         eblc_limit;

  } TT_SBitDecoderRec, *TT_SBitDecoder;


  static FT_Error
  tt_sbit_decoder_init( TT_SBitDecoder       decoder,
                        TT_Face              face,
                        FT_ULong             strike_index,
                        TT_SBit_MetricsRec*  metrics )
  {
    FT_Error   error;
    FT_Stream  stream = face->root.stream;
    FT_ULong   ebdt_size;


    error = face->goto_table( face, TTAG_CBDT, stream, &ebdt_size );
    if ( error )
      error = face->goto_table( face, TTAG_EBDT, stream, &ebdt_size );
    if ( error )
      error = face->goto_table( face, TTAG_bdat, stream, &ebdt_size );
    if ( error )
      goto Exit;

    decoder->face    = face;
    decoder->stream  = stream;
    decoder->bitmap  = &face->root.glyph->bitmap;
    decoder->metrics = metrics;

    decoder->metrics_loaded   = 0;
    decoder->bitmap_allocated = 0;

    decoder->ebdt_start = FT_STREAM_POS();
    decoder->ebdt_size  = ebdt_size;

    decoder->eblc_base  = face->sbit_table;
    decoder->eblc_limit = face->sbit_table + face->sbit_table_size;

    /* now find the strike corresponding to the index */
    {
      FT_Byte*  p;


      if ( 8 + 48 * strike_index + 3 * 4 + 34 + 1 > face->sbit_table_size )
      {
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      p = decoder->eblc_base + 8 + 48 * strike_index;

      decoder->strike_index_array = FT_NEXT_ULONG( p );
      p                          += 4;
      decoder->strike_index_count = FT_NEXT_ULONG( p );
      p                          += 34;
      decoder->bit_depth          = *p;

      if ( decoder->strike_index_array > face->sbit_table_size             ||
           decoder->strike_index_array + 8 * decoder->strike_index_count >
             face->sbit_table_size                                         )
        error = FT_THROW( Invalid_File_Format );
    }

  Exit:
    return error;
  }


  static void
  tt_sbit_decoder_done( TT_SBitDecoder  decoder )
  {
    FT_UNUSED( decoder );
  }


  static FT_Error
  tt_sbit_decoder_alloc_bitmap( TT_SBitDecoder  decoder )
  {
    FT_Error    error = FT_Err_Ok;
    FT_UInt     width, height;
    FT_Bitmap*  map = decoder->bitmap;
    FT_Long     size;


    if ( !decoder->metrics_loaded )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    width  = decoder->metrics->width;
    height = decoder->metrics->height;

    map->width = (int)width;
    map->rows  = (int)height;

    switch ( decoder->bit_depth )
    {
    case 1:
      map->pixel_mode = FT_PIXEL_MODE_MONO;
      map->pitch      = ( map->width + 7 ) >> 3;
      map->num_grays  = 2;
      break;

    case 2:
      map->pixel_mode = FT_PIXEL_MODE_GRAY2;
      map->pitch      = ( map->width + 3 ) >> 2;
      map->num_grays  = 4;
      break;

    case 4:
      map->pixel_mode = FT_PIXEL_MODE_GRAY4;
      map->pitch      = ( map->width + 1 ) >> 1;
      map->num_grays  = 16;
      break;

    case 8:
      map->pixel_mode = FT_PIXEL_MODE_GRAY;
      map->pitch      = map->width;
      map->num_grays  = 256;
      break;

    case 32:
      map->pixel_mode = FT_PIXEL_MODE_BGRA;
      map->pitch      = map->width * 4;
      map->num_grays  = 256;
      break;

    default:
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    size = map->rows * map->pitch;

    /* check that there is no empty image */
    if ( size == 0 )
      goto Exit;     /* exit successfully! */

    error = ft_glyphslot_alloc_bitmap( decoder->face->root.glyph, size );
    if ( error )
      goto Exit;

    decoder->bitmap_allocated = 1;

  Exit:
    return error;
  }


  static FT_Error
  tt_sbit_decoder_load_metrics( TT_SBitDecoder  decoder,
                                FT_Byte*       *pp,
                                FT_Byte*        limit,
                                FT_Bool         big )
  {
    FT_Byte*         p       = *pp;
    TT_SBit_Metrics  metrics = decoder->metrics;


    if ( p + 5 > limit )
      goto Fail;

    metrics->height       = p[0];
    metrics->width        = p[1];
    metrics->horiBearingX = (FT_Char)p[2];
    metrics->horiBearingY = (FT_Char)p[3];
    metrics->horiAdvance  = p[4];

    p += 5;
    if ( big )
    {
      if ( p + 3 > limit )
        goto Fail;

      metrics->vertBearingX = (FT_Char)p[0];
      metrics->vertBearingY = (FT_Char)p[1];
      metrics->vertAdvance  = p[2];

      p += 3;
    }

    decoder->metrics_loaded = 1;
    *pp = p;
    return FT_Err_Ok;

  Fail:
    FT_TRACE1(( "tt_sbit_decoder_load_metrics: broken table" ));
    return FT_THROW( Invalid_Argument );
  }


  /* forward declaration */
  static FT_Error
  tt_sbit_decoder_load_image( TT_SBitDecoder  decoder,
                              FT_UInt         glyph_index,
                              FT_Int          x_pos,
                              FT_Int          y_pos );

  typedef FT_Error  (*TT_SBitDecoder_LoadFunc)( TT_SBitDecoder  decoder,
                                                FT_Byte*        p,
                                                FT_Byte*        plimit,
                                                FT_Int          x_pos,
                                                FT_Int          y_pos );


  static FT_Error
  tt_sbit_decoder_load_byte_aligned( TT_SBitDecoder  decoder,
                                     FT_Byte*        p,
                                     FT_Byte*        limit,
                                     FT_Int          x_pos,
                                     FT_Int          y_pos )
  {
    FT_Error    error = FT_Err_Ok;
    FT_Byte*    line;
    FT_Int      bit_height, bit_width, pitch, width, height, line_bits, h;
    FT_Bitmap*  bitmap;


    /* check that we can write the glyph into the bitmap */
    bitmap     = decoder->bitmap;
    bit_width  = bitmap->width;
    bit_height = bitmap->rows;
    pitch      = bitmap->pitch;
    line       = bitmap->buffer;

    width  = decoder->metrics->width;
    height = decoder->metrics->height;

    line_bits = width * decoder->bit_depth;

    if ( x_pos < 0 || x_pos + width > bit_width   ||
         y_pos < 0 || y_pos + height > bit_height )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_byte_aligned:"
                  " invalid bitmap dimensions\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    if ( p + ( ( line_bits + 7 ) >> 3 ) * height > limit )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_byte_aligned: broken bitmap\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    /* now do the blit */
    line  += y_pos * pitch + ( x_pos >> 3 );
    x_pos &= 7;

    if ( x_pos == 0 )  /* the easy one */
    {
      for ( h = height; h > 0; h--, line += pitch )
      {
        FT_Byte*  pwrite = line;
        FT_Int    w;


        for ( w = line_bits; w >= 8; w -= 8 )
        {
          pwrite[0] = (FT_Byte)( pwrite[0] | *p++ );
          pwrite   += 1;
        }

        if ( w > 0 )
          pwrite[0] = (FT_Byte)( pwrite[0] | ( *p++ & ( 0xFF00U >> w ) ) );
      }
    }
    else  /* x_pos > 0 */
    {
      for ( h = height; h > 0; h--, line += pitch )
      {
        FT_Byte*  pwrite = line;
        FT_Int    w;
        FT_UInt   wval = 0;


        for ( w = line_bits; w >= 8; w -= 8 )
        {
          wval       = (FT_UInt)( wval | *p++ );
          pwrite[0]  = (FT_Byte)( pwrite[0] | ( wval >> x_pos ) );
          pwrite    += 1;
          wval     <<= 8;
        }

        if ( w > 0 )
          wval = (FT_UInt)( wval | ( *p++ & ( 0xFF00U >> w ) ) );

        /* all bits read and there are `x_pos + w' bits to be written */

        pwrite[0] = (FT_Byte)( pwrite[0] | ( wval >> x_pos ) );

        if ( x_pos + w > 8 )
        {
          pwrite++;
          wval     <<= 8;
          pwrite[0]  = (FT_Byte)( pwrite[0] | ( wval >> x_pos ) );
        }
      }
    }

  Exit:
    if ( !error )
      FT_TRACE3(( "tt_sbit_decoder_load_byte_aligned: loaded\n" ));
    return error;
  }


  /*
   * Load a bit-aligned bitmap (with pointer `p') into a line-aligned bitmap
   * (with pointer `pwrite').  In the example below, the width is 3 pixel,
   * and `x_pos' is 1 pixel.
   *
   *       p                               p+1
   *     |                               |                               |
   *     | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 |...
   *     |                               |                               |
   *       +-------+   +-------+   +-------+ ...
   *           .           .           .
   *           .           .           .
   *           v           .           .
   *       +-------+       .           .
   * |                               | .
   * | 7   6   5   4   3   2   1   0 | .
   * |                               | .
   *   pwrite              .           .
   *                       .           .
   *                       v           .
   *                   +-------+       .
   *             |                               |
   *             | 7   6   5   4   3   2   1   0 |
   *             |                               |
   *               pwrite+1            .
   *                                   .
   *                                   v
   *                               +-------+
   *                         |                               |
   *                         | 7   6   5   4   3   2   1   0 |
   *                         |                               |
   *                           pwrite+2
   *
   */

  static FT_Error
  tt_sbit_decoder_load_bit_aligned( TT_SBitDecoder  decoder,
                                    FT_Byte*        p,
                                    FT_Byte*        limit,
                                    FT_Int          x_pos,
                                    FT_Int          y_pos )
  {
    FT_Error    error = FT_Err_Ok;
    FT_Byte*    line;
    FT_Int      bit_height, bit_width, pitch, width, height, line_bits, h, nbits;
    FT_Bitmap*  bitmap;
    FT_UShort   rval;


    /* check that we can write the glyph into the bitmap */
    bitmap     = decoder->bitmap;
    bit_width  = bitmap->width;
    bit_height = bitmap->rows;
    pitch      = bitmap->pitch;
    line       = bitmap->buffer;

    width  = decoder->metrics->width;
    height = decoder->metrics->height;

    line_bits = width * decoder->bit_depth;

    if ( x_pos < 0 || x_pos + width  > bit_width  ||
         y_pos < 0 || y_pos + height > bit_height )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_bit_aligned:"
                  " invalid bitmap dimensions\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    if ( p + ( ( line_bits * height + 7 ) >> 3 ) > limit )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_bit_aligned: broken bitmap\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    /* now do the blit */

    /* adjust `line' to point to the first byte of the bitmap */
    line  += y_pos * pitch + ( x_pos >> 3 );
    x_pos &= 7;

    /* the higher byte of `rval' is used as a buffer */
    rval  = 0;
    nbits = 0;

    for ( h = height; h > 0; h--, line += pitch )
    {
      FT_Byte*  pwrite = line;
      FT_Int    w      = line_bits;


      /* handle initial byte (in target bitmap) specially if necessary */
      if ( x_pos )
      {
        w = ( line_bits < 8 - x_pos ) ? line_bits : 8 - x_pos;

        if ( h == height )
        {
          rval  = *p++;
          nbits = x_pos;
        }
        else if ( nbits < w )
        {
          if ( p < limit )
            rval |= *p++;
          nbits += 8 - w;
        }
        else
        {
          rval  >>= 8;
          nbits  -= w;
        }

        *pwrite++ |= ( ( rval >> nbits ) & 0xFF ) &
                     ( ~( 0xFF << w ) << ( 8 - w - x_pos ) );
        rval     <<= 8;

        w = line_bits - w;
      }

      /* handle medial bytes */
      for ( ; w >= 8; w -= 8 )
      {
        rval      |= *p++;
        *pwrite++ |= ( rval >> nbits ) & 0xFF;

        rval <<= 8;
      }

      /* handle final byte if necessary */
      if ( w > 0 )
      {
        if ( nbits < w )
        {
          if ( p < limit )
            rval |= *p++;
          *pwrite |= ( ( rval >> nbits ) & 0xFF ) & ( 0xFF00U >> w );
          nbits   += 8 - w;

          rval <<= 8;
        }
        else
        {
          *pwrite |= ( ( rval >> nbits ) & 0xFF ) & ( 0xFF00U >> w );
          nbits   -= w;
        }
      }
    }

  Exit:
    if ( !error )
      FT_TRACE3(( "tt_sbit_decoder_load_bit_aligned: loaded\n" ));
    return error;
  }


  static FT_Error
  tt_sbit_decoder_load_compound( TT_SBitDecoder  decoder,
                                 FT_Byte*        p,
                                 FT_Byte*        limit,
                                 FT_Int          x_pos,
                                 FT_Int          y_pos )
  {
    FT_Error  error = FT_Err_Ok;
    FT_UInt   num_components, nn;

    FT_Char  horiBearingX = (FT_Char)decoder->metrics->horiBearingX;
    FT_Char  horiBearingY = (FT_Char)decoder->metrics->horiBearingY;
    FT_Byte  horiAdvance  = (FT_Byte)decoder->metrics->horiAdvance;
    FT_Char  vertBearingX = (FT_Char)decoder->metrics->vertBearingX;
    FT_Char  vertBearingY = (FT_Char)decoder->metrics->vertBearingY;
    FT_Byte  vertAdvance  = (FT_Byte)decoder->metrics->vertAdvance;


    if ( p + 2 > limit )
      goto Fail;

    num_components = FT_NEXT_USHORT( p );
    if ( p + 4 * num_components > limit )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_compound: broken table\n" ));
      goto Fail;
    }

    FT_TRACE3(( "tt_sbit_decoder_load_compound: loading %d components\n",
                num_components ));

    for ( nn = 0; nn < num_components; nn++ )
    {
      FT_UInt  gindex = FT_NEXT_USHORT( p );
      FT_Byte  dx     = FT_NEXT_BYTE( p );
      FT_Byte  dy     = FT_NEXT_BYTE( p );


      /* NB: a recursive call */
      error = tt_sbit_decoder_load_image( decoder, gindex,
                                          x_pos + dx, y_pos + dy );
      if ( error )
        break;
    }

    FT_TRACE3(( "tt_sbit_decoder_load_compound: done\n" ));

    decoder->metrics->horiBearingX = horiBearingX;
    decoder->metrics->horiBearingY = horiBearingY;
    decoder->metrics->horiAdvance  = horiAdvance;
    decoder->metrics->vertBearingX = vertBearingX;
    decoder->metrics->vertBearingY = vertBearingY;
    decoder->metrics->vertAdvance  = vertAdvance;
    decoder->metrics->width        = (FT_Byte)decoder->bitmap->width;
    decoder->metrics->height       = (FT_Byte)decoder->bitmap->rows;

  Exit:
    return error;

  Fail:
    error = FT_THROW( Invalid_File_Format );
    goto Exit;
  }


#ifdef FT_CONFIG_OPTION_USE_PNG

  static FT_Error
  tt_sbit_decoder_load_png( TT_SBitDecoder  decoder,
                            FT_Byte*        p,
                            FT_Byte*        limit,
                            FT_Int          x_pos,
                            FT_Int          y_pos )
  {
    FT_Error  error = FT_Err_Ok;
    FT_ULong  png_len;


    if ( limit - p < 4 )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_png: broken bitmap\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    png_len = FT_NEXT_ULONG( p );
    if ( (FT_ULong)( limit - p ) < png_len )
    {
      FT_TRACE1(( "tt_sbit_decoder_load_png: broken bitmap\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    error = Load_SBit_Png( decoder->face->root.glyph,
                           x_pos,
                           y_pos,
                           decoder->bit_depth,
                           decoder->metrics,
                           decoder->stream->memory,
                           p,
                           png_len,
                           FALSE );

  Exit:
    if ( !error )
      FT_TRACE3(( "tt_sbit_decoder_load_png: loaded\n" ));
    return error;
  }

#endif /* FT_CONFIG_OPTION_USE_PNG */


  static FT_Error
  tt_sbit_decoder_load_bitmap( TT_SBitDecoder  decoder,
                               FT_UInt         glyph_format,
                               FT_ULong        glyph_start,
                               FT_ULong        glyph_size,
                               FT_Int          x_pos,
                               FT_Int          y_pos )
  {
    FT_Error   error;
    FT_Stream  stream = decoder->stream;
    FT_Byte*   p;
    FT_Byte*   p_limit;
    FT_Byte*   data;


    /* seek into the EBDT table now */
    if ( glyph_start + glyph_size > decoder->ebdt_size )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    if ( FT_STREAM_SEEK( decoder->ebdt_start + glyph_start ) ||
         FT_FRAME_EXTRACT( glyph_size, data )                )
      goto Exit;

    p       = data;
    p_limit = p + glyph_size;

    /* read the data, depending on the glyph format */
    switch ( glyph_format )
    {
    case 1:
    case 2:
    case 8:
    case 17:
      error = tt_sbit_decoder_load_metrics( decoder, &p, p_limit, 0 );
      break;

    case 6:
    case 7:
    case 9:
    case 18:
      error = tt_sbit_decoder_load_metrics( decoder, &p, p_limit, 1 );
      break;

    default:
      error = FT_Err_Ok;
    }

    if ( error )
      goto Fail;

    {
      TT_SBitDecoder_LoadFunc  loader;


      switch ( glyph_format )
      {
      case 1:
      case 6:
        loader = tt_sbit_decoder_load_byte_aligned;
        break;

      case 2:
      case 7:
        {
          /* Don't trust `glyph_format'.  For example, Apple's main Korean */
          /* system font, `AppleMyungJo.ttf' (version 7.0d2e6), uses glyph */
          /* format 7, but the data is format 6.  We check whether we have */
          /* an excessive number of bytes in the image: If it is equal to  */
          /* the value for a byte-aligned glyph, use the other loading     */
          /* routine.                                                      */
          /*                                                               */
          /* Note that for some (width,height) combinations, where the     */
          /* width is not a multiple of 8, the sizes for bit- and          */
          /* byte-aligned data are equal, for example (7,7) or (15,6).  We */
          /* then prefer what `glyph_format' specifies.                    */

          FT_UInt  width  = decoder->metrics->width;
          FT_UInt  height = decoder->metrics->height;

          FT_UInt  bit_size  = ( width * height + 7 ) >> 3;
          FT_UInt  byte_size = height * ( ( width + 7 ) >> 3 );


          if ( bit_size < byte_size                  &&
               byte_size == (FT_UInt)( p_limit - p ) )
            loader = tt_sbit_decoder_load_byte_aligned;
          else
            loader = tt_sbit_decoder_load_bit_aligned;
        }
        break;

      case 5:
        loader = tt_sbit_decoder_load_bit_aligned;
        break;

      case 8:
        if ( p + 1 > p_limit )
          goto Fail;

        p += 1;  /* skip padding */
        /* fall-through */

      case 9:
        loader = tt_sbit_decoder_load_compound;
        break;

      case 17: /* small metrics, PNG image data   */
      case 18: /* big metrics, PNG image data     */
      case 19: /* metrics in EBLC, PNG image data */
#ifdef FT_CONFIG_OPTION_USE_PNG
        loader = tt_sbit_decoder_load_png;
        break;
#else
        error = FT_THROW( Unimplemented_Feature );
        goto Fail;
#endif /* FT_CONFIG_OPTION_USE_PNG */

      default:
        error = FT_THROW( Invalid_Table );
        goto Fail;
      }

      if ( !decoder->bitmap_allocated )
      {
        error = tt_sbit_decoder_alloc_bitmap( decoder );
        if ( error )
          goto Fail;
      }

      error = loader( decoder, p, p_limit, x_pos, y_pos );
    }

  Fail:
    FT_FRAME_RELEASE( data );

  Exit:
    return error;
  }


  static FT_Error
  tt_sbit_decoder_load_image( TT_SBitDecoder  decoder,
                              FT_UInt         glyph_index,
                              FT_Int          x_pos,
                              FT_Int          y_pos )
  {
    /*
     *  First, we find the correct strike range that applies to this
     *  glyph index.
     */

    FT_Byte*  p          = decoder->eblc_base + decoder->strike_index_array;
    FT_Byte*  p_limit    = decoder->eblc_limit;
    FT_ULong  num_ranges = decoder->strike_index_count;
    FT_UInt   start, end, index_format, image_format;
    FT_ULong  image_start = 0, image_end = 0, image_offset;


    for ( ; num_ranges > 0; num_ranges-- )
    {
      start = FT_NEXT_USHORT( p );
      end   = FT_NEXT_USHORT( p );

      if ( glyph_index >= start && glyph_index <= end )
        goto FoundRange;

      p += 4;  /* ignore index offset */
    }
    goto NoBitmap;

  FoundRange:
    image_offset = FT_NEXT_ULONG( p );

    /* overflow check */
    p = decoder->eblc_base + decoder->strike_index_array;
    if ( image_offset > (FT_ULong)( p_limit - p ) )
      goto Failure;

    p += image_offset;
    if ( p + 8 > p_limit )
      goto NoBitmap;

    /* now find the glyph's location and extend within the ebdt table */
    index_format = FT_NEXT_USHORT( p );
    image_format = FT_NEXT_USHORT( p );
    image_offset = FT_NEXT_ULONG ( p );

    switch ( index_format )
    {
    case 1: /* 4-byte offsets relative to `image_offset' */
      p += 4 * ( glyph_index - start );
      if ( p + 8 > p_limit )
        goto NoBitmap;

      image_start = FT_NEXT_ULONG( p );
      image_end   = FT_NEXT_ULONG( p );

      if ( image_start == image_end )  /* missing glyph */
        goto NoBitmap;
      break;

    case 2: /* big metrics, constant image size */
      {
        FT_ULong  image_size;


        if ( p + 12 > p_limit )
          goto NoBitmap;

        image_size = FT_NEXT_ULONG( p );

        if ( tt_sbit_decoder_load_metrics( decoder, &p, p_limit, 1 ) )
          goto NoBitmap;

        image_start = image_size * ( glyph_index - start );
        image_end   = image_start + image_size;
      }
      break;

    case 3: /* 2-byte offsets relative to 'image_offset' */
      p += 2 * ( glyph_index - start );
      if ( p + 4 > p_limit )
        goto NoBitmap;

      image_start = FT_NEXT_USHORT( p );
      image_end   = FT_NEXT_USHORT( p );

      if ( image_start == image_end )  /* missing glyph */
        goto NoBitmap;
      break;

    case 4: /* sparse glyph array with (glyph,offset) pairs */
      {
        FT_ULong  mm, num_glyphs;


        if ( p + 4 > p_limit )
          goto NoBitmap;

        num_glyphs = FT_NEXT_ULONG( p );

        /* overflow check for p + ( num_glyphs + 1 ) * 4 */
        if ( num_glyphs > (FT_ULong)( ( ( p_limit - p ) >> 2 ) - 1 ) )
          goto NoBitmap;

        for ( mm = 0; mm < num_glyphs; mm++ )
        {
          FT_UInt  gindex = FT_NEXT_USHORT( p );


          if ( gindex == glyph_index )
          {
            image_start = FT_NEXT_USHORT( p );
            p          += 2;
            image_end   = FT_PEEK_USHORT( p );
            break;
          }
          p += 2;
        }

        if ( mm >= num_glyphs )
          goto NoBitmap;
      }
      break;

    case 5: /* constant metrics with sparse glyph codes */
    case 19:
      {
        FT_ULong  image_size, mm, num_glyphs;


        if ( p + 16 > p_limit )
          goto NoBitmap;

        image_size = FT_NEXT_ULONG( p );

        if ( tt_sbit_decoder_load_metrics( decoder, &p, p_limit, 1 ) )
          goto NoBitmap;

        num_glyphs = FT_NEXT_ULONG( p );

        /* overflow check for p + 2 * num_glyphs */
        if ( num_glyphs > (FT_ULong)( ( p_limit - p ) >> 1 ) )
          goto NoBitmap;

        for ( mm = 0; mm < num_glyphs; mm++ )
        {
          FT_UInt  gindex = FT_NEXT_USHORT( p );


          if ( gindex == glyph_index )
            break;
        }

        if ( mm >= num_glyphs )
          goto NoBitmap;

        image_start = image_size * mm;
        image_end   = image_start + image_size;
      }
      break;

    default:
      goto NoBitmap;
    }

    if ( image_start > image_end )
      goto NoBitmap;

    image_end  -= image_start;
    image_start = image_offset + image_start;

    FT_TRACE3(( "tt_sbit_decoder_load_image:"
                " found sbit (format %d) for glyph index %d\n",
                image_format, glyph_index ));

    return tt_sbit_decoder_load_bitmap( decoder,
                                        image_format,
                                        image_start,
                                        image_end,
                                        x_pos,
                                        y_pos );

  Failure:
    return FT_THROW( Invalid_Table );

  NoBitmap:
    FT_TRACE4(( "tt_sbit_decoder_load_image:"
                " no sbit found for glyph index %d\n", glyph_index ));

    return FT_THROW( Invalid_Argument );
  }


  static FT_Error
  tt_face_load_sbix_image( TT_Face              face,
                           FT_ULong             strike_index,
                           FT_UInt              glyph_index,
                           FT_Stream            stream,
                           FT_Bitmap           *map,
                           TT_SBit_MetricsRec  *metrics )
  {
    FT_UInt   sbix_pos, strike_offset, glyph_start, glyph_end;
    FT_ULong  table_size;
    FT_Int    originOffsetX, originOffsetY;
    FT_Tag    graphicType;
    FT_Int    recurse_depth = 0;

    FT_Error  error;
    FT_Byte*  p;

    FT_UNUSED( map );


    metrics->width  = 0;
    metrics->height = 0;

    p = face->sbit_table + 8 + 4 * strike_index;
    strike_offset = FT_NEXT_ULONG( p );

    error = face->goto_table( face, TTAG_sbix, stream, &table_size );
    if ( error )
      return error;
    sbix_pos = FT_STREAM_POS();

  retry:
    if ( glyph_index > (FT_UInt)face->root.num_glyphs )
      return FT_THROW( Invalid_Argument );

    if ( strike_offset >= table_size                          ||
         table_size - strike_offset < 4 + glyph_index * 4 + 8 )
      return FT_THROW( Invalid_File_Format );

    if ( FT_STREAM_SEEK( sbix_pos + strike_offset + 4 + glyph_index * 4 ) ||
         FT_FRAME_ENTER( 8 )                                              )
      return error;

    glyph_start = FT_GET_ULONG();
    glyph_end   = FT_GET_ULONG();

    FT_FRAME_EXIT();

    if ( glyph_start == glyph_end )
      return FT_THROW( Invalid_Argument );
    if ( glyph_start > glyph_end                ||
         glyph_end - glyph_start < 8            ||
         table_size - strike_offset < glyph_end )
      return FT_THROW( Invalid_File_Format );

    if ( FT_STREAM_SEEK( sbix_pos + strike_offset + glyph_start ) ||
         FT_FRAME_ENTER( glyph_end - glyph_start )                )
      return error;

    originOffsetX = FT_GET_SHORT();
    originOffsetY = FT_GET_SHORT();

    graphicType = FT_GET_TAG4();

    switch ( graphicType )
    {
    case FT_MAKE_TAG( 'd', 'u', 'p', 'e' ):
      if ( recurse_depth < 4 )
      {
        glyph_index = FT_GET_USHORT();
        FT_FRAME_EXIT();
        recurse_depth++;
        goto retry;
      }
      error = FT_THROW( Invalid_File_Format );
      break;

    case FT_MAKE_TAG( 'p', 'n', 'g', ' ' ):
#ifdef FT_CONFIG_OPTION_USE_PNG
      error = Load_SBit_Png( face->root.glyph,
                             0,
                             0,
                             32,
                             metrics,
                             stream->memory,
                             stream->cursor,
                             glyph_end - glyph_start - 8,
                             TRUE );
#else
      error = FT_THROW( Unimplemented_Feature );
#endif
      break;

    case FT_MAKE_TAG( 'j', 'p', 'g', ' ' ):
    case FT_MAKE_TAG( 't', 'i', 'f', 'f' ):
      error = FT_THROW( Unknown_File_Format );
      break;

    default:
      error = FT_THROW( Unimplemented_Feature );
      break;
    }

    FT_FRAME_EXIT();

    if ( !error )
    {
      FT_Short   abearing;
      FT_UShort  aadvance;


      tt_face_get_metrics( face, FALSE, glyph_index, &abearing, &aadvance );

      metrics->horiBearingX = originOffsetX;
      metrics->horiBearingY = -originOffsetY + metrics->height;
      metrics->horiAdvance  = aadvance * face->root.size->metrics.x_ppem /
                                face->header.Units_Per_EM;
    }

    return error;
  }

  FT_LOCAL( FT_Error )
  tt_face_load_sbit_image( TT_Face              face,
                           FT_ULong             strike_index,
                           FT_UInt              glyph_index,
                           FT_UInt              load_flags,
                           FT_Stream            stream,
                           FT_Bitmap           *map,
                           TT_SBit_MetricsRec  *metrics )
  {
    FT_Error  error = FT_Err_Ok;


    switch ( (FT_UInt)face->sbit_table_type )
    {
    case TT_SBIT_TABLE_TYPE_EBLC:
    case TT_SBIT_TABLE_TYPE_CBLC:
      {
        TT_SBitDecoderRec  decoder[1];


        error = tt_sbit_decoder_init( decoder, face, strike_index, metrics );
        if ( !error )
        {
          error = tt_sbit_decoder_load_image( decoder,
                                              glyph_index,
                                              0,
                                              0 );
          tt_sbit_decoder_done( decoder );
        }
      }
      break;

    case TT_SBIT_TABLE_TYPE_SBIX:
      error = tt_face_load_sbix_image( face,
                                       strike_index,
                                       glyph_index,
                                       stream,
                                       map,
                                       metrics );
      break;

    default:
      error = FT_THROW( Unknown_File_Format );
      break;
    }

    /* Flatten color bitmaps if color was not requested. */
    if ( !error                                &&
         !( load_flags & FT_LOAD_COLOR )       &&
         map->pixel_mode == FT_PIXEL_MODE_BGRA )
    {
      FT_Bitmap   new_map;
      FT_Library  library = face->root.glyph->library;


      FT_Bitmap_New( &new_map );

      /* Convert to 8bit grayscale. */
      error = FT_Bitmap_Convert( library, map, &new_map, 1 );
      if ( error )
        FT_Bitmap_Done( library, &new_map );
      else
      {
        map->pixel_mode = new_map.pixel_mode;
        map->pitch      = new_map.pitch;
        map->num_grays  = new_map.num_grays;

        ft_glyphslot_set_bitmap( face->root.glyph, new_map.buffer );
        face->root.glyph->internal->flags |= FT_GLYPH_OWN_BITMAP;
      }
    }

    return error;
  }


/* EOF */
#endif

#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
/***************************************************************************/
/*                                                                         */
/*  ttpost.c                                                               */
/*                                                                         */
/*    Postcript name table processing for TrueType and OpenType fonts      */
/*    (body).                                                              */
/*                                                                         */
/*  Copyright 1996-2003, 2006-2010, 2013 by                                */
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
  /* The post table is not completely loaded by the core engine.  This     */
  /* file loads the missing PS glyph names and implements an API to access */
  /* them.                                                                 */
  /*                                                                       */
  /*************************************************************************/


#include "ft2build.h"
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include "ttpost.h"

#include "sferrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttpost


  /* If this configuration macro is defined, we rely on the `PSNames' */
  /* module to grab the glyph names.                                  */

#ifdef FT_CONFIG_OPTION_POSTSCRIPT_NAMES


#include FT_SERVICE_POSTSCRIPT_CMAPS_H

#define MAC_NAME( x )  ( (FT_String*)psnames->macintosh_name( x ) )


#else /* FT_CONFIG_OPTION_POSTSCRIPT_NAMES */


   /* Otherwise, we ignore the `PSNames' module, and provide our own  */
   /* table of Mac names.  Thus, it is possible to build a version of */
   /* FreeType without the Type 1 driver & PSNames module.            */

#define MAC_NAME( x )  ( (FT_String*)tt_post_default_names[x] )

  /* the 258 default Mac PS glyph names */

  static const FT_String* const  tt_post_default_names[258] =
  {
    /*   0 */
    ".notdef", ".null", "CR", "space", "exclam",
    "quotedbl", "numbersign", "dollar", "percent", "ampersand",
    /*  10 */
    "quotesingle", "parenleft", "parenright", "asterisk", "plus",
    "comma", "hyphen", "period", "slash", "zero",
    /*  20 */
    "one", "two", "three", "four", "five",
    "six", "seven", "eight", "nine", "colon",
    /*  30 */
    "semicolon", "less", "equal", "greater", "question",
    "at", "A", "B", "C", "D",
    /*  40 */
    "E", "F", "G", "H", "I",
    "J", "K", "L", "M", "N",
    /*  50 */
    "O", "P", "Q", "R", "S",
    "T", "U", "V", "W", "X",
    /*  60 */
    "Y", "Z", "bracketleft", "backslash", "bracketright",
    "asciicircum", "underscore", "grave", "a", "b",
    /*  70 */
    "c", "d", "e", "f", "g",
    "h", "i", "j", "k", "l",
    /*  80 */
    "m", "n", "o", "p", "q",
    "r", "s", "t", "u", "v",
    /*  90 */
    "w", "x", "y", "z", "braceleft",
    "bar", "braceright", "asciitilde", "Adieresis", "Aring",
    /* 100 */
    "Ccedilla", "Eacute", "Ntilde", "Odieresis", "Udieresis",
    "aacute", "agrave", "acircumflex", "adieresis", "atilde",
    /* 110 */
    "aring", "ccedilla", "eacute", "egrave", "ecircumflex",
    "edieresis", "iacute", "igrave", "icircumflex", "idieresis",
    /* 120 */
    "ntilde", "oacute", "ograve", "ocircumflex", "odieresis",
    "otilde", "uacute", "ugrave", "ucircumflex", "udieresis",
    /* 130 */
    "dagger", "degree", "cent", "sterling", "section",
    "bullet", "paragraph", "germandbls", "registered", "copyright",
    /* 140 */
    "trademark", "acute", "dieresis", "notequal", "AE",
    "Oslash", "infinity", "plusminus", "lessequal", "greaterequal",
    /* 150 */
    "yen", "mu", "partialdiff", "summation", "product",
    "pi", "integral", "ordfeminine", "ordmasculine", "Omega",
    /* 160 */
    "ae", "oslash", "questiondown", "exclamdown", "logicalnot",
    "radical", "florin", "approxequal", "Delta", "guillemotleft",
    /* 170 */
    "guillemotright", "ellipsis", "nbspace", "Agrave", "Atilde",
    "Otilde", "OE", "oe", "endash", "emdash",
    /* 180 */
    "quotedblleft", "quotedblright", "quoteleft", "quoteright", "divide",
    "lozenge", "ydieresis", "Ydieresis", "fraction", "currency",
    /* 190 */
    "guilsinglleft", "guilsinglright", "fi", "fl", "daggerdbl",
    "periodcentered", "quotesinglbase", "quotedblbase", "perthousand", "Acircumflex",
    /* 200 */
    "Ecircumflex", "Aacute", "Edieresis", "Egrave", "Iacute",
    "Icircumflex", "Idieresis", "Igrave", "Oacute", "Ocircumflex",
    /* 210 */
    "apple", "Ograve", "Uacute", "Ucircumflex", "Ugrave",
    "dotlessi", "circumflex", "tilde", "macron", "breve",
    /* 220 */
    "dotaccent", "ring", "cedilla", "hungarumlaut", "ogonek",
    "caron", "Lslash", "lslash", "Scaron", "scaron",
    /* 230 */
    "Zcaron", "zcaron", "brokenbar", "Eth", "eth",
    "Yacute", "yacute", "Thorn", "thorn", "minus",
    /* 240 */
    "multiply", "onesuperior", "twosuperior", "threesuperior", "onehalf",
    "onequarter", "threequarters", "franc", "Gbreve", "gbreve",
    /* 250 */
    "Idot", "Scedilla", "scedilla", "Cacute", "cacute",
    "Ccaron", "ccaron", "dmacron",
  };


#endif /* FT_CONFIG_OPTION_POSTSCRIPT_NAMES */


  static FT_Error
  load_format_20( TT_Face    face,
                  FT_Stream  stream,
                  FT_Long    post_limit )
  {
    FT_Memory   memory = stream->memory;
    FT_Error    error;

    FT_Int      num_glyphs;
    FT_UShort   num_names;

    FT_UShort*  glyph_indices = 0;
    FT_Char**   name_strings  = 0;


    if ( FT_READ_USHORT( num_glyphs ) )
      goto Exit;

    /* UNDOCUMENTED!  The number of glyphs in this table can be smaller */
    /* than the value in the maxp table (cf. cyberbit.ttf).             */

    /* There already exist fonts which have more than 32768 glyph names */
    /* in this table, so the test for this threshold has been dropped.  */

    if ( num_glyphs > face->max_profile.numGlyphs )
    {
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    /* load the indices */
    {
      FT_Int  n;


      if ( FT_NEW_ARRAY ( glyph_indices, num_glyphs ) ||
           FT_FRAME_ENTER( num_glyphs * 2L )          )
        goto Fail;

      for ( n = 0; n < num_glyphs; n++ )
        glyph_indices[n] = FT_GET_USHORT();

      FT_FRAME_EXIT();
    }

    /* compute number of names stored in table */
    {
      FT_Int  n;


      num_names = 0;

      for ( n = 0; n < num_glyphs; n++ )
      {
        FT_Int  idx;


        idx = glyph_indices[n];
        if ( idx >= 258 )
        {
          idx -= 257;
          if ( idx > num_names )
            num_names = (FT_UShort)idx;
        }
      }
    }

    /* now load the name strings */
    {
      FT_UShort  n;


      if ( FT_NEW_ARRAY( name_strings, num_names ) )
        goto Fail;

      for ( n = 0; n < num_names; n++ )
      {
        FT_UInt  len;


        if ( FT_STREAM_POS() >= post_limit )
          break;
        else
        {
          FT_TRACE6(( "load_format_20: %d byte left in post table\n",
                      post_limit - FT_STREAM_POS() ));

          if ( FT_READ_BYTE( len ) )
            goto Fail1;
        }

        if ( (FT_Int)len > post_limit                   ||
             FT_STREAM_POS() > post_limit - (FT_Int)len )
        {
          FT_ERROR(( "load_format_20:"
                     " exceeding string length (%d),"
                     " truncating at end of post table (%d byte left)\n",
                     len, post_limit - FT_STREAM_POS() ));
          len = FT_MAX( 0, post_limit - FT_STREAM_POS() );
        }

        if ( FT_NEW_ARRAY( name_strings[n], len + 1 ) ||
             FT_STREAM_READ( name_strings[n], len   ) )
          goto Fail1;

        name_strings[n][len] = '\0';
      }

      if ( n < num_names )
      {
        FT_ERROR(( "load_format_20:"
                   " all entries in post table are already parsed,"
                   " using NULL names for gid %d - %d\n",
                    n, num_names - 1 ));
        for ( ; n < num_names; n++ )
          if ( FT_NEW_ARRAY( name_strings[n], 1 ) )
            goto Fail1;
          else
            name_strings[n][0] = '\0';
      }
    }

    /* all right, set table fields and exit successfully */
    {
      TT_Post_20  table = &face->postscript_names.names.format_20;


      table->num_glyphs    = (FT_UShort)num_glyphs;
      table->num_names     = (FT_UShort)num_names;
      table->glyph_indices = glyph_indices;
      table->glyph_names   = name_strings;
    }
    return FT_Err_Ok;

  Fail1:
    {
      FT_UShort  n;


      for ( n = 0; n < num_names; n++ )
        FT_FREE( name_strings[n] );
    }

  Fail:
    FT_FREE( name_strings );
    FT_FREE( glyph_indices );

  Exit:
    return error;
  }


  static FT_Error
  load_format_25( TT_Face    face,
                  FT_Stream  stream,
                  FT_Long    post_limit )
  {
    FT_Memory  memory = stream->memory;
    FT_Error   error;

    FT_Int     num_glyphs;
    FT_Char*   offset_table = 0;

    FT_UNUSED( post_limit );


    /* UNDOCUMENTED!  This value appears only in the Apple TT specs. */
    if ( FT_READ_USHORT( num_glyphs ) )
      goto Exit;

    /* check the number of glyphs */
    if ( num_glyphs > face->max_profile.numGlyphs || num_glyphs > 258 )
    {
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    if ( FT_NEW_ARRAY( offset_table, num_glyphs )   ||
         FT_STREAM_READ( offset_table, num_glyphs ) )
      goto Fail;

    /* now check the offset table */
    {
      FT_Int  n;


      for ( n = 0; n < num_glyphs; n++ )
      {
        FT_Long  idx = (FT_Long)n + offset_table[n];


        if ( idx < 0 || idx > num_glyphs )
        {
          error = FT_THROW( Invalid_File_Format );
          goto Fail;
        }
      }
    }

    /* OK, set table fields and exit successfully */
    {
      TT_Post_25  table = &face->postscript_names.names.format_25;


      table->num_glyphs = (FT_UShort)num_glyphs;
      table->offsets    = offset_table;
    }

    return FT_Err_Ok;

  Fail:
    FT_FREE( offset_table );

  Exit:
    return error;
  }


  static FT_Error
  load_post_names( TT_Face  face )
  {
    FT_Stream  stream;
    FT_Error   error;
    FT_Fixed   format;
    FT_ULong   post_len;
    FT_Long    post_limit;


    /* get a stream for the face's resource */
    stream = face->root.stream;

    /* seek to the beginning of the PS names table */
    error = face->goto_table( face, TTAG_post, stream, &post_len );
    if ( error )
      goto Exit;

    post_limit = FT_STREAM_POS() + post_len;

    format = face->postscript.FormatType;

    /* go to beginning of subtable */
    if ( FT_STREAM_SKIP( 32 ) )
      goto Exit;

    /* now read postscript table */
    if ( format == 0x00020000L )
      error = load_format_20( face, stream, post_limit );
    else if ( format == 0x00028000L )
      error = load_format_25( face, stream, post_limit );
    else
      error = FT_THROW( Invalid_File_Format );

    face->postscript_names.loaded = 1;

  Exit:
    return error;
  }


  FT_LOCAL_DEF( void )
  tt_face_free_ps_names( TT_Face  face )
  {
    FT_Memory      memory = face->root.memory;
    TT_Post_Names  names  = &face->postscript_names;
    FT_Fixed       format;


    if ( names->loaded )
    {
      format = face->postscript.FormatType;

      if ( format == 0x00020000L )
      {
        TT_Post_20  table = &names->names.format_20;
        FT_UShort   n;


        FT_FREE( table->glyph_indices );
        table->num_glyphs = 0;

        for ( n = 0; n < table->num_names; n++ )
          FT_FREE( table->glyph_names[n] );

        FT_FREE( table->glyph_names );
        table->num_names = 0;
      }
      else if ( format == 0x00028000L )
      {
        TT_Post_25  table = &names->names.format_25;


        FT_FREE( table->offsets );
        table->num_glyphs = 0;
      }
    }
    names->loaded = 0;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_get_ps_name                                                */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Get the PostScript glyph name of a glyph.                          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face   :: A handle to the parent face.                             */
  /*                                                                       */
  /*    idx    :: The glyph index.                                         */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    PSname :: The address of a string pointer.  Will be NULL in case   */
  /*              of error, otherwise it is a pointer to the glyph name.   */
  /*                                                                       */
  /*              You must not modify the returned string!                 */
  /*                                                                       */
  /* <Output>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_get_ps_name( TT_Face      face,
                       FT_UInt      idx,
                       FT_String**  PSname )
  {
    FT_Error       error;
    TT_Post_Names  names;
    FT_Fixed       format;

#ifdef FT_CONFIG_OPTION_POSTSCRIPT_NAMES
    FT_Service_PsCMaps  psnames;
#endif


    if ( !face )
      return FT_THROW( Invalid_Face_Handle );

    if ( idx >= (FT_UInt)face->max_profile.numGlyphs )
      return FT_THROW( Invalid_Glyph_Index );

#ifdef FT_CONFIG_OPTION_POSTSCRIPT_NAMES
    psnames = (FT_Service_PsCMaps)face->psnames;
    if ( !psnames )
      return FT_THROW( Unimplemented_Feature );
#endif

    names = &face->postscript_names;

    /* `.notdef' by default */
    *PSname = MAC_NAME( 0 );

    format = face->postscript.FormatType;

    if ( format == 0x00010000L )
    {
      if ( idx < 258 )                    /* paranoid checking */
        *PSname = MAC_NAME( idx );
    }
    else if ( format == 0x00020000L )
    {
      TT_Post_20  table = &names->names.format_20;


      if ( !names->loaded )
      {
        error = load_post_names( face );
        if ( error )
          goto End;
      }

      if ( idx < (FT_UInt)table->num_glyphs )
      {
        FT_UShort  name_index = table->glyph_indices[idx];


        if ( name_index < 258 )
          *PSname = MAC_NAME( name_index );
        else
          *PSname = (FT_String*)table->glyph_names[name_index - 258];
      }
    }
    else if ( format == 0x00028000L )
    {
      TT_Post_25  table = &names->names.format_25;


      if ( !names->loaded )
      {
        error = load_post_names( face );
        if ( error )
          goto End;
      }

      if ( idx < (FT_UInt)table->num_glyphs )    /* paranoid checking */
      {
        idx    += table->offsets[idx];
        *PSname = MAC_NAME( idx );
      }
    }

    /* nothing to do for format == 0x00030000L */

  End:
    return FT_Err_Ok;
  }


/* END */
#endif

#ifdef TT_CONFIG_OPTION_BDF
/***************************************************************************/
/*                                                                         */
/*  ttbdf.c                                                                */
/*                                                                         */
/*    TrueType and OpenType embedded BDF properties (body).                */
/*                                                                         */
/*  Copyright 2005, 2006, 2010, 2013 by                                    */
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
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include "ttbdf.h"

#include "sferrors.h"


#ifdef TT_CONFIG_OPTION_BDF

  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttbdf


  FT_LOCAL_DEF( void )
  tt_face_free_bdf_props( TT_Face  face )
  {
    TT_BDF  bdf = &face->bdf;


    if ( bdf->loaded )
    {
      FT_Stream  stream = FT_FACE(face)->stream;


      if ( bdf->table != NULL )
        FT_FRAME_RELEASE( bdf->table );

      bdf->table_end    = NULL;
      bdf->strings      = NULL;
      bdf->strings_size = 0;
    }
  }


  static FT_Error
  tt_face_load_bdf_props( TT_Face    face,
                          FT_Stream  stream )
  {
    TT_BDF    bdf = &face->bdf;
    FT_ULong  length;
    FT_Error  error;


    FT_ZERO( bdf );

    error = tt_face_goto_table( face, TTAG_BDF, stream, &length );
    if ( error                                  ||
         length < 8                             ||
         FT_FRAME_EXTRACT( length, bdf->table ) )
    {
      error = FT_THROW( Invalid_Table );
      goto Exit;
    }

    bdf->table_end = bdf->table + length;

    {
      FT_Byte*   p           = bdf->table;
      FT_UInt    version     = FT_NEXT_USHORT( p );
      FT_UInt    num_strikes = FT_NEXT_USHORT( p );
      FT_ULong   strings     = FT_NEXT_ULONG ( p );
      FT_UInt    count;
      FT_Byte*   strike;


      if ( version != 0x0001                 ||
           strings < 8                       ||
           ( strings - 8 ) / 4 < num_strikes ||
           strings + 1 > length              )
      {
        goto BadTable;
      }

      bdf->num_strikes  = num_strikes;
      bdf->strings      = bdf->table + strings;
      bdf->strings_size = length - strings;

      count  = bdf->num_strikes;
      p      = bdf->table + 8;
      strike = p + count * 4;


      for ( ; count > 0; count-- )
      {
        FT_UInt  num_items = FT_PEEK_USHORT( p + 2 );

        /*
         *  We don't need to check the value sets themselves, since this
         *  is done later.
         */
        strike += 10 * num_items;

        p += 4;
      }

      if ( strike > bdf->strings )
        goto BadTable;
    }

    bdf->loaded = 1;

  Exit:
    return error;

  BadTable:
    FT_FRAME_RELEASE( bdf->table );
    FT_ZERO( bdf );
    error = FT_THROW( Invalid_Table );
    goto Exit;
  }


  FT_LOCAL_DEF( FT_Error )
  tt_face_find_bdf_prop( TT_Face           face,
                         const char*       property_name,
                         BDF_PropertyRec  *aprop )
  {
    TT_BDF     bdf   = &face->bdf;
    FT_Size    size  = FT_FACE(face)->size;
    FT_Error   error = FT_Err_Ok;
    FT_Byte*   p;
    FT_UInt    count;
    FT_Byte*   strike;
    FT_Offset  property_len;


    aprop->type = BDF_PROPERTY_TYPE_NONE;

    if ( bdf->loaded == 0 )
    {
      error = tt_face_load_bdf_props( face, FT_FACE( face )->stream );
      if ( error )
        goto Exit;
    }

    count  = bdf->num_strikes;
    p      = bdf->table + 8;
    strike = p + 4 * count;

    error = FT_ERR( Invalid_Argument );

    if ( size == NULL || property_name == NULL )
      goto Exit;

    property_len = ft_strlen( property_name );
    if ( property_len == 0 )
      goto Exit;

    for ( ; count > 0; count-- )
    {
      FT_UInt  _ppem  = FT_NEXT_USHORT( p );
      FT_UInt  _count = FT_NEXT_USHORT( p );

      if ( _ppem == size->metrics.y_ppem )
      {
        count = _count;
        goto FoundStrike;
      }

      strike += 10 * _count;
    }
    goto Exit;

  FoundStrike:
    p = strike;
    for ( ; count > 0; count-- )
    {
      FT_UInt  type = FT_PEEK_USHORT( p + 4 );

      if ( ( type & 0x10 ) != 0 )
      {
        FT_UInt32  name_offset = FT_PEEK_ULONG( p     );
        FT_UInt32  value       = FT_PEEK_ULONG( p + 6 );

        /* be a bit paranoid for invalid entries here */
        if ( name_offset < bdf->strings_size                    &&
             property_len < bdf->strings_size - name_offset     &&
             ft_strncmp( property_name,
                         (const char*)bdf->strings + name_offset,
                         bdf->strings_size - name_offset ) == 0 )
        {
          switch ( type & 0x0F )
          {
          case 0x00:  /* string */
          case 0x01:  /* atoms */
            /* check that the content is really 0-terminated */
            if ( value < bdf->strings_size &&
                 ft_memchr( bdf->strings + value, 0, bdf->strings_size ) )
            {
              aprop->type   = BDF_PROPERTY_TYPE_ATOM;
              aprop->u.atom = (const char*)bdf->strings + value;
              error         = FT_Err_Ok;
              goto Exit;
            }
            break;

          case 0x02:
            aprop->type      = BDF_PROPERTY_TYPE_INTEGER;
            aprop->u.integer = (FT_Int32)value;
            error            = FT_Err_Ok;
            goto Exit;

          case 0x03:
            aprop->type       = BDF_PROPERTY_TYPE_CARDINAL;
            aprop->u.cardinal = value;
            error             = FT_Err_Ok;
            goto Exit;

          default:
            ;
          }
        }
      }
      p += 10;
    }

  Exit:
    return error;
  }

#endif /* TT_CONFIG_OPTION_BDF */


/* END */
#endif

/* END */
