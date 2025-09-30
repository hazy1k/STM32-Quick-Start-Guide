/***************************************************************************/
/*                                                                         */
/*  cff.c                                                                  */
/*                                                                         */
/*    FreeType OpenType driver component (body only).                      */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2013 by                                     */
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
/*  cffpic.c                                                               */
/*                                                                         */
/*    The FreeType position independent code services for cff module.      */
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
#include "cffcmap.h"
#include "cffpic.h"
#include "cfferrs.h"


#ifdef FT_CONFIG_OPTION_PIC

  /* forward declaration of PIC init functions from cffdrivr.c */
  FT_Error
  FT_Create_Class_cff_services( FT_Library           library,
                                FT_ServiceDescRec**  output_class );
  void
  FT_Destroy_Class_cff_services( FT_Library          library,
                                 FT_ServiceDescRec*  clazz );
  void
  FT_Init_Class_cff_service_ps_info( FT_Library             library,
                                     FT_Service_PsInfoRec*  clazz );
  void
  FT_Init_Class_cff_service_glyph_dict( FT_Library                library,
                                        FT_Service_GlyphDictRec*  clazz );
  void
  FT_Init_Class_cff_service_ps_name( FT_Library                 library,
                                     FT_Service_PsFontNameRec*  clazz );
  void
  FT_Init_Class_cff_service_get_cmap_info( FT_Library              library,
                                           FT_Service_TTCMapsRec*  clazz );
  void
  FT_Init_Class_cff_service_cid_info( FT_Library          library,
                                      FT_Service_CIDRec*  clazz );

  /* forward declaration of PIC init functions from cffparse.c */
  FT_Error
  FT_Create_Class_cff_field_handlers( FT_Library           library,
                                      CFF_Field_Handler**  output_class );
  void
  FT_Destroy_Class_cff_field_handlers( FT_Library          library,
                                       CFF_Field_Handler*  clazz );


  void
  cff_driver_class_pic_free( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Memory          memory        = library->memory;


    if ( pic_container->cff )
    {
      CffModulePIC*  container = (CffModulePIC*)pic_container->cff;


      if ( container->cff_services )
        FT_Destroy_Class_cff_services( library,
                                       container->cff_services );
      container->cff_services = NULL;
      if ( container->cff_field_handlers )
        FT_Destroy_Class_cff_field_handlers(
          library, container->cff_field_handlers );
      container->cff_field_handlers = NULL;
      FT_FREE( container );
      pic_container->cff = NULL;
    }
  }


  FT_Error
  cff_driver_class_pic_init( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Error           error         = FT_Err_Ok;
    CffModulePIC*      container     = NULL;
    FT_Memory          memory        = library->memory;


    /* allocate pointer, clear and set global container pointer */
    if ( FT_ALLOC ( container, sizeof ( *container ) ) )
      return error;
    FT_MEM_SET( container, 0, sizeof ( *container ) );
    pic_container->cff = container;

    /* initialize pointer table -                       */
    /* this is how the module usually expects this data */
    error = FT_Create_Class_cff_services( library,
                                          &container->cff_services );
    if ( error )
      goto Exit;

    error = FT_Create_Class_cff_field_handlers(
              library, &container->cff_field_handlers );
    if ( error )
      goto Exit;

    FT_Init_Class_cff_service_ps_info(
      library, &container->cff_service_ps_info );
    FT_Init_Class_cff_service_glyph_dict(
      library, &container->cff_service_glyph_dict );
    FT_Init_Class_cff_service_ps_name(
      library, &container->cff_service_ps_name );
    FT_Init_Class_cff_service_get_cmap_info(
      library, &container->cff_service_get_cmap_info );
    FT_Init_Class_cff_service_cid_info(
      library, &container->cff_service_cid_info );
    FT_Init_Class_cff_cmap_encoding_class_rec(
      library, &container->cff_cmap_encoding_class_rec );
    FT_Init_Class_cff_cmap_unicode_class_rec(
      library, &container->cff_cmap_unicode_class_rec );

  Exit:
    if ( error )
      cff_driver_class_pic_free( library );
    return error;
  }

#endif /* FT_CONFIG_OPTION_PIC */


/* END */
/***************************************************************************/
/*                                                                         */
/*  cffdrivr.c                                                             */
/*                                                                         */
/*    OpenType font driver implementation (body).                          */
/*                                                                         */
/*  Copyright 1996-2013 by                                                 */
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
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_SFNT_H
#include FT_SERVICE_CID_H
#include FT_SERVICE_POSTSCRIPT_INFO_H
#include FT_SERVICE_POSTSCRIPT_NAME_H
#include FT_SERVICE_TT_CMAP_H

#include "cffdrivr.h"
#include "cffgload.h"
#include "cffload.h"
#include "cffcmap.h"
#include "cffparse.h"

#include "cfferrs.h"
#include "cffpic.h"

#include FT_SERVICE_XFREE86_NAME_H
#include FT_SERVICE_GLYPH_DICT_H
#include FT_SERVICE_PROPERTIES_H
#include FT_CFF_DRIVER_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cffdriver


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                          F A C E S                              ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


#undef  PAIR_TAG
#define PAIR_TAG( left, right )  ( ( (FT_ULong)left << 16 ) | \
                                     (FT_ULong)right        )


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_get_kerning                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A driver method used to return the kerning vector between two      */
  /*    glyphs of the same face.                                           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face        :: A handle to the source face object.                 */
  /*                                                                       */
  /*    left_glyph  :: The index of the left glyph in the kern pair.       */
  /*                                                                       */
  /*    right_glyph :: The index of the right glyph in the kern pair.      */
  /*                                                                       */
  /* <Output>                                                              */
  /*    kerning     :: The kerning vector.  This is in font units for      */
  /*                   scalable formats, and in pixels for fixed-sizes     */
  /*                   formats.                                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Only horizontal layouts (left-to-right & right-to-left) are        */
  /*    supported by this function.  Other layouts, or more sophisticated  */
  /*    kernings, are out of scope of this method (the basic driver        */
  /*    interface is meant to be simple).                                  */
  /*                                                                       */
  /*    They can be implemented by format-specific interfaces.             */
  /*                                                                       */
  FT_CALLBACK_DEF( FT_Error )
  cff_get_kerning( FT_Face     ttface,          /* TT_Face */
                   FT_UInt     left_glyph,
                   FT_UInt     right_glyph,
                   FT_Vector*  kerning )
  {
    TT_Face       face = (TT_Face)ttface;
    SFNT_Service  sfnt = (SFNT_Service)face->sfnt;


    kerning->x = 0;
    kerning->y = 0;

    if ( sfnt )
      kerning->x = sfnt->get_kerning( face, left_glyph, right_glyph );

    return FT_Err_Ok;
  }


#undef PAIR_TAG


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_glyph_load                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A driver method used to load a glyph within a given glyph slot.    */
  /*                                                                       */
  /* <Input>                                                               */
  /*    slot        :: A handle to the target slot object where the glyph  */
  /*                   will be loaded.                                     */
  /*                                                                       */
  /*    size        :: A handle to the source face size at which the glyph */
  /*                   must be scaled, loaded, etc.                        */
  /*                                                                       */
  /*    glyph_index :: The index of the glyph in the font file.            */
  /*                                                                       */
  /*    load_flags  :: A flag indicating what to load for this glyph.  The */
  /*                   FT_LOAD_??? constants can be used to control the    */
  /*                   glyph loading process (e.g., whether the outline    */
  /*                   should be scaled, whether to load bitmaps or not,   */
  /*                   whether to hint the outline, etc).                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_CALLBACK_DEF( FT_Error )
  cff_glyph_load( FT_GlyphSlot  cffslot,      /* CFF_GlyphSlot */
                  FT_Size       cffsize,      /* CFF_Size      */
                  FT_UInt       glyph_index,
                  FT_Int32      load_flags )
  {
    FT_Error       error;
    CFF_GlyphSlot  slot = (CFF_GlyphSlot)cffslot;
    CFF_Size       size = (CFF_Size)cffsize;


    if ( !slot )
      return FT_THROW( Invalid_Slot_Handle );

    FT_TRACE1(( "cff_glyph_load: glyph index %d\n", glyph_index ));

    /* check whether we want a scaled outline or bitmap */
    if ( !size )
      load_flags |= FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING;

    /* reset the size object if necessary */
    if ( load_flags & FT_LOAD_NO_SCALE )
      size = NULL;

    if ( size )
    {
      /* these two objects must have the same parent */
      if ( cffsize->face != cffslot->face )
        return FT_THROW( Invalid_Face_Handle );
    }

    /* now load the glyph outline if necessary */
    error = cff_slot_load( slot, size, glyph_index, load_flags );

    /* force drop-out mode to 2 - irrelevant now */
    /* slot->outline.dropout_mode = 2; */

    return error;
  }


  FT_CALLBACK_DEF( FT_Error )
  cff_get_advances( FT_Face    face,
                    FT_UInt    start,
                    FT_UInt    count,
                    FT_Int32   flags,
                    FT_Fixed*  advances )
  {
    FT_UInt       nn;
    FT_Error      error = FT_Err_Ok;
    FT_GlyphSlot  slot  = face->glyph;


    flags |= (FT_UInt32)FT_LOAD_ADVANCE_ONLY;

    for ( nn = 0; nn < count; nn++ )
    {
      error = cff_glyph_load( slot, face->size, start + nn, flags );
      if ( error )
        break;

      advances[nn] = ( flags & FT_LOAD_VERTICAL_LAYOUT )
                     ? slot->linearVertAdvance
                     : slot->linearHoriAdvance;
    }

    return error;
  }


  /*
   *  GLYPH DICT SERVICE
   *
   */

  static FT_Error
  cff_get_glyph_name( CFF_Face    face,
                      FT_UInt     glyph_index,
                      FT_Pointer  buffer,
                      FT_UInt     buffer_max )
  {
    CFF_Font    font   = (CFF_Font)face->extra.data;
    FT_String*  gname;
    FT_UShort   sid;
    FT_Error    error;


    if ( !font->psnames )
    {
      FT_ERROR(( "cff_get_glyph_name:"
                 " cannot get glyph name from CFF & CEF fonts\n"
                 "                   "
                 " without the `PSNames' module\n" ));
      error = FT_THROW( Missing_Module );
      goto Exit;
    }

    /* first, locate the sid in the charset table */
    sid = font->charset.sids[glyph_index];

    /* now, lookup the name itself */
    gname = cff_index_get_sid_string( font, sid );

    if ( gname )
      FT_STRCPYN( buffer, gname, buffer_max );

    error = FT_Err_Ok;

  Exit:
    return error;
  }


  static FT_UInt
  cff_get_name_index( CFF_Face    face,
                      FT_String*  glyph_name )
  {
    CFF_Font            cff;
    CFF_Charset         charset;
    FT_Service_PsCMaps  psnames;
    FT_String*          name;
    FT_UShort           sid;
    FT_UInt             i;


    cff     = (CFF_FontRec *)face->extra.data;
    charset = &cff->charset;

    FT_FACE_FIND_GLOBAL_SERVICE( face, psnames, POSTSCRIPT_CMAPS );
    if ( !psnames )
      return 0;

    for ( i = 0; i < cff->num_glyphs; i++ )
    {
      sid = charset->sids[i];

      if ( sid > 390 )
        name = cff_index_get_string( cff, sid - 391 );
      else
        name = (FT_String *)psnames->adobe_std_strings( sid );

      if ( !name )
        continue;

      if ( !ft_strcmp( glyph_name, name ) )
        return i;
    }

    return 0;
  }


  FT_DEFINE_SERVICE_GLYPHDICTREC(
    cff_service_glyph_dict,
    (FT_GlyphDict_GetNameFunc)  cff_get_glyph_name,
    (FT_GlyphDict_NameIndexFunc)cff_get_name_index
  )


  /*
   *  POSTSCRIPT INFO SERVICE
   *
   */

  static FT_Int
  cff_ps_has_glyph_names( FT_Face  face )
  {
    return ( face->face_flags & FT_FACE_FLAG_GLYPH_NAMES ) > 0;
  }


  static FT_Error
  cff_ps_get_font_info( CFF_Face         face,
                        PS_FontInfoRec*  afont_info )
  {
    CFF_Font  cff   = (CFF_Font)face->extra.data;
    FT_Error  error = FT_Err_Ok;


    if ( cff && cff->font_info == NULL )
    {
      CFF_FontRecDict  dict   = &cff->top_font.font_dict;
      PS_FontInfoRec  *font_info = NULL;
      FT_Memory        memory = face->root.memory;


      if ( FT_ALLOC( font_info, sizeof ( *font_info ) ) )
        goto Fail;

      font_info->version     = cff_index_get_sid_string( cff,
                                                         dict->version );
      font_info->notice      = cff_index_get_sid_string( cff,
                                                         dict->notice );
      font_info->full_name   = cff_index_get_sid_string( cff,
                                                         dict->full_name );
      font_info->family_name = cff_index_get_sid_string( cff,
                                                         dict->family_name );
      font_info->weight      = cff_index_get_sid_string( cff,
                                                         dict->weight );
      font_info->italic_angle        = dict->italic_angle;
      font_info->is_fixed_pitch      = dict->is_fixed_pitch;
      font_info->underline_position  = (FT_Short)dict->underline_position;
      font_info->underline_thickness = (FT_Short)dict->underline_thickness;

      cff->font_info = font_info;
    }

    if ( cff )
      *afont_info = *cff->font_info;

  Fail:
    return error;
  }


  FT_DEFINE_SERVICE_PSINFOREC(
    cff_service_ps_info,
    (PS_GetFontInfoFunc)   cff_ps_get_font_info,
    (PS_GetFontExtraFunc)  NULL,
    (PS_HasGlyphNamesFunc) cff_ps_has_glyph_names,
    (PS_GetFontPrivateFunc)NULL,        /* unsupported with CFF fonts */
    (PS_GetFontValueFunc)  NULL         /* not implemented            */
  )


  /*
   *  POSTSCRIPT NAME SERVICE
   *
   */

  static const char*
  cff_get_ps_name( CFF_Face  face )
  {
    CFF_Font  cff = (CFF_Font)face->extra.data;


    return (const char*)cff->font_name;
  }


  FT_DEFINE_SERVICE_PSFONTNAMEREC(
    cff_service_ps_name,
    (FT_PsName_GetFunc)cff_get_ps_name
  )


  /*
   * TT CMAP INFO
   *
   * If the charmap is a synthetic Unicode encoding cmap or
   * a Type 1 standard (or expert) encoding cmap, hide TT CMAP INFO
   * service defined in SFNT module.
   *
   * Otherwise call the service function in the sfnt module.
   *
   */
  static FT_Error
  cff_get_cmap_info( FT_CharMap    charmap,
                     TT_CMapInfo  *cmap_info )
  {
    FT_CMap   cmap  = FT_CMAP( charmap );
    FT_Error  error = FT_Err_Ok;

    FT_Face     face    = FT_CMAP_FACE( cmap );
    FT_Library  library = FT_FACE_LIBRARY( face );


    cmap_info->language = 0;
    cmap_info->format   = 0;

    if ( cmap->clazz != &CFF_CMAP_ENCODING_CLASS_REC_GET &&
         cmap->clazz != &CFF_CMAP_UNICODE_CLASS_REC_GET  )
    {
      FT_Module           sfnt    = FT_Get_Module( library, "sfnt" );
      FT_Service_TTCMaps  service =
        (FT_Service_TTCMaps)ft_module_get_service( sfnt,
                                                   FT_SERVICE_ID_TT_CMAP );


      if ( service && service->get_cmap_info )
        error = service->get_cmap_info( charmap, cmap_info );
    }

    return error;
  }


  FT_DEFINE_SERVICE_TTCMAPSREC(
    cff_service_get_cmap_info,
    (TT_CMap_Info_GetFunc)cff_get_cmap_info
  )


  /*
   *  CID INFO SERVICE
   *
   */
  static FT_Error
  cff_get_ros( CFF_Face      face,
               const char*  *registry,
               const char*  *ordering,
               FT_Int       *supplement )
  {
    FT_Error  error = FT_Err_Ok;
    CFF_Font  cff   = (CFF_Font)face->extra.data;


    if ( cff )
    {
      CFF_FontRecDict  dict = &cff->top_font.font_dict;


      if ( dict->cid_registry == 0xFFFFU )
      {
        error = FT_THROW( Invalid_Argument );
        goto Fail;
      }

      if ( registry )
      {
        if ( cff->registry == NULL )
          cff->registry = cff_index_get_sid_string( cff,
                                                    dict->cid_registry );
        *registry = cff->registry;
      }

      if ( ordering )
      {
        if ( cff->ordering == NULL )
          cff->ordering = cff_index_get_sid_string( cff,
                                                    dict->cid_ordering );
        *ordering = cff->ordering;
      }

      /*
       * XXX: According to Adobe TechNote #5176, the supplement in CFF
       *      can be a real number. We truncate it to fit public API
       *      since freetype-2.3.6.
       */
      if ( supplement )
      {
        if ( dict->cid_supplement < FT_INT_MIN ||
             dict->cid_supplement > FT_INT_MAX )
          FT_TRACE1(( "cff_get_ros: too large supplement %d is truncated\n",
                      dict->cid_supplement ));
        *supplement = (FT_Int)dict->cid_supplement;
      }
    }

  Fail:
    return error;
  }


  static FT_Error
  cff_get_is_cid( CFF_Face  face,
                  FT_Bool  *is_cid )
  {
    FT_Error  error = FT_Err_Ok;
    CFF_Font  cff   = (CFF_Font)face->extra.data;


    *is_cid = 0;

    if ( cff )
    {
      CFF_FontRecDict  dict = &cff->top_font.font_dict;


      if ( dict->cid_registry != 0xFFFFU )
        *is_cid = 1;
    }

    return error;
  }


  static FT_Error
  cff_get_cid_from_glyph_index( CFF_Face  face,
                                FT_UInt   glyph_index,
                                FT_UInt  *cid )
  {
    FT_Error  error = FT_Err_Ok;
    CFF_Font  cff;


    cff = (CFF_Font)face->extra.data;

    if ( cff )
    {
      FT_UInt          c;
      CFF_FontRecDict  dict = &cff->top_font.font_dict;


      if ( dict->cid_registry == 0xFFFFU )
      {
        error = FT_THROW( Invalid_Argument );
        goto Fail;
      }

      if ( glyph_index > cff->num_glyphs )
      {
        error = FT_THROW( Invalid_Argument );
        goto Fail;
      }

      c = cff->charset.sids[glyph_index];

      if ( cid )
        *cid = c;
    }

  Fail:
    return error;
  }


  FT_DEFINE_SERVICE_CIDREC(
    cff_service_cid_info,
    (FT_CID_GetRegistryOrderingSupplementFunc)cff_get_ros,
    (FT_CID_GetIsInternallyCIDKeyedFunc)      cff_get_is_cid,
    (FT_CID_GetCIDFromGlyphIndexFunc)         cff_get_cid_from_glyph_index
  )


  /*
   *  PROPERTY SERVICE
   *
   */
  static FT_Error
  cff_property_set( FT_Module    module,         /* CFF_Driver */
                    const char*  property_name,
                    const void*  value )
  {
    FT_Error    error  = FT_Err_Ok;
    CFF_Driver  driver = (CFF_Driver)module;


    if ( !ft_strcmp( property_name, "darkening-parameters" ) )
    {
      FT_Int*  darken_params = (FT_Int*)value;

      FT_Int  x1 = darken_params[0];
      FT_Int  y1 = darken_params[1];
      FT_Int  x2 = darken_params[2];
      FT_Int  y2 = darken_params[3];
      FT_Int  x3 = darken_params[4];
      FT_Int  y3 = darken_params[5];
      FT_Int  x4 = darken_params[6];
      FT_Int  y4 = darken_params[7];


      if ( x1 < 0   || x2 < 0   || x3 < 0   || x4 < 0   ||
           y1 < 0   || y2 < 0   || y3 < 0   || y4 < 0   ||
           x1 > x2  || x2 > x3  || x3 > x4              ||
           y1 > 500 || y2 > 500 || y3 > 500 || y4 > 500 )
        return FT_THROW( Invalid_Argument );

      driver->darken_params[0] = x1;
      driver->darken_params[1] = y1;
      driver->darken_params[2] = x2;
      driver->darken_params[3] = y2;
      driver->darken_params[4] = x3;
      driver->darken_params[5] = y3;
      driver->darken_params[6] = x4;
      driver->darken_params[7] = y4;

      return error;
    }
    else if ( !ft_strcmp( property_name, "hinting-engine" ) )
    {
      FT_UInt*  hinting_engine = (FT_UInt*)value;


#ifndef CFF_CONFIG_OPTION_OLD_ENGINE
      if ( *hinting_engine != FT_CFF_HINTING_ADOBE )
        error = FT_ERR( Unimplemented_Feature );
      else
#endif
        driver->hinting_engine = *hinting_engine;

      return error;
    }
    else if ( !ft_strcmp( property_name, "no-stem-darkening" ) )
    {
      FT_Bool*  no_stem_darkening = (FT_Bool*)value;


      driver->no_stem_darkening = *no_stem_darkening;

      return error;
    }

    FT_TRACE0(( "cff_property_set: missing property `%s'\n",
                property_name ));
    return FT_THROW( Missing_Property );
  }


  static FT_Error
  cff_property_get( FT_Module    module,         /* CFF_Driver */
                    const char*  property_name,
                    const void*  value )
  {
    FT_Error    error  = FT_Err_Ok;
    CFF_Driver  driver = (CFF_Driver)module;


    if ( !ft_strcmp( property_name, "darkening-parameters" ) )
    {
      FT_Int*  darken_params = driver->darken_params;
      FT_Int*  val           = (FT_Int*)value;


      val[0] = darken_params[0];
      val[1] = darken_params[1];
      val[2] = darken_params[2];
      val[3] = darken_params[3];
      val[4] = darken_params[4];
      val[5] = darken_params[5];
      val[6] = darken_params[6];
      val[7] = darken_params[7];

      return error;
    }
    else if ( !ft_strcmp( property_name, "hinting-engine" ) )
    {
      FT_UInt   hinting_engine    = driver->hinting_engine;
      FT_UInt*  val               = (FT_UInt*)value;


      *val = hinting_engine;

      return error;
    }
    else if ( !ft_strcmp( property_name, "no-stem-darkening" ) )
    {
      FT_Bool   no_stem_darkening = driver->no_stem_darkening;
      FT_Bool*  val               = (FT_Bool*)value;


      *val = no_stem_darkening;

      return error;
    }

    FT_TRACE0(( "cff_property_get: missing property `%s'\n",
                property_name ));
    return FT_THROW( Missing_Property );
  }


  FT_DEFINE_SERVICE_PROPERTIESREC(
    cff_service_properties,
    (FT_Properties_SetFunc)cff_property_set,
    (FT_Properties_GetFunc)cff_property_get )


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                D R I V E R  I N T E R F A C E                   ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

#ifndef FT_CONFIG_OPTION_NO_GLYPH_NAMES
  FT_DEFINE_SERVICEDESCREC7(
    cff_services,
    FT_SERVICE_ID_XF86_NAME,            FT_XF86_FORMAT_CFF,
    FT_SERVICE_ID_POSTSCRIPT_INFO,      &CFF_SERVICE_PS_INFO_GET,
    FT_SERVICE_ID_POSTSCRIPT_FONT_NAME, &CFF_SERVICE_PS_NAME_GET,
    FT_SERVICE_ID_GLYPH_DICT,           &CFF_SERVICE_GLYPH_DICT_GET,
    FT_SERVICE_ID_TT_CMAP,              &CFF_SERVICE_GET_CMAP_INFO_GET,
    FT_SERVICE_ID_CID,                  &CFF_SERVICE_CID_INFO_GET,
    FT_SERVICE_ID_PROPERTIES,           &CFF_SERVICE_PROPERTIES_GET
  )
#else
  FT_DEFINE_SERVICEDESCREC6(
    cff_services,
    FT_SERVICE_ID_XF86_NAME,            FT_XF86_FORMAT_CFF,
    FT_SERVICE_ID_POSTSCRIPT_INFO,      &CFF_SERVICE_PS_INFO_GET,
    FT_SERVICE_ID_POSTSCRIPT_FONT_NAME, &CFF_SERVICE_PS_NAME_GET,
    FT_SERVICE_ID_TT_CMAP,              &CFF_SERVICE_GET_CMAP_INFO_GET,
    FT_SERVICE_ID_CID,                  &CFF_SERVICE_CID_INFO_GET,
    FT_SERVICE_ID_PROPERTIES,           &CFF_SERVICE_PROPERTIES_GET
  )
#endif


  FT_CALLBACK_DEF( FT_Module_Interface )
  cff_get_interface( FT_Module    driver,       /* CFF_Driver */
                     const char*  module_interface )
  {
    FT_Library           library;
    FT_Module            sfnt;
    FT_Module_Interface  result;


    /* CFF_SERVICES_GET derefers `library' in PIC mode */
#ifdef FT_CONFIG_OPTION_PIC
    if ( !driver )
      return NULL;
    library = driver->library;
    if ( !library )
      return NULL;
#endif

    result = ft_service_list_lookup( CFF_SERVICES_GET, module_interface );
    if ( result != NULL )
      return result;

    /* `driver' is not yet evaluated in non-PIC mode */
#ifndef FT_CONFIG_OPTION_PIC
    if ( !driver )
      return NULL;
    library = driver->library;
    if ( !library )
      return NULL;
#endif

    /* we pass our request to the `sfnt' module */
    sfnt = FT_Get_Module( library, "sfnt" );

    return sfnt ? sfnt->clazz->get_interface( sfnt, module_interface ) : 0;
  }


  /* The FT_DriverInterface structure is defined in ftdriver.h. */

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
#define CFF_SIZE_SELECT cff_size_select
#else
#define CFF_SIZE_SELECT 0
#endif

  FT_DEFINE_DRIVER(
    cff_driver_class,

      FT_MODULE_FONT_DRIVER       |
      FT_MODULE_DRIVER_SCALABLE   |
      FT_MODULE_DRIVER_HAS_HINTER,

      sizeof ( CFF_DriverRec ),
      "cff",
      0x10000L,
      0x20000L,

      0,   /* module-specific interface */

      cff_driver_init,
      cff_driver_done,
      cff_get_interface,

    /* now the specific driver fields */
    sizeof ( TT_FaceRec ),
    sizeof ( CFF_SizeRec ),
    sizeof ( CFF_GlyphSlotRec ),

    cff_face_init,
    cff_face_done,
    cff_size_init,
    cff_size_done,
    cff_slot_init,
    cff_slot_done,

    cff_glyph_load,

    cff_get_kerning,
    0,                       /* FT_Face_AttachFunc */
    cff_get_advances,

    cff_size_request,

    CFF_SIZE_SELECT
  )


/* END */
/***************************************************************************/
/*                                                                         */
/*  cffparse.c                                                             */
/*                                                                         */
/*    CFF token stream parser (body)                                       */
/*                                                                         */
/*  Copyright 1996-2004, 2007-2014 by                                      */
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
#include "cffparse.h"
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_DEBUG_H

#include "cfferrs.h"
#include "cffpic.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cffparse


  FT_LOCAL_DEF( void )
  cff_parser_init( CFF_Parser  parser,
                   FT_UInt     code,
                   void*       object,
                   FT_Library  library)
  {
    FT_MEM_ZERO( parser, sizeof ( *parser ) );

    parser->top         = parser->stack;
    parser->object_code = code;
    parser->object      = object;
    parser->library     = library;
  }


  /* read an integer */
  static FT_Long
  cff_parse_integer( FT_Byte*  start,
                     FT_Byte*  limit )
  {
    FT_Byte*  p   = start;
    FT_Int    v   = *p++;
    FT_Long   val = 0;


    if ( v == 28 )
    {
      if ( p + 2 > limit )
        goto Bad;

      val = (FT_Short)( ( (FT_UShort)p[0] << 8 ) | p[1] );
    }
    else if ( v == 29 )
    {
      if ( p + 4 > limit )
        goto Bad;

      val = (FT_Long)( ( (FT_ULong)p[0] << 24 ) |
                       ( (FT_ULong)p[1] << 16 ) |
                       ( (FT_ULong)p[2] <<  8 ) |
                         (FT_ULong)p[3]         );
    }
    else if ( v < 247 )
    {
      val = v - 139;
    }
    else if ( v < 251 )
    {
      if ( p + 1 > limit )
        goto Bad;

      val = ( v - 247 ) * 256 + p[0] + 108;
    }
    else
    {
      if ( p + 1 > limit )
        goto Bad;

      val = -( v - 251 ) * 256 - p[0] - 108;
    }

  Exit:
    return val;

  Bad:
    val = 0;
    FT_TRACE4(( "!!!END OF DATA:!!!" ));
    goto Exit;
  }


  static const FT_Long power_tens[] =
  {
    1L,
    10L,
    100L,
    1000L,
    10000L,
    100000L,
    1000000L,
    10000000L,
    100000000L,
    1000000000L
  };


  /* read a real */
  static FT_Fixed
  cff_parse_real( FT_Byte*  start,
                  FT_Byte*  limit,
                  FT_Long   power_ten,
                  FT_Long*  scaling )
  {
    FT_Byte*  p = start;
    FT_UInt   nib;
    FT_UInt   phase;

    FT_Long   result, number, exponent;
    FT_Int    sign = 0, exponent_sign = 0, have_overflow = 0;
    FT_Long   exponent_add, integer_length, fraction_length;


    if ( scaling )
      *scaling = 0;

    result = 0;

    number   = 0;
    exponent = 0;

    exponent_add    = 0;
    integer_length  = 0;
    fraction_length = 0;

    /* First of all, read the integer part. */
    phase = 4;

    for (;;)
    {
      /* If we entered this iteration with phase == 4, we need to */
      /* read a new byte.  This also skips past the initial 0x1E. */
      if ( phase )
      {
        p++;

        /* Make sure we don't read past the end. */
        if ( p >= limit )
          goto Bad;
      }

      /* Get the nibble. */
      nib   = ( p[0] >> phase ) & 0xF;
      phase = 4 - phase;

      if ( nib == 0xE )
        sign = 1;
      else if ( nib > 9 )
        break;
      else
      {
        /* Increase exponent if we can't add the digit. */
        if ( number >= 0xCCCCCCCL )
          exponent_add++;
        /* Skip leading zeros. */
        else if ( nib || number )
        {
          integer_length++;
          number = number * 10 + nib;
        }
      }
    }

    /* Read fraction part, if any. */
    if ( nib == 0xa )
      for (;;)
      {
        /* If we entered this iteration with phase == 4, we need */
        /* to read a new byte.                                   */
        if ( phase )
        {
          p++;

          /* Make sure we don't read past the end. */
          if ( p >= limit )
            goto Bad;
        }

        /* Get the nibble. */
        nib   = ( p[0] >> phase ) & 0xF;
        phase = 4 - phase;
        if ( nib >= 10 )
          break;

        /* Skip leading zeros if possible. */
        if ( !nib && !number )
          exponent_add--;
        /* Only add digit if we don't overflow. */
        else if ( number < 0xCCCCCCCL && fraction_length < 9 )
        {
          fraction_length++;
          number = number * 10 + nib;
        }
      }

    /* Read exponent, if any. */
    if ( nib == 12 )
    {
      exponent_sign = 1;
      nib           = 11;
    }

    if ( nib == 11 )
    {
      for (;;)
      {
        /* If we entered this iteration with phase == 4, */
        /* we need to read a new byte.                   */
        if ( phase )
        {
          p++;

          /* Make sure we don't read past the end. */
          if ( p >= limit )
            goto Bad;
        }

        /* Get the nibble. */
        nib   = ( p[0] >> phase ) & 0xF;
        phase = 4 - phase;
        if ( nib >= 10 )
          break;

        /* Arbitrarily limit exponent. */
        if ( exponent > 1000 )
          have_overflow = 1;
        else
          exponent = exponent * 10 + nib;
      }

      if ( exponent_sign )
        exponent = -exponent;
    }

    if ( !number )
      goto Exit;

    if ( have_overflow )
    {
      if ( exponent_sign )
        goto Underflow;
      else
        goto Overflow;
    }

    /* We don't check `power_ten' and `exponent_add'. */
    exponent += power_ten + exponent_add;

    if ( scaling )
    {
      /* Only use `fraction_length'. */
      fraction_length += integer_length;
      exponent        += integer_length;

      if ( fraction_length <= 5 )
      {
        if ( number > 0x7FFFL )
        {
          result   = FT_DivFix( number, 10 );
          *scaling = exponent - fraction_length + 1;
        }
        else
        {
          if ( exponent > 0 )
          {
            FT_Long  new_fraction_length, shift;


            /* Make `scaling' as small as possible. */
            new_fraction_length = FT_MIN( exponent, 5 );
            shift               = new_fraction_length - fraction_length;

            if ( shift > 0 )
            {
              exponent -= new_fraction_length;
              number   *= power_tens[shift];
              if ( number > 0x7FFFL )
              {
                number   /= 10;
                exponent += 1;
              }
            }
            else
              exponent -= fraction_length;
          }
          else
            exponent -= fraction_length;

          result   = (FT_Long)( (FT_ULong)number << 16 );
          *scaling = exponent;
        }
      }
      else
      {
        if ( ( number / power_tens[fraction_length - 5] ) > 0x7FFFL )
        {
          result   = FT_DivFix( number, power_tens[fraction_length - 4] );
          *scaling = exponent - 4;
        }
        else
        {
          result   = FT_DivFix( number, power_tens[fraction_length - 5] );
          *scaling = exponent - 5;
        }
      }
    }
    else
    {
      integer_length  += exponent;
      fraction_length -= exponent;

      if ( integer_length > 5 )
        goto Overflow;
      if ( integer_length < -5 )
        goto Underflow;

      /* Remove non-significant digits. */
      if ( integer_length < 0 )
      {
        number          /= power_tens[-integer_length];
        fraction_length += integer_length;
      }

      /* this can only happen if exponent was non-zero */
      if ( fraction_length == 10 )
      {
        number          /= 10;
        fraction_length -= 1;
      }

      /* Convert into 16.16 format. */
      if ( fraction_length > 0 )
      {
        if ( ( number / power_tens[fraction_length] ) > 0x7FFFL )
          goto Exit;

        result = FT_DivFix( number, power_tens[fraction_length] );
      }
      else
      {
        number *= power_tens[-fraction_length];

        if ( number > 0x7FFFL )
          goto Overflow;

        result = (FT_Long)( (FT_ULong)number << 16 );
      }
    }

  Exit:
    if ( sign )
      result = -result;

    return result;

  Overflow:
    result = 0x7FFFFFFFL;
    FT_TRACE4(( "!!!OVERFLOW:!!!" ));
    goto Exit;

  Underflow:
    result = 0;
    FT_TRACE4(( "!!!UNDERFLOW:!!!" ));
    goto Exit;

  Bad:
    result = 0;
    FT_TRACE4(( "!!!END OF DATA:!!!" ));
    goto Exit;
  }


  /* read a number, either integer or real */
  static FT_Long
  cff_parse_num( FT_Byte**  d )
  {
    return **d == 30 ? ( cff_parse_real( d[0], d[1], 0, NULL ) >> 16 )
                     :   cff_parse_integer( d[0], d[1] );
  }


  /* read a floating point number, either integer or real */
  static FT_Fixed
  do_fixed( FT_Byte**  d,
            FT_Long    scaling )
  {
    if ( **d == 30 )
      return cff_parse_real( d[0], d[1], scaling, NULL );
    else
    {
      FT_Long  val = cff_parse_integer( d[0], d[1] );


      if ( scaling )
        val *= power_tens[scaling];

      if ( val > 0x7FFF )
      {
        val = 0x7FFFFFFFL;
        goto Overflow;
      }
      else if ( val < -0x7FFF )
      {
        val = -0x7FFFFFFFL;
        goto Overflow;
      }

      return (FT_Long)( (FT_ULong)val << 16 );

    Overflow:
      FT_TRACE4(( "!!!OVERFLOW:!!!" ));
      return val;
    }
  }


  /* read a floating point number, either integer or real */
  static FT_Fixed
  cff_parse_fixed( FT_Byte**  d )
  {
    return do_fixed( d, 0 );
  }


  /* read a floating point number, either integer or real, */
  /* but return `10^scaling' times the number read in      */
  static FT_Fixed
  cff_parse_fixed_scaled( FT_Byte**  d,
                          FT_Long    scaling )
  {
    return do_fixed( d, scaling );
  }


  /* read a floating point number, either integer or real,     */
  /* and return it as precise as possible -- `scaling' returns */
  /* the scaling factor (as a power of 10)                     */
  static FT_Fixed
  cff_parse_fixed_dynamic( FT_Byte**  d,
                           FT_Long*   scaling )
  {
    FT_ASSERT( scaling );

    if ( **d == 30 )
      return cff_parse_real( d[0], d[1], 0, scaling );
    else
    {
      FT_Long  number;
      FT_Int   integer_length;


      number = cff_parse_integer( d[0], d[1] );

      if ( number > 0x7FFFL )
      {
        for ( integer_length = 5; integer_length < 10; integer_length++ )
          if ( number < power_tens[integer_length] )
            break;

        if ( ( number / power_tens[integer_length - 5] ) > 0x7FFFL )
        {
          *scaling = integer_length - 4;
          return FT_DivFix( number, power_tens[integer_length - 4] );
        }
        else
        {
          *scaling = integer_length - 5;
          return FT_DivFix( number, power_tens[integer_length - 5] );
        }
      }
      else
      {
        *scaling = 0;
        return (FT_Long)( (FT_ULong)number << 16 );
      }
    }
  }


  static FT_Error
  cff_parse_font_matrix( CFF_Parser  parser )
  {
    CFF_FontRecDict  dict   = (CFF_FontRecDict)parser->object;
    FT_Matrix*       matrix = &dict->font_matrix;
    FT_Vector*       offset = &dict->font_offset;
    FT_ULong*        upm    = &dict->units_per_em;
    FT_Byte**        data   = parser->stack;
    FT_Error         error  = FT_ERR( Stack_Underflow );


    if ( parser->top >= parser->stack + 6 )
    {
      FT_Long  scaling;


      error = FT_Err_Ok;

      dict->has_font_matrix = TRUE;

      /* We expect a well-formed font matrix, this is, the matrix elements */
      /* `xx' and `yy' are of approximately the same magnitude.  To avoid  */
      /* loss of precision, we use the magnitude of element `xx' to scale  */
      /* all other elements.  The scaling factor is then contained in the  */
      /* `units_per_em' value.                                             */

      matrix->xx = cff_parse_fixed_dynamic( data++, &scaling );

      scaling = -scaling;

      if ( scaling < 0 || scaling > 9 )
      {
        /* Return default matrix in case of unlikely values. */

        FT_TRACE1(( "cff_parse_font_matrix:"
                    " strange scaling value for xx element (%d),\n"
                    "                      "
                    " using default matrix\n", scaling ));

        matrix->xx = 0x10000L;
        matrix->yx = 0;
        matrix->xy = 0;
        matrix->yy = 0x10000L;
        offset->x  = 0;
        offset->y  = 0;
        *upm       = 1;

        goto Exit;
      }

      matrix->yx = cff_parse_fixed_scaled( data++, scaling );
      matrix->xy = cff_parse_fixed_scaled( data++, scaling );
      matrix->yy = cff_parse_fixed_scaled( data++, scaling );
      offset->x  = cff_parse_fixed_scaled( data++, scaling );
      offset->y  = cff_parse_fixed_scaled( data,   scaling );

      *upm = power_tens[scaling];

      FT_TRACE4(( " [%f %f %f %f %f %f]\n",
                  (double)matrix->xx / *upm / 65536,
                  (double)matrix->xy / *upm / 65536,
                  (double)matrix->yx / *upm / 65536,
                  (double)matrix->yy / *upm / 65536,
                  (double)offset->x  / *upm / 65536,
                  (double)offset->y  / *upm / 65536 ));
    }

  Exit:
    return error;
  }


  static FT_Error
  cff_parse_font_bbox( CFF_Parser  parser )
  {
    CFF_FontRecDict  dict = (CFF_FontRecDict)parser->object;
    FT_BBox*         bbox = &dict->font_bbox;
    FT_Byte**        data = parser->stack;
    FT_Error         error;


    error = FT_ERR( Stack_Underflow );

    if ( parser->top >= parser->stack + 4 )
    {
      bbox->xMin = FT_RoundFix( cff_parse_fixed( data++ ) );
      bbox->yMin = FT_RoundFix( cff_parse_fixed( data++ ) );
      bbox->xMax = FT_RoundFix( cff_parse_fixed( data++ ) );
      bbox->yMax = FT_RoundFix( cff_parse_fixed( data   ) );
      error = FT_Err_Ok;

      FT_TRACE4(( " [%d %d %d %d]\n",
                  bbox->xMin / 65536,
                  bbox->yMin / 65536,
                  bbox->xMax / 65536,
                  bbox->yMax / 65536 ));
    }

    return error;
  }


  static FT_Error
  cff_parse_private_dict( CFF_Parser  parser )
  {
    CFF_FontRecDict  dict = (CFF_FontRecDict)parser->object;
    FT_Byte**        data = parser->stack;
    FT_Error         error;


    error = FT_ERR( Stack_Underflow );

    if ( parser->top >= parser->stack + 2 )
    {
      dict->private_size   = cff_parse_num( data++ );
      dict->private_offset = cff_parse_num( data   );
      FT_TRACE4(( " %lu %lu\n",
                  dict->private_size, dict->private_offset ));

      error = FT_Err_Ok;
    }

    return error;
  }


  static FT_Error
  cff_parse_cid_ros( CFF_Parser  parser )
  {
    CFF_FontRecDict  dict = (CFF_FontRecDict)parser->object;
    FT_Byte**        data = parser->stack;
    FT_Error         error;


    error = FT_ERR( Stack_Underflow );

    if ( parser->top >= parser->stack + 3 )
    {
      dict->cid_registry = (FT_UInt)cff_parse_num( data++ );
      dict->cid_ordering = (FT_UInt)cff_parse_num( data++ );
      if ( **data == 30 )
        FT_TRACE1(( "cff_parse_cid_ros: real supplement is rounded\n" ));
      dict->cid_supplement = cff_parse_num( data );
      if ( dict->cid_supplement < 0 )
        FT_TRACE1(( "cff_parse_cid_ros: negative supplement %d is found\n",
                   dict->cid_supplement ));
      error = FT_Err_Ok;

      FT_TRACE4(( " %d %d %d\n",
                  dict->cid_registry,
                  dict->cid_ordering,
                  dict->cid_supplement ));
    }

    return error;
  }


#define CFF_FIELD_NUM( code, name, id )             \
          CFF_FIELD( code, name, id, cff_kind_num )
#define CFF_FIELD_FIXED( code, name, id )             \
          CFF_FIELD( code, name, id, cff_kind_fixed )
#define CFF_FIELD_FIXED_1000( code, name, id )                 \
          CFF_FIELD( code, name, id, cff_kind_fixed_thousand )
#define CFF_FIELD_STRING( code, name, id )             \
          CFF_FIELD( code, name, id, cff_kind_string )
#define CFF_FIELD_BOOL( code, name, id )             \
          CFF_FIELD( code, name, id, cff_kind_bool )

#define CFFCODE_TOPDICT  0x1000
#define CFFCODE_PRIVATE  0x2000


#ifndef FT_CONFIG_OPTION_PIC


#undef  CFF_FIELD
#undef  CFF_FIELD_DELTA


#ifndef FT_DEBUG_LEVEL_TRACE


#define CFF_FIELD_CALLBACK( code, name, id ) \
          {                                  \
            cff_kind_callback,               \
            code | CFFCODE,                  \
            0, 0,                            \
            cff_parse_ ## name,              \
            0, 0                             \
          },

#define CFF_FIELD( code, name, id, kind ) \
          {                               \
            kind,                         \
            code | CFFCODE,               \
            FT_FIELD_OFFSET( name ),      \
            FT_FIELD_SIZE( name ),        \
            0, 0, 0                       \
          },

#define CFF_FIELD_DELTA( code, name, max, id ) \
          {                                    \
            cff_kind_delta,                    \
            code | CFFCODE,                    \
            FT_FIELD_OFFSET( name ),           \
            FT_FIELD_SIZE_DELTA( name ),       \
            0,                                 \
            max,                               \
            FT_FIELD_OFFSET( num_ ## name )    \
          },

  static const CFF_Field_Handler  cff_field_handlers[] =
  {

#include "cfftoken.h"

    { 0, 0, 0, 0, 0, 0, 0 }
  };


#else /* FT_DEBUG_LEVEL_TRACE */



#define CFF_FIELD_CALLBACK( code, name, id ) \
          {                                  \
            cff_kind_callback,               \
            code | CFFCODE,                  \
            0, 0,                            \
            cff_parse_ ## name,              \
            0, 0,                            \
            id                               \
          },

#define CFF_FIELD( code, name, id, kind ) \
          {                               \
            kind,                         \
            code | CFFCODE,               \
            FT_FIELD_OFFSET( name ),      \
            FT_FIELD_SIZE( name ),        \
            0, 0, 0,                      \
            id                            \
          },

#define CFF_FIELD_DELTA( code, name, max, id ) \
          {                                    \
            cff_kind_delta,                    \
            code | CFFCODE,                    \
            FT_FIELD_OFFSET( name ),           \
            FT_FIELD_SIZE_DELTA( name ),       \
            0,                                 \
            max,                               \
            FT_FIELD_OFFSET( num_ ## name ),   \
            id                                 \
          },

  static const CFF_Field_Handler  cff_field_handlers[] =
  {

#include "cfftoken.h"

    { 0, 0, 0, 0, 0, 0, 0, 0 }
  };


#endif /* FT_DEBUG_LEVEL_TRACE */


#else /* FT_CONFIG_OPTION_PIC */


  void
  FT_Destroy_Class_cff_field_handlers( FT_Library          library,
                                       CFF_Field_Handler*  clazz )
  {
    FT_Memory  memory = library->memory;


    if ( clazz )
      FT_FREE( clazz );
  }


  FT_Error
  FT_Create_Class_cff_field_handlers( FT_Library           library,
                                      CFF_Field_Handler**  output_class )
  {
    CFF_Field_Handler*  clazz  = NULL;
    FT_Error            error;
    FT_Memory           memory = library->memory;

    int  i = 0;


#undef CFF_FIELD
#define CFF_FIELD( code, name, id, kind ) i++;
#undef CFF_FIELD_DELTA
#define CFF_FIELD_DELTA( code, name, max, id ) i++;
#undef CFF_FIELD_CALLBACK
#define CFF_FIELD_CALLBACK( code, name, id ) i++;

#include "cfftoken.h"

    i++; /* { 0, 0, 0, 0, 0, 0, 0 } */

    if ( FT_ALLOC( clazz, sizeof ( CFF_Field_Handler ) * i ) )
      return error;

    i = 0;


#ifndef FT_DEBUG_LEVEL_TRACE


#undef CFF_FIELD_CALLBACK
#define CFF_FIELD_CALLBACK( code_, name_, id_ )        \
          clazz[i].kind         = cff_kind_callback;   \
          clazz[i].code         = code_ | CFFCODE;     \
          clazz[i].offset       = 0;                   \
          clazz[i].size         = 0;                   \
          clazz[i].reader       = cff_parse_ ## name_; \
          clazz[i].array_max    = 0;                   \
          clazz[i].count_offset = 0;                   \
          i++;

#undef  CFF_FIELD
#define CFF_FIELD( code_, name_, id_, kind_ )               \
          clazz[i].kind         = kind_;                    \
          clazz[i].code         = code_ | CFFCODE;          \
          clazz[i].offset       = FT_FIELD_OFFSET( name_ ); \
          clazz[i].size         = FT_FIELD_SIZE( name_ );   \
          clazz[i].reader       = 0;                        \
          clazz[i].array_max    = 0;                        \
          clazz[i].count_offset = 0;                        \
          i++;                                              \

#undef  CFF_FIELD_DELTA
#define CFF_FIELD_DELTA( code_, name_, max_, id_ )                  \
          clazz[i].kind         = cff_kind_delta;                   \
          clazz[i].code         = code_ | CFFCODE;                  \
          clazz[i].offset       = FT_FIELD_OFFSET( name_ );         \
          clazz[i].size         = FT_FIELD_SIZE_DELTA( name_ );     \
          clazz[i].reader       = 0;                                \
          clazz[i].array_max    = max_;                             \
          clazz[i].count_offset = FT_FIELD_OFFSET( num_ ## name_ ); \
          i++;

#include "cfftoken.h"

    clazz[i].kind         = 0;
    clazz[i].code         = 0;
    clazz[i].offset       = 0;
    clazz[i].size         = 0;
    clazz[i].reader       = 0;
    clazz[i].array_max    = 0;
    clazz[i].count_offset = 0;


#else /* FT_DEBUG_LEVEL_TRACE */


#undef CFF_FIELD_CALLBACK
#define CFF_FIELD_CALLBACK( code_, name_, id_ )        \
          clazz[i].kind         = cff_kind_callback;   \
          clazz[i].code         = code_ | CFFCODE;     \
          clazz[i].offset       = 0;                   \
          clazz[i].size         = 0;                   \
          clazz[i].reader       = cff_parse_ ## name_; \
          clazz[i].array_max    = 0;                   \
          clazz[i].count_offset = 0;                   \
          clazz[i].id           = id_;                 \
          i++;

#undef  CFF_FIELD
#define CFF_FIELD( code_, name_, id_, kind_ )               \
          clazz[i].kind         = kind_;                    \
          clazz[i].code         = code_ | CFFCODE;          \
          clazz[i].offset       = FT_FIELD_OFFSET( name_ ); \
          clazz[i].size         = FT_FIELD_SIZE( name_ );   \
          clazz[i].reader       = 0;                        \
          clazz[i].array_max    = 0;                        \
          clazz[i].count_offset = 0;                        \
          clazz[i].id           = id_;                      \
          i++;                                              \

#undef  CFF_FIELD_DELTA
#define CFF_FIELD_DELTA( code_, name_, max_, id_ )                  \
          clazz[i].kind         = cff_kind_delta;                   \
          clazz[i].code         = code_ | CFFCODE;                  \
          clazz[i].offset       = FT_FIELD_OFFSET( name_ );         \
          clazz[i].size         = FT_FIELD_SIZE_DELTA( name_ );     \
          clazz[i].reader       = 0;                                \
          clazz[i].array_max    = max_;                             \
          clazz[i].count_offset = FT_FIELD_OFFSET( num_ ## name_ ); \
          clazz[i].id           = id_;                              \
          i++;

#include "cfftoken.h"

    clazz[i].kind         = 0;
    clazz[i].code         = 0;
    clazz[i].offset       = 0;
    clazz[i].size         = 0;
    clazz[i].reader       = 0;
    clazz[i].array_max    = 0;
    clazz[i].count_offset = 0;
    clazz[i].id           = 0;


#endif /* FT_DEBUG_LEVEL_TRACE */


    *output_class = clazz;

    return FT_Err_Ok;
  }


#endif /* FT_CONFIG_OPTION_PIC */


  FT_LOCAL_DEF( FT_Error )
  cff_parser_run( CFF_Parser  parser,
                  FT_Byte*    start,
                  FT_Byte*    limit )
  {
    FT_Byte*    p       = start;
    FT_Error    error   = FT_Err_Ok;
    FT_Library  library = parser->library;
    FT_UNUSED( library );


    parser->top    = parser->stack;
    parser->start  = start;
    parser->limit  = limit;
    parser->cursor = start;

    while ( p < limit )
    {
      FT_UInt  v = *p;


      if ( v >= 27 && v != 31 )
      {
        /* it's a number; we will push its position on the stack */
        if ( parser->top - parser->stack >= CFF_MAX_STACK_DEPTH )
          goto Stack_Overflow;

        *parser->top ++ = p;

        /* now, skip it */
        if ( v == 30 )
        {
          /* skip real number */
          p++;
          for (;;)
          {
            /* An unterminated floating point number at the */
            /* end of a dictionary is invalid but harmless. */
            if ( p >= limit )
              goto Exit;
            v = p[0] >> 4;
            if ( v == 15 )
              break;
            v = p[0] & 0xF;
            if ( v == 15 )
              break;
            p++;
          }
        }
        else if ( v == 28 )
          p += 2;
        else if ( v == 29 )
          p += 4;
        else if ( v > 246 )
          p += 1;
      }
      else
      {
        /* This is not a number, hence it's an operator.  Compute its code */
        /* and look for it in our current list.                            */

        FT_UInt                   code;
        FT_UInt                   num_args = (FT_UInt)
                                             ( parser->top - parser->stack );
        const CFF_Field_Handler*  field;


        *parser->top = p;
        code = v;
        if ( v == 12 )
        {
          /* two byte operator */
          p++;
          if ( p >= limit )
            goto Syntax_Error;

          code = 0x100 | p[0];
        }
        code = code | parser->object_code;

        for ( field = CFF_FIELD_HANDLERS_GET; field->kind; field++ )
        {
          if ( field->code == (FT_Int)code )
          {
            /* we found our field's handler; read it */
            FT_Long   val;
            FT_Byte*  q = (FT_Byte*)parser->object + field->offset;


#ifdef FT_DEBUG_LEVEL_TRACE
            FT_TRACE4(( "  %s", field->id ));
#endif

            /* check that we have enough arguments -- except for */
            /* delta encoded arrays, which can be empty          */
            if ( field->kind != cff_kind_delta && num_args < 1 )
              goto Stack_Underflow;

            switch ( field->kind )
            {
            case cff_kind_bool:
            case cff_kind_string:
            case cff_kind_num:
              val = cff_parse_num( parser->stack );
              goto Store_Number;

            case cff_kind_fixed:
              val = cff_parse_fixed( parser->stack );
              goto Store_Number;

            case cff_kind_fixed_thousand:
              val = cff_parse_fixed_scaled( parser->stack, 3 );

            Store_Number:
              switch ( field->size )
              {
              case (8 / FT_CHAR_BIT):
                *(FT_Byte*)q = (FT_Byte)val;
                break;

              case (16 / FT_CHAR_BIT):
                *(FT_Short*)q = (FT_Short)val;
                break;

              case (32 / FT_CHAR_BIT):
                *(FT_Int32*)q = (FT_Int)val;
                break;

              default:  /* for 64-bit systems */
                *(FT_Long*)q = val;
              }

#ifdef FT_DEBUG_LEVEL_TRACE
              switch ( field->kind )
              {
              case cff_kind_bool:
                FT_TRACE4(( " %s\n", val ? "true" : "false" ));
                break;

              case cff_kind_string:
                FT_TRACE4(( " %ld (SID)\n", val ));
                break;

              case cff_kind_num:
                FT_TRACE4(( " %ld\n", val ));
                break;

              case cff_kind_fixed:
                FT_TRACE4(( " %f\n", (double)val / 65536 ));
                break;

              case cff_kind_fixed_thousand:
                FT_TRACE4(( " %f\n", (double)val / 65536 / 1000 ));

              default:
                ; /* never reached */
              }
#endif

              break;

            case cff_kind_delta:
              {
                FT_Byte*   qcount = (FT_Byte*)parser->object +
                                      field->count_offset;

                FT_Byte**  data = parser->stack;


                if ( num_args > field->array_max )
                  num_args = field->array_max;

                FT_TRACE4(( " [" ));

                /* store count */
                *qcount = (FT_Byte)num_args;

                val = 0;
                while ( num_args > 0 )
                {
                  val += cff_parse_num( data++ );
                  switch ( field->size )
                  {
                  case (8 / FT_CHAR_BIT):
                    *(FT_Byte*)q = (FT_Byte)val;
                    break;

                  case (16 / FT_CHAR_BIT):
                    *(FT_Short*)q = (FT_Short)val;
                    break;

                  case (32 / FT_CHAR_BIT):
                    *(FT_Int32*)q = (FT_Int)val;
                    break;

                  default:  /* for 64-bit systems */
                    *(FT_Long*)q = val;
                  }

                  FT_TRACE4(( " %ld", val ));

                  q += field->size;
                  num_args--;
                }

                FT_TRACE4(( "]\n" ));
              }
              break;

            default:  /* callback */
              error = field->reader( parser );
              if ( error )
                goto Exit;
            }
            goto Found;
          }
        }

        /* this is an unknown operator, or it is unsupported; */
        /* we will ignore it for now.                         */

      Found:
        /* clear stack */
        parser->top = parser->stack;
      }
      p++;
    }

  Exit:
    return error;

  Stack_Overflow:
    error = FT_THROW( Invalid_Argument );
    goto Exit;

  Stack_Underflow:
    error = FT_THROW( Invalid_Argument );
    goto Exit;

  Syntax_Error:
    error = FT_THROW( Invalid_Argument );
    goto Exit;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cffload.c                                                              */
/*                                                                         */
/*    OpenType and CFF data/program tables loader (body).                  */
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
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H
#include FT_TYPE1_TABLES_H

#include "cffload.h"
#include "cffparse.h"

#include "cfferrs.h"


#if 1

  static const FT_UShort  cff_isoadobe_charset[229] =
  {
      0,   1,   2,   3,   4,   5,   6,   7,
      8,   9,  10,  11,  12,  13,  14,  15,
     16,  17,  18,  19,  20,  21,  22,  23,
     24,  25,  26,  27,  28,  29,  30,  31,
     32,  33,  34,  35,  36,  37,  38,  39,
     40,  41,  42,  43,  44,  45,  46,  47,
     48,  49,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,
     64,  65,  66,  67,  68,  69,  70,  71,
     72,  73,  74,  75,  76,  77,  78,  79,
     80,  81,  82,  83,  84,  85,  86,  87,
     88,  89,  90,  91,  92,  93,  94,  95,
     96,  97,  98,  99, 100, 101, 102, 103,
    104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151,
    152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183,
    184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215,
    216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228
  };

  static const FT_UShort  cff_expert_charset[166] =
  {
      0,   1, 229, 230, 231, 232, 233, 234,
    235, 236, 237, 238,  13,  14,  15,  99,
    239, 240, 241, 242, 243, 244, 245, 246,
    247, 248,  27,  28, 249, 250, 251, 252,
    253, 254, 255, 256, 257, 258, 259, 260,
    261, 262, 263, 264, 265, 266, 109, 110,
    267, 268, 269, 270, 271, 272, 273, 274,
    275, 276, 277, 278, 279, 280, 281, 282,
    283, 284, 285, 286, 287, 288, 289, 290,
    291, 292, 293, 294, 295, 296, 297, 298,
    299, 300, 301, 302, 303, 304, 305, 306,
    307, 308, 309, 310, 311, 312, 313, 314,
    315, 316, 317, 318, 158, 155, 163, 319,
    320, 321, 322, 323, 324, 325, 326, 150,
    164, 169, 327, 328, 329, 330, 331, 332,
    333, 334, 335, 336, 337, 338, 339, 340,
    341, 342, 343, 344, 345, 346, 347, 348,
    349, 350, 351, 352, 353, 354, 355, 356,
    357, 358, 359, 360, 361, 362, 363, 364,
    365, 366, 367, 368, 369, 370, 371, 372,
    373, 374, 375, 376, 377, 378
  };

  static const FT_UShort  cff_expertsubset_charset[87] =
  {
      0,   1, 231, 232, 235, 236, 237, 238,
     13,  14,  15,  99, 239, 240, 241, 242,
    243, 244, 245, 246, 247, 248,  27,  28,
    249, 250, 251, 253, 254, 255, 256, 257,
    258, 259, 260, 261, 262, 263, 264, 265,
    266, 109, 110, 267, 268, 269, 270, 272,
    300, 301, 302, 305, 314, 315, 158, 155,
    163, 320, 321, 322, 323, 324, 325, 326,
    150, 164, 169, 327, 328, 329, 330, 331,
    332, 333, 334, 335, 336, 337, 338, 339,
    340, 341, 342, 343, 344, 345, 346
  };

  static const FT_UShort  cff_standard_encoding[256] =
  {
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      1,   2,   3,   4,   5,   6,   7,   8,
      9,  10,  11,  12,  13,  14,  15,  16,
     17,  18,  19,  20,  21,  22,  23,  24,
     25,  26,  27,  28,  29,  30,  31,  32,
     33,  34,  35,  36,  37,  38,  39,  40,
     41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51,  52,  53,  54,  55,  56,
     57,  58,  59,  60,  61,  62,  63,  64,
     65,  66,  67,  68,  69,  70,  71,  72,
     73,  74,  75,  76,  77,  78,  79,  80,
     81,  82,  83,  84,  85,  86,  87,  88,
     89,  90,  91,  92,  93,  94,  95,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,  96,  97,  98,  99, 100, 101, 102,
    103, 104, 105, 106, 107, 108, 109, 110,
      0, 111, 112, 113, 114,   0, 115, 116,
    117, 118, 119, 120, 121, 122,   0, 123,
      0, 124, 125, 126, 127, 128, 129, 130,
    131,   0, 132, 133,   0, 134, 135, 136,
    137,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0, 138,   0, 139,   0,   0,   0,   0,
    140, 141, 142, 143,   0,   0,   0,   0,
      0, 144,   0,   0,   0, 145,   0,   0,
    146, 147, 148, 149,   0,   0,   0,   0
  };

  static const FT_UShort  cff_expert_encoding[256] =
  {
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      1, 229, 230,   0, 231, 232, 233, 234,
    235, 236, 237, 238,  13,  14,  15,  99,
    239, 240, 241, 242, 243, 244, 245, 246,
    247, 248,  27,  28, 249, 250, 251, 252,
      0, 253, 254, 255, 256, 257,   0,   0,
      0, 258,   0,   0, 259, 260, 261, 262,
      0,   0, 263, 264, 265,   0, 266, 109,
    110, 267, 268, 269,   0, 270, 271, 272,
    273, 274, 275, 276, 277, 278, 279, 280,
    281, 282, 283, 284, 285, 286, 287, 288,
    289, 290, 291, 292, 293, 294, 295, 296,
    297, 298, 299, 300, 301, 302, 303,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0, 304, 305, 306,   0,   0, 307, 308,
    309, 310, 311,   0, 312,   0,   0, 312,
      0,   0, 314, 315,   0,   0, 316, 317,
    318,   0,   0,   0, 158, 155, 163, 319,
    320, 321, 322, 323, 324, 325,   0,   0,
    326, 150, 164, 169, 327, 328, 329, 330,
    331, 332, 333, 334, 335, 336, 337, 338,
    339, 340, 341, 342, 343, 344, 345, 346,
    347, 348, 349, 350, 351, 352, 353, 354,
    355, 356, 357, 358, 359, 360, 361, 362,
    363, 364, 365, 366, 367, 368, 369, 370,
    371, 372, 373, 374, 375, 376, 377, 378
  };

#endif /* 1 */


  FT_LOCAL_DEF( FT_UShort )
  cff_get_standard_encoding( FT_UInt  charcode )
  {
    return (FT_UShort)( charcode < 256 ? cff_standard_encoding[charcode]
                                       : 0 );
  }


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cffload


  /* read an offset from the index's stream current position */
  static FT_ULong
  cff_index_read_offset( CFF_Index  idx,
                         FT_Error  *errorp )
  {
    FT_Error   error;
    FT_Stream  stream = idx->stream;
    FT_Byte    tmp[4];
    FT_ULong   result = 0;


    if ( !FT_STREAM_READ( tmp, idx->off_size ) )
    {
      FT_Int  nn;


      for ( nn = 0; nn < idx->off_size; nn++ )
        result = ( result << 8 ) | tmp[nn];
    }

    *errorp = error;
    return result;
  }


  static FT_Error
  cff_index_init( CFF_Index  idx,
                  FT_Stream  stream,
                  FT_Bool    load )
  {
    FT_Error   error;
    FT_Memory  memory = stream->memory;
    FT_UShort  count;


    FT_MEM_ZERO( idx, sizeof ( *idx ) );

    idx->stream = stream;
    idx->start  = FT_STREAM_POS();
    if ( !FT_READ_USHORT( count ) &&
         count > 0                )
    {
      FT_Byte   offsize;
      FT_ULong  size;


      /* there is at least one element; read the offset size,           */
      /* then access the offset table to compute the index's total size */
      if ( FT_READ_BYTE( offsize ) )
        goto Exit;

      if ( offsize < 1 || offsize > 4 )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      idx->count    = count;
      idx->off_size = offsize;
      size          = (FT_ULong)( count + 1 ) * offsize;

      idx->data_offset = idx->start + 3 + size;

      if ( FT_STREAM_SKIP( size - offsize ) )
        goto Exit;

      size = cff_index_read_offset( idx, &error );
      if ( error )
        goto Exit;

      if ( size == 0 )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      idx->data_size = --size;

      if ( load )
      {
        /* load the data */
        if ( FT_FRAME_EXTRACT( size, idx->bytes ) )
          goto Exit;
      }
      else
      {
        /* skip the data */
        if ( FT_STREAM_SKIP( size ) )
          goto Exit;
      }
    }

  Exit:
    if ( error )
      FT_FREE( idx->offsets );

    return error;
  }


  static void
  cff_index_done( CFF_Index  idx )
  {
    if ( idx->stream )
    {
      FT_Stream  stream = idx->stream;
      FT_Memory  memory = stream->memory;


      if ( idx->bytes )
        FT_FRAME_RELEASE( idx->bytes );

      FT_FREE( idx->offsets );
      FT_MEM_ZERO( idx, sizeof ( *idx ) );
    }
  }


  static FT_Error
  cff_index_load_offsets( CFF_Index  idx )
  {
    FT_Error   error  = FT_Err_Ok;
    FT_Stream  stream = idx->stream;
    FT_Memory  memory = stream->memory;


    if ( idx->count > 0 && idx->offsets == NULL )
    {
      FT_Byte    offsize = idx->off_size;
      FT_ULong   data_size;
      FT_Byte*   p;
      FT_Byte*   p_end;
      FT_ULong*  poff;


      data_size = (FT_ULong)( idx->count + 1 ) * offsize;

      if ( FT_NEW_ARRAY( idx->offsets, idx->count + 1 ) ||
           FT_STREAM_SEEK( idx->start + 3 )             ||
           FT_FRAME_ENTER( data_size )                  )
        goto Exit;

      poff   = idx->offsets;
      p      = (FT_Byte*)stream->cursor;
      p_end  = p + data_size;

      switch ( offsize )
      {
      case 1:
        for ( ; p < p_end; p++, poff++ )
          poff[0] = p[0];
        break;

      case 2:
        for ( ; p < p_end; p += 2, poff++ )
          poff[0] = FT_PEEK_USHORT( p );
        break;

      case 3:
        for ( ; p < p_end; p += 3, poff++ )
          poff[0] = FT_PEEK_OFF3( p );
        break;

      default:
        for ( ; p < p_end; p += 4, poff++ )
          poff[0] = FT_PEEK_ULONG( p );
      }

      FT_FRAME_EXIT();
    }

  Exit:
    if ( error )
      FT_FREE( idx->offsets );

    return error;
  }


  /* Allocate a table containing pointers to an index's elements. */
  /* The `pool' argument makes this function convert the index    */
  /* entries to C-style strings (this is, NULL-terminated).       */
  static FT_Error
  cff_index_get_pointers( CFF_Index   idx,
                          FT_Byte***  table,
                          FT_Byte**   pool )
  {
    FT_Error   error     = FT_Err_Ok;
    FT_Memory  memory    = idx->stream->memory;

    FT_Byte**  t         = NULL;
    FT_Byte*   new_bytes = NULL;


    *table = NULL;

    if ( idx->offsets == NULL )
    {
      error = cff_index_load_offsets( idx );
      if ( error )
        goto Exit;
    }

    if ( idx->count > 0                                        &&
         !FT_NEW_ARRAY( t, idx->count + 1 )                    &&
         ( !pool || !FT_ALLOC( new_bytes,
                               idx->data_size + idx->count ) ) )
    {
      FT_ULong  n, cur_offset;
      FT_ULong  extra = 0;
      FT_Byte*  org_bytes = idx->bytes;


      /* at this point, `idx->offsets' can't be NULL */
      cur_offset = idx->offsets[0] - 1;

      /* sanity check */
      if ( cur_offset != 0 )
      {
        FT_TRACE0(( "cff_index_get_pointers:"
                    " invalid first offset value %d set to zero\n",
                    cur_offset ));
        cur_offset = 0;
      }

      if ( !pool )
        t[0] = org_bytes + cur_offset;
      else
        t[0] = new_bytes + cur_offset;

      for ( n = 1; n <= idx->count; n++ )
      {
        FT_ULong  next_offset = idx->offsets[n] - 1;


        /* two sanity checks for invalid offset tables */
        if ( next_offset < cur_offset )
          next_offset = cur_offset;
        else if ( next_offset > idx->data_size )
          next_offset = idx->data_size;

        if ( !pool )
          t[n] = org_bytes + next_offset;
        else
        {
          t[n] = new_bytes + next_offset + extra;

          if ( next_offset != cur_offset )
          {
            FT_MEM_COPY( t[n - 1], org_bytes + cur_offset, t[n] - t[n - 1] );
            t[n][0] = '\0';
            t[n]   += 1;
            extra++;
          }
        }

        cur_offset = next_offset;
      }
      *table = t;

      if ( pool )
        *pool = new_bytes;
    }

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  cff_index_access_element( CFF_Index  idx,
                            FT_UInt    element,
                            FT_Byte**  pbytes,
                            FT_ULong*  pbyte_len )
  {
    FT_Error  error = FT_Err_Ok;


    if ( idx && idx->count > element )
    {
      /* compute start and end offsets */
      FT_Stream  stream = idx->stream;
      FT_ULong   off1, off2 = 0;


      /* load offsets from file or the offset table */
      if ( !idx->offsets )
      {
        FT_ULong  pos = element * idx->off_size;


        if ( FT_STREAM_SEEK( idx->start + 3 + pos ) )
          goto Exit;

        off1 = cff_index_read_offset( idx, &error );
        if ( error )
          goto Exit;

        if ( off1 != 0 )
        {
          do
          {
            element++;
            off2 = cff_index_read_offset( idx, &error );
          }
          while ( off2 == 0 && element < idx->count );
        }
      }
      else   /* use offsets table */
      {
        off1 = idx->offsets[element];
        if ( off1 )
        {
          do
          {
            element++;
            off2 = idx->offsets[element];

          } while ( off2 == 0 && element < idx->count );
        }
      }

      /* XXX: should check off2 does not exceed the end of this entry; */
      /*      at present, only truncate off2 at the end of this stream */
      if ( off2 > stream->size + 1                    ||
           idx->data_offset > stream->size - off2 + 1 )
      {
        FT_ERROR(( "cff_index_access_element:"
                   " offset to next entry (%d)"
                   " exceeds the end of stream (%d)\n",
                   off2, stream->size - idx->data_offset + 1 ));
        off2 = stream->size - idx->data_offset + 1;
      }

      /* access element */
      if ( off1 && off2 > off1 )
      {
        *pbyte_len = off2 - off1;

        if ( idx->bytes )
        {
          /* this index was completely loaded in memory, that's easy */
          *pbytes = idx->bytes + off1 - 1;
        }
        else
        {
          /* this index is still on disk/file, access it through a frame */
          if ( FT_STREAM_SEEK( idx->data_offset + off1 - 1 ) ||
               FT_FRAME_EXTRACT( off2 - off1, *pbytes )      )
            goto Exit;
        }
      }
      else
      {
        /* empty index element */
        *pbytes    = 0;
        *pbyte_len = 0;
      }
    }
    else
      error = FT_THROW( Invalid_Argument );

  Exit:
    return error;
  }


  FT_LOCAL_DEF( void )
  cff_index_forget_element( CFF_Index  idx,
                            FT_Byte**  pbytes )
  {
    if ( idx->bytes == 0 )
    {
      FT_Stream  stream = idx->stream;


      FT_FRAME_RELEASE( *pbytes );
    }
  }


  /* get an entry from Name INDEX */
  FT_LOCAL_DEF( FT_String* )
  cff_index_get_name( CFF_Font  font,
                      FT_UInt   element )
  {
    CFF_Index   idx = &font->name_index;
    FT_Memory   memory = idx->stream->memory;
    FT_Byte*    bytes;
    FT_ULong    byte_len;
    FT_Error    error;
    FT_String*  name = 0;


    error = cff_index_access_element( idx, element, &bytes, &byte_len );
    if ( error )
      goto Exit;

    if ( !FT_ALLOC( name, byte_len + 1 ) )
    {
      FT_MEM_COPY( name, bytes, byte_len );
      name[byte_len] = 0;
    }
    cff_index_forget_element( idx, &bytes );

  Exit:
    return name;
  }


  /* get an entry from String INDEX */
  FT_LOCAL_DEF( FT_String* )
  cff_index_get_string( CFF_Font  font,
                        FT_UInt   element )
  {
    return ( element < font->num_strings )
             ? (FT_String*)font->strings[element]
             : NULL;
  }


  FT_LOCAL_DEF( FT_String* )
  cff_index_get_sid_string( CFF_Font  font,
                            FT_UInt   sid )
  {
    /* value 0xFFFFU indicates a missing dictionary entry */
    if ( sid == 0xFFFFU )
      return NULL;

    /* if it is not a standard string, return it */
    if ( sid > 390 )
      return cff_index_get_string( font, sid - 391 );

    /* CID-keyed CFF fonts don't have glyph names */
    if ( !font->psnames )
      return NULL;

    /* this is a standard string */
    return (FT_String *)font->psnames->adobe_std_strings( sid );
  }


  /*************************************************************************/
  /*************************************************************************/
  /***                                                                   ***/
  /***   FD Select table support                                         ***/
  /***                                                                   ***/
  /*************************************************************************/
  /*************************************************************************/


  static void
  CFF_Done_FD_Select( CFF_FDSelect  fdselect,
                      FT_Stream     stream )
  {
    if ( fdselect->data )
      FT_FRAME_RELEASE( fdselect->data );

    fdselect->data_size   = 0;
    fdselect->format      = 0;
    fdselect->range_count = 0;
  }


  static FT_Error
  CFF_Load_FD_Select( CFF_FDSelect  fdselect,
                      FT_UInt       num_glyphs,
                      FT_Stream     stream,
                      FT_ULong      offset )
  {
    FT_Error  error;
    FT_Byte   format;
    FT_UInt   num_ranges;


    /* read format */
    if ( FT_STREAM_SEEK( offset ) || FT_READ_BYTE( format ) )
      goto Exit;

    fdselect->format      = format;
    fdselect->cache_count = 0;   /* clear cache */

    switch ( format )
    {
    case 0:     /* format 0, that's simple */
      fdselect->data_size = num_glyphs;
      goto Load_Data;

    case 3:     /* format 3, a tad more complex */
      if ( FT_READ_USHORT( num_ranges ) )
        goto Exit;

      if ( !num_ranges )
      {
        FT_TRACE0(( "CFF_Load_FD_Select: empty FDSelect array\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      fdselect->data_size = num_ranges * 3 + 2;

    Load_Data:
      if ( FT_FRAME_EXTRACT( fdselect->data_size, fdselect->data ) )
        goto Exit;
      break;

    default:    /* hmm... that's wrong */
      error = FT_THROW( Invalid_File_Format );
    }

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_Byte )
  cff_fd_select_get( CFF_FDSelect  fdselect,
                     FT_UInt       glyph_index )
  {
    FT_Byte  fd = 0;


    switch ( fdselect->format )
    {
    case 0:
      fd = fdselect->data[glyph_index];
      break;

    case 3:
      /* first, compare to the cache */
      if ( (FT_UInt)( glyph_index - fdselect->cache_first ) <
                        fdselect->cache_count )
      {
        fd = fdselect->cache_fd;
        break;
      }

      /* then, look up the ranges array */
      {
        FT_Byte*  p       = fdselect->data;
        FT_Byte*  p_limit = p + fdselect->data_size;
        FT_Byte   fd2;
        FT_UInt   first, limit;


        first = FT_NEXT_USHORT( p );
        do
        {
          if ( glyph_index < first )
            break;

          fd2   = *p++;
          limit = FT_NEXT_USHORT( p );

          if ( glyph_index < limit )
          {
            fd = fd2;

            /* update cache */
            fdselect->cache_first = first;
            fdselect->cache_count = limit - first;
            fdselect->cache_fd    = fd2;
            break;
          }
          first = limit;

        } while ( p < p_limit );
      }
      break;

    default:
      ;
    }

    return fd;
  }


  /*************************************************************************/
  /*************************************************************************/
  /***                                                                   ***/
  /***   CFF font support                                                ***/
  /***                                                                   ***/
  /*************************************************************************/
  /*************************************************************************/

  static FT_Error
  cff_charset_compute_cids( CFF_Charset  charset,
                            FT_UInt      num_glyphs,
                            FT_Memory    memory )
  {
    FT_Error   error   = FT_Err_Ok;
    FT_UInt    i;
    FT_Long    j;
    FT_UShort  max_cid = 0;


    if ( charset->max_cid > 0 )
      goto Exit;

    for ( i = 0; i < num_glyphs; i++ )
    {
      if ( charset->sids[i] > max_cid )
        max_cid = charset->sids[i];
    }

    if ( FT_NEW_ARRAY( charset->cids, (FT_ULong)max_cid + 1 ) )
      goto Exit;

    /* When multiple GIDs map to the same CID, we choose the lowest */
    /* GID.  This is not described in any spec, but it matches the  */
    /* behaviour of recent Acroread versions.                       */
    for ( j = num_glyphs - 1; j >= 0 ; j-- )
      charset->cids[charset->sids[j]] = (FT_UShort)j;

    charset->max_cid    = max_cid;
    charset->num_glyphs = num_glyphs;

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_UInt )
  cff_charset_cid_to_gindex( CFF_Charset  charset,
                             FT_UInt      cid )
  {
    FT_UInt  result = 0;


    if ( cid <= charset->max_cid )
      result = charset->cids[cid];

    return result;
  }


  static void
  cff_charset_free_cids( CFF_Charset  charset,
                         FT_Memory    memory )
  {
    FT_FREE( charset->cids );
    charset->max_cid = 0;
  }


  static void
  cff_charset_done( CFF_Charset  charset,
                    FT_Stream    stream )
  {
    FT_Memory  memory = stream->memory;


    cff_charset_free_cids( charset, memory );

    FT_FREE( charset->sids );
    charset->format = 0;
    charset->offset = 0;
  }


  static FT_Error
  cff_charset_load( CFF_Charset  charset,
                    FT_UInt      num_glyphs,
                    FT_Stream    stream,
                    FT_ULong     base_offset,
                    FT_ULong     offset,
                    FT_Bool      invert )
  {
    FT_Memory  memory = stream->memory;
    FT_Error   error  = FT_Err_Ok;
    FT_UShort  glyph_sid;


    /* If the the offset is greater than 2, we have to parse the */
    /* charset table.                                            */
    if ( offset > 2 )
    {
      FT_UInt  j;


      charset->offset = base_offset + offset;

      /* Get the format of the table. */
      if ( FT_STREAM_SEEK( charset->offset ) ||
           FT_READ_BYTE( charset->format )   )
        goto Exit;

      /* Allocate memory for sids. */
      if ( FT_NEW_ARRAY( charset->sids, num_glyphs ) )
        goto Exit;

      /* assign the .notdef glyph */
      charset->sids[0] = 0;

      switch ( charset->format )
      {
      case 0:
        if ( num_glyphs > 0 )
        {
          if ( FT_FRAME_ENTER( ( num_glyphs - 1 ) * 2 ) )
            goto Exit;

          for ( j = 1; j < num_glyphs; j++ )
            charset->sids[j] = FT_GET_USHORT();

          FT_FRAME_EXIT();
        }
        break;

      case 1:
      case 2:
        {
          FT_UInt  nleft;
          FT_UInt  i;


          j = 1;

          while ( j < num_glyphs )
          {
            /* Read the first glyph sid of the range. */
            if ( FT_READ_USHORT( glyph_sid ) )
              goto Exit;

            /* Read the number of glyphs in the range.  */
            if ( charset->format == 2 )
            {
              if ( FT_READ_USHORT( nleft ) )
                goto Exit;
            }
            else
            {
              if ( FT_READ_BYTE( nleft ) )
                goto Exit;
            }

            /* try to rescue some of the SIDs if `nleft' is too large */
            if ( glyph_sid > 0xFFFFL - nleft )
            {
              FT_ERROR(( "cff_charset_load: invalid SID range trimmed"
                         " nleft=%d -> %d\n", nleft, 0xFFFFL - glyph_sid ));
              nleft = ( FT_UInt )( 0xFFFFL - glyph_sid );
            }

            /* Fill in the range of sids -- `nleft + 1' glyphs. */
            for ( i = 0; j < num_glyphs && i <= nleft; i++, j++, glyph_sid++ )
              charset->sids[j] = glyph_sid;
          }
        }
        break;

      default:
        FT_ERROR(( "cff_charset_load: invalid table format\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }
    }
    else
    {
      /* Parse default tables corresponding to offset == 0, 1, or 2.  */
      /* CFF specification intimates the following:                   */
      /*                                                              */
      /* In order to use a predefined charset, the following must be  */
      /* true: The charset constructed for the glyphs in the font's   */
      /* charstrings dictionary must match the predefined charset in  */
      /* the first num_glyphs.                                        */

      charset->offset = offset;  /* record charset type */

      switch ( (FT_UInt)offset )
      {
      case 0:
        if ( num_glyphs > 229 )
        {
          FT_ERROR(( "cff_charset_load: implicit charset larger than\n"
                     "predefined charset (Adobe ISO-Latin)\n" ));
          error = FT_THROW( Invalid_File_Format );
          goto Exit;
        }

        /* Allocate memory for sids. */
        if ( FT_NEW_ARRAY( charset->sids, num_glyphs ) )
          goto Exit;

        /* Copy the predefined charset into the allocated memory. */
        FT_ARRAY_COPY( charset->sids, cff_isoadobe_charset, num_glyphs );

        break;

      case 1:
        if ( num_glyphs > 166 )
        {
          FT_ERROR(( "cff_charset_load: implicit charset larger than\n"
                     "predefined charset (Adobe Expert)\n" ));
          error = FT_THROW( Invalid_File_Format );
          goto Exit;
        }

        /* Allocate memory for sids. */
        if ( FT_NEW_ARRAY( charset->sids, num_glyphs ) )
          goto Exit;

        /* Copy the predefined charset into the allocated memory.     */
        FT_ARRAY_COPY( charset->sids, cff_expert_charset, num_glyphs );

        break;

      case 2:
        if ( num_glyphs > 87 )
        {
          FT_ERROR(( "cff_charset_load: implicit charset larger than\n"
                     "predefined charset (Adobe Expert Subset)\n" ));
          error = FT_THROW( Invalid_File_Format );
          goto Exit;
        }

        /* Allocate memory for sids. */
        if ( FT_NEW_ARRAY( charset->sids, num_glyphs ) )
          goto Exit;

        /* Copy the predefined charset into the allocated memory.     */
        FT_ARRAY_COPY( charset->sids, cff_expertsubset_charset, num_glyphs );

        break;

      default:
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }
    }

    /* we have to invert the `sids' array for subsetted CID-keyed fonts */
    if ( invert )
      error = cff_charset_compute_cids( charset, num_glyphs, memory );

  Exit:
    /* Clean up if there was an error. */
    if ( error )
    {
      FT_FREE( charset->sids );
      FT_FREE( charset->cids );
      charset->format = 0;
      charset->offset = 0;
      charset->sids   = 0;
    }

    return error;
  }


  static void
  cff_encoding_done( CFF_Encoding  encoding )
  {
    encoding->format = 0;
    encoding->offset = 0;
    encoding->count  = 0;
  }


  static FT_Error
  cff_encoding_load( CFF_Encoding  encoding,
                     CFF_Charset   charset,
                     FT_UInt       num_glyphs,
                     FT_Stream     stream,
                     FT_ULong      base_offset,
                     FT_ULong      offset )
  {
    FT_Error   error = FT_Err_Ok;
    FT_UInt    count;
    FT_UInt    j;
    FT_UShort  glyph_sid;
    FT_UInt    glyph_code;


    /* Check for charset->sids.  If we do not have this, we fail. */
    if ( !charset->sids )
    {
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    /* Zero out the code to gid/sid mappings. */
    for ( j = 0; j < 256; j++ )
    {
      encoding->sids [j] = 0;
      encoding->codes[j] = 0;
    }

    /* Note: The encoding table in a CFF font is indexed by glyph index;  */
    /* the first encoded glyph index is 1.  Hence, we read the character  */
    /* code (`glyph_code') at index j and make the assignment:            */
    /*                                                                    */
    /*    encoding->codes[glyph_code] = j + 1                             */
    /*                                                                    */
    /* We also make the assignment:                                       */
    /*                                                                    */
    /*    encoding->sids[glyph_code] = charset->sids[j + 1]               */
    /*                                                                    */
    /* This gives us both a code to GID and a code to SID mapping.        */

    if ( offset > 1 )
    {
      encoding->offset = base_offset + offset;

      /* we need to parse the table to determine its size */
      if ( FT_STREAM_SEEK( encoding->offset ) ||
           FT_READ_BYTE( encoding->format )   ||
           FT_READ_BYTE( count )              )
        goto Exit;

      switch ( encoding->format & 0x7F )
      {
      case 0:
        {
          FT_Byte*  p;


          /* By convention, GID 0 is always ".notdef" and is never */
          /* coded in the font.  Hence, the number of codes found  */
          /* in the table is `count+1'.                            */
          /*                                                       */
          encoding->count = count + 1;

          if ( FT_FRAME_ENTER( count ) )
            goto Exit;

          p = (FT_Byte*)stream->cursor;

          for ( j = 1; j <= count; j++ )
          {
            glyph_code = *p++;

            /* Make sure j is not too big. */
            if ( j < num_glyphs )
            {
              /* Assign code to GID mapping. */
              encoding->codes[glyph_code] = (FT_UShort)j;

              /* Assign code to SID mapping. */
              encoding->sids[glyph_code] = charset->sids[j];
            }
          }

          FT_FRAME_EXIT();
        }
        break;

      case 1:
        {
          FT_UInt  nleft;
          FT_UInt  i = 1;
          FT_UInt  k;


          encoding->count = 0;

          /* Parse the Format1 ranges. */
          for ( j = 0;  j < count; j++, i += nleft )
          {
            /* Read the first glyph code of the range. */
            if ( FT_READ_BYTE( glyph_code ) )
              goto Exit;

            /* Read the number of codes in the range. */
            if ( FT_READ_BYTE( nleft ) )
              goto Exit;

            /* Increment nleft, so we read `nleft + 1' codes/sids. */
            nleft++;

            /* compute max number of character codes */
            if ( (FT_UInt)nleft > encoding->count )
              encoding->count = nleft;

            /* Fill in the range of codes/sids. */
            for ( k = i; k < nleft + i; k++, glyph_code++ )
            {
              /* Make sure k is not too big. */
              if ( k < num_glyphs && glyph_code < 256 )
              {
                /* Assign code to GID mapping. */
                encoding->codes[glyph_code] = (FT_UShort)k;

                /* Assign code to SID mapping. */
                encoding->sids[glyph_code] = charset->sids[k];
              }
            }
          }

          /* simple check; one never knows what can be found in a font */
          if ( encoding->count > 256 )
            encoding->count = 256;
        }
        break;

      default:
        FT_ERROR(( "cff_encoding_load: invalid table format\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      /* Parse supplemental encodings, if any. */
      if ( encoding->format & 0x80 )
      {
        FT_UInt  gindex;


        /* count supplements */
        if ( FT_READ_BYTE( count ) )
          goto Exit;

        for ( j = 0; j < count; j++ )
        {
          /* Read supplemental glyph code. */
          if ( FT_READ_BYTE( glyph_code ) )
            goto Exit;

          /* Read the SID associated with this glyph code. */
          if ( FT_READ_USHORT( glyph_sid ) )
            goto Exit;

          /* Assign code to SID mapping. */
          encoding->sids[glyph_code] = glyph_sid;

          /* First, look up GID which has been assigned to */
          /* SID glyph_sid.                                */
          for ( gindex = 0; gindex < num_glyphs; gindex++ )
          {
            if ( charset->sids[gindex] == glyph_sid )
            {
              encoding->codes[glyph_code] = (FT_UShort)gindex;
              break;
            }
          }
        }
      }
    }
    else
    {
      /* We take into account the fact a CFF font can use a predefined */
      /* encoding without containing all of the glyphs encoded by this */
      /* encoding (see the note at the end of section 12 in the CFF    */
      /* specification).                                               */

      switch ( (FT_UInt)offset )
      {
      case 0:
        /* First, copy the code to SID mapping. */
        FT_ARRAY_COPY( encoding->sids, cff_standard_encoding, 256 );
        goto Populate;

      case 1:
        /* First, copy the code to SID mapping. */
        FT_ARRAY_COPY( encoding->sids, cff_expert_encoding, 256 );

      Populate:
        /* Construct code to GID mapping from code to SID mapping */
        /* and charset.                                           */

        encoding->count = 0;

        error = cff_charset_compute_cids( charset, num_glyphs,
                                          stream->memory );
        if ( error )
          goto Exit;

        for ( j = 0; j < 256; j++ )
        {
          FT_UInt  sid = encoding->sids[j];
          FT_UInt  gid = 0;


          if ( sid )
            gid = cff_charset_cid_to_gindex( charset, sid );

          if ( gid != 0 )
          {
            encoding->codes[j] = (FT_UShort)gid;
            encoding->count    = j + 1;
          }
          else
          {
            encoding->codes[j] = 0;
            encoding->sids [j] = 0;
          }
        }
        break;

      default:
        FT_ERROR(( "cff_encoding_load: invalid table format\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }
    }

  Exit:

    /* Clean up if there was an error. */
    return error;
  }


  static FT_Error
  cff_subfont_load( CFF_SubFont  font,
                    CFF_Index    idx,
                    FT_UInt      font_index,
                    FT_Stream    stream,
                    FT_ULong     base_offset,
                    FT_Library   library )
  {
    FT_Error         error;
    CFF_ParserRec    parser;
    FT_Byte*         dict = NULL;
    FT_ULong         dict_len;
    CFF_FontRecDict  top  = &font->font_dict;
    CFF_Private      priv = &font->private_dict;


    cff_parser_init( &parser, CFF_CODE_TOPDICT, &font->font_dict, library );

    /* set defaults */
    FT_MEM_ZERO( top, sizeof ( *top ) );

    top->underline_position  = -( 100L << 16 );
    top->underline_thickness = 50L << 16;
    top->charstring_type     = 2;
    top->font_matrix.xx      = 0x10000L;
    top->font_matrix.yy      = 0x10000L;
    top->cid_count           = 8720;

    /* we use the implementation specific SID value 0xFFFF to indicate */
    /* missing entries                                                 */
    top->version             = 0xFFFFU;
    top->notice              = 0xFFFFU;
    top->copyright           = 0xFFFFU;
    top->full_name           = 0xFFFFU;
    top->family_name         = 0xFFFFU;
    top->weight              = 0xFFFFU;
    top->embedded_postscript = 0xFFFFU;

    top->cid_registry        = 0xFFFFU;
    top->cid_ordering        = 0xFFFFU;
    top->cid_font_name       = 0xFFFFU;

    error = cff_index_access_element( idx, font_index, &dict, &dict_len );
    if ( !error )
    {
      FT_TRACE4(( " top dictionary:\n" ));
      error = cff_parser_run( &parser, dict, dict + dict_len );
    }

    cff_index_forget_element( idx, &dict );

    if ( error )
      goto Exit;

    /* if it is a CID font, we stop there */
    if ( top->cid_registry != 0xFFFFU )
      goto Exit;

    /* parse the private dictionary, if any */
    if ( top->private_offset && top->private_size )
    {
      /* set defaults */
      FT_MEM_ZERO( priv, sizeof ( *priv ) );

      priv->blue_shift       = 7;
      priv->blue_fuzz        = 1;
      priv->lenIV            = -1;
      priv->expansion_factor = (FT_Fixed)( 0.06 * 0x10000L );
      priv->blue_scale       = (FT_Fixed)( 0.039625 * 0x10000L * 1000 );

      cff_parser_init( &parser, CFF_CODE_PRIVATE, priv, library );

      if ( FT_STREAM_SEEK( base_offset + font->font_dict.private_offset ) ||
           FT_FRAME_ENTER( font->font_dict.private_size )                 )
        goto Exit;

      FT_TRACE4(( " private dictionary:\n" ));
      error = cff_parser_run( &parser,
                              (FT_Byte*)stream->cursor,
                              (FT_Byte*)stream->limit );
      FT_FRAME_EXIT();
      if ( error )
        goto Exit;

      /* ensure that `num_blue_values' is even */
      priv->num_blue_values &= ~1;
    }

    /* read the local subrs, if any */
    if ( priv->local_subrs_offset )
    {
      if ( FT_STREAM_SEEK( base_offset + top->private_offset +
                           priv->local_subrs_offset ) )
        goto Exit;

      error = cff_index_init( &font->local_subrs_index, stream, 1 );
      if ( error )
        goto Exit;

      error = cff_index_get_pointers( &font->local_subrs_index,
                                      &font->local_subrs, NULL );
      if ( error )
        goto Exit;
    }

  Exit:
    return error;
  }


  static void
  cff_subfont_done( FT_Memory    memory,
                    CFF_SubFont  subfont )
  {
    if ( subfont )
    {
      cff_index_done( &subfont->local_subrs_index );
      FT_FREE( subfont->local_subrs );
    }
  }


  FT_LOCAL_DEF( FT_Error )
  cff_font_load( FT_Library library,
                 FT_Stream  stream,
                 FT_Int     face_index,
                 CFF_Font   font,
                 FT_Bool    pure_cff )
  {
    static const FT_Frame_Field  cff_header_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  CFF_FontRec

      FT_FRAME_START( 4 ),
        FT_FRAME_BYTE( version_major ),
        FT_FRAME_BYTE( version_minor ),
        FT_FRAME_BYTE( header_size ),
        FT_FRAME_BYTE( absolute_offsize ),
      FT_FRAME_END
    };

    FT_Error         error;
    FT_Memory        memory = stream->memory;
    FT_ULong         base_offset;
    CFF_FontRecDict  dict;
    CFF_IndexRec     string_index;
    FT_Int           subfont_index;


    FT_ZERO( font );
    FT_ZERO( &string_index );

    font->stream = stream;
    font->memory = memory;
    dict         = &font->top_font.font_dict;
    base_offset  = FT_STREAM_POS();

    /* read CFF font header */
    if ( FT_STREAM_READ_FIELDS( cff_header_fields, font ) )
      goto Exit;

    /* check format */
    if ( font->version_major   != 1 ||
         font->header_size      < 4 ||
         font->absolute_offsize > 4 )
    {
      FT_TRACE2(( "  not a CFF font header\n" ));
      error = FT_THROW( Unknown_File_Format );
      goto Exit;
    }

    /* skip the rest of the header */
    if ( FT_STREAM_SKIP( font->header_size - 4 ) )
      goto Exit;

    /* read the name, top dict, string and global subrs index */
    if ( FT_SET_ERROR( cff_index_init( &font->name_index,
                                       stream, 0 ) )                  ||
         FT_SET_ERROR( cff_index_init( &font->font_dict_index,
                                       stream, 0 ) )                  ||
         FT_SET_ERROR( cff_index_init( &string_index,
                                       stream, 1 ) )                  ||
         FT_SET_ERROR( cff_index_init( &font->global_subrs_index,
                                       stream, 1 ) )                  ||
         FT_SET_ERROR( cff_index_get_pointers( &string_index,
                                               &font->strings,
                                               &font->string_pool ) ) )
      goto Exit;

    font->num_strings = string_index.count;

    if ( pure_cff )
    {
      /* well, we don't really forget the `disabled' fonts... */
      subfont_index = face_index;

      if ( subfont_index >= (FT_Int)font->name_index.count )
      {
        FT_ERROR(( "cff_font_load:"
                   " invalid subfont index for pure CFF font (%d)\n",
                   subfont_index ));
        error = FT_THROW( Invalid_Argument );
        goto Exit;
      }

      font->num_faces = font->name_index.count;
    }
    else
    {
      subfont_index = 0;

      if ( font->name_index.count > 1 )
      {
        FT_ERROR(( "cff_font_load:"
                   " invalid CFF font with multiple subfonts\n"
                   "              "
                   " in SFNT wrapper\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }
    }

    /* in case of a font format check, simply exit now */
    if ( face_index < 0 )
      goto Exit;

    /* now, parse the top-level font dictionary */
    FT_TRACE4(( "parsing top-level\n" ));
    error = cff_subfont_load( &font->top_font,
                              &font->font_dict_index,
                              subfont_index,
                              stream,
                              base_offset,
                              library );
    if ( error )
      goto Exit;

    if ( FT_STREAM_SEEK( base_offset + dict->charstrings_offset ) )
      goto Exit;

    error = cff_index_init( &font->charstrings_index, stream, 0 );
    if ( error )
      goto Exit;

    /* now, check for a CID font */
    if ( dict->cid_registry != 0xFFFFU )
    {
      CFF_IndexRec  fd_index;
      CFF_SubFont   sub = NULL;
      FT_UInt       idx;


      /* this is a CID-keyed font, we must now allocate a table of */
      /* sub-fonts, then load each of them separately              */
      if ( FT_STREAM_SEEK( base_offset + dict->cid_fd_array_offset ) )
        goto Exit;

      error = cff_index_init( &fd_index, stream, 0 );
      if ( error )
        goto Exit;

      if ( fd_index.count > CFF_MAX_CID_FONTS )
      {
        FT_TRACE0(( "cff_font_load: FD array too large in CID font\n" ));
        goto Fail_CID;
      }

      /* allocate & read each font dict independently */
      font->num_subfonts = fd_index.count;
      if ( FT_NEW_ARRAY( sub, fd_index.count ) )
        goto Fail_CID;

      /* set up pointer table */
      for ( idx = 0; idx < fd_index.count; idx++ )
        font->subfonts[idx] = sub + idx;

      /* now load each subfont independently */
      for ( idx = 0; idx < fd_index.count; idx++ )
      {
        sub = font->subfonts[idx];
        FT_TRACE4(( "parsing subfont %u\n", idx ));
        error = cff_subfont_load( sub, &fd_index, idx,
                                  stream, base_offset, library );
        if ( error )
          goto Fail_CID;
      }

      /* now load the FD Select array */
      error = CFF_Load_FD_Select( &font->fd_select,
                                  font->charstrings_index.count,
                                  stream,
                                  base_offset + dict->cid_fd_select_offset );

    Fail_CID:
      cff_index_done( &fd_index );

      if ( error )
        goto Exit;
    }
    else
      font->num_subfonts = 0;

    /* read the charstrings index now */
    if ( dict->charstrings_offset == 0 )
    {
      FT_ERROR(( "cff_font_load: no charstrings offset\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Exit;
    }

    font->num_glyphs = font->charstrings_index.count;

    error = cff_index_get_pointers( &font->global_subrs_index,
                                    &font->global_subrs, NULL );

    if ( error )
      goto Exit;

    /* read the Charset and Encoding tables if available */
    if ( font->num_glyphs > 0 )
    {
      FT_Bool  invert = FT_BOOL( dict->cid_registry != 0xFFFFU && pure_cff );


      error = cff_charset_load( &font->charset, font->num_glyphs, stream,
                                base_offset, dict->charset_offset, invert );
      if ( error )
        goto Exit;

      /* CID-keyed CFFs don't have an encoding */
      if ( dict->cid_registry == 0xFFFFU )
      {
        error = cff_encoding_load( &font->encoding,
                                   &font->charset,
                                   font->num_glyphs,
                                   stream,
                                   base_offset,
                                   dict->encoding_offset );
        if ( error )
          goto Exit;
      }
    }

    /* get the font name (/CIDFontName for CID-keyed fonts, */
    /* /FontName otherwise)                                 */
    font->font_name = cff_index_get_name( font, subfont_index );

  Exit:
    cff_index_done( &string_index );

    return error;
  }


  FT_LOCAL_DEF( void )
  cff_font_done( CFF_Font  font )
  {
    FT_Memory  memory = font->memory;
    FT_UInt    idx;


    cff_index_done( &font->global_subrs_index );
    cff_index_done( &font->font_dict_index );
    cff_index_done( &font->name_index );
    cff_index_done( &font->charstrings_index );

    /* release font dictionaries, but only if working with */
    /* a CID keyed CFF font                                */
    if ( font->num_subfonts > 0 )
    {
      for ( idx = 0; idx < font->num_subfonts; idx++ )
        cff_subfont_done( memory, font->subfonts[idx] );

      /* the subfonts array has been allocated as a single block */
      FT_FREE( font->subfonts[0] );
    }

    cff_encoding_done( &font->encoding );
    cff_charset_done( &font->charset, font->stream );

    cff_subfont_done( memory, &font->top_font );

    CFF_Done_FD_Select( &font->fd_select, font->stream );

    FT_FREE( font->font_info );

    FT_FREE( font->font_name );
    FT_FREE( font->global_subrs );
    FT_FREE( font->strings );
    FT_FREE( font->string_pool );

    if ( font->cf2_instance.finalizer )
    {
      font->cf2_instance.finalizer( font->cf2_instance.data );
      FT_FREE( font->cf2_instance.data );
    }
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cffobjs.c                                                              */
/*                                                                         */
/*    OpenType objects manager (body).                                     */
/*                                                                         */
/*  Copyright 1996-2013 by                                                 */
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
#include FT_INTERNAL_CALC_H
#include FT_INTERNAL_STREAM_H
#include FT_ERRORS_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TAGS_H
#include FT_INTERNAL_SFNT_H
#include FT_CFF_DRIVER_H

#include "cffobjs.h"
#include "cffload.h"
#include "cffcmap.h"
#include "cffpic.h"

#include "cfferrs.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cffobjs


  /*************************************************************************/
  /*                                                                       */
  /*                            SIZE FUNCTIONS                             */
  /*                                                                       */
  /*  Note that we store the global hints in the size's `internal' root    */
  /*  field.                                                               */
  /*                                                                       */
  /*************************************************************************/


  static PSH_Globals_Funcs
  cff_size_get_globals_funcs( CFF_Size  size )
  {
    CFF_Face          face     = (CFF_Face)size->root.face;
    CFF_Font          font     = (CFF_Font)face->extra.data;
    PSHinter_Service  pshinter = font->pshinter;
    FT_Module         module;


    module = FT_Get_Module( size->root.face->driver->root.library,
                            "pshinter" );
    return ( module && pshinter && pshinter->get_globals_funcs )
           ? pshinter->get_globals_funcs( module )
           : 0;
  }


  FT_LOCAL_DEF( void )
  cff_size_done( FT_Size  cffsize )        /* CFF_Size */
  {
    CFF_Size      size     = (CFF_Size)cffsize;
    CFF_Face      face     = (CFF_Face)size->root.face;
    CFF_Font      font     = (CFF_Font)face->extra.data;
    CFF_Internal  internal = (CFF_Internal)cffsize->internal;


    if ( internal )
    {
      PSH_Globals_Funcs  funcs;


      funcs = cff_size_get_globals_funcs( size );
      if ( funcs )
      {
        FT_UInt  i;


        funcs->destroy( internal->topfont );

        for ( i = font->num_subfonts; i > 0; i-- )
          funcs->destroy( internal->subfonts[i - 1] );
      }

      /* `internal' is freed by destroy_size (in ftobjs.c) */
    }
  }


  /* CFF and Type 1 private dictionaries have slightly different      */
  /* structures; we need to synthesize a Type 1 dictionary on the fly */

  static void
  cff_make_private_dict( CFF_SubFont  subfont,
                         PS_Private   priv )
  {
    CFF_Private  cpriv = &subfont->private_dict;
    FT_UInt      n, count;


    FT_MEM_ZERO( priv, sizeof ( *priv ) );

    count = priv->num_blue_values = cpriv->num_blue_values;
    for ( n = 0; n < count; n++ )
      priv->blue_values[n] = (FT_Short)cpriv->blue_values[n];

    count = priv->num_other_blues = cpriv->num_other_blues;
    for ( n = 0; n < count; n++ )
      priv->other_blues[n] = (FT_Short)cpriv->other_blues[n];

    count = priv->num_family_blues = cpriv->num_family_blues;
    for ( n = 0; n < count; n++ )
      priv->family_blues[n] = (FT_Short)cpriv->family_blues[n];

    count = priv->num_family_other_blues = cpriv->num_family_other_blues;
    for ( n = 0; n < count; n++ )
      priv->family_other_blues[n] = (FT_Short)cpriv->family_other_blues[n];

    priv->blue_scale = cpriv->blue_scale;
    priv->blue_shift = (FT_Int)cpriv->blue_shift;
    priv->blue_fuzz  = (FT_Int)cpriv->blue_fuzz;

    priv->standard_width[0]  = (FT_UShort)cpriv->standard_width;
    priv->standard_height[0] = (FT_UShort)cpriv->standard_height;

    count = priv->num_snap_widths = cpriv->num_snap_widths;
    for ( n = 0; n < count; n++ )
      priv->snap_widths[n] = (FT_Short)cpriv->snap_widths[n];

    count = priv->num_snap_heights = cpriv->num_snap_heights;
    for ( n = 0; n < count; n++ )
      priv->snap_heights[n] = (FT_Short)cpriv->snap_heights[n];

    priv->force_bold     = cpriv->force_bold;
    priv->language_group = cpriv->language_group;
    priv->lenIV          = cpriv->lenIV;
  }


  FT_LOCAL_DEF( FT_Error )
  cff_size_init( FT_Size  cffsize )         /* CFF_Size */
  {
    CFF_Size           size  = (CFF_Size)cffsize;
    FT_Error           error = FT_Err_Ok;
    PSH_Globals_Funcs  funcs = cff_size_get_globals_funcs( size );


    if ( funcs )
    {
      CFF_Face      face     = (CFF_Face)cffsize->face;
      CFF_Font      font     = (CFF_Font)face->extra.data;
      CFF_Internal  internal = NULL;

      PS_PrivateRec  priv;
      FT_Memory      memory = cffsize->face->memory;

      FT_UInt  i;


      if ( FT_NEW( internal ) )
        goto Exit;

      cff_make_private_dict( &font->top_font, &priv );
      error = funcs->create( cffsize->face->memory, &priv,
                             &internal->topfont );
      if ( error )
        goto Exit;

      for ( i = font->num_subfonts; i > 0; i-- )
      {
        CFF_SubFont  sub = font->subfonts[i - 1];


        cff_make_private_dict( sub, &priv );
        error = funcs->create( cffsize->face->memory, &priv,
                               &internal->subfonts[i - 1] );
        if ( error )
          goto Exit;
      }

      cffsize->internal = (FT_Size_Internal)(void*)internal;
    }

    size->strike_index = 0xFFFFFFFFUL;

  Exit:
    return error;
  }


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

  FT_LOCAL_DEF( FT_Error )
  cff_size_select( FT_Size   size,
                   FT_ULong  strike_index )
  {
    CFF_Size           cffsize = (CFF_Size)size;
    PSH_Globals_Funcs  funcs;


    cffsize->strike_index = strike_index;

    FT_Select_Metrics( size->face, strike_index );

    funcs = cff_size_get_globals_funcs( cffsize );

    if ( funcs )
    {
      CFF_Face      face     = (CFF_Face)size->face;
      CFF_Font      font     = (CFF_Font)face->extra.data;
      CFF_Internal  internal = (CFF_Internal)size->internal;

      FT_ULong  top_upm  = font->top_font.font_dict.units_per_em;
      FT_UInt   i;


      funcs->set_scale( internal->topfont,
                        size->metrics.x_scale, size->metrics.y_scale,
                        0, 0 );

      for ( i = font->num_subfonts; i > 0; i-- )
      {
        CFF_SubFont  sub     = font->subfonts[i - 1];
        FT_ULong     sub_upm = sub->font_dict.units_per_em;
        FT_Pos       x_scale, y_scale;


        if ( top_upm != sub_upm )
        {
          x_scale = FT_MulDiv( size->metrics.x_scale, top_upm, sub_upm );
          y_scale = FT_MulDiv( size->metrics.y_scale, top_upm, sub_upm );
        }
        else
        {
          x_scale = size->metrics.x_scale;
          y_scale = size->metrics.y_scale;
        }

        funcs->set_scale( internal->subfonts[i - 1],
                          x_scale, y_scale, 0, 0 );
      }
    }

    return FT_Err_Ok;
  }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */


  FT_LOCAL_DEF( FT_Error )
  cff_size_request( FT_Size          size,
                    FT_Size_Request  req )
  {
    CFF_Size           cffsize = (CFF_Size)size;
    PSH_Globals_Funcs  funcs;


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

    if ( FT_HAS_FIXED_SIZES( size->face ) )
    {
      CFF_Face      cffface = (CFF_Face)size->face;
      SFNT_Service  sfnt    = (SFNT_Service)cffface->sfnt;
      FT_ULong      strike_index;


      if ( sfnt->set_sbit_strike( cffface, req, &strike_index ) )
        cffsize->strike_index = 0xFFFFFFFFUL;
      else
        return cff_size_select( size, strike_index );
    }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */

    FT_Request_Metrics( size->face, req );

    funcs = cff_size_get_globals_funcs( cffsize );

    if ( funcs )
    {
      CFF_Face      cffface  = (CFF_Face)size->face;
      CFF_Font      font     = (CFF_Font)cffface->extra.data;
      CFF_Internal  internal = (CFF_Internal)size->internal;

      FT_ULong  top_upm  = font->top_font.font_dict.units_per_em;
      FT_UInt   i;


      funcs->set_scale( internal->topfont,
                        size->metrics.x_scale, size->metrics.y_scale,
                        0, 0 );

      for ( i = font->num_subfonts; i > 0; i-- )
      {
        CFF_SubFont  sub     = font->subfonts[i - 1];
        FT_ULong     sub_upm = sub->font_dict.units_per_em;
        FT_Pos       x_scale, y_scale;


        if ( top_upm != sub_upm )
        {
          x_scale = FT_MulDiv( size->metrics.x_scale, top_upm, sub_upm );
          y_scale = FT_MulDiv( size->metrics.y_scale, top_upm, sub_upm );
        }
        else
        {
          x_scale = size->metrics.x_scale;
          y_scale = size->metrics.y_scale;
        }

        funcs->set_scale( internal->subfonts[i - 1],
                          x_scale, y_scale, 0, 0 );
      }
    }

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /*                            SLOT  FUNCTIONS                            */
  /*                                                                       */
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  cff_slot_done( FT_GlyphSlot  slot )
  {
    slot->internal->glyph_hints = 0;
  }


  FT_LOCAL_DEF( FT_Error )
  cff_slot_init( FT_GlyphSlot  slot )
  {
    CFF_Face          face     = (CFF_Face)slot->face;
    CFF_Font          font     = (CFF_Font)face->extra.data;
    PSHinter_Service  pshinter = font->pshinter;


    if ( pshinter )
    {
      FT_Module  module;


      module = FT_Get_Module( slot->face->driver->root.library,
                              "pshinter" );
      if ( module )
      {
        T2_Hints_Funcs  funcs;


        funcs = pshinter->get_t2_funcs( module );
        slot->internal->glyph_hints = (void*)funcs;
      }
    }

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /*                           FACE  FUNCTIONS                             */
  /*                                                                       */
  /*************************************************************************/

  static FT_String*
  cff_strcpy( FT_Memory         memory,
              const FT_String*  source )
  {
    FT_Error    error;
    FT_String*  result;


    (void)FT_STRDUP( result, source );

    FT_UNUSED( error );

    return result;
  }


  /* Strip all subset prefixes of the form `ABCDEF+'.  Usually, there */
  /* is only one, but font names like `APCOOG+JFABTD+FuturaBQ-Bold'   */
  /* have been seen in the wild.                                      */

  static void
  remove_subset_prefix( FT_String*  name )
  {
    FT_Int32  idx             = 0;
    FT_Int32  length          = (FT_Int32)strlen( name ) + 1;
    FT_Bool   continue_search = 1;


    while ( continue_search )
    {
      if ( length >= 7 && name[6] == '+' )
      {
        for ( idx = 0; idx < 6; idx++ )
        {
          /* ASCII uppercase letters */
          if ( !( 'A' <= name[idx] && name[idx] <= 'Z' ) )
            continue_search = 0;
        }

        if ( continue_search )
        {
          for ( idx = 7; idx < length; idx++ )
            name[idx - 7] = name[idx];
          length -= 7;
        }
      }
      else
        continue_search = 0;
    }
  }


  /* Remove the style part from the family name (if present). */

  static void
  remove_style( FT_String*        family_name,
                const FT_String*  style_name )
  {
    FT_Int32  family_name_length, style_name_length;


    family_name_length = (FT_Int32)strlen( family_name );
    style_name_length  = (FT_Int32)strlen( style_name );

    if ( family_name_length > style_name_length )
    {
      FT_Int  idx;


      for ( idx = 1; idx <= style_name_length; ++idx )
      {
        if ( family_name[family_name_length - idx] !=
             style_name[style_name_length - idx] )
          break;
      }

      if ( idx > style_name_length )
      {
        /* family_name ends with style_name; remove it */
        idx = family_name_length - style_name_length - 1;

        /* also remove special characters     */
        /* between real family name and style */
        while ( idx > 0                     &&
                ( family_name[idx] == '-' ||
                  family_name[idx] == ' ' ||
                  family_name[idx] == '_' ||
                  family_name[idx] == '+' ) )
          --idx;

        if ( idx > 0 )
          family_name[idx + 1] = '\0';
      }
    }
  }


  FT_LOCAL_DEF( FT_Error )
  cff_face_init( FT_Stream      stream,
                 FT_Face        cffface,        /* CFF_Face */
                 FT_Int         face_index,
                 FT_Int         num_params,
                 FT_Parameter*  params )
  {
    CFF_Face            face        = (CFF_Face)cffface;
    FT_Error            error;
    SFNT_Service        sfnt;
    FT_Service_PsCMaps  psnames;
    PSHinter_Service    pshinter;
    FT_Bool             pure_cff    = 1;
    FT_Bool             sfnt_format = 0;
    FT_Library          library     = cffface->driver->root.library;


    sfnt = (SFNT_Service)FT_Get_Module_Interface(
             library, "sfnt" );
    if ( !sfnt )
    {
      FT_ERROR(( "cff_face_init: cannot access `sfnt' module\n" ));
      error = FT_THROW( Missing_Module );
      goto Exit;
    }

    FT_FACE_FIND_GLOBAL_SERVICE( face, psnames, POSTSCRIPT_CMAPS );

    pshinter = (PSHinter_Service)FT_Get_Module_Interface(
                 library, "pshinter" );

    FT_TRACE2(( "CFF driver\n" ));

    /* create input stream from resource */
    if ( FT_STREAM_SEEK( 0 ) )
      goto Exit;

    /* check whether we have a valid OpenType file */
    error = sfnt->init_face( stream, face, face_index, num_params, params );
    if ( !error )
    {
      if ( face->format_tag != TTAG_OTTO )  /* `OTTO'; OpenType/CFF font */
      {
        FT_TRACE2(( "  not an OpenType/CFF font\n" ));
        error = FT_THROW( Unknown_File_Format );
        goto Exit;
      }

      /* if we are performing a simple font format check, exit immediately */
      if ( face_index < 0 )
        return FT_Err_Ok;

      sfnt_format = 1;

      /* now, the font can be either an OpenType/CFF font, or an SVG CEF */
      /* font; in the latter case it doesn't have a `head' table         */
      error = face->goto_table( face, TTAG_head, stream, 0 );
      if ( !error )
      {
        pure_cff = 0;

        /* load font directory */
        error = sfnt->load_face( stream, face, face_index,
                                 num_params, params );
        if ( error )
          goto Exit;
      }
      else
      {
        /* load the `cmap' table explicitly */
        error = sfnt->load_cmap( face, stream );
        if ( error )
          goto Exit;
      }

      /* now load the CFF part of the file */
      error = face->goto_table( face, TTAG_CFF, stream, 0 );
      if ( error )
        goto Exit;
    }
    else
    {
      /* rewind to start of file; we are going to load a pure-CFF font */
      if ( FT_STREAM_SEEK( 0 ) )
        goto Exit;
      error = FT_Err_Ok;
    }

    /* now load and parse the CFF table in the file */
    {
      CFF_Font         cff = NULL;
      CFF_FontRecDict  dict;
      FT_Memory        memory = cffface->memory;
      FT_Int32         flags;
      FT_UInt          i;


      if ( FT_NEW( cff ) )
        goto Exit;

      face->extra.data = cff;
      error = cff_font_load( library, stream, face_index, cff, pure_cff );
      if ( error )
        goto Exit;

      cff->pshinter = pshinter;
      cff->psnames  = psnames;

      cffface->face_index = face_index;

      /* Complement the root flags with some interesting information. */
      /* Note that this is only necessary for pure CFF and CEF fonts; */
      /* SFNT based fonts use the `name' table instead.               */

      cffface->num_glyphs = cff->num_glyphs;

      dict = &cff->top_font.font_dict;

      /* we need the `PSNames' module for CFF and CEF formats */
      /* which aren't CID-keyed                               */
      if ( dict->cid_registry == 0xFFFFU && !psnames )
      {
        FT_ERROR(( "cff_face_init:"
                   " cannot open CFF & CEF fonts\n"
                   "              "
                   " without the `PSNames' module\n" ));
        error = FT_THROW( Missing_Module );
        goto Exit;
      }

#ifdef FT_DEBUG_LEVEL_TRACE
      {
        FT_UInt     idx;
        FT_String*  s;


        FT_TRACE4(( "SIDs\n" ));

        /* dump string index, including default strings for convenience */
        for ( idx = 0; idx < cff->num_strings + 390; idx++ )
        {
          s = cff_index_get_sid_string( cff, idx );
          if ( s )
            FT_TRACE4(("  %5d %s\n", idx, s ));
        }
      }
#endif /* FT_DEBUG_LEVEL_TRACE */

      if ( !dict->has_font_matrix )
        dict->units_per_em = pure_cff ? 1000 : face->root.units_per_EM;

      /* Normalize the font matrix so that `matrix->xx' is 1; the */
      /* scaling is done with `units_per_em' then (at this point, */
      /* it already contains the scaling factor, but without      */
      /* normalization of the matrix).                            */
      /*                                                          */
      /* Note that the offsets must be expressed in integer font  */
      /* units.                                                   */

      {
        FT_Matrix*  matrix = &dict->font_matrix;
        FT_Vector*  offset = &dict->font_offset;
        FT_ULong*   upm    = &dict->units_per_em;
        FT_Fixed    temp   = FT_ABS( matrix->yy );


        if ( temp != 0x10000L )
        {
          *upm = FT_DivFix( *upm, temp );

          matrix->xx = FT_DivFix( matrix->xx, temp );
          matrix->yx = FT_DivFix( matrix->yx, temp );
          matrix->xy = FT_DivFix( matrix->xy, temp );
          matrix->yy = FT_DivFix( matrix->yy, temp );
          offset->x  = FT_DivFix( offset->x,  temp );
          offset->y  = FT_DivFix( offset->y,  temp );
        }

        offset->x >>= 16;
        offset->y >>= 16;
      }

      for ( i = cff->num_subfonts; i > 0; i-- )
      {
        CFF_FontRecDict  sub = &cff->subfonts[i - 1]->font_dict;
        CFF_FontRecDict  top = &cff->top_font.font_dict;

        FT_Matrix*  matrix;
        FT_Vector*  offset;
        FT_ULong*   upm;
        FT_Fixed    temp;


        if ( sub->has_font_matrix )
        {
          FT_Long  scaling;


          /* if we have a top-level matrix, */
          /* concatenate the subfont matrix */

          if ( top->has_font_matrix )
          {
            if ( top->units_per_em > 1 && sub->units_per_em > 1 )
              scaling = FT_MIN( top->units_per_em, sub->units_per_em );
            else
              scaling = 1;

            FT_Matrix_Multiply_Scaled( &top->font_matrix,
                                       &sub->font_matrix,
                                       scaling );
            FT_Vector_Transform_Scaled( &sub->font_offset,
                                        &top->font_matrix,
                                        scaling );

            sub->units_per_em = FT_MulDiv( sub->units_per_em,
                                           top->units_per_em,
                                           scaling );
          }
        }
        else
        {
          sub->font_matrix = top->font_matrix;
          sub->font_offset = top->font_offset;

          sub->units_per_em = top->units_per_em;
        }

        matrix = &sub->font_matrix;
        offset = &sub->font_offset;
        upm    = &sub->units_per_em;
        temp   = FT_ABS( matrix->yy );

        if ( temp != 0x10000L )
        {
          *upm = FT_DivFix( *upm, temp );

          matrix->xx = FT_DivFix( matrix->xx, temp );
          matrix->yx = FT_DivFix( matrix->yx, temp );
          matrix->xy = FT_DivFix( matrix->xy, temp );
          matrix->yy = FT_DivFix( matrix->yy, temp );
          offset->x  = FT_DivFix( offset->x,  temp );
          offset->y  = FT_DivFix( offset->y,  temp );
        }

        offset->x >>= 16;
        offset->y >>= 16;
      }

      if ( pure_cff )
      {
        char*  style_name = NULL;


        /* set up num_faces */
        cffface->num_faces = cff->num_faces;

        /* compute number of glyphs */
        if ( dict->cid_registry != 0xFFFFU )
          cffface->num_glyphs = cff->charset.max_cid + 1;
        else
          cffface->num_glyphs = cff->charstrings_index.count;

        /* set global bbox, as well as EM size */
        cffface->bbox.xMin =   dict->font_bbox.xMin            >> 16;
        cffface->bbox.yMin =   dict->font_bbox.yMin            >> 16;
        /* no `U' suffix here to 0xFFFF! */
        cffface->bbox.xMax = ( dict->font_bbox.xMax + 0xFFFF ) >> 16;
        cffface->bbox.yMax = ( dict->font_bbox.yMax + 0xFFFF ) >> 16;

        cffface->units_per_EM = (FT_UShort)( dict->units_per_em );

        cffface->ascender  = (FT_Short)( cffface->bbox.yMax );
        cffface->descender = (FT_Short)( cffface->bbox.yMin );

        cffface->height = (FT_Short)( ( cffface->units_per_EM * 12 ) / 10 );
        if ( cffface->height < cffface->ascender - cffface->descender )
          cffface->height = (FT_Short)( cffface->ascender - cffface->descender );

        cffface->underline_position  =
          (FT_Short)( dict->underline_position >> 16 );
        cffface->underline_thickness =
          (FT_Short)( dict->underline_thickness >> 16 );

        /* retrieve font family & style name */
        cffface->family_name = cff_index_get_name( cff, face_index );
        if ( cffface->family_name )
        {
          char*  full   = cff_index_get_sid_string( cff,
                                                    dict->full_name );
          char*  fullp  = full;
          char*  family = cffface->family_name;
          char*  family_name = NULL;


          remove_subset_prefix( cffface->family_name );

          if ( dict->family_name )
          {
            family_name = cff_index_get_sid_string( cff,
                                                    dict->family_name );
            if ( family_name )
              family = family_name;
          }

          /* We try to extract the style name from the full name.   */
          /* We need to ignore spaces and dashes during the search. */
          if ( full && family )
          {
            while ( *fullp )
            {
              /* skip common characters at the start of both strings */
              if ( *fullp == *family )
              {
                family++;
                fullp++;
                continue;
              }

              /* ignore spaces and dashes in full name during comparison */
              if ( *fullp == ' ' || *fullp == '-' )
              {
                fullp++;
                continue;
              }

              /* ignore spaces and dashes in family name during comparison */
              if ( *family == ' ' || *family == '-' )
              {
                family++;
                continue;
              }

              if ( !*family && *fullp )
              {
                /* The full name begins with the same characters as the  */
                /* family name, with spaces and dashes removed.  In this */
                /* case, the remaining string in `fullp' will be used as */
                /* the style name.                                       */
                style_name = cff_strcpy( memory, fullp );

                /* remove the style part from the family name (if present) */
                remove_style( cffface->family_name, style_name );
              }
              break;
            }
          }
        }
        else
        {
          char  *cid_font_name =
                   cff_index_get_sid_string( cff,
                                             dict->cid_font_name );


          /* do we have a `/FontName' for a CID-keyed font? */
          if ( cid_font_name )
            cffface->family_name = cff_strcpy( memory, cid_font_name );
        }

        if ( style_name )
          cffface->style_name = style_name;
        else
          /* assume "Regular" style if we don't know better */
          cffface->style_name = cff_strcpy( memory, (char *)"Regular" );

        /*******************************************************************/
        /*                                                                 */
        /* Compute face flags.                                             */
        /*                                                                 */
        flags = FT_FACE_FLAG_SCALABLE   | /* scalable outlines */
                FT_FACE_FLAG_HORIZONTAL | /* horizontal data   */
                FT_FACE_FLAG_HINTER;      /* has native hinter */

        if ( sfnt_format )
          flags |= FT_FACE_FLAG_SFNT;

        /* fixed width font? */
        if ( dict->is_fixed_pitch )
          flags |= FT_FACE_FLAG_FIXED_WIDTH;

  /* XXX: WE DO NOT SUPPORT KERNING METRICS IN THE GPOS TABLE FOR NOW */
#if 0
        /* kerning available? */
        if ( face->kern_pairs )
          flags |= FT_FACE_FLAG_KERNING;
#endif

        cffface->face_flags |= flags;

        /*******************************************************************/
        /*                                                                 */
        /* Compute style flags.                                            */
        /*                                                                 */
        flags = 0;

        if ( dict->italic_angle )
          flags |= FT_STYLE_FLAG_ITALIC;

        {
          char  *weight = cff_index_get_sid_string( cff,
                                                    dict->weight );


          if ( weight )
            if ( !ft_strcmp( weight, "Bold"  ) ||
                 !ft_strcmp( weight, "Black" ) )
              flags |= FT_STYLE_FLAG_BOLD;
        }

        /* double check */
        if ( !(flags & FT_STYLE_FLAG_BOLD) && cffface->style_name )
          if ( !ft_strncmp( cffface->style_name, "Bold", 4 )  ||
               !ft_strncmp( cffface->style_name, "Black", 5 ) )
            flags |= FT_STYLE_FLAG_BOLD;

        cffface->style_flags = flags;
      }


#ifndef FT_CONFIG_OPTION_NO_GLYPH_NAMES
      /* CID-keyed CFF fonts don't have glyph names -- the SFNT loader */
      /* has unset this flag because of the 3.0 `post' table.          */
      if ( dict->cid_registry == 0xFFFFU )
        cffface->face_flags |= FT_FACE_FLAG_GLYPH_NAMES;
#endif

      if ( dict->cid_registry != 0xFFFFU && pure_cff )
        cffface->face_flags |= FT_FACE_FLAG_CID_KEYED;


      /*******************************************************************/
      /*                                                                 */
      /* Compute char maps.                                              */
      /*                                                                 */

      /* Try to synthesize a Unicode charmap if there is none available */
      /* already.  If an OpenType font contains a Unicode "cmap", we    */
      /* will use it, whatever be in the CFF part of the file.          */
      {
        FT_CharMapRec  cmaprec;
        FT_CharMap     cmap;
        FT_UInt        nn;
        CFF_Encoding   encoding = &cff->encoding;


        for ( nn = 0; nn < (FT_UInt)cffface->num_charmaps; nn++ )
        {
          cmap = cffface->charmaps[nn];

          /* Windows Unicode? */
          if ( cmap->platform_id == TT_PLATFORM_MICROSOFT &&
               cmap->encoding_id == TT_MS_ID_UNICODE_CS   )
            goto Skip_Unicode;

          /* Apple Unicode platform id? */
          if ( cmap->platform_id == TT_PLATFORM_APPLE_UNICODE )
            goto Skip_Unicode; /* Apple Unicode */
        }

        /* since CID-keyed fonts don't contain glyph names, we can't */
        /* construct a cmap                                          */
        if ( pure_cff && cff->top_font.font_dict.cid_registry != 0xFFFFU )
          goto Exit;

#ifdef FT_MAX_CHARMAP_CACHEABLE
        if ( nn + 1 > FT_MAX_CHARMAP_CACHEABLE )
        {
          FT_ERROR(( "cff_face_init: no Unicode cmap is found, "
                     "and too many subtables (%d) to add synthesized cmap\n",
                     nn ));
          goto Exit;
        }
#endif

        /* we didn't find a Unicode charmap -- synthesize one */
        cmaprec.face        = cffface;
        cmaprec.platform_id = TT_PLATFORM_MICROSOFT;
        cmaprec.encoding_id = TT_MS_ID_UNICODE_CS;
        cmaprec.encoding    = FT_ENCODING_UNICODE;

        nn = (FT_UInt)cffface->num_charmaps;

        error = FT_CMap_New( &CFF_CMAP_UNICODE_CLASS_REC_GET, NULL,
                             &cmaprec, NULL );
        if ( error                                      &&
             FT_ERR_NEQ( error, No_Unicode_Glyph_Name ) )
          goto Exit;
        error = FT_Err_Ok;

        /* if no Unicode charmap was previously selected, select this one */
        if ( cffface->charmap == NULL && nn != (FT_UInt)cffface->num_charmaps )
          cffface->charmap = cffface->charmaps[nn];

      Skip_Unicode:
#ifdef FT_MAX_CHARMAP_CACHEABLE
        if ( nn > FT_MAX_CHARMAP_CACHEABLE )
        {
          FT_ERROR(( "cff_face_init: Unicode cmap is found, "
                     "but too many preceding subtables (%d) to access\n",
                     nn - 1 ));
          goto Exit;
        }
#endif
        if ( encoding->count > 0 )
        {
          FT_CMap_Class  clazz;


          cmaprec.face        = cffface;
          cmaprec.platform_id = TT_PLATFORM_ADOBE;  /* Adobe platform id */

          if ( encoding->offset == 0 )
          {
            cmaprec.encoding_id = TT_ADOBE_ID_STANDARD;
            cmaprec.encoding    = FT_ENCODING_ADOBE_STANDARD;
            clazz               = &CFF_CMAP_ENCODING_CLASS_REC_GET;
          }
          else if ( encoding->offset == 1 )
          {
            cmaprec.encoding_id = TT_ADOBE_ID_EXPERT;
            cmaprec.encoding    = FT_ENCODING_ADOBE_EXPERT;
            clazz               = &CFF_CMAP_ENCODING_CLASS_REC_GET;
          }
          else
          {
            cmaprec.encoding_id = TT_ADOBE_ID_CUSTOM;
            cmaprec.encoding    = FT_ENCODING_ADOBE_CUSTOM;
            clazz               = &CFF_CMAP_ENCODING_CLASS_REC_GET;
          }

          error = FT_CMap_New( clazz, NULL, &cmaprec, NULL );
        }
      }
    }

  Exit:
    return error;
  }


  FT_LOCAL_DEF( void )
  cff_face_done( FT_Face  cffface )         /* CFF_Face */
  {
    CFF_Face      face = (CFF_Face)cffface;
    FT_Memory     memory;
    SFNT_Service  sfnt;


    if ( !face )
      return;

    memory = cffface->memory;
    sfnt   = (SFNT_Service)face->sfnt;

    if ( sfnt )
      sfnt->done_face( face );

    {
      CFF_Font  cff = (CFF_Font)face->extra.data;


      if ( cff )
      {
        cff_font_done( cff );
        FT_FREE( face->extra.data );
      }
    }
  }


  FT_LOCAL_DEF( FT_Error )
  cff_driver_init( FT_Module  module )        /* CFF_Driver */
  {
    CFF_Driver  driver = (CFF_Driver)module;


    /* set default property values, cf `ftcffdrv.h' */
#ifdef CFF_CONFIG_OPTION_OLD_ENGINE
    driver->hinting_engine    = FT_CFF_HINTING_FREETYPE;
#else
    driver->hinting_engine    = FT_CFF_HINTING_ADOBE;
#endif
    driver->no_stem_darkening = FALSE;

    driver->darken_params[0] =  500;
    driver->darken_params[1] =  400;
    driver->darken_params[2] = 1000;
    driver->darken_params[3] =  275;
    driver->darken_params[4] = 1667;
    driver->darken_params[5] =  275;
    driver->darken_params[6] = 2333;
    driver->darken_params[7] =    0;

    return FT_Err_Ok;
  }


  FT_LOCAL_DEF( void )
  cff_driver_done( FT_Module  module )        /* CFF_Driver */
  {
    FT_UNUSED( module );
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cffgload.c                                                             */
/*                                                                         */
/*    OpenType Glyph Loader (body).                                        */
/*                                                                         */
/*  Copyright 1996-2013 by                                                 */
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
#include FT_INTERNAL_SFNT_H
#include FT_OUTLINE_H
#include FT_CFF_DRIVER_H

#include "cffobjs.h"
#include "cffload.h"
#include "cffgload.h"
#include "cf2ft.h"      /* for cf2_decoder_parse_charstrings */

#include "cfferrs.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cffgload


#ifdef CFF_CONFIG_OPTION_OLD_ENGINE

  typedef enum  CFF_Operator_
  {
    cff_op_unknown = 0,

    cff_op_rmoveto,
    cff_op_hmoveto,
    cff_op_vmoveto,

    cff_op_rlineto,
    cff_op_hlineto,
    cff_op_vlineto,

    cff_op_rrcurveto,
    cff_op_hhcurveto,
    cff_op_hvcurveto,
    cff_op_rcurveline,
    cff_op_rlinecurve,
    cff_op_vhcurveto,
    cff_op_vvcurveto,

    cff_op_flex,
    cff_op_hflex,
    cff_op_hflex1,
    cff_op_flex1,

    cff_op_endchar,

    cff_op_hstem,
    cff_op_vstem,
    cff_op_hstemhm,
    cff_op_vstemhm,

    cff_op_hintmask,
    cff_op_cntrmask,
    cff_op_dotsection,  /* deprecated, acts as no-op */

    cff_op_abs,
    cff_op_add,
    cff_op_sub,
    cff_op_div,
    cff_op_neg,
    cff_op_random,
    cff_op_mul,
    cff_op_sqrt,

    cff_op_blend,

    cff_op_drop,
    cff_op_exch,
    cff_op_index,
    cff_op_roll,
    cff_op_dup,

    cff_op_put,
    cff_op_get,
    cff_op_store,
    cff_op_load,

    cff_op_and,
    cff_op_or,
    cff_op_not,
    cff_op_eq,
    cff_op_ifelse,

    cff_op_callsubr,
    cff_op_callgsubr,
    cff_op_return,

    /* Type 1 opcodes: invalid but seen in real life */
    cff_op_hsbw,
    cff_op_closepath,
    cff_op_callothersubr,
    cff_op_pop,
    cff_op_seac,
    cff_op_sbw,
    cff_op_setcurrentpoint,

    /* do not remove */
    cff_op_max

  } CFF_Operator;


#define CFF_COUNT_CHECK_WIDTH  0x80
#define CFF_COUNT_EXACT        0x40
#define CFF_COUNT_CLEAR_STACK  0x20

  /* count values which have the `CFF_COUNT_CHECK_WIDTH' flag set are  */
  /* used for checking the width and requested numbers of arguments    */
  /* only; they are set to zero afterwards                             */

  /* the other two flags are informative only and unused currently     */

  static const FT_Byte  cff_argument_counts[] =
  {
    0,  /* unknown */

    2 | CFF_COUNT_CHECK_WIDTH | CFF_COUNT_EXACT, /* rmoveto */
    1 | CFF_COUNT_CHECK_WIDTH | CFF_COUNT_EXACT,
    1 | CFF_COUNT_CHECK_WIDTH | CFF_COUNT_EXACT,

    0 | CFF_COUNT_CLEAR_STACK, /* rlineto */
    0 | CFF_COUNT_CLEAR_STACK,
    0 | CFF_COUNT_CLEAR_STACK,

    0 | CFF_COUNT_CLEAR_STACK, /* rrcurveto */
    0 | CFF_COUNT_CLEAR_STACK,
    0 | CFF_COUNT_CLEAR_STACK,
    0 | CFF_COUNT_CLEAR_STACK,
    0 | CFF_COUNT_CLEAR_STACK,
    0 | CFF_COUNT_CLEAR_STACK,
    0 | CFF_COUNT_CLEAR_STACK,

    13, /* flex */
    7,
    9,
    11,

    0 | CFF_COUNT_CHECK_WIDTH, /* endchar */

    2 | CFF_COUNT_CHECK_WIDTH, /* hstem */
    2 | CFF_COUNT_CHECK_WIDTH,
    2 | CFF_COUNT_CHECK_WIDTH,
    2 | CFF_COUNT_CHECK_WIDTH,

    0 | CFF_COUNT_CHECK_WIDTH, /* hintmask */
    0 | CFF_COUNT_CHECK_WIDTH, /* cntrmask */
    0, /* dotsection */

    1, /* abs */
    2,
    2,
    2,
    1,
    0,
    2,
    1,

    1, /* blend */

    1, /* drop */
    2,
    1,
    2,
    1,

    2, /* put */
    1,
    4,
    3,

    2, /* and */
    2,
    1,
    2,
    4,

    1, /* callsubr */
    1,
    0,

    2, /* hsbw */
    0,
    0,
    0,
    5, /* seac */
    4, /* sbw */
    2  /* setcurrentpoint */
  };

#endif /* CFF_CONFIG_OPTION_OLD_ENGINE */


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /**********                                                      *********/
  /**********                                                      *********/
  /**********             GENERIC CHARSTRING PARSING               *********/
  /**********                                                      *********/
  /**********                                                      *********/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_builder_init                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given glyph builder.                                 */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    builder :: A pointer to the glyph builder to initialize.           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face    :: The current face object.                                */
  /*                                                                       */
  /*    size    :: The current size object.                                */
  /*                                                                       */
  /*    glyph   :: The current glyph object.                               */
  /*                                                                       */
  /*    hinting :: Whether hinting is active.                              */
  /*                                                                       */
  static void
  cff_builder_init( CFF_Builder*   builder,
                    TT_Face        face,
                    CFF_Size       size,
                    CFF_GlyphSlot  glyph,
                    FT_Bool        hinting )
  {
    builder->path_begun  = 0;
    builder->load_points = 1;

    builder->face   = face;
    builder->glyph  = glyph;
    builder->memory = face->root.memory;

    if ( glyph )
    {
      FT_GlyphLoader  loader = glyph->root.internal->loader;


      builder->loader  = loader;
      builder->base    = &loader->base.outline;
      builder->current = &loader->current.outline;
      FT_GlyphLoader_Rewind( loader );

      builder->hints_globals = 0;
      builder->hints_funcs   = 0;

      if ( hinting && size )
      {
        CFF_Internal  internal = (CFF_Internal)size->root.internal;


        builder->hints_globals = (void *)internal->topfont;
        builder->hints_funcs   = glyph->root.internal->glyph_hints;
      }
    }

    builder->pos_x = 0;
    builder->pos_y = 0;

    builder->left_bearing.x = 0;
    builder->left_bearing.y = 0;
    builder->advance.x      = 0;
    builder->advance.y      = 0;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_builder_done                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalizes a given glyph builder.  Its contents can still be used   */
  /*    after the call, but the function saves important information       */
  /*    within the corresponding glyph slot.                               */
  /*                                                                       */
  /* <Input>                                                               */
  /*    builder :: A pointer to the glyph builder to finalize.             */
  /*                                                                       */
  static void
  cff_builder_done( CFF_Builder*  builder )
  {
    CFF_GlyphSlot  glyph = builder->glyph;


    if ( glyph )
      glyph->root.outline = *builder->base;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_compute_bias                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the bias value in dependence of the number of glyph       */
  /*    subroutines.                                                       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    in_charstring_type :: The `CharstringType' value of the top DICT   */
  /*                          dictionary.                                  */
  /*                                                                       */
  /*    num_subrs          :: The number of glyph subroutines.             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The bias value.                                                    */
  static FT_Int
  cff_compute_bias( FT_Int   in_charstring_type,
                    FT_UInt  num_subrs )
  {
    FT_Int  result;


    if ( in_charstring_type == 1 )
      result = 0;
    else if ( num_subrs < 1240 )
      result = 107;
    else if ( num_subrs < 33900U )
      result = 1131;
    else
      result = 32768U;

    return result;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_decoder_init                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given glyph decoder.                                 */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    decoder :: A pointer to the glyph builder to initialize.           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face      :: The current face object.                              */
  /*                                                                       */
  /*    size      :: The current size object.                              */
  /*                                                                       */
  /*    slot      :: The current glyph object.                             */
  /*                                                                       */
  /*    hinting   :: Whether hinting is active.                            */
  /*                                                                       */
  /*    hint_mode :: The hinting mode.                                     */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  cff_decoder_init( CFF_Decoder*    decoder,
                    TT_Face         face,
                    CFF_Size        size,
                    CFF_GlyphSlot   slot,
                    FT_Bool         hinting,
                    FT_Render_Mode  hint_mode )
  {
    CFF_Font  cff = (CFF_Font)face->extra.data;


    /* clear everything */
    FT_MEM_ZERO( decoder, sizeof ( *decoder ) );

    /* initialize builder */
    cff_builder_init( &decoder->builder, face, size, slot, hinting );

    /* initialize Type2 decoder */
    decoder->cff          = cff;
    decoder->num_globals  = cff->global_subrs_index.count;
    decoder->globals      = cff->global_subrs;
    decoder->globals_bias = cff_compute_bias(
                              cff->top_font.font_dict.charstring_type,
                              decoder->num_globals );

    decoder->hint_mode    = hint_mode;
  }


  /* this function is used to select the subfont */
  /* and the locals subrs array                  */
  FT_LOCAL_DEF( FT_Error )
  cff_decoder_prepare( CFF_Decoder*  decoder,
                       CFF_Size      size,
                       FT_UInt       glyph_index )
  {
    CFF_Builder  *builder = &decoder->builder;
    CFF_Font      cff     = (CFF_Font)builder->face->extra.data;
    CFF_SubFont   sub     = &cff->top_font;
    FT_Error      error   = FT_Err_Ok;


    /* manage CID fonts */
    if ( cff->num_subfonts )
    {
      FT_Byte  fd_index = cff_fd_select_get( &cff->fd_select, glyph_index );


      if ( fd_index >= cff->num_subfonts )
      {
        FT_TRACE4(( "cff_decoder_prepare: invalid CID subfont index\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      FT_TRACE3(( "  in subfont %d:\n", fd_index ));

      sub = cff->subfonts[fd_index];

      if ( builder->hints_funcs && size )
      {
        CFF_Internal  internal = (CFF_Internal)size->root.internal;


        /* for CFFs without subfonts, this value has already been set */
        builder->hints_globals = (void *)internal->subfonts[fd_index];
      }
    }

    decoder->num_locals    = sub->local_subrs_index.count;
    decoder->locals        = sub->local_subrs;
    decoder->locals_bias   = cff_compute_bias(
                               decoder->cff->top_font.font_dict.charstring_type,
                               decoder->num_locals );

    decoder->glyph_width   = sub->private_dict.default_width;
    decoder->nominal_width = sub->private_dict.nominal_width;

    decoder->current_subfont = sub;     /* for Adobe's CFF handler */

  Exit:
    return error;
  }


  /* check that there is enough space for `count' more points */
  FT_LOCAL_DEF( FT_Error )
  cff_check_points( CFF_Builder*  builder,
                    FT_Int        count )
  {
    return FT_GLYPHLOADER_CHECK_POINTS( builder->loader, count, 0 );
  }


  /* add a new point, do not check space */
  FT_LOCAL_DEF( void )
  cff_builder_add_point( CFF_Builder*  builder,
                         FT_Pos        x,
                         FT_Pos        y,
                         FT_Byte       flag )
  {
    FT_Outline*  outline = builder->current;


    if ( builder->load_points )
    {
      FT_Vector*  point   = outline->points + outline->n_points;
      FT_Byte*    control = (FT_Byte*)outline->tags + outline->n_points;

#ifdef CFF_CONFIG_OPTION_OLD_ENGINE
      CFF_Driver  driver  = (CFF_Driver)FT_FACE_DRIVER( builder->face );


      if ( driver->hinting_engine == FT_CFF_HINTING_FREETYPE )
      {
        point->x = x >> 16;
        point->y = y >> 16;
      }
      else
#endif
      {
        /* cf2_decoder_parse_charstrings uses 16.16 coordinates */
        point->x = x >> 10;
        point->y = y >> 10;
      }
      *control = (FT_Byte)( flag ? FT_CURVE_TAG_ON : FT_CURVE_TAG_CUBIC );
    }

    outline->n_points++;
  }


  /* check space for a new on-curve point, then add it */
  FT_LOCAL_DEF( FT_Error )
  cff_builder_add_point1( CFF_Builder*  builder,
                          FT_Pos        x,
                          FT_Pos        y )
  {
    FT_Error  error;


    error = cff_check_points( builder, 1 );
    if ( !error )
      cff_builder_add_point( builder, x, y, 1 );

    return error;
  }


  /* check space for a new contour, then add it */
  static FT_Error
  cff_builder_add_contour( CFF_Builder*  builder )
  {
    FT_Outline*  outline = builder->current;
    FT_Error     error;


    if ( !builder->load_points )
    {
      outline->n_contours++;
      return FT_Err_Ok;
    }

    error = FT_GLYPHLOADER_CHECK_POINTS( builder->loader, 0, 1 );
    if ( !error )
    {
      if ( outline->n_contours > 0 )
        outline->contours[outline->n_contours - 1] =
          (short)( outline->n_points - 1 );

      outline->n_contours++;
    }

    return error;
  }


  /* if a path was begun, add its first on-curve point */
  FT_LOCAL_DEF( FT_Error )
  cff_builder_start_point( CFF_Builder*  builder,
                           FT_Pos        x,
                           FT_Pos        y )
  {
    FT_Error  error = FT_Err_Ok;


    /* test whether we are building a new contour */
    if ( !builder->path_begun )
    {
      builder->path_begun = 1;
      error = cff_builder_add_contour( builder );
      if ( !error )
        error = cff_builder_add_point1( builder, x, y );
    }

    return error;
  }


  /* close the current contour */
  FT_LOCAL_DEF( void )
  cff_builder_close_contour( CFF_Builder*  builder )
  {
    FT_Outline*  outline = builder->current;
    FT_Int       first;


    if ( !outline )
      return;

    first = outline->n_contours <= 1
            ? 0 : outline->contours[outline->n_contours - 2] + 1;

    /* We must not include the last point in the path if it */
    /* is located on the first point.                       */
    if ( outline->n_points > 1 )
    {
      FT_Vector*  p1      = outline->points + first;
      FT_Vector*  p2      = outline->points + outline->n_points - 1;
      FT_Byte*    control = (FT_Byte*)outline->tags + outline->n_points - 1;


      /* `delete' last point only if it coincides with the first    */
      /* point and if it is not a control point (which can happen). */
      if ( p1->x == p2->x && p1->y == p2->y )
        if ( *control == FT_CURVE_TAG_ON )
          outline->n_points--;
    }

    if ( outline->n_contours > 0 )
    {
      /* Don't add contours only consisting of one point, i.e., */
      /* check whether begin point and last point are the same. */
      if ( first == outline->n_points - 1 )
      {
        outline->n_contours--;
        outline->n_points--;
      }
      else
        outline->contours[outline->n_contours - 1] =
          (short)( outline->n_points - 1 );
    }
  }


  FT_LOCAL_DEF( FT_Int )
  cff_lookup_glyph_by_stdcharcode( CFF_Font  cff,
                                   FT_Int    charcode )
  {
    FT_UInt    n;
    FT_UShort  glyph_sid;


    /* CID-keyed fonts don't have glyph names */
    if ( !cff->charset.sids )
      return -1;

    /* check range of standard char code */
    if ( charcode < 0 || charcode > 255 )
      return -1;

    /* Get code to SID mapping from `cff_standard_encoding'. */
    glyph_sid = cff_get_standard_encoding( (FT_UInt)charcode );

    for ( n = 0; n < cff->num_glyphs; n++ )
    {
      if ( cff->charset.sids[n] == glyph_sid )
        return n;
    }

    return -1;
  }


  FT_LOCAL_DEF( FT_Error )
  cff_get_glyph_data( TT_Face    face,
                      FT_UInt    glyph_index,
                      FT_Byte**  pointer,
                      FT_ULong*  length )
  {
#ifdef FT_CONFIG_OPTION_INCREMENTAL
    /* For incremental fonts get the character data using the */
    /* callback function.                                     */
    if ( face->root.internal->incremental_interface )
    {
      FT_Data   data;
      FT_Error  error =
                  face->root.internal->incremental_interface->funcs->get_glyph_data(
                    face->root.internal->incremental_interface->object,
                    glyph_index, &data );


      *pointer = (FT_Byte*)data.pointer;
      *length = data.length;

      return error;
    }
    else
#endif /* FT_CONFIG_OPTION_INCREMENTAL */

    {
      CFF_Font  cff  = (CFF_Font)(face->extra.data);


      return cff_index_access_element( &cff->charstrings_index, glyph_index,
                                       pointer, length );
    }
  }


  FT_LOCAL_DEF( void )
  cff_free_glyph_data( TT_Face    face,
                       FT_Byte**  pointer,
                       FT_ULong   length )
  {
#ifndef FT_CONFIG_OPTION_INCREMENTAL
    FT_UNUSED( length );
#endif

#ifdef FT_CONFIG_OPTION_INCREMENTAL
    /* For incremental fonts get the character data using the */
    /* callback function.                                     */
    if ( face->root.internal->incremental_interface )
    {
      FT_Data data;


      data.pointer = *pointer;
      data.length  = length;

      face->root.internal->incremental_interface->funcs->free_glyph_data(
        face->root.internal->incremental_interface->object, &data );
    }
    else
#endif /* FT_CONFIG_OPTION_INCREMENTAL */

    {
      CFF_Font  cff = (CFF_Font)(face->extra.data);


      cff_index_forget_element( &cff->charstrings_index, pointer );
    }
  }


#ifdef CFF_CONFIG_OPTION_OLD_ENGINE

  static FT_Error
  cff_operator_seac( CFF_Decoder*  decoder,
                     FT_Pos        asb,
                     FT_Pos        adx,
                     FT_Pos        ady,
                     FT_Int        bchar,
                     FT_Int        achar )
  {
    FT_Error      error;
    CFF_Builder*  builder = &decoder->builder;
    FT_Int        bchar_index, achar_index;
    TT_Face       face = decoder->builder.face;
    FT_Vector     left_bearing, advance;
    FT_Byte*      charstring;
    FT_ULong      charstring_len;
    FT_Pos        glyph_width;


    if ( decoder->seac )
    {
      FT_ERROR(( "cff_operator_seac: invalid nested seac\n" ));
      return FT_THROW( Syntax_Error );
    }

    adx += decoder->builder.left_bearing.x;
    ady += decoder->builder.left_bearing.y;

#ifdef FT_CONFIG_OPTION_INCREMENTAL
    /* Incremental fonts don't necessarily have valid charsets.        */
    /* They use the character code, not the glyph index, in this case. */
    if ( face->root.internal->incremental_interface )
    {
      bchar_index = bchar;
      achar_index = achar;
    }
    else
#endif /* FT_CONFIG_OPTION_INCREMENTAL */
    {
      CFF_Font cff = (CFF_Font)(face->extra.data);


      bchar_index = cff_lookup_glyph_by_stdcharcode( cff, bchar );
      achar_index = cff_lookup_glyph_by_stdcharcode( cff, achar );
    }

    if ( bchar_index < 0 || achar_index < 0 )
    {
      FT_ERROR(( "cff_operator_seac:"
                 " invalid seac character code arguments\n" ));
      return FT_THROW( Syntax_Error );
    }

    /* If we are trying to load a composite glyph, do not load the */
    /* accent character and return the array of subglyphs.         */
    if ( builder->no_recurse )
    {
      FT_GlyphSlot    glyph  = (FT_GlyphSlot)builder->glyph;
      FT_GlyphLoader  loader = glyph->internal->loader;
      FT_SubGlyph     subg;


      /* reallocate subglyph array if necessary */
      error = FT_GlyphLoader_CheckSubGlyphs( loader, 2 );
      if ( error )
        goto Exit;

      subg = loader->current.subglyphs;

      /* subglyph 0 = base character */
      subg->index = bchar_index;
      subg->flags = FT_SUBGLYPH_FLAG_ARGS_ARE_XY_VALUES |
                    FT_SUBGLYPH_FLAG_USE_MY_METRICS;
      subg->arg1  = 0;
      subg->arg2  = 0;
      subg++;

      /* subglyph 1 = accent character */
      subg->index = achar_index;
      subg->flags = FT_SUBGLYPH_FLAG_ARGS_ARE_XY_VALUES;
      subg->arg1  = (FT_Int)( adx >> 16 );
      subg->arg2  = (FT_Int)( ady >> 16 );

      /* set up remaining glyph fields */
      glyph->num_subglyphs = 2;
      glyph->subglyphs     = loader->base.subglyphs;
      glyph->format        = FT_GLYPH_FORMAT_COMPOSITE;

      loader->current.num_subglyphs = 2;
    }

    FT_GlyphLoader_Prepare( builder->loader );

    /* First load `bchar' in builder */
    error = cff_get_glyph_data( face, bchar_index,
                                &charstring, &charstring_len );
    if ( !error )
    {
      /* the seac operator must not be nested */
      decoder->seac = TRUE;
      error = cff_decoder_parse_charstrings( decoder, charstring,
                                             charstring_len );
      decoder->seac = FALSE;

      cff_free_glyph_data( face, &charstring, charstring_len );

      if ( error )
        goto Exit;
    }

    /* Save the left bearing, advance and glyph width of the base */
    /* character as they will be erased by the next load.         */

    left_bearing = builder->left_bearing;
    advance      = builder->advance;
    glyph_width  = decoder->glyph_width;

    builder->left_bearing.x = 0;
    builder->left_bearing.y = 0;

    builder->pos_x = adx - asb;
    builder->pos_y = ady;

    /* Now load `achar' on top of the base outline. */
    error = cff_get_glyph_data( face, achar_index,
                                &charstring, &charstring_len );
    if ( !error )
    {
      /* the seac operator must not be nested */
      decoder->seac = TRUE;
      error = cff_decoder_parse_charstrings( decoder, charstring,
                                             charstring_len );
      decoder->seac = FALSE;

      cff_free_glyph_data( face, &charstring, charstring_len );

      if ( error )
        goto Exit;
    }

    /* Restore the left side bearing, advance and glyph width */
    /* of the base character.                                 */
    builder->left_bearing = left_bearing;
    builder->advance      = advance;
    decoder->glyph_width  = glyph_width;

    builder->pos_x = 0;
    builder->pos_y = 0;

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    cff_decoder_parse_charstrings                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Parses a given Type 2 charstrings program.                         */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    decoder         :: The current Type 1 decoder.                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    charstring_base :: The base of the charstring stream.              */
  /*                                                                       */
  /*    charstring_len  :: The length in bytes of the charstring stream.   */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  cff_decoder_parse_charstrings( CFF_Decoder*  decoder,
                                 FT_Byte*      charstring_base,
                                 FT_ULong      charstring_len )
  {
    FT_Error           error;
    CFF_Decoder_Zone*  zone;
    FT_Byte*           ip;
    FT_Byte*           limit;
    CFF_Builder*       builder = &decoder->builder;
    FT_Pos             x, y;
    FT_Fixed           seed;
    FT_Fixed*          stack;
    FT_Int             charstring_type =
                         decoder->cff->top_font.font_dict.charstring_type;

    T2_Hints_Funcs     hinter;


    /* set default width */
    decoder->num_hints  = 0;
    decoder->read_width = 1;

    /* compute random seed from stack address of parameter */
    seed = (FT_Fixed)( ( (FT_PtrDist)(char*)&seed              ^
                         (FT_PtrDist)(char*)&decoder           ^
                         (FT_PtrDist)(char*)&charstring_base ) &
                         FT_ULONG_MAX ) ;
    seed = ( seed ^ ( seed >> 10 ) ^ ( seed >> 20 ) ) & 0xFFFFL;
    if ( seed == 0 )
      seed = 0x7384;

    /* initialize the decoder */
    decoder->top  = decoder->stack;
    decoder->zone = decoder->zones;
    zone          = decoder->zones;
    stack         = decoder->top;

    hinter = (T2_Hints_Funcs)builder->hints_funcs;

    builder->path_begun = 0;

    zone->base           = charstring_base;
    limit = zone->limit  = charstring_base + charstring_len;
    ip    = zone->cursor = zone->base;

    error = FT_Err_Ok;

    x = builder->pos_x;
    y = builder->pos_y;

    /* begin hints recording session, if any */
    if ( hinter )
      hinter->open( hinter->hints );

    /* now execute loop */
    while ( ip < limit )
    {
      CFF_Operator  op;
      FT_Byte       v;


      /********************************************************************/
      /*                                                                  */
      /* Decode operator or operand                                       */
      /*                                                                  */
      v = *ip++;
      if ( v >= 32 || v == 28 )
      {
        FT_Int    shift = 16;
        FT_Int32  val;


        /* this is an operand, push it on the stack */

        /* if we use shifts, all computations are done with unsigned */
        /* values; the conversion to a signed value is the last step */
        if ( v == 28 )
        {
          if ( ip + 1 >= limit )
            goto Syntax_Error;
          val = (FT_Short)( ( (FT_UShort)ip[0] << 8 ) | ip[1] );
          ip += 2;
        }
        else if ( v < 247 )
          val = (FT_Int32)v - 139;
        else if ( v < 251 )
        {
          if ( ip >= limit )
            goto Syntax_Error;
          val = ( (FT_Int32)v - 247 ) * 256 + *ip++ + 108;
        }
        else if ( v < 255 )
        {
          if ( ip >= limit )
            goto Syntax_Error;
          val = -( (FT_Int32)v - 251 ) * 256 - *ip++ - 108;
        }
        else
        {
          if ( ip + 3 >= limit )
            goto Syntax_Error;
          val = (FT_Int32)( ( (FT_UInt32)ip[0] << 24 ) |
                            ( (FT_UInt32)ip[1] << 16 ) |
                            ( (FT_UInt32)ip[2] <<  8 ) |
                              (FT_UInt32)ip[3]         );
          ip    += 4;
          if ( charstring_type == 2 )
            shift = 0;
        }
        if ( decoder->top - stack >= CFF_MAX_OPERANDS )
          goto Stack_Overflow;

        val             = (FT_Int32)( (FT_UInt32)val << shift );
        *decoder->top++ = val;

#ifdef FT_DEBUG_LEVEL_TRACE
        if ( !( val & 0xFFFFL ) )
          FT_TRACE4(( " %hd", (FT_Short)( (FT_UInt32)val >> 16 ) ));
        else
          FT_TRACE4(( " %.2f", val / 65536.0 ));
#endif

      }
      else
      {
        /* The specification says that normally arguments are to be taken */
        /* from the bottom of the stack.  However, this seems not to be   */
        /* correct, at least for Acroread 7.0.8 on GNU/Linux: It pops the */
        /* arguments similar to a PS interpreter.                         */

        FT_Fixed*  args     = decoder->top;
        FT_Int     num_args = (FT_Int)( args - decoder->stack );
        FT_Int     req_args;


        /* find operator */
        op = cff_op_unknown;

        switch ( v )
        {
        case 1:
          op = cff_op_hstem;
          break;
        case 3:
          op = cff_op_vstem;
          break;
        case 4:
          op = cff_op_vmoveto;
          break;
        case 5:
          op = cff_op_rlineto;
          break;
        case 6:
          op = cff_op_hlineto;
          break;
        case 7:
          op = cff_op_vlineto;
          break;
        case 8:
          op = cff_op_rrcurveto;
          break;
        case 9:
          op = cff_op_closepath;
          break;
        case 10:
          op = cff_op_callsubr;
          break;
        case 11:
          op = cff_op_return;
          break;
        case 12:
          {
            if ( ip >= limit )
              goto Syntax_Error;
            v = *ip++;

            switch ( v )
            {
            case 0:
              op = cff_op_dotsection;
              break;
            case 1: /* this is actually the Type1 vstem3 operator */
              op = cff_op_vstem;
              break;
            case 2: /* this is actually the Type1 hstem3 operator */
              op = cff_op_hstem;
              break;
            case 3:
              op = cff_op_and;
              break;
            case 4:
              op = cff_op_or;
              break;
            case 5:
              op = cff_op_not;
              break;
            case 6:
              op = cff_op_seac;
              break;
            case 7:
              op = cff_op_sbw;
              break;
            case 8:
              op = cff_op_store;
              break;
            case 9:
              op = cff_op_abs;
              break;
            case 10:
              op = cff_op_add;
              break;
            case 11:
              op = cff_op_sub;
              break;
            case 12:
              op = cff_op_div;
              break;
            case 13:
              op = cff_op_load;
              break;
            case 14:
              op = cff_op_neg;
              break;
            case 15:
              op = cff_op_eq;
              break;
            case 16:
              op = cff_op_callothersubr;
              break;
            case 17:
              op = cff_op_pop;
              break;
            case 18:
              op = cff_op_drop;
              break;
            case 20:
              op = cff_op_put;
              break;
            case 21:
              op = cff_op_get;
              break;
            case 22:
              op = cff_op_ifelse;
              break;
            case 23:
              op = cff_op_random;
              break;
            case 24:
              op = cff_op_mul;
              break;
            case 26:
              op = cff_op_sqrt;
              break;
            case 27:
              op = cff_op_dup;
              break;
            case 28:
              op = cff_op_exch;
              break;
            case 29:
              op = cff_op_index;
              break;
            case 30:
              op = cff_op_roll;
              break;
            case 33:
              op = cff_op_setcurrentpoint;
              break;
            case 34:
              op = cff_op_hflex;
              break;
            case 35:
              op = cff_op_flex;
              break;
            case 36:
              op = cff_op_hflex1;
              break;
            case 37:
              op = cff_op_flex1;
              break;
            default:
              FT_TRACE4(( " unknown op (12, %d)\n", v ));
              break;
            }
          }
          break;
        case 13:
          op = cff_op_hsbw;
          break;
        case 14:
          op = cff_op_endchar;
          break;
        case 16:
          op = cff_op_blend;
          break;
        case 18:
          op = cff_op_hstemhm;
          break;
        case 19:
          op = cff_op_hintmask;
          break;
        case 20:
          op = cff_op_cntrmask;
          break;
        case 21:
          op = cff_op_rmoveto;
          break;
        case 22:
          op = cff_op_hmoveto;
          break;
        case 23:
          op = cff_op_vstemhm;
          break;
        case 24:
          op = cff_op_rcurveline;
          break;
        case 25:
          op = cff_op_rlinecurve;
          break;
        case 26:
          op = cff_op_vvcurveto;
          break;
        case 27:
          op = cff_op_hhcurveto;
          break;
        case 29:
          op = cff_op_callgsubr;
          break;
        case 30:
          op = cff_op_vhcurveto;
          break;
        case 31:
          op = cff_op_hvcurveto;
          break;
        default:
          FT_TRACE4(( " unknown op (%d)\n", v ));
          break;
        }

        if ( op == cff_op_unknown )
          continue;

        /* check arguments */
        req_args = cff_argument_counts[op];
        if ( req_args & CFF_COUNT_CHECK_WIDTH )
        {
          if ( num_args > 0 && decoder->read_width )
          {
            /* If `nominal_width' is non-zero, the number is really a      */
            /* difference against `nominal_width'.  Else, the number here  */
            /* is truly a width, not a difference against `nominal_width'. */
            /* If the font does not set `nominal_width', then              */
            /* `nominal_width' defaults to zero, and so we can set         */
            /* `glyph_width' to `nominal_width' plus number on the stack   */
            /* -- for either case.                                         */

            FT_Int  set_width_ok;


            switch ( op )
            {
            case cff_op_hmoveto:
            case cff_op_vmoveto:
              set_width_ok = num_args & 2;
              break;

            case cff_op_hstem:
            case cff_op_vstem:
            case cff_op_hstemhm:
            case cff_op_vstemhm:
            case cff_op_rmoveto:
            case cff_op_hintmask:
            case cff_op_cntrmask:
              set_width_ok = num_args & 1;
              break;

            case cff_op_endchar:
              /* If there is a width specified for endchar, we either have */
              /* 1 argument or 5 arguments.  We like to argue.             */
              set_width_ok = ( num_args == 5 ) || ( num_args == 1 );
              break;

            default:
              set_width_ok = 0;
              break;
            }

            if ( set_width_ok )
            {
              decoder->glyph_width = decoder->nominal_width +
                                       ( stack[0] >> 16 );

              if ( decoder->width_only )
              {
                /* we only want the advance width; stop here */
                break;
              }

              /* Consumed an argument. */
              num_args--;
            }
          }

          decoder->read_width = 0;
          req_args            = 0;
        }

        req_args &= 0x000F;
        if ( num_args < req_args )
          goto Stack_Underflow;
        args     -= req_args;
        num_args -= req_args;

        /* At this point, `args' points to the first argument of the  */
        /* operand in case `req_args' isn't zero.  Otherwise, we have */
        /* to adjust `args' manually.                                 */

        /* Note that we only pop arguments from the stack which we    */
        /* really need and can digest so that we can continue in case */
        /* of superfluous stack elements.                             */

        switch ( op )
        {
        case cff_op_hstem:
        case cff_op_vstem:
        case cff_op_hstemhm:
        case cff_op_vstemhm:
          /* the number of arguments is always even here */
          FT_TRACE4((
              op == cff_op_hstem   ? " hstem\n"   :
            ( op == cff_op_vstem   ? " vstem\n"   :
            ( op == cff_op_hstemhm ? " hstemhm\n" : " vstemhm\n" ) ) ));

          if ( hinter )
            hinter->stems( hinter->hints,
                           ( op == cff_op_hstem || op == cff_op_hstemhm ),
                           num_args / 2,
                           args - ( num_args & ~1 ) );

          decoder->num_hints += num_args / 2;
          args = stack;
          break;

        case cff_op_hintmask:
        case cff_op_cntrmask:
          FT_TRACE4(( op == cff_op_hintmask ? " hintmask" : " cntrmask" ));

          /* implement vstem when needed --                        */
          /* the specification doesn't say it, but this also works */
          /* with the 'cntrmask' operator                          */
          /*                                                       */
          if ( num_args > 0 )
          {
            if ( hinter )
              hinter->stems( hinter->hints,
                             0,
                             num_args / 2,
                             args - ( num_args & ~1 ) );

            decoder->num_hints += num_args / 2;
          }

          /* In a valid charstring there must be at least one byte */
          /* after `hintmask' or `cntrmask' (e.g., for a `return'  */
          /* instruction).  Additionally, there must be space for  */
          /* `num_hints' bits.                                     */

          if ( ( ip + ( ( decoder->num_hints + 7 ) >> 3 ) ) >= limit )
            goto Syntax_Error;

          if ( hinter )
          {
            if ( op == cff_op_hintmask )
              hinter->hintmask( hinter->hints,
                                builder->current->n_points,
                                decoder->num_hints,
                                ip );
            else
              hinter->counter( hinter->hints,
                               decoder->num_hints,
                               ip );
          }

#ifdef FT_DEBUG_LEVEL_TRACE
          {
            FT_UInt maskbyte;


            FT_TRACE4(( " (maskbytes:" ));

            for ( maskbyte = 0;
                  maskbyte < (FT_UInt)( ( decoder->num_hints + 7 ) >> 3 );
                  maskbyte++, ip++ )
              FT_TRACE4(( " 0x%02X", *ip ));

            FT_TRACE4(( ")\n" ));
          }
#else
          ip += ( decoder->num_hints + 7 ) >> 3;
#endif
          args = stack;
          break;

        case cff_op_rmoveto:
          FT_TRACE4(( " rmoveto\n" ));

          cff_builder_close_contour( builder );
          builder->path_begun = 0;
          x   += args[-2];
          y   += args[-1];
          args = stack;
          break;

        case cff_op_vmoveto:
          FT_TRACE4(( " vmoveto\n" ));

          cff_builder_close_contour( builder );
          builder->path_begun = 0;
          y   += args[-1];
          args = stack;
          break;

        case cff_op_hmoveto:
          FT_TRACE4(( " hmoveto\n" ));

          cff_builder_close_contour( builder );
          builder->path_begun = 0;
          x   += args[-1];
          args = stack;
          break;

        case cff_op_rlineto:
          FT_TRACE4(( " rlineto\n" ));

          if ( cff_builder_start_point( builder, x, y )  ||
               cff_check_points( builder, num_args / 2 ) )
            goto Fail;

          if ( num_args < 2 )
            goto Stack_Underflow;

          args -= num_args & ~1;
          while ( args < decoder->top )
          {
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 1 );
            args += 2;
          }
          args = stack;
          break;

        case cff_op_hlineto:
        case cff_op_vlineto:
          {
            FT_Int  phase = ( op == cff_op_hlineto );


            FT_TRACE4(( op == cff_op_hlineto ? " hlineto\n"
                                             : " vlineto\n" ));

            if ( num_args < 0 )
              goto Stack_Underflow;

            /* there exist subsetted fonts (found in PDFs) */
            /* which call `hlineto' without arguments      */
            if ( num_args == 0 )
              break;

            if ( cff_builder_start_point( builder, x, y ) ||
                 cff_check_points( builder, num_args )    )
              goto Fail;

            args = stack;
            while ( args < decoder->top )
            {
              if ( phase )
                x += args[0];
              else
                y += args[0];

              if ( cff_builder_add_point1( builder, x, y ) )
                goto Fail;

              args++;
              phase ^= 1;
            }
            args = stack;
          }
          break;

        case cff_op_rrcurveto:
          {
            FT_Int  nargs;


            FT_TRACE4(( " rrcurveto\n" ));

            if ( num_args < 6 )
              goto Stack_Underflow;

            nargs = num_args - num_args % 6;

            if ( cff_builder_start_point( builder, x, y ) ||
                 cff_check_points( builder, nargs / 2 )   )
              goto Fail;

            args -= nargs;
            while ( args < decoder->top )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[2];
              y += args[3];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[4];
              y += args[5];
              cff_builder_add_point( builder, x, y, 1 );
              args += 6;
            }
            args = stack;
          }
          break;

        case cff_op_vvcurveto:
          {
            FT_Int  nargs;


            FT_TRACE4(( " vvcurveto\n" ));

            if ( num_args < 4 )
              goto Stack_Underflow;

            /* if num_args isn't of the form 4n or 4n+1, */
            /* we enforce it by clearing the second bit  */

            nargs = num_args & ~2;

            if ( cff_builder_start_point( builder, x, y ) )
              goto Fail;

            args -= nargs;

            if ( nargs & 1 )
            {
              x += args[0];
              args++;
              nargs--;
            }

            if ( cff_check_points( builder, 3 * ( nargs / 4 ) ) )
              goto Fail;

            while ( args < decoder->top )
            {
              y += args[0];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[1];
              y += args[2];
              cff_builder_add_point( builder, x, y, 0 );
              y += args[3];
              cff_builder_add_point( builder, x, y, 1 );
              args += 4;
            }
            args = stack;
          }
          break;

        case cff_op_hhcurveto:
          {
            FT_Int  nargs;


            FT_TRACE4(( " hhcurveto\n" ));

            if ( num_args < 4 )
              goto Stack_Underflow;

            /* if num_args isn't of the form 4n or 4n+1, */
            /* we enforce it by clearing the second bit  */

            nargs = num_args & ~2;

            if ( cff_builder_start_point( builder, x, y ) )
              goto Fail;

            args -= nargs;
            if ( nargs & 1 )
            {
              y += args[0];
              args++;
              nargs--;
            }

            if ( cff_check_points( builder, 3 * ( nargs / 4 ) ) )
              goto Fail;

            while ( args < decoder->top )
            {
              x += args[0];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[1];
              y += args[2];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[3];
              cff_builder_add_point( builder, x, y, 1 );
              args += 4;
            }
            args = stack;
          }
          break;

        case cff_op_vhcurveto:
        case cff_op_hvcurveto:
          {
            FT_Int  phase;
            FT_Int  nargs;


            FT_TRACE4(( op == cff_op_vhcurveto ? " vhcurveto\n"
                                               : " hvcurveto\n" ));

            if ( cff_builder_start_point( builder, x, y ) )
              goto Fail;

            if ( num_args < 4 )
              goto Stack_Underflow;

            /* if num_args isn't of the form 8n, 8n+1, 8n+4, or 8n+5, */
            /* we enforce it by clearing the second bit               */

            nargs = num_args & ~2;

            args -= nargs;
            if ( cff_check_points( builder, ( nargs / 4 ) * 3 ) )
              goto Stack_Underflow;

            phase = ( op == cff_op_hvcurveto );

            while ( nargs >= 4 )
            {
              nargs -= 4;
              if ( phase )
              {
                x += args[0];
                cff_builder_add_point( builder, x, y, 0 );
                x += args[1];
                y += args[2];
                cff_builder_add_point( builder, x, y, 0 );
                y += args[3];
                if ( nargs == 1 )
                  x += args[4];
                cff_builder_add_point( builder, x, y, 1 );
              }
              else
              {
                y += args[0];
                cff_builder_add_point( builder, x, y, 0 );
                x += args[1];
                y += args[2];
                cff_builder_add_point( builder, x, y, 0 );
                x += args[3];
                if ( nargs == 1 )
                  y += args[4];
                cff_builder_add_point( builder, x, y, 1 );
              }
              args  += 4;
              phase ^= 1;
            }
            args = stack;
          }
          break;

        case cff_op_rlinecurve:
          {
            FT_Int  num_lines;
            FT_Int  nargs;


            FT_TRACE4(( " rlinecurve\n" ));

            if ( num_args < 8 )
              goto Stack_Underflow;

            nargs     = num_args & ~1;
            num_lines = ( nargs - 6 ) / 2;

            if ( cff_builder_start_point( builder, x, y )   ||
                 cff_check_points( builder, num_lines + 3 ) )
              goto Fail;

            args -= nargs;

            /* first, add the line segments */
            while ( num_lines > 0 )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y, 1 );
              args += 2;
              num_lines--;
            }

            /* then the curve */
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 0 );
            x += args[2];
            y += args[3];
            cff_builder_add_point( builder, x, y, 0 );
            x += args[4];
            y += args[5];
            cff_builder_add_point( builder, x, y, 1 );
            args = stack;
          }
          break;

        case cff_op_rcurveline:
          {
            FT_Int  num_curves;
            FT_Int  nargs;


            FT_TRACE4(( " rcurveline\n" ));

            if ( num_args < 8 )
              goto Stack_Underflow;

            nargs      = num_args - 2;
            nargs      = nargs - nargs % 6 + 2;
            num_curves = ( nargs - 2 ) / 6;

            if ( cff_builder_start_point( builder, x, y )        ||
                 cff_check_points( builder, num_curves * 3 + 2 ) )
              goto Fail;

            args -= nargs;

            /* first, add the curves */
            while ( num_curves > 0 )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[2];
              y += args[3];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[4];
              y += args[5];
              cff_builder_add_point( builder, x, y, 1 );
              args += 6;
              num_curves--;
            }

            /* then the final line */
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 1 );
            args = stack;
          }
          break;

        case cff_op_hflex1:
          {
            FT_Pos start_y;


            FT_TRACE4(( " hflex1\n" ));

            /* adding five more points: 4 control points, 1 on-curve point */
            /* -- make sure we have enough space for the start point if it */
            /* needs to be added                                           */
            if ( cff_builder_start_point( builder, x, y ) ||
                 cff_check_points( builder, 6 )           )
              goto Fail;

            /* record the starting point's y position for later use */
            start_y = y;

            /* first control point */
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 0 );

            /* second control point */
            x += args[2];
            y += args[3];
            cff_builder_add_point( builder, x, y, 0 );

            /* join point; on curve, with y-value the same as the last */
            /* control point's y-value                                 */
            x += args[4];
            cff_builder_add_point( builder, x, y, 1 );

            /* third control point, with y-value the same as the join */
            /* point's y-value                                        */
            x += args[5];
            cff_builder_add_point( builder, x, y, 0 );

            /* fourth control point */
            x += args[6];
            y += args[7];
            cff_builder_add_point( builder, x, y, 0 );

            /* ending point, with y-value the same as the start   */
            x += args[8];
            y  = start_y;
            cff_builder_add_point( builder, x, y, 1 );

            args = stack;
            break;
          }

        case cff_op_hflex:
          {
            FT_Pos start_y;


            FT_TRACE4(( " hflex\n" ));

            /* adding six more points; 4 control points, 2 on-curve points */
            if ( cff_builder_start_point( builder, x, y ) ||
                 cff_check_points( builder, 6 )           )
              goto Fail;

            /* record the starting point's y-position for later use */
            start_y = y;

            /* first control point */
            x += args[0];
            cff_builder_add_point( builder, x, y, 0 );

            /* second control point */
            x += args[1];
            y += args[2];
            cff_builder_add_point( builder, x, y, 0 );

            /* join point; on curve, with y-value the same as the last */
            /* control point's y-value                                 */
            x += args[3];
            cff_builder_add_point( builder, x, y, 1 );

            /* third control point, with y-value the same as the join */
            /* point's y-value                                        */
            x += args[4];
            cff_builder_add_point( builder, x, y, 0 );

            /* fourth control point */
            x += args[5];
            y  = start_y;
            cff_builder_add_point( builder, x, y, 0 );

            /* ending point, with y-value the same as the start point's */
            /* y-value -- we don't add this point, though               */
            x += args[6];
            cff_builder_add_point( builder, x, y, 1 );

            args = stack;
            break;
          }

        case cff_op_flex1:
          {
            FT_Pos     start_x, start_y; /* record start x, y values for */
                                         /* alter use                    */
            FT_Fixed   dx = 0, dy = 0;   /* used in horizontal/vertical  */
                                         /* algorithm below              */
            FT_Int     horizontal, count;
            FT_Fixed*  temp;


            FT_TRACE4(( " flex1\n" ));

            /* adding six more points; 4 control points, 2 on-curve points */
            if ( cff_builder_start_point( builder, x, y ) ||
                 cff_check_points( builder, 6 )           )
              goto Fail;

            /* record the starting point's x, y position for later use */
            start_x = x;
            start_y = y;

            /* XXX: figure out whether this is supposed to be a horizontal */
            /*      or vertical flex; the Type 2 specification is vague... */

            temp = args;

            /* grab up to the last argument */
            for ( count = 5; count > 0; count-- )
            {
              dx += temp[0];
              dy += temp[1];
              temp += 2;
            }

            if ( dx < 0 )
              dx = -dx;
            if ( dy < 0 )
              dy = -dy;

            /* strange test, but here it is... */
            horizontal = ( dx > dy );

            for ( count = 5; count > 0; count-- )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y,
                                     (FT_Bool)( count == 3 ) );
              args += 2;
            }

            /* is last operand an x- or y-delta? */
            if ( horizontal )
            {
              x += args[0];
              y  = start_y;
            }
            else
            {
              x  = start_x;
              y += args[0];
            }

            cff_builder_add_point( builder, x, y, 1 );

            args = stack;
            break;
           }

        case cff_op_flex:
          {
            FT_UInt  count;


            FT_TRACE4(( " flex\n" ));

            if ( cff_builder_start_point( builder, x, y ) ||
                 cff_check_points( builder, 6 )           )
              goto Fail;

            for ( count = 6; count > 0; count-- )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y,
                                     (FT_Bool)( count == 4 || count == 1 ) );
              args += 2;
            }

            args = stack;
          }
          break;

        case cff_op_seac:
            FT_TRACE4(( " seac\n" ));

            error = cff_operator_seac( decoder,
                                       args[0], args[1], args[2],
                                       (FT_Int)( args[3] >> 16 ),
                                       (FT_Int)( args[4] >> 16 ) );

            /* add current outline to the glyph slot */
            FT_GlyphLoader_Add( builder->loader );

            /* return now! */
            FT_TRACE4(( "\n" ));
            return error;

        case cff_op_endchar:
          FT_TRACE4(( " endchar\n" ));

          /* We are going to emulate the seac operator. */
          if ( num_args >= 4 )
          {
            /* Save glyph width so that the subglyphs don't overwrite it. */
            FT_Pos  glyph_width = decoder->glyph_width;


            error = cff_operator_seac( decoder,
                                       0L, args[-4], args[-3],
                                       (FT_Int)( args[-2] >> 16 ),
                                       (FT_Int)( args[-1] >> 16 ) );

            decoder->glyph_width = glyph_width;
          }
          else
          {
            if ( !error )
              error = FT_Err_Ok;

            cff_builder_close_contour( builder );

            /* close hints recording session */
            if ( hinter )
            {
              if ( hinter->close( hinter->hints,
                                  builder->current->n_points ) )
                goto Syntax_Error;

              /* apply hints to the loaded glyph outline now */
              hinter->apply( hinter->hints,
                             builder->current,
                             (PSH_Globals)builder->hints_globals,
                             decoder->hint_mode );
            }

            /* add current outline to the glyph slot */
            FT_GlyphLoader_Add( builder->loader );
          }

          /* return now! */
          FT_TRACE4(( "\n" ));
          return error;

        case cff_op_abs:
          FT_TRACE4(( " abs\n" ));

          if ( args[0] < 0 )
            args[0] = -args[0];
          args++;
          break;

        case cff_op_add:
          FT_TRACE4(( " add\n" ));

          args[0] += args[1];
          args++;
          break;

        case cff_op_sub:
          FT_TRACE4(( " sub\n" ));

          args[0] -= args[1];
          args++;
          break;

        case cff_op_div:
          FT_TRACE4(( " div\n" ));

          args[0] = FT_DivFix( args[0], args[1] );
          args++;
          break;

        case cff_op_neg:
          FT_TRACE4(( " neg\n" ));

          args[0] = -args[0];
          args++;
          break;

        case cff_op_random:
          {
            FT_Fixed  Rand;


            FT_TRACE4(( " rand\n" ));

            Rand = seed;
            if ( Rand >= 0x8000L )
              Rand++;

            args[0] = Rand;
            seed    = FT_MulFix( seed, 0x10000L - seed );
            if ( seed == 0 )
              seed += 0x2873;
            args++;
          }
          break;

        case cff_op_mul:
          FT_TRACE4(( " mul\n" ));

          args[0] = FT_MulFix( args[0], args[1] );
          args++;
          break;

        case cff_op_sqrt:
          FT_TRACE4(( " sqrt\n" ));

          if ( args[0] > 0 )
          {
            FT_Int    count = 9;
            FT_Fixed  root  = args[0];
            FT_Fixed  new_root;


            for (;;)
            {
              new_root = ( root + FT_DivFix( args[0], root ) + 1 ) >> 1;
              if ( new_root == root || count <= 0 )
                break;
              root = new_root;
            }
            args[0] = new_root;
          }
          else
            args[0] = 0;
          args++;
          break;

        case cff_op_drop:
          /* nothing */
          FT_TRACE4(( " drop\n" ));

          break;

        case cff_op_exch:
          {
            FT_Fixed  tmp;


            FT_TRACE4(( " exch\n" ));

            tmp     = args[0];
            args[0] = args[1];
            args[1] = tmp;
            args   += 2;
          }
          break;

        case cff_op_index:
          {
            FT_Int  idx = (FT_Int)( args[0] >> 16 );


            FT_TRACE4(( " index\n" ));

            if ( idx < 0 )
              idx = 0;
            else if ( idx > num_args - 2 )
              idx = num_args - 2;
            args[0] = args[-( idx + 1 )];
            args++;
          }
          break;

        case cff_op_roll:
          {
            FT_Int  count = (FT_Int)( args[0] >> 16 );
            FT_Int  idx   = (FT_Int)( args[1] >> 16 );


            FT_TRACE4(( " roll\n" ));

            if ( count <= 0 )
              count = 1;

            args -= count;
            if ( args < stack )
              goto Stack_Underflow;

            if ( idx >= 0 )
            {
              while ( idx > 0 )
              {
                FT_Fixed  tmp = args[count - 1];
                FT_Int    i;


                for ( i = count - 2; i >= 0; i-- )
                  args[i + 1] = args[i];
                args[0] = tmp;
                idx--;
              }
            }
            else
            {
              while ( idx < 0 )
              {
                FT_Fixed  tmp = args[0];
                FT_Int    i;


                for ( i = 0; i < count - 1; i++ )
                  args[i] = args[i + 1];
                args[count - 1] = tmp;
                idx++;
              }
            }
            args += count;
          }
          break;

        case cff_op_dup:
          FT_TRACE4(( " dup\n" ));

          args[1] = args[0];
          args += 2;
          break;

        case cff_op_put:
          {
            FT_Fixed  val = args[0];
            FT_Int    idx = (FT_Int)( args[1] >> 16 );


            FT_TRACE4(( " put\n" ));

            if ( idx >= 0 && idx < CFF_MAX_TRANS_ELEMENTS )
              decoder->buildchar[idx] = val;
          }
          break;

        case cff_op_get:
          {
            FT_Int    idx = (FT_Int)( args[0] >> 16 );
            FT_Fixed  val = 0;


            FT_TRACE4(( " get\n" ));

            if ( idx >= 0 && idx < CFF_MAX_TRANS_ELEMENTS )
              val = decoder->buildchar[idx];

            args[0] = val;
            args++;
          }
          break;

        case cff_op_store:
          FT_TRACE4(( " store\n"));

          goto Unimplemented;

        case cff_op_load:
          FT_TRACE4(( " load\n" ));

          goto Unimplemented;

        case cff_op_dotsection:
          /* this operator is deprecated and ignored by the parser */
          FT_TRACE4(( " dotsection\n" ));
          break;

        case cff_op_closepath:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " closepath (invalid op)\n" ));

          args = stack;
          break;

        case cff_op_hsbw:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " hsbw (invalid op)\n" ));

          decoder->glyph_width = decoder->nominal_width + ( args[1] >> 16 );

          decoder->builder.left_bearing.x = args[0];
          decoder->builder.left_bearing.y = 0;

          x    = decoder->builder.pos_x + args[0];
          y    = decoder->builder.pos_y;
          args = stack;
          break;

        case cff_op_sbw:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " sbw (invalid op)\n" ));

          decoder->glyph_width = decoder->nominal_width + ( args[2] >> 16 );

          decoder->builder.left_bearing.x = args[0];
          decoder->builder.left_bearing.y = args[1];

          x    = decoder->builder.pos_x + args[0];
          y    = decoder->builder.pos_y + args[1];
          args = stack;
          break;

        case cff_op_setcurrentpoint:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " setcurrentpoint (invalid op)\n" ));

          x    = decoder->builder.pos_x + args[0];
          y    = decoder->builder.pos_y + args[1];
          args = stack;
          break;

        case cff_op_callothersubr:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " callothersubr (invalid op)\n" ));

          /* subsequent `pop' operands should add the arguments,       */
          /* this is the implementation described for `unknown' other  */
          /* subroutines in the Type1 spec.                            */
          /*                                                           */
          /* XXX Fix return arguments (see discussion below).          */
          args -= 2 + ( args[-2] >> 16 );
          if ( args < stack )
            goto Stack_Underflow;
          break;

        case cff_op_pop:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " pop (invalid op)\n" ));

          /* XXX Increasing `args' is wrong: After a certain number of */
          /* `pop's we get a stack overflow.  Reason for doing it is   */
          /* code like this (actually found in a CFF font):            */
          /*                                                           */
          /*   17 1 3 callothersubr                                    */
          /*   pop                                                     */
          /*   callsubr                                                */
          /*                                                           */
          /* Since we handle `callothersubr' as a no-op, and           */
          /* `callsubr' needs at least one argument, `pop' can't be a  */
          /* no-op too as it basically should be.                      */
          /*                                                           */
          /* The right solution would be to provide real support for   */
          /* `callothersubr' as done in `t1decode.c', however, given   */
          /* the fact that CFF fonts with `pop' are invalid, it is     */
          /* questionable whether it is worth the time.                */
          args++;
          break;

        case cff_op_and:
          {
            FT_Fixed  cond = args[0] && args[1];


            FT_TRACE4(( " and\n" ));

            args[0] = cond ? 0x10000L : 0;
            args++;
          }
          break;

        case cff_op_or:
          {
            FT_Fixed  cond = args[0] || args[1];


            FT_TRACE4(( " or\n" ));

            args[0] = cond ? 0x10000L : 0;
            args++;
          }
          break;

        case cff_op_eq:
          {
            FT_Fixed  cond = !args[0];


            FT_TRACE4(( " eq\n" ));

            args[0] = cond ? 0x10000L : 0;
            args++;
          }
          break;

        case cff_op_ifelse:
          {
            FT_Fixed  cond = ( args[2] <= args[3] );


            FT_TRACE4(( " ifelse\n" ));

            if ( !cond )
              args[0] = args[1];
            args++;
          }
          break;

        case cff_op_callsubr:
          {
            FT_UInt  idx = (FT_UInt)( ( args[0] >> 16 ) +
                                      decoder->locals_bias );


            FT_TRACE4(( " callsubr(%d)\n", idx ));

            if ( idx >= decoder->num_locals )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invalid local subr index\n" ));
              goto Syntax_Error;
            }

            if ( zone - decoder->zones >= CFF_MAX_SUBRS_CALLS )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " too many nested subrs\n" ));
              goto Syntax_Error;
            }

            zone->cursor = ip;  /* save current instruction pointer */

            zone++;
            zone->base   = decoder->locals[idx];
            zone->limit  = decoder->locals[idx + 1];
            zone->cursor = zone->base;

            if ( !zone->base || zone->limit == zone->base )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invoking empty subrs\n" ));
              goto Syntax_Error;
            }

            decoder->zone = zone;
            ip            = zone->base;
            limit         = zone->limit;
          }
          break;

        case cff_op_callgsubr:
          {
            FT_UInt  idx = (FT_UInt)( ( args[0] >> 16 ) +
                                      decoder->globals_bias );


            FT_TRACE4(( " callgsubr(%d)\n", idx ));

            if ( idx >= decoder->num_globals )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invalid global subr index\n" ));
              goto Syntax_Error;
            }

            if ( zone - decoder->zones >= CFF_MAX_SUBRS_CALLS )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " too many nested subrs\n" ));
              goto Syntax_Error;
            }

            zone->cursor = ip;  /* save current instruction pointer */

            zone++;
            zone->base   = decoder->globals[idx];
            zone->limit  = decoder->globals[idx + 1];
            zone->cursor = zone->base;

            if ( !zone->base || zone->limit == zone->base )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invoking empty subrs\n" ));
              goto Syntax_Error;
            }

            decoder->zone = zone;
            ip            = zone->base;
            limit         = zone->limit;
          }
          break;

        case cff_op_return:
          FT_TRACE4(( " return\n" ));

          if ( decoder->zone <= decoder->zones )
          {
            FT_ERROR(( "cff_decoder_parse_charstrings:"
                       " unexpected return\n" ));
            goto Syntax_Error;
          }

          decoder->zone--;
          zone  = decoder->zone;
          ip    = zone->cursor;
          limit = zone->limit;
          break;

        default:
        Unimplemented:
          FT_ERROR(( "Unimplemented opcode: %d", ip[-1] ));

          if ( ip[-1] == 12 )
            FT_ERROR(( " %d", ip[0] ));
          FT_ERROR(( "\n" ));

          return FT_THROW( Unimplemented_Feature );
        }

        decoder->top = args;

        if ( decoder->top - stack >= CFF_MAX_OPERANDS )
          goto Stack_Overflow;

      } /* general operator processing */

    } /* while ip < limit */

    FT_TRACE4(( "..end..\n\n" ));

  Fail:
    return error;

  Syntax_Error:
    FT_TRACE4(( "cff_decoder_parse_charstrings: syntax error\n" ));
    return FT_THROW( Invalid_File_Format );

  Stack_Underflow:
    FT_TRACE4(( "cff_decoder_parse_charstrings: stack underflow\n" ));
    return FT_THROW( Too_Few_Arguments );

  Stack_Overflow:
    FT_TRACE4(( "cff_decoder_parse_charstrings: stack overflow\n" ));
    return FT_THROW( Stack_Overflow );
  }

#endif /* CFF_CONFIG_OPTION_OLD_ENGINE */


  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /**********                                                      *********/
  /**********                                                      *********/
  /**********            COMPUTE THE MAXIMUM ADVANCE WIDTH         *********/
  /**********                                                      *********/
  /**********    The following code is in charge of computing      *********/
  /**********    the maximum advance width of the font.  It        *********/
  /**********    quickly processes each glyph charstring to        *********/
  /**********    extract the value from either a `sbw' or `seac'   *********/
  /**********    operator.                                         *********/
  /**********                                                      *********/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


#if 0 /* unused until we support pure CFF fonts */


  FT_LOCAL_DEF( FT_Error )
  cff_compute_max_advance( TT_Face  face,
                           FT_Int*  max_advance )
  {
    FT_Error     error = FT_Err_Ok;
    CFF_Decoder  decoder;
    FT_Int       glyph_index;
    CFF_Font     cff = (CFF_Font)face->other;


    *max_advance = 0;

    /* Initialize load decoder */
    cff_decoder_init( &decoder, face, 0, 0, 0, 0 );

    decoder.builder.metrics_only = 1;
    decoder.builder.load_points  = 0;

    /* For each glyph, parse the glyph charstring and extract */
    /* the advance width.                                     */
    for ( glyph_index = 0; glyph_index < face->root.num_glyphs;
          glyph_index++ )
    {
      FT_Byte*  charstring;
      FT_ULong  charstring_len;


      /* now get load the unscaled outline */
      error = cff_get_glyph_data( face, glyph_index,
                                  &charstring, &charstring_len );
      if ( !error )
      {
        error = cff_decoder_prepare( &decoder, size, glyph_index );
        if ( !error )
          error = cff_decoder_parse_charstrings( &decoder,
                                                 charstring,
                                                 charstring_len );

        cff_free_glyph_data( face, &charstring, &charstring_len );
      }

      /* ignore the error if one has occurred -- skip to next glyph */
      error = FT_Err_Ok;
    }

    *max_advance = decoder.builder.advance.x;

    return FT_Err_Ok;
  }


#endif /* 0 */


  FT_LOCAL_DEF( FT_Error )
  cff_slot_load( CFF_GlyphSlot  glyph,
                 CFF_Size       size,
                 FT_UInt        glyph_index,
                 FT_Int32       load_flags )
  {
    FT_Error     error;
    CFF_Decoder  decoder;
    TT_Face      face = (TT_Face)glyph->root.face;
    FT_Bool      hinting, scaled, force_scaling;
    CFF_Font     cff  = (CFF_Font)face->extra.data;

    FT_Matrix    font_matrix;
    FT_Vector    font_offset;


    force_scaling = FALSE;

    /* in a CID-keyed font, consider `glyph_index' as a CID and map */
    /* it immediately to the real glyph_index -- if it isn't a      */
    /* subsetted font, glyph_indices and CIDs are identical, though */
    if ( cff->top_font.font_dict.cid_registry != 0xFFFFU &&
         cff->charset.cids                               )
    {
      /* don't handle CID 0 (.notdef) which is directly mapped to GID 0 */
      if ( glyph_index != 0 )
      {
        glyph_index = cff_charset_cid_to_gindex( &cff->charset,
                                                 glyph_index );
        if ( glyph_index == 0 )
          return FT_THROW( Invalid_Argument );
      }
    }
    else if ( glyph_index >= cff->num_glyphs )
      return FT_THROW( Invalid_Argument );

    if ( load_flags & FT_LOAD_NO_RECURSE )
      load_flags |= FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING;

    glyph->x_scale = 0x10000L;
    glyph->y_scale = 0x10000L;
    if ( size )
    {
      glyph->x_scale = size->root.metrics.x_scale;
      glyph->y_scale = size->root.metrics.y_scale;
    }

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

    /* try to load embedded bitmap if any              */
    /*                                                 */
    /* XXX: The convention should be emphasized in     */
    /*      the documents because it can be confusing. */
    if ( size )
    {
      CFF_Face      cff_face = (CFF_Face)size->root.face;
      SFNT_Service  sfnt     = (SFNT_Service)cff_face->sfnt;
      FT_Stream     stream   = cff_face->root.stream;


      if ( size->strike_index != 0xFFFFFFFFUL      &&
           sfnt->load_eblc                         &&
           ( load_flags & FT_LOAD_NO_BITMAP ) == 0 )
      {
        TT_SBit_MetricsRec  metrics;


        error = sfnt->load_sbit_image( face,
                                       size->strike_index,
                                       glyph_index,
                                       (FT_Int)load_flags,
                                       stream,
                                       &glyph->root.bitmap,
                                       &metrics );

        if ( !error )
        {
          FT_Bool    has_vertical_info;
          FT_UShort  advance;
          FT_Short   dummy;


          glyph->root.outline.n_points   = 0;
          glyph->root.outline.n_contours = 0;

          glyph->root.metrics.width  = (FT_Pos)metrics.width  << 6;
          glyph->root.metrics.height = (FT_Pos)metrics.height << 6;

          glyph->root.metrics.horiBearingX = (FT_Pos)metrics.horiBearingX << 6;
          glyph->root.metrics.horiBearingY = (FT_Pos)metrics.horiBearingY << 6;
          glyph->root.metrics.horiAdvance  = (FT_Pos)metrics.horiAdvance  << 6;

          glyph->root.metrics.vertBearingX = (FT_Pos)metrics.vertBearingX << 6;
          glyph->root.metrics.vertBearingY = (FT_Pos)metrics.vertBearingY << 6;
          glyph->root.metrics.vertAdvance  = (FT_Pos)metrics.vertAdvance  << 6;

          glyph->root.format = FT_GLYPH_FORMAT_BITMAP;

          if ( load_flags & FT_LOAD_VERTICAL_LAYOUT )
          {
            glyph->root.bitmap_left = metrics.vertBearingX;
            glyph->root.bitmap_top  = metrics.vertBearingY;
          }
          else
          {
            glyph->root.bitmap_left = metrics.horiBearingX;
            glyph->root.bitmap_top  = metrics.horiBearingY;
          }

          /* compute linear advance widths */

          ( (SFNT_Service)face->sfnt )->get_metrics( face, 0,
                                                     glyph_index,
                                                     &dummy,
                                                     &advance );
          glyph->root.linearHoriAdvance = advance;

          has_vertical_info = FT_BOOL(
                                face->vertical_info                   &&
                                face->vertical.number_Of_VMetrics > 0 );

          /* get the vertical metrics from the vtmx table if we have one */
          if ( has_vertical_info )
          {
            ( (SFNT_Service)face->sfnt )->get_metrics( face, 1,
                                                       glyph_index,
                                                       &dummy,
                                                       &advance );
            glyph->root.linearVertAdvance = advance;
          }
          else
          {
            /* make up vertical ones */
            if ( face->os2.version != 0xFFFFU )
              glyph->root.linearVertAdvance = (FT_Pos)
                ( face->os2.sTypoAscender - face->os2.sTypoDescender );
            else
              glyph->root.linearVertAdvance = (FT_Pos)
                ( face->horizontal.Ascender - face->horizontal.Descender );
          }

          return error;
        }
      }
    }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */

    /* return immediately if we only want the embedded bitmaps */
    if ( load_flags & FT_LOAD_SBITS_ONLY )
      return FT_THROW( Invalid_Argument );

    /* if we have a CID subfont, use its matrix (which has already */
    /* been multiplied with the root matrix)                       */

    /* this scaling is only relevant if the PS hinter isn't active */
    if ( cff->num_subfonts )
    {
      FT_ULong  top_upm, sub_upm;
      FT_Byte   fd_index = cff_fd_select_get( &cff->fd_select,
                                              glyph_index );


      if ( fd_index >= cff->num_subfonts )
        fd_index = (FT_Byte)( cff->num_subfonts - 1 );

      top_upm = cff->top_font.font_dict.units_per_em;
      sub_upm = cff->subfonts[fd_index]->font_dict.units_per_em;


      font_matrix = cff->subfonts[fd_index]->font_dict.font_matrix;
      font_offset = cff->subfonts[fd_index]->font_dict.font_offset;

      if ( top_upm != sub_upm )
      {
        glyph->x_scale = FT_MulDiv( glyph->x_scale, top_upm, sub_upm );
        glyph->y_scale = FT_MulDiv( glyph->y_scale, top_upm, sub_upm );

        force_scaling = TRUE;
      }
    }
    else
    {
      font_matrix = cff->top_font.font_dict.font_matrix;
      font_offset = cff->top_font.font_dict.font_offset;
    }

    glyph->root.outline.n_points   = 0;
    glyph->root.outline.n_contours = 0;

    /* top-level code ensures that FT_LOAD_NO_HINTING is set */
    /* if FT_LOAD_NO_SCALE is active                         */
    hinting = FT_BOOL( ( load_flags & FT_LOAD_NO_HINTING ) == 0 );
    scaled  = FT_BOOL( ( load_flags & FT_LOAD_NO_SCALE   ) == 0 );

    glyph->hint        = hinting;
    glyph->scaled      = scaled;
    glyph->root.format = FT_GLYPH_FORMAT_OUTLINE;  /* by default */

    {
#ifdef CFF_CONFIG_OPTION_OLD_ENGINE
      CFF_Driver  driver = (CFF_Driver)FT_FACE_DRIVER( face );
#endif


      FT_Byte*  charstring;
      FT_ULong  charstring_len;


      cff_decoder_init( &decoder, face, size, glyph, hinting,
                        FT_LOAD_TARGET_MODE( load_flags ) );

      if ( load_flags & FT_LOAD_ADVANCE_ONLY )
        decoder.width_only = TRUE;

      decoder.builder.no_recurse =
        (FT_Bool)( load_flags & FT_LOAD_NO_RECURSE );

      /* now load the unscaled outline */
      error = cff_get_glyph_data( face, glyph_index,
                                  &charstring, &charstring_len );
      if ( error )
        goto Glyph_Build_Finished;

      error = cff_decoder_prepare( &decoder, size, glyph_index );
      if ( error )
        goto Glyph_Build_Finished;

#ifdef CFF_CONFIG_OPTION_OLD_ENGINE
      /* choose which CFF renderer to use */
      if ( driver->hinting_engine == FT_CFF_HINTING_FREETYPE )
        error = cff_decoder_parse_charstrings( &decoder,
                                               charstring,
                                               charstring_len );
      else
#endif
      {
        error = cf2_decoder_parse_charstrings( &decoder,
                                               charstring,
                                               charstring_len );

        /* Adobe's engine uses 16.16 numbers everywhere;              */
        /* as a consequence, glyphs larger than 2000ppem get rejected */
        if ( FT_ERR_EQ( error, Glyph_Too_Big ) )
        {
          /* this time, we retry unhinted and scale up the glyph later on */
          /* (the engine uses and sets the hardcoded value 0x10000 / 64 = */
          /* 0x400 for both `x_scale' and `y_scale' in this case)         */
          hinting       = FALSE;
          force_scaling = TRUE;
          glyph->hint   = hinting;

          error = cf2_decoder_parse_charstrings( &decoder,
                                                 charstring,
                                                 charstring_len );
        }
      }

      cff_free_glyph_data( face, &charstring, charstring_len );

      if ( error )
        goto Glyph_Build_Finished;

#ifdef FT_CONFIG_OPTION_INCREMENTAL
      /* Control data and length may not be available for incremental */
      /* fonts.                                                       */
      if ( face->root.internal->incremental_interface )
      {
        glyph->root.control_data = 0;
        glyph->root.control_len = 0;
      }
      else
#endif /* FT_CONFIG_OPTION_INCREMENTAL */

      /* We set control_data and control_len if charstrings is loaded. */
      /* See how charstring loads at cff_index_access_element() in     */
      /* cffload.c.                                                    */
      {
        CFF_Index  csindex = &cff->charstrings_index;


        if ( csindex->offsets )
        {
          glyph->root.control_data = csindex->bytes +
                                     csindex->offsets[glyph_index] - 1;
          glyph->root.control_len  = charstring_len;
        }
      }

  Glyph_Build_Finished:
      /* save new glyph tables, if no error */
      if ( !error )
        cff_builder_done( &decoder.builder );
      /* XXX: anything to do for broken glyph entry? */
    }

#ifdef FT_CONFIG_OPTION_INCREMENTAL

    /* Incremental fonts can optionally override the metrics. */
    if ( !error                                                               &&
         face->root.internal->incremental_interface                           &&
         face->root.internal->incremental_interface->funcs->get_glyph_metrics )
    {
      FT_Incremental_MetricsRec  metrics;


      metrics.bearing_x = decoder.builder.left_bearing.x;
      metrics.bearing_y = 0;
      metrics.advance   = decoder.builder.advance.x;
      metrics.advance_v = decoder.builder.advance.y;

      error = face->root.internal->incremental_interface->funcs->get_glyph_metrics(
                face->root.internal->incremental_interface->object,
                glyph_index, FALSE, &metrics );

      decoder.builder.left_bearing.x = metrics.bearing_x;
      decoder.builder.advance.x      = metrics.advance;
      decoder.builder.advance.y      = metrics.advance_v;
    }

#endif /* FT_CONFIG_OPTION_INCREMENTAL */

    if ( !error )
    {
      /* Now, set the metrics -- this is rather simple, as   */
      /* the left side bearing is the xMin, and the top side */
      /* bearing the yMax.                                   */

      /* For composite glyphs, return only left side bearing and */
      /* advance width.                                          */
      if ( load_flags & FT_LOAD_NO_RECURSE )
      {
        FT_Slot_Internal  internal = glyph->root.internal;


        glyph->root.metrics.horiBearingX = decoder.builder.left_bearing.x;
        glyph->root.metrics.horiAdvance  = decoder.glyph_width;
        internal->glyph_matrix           = font_matrix;
        internal->glyph_delta            = font_offset;
        internal->glyph_transformed      = 1;
      }
      else
      {
        FT_BBox            cbox;
        FT_Glyph_Metrics*  metrics = &glyph->root.metrics;
        FT_Vector          advance;
        FT_Bool            has_vertical_info;


        /* copy the _unscaled_ advance width */
        metrics->horiAdvance                    = decoder.glyph_width;
        glyph->root.linearHoriAdvance           = decoder.glyph_width;
        glyph->root.internal->glyph_transformed = 0;

        has_vertical_info = FT_BOOL( face->vertical_info                   &&
                                     face->vertical.number_Of_VMetrics > 0 );

        /* get the vertical metrics from the vtmx table if we have one */
        if ( has_vertical_info )
        {
          FT_Short   vertBearingY = 0;
          FT_UShort  vertAdvance  = 0;


          ( (SFNT_Service)face->sfnt )->get_metrics( face, 1,
                                                     glyph_index,
                                                     &vertBearingY,
                                                     &vertAdvance );
          metrics->vertBearingY = vertBearingY;
          metrics->vertAdvance  = vertAdvance;
        }
        else
        {
          /* make up vertical ones */
          if ( face->os2.version != 0xFFFFU )
            metrics->vertAdvance = (FT_Pos)( face->os2.sTypoAscender -
                                             face->os2.sTypoDescender );
          else
            metrics->vertAdvance = (FT_Pos)( face->horizontal.Ascender -
                                             face->horizontal.Descender );
        }

        glyph->root.linearVertAdvance = metrics->vertAdvance;

        glyph->root.format = FT_GLYPH_FORMAT_OUTLINE;

        glyph->root.outline.flags = 0;
        if ( size && size->root.metrics.y_ppem < 24 )
          glyph->root.outline.flags |= FT_OUTLINE_HIGH_PRECISION;

        glyph->root.outline.flags |= FT_OUTLINE_REVERSE_FILL;

        if ( !( font_matrix.xx == 0x10000L &&
                font_matrix.yy == 0x10000L &&
                font_matrix.xy == 0        &&
                font_matrix.yx == 0        ) )
          FT_Outline_Transform( &glyph->root.outline, &font_matrix );

        if ( !( font_offset.x == 0 &&
                font_offset.y == 0 ) )
          FT_Outline_Translate( &glyph->root.outline,
                                font_offset.x, font_offset.y );

        advance.x = metrics->horiAdvance;
        advance.y = 0;
        FT_Vector_Transform( &advance, &font_matrix );
        metrics->horiAdvance = advance.x + font_offset.x;

        advance.x = 0;
        advance.y = metrics->vertAdvance;
        FT_Vector_Transform( &advance, &font_matrix );
        metrics->vertAdvance = advance.y + font_offset.y;

        if ( ( load_flags & FT_LOAD_NO_SCALE ) == 0 || force_scaling )
        {
          /* scale the outline and the metrics */
          FT_Int       n;
          FT_Outline*  cur     = &glyph->root.outline;
          FT_Vector*   vec     = cur->points;
          FT_Fixed     x_scale = glyph->x_scale;
          FT_Fixed     y_scale = glyph->y_scale;


          /* First of all, scale the points */
          if ( !hinting || !decoder.builder.hints_funcs )
            for ( n = cur->n_points; n > 0; n--, vec++ )
            {
              vec->x = FT_MulFix( vec->x, x_scale );
              vec->y = FT_MulFix( vec->y, y_scale );
            }

          /* Then scale the metrics */
          metrics->horiAdvance = FT_MulFix( metrics->horiAdvance, x_scale );
          metrics->vertAdvance = FT_MulFix( metrics->vertAdvance, y_scale );
        }

        /* compute the other metrics */
        FT_Outline_Get_CBox( &glyph->root.outline, &cbox );

        metrics->width  = cbox.xMax - cbox.xMin;
        metrics->height = cbox.yMax - cbox.yMin;

        metrics->horiBearingX = cbox.xMin;
        metrics->horiBearingY = cbox.yMax;

        if ( has_vertical_info )
          metrics->vertBearingX = metrics->horiBearingX -
                                    metrics->horiAdvance / 2;
        else
        {
          if ( load_flags & FT_LOAD_VERTICAL_LAYOUT )
            ft_synthesize_vertical_metrics( metrics,
                                            metrics->vertAdvance );
        }
      }
    }

    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cffcmap.c                                                              */
/*                                                                         */
/*    CFF character mapping table (cmap) support (body).                   */
/*                                                                         */
/*  Copyright 2002-2007, 2010, 2013 by                                     */
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
#include "cffcmap.h"
#include "cffload.h"

#include "cfferrs.h"


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****           CFF STANDARD (AND EXPERT) ENCODING CMAPS            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_CALLBACK_DEF( FT_Error )
  cff_cmap_encoding_init( CFF_CMapStd  cmap )
  {
    TT_Face       face     = (TT_Face)FT_CMAP_FACE( cmap );
    CFF_Font      cff      = (CFF_Font)face->extra.data;
    CFF_Encoding  encoding = &cff->encoding;


    cmap->gids  = encoding->codes;

    return 0;
  }


  FT_CALLBACK_DEF( void )
  cff_cmap_encoding_done( CFF_CMapStd  cmap )
  {
    cmap->gids  = NULL;
  }


  FT_CALLBACK_DEF( FT_UInt )
  cff_cmap_encoding_char_index( CFF_CMapStd  cmap,
                                FT_UInt32    char_code )
  {
    FT_UInt  result = 0;


    if ( char_code < 256 )
      result = cmap->gids[char_code];

    return result;
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  cff_cmap_encoding_char_next( CFF_CMapStd   cmap,
                               FT_UInt32    *pchar_code )
  {
    FT_UInt    result    = 0;
    FT_UInt32  char_code = *pchar_code;


    *pchar_code = 0;

    if ( char_code < 255 )
    {
      FT_UInt  code = (FT_UInt)(char_code + 1);


      for (;;)
      {
        if ( code >= 256 )
          break;

        result = cmap->gids[code];
        if ( result != 0 )
        {
          *pchar_code = code;
          break;
        }

        code++;
      }
    }
    return result;
  }


  FT_DEFINE_CMAP_CLASS(cff_cmap_encoding_class_rec,
    sizeof ( CFF_CMapStdRec ),

    (FT_CMap_InitFunc)     cff_cmap_encoding_init,
    (FT_CMap_DoneFunc)     cff_cmap_encoding_done,
    (FT_CMap_CharIndexFunc)cff_cmap_encoding_char_index,
    (FT_CMap_CharNextFunc) cff_cmap_encoding_char_next,

    NULL, NULL, NULL, NULL, NULL
  )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****              CFF SYNTHETIC UNICODE ENCODING CMAP              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_CALLBACK_DEF( const char* )
  cff_sid_to_glyph_name( TT_Face  face,
                         FT_UInt  idx )
  {
    CFF_Font     cff     = (CFF_Font)face->extra.data;
    CFF_Charset  charset = &cff->charset;
    FT_UInt      sid     = charset->sids[idx];


    return cff_index_get_sid_string( cff, sid );
  }


  FT_CALLBACK_DEF( FT_Error )
  cff_cmap_unicode_init( PS_Unicodes  unicodes )
  {
    TT_Face             face    = (TT_Face)FT_CMAP_FACE( unicodes );
    FT_Memory           memory  = FT_FACE_MEMORY( face );
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    CFF_Charset         charset = &cff->charset;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;


    /* can't build Unicode map for CID-keyed font */
    /* because we don't know glyph names.         */
    if ( !charset->sids )
      return FT_THROW( No_Unicode_Glyph_Name );

    return psnames->unicodes_init( memory,
                                   unicodes,
                                   cff->num_glyphs,
                                   (PS_GetGlyphNameFunc)&cff_sid_to_glyph_name,
                                   (PS_FreeGlyphNameFunc)NULL,
                                   (FT_Pointer)face );
  }


  FT_CALLBACK_DEF( void )
  cff_cmap_unicode_done( PS_Unicodes  unicodes )
  {
    FT_Face    face   = FT_CMAP_FACE( unicodes );
    FT_Memory  memory = FT_FACE_MEMORY( face );


    FT_FREE( unicodes->maps );
    unicodes->num_maps = 0;
  }


  FT_CALLBACK_DEF( FT_UInt )
  cff_cmap_unicode_char_index( PS_Unicodes  unicodes,
                               FT_UInt32    char_code )
  {
    TT_Face             face    = (TT_Face)FT_CMAP_FACE( unicodes );
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;


    return psnames->unicodes_char_index( unicodes, char_code );
  }


  FT_CALLBACK_DEF( FT_UInt32 )
  cff_cmap_unicode_char_next( PS_Unicodes  unicodes,
                              FT_UInt32   *pchar_code )
  {
    TT_Face             face    = (TT_Face)FT_CMAP_FACE( unicodes );
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;


    return psnames->unicodes_char_next( unicodes, pchar_code );
  }


  FT_DEFINE_CMAP_CLASS(cff_cmap_unicode_class_rec,
    sizeof ( PS_UnicodesRec ),

    (FT_CMap_InitFunc)     cff_cmap_unicode_init,
    (FT_CMap_DoneFunc)     cff_cmap_unicode_done,
    (FT_CMap_CharIndexFunc)cff_cmap_unicode_char_index,
    (FT_CMap_CharNextFunc) cff_cmap_unicode_char_next,

    NULL, NULL, NULL, NULL, NULL
  )

/* END */

/***************************************************************************/
/*                                                                         */
/*  cf2arrst.c                                                             */
/*                                                                         */
/*    Adobe's code for Array Stacks (body).                                */
/*                                                                         */
/*  Copyright 2007-2013 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2glue.h"
#include "cf2arrst.h"

#include "cf2error.h"


  /*
   * CF2_ArrStack uses an error pointer, to enable shared errors.
   * Shared errors are necessary when multiple objects allow the program
   * to continue after detecting errors.  Only the first error should be
   * recorded.
   */

  FT_LOCAL_DEF( void )
  cf2_arrstack_init( CF2_ArrStack  arrstack,
                     FT_Memory     memory,
                     FT_Error*     error,
                     size_t        sizeItem )
  {
    FT_ASSERT( arrstack != NULL );

    /* initialize the structure */
    arrstack->memory    = memory;
    arrstack->error     = error;
    arrstack->sizeItem  = sizeItem;
    arrstack->allocated = 0;
    arrstack->chunk     = 10;    /* chunks of 10 items */
    arrstack->count     = 0;
    arrstack->totalSize = 0;
    arrstack->ptr       = NULL;
  }


  FT_LOCAL_DEF( void )
  cf2_arrstack_finalize( CF2_ArrStack  arrstack )
  {
    FT_Memory  memory = arrstack->memory;     /* for FT_FREE */


    FT_ASSERT( arrstack != NULL );

    arrstack->allocated = 0;
    arrstack->count     = 0;
    arrstack->totalSize = 0;

    /* free the data buffer */
    FT_FREE( arrstack->ptr );
  }


  /* allocate or reallocate the buffer size; */
  /* return false on memory error */
  static FT_Bool
  cf2_arrstack_setNumElements( CF2_ArrStack  arrstack,
                               size_t        numElements )
  {
    FT_ASSERT( arrstack != NULL );

    {
      FT_Error   error  = FT_Err_Ok;        /* for FT_REALLOC */
      FT_Memory  memory = arrstack->memory; /* for FT_REALLOC */

      FT_Long  newSize = (FT_Long)( numElements * arrstack->sizeItem );


      if ( numElements > LONG_MAX / arrstack->sizeItem )
        goto exit;


      FT_ASSERT( newSize > 0 );   /* avoid realloc with zero size */

      if ( !FT_REALLOC( arrstack->ptr, arrstack->totalSize, newSize ) )
      {
        arrstack->allocated = numElements;
        arrstack->totalSize = newSize;

        if ( arrstack->count > numElements )
        {
          /* we truncated the list! */
          CF2_SET_ERROR( arrstack->error, Stack_Overflow );
          arrstack->count = numElements;
          return FALSE;
        }

        return TRUE;     /* success */
      }
    }

  exit:
    /* if there's not already an error, store this one */
    CF2_SET_ERROR( arrstack->error, Out_Of_Memory );

    return FALSE;
  }


  /* set the count, ensuring allocation is sufficient */
  FT_LOCAL_DEF( void )
  cf2_arrstack_setCount( CF2_ArrStack  arrstack,
                         size_t        numElements )
  {
    FT_ASSERT( arrstack != NULL );

    if ( numElements > arrstack->allocated )
    {
      /* expand the allocation first */
      if ( !cf2_arrstack_setNumElements( arrstack, numElements ) )
        return;
    }

    arrstack->count = numElements;
  }


  /* clear the count */
  FT_LOCAL_DEF( void )
  cf2_arrstack_clear( CF2_ArrStack  arrstack )
  {
    FT_ASSERT( arrstack != NULL );

    arrstack->count = 0;
  }


  /* current number of items */
  FT_LOCAL_DEF( size_t )
  cf2_arrstack_size( const CF2_ArrStack  arrstack )
  {
    FT_ASSERT( arrstack != NULL );

    return arrstack->count;
  }


  FT_LOCAL_DEF( void* )
  cf2_arrstack_getBuffer( const CF2_ArrStack  arrstack )
  {
    FT_ASSERT( arrstack != NULL );

    return arrstack->ptr;
  }


  /* return pointer to the given element */
  FT_LOCAL_DEF( void* )
  cf2_arrstack_getPointer( const CF2_ArrStack  arrstack,
                           size_t              idx )
  {
    void*  newPtr;


    FT_ASSERT( arrstack != NULL );

    if ( idx >= arrstack->count )
    {
      /* overflow */
      CF2_SET_ERROR( arrstack->error, Stack_Overflow );
      idx = 0;    /* choose safe default */
    }

    newPtr = (FT_Byte*)arrstack->ptr + idx * arrstack->sizeItem;

    return newPtr;
  }


  /* push (append) an element at the end of the list;         */
  /* return false on memory error                             */
  /* TODO: should there be a length param for extra checking? */
  FT_LOCAL_DEF( void )
  cf2_arrstack_push( CF2_ArrStack  arrstack,
                     const void*   ptr )
  {
    FT_ASSERT( arrstack != NULL );

    if ( arrstack->count == arrstack->allocated )
    {
      /* grow the buffer by one chunk */
      if ( !cf2_arrstack_setNumElements(
             arrstack, arrstack->allocated + arrstack->chunk ) )
      {
        /* on error, ignore the push */
        return;
      }
    }

    FT_ASSERT( ptr != NULL );

    {
      size_t  offset = arrstack->count * arrstack->sizeItem;
      void*   newPtr = (FT_Byte*)arrstack->ptr + offset;


      FT_MEM_COPY( newPtr, ptr, arrstack->sizeItem );
      arrstack->count += 1;
    }
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2blues.c                                                             */
/*                                                                         */
/*    Adobe's code for handling Blue Zones (body).                         */
/*                                                                         */
/*  Copyright 2009-2014 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2blues.h"
#include "cf2hints.h"
#include "cf2font.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cf2blues


  /*
   * For blue values, the FreeType parser produces an array of integers,
   * while the Adobe CFF engine produces an array of fixed.
   * Define a macro to convert FreeType to fixed.
   */
#define cf2_blueToFixed( x )  cf2_intToFixed( x )


  FT_LOCAL_DEF( void )
  cf2_blues_init( CF2_Blues  blues,
                  CF2_Font   font )
  {
    /* pointer to parsed font object */
    CFF_Decoder*  decoder = font->decoder;

    CF2_Fixed  zoneHeight;
    CF2_Fixed  maxZoneHeight = 0;
    CF2_Fixed  csUnitsPerPixel;

    size_t  numBlueValues;
    size_t  numOtherBlues;
    size_t  numFamilyBlues;
    size_t  numFamilyOtherBlues;

    FT_Pos*  blueValues;
    FT_Pos*  otherBlues;
    FT_Pos*  familyBlues;
    FT_Pos*  familyOtherBlues;

    size_t     i;
    CF2_Fixed  emBoxBottom, emBoxTop;

#if 0
    CF2_Int  unitsPerEm = font->unitsPerEm;


    if ( unitsPerEm == 0 )
      unitsPerEm = 1000;
#endif

    FT_ZERO( blues );
    blues->scale = font->innerTransform.d;

    cf2_getBlueMetrics( decoder,
                        &blues->blueScale,
                        &blues->blueShift,
                        &blues->blueFuzz );

    cf2_getBlueValues( decoder, &numBlueValues, &blueValues );
    cf2_getOtherBlues( decoder, &numOtherBlues, &otherBlues );
    cf2_getFamilyBlues( decoder, &numFamilyBlues, &familyBlues );
    cf2_getFamilyOtherBlues( decoder, &numFamilyOtherBlues, &familyOtherBlues );

    /*
     * synthetic em box hint heuristic
     *
     * Apply this when ideographic dictionary (LanguageGroup 1) has no
     * real alignment zones.  Adobe tools generate dummy zones at -250 and
     * 1100 for a 1000 unit em.  Fonts with ICF-based alignment zones
     * should not enable the heuristic.  When the heuristic is enabled,
     * the font's blue zones are ignored.
     *
     */

    /* get em box from OS/2 typoAscender/Descender                      */
    /* TODO: FreeType does not parse these metrics.  Skip them for now. */
#if 0
    FCM_getHorizontalLineMetrics( &e,
                                  font->font,
                                  &ascender,
                                  &descender,
                                  &linegap );
    if ( ascender - descender == unitsPerEm )
    {
      emBoxBottom = cf2_intToFixed( descender );
      emBoxTop    = cf2_intToFixed( ascender );
    }
    else
#endif
    {
      emBoxBottom = CF2_ICF_Bottom;
      emBoxTop    = CF2_ICF_Top;
    }

    if ( cf2_getLanguageGroup( decoder ) == 1                   &&
         ( numBlueValues == 0                                 ||
           ( numBlueValues == 4                             &&
             cf2_blueToFixed( blueValues[0] ) < emBoxBottom &&
             cf2_blueToFixed( blueValues[1] ) < emBoxBottom &&
             cf2_blueToFixed( blueValues[2] ) > emBoxTop    &&
             cf2_blueToFixed( blueValues[3] ) > emBoxTop    ) ) )
    {
      /*
       * Construct hint edges suitable for synthetic ghost hints at top
       * and bottom of em box.  +-CF2_MIN_COUNTER allows for unhinted
       * features above or below the last hinted edge.  This also gives a
       * net 1 pixel boost to the height of ideographic glyphs.
       *
       * Note: Adjust synthetic hints outward by epsilon (0x.0001) to
       *       avoid interference.  E.g., some fonts have real hints at
       *       880 and -120.
       */

      blues->emBoxBottomEdge.csCoord = emBoxBottom - CF2_FIXED_EPSILON;
      blues->emBoxBottomEdge.dsCoord = cf2_fixedRound(
                                         FT_MulFix(
                                           blues->emBoxBottomEdge.csCoord,
                                           blues->scale ) ) -
                                       CF2_MIN_COUNTER;
      blues->emBoxBottomEdge.scale   = blues->scale;
      blues->emBoxBottomEdge.flags   = CF2_GhostBottom |
                                       CF2_Locked |
                                       CF2_Synthetic;

      blues->emBoxTopEdge.csCoord = emBoxTop + CF2_FIXED_EPSILON +
                                    2 * font->darkenY;
      blues->emBoxTopEdge.dsCoord = cf2_fixedRound(
                                      FT_MulFix(
                                        blues->emBoxTopEdge.csCoord,
                                        blues->scale ) ) +
                                    CF2_MIN_COUNTER;
      blues->emBoxTopEdge.scale   = blues->scale;
      blues->emBoxTopEdge.flags   = CF2_GhostTop |
                                    CF2_Locked |
                                    CF2_Synthetic;

      blues->doEmBoxHints = TRUE;    /* enable the heuristic */

      return;
    }

    /* copy `BlueValues' and `OtherBlues' to a combined array of top and */
    /* bottom zones                                                      */
    for ( i = 0; i < numBlueValues; i += 2 )
    {
      blues->zone[blues->count].csBottomEdge =
        cf2_blueToFixed( blueValues[i] );
      blues->zone[blues->count].csTopEdge =
        cf2_blueToFixed( blueValues[i + 1] );

      zoneHeight = blues->zone[blues->count].csTopEdge -
                   blues->zone[blues->count].csBottomEdge;

      if ( zoneHeight < 0 )
      {
        FT_TRACE4(( "cf2_blues_init: ignoring negative zone height\n" ));
        continue;   /* reject this zone */
      }

      if ( zoneHeight > maxZoneHeight )
      {
        /* take maximum before darkening adjustment      */
        /* so overshoot suppression point doesn't change */
        maxZoneHeight = zoneHeight;
      }

      /* adjust both edges of top zone upward by twice darkening amount */
      if ( i != 0 )
      {
        blues->zone[blues->count].csTopEdge    += 2 * font->darkenY;
        blues->zone[blues->count].csBottomEdge += 2 * font->darkenY;
      }

      /* first `BlueValue' is bottom zone; others are top */
      if ( i == 0 )
      {
        blues->zone[blues->count].bottomZone =
          TRUE;
        blues->zone[blues->count].csFlatEdge =
          blues->zone[blues->count].csTopEdge;
      }
      else
      {
        blues->zone[blues->count].bottomZone =
          FALSE;
        blues->zone[blues->count].csFlatEdge =
          blues->zone[blues->count].csBottomEdge;
      }

      blues->count += 1;
    }

    for ( i = 0; i < numOtherBlues; i += 2 )
    {
      blues->zone[blues->count].csBottomEdge =
        cf2_blueToFixed( otherBlues[i] );
      blues->zone[blues->count].csTopEdge =
        cf2_blueToFixed( otherBlues[i + 1] );

      zoneHeight = blues->zone[blues->count].csTopEdge -
                   blues->zone[blues->count].csBottomEdge;

      if ( zoneHeight < 0 )
      {
        FT_TRACE4(( "cf2_blues_init: ignoring negative zone height\n" ));
        continue;   /* reject this zone */
      }

      if ( zoneHeight > maxZoneHeight )
      {
        /* take maximum before darkening adjustment      */
        /* so overshoot suppression point doesn't change */
        maxZoneHeight = zoneHeight;
      }

      /* Note: bottom zones are not adjusted for darkening amount */

      /* all OtherBlues are bottom zone */
      blues->zone[blues->count].bottomZone =
        TRUE;
      blues->zone[blues->count].csFlatEdge =
        blues->zone[blues->count].csTopEdge;

      blues->count += 1;
    }

    /* Adjust for FamilyBlues */

    /* Search for the nearest flat edge in `FamilyBlues' or                */
    /* `FamilyOtherBlues'.  According to the Black Book, any matching edge */
    /* must be within one device pixel                                     */

    csUnitsPerPixel = FT_DivFix( cf2_intToFixed( 1 ), blues->scale );

    /* loop on all zones in this font */
    for ( i = 0; i < blues->count; i++ )
    {
      size_t     j;
      CF2_Fixed  minDiff;
      CF2_Fixed  flatFamilyEdge, diff;
      /* value for this font */
      CF2_Fixed  flatEdge = blues->zone[i].csFlatEdge;


      if ( blues->zone[i].bottomZone )
      {
        /* In a bottom zone, the top edge is the flat edge.             */
        /* Search `FamilyOtherBlues' for bottom zones; look for closest */
        /* Family edge that is within the one pixel threshold.          */

        minDiff = CF2_FIXED_MAX;

        for ( j = 0; j < numFamilyOtherBlues; j += 2 )
        {
          /* top edge */
          flatFamilyEdge = cf2_blueToFixed( familyOtherBlues[j + 1] );

          diff = cf2_fixedAbs( flatEdge - flatFamilyEdge );

          if ( diff < minDiff && diff < csUnitsPerPixel )
          {
            blues->zone[i].csFlatEdge = flatFamilyEdge;
            minDiff                   = diff;

            if ( diff == 0 )
              break;
          }
        }

        /* check the first member of FamilyBlues, which is a bottom zone */
        if ( numFamilyBlues >= 2 )
        {
          /* top edge */
          flatFamilyEdge = cf2_blueToFixed( familyBlues[1] );

          diff = cf2_fixedAbs( flatEdge - flatFamilyEdge );

          if ( diff < minDiff && diff < csUnitsPerPixel )
            blues->zone[i].csFlatEdge = flatFamilyEdge;
        }
      }
      else
      {
        /* In a top zone, the bottom edge is the flat edge.                */
        /* Search `FamilyBlues' for top zones; skip first zone, which is a */
        /* bottom zone; look for closest Family edge that is within the    */
        /* one pixel threshold                                             */

        minDiff = CF2_FIXED_MAX;

        for ( j = 2; j < numFamilyBlues; j += 2 )
        {
          /* bottom edge */
          flatFamilyEdge = cf2_blueToFixed( familyBlues[j] );

          /* adjust edges of top zone upward by twice darkening amount */
          flatFamilyEdge += 2 * font->darkenY;      /* bottom edge */

          diff = cf2_fixedAbs( flatEdge - flatFamilyEdge );

          if ( diff < minDiff && diff < csUnitsPerPixel )
          {
            blues->zone[i].csFlatEdge = flatFamilyEdge;
            minDiff                   = diff;

            if ( diff == 0 )
              break;
          }
        }
      }
    }

    /* TODO: enforce separation of zones, including BlueFuzz */

    /* Adjust BlueScale; similar to AdjustBlueScale() in coretype */
    /* `bcsetup.c'.                                               */

    if ( maxZoneHeight > 0 )
    {
      if ( blues->blueScale > FT_DivFix( cf2_intToFixed( 1 ),
                                         maxZoneHeight ) )
      {
        /* clamp at maximum scale */
        blues->blueScale = FT_DivFix( cf2_intToFixed( 1 ),
                                      maxZoneHeight );
      }

      /*
       * TODO: Revisit the bug fix for 613448.  The minimum scale
       *       requirement catches a number of library fonts.  For
       *       example, with default BlueScale (.039625) and 0.4 minimum,
       *       the test below catches any font with maxZoneHeight < 10.1.
       *       There are library fonts ranging from 2 to 10 that get
       *       caught, including e.g., Eurostile LT Std Medium with
       *       maxZoneHeight of 6.
       *
       */
#if 0
      if ( blueScale < .4 / maxZoneHeight )
      {
        tetraphilia_assert( 0 );
        /* clamp at minimum scale, per bug 0613448 fix */
        blueScale = .4 / maxZoneHeight;
      }
#endif

    }

    /*
     * Suppress overshoot and boost blue zones at small sizes.  Boost
     * amount varies linearly from 0.5 pixel near 0 to 0 pixel at
     * blueScale cutoff.
     * Note: This boost amount is different from the coretype heuristic.
     *
     */

    if ( blues->scale < blues->blueScale )
    {
      blues->suppressOvershoot = TRUE;

      /* Change rounding threshold for `dsFlatEdge'.                    */
      /* Note: constant changed from 0.5 to 0.6 to avoid a problem with */
      /*       10ppem Arial                                             */

      blues->boost = cf2_floatToFixed( .6 ) -
                       FT_MulDiv( cf2_floatToFixed ( .6 ),
                                  blues->scale,
                                  blues->blueScale );
      if ( blues->boost > 0x7FFF )
      {
        /* boost must remain less than 0.5, or baseline could go negative */
        blues->boost = 0x7FFF;
      }
    }

    /* boost and darkening have similar effects; don't do both */
    if ( font->stemDarkened )
      blues->boost = 0;

    /* set device space alignment for each zone;    */
    /* apply boost amount before rounding flat edge */

    for ( i = 0; i < blues->count; i++ )
    {
      if ( blues->zone[i].bottomZone )
        blues->zone[i].dsFlatEdge = cf2_fixedRound(
                                      FT_MulFix(
                                        blues->zone[i].csFlatEdge,
                                        blues->scale ) -
                                      blues->boost );
      else
        blues->zone[i].dsFlatEdge = cf2_fixedRound(
                                      FT_MulFix(
                                        blues->zone[i].csFlatEdge,
                                        blues->scale ) +
                                      blues->boost );
    }
  }


  /*
   * Check whether `stemHint' is captured by one of the blue zones.
   *
   * Zero, one or both edges may be valid; only valid edges can be
   * captured.  For compatibility with CoolType, search top and bottom
   * zones in the same pass (see `BlueLock').  If a hint is captured,
   * return true and position the edge(s) in one of 3 ways:
   *
   *  1) If `BlueScale' suppresses overshoot, position the captured edge
   *     at the flat edge of the zone.
   *  2) If overshoot is not suppressed and `BlueShift' requires
   *     overshoot, position the captured edge a minimum of 1 device pixel
   *     from the flat edge.
   *  3) If overshoot is not suppressed or required, position the captured
   *     edge at the nearest device pixel.
   *
   */
  FT_LOCAL_DEF( FT_Bool )
  cf2_blues_capture( const CF2_Blues  blues,
                     CF2_Hint         bottomHintEdge,
                     CF2_Hint         topHintEdge )
  {
    /* TODO: validate? */
    CF2_Fixed  csFuzz = blues->blueFuzz;

    /* new position of captured edge */
    CF2_Fixed  dsNew;

    /* amount that hint is moved when positioned */
    CF2_Fixed  dsMove = 0;

    FT_Bool   captured = FALSE;
    CF2_UInt  i;


    /* assert edge flags are consistent */
    FT_ASSERT( !cf2_hint_isTop( bottomHintEdge ) &&
               !cf2_hint_isBottom( topHintEdge ) );

    /* TODO: search once without blue fuzz for compatibility with coretype? */
    for ( i = 0; i < blues->count; i++ )
    {
      if ( blues->zone[i].bottomZone           &&
           cf2_hint_isBottom( bottomHintEdge ) )
      {
        if ( ( blues->zone[i].csBottomEdge - csFuzz ) <=
               bottomHintEdge->csCoord                   &&
             bottomHintEdge->csCoord <=
               ( blues->zone[i].csTopEdge + csFuzz )     )
        {
          /* bottom edge captured by bottom zone */

          if ( blues->suppressOvershoot )
            dsNew = blues->zone[i].dsFlatEdge;

          else if ( ( blues->zone[i].csTopEdge - bottomHintEdge->csCoord ) >=
                      blues->blueShift )
          {
            /* guarantee minimum of 1 pixel overshoot */
            dsNew = FT_MIN(
                      cf2_fixedRound( bottomHintEdge->dsCoord ),
                      blues->zone[i].dsFlatEdge - cf2_intToFixed( 1 ) );
          }

          else
          {
            /* simply round captured edge */
            dsNew = cf2_fixedRound( bottomHintEdge->dsCoord );
          }

          dsMove   = dsNew - bottomHintEdge->dsCoord;
          captured = TRUE;

          break;
        }
      }

      if ( !blues->zone[i].bottomZone && cf2_hint_isTop( topHintEdge ) )
      {
        if ( ( blues->zone[i].csBottomEdge - csFuzz ) <=
               topHintEdge->csCoord                      &&
             topHintEdge->csCoord <=
               ( blues->zone[i].csTopEdge + csFuzz )     )
        {
          /* top edge captured by top zone */

          if ( blues->suppressOvershoot )
            dsNew = blues->zone[i].dsFlatEdge;

          else if ( ( topHintEdge->csCoord - blues->zone[i].csBottomEdge ) >=
                      blues->blueShift )
          {
            /* guarantee minimum of 1 pixel overshoot */
            dsNew = FT_MAX(
                      cf2_fixedRound( topHintEdge->dsCoord ),
                      blues->zone[i].dsFlatEdge + cf2_intToFixed( 1 ) );
          }

          else
          {
            /* simply round captured edge */
            dsNew = cf2_fixedRound( topHintEdge->dsCoord );
          }

          dsMove   = dsNew - topHintEdge->dsCoord;
          captured = TRUE;

          break;
        }
      }
    }

    if ( captured )
    {
      /* move both edges and flag them `locked' */
      if ( cf2_hint_isValid( bottomHintEdge ) )
      {
        bottomHintEdge->dsCoord += dsMove;
        cf2_hint_lock( bottomHintEdge );
      }

      if ( cf2_hint_isValid( topHintEdge ) )
      {
        topHintEdge->dsCoord += dsMove;
        cf2_hint_lock( topHintEdge );
      }
    }

    return captured;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2error.c                                                             */
/*                                                                         */
/*    Adobe's code for error handling (body).                              */
/*                                                                         */
/*  Copyright 2006-2013 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include "cf2error.h"


  FT_LOCAL_DEF( void )
  cf2_setError( FT_Error*  error,
                FT_Error   value )
  {
    if ( error && *error == 0 )
      *error = value;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2font.c                                                              */
/*                                                                         */
/*    Adobe's code for font instances (body).                              */
/*                                                                         */
/*  Copyright 2007-2014 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"

#include "cf2glue.h"
#include "cf2font.h"
#include "cf2error.h"
#include "cf2intrp.h"


  /* Compute a stem darkening amount in character space. */
  static void
  cf2_computeDarkening( CF2_Fixed   emRatio,
                        CF2_Fixed   ppem,
                        CF2_Fixed   stemWidth,
                        CF2_Fixed*  darkenAmount,
                        CF2_Fixed   boldenAmount,
                        FT_Bool     stemDarkened,
                        FT_Int*     darkenParams )
  {
    /*
     * Total darkening amount is computed in 1000 unit character space
     * using the modified 5 part curve as Adobe's Avalon rasterizer.
     * The darkening amount is smaller for thicker stems.
     * It becomes zero when the stem is thicker than 2.333 pixels.
     *
     * By default, we use
     *
     *   darkenAmount = 0.4 pixels   if scaledStem <= 0.5 pixels,
     *   darkenAmount = 0.275 pixels if 1 <= scaledStem <= 1.667 pixels,
     *   darkenAmount = 0 pixel      if scaledStem >= 2.333 pixels,
     *
     * and piecewise linear in-between:
     *
     *
     *   darkening
     *       ^
     *       |
     *       |      (x1,y1)
     *       |--------+
     *       |         \
     *       |          \
     *       |           \          (x3,y3)
     *       |            +----------+
     *       |        (x2,y2)         \
     *       |                         \
     *       |                          \
     *       |                           +-----------------
     *       |                         (x4,y4)
     *       +--------------------------------------------->   stem
     *                                                       thickness
     *
     *
     * This corresponds to the following values for the
     * `darkening-parameters' property:
     *
     *   (x1, y1) = (500, 400)
     *   (x2, y2) = (1000, 275)
     *   (x3, y3) = (1667, 275)
     *   (x4, y4) = (2333, 0)
     *
     */

    /* Internal calculations are done in units per thousand for */
    /* convenience. The x axis is scaled stem width in          */
    /* thousandths of a pixel. That is, 1000 is 1 pixel.        */
    /* The y axis is darkening amount in thousandths of a pixel.*/
    /* In the code, below, dividing by ppem and                 */
    /* adjusting for emRatio converts darkenAmount to character */
    /* space (font units).                                      */
    CF2_Fixed  stemWidthPer1000, scaledStem;


    *darkenAmount = 0;

    if ( boldenAmount == 0 && !stemDarkened )
      return;

    /* protect against range problems and divide by zero */
    if ( emRatio < cf2_floatToFixed( .01 ) )
      return;

    if ( stemDarkened )
    {
      FT_Int  x1 = darkenParams[0];
      FT_Int  y1 = darkenParams[1];
      FT_Int  x2 = darkenParams[2];
      FT_Int  y2 = darkenParams[3];
      FT_Int  x3 = darkenParams[4];
      FT_Int  y3 = darkenParams[5];
      FT_Int  x4 = darkenParams[6];
      FT_Int  y4 = darkenParams[7];


      /* convert from true character space to 1000 unit character space; */
      /* add synthetic emboldening effect                                */

      /* we have to assure that the computation of `scaledStem' */
      /* and `stemWidthPer1000' don't overflow                  */

      stemWidthPer1000 = FT_MulFix( stemWidth + boldenAmount, emRatio );

      if ( emRatio > CF2_FIXED_ONE                          &&
           stemWidthPer1000 <= ( stemWidth + boldenAmount ) )
      {
        stemWidthPer1000 = 0;                      /* to pacify compiler */
        scaledStem       = cf2_intToFixed( x4 );
      }
      else
      {
        scaledStem = FT_MulFix( stemWidthPer1000, ppem );

        if ( ppem > CF2_FIXED_ONE           &&
             scaledStem <= stemWidthPer1000 )
          scaledStem = cf2_intToFixed( x4 );
      }

      /* now apply the darkening parameters */

      if ( scaledStem < cf2_intToFixed( x1 ) )
        *darkenAmount = FT_DivFix( cf2_intToFixed( y1 ), ppem );

      else if ( scaledStem < cf2_intToFixed( x2 ) )
      {
        FT_Int  xdelta = x2 - x1;
        FT_Int  ydelta = y2 - y1;
        FT_Int  x      = stemWidthPer1000 -
                           FT_DivFix( cf2_intToFixed( x1 ), ppem );


        if ( !xdelta )
          goto Try_x3;

        *darkenAmount = FT_MulDiv( x, ydelta, xdelta ) +
                          FT_DivFix( cf2_intToFixed( y1 ), ppem );
      }

      else if ( scaledStem < cf2_intToFixed( x3 ) )
      {
      Try_x3:
        {
          FT_Int  xdelta = x3 - x2;
          FT_Int  ydelta = y3 - y2;
          FT_Int  x      = stemWidthPer1000 -
                             FT_DivFix( cf2_intToFixed( x2 ), ppem );


          if ( !xdelta )
            goto Try_x4;

          *darkenAmount = FT_MulDiv( x, ydelta, xdelta ) +
                            FT_DivFix( cf2_intToFixed( y2 ), ppem );
        }
      }

      else if ( scaledStem < cf2_intToFixed( x4 ) )
      {
      Try_x4:
        {
          FT_Int  xdelta = x4 - x3;
          FT_Int  ydelta = y4 - y3;
          FT_Int  x      = stemWidthPer1000 -
                             FT_DivFix( cf2_intToFixed( x3 ), ppem );


          if ( !xdelta )
            goto Use_y4;

          *darkenAmount = FT_MulDiv( x, ydelta, xdelta ) +
                            FT_DivFix( cf2_intToFixed( y3 ), ppem );
        }
      }

      else
      {
      Use_y4:
        *darkenAmount = FT_DivFix( cf2_intToFixed( y4 ), ppem );
      }

      /* use half the amount on each side and convert back to true */
      /* character space                                           */
      *darkenAmount = FT_DivFix( *darkenAmount, 2 * emRatio );
    }

    /* add synthetic emboldening effect in character space */
    *darkenAmount += boldenAmount / 2;
  }


  /* set up values for the current FontDict and matrix */

  /* caller's transform is adjusted for subpixel positioning */
  static void
  cf2_font_setup( CF2_Font           font,
                  const CF2_Matrix*  transform )
  {
    /* pointer to parsed font object */
    CFF_Decoder*  decoder = font->decoder;

    FT_Bool  needExtraSetup = FALSE;

    /* character space units */
    CF2_Fixed  boldenX = font->syntheticEmboldeningAmountX;
    CF2_Fixed  boldenY = font->syntheticEmboldeningAmountY;

    CFF_SubFont  subFont;
    CF2_Fixed    ppem;


    /* clear previous error */
    font->error = FT_Err_Ok;

    /* if a CID fontDict has changed, we need to recompute some cached */
    /* data                                                            */
    subFont = cf2_getSubfont( decoder );
    if ( font->lastSubfont != subFont )
    {
      font->lastSubfont = subFont;
      needExtraSetup    = TRUE;
    }

    /* if ppem has changed, we need to recompute some cached data         */
    /* note: because of CID font matrix concatenation, ppem and transform */
    /*       do not necessarily track.                                    */
    ppem = cf2_getPpemY( decoder );
    if ( font->ppem != ppem )
    {
      font->ppem     = ppem;
      needExtraSetup = TRUE;
    }

    /* copy hinted flag on each call */
    font->hinted = (FT_Bool)( font->renderingFlags & CF2_FlagsHinted );

    /* determine if transform has changed;       */
    /* include Fontmatrix but ignore translation */
    if ( ft_memcmp( transform,
                    &font->currentTransform,
                    4 * sizeof ( CF2_Fixed ) ) != 0 )
    {
      /* save `key' information for `cache of one' matrix data; */
      /* save client transform, without the translation         */
      font->currentTransform    = *transform;
      font->currentTransform.tx =
      font->currentTransform.ty = cf2_intToFixed( 0 );

      /* TODO: FreeType transform is simple scalar; for now, use identity */
      /*       for outer                                                  */
      font->innerTransform   = *transform;
      font->outerTransform.a =
      font->outerTransform.d = cf2_intToFixed( 1 );
      font->outerTransform.b =
      font->outerTransform.c = cf2_intToFixed( 0 );

      needExtraSetup = TRUE;
    }

    /*
     * font->darkened is set to true if there is a stem darkening request or
     * the font is synthetic emboldened.
     * font->darkened controls whether to adjust blue zones, winding order,
     * and hinting.
     *
     */
    if ( font->stemDarkened != ( font->renderingFlags & CF2_FlagsDarkened ) )
    {
      font->stemDarkened =
        (FT_Bool)( font->renderingFlags & CF2_FlagsDarkened );

      /* blue zones depend on darkened flag */
      needExtraSetup = TRUE;
    }

    /* recompute variables that are dependent on transform or FontDict or */
    /* darken flag                                                        */
    if ( needExtraSetup )
    {
      /* StdVW is found in the private dictionary;                       */
      /* recompute darkening amounts whenever private dictionary or      */
      /* transform change                                                */
      /* Note: a rendering flag turns darkening on or off, so we want to */
      /*       store the `on' amounts;                                   */
      /*       darkening amount is computed in character space           */
      /* TODO: testing size-dependent darkening here;                    */
      /*       what to do for rotations?                                 */

      CF2_Fixed  emRatio;
      CF2_Fixed  stdHW;
      CF2_Int    unitsPerEm = font->unitsPerEm;


      if ( unitsPerEm == 0 )
        unitsPerEm = 1000;

      ppem = FT_MAX( cf2_intToFixed( 4 ),
                     font->ppem ); /* use minimum ppem of 4 */

#if 0
      /* since vstem is measured in the x-direction, we use the `a' member */
      /* of the fontMatrix                                                 */
      emRatio = cf2_fixedFracMul( cf2_intToFixed( 1000 ), fontMatrix->a );
#endif

      /* Freetype does not preserve the fontMatrix when parsing; use */
      /* unitsPerEm instead.                                         */
      /* TODO: check precision of this                               */
      emRatio     = cf2_intToFixed( 1000 ) / unitsPerEm;
      font->stdVW = cf2_getStdVW( decoder );

      if ( font->stdVW <= 0 )
        font->stdVW = FT_DivFix( cf2_intToFixed( 75 ), emRatio );

      if ( boldenX > 0 )
      {
        /* Ensure that boldenX is at least 1 pixel for synthetic bold font */
        /* (similar to what Avalon does)                                   */
        boldenX = FT_MAX( boldenX,
                          FT_DivFix( cf2_intToFixed( unitsPerEm ), ppem ) );

        /* Synthetic emboldening adds at least 1 pixel to darkenX, while */
        /* stem darkening adds at most half pixel.  Since the purpose of */
        /* stem darkening (readability at small sizes) is met with       */
        /* synthetic emboldening, no need to add stem darkening for a    */
        /* synthetic bold font.                                          */
        cf2_computeDarkening( emRatio,
                              ppem,
                              font->stdVW,
                              &font->darkenX,
                              boldenX,
                              FALSE,
                              font->darkenParams );
      }
      else
        cf2_computeDarkening( emRatio,
                              ppem,
                              font->stdVW,
                              &font->darkenX,
                              0,
                              font->stemDarkened,
                              font->darkenParams );

#if 0
      /* since hstem is measured in the y-direction, we use the `d' member */
      /* of the fontMatrix                                                 */
      /* TODO: use the same units per em as above; check this              */
      emRatio = cf2_fixedFracMul( cf2_intToFixed( 1000 ), fontMatrix->d );
#endif

      /* set the default stem width, because it must be the same for all */
      /* family members;                                                 */
      /* choose a constant for StdHW that depends on font contrast       */
      stdHW = cf2_getStdHW( decoder );

      if ( stdHW > 0 && font->stdVW > 2 * stdHW )
        font->stdHW = FT_DivFix( cf2_intToFixed( 75 ), emRatio );
      else
      {
        /* low contrast font gets less hstem darkening */
        font->stdHW = FT_DivFix( cf2_intToFixed( 110 ), emRatio );
      }

      cf2_computeDarkening( emRatio,
                            ppem,
                            font->stdHW,
                            &font->darkenY,
                            boldenY,
                            font->stemDarkened,
                            font->darkenParams );

      if ( font->darkenX != 0 || font->darkenY != 0 )
        font->darkened = TRUE;
      else
        font->darkened = FALSE;

      font->reverseWinding = FALSE; /* initial expectation is CCW */

      /* compute blue zones for this instance */
      cf2_blues_init( &font->blues, font );
    }
  }


  /* equivalent to AdobeGetOutline */
  FT_LOCAL_DEF( FT_Error )
  cf2_getGlyphOutline( CF2_Font           font,
                       CF2_Buffer         charstring,
                       const CF2_Matrix*  transform,
                       CF2_F16Dot16*      glyphWidth )
  {
    FT_Error  lastError = FT_Err_Ok;

    FT_Vector  translation;

#if 0
    FT_Vector  advancePoint;
#endif

    CF2_Fixed  advWidth = 0;
    FT_Bool    needWinding;


    /* Note: use both integer and fraction for outlines.  This allows bbox */
    /*       to come out directly.                                         */

    translation.x = transform->tx;
    translation.y = transform->ty;

    /* set up values based on transform */
    cf2_font_setup( font, transform );
    if ( font->error )
      goto exit;                      /* setup encountered an error */

    /* reset darken direction */
    font->reverseWinding = FALSE;

    /* winding order only affects darkening */
    needWinding = font->darkened;

    while ( 1 )
    {
      /* reset output buffer */
      cf2_outline_reset( &font->outline );

      /* build the outline, passing the full translation */
      cf2_interpT2CharString( font,
                              charstring,
                              (CF2_OutlineCallbacks)&font->outline,
                              &translation,
                              FALSE,
                              0,
                              0,
                              &advWidth );

      if ( font->error )
        goto exit;

      if ( !needWinding )
        break;

      /* check winding order */
      if ( font->outline.root.windingMomentum >= 0 ) /* CFF is CCW */
        break;

      /* invert darkening and render again                            */
      /* TODO: this should be a parameter to getOutline-computeOffset */
      font->reverseWinding = TRUE;

      needWinding = FALSE;    /* exit after next iteration */
    }

    /* finish storing client outline */
    cf2_outline_close( &font->outline );

  exit:
    /* FreeType just wants the advance width; there is no translation */
    *glyphWidth = advWidth;

    /* free resources and collect errors from objects we've used */
    cf2_setError( &font->error, lastError );

    return font->error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2ft.c                                                                */
/*                                                                         */
/*    FreeType Glue Component to Adobe's Interpreter (body).               */
/*                                                                         */
/*  Copyright 2013-2014 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2font.h"
#include "cf2error.h"


#define CF2_MAX_SIZE  cf2_intToFixed( 2000 )    /* max ppem */


  /*
   * This check should avoid most internal overflow cases.  Clients should
   * generally respond to `Glyph_Too_Big' by getting a glyph outline
   * at EM size, scaling it and filling it as a graphics operation.
   *
   */
  static FT_Error
  cf2_checkTransform( const CF2_Matrix*  transform,
                      CF2_Int            unitsPerEm )
  {
    CF2_Fixed  maxScale;


    FT_ASSERT( unitsPerEm > 0 );

    if ( transform->a <= 0 || transform->d <= 0 )
      return FT_THROW( Invalid_Size_Handle );

    FT_ASSERT( transform->b == 0 && transform->c == 0 );
    FT_ASSERT( transform->tx == 0 && transform->ty == 0 );

    if ( unitsPerEm > 0x7FFF )
      return FT_THROW( Glyph_Too_Big );

    maxScale = FT_DivFix( CF2_MAX_SIZE, cf2_intToFixed( unitsPerEm ) );

    if ( transform->a > maxScale || transform->d > maxScale )
      return FT_THROW( Glyph_Too_Big );

    return FT_Err_Ok;
  }


  static void
  cf2_setGlyphWidth( CF2_Outline  outline,
                     CF2_Fixed    width )
  {
    CFF_Decoder*  decoder = outline->decoder;


    FT_ASSERT( decoder );

    decoder->glyph_width = cf2_fixedToInt( width );
  }


  /* Clean up font instance. */
  static void
  cf2_free_instance( void*  ptr )
  {
    CF2_Font  font = (CF2_Font)ptr;


    if ( font )
    {
      FT_Memory  memory = font->memory;


      (void)memory;
    }
  }


  /********************************************/
  /*                                          */
  /* functions for handling client outline;   */
  /* FreeType uses coordinates in 26.6 format */
  /*                                          */
  /********************************************/

  static void
  cf2_builder_moveTo( CF2_OutlineCallbacks      callbacks,
                      const CF2_CallbackParams  params )
  {
    /* downcast the object pointer */
    CF2_Outline   outline = (CF2_Outline)callbacks;
    CFF_Builder*  builder;

    (void)params;        /* only used in debug mode */


    FT_ASSERT( outline && outline->decoder );
    FT_ASSERT( params->op == CF2_PathOpMoveTo );

    builder = &outline->decoder->builder;

    /* note: two successive moves simply close the contour twice */
    cff_builder_close_contour( builder );
    builder->path_begun = 0;
  }


  static void
  cf2_builder_lineTo( CF2_OutlineCallbacks      callbacks,
                      const CF2_CallbackParams  params )
  {
    /* downcast the object pointer */
    CF2_Outline   outline = (CF2_Outline)callbacks;
    CFF_Builder*  builder;


    FT_ASSERT( outline && outline->decoder );
    FT_ASSERT( params->op == CF2_PathOpLineTo );

    builder = &outline->decoder->builder;

    if ( !builder->path_begun )
    {
      /* record the move before the line; also check points and set */
      /* `path_begun'                                               */
      cff_builder_start_point( builder,
                               params->pt0.x,
                               params->pt0.y );
    }

    /* `cff_builder_add_point1' includes a check_points call for one point */
    cff_builder_add_point1( builder,
                            params->pt1.x,
                            params->pt1.y );
  }


  static void
  cf2_builder_cubeTo( CF2_OutlineCallbacks      callbacks,
                      const CF2_CallbackParams  params )
  {
    /* downcast the object pointer */
    CF2_Outline   outline = (CF2_Outline)callbacks;
    CFF_Builder*  builder;


    FT_ASSERT( outline && outline->decoder );
    FT_ASSERT( params->op == CF2_PathOpCubeTo );

    builder = &outline->decoder->builder;

    if ( !builder->path_begun )
    {
      /* record the move before the line; also check points and set */
      /* `path_begun'                                               */
      cff_builder_start_point( builder,
                               params->pt0.x,
                               params->pt0.y );
    }

    /* prepare room for 3 points: 2 off-curve, 1 on-curve */
    cff_check_points( builder, 3 );

    cff_builder_add_point( builder,
                           params->pt1.x,
                           params->pt1.y, 0 );
    cff_builder_add_point( builder,
                           params->pt2.x,
                           params->pt2.y, 0 );
    cff_builder_add_point( builder,
                           params->pt3.x,
                           params->pt3.y, 1 );
  }


  static void
  cf2_outline_init( CF2_Outline  outline,
                    FT_Memory    memory,
                    FT_Error*    error )
  {
    FT_MEM_ZERO( outline, sizeof ( CF2_OutlineRec ) );

    outline->root.memory = memory;
    outline->root.error  = error;

    outline->root.moveTo = cf2_builder_moveTo;
    outline->root.lineTo = cf2_builder_lineTo;
    outline->root.cubeTo = cf2_builder_cubeTo;
  }


  /* get scaling and hint flag from GlyphSlot */
  static void
  cf2_getScaleAndHintFlag( CFF_Decoder*  decoder,
                           CF2_Fixed*    x_scale,
                           CF2_Fixed*    y_scale,
                           FT_Bool*      hinted,
                           FT_Bool*      scaled )
  {
    FT_ASSERT( decoder && decoder->builder.glyph );

    /* note: FreeType scale includes a factor of 64 */
    *hinted = decoder->builder.glyph->hint;
    *scaled = decoder->builder.glyph->scaled;

    if ( *hinted )
    {
      *x_scale = ( decoder->builder.glyph->x_scale + 32 ) / 64;
      *y_scale = ( decoder->builder.glyph->y_scale + 32 ) / 64;
    }
    else
    {
      /* for unhinted outlines, `cff_slot_load' does the scaling, */
      /* thus render at `unity' scale                             */

      *x_scale = 0x0400;   /* 1/64 as 16.16 */
      *y_scale = 0x0400;
    }
  }


  /* get units per em from `FT_Face' */
  /* TODO: should handle font matrix concatenation? */
  static FT_UShort
  cf2_getUnitsPerEm( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->builder.face );
    FT_ASSERT( decoder->builder.face->root.units_per_EM );

    return decoder->builder.face->root.units_per_EM;
  }


  /* Main entry point: Render one glyph. */
  FT_LOCAL_DEF( FT_Error )
  cf2_decoder_parse_charstrings( CFF_Decoder*  decoder,
                                 FT_Byte*      charstring_base,
                                 FT_ULong      charstring_len )
  {
    FT_Memory  memory;
    FT_Error   error = FT_Err_Ok;
    CF2_Font   font;


    FT_ASSERT( decoder && decoder->cff );

    memory = decoder->builder.memory;

    /* CF2 data is saved here across glyphs */
    font = (CF2_Font)decoder->cff->cf2_instance.data;

    /* on first glyph, allocate instance structure */
    if ( decoder->cff->cf2_instance.data == NULL )
    {
      decoder->cff->cf2_instance.finalizer =
        (FT_Generic_Finalizer)cf2_free_instance;

      if ( FT_ALLOC( decoder->cff->cf2_instance.data,
                     sizeof ( CF2_FontRec ) ) )
        return FT_THROW( Out_Of_Memory );

      font = (CF2_Font)decoder->cff->cf2_instance.data;

      font->memory = memory;

      /* initialize a client outline, to be shared by each glyph rendered */
      cf2_outline_init( &font->outline, font->memory, &font->error );
    }

    /* save decoder; it is a stack variable and will be different on each */
    /* call                                                               */
    font->decoder         = decoder;
    font->outline.decoder = decoder;

    {
      /* build parameters for Adobe engine */

      CFF_Builder*  builder = &decoder->builder;
      CFF_Driver    driver  = (CFF_Driver)FT_FACE_DRIVER( builder->face );

      /* local error */
      FT_Error       error2 = FT_Err_Ok;
      CF2_BufferRec  buf;
      CF2_Matrix     transform;
      CF2_F16Dot16   glyphWidth;

      FT_Bool  hinted;
      FT_Bool  scaled;


      /* FreeType has already looked up the GID; convert to         */
      /* `RegionBuffer', assuming that the input has been validated */
      FT_ASSERT( charstring_base + charstring_len >= charstring_base );

      FT_ZERO( &buf );
      buf.start =
      buf.ptr   = charstring_base;
      buf.end   = charstring_base + charstring_len;

      FT_ZERO( &transform );

      cf2_getScaleAndHintFlag( decoder,
                               &transform.a,
                               &transform.d,
                               &hinted,
                               &scaled );

      font->renderingFlags = 0;
      if ( hinted )
        font->renderingFlags |= CF2_FlagsHinted;
      if ( scaled && !driver->no_stem_darkening )
        font->renderingFlags |= CF2_FlagsDarkened;

      font->darkenParams[0] = driver->darken_params[0];
      font->darkenParams[1] = driver->darken_params[1];
      font->darkenParams[2] = driver->darken_params[2];
      font->darkenParams[3] = driver->darken_params[3];
      font->darkenParams[4] = driver->darken_params[4];
      font->darkenParams[5] = driver->darken_params[5];
      font->darkenParams[6] = driver->darken_params[6];
      font->darkenParams[7] = driver->darken_params[7];

      /* now get an outline for this glyph;      */
      /* also get units per em to validate scale */
      font->unitsPerEm = (CF2_Int)cf2_getUnitsPerEm( decoder );

      if ( scaled )
      {
        error2 = cf2_checkTransform( &transform, font->unitsPerEm );
        if ( error2 )
          return error2;
      }

      error2 = cf2_getGlyphOutline( font, &buf, &transform, &glyphWidth );
      if ( error2 )
        return FT_ERR( Invalid_File_Format );

      cf2_setGlyphWidth( &font->outline, glyphWidth );

      return FT_Err_Ok;
    }
  }


  /* get pointer to current FreeType subfont (based on current glyphID) */
  FT_LOCAL_DEF( CFF_SubFont )
  cf2_getSubfont( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    return decoder->current_subfont;
  }


  /* get `y_ppem' from `CFF_Size' */
  FT_LOCAL_DEF( CF2_Fixed )
  cf2_getPpemY( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder                          &&
               decoder->builder.face            &&
               decoder->builder.face->root.size );

    /*
     * Note that `y_ppem' can be zero if there wasn't a call to
     * `FT_Set_Char_Size' or something similar.  However, this isn't a
     * problem since we come to this place in the code only if
     * FT_LOAD_NO_SCALE is set (the other case gets caught by
     * `cf2_checkTransform').  The ppem value is needed to compute the stem
     * darkening, which is disabled for getting the unscaled outline.
     *
     */
    return cf2_intToFixed(
             decoder->builder.face->root.size->metrics.y_ppem );
  }


  /* get standard stem widths for the current subfont; */
  /* FreeType stores these as integer font units       */
  /* (note: variable names seem swapped)               */
  FT_LOCAL_DEF( CF2_Fixed )
  cf2_getStdVW( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    return cf2_intToFixed(
             decoder->current_subfont->private_dict.standard_height );
  }


  FT_LOCAL_DEF( CF2_Fixed )
  cf2_getStdHW( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    return cf2_intToFixed(
             decoder->current_subfont->private_dict.standard_width );
  }


  /* note: FreeType stores 1000 times the actual value for `BlueScale' */
  FT_LOCAL_DEF( void )
  cf2_getBlueMetrics( CFF_Decoder*  decoder,
                      CF2_Fixed*    blueScale,
                      CF2_Fixed*    blueShift,
                      CF2_Fixed*    blueFuzz )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    *blueScale = FT_DivFix(
                   decoder->current_subfont->private_dict.blue_scale,
                   cf2_intToFixed( 1000 ) );
    *blueShift = cf2_intToFixed(
                   decoder->current_subfont->private_dict.blue_shift );
    *blueFuzz  = cf2_intToFixed(
                   decoder->current_subfont->private_dict.blue_fuzz );
  }


  /* get blue values counts and arrays; the FreeType parser has validated */
  /* the counts and verified that each is an even number                  */
  FT_LOCAL_DEF( void )
  cf2_getBlueValues( CFF_Decoder*  decoder,
                     size_t*       count,
                     FT_Pos*      *data )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    *count = decoder->current_subfont->private_dict.num_blue_values;
    *data  = (FT_Pos*)
               &decoder->current_subfont->private_dict.blue_values;
  }


  FT_LOCAL_DEF( void )
  cf2_getOtherBlues( CFF_Decoder*  decoder,
                     size_t*       count,
                     FT_Pos*      *data )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    *count = decoder->current_subfont->private_dict.num_other_blues;
    *data  = (FT_Pos*)
               &decoder->current_subfont->private_dict.other_blues;
  }


  FT_LOCAL_DEF( void )
  cf2_getFamilyBlues( CFF_Decoder*  decoder,
                      size_t*       count,
                      FT_Pos*      *data )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    *count = decoder->current_subfont->private_dict.num_family_blues;
    *data  = (FT_Pos*)
               &decoder->current_subfont->private_dict.family_blues;
  }


  FT_LOCAL_DEF( void )
  cf2_getFamilyOtherBlues( CFF_Decoder*  decoder,
                           size_t*       count,
                           FT_Pos*      *data )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    *count = decoder->current_subfont->private_dict.num_family_other_blues;
    *data  = (FT_Pos*)
               &decoder->current_subfont->private_dict.family_other_blues;
  }


  FT_LOCAL_DEF( CF2_Int )
  cf2_getLanguageGroup( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    return decoder->current_subfont->private_dict.language_group;
  }


  /* convert unbiased subroutine index to `CF2_Buffer' and */
  /* return 0 on success                                   */
  FT_LOCAL_DEF( CF2_Int )
  cf2_initGlobalRegionBuffer( CFF_Decoder*  decoder,
                              CF2_UInt      idx,
                              CF2_Buffer    buf )
  {
    FT_ASSERT( decoder );

    FT_ZERO( buf );

    idx += decoder->globals_bias;
    if ( idx >= decoder->num_globals )
      return TRUE;     /* error */

    FT_ASSERT( decoder->globals );

    buf->start =
    buf->ptr   = decoder->globals[idx];
    buf->end   = decoder->globals[idx + 1];

    return FALSE;      /* success */
  }


  /* convert AdobeStandardEncoding code to CF2_Buffer; */
  /* used for seac component                           */
  FT_LOCAL_DEF( FT_Error )
  cf2_getSeacComponent( CFF_Decoder*  decoder,
                        CF2_UInt      code,
                        CF2_Buffer    buf )
  {
    CF2_Int   gid;
    FT_Byte*  charstring;
    FT_ULong  len;
    FT_Error  error;


    FT_ASSERT( decoder );

    FT_ZERO( buf );

    gid = cff_lookup_glyph_by_stdcharcode( decoder->cff, code );
    if ( gid < 0 )
      return FT_THROW( Invalid_Glyph_Format );

    error = cff_get_glyph_data( decoder->builder.face,
                                gid,
                                &charstring,
                                &len );
    /* TODO: for now, just pass the FreeType error through */
    if ( error )
      return error;

    /* assume input has been validated */
    FT_ASSERT( charstring + len >= charstring );

    buf->start = charstring;
    buf->end   = charstring + len;
    buf->ptr   = buf->start;

    return FT_Err_Ok;
  }


  FT_LOCAL_DEF( void )
  cf2_freeSeacComponent( CFF_Decoder*  decoder,
                         CF2_Buffer    buf )
  {
    FT_ASSERT( decoder );

    cff_free_glyph_data( decoder->builder.face,
                         (FT_Byte**)&buf->start,
                         (FT_ULong)( buf->end - buf->start ) );
  }


  FT_LOCAL_DEF( CF2_Int )
  cf2_initLocalRegionBuffer( CFF_Decoder*  decoder,
                             CF2_UInt      idx,
                             CF2_Buffer    buf )
  {
    FT_ASSERT( decoder );

    FT_ZERO( buf );

    idx += decoder->locals_bias;
    if ( idx >= decoder->num_locals )
      return TRUE;     /* error */

    FT_ASSERT( decoder->locals );

    buf->start =
    buf->ptr   = decoder->locals[idx];
    buf->end   = decoder->locals[idx + 1];

    return FALSE;      /* success */
  }


  FT_LOCAL_DEF( CF2_Fixed )
  cf2_getDefaultWidthX( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    return cf2_intToFixed(
             decoder->current_subfont->private_dict.default_width );
  }


  FT_LOCAL_DEF( CF2_Fixed )
  cf2_getNominalWidthX( CFF_Decoder*  decoder )
  {
    FT_ASSERT( decoder && decoder->current_subfont );

    return cf2_intToFixed(
             decoder->current_subfont->private_dict.nominal_width );
  }


  FT_LOCAL_DEF( void )
  cf2_outline_reset( CF2_Outline  outline )
  {
    CFF_Decoder*  decoder = outline->decoder;


    FT_ASSERT( decoder );

    outline->root.windingMomentum = 0;

    FT_GlyphLoader_Rewind( decoder->builder.loader );
  }


  FT_LOCAL_DEF( void )
  cf2_outline_close( CF2_Outline  outline )
  {
    CFF_Decoder*  decoder = outline->decoder;


    FT_ASSERT( decoder );

    cff_builder_close_contour( &decoder->builder );

    FT_GlyphLoader_Add( decoder->builder.loader );
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2hints.c                                                             */
/*                                                                         */
/*    Adobe's code for handling CFF hints (body).                          */
/*                                                                         */
/*  Copyright 2007-2014 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2glue.h"
#include "cf2font.h"
#include "cf2hints.h"
#include "cf2intrp.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cf2hints


  typedef struct  CF2_HintMoveRec_
  {
    size_t     j;          /* index of upper hint map edge   */
    CF2_Fixed  moveUp;     /* adjustment to optimum position */

  } CF2_HintMoveRec, *CF2_HintMove;


  /* Compute angular momentum for winding order detection.  It is called */
  /* for all lines and curves, but not necessarily in element order.     */
  static CF2_Int
  cf2_getWindingMomentum( CF2_Fixed  x1,
                          CF2_Fixed  y1,
                          CF2_Fixed  x2,
                          CF2_Fixed  y2 )
  {
    /* cross product of pt1 position from origin with pt2 position from  */
    /* pt1; we reduce the precision so that the result fits into 32 bits */

    return ( x1 >> 16 ) * ( ( y2 - y1 ) >> 16 ) -
           ( y1 >> 16 ) * ( ( x2 - x1 ) >> 16 );
  }


  /*
   * Construct from a StemHint; this is used as a parameter to
   * `cf2_blues_capture'.
   * `hintOrigin' is the character space displacement of a seac accent.
   * Adjust stem hint for darkening here.
   *
   */
  static void
  cf2_hint_init( CF2_Hint            hint,
                 const CF2_ArrStack  stemHintArray,
                 size_t              indexStemHint,
                 const CF2_Font      font,
                 CF2_Fixed           hintOrigin,
                 CF2_Fixed           scale,
                 FT_Bool             bottom )
  {
    CF2_Fixed               width;
    const CF2_StemHintRec*  stemHint;


    FT_ZERO( hint );

    stemHint = (const CF2_StemHintRec*)cf2_arrstack_getPointer(
                                         stemHintArray,
                                         indexStemHint );

    width = stemHint->max - stemHint->min;

    if ( width == cf2_intToFixed( -21 ) )
    {
      /* ghost bottom */

      if ( bottom )
      {
        hint->csCoord = stemHint->max;
        hint->flags   = CF2_GhostBottom;
      }
      else
        hint->flags = 0;
    }

    else if ( width == cf2_intToFixed( -20 ) )
    {
      /* ghost top */

      if ( bottom )
        hint->flags = 0;
      else
      {
        hint->csCoord = stemHint->min;
        hint->flags   = CF2_GhostTop;
      }
    }

    else if ( width < 0 )
    {
      /* inverted pair */

      /*
       * Hints with negative widths were produced by an early version of a
       * non-Adobe font tool.  The Type 2 spec allows edge (ghost) hints
       * with negative widths, but says
       *
       *   All other negative widths have undefined meaning.
       *
       * CoolType has a silent workaround that negates the hint width; for
       * permissive mode, we do the same here.
       *
       * Note: Such fonts cannot use ghost hints, but should otherwise work.
       * Note: Some poor hints in our faux fonts can produce negative
       *       widths at some blends.  For example, see a light weight of
       *       `u' in ASerifMM.
       *
       */
      if ( bottom )
      {
        hint->csCoord = stemHint->max;
        hint->flags   = CF2_PairBottom;
      }
      else
      {
        hint->csCoord = stemHint->min;
        hint->flags   = CF2_PairTop;
      }
    }

    else
    {
      /* normal pair */

      if ( bottom )
      {
        hint->csCoord = stemHint->min;
        hint->flags   = CF2_PairBottom;
      }
      else
      {
        hint->csCoord = stemHint->max;
        hint->flags   = CF2_PairTop;
      }
    }

    /* Now that ghost hints have been detected, adjust this edge for      */
    /* darkening.  Bottoms are not changed; tops are incremented by twice */
    /* `darkenY'.                                                         */
    if ( cf2_hint_isTop( hint ) )
      hint->csCoord += 2 * font->darkenY;

    hint->csCoord += hintOrigin;
    hint->scale    = scale;
    hint->index    = indexStemHint;   /* index in original stem hint array */

    /* if original stem hint has been used, use the same position */
    if ( hint->flags != 0 && stemHint->used )
    {
      if ( cf2_hint_isTop( hint ) )
        hint->dsCoord = stemHint->maxDS;
      else
        hint->dsCoord = stemHint->minDS;

      cf2_hint_lock( hint );
    }
    else
      hint->dsCoord = FT_MulFix( hint->csCoord, scale );
  }


  /* initialize an invalid hint map element */
  static void
  cf2_hint_initZero( CF2_Hint  hint )
  {
    FT_ZERO( hint );
  }


  FT_LOCAL_DEF( FT_Bool )
  cf2_hint_isValid( const CF2_Hint  hint )
  {
    return (FT_Bool)( hint->flags != 0 );
  }


  static FT_Bool
  cf2_hint_isPair( const CF2_Hint  hint )
  {
    return (FT_Bool)( ( hint->flags                      &
                        ( CF2_PairBottom | CF2_PairTop ) ) != 0 );
  }


  static FT_Bool
  cf2_hint_isPairTop( const CF2_Hint  hint )
  {
    return (FT_Bool)( ( hint->flags & CF2_PairTop ) != 0 );
  }


  FT_LOCAL_DEF( FT_Bool )
  cf2_hint_isTop( const CF2_Hint  hint )
  {
    return (FT_Bool)( ( hint->flags                    &
                        ( CF2_PairTop | CF2_GhostTop ) ) != 0 );
  }


  FT_LOCAL_DEF( FT_Bool )
  cf2_hint_isBottom( const CF2_Hint  hint )
  {
    return (FT_Bool)( ( hint->flags                          &
                        ( CF2_PairBottom | CF2_GhostBottom ) ) != 0 );
  }


  static FT_Bool
  cf2_hint_isLocked( const CF2_Hint  hint )
  {
    return (FT_Bool)( ( hint->flags & CF2_Locked ) != 0 );
  }


  static FT_Bool
  cf2_hint_isSynthetic( const CF2_Hint  hint )
  {
    return (FT_Bool)( ( hint->flags & CF2_Synthetic ) != 0 );
  }


  FT_LOCAL_DEF( void )
  cf2_hint_lock( CF2_Hint  hint )
  {
    hint->flags |= CF2_Locked;
  }


  FT_LOCAL_DEF( void )
  cf2_hintmap_init( CF2_HintMap   hintmap,
                    CF2_Font      font,
                    CF2_HintMap   initialMap,
                    CF2_ArrStack  hintMoves,
                    CF2_Fixed     scale )
  {
    FT_ZERO( hintmap );

    /* copy parameters from font instance */
    hintmap->hinted         = font->hinted;
    hintmap->scale          = scale;
    hintmap->font           = font;
    hintmap->initialHintMap = initialMap;
    /* will clear in `cf2_hintmap_adjustHints' */
    hintmap->hintMoves      = hintMoves;
  }


  static FT_Bool
  cf2_hintmap_isValid( const CF2_HintMap  hintmap )
  {
    return hintmap->isValid;
  }


  /* transform character space coordinate to device space using hint map */
  static CF2_Fixed
  cf2_hintmap_map( CF2_HintMap  hintmap,
                   CF2_Fixed    csCoord )
  {
    FT_ASSERT( hintmap->isValid );  /* must call Build before Map */
    FT_ASSERT( hintmap->lastIndex < CF2_MAX_HINT_EDGES );

    if ( hintmap->count == 0 || ! hintmap->hinted )
    {
      /* there are no hints; use uniform scale and zero offset */
      return FT_MulFix( csCoord, hintmap->scale );
    }
    else
    {
      /* start linear search from last hit */
      CF2_UInt  i = hintmap->lastIndex;


      /* search up */
      while ( i < hintmap->count - 1                  &&
              csCoord >= hintmap->edge[i + 1].csCoord )
        i += 1;

      /* search down */
      while ( i > 0 && csCoord < hintmap->edge[i].csCoord )
        i -= 1;

      hintmap->lastIndex = i;

      if ( i == 0 && csCoord < hintmap->edge[0].csCoord )
      {
        /* special case for points below first edge: use uniform scale */
        return FT_MulFix( csCoord - hintmap->edge[0].csCoord,
                          hintmap->scale ) +
                 hintmap->edge[0].dsCoord;
      }
      else
      {
        /*
         * Note: entries with duplicate csCoord are allowed.
         * Use edge[i], the highest entry where csCoord >= entry[i].csCoord
         */
        return FT_MulFix( csCoord - hintmap->edge[i].csCoord,
                          hintmap->edge[i].scale ) +
                 hintmap->edge[i].dsCoord;
      }
    }
  }


  /*
   * This hinting policy moves a hint pair in device space so that one of
   * its two edges is on a device pixel boundary (its fractional part is
   * zero).  `cf2_hintmap_insertHint' guarantees no overlap in CS
   * space.  Ensure here that there is no overlap in DS.
   *
   * In the first pass, edges are adjusted relative to adjacent hints.
   * Those that are below have already been adjusted.  Those that are
   * above have not yet been adjusted.  If a hint above blocks an
   * adjustment to an optimal position, we will try again in a second
   * pass.  The second pass is top-down.
   *
   */

  static void
  cf2_hintmap_adjustHints( CF2_HintMap  hintmap )
  {
    size_t  i, j;


    cf2_arrstack_clear( hintmap->hintMoves );      /* working storage */

    /*
     * First pass is bottom-up (font hint order) without look-ahead.
     * Locked edges are already adjusted.
     * Unlocked edges begin with dsCoord from `initialHintMap'.
     * Save edges that are not optimally adjusted in `hintMoves' array,
     * and process them in second pass.
     */

    for ( i = 0; i < hintmap->count; i++ )
    {
      FT_Bool  isPair = cf2_hint_isPair( &hintmap->edge[i] );


      /* index of upper edge (same value for ghost hint) */
      j = isPair ? i + 1 : i;

      FT_ASSERT( j < hintmap->count );
      FT_ASSERT( cf2_hint_isValid( &hintmap->edge[i] ) );
      FT_ASSERT( cf2_hint_isValid( &hintmap->edge[j] ) );
      FT_ASSERT( cf2_hint_isLocked( &hintmap->edge[i] ) ==
                   cf2_hint_isLocked( &hintmap->edge[j] ) );

      if ( !cf2_hint_isLocked( &hintmap->edge[i] ) )
      {
        /* hint edge is not locked, we can adjust it */
        CF2_Fixed  fracDown = cf2_fixedFraction( hintmap->edge[i].dsCoord );
        CF2_Fixed  fracUp   = cf2_fixedFraction( hintmap->edge[j].dsCoord );

        /* calculate all four possibilities; moves down are negative */
        CF2_Fixed  downMoveDown = 0 - fracDown;
        CF2_Fixed  upMoveDown   = 0 - fracUp;
        CF2_Fixed  downMoveUp   = fracDown == 0
                                    ? 0
                                    : cf2_intToFixed( 1 ) - fracDown;
        CF2_Fixed  upMoveUp     = fracUp == 0
                                    ? 0
                                    : cf2_intToFixed( 1 ) - fracUp;

        /* smallest move up */
        CF2_Fixed  moveUp   = FT_MIN( downMoveUp, upMoveUp );
        /* smallest move down */
        CF2_Fixed  moveDown = FT_MAX( downMoveDown, upMoveDown );

        /* final amount to move edge or edge pair */
        CF2_Fixed  move;

        CF2_Fixed  downMinCounter = CF2_MIN_COUNTER;
        CF2_Fixed  upMinCounter   = CF2_MIN_COUNTER;
        FT_Bool    saveEdge       = FALSE;


        /* minimum counter constraint doesn't apply when adjacent edges */
        /* are synthetic                                                */
        /* TODO: doesn't seem a big effect; for now, reduce the code    */
#if 0
        if ( i == 0                                        ||
             cf2_hint_isSynthetic( &hintmap->edge[i - 1] ) )
          downMinCounter = 0;

        if ( j >= hintmap->count - 1                       ||
             cf2_hint_isSynthetic( &hintmap->edge[j + 1] ) )
          upMinCounter = 0;
#endif

        /* is there room to move up?                                    */
        /* there is if we are at top of array or the next edge is at or */
        /* beyond proposed move up?                                     */
        if ( j >= hintmap->count - 1                            ||
             hintmap->edge[j + 1].dsCoord >=
               hintmap->edge[j].dsCoord + moveUp + upMinCounter )
        {
          /* there is room to move up; is there also room to move down? */
          if ( i == 0                                                 ||
               hintmap->edge[i - 1].dsCoord <=
                 hintmap->edge[i].dsCoord + moveDown - downMinCounter )
          {
            /* move smaller absolute amount */
            move = ( -moveDown < moveUp ) ? moveDown : moveUp;  /* optimum */
          }
          else
            move = moveUp;
        }
        else
        {
          /* is there room to move down? */
          if ( i == 0                                                 ||
               hintmap->edge[i - 1].dsCoord <=
                 hintmap->edge[i].dsCoord + moveDown - downMinCounter )
          {
            move     = moveDown;
            /* true if non-optimum move */
            saveEdge = (FT_Bool)( moveUp < -moveDown );
          }
          else
          {
            /* no room to move either way without overlapping or reducing */
            /* the counter too much                                       */
            move     = 0;
            saveEdge = TRUE;
          }
        }

        /* Identify non-moves and moves down that aren't optimal, and save */
        /* them for second pass.                                           */
        /* Do this only if there is an unlocked edge above (which could    */
        /* possibly move).                                                 */
        if ( saveEdge                                    &&
             j < hintmap->count - 1                      &&
             !cf2_hint_isLocked( &hintmap->edge[j + 1] ) )
        {
          CF2_HintMoveRec  savedMove;


          savedMove.j      = j;
          /* desired adjustment in second pass */
          savedMove.moveUp = moveUp - move;

          cf2_arrstack_push( hintmap->hintMoves, &savedMove );
        }

        /* move the edge(s) */
        hintmap->edge[i].dsCoord += move;
        if ( isPair )
          hintmap->edge[j].dsCoord += move;
      }

      /* assert there are no overlaps in device space */
      FT_ASSERT( i == 0                                                   ||
                 hintmap->edge[i - 1].dsCoord <= hintmap->edge[i].dsCoord );
      FT_ASSERT( i < j                                                ||
                 hintmap->edge[i].dsCoord <= hintmap->edge[j].dsCoord );

      /* adjust the scales, avoiding divide by zero */
      if ( i > 0 )
      {
        if ( hintmap->edge[i].csCoord != hintmap->edge[i - 1].csCoord )
          hintmap->edge[i - 1].scale =
            FT_DivFix(
              hintmap->edge[i].dsCoord - hintmap->edge[i - 1].dsCoord,
              hintmap->edge[i].csCoord - hintmap->edge[i - 1].csCoord );
      }

      if ( isPair )
      {
        if ( hintmap->edge[j].csCoord != hintmap->edge[j - 1].csCoord )
          hintmap->edge[j - 1].scale =
            FT_DivFix(
              hintmap->edge[j].dsCoord - hintmap->edge[j - 1].dsCoord,
              hintmap->edge[j].csCoord - hintmap->edge[j - 1].csCoord );

        i += 1;     /* skip upper edge on next loop */
      }
    }

    /* second pass tries to move non-optimal hints up, in case there is */
    /* room now                                                         */
    for ( i = cf2_arrstack_size( hintmap->hintMoves ); i > 0; i-- )
    {
      CF2_HintMove  hintMove = (CF2_HintMove)
                      cf2_arrstack_getPointer( hintmap->hintMoves, i - 1 );


      j = hintMove->j;

      /* this was tested before the push, above */
      FT_ASSERT( j < hintmap->count - 1 );

      /* is there room to move up? */
      if ( hintmap->edge[j + 1].dsCoord >=
             hintmap->edge[j].dsCoord + hintMove->moveUp + CF2_MIN_COUNTER )
      {
        /* there is more room now, move edge up */
        hintmap->edge[j].dsCoord += hintMove->moveUp;

        if ( cf2_hint_isPair( &hintmap->edge[j] ) )
        {
          FT_ASSERT( j > 0 );
          hintmap->edge[j - 1].dsCoord += hintMove->moveUp;
        }
      }
    }
  }


  /* insert hint edges into map, sorted by csCoord */
  static void
  cf2_hintmap_insertHint( CF2_HintMap  hintmap,
                          CF2_Hint     bottomHintEdge,
                          CF2_Hint     topHintEdge )
  {
    CF2_UInt  indexInsert;

    /* set default values, then check for edge hints */
    FT_Bool   isPair         = TRUE;
    CF2_Hint  firstHintEdge  = bottomHintEdge;
    CF2_Hint  secondHintEdge = topHintEdge;


    /* one or none of the input params may be invalid when dealing with */
    /* edge hints; at least one edge must be valid                      */
    FT_ASSERT( cf2_hint_isValid( bottomHintEdge ) ||
               cf2_hint_isValid( topHintEdge )    );

    /* determine how many and which edges to insert */
    if ( !cf2_hint_isValid( bottomHintEdge ) )
    {
      /* insert only the top edge */
      firstHintEdge = topHintEdge;
      isPair        = FALSE;
    }
    else if ( !cf2_hint_isValid( topHintEdge ) )
    {
      /* insert only the bottom edge */
      isPair = FALSE;
    }

    /* paired edges must be in proper order */
    FT_ASSERT( !isPair                                         ||
               topHintEdge->csCoord >= bottomHintEdge->csCoord );

    /* linear search to find index value of insertion point */
    indexInsert = 0;
    for ( ; indexInsert < hintmap->count; indexInsert++ )
    {
      if ( hintmap->edge[indexInsert].csCoord >= firstHintEdge->csCoord )
        break;
    }

    /*
     * Discard any hints that overlap in character space.  Most often, this
     * is while building the initial map, where captured hints from all
     * zones are combined.  Define overlap to include hints that `touch'
     * (overlap zero).  Hiragino Sans/Gothic fonts have numerous hints that
     * touch.  Some fonts have non-ideographic glyphs that overlap our
     * synthetic hints.
     *
     * Overlap also occurs when darkening stem hints that are close.
     *
     */
    if ( indexInsert < hintmap->count )
    {
      /* we are inserting before an existing edge:    */
      /* verify that an existing edge is not the same */
      if ( hintmap->edge[indexInsert].csCoord == firstHintEdge->csCoord )
        return; /* ignore overlapping stem hint */

      /* verify that a new pair does not straddle the next edge */
      if ( isPair                                                        &&
           hintmap->edge[indexInsert].csCoord <= secondHintEdge->csCoord )
        return; /* ignore overlapping stem hint */

      /* verify that we are not inserting between paired edges */
      if ( cf2_hint_isPairTop( &hintmap->edge[indexInsert] ) )
        return; /* ignore overlapping stem hint */
    }

    /* recompute device space locations using initial hint map */
    if ( cf2_hintmap_isValid( hintmap->initialHintMap ) &&
         !cf2_hint_isLocked( firstHintEdge )            )
    {
      if ( isPair )
      {
        /* Use hint map to position the center of stem, and nominal scale */
        /* to position the two edges.  This preserves the stem width.     */
        CF2_Fixed  midpoint  = cf2_hintmap_map(
                                 hintmap->initialHintMap,
                                 ( secondHintEdge->csCoord +
                                   firstHintEdge->csCoord ) / 2 );
        CF2_Fixed  halfWidth = FT_MulFix(
                                 ( secondHintEdge->csCoord -
                                   firstHintEdge->csCoord ) / 2,
                                 hintmap->scale );


        firstHintEdge->dsCoord  = midpoint - halfWidth;
        secondHintEdge->dsCoord = midpoint + halfWidth;
      }
      else
        firstHintEdge->dsCoord = cf2_hintmap_map( hintmap->initialHintMap,
                                                  firstHintEdge->csCoord );
    }

    /*
     * Discard any hints that overlap in device space; this can occur
     * because locked hints have been moved to align with blue zones.
     *
     * TODO: Although we might correct this later during adjustment, we
     * don't currently have a way to delete a conflicting hint once it has
     * been inserted.  See v2.030 MinionPro-Regular, 12 ppem darkened,
     * initial hint map for second path, glyph 945 (the perispomeni (tilde)
     * in U+1F6E, Greek omega with psili and perispomeni).  Darkening is
     * 25.  Pair 667,747 initially conflicts in design space with top edge
     * 660.  This is because 667 maps to 7.87, and the top edge was
     * captured by a zone at 8.0.  The pair is later successfully inserted
     * in a zone without the top edge.  In this zone it is adjusted to 8.0,
     * and no longer conflicts with the top edge in design space.  This
     * means it can be included in yet a later zone which does have the top
     * edge hint.  This produces a small mismatch between the first and
     * last points of this path, even though the hint masks are the same.
     * The density map difference is tiny (1/256).
     *
     */

    if ( indexInsert > 0 )
    {
      /* we are inserting after an existing edge */
      if ( firstHintEdge->dsCoord < hintmap->edge[indexInsert - 1].dsCoord )
        return;
    }

    if ( indexInsert < hintmap->count )
    {
      /* we are inserting before an existing edge */
      if ( isPair )
      {
        if ( secondHintEdge->dsCoord > hintmap->edge[indexInsert].dsCoord )
          return;
      }
      else
      {
        if ( firstHintEdge->dsCoord > hintmap->edge[indexInsert].dsCoord )
          return;
      }
    }

    /* make room to insert */
    {
      CF2_Int  iSrc = hintmap->count - 1;
      CF2_Int  iDst = isPair ? hintmap->count + 1 : hintmap->count;

      CF2_Int  count = hintmap->count - indexInsert;


      if ( iDst >= CF2_MAX_HINT_EDGES )
      {
        FT_TRACE4(( "cf2_hintmap_insertHint: too many hintmaps\n" ));
        return;
      }

      while ( count-- )
        hintmap->edge[iDst--] = hintmap->edge[iSrc--];

      /* insert first edge */
      hintmap->edge[indexInsert] = *firstHintEdge;         /* copy struct */
      hintmap->count += 1;

      if ( isPair )
      {
        /* insert second edge */
        hintmap->edge[indexInsert + 1] = *secondHintEdge;  /* copy struct */
        hintmap->count                += 1;
      }
    }

    return;
  }


  /*
   * Build a map from hints and mask.
   *
   * This function may recur one level if `hintmap->initialHintMap' is not yet
   * valid.
   * If `initialMap' is true, simply build initial map.
   *
   * Synthetic hints are used in two ways.  A hint at zero is inserted, if
   * needed, in the initial hint map, to prevent translations from
   * propagating across the origin.  If synthetic em box hints are enabled
   * for ideographic dictionaries, then they are inserted in all hint
   * maps, including the initial one.
   *
   */
  FT_LOCAL_DEF( void )
  cf2_hintmap_build( CF2_HintMap   hintmap,
                     CF2_ArrStack  hStemHintArray,
                     CF2_ArrStack  vStemHintArray,
                     CF2_HintMask  hintMask,
                     CF2_Fixed     hintOrigin,
                     FT_Bool       initialMap )
  {
    FT_Byte*  maskPtr;

    CF2_Font         font = hintmap->font;
    CF2_HintMaskRec  tempHintMask;

    size_t   bitCount, i;
    FT_Byte  maskByte;


    /* check whether initial map is constructed */
    if ( !initialMap && !cf2_hintmap_isValid( hintmap->initialHintMap ) )
    {
      /* make recursive call with initialHintMap and temporary mask; */
      /* temporary mask will get all bits set, below */
      cf2_hintmask_init( &tempHintMask, hintMask->error );
      cf2_hintmap_build( hintmap->initialHintMap,
                         hStemHintArray,
                         vStemHintArray,
                         &tempHintMask,
                         hintOrigin,
                         TRUE );
    }

    if ( !cf2_hintmask_isValid( hintMask ) )
    {
      /* without a hint mask, assume all hints are active */
      cf2_hintmask_setAll( hintMask,
                           cf2_arrstack_size( hStemHintArray ) +
                             cf2_arrstack_size( vStemHintArray ) );
      if ( !cf2_hintmask_isValid( hintMask ) )
          return;                   /* too many stem hints */
    }

    /* begin by clearing the map */
    hintmap->count     = 0;
    hintmap->lastIndex = 0;

    /* make a copy of the hint mask so we can modify it */
    tempHintMask = *hintMask;
    maskPtr      = cf2_hintmask_getMaskPtr( &tempHintMask );

    /* use the hStem hints only, which are first in the mask */
    /* TODO: compare this to cffhintmaskGetBitCount */
    bitCount = cf2_arrstack_size( hStemHintArray );

    /* synthetic embox hints get highest priority */
    if ( font->blues.doEmBoxHints )
    {
      CF2_HintRec  dummy;


      cf2_hint_initZero( &dummy );   /* invalid hint map element */

      /* ghost bottom */
      cf2_hintmap_insertHint( hintmap,
                              &font->blues.emBoxBottomEdge,
                              &dummy );
      /* ghost top */
      cf2_hintmap_insertHint( hintmap,
                              &dummy,
                              &font->blues.emBoxTopEdge );
    }

    /* insert hints captured by a blue zone or already locked (higher */
    /* priority)                                                      */
    for ( i = 0, maskByte = 0x80; i < bitCount; i++ )
    {
      if ( maskByte & *maskPtr )
      {
        /* expand StemHint into two `CF2_Hint' elements */
        CF2_HintRec  bottomHintEdge, topHintEdge;


        cf2_hint_init( &bottomHintEdge,
                       hStemHintArray,
                       i,
                       font,
                       hintOrigin,
                       hintmap->scale,
                       TRUE /* bottom */ );
        cf2_hint_init( &topHintEdge,
                       hStemHintArray,
                       i,
                       font,
                       hintOrigin,
                       hintmap->scale,
                       FALSE /* top */ );

        if ( cf2_hint_isLocked( &bottomHintEdge ) ||
             cf2_hint_isLocked( &topHintEdge )    ||
             cf2_blues_capture( &font->blues,
                                &bottomHintEdge,
                                &topHintEdge )   )
        {
          /* insert captured hint into map */
          cf2_hintmap_insertHint( hintmap, &bottomHintEdge, &topHintEdge );

          *maskPtr &= ~maskByte;      /* turn off the bit for this hint */
        }
      }

      if ( ( i & 7 ) == 7 )
      {
        /* move to next mask byte */
        maskPtr++;
        maskByte = 0x80;
      }
      else
        maskByte >>= 1;
    }

    /* initial hint map includes only captured hints plus maybe one at 0 */

    /*
     * TODO: There is a problem here because we are trying to build a
     *       single hint map containing all captured hints.  It is
     *       possible for there to be conflicts between captured hints,
     *       either because of darkening or because the hints are in
     *       separate hint zones (we are ignoring hint zones for the
     *       initial map).  An example of the latter is MinionPro-Regular
     *       v2.030 glyph 883 (Greek Capital Alpha with Psili) at 15ppem.
     *       A stem hint for the psili conflicts with the top edge hint
     *       for the base character.  The stem hint gets priority because
     *       of its sort order.  In glyph 884 (Greek Capital Alpha with
     *       Psili and Oxia), the top of the base character gets a stem
     *       hint, and the psili does not.  This creates different initial
     *       maps for the two glyphs resulting in different renderings of
     *       the base character.  Will probably defer this either as not
     *       worth the cost or as a font bug.  I don't think there is any
     *       good reason for an accent to be captured by an alignment
     *       zone.  -darnold 2/12/10
     */

    if ( initialMap )
    {
      /* Apply a heuristic that inserts a point for (0,0), unless it's     */
      /* already covered by a mapping.  This locks the baseline for glyphs */
      /* that have no baseline hints.                                      */

      if ( hintmap->count == 0                           ||
           hintmap->edge[0].csCoord > 0                  ||
           hintmap->edge[hintmap->count - 1].csCoord < 0 )
      {
        /* all edges are above 0 or all edges are below 0; */
        /* construct a locked edge hint at 0               */

        CF2_HintRec  edge, invalid;


        cf2_hint_initZero( &edge );

        edge.flags = CF2_GhostBottom |
                     CF2_Locked      |
                     CF2_Synthetic;
        edge.scale = hintmap->scale;

        cf2_hint_initZero( &invalid );
        cf2_hintmap_insertHint( hintmap, &edge, &invalid );
      }
    }
    else
    {
      /* insert remaining hints */

      maskPtr = cf2_hintmask_getMaskPtr( &tempHintMask );

      for ( i = 0, maskByte = 0x80; i < bitCount; i++ )
      {
        if ( maskByte & *maskPtr )
        {
          CF2_HintRec  bottomHintEdge, topHintEdge;


          cf2_hint_init( &bottomHintEdge,
                         hStemHintArray,
                         i,
                         font,
                         hintOrigin,
                         hintmap->scale,
                         TRUE /* bottom */ );
          cf2_hint_init( &topHintEdge,
                         hStemHintArray,
                         i,
                         font,
                         hintOrigin,
                         hintmap->scale,
                         FALSE /* top */ );

          cf2_hintmap_insertHint( hintmap, &bottomHintEdge, &topHintEdge );
        }

        if ( ( i & 7 ) == 7 )
        {
          /* move to next mask byte */
          maskPtr++;
          maskByte = 0x80;
        }
        else
          maskByte >>= 1;
      }
    }

    /*
     * Note: The following line is a convenient place to break when
     *       debugging hinting.  Examine `hintmap->edge' for the list of
     *       enabled hints, then step over the call to see the effect of
     *       adjustment.  We stop here first on the recursive call that
     *       creates the initial map, and then on each counter group and
     *       hint zone.
     */

    /* adjust positions of hint edges that are not locked to blue zones */
    cf2_hintmap_adjustHints( hintmap );

    /* save the position of all hints that were used in this hint map; */
    /* if we use them again, we'll locate them in the same position    */
    if ( !initialMap )
    {
      for ( i = 0; i < hintmap->count; i++ )
      {
        if ( !cf2_hint_isSynthetic( &hintmap->edge[i] ) )
        {
          /* Note: include both valid and invalid edges            */
          /* Note: top and bottom edges are copied back separately */
          CF2_StemHint  stemhint = (CF2_StemHint)
                          cf2_arrstack_getPointer( hStemHintArray,
                                                   hintmap->edge[i].index );


          if ( cf2_hint_isTop( &hintmap->edge[i] ) )
            stemhint->maxDS = hintmap->edge[i].dsCoord;
          else
            stemhint->minDS = hintmap->edge[i].dsCoord;

          stemhint->used = TRUE;
        }
      }
    }

    /* hint map is ready to use */
    hintmap->isValid = TRUE;

    /* remember this mask has been used */
    cf2_hintmask_setNew( hintMask, FALSE );
  }


  FT_LOCAL_DEF( void )
  cf2_glyphpath_init( CF2_GlyphPath         glyphpath,
                      CF2_Font              font,
                      CF2_OutlineCallbacks  callbacks,
                      CF2_Fixed             scaleY,
                      /* CF2_Fixed  hShift, */
                      CF2_ArrStack          hStemHintArray,
                      CF2_ArrStack          vStemHintArray,
                      CF2_HintMask          hintMask,
                      CF2_Fixed             hintOriginY,
                      const CF2_Blues       blues,
                      const FT_Vector*      fractionalTranslation )
  {
    FT_ZERO( glyphpath );

    glyphpath->font      = font;
    glyphpath->callbacks = callbacks;

    cf2_arrstack_init( &glyphpath->hintMoves,
                       font->memory,
                       &font->error,
                       sizeof ( CF2_HintMoveRec ) );

    cf2_hintmap_init( &glyphpath->initialHintMap,
                      font,
                      &glyphpath->initialHintMap,
                      &glyphpath->hintMoves,
                      scaleY );
    cf2_hintmap_init( &glyphpath->firstHintMap,
                      font,
                      &glyphpath->initialHintMap,
                      &glyphpath->hintMoves,
                      scaleY );
    cf2_hintmap_init( &glyphpath->hintMap,
                      font,
                      &glyphpath->initialHintMap,
                      &glyphpath->hintMoves,
                      scaleY );

    glyphpath->scaleX = font->innerTransform.a;
    glyphpath->scaleC = font->innerTransform.c;
    glyphpath->scaleY = font->innerTransform.d;

    glyphpath->fractionalTranslation = *fractionalTranslation;

#if 0
    glyphpath->hShift = hShift;       /* for fauxing */
#endif

    glyphpath->hStemHintArray = hStemHintArray;
    glyphpath->vStemHintArray = vStemHintArray;
    glyphpath->hintMask       = hintMask;      /* ptr to current mask */
    glyphpath->hintOriginY    = hintOriginY;
    glyphpath->blues          = blues;
    glyphpath->darken         = font->darkened; /* TODO: should we make copies? */
    glyphpath->xOffset        = font->darkenX;
    glyphpath->yOffset        = font->darkenY;
    glyphpath->miterLimit     = 2 * FT_MAX(
                                     cf2_fixedAbs( glyphpath->xOffset ),
                                     cf2_fixedAbs( glyphpath->yOffset ) );

    /* .1 character space unit */
    glyphpath->snapThreshold = cf2_floatToFixed( 0.1f );

    glyphpath->moveIsPending = TRUE;
    glyphpath->pathIsOpen    = FALSE;
    glyphpath->pathIsClosing = FALSE;
    glyphpath->elemIsQueued  = FALSE;
  }


  FT_LOCAL_DEF( void )
  cf2_glyphpath_finalize( CF2_GlyphPath  glyphpath )
  {
    cf2_arrstack_finalize( &glyphpath->hintMoves );
  }


  /*
   * Hint point in y-direction and apply outerTransform.
   * Input `current' hint map (which is actually delayed by one element).
   * Input x,y point in Character Space.
   * Output x,y point in Device Space, including translation.
   */
  static void
  cf2_glyphpath_hintPoint( CF2_GlyphPath  glyphpath,
                           CF2_HintMap    hintmap,
                           FT_Vector*     ppt,
                           CF2_Fixed      x,
                           CF2_Fixed      y )
  {
    FT_Vector  pt;   /* hinted point in upright DS */


    pt.x = FT_MulFix( glyphpath->scaleX, x ) +
             FT_MulFix( glyphpath->scaleC, y );
    pt.y = cf2_hintmap_map( hintmap, y );

    ppt->x = FT_MulFix( glyphpath->font->outerTransform.a, pt.x )   +
               FT_MulFix( glyphpath->font->outerTransform.c, pt.y ) +
               glyphpath->fractionalTranslation.x;
    ppt->y = FT_MulFix( glyphpath->font->outerTransform.b, pt.x )   +
               FT_MulFix( glyphpath->font->outerTransform.d, pt.y ) +
               glyphpath->fractionalTranslation.y;
  }


  /*
   * From two line segments, (u1,u2) and (v1,v2), compute a point of
   * intersection on the corresponding lines.
   * Return false if no intersection is found, or if the intersection is
   * too far away from the ends of the line segments, u2 and v1.
   *
   */
  static FT_Bool
  cf2_glyphpath_computeIntersection( CF2_GlyphPath     glyphpath,
                                     const FT_Vector*  u1,
                                     const FT_Vector*  u2,
                                     const FT_Vector*  v1,
                                     const FT_Vector*  v2,
                                     FT_Vector*        intersection )
  {
    /*
     * Let `u' be a zero-based vector from the first segment, `v' from the
     * second segment.
     * Let `w 'be the zero-based vector from `u1' to `v1'.
     * `perp' is the `perpendicular dot product'; see
     * http://mathworld.wolfram.com/PerpDotProduct.html.
     * `s' is the parameter for the parametric line for the first segment
     * (`u').
     *
     * See notation in
     * http://softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm.
     * Calculations are done in 16.16, but must handle the squaring of
     * line lengths in character space.  We scale all vectors by 1/32 to
     * avoid overflow.  This allows values up to 4095 to be squared.  The
     * scale factor cancels in the divide.
     *
     * TODO: the scale factor could be computed from UnitsPerEm.
     *
     */

#define cf2_perp( a, b )                                    \
          ( FT_MulFix( a.x, b.y ) - FT_MulFix( a.y, b.x ) )

  /* round and divide by 32 */
#define CF2_CS_SCALE( x )         \
          ( ( (x) + 0x10 ) >> 5 )

    FT_Vector  u, v, w;      /* scaled vectors */
    CF2_Fixed  denominator, s;


    u.x = CF2_CS_SCALE( u2->x - u1->x );
    u.y = CF2_CS_SCALE( u2->y - u1->y );
    v.x = CF2_CS_SCALE( v2->x - v1->x );
    v.y = CF2_CS_SCALE( v2->y - v1->y );
    w.x = CF2_CS_SCALE( v1->x - u1->x );
    w.y = CF2_CS_SCALE( v1->y - u1->y );

    denominator = cf2_perp( u, v );

    if ( denominator == 0 )
      return FALSE;           /* parallel or coincident lines */

    s = FT_DivFix( cf2_perp( w, v ), denominator );

    intersection->x = u1->x + FT_MulFix( s, u2->x - u1->x );
    intersection->y = u1->y + FT_MulFix( s, u2->y - u1->y );

    /*
     * Special case snapping for horizontal and vertical lines.
     * This cleans up intersections and reduces problems with winding
     * order detection.
     * Sample case is sbc cd KozGoPr6N-Medium.otf 20 16685.
     * Note: these calculations are in character space.
     *
     */

    if ( u1->x == u2->x                                                     &&
         cf2_fixedAbs( intersection->x - u1->x ) < glyphpath->snapThreshold )
      intersection->x = u1->x;
    if ( u1->y == u2->y                                                     &&
         cf2_fixedAbs( intersection->y - u1->y ) < glyphpath->snapThreshold )
      intersection->y = u1->y;

    if ( v1->x == v2->x                                                     &&
         cf2_fixedAbs( intersection->x - v1->x ) < glyphpath->snapThreshold )
      intersection->x = v1->x;
    if ( v1->y == v2->y                                                     &&
         cf2_fixedAbs( intersection->y - v1->y ) < glyphpath->snapThreshold )
      intersection->y = v1->y;

    /* limit the intersection distance from midpoint of u2 and v1 */
    if ( cf2_fixedAbs( intersection->x - ( u2->x + v1->x ) / 2 ) >
           glyphpath->miterLimit                                   ||
         cf2_fixedAbs( intersection->y - ( u2->y + v1->y ) / 2 ) >
           glyphpath->miterLimit                                   )
      return FALSE;

    return TRUE;
  }


  /*
   * Push the cached element (glyphpath->prevElem*) to the outline
   * consumer.  When a darkening offset is used, the end point of the
   * cached element may be adjusted to an intersection point or we may
   * synthesize a connecting line to the current element.  If we are
   * closing a subpath, we may also generate a connecting line to the start
   * point.
   *
   * This is where Character Space (CS) is converted to Device Space (DS)
   * using a hint map.  This calculation must use a HintMap that was valid
   * at the time the element was saved.  For the first point in a subpath,
   * that is a saved HintMap.  For most elements, it just means the caller
   * has delayed building a HintMap from the current HintMask.
   *
   * Transform each point with outerTransform and call the outline
   * callbacks.  This is a general 3x3 transform:
   *
   *   x' = a*x + c*y + tx, y' = b*x + d*y + ty
   *
   * but it uses 4 elements from CF2_Font and the translation part
   * from CF2_GlyphPath.
   *
   */
  static void
  cf2_glyphpath_pushPrevElem( CF2_GlyphPath  glyphpath,
                              CF2_HintMap    hintmap,
                              FT_Vector*     nextP0,
                              FT_Vector      nextP1,
                              FT_Bool        close )
  {
    CF2_CallbackParamsRec  params;

    FT_Vector*  prevP0;
    FT_Vector*  prevP1;

    FT_Vector  intersection    = { 0, 0 };
    FT_Bool    useIntersection = FALSE;


    FT_ASSERT( glyphpath->prevElemOp == CF2_PathOpLineTo ||
               glyphpath->prevElemOp == CF2_PathOpCubeTo );

    if ( glyphpath->prevElemOp == CF2_PathOpLineTo )
    {
      prevP0 = &glyphpath->prevElemP0;
      prevP1 = &glyphpath->prevElemP1;
    }
    else
    {
      prevP0 = &glyphpath->prevElemP2;
      prevP1 = &glyphpath->prevElemP3;
    }

    /* optimization: if previous and next elements are offset by the same */
    /* amount, then there will be no gap, and no need to compute an       */
    /* intersection.                                                      */
    if ( prevP1->x != nextP0->x || prevP1->y != nextP0->y )
    {
      /* previous element does not join next element:             */
      /* adjust end point of previous element to the intersection */
      useIntersection = cf2_glyphpath_computeIntersection( glyphpath,
                                                           prevP0,
                                                           prevP1,
                                                           nextP0,
                                                           &nextP1,
                                                           &intersection );
      if ( useIntersection )
      {
        /* modify the last point of the cached element (either line or */
        /* curve)                                                      */
        *prevP1 = intersection;
      }
    }

    params.pt0 = glyphpath->currentDS;

    switch( glyphpath->prevElemOp )
    {
    case CF2_PathOpLineTo:
      params.op = CF2_PathOpLineTo;

      /* note: pt2 and pt3 are unused */

      if ( close )
      {
        /* use first hint map if closing */
        cf2_glyphpath_hintPoint( glyphpath,
                                 &glyphpath->firstHintMap,
                                 &params.pt1,
                                 glyphpath->prevElemP1.x,
                                 glyphpath->prevElemP1.y );
      }
      else
      {
        cf2_glyphpath_hintPoint( glyphpath,
                                 hintmap,
                                 &params.pt1,
                                 glyphpath->prevElemP1.x,
                                 glyphpath->prevElemP1.y );
      }

      /* output only non-zero length lines */
      if ( params.pt0.x != params.pt1.x || params.pt0.y != params.pt1.y )
      {
        glyphpath->callbacks->lineTo( glyphpath->callbacks, &params );

        glyphpath->currentDS = params.pt1;
      }
      break;

    case CF2_PathOpCubeTo:
      params.op = CF2_PathOpCubeTo;

      /* TODO: should we intersect the interior joins (p1-p2 and p2-p3)? */
      cf2_glyphpath_hintPoint( glyphpath,
                               hintmap,
                               &params.pt1,
                               glyphpath->prevElemP1.x,
                               glyphpath->prevElemP1.y );
      cf2_glyphpath_hintPoint( glyphpath,
                               hintmap,
                               &params.pt2,
                               glyphpath->prevElemP2.x,
                               glyphpath->prevElemP2.y );
      cf2_glyphpath_hintPoint( glyphpath,
                               hintmap,
                               &params.pt3,
                               glyphpath->prevElemP3.x,
                               glyphpath->prevElemP3.y );

      glyphpath->callbacks->cubeTo( glyphpath->callbacks, &params );

      glyphpath->currentDS = params.pt3;

      break;
    }

    if ( !useIntersection || close )
    {
      /* insert connecting line between end of previous element and start */
      /* of current one                                                   */
      /* note: at the end of a subpath, we might do both, so use `nextP0' */
      /* before we change it, below                                       */

      if ( close )
      {
        /* if we are closing the subpath, then nextP0 is in the first     */
        /* hint zone                                                      */
        cf2_glyphpath_hintPoint( glyphpath,
                                 &glyphpath->firstHintMap,
                                 &params.pt1,
                                 nextP0->x,
                                 nextP0->y );
      }
      else
      {
        cf2_glyphpath_hintPoint( glyphpath,
                                 hintmap,
                                 &params.pt1,
                                 nextP0->x,
                                 nextP0->y );
      }

      if ( params.pt1.x != glyphpath->currentDS.x ||
           params.pt1.y != glyphpath->currentDS.y )
      {
        /* length is nonzero */
        params.op  = CF2_PathOpLineTo;
        params.pt0 = glyphpath->currentDS;

        /* note: pt2 and pt3 are unused */
        glyphpath->callbacks->lineTo( glyphpath->callbacks, &params );

        glyphpath->currentDS = params.pt1;
      }
    }

    if ( useIntersection )
    {
      /* return intersection point to caller */
      *nextP0 = intersection;
    }
  }


  /* push a MoveTo element based on current point and offset of current */
  /* element                                                            */
  static void
  cf2_glyphpath_pushMove( CF2_GlyphPath  glyphpath,
                          FT_Vector      start )
  {
    CF2_CallbackParamsRec  params;


    params.op  = CF2_PathOpMoveTo;
    params.pt0 = glyphpath->currentDS;

    /* Test if move has really happened yet; it would have called */
    /* `cf2_hintmap_build' to set `isValid'.                   */
    if ( !cf2_hintmap_isValid( &glyphpath->hintMap ) )
    {
      /* we are here iff first subpath is missing a moveto operator: */
      /* synthesize first moveTo to finish initialization of hintMap */
      cf2_glyphpath_moveTo( glyphpath,
                            glyphpath->start.x,
                            glyphpath->start.y );
    }

    cf2_glyphpath_hintPoint( glyphpath,
                             &glyphpath->hintMap,
                             &params.pt1,
                             start.x,
                             start.y );

    /* note: pt2 and pt3 are unused */
    glyphpath->callbacks->moveTo( glyphpath->callbacks, &params );

    glyphpath->currentDS    = params.pt1;
    glyphpath->offsetStart0 = start;
  }


  /*
   * All coordinates are in character space.
   * On input, (x1, y1) and (x2, y2) give line segment.
   * On output, (x, y) give offset vector.
   * We use a piecewise approximation to trig functions.
   *
   * TODO: Offset true perpendicular and proper length
   *       supply the y-translation for hinting here, too,
   *       that adds yOffset unconditionally to *y.
   */
  static void
  cf2_glyphpath_computeOffset( CF2_GlyphPath  glyphpath,
                               CF2_Fixed      x1,
                               CF2_Fixed      y1,
                               CF2_Fixed      x2,
                               CF2_Fixed      y2,
                               CF2_Fixed*     x,
                               CF2_Fixed*     y )
  {
    CF2_Fixed  dx = x2 - x1;
    CF2_Fixed  dy = y2 - y1;


    /* note: negative offsets don't work here; negate deltas to change */
    /* quadrants, below                                                */
    if ( glyphpath->font->reverseWinding )
    {
      dx = -dx;
      dy = -dy;
    }

    *x = *y = 0;

    if ( !glyphpath->darken )
        return;

    /* add momentum for this path element */
    glyphpath->callbacks->windingMomentum +=
      cf2_getWindingMomentum( x1, y1, x2, y2 );

    /* note: allow mixed integer and fixed multiplication here */
    if ( dx >= 0 )
    {
      if ( dy >= 0 )
      {
        /* first quadrant, +x +y */

        if ( dx > 2 * dy )
        {
          /* +x */
          *x = 0;
          *y = 0;
        }
        else if ( dy > 2 * dx )
        {
          /* +y */
          *x = glyphpath->xOffset;
          *y = glyphpath->yOffset;
        }
        else
        {
          /* +x +y */
          *x = FT_MulFix( cf2_floatToFixed( 0.7 ),
                          glyphpath->xOffset );
          *y = FT_MulFix( cf2_floatToFixed( 1.0 - 0.7 ),
                          glyphpath->yOffset );
        }
      }
      else
      {
        /* fourth quadrant, +x -y */

        if ( dx > -2 * dy )
        {
          /* +x */
          *x = 0;
          *y = 0;
        }
        else if ( -dy > 2 * dx )
        {
          /* -y */
          *x = -glyphpath->xOffset;
          *y = glyphpath->yOffset;
        }
        else
        {
          /* +x -y */
          *x = FT_MulFix( cf2_floatToFixed( -0.7 ),
                          glyphpath->xOffset );
          *y = FT_MulFix( cf2_floatToFixed( 1.0 - 0.7 ),
                          glyphpath->yOffset );
        }
      }
    }
    else
    {
      if ( dy >= 0 )
      {
        /* second quadrant, -x +y */

        if ( -dx > 2 * dy )
        {
          /* -x */
          *x = 0;
          *y = 2 * glyphpath->yOffset;
        }
        else if ( dy > -2 * dx )
        {
          /* +y */
          *x = glyphpath->xOffset;
          *y = glyphpath->yOffset;
        }
        else
        {
          /* -x +y */
          *x = FT_MulFix( cf2_floatToFixed( 0.7 ),
                          glyphpath->xOffset );
          *y = FT_MulFix( cf2_floatToFixed( 1.0 + 0.7 ),
                          glyphpath->yOffset );
        }
      }
      else
      {
        /* third quadrant, -x -y */

        if ( -dx > -2 * dy )
        {
          /* -x */
          *x = 0;
          *y = 2 * glyphpath->yOffset;
        }
        else if ( -dy > -2 * dx )
        {
          /* -y */
          *x = -glyphpath->xOffset;
          *y = glyphpath->xOffset;
        }
        else
        {
          /* -x -y */
          *x = FT_MulFix( cf2_floatToFixed( -0.7 ),
                          glyphpath->xOffset );
          *y = FT_MulFix( cf2_floatToFixed( 1.0 + 0.7 ),
                          glyphpath->yOffset );
        }
      }
    }
  }


  /*
   * The functions cf2_glyphpath_{moveTo,lineTo,curveTo,closeOpenPath} are
   * called by the interpreter with Character Space (CS) coordinates.  Each
   * path element is placed into a queue of length one to await the
   * calculation of the following element.  At that time, the darkening
   * offset of the following element is known and joins can be computed,
   * including possible modification of this element, before mapping to
   * Device Space (DS) and passing it on to the outline consumer.
   *
   */
  FT_LOCAL_DEF( void )
  cf2_glyphpath_moveTo( CF2_GlyphPath  glyphpath,
                        CF2_Fixed      x,
                        CF2_Fixed      y )
  {
    cf2_glyphpath_closeOpenPath( glyphpath );

    /* save the parameters of the move for later, when we'll know how to */
    /* offset it;                                                        */
    /* also save last move point */
    glyphpath->currentCS.x = glyphpath->start.x = x;
    glyphpath->currentCS.y = glyphpath->start.y = y;

    glyphpath->moveIsPending = TRUE;

    /* ensure we have a valid map with current mask */
    if ( !cf2_hintmap_isValid( &glyphpath->hintMap ) ||
         cf2_hintmask_isNew( glyphpath->hintMask )   )
      cf2_hintmap_build( &glyphpath->hintMap,
                         glyphpath->hStemHintArray,
                         glyphpath->vStemHintArray,
                         glyphpath->hintMask,
                         glyphpath->hintOriginY,
                         FALSE );

    /* save a copy of current HintMap to use when drawing initial point */
    glyphpath->firstHintMap = glyphpath->hintMap;     /* structure copy */
  }


  FT_LOCAL_DEF( void )
  cf2_glyphpath_lineTo( CF2_GlyphPath  glyphpath,
                        CF2_Fixed      x,
                        CF2_Fixed      y )
  {
    CF2_Fixed  xOffset, yOffset;
    FT_Vector  P0, P1;
    FT_Bool    newHintMap;

    /*
     * New hints will be applied after cf2_glyphpath_pushPrevElem has run.
     * In case this is a synthesized closing line, any new hints should be
     * delayed until this path is closed (`cf2_hintmask_isNew' will be
     * called again before the next line or curve).
     */

    /* true if new hint map not on close */
    newHintMap = cf2_hintmask_isNew( glyphpath->hintMask ) &&
                 !glyphpath->pathIsClosing;

    /*
     * Zero-length lines may occur in the charstring.  Because we cannot
     * compute darkening offsets or intersections from zero-length lines,
     * it is best to remove them and avoid artifacts.  However, zero-length
     * lines in CS at the start of a new hint map can generate non-zero
     * lines in DS due to hint substitution.  We detect a change in hint
     * map here and pass those zero-length lines along.
     */

    /*
     * Note: Find explicitly closed paths here with a conditional
     *       breakpoint using
     *
     *         !gp->pathIsClosing && gp->start.x == x && gp->start.y == y
     *
     */

    if ( glyphpath->currentCS.x == x &&
         glyphpath->currentCS.y == y &&
         !newHintMap                 )
      /*
       * Ignore zero-length lines in CS where the hint map is the same
       * because the line in DS will also be zero length.
       *
       * Ignore zero-length lines when we synthesize a closing line because
       * the close will be handled in cf2_glyphPath_pushPrevElem.
       */
      return;

    cf2_glyphpath_computeOffset( glyphpath,
                                 glyphpath->currentCS.x,
                                 glyphpath->currentCS.y,
                                 x,
                                 y,
                                 &xOffset,
                                 &yOffset );

    /* construct offset points */
    P0.x = glyphpath->currentCS.x + xOffset;
    P0.y = glyphpath->currentCS.y + yOffset;
    P1.x = x + xOffset;
    P1.y = y + yOffset;

    if ( glyphpath->moveIsPending )
    {
      /* emit offset 1st point as MoveTo */
      cf2_glyphpath_pushMove( glyphpath, P0 );

      glyphpath->moveIsPending = FALSE;  /* adjust state machine */
      glyphpath->pathIsOpen    = TRUE;

      glyphpath->offsetStart1 = P1;              /* record second point */
    }

    if ( glyphpath->elemIsQueued )
    {
      FT_ASSERT( cf2_hintmap_isValid( &glyphpath->hintMap ) );

      cf2_glyphpath_pushPrevElem( glyphpath,
                                  &glyphpath->hintMap,
                                  &P0,
                                  P1,
                                  FALSE );
    }

    /* queue the current element with offset points */
    glyphpath->elemIsQueued = TRUE;
    glyphpath->prevElemOp   = CF2_PathOpLineTo;
    glyphpath->prevElemP0   = P0;
    glyphpath->prevElemP1   = P1;

    /* update current map */
    if ( newHintMap )
      cf2_hintmap_build( &glyphpath->hintMap,
                         glyphpath->hStemHintArray,
                         glyphpath->vStemHintArray,
                         glyphpath->hintMask,
                         glyphpath->hintOriginY,
                         FALSE );

    glyphpath->currentCS.x = x;     /* pre-offset current point */
    glyphpath->currentCS.y = y;
  }


  FT_LOCAL_DEF( void )
  cf2_glyphpath_curveTo( CF2_GlyphPath  glyphpath,
                         CF2_Fixed      x1,
                         CF2_Fixed      y1,
                         CF2_Fixed      x2,
                         CF2_Fixed      y2,
                         CF2_Fixed      x3,
                         CF2_Fixed      y3 )
  {
    CF2_Fixed  xOffset1, yOffset1, xOffset3, yOffset3;
    FT_Vector  P0, P1, P2, P3;


    /* TODO: ignore zero length portions of curve?? */
    cf2_glyphpath_computeOffset( glyphpath,
                                 glyphpath->currentCS.x,
                                 glyphpath->currentCS.y,
                                 x1,
                                 y1,
                                 &xOffset1,
                                 &yOffset1 );
    cf2_glyphpath_computeOffset( glyphpath,
                                 x2,
                                 y2,
                                 x3,
                                 y3,
                                 &xOffset3,
                                 &yOffset3 );

    /* add momentum from the middle segment */
    glyphpath->callbacks->windingMomentum +=
      cf2_getWindingMomentum( x1, y1, x2, y2 );

    /* construct offset points */
    P0.x = glyphpath->currentCS.x + xOffset1;
    P0.y = glyphpath->currentCS.y + yOffset1;
    P1.x = x1 + xOffset1;
    P1.y = y1 + yOffset1;
    /* note: preserve angle of final segment by using offset3 at both ends */
    P2.x = x2 + xOffset3;
    P2.y = y2 + yOffset3;
    P3.x = x3 + xOffset3;
    P3.y = y3 + yOffset3;

    if ( glyphpath->moveIsPending )
    {
      /* emit offset 1st point as MoveTo */
      cf2_glyphpath_pushMove( glyphpath, P0 );

      glyphpath->moveIsPending = FALSE;
      glyphpath->pathIsOpen    = TRUE;

      glyphpath->offsetStart1 = P1;              /* record second point */
    }

    if ( glyphpath->elemIsQueued )
    {
      FT_ASSERT( cf2_hintmap_isValid( &glyphpath->hintMap ) );

      cf2_glyphpath_pushPrevElem( glyphpath,
                                  &glyphpath->hintMap,
                                  &P0,
                                  P1,
                                  FALSE );
    }

    /* queue the current element with offset points */
    glyphpath->elemIsQueued = TRUE;
    glyphpath->prevElemOp   = CF2_PathOpCubeTo;
    glyphpath->prevElemP0   = P0;
    glyphpath->prevElemP1   = P1;
    glyphpath->prevElemP2   = P2;
    glyphpath->prevElemP3   = P3;

    /* update current map */
    if ( cf2_hintmask_isNew( glyphpath->hintMask ) )
      cf2_hintmap_build( &glyphpath->hintMap,
                         glyphpath->hStemHintArray,
                         glyphpath->vStemHintArray,
                         glyphpath->hintMask,
                         glyphpath->hintOriginY,
                         FALSE );

    glyphpath->currentCS.x = x3;       /* pre-offset current point */
    glyphpath->currentCS.y = y3;
  }


  FT_LOCAL_DEF( void )
  cf2_glyphpath_closeOpenPath( CF2_GlyphPath  glyphpath )
  {
    if ( glyphpath->pathIsOpen )
    {
      /*
       * A closing line in Character Space line is always generated below
       * with `cf2_glyphPath_lineTo'.  It may be ignored later if it turns
       * out to be zero length in Device Space.
       */
      glyphpath->pathIsClosing = TRUE;

      cf2_glyphpath_lineTo( glyphpath,
                            glyphpath->start.x,
                            glyphpath->start.y );

      /* empty the final element from the queue and close the path */
      if ( glyphpath->elemIsQueued )
        cf2_glyphpath_pushPrevElem( glyphpath,
                                    &glyphpath->hintMap,
                                    &glyphpath->offsetStart0,
                                    glyphpath->offsetStart1,
                                    TRUE );

      /* reset state machine */
      glyphpath->moveIsPending = TRUE;
      glyphpath->pathIsOpen    = FALSE;
      glyphpath->pathIsClosing = FALSE;
      glyphpath->elemIsQueued  = FALSE;
    }
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2intrp.c                                                             */
/*                                                                         */
/*    Adobe's CFF Interpreter (body).                                      */
/*                                                                         */
/*  Copyright 2007-2013 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2glue.h"
#include "cf2font.h"
#include "cf2stack.h"
#include "cf2hints.h"

#include "cf2error.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cf2interp


  /* some operators are not implemented yet */
#define CF2_FIXME  FT_TRACE4(( "cf2_interpT2CharString:"            \
                               " operator not implemented yet\n" ))



  FT_LOCAL_DEF( void )
  cf2_hintmask_init( CF2_HintMask  hintmask,
                     FT_Error*     error )
  {
    FT_ZERO( hintmask );

    hintmask->error = error;
  }


  FT_LOCAL_DEF( FT_Bool )
  cf2_hintmask_isValid( const CF2_HintMask  hintmask )
  {
    return hintmask->isValid;
  }


  FT_LOCAL_DEF( FT_Bool )
  cf2_hintmask_isNew( const CF2_HintMask  hintmask )
  {
    return hintmask->isNew;
  }


  FT_LOCAL_DEF( void )
  cf2_hintmask_setNew( CF2_HintMask  hintmask,
                       FT_Bool       val )
  {
    hintmask->isNew = val;
  }


  /* clients call `getMaskPtr' in order to iterate */
  /* through hint mask                             */

  FT_LOCAL_DEF( FT_Byte* )
  cf2_hintmask_getMaskPtr( CF2_HintMask  hintmask )
  {
    return hintmask->mask;
  }


  static size_t
  cf2_hintmask_setCounts( CF2_HintMask  hintmask,
                          size_t        bitCount )
  {
    if ( bitCount > CF2_MAX_HINTS )
    {
      /* total of h and v stems must be <= 96 */
      CF2_SET_ERROR( hintmask->error, Invalid_Glyph_Format );
      return 0;
    }

    hintmask->bitCount  = bitCount;
    hintmask->byteCount = ( hintmask->bitCount + 7 ) / 8;

    hintmask->isValid = TRUE;
    hintmask->isNew   = TRUE;

    return bitCount;
  }


  /* consume the hintmask bytes from the charstring, advancing the src */
  /* pointer                                                           */
  static void
  cf2_hintmask_read( CF2_HintMask  hintmask,
                     CF2_Buffer    charstring,
                     size_t        bitCount )
  {
    size_t  i;

#ifndef CF2_NDEBUG
    /* these are the bits in the final mask byte that should be zero  */
    /* Note: this variable is only used in an assert expression below */
    /* and then only if CF2_NDEBUG is not defined                     */
    CF2_UInt  mask = ( 1 << ( -(CF2_Int)bitCount & 7 ) ) - 1;
#endif


    /* initialize counts and isValid */
    if ( cf2_hintmask_setCounts( hintmask, bitCount ) == 0 )
      return;

    FT_ASSERT( hintmask->byteCount > 0 );

    FT_TRACE4(( " (maskbytes:" ));

    /* set mask and advance interpreter's charstring pointer */
    for ( i = 0; i < hintmask->byteCount; i++ )
    {
      hintmask->mask[i] = (FT_Byte)cf2_buf_readByte( charstring );
      FT_TRACE4(( " 0x%02X", hintmask->mask[i] ));
    }

    FT_TRACE4(( ")\n" ));

    /* assert any unused bits in last byte are zero unless there's a prior */
    /* error                                                               */
    /* bitCount -> mask, 0 -> 0, 1 -> 7f, 2 -> 3f, ... 6 -> 3, 7 -> 1      */
#ifndef CF2_NDEBUG
    FT_ASSERT( ( hintmask->mask[hintmask->byteCount - 1] & mask ) == 0 ||
               *hintmask->error                                        );
#endif
  }


  FT_LOCAL_DEF( void )
  cf2_hintmask_setAll( CF2_HintMask  hintmask,
                       size_t        bitCount )
  {
    size_t    i;
    CF2_UInt  mask = ( 1 << ( -(CF2_Int)bitCount & 7 ) ) - 1;


    /* initialize counts and isValid */
    if ( cf2_hintmask_setCounts( hintmask, bitCount ) == 0 )
      return;

    FT_ASSERT( hintmask->byteCount > 0 );
    FT_ASSERT( hintmask->byteCount <
                 sizeof ( hintmask->mask ) / sizeof ( hintmask->mask[0] ) );

    /* set mask to all ones */
    for ( i = 0; i < hintmask->byteCount; i++ )
      hintmask->mask[i] = 0xFF;

    /* clear unused bits                                              */
    /* bitCount -> mask, 0 -> 0, 1 -> 7f, 2 -> 3f, ... 6 -> 3, 7 -> 1 */
    hintmask->mask[hintmask->byteCount - 1] &= ~mask;
  }


  /* Type2 charstring opcodes */
  enum
  {
    cf2_cmdRESERVED_0,   /* 0 */
    cf2_cmdHSTEM,        /* 1 */
    cf2_cmdRESERVED_2,   /* 2 */
    cf2_cmdVSTEM,        /* 3 */
    cf2_cmdVMOVETO,      /* 4 */
    cf2_cmdRLINETO,      /* 5 */
    cf2_cmdHLINETO,      /* 6 */
    cf2_cmdVLINETO,      /* 7 */
    cf2_cmdRRCURVETO,    /* 8 */
    cf2_cmdRESERVED_9,   /* 9 */
    cf2_cmdCALLSUBR,     /* 10 */
    cf2_cmdRETURN,       /* 11 */
    cf2_cmdESC,          /* 12 */
    cf2_cmdRESERVED_13,  /* 13 */
    cf2_cmdENDCHAR,      /* 14 */
    cf2_cmdRESERVED_15,  /* 15 */
    cf2_cmdRESERVED_16,  /* 16 */
    cf2_cmdRESERVED_17,  /* 17 */
    cf2_cmdHSTEMHM,      /* 18 */
    cf2_cmdHINTMASK,     /* 19 */
    cf2_cmdCNTRMASK,     /* 20 */
    cf2_cmdRMOVETO,      /* 21 */
    cf2_cmdHMOVETO,      /* 22 */
    cf2_cmdVSTEMHM,      /* 23 */
    cf2_cmdRCURVELINE,   /* 24 */
    cf2_cmdRLINECURVE,   /* 25 */
    cf2_cmdVVCURVETO,    /* 26 */
    cf2_cmdHHCURVETO,    /* 27 */
    cf2_cmdEXTENDEDNMBR, /* 28 */
    cf2_cmdCALLGSUBR,    /* 29 */
    cf2_cmdVHCURVETO,    /* 30 */
    cf2_cmdHVCURVETO     /* 31 */
  };

  enum
  {
    cf2_escDOTSECTION,   /* 0 */
    cf2_escRESERVED_1,   /* 1 */
    cf2_escRESERVED_2,   /* 2 */
    cf2_escAND,          /* 3 */
    cf2_escOR,           /* 4 */
    cf2_escNOT,          /* 5 */
    cf2_escRESERVED_6,   /* 6 */
    cf2_escRESERVED_7,   /* 7 */
    cf2_escRESERVED_8,   /* 8 */
    cf2_escABS,          /* 9 */
    cf2_escADD,          /* 10     like otherADD */
    cf2_escSUB,          /* 11     like otherSUB */
    cf2_escDIV,          /* 12 */
    cf2_escRESERVED_13,  /* 13 */
    cf2_escNEG,          /* 14 */
    cf2_escEQ,           /* 15 */
    cf2_escRESERVED_16,  /* 16 */
    cf2_escRESERVED_17,  /* 17 */
    cf2_escDROP,         /* 18 */
    cf2_escRESERVED_19,  /* 19 */
    cf2_escPUT,          /* 20     like otherPUT    */
    cf2_escGET,          /* 21     like otherGET    */
    cf2_escIFELSE,       /* 22     like otherIFELSE */
    cf2_escRANDOM,       /* 23     like otherRANDOM */
    cf2_escMUL,          /* 24     like otherMUL    */
    cf2_escRESERVED_25,  /* 25 */
    cf2_escSQRT,         /* 26 */
    cf2_escDUP,          /* 27     like otherDUP    */
    cf2_escEXCH,         /* 28     like otherEXCH   */
    cf2_escINDEX,        /* 29 */
    cf2_escROLL,         /* 30 */
    cf2_escRESERVED_31,  /* 31 */
    cf2_escRESERVED_32,  /* 32 */
    cf2_escRESERVED_33,  /* 33 */
    cf2_escHFLEX,        /* 34 */
    cf2_escFLEX,         /* 35 */
    cf2_escHFLEX1,       /* 36 */
    cf2_escFLEX1         /* 37 */
  };


  /* `stemHintArray' does not change once we start drawing the outline. */
  static void
  cf2_doStems( const CF2_Font  font,
               CF2_Stack       opStack,
               CF2_ArrStack    stemHintArray,
               CF2_Fixed*      width,
               FT_Bool*        haveWidth,
               CF2_Fixed       hintOffset )
  {
    CF2_UInt  i;
    CF2_UInt  count       = cf2_stack_count( opStack );
    FT_Bool   hasWidthArg = (FT_Bool)( count & 1 );

    /* variable accumulates delta values from operand stack */
    CF2_Fixed  position = hintOffset;

    if ( hasWidthArg && ! *haveWidth )
      *width = cf2_stack_getReal( opStack, 0 ) +
                 cf2_getNominalWidthX( font->decoder );

    if ( font->decoder->width_only )
      goto exit;

    for ( i = hasWidthArg ? 1 : 0; i < count; i += 2 )
    {
      /* construct a CF2_StemHint and push it onto the list */
      CF2_StemHintRec  stemhint;


      stemhint.min  =
        position   += cf2_stack_getReal( opStack, i );
      stemhint.max  =
        position   += cf2_stack_getReal( opStack, i + 1 );

      stemhint.used  = FALSE;
      stemhint.maxDS =
      stemhint.minDS = 0;

      cf2_arrstack_push( stemHintArray, &stemhint ); /* defer error check */
    }

    cf2_stack_clear( opStack );

  exit:
    /* cf2_doStems must define a width (may be default) */
    *haveWidth = TRUE;
  }


  static void
  cf2_doFlex( CF2_Stack       opStack,
              CF2_Fixed*      curX,
              CF2_Fixed*      curY,
              CF2_GlyphPath   glyphPath,
              const FT_Bool*  readFromStack,
              FT_Bool         doConditionalLastRead )
  {
    CF2_Fixed  vals[14];
    CF2_UInt   index;
    FT_Bool    isHFlex;
    CF2_Int    top, i, j;


    vals[0] = *curX;
    vals[1] = *curY;
    index   = 0;
    isHFlex = readFromStack[9] == FALSE;
    top     = isHFlex ? 9 : 10;

    for ( i = 0; i < top; i++ )
    {
      vals[i + 2] = vals[i];
      if ( readFromStack[i] )
        vals[i + 2] += cf2_stack_getReal( opStack, index++ );
    }

    if ( isHFlex )
      vals[9 + 2] = *curY;

    if ( doConditionalLastRead )
    {
      FT_Bool    lastIsX = (FT_Bool)( cf2_fixedAbs( vals[10] - *curX ) >
                                        cf2_fixedAbs( vals[11] - *curY ) );
      CF2_Fixed  lastVal = cf2_stack_getReal( opStack, index );


      if ( lastIsX )
      {
        vals[12] = vals[10] + lastVal;
        vals[13] = *curY;
      }
      else
      {
        vals[12] = *curX;
        vals[13] = vals[11] + lastVal;
      }
    }
    else
    {
      if ( readFromStack[10] )
        vals[12] = vals[10] + cf2_stack_getReal( opStack, index++ );
      else
        vals[12] = *curX;

      if ( readFromStack[11] )
        vals[13] = vals[11] + cf2_stack_getReal( opStack, index );
      else
        vals[13] = *curY;
    }

    for ( j = 0; j < 2; j++ )
      cf2_glyphpath_curveTo( glyphPath, vals[j * 6 + 2],
                                        vals[j * 6 + 3],
                                        vals[j * 6 + 4],
                                        vals[j * 6 + 5],
                                        vals[j * 6 + 6],
                                        vals[j * 6 + 7] );

    cf2_stack_clear( opStack );

    *curX = vals[12];
    *curY = vals[13];
  }


  /*
   * `error' is a shared error code used by many objects in this
   * routine.  Before the code continues from an error, it must check and
   * record the error in `*error'.  The idea is that this shared
   * error code will record the first error encountered.  If testing
   * for an error anyway, the cost of `goto exit' is small, so we do it,
   * even if continuing would be safe.  In this case, `lastError' is
   * set, so the testing and storing can be done in one place, at `exit'.
   *
   * Continuing after an error is intended for objects which do their own
   * testing of `*error', e.g., array stack functions.  This allows us to
   * avoid an extra test after the call.
   *
   * Unimplemented opcodes are ignored.
   *
   */
  FT_LOCAL_DEF( void )
  cf2_interpT2CharString( CF2_Font              font,
                          CF2_Buffer            buf,
                          CF2_OutlineCallbacks  callbacks,
                          const FT_Vector*      translation,
                          FT_Bool               doingSeac,
                          CF2_Fixed             curX,
                          CF2_Fixed             curY,
                          CF2_Fixed*            width )
  {
    /* lastError is used for errors that are immediately tested */
    FT_Error  lastError = FT_Err_Ok;

    /* pointer to parsed font object */
    CFF_Decoder*  decoder = font->decoder;

    FT_Error*  error  = &font->error;
    FT_Memory  memory = font->memory;

    CF2_Fixed  scaleY        = font->innerTransform.d;
    CF2_Fixed  nominalWidthX = cf2_getNominalWidthX( decoder );

    /* save this for hinting seac accents */
    CF2_Fixed  hintOriginY = curY;

    CF2_Stack  opStack = NULL;
    FT_Byte    op1;                       /* first opcode byte */

    /* instruction limit; 20,000,000 matches Avalon */
    FT_UInt32  instructionLimit = 20000000UL;

    CF2_ArrStackRec  subrStack;

    FT_Bool     haveWidth;
    CF2_Buffer  charstring = NULL;

    CF2_Int  charstringIndex = -1;       /* initialize to empty */

    /* TODO: placeholders for hint structures */

    /* objects used for hinting */
    CF2_ArrStackRec  hStemHintArray;
    CF2_ArrStackRec  vStemHintArray;

    CF2_HintMaskRec   hintMask;
    CF2_GlyphPathRec  glyphPath;


    /* initialize the remaining objects */
    cf2_arrstack_init( &subrStack,
                       memory,
                       error,
                       sizeof ( CF2_BufferRec ) );
    cf2_arrstack_init( &hStemHintArray,
                       memory,
                       error,
                       sizeof ( CF2_StemHintRec ) );
    cf2_arrstack_init( &vStemHintArray,
                       memory,
                       error,
                       sizeof ( CF2_StemHintRec ) );

    /* initialize CF2_StemHint arrays */
    cf2_hintmask_init( &hintMask, error );

    /* initialize path map to manage drawing operations */

    /* Note: last 4 params are used to handle `MoveToPermissive', which */
    /*       may need to call `hintMap.Build'                           */
    /* TODO: MoveToPermissive is gone; are these still needed?          */
    cf2_glyphpath_init( &glyphPath,
                        font,
                        callbacks,
                        scaleY,
                        /* hShift, */
                        &hStemHintArray,
                        &vStemHintArray,
                        &hintMask,
                        hintOriginY,
                        &font->blues,
                        translation );

    /*
     * Initialize state for width parsing.  From the CFF Spec:
     *
     *   The first stack-clearing operator, which must be one of hstem,
     *   hstemhm, vstem, vstemhm, cntrmask, hintmask, hmoveto, vmoveto,
     *   rmoveto, or endchar, takes an additional argument - the width (as
     *   described earlier), which may be expressed as zero or one numeric
     *   argument.
     *
     * What we implement here uses the first validly specified width, but
     * does not detect errors for specifying more than one width.
     *
     * If one of the above operators occurs without explicitly specifying
     * a width, we assume the default width.
     *
     */
    haveWidth = FALSE;
    *width    = cf2_getDefaultWidthX( decoder );

    /*
     * Note: at this point, all pointers to resources must be NULL
     * and all local objects must be initialized.
     * There must be no branches to exit: above this point.
     *
     */

    /* allocate an operand stack */
    opStack = cf2_stack_init( memory, error );
    if ( !opStack )
    {
      lastError = FT_THROW( Out_Of_Memory );
      goto exit;
    }

    /* initialize subroutine stack by placing top level charstring as */
    /* first element (max depth plus one for the charstring)          */
    /* Note: Caller owns and must finalize the first charstring.      */
    /*       Our copy of it does not change that requirement.         */
    cf2_arrstack_setCount( &subrStack, CF2_MAX_SUBR + 1 );

    charstring  = (CF2_Buffer)cf2_arrstack_getBuffer( &subrStack );
    *charstring = *buf;    /* structure copy */

    charstringIndex = 0;       /* entry is valid now */

    /* catch errors so far */
    if ( *error )
      goto exit;

    /* main interpreter loop */
    while ( 1 )
    {
      if ( cf2_buf_isEnd( charstring ) )
      {
        /* If we've reached the end of the charstring, simulate a */
        /* cf2_cmdRETURN or cf2_cmdENDCHAR.                       */
        if ( charstringIndex )
          op1 = cf2_cmdRETURN;  /* end of buffer for subroutine */
        else
          op1 = cf2_cmdENDCHAR; /* end of buffer for top level charstring */
      }
      else
        op1 = (FT_Byte)cf2_buf_readByte( charstring );

      /* check for errors once per loop */
      if ( *error )
        goto exit;

      instructionLimit--;
      if ( instructionLimit == 0 )
      {
        lastError = FT_THROW( Invalid_Glyph_Format );
        goto exit;
      }

      switch( op1 )
      {
      case cf2_cmdRESERVED_0:
      case cf2_cmdRESERVED_2:
      case cf2_cmdRESERVED_9:
      case cf2_cmdRESERVED_13:
      case cf2_cmdRESERVED_15:
      case cf2_cmdRESERVED_16:
      case cf2_cmdRESERVED_17:
        /* we may get here if we have a prior error */
        FT_TRACE4(( " unknown op (%d)\n", op1 ));
        break;

      case cf2_cmdHSTEMHM:
      case cf2_cmdHSTEM:
        FT_TRACE4(( op1 == cf2_cmdHSTEMHM ? " hstemhm\n" : " hstem\n" ));

        /* never add hints after the mask is computed */
        if ( cf2_hintmask_isValid( &hintMask ) )
          FT_TRACE4(( "cf2_interpT2CharString:"
                      " invalid horizontal hint mask\n" ));

        cf2_doStems( font,
                     opStack,
                     &hStemHintArray,
                     width,
                     &haveWidth,
                     0 );

        if ( font->decoder->width_only )
            goto exit;

        break;

      case cf2_cmdVSTEMHM:
      case cf2_cmdVSTEM:
        FT_TRACE4(( op1 == cf2_cmdVSTEMHM ? " vstemhm\n" : " vstem\n" ));

        /* never add hints after the mask is computed */
        if ( cf2_hintmask_isValid( &hintMask ) )
          FT_TRACE4(( "cf2_interpT2CharString:"
                      " invalid vertical hint mask\n" ));

        cf2_doStems( font,
                     opStack,
                     &vStemHintArray,
                     width,
                     &haveWidth,
                     0 );

        if ( font->decoder->width_only )
            goto exit;

        break;

      case cf2_cmdVMOVETO:
        FT_TRACE4(( " vmoveto\n" ));

        if ( cf2_stack_count( opStack ) > 1 && !haveWidth )
          *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        curY += cf2_stack_popFixed( opStack );

        cf2_glyphpath_moveTo( &glyphPath, curX, curY );

        break;

      case cf2_cmdRLINETO:
        {
          CF2_UInt  index;
          CF2_UInt  count = cf2_stack_count( opStack );


          FT_TRACE4(( " rlineto\n" ));

          for ( index = 0; index < count; index += 2 )
          {
            curX += cf2_stack_getReal( opStack, index + 0 );
            curY += cf2_stack_getReal( opStack, index + 1 );

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdHLINETO:
      case cf2_cmdVLINETO:
        {
          CF2_UInt  index;
          CF2_UInt  count = cf2_stack_count( opStack );

          FT_Bool  isX = op1 == cf2_cmdHLINETO;


          FT_TRACE4(( isX ? " hlineto\n" : " vlineto\n" ));

          for ( index = 0; index < count; index++ )
          {
            CF2_Fixed  v = cf2_stack_getReal( opStack, index );


            if ( isX )
              curX += v;
            else
              curY += v;

            isX = !isX;

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
          }

          cf2_stack_clear( opStack );
        }
        continue;

      case cf2_cmdRCURVELINE:
      case cf2_cmdRRCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( op1 == cf2_cmdRCURVELINE ? " rcurveline\n"
                                               : " rrcurveto\n" ));

          while ( index + 6 <= count )
          {
            CF2_Fixed  x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
            CF2_Fixed  y1 = cf2_stack_getReal( opStack, index + 1 ) + curY;
            CF2_Fixed  x2 = cf2_stack_getReal( opStack, index + 2 ) + x1;
            CF2_Fixed  y2 = cf2_stack_getReal( opStack, index + 3 ) + y1;
            CF2_Fixed  x3 = cf2_stack_getReal( opStack, index + 4 ) + x2;
            CF2_Fixed  y3 = cf2_stack_getReal( opStack, index + 5 ) + y2;


            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 6;
          }

          if ( op1 == cf2_cmdRCURVELINE )
          {
            curX += cf2_stack_getReal( opStack, index + 0 );
            curY += cf2_stack_getReal( opStack, index + 1 );

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdCALLGSUBR:
      case cf2_cmdCALLSUBR:
        {
          CF2_UInt  subrIndex;


          FT_TRACE4(( op1 == cf2_cmdCALLGSUBR ? " callgsubr"
                                              : " callsubr" ));

          if ( charstringIndex > CF2_MAX_SUBR )
          {
            /* max subr plus one for charstring */
            lastError = FT_THROW( Invalid_Glyph_Format );
            goto exit;                      /* overflow of stack */
          }

          /* push our current CFF charstring region on subrStack */
          charstring = (CF2_Buffer)
                         cf2_arrstack_getPointer( &subrStack,
                                                  charstringIndex + 1 );

          /* set up the new CFF region and pointer */
          subrIndex = cf2_stack_popInt( opStack );

          switch ( op1 )
          {
          case cf2_cmdCALLGSUBR:
            FT_TRACE4(( "(%d)\n", subrIndex + decoder->globals_bias ));

            if ( cf2_initGlobalRegionBuffer( decoder,
                                             subrIndex,
                                             charstring ) )
            {
              lastError = FT_THROW( Invalid_Glyph_Format );
              goto exit;  /* subroutine lookup or stream error */
            }
            break;

          default:
            /* cf2_cmdCALLSUBR */
            FT_TRACE4(( "(%d)\n", subrIndex + decoder->locals_bias ));

            if ( cf2_initLocalRegionBuffer( decoder,
                                            subrIndex,
                                            charstring ) )
            {
              lastError = FT_THROW( Invalid_Glyph_Format );
              goto exit;  /* subroutine lookup or stream error */
            }
          }

          charstringIndex += 1;       /* entry is valid now */
        }
        continue; /* do not clear the stack */

      case cf2_cmdRETURN:
        FT_TRACE4(( " return\n" ));

        if ( charstringIndex < 1 )
        {
          /* Note: cannot return from top charstring */
          lastError = FT_THROW( Invalid_Glyph_Format );
          goto exit;                      /* underflow of stack */
        }

        /* restore position in previous charstring */
        charstring = (CF2_Buffer)
                       cf2_arrstack_getPointer( &subrStack,
                                                --charstringIndex );
        continue;     /* do not clear the stack */

      case cf2_cmdESC:
        {
          FT_Byte  op2 = (FT_Byte)cf2_buf_readByte( charstring );


          switch ( op2 )
          {
          case cf2_escDOTSECTION:
            /* something about `flip type of locking' -- ignore it */
            FT_TRACE4(( " dotsection\n" ));

            break;

          /* TODO: should these operators be supported? */
          case cf2_escAND: /* in spec */
            FT_TRACE4(( " and\n" ));

            CF2_FIXME;
            break;

          case cf2_escOR: /* in spec */
            FT_TRACE4(( " or\n" ));

            CF2_FIXME;
            break;

          case cf2_escNOT: /* in spec */
            FT_TRACE4(( " not\n" ));

            CF2_FIXME;
            break;

          case cf2_escABS: /* in spec */
            FT_TRACE4(( " abs\n" ));

            CF2_FIXME;
            break;

          case cf2_escADD: /* in spec */
            FT_TRACE4(( " add\n" ));

            CF2_FIXME;
            break;

          case cf2_escSUB: /* in spec */
            FT_TRACE4(( " sub\n" ));

            CF2_FIXME;
            break;

          case cf2_escDIV: /* in spec */
            FT_TRACE4(( " div\n" ));

            CF2_FIXME;
            break;

          case cf2_escNEG: /* in spec */
            FT_TRACE4(( " neg\n" ));

            CF2_FIXME;
            break;

          case cf2_escEQ: /* in spec */
            FT_TRACE4(( " eq\n" ));

            CF2_FIXME;
            break;

          case cf2_escDROP: /* in spec */
            FT_TRACE4(( " drop\n" ));

            CF2_FIXME;
            break;

          case cf2_escPUT: /* in spec */
            FT_TRACE4(( " put\n" ));

            CF2_FIXME;
            break;

          case cf2_escGET: /* in spec */
            FT_TRACE4(( " get\n" ));

            CF2_FIXME;
            break;

          case cf2_escIFELSE: /* in spec */
            FT_TRACE4(( " ifelse\n" ));

            CF2_FIXME;
            break;

          case cf2_escRANDOM: /* in spec */
            FT_TRACE4(( " random\n" ));

            CF2_FIXME;
            break;

          case cf2_escMUL: /* in spec */
            FT_TRACE4(( " mul\n" ));

            CF2_FIXME;
            break;

          case cf2_escSQRT: /* in spec */
            FT_TRACE4(( " sqrt\n" ));

            CF2_FIXME;
            break;

          case cf2_escDUP: /* in spec */
            FT_TRACE4(( " dup\n" ));

            CF2_FIXME;
            break;

          case cf2_escEXCH: /* in spec */
            FT_TRACE4(( " exch\n" ));

            CF2_FIXME;
            break;

          case cf2_escINDEX: /* in spec */
            FT_TRACE4(( " index\n" ));

            CF2_FIXME;
            break;

          case cf2_escROLL: /* in spec */
            FT_TRACE4(( " roll\n" ));

            CF2_FIXME;
            break;

          case cf2_escHFLEX:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE /* dx1 */, FALSE /* dy1 */,
                TRUE /* dx2 */, TRUE  /* dy2 */,
                TRUE /* dx3 */, FALSE /* dy3 */,
                TRUE /* dx4 */, FALSE /* dy4 */,
                TRUE /* dx5 */, FALSE /* dy5 */,
                TRUE /* dx6 */, FALSE /* dy6 */
              };


              FT_TRACE4(( " hflex\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          FALSE /* doConditionalLastRead */ );
            }
            continue;

          case cf2_escFLEX:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE /* dx1 */, TRUE /* dy1 */,
                TRUE /* dx2 */, TRUE /* dy2 */,
                TRUE /* dx3 */, TRUE /* dy3 */,
                TRUE /* dx4 */, TRUE /* dy4 */,
                TRUE /* dx5 */, TRUE /* dy5 */,
                TRUE /* dx6 */, TRUE /* dy6 */
              };


              FT_TRACE4(( " flex\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          FALSE /* doConditionalLastRead */ );
            }
            break;      /* TODO: why is this not a continue? */

          case cf2_escHFLEX1:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE /* dx1 */, TRUE  /* dy1 */,
                TRUE /* dx2 */, TRUE  /* dy2 */,
                TRUE /* dx3 */, FALSE /* dy3 */,
                TRUE /* dx4 */, FALSE /* dy4 */,
                TRUE /* dx5 */, TRUE  /* dy5 */,
                TRUE /* dx6 */, FALSE /* dy6 */
              };


              FT_TRACE4(( " hflex1\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          FALSE /* doConditionalLastRead */ );
            }
            continue;

          case cf2_escFLEX1:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE  /* dx1 */, TRUE  /* dy1 */,
                TRUE  /* dx2 */, TRUE  /* dy2 */,
                TRUE  /* dx3 */, TRUE  /* dy3 */,
                TRUE  /* dx4 */, TRUE  /* dy4 */,
                TRUE  /* dx5 */, TRUE  /* dy5 */,
                FALSE /* dx6 */, FALSE /* dy6 */
              };


              FT_TRACE4(( " flex1\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          TRUE /* doConditionalLastRead */ );
            }
            continue;

          case cf2_escRESERVED_1:
          case cf2_escRESERVED_2:
          case cf2_escRESERVED_6:
          case cf2_escRESERVED_7:
          case cf2_escRESERVED_8:
          case cf2_escRESERVED_13:
          case cf2_escRESERVED_16:
          case cf2_escRESERVED_17:
          case cf2_escRESERVED_19:
          case cf2_escRESERVED_25:
          case cf2_escRESERVED_31:
          case cf2_escRESERVED_32:
          case cf2_escRESERVED_33:
          default:
            FT_TRACE4(( " unknown op (12, %d)\n", op2 ));

          }; /* end of switch statement checking `op2' */

        } /* case cf2_cmdESC */
        break;

      case cf2_cmdENDCHAR:
        FT_TRACE4(( " endchar\n" ));

        if ( cf2_stack_count( opStack ) == 1 ||
             cf2_stack_count( opStack ) == 5 )
        {
          if ( !haveWidth )
            *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;
        }

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        /* close path if still open */
        cf2_glyphpath_closeOpenPath( &glyphPath );

        if ( cf2_stack_count( opStack ) > 1 )
        {
          /* must be either 4 or 5 --                       */
          /* this is a (deprecated) implied `seac' operator */

          CF2_UInt       achar;
          CF2_UInt       bchar;
          CF2_BufferRec  component;
          CF2_Fixed      dummyWidth;   /* ignore component width */
          FT_Error       error2;


          if ( doingSeac )
          {
            lastError = FT_THROW( Invalid_Glyph_Format );
            goto exit;      /* nested seac */
          }

          achar = cf2_stack_popInt( opStack );
          bchar = cf2_stack_popInt( opStack );

          curY = cf2_stack_popFixed( opStack );
          curX = cf2_stack_popFixed( opStack );

          error2 = cf2_getSeacComponent( decoder, achar, &component );
          if ( error2 )
          {
             lastError = error2;      /* pass FreeType error through */
             goto exit;
          }
          cf2_interpT2CharString( font,
                                  &component,
                                  callbacks,
                                  translation,
                                  TRUE,
                                  curX,
                                  curY,
                                  &dummyWidth );
          cf2_freeSeacComponent( decoder, &component );

          error2 = cf2_getSeacComponent( decoder, bchar, &component );
          if ( error2 )
          {
            lastError = error2;      /* pass FreeType error through */
            goto exit;
          }
          cf2_interpT2CharString( font,
                                  &component,
                                  callbacks,
                                  translation,
                                  TRUE,
                                  0,
                                  0,
                                  &dummyWidth );
          cf2_freeSeacComponent( decoder, &component );
        }
        goto exit;

      case cf2_cmdCNTRMASK:
      case cf2_cmdHINTMASK:
        /* the final \n in the tracing message gets added in      */
        /* `cf2_hintmask_read' (which also traces the mask bytes) */
        FT_TRACE4(( op1 == cf2_cmdCNTRMASK ? " cntrmask" : " hintmask" ));

        /* if there are arguments on the stack, there this is an */
        /* implied cf2_cmdVSTEMHM                                */
        if ( cf2_stack_count( opStack ) != 0 )
        {
          /* never add hints after the mask is computed */
          if ( cf2_hintmask_isValid( &hintMask ) )
            FT_TRACE4(( "cf2_interpT2CharString: invalid hint mask\n" ));
        }

        cf2_doStems( font,
                     opStack,
                     &vStemHintArray,
                     width,
                     &haveWidth,
                     0 );

        if ( font->decoder->width_only )
            goto exit;

        if ( op1 == cf2_cmdHINTMASK )
        {
          /* consume the hint mask bytes which follow the operator */
          cf2_hintmask_read( &hintMask,
                             charstring,
                             cf2_arrstack_size( &hStemHintArray ) +
                               cf2_arrstack_size( &vStemHintArray ) );
        }
        else
        {
          /*
           * Consume the counter mask bytes which follow the operator:
           * Build a temporary hint map, just to place and lock those
           * stems participating in the counter mask.  These are most
           * likely the dominant hstems, and are grouped together in a
           * few counter groups, not necessarily in correspondence
           * with the hint groups.  This reduces the chances of
           * conflicts between hstems that are initially placed in
           * separate hint groups and then brought together.  The
           * positions are copied back to `hStemHintArray', so we can
           * discard `counterMask' and `counterHintMap'.
           *
           */
          CF2_HintMapRec   counterHintMap;
          CF2_HintMaskRec  counterMask;


          cf2_hintmap_init( &counterHintMap,
                            font,
                            &glyphPath.initialHintMap,
                            &glyphPath.hintMoves,
                            scaleY );
          cf2_hintmask_init( &counterMask, error );

          cf2_hintmask_read( &counterMask,
                             charstring,
                             cf2_arrstack_size( &hStemHintArray ) +
                               cf2_arrstack_size( &vStemHintArray ) );
          cf2_hintmap_build( &counterHintMap,
                             &hStemHintArray,
                             &vStemHintArray,
                             &counterMask,
                             0,
                             FALSE );
        }
        break;

      case cf2_cmdRMOVETO:
        FT_TRACE4(( " rmoveto\n" ));

        if ( cf2_stack_count( opStack ) > 2 && !haveWidth )
          *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        curY += cf2_stack_popFixed( opStack );
        curX += cf2_stack_popFixed( opStack );

        cf2_glyphpath_moveTo( &glyphPath, curX, curY );

        break;

      case cf2_cmdHMOVETO:
        FT_TRACE4(( " hmoveto\n" ));

        if ( cf2_stack_count( opStack ) > 1 && !haveWidth )
          *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        curX += cf2_stack_popFixed( opStack );

        cf2_glyphpath_moveTo( &glyphPath, curX, curY );

        break;

      case cf2_cmdRLINECURVE:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( " rlinecurve\n" ));

          while ( index + 6 < count )
          {
            curX += cf2_stack_getReal( opStack, index + 0 );
            curY += cf2_stack_getReal( opStack, index + 1 );

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
            index += 2;
          }

          while ( index < count )
          {
            CF2_Fixed  x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
            CF2_Fixed  y1 = cf2_stack_getReal( opStack, index + 1 ) + curY;
            CF2_Fixed  x2 = cf2_stack_getReal( opStack, index + 2 ) + x1;
            CF2_Fixed  y2 = cf2_stack_getReal( opStack, index + 3 ) + y1;
            CF2_Fixed  x3 = cf2_stack_getReal( opStack, index + 4 ) + x2;
            CF2_Fixed  y3 = cf2_stack_getReal( opStack, index + 5 ) + y2;


            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 6;
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdVVCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( " vvcurveto\n" ));

          while ( index < count )
          {
            CF2_Fixed  x1, y1, x2, y2, x3, y3;


            if ( ( count - index ) & 1 )
            {
              x1 = cf2_stack_getReal( opStack, index ) + curX;

              ++index;
            }
            else
              x1 = curX;

            y1 = cf2_stack_getReal( opStack, index + 0 ) + curY;
            x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
            y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
            x3 = x2;
            y3 = cf2_stack_getReal( opStack, index + 3 ) + y2;

            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 4;
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdHHCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( " hhcurveto\n" ));

          while ( index < count )
          {
            CF2_Fixed  x1, y1, x2, y2, x3, y3;


            if ( ( count - index ) & 1 )
            {
              y1 = cf2_stack_getReal( opStack, index ) + curY;

              ++index;
            }
            else
              y1 = curY;

            x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
            x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
            y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
            x3 = cf2_stack_getReal( opStack, index + 3 ) + x2;
            y3 = y2;

            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 4;
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdVHCURVETO:
      case cf2_cmdHVCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;

          FT_Bool  alternate = op1 == cf2_cmdHVCURVETO;


          FT_TRACE4(( alternate ? " hvcurveto\n" : " vhcurveto\n" ));

          while ( index < count )
          {
            CF2_Fixed x1, x2, x3, y1, y2, y3;


            if ( alternate )
            {
              x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
              y1 = curY;
              x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
              y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
              y3 = cf2_stack_getReal( opStack, index + 3 ) + y2;

              if ( count - index == 5 )
              {
                x3 = cf2_stack_getReal( opStack, index + 4 ) + x2;

                ++index;
              }
              else
                x3 = x2;

              alternate = FALSE;
            }
            else
            {
              x1 = curX;
              y1 = cf2_stack_getReal( opStack, index + 0 ) + curY;
              x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
              y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
              x3 = cf2_stack_getReal( opStack, index + 3 ) + x2;

              if ( count - index == 5 )
              {
                y3 = cf2_stack_getReal( opStack, index + 4 ) + y2;

                ++index;
              }
              else
                y3 = y2;

              alternate = TRUE;
            }

            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 4;
          }

          cf2_stack_clear( opStack );
        }
        continue;     /* no need to clear stack again */

      case cf2_cmdEXTENDEDNMBR:
        {
          CF2_Int  v;

#if 1
          CF2_Int  byte1 = cf2_buf_readByte( charstring );
          CF2_Int  byte2 = cf2_buf_readByte( charstring );

          v = (FT_Short)( ( byte1 << 8 ) |
                            byte2        );
#else
          v = (FT_Short)( ( cf2_buf_readByte( charstring ) << 8 ) |
                            cf2_buf_readByte( charstring )        );
#endif
          FT_TRACE4(( " %d", v ));

          cf2_stack_pushInt( opStack, v );
        }
        continue;

      default:
        /* numbers */
        {
          if ( /* op1 >= 32 && */ op1 <= 246 )
          {
            CF2_Int  v;


            v = op1 - 139;

            FT_TRACE4(( " %d", v ));

            /* -107 .. 107 */
            cf2_stack_pushInt( opStack, v );
          }

          else if ( /* op1 >= 247 && */ op1 <= 250 )
          {
            CF2_Int  v;


            v  = op1;
            v -= 247;
            v *= 256;
            v += cf2_buf_readByte( charstring );
            v += 108;

            FT_TRACE4(( " %d", v ));

            /* 108 .. 1131 */
            cf2_stack_pushInt( opStack, v );
          }

          else if ( /* op1 >= 251 && */ op1 <= 254 )
          {
            CF2_Int  v;


            v  = op1;
            v -= 251;
            v *= 256;
            v += cf2_buf_readByte( charstring );
            v  = -v - 108;

            FT_TRACE4(( " %d", v ));

            /* -1131 .. -108 */
            cf2_stack_pushInt( opStack, v );
          }

          else /* op1 == 255 */
          {
            CF2_Fixed  v;
          
#if 1
            CF2_Int  byte1 = cf2_buf_readByte( charstring );
            CF2_Int  byte2 = cf2_buf_readByte( charstring );
            CF2_Int  byte3 = cf2_buf_readByte( charstring );
            CF2_Int  byte4 = cf2_buf_readByte( charstring );

            v = (CF2_Fixed)
                  ( ( (FT_UInt32)byte1 << 24 ) |
                    ( (FT_UInt32)byte2 << 16 ) |
                    ( (FT_UInt32)byte3 <<  8 ) |
                    ( (FT_UInt32)byte4       ) );
#else
            v = (CF2_Fixed)
                  ( ( (FT_UInt32)cf2_buf_readByte( charstring ) << 24 ) |
                    ( (FT_UInt32)cf2_buf_readByte( charstring ) << 16 ) |
                    ( (FT_UInt32)cf2_buf_readByte( charstring ) <<  8 ) |
                      (FT_UInt32)cf2_buf_readByte( charstring )         );
#endif
            FT_TRACE4(( " %.2f", v / 65536.0 ));

            cf2_stack_pushFixed( opStack, v );
          }
        }
        continue;   /* don't clear stack */

      } /* end of switch statement checking `op1' */

      cf2_stack_clear( opStack );

    } /* end of main interpreter loop */

    /* we get here if the charstring ends without cf2_cmdENDCHAR */
    FT_TRACE4(( "cf2_interpT2CharString:"
                "  charstring ends without ENDCHAR\n" ));

  exit:
    /* check whether last error seen is also the first one */
    cf2_setError( error, lastError );

    /* free resources from objects we've used */
    cf2_glyphpath_finalize( &glyphPath );
    cf2_arrstack_finalize( &vStemHintArray );
    cf2_arrstack_finalize( &hStemHintArray );
    cf2_arrstack_finalize( &subrStack );
    cf2_stack_free( opStack );

    FT_TRACE4(( "\n" ));

    return;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2read.c                                                              */
/*                                                                         */
/*    Adobe's code for stream handling (body).                             */
/*                                                                         */
/*  Copyright 2007-2013 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2glue.h"

#include "cf2error.h"


  /* Define CF2_IO_FAIL as 1 to enable random errors and random */
  /* value errors in I/O.                                       */
#define CF2_IO_FAIL  0


#if CF2_IO_FAIL

  /* set the .00 value to a nonzero probability */
  static int
  randomError2( void )
  {
    /* for region buffer ReadByte (interp) function */
    return (double)rand() / RAND_MAX < .00;
  }

  /* set the .00 value to a nonzero probability */
  static CF2_Int
  randomValue()
  {
    return (double)rand() / RAND_MAX < .00 ? rand() : 0;
  }

#endif /* CF2_IO_FAIL */


  /* Region Buffer                                      */
  /*                                                    */
  /* Can be constructed from a copied buffer managed by */
  /* `FCM_getDatablock'.                                */
  /* Reads bytes with check for end of buffer.          */

  /* reading past the end of the buffer sets error and returns zero */
  FT_LOCAL_DEF( CF2_Int )
  cf2_buf_readByte( CF2_Buffer  buf )
  {
    if ( buf->ptr < buf->end )
    {
#if CF2_IO_FAIL
      if ( randomError2() )
      {
        CF2_SET_ERROR( buf->error, Invalid_Stream_Operation );
        return 0;
      }

      return *(buf->ptr)++ + randomValue();
#else
      return *(buf->ptr)++;
#endif
    }
    else
    {
      CF2_SET_ERROR( buf->error, Invalid_Stream_Operation );
      return 0;
    }
  }


  /* note: end condition can occur without error */
  FT_LOCAL_DEF( FT_Bool )
  cf2_buf_isEnd( CF2_Buffer  buf )
  {
    return (FT_Bool)( buf->ptr >= buf->end );
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  cf2stack.c                                                             */
/*                                                                         */
/*    Adobe's code for emulating a CFF stack (body).                       */
/*                                                                         */
/*  Copyright 2007-2013 Adobe Systems Incorporated.                        */
/*                                                                         */
/*  This software, and all works of authorship, whether in source or       */
/*  object code form as indicated by the copyright notice(s) included      */
/*  herein (collectively, the "Work") is made available, and may only be   */
/*  used, modified, and distributed under the FreeType Project License,    */
/*  LICENSE.TXT.  Additionally, subject to the terms and conditions of the */
/*  FreeType Project License, each contributor to the Work hereby grants   */
/*  to any individual or legal entity exercising permissions granted by    */
/*  the FreeType Project License and this section (hereafter, "You" or     */
/*  "Your") a perpetual, worldwide, non-exclusive, no-charge,              */
/*  royalty-free, irrevocable (except as stated in this section) patent    */
/*  license to make, have made, use, offer to sell, sell, import, and      */
/*  otherwise transfer the Work, where such license applies only to those  */
/*  patent claims licensable by such contributor that are necessarily      */
/*  infringed by their contribution(s) alone or by combination of their    */
/*  contribution(s) with the Work to which such contribution(s) was        */
/*  submitted.  If You institute patent litigation against any entity      */
/*  (including a cross-claim or counterclaim in a lawsuit) alleging that   */
/*  the Work or a contribution incorporated within the Work constitutes    */
/*  direct or contributory patent infringement, then any patent licenses   */
/*  granted to You under this License for that Work shall terminate as of  */
/*  the date such litigation is filed.                                     */
/*                                                                         */
/*  By using, modifying, or distributing the Work you indicate that you    */
/*  have read and understood the terms and conditions of the               */
/*  FreeType Project License as well as those provided in this section,    */
/*  and you accept them fully.                                             */
/*                                                                         */
/***************************************************************************/


#include "cf2ft.h"
#include FT_INTERNAL_DEBUG_H

#include "cf2glue.h"
#include "cf2font.h"
#include "cf2stack.h"

#include "cf2error.h"


  /* Allocate and initialize an instance of CF2_Stack.       */
  /* Note: This function returns NULL on error (does not set */
  /* `error').                                               */
  FT_LOCAL_DEF( CF2_Stack )
  cf2_stack_init( FT_Memory  memory,
                  FT_Error*  e )
  {
    FT_Error  error = FT_Err_Ok;     /* for FT_QNEW */

    CF2_Stack  stack = NULL;


    if ( !FT_QNEW( stack ) )
    {
      /* initialize the structure; FT_QNEW zeroes it */
      stack->memory = memory;
      stack->error  = e;
      stack->top    = &stack->buffer[0]; /* empty stack */
    }

    return stack;
  }


  FT_LOCAL_DEF( void )
  cf2_stack_free( CF2_Stack  stack )
  {
    if ( stack )
    {
      FT_Memory  memory = stack->memory;


      /* free the main structure */
      FT_FREE( stack );
    }
  }


  FT_LOCAL_DEF( CF2_UInt )
  cf2_stack_count( CF2_Stack  stack )
  {
    return (CF2_UInt)( stack->top - &stack->buffer[0] );
  }


  FT_LOCAL_DEF( void )
  cf2_stack_pushInt( CF2_Stack  stack,
                     CF2_Int    val )
  {
    if ( stack->top == &stack->buffer[CF2_OPERAND_STACK_SIZE] )
    {
      CF2_SET_ERROR( stack->error, Stack_Overflow );
      return;     /* stack overflow */
    }

    stack->top->u.i  = val;
    stack->top->type = CF2_NumberInt;
    ++stack->top;
  }


  FT_LOCAL_DEF( void )
  cf2_stack_pushFixed( CF2_Stack  stack,
                       CF2_Fixed  val )
  {
    if ( stack->top == &stack->buffer[CF2_OPERAND_STACK_SIZE] )
    {
      CF2_SET_ERROR( stack->error, Stack_Overflow );
      return;     /* stack overflow */
    }

    stack->top->u.r  = val;
    stack->top->type = CF2_NumberFixed;
    ++stack->top;
  }


  /* this function is only allowed to pop an integer type */
  FT_LOCAL_DEF( CF2_Int )
  cf2_stack_popInt( CF2_Stack  stack )
  {
    if ( stack->top == &stack->buffer[0] )
    {
      CF2_SET_ERROR( stack->error, Stack_Underflow );
      return 0;   /* underflow */
    }
    if ( stack->top[-1].type != CF2_NumberInt )
    {
      CF2_SET_ERROR( stack->error, Syntax_Error );
      return 0;   /* type mismatch */
    }

    --stack->top;

    return stack->top->u.i;
  }


  /* Note: type mismatch is silently cast */
  /* TODO: check this */
  FT_LOCAL_DEF( CF2_Fixed )
  cf2_stack_popFixed( CF2_Stack  stack )
  {
    if ( stack->top == &stack->buffer[0] )
    {
      CF2_SET_ERROR( stack->error, Stack_Underflow );
      return cf2_intToFixed( 0 );    /* underflow */
    }

    --stack->top;

    switch ( stack->top->type )
    {
    case CF2_NumberInt:
      return cf2_intToFixed( stack->top->u.i );
    case CF2_NumberFrac:
      return cf2_fracToFixed( stack->top->u.f );
    default:
      return stack->top->u.r;
    }
  }


  /* Note: type mismatch is silently cast */
  /* TODO: check this */
  FT_LOCAL_DEF( CF2_Fixed )
  cf2_stack_getReal( CF2_Stack  stack,
                     CF2_UInt   idx )
  {
    FT_ASSERT( cf2_stack_count( stack ) <= CF2_OPERAND_STACK_SIZE );

    if ( idx >= cf2_stack_count( stack ) )
    {
      CF2_SET_ERROR( stack->error, Stack_Overflow );
      return cf2_intToFixed( 0 );    /* bounds error */
    }

    switch ( stack->buffer[idx].type )
    {
    case CF2_NumberInt:
      return cf2_intToFixed( stack->buffer[idx].u.i );
    case CF2_NumberFrac:
      return cf2_fracToFixed( stack->buffer[idx].u.f );
    default:
      return stack->buffer[idx].u.r;
    }
  }


  FT_LOCAL_DEF( void )
  cf2_stack_clear( CF2_Stack  stack )
  {
    stack->top = &stack->buffer[0];
  }


/* END */

/* END */
