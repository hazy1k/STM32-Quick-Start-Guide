/***************************************************************************/
/*                                                                         */
/*  truetype.c                                                             */
/*                                                                         */
/*    FreeType TrueType driver component (body only).                      */
/*                                                                         */
/*  Copyright 1996-2001, 2004, 2006, 2012 by                               */
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
/*  ttpic.c                                                                */
/*                                                                         */
/*    The FreeType position independent code services for truetype module. */
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
#include "ttpic.h"
#include "tterrors.h"


#ifdef FT_CONFIG_OPTION_PIC

  /* forward declaration of PIC init functions from ttdriver.c */
  FT_Error
  FT_Create_Class_tt_services( FT_Library           library,
                               FT_ServiceDescRec**  output_class );
  void
  FT_Destroy_Class_tt_services( FT_Library          library,
                                FT_ServiceDescRec*  clazz );
  void
  FT_Init_Class_tt_service_gx_multi_masters(
    FT_Service_MultiMastersRec*  sv_mm );
  void
  FT_Init_Class_tt_service_truetype_glyf(
    FT_Service_TTGlyfRec*  sv_ttglyf );


  void
  tt_driver_class_pic_free( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Memory          memory        = library->memory;


    if ( pic_container->truetype )
    {
      TTModulePIC*  container = (TTModulePIC*)pic_container->truetype;


      if ( container->tt_services )
        FT_Destroy_Class_tt_services( library, container->tt_services );
      container->tt_services = NULL;
      FT_FREE( container );
      pic_container->truetype = NULL;
    }
  }


  FT_Error
  tt_driver_class_pic_init( FT_Library  library )
  {
    FT_PIC_Container*  pic_container = &library->pic_container;
    FT_Error           error         = FT_Err_Ok;
    TTModulePIC*       container     = NULL;
    FT_Memory          memory        = library->memory;


    /* allocate pointer, clear and set global container pointer */
    if ( FT_ALLOC( container, sizeof ( *container ) ) )
      return error;
    FT_MEM_SET( container, 0, sizeof ( *container ) );
    pic_container->truetype = container;

    /* initialize pointer table - this is how the module usually */
    /* expects this data                                         */
    error = FT_Create_Class_tt_services( library,
                                         &container->tt_services );
    if ( error )
      goto Exit;
#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    FT_Init_Class_tt_service_gx_multi_masters(
      &container->tt_service_gx_multi_masters );
#endif
    FT_Init_Class_tt_service_truetype_glyf(
      &container->tt_service_truetype_glyf );

  Exit:
    if ( error )
      tt_driver_class_pic_free( library );
    return error;
  }

#endif /* FT_CONFIG_OPTION_PIC */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttdriver.c                                                             */
/*                                                                         */
/*    TrueType font driver implementation (body).                          */
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
#include FT_SERVICE_XFREE86_NAME_H

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include FT_MULTIPLE_MASTERS_H
#include FT_SERVICE_MULTIPLE_MASTERS_H
#endif

#include FT_SERVICE_TRUETYPE_ENGINE_H
#include FT_SERVICE_TRUETYPE_GLYF_H
#include FT_SERVICE_PROPERTIES_H
#include FT_TRUETYPE_DRIVER_H

#include "ttdriver.h"
#include "ttgload.h"
#include "ttpload.h"

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include "ttgxvar.h"
#endif

#include "tterrors.h"

#include "ttpic.h"

  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttdriver


  /*
   *  PROPERTY SERVICE
   *
   */
  static FT_Error
  tt_property_set( FT_Module    module,         /* TT_Driver */
                   const char*  property_name,
                   const void*  value )
  {
    FT_Error   error  = FT_Err_Ok;
    TT_Driver  driver = (TT_Driver)module;


    if ( !ft_strcmp( property_name, "interpreter-version" ) )
    {
      FT_UInt*  interpreter_version = (FT_UInt*)value;


#ifndef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      if ( *interpreter_version != TT_INTERPRETER_VERSION_35 )
        error = FT_ERR( Unimplemented_Feature );
      else
#endif
        driver->interpreter_version = *interpreter_version;

      return error;
    }

    FT_TRACE0(( "tt_property_set: missing property `%s'\n",
                property_name ));
    return FT_THROW( Missing_Property );
  }


  static FT_Error
  tt_property_get( FT_Module    module,         /* TT_Driver */
                   const char*  property_name,
                   const void*  value )
  {
    FT_Error   error  = FT_Err_Ok;
    TT_Driver  driver = (TT_Driver)module;

    FT_UInt  interpreter_version = driver->interpreter_version;


    if ( !ft_strcmp( property_name, "interpreter-version" ) )
    {
      FT_UInt*  val = (FT_UInt*)value;


      *val = interpreter_version;

      return error;
    }

    FT_TRACE0(( "tt_property_get: missing property `%s'\n",
                property_name ));
    return FT_THROW( Missing_Property );
  }


  FT_DEFINE_SERVICE_PROPERTIESREC(
    tt_service_properties,
    (FT_Properties_SetFunc)tt_property_set,
    (FT_Properties_GetFunc)tt_property_get )


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
  /*    tt_get_kerning                                                     */
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
  static FT_Error
  tt_get_kerning( FT_Face     ttface,          /* TT_Face */
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

    return 0;
  }


#undef PAIR_TAG


  static FT_Error
  tt_get_advances( FT_Face    ttface,
                   FT_UInt    start,
                   FT_UInt    count,
                   FT_Int32   flags,
                   FT_Fixed  *advances )
  {
    FT_UInt  nn;
    TT_Face  face  = (TT_Face) ttface;


    /* XXX: TODO: check for sbits */

    if ( flags & FT_LOAD_VERTICAL_LAYOUT )
    {
      for ( nn = 0; nn < count; nn++ )
      {
        FT_Short   tsb;
        FT_UShort  ah;


        /* since we don't need `tsb', we use zero for `yMax' parameter */
        TT_Get_VMetrics( face, start + nn, 0, &tsb, &ah );
        advances[nn] = ah;
      }
    }
    else
    {
      for ( nn = 0; nn < count; nn++ )
      {
        FT_Short   lsb;
        FT_UShort  aw;


        TT_Get_HMetrics( face, start + nn, &lsb, &aw );
        advances[nn] = aw;
      }
    }

    return FT_Err_Ok;
  }

  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/
  /****                                                                 ****/
  /****                                                                 ****/
  /****                           S I Z E S                             ****/
  /****                                                                 ****/
  /****                                                                 ****/
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

  static FT_Error
  tt_size_select( FT_Size   size,
                  FT_ULong  strike_index )
  {
    TT_Face   ttface = (TT_Face)size->face;
    TT_Size   ttsize = (TT_Size)size;
    FT_Error  error  = FT_Err_Ok;


    ttsize->strike_index = strike_index;

    if ( FT_IS_SCALABLE( size->face ) )
    {
      /* use the scaled metrics, even when tt_size_reset fails */
      FT_Select_Metrics( size->face, strike_index );

      tt_size_reset( ttsize );
    }
    else
    {
      SFNT_Service      sfnt    = (SFNT_Service) ttface->sfnt;
      FT_Size_Metrics*  metrics = &size->metrics;


      error = sfnt->load_strike_metrics( ttface, strike_index, metrics );
      if ( error )
        ttsize->strike_index = 0xFFFFFFFFUL;
    }

    return error;
  }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */


  static FT_Error
  tt_size_request( FT_Size          size,
                   FT_Size_Request  req )
  {
    TT_Size   ttsize = (TT_Size)size;
    FT_Error  error  = FT_Err_Ok;


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

    if ( FT_HAS_FIXED_SIZES( size->face ) )
    {
      TT_Face       ttface = (TT_Face)size->face;
      SFNT_Service  sfnt   = (SFNT_Service) ttface->sfnt;
      FT_ULong      strike_index;


      error = sfnt->set_sbit_strike( ttface, req, &strike_index );

      if ( error )
        ttsize->strike_index = 0xFFFFFFFFUL;
      else
        return tt_size_select( size, strike_index );
    }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */

    FT_Request_Metrics( size->face, req );

    if ( FT_IS_SCALABLE( size->face ) )
    {
      error = tt_size_reset( ttsize );
      ttsize->root.metrics = ttsize->metrics;
    }

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_glyph_load                                                      */
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
  /*                   FT_LOAD_XXX constants can be used to control the    */
  /*                   glyph loading process (e.g., whether the outline    */
  /*                   should be scaled, whether to load bitmaps or not,   */
  /*                   whether to hint the outline, etc).                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  static FT_Error
  tt_glyph_load( FT_GlyphSlot  ttslot,      /* TT_GlyphSlot */
                 FT_Size       ttsize,      /* TT_Size      */
                 FT_UInt       glyph_index,
                 FT_Int32      load_flags )
  {
    TT_GlyphSlot  slot = (TT_GlyphSlot)ttslot;
    TT_Size       size = (TT_Size)ttsize;
    FT_Face       face = ttslot->face;
    FT_Error      error;


    if ( !slot )
      return FT_THROW( Invalid_Slot_Handle );

    if ( !size )
      return FT_THROW( Invalid_Size_Handle );

    if ( !face )
      return FT_THROW( Invalid_Argument );

#ifdef FT_CONFIG_OPTION_INCREMENTAL
    if ( glyph_index >= (FT_UInt)face->num_glyphs &&
         !face->internal->incremental_interface   )
#else
    if ( glyph_index >= (FT_UInt)face->num_glyphs )
#endif
      return FT_THROW( Invalid_Argument );

    if ( load_flags & FT_LOAD_NO_HINTING )
    {
      /* both FT_LOAD_NO_HINTING and FT_LOAD_NO_AUTOHINT   */
      /* are necessary to disable hinting for tricky fonts */

      if ( FT_IS_TRICKY( face ) )
        load_flags &= ~FT_LOAD_NO_HINTING;

      if ( load_flags & FT_LOAD_NO_AUTOHINT )
        load_flags |= FT_LOAD_NO_HINTING;
    }

    if ( load_flags & ( FT_LOAD_NO_RECURSE | FT_LOAD_NO_SCALE ) )
    {
      load_flags |= FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE;

      if ( !FT_IS_TRICKY( face ) )
        load_flags |= FT_LOAD_NO_HINTING;
    }

    /* now load the glyph outline if necessary */
    error = TT_Load_Glyph( size, slot, glyph_index, load_flags );

    /* force drop-out mode to 2 - irrelevant now */
    /* slot->outline.dropout_mode = 2; */

    return error;
  }


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

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
  FT_DEFINE_SERVICE_MULTIMASTERSREC(
    tt_service_gx_multi_masters,
    (FT_Get_MM_Func)        NULL,
    (FT_Set_MM_Design_Func) NULL,
    (FT_Set_MM_Blend_Func)  TT_Set_MM_Blend,
    (FT_Get_MM_Var_Func)    TT_Get_MM_Var,
    (FT_Set_Var_Design_Func)TT_Set_Var_Design )
#endif

  static const FT_Service_TrueTypeEngineRec  tt_service_truetype_engine =
  {
#ifdef TT_USE_BYTECODE_INTERPRETER

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    FT_TRUETYPE_ENGINE_TYPE_UNPATENTED
#else
    FT_TRUETYPE_ENGINE_TYPE_PATENTED
#endif

#else /* !TT_USE_BYTECODE_INTERPRETER */

    FT_TRUETYPE_ENGINE_TYPE_NONE

#endif /* TT_USE_BYTECODE_INTERPRETER */
  };

  FT_DEFINE_SERVICE_TTGLYFREC(
    tt_service_truetype_glyf,
    (TT_Glyf_GetLocationFunc)tt_face_get_location )

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
  FT_DEFINE_SERVICEDESCREC5(
    tt_services,
    FT_SERVICE_ID_XF86_NAME,       FT_XF86_FORMAT_TRUETYPE,
    FT_SERVICE_ID_MULTI_MASTERS,   &TT_SERVICE_GX_MULTI_MASTERS_GET,
    FT_SERVICE_ID_TRUETYPE_ENGINE, &tt_service_truetype_engine,
    FT_SERVICE_ID_TT_GLYF,         &TT_SERVICE_TRUETYPE_GLYF_GET,
    FT_SERVICE_ID_PROPERTIES,      &TT_SERVICE_PROPERTIES_GET )
#else
  FT_DEFINE_SERVICEDESCREC4(
    tt_services,
    FT_SERVICE_ID_XF86_NAME,       FT_XF86_FORMAT_TRUETYPE,
    FT_SERVICE_ID_TRUETYPE_ENGINE, &tt_service_truetype_engine,
    FT_SERVICE_ID_TT_GLYF,         &TT_SERVICE_TRUETYPE_GLYF_GET,
    FT_SERVICE_ID_PROPERTIES,      &TT_SERVICE_PROPERTIES_GET )
#endif


  FT_CALLBACK_DEF( FT_Module_Interface )
  tt_get_interface( FT_Module    driver,    /* TT_Driver */
                    const char*  tt_interface )
  {
    FT_Library           library;
    FT_Module_Interface  result;
    FT_Module            sfntd;
    SFNT_Service         sfnt;


    /* TT_SERVICES_GET derefers `library' in PIC mode */
#ifdef FT_CONFIG_OPTION_PIC
    if ( !driver )
      return NULL;
    library = driver->library;
    if ( !library )
      return NULL;
#endif

    result = ft_service_list_lookup( TT_SERVICES_GET, tt_interface );
    if ( result != NULL )
      return result;

#ifndef FT_CONFIG_OPTION_PIC
    if ( !driver )
      return NULL;
    library = driver->library;
    if ( !library )
      return NULL;
#endif

    /* only return the default interface from the SFNT module */
    sfntd = FT_Get_Module( library, "sfnt" );
    if ( sfntd )
    {
      sfnt = (SFNT_Service)( sfntd->clazz->module_interface );
      if ( sfnt )
        return sfnt->get_interface( driver, tt_interface );
    }

    return 0;
  }


  /* The FT_DriverInterface structure is defined in ftdriver.h. */

#ifdef TT_USE_BYTECODE_INTERPRETER
#define TT_HINTER_FLAG  FT_MODULE_DRIVER_HAS_HINTER
#else
#define TT_HINTER_FLAG  0
#endif

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
#define TT_SIZE_SELECT  tt_size_select
#else
#define TT_SIZE_SELECT  0
#endif

  FT_DEFINE_DRIVER(
    tt_driver_class,

      FT_MODULE_FONT_DRIVER     |
      FT_MODULE_DRIVER_SCALABLE |
      TT_HINTER_FLAG,

      sizeof ( TT_DriverRec ),

      "truetype",      /* driver name                           */
      0x10000L,        /* driver version == 1.0                 */
      0x20000L,        /* driver requires FreeType 2.0 or above */

      (void*)0,        /* driver specific interface */

      tt_driver_init,
      tt_driver_done,
      tt_get_interface,

    sizeof ( TT_FaceRec ),
    sizeof ( TT_SizeRec ),
    sizeof ( FT_GlyphSlotRec ),

    tt_face_init,
    tt_face_done,
    tt_size_init,
    tt_size_done,
    tt_slot_init,
    0,                       /* FT_Slot_DoneFunc */

    tt_glyph_load,

    tt_get_kerning,
    0,                       /* FT_Face_AttachFunc */
    tt_get_advances,

    tt_size_request,
    TT_SIZE_SELECT
  )


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttpload.c                                                              */
/*                                                                         */
/*    TrueType-specific tables loader (body).                              */
/*                                                                         */
/*  Copyright 1996-2002, 2004-2013 by                                      */
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

#include "ttpload.h"

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include "ttgxvar.h"
#endif

#include "tterrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttpload


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_loca                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the locations table.                                          */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: The input stream.                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_loca( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error  error;
    FT_ULong  table_len;
    FT_Int    shift;


    /* we need the size of the `glyf' table for malformed `loca' tables */
    error = face->goto_table( face, TTAG_glyf, stream, &face->glyf_len );

    /* it is possible that a font doesn't have a glyf table at all */
    /* or its size is zero                                         */
    if ( FT_ERR_EQ( error, Table_Missing ) )
      face->glyf_len = 0;
    else if ( error )
      goto Exit;

    FT_TRACE2(( "Locations " ));
    error = face->goto_table( face, TTAG_loca, stream, &table_len );
    if ( error )
    {
      error = FT_THROW( Locations_Missing );
      goto Exit;
    }

    if ( face->header.Index_To_Loc_Format != 0 )
    {
      shift = 2;

      if ( table_len >= 0x40000L )
      {
        FT_TRACE2(( "table too large\n" ));
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }
      face->num_locations = table_len >> shift;
    }
    else
    {
      shift = 1;

      if ( table_len >= 0x20000L )
      {
        FT_TRACE2(( "table too large\n" ));
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }
      face->num_locations = table_len >> shift;
    }

    if ( face->num_locations != (FT_ULong)face->root.num_glyphs + 1 )
    {
      FT_TRACE2(( "glyph count mismatch!  loca: %d, maxp: %d\n",
                  face->num_locations - 1, face->root.num_glyphs ));

      /* we only handle the case where `maxp' gives a larger value */
      if ( face->num_locations <= (FT_ULong)face->root.num_glyphs )
      {
        FT_Long   new_loca_len =
                    ( (FT_Long)( face->root.num_glyphs ) + 1 ) << shift;

        TT_Table  entry = face->dir_tables;
        TT_Table  limit = entry + face->num_tables;

        FT_Long   pos  = FT_Stream_Pos( stream );
        FT_Long   dist = 0x7FFFFFFFL;


        /* compute the distance to next table in font file */
        for ( ; entry < limit; entry++ )
        {
          FT_Long  diff = entry->Offset - pos;


          if ( diff > 0 && diff < dist )
            dist = diff;
        }

        if ( entry == limit )
        {
          /* `loca' is the last table */
          dist = stream->size - pos;
        }

        if ( new_loca_len <= dist )
        {
          face->num_locations = face->root.num_glyphs + 1;
          table_len           = new_loca_len;

          FT_TRACE2(( "adjusting num_locations to %d\n",
                      face->num_locations ));
        }
      }
    }

    /*
     * Extract the frame.  We don't need to decompress it since
     * we are able to parse it directly.
     */
    if ( FT_FRAME_EXTRACT( table_len, face->glyph_locations ) )
      goto Exit;

    FT_TRACE2(( "loaded\n" ));

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_ULong )
  tt_face_get_location( TT_Face   face,
                        FT_UInt   gindex,
                        FT_UInt  *asize )
  {
    FT_ULong  pos1, pos2;
    FT_Byte*  p;
    FT_Byte*  p_limit;


    pos1 = pos2 = 0;

    if ( gindex < face->num_locations )
    {
      if ( face->header.Index_To_Loc_Format != 0 )
      {
        p       = face->glyph_locations + gindex * 4;
        p_limit = face->glyph_locations + face->num_locations * 4;

        pos1 = FT_NEXT_ULONG( p );
        pos2 = pos1;

        if ( p + 4 <= p_limit )
          pos2 = FT_NEXT_ULONG( p );
      }
      else
      {
        p       = face->glyph_locations + gindex * 2;
        p_limit = face->glyph_locations + face->num_locations * 2;

        pos1 = FT_NEXT_USHORT( p );
        pos2 = pos1;

        if ( p + 2 <= p_limit )
          pos2 = FT_NEXT_USHORT( p );

        pos1 <<= 1;
        pos2 <<= 1;
      }
    }

    /* Check broken location data */
    if ( pos1 > face->glyf_len )
    {
      FT_TRACE1(( "tt_face_get_location:"
                  " too large offset=0x%08lx found for gid=0x%04lx,"
                  " exceeding the end of glyf table (0x%08lx)\n",
                  pos1, gindex, face->glyf_len ));
      *asize = 0;
      return 0;
    }

    if ( pos2 > face->glyf_len )
    {
      FT_TRACE1(( "tt_face_get_location:"
                  " too large offset=0x%08lx found for gid=0x%04lx,"
                  " truncate at the end of glyf table (0x%08lx)\n",
                  pos2, gindex + 1, face->glyf_len ));
      pos2 = face->glyf_len;
    }

    /* The `loca' table must be ordered; it refers to the length of */
    /* an entry as the difference between the current and the next  */
    /* position.  However, there do exist (malformed) fonts which   */
    /* don't obey this rule, so we are only able to provide an      */
    /* upper bound for the size.                                    */
    /*                                                              */
    /* We get (intentionally) a wrong, non-zero result in case the  */
    /* `glyf' table is missing.                                     */
    if ( pos2 >= pos1 )
      *asize = (FT_UInt)( pos2 - pos1 );
    else
      *asize = (FT_UInt)( face->glyf_len - pos1 );

    return pos1;
  }


  FT_LOCAL_DEF( void )
  tt_face_done_loca( TT_Face  face )
  {
    FT_Stream  stream = face->root.stream;


    FT_FRAME_RELEASE( face->glyph_locations );
    face->num_locations = 0;
  }



  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_cvt                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the control value table into a face object.                   */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_cvt( TT_Face    face,
                    FT_Stream  stream )
  {
#ifdef TT_USE_BYTECODE_INTERPRETER

    FT_Error   error;
    FT_Memory  memory = stream->memory;
    FT_ULong   table_len;


    FT_TRACE2(( "CVT " ));

    error = face->goto_table( face, TTAG_cvt, stream, &table_len );
    if ( error )
    {
      FT_TRACE2(( "is missing\n" ));

      face->cvt_size = 0;
      face->cvt      = NULL;
      error          = FT_Err_Ok;

      goto Exit;
    }

    face->cvt_size = table_len / 2;

    if ( FT_NEW_ARRAY( face->cvt, face->cvt_size ) )
      goto Exit;

    if ( FT_FRAME_ENTER( face->cvt_size * 2L ) )
      goto Exit;

    {
      FT_Short*  cur   = face->cvt;
      FT_Short*  limit = cur + face->cvt_size;


      for ( ; cur < limit; cur++ )
        *cur = FT_GET_SHORT();
    }

    FT_FRAME_EXIT();
    FT_TRACE2(( "loaded\n" ));

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    if ( face->doblend )
      error = tt_face_vary_cvt( face, stream );
#endif

  Exit:
    return error;

#else /* !TT_USE_BYTECODE_INTERPRETER */

    FT_UNUSED( face   );
    FT_UNUSED( stream );

    return FT_Err_Ok;

#endif
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_fpgm                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the font program.                                             */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_fpgm( TT_Face    face,
                     FT_Stream  stream )
  {
#ifdef TT_USE_BYTECODE_INTERPRETER

    FT_Error  error;
    FT_ULong  table_len;


    FT_TRACE2(( "Font program " ));

    /* The font program is optional */
    error = face->goto_table( face, TTAG_fpgm, stream, &table_len );
    if ( error )
    {
      face->font_program      = NULL;
      face->font_program_size = 0;
      error                   = FT_Err_Ok;

      FT_TRACE2(( "is missing\n" ));
    }
    else
    {
      face->font_program_size = table_len;
      if ( FT_FRAME_EXTRACT( table_len, face->font_program ) )
        goto Exit;

      FT_TRACE2(( "loaded, %12d bytes\n", face->font_program_size ));
    }

  Exit:
    return error;

#else /* !TT_USE_BYTECODE_INTERPRETER */

    FT_UNUSED( face   );
    FT_UNUSED( stream );

    return FT_Err_Ok;

#endif
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_prep                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the cvt program.                                              */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_prep( TT_Face    face,
                     FT_Stream  stream )
  {
#ifdef TT_USE_BYTECODE_INTERPRETER

    FT_Error  error;
    FT_ULong  table_len;


    FT_TRACE2(( "Prep program " ));

    error = face->goto_table( face, TTAG_prep, stream, &table_len );
    if ( error )
    {
      face->cvt_program      = NULL;
      face->cvt_program_size = 0;
      error                  = FT_Err_Ok;

      FT_TRACE2(( "is missing\n" ));
    }
    else
    {
      face->cvt_program_size = table_len;
      if ( FT_FRAME_EXTRACT( table_len, face->cvt_program ) )
        goto Exit;

      FT_TRACE2(( "loaded, %12d bytes\n", face->cvt_program_size ));
    }

  Exit:
    return error;

#else /* !TT_USE_BYTECODE_INTERPRETER */

    FT_UNUSED( face   );
    FT_UNUSED( stream );

    return FT_Err_Ok;

#endif
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_hdmx                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the `hdmx' table into the face object.                        */
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
  tt_face_load_hdmx( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error   error;
    FT_Memory  memory = stream->memory;
    FT_UInt    version, nn, num_records;
    FT_ULong   table_size, record_size;
    FT_Byte*   p;
    FT_Byte*   limit;


    /* this table is optional */
    error = face->goto_table( face, TTAG_hdmx, stream, &table_size );
    if ( error || table_size < 8 )
      return FT_Err_Ok;

    if ( FT_FRAME_EXTRACT( table_size, face->hdmx_table ) )
      goto Exit;

    p     = face->hdmx_table;
    limit = p + table_size;

    version     = FT_NEXT_USHORT( p );
    num_records = FT_NEXT_USHORT( p );
    record_size = FT_NEXT_ULONG( p );

    /* The maximum number of bytes in an hdmx device record is the */
    /* maximum number of glyphs + 2; this is 0xFFFF + 2; this is   */
    /* the reason why `record_size' is a long (which we read as    */
    /* unsigned long for convenience).  In practice, two bytes     */
    /* sufficient to hold the size value.                          */
    /*                                                             */
    /* There are at least two fonts, HANNOM-A and HANNOM-B version */
    /* 2.0 (2005), which get this wrong: The upper two bytes of    */
    /* the size value are set to 0xFF instead of 0x00.  We catch   */
    /* and fix this.                                               */

    if ( record_size >= 0xFFFF0000UL )
      record_size &= 0xFFFFU;

    /* The limit for `num_records' is a heuristic value. */

    if ( version != 0 || num_records > 255 || record_size > 0x10001L )
    {
      error = FT_THROW( Invalid_File_Format );
      goto Fail;
    }

    if ( FT_NEW_ARRAY( face->hdmx_record_sizes, num_records ) )
      goto Fail;

    for ( nn = 0; nn < num_records; nn++ )
    {
      if ( p + record_size > limit )
        break;

      face->hdmx_record_sizes[nn] = p[0];
      p                          += record_size;
    }

    face->hdmx_record_count = nn;
    face->hdmx_table_size   = table_size;
    face->hdmx_record_size  = record_size;

  Exit:
    return error;

  Fail:
    FT_FRAME_RELEASE( face->hdmx_table );
    face->hdmx_table_size = 0;
    goto Exit;
  }


  FT_LOCAL_DEF( void )
  tt_face_free_hdmx( TT_Face  face )
  {
    FT_Stream  stream = face->root.stream;
    FT_Memory  memory = stream->memory;


    FT_FREE( face->hdmx_record_sizes );
    FT_FRAME_RELEASE( face->hdmx_table );
  }


  /*************************************************************************/
  /*                                                                       */
  /* Return the advance width table for a given pixel size if it is found  */
  /* in the font's `hdmx' table (if any).                                  */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Byte* )
  tt_face_get_device_metrics( TT_Face  face,
                              FT_UInt  ppem,
                              FT_UInt  gindex )
  {
    FT_UInt   nn;
    FT_Byte*  result      = NULL;
    FT_ULong  record_size = face->hdmx_record_size;
    FT_Byte*  record      = face->hdmx_table + 8;


    for ( nn = 0; nn < face->hdmx_record_count; nn++ )
      if ( face->hdmx_record_sizes[nn] == ppem )
      {
        gindex += 2;
        if ( gindex < record_size )
          result = record + nn * record_size + gindex;
        break;
      }

    return result;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttgload.c                                                              */
/*                                                                         */
/*    TrueType Glyph Loader (body).                                        */
/*                                                                         */
/*  Copyright 1996-2013                                                    */
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
#include FT_INTERNAL_SFNT_H
#include FT_TRUETYPE_TAGS_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_DRIVER_H

#include "ttgload.h"
#include "ttpload.h"

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include "ttgxvar.h"
#endif

#include "tterrors.h"
#include "ttsubpix.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttgload


  /*************************************************************************/
  /*                                                                       */
  /* Composite glyph flags.                                                */
  /*                                                                       */
#define ARGS_ARE_WORDS             0x0001
#define ARGS_ARE_XY_VALUES         0x0002
#define ROUND_XY_TO_GRID           0x0004
#define WE_HAVE_A_SCALE            0x0008
/* reserved                        0x0010 */
#define MORE_COMPONENTS            0x0020
#define WE_HAVE_AN_XY_SCALE        0x0040
#define WE_HAVE_A_2X2              0x0080
#define WE_HAVE_INSTR              0x0100
#define USE_MY_METRICS             0x0200
#define OVERLAP_COMPOUND           0x0400
#define SCALED_COMPONENT_OFFSET    0x0800
#define UNSCALED_COMPONENT_OFFSET  0x1000


  /*************************************************************************/
  /*                                                                       */
  /* Return the horizontal metrics in font units for a given glyph.        */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  TT_Get_HMetrics( TT_Face     face,
                   FT_UInt     idx,
                   FT_Short*   lsb,
                   FT_UShort*  aw )
  {
    ( (SFNT_Service)face->sfnt )->get_metrics( face, 0, idx, lsb, aw );

    FT_TRACE5(( "  advance width (font units): %d\n", *aw ));
    FT_TRACE5(( "  left side bearing (font units): %d\n", *lsb ));
  }


  /*************************************************************************/
  /*                                                                       */
  /* Return the vertical metrics in font units for a given glyph.          */
  /* See macro `TT_LOADER_SET_PP' below for explanations.                  */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  TT_Get_VMetrics( TT_Face     face,
                   FT_UInt     idx,
                   FT_Pos      yMax,
                   FT_Short*   tsb,
                   FT_UShort*  ah )
  {
    if ( face->vertical_info )
      ( (SFNT_Service)face->sfnt )->get_metrics( face, 1, idx, tsb, ah );

    else if ( face->os2.version != 0xFFFFU )
    {
      *tsb = (FT_Short)(face->os2.sTypoAscender - yMax);
      *ah  = face->os2.sTypoAscender - face->os2.sTypoDescender;
    }

    else
    {
      *tsb = (FT_Short)(face->horizontal.Ascender - yMax);
      *ah  = face->horizontal.Ascender - face->horizontal.Descender;
    }

    FT_TRACE5(( "  advance height (font units): %d\n", *ah ));
    FT_TRACE5(( "  top side bearing (font units): %d\n", *tsb ));
  }


  static FT_Error
  tt_get_metrics( TT_Loader  loader,
                  FT_UInt    glyph_index )
  {
    TT_Face    face   = (TT_Face)loader->face;
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    TT_Driver  driver = (TT_Driver)FT_FACE_DRIVER( face );
#endif

    FT_Error   error;
    FT_Stream  stream = loader->stream;

    FT_Short   left_bearing = 0, top_bearing = 0;
    FT_UShort  advance_width = 0, advance_height = 0;

    /* we must preserve the stream position          */
    /* (which gets altered by the metrics functions) */
    FT_ULong  pos = FT_STREAM_POS();


    TT_Get_HMetrics( face, glyph_index,
                     &left_bearing,
                     &advance_width );
    TT_Get_VMetrics( face, glyph_index,
                     loader->bbox.yMax,
                     &top_bearing,
                     &advance_height );

    if ( FT_STREAM_SEEK( pos ) )
      return error;

    loader->left_bearing = left_bearing;
    loader->advance      = advance_width;
    loader->top_bearing  = top_bearing;
    loader->vadvance     = advance_height;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( driver->interpreter_version == TT_INTERPRETER_VERSION_38 )
    {
      if ( loader->exec )
        loader->exec->sph_tweak_flags = 0;

      /* this may not be the right place for this, but it works */
      if ( loader->exec && loader->exec->ignore_x_mode )
        sph_set_tweaks( loader, glyph_index );
    }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    if ( !loader->linear_def )
    {
      loader->linear_def = 1;
      loader->linear     = advance_width;
    }

    return FT_Err_Ok;
  }


#ifdef FT_CONFIG_OPTION_INCREMENTAL

  static void
  tt_get_metrics_incr_overrides( TT_Loader  loader,
                                 FT_UInt    glyph_index )
  {
    TT_Face  face = (TT_Face)loader->face;

    FT_Short   left_bearing = 0, top_bearing = 0;
    FT_UShort  advance_width = 0, advance_height = 0;


    /* If this is an incrementally loaded font check whether there are */
    /* overriding metrics for this glyph.                              */
    if ( face->root.internal->incremental_interface                           &&
         face->root.internal->incremental_interface->funcs->get_glyph_metrics )
    {
      FT_Incremental_MetricsRec  metrics;
      FT_Error                   error;


      metrics.bearing_x = loader->left_bearing;
      metrics.bearing_y = 0;
      metrics.advance   = loader->advance;
      metrics.advance_v = 0;

      error = face->root.internal->incremental_interface->funcs->get_glyph_metrics(
                face->root.internal->incremental_interface->object,
                glyph_index, FALSE, &metrics );
      if ( error )
        goto Exit;

      left_bearing  = (FT_Short)metrics.bearing_x;
      advance_width = (FT_UShort)metrics.advance;

#if 0

      /* GWW: Do I do the same for vertical metrics? */
      metrics.bearing_x = 0;
      metrics.bearing_y = loader->top_bearing;
      metrics.advance   = loader->vadvance;

      error = face->root.internal->incremental_interface->funcs->get_glyph_metrics(
                face->root.internal->incremental_interface->object,
                glyph_index, TRUE, &metrics );
      if ( error )
        goto Exit;

      top_bearing    = (FT_Short)metrics.bearing_y;
      advance_height = (FT_UShort)metrics.advance;

#endif /* 0 */

      loader->left_bearing = left_bearing;
      loader->advance      = advance_width;
      loader->top_bearing  = top_bearing;
      loader->vadvance     = advance_height;

      if ( !loader->linear_def )
      {
        loader->linear_def = 1;
        loader->linear     = advance_width;
      }
    }

  Exit:
    return;
  }

#endif /* FT_CONFIG_OPTION_INCREMENTAL */


  /*************************************************************************/
  /*                                                                       */
  /* Translates an array of coordinates.                                   */
  /*                                                                       */
  static void
  translate_array( FT_UInt     n,
                   FT_Vector*  coords,
                   FT_Pos      delta_x,
                   FT_Pos      delta_y )
  {
    FT_UInt  k;


    if ( delta_x )
      for ( k = 0; k < n; k++ )
        coords[k].x += delta_x;

    if ( delta_y )
      for ( k = 0; k < n; k++ )
        coords[k].y += delta_y;
  }


  /*************************************************************************/
  /*                                                                       */
  /* The following functions are used by default with TrueType fonts.      */
  /* However, they can be replaced by alternatives if we need to support   */
  /* TrueType-compressed formats (like MicroType) in the future.           */
  /*                                                                       */
  /*************************************************************************/

  FT_CALLBACK_DEF( FT_Error )
  TT_Access_Glyph_Frame( TT_Loader  loader,
                         FT_UInt    glyph_index,
                         FT_ULong   offset,
                         FT_UInt    byte_count )
  {
    FT_Error   error;
    FT_Stream  stream = loader->stream;

    /* for non-debug mode */
    FT_UNUSED( glyph_index );


    FT_TRACE4(( "Glyph %ld\n", glyph_index ));

    /* the following line sets the `error' variable through macros! */
    if ( FT_STREAM_SEEK( offset ) || FT_FRAME_ENTER( byte_count ) )
      return error;

    loader->cursor = stream->cursor;
    loader->limit  = stream->limit;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( void )
  TT_Forget_Glyph_Frame( TT_Loader  loader )
  {
    FT_Stream  stream = loader->stream;


    FT_FRAME_EXIT();
  }


  FT_CALLBACK_DEF( FT_Error )
  TT_Load_Glyph_Header( TT_Loader  loader )
  {
    FT_Byte*  p     = loader->cursor;
    FT_Byte*  limit = loader->limit;


    if ( p + 10 > limit )
      return FT_THROW( Invalid_Outline );

    loader->n_contours = FT_NEXT_SHORT( p );

    loader->bbox.xMin = FT_NEXT_SHORT( p );
    loader->bbox.yMin = FT_NEXT_SHORT( p );
    loader->bbox.xMax = FT_NEXT_SHORT( p );
    loader->bbox.yMax = FT_NEXT_SHORT( p );

    FT_TRACE5(( "  # of contours: %d\n", loader->n_contours ));
    FT_TRACE5(( "  xMin: %4d  xMax: %4d\n", loader->bbox.xMin,
                                            loader->bbox.xMax ));
    FT_TRACE5(( "  yMin: %4d  yMax: %4d\n", loader->bbox.yMin,
                                            loader->bbox.yMax ));
    loader->cursor = p;

    return FT_Err_Ok;
  }


  FT_CALLBACK_DEF( FT_Error )
  TT_Load_Simple_Glyph( TT_Loader  load )
  {
    FT_Error        error;
    FT_Byte*        p          = load->cursor;
    FT_Byte*        limit      = load->limit;
    FT_GlyphLoader  gloader    = load->gloader;
    FT_Int          n_contours = load->n_contours;
    FT_Outline*     outline;
    FT_UShort       n_ins;
    FT_Int          n_points;
    FT_ULong        tmp;

    FT_Byte         *flag, *flag_limit;
    FT_Byte         c, count;
    FT_Vector       *vec, *vec_limit;
    FT_Pos          x;
    FT_Short        *cont, *cont_limit, prev_cont;
    FT_Int          xy_size = 0;


    /* check that we can add the contours to the glyph */
    error = FT_GLYPHLOADER_CHECK_POINTS( gloader, 0, n_contours );
    if ( error )
      goto Fail;

    /* reading the contours' endpoints & number of points */
    cont       = gloader->current.outline.contours;
    cont_limit = cont + n_contours;

    /* check space for contours array + instructions count */
    if ( n_contours >= 0xFFF || p + ( n_contours + 1 ) * 2 > limit )
      goto Invalid_Outline;

    prev_cont = FT_NEXT_SHORT( p );

    if ( n_contours > 0 )
      cont[0] = prev_cont;

    if ( prev_cont < 0 )
      goto Invalid_Outline;

    for ( cont++; cont < cont_limit; cont++ )
    {
      cont[0] = FT_NEXT_SHORT( p );
      if ( cont[0] <= prev_cont )
      {
        /* unordered contours: this is invalid */
        goto Invalid_Outline;
      }
      prev_cont = cont[0];
    }

    n_points = 0;
    if ( n_contours > 0 )
    {
      n_points = cont[-1] + 1;
      if ( n_points < 0 )
        goto Invalid_Outline;
    }

    /* note that we will add four phantom points later */
    error = FT_GLYPHLOADER_CHECK_POINTS( gloader, n_points + 4, 0 );
    if ( error )
      goto Fail;

    /* reading the bytecode instructions */
    load->glyph->control_len  = 0;
    load->glyph->control_data = 0;

    if ( p + 2 > limit )
      goto Invalid_Outline;

    n_ins = FT_NEXT_USHORT( p );

    FT_TRACE5(( "  Instructions size: %u\n", n_ins ));

    /* check it */
    if ( ( limit - p ) < n_ins )
    {
      FT_TRACE0(( "TT_Load_Simple_Glyph: instruction count mismatch\n" ));
      error = FT_THROW( Too_Many_Hints );
      goto Fail;
    }

#ifdef TT_USE_BYTECODE_INTERPRETER

    if ( IS_HINTED( load->load_flags ) )
    {
      /* we don't trust `maxSizeOfInstructions' in the `maxp' table */
      /* and thus update the bytecode array size by ourselves       */

      tmp   = load->exec->glyphSize;
      error = Update_Max( load->exec->memory,
                          &tmp,
                          sizeof ( FT_Byte ),
                          (void*)&load->exec->glyphIns,
                          n_ins );

      load->exec->glyphSize = (FT_UShort)tmp;
      if ( error )
        return error;

      load->glyph->control_len  = n_ins;
      load->glyph->control_data = load->exec->glyphIns;

      FT_MEM_COPY( load->exec->glyphIns, p, (FT_Long)n_ins );
    }

#endif /* TT_USE_BYTECODE_INTERPRETER */

    p += n_ins;

    outline = &gloader->current.outline;

    /* reading the point tags */
    flag       = (FT_Byte*)outline->tags;
    flag_limit = flag + n_points;

    FT_ASSERT( flag != NULL );

    while ( flag < flag_limit )
    {
      if ( p + 1 > limit )
        goto Invalid_Outline;

      *flag++ = c = FT_NEXT_BYTE( p );
      if ( c & 8 )
      {
        if ( p + 1 > limit )
          goto Invalid_Outline;

        count = FT_NEXT_BYTE( p );
        if ( flag + (FT_Int)count > flag_limit )
          goto Invalid_Outline;

        for ( ; count > 0; count-- )
          *flag++ = c;
      }
    }

    /* reading the X coordinates */

    vec       = outline->points;
    vec_limit = vec + n_points;
    flag      = (FT_Byte*)outline->tags;
    x         = 0;

    if ( p + xy_size > limit )
      goto Invalid_Outline;

    for ( ; vec < vec_limit; vec++, flag++ )
    {
      FT_Pos   y = 0;
      FT_Byte  f = *flag;


      if ( f & 2 )
      {
        if ( p + 1 > limit )
          goto Invalid_Outline;

        y = (FT_Pos)FT_NEXT_BYTE( p );
        if ( ( f & 16 ) == 0 )
          y = -y;
      }
      else if ( ( f & 16 ) == 0 )
      {
        if ( p + 2 > limit )
          goto Invalid_Outline;

        y = (FT_Pos)FT_NEXT_SHORT( p );
      }

      x     += y;
      vec->x = x;
      /* the cast is for stupid compilers */
      *flag  = (FT_Byte)( f & ~( 2 | 16 ) );
    }

    /* reading the Y coordinates */

    vec       = gloader->current.outline.points;
    vec_limit = vec + n_points;
    flag      = (FT_Byte*)outline->tags;
    x         = 0;

    for ( ; vec < vec_limit; vec++, flag++ )
    {
      FT_Pos   y = 0;
      FT_Byte  f = *flag;


      if ( f & 4 )
      {
        if ( p + 1 > limit )
          goto Invalid_Outline;

        y = (FT_Pos)FT_NEXT_BYTE( p );
        if ( ( f & 32 ) == 0 )
          y = -y;
      }
      else if ( ( f & 32 ) == 0 )
      {
        if ( p + 2 > limit )
          goto Invalid_Outline;

        y = (FT_Pos)FT_NEXT_SHORT( p );
      }

      x     += y;
      vec->y = x;
      /* the cast is for stupid compilers */
      *flag  = (FT_Byte)( f & FT_CURVE_TAG_ON );
    }

    outline->n_points   = (FT_UShort)n_points;
    outline->n_contours = (FT_Short) n_contours;

    load->cursor = p;

  Fail:
    return error;

  Invalid_Outline:
    error = FT_THROW( Invalid_Outline );
    goto Fail;
  }


  FT_CALLBACK_DEF( FT_Error )
  TT_Load_Composite_Glyph( TT_Loader  loader )
  {
    FT_Error        error;
    FT_Byte*        p       = loader->cursor;
    FT_Byte*        limit   = loader->limit;
    FT_GlyphLoader  gloader = loader->gloader;
    FT_SubGlyph     subglyph;
    FT_UInt         num_subglyphs;


    num_subglyphs = 0;

    do
    {
      FT_Fixed  xx, xy, yy, yx;
      FT_UInt   count;


      /* check that we can load a new subglyph */
      error = FT_GlyphLoader_CheckSubGlyphs( gloader, num_subglyphs + 1 );
      if ( error )
        goto Fail;

      /* check space */
      if ( p + 4 > limit )
        goto Invalid_Composite;

      subglyph = gloader->current.subglyphs + num_subglyphs;

      subglyph->arg1 = subglyph->arg2 = 0;

      subglyph->flags = FT_NEXT_USHORT( p );
      subglyph->index = FT_NEXT_USHORT( p );

      /* check space */
      count = 2;
      if ( subglyph->flags & ARGS_ARE_WORDS )
        count += 2;
      if ( subglyph->flags & WE_HAVE_A_SCALE )
        count += 2;
      else if ( subglyph->flags & WE_HAVE_AN_XY_SCALE )
        count += 4;
      else if ( subglyph->flags & WE_HAVE_A_2X2 )
        count += 8;

      if ( p + count > limit )
        goto Invalid_Composite;

      /* read arguments */
      if ( subglyph->flags & ARGS_ARE_WORDS )
      {
        subglyph->arg1 = FT_NEXT_SHORT( p );
        subglyph->arg2 = FT_NEXT_SHORT( p );
      }
      else
      {
        subglyph->arg1 = FT_NEXT_CHAR( p );
        subglyph->arg2 = FT_NEXT_CHAR( p );
      }

      /* read transform */
      xx = yy = 0x10000L;
      xy = yx = 0;

      if ( subglyph->flags & WE_HAVE_A_SCALE )
      {
        xx = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
        yy = xx;
      }
      else if ( subglyph->flags & WE_HAVE_AN_XY_SCALE )
      {
        xx = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
        yy = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
      }
      else if ( subglyph->flags & WE_HAVE_A_2X2 )
      {
        xx = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
        yx = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
        xy = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
        yy = (FT_Fixed)FT_NEXT_SHORT( p ) << 2;
      }

      subglyph->transform.xx = xx;
      subglyph->transform.xy = xy;
      subglyph->transform.yx = yx;
      subglyph->transform.yy = yy;

      num_subglyphs++;

    } while ( subglyph->flags & MORE_COMPONENTS );

    gloader->current.num_subglyphs = num_subglyphs;

#ifdef TT_USE_BYTECODE_INTERPRETER

    {
      FT_Stream  stream = loader->stream;


      /* we must undo the FT_FRAME_ENTER in order to point */
      /* to the composite instructions, if we find some.   */
      /* We will process them later.                       */
      /*                                                   */
      loader->ins_pos = (FT_ULong)( FT_STREAM_POS() +
                                    p - limit );
    }

#endif

    loader->cursor = p;

  Fail:
    return error;

  Invalid_Composite:
    error = FT_THROW( Invalid_Composite );
    goto Fail;
  }


  FT_LOCAL_DEF( void )
  TT_Init_Glyph_Loading( TT_Face  face )
  {
    face->access_glyph_frame   = TT_Access_Glyph_Frame;
    face->read_glyph_header    = TT_Load_Glyph_Header;
    face->read_simple_glyph    = TT_Load_Simple_Glyph;
    face->read_composite_glyph = TT_Load_Composite_Glyph;
    face->forget_glyph_frame   = TT_Forget_Glyph_Frame;
  }


  static void
  tt_prepare_zone( TT_GlyphZone  zone,
                   FT_GlyphLoad  load,
                   FT_UInt       start_point,
                   FT_UInt       start_contour )
  {
    zone->n_points    = (FT_UShort)( load->outline.n_points - start_point );
    zone->n_contours  = (FT_Short) ( load->outline.n_contours -
                                       start_contour );
    zone->org         = load->extra_points + start_point;
    zone->cur         = load->outline.points + start_point;
    zone->orus        = load->extra_points2 + start_point;
    zone->tags        = (FT_Byte*)load->outline.tags + start_point;
    zone->contours    = (FT_UShort*)load->outline.contours + start_contour;
    zone->first_point = (FT_UShort)start_point;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Hint_Glyph                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Hint the glyph using the zone prepared by the caller.  Note that   */
  /*    the zone is supposed to include four phantom points.               */
  /*                                                                       */
  static FT_Error
  TT_Hint_Glyph( TT_Loader  loader,
                 FT_Bool    is_composite )
  {
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    TT_Face    face   = (TT_Face)loader->face;
    TT_Driver  driver = (TT_Driver)FT_FACE_DRIVER( face );
#endif

    TT_GlyphZone  zone = &loader->zone;

#ifdef TT_USE_BYTECODE_INTERPRETER
    FT_UInt       n_ins;
#else
    FT_UNUSED( is_composite );
#endif


#ifdef TT_USE_BYTECODE_INTERPRETER
    if ( loader->glyph->control_len > 0xFFFFL )
    {
      FT_TRACE1(( "TT_Hint_Glyph: too long instructions" ));
      FT_TRACE1(( " (0x%lx byte) is truncated\n",
                 loader->glyph->control_len ));
    }
    n_ins = (FT_UInt)( loader->glyph->control_len );

    /* save original point position in org */
    if ( n_ins > 0 )
      FT_ARRAY_COPY( zone->org, zone->cur, zone->n_points );

    /* Reset graphics state. */
    loader->exec->GS = ((TT_Size)loader->size)->GS;

    /* XXX: UNDOCUMENTED! Hinting instructions of a composite glyph */
    /*      completely refer to the (already) hinted subglyphs.     */
    if ( is_composite )
    {
      loader->exec->metrics.x_scale = 1 << 16;
      loader->exec->metrics.y_scale = 1 << 16;

      FT_ARRAY_COPY( zone->orus, zone->cur, zone->n_points );
    }
    else
    {
      loader->exec->metrics.x_scale =
        ((TT_Size)loader->size)->metrics.x_scale;
      loader->exec->metrics.y_scale =
        ((TT_Size)loader->size)->metrics.y_scale;
    }
#endif

    /* round phantom points */
    zone->cur[zone->n_points - 4].x =
      FT_PIX_ROUND( zone->cur[zone->n_points - 4].x );
    zone->cur[zone->n_points - 3].x =
      FT_PIX_ROUND( zone->cur[zone->n_points - 3].x );
    zone->cur[zone->n_points - 2].y =
      FT_PIX_ROUND( zone->cur[zone->n_points - 2].y );
    zone->cur[zone->n_points - 1].y =
      FT_PIX_ROUND( zone->cur[zone->n_points - 1].y );

#ifdef TT_USE_BYTECODE_INTERPRETER

    if ( n_ins > 0 )
    {
      FT_Bool   debug;
      FT_Error  error;

      FT_GlyphLoader  gloader         = loader->gloader;
      FT_Outline      current_outline = gloader->current.outline;


      error = TT_Set_CodeRange( loader->exec, tt_coderange_glyph,
                                loader->exec->glyphIns, n_ins );
      if ( error )
        return error;

      loader->exec->is_composite = is_composite;
      loader->exec->pts          = *zone;

      debug = FT_BOOL( !( loader->load_flags & FT_LOAD_NO_SCALE ) &&
                       ((TT_Size)loader->size)->debug             );

      error = TT_Run_Context( loader->exec, debug );
      if ( error && loader->exec->pedantic_hinting )
        return error;

      /* store drop-out mode in bits 5-7; set bit 2 also as a marker */
      current_outline.tags[0] |=
        ( loader->exec->GS.scan_type << 5 ) | FT_CURVE_TAG_HAS_SCANMODE;
    }

#endif

    /* save glyph phantom points */
    loader->pp1 = zone->cur[zone->n_points - 4];
    loader->pp2 = zone->cur[zone->n_points - 3];
    loader->pp3 = zone->cur[zone->n_points - 2];
    loader->pp4 = zone->cur[zone->n_points - 1];

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( driver->interpreter_version == TT_INTERPRETER_VERSION_38 )
    {
      if ( loader->exec->sph_tweak_flags & SPH_TWEAK_DEEMBOLDEN )
        FT_Outline_EmboldenXY( &loader->gloader->current.outline, -24, 0 );

      else if ( loader->exec->sph_tweak_flags & SPH_TWEAK_EMBOLDEN )
        FT_Outline_EmboldenXY( &loader->gloader->current.outline, 24, 0 );
    }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Process_Simple_Glyph                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Once a simple glyph has been loaded, it needs to be processed.     */
  /*    Usually, this means scaling and hinting through bytecode           */
  /*    interpretation.                                                    */
  /*                                                                       */
  static FT_Error
  TT_Process_Simple_Glyph( TT_Loader  loader )
  {
    FT_GlyphLoader  gloader = loader->gloader;
    FT_Error        error   = FT_Err_Ok;
    FT_Outline*     outline;
    FT_Int          n_points;


    outline  = &gloader->current.outline;
    n_points = outline->n_points;

    /* set phantom points */

    outline->points[n_points    ] = loader->pp1;
    outline->points[n_points + 1] = loader->pp2;
    outline->points[n_points + 2] = loader->pp3;
    outline->points[n_points + 3] = loader->pp4;

    outline->tags[n_points    ] = 0;
    outline->tags[n_points + 1] = 0;
    outline->tags[n_points + 2] = 0;
    outline->tags[n_points + 3] = 0;

    n_points += 4;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT

    if ( ((TT_Face)loader->face)->doblend )
    {
      /* Deltas apply to the unscaled data. */
      FT_Vector*  deltas;
      FT_Memory   memory = loader->face->memory;
      FT_Int      i;


      error = TT_Vary_Get_Glyph_Deltas( (TT_Face)(loader->face),
                                        loader->glyph_index,
                                        &deltas,
                                        n_points );
      if ( error )
        return error;

      for ( i = 0; i < n_points; ++i )
      {
        outline->points[i].x += deltas[i].x;
        outline->points[i].y += deltas[i].y;
      }

      FT_FREE( deltas );
    }

#endif /* TT_CONFIG_OPTION_GX_VAR_SUPPORT */

    if ( IS_HINTED( loader->load_flags ) )
    {
      tt_prepare_zone( &loader->zone, &gloader->current, 0, 0 );

      FT_ARRAY_COPY( loader->zone.orus, loader->zone.cur,
                     loader->zone.n_points + 4 );
    }

    {
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      TT_Face    face   = (TT_Face)loader->face;
      TT_Driver  driver = (TT_Driver)FT_FACE_DRIVER( face );

      FT_String*  family         = face->root.family_name;
      FT_Int      ppem           = loader->size->metrics.x_ppem;
      FT_String*  style          = face->root.style_name;
      FT_Int      x_scale_factor = 1000;
#endif

      FT_Vector*  vec   = outline->points;
      FT_Vector*  limit = outline->points + n_points;

      FT_Fixed  x_scale = 0; /* pacify compiler */
      FT_Fixed  y_scale = 0;

      FT_Bool  do_scale = FALSE;


#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

      if ( driver->interpreter_version == TT_INTERPRETER_VERSION_38 )
      {
        /* scale, but only if enabled and only if TT hinting is being used */
        if ( IS_HINTED( loader->load_flags ) )
          x_scale_factor = sph_test_tweak_x_scaling( face,
                                                     family,
                                                     ppem,
                                                     style,
                                                     loader->glyph_index );
        /* scale the glyph */
        if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 ||
             x_scale_factor != 1000                         )
        {
          x_scale = FT_MulDiv( ((TT_Size)loader->size)->metrics.x_scale,
                               x_scale_factor, 1000 );
          y_scale = ((TT_Size)loader->size)->metrics.y_scale;

          /* compensate for any scaling by de/emboldening; */
          /* the amount was determined via experimentation */
          if ( x_scale_factor != 1000 && ppem > 11 )
            FT_Outline_EmboldenXY( outline,
                                   FT_MulFix( 1280 * ppem,
                                              1000 - x_scale_factor ),
                                   0 );
          do_scale = TRUE;
        }
      }
      else

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      {
        /* scale the glyph */
        if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
        {
          x_scale = ((TT_Size)loader->size)->metrics.x_scale;
          y_scale = ((TT_Size)loader->size)->metrics.y_scale;

          do_scale = TRUE;
        }
      }

      if ( do_scale )
      {
        for ( ; vec < limit; vec++ )
        {
          vec->x = FT_MulFix( vec->x, x_scale );
          vec->y = FT_MulFix( vec->y, y_scale );
        }

        loader->pp1 = outline->points[n_points - 4];
        loader->pp2 = outline->points[n_points - 3];
        loader->pp3 = outline->points[n_points - 2];
        loader->pp4 = outline->points[n_points - 1];
      }
    }

    if ( IS_HINTED( loader->load_flags ) )
    {
      loader->zone.n_points += 4;

      error = TT_Hint_Glyph( loader, 0 );
    }

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Process_Composite_Component                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Once a composite component has been loaded, it needs to be         */
  /*    processed.  Usually, this means transforming and translating.      */
  /*                                                                       */
  static FT_Error
  TT_Process_Composite_Component( TT_Loader    loader,
                                  FT_SubGlyph  subglyph,
                                  FT_UInt      start_point,
                                  FT_UInt      num_base_points )
  {
    FT_GlyphLoader  gloader    = loader->gloader;
    FT_Vector*      base_vec   = gloader->base.outline.points;
    FT_UInt         num_points = gloader->base.outline.n_points;
    FT_Bool         have_scale;
    FT_Pos          x, y;


    have_scale = FT_BOOL( subglyph->flags & ( WE_HAVE_A_SCALE     |
                                              WE_HAVE_AN_XY_SCALE |
                                              WE_HAVE_A_2X2       ) );

    /* perform the transform required for this subglyph */
    if ( have_scale )
    {
      FT_UInt  i;


      for ( i = num_base_points; i < num_points; i++ )
        FT_Vector_Transform( base_vec + i, &subglyph->transform );
    }

    /* get offset */
    if ( !( subglyph->flags & ARGS_ARE_XY_VALUES ) )
    {
      FT_UInt     k = subglyph->arg1;
      FT_UInt     l = subglyph->arg2;
      FT_Vector*  p1;
      FT_Vector*  p2;


      /* match l-th point of the newly loaded component to the k-th point */
      /* of the previously loaded components.                             */

      /* change to the point numbers used by our outline */
      k += start_point;
      l += num_base_points;
      if ( k >= num_base_points ||
           l >= num_points      )
        return FT_THROW( Invalid_Composite );

      p1 = gloader->base.outline.points + k;
      p2 = gloader->base.outline.points + l;

      x = p1->x - p2->x;
      y = p1->y - p2->y;
    }
    else
    {
      x = subglyph->arg1;
      y = subglyph->arg2;

      if ( !x && !y )
        return FT_Err_Ok;

      /* Use a default value dependent on                                  */
      /* TT_CONFIG_OPTION_COMPONENT_OFFSET_SCALED.  This is useful for old */
      /* TT fonts which don't set the xxx_COMPONENT_OFFSET bit.            */

      if ( have_scale &&
#ifdef TT_CONFIG_OPTION_COMPONENT_OFFSET_SCALED
           !( subglyph->flags & UNSCALED_COMPONENT_OFFSET ) )
#else
            ( subglyph->flags & SCALED_COMPONENT_OFFSET ) )
#endif
      {

#if 0

        /*******************************************************************/
        /*                                                                 */
        /* This algorithm is what Apple documents.  But it doesn't work.   */
        /*                                                                 */
        int  a = subglyph->transform.xx > 0 ?  subglyph->transform.xx
                                            : -subglyph->transform.xx;
        int  b = subglyph->transform.yx > 0 ?  subglyph->transform.yx
                                            : -subglyph->transform.yx;
        int  c = subglyph->transform.xy > 0 ?  subglyph->transform.xy
                                            : -subglyph->transform.xy;
        int  d = subglyph->transform.yy > 0 ? subglyph->transform.yy
                                            : -subglyph->transform.yy;
        int  m = a > b ? a : b;
        int  n = c > d ? c : d;


        if ( a - b <= 33 && a - b >= -33 )
          m *= 2;
        if ( c - d <= 33 && c - d >= -33 )
          n *= 2;
        x = FT_MulFix( x, m );
        y = FT_MulFix( y, n );

#else /* 1 */

        /*******************************************************************/
        /*                                                                 */
        /* This algorithm is a guess and works much better than the above. */
        /*                                                                 */
        FT_Fixed  mac_xscale = FT_Hypot( subglyph->transform.xx,
                                         subglyph->transform.xy );
        FT_Fixed  mac_yscale = FT_Hypot( subglyph->transform.yy,
                                         subglyph->transform.yx );


        x = FT_MulFix( x, mac_xscale );
        y = FT_MulFix( y, mac_yscale );

#endif /* 1 */

      }

      if ( !( loader->load_flags & FT_LOAD_NO_SCALE ) )
      {
        FT_Fixed  x_scale = ((TT_Size)loader->size)->metrics.x_scale;
        FT_Fixed  y_scale = ((TT_Size)loader->size)->metrics.y_scale;


        x = FT_MulFix( x, x_scale );
        y = FT_MulFix( y, y_scale );

        if ( subglyph->flags & ROUND_XY_TO_GRID )
        {
          x = FT_PIX_ROUND( x );
          y = FT_PIX_ROUND( y );
        }
      }
    }

    if ( x || y )
      translate_array( num_points - num_base_points,
                       base_vec + num_base_points,
                       x, y );

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Process_Composite_Glyph                                         */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This is slightly different from TT_Process_Simple_Glyph, in that   */
  /*    its sole purpose is to hint the glyph.  Thus this function is      */
  /*    only available when bytecode interpreter is enabled.               */
  /*                                                                       */
  static FT_Error
  TT_Process_Composite_Glyph( TT_Loader  loader,
                              FT_UInt    start_point,
                              FT_UInt    start_contour )
  {
    FT_Error     error;
    FT_Outline*  outline;
    FT_UInt      i;


    outline = &loader->gloader->base.outline;

    /* make room for phantom points */
    error = FT_GLYPHLOADER_CHECK_POINTS( loader->gloader,
                                         outline->n_points + 4,
                                         0 );
    if ( error )
      return error;

    outline->points[outline->n_points    ] = loader->pp1;
    outline->points[outline->n_points + 1] = loader->pp2;
    outline->points[outline->n_points + 2] = loader->pp3;
    outline->points[outline->n_points + 3] = loader->pp4;

    outline->tags[outline->n_points    ] = 0;
    outline->tags[outline->n_points + 1] = 0;
    outline->tags[outline->n_points + 2] = 0;
    outline->tags[outline->n_points + 3] = 0;

#ifdef TT_USE_BYTECODE_INTERPRETER

    {
      FT_Stream  stream = loader->stream;
      FT_UShort  n_ins, max_ins;
      FT_ULong   tmp;


      /* TT_Load_Composite_Glyph only gives us the offset of instructions */
      /* so we read them here                                             */
      if ( FT_STREAM_SEEK( loader->ins_pos ) ||
           FT_READ_USHORT( n_ins )           )
        return error;

      FT_TRACE5(( "  Instructions size = %d\n", n_ins ));

      /* check it */
      max_ins = ((TT_Face)loader->face)->max_profile.maxSizeOfInstructions;
      if ( n_ins > max_ins )
      {
        /* don't trust `maxSizeOfInstructions'; */
        /* only do a rough safety check         */
        if ( (FT_Int)n_ins > loader->byte_len )
        {
          FT_TRACE1(( "TT_Process_Composite_Glyph:"
                      " too many instructions (%d) for glyph with length %d\n",
                      n_ins, loader->byte_len ));
          return FT_THROW( Too_Many_Hints );
        }

        tmp   = loader->exec->glyphSize;
        error = Update_Max( loader->exec->memory,
                            &tmp,
                            sizeof ( FT_Byte ),
                            (void*)&loader->exec->glyphIns,
                            n_ins );

        loader->exec->glyphSize = (FT_UShort)tmp;
        if ( error )
          return error;
      }
      else if ( n_ins == 0 )
        return FT_Err_Ok;

      if ( FT_STREAM_READ( loader->exec->glyphIns, n_ins ) )
        return error;

      loader->glyph->control_data = loader->exec->glyphIns;
      loader->glyph->control_len  = n_ins;
    }

#endif

    tt_prepare_zone( &loader->zone, &loader->gloader->base,
                     start_point, start_contour );

    /* Some points are likely touched during execution of  */
    /* instructions on components.  So let's untouch them. */
    for ( i = 0; i < loader->zone.n_points; i++ )
      loader->zone.tags[i] &= ~FT_CURVE_TAG_TOUCH_BOTH;

    loader->zone.n_points += 4;

    return TT_Hint_Glyph( loader, 1 );
  }


  /*
   * Calculate the phantom points
   *
   * Defining the right side bearing (rsb) as
   *
   *   rsb = aw - (lsb + xmax - xmin)
   *
   * (with `aw' the advance width, `lsb' the left side bearing, and `xmin'
   * and `xmax' the glyph's minimum and maximum x value), the OpenType
   * specification defines the initial position of horizontal phantom points
   * as
   *
   *   pp1 = (round(xmin - lsb), 0)      ,
   *   pp2 = (round(pp1 + aw), 0)        .
   *
   * Note that the rounding to the grid (in the device space) is not
   * documented currently in the specification.
   *
   * However, the specification lacks the precise definition of vertical
   * phantom points.  Greg Hitchcock provided the following explanation.
   *
   * - a `vmtx' table is present
   *
   *   For any glyph, the minimum and maximum y values (`ymin' and `ymax')
   *   are given in the `glyf' table, the top side bearing (tsb) and advance
   *   height (ah) are given in the `vmtx' table.  The bottom side bearing
   *   (bsb) is then calculated as
   *
   *     bsb = ah - (tsb + ymax - ymin)       ,
   *
   *   and the initial position of vertical phantom points is
   *
   *     pp3 = (x, round(ymax + tsb))       ,
   *     pp4 = (x, round(pp3 - ah))         .
   *
   *   See below for value `x'.
   *
   * - no `vmtx' table in the font
   *
   *   If there is an `OS/2' table, we set
   *
   *     DefaultAscender = sTypoAscender       ,
   *     DefaultDescender = sTypoDescender     ,
   *
   *   otherwise we use data from the `hhea' table:
   *
   *     DefaultAscender = Ascender         ,
   *     DefaultDescender = Descender       .
   *
   *   With these two variables we can now set
   *
   *     ah = DefaultAscender - sDefaultDescender    ,
   *     tsb = DefaultAscender - yMax                ,
   *
   *   and proceed as if a `vmtx' table was present.
   *
   * Usually we have
   *
   *   x = aw / 2      ,                                                (1)
   *
   * but there is one compatibility case where it can be set to
   *
   *   x = -DefaultDescender -
   *         ((DefaultAscender - DefaultDescender - aw) / 2)     .      (2)
   *
   * and another one with
   *
   *   x = 0     .                                                      (3)
   *
   * In Windows, the history of those values is quite complicated,
   * depending on the hinting engine (that is, the graphics framework).
   *
   *   framework        from                 to       formula
   *  ----------------------------------------------------------
   *    GDI       Windows 98               current      (1)
   *              (Windows 2000 for NT)
   *    GDI+      Windows XP               Windows 7    (2)
   *    GDI+      Windows 8                current      (3)
   *    DWrite    Windows 7                current      (3)
   *
   * For simplicity, FreeType uses (1) for grayscale subpixel hinting and
   * (3) for everything else.
   *
   */
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

#define TT_LOADER_SET_PP( loader )                                          \
          do                                                                \
          {                                                                 \
            FT_Bool  subpixel_  = loader->exec ? loader->exec->subpixel     \
                                               : 0;                         \
            FT_Bool  grayscale_ = loader->exec ? loader->exec->grayscale    \
                                               : 0;                         \
            FT_Bool  use_aw_2_  = (FT_Bool)( subpixel_ && grayscale_ );     \
                                                                            \
                                                                            \
            (loader)->pp1.x = (loader)->bbox.xMin - (loader)->left_bearing; \
            (loader)->pp1.y = 0;                                            \
            (loader)->pp2.x = (loader)->pp1.x + (loader)->advance;          \
            (loader)->pp2.y = 0;                                            \
                                                                            \
            (loader)->pp3.x = use_aw_2_ ? (loader)->advance / 2 : 0;        \
            (loader)->pp3.y = (loader)->bbox.yMax + (loader)->top_bearing;  \
            (loader)->pp4.x = use_aw_2_ ? (loader)->advance / 2 : 0;        \
            (loader)->pp4.y = (loader)->pp3.y - (loader)->vadvance;         \
          } while ( 0 )

#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */

#define TT_LOADER_SET_PP( loader )                                          \
          do                                                                \
          {                                                                 \
            (loader)->pp1.x = (loader)->bbox.xMin - (loader)->left_bearing; \
            (loader)->pp1.y = 0;                                            \
            (loader)->pp2.x = (loader)->pp1.x + (loader)->advance;          \
            (loader)->pp2.y = 0;                                            \
                                                                            \
            (loader)->pp3.x = 0;                                            \
            (loader)->pp3.y = (loader)->bbox.yMax + (loader)->top_bearing;  \
            (loader)->pp4.x = 0;                                            \
            (loader)->pp4.y = (loader)->pp3.y - (loader)->vadvance;         \
          } while ( 0 )

#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    load_truetype_glyph                                                */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads a given truetype glyph.  Handles composites and uses a       */
  /*    TT_Loader object.                                                  */
  /*                                                                       */
  static FT_Error
  load_truetype_glyph( TT_Loader  loader,
                       FT_UInt    glyph_index,
                       FT_UInt    recurse_count,
                       FT_Bool    header_only )
  {
    FT_Error        error        = FT_Err_Ok;
    FT_Fixed        x_scale, y_scale;
    FT_ULong        offset;
    TT_Face         face         = (TT_Face)loader->face;
    FT_GlyphLoader  gloader      = loader->gloader;
    FT_Bool         opened_frame = 0;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    FT_Vector*      deltas       = NULL;
#endif

#ifdef FT_CONFIG_OPTION_INCREMENTAL
    FT_StreamRec    inc_stream;
    FT_Data         glyph_data;
    FT_Bool         glyph_data_loaded = 0;
#endif


    /* some fonts have an incorrect value of `maxComponentDepth', */
    /* thus we allow depth 1 to catch the majority of them        */
    if ( recurse_count > 1                                   &&
         recurse_count > face->max_profile.maxComponentDepth )
    {
      error = FT_THROW( Invalid_Composite );
      goto Exit;
    }

    /* check glyph index */
    if ( glyph_index >= (FT_UInt)face->root.num_glyphs )
    {
      error = FT_THROW( Invalid_Glyph_Index );
      goto Exit;
    }

    loader->glyph_index = glyph_index;

    if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
    {
      x_scale = ((TT_Size)loader->size)->metrics.x_scale;
      y_scale = ((TT_Size)loader->size)->metrics.y_scale;
    }
    else
    {
      x_scale = 0x10000L;
      y_scale = 0x10000L;
    }

    /* Set `offset' to the start of the glyph relative to the start of */
    /* the `glyf' table, and `byte_len' to the length of the glyph in  */
    /* bytes.                                                          */

#ifdef FT_CONFIG_OPTION_INCREMENTAL

    /* If we are loading glyph data via the incremental interface, set */
    /* the loader stream to a memory stream reading the data returned  */
    /* by the interface.                                               */
    if ( face->root.internal->incremental_interface )
    {
      error = face->root.internal->incremental_interface->funcs->get_glyph_data(
                face->root.internal->incremental_interface->object,
                glyph_index, &glyph_data );
      if ( error )
        goto Exit;

      glyph_data_loaded = 1;
      offset            = 0;
      loader->byte_len  = glyph_data.length;

      FT_MEM_ZERO( &inc_stream, sizeof ( inc_stream ) );
      FT_Stream_OpenMemory( &inc_stream,
                            glyph_data.pointer, glyph_data.length );

      loader->stream = &inc_stream;
    }
    else

#endif /* FT_CONFIG_OPTION_INCREMENTAL */

      offset = tt_face_get_location( face, glyph_index,
                                     (FT_UInt*)&loader->byte_len );

    if ( loader->byte_len > 0 )
    {
#ifdef FT_CONFIG_OPTION_INCREMENTAL
      /* for the incremental interface, `glyf_offset' is always zero */
      if ( !loader->glyf_offset                        &&
           !face->root.internal->incremental_interface )
#else
      if ( !loader->glyf_offset )
#endif /* FT_CONFIG_OPTION_INCREMENTAL */
      {
        FT_TRACE2(( "no `glyf' table but non-zero `loca' entry\n" ));
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      error = face->access_glyph_frame( loader, glyph_index,
                                        loader->glyf_offset + offset,
                                        loader->byte_len );
      if ( error )
        goto Exit;

      opened_frame = 1;

      /* read glyph header first */
      error = face->read_glyph_header( loader );
      if ( error )
        goto Exit;

      /* the metrics must be computed after loading the glyph header */
      /* since we need the glyph's `yMax' value in case the vertical */
      /* metrics must be emulated                                    */
      error = tt_get_metrics( loader, glyph_index );
      if ( error )
        goto Exit;

      if ( header_only )
        goto Exit;
    }

    if ( loader->byte_len == 0 || loader->n_contours == 0 )
    {
      loader->bbox.xMin = 0;
      loader->bbox.xMax = 0;
      loader->bbox.yMin = 0;
      loader->bbox.yMax = 0;

      error = tt_get_metrics( loader, glyph_index );
      if ( error )
        goto Exit;

      if ( header_only )
        goto Exit;

      /* must initialize points before (possibly) overriding */
      /* glyph metrics from the incremental interface        */
      TT_LOADER_SET_PP( loader );

#ifdef FT_CONFIG_OPTION_INCREMENTAL
      tt_get_metrics_incr_overrides( loader, glyph_index );
#endif

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT

      if ( ((TT_Face)(loader->face))->doblend )
      {
        /* this must be done before scaling */
        FT_Memory  memory = loader->face->memory;


        error = TT_Vary_Get_Glyph_Deltas( (TT_Face)(loader->face),
                                          glyph_index, &deltas, 4 );
        if ( error )
          goto Exit;

        loader->pp1.x += deltas[0].x;
        loader->pp1.y += deltas[0].y;
        loader->pp2.x += deltas[1].x;
        loader->pp2.y += deltas[1].y;

        loader->pp3.x += deltas[2].x;
        loader->pp3.y += deltas[2].y;
        loader->pp4.x += deltas[3].x;
        loader->pp4.y += deltas[3].y;

        FT_FREE( deltas );
      }

#endif /* TT_CONFIG_OPTION_GX_VAR_SUPPORT */

      /* scale phantom points, if necessary; */
      /* they get rounded in `TT_Hint_Glyph' */
      if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
      {
        loader->pp1.x = FT_MulFix( loader->pp1.x, x_scale );
        loader->pp2.x = FT_MulFix( loader->pp2.x, x_scale );
        /* pp1.y and pp2.y are always zero */

        loader->pp3.x = FT_MulFix( loader->pp3.x, x_scale );
        loader->pp3.y = FT_MulFix( loader->pp3.y, y_scale );
        loader->pp4.x = FT_MulFix( loader->pp4.x, x_scale );
        loader->pp4.y = FT_MulFix( loader->pp4.y, y_scale );
      }

      error = FT_Err_Ok;
      goto Exit;
    }

    /* must initialize phantom points before (possibly) overriding */
    /* glyph metrics from the incremental interface                */
    TT_LOADER_SET_PP( loader );

#ifdef FT_CONFIG_OPTION_INCREMENTAL
    tt_get_metrics_incr_overrides( loader, glyph_index );
#endif

    /***********************************************************************/
    /***********************************************************************/
    /***********************************************************************/

    /* if it is a simple glyph, load it */

    if ( loader->n_contours > 0 )
    {
      error = face->read_simple_glyph( loader );
      if ( error )
        goto Exit;

      /* all data have been read */
      face->forget_glyph_frame( loader );
      opened_frame = 0;

      error = TT_Process_Simple_Glyph( loader );
      if ( error )
        goto Exit;

      FT_GlyphLoader_Add( gloader );
    }

    /***********************************************************************/
    /***********************************************************************/
    /***********************************************************************/

    /* otherwise, load a composite! */
    else if ( loader->n_contours == -1 )
    {
      FT_UInt   start_point;
      FT_UInt   start_contour;
      FT_ULong  ins_pos;  /* position of composite instructions, if any */


      start_point   = gloader->base.outline.n_points;
      start_contour = gloader->base.outline.n_contours;

      /* for each subglyph, read composite header */
      error = face->read_composite_glyph( loader );
      if ( error )
        goto Exit;

      /* store the offset of instructions */
      ins_pos = loader->ins_pos;

      /* all data we need are read */
      face->forget_glyph_frame( loader );
      opened_frame = 0;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT

      if ( face->doblend )
      {
        FT_Int       i, limit;
        FT_SubGlyph  subglyph;
        FT_Memory    memory = face->root.memory;


        /* this provides additional offsets */
        /* for each component's translation */

        if ( ( error = TT_Vary_Get_Glyph_Deltas(
                         face,
                         glyph_index,
                         &deltas,
                         gloader->current.num_subglyphs + 4 ) ) != 0 )
          goto Exit;

        subglyph = gloader->current.subglyphs + gloader->base.num_subglyphs;
        limit    = gloader->current.num_subglyphs;

        for ( i = 0; i < limit; ++i, ++subglyph )
        {
          if ( subglyph->flags & ARGS_ARE_XY_VALUES )
          {
            /* XXX: overflow check for subglyph->{arg1,arg2}.   */
            /* deltas[i].{x,y} must be within signed 16-bit,    */
            /* but the restriction of summed delta is not clear */
            subglyph->arg1 += (FT_Int16)deltas[i].x;
            subglyph->arg2 += (FT_Int16)deltas[i].y;
          }
        }

        loader->pp1.x += deltas[i + 0].x;
        loader->pp1.y += deltas[i + 0].y;
        loader->pp2.x += deltas[i + 1].x;
        loader->pp2.y += deltas[i + 1].y;

        loader->pp3.x += deltas[i + 2].x;
        loader->pp3.y += deltas[i + 2].y;
        loader->pp4.x += deltas[i + 3].x;
        loader->pp4.y += deltas[i + 3].y;

        FT_FREE( deltas );
      }

#endif /* TT_CONFIG_OPTION_GX_VAR_SUPPORT */

      /* scale phantom points, if necessary; */
      /* they get rounded in `TT_Hint_Glyph' */
      if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
      {
        loader->pp1.x = FT_MulFix( loader->pp1.x, x_scale );
        loader->pp2.x = FT_MulFix( loader->pp2.x, x_scale );
        /* pp1.y and pp2.y are always zero */

        loader->pp3.x = FT_MulFix( loader->pp3.x, x_scale );
        loader->pp3.y = FT_MulFix( loader->pp3.y, y_scale );
        loader->pp4.x = FT_MulFix( loader->pp4.x, x_scale );
        loader->pp4.y = FT_MulFix( loader->pp4.y, y_scale );
      }

      /* if the flag FT_LOAD_NO_RECURSE is set, we return the subglyph */
      /* `as is' in the glyph slot (the client application will be     */
      /* responsible for interpreting these data)...                   */
      if ( loader->load_flags & FT_LOAD_NO_RECURSE )
      {
        FT_GlyphLoader_Add( gloader );
        loader->glyph->format = FT_GLYPH_FORMAT_COMPOSITE;

        goto Exit;
      }

      /*********************************************************************/
      /*********************************************************************/
      /*********************************************************************/

      {
        FT_UInt      n, num_base_points;
        FT_SubGlyph  subglyph       = 0;

        FT_UInt      num_points     = start_point;
        FT_UInt      num_subglyphs  = gloader->current.num_subglyphs;
        FT_UInt      num_base_subgs = gloader->base.num_subglyphs;

        FT_Stream    old_stream     = loader->stream;
        FT_Int       old_byte_len   = loader->byte_len;


        FT_GlyphLoader_Add( gloader );

        /* read each subglyph independently */
        for ( n = 0; n < num_subglyphs; n++ )
        {
          FT_Vector  pp[4];


          /* Each time we call load_truetype_glyph in this loop, the   */
          /* value of `gloader.base.subglyphs' can change due to table */
          /* reallocations.  We thus need to recompute the subglyph    */
          /* pointer on each iteration.                                */
          subglyph = gloader->base.subglyphs + num_base_subgs + n;

          pp[0] = loader->pp1;
          pp[1] = loader->pp2;
          pp[2] = loader->pp3;
          pp[3] = loader->pp4;

          num_base_points = gloader->base.outline.n_points;

          error = load_truetype_glyph( loader, subglyph->index,
                                       recurse_count + 1, FALSE );
          if ( error )
            goto Exit;

          /* restore subglyph pointer */
          subglyph = gloader->base.subglyphs + num_base_subgs + n;

          /* restore phantom points if necessary */
          if ( !( subglyph->flags & USE_MY_METRICS ) )
          {
            loader->pp1 = pp[0];
            loader->pp2 = pp[1];
            loader->pp3 = pp[2];
            loader->pp4 = pp[3];
          }

          num_points = gloader->base.outline.n_points;

          if ( num_points == num_base_points )
            continue;

          /* gloader->base.outline consists of three parts:               */
          /* 0 -(1)-> start_point -(2)-> num_base_points -(3)-> n_points. */
          /*                                                              */
          /* (1): exists from the beginning                               */
          /* (2): components that have been loaded so far                 */
          /* (3): the newly loaded component                              */
          TT_Process_Composite_Component( loader, subglyph, start_point,
                                          num_base_points );
        }

        loader->stream   = old_stream;
        loader->byte_len = old_byte_len;

        /* process the glyph */
        loader->ins_pos = ins_pos;
        if ( IS_HINTED( loader->load_flags ) &&

#ifdef TT_USE_BYTECODE_INTERPRETER

             subglyph->flags & WE_HAVE_INSTR &&

#endif

             num_points > start_point )
          TT_Process_Composite_Glyph( loader, start_point, start_contour );

      }
    }
    else
    {
      /* invalid composite count (negative but not -1) */
      error = FT_THROW( Invalid_Outline );
      goto Exit;
    }

    /***********************************************************************/
    /***********************************************************************/
    /***********************************************************************/

  Exit:

    if ( opened_frame )
      face->forget_glyph_frame( loader );

#ifdef FT_CONFIG_OPTION_INCREMENTAL

    if ( glyph_data_loaded )
      face->root.internal->incremental_interface->funcs->free_glyph_data(
        face->root.internal->incremental_interface->object,
        &glyph_data );

#endif

    return error;
  }


  static FT_Error
  compute_glyph_metrics( TT_Loader  loader,
                         FT_UInt    glyph_index )
  {
    TT_Face    face   = (TT_Face)loader->face;
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    TT_Driver  driver = (TT_Driver)FT_FACE_DRIVER( face );
#endif

    FT_BBox       bbox;
    FT_Fixed      y_scale;
    TT_GlyphSlot  glyph = loader->glyph;
    TT_Size       size  = (TT_Size)loader->size;


    y_scale = 0x10000L;
    if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
      y_scale = size->root.metrics.y_scale;

    if ( glyph->format != FT_GLYPH_FORMAT_COMPOSITE )
      FT_Outline_Get_CBox( &glyph->outline, &bbox );
    else
      bbox = loader->bbox;

    /* get the device-independent horizontal advance; it is scaled later */
    /* by the base layer.                                                */
    glyph->linearHoriAdvance = loader->linear;

    glyph->metrics.horiBearingX = bbox.xMin;
    glyph->metrics.horiBearingY = bbox.yMax;
    glyph->metrics.horiAdvance  = loader->pp2.x - loader->pp1.x;

    /* adjust advance width to the value contained in the hdmx table */
    if ( !face->postscript.isFixedPitch  &&
         IS_HINTED( loader->load_flags ) )
    {
      FT_Byte*  widthp;


      widthp = tt_face_get_device_metrics( face,
                                           size->root.metrics.x_ppem,
                                           glyph_index );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

      if ( driver->interpreter_version == TT_INTERPRETER_VERSION_38 )
      {
        FT_Bool  ignore_x_mode;


        ignore_x_mode = FT_BOOL( FT_LOAD_TARGET_MODE( loader->load_flags ) !=
                                 FT_RENDER_MODE_MONO );

        if ( widthp                                                   &&
             ( ( ignore_x_mode && loader->exec->compatible_widths ) ||
                !ignore_x_mode                                      ||
                SPH_OPTION_BITMAP_WIDTHS                            ) )
          glyph->metrics.horiAdvance = *widthp << 6;
      }
      else

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      {
        if ( widthp )
          glyph->metrics.horiAdvance = *widthp << 6;
      }
    }

    /* set glyph dimensions */
    glyph->metrics.width  = bbox.xMax - bbox.xMin;
    glyph->metrics.height = bbox.yMax - bbox.yMin;

    /* Now take care of vertical metrics.  In the case where there is */
    /* no vertical information within the font (relatively common),   */
    /* create some metrics manually                                   */
    {
      FT_Pos  top;      /* scaled vertical top side bearing  */
      FT_Pos  advance;  /* scaled vertical advance height    */


      /* Get the unscaled top bearing and advance height. */
      if ( face->vertical_info                   &&
           face->vertical.number_Of_VMetrics > 0 )
      {
        top = (FT_Short)FT_DivFix( loader->pp3.y - bbox.yMax,
                                   y_scale );

        if ( loader->pp3.y <= loader->pp4.y )
          advance = 0;
        else
          advance = (FT_UShort)FT_DivFix( loader->pp3.y - loader->pp4.y,
                                          y_scale );
      }
      else
      {
        FT_Pos  height;


        /* XXX Compute top side bearing and advance height in  */
        /*     Get_VMetrics instead of here.                   */

        /* NOTE: The OS/2 values are the only `portable' ones, */
        /*       which is why we use them, if there is an OS/2 */
        /*       table in the font.  Otherwise, we use the     */
        /*       values defined in the horizontal header.      */

        height = (FT_Short)FT_DivFix( bbox.yMax - bbox.yMin,
                                      y_scale );
        if ( face->os2.version != 0xFFFFU )
          advance = (FT_Pos)( face->os2.sTypoAscender -
                              face->os2.sTypoDescender );
        else
          advance = (FT_Pos)( face->horizontal.Ascender -
                              face->horizontal.Descender );

        top = ( advance - height ) / 2;
      }

#ifdef FT_CONFIG_OPTION_INCREMENTAL
      {
        FT_Incremental_InterfaceRec*  incr;
        FT_Incremental_MetricsRec     metrics;
        FT_Error                      error;


        incr = face->root.internal->incremental_interface;

        /* If this is an incrementally loaded font see if there are */
        /* overriding metrics for this glyph.                       */
        if ( incr && incr->funcs->get_glyph_metrics )
        {
          metrics.bearing_x = 0;
          metrics.bearing_y = top;
          metrics.advance   = advance;

          error = incr->funcs->get_glyph_metrics( incr->object,
                                                  glyph_index,
                                                  TRUE,
                                                  &metrics );
          if ( error )
            return error;

          top     = metrics.bearing_y;
          advance = metrics.advance;
        }
      }

      /* GWW: Do vertical metrics get loaded incrementally too? */

#endif /* FT_CONFIG_OPTION_INCREMENTAL */

      glyph->linearVertAdvance = advance;

      /* scale the metrics */
      if ( !( loader->load_flags & FT_LOAD_NO_SCALE ) )
      {
        top     = FT_MulFix( top,     y_scale );
        advance = FT_MulFix( advance, y_scale );
      }

      /* XXX: for now, we have no better algorithm for the lsb, but it */
      /*      should work fine.                                        */
      /*                                                               */
      glyph->metrics.vertBearingX = glyph->metrics.horiBearingX -
                                      glyph->metrics.horiAdvance / 2;
      glyph->metrics.vertBearingY = top;
      glyph->metrics.vertAdvance  = advance;
    }

    return 0;
  }


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

  static FT_Error
  load_sbit_image( TT_Size       size,
                   TT_GlyphSlot  glyph,
                   FT_UInt       glyph_index,
                   FT_Int32      load_flags )
  {
    TT_Face             face;
    SFNT_Service        sfnt;
    FT_Stream           stream;
    FT_Error            error;
    TT_SBit_MetricsRec  metrics;


    face   = (TT_Face)glyph->face;
    sfnt   = (SFNT_Service)face->sfnt;
    stream = face->root.stream;

    error = sfnt->load_sbit_image( face,
                                   size->strike_index,
                                   glyph_index,
                                   (FT_Int)load_flags,
                                   stream,
                                   &glyph->bitmap,
                                   &metrics );
    if ( !error )
    {
      glyph->outline.n_points   = 0;
      glyph->outline.n_contours = 0;

      glyph->metrics.width  = (FT_Pos)metrics.width  << 6;
      glyph->metrics.height = (FT_Pos)metrics.height << 6;

      glyph->metrics.horiBearingX = (FT_Pos)metrics.horiBearingX << 6;
      glyph->metrics.horiBearingY = (FT_Pos)metrics.horiBearingY << 6;
      glyph->metrics.horiAdvance  = (FT_Pos)metrics.horiAdvance  << 6;

      glyph->metrics.vertBearingX = (FT_Pos)metrics.vertBearingX << 6;
      glyph->metrics.vertBearingY = (FT_Pos)metrics.vertBearingY << 6;
      glyph->metrics.vertAdvance  = (FT_Pos)metrics.vertAdvance  << 6;

      glyph->format = FT_GLYPH_FORMAT_BITMAP;

      if ( load_flags & FT_LOAD_VERTICAL_LAYOUT )
      {
        glyph->bitmap_left = metrics.vertBearingX;
        glyph->bitmap_top  = metrics.vertBearingY;
      }
      else
      {
        glyph->bitmap_left = metrics.horiBearingX;
        glyph->bitmap_top  = metrics.horiBearingY;
      }
    }

    return error;
  }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */


  static FT_Error
  tt_loader_init( TT_Loader     loader,
                  TT_Size       size,
                  TT_GlyphSlot  glyph,
                  FT_Int32      load_flags,
                  FT_Bool       glyf_table_only )
  {
    TT_Face    face;
    FT_Stream  stream;
#ifdef TT_USE_BYTECODE_INTERPRETER
    FT_Bool    pedantic = FT_BOOL( load_flags & FT_LOAD_PEDANTIC );
#endif


    face   = (TT_Face)glyph->face;
    stream = face->root.stream;

    FT_MEM_ZERO( loader, sizeof ( TT_LoaderRec ) );

#ifdef TT_USE_BYTECODE_INTERPRETER

    /* load execution context */
    if ( IS_HINTED( load_flags ) && !glyf_table_only )
    {
      TT_ExecContext  exec;
      FT_Bool         grayscale;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      TT_Driver  driver = (TT_Driver)FT_FACE_DRIVER( face );

      FT_Bool  subpixel = FALSE;

#if 0
      /* not used yet */
      FT_Bool  compatible_widths;
      FT_Bool  symmetrical_smoothing;
      FT_Bool  bgr;
      FT_Bool  subpixel_positioned;
#endif
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      FT_Bool  reexecute = FALSE;


      if ( !size->cvt_ready )
      {
        FT_Error  error = tt_size_ready_bytecode( size, pedantic );


        if ( error )
          return error;
      }

      /* query new execution context */
      exec = size->debug ? size->context
                         : ( (TT_Driver)FT_FACE_DRIVER( face ) )->context;
      if ( !exec )
        return FT_THROW( Could_Not_Find_Context );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

      if ( driver->interpreter_version == TT_INTERPRETER_VERSION_38 )
      {
        subpixel = FT_BOOL( ( FT_LOAD_TARGET_MODE( load_flags ) !=
                              FT_RENDER_MODE_MONO               )  &&
                            SPH_OPTION_SET_SUBPIXEL                );

        if ( subpixel )
          grayscale = FALSE;
        else if ( SPH_OPTION_SET_GRAYSCALE )
        {
          grayscale = TRUE;
          subpixel  = FALSE;
        }
        else
          grayscale = FALSE;

        if ( FT_IS_TRICKY( glyph->face ) )
          subpixel = FALSE;

        exec->ignore_x_mode      = subpixel || grayscale;
        exec->rasterizer_version = SPH_OPTION_SET_RASTERIZER_VERSION;
        if ( exec->sph_tweak_flags & SPH_TWEAK_RASTERIZER_35 )
          exec->rasterizer_version = TT_INTERPRETER_VERSION_35;

#if 1
        exec->compatible_widths     = SPH_OPTION_SET_COMPATIBLE_WIDTHS;
        exec->symmetrical_smoothing = FALSE;
        exec->bgr                   = FALSE;
        exec->subpixel_positioned   = TRUE;
#else /* 0 */
        exec->compatible_widths =
          FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
                   TT_LOAD_COMPATIBLE_WIDTHS );
        exec->symmetrical_smoothing =
          FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
                   TT_LOAD_SYMMETRICAL_SMOOTHING );
        exec->bgr =
          FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
                   TT_LOAD_BGR );
        exec->subpixel_positioned =
          FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
                   TT_LOAD_SUBPIXEL_POSITIONED );
#endif /* 0 */

      }
      else

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      {
        grayscale = FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
                             FT_RENDER_MODE_MONO );
      }

      TT_Load_Context( exec, face, size );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

      if ( driver->interpreter_version == TT_INTERPRETER_VERSION_38 )
      {
        /* a change from mono to subpixel rendering (and vice versa) */
        /* requires a re-execution of the CVT program                */
        if ( subpixel != exec->subpixel )
        {
          FT_TRACE4(( "tt_loader_init: subpixel hinting change,"
                      " re-executing `prep' table\n" ));

          exec->subpixel = subpixel;
          reexecute      = TRUE;
        }

        /* a change from mono to grayscale rendering (and vice versa) */
        /* requires a re-execution of the CVT program                 */
        if ( grayscale != exec->grayscale )
        {
          FT_TRACE4(( "tt_loader_init: grayscale hinting change,"
                      " re-executing `prep' table\n" ));

          exec->grayscale = grayscale;
          reexecute       = TRUE;
        }
      }
      else

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      {
        /* a change from mono to grayscale rendering (and vice versa) */
        /* requires a re-execution of the CVT program                 */
        if ( grayscale != exec->grayscale )
        {
          FT_TRACE4(( "tt_loader_init: grayscale change,"
                      " re-executing `prep' table\n" ));

          exec->grayscale = grayscale;
          reexecute       = TRUE;
        }
      }

      if ( reexecute )
      {
        FT_UInt  i;


        for ( i = 0; i < size->cvt_size; i++ )
          size->cvt[i] = FT_MulFix( face->cvt[i], size->ttmetrics.scale );
        tt_size_run_prep( size, pedantic );
      }

      /* see whether the cvt program has disabled hinting */
      if ( exec->GS.instruct_control & 1 )
        load_flags |= FT_LOAD_NO_HINTING;

      /* load default graphics state -- if needed */
      if ( exec->GS.instruct_control & 2 )
        exec->GS = tt_default_graphics_state;

      exec->pedantic_hinting = FT_BOOL( load_flags & FT_LOAD_PEDANTIC );
      loader->exec = exec;
      loader->instructions = exec->glyphIns;
    }

#endif /* TT_USE_BYTECODE_INTERPRETER */

    /* seek to the beginning of the glyph table -- for Type 42 fonts     */
    /* the table might be accessed from a Postscript stream or something */
    /* else...                                                           */

#ifdef FT_CONFIG_OPTION_INCREMENTAL

    if ( face->root.internal->incremental_interface )
      loader->glyf_offset = 0;
    else

#endif

    {
      FT_Error  error = face->goto_table( face, TTAG_glyf, stream, 0 );


      if ( FT_ERR_EQ( error, Table_Missing ) )
        loader->glyf_offset = 0;
      else if ( error )
      {
        FT_ERROR(( "tt_loader_init: could not access glyph table\n" ));
        return error;
      }
      else
        loader->glyf_offset = FT_STREAM_POS();
    }

    /* get face's glyph loader */
    if ( !glyf_table_only )
    {
      FT_GlyphLoader  gloader = glyph->internal->loader;


      FT_GlyphLoader_Rewind( gloader );
      loader->gloader = gloader;
    }

    loader->load_flags = load_flags;

    loader->face   = (FT_Face)face;
    loader->size   = (FT_Size)size;
    loader->glyph  = (FT_GlyphSlot)glyph;
    loader->stream = stream;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Load_Glyph                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A function used to load a single glyph within a given glyph slot,  */
  /*    for a given size.                                                  */
  /*                                                                       */
  /* <Input>                                                               */
  /*    glyph       :: A handle to a target slot object where the glyph    */
  /*                   will be loaded.                                     */
  /*                                                                       */
  /*    size        :: A handle to the source face size at which the glyph */
  /*                   must be scaled/loaded.                              */
  /*                                                                       */
  /*    glyph_index :: The index of the glyph in the font file.            */
  /*                                                                       */
  /*    load_flags  :: A flag indicating what to load for this glyph.  The */
  /*                   FT_LOAD_XXX constants can be used to control the    */
  /*                   glyph loading process (e.g., whether the outline    */
  /*                   should be scaled, whether to load bitmaps or not,   */
  /*                   whether to hint the outline, etc).                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Load_Glyph( TT_Size       size,
                 TT_GlyphSlot  glyph,
                 FT_UInt       glyph_index,
                 FT_Int32      load_flags )
  {
    FT_Error      error;
    TT_LoaderRec  loader;


    FT_TRACE1(( "TT_Load_Glyph: glyph index %d\n", glyph_index ));

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

    /* try to load embedded bitmap if any              */
    /*                                                 */
    /* XXX: The convention should be emphasized in     */
    /*      the documents because it can be confusing. */
    if ( size->strike_index != 0xFFFFFFFFUL      &&
         ( load_flags & FT_LOAD_NO_BITMAP ) == 0 )
    {
      error = load_sbit_image( size, glyph, glyph_index, load_flags );
      if ( !error )
      {
        if ( FT_IS_SCALABLE( glyph->face ) )
        {
          /* for the bbox we need the header only */
          (void)tt_loader_init( &loader, size, glyph, load_flags, TRUE );
          (void)load_truetype_glyph( &loader, glyph_index, 0, TRUE );
          glyph->linearHoriAdvance = loader.linear;
          glyph->linearVertAdvance = loader.top_bearing + loader.bbox.yMax -
                                       loader.vadvance;

          /* sanity check: if `horiAdvance' in the sbit metric */
          /* structure isn't set, use `linearHoriAdvance'      */
          if ( !glyph->metrics.horiAdvance && glyph->linearHoriAdvance )
            glyph->metrics.horiAdvance =
              FT_MulFix( glyph->linearHoriAdvance,
                         size->root.metrics.x_scale );
        }

        return FT_Err_Ok;
      }
    }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */

    /* if FT_LOAD_NO_SCALE is not set, `ttmetrics' must be valid */
    if ( !( load_flags & FT_LOAD_NO_SCALE ) && !size->ttmetrics.valid )
      return FT_THROW( Invalid_Size_Handle );

    if ( load_flags & FT_LOAD_SBITS_ONLY )
      return FT_THROW( Invalid_Argument );

    error = tt_loader_init( &loader, size, glyph, load_flags, FALSE );
    if ( error )
      return error;

    glyph->format        = FT_GLYPH_FORMAT_OUTLINE;
    glyph->num_subglyphs = 0;
    glyph->outline.flags = 0;

    /* main loading loop */
    error = load_truetype_glyph( &loader, glyph_index, 0, FALSE );
    if ( !error )
    {
      if ( glyph->format == FT_GLYPH_FORMAT_COMPOSITE )
      {
        glyph->num_subglyphs = loader.gloader->base.num_subglyphs;
        glyph->subglyphs     = loader.gloader->base.subglyphs;
      }
      else
      {
        glyph->outline        = loader.gloader->base.outline;
        glyph->outline.flags &= ~FT_OUTLINE_SINGLE_PASS;

        /* Translate array so that (0,0) is the glyph's origin.  Note  */
        /* that this behaviour is independent on the value of bit 1 of */
        /* the `flags' field in the `head' table -- at least major     */
        /* applications like Acroread indicate that.                   */
        if ( loader.pp1.x )
          FT_Outline_Translate( &glyph->outline, -loader.pp1.x, 0 );
      }

#ifdef TT_USE_BYTECODE_INTERPRETER

      if ( IS_HINTED( load_flags ) )
      {
        if ( loader.exec->GS.scan_control )
        {
          /* convert scan conversion mode to FT_OUTLINE_XXX flags */
          switch ( loader.exec->GS.scan_type )
          {
          case 0: /* simple drop-outs including stubs */
            glyph->outline.flags |= FT_OUTLINE_INCLUDE_STUBS;
            break;
          case 1: /* simple drop-outs excluding stubs */
            /* nothing; it's the default rendering mode */
            break;
          case 4: /* smart drop-outs including stubs */
            glyph->outline.flags |= FT_OUTLINE_SMART_DROPOUTS |
                                    FT_OUTLINE_INCLUDE_STUBS;
            break;
          case 5: /* smart drop-outs excluding stubs  */
            glyph->outline.flags |= FT_OUTLINE_SMART_DROPOUTS;
            break;

          default: /* no drop-out control */
            glyph->outline.flags |= FT_OUTLINE_IGNORE_DROPOUTS;
            break;
          }
        }
        else
          glyph->outline.flags |= FT_OUTLINE_IGNORE_DROPOUTS;
      }

#endif /* TT_USE_BYTECODE_INTERPRETER */

      compute_glyph_metrics( &loader, glyph_index );
    }

    /* Set the `high precision' bit flag.                           */
    /* This is _critical_ to get correct output for monochrome      */
    /* TrueType glyphs at all sizes using the bytecode interpreter. */
    /*                                                              */
    if ( !( load_flags & FT_LOAD_NO_SCALE ) &&
         size->root.metrics.y_ppem < 24     )
      glyph->outline.flags |= FT_OUTLINE_HIGH_PRECISION;

    return error;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttobjs.c                                                               */
/*                                                                         */
/*    Objects manager (body).                                              */
/*                                                                         */
/*  Copyright 1996-2013                                                    */
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
#include FT_INTERNAL_SFNT_H
#include FT_TRUETYPE_DRIVER_H

#include "ttgload.h"
#include "ttpload.h"

#include "tterrors.h"

#ifdef TT_USE_BYTECODE_INTERPRETER
#include "ttinterp.h"
#endif

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
#include FT_TRUETYPE_UNPATENTED_H
#endif

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include "ttgxvar.h"
#endif

  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttobjs


#ifdef TT_USE_BYTECODE_INTERPRETER

  /*************************************************************************/
  /*                                                                       */
  /*                       GLYPH ZONE FUNCTIONS                            */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_glyphzone_done                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Deallocate a glyph zone.                                           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    zone :: A pointer to the target glyph zone.                        */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_glyphzone_done( TT_GlyphZone  zone )
  {
    FT_Memory  memory = zone->memory;


    if ( memory )
    {
      FT_FREE( zone->contours );
      FT_FREE( zone->tags );
      FT_FREE( zone->cur );
      FT_FREE( zone->org );
      FT_FREE( zone->orus );

      zone->max_points   = zone->n_points   = 0;
      zone->max_contours = zone->n_contours = 0;
      zone->memory       = NULL;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_glyphzone_new                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Allocate a new glyph zone.                                         */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory      :: A handle to the current memory object.              */
  /*                                                                       */
  /*    maxPoints   :: The capacity of glyph zone in points.               */
  /*                                                                       */
  /*    maxContours :: The capacity of glyph zone in contours.             */
  /*                                                                       */
  /* <Output>                                                              */
  /*    zone        :: A pointer to the target glyph zone record.          */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_glyphzone_new( FT_Memory     memory,
                    FT_UShort     maxPoints,
                    FT_Short      maxContours,
                    TT_GlyphZone  zone )
  {
    FT_Error  error;


    FT_MEM_ZERO( zone, sizeof ( *zone ) );
    zone->memory = memory;

    if ( FT_NEW_ARRAY( zone->org,      maxPoints   ) ||
         FT_NEW_ARRAY( zone->cur,      maxPoints   ) ||
         FT_NEW_ARRAY( zone->orus,     maxPoints   ) ||
         FT_NEW_ARRAY( zone->tags,     maxPoints   ) ||
         FT_NEW_ARRAY( zone->contours, maxContours ) )
    {
      tt_glyphzone_done( zone );
    }
    else
    {
      zone->max_points   = maxPoints;
      zone->max_contours = maxContours;
    }

    return error;
  }
#endif /* TT_USE_BYTECODE_INTERPRETER */


  /* Compare the face with a list of well-known `tricky' fonts. */
  /* This list shall be expanded as we find more of them.       */

  static FT_Bool
  tt_check_trickyness_family( FT_String*  name )
  {

#define TRICK_NAMES_MAX_CHARACTERS  19
#define TRICK_NAMES_COUNT            9

    static const char trick_names[TRICK_NAMES_COUNT]
                                 [TRICK_NAMES_MAX_CHARACTERS + 1] =
    {
      "DFKaiSho-SB",        /* dfkaisb.ttf */
      "DFKaiShu",
      "DFKai-SB",           /* kaiu.ttf */
      "HuaTianKaiTi?",      /* htkt2.ttf */
      "HuaTianSongTi?",     /* htst3.ttf */
      "Ming(for ISO10646)", /* hkscsiic.ttf & iicore.ttf */
      "MingLiU",            /* mingliu.ttf & mingliu.ttc */
      "PMingLiU",           /* mingliu.ttc */
      "MingLi43",           /* mingli.ttf */
    };

    int  nn;


    for ( nn = 0; nn < TRICK_NAMES_COUNT; nn++ )
      if ( ft_strstr( name, trick_names[nn] ) )
        return TRUE;

    return FALSE;
  }


  /* XXX: This function should be in the `sfnt' module. */

  /* Some PDF generators clear the checksums in the TrueType header table. */
  /* For example, Quartz ContextPDF clears all entries, or Bullzip PDF     */
  /* Printer clears the entries for subsetted subtables.  We thus have to  */
  /* recalculate the checksums  where necessary.                           */

  static FT_UInt32
  tt_synth_sfnt_checksum( FT_Stream  stream,
                          FT_ULong   length )
  {
    FT_Error   error;
    FT_UInt32  checksum = 0;
    int        i;


    if ( FT_FRAME_ENTER( length ) )
      return 0;

    for ( ; length > 3; length -= 4 )
      checksum += (FT_UInt32)FT_GET_ULONG();

    for ( i = 3; length > 0; length --, i-- )
      checksum += (FT_UInt32)( FT_GET_BYTE() << ( i * 8 ) );

    FT_FRAME_EXIT();

    return checksum;
  }


  /* XXX: This function should be in the `sfnt' module. */

  static FT_ULong
  tt_get_sfnt_checksum( TT_Face    face,
                        FT_UShort  i )
  {
#if 0 /* if we believe the written value, use following part. */
    if ( face->dir_tables[i].CheckSum )
      return face->dir_tables[i].CheckSum;
#endif

    if ( !face->goto_table )
      return 0;

    if ( face->goto_table( face,
                           face->dir_tables[i].Tag,
                           face->root.stream,
                           NULL ) )
      return 0;

    return (FT_ULong)tt_synth_sfnt_checksum( face->root.stream,
                                             face->dir_tables[i].Length );
  }


  typedef struct tt_sfnt_id_rec_
  {
    FT_ULong  CheckSum;
    FT_ULong  Length;

  } tt_sfnt_id_rec;


  static FT_Bool
  tt_check_trickyness_sfnt_ids( TT_Face  face )
  {
#define TRICK_SFNT_IDS_PER_FACE   3
#define TRICK_SFNT_IDS_NUM_FACES  17

    static const tt_sfnt_id_rec sfnt_id[TRICK_SFNT_IDS_NUM_FACES]
                                       [TRICK_SFNT_IDS_PER_FACE] = {

#define TRICK_SFNT_ID_cvt   0
#define TRICK_SFNT_ID_fpgm  1
#define TRICK_SFNT_ID_prep  2

      { /* MingLiU 1995 */
        { 0x05bcf058, 0x000002e4 }, /* cvt  */
        { 0x28233bf1, 0x000087c4 }, /* fpgm */
        { 0xa344a1ea, 0x000001e1 }  /* prep */
      },
      { /* MingLiU 1996- */
        { 0x05bcf058, 0x000002e4 }, /* cvt  */
        { 0x28233bf1, 0x000087c4 }, /* fpgm */
        { 0xa344a1eb, 0x000001e1 }  /* prep */
      },
      { /* DFKaiShu */
        { 0x11e5ead4, 0x00000350 }, /* cvt  */
        { 0x5a30ca3b, 0x00009063 }, /* fpgm */
        { 0x13a42602, 0x0000007e }  /* prep */
      },
      { /* HuaTianKaiTi */
        { 0xfffbfffc, 0x00000008 }, /* cvt  */
        { 0x9c9e48b8, 0x0000bea2 }, /* fpgm */
        { 0x70020112, 0x00000008 }  /* prep */
      },
      { /* HuaTianSongTi */
        { 0xfffbfffc, 0x00000008 }, /* cvt  */
        { 0x0a5a0483, 0x00017c39 }, /* fpgm */
        { 0x70020112, 0x00000008 }  /* prep */
      },
      { /* NEC fadpop7.ttf */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x40c92555, 0x000000e5 }, /* fpgm */
        { 0xa39b58e3, 0x0000117c }  /* prep */
      },
      { /* NEC fadrei5.ttf */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x33c41652, 0x000000e5 }, /* fpgm */
        { 0x26d6c52a, 0x00000f6a }  /* prep */
      },
      { /* NEC fangot7.ttf */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x6db1651d, 0x0000019d }, /* fpgm */
        { 0x6c6e4b03, 0x00002492 }  /* prep */
      },
      { /* NEC fangyo5.ttf */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x40c92555, 0x000000e5 }, /* fpgm */
        { 0xde51fad0, 0x0000117c }  /* prep */
      },
      { /* NEC fankyo5.ttf */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x85e47664, 0x000000e5 }, /* fpgm */
        { 0xa6c62831, 0x00001caa }  /* prep */
      },
      { /* NEC fanrgo5.ttf */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x2d891cfd, 0x0000019d }, /* fpgm */
        { 0xa0604633, 0x00001de8 }  /* prep */
      },
      { /* NEC fangot5.ttc */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x40aa774c, 0x000001cb }, /* fpgm */
        { 0x9b5caa96, 0x00001f9a }  /* prep */
      },
      { /* NEC fanmin3.ttc */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x0d3de9cb, 0x00000141 }, /* fpgm */
        { 0xd4127766, 0x00002280 }  /* prep */
      },
      { /* NEC FA-Gothic, 1996 */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x4a692698, 0x000001f0 }, /* fpgm */
        { 0x340d4346, 0x00001fca }  /* prep */
      },
      { /* NEC FA-Minchou, 1996 */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0xcd34c604, 0x00000166 }, /* fpgm */
        { 0x6cf31046, 0x000022b0 }  /* prep */
      },
      { /* NEC FA-RoundGothicB, 1996 */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0x5da75315, 0x0000019d }, /* fpgm */
        { 0x40745a5f, 0x000022e0 }  /* prep */
      },
      { /* NEC FA-RoundGothicM, 1996 */
        { 0x00000000, 0x00000000 }, /* cvt  */
        { 0xf055fc48, 0x000001c2 }, /* fpgm */
        { 0x3900ded3, 0x00001e18 }  /* prep */
      }
    };

    FT_ULong   checksum;
    int        num_matched_ids[TRICK_SFNT_IDS_NUM_FACES];
    FT_Bool    has_cvt, has_fpgm, has_prep;
    FT_UShort  i;
    int        j, k;


    FT_MEM_SET( num_matched_ids, 0,
                sizeof ( int ) * TRICK_SFNT_IDS_NUM_FACES );
    has_cvt  = FALSE;
    has_fpgm = FALSE;
    has_prep = FALSE;

    for ( i = 0; i < face->num_tables; i++ )
    {
      checksum = 0;

      switch( face->dir_tables[i].Tag )
      {
      case TTAG_cvt:
        k = TRICK_SFNT_ID_cvt;
        has_cvt  = TRUE;
        break;

      case TTAG_fpgm:
        k = TRICK_SFNT_ID_fpgm;
        has_fpgm = TRUE;
        break;

      case TTAG_prep:
        k = TRICK_SFNT_ID_prep;
        has_prep = TRUE;
        break;

      default:
        continue;
      }

      for ( j = 0; j < TRICK_SFNT_IDS_NUM_FACES; j++ )
        if ( face->dir_tables[i].Length == sfnt_id[j][k].Length )
        {
          if ( !checksum )
            checksum = tt_get_sfnt_checksum( face, i );

          if ( sfnt_id[j][k].CheckSum == checksum )
            num_matched_ids[j]++;

          if ( num_matched_ids[j] == TRICK_SFNT_IDS_PER_FACE )
            return TRUE;
        }
    }

    for ( j = 0; j < TRICK_SFNT_IDS_NUM_FACES; j++ )
    {
      if ( !has_cvt  && !sfnt_id[j][TRICK_SFNT_ID_cvt].Length )
        num_matched_ids[j] ++;
      if ( !has_fpgm && !sfnt_id[j][TRICK_SFNT_ID_fpgm].Length )
        num_matched_ids[j] ++;
      if ( !has_prep && !sfnt_id[j][TRICK_SFNT_ID_prep].Length )
        num_matched_ids[j] ++;
      if ( num_matched_ids[j] == TRICK_SFNT_IDS_PER_FACE )
        return TRUE;
    }

    return FALSE;
  }


  static FT_Bool
  tt_check_trickyness( FT_Face  face )
  {
    if ( !face )
      return FALSE;

    /* For first, check the face name for quick check. */
    if ( face->family_name                               &&
         tt_check_trickyness_family( face->family_name ) )
      return TRUE;

    /* Type42 fonts may lack `name' tables, we thus try to identify */
    /* tricky fonts by checking the checksums of Type42-persistent  */
    /* sfnt tables (`cvt', `fpgm', and `prep').                     */
    if ( tt_check_trickyness_sfnt_ids( (TT_Face)face ) )
      return TRUE;

    return FALSE;
  }


  /* Check whether `.notdef' is the only glyph in the `loca' table. */
  static FT_Bool
  tt_check_single_notdef( FT_Face  ttface )
  {
    FT_Bool   result = FALSE;

    TT_Face   face = (TT_Face)ttface;
    FT_UInt   asize;
    FT_ULong  i;
    FT_ULong  glyph_index = 0;
    FT_UInt   count       = 0;


    for( i = 0; i < face->num_locations; i++ )
    {
      tt_face_get_location( face, i, &asize );
      if ( asize > 0 )
      {
        count += 1;
        if ( count > 1 )
          break;
        glyph_index = i;
      }
    }

    /* Only have a single outline. */
    if ( count == 1 )
    {
      if ( glyph_index == 0 )
        result = TRUE;
      else
      {
        /* FIXME: Need to test glyphname == .notdef ? */
        FT_Error error;
        char buf[8];


        error = FT_Get_Glyph_Name( ttface, glyph_index, buf, 8 );
        if ( !error                                            &&
             buf[0] == '.' && !ft_strncmp( buf, ".notdef", 8 ) )
          result = TRUE;
      }
    }

    return result;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_init                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initialize a given TrueType face object.                           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream     :: The source font stream.                              */
  /*                                                                       */
  /*    face_index :: The index of the font face in the resource.          */
  /*                                                                       */
  /*    num_params :: Number of additional generic parameters.  Ignored.   */
  /*                                                                       */
  /*    params     :: Additional generic parameters.  Ignored.             */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face       :: The newly built face object.                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_init( FT_Stream      stream,
                FT_Face        ttface,      /* TT_Face */
                FT_Int         face_index,
                FT_Int         num_params,
                FT_Parameter*  params )
  {
    FT_Error      error;
    FT_Library    library;
    SFNT_Service  sfnt;
    TT_Face       face = (TT_Face)ttface;


    FT_TRACE2(( "TTF driver\n" ));

    library = ttface->driver->root.library;

    sfnt = (SFNT_Service)FT_Get_Module_Interface( library, "sfnt" );
    if ( !sfnt )
    {
      FT_ERROR(( "tt_face_init: cannot access `sfnt' module\n" ));
      error = FT_THROW( Missing_Module );
      goto Exit;
    }

    /* create input stream from resource */
    if ( FT_STREAM_SEEK( 0 ) )
      goto Exit;

    /* check that we have a valid TrueType file */
    error = sfnt->init_face( stream, face, face_index, num_params, params );

    /* Stream may have changed. */
    stream = face->root.stream;

    if ( error )
      goto Exit;

    /* We must also be able to accept Mac/GX fonts, as well as OT ones. */
    /* The 0x00020000 tag is completely undocumented; some fonts from   */
    /* Arphic made for Chinese Windows 3.1 have this.                   */
    if ( face->format_tag != 0x00010000L &&    /* MS fonts  */
         face->format_tag != 0x00020000L &&    /* CJK fonts for Win 3.1 */
         face->format_tag != TTAG_true   )     /* Mac fonts */
    {
      FT_TRACE2(( "  not a TTF font\n" ));
      goto Bad_Format;
    }

#ifdef TT_USE_BYTECODE_INTERPRETER
    ttface->face_flags |= FT_FACE_FLAG_HINTER;
#endif

    /* If we are performing a simple font format check, exit immediately. */
    if ( face_index < 0 )
      return FT_Err_Ok;

    /* Load font directory */
    error = sfnt->load_face( stream, face, face_index, num_params, params );
    if ( error )
      goto Exit;

    if ( tt_check_trickyness( ttface ) )
      ttface->face_flags |= FT_FACE_FLAG_TRICKY;

    error = tt_face_load_hdmx( face, stream );
    if ( error )
      goto Exit;

    if ( FT_IS_SCALABLE( ttface ) )
    {

#ifdef FT_CONFIG_OPTION_INCREMENTAL

      if ( !ttface->internal->incremental_interface )
        error = tt_face_load_loca( face, stream );
      if ( !error )
        error = tt_face_load_cvt( face, stream );
      if ( !error )
        error = tt_face_load_fpgm( face, stream );
      if ( !error )
        error = tt_face_load_prep( face, stream );

      /* Check the scalable flag based on `loca'. */
      if ( !ttface->internal->incremental_interface &&
           ttface->num_fixed_sizes                  &&
           face->glyph_locations                    &&
           tt_check_single_notdef( ttface )         )
      {
        FT_TRACE5(( "tt_face_init:"
                    " Only the `.notdef' glyph has an outline.\n"
                    "             "
                    " Resetting scalable flag to FALSE.\n" ));

        ttface->face_flags &= ~FT_FACE_FLAG_SCALABLE;
      }

#else

      if ( !error )
        error = tt_face_load_loca( face, stream );
      if ( !error )
        error = tt_face_load_cvt( face, stream );
      if ( !error )
        error = tt_face_load_fpgm( face, stream );
      if ( !error )
        error = tt_face_load_prep( face, stream );

      /* Check the scalable flag based on `loca'. */
      if ( ttface->num_fixed_sizes          &&
           face->glyph_locations            &&
           tt_check_single_notdef( ttface ) )
      {
        FT_TRACE5(( "tt_face_init:"
                    " Only the `.notdef' glyph has an outline.\n"
                    "             "
                    " Resetting scalable flag to FALSE.\n" ));

        ttface->face_flags &= ~FT_FACE_FLAG_SCALABLE;
      }

#endif

    }

#if defined( TT_CONFIG_OPTION_UNPATENTED_HINTING    ) && \
    !defined( TT_CONFIG_OPTION_BYTECODE_INTERPRETER )

    {
      FT_Bool  unpatented_hinting;
      int      i;


      /* Determine whether unpatented hinting is to be used for this face. */
      unpatented_hinting = FT_BOOL
        ( library->debug_hooks[FT_DEBUG_HOOK_UNPATENTED_HINTING] != NULL );

      for ( i = 0; i < num_params && !face->unpatented_hinting; i++ )
        if ( params[i].tag == FT_PARAM_TAG_UNPATENTED_HINTING )
          unpatented_hinting = TRUE;

      if ( !unpatented_hinting )
        ttface->internal->ignore_unpatented_hinter = TRUE;
    }

#endif /* TT_CONFIG_OPTION_UNPATENTED_HINTING &&
          !TT_CONFIG_OPTION_BYTECODE_INTERPRETER */

    /* initialize standard glyph loading routines */
    TT_Init_Glyph_Loading( face );

  Exit:
    return error;

  Bad_Format:
    error = FT_THROW( Unknown_File_Format );
    goto Exit;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_done                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalize a given face object.                                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A pointer to the face object to destroy.                   */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_face_done( FT_Face  ttface )           /* TT_Face */
  {
    TT_Face       face = (TT_Face)ttface;
    FT_Memory     memory;
    FT_Stream     stream;
    SFNT_Service  sfnt;


    if ( !face )
      return;

    memory = ttface->memory;
    stream = ttface->stream;
    sfnt   = (SFNT_Service)face->sfnt;

    /* for `extended TrueType formats' (i.e. compressed versions) */
    if ( face->extra.finalizer )
      face->extra.finalizer( face->extra.data );

    if ( sfnt )
      sfnt->done_face( face );

    /* freeing the locations table */
    tt_face_done_loca( face );

    tt_face_free_hdmx( face );

    /* freeing the CVT */
    FT_FREE( face->cvt );
    face->cvt_size = 0;

    /* freeing the programs */
    FT_FRAME_RELEASE( face->font_program );
    FT_FRAME_RELEASE( face->cvt_program );
    face->font_program_size = 0;
    face->cvt_program_size  = 0;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    tt_done_blend( memory, face->blend );
    face->blend = NULL;
#endif
  }


  /*************************************************************************/
  /*                                                                       */
  /*                           SIZE  FUNCTIONS                             */
  /*                                                                       */
  /*************************************************************************/

#ifdef TT_USE_BYTECODE_INTERPRETER

  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_run_fpgm                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Run the font program.                                              */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size     :: A handle to the size object.                           */
  /*                                                                       */
  /*    pedantic :: Set if bytecode execution should be pedantic.          */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_size_run_fpgm( TT_Size  size,
                    FT_Bool  pedantic )
  {
    TT_Face         face = (TT_Face)size->root.face;
    TT_ExecContext  exec;
    FT_Error        error;


    /* debugging instances have their own context */
    if ( size->debug )
      exec = size->context;
    else
      exec = ( (TT_Driver)FT_FACE_DRIVER( face ) )->context;

    if ( !exec )
      return FT_THROW( Could_Not_Find_Context );

    TT_Load_Context( exec, face, size );

    exec->callTop = 0;
    exec->top     = 0;

    exec->period    = 64;
    exec->phase     = 0;
    exec->threshold = 0;

    exec->instruction_trap = FALSE;
    exec->F_dot_P          = 0x4000L;

    exec->pedantic_hinting = pedantic;

    {
      FT_Size_Metrics*  metrics    = &exec->metrics;
      TT_Size_Metrics*  tt_metrics = &exec->tt_metrics;


      metrics->x_ppem   = 0;
      metrics->y_ppem   = 0;
      metrics->x_scale  = 0;
      metrics->y_scale  = 0;

      tt_metrics->ppem  = 0;
      tt_metrics->scale = 0;
      tt_metrics->ratio = 0x10000L;
    }

    /* allow font program execution */
    TT_Set_CodeRange( exec,
                      tt_coderange_font,
                      face->font_program,
                      face->font_program_size );

    /* disable CVT and glyph programs coderange */
    TT_Clear_CodeRange( exec, tt_coderange_cvt );
    TT_Clear_CodeRange( exec, tt_coderange_glyph );

    if ( face->font_program_size > 0 )
    {
      error = TT_Goto_CodeRange( exec, tt_coderange_font, 0 );

      if ( !error )
      {
        FT_TRACE4(( "Executing `fpgm' table.\n" ));

        error = face->interpreter( exec );
      }
    }
    else
      error = FT_Err_Ok;

    if ( !error )
      TT_Save_Context( exec, size );

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_run_prep                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Run the control value program.                                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size     :: A handle to the size object.                           */
  /*                                                                       */
  /*    pedantic :: Set if bytecode execution should be pedantic.          */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_size_run_prep( TT_Size  size,
                    FT_Bool  pedantic )
  {
    TT_Face         face = (TT_Face)size->root.face;
    TT_ExecContext  exec;
    FT_Error        error;


    /* debugging instances have their own context */
    if ( size->debug )
      exec = size->context;
    else
      exec = ( (TT_Driver)FT_FACE_DRIVER( face ) )->context;

    if ( !exec )
      return FT_THROW( Could_Not_Find_Context );

    TT_Load_Context( exec, face, size );

    exec->callTop = 0;
    exec->top     = 0;

    exec->instruction_trap = FALSE;

    exec->pedantic_hinting = pedantic;

    TT_Set_CodeRange( exec,
                      tt_coderange_cvt,
                      face->cvt_program,
                      face->cvt_program_size );

    TT_Clear_CodeRange( exec, tt_coderange_glyph );

    if ( face->cvt_program_size > 0 )
    {
      error = TT_Goto_CodeRange( exec, tt_coderange_cvt, 0 );

      if ( !error && !size->debug )
      {
        FT_TRACE4(( "Executing `prep' table.\n" ));

        error = face->interpreter( exec );
      }
    }
    else
      error = FT_Err_Ok;

    /* UNDOCUMENTED!  The MS rasterizer doesn't allow the following */
    /* graphics state variables to be modified by the CVT program.  */

    exec->GS.dualVector.x = 0x4000;
    exec->GS.dualVector.y = 0;
    exec->GS.projVector.x = 0x4000;
    exec->GS.projVector.y = 0x0;
    exec->GS.freeVector.x = 0x4000;
    exec->GS.freeVector.y = 0x0;

    exec->GS.rp0 = 0;
    exec->GS.rp1 = 0;
    exec->GS.rp2 = 0;

    exec->GS.gep0 = 1;
    exec->GS.gep1 = 1;
    exec->GS.gep2 = 1;

    exec->GS.loop = 1;

    /* save as default graphics state */
    size->GS = exec->GS;

    TT_Save_Context( exec, size );

    return error;
  }

#endif /* TT_USE_BYTECODE_INTERPRETER */


#ifdef TT_USE_BYTECODE_INTERPRETER

  static void
  tt_size_done_bytecode( FT_Size  ftsize )
  {
    TT_Size    size   = (TT_Size)ftsize;
    TT_Face    face   = (TT_Face)ftsize->face;
    FT_Memory  memory = face->root.memory;


    if ( size->debug )
    {
      /* the debug context must be deleted by the debugger itself */
      size->context = NULL;
      size->debug   = FALSE;
    }

    FT_FREE( size->cvt );
    size->cvt_size = 0;

    /* free storage area */
    FT_FREE( size->storage );
    size->storage_size = 0;

    /* twilight zone */
    tt_glyphzone_done( &size->twilight );

    FT_FREE( size->function_defs );
    FT_FREE( size->instruction_defs );

    size->num_function_defs    = 0;
    size->max_function_defs    = 0;
    size->num_instruction_defs = 0;
    size->max_instruction_defs = 0;

    size->max_func = 0;
    size->max_ins  = 0;

    size->bytecode_ready = 0;
    size->cvt_ready      = 0;
  }


  /* Initialize bytecode-related fields in the size object.       */
  /* We do this only if bytecode interpretation is really needed. */
  static FT_Error
  tt_size_init_bytecode( FT_Size  ftsize,
                         FT_Bool  pedantic )
  {
    FT_Error   error;
    TT_Size    size = (TT_Size)ftsize;
    TT_Face    face = (TT_Face)ftsize->face;
    FT_Memory  memory = face->root.memory;
    FT_Int     i;

    FT_UShort       n_twilight;
    TT_MaxProfile*  maxp = &face->max_profile;


    size->bytecode_ready = 1;
    size->cvt_ready      = 0;

    size->max_function_defs    = maxp->maxFunctionDefs;
    size->max_instruction_defs = maxp->maxInstructionDefs;

    size->num_function_defs    = 0;
    size->num_instruction_defs = 0;

    size->max_func = 0;
    size->max_ins  = 0;

    size->cvt_size     = face->cvt_size;
    size->storage_size = maxp->maxStorage;

    /* Set default metrics */
    {
      TT_Size_Metrics*  metrics = &size->ttmetrics;


      metrics->rotated   = FALSE;
      metrics->stretched = FALSE;

      /* set default compensation (all 0) */
      for ( i = 0; i < 4; i++ )
        metrics->compensations[i] = 0;
    }

    /* allocate function defs, instruction defs, cvt, and storage area */
    if ( FT_NEW_ARRAY( size->function_defs,    size->max_function_defs    ) ||
         FT_NEW_ARRAY( size->instruction_defs, size->max_instruction_defs ) ||
         FT_NEW_ARRAY( size->cvt,              size->cvt_size             ) ||
         FT_NEW_ARRAY( size->storage,          size->storage_size         ) )
      goto Exit;

    /* reserve twilight zone */
    n_twilight = maxp->maxTwilightPoints;

    /* there are 4 phantom points (do we need this?) */
    n_twilight += 4;

    error = tt_glyphzone_new( memory, n_twilight, 0, &size->twilight );
    if ( error )
      goto Exit;

    size->twilight.n_points = n_twilight;

    size->GS = tt_default_graphics_state;

    /* set `face->interpreter' according to the debug hook present */
    {
      FT_Library  library = face->root.driver->root.library;


      face->interpreter = (TT_Interpreter)
                            library->debug_hooks[FT_DEBUG_HOOK_TRUETYPE];
      if ( !face->interpreter )
        face->interpreter = (TT_Interpreter)TT_RunIns;
    }

    /* Fine, now run the font program! */
    error = tt_size_run_fpgm( size, pedantic );

  Exit:
    if ( error )
      tt_size_done_bytecode( ftsize );

    return error;
  }


  FT_LOCAL_DEF( FT_Error )
  tt_size_ready_bytecode( TT_Size  size,
                          FT_Bool  pedantic )
  {
    FT_Error  error = FT_Err_Ok;


    if ( !size->bytecode_ready )
    {
      error = tt_size_init_bytecode( (FT_Size)size, pedantic );
      if ( error )
        goto Exit;
    }

    /* rescale CVT when needed */
    if ( !size->cvt_ready )
    {
      FT_UInt  i;
      TT_Face  face = (TT_Face)size->root.face;


      /* Scale the cvt values to the new ppem.          */
      /* We use by default the y ppem to scale the CVT. */
      for ( i = 0; i < size->cvt_size; i++ )
        size->cvt[i] = FT_MulFix( face->cvt[i], size->ttmetrics.scale );

      /* all twilight points are originally zero */
      for ( i = 0; i < (FT_UInt)size->twilight.n_points; i++ )
      {
        size->twilight.org[i].x = 0;
        size->twilight.org[i].y = 0;
        size->twilight.cur[i].x = 0;
        size->twilight.cur[i].y = 0;
      }

      /* clear storage area */
      for ( i = 0; i < (FT_UInt)size->storage_size; i++ )
        size->storage[i] = 0;

      size->GS = tt_default_graphics_state;

      error = tt_size_run_prep( size, pedantic );
      if ( !error )
        size->cvt_ready = 1;
    }

  Exit:
    return error;
  }

#endif /* TT_USE_BYTECODE_INTERPRETER */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_init                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initialize a new TrueType size object.                             */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    size :: A handle to the size object.                               */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_size_init( FT_Size  ttsize )           /* TT_Size */
  {
    TT_Size   size  = (TT_Size)ttsize;
    FT_Error  error = FT_Err_Ok;

#ifdef TT_USE_BYTECODE_INTERPRETER
    size->bytecode_ready = 0;
    size->cvt_ready      = 0;
#endif

    size->ttmetrics.valid = FALSE;
    size->strike_index    = 0xFFFFFFFFUL;

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_done                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The TrueType size object finalizer.                                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_size_done( FT_Size  ttsize )           /* TT_Size */
  {
    TT_Size  size = (TT_Size)ttsize;


#ifdef TT_USE_BYTECODE_INTERPRETER
    if ( size->bytecode_ready )
      tt_size_done_bytecode( ttsize );
#endif

    size->ttmetrics.valid = FALSE;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_reset                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Reset a TrueType size when resolutions and character dimensions    */
  /*    have been changed.                                                 */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_size_reset( TT_Size  size )
  {
    TT_Face           face;
    FT_Error          error = FT_Err_Ok;
    FT_Size_Metrics*  metrics;


    size->ttmetrics.valid = FALSE;

    face = (TT_Face)size->root.face;

    metrics = &size->metrics;

    /* copy the result from base layer */
    *metrics = size->root.metrics;

    if ( metrics->x_ppem < 1 || metrics->y_ppem < 1 )
      return FT_THROW( Invalid_PPem );

    /* This bit flag, if set, indicates that the ppems must be       */
    /* rounded to integers.  Nearly all TrueType fonts have this bit */
    /* set, as hinting won't work really well otherwise.             */
    /*                                                               */
    if ( face->header.Flags & 8 )
    {
      metrics->x_scale = FT_DivFix( metrics->x_ppem << 6,
                                    face->root.units_per_EM );
      metrics->y_scale = FT_DivFix( metrics->y_ppem << 6,
                                    face->root.units_per_EM );

      metrics->ascender =
        FT_PIX_ROUND( FT_MulFix( face->root.ascender, metrics->y_scale ) );
      metrics->descender =
        FT_PIX_ROUND( FT_MulFix( face->root.descender, metrics->y_scale ) );
      metrics->height =
        FT_PIX_ROUND( FT_MulFix( face->root.height, metrics->y_scale ) );
      metrics->max_advance =
        FT_PIX_ROUND( FT_MulFix( face->root.max_advance_width,
                                 metrics->x_scale ) );
    }

    /* compute new transformation */
    if ( metrics->x_ppem >= metrics->y_ppem )
    {
      size->ttmetrics.scale   = metrics->x_scale;
      size->ttmetrics.ppem    = metrics->x_ppem;
      size->ttmetrics.x_ratio = 0x10000L;
      size->ttmetrics.y_ratio = FT_DivFix( metrics->y_ppem,
                                           metrics->x_ppem );
    }
    else
    {
      size->ttmetrics.scale   = metrics->y_scale;
      size->ttmetrics.ppem    = metrics->y_ppem;
      size->ttmetrics.x_ratio = FT_DivFix( metrics->x_ppem,
                                           metrics->y_ppem );
      size->ttmetrics.y_ratio = 0x10000L;
    }

#ifdef TT_USE_BYTECODE_INTERPRETER
    size->cvt_ready = 0;
#endif /* TT_USE_BYTECODE_INTERPRETER */

    if ( !error )
      size->ttmetrics.valid = TRUE;

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_driver_init                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initialize a given TrueType driver object.                         */
  /*                                                                       */
  /* <Input>                                                               */
  /*    driver :: A handle to the target driver object.                    */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_driver_init( FT_Module  ttdriver )     /* TT_Driver */
  {

#ifdef TT_USE_BYTECODE_INTERPRETER

    TT_Driver  driver = (TT_Driver)ttdriver;


    if ( !TT_New_Context( driver ) )
      return FT_THROW( Could_Not_Find_Context );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    driver->interpreter_version = TT_INTERPRETER_VERSION_38;
#else
    driver->interpreter_version = TT_INTERPRETER_VERSION_35;
#endif

#else /* !TT_USE_BYTECODE_INTERPRETER */

    FT_UNUSED( ttdriver );

#endif /* !TT_USE_BYTECODE_INTERPRETER */

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_driver_done                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalize a given TrueType driver.                                  */
  /*                                                                       */
  /* <Input>                                                               */
  /*    driver :: A handle to the target TrueType driver.                  */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_driver_done( FT_Module  ttdriver )     /* TT_Driver */
  {
#ifdef TT_USE_BYTECODE_INTERPRETER
    TT_Driver  driver = (TT_Driver)ttdriver;


    /* destroy the execution context */
    if ( driver->context )
    {
      TT_Done_Context( driver->context );
      driver->context = NULL;
    }
#else
    FT_UNUSED( ttdriver );
#endif

  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_slot_init                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initialize a new slot object.                                      */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    slot :: A handle to the slot object.                               */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_slot_init( FT_GlyphSlot  slot )
  {
    return FT_GlyphLoader_CreateExtra( slot->internal->loader );
  }


/* END */

#ifdef TT_USE_BYTECODE_INTERPRETER
/***************************************************************************/
/*                                                                         */
/*  ttinterp.c                                                             */
/*                                                                         */
/*    TrueType bytecode interpreter (body).                                */
/*                                                                         */
/*  Copyright 1996-2014                                                    */
/*  by David Turner, Robert Wilhelm, and Werner Lemberg.                   */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


/* Greg Hitchcock from Microsoft has helped a lot in resolving unclear */
/* issues; many thanks!                                                */


#include "ft2build.h"
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_CALC_H
#include FT_TRIGONOMETRY_H
#include FT_SYSTEM_H
#include FT_TRUETYPE_DRIVER_H

#include "ttinterp.h"
#include "tterrors.h"
#include "ttsubpix.h"


#ifdef TT_USE_BYTECODE_INTERPRETER


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttinterp

  /*************************************************************************/
  /*                                                                       */
  /* In order to detect infinite loops in the code, we set up a counter    */
  /* within the run loop.  A single stroke of interpretation is now        */
  /* limited to a maximum number of opcodes defined below.                 */
  /*                                                                       */
#define MAX_RUNNABLE_OPCODES  1000000L


  /*************************************************************************/
  /*                                                                       */
  /* There are two kinds of implementations:                               */
  /*                                                                       */
  /* a. static implementation                                              */
  /*                                                                       */
  /*    The current execution context is a static variable, which fields   */
  /*    are accessed directly by the interpreter during execution.  The    */
  /*    context is named `cur'.                                            */
  /*                                                                       */
  /*    This version is non-reentrant, of course.                          */
  /*                                                                       */
  /* b. indirect implementation                                            */
  /*                                                                       */
  /*    The current execution context is passed to _each_ function as its  */
  /*    first argument, and each field is thus accessed indirectly.        */
  /*                                                                       */
  /*    This version is fully re-entrant.                                  */
  /*                                                                       */
  /* The idea is that an indirect implementation may be slower to execute  */
  /* on low-end processors that are used in some systems (like 386s or     */
  /* even 486s).                                                           */
  /*                                                                       */
  /* As a consequence, the indirect implementation is now the default, as  */
  /* its performance costs can be considered negligible in our context.    */
  /* Note, however, that we kept the same source with macros because:      */
  /*                                                                       */
  /* - The code is kept very close in design to the Pascal code used for   */
  /*   development.                                                        */
  /*                                                                       */
  /* - It's much more readable that way!                                   */
  /*                                                                       */
  /* - It's still open to experimentation and tuning.                      */
  /*                                                                       */
  /*************************************************************************/


#ifndef TT_CONFIG_OPTION_STATIC_INTERPRETER     /* indirect implementation */

#define CUR  (*exc)                             /* see ttobjs.h */

  /*************************************************************************/
  /*                                                                       */
  /* This macro is used whenever `exec' is unused in a function, to avoid  */
  /* stupid warnings from pedantic compilers.                              */
  /*                                                                       */
#define FT_UNUSED_EXEC  FT_UNUSED( exc )

#else                                           /* static implementation */

#define CUR  cur

#define FT_UNUSED_EXEC  int  __dummy = __dummy

  static
  TT_ExecContextRec  cur;   /* static exec. context variable */

  /* apparently, we have a _lot_ of direct indexing when accessing  */
  /* the static `cur', which makes the code bigger (due to all the  */
  /* four bytes addresses).                                         */

#endif /* TT_CONFIG_OPTION_STATIC_INTERPRETER */


  /*************************************************************************/
  /*                                                                       */
  /* The instruction argument stack.                                       */
  /*                                                                       */
#define INS_ARG  EXEC_OP_ FT_Long*  args    /* see ttobjs.h for EXEC_OP_ */


  /*************************************************************************/
  /*                                                                       */
  /* This macro is used whenever `args' is unused in a function, to avoid  */
  /* stupid warnings from pedantic compilers.                              */
  /*                                                                       */
#define FT_UNUSED_ARG  FT_UNUSED_EXEC; FT_UNUSED( args )


#define SUBPIXEL_HINTING                                                    \
          ( ((TT_Driver)FT_FACE_DRIVER( CUR.face ))->interpreter_version == \
            TT_INTERPRETER_VERSION_38 )


  /*************************************************************************/
  /*                                                                       */
  /* The following macros hide the use of EXEC_ARG and EXEC_ARG_ to        */
  /* increase readability of the code.                                     */
  /*                                                                       */
  /*************************************************************************/


#define SKIP_Code() \
          SkipCode( EXEC_ARG )

#define GET_ShortIns() \
          GetShortIns( EXEC_ARG )

#define NORMalize( x, y, v ) \
          Normalize( EXEC_ARG_ x, y, v )

#define SET_SuperRound( scale, flags ) \
          SetSuperRound( EXEC_ARG_ scale, flags )

#define ROUND_None( d, c ) \
          Round_None( EXEC_ARG_ d, c )

#define INS_Goto_CodeRange( range, ip ) \
          Ins_Goto_CodeRange( EXEC_ARG_ range, ip )

#define CUR_Func_move( z, p, d ) \
          CUR.func_move( EXEC_ARG_ z, p, d )

#define CUR_Func_move_orig( z, p, d ) \
          CUR.func_move_orig( EXEC_ARG_ z, p, d )

#define CUR_Func_round( d, c ) \
          CUR.func_round( EXEC_ARG_ d, c )

#define CUR_Func_read_cvt( index ) \
          CUR.func_read_cvt( EXEC_ARG_ index )

#define CUR_Func_write_cvt( index, val ) \
          CUR.func_write_cvt( EXEC_ARG_ index, val )

#define CUR_Func_move_cvt( index, val ) \
          CUR.func_move_cvt( EXEC_ARG_ index, val )

#define CURRENT_Ratio() \
          Current_Ratio( EXEC_ARG )

#define CURRENT_Ppem() \
          Current_Ppem( EXEC_ARG )

#define CUR_Ppem() \
          Cur_PPEM( EXEC_ARG )

#define INS_SxVTL( a, b, c, d ) \
          Ins_SxVTL( EXEC_ARG_ a, b, c, d )

#define COMPUTE_Funcs() \
          Compute_Funcs( EXEC_ARG )

#define COMPUTE_Round( a ) \
          Compute_Round( EXEC_ARG_ a )

#define COMPUTE_Point_Displacement( a, b, c, d ) \
          Compute_Point_Displacement( EXEC_ARG_ a, b, c, d )

#define MOVE_Zp2_Point( a, b, c, t ) \
          Move_Zp2_Point( EXEC_ARG_ a, b, c, t )


#define CUR_Func_project( v1, v2 )  \
          CUR.func_project( EXEC_ARG_ (v1)->x - (v2)->x, (v1)->y - (v2)->y )

#define CUR_Func_dualproj( v1, v2 )  \
          CUR.func_dualproj( EXEC_ARG_ (v1)->x - (v2)->x, (v1)->y - (v2)->y )

#define CUR_fast_project( v ) \
          CUR.func_project( EXEC_ARG_ (v)->x, (v)->y )

#define CUR_fast_dualproj( v ) \
          CUR.func_dualproj( EXEC_ARG_ (v)->x, (v)->y )


  /*************************************************************************/
  /*                                                                       */
  /* Instruction dispatch function, as used by the interpreter.            */
  /*                                                                       */
  typedef void  (*TInstruction_Function)( INS_ARG );


  /*************************************************************************/
  /*                                                                       */
  /* Two simple bounds-checking macros.                                    */
  /*                                                                       */
#define BOUNDS( x, n )   ( (FT_UInt)(x)  >= (FT_UInt)(n)  )
#define BOUNDSL( x, n )  ( (FT_ULong)(x) >= (FT_ULong)(n) )

  /*************************************************************************/
  /*                                                                       */
  /* This macro computes (a*2^14)/b and complements TT_MulFix14.           */
  /*                                                                       */
#define TT_DivFix14( a, b ) \
          FT_DivFix( a, (b) << 2 )


#undef  SUCCESS
#define SUCCESS  0

#undef  FAILURE
#define FAILURE  1

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
#define GUESS_VECTOR( V )                                         \
  if ( CUR.face->unpatented_hinting )                             \
  {                                                               \
    CUR.GS.V.x = (FT_F2Dot14)( CUR.GS.both_x_axis ? 0x4000 : 0 ); \
    CUR.GS.V.y = (FT_F2Dot14)( CUR.GS.both_x_axis ? 0 : 0x4000 ); \
  }
#else
#define GUESS_VECTOR( V )
#endif

  /*************************************************************************/
  /*                                                                       */
  /*                        CODERANGE FUNCTIONS                            */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Goto_CodeRange                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Switches to a new code range (updates the code related elements in */
  /*    `exec', and `IP').                                                 */
  /*                                                                       */
  /* <Input>                                                               */
  /*    range :: The new execution code range.                             */
  /*                                                                       */
  /*    IP    :: The new IP in the new code range.                         */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    exec  :: The target execution context.                             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Goto_CodeRange( TT_ExecContext  exec,
                     FT_Int          range,
                     FT_Long         IP )
  {
    TT_CodeRange*  coderange;


    FT_ASSERT( range >= 1 && range <= 3 );

    coderange = &exec->codeRangeTable[range - 1];

    FT_ASSERT( coderange->base != NULL );

    /* NOTE: Because the last instruction of a program may be a CALL */
    /*       which will return to the first byte *after* the code    */
    /*       range, we test for IP <= Size instead of IP < Size.     */
    /*                                                               */
    FT_ASSERT( (FT_ULong)IP <= coderange->size );

    exec->code     = coderange->base;
    exec->codeSize = coderange->size;
    exec->IP       = IP;
    exec->curRange = range;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Set_CodeRange                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Sets a code range.                                                 */
  /*                                                                       */
  /* <Input>                                                               */
  /*    range  :: The code range index.                                    */
  /*                                                                       */
  /*    base   :: The new code base.                                       */
  /*                                                                       */
  /*    length :: The range size in bytes.                                 */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    exec   :: The target execution context.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Set_CodeRange( TT_ExecContext  exec,
                    FT_Int          range,
                    void*           base,
                    FT_Long         length )
  {
    FT_ASSERT( range >= 1 && range <= 3 );

    exec->codeRangeTable[range - 1].base = (FT_Byte*)base;
    exec->codeRangeTable[range - 1].size = length;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Clear_CodeRange                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Clears a code range.                                               */
  /*                                                                       */
  /* <Input>                                                               */
  /*    range :: The code range index.                                     */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    exec  :: The target execution context.                             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Does not set the Error variable.                                   */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Clear_CodeRange( TT_ExecContext  exec,
                      FT_Int          range )
  {
    FT_ASSERT( range >= 1 && range <= 3 );

    exec->codeRangeTable[range - 1].base = NULL;
    exec->codeRangeTable[range - 1].size = 0;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /*                   EXECUTION CONTEXT ROUTINES                          */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Done_Context                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Destroys a given context.                                          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    exec   :: A handle to the target execution context.                */
  /*                                                                       */
  /*    memory :: A handle to the parent memory object.                    */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Only the glyph loader and debugger should call this function.      */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Done_Context( TT_ExecContext  exec )
  {
    FT_Memory  memory = exec->memory;


    /* points zone */
    exec->maxPoints   = 0;
    exec->maxContours = 0;

    /* free stack */
    FT_FREE( exec->stack );
    exec->stackSize = 0;

    /* free call stack */
    FT_FREE( exec->callStack );
    exec->callSize = 0;
    exec->callTop  = 0;

    /* free glyph code range */
    FT_FREE( exec->glyphIns );
    exec->glyphSize = 0;

    exec->size = NULL;
    exec->face = NULL;

    FT_FREE( exec );

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Init_Context                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a context object.                                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory :: A handle to the parent memory object.                    */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    exec   :: A handle to the target execution context.                */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  static FT_Error
  Init_Context( TT_ExecContext  exec,
                FT_Memory       memory )
  {
    FT_Error  error;


    FT_TRACE1(( "Init_Context: new object at 0x%08p\n", exec ));

    exec->memory   = memory;
    exec->callSize = 32;

    if ( FT_NEW_ARRAY( exec->callStack, exec->callSize ) )
      goto Fail_Memory;

    /* all values in the context are set to 0 already, but this is */
    /* here as a remainder                                         */
    exec->maxPoints   = 0;
    exec->maxContours = 0;

    exec->stackSize = 0;
    exec->glyphSize = 0;

    exec->stack     = NULL;
    exec->glyphIns  = NULL;

    exec->face = NULL;
    exec->size = NULL;

    return FT_Err_Ok;

  Fail_Memory:
    FT_ERROR(( "Init_Context: not enough memory for %p\n", exec ));
    TT_Done_Context( exec );

    return error;
 }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Update_Max                                                         */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Checks the size of a buffer and reallocates it if necessary.       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory     :: A handle to the parent memory object.                */
  /*                                                                       */
  /*    multiplier :: The size in bytes of each element in the buffer.     */
  /*                                                                       */
  /*    new_max    :: The new capacity (size) of the buffer.               */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    size       :: The address of the buffer's current size expressed   */
  /*                  in elements.                                         */
  /*                                                                       */
  /*    buff       :: The address of the buffer base pointer.              */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  Update_Max( FT_Memory  memory,
              FT_ULong*  size,
              FT_Long    multiplier,
              void*      _pbuff,
              FT_ULong   new_max )
  {
    FT_Error  error;
    void**    pbuff = (void**)_pbuff;


    if ( *size < new_max )
    {
      if ( FT_REALLOC( *pbuff, *size * multiplier, new_max * multiplier ) )
        return error;
      *size = new_max;
    }

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Load_Context                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Prepare an execution context for glyph hinting.                    */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A handle to the source face object.                        */
  /*                                                                       */
  /*    size :: A handle to the source size object.                        */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    exec :: A handle to the target execution context.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Only the glyph loader and debugger should call this function.      */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Load_Context( TT_ExecContext  exec,
                   TT_Face         face,
                   TT_Size         size )
  {
    FT_Int          i;
    FT_ULong        tmp;
    TT_MaxProfile*  maxp;
    FT_Error        error;


    exec->face = face;
    maxp       = &face->max_profile;
    exec->size = size;

    if ( size )
    {
      exec->numFDefs   = size->num_function_defs;
      exec->maxFDefs   = size->max_function_defs;
      exec->numIDefs   = size->num_instruction_defs;
      exec->maxIDefs   = size->max_instruction_defs;
      exec->FDefs      = size->function_defs;
      exec->IDefs      = size->instruction_defs;
      exec->tt_metrics = size->ttmetrics;
      exec->metrics    = size->metrics;

      exec->maxFunc    = size->max_func;
      exec->maxIns     = size->max_ins;

      for ( i = 0; i < TT_MAX_CODE_RANGES; i++ )
        exec->codeRangeTable[i] = size->codeRangeTable[i];

      /* set graphics state */
      exec->GS = size->GS;

      exec->cvtSize = size->cvt_size;
      exec->cvt     = size->cvt;

      exec->storeSize = size->storage_size;
      exec->storage   = size->storage;

      exec->twilight  = size->twilight;

      /* In case of multi-threading it can happen that the old size object */
      /* no longer exists, thus we must clear all glyph zone references.   */
      ft_memset( &exec->zp0, 0, sizeof ( exec->zp0 ) );
      exec->zp1 = exec->zp0;
      exec->zp2 = exec->zp0;
    }

    /* XXX: We reserve a little more elements on the stack to deal safely */
    /*      with broken fonts like arialbs, courbs, timesbs, etc.         */
    tmp = exec->stackSize;
    error = Update_Max( exec->memory,
                        &tmp,
                        sizeof ( FT_F26Dot6 ),
                        (void*)&exec->stack,
                        maxp->maxStackElements + 32 );
    exec->stackSize = (FT_UInt)tmp;
    if ( error )
      return error;

    tmp = exec->glyphSize;
    error = Update_Max( exec->memory,
                        &tmp,
                        sizeof ( FT_Byte ),
                        (void*)&exec->glyphIns,
                        maxp->maxSizeOfInstructions );
    exec->glyphSize = (FT_UShort)tmp;
    if ( error )
      return error;

    exec->pts.n_points   = 0;
    exec->pts.n_contours = 0;

    exec->zp1 = exec->pts;
    exec->zp2 = exec->pts;
    exec->zp0 = exec->pts;

    exec->instruction_trap = FALSE;

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Save_Context                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Saves the code ranges in a `size' object.                          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    exec :: A handle to the source execution context.                  */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Only the glyph loader and debugger should call this function.      */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Save_Context( TT_ExecContext  exec,
                   TT_Size         size )
  {
    FT_Int  i;


    /* XXX: Will probably disappear soon with all the code range */
    /*      management, which is now rather obsolete.            */
    /*                                                           */
    size->num_function_defs    = exec->numFDefs;
    size->num_instruction_defs = exec->numIDefs;

    size->max_func = exec->maxFunc;
    size->max_ins  = exec->maxIns;

    for ( i = 0; i < TT_MAX_CODE_RANGES; i++ )
      size->codeRangeTable[i] = exec->codeRangeTable[i];

    return FT_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Run_Context                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Executes one or more instructions in the execution context.        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    debug :: A Boolean flag.  If set, the function sets some internal  */
  /*             variables and returns immediately, otherwise TT_RunIns()  */
  /*             is called.                                                */
  /*                                                                       */
  /*             This is commented out currently.                          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    exec  :: A handle to the target execution context.                 */
  /*                                                                       */
  /* <Return>                                                              */
  /*    TrueType error code.  0 means success.                             */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Only the glyph loader and debugger should call this function.      */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Run_Context( TT_ExecContext  exec,
                  FT_Bool         debug )
  {
    FT_Error  error;


    if ( ( error = TT_Goto_CodeRange( exec, tt_coderange_glyph, 0 ) )
           != FT_Err_Ok )
      return error;

    exec->zp0 = exec->pts;
    exec->zp1 = exec->pts;
    exec->zp2 = exec->pts;

    exec->GS.gep0 = 1;
    exec->GS.gep1 = 1;
    exec->GS.gep2 = 1;

    exec->GS.projVector.x = 0x4000;
    exec->GS.projVector.y = 0x0000;

    exec->GS.freeVector = exec->GS.projVector;
    exec->GS.dualVector = exec->GS.projVector;

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    exec->GS.both_x_axis = TRUE;
#endif

    exec->GS.round_state = 1;
    exec->GS.loop        = 1;

    /* some glyphs leave something on the stack. so we clean it */
    /* before a new execution.                                  */
    exec->top     = 0;
    exec->callTop = 0;

#if 1
    FT_UNUSED( debug );

    return exec->face->interpreter( exec );
#else
    if ( !debug )
      return TT_RunIns( exec );
    else
      return FT_Err_Ok;
#endif
  }


  /* The default value for `scan_control' is documented as FALSE in the */
  /* TrueType specification.  This is confusing since it implies a      */
  /* Boolean value.  However, this is not the case, thus both the       */
  /* default values of our `scan_type' and `scan_control' fields (which */
  /* the documentation's `scan_control' variable is split into) are     */
  /* zero.                                                              */

  const TT_GraphicsState  tt_default_graphics_state =
  {
    0, 0, 0,
    { 0x4000, 0 },
    { 0x4000, 0 },
    { 0x4000, 0 },

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    TRUE,
#endif

    1, 64, 1,
    TRUE, 68, 0, 0, 9, 3,
    0, FALSE, 0, 1, 1, 1
  };


  /* documentation is in ttinterp.h */

  FT_EXPORT_DEF( TT_ExecContext )
  TT_New_Context( TT_Driver  driver )
  {
    FT_Memory  memory = driver->root.root.memory;


    if ( !driver->context )
    {
      FT_Error        error;
      TT_ExecContext  exec;


      /* allocate object */
      if ( FT_NEW( exec ) )
        goto Fail;

      /* initialize it; in case of error this deallocates `exec' too */
      error = Init_Context( exec, memory );
      if ( error )
        goto Fail;

      /* store it into the driver */
      driver->context = exec;
    }

    return driver->context;

  Fail:
    return NULL;
  }


  /*************************************************************************/
  /*                                                                       */
  /* Before an opcode is executed, the interpreter verifies that there are */
  /* enough arguments on the stack, with the help of the `Pop_Push_Count'  */
  /* table.                                                                */
  /*                                                                       */
  /* For each opcode, the first column gives the number of arguments that  */
  /* are popped from the stack; the second one gives the number of those   */
  /* that are pushed in result.                                            */
  /*                                                                       */
  /* Opcodes which have a varying number of parameters in the data stream  */
  /* (NPUSHB, NPUSHW) are handled specially; they have a negative value in */
  /* the `opcode_length' table, and the value in `Pop_Push_Count' is set   */
  /* to zero.                                                              */
  /*                                                                       */
  /*************************************************************************/


#undef  PACK
#define PACK( x, y )  ( ( x << 4 ) | y )


  static
  const FT_Byte  Pop_Push_Count[256] =
  {
    /* opcodes are gathered in groups of 16 */
    /* please keep the spaces as they are   */

    /*  SVTCA  y  */  PACK( 0, 0 ),
    /*  SVTCA  x  */  PACK( 0, 0 ),
    /*  SPvTCA y  */  PACK( 0, 0 ),
    /*  SPvTCA x  */  PACK( 0, 0 ),
    /*  SFvTCA y  */  PACK( 0, 0 ),
    /*  SFvTCA x  */  PACK( 0, 0 ),
    /*  SPvTL //  */  PACK( 2, 0 ),
    /*  SPvTL +   */  PACK( 2, 0 ),
    /*  SFvTL //  */  PACK( 2, 0 ),
    /*  SFvTL +   */  PACK( 2, 0 ),
    /*  SPvFS     */  PACK( 2, 0 ),
    /*  SFvFS     */  PACK( 2, 0 ),
    /*  GPV       */  PACK( 0, 2 ),
    /*  GFV       */  PACK( 0, 2 ),
    /*  SFvTPv    */  PACK( 0, 0 ),
    /*  ISECT     */  PACK( 5, 0 ),

    /*  SRP0      */  PACK( 1, 0 ),
    /*  SRP1      */  PACK( 1, 0 ),
    /*  SRP2      */  PACK( 1, 0 ),
    /*  SZP0      */  PACK( 1, 0 ),
    /*  SZP1      */  PACK( 1, 0 ),
    /*  SZP2      */  PACK( 1, 0 ),
    /*  SZPS      */  PACK( 1, 0 ),
    /*  SLOOP     */  PACK( 1, 0 ),
    /*  RTG       */  PACK( 0, 0 ),
    /*  RTHG      */  PACK( 0, 0 ),
    /*  SMD       */  PACK( 1, 0 ),
    /*  ELSE      */  PACK( 0, 0 ),
    /*  JMPR      */  PACK( 1, 0 ),
    /*  SCvTCi    */  PACK( 1, 0 ),
    /*  SSwCi     */  PACK( 1, 0 ),
    /*  SSW       */  PACK( 1, 0 ),

    /*  DUP       */  PACK( 1, 2 ),
    /*  POP       */  PACK( 1, 0 ),
    /*  CLEAR     */  PACK( 0, 0 ),
    /*  SWAP      */  PACK( 2, 2 ),
    /*  DEPTH     */  PACK( 0, 1 ),
    /*  CINDEX    */  PACK( 1, 1 ),
    /*  MINDEX    */  PACK( 1, 0 ),
    /*  AlignPTS  */  PACK( 2, 0 ),
    /*  INS_$28   */  PACK( 0, 0 ),
    /*  UTP       */  PACK( 1, 0 ),
    /*  LOOPCALL  */  PACK( 2, 0 ),
    /*  CALL      */  PACK( 1, 0 ),
    /*  FDEF      */  PACK( 1, 0 ),
    /*  ENDF      */  PACK( 0, 0 ),
    /*  MDAP[0]   */  PACK( 1, 0 ),
    /*  MDAP[1]   */  PACK( 1, 0 ),

    /*  IUP[0]    */  PACK( 0, 0 ),
    /*  IUP[1]    */  PACK( 0, 0 ),
    /*  SHP[0]    */  PACK( 0, 0 ),
    /*  SHP[1]    */  PACK( 0, 0 ),
    /*  SHC[0]    */  PACK( 1, 0 ),
    /*  SHC[1]    */  PACK( 1, 0 ),
    /*  SHZ[0]    */  PACK( 1, 0 ),
    /*  SHZ[1]    */  PACK( 1, 0 ),
    /*  SHPIX     */  PACK( 1, 0 ),
    /*  IP        */  PACK( 0, 0 ),
    /*  MSIRP[0]  */  PACK( 2, 0 ),
    /*  MSIRP[1]  */  PACK( 2, 0 ),
    /*  AlignRP   */  PACK( 0, 0 ),
    /*  RTDG      */  PACK( 0, 0 ),
    /*  MIAP[0]   */  PACK( 2, 0 ),
    /*  MIAP[1]   */  PACK( 2, 0 ),

    /*  NPushB    */  PACK( 0, 0 ),
    /*  NPushW    */  PACK( 0, 0 ),
    /*  WS        */  PACK( 2, 0 ),
    /*  RS        */  PACK( 1, 1 ),
    /*  WCvtP     */  PACK( 2, 0 ),
    /*  RCvt      */  PACK( 1, 1 ),
    /*  GC[0]     */  PACK( 1, 1 ),
    /*  GC[1]     */  PACK( 1, 1 ),
    /*  SCFS      */  PACK( 2, 0 ),
    /*  MD[0]     */  PACK( 2, 1 ),
    /*  MD[1]     */  PACK( 2, 1 ),
    /*  MPPEM     */  PACK( 0, 1 ),
    /*  MPS       */  PACK( 0, 1 ),
    /*  FlipON    */  PACK( 0, 0 ),
    /*  FlipOFF   */  PACK( 0, 0 ),
    /*  DEBUG     */  PACK( 1, 0 ),

    /*  LT        */  PACK( 2, 1 ),
    /*  LTEQ      */  PACK( 2, 1 ),
    /*  GT        */  PACK( 2, 1 ),
    /*  GTEQ      */  PACK( 2, 1 ),
    /*  EQ        */  PACK( 2, 1 ),
    /*  NEQ       */  PACK( 2, 1 ),
    /*  ODD       */  PACK( 1, 1 ),
    /*  EVEN      */  PACK( 1, 1 ),
    /*  IF        */  PACK( 1, 0 ),
    /*  EIF       */  PACK( 0, 0 ),
    /*  AND       */  PACK( 2, 1 ),
    /*  OR        */  PACK( 2, 1 ),
    /*  NOT       */  PACK( 1, 1 ),
    /*  DeltaP1   */  PACK( 1, 0 ),
    /*  SDB       */  PACK( 1, 0 ),
    /*  SDS       */  PACK( 1, 0 ),

    /*  ADD       */  PACK( 2, 1 ),
    /*  SUB       */  PACK( 2, 1 ),
    /*  DIV       */  PACK( 2, 1 ),
    /*  MUL       */  PACK( 2, 1 ),
    /*  ABS       */  PACK( 1, 1 ),
    /*  NEG       */  PACK( 1, 1 ),
    /*  FLOOR     */  PACK( 1, 1 ),
    /*  CEILING   */  PACK( 1, 1 ),
    /*  ROUND[0]  */  PACK( 1, 1 ),
    /*  ROUND[1]  */  PACK( 1, 1 ),
    /*  ROUND[2]  */  PACK( 1, 1 ),
    /*  ROUND[3]  */  PACK( 1, 1 ),
    /*  NROUND[0] */  PACK( 1, 1 ),
    /*  NROUND[1] */  PACK( 1, 1 ),
    /*  NROUND[2] */  PACK( 1, 1 ),
    /*  NROUND[3] */  PACK( 1, 1 ),

    /*  WCvtF     */  PACK( 2, 0 ),
    /*  DeltaP2   */  PACK( 1, 0 ),
    /*  DeltaP3   */  PACK( 1, 0 ),
    /*  DeltaCn[0] */ PACK( 1, 0 ),
    /*  DeltaCn[1] */ PACK( 1, 0 ),
    /*  DeltaCn[2] */ PACK( 1, 0 ),
    /*  SROUND    */  PACK( 1, 0 ),
    /*  S45Round  */  PACK( 1, 0 ),
    /*  JROT      */  PACK( 2, 0 ),
    /*  JROF      */  PACK( 2, 0 ),
    /*  ROFF      */  PACK( 0, 0 ),
    /*  INS_$7B   */  PACK( 0, 0 ),
    /*  RUTG      */  PACK( 0, 0 ),
    /*  RDTG      */  PACK( 0, 0 ),
    /*  SANGW     */  PACK( 1, 0 ),
    /*  AA        */  PACK( 1, 0 ),

    /*  FlipPT    */  PACK( 0, 0 ),
    /*  FlipRgON  */  PACK( 2, 0 ),
    /*  FlipRgOFF */  PACK( 2, 0 ),
    /*  INS_$83   */  PACK( 0, 0 ),
    /*  INS_$84   */  PACK( 0, 0 ),
    /*  ScanCTRL  */  PACK( 1, 0 ),
    /*  SDPVTL[0] */  PACK( 2, 0 ),
    /*  SDPVTL[1] */  PACK( 2, 0 ),
    /*  GetINFO   */  PACK( 1, 1 ),
    /*  IDEF      */  PACK( 1, 0 ),
    /*  ROLL      */  PACK( 3, 3 ),
    /*  MAX       */  PACK( 2, 1 ),
    /*  MIN       */  PACK( 2, 1 ),
    /*  ScanTYPE  */  PACK( 1, 0 ),
    /*  InstCTRL  */  PACK( 2, 0 ),
    /*  INS_$8F   */  PACK( 0, 0 ),

    /*  INS_$90  */   PACK( 0, 0 ),
    /*  INS_$91  */   PACK( 0, 0 ),
    /*  INS_$92  */   PACK( 0, 0 ),
    /*  INS_$93  */   PACK( 0, 0 ),
    /*  INS_$94  */   PACK( 0, 0 ),
    /*  INS_$95  */   PACK( 0, 0 ),
    /*  INS_$96  */   PACK( 0, 0 ),
    /*  INS_$97  */   PACK( 0, 0 ),
    /*  INS_$98  */   PACK( 0, 0 ),
    /*  INS_$99  */   PACK( 0, 0 ),
    /*  INS_$9A  */   PACK( 0, 0 ),
    /*  INS_$9B  */   PACK( 0, 0 ),
    /*  INS_$9C  */   PACK( 0, 0 ),
    /*  INS_$9D  */   PACK( 0, 0 ),
    /*  INS_$9E  */   PACK( 0, 0 ),
    /*  INS_$9F  */   PACK( 0, 0 ),

    /*  INS_$A0  */   PACK( 0, 0 ),
    /*  INS_$A1  */   PACK( 0, 0 ),
    /*  INS_$A2  */   PACK( 0, 0 ),
    /*  INS_$A3  */   PACK( 0, 0 ),
    /*  INS_$A4  */   PACK( 0, 0 ),
    /*  INS_$A5  */   PACK( 0, 0 ),
    /*  INS_$A6  */   PACK( 0, 0 ),
    /*  INS_$A7  */   PACK( 0, 0 ),
    /*  INS_$A8  */   PACK( 0, 0 ),
    /*  INS_$A9  */   PACK( 0, 0 ),
    /*  INS_$AA  */   PACK( 0, 0 ),
    /*  INS_$AB  */   PACK( 0, 0 ),
    /*  INS_$AC  */   PACK( 0, 0 ),
    /*  INS_$AD  */   PACK( 0, 0 ),
    /*  INS_$AE  */   PACK( 0, 0 ),
    /*  INS_$AF  */   PACK( 0, 0 ),

    /*  PushB[0]  */  PACK( 0, 1 ),
    /*  PushB[1]  */  PACK( 0, 2 ),
    /*  PushB[2]  */  PACK( 0, 3 ),
    /*  PushB[3]  */  PACK( 0, 4 ),
    /*  PushB[4]  */  PACK( 0, 5 ),
    /*  PushB[5]  */  PACK( 0, 6 ),
    /*  PushB[6]  */  PACK( 0, 7 ),
    /*  PushB[7]  */  PACK( 0, 8 ),
    /*  PushW[0]  */  PACK( 0, 1 ),
    /*  PushW[1]  */  PACK( 0, 2 ),
    /*  PushW[2]  */  PACK( 0, 3 ),
    /*  PushW[3]  */  PACK( 0, 4 ),
    /*  PushW[4]  */  PACK( 0, 5 ),
    /*  PushW[5]  */  PACK( 0, 6 ),
    /*  PushW[6]  */  PACK( 0, 7 ),
    /*  PushW[7]  */  PACK( 0, 8 ),

    /*  MDRP[00]  */  PACK( 1, 0 ),
    /*  MDRP[01]  */  PACK( 1, 0 ),
    /*  MDRP[02]  */  PACK( 1, 0 ),
    /*  MDRP[03]  */  PACK( 1, 0 ),
    /*  MDRP[04]  */  PACK( 1, 0 ),
    /*  MDRP[05]  */  PACK( 1, 0 ),
    /*  MDRP[06]  */  PACK( 1, 0 ),
    /*  MDRP[07]  */  PACK( 1, 0 ),
    /*  MDRP[08]  */  PACK( 1, 0 ),
    /*  MDRP[09]  */  PACK( 1, 0 ),
    /*  MDRP[10]  */  PACK( 1, 0 ),
    /*  MDRP[11]  */  PACK( 1, 0 ),
    /*  MDRP[12]  */  PACK( 1, 0 ),
    /*  MDRP[13]  */  PACK( 1, 0 ),
    /*  MDRP[14]  */  PACK( 1, 0 ),
    /*  MDRP[15]  */  PACK( 1, 0 ),

    /*  MDRP[16]  */  PACK( 1, 0 ),
    /*  MDRP[17]  */  PACK( 1, 0 ),
    /*  MDRP[18]  */  PACK( 1, 0 ),
    /*  MDRP[19]  */  PACK( 1, 0 ),
    /*  MDRP[20]  */  PACK( 1, 0 ),
    /*  MDRP[21]  */  PACK( 1, 0 ),
    /*  MDRP[22]  */  PACK( 1, 0 ),
    /*  MDRP[23]  */  PACK( 1, 0 ),
    /*  MDRP[24]  */  PACK( 1, 0 ),
    /*  MDRP[25]  */  PACK( 1, 0 ),
    /*  MDRP[26]  */  PACK( 1, 0 ),
    /*  MDRP[27]  */  PACK( 1, 0 ),
    /*  MDRP[28]  */  PACK( 1, 0 ),
    /*  MDRP[29]  */  PACK( 1, 0 ),
    /*  MDRP[30]  */  PACK( 1, 0 ),
    /*  MDRP[31]  */  PACK( 1, 0 ),

    /*  MIRP[00]  */  PACK( 2, 0 ),
    /*  MIRP[01]  */  PACK( 2, 0 ),
    /*  MIRP[02]  */  PACK( 2, 0 ),
    /*  MIRP[03]  */  PACK( 2, 0 ),
    /*  MIRP[04]  */  PACK( 2, 0 ),
    /*  MIRP[05]  */  PACK( 2, 0 ),
    /*  MIRP[06]  */  PACK( 2, 0 ),
    /*  MIRP[07]  */  PACK( 2, 0 ),
    /*  MIRP[08]  */  PACK( 2, 0 ),
    /*  MIRP[09]  */  PACK( 2, 0 ),
    /*  MIRP[10]  */  PACK( 2, 0 ),
    /*  MIRP[11]  */  PACK( 2, 0 ),
    /*  MIRP[12]  */  PACK( 2, 0 ),
    /*  MIRP[13]  */  PACK( 2, 0 ),
    /*  MIRP[14]  */  PACK( 2, 0 ),
    /*  MIRP[15]  */  PACK( 2, 0 ),

    /*  MIRP[16]  */  PACK( 2, 0 ),
    /*  MIRP[17]  */  PACK( 2, 0 ),
    /*  MIRP[18]  */  PACK( 2, 0 ),
    /*  MIRP[19]  */  PACK( 2, 0 ),
    /*  MIRP[20]  */  PACK( 2, 0 ),
    /*  MIRP[21]  */  PACK( 2, 0 ),
    /*  MIRP[22]  */  PACK( 2, 0 ),
    /*  MIRP[23]  */  PACK( 2, 0 ),
    /*  MIRP[24]  */  PACK( 2, 0 ),
    /*  MIRP[25]  */  PACK( 2, 0 ),
    /*  MIRP[26]  */  PACK( 2, 0 ),
    /*  MIRP[27]  */  PACK( 2, 0 ),
    /*  MIRP[28]  */  PACK( 2, 0 ),
    /*  MIRP[29]  */  PACK( 2, 0 ),
    /*  MIRP[30]  */  PACK( 2, 0 ),
    /*  MIRP[31]  */  PACK( 2, 0 )
  };


#ifdef FT_DEBUG_LEVEL_TRACE

  static
  const char*  const opcode_name[256] =
  {
    "SVTCA y",
    "SVTCA x",
    "SPvTCA y",
    "SPvTCA x",
    "SFvTCA y",
    "SFvTCA x",
    "SPvTL ||",
    "SPvTL +",
    "SFvTL ||",
    "SFvTL +",
    "SPvFS",
    "SFvFS",
    "GPV",
    "GFV",
    "SFvTPv",
    "ISECT",

    "SRP0",
    "SRP1",
    "SRP2",
    "SZP0",
    "SZP1",
    "SZP2",
    "SZPS",
    "SLOOP",
    "RTG",
    "RTHG",
    "SMD",
    "ELSE",
    "JMPR",
    "SCvTCi",
    "SSwCi",
    "SSW",

    "DUP",
    "POP",
    "CLEAR",
    "SWAP",
    "DEPTH",
    "CINDEX",
    "MINDEX",
    "AlignPTS",
    "INS_$28",
    "UTP",
    "LOOPCALL",
    "CALL",
    "FDEF",
    "ENDF",
    "MDAP[0]",
    "MDAP[1]",

    "IUP[0]",
    "IUP[1]",
    "SHP[0]",
    "SHP[1]",
    "SHC[0]",
    "SHC[1]",
    "SHZ[0]",
    "SHZ[1]",
    "SHPIX",
    "IP",
    "MSIRP[0]",
    "MSIRP[1]",
    "AlignRP",
    "RTDG",
    "MIAP[0]",
    "MIAP[1]",

    "NPushB",
    "NPushW",
    "WS",
    "RS",
    "WCvtP",
    "RCvt",
    "GC[0]",
    "GC[1]",
    "SCFS",
    "MD[0]",
    "MD[1]",
    "MPPEM",
    "MPS",
    "FlipON",
    "FlipOFF",
    "DEBUG",

    "LT",
    "LTEQ",
    "GT",
    "GTEQ",
    "EQ",
    "NEQ",
    "ODD",
    "EVEN",
    "IF",
    "EIF",
    "AND",
    "OR",
    "NOT",
    "DeltaP1",
    "SDB",
    "SDS",

    "ADD",
    "SUB",
    "DIV",
    "MUL",
    "ABS",
    "NEG",
    "FLOOR",
    "CEILING",
    "ROUND[0]",
    "ROUND[1]",
    "ROUND[2]",
    "ROUND[3]",
    "NROUND[0]",
    "NROUND[1]",
    "NROUND[2]",
    "NROUND[3]",

    "WCvtF",
    "DeltaP2",
    "DeltaP3",
    "DeltaCn[0]",
    "DeltaCn[1]",
    "DeltaCn[2]",
    "SROUND",
    "S45Round",
    "JROT",
    "JROF",
    "ROFF",
    "INS_$7B",
    "RUTG",
    "RDTG",
    "SANGW",
    "AA",

    "FlipPT",
    "FlipRgON",
    "FlipRgOFF",
    "INS_$83",
    "INS_$84",
    "ScanCTRL",
    "SDVPTL[0]",
    "SDVPTL[1]",
    "GetINFO",
    "IDEF",
    "ROLL",
    "MAX",
    "MIN",
    "ScanTYPE",
    "InstCTRL",
    "INS_$8F",

    "INS_$90",
    "INS_$91",
    "INS_$92",
    "INS_$93",
    "INS_$94",
    "INS_$95",
    "INS_$96",
    "INS_$97",
    "INS_$98",
    "INS_$99",
    "INS_$9A",
    "INS_$9B",
    "INS_$9C",
    "INS_$9D",
    "INS_$9E",
    "INS_$9F",

    "INS_$A0",
    "INS_$A1",
    "INS_$A2",
    "INS_$A3",
    "INS_$A4",
    "INS_$A5",
    "INS_$A6",
    "INS_$A7",
    "INS_$A8",
    "INS_$A9",
    "INS_$AA",
    "INS_$AB",
    "INS_$AC",
    "INS_$AD",
    "INS_$AE",
    "INS_$AF",

    "PushB[0]",
    "PushB[1]",
    "PushB[2]",
    "PushB[3]",
    "PushB[4]",
    "PushB[5]",
    "PushB[6]",
    "PushB[7]",
    "PushW[0]",
    "PushW[1]",
    "PushW[2]",
    "PushW[3]",
    "PushW[4]",
    "PushW[5]",
    "PushW[6]",
    "PushW[7]",

    "MDRP[00]",
    "MDRP[01]",
    "MDRP[02]",
    "MDRP[03]",
    "MDRP[04]",
    "MDRP[05]",
    "MDRP[06]",
    "MDRP[07]",
    "MDRP[08]",
    "MDRP[09]",
    "MDRP[10]",
    "MDRP[11]",
    "MDRP[12]",
    "MDRP[13]",
    "MDRP[14]",
    "MDRP[15]",

    "MDRP[16]",
    "MDRP[17]",
    "MDRP[18]",
    "MDRP[19]",
    "MDRP[20]",
    "MDRP[21]",
    "MDRP[22]",
    "MDRP[23]",
    "MDRP[24]",
    "MDRP[25]",
    "MDRP[26]",
    "MDRP[27]",
    "MDRP[28]",
    "MDRP[29]",
    "MDRP[30]",
    "MDRP[31]",

    "MIRP[00]",
    "MIRP[01]",
    "MIRP[02]",
    "MIRP[03]",
    "MIRP[04]",
    "MIRP[05]",
    "MIRP[06]",
    "MIRP[07]",
    "MIRP[08]",
    "MIRP[09]",
    "MIRP[10]",
    "MIRP[11]",
    "MIRP[12]",
    "MIRP[13]",
    "MIRP[14]",
    "MIRP[15]",

    "MIRP[16]",
    "MIRP[17]",
    "MIRP[18]",
    "MIRP[19]",
    "MIRP[20]",
    "MIRP[21]",
    "MIRP[22]",
    "MIRP[23]",
    "MIRP[24]",
    "MIRP[25]",
    "MIRP[26]",
    "MIRP[27]",
    "MIRP[28]",
    "MIRP[29]",
    "MIRP[30]",
    "MIRP[31]"
  };

#endif /* FT_DEBUG_LEVEL_TRACE */


  static
  const FT_Char  opcode_length[256] =
  {
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,

   -1,-2, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,

    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    2, 3, 4, 5,  6, 7, 8, 9,  3, 5, 7, 9, 11,13,15,17,

    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1
  };

#undef PACK


#ifndef FT_CONFIG_OPTION_NO_ASSEMBLER

#if defined( __arm__ )                                 && \
    ( defined( __thumb2__ ) || !defined( __thumb__ ) )

#define TT_MulFix14  TT_MulFix14_arm

  static FT_Int32
  TT_MulFix14_arm( FT_Int32  a,
                   FT_Int    b )
  {
    register FT_Int32  t, t2;


#if defined( __CC_ARM ) || defined( __ARMCC__ )

    __asm
    {
      smull t2, t,  b,  a           /* (lo=t2,hi=t) = a*b */
      mov   a,  t,  asr #31         /* a   = (hi >> 31) */
      add   a,  a,  #0x2000         /* a  += 0x2000 */
      adds  t2, t2, a               /* t2 += a */
      adc   t,  t,  #0              /* t  += carry */
      mov   a,  t2, lsr #14         /* a   = t2 >> 14 */
      orr   a,  a,  t,  lsl #18     /* a  |= t << 18 */
    }

#elif defined( __GNUC__ )

    __asm__ __volatile__ (
      "smull  %1, %2, %4, %3\n\t"       /* (lo=%1,hi=%2) = a*b */
      "mov    %0, %2, asr #31\n\t"      /* %0  = (hi >> 31) */
#if defined( __clang__ ) && defined( __thumb2__ )
      "add.w  %0, %0, #0x2000\n\t"      /* %0 += 0x2000 */
#else
      "add    %0, %0, #0x2000\n\t"      /* %0 += 0x2000 */
#endif
      "adds   %1, %1, %0\n\t"           /* %1 += %0 */
      "adc    %2, %2, #0\n\t"           /* %2 += carry */
      "mov    %0, %1, lsr #14\n\t"      /* %0  = %1 >> 16 */
      "orr    %0, %0, %2, lsl #18\n\t"  /* %0 |= %2 << 16 */
      : "=r"(a), "=&r"(t2), "=&r"(t)
      : "r"(a), "r"(b)
      : "cc" );

#endif

    return a;
  }

#endif /* __arm__ && ( __thumb2__ || !__thumb__ ) */

#endif /* !FT_CONFIG_OPTION_NO_ASSEMBLER */


#if defined( __GNUC__ )                              && \
    ( defined( __i386__ ) || defined( __x86_64__ ) )

#define TT_MulFix14  TT_MulFix14_long_long

  /* Temporarily disable the warning that C90 doesn't support `long long'. */
#if ( __GNUC__ * 100 + __GNUC_MINOR__ ) >= 406
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wlong-long"

  /* This is declared `noinline' because inlining the function results */
  /* in slower code.  The `pure' attribute indicates that the result   */
  /* only depends on the parameters.                                   */
  static __attribute__(( noinline ))
         __attribute__(( pure )) FT_Int32
  TT_MulFix14_long_long( FT_Int32  a,
                         FT_Int    b )
  {

    long long  ret = (long long)a * b;

    /* The following line assumes that right shifting of signed values */
    /* will actually preserve the sign bit.  The exact behaviour is    */
    /* undefined, but this is true on x86 and x86_64.                  */
    long long  tmp = ret >> 63;


    ret += 0x2000 + tmp;

    return (FT_Int32)( ret >> 14 );
  }

#if ( __GNUC__ * 100 + __GNUC_MINOR__ ) >= 406
#pragma GCC diagnostic pop
#endif

#endif /* __GNUC__ && ( __i386__ || __x86_64__ ) */


#ifndef TT_MulFix14

  /* Compute (a*b)/2^14 with maximum accuracy and rounding.  */
  /* This is optimized to be faster than calling FT_MulFix() */
  /* for platforms where sizeof(int) == 2.                   */
  static FT_Int32
  TT_MulFix14( FT_Int32  a,
               FT_Int    b )
  {
    FT_Int32   sign;
    FT_UInt32  ah, al, mid, lo, hi;


    sign = a ^ b;

    if ( a < 0 )
      a = -a;
    if ( b < 0 )
      b = -b;

    ah = (FT_UInt32)( ( a >> 16 ) & 0xFFFFU );
    al = (FT_UInt32)( a & 0xFFFFU );

    lo    = al * b;
    mid   = ah * b;
    hi    = mid >> 16;
    mid   = ( mid << 16 ) + ( 1 << 13 ); /* rounding */
    lo   += mid;
    if ( lo < mid )
      hi += 1;

    mid = ( lo >> 14 ) | ( hi << 18 );

    return sign >= 0 ? (FT_Int32)mid : -(FT_Int32)mid;
  }

#endif  /* !TT_MulFix14 */


#if defined( __GNUC__ )        && \
    ( defined( __i386__ )   ||    \
      defined( __x86_64__ ) ||    \
      defined( __arm__ )    )

#define TT_DotFix14  TT_DotFix14_long_long

#if ( __GNUC__ * 100 + __GNUC_MINOR__ ) >= 406
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wlong-long"

  static __attribute__(( pure )) FT_Int32
  TT_DotFix14_long_long( FT_Int32  ax,
                         FT_Int32  ay,
                         FT_Int    bx,
                         FT_Int    by )
  {
    /* Temporarily disable the warning that C90 doesn't support */
    /* `long long'.                                             */

    long long  temp1 = (long long)ax * bx;
    long long  temp2 = (long long)ay * by;


    temp1 += temp2;
    temp2  = temp1 >> 63;
    temp1 += 0x2000 + temp2;

    return (FT_Int32)( temp1 >> 14 );

  }

#if ( __GNUC__ * 100 + __GNUC_MINOR__ ) >= 406
#pragma GCC diagnostic pop
#endif

#endif /* __GNUC__ && (__arm__ || __i386__ || __x86_64__) */


#ifndef TT_DotFix14

  /* compute (ax*bx+ay*by)/2^14 with maximum accuracy and rounding */
  static FT_Int32
  TT_DotFix14( FT_Int32  ax,
               FT_Int32  ay,
               FT_Int    bx,
               FT_Int    by )
  {
    FT_Int32   m, s, hi1, hi2, hi;
    FT_UInt32  l, lo1, lo2, lo;


    /* compute ax*bx as 64-bit value */
    l = (FT_UInt32)( ( ax & 0xFFFFU ) * bx );
    m = ( ax >> 16 ) * bx;

    lo1 = l + ( (FT_UInt32)m << 16 );
    hi1 = ( m >> 16 ) + ( (FT_Int32)l >> 31 ) + ( lo1 < l );

    /* compute ay*by as 64-bit value */
    l = (FT_UInt32)( ( ay & 0xFFFFU ) * by );
    m = ( ay >> 16 ) * by;

    lo2 = l + ( (FT_UInt32)m << 16 );
    hi2 = ( m >> 16 ) + ( (FT_Int32)l >> 31 ) + ( lo2 < l );

    /* add them */
    lo = lo1 + lo2;
    hi = hi1 + hi2 + ( lo < lo1 );

    /* divide the result by 2^14 with rounding */
    s   = hi >> 31;
    l   = lo + (FT_UInt32)s;
    hi += s + ( l < lo );
    lo  = l;

    l   = lo + 0x2000U;
    hi += ( l < lo );

    return (FT_Int32)( ( (FT_UInt32)hi << 18 ) | ( l >> 14 ) );
  }

#endif /* TT_DotFix14 */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Current_Ratio                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Returns the current aspect ratio scaling factor depending on the   */
  /*    projection vector's state and device resolutions.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The aspect ratio in 16.16 format, always <= 1.0 .                  */
  /*                                                                       */
  static FT_Long
  Current_Ratio( EXEC_OP )
  {
    if ( !CUR.tt_metrics.ratio )
    {
#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
      if ( CUR.face->unpatented_hinting )
      {
        if ( CUR.GS.both_x_axis )
          CUR.tt_metrics.ratio = CUR.tt_metrics.x_ratio;
        else
          CUR.tt_metrics.ratio = CUR.tt_metrics.y_ratio;
      }
      else
#endif
      {
        if ( CUR.GS.projVector.y == 0 )
          CUR.tt_metrics.ratio = CUR.tt_metrics.x_ratio;

        else if ( CUR.GS.projVector.x == 0 )
          CUR.tt_metrics.ratio = CUR.tt_metrics.y_ratio;

        else
        {
          FT_F26Dot6  x, y;


          x = TT_MulFix14( CUR.tt_metrics.x_ratio,
                           CUR.GS.projVector.x );
          y = TT_MulFix14( CUR.tt_metrics.y_ratio,
                           CUR.GS.projVector.y );
          CUR.tt_metrics.ratio = FT_Hypot( x, y );
        }
      }
    }
    return CUR.tt_metrics.ratio;
  }


  static FT_Long
  Current_Ppem( EXEC_OP )
  {
    return FT_MulFix( CUR.tt_metrics.ppem, CURRENT_Ratio() );
  }


  /*************************************************************************/
  /*                                                                       */
  /* Functions related to the control value table (CVT).                   */
  /*                                                                       */
  /*************************************************************************/


  FT_CALLBACK_DEF( FT_F26Dot6 )
  Read_CVT( EXEC_OP_ FT_ULong  idx )
  {
    return CUR.cvt[idx];
  }


  FT_CALLBACK_DEF( FT_F26Dot6 )
  Read_CVT_Stretched( EXEC_OP_ FT_ULong  idx )
  {
    return FT_MulFix( CUR.cvt[idx], CURRENT_Ratio() );
  }


  FT_CALLBACK_DEF( void )
  Write_CVT( EXEC_OP_ FT_ULong    idx,
                      FT_F26Dot6  value )
  {
    CUR.cvt[idx] = value;
  }


  FT_CALLBACK_DEF( void )
  Write_CVT_Stretched( EXEC_OP_ FT_ULong    idx,
                                FT_F26Dot6  value )
  {
    CUR.cvt[idx] = FT_DivFix( value, CURRENT_Ratio() );
  }


  FT_CALLBACK_DEF( void )
  Move_CVT( EXEC_OP_ FT_ULong    idx,
                     FT_F26Dot6  value )
  {
    CUR.cvt[idx] += value;
  }


  FT_CALLBACK_DEF( void )
  Move_CVT_Stretched( EXEC_OP_ FT_ULong    idx,
                               FT_F26Dot6  value )
  {
    CUR.cvt[idx] += FT_DivFix( value, CURRENT_Ratio() );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    GetShortIns                                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Returns a short integer taken from the instruction stream at       */
  /*    address IP.                                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Short read at code[IP].                                            */
  /*                                                                       */
  /* <Note>                                                                */
  /*    This one could become a macro.                                     */
  /*                                                                       */
  static FT_Short
  GetShortIns( EXEC_OP )
  {
    /* Reading a byte stream so there is no endianess (DaveP) */
    CUR.IP += 2;
    return (FT_Short)( ( CUR.code[CUR.IP - 2] << 8 ) +
                         CUR.code[CUR.IP - 1]      );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Ins_Goto_CodeRange                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Goes to a certain code range in the instruction stream.            */
  /*                                                                       */
  /* <Input>                                                               */
  /*    aRange :: The index of the code range.                             */
  /*                                                                       */
  /*    aIP    :: The new IP address in the code range.                    */
  /*                                                                       */
  /* <Return>                                                              */
  /*    SUCCESS or FAILURE.                                                */
  /*                                                                       */
  static FT_Bool
  Ins_Goto_CodeRange( EXEC_OP_ FT_Int    aRange,
                               FT_ULong  aIP )
  {
    TT_CodeRange*  range;


    if ( aRange < 1 || aRange > 3 )
    {
      CUR.error = FT_THROW( Bad_Argument );
      return FAILURE;
    }

    range = &CUR.codeRangeTable[aRange - 1];

    if ( range->base == NULL )     /* invalid coderange */
    {
      CUR.error = FT_THROW( Invalid_CodeRange );
      return FAILURE;
    }

    /* NOTE: Because the last instruction of a program may be a CALL */
    /*       which will return to the first byte *after* the code    */
    /*       range, we test for aIP <= Size, instead of aIP < Size.  */

    if ( aIP > range->size )
    {
      CUR.error = FT_THROW( Code_Overflow );
      return FAILURE;
    }

    CUR.code     = range->base;
    CUR.codeSize = range->size;
    CUR.IP       = aIP;
    CUR.curRange = aRange;

    return SUCCESS;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Direct_Move                                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Moves a point by a given distance along the freedom vector.  The   */
  /*    point will be `touched'.                                           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    point    :: The index of the point to move.                        */
  /*                                                                       */
  /*    distance :: The distance to apply.                                 */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    zone     :: The affected glyph zone.                               */
  /*                                                                       */
  static void
  Direct_Move( EXEC_OP_ TT_GlyphZone  zone,
                        FT_UShort     point,
                        FT_F26Dot6    distance )
  {
    FT_F26Dot6  v;


#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    FT_ASSERT( !CUR.face->unpatented_hinting );
#endif

    v = CUR.GS.freeVector.x;

    if ( v != 0 )
    {
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      if ( !SUBPIXEL_HINTING                                     ||
           ( !CUR.ignore_x_mode                                ||
             ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_X_DMOVE ) ) )
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
        zone->cur[point].x += FT_MulDiv( distance, v, CUR.F_dot_P );

      zone->tags[point] |= FT_CURVE_TAG_TOUCH_X;
    }

    v = CUR.GS.freeVector.y;

    if ( v != 0 )
    {
      zone->cur[point].y += FT_MulDiv( distance, v, CUR.F_dot_P );

      zone->tags[point] |= FT_CURVE_TAG_TOUCH_Y;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Direct_Move_Orig                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Moves the *original* position of a point by a given distance along */
  /*    the freedom vector.  Obviously, the point will not be `touched'.   */
  /*                                                                       */
  /* <Input>                                                               */
  /*    point    :: The index of the point to move.                        */
  /*                                                                       */
  /*    distance :: The distance to apply.                                 */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    zone     :: The affected glyph zone.                               */
  /*                                                                       */
  static void
  Direct_Move_Orig( EXEC_OP_ TT_GlyphZone  zone,
                             FT_UShort     point,
                             FT_F26Dot6    distance )
  {
    FT_F26Dot6  v;


#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    FT_ASSERT( !CUR.face->unpatented_hinting );
#endif

    v = CUR.GS.freeVector.x;

    if ( v != 0 )
      zone->org[point].x += FT_MulDiv( distance, v, CUR.F_dot_P );

    v = CUR.GS.freeVector.y;

    if ( v != 0 )
      zone->org[point].y += FT_MulDiv( distance, v, CUR.F_dot_P );
  }


  /*************************************************************************/
  /*                                                                       */
  /* Special versions of Direct_Move()                                     */
  /*                                                                       */
  /*   The following versions are used whenever both vectors are both      */
  /*   along one of the coordinate unit vectors, i.e. in 90% of the cases. */
  /*                                                                       */
  /*************************************************************************/


  static void
  Direct_Move_X( EXEC_OP_ TT_GlyphZone  zone,
                          FT_UShort     point,
                          FT_F26Dot6    distance )
  {
    FT_UNUSED_EXEC;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( !SUBPIXEL_HINTING  ||
         !CUR.ignore_x_mode )
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
      zone->cur[point].x += distance;

    zone->tags[point]  |= FT_CURVE_TAG_TOUCH_X;
  }


  static void
  Direct_Move_Y( EXEC_OP_ TT_GlyphZone  zone,
                          FT_UShort     point,
                          FT_F26Dot6    distance )
  {
    FT_UNUSED_EXEC;

    zone->cur[point].y += distance;
    zone->tags[point]  |= FT_CURVE_TAG_TOUCH_Y;
  }


  /*************************************************************************/
  /*                                                                       */
  /* Special versions of Direct_Move_Orig()                                */
  /*                                                                       */
  /*   The following versions are used whenever both vectors are both      */
  /*   along one of the coordinate unit vectors, i.e. in 90% of the cases. */
  /*                                                                       */
  /*************************************************************************/


  static void
  Direct_Move_Orig_X( EXEC_OP_ TT_GlyphZone  zone,
                               FT_UShort     point,
                               FT_F26Dot6    distance )
  {
    FT_UNUSED_EXEC;

    zone->org[point].x += distance;
  }


  static void
  Direct_Move_Orig_Y( EXEC_OP_ TT_GlyphZone  zone,
                               FT_UShort     point,
                               FT_F26Dot6    distance )
  {
    FT_UNUSED_EXEC;

    zone->org[point].y += distance;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_None                                                         */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Does not round, but adds engine compensation.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance (not) to round.                       */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The compensated distance.                                          */
  /*                                                                       */
  /* <Note>                                                                */
  /*    The TrueType specification says very few about the relationship    */
  /*    between rounding and engine compensation.  However, it seems from  */
  /*    the description of super round that we should add the compensation */
  /*    before rounding.                                                   */
  /*                                                                       */
  static FT_F26Dot6
  Round_None( EXEC_OP_ FT_F26Dot6  distance,
                       FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;

    FT_UNUSED_EXEC;


    if ( distance >= 0 )
    {
      val = distance + compensation;
      if ( distance && val < 0 )
        val = 0;
    }
    else
    {
      val = distance - compensation;
      if ( val > 0 )
        val = 0;
    }
    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_To_Grid                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Rounds value to grid after adding engine compensation.             */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  static FT_F26Dot6
  Round_To_Grid( EXEC_OP_ FT_F26Dot6  distance,
                          FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;

    FT_UNUSED_EXEC;


    if ( distance >= 0 )
    {
      val = distance + compensation + 32;
      if ( distance && val > 0 )
        val &= ~63;
      else
        val = 0;
    }
    else
    {
      val = -FT_PIX_ROUND( compensation - distance );
      if ( val > 0 )
        val = 0;
    }

    return  val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_To_Half_Grid                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Rounds value to half grid after adding engine compensation.        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  static FT_F26Dot6
  Round_To_Half_Grid( EXEC_OP_ FT_F26Dot6  distance,
                               FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;

    FT_UNUSED_EXEC;


    if ( distance >= 0 )
    {
      val = FT_PIX_FLOOR( distance + compensation ) + 32;
      if ( distance && val < 0 )
        val = 0;
    }
    else
    {
      val = -( FT_PIX_FLOOR( compensation - distance ) + 32 );
      if ( val > 0 )
        val = 0;
    }

    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_Down_To_Grid                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Rounds value down to grid after adding engine compensation.        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  static FT_F26Dot6
  Round_Down_To_Grid( EXEC_OP_ FT_F26Dot6  distance,
                               FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;

    FT_UNUSED_EXEC;


    if ( distance >= 0 )
    {
      val = distance + compensation;
      if ( distance && val > 0 )
        val &= ~63;
      else
        val = 0;
    }
    else
    {
      val = -( ( compensation - distance ) & -64 );
      if ( val > 0 )
        val = 0;
    }

    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_Up_To_Grid                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Rounds value up to grid after adding engine compensation.          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  static FT_F26Dot6
  Round_Up_To_Grid( EXEC_OP_ FT_F26Dot6  distance,
                             FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;

    FT_UNUSED_EXEC;


    if ( distance >= 0 )
    {
      val = distance + compensation + 63;
      if ( distance && val > 0 )
        val &= ~63;
      else
        val = 0;
    }
    else
    {
      val = -FT_PIX_CEIL( compensation - distance );
      if ( val > 0 )
        val = 0;
    }

    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_To_Double_Grid                                               */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Rounds value to double grid after adding engine compensation.      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  static FT_F26Dot6
  Round_To_Double_Grid( EXEC_OP_ FT_F26Dot6  distance,
                                 FT_F26Dot6  compensation )
  {
    FT_F26Dot6 val;

    FT_UNUSED_EXEC;


    if ( distance >= 0 )
    {
      val = distance + compensation + 16;
      if ( distance && val > 0 )
        val &= ~31;
      else
        val = 0;
    }
    else
    {
      val = -FT_PAD_ROUND( compensation - distance, 32 );
      if ( val > 0 )
        val = 0;
    }

    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_Super                                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Super-rounds value to grid after adding engine compensation.       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  /* <Note>                                                                */
  /*    The TrueType specification says very few about the relationship    */
  /*    between rounding and engine compensation.  However, it seems from  */
  /*    the description of super round that we should add the compensation */
  /*    before rounding.                                                   */
  /*                                                                       */
  static FT_F26Dot6
  Round_Super( EXEC_OP_ FT_F26Dot6  distance,
                        FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;


    if ( distance >= 0 )
    {
      val = ( distance - CUR.phase + CUR.threshold + compensation ) &
              -CUR.period;
      if ( distance && val < 0 )
        val = 0;
      val += CUR.phase;
    }
    else
    {
      val = -( ( CUR.threshold - CUR.phase - distance + compensation ) &
               -CUR.period );
      if ( val > 0 )
        val = 0;
      val -= CUR.phase;
    }

    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Round_Super_45                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Super-rounds value to grid after adding engine compensation.       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    distance     :: The distance to round.                             */
  /*                                                                       */
  /*    compensation :: The engine compensation.                           */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Rounded distance.                                                  */
  /*                                                                       */
  /* <Note>                                                                */
  /*    There is a separate function for Round_Super_45() as we may need   */
  /*    greater precision.                                                 */
  /*                                                                       */
  static FT_F26Dot6
  Round_Super_45( EXEC_OP_ FT_F26Dot6  distance,
                           FT_F26Dot6  compensation )
  {
    FT_F26Dot6  val;


    if ( distance >= 0 )
    {
      val = ( ( distance - CUR.phase + CUR.threshold + compensation ) /
                CUR.period ) * CUR.period;
      if ( distance && val < 0 )
        val = 0;
      val += CUR.phase;
    }
    else
    {
      val = -( ( ( CUR.threshold - CUR.phase - distance + compensation ) /
                   CUR.period ) * CUR.period );
      if ( val > 0 )
        val = 0;
      val -= CUR.phase;
    }

    return val;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Compute_Round                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Sets the rounding mode.                                            */
  /*                                                                       */
  /* <Input>                                                               */
  /*    round_mode :: The rounding mode to be used.                        */
  /*                                                                       */
  static void
  Compute_Round( EXEC_OP_ FT_Byte  round_mode )
  {
    switch ( round_mode )
    {
    case TT_Round_Off:
      CUR.func_round = (TT_Round_Func)Round_None;
      break;

    case TT_Round_To_Grid:
      CUR.func_round = (TT_Round_Func)Round_To_Grid;
      break;

    case TT_Round_Up_To_Grid:
      CUR.func_round = (TT_Round_Func)Round_Up_To_Grid;
      break;

    case TT_Round_Down_To_Grid:
      CUR.func_round = (TT_Round_Func)Round_Down_To_Grid;
      break;

    case TT_Round_To_Half_Grid:
      CUR.func_round = (TT_Round_Func)Round_To_Half_Grid;
      break;

    case TT_Round_To_Double_Grid:
      CUR.func_round = (TT_Round_Func)Round_To_Double_Grid;
      break;

    case TT_Round_Super:
      CUR.func_round = (TT_Round_Func)Round_Super;
      break;

    case TT_Round_Super_45:
      CUR.func_round = (TT_Round_Func)Round_Super_45;
      break;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    SetSuperRound                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Sets Super Round parameters.                                       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    GridPeriod :: The grid period.                                     */
  /*                                                                       */
  /*    selector   :: The SROUND opcode.                                   */
  /*                                                                       */
  static void
  SetSuperRound( EXEC_OP_ FT_F26Dot6  GridPeriod,
                          FT_Long     selector )
  {
    switch ( (FT_Int)( selector & 0xC0 ) )
    {
      case 0:
        CUR.period = GridPeriod / 2;
        break;

      case 0x40:
        CUR.period = GridPeriod;
        break;

      case 0x80:
        CUR.period = GridPeriod * 2;
        break;

      /* This opcode is reserved, but... */

      case 0xC0:
        CUR.period = GridPeriod;
        break;
    }

    switch ( (FT_Int)( selector & 0x30 ) )
    {
    case 0:
      CUR.phase = 0;
      break;

    case 0x10:
      CUR.phase = CUR.period / 4;
      break;

    case 0x20:
      CUR.phase = CUR.period / 2;
      break;

    case 0x30:
      CUR.phase = CUR.period * 3 / 4;
      break;
    }

    if ( ( selector & 0x0F ) == 0 )
      CUR.threshold = CUR.period - 1;
    else
      CUR.threshold = ( (FT_Int)( selector & 0x0F ) - 4 ) * CUR.period / 8;

    CUR.period    /= 256;
    CUR.phase     /= 256;
    CUR.threshold /= 256;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Project                                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the projection of vector given by (v2-v1) along the       */
  /*    current projection vector.                                         */
  /*                                                                       */
  /* <Input>                                                               */
  /*    v1 :: First input vector.                                          */
  /*    v2 :: Second input vector.                                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The distance in F26dot6 format.                                    */
  /*                                                                       */
  static FT_F26Dot6
  Project( EXEC_OP_ FT_Pos  dx,
                    FT_Pos  dy )
  {
#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    FT_ASSERT( !CUR.face->unpatented_hinting );
#endif

    return TT_DotFix14( (FT_UInt32)dx, (FT_UInt32)dy,
                        CUR.GS.projVector.x,
                        CUR.GS.projVector.y );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Dual_Project                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the projection of the vector given by (v2-v1) along the   */
  /*    current dual vector.                                               */
  /*                                                                       */
  /* <Input>                                                               */
  /*    v1 :: First input vector.                                          */
  /*    v2 :: Second input vector.                                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The distance in F26dot6 format.                                    */
  /*                                                                       */
  static FT_F26Dot6
  Dual_Project( EXEC_OP_ FT_Pos  dx,
                         FT_Pos  dy )
  {
    return TT_DotFix14( (FT_UInt32)dx, (FT_UInt32)dy,
                        CUR.GS.dualVector.x,
                        CUR.GS.dualVector.y );
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Project_x                                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the projection of the vector given by (v2-v1) along the   */
  /*    horizontal axis.                                                   */
  /*                                                                       */
  /* <Input>                                                               */
  /*    v1 :: First input vector.                                          */
  /*    v2 :: Second input vector.                                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The distance in F26dot6 format.                                    */
  /*                                                                       */
  static FT_F26Dot6
  Project_x( EXEC_OP_ FT_Pos  dx,
                      FT_Pos  dy )
  {
    FT_UNUSED_EXEC;
    FT_UNUSED( dy );

    return dx;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Project_y                                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the projection of the vector given by (v2-v1) along the   */
  /*    vertical axis.                                                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    v1 :: First input vector.                                          */
  /*    v2 :: Second input vector.                                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The distance in F26dot6 format.                                    */
  /*                                                                       */
  static FT_F26Dot6
  Project_y( EXEC_OP_ FT_Pos  dx,
                      FT_Pos  dy )
  {
    FT_UNUSED_EXEC;
    FT_UNUSED( dx );

    return dy;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Compute_Funcs                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the projection and movement function pointers according   */
  /*    to the current graphics state.                                     */
  /*                                                                       */
  static void
  Compute_Funcs( EXEC_OP )
  {
#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    if ( CUR.face->unpatented_hinting )
    {
      /* If both vectors point rightwards along the x axis, set             */
      /* `both-x-axis' true, otherwise set it false.  The x values only     */
      /* need be tested because the vector has been normalised to a unit    */
      /* vector of length 0x4000 = unity.                                   */
      CUR.GS.both_x_axis = (FT_Bool)( CUR.GS.projVector.x == 0x4000 &&
                                      CUR.GS.freeVector.x == 0x4000 );

      /* Throw away projection and freedom vector information */
      /* because the patents don't allow them to be stored.   */
      /* The relevant US Patents are 5155805 and 5325479.     */
      CUR.GS.projVector.x = 0;
      CUR.GS.projVector.y = 0;
      CUR.GS.freeVector.x = 0;
      CUR.GS.freeVector.y = 0;

      if ( CUR.GS.both_x_axis )
      {
        CUR.func_project   = Project_x;
        CUR.func_move      = Direct_Move_X;
        CUR.func_move_orig = Direct_Move_Orig_X;
      }
      else
      {
        CUR.func_project   = Project_y;
        CUR.func_move      = Direct_Move_Y;
        CUR.func_move_orig = Direct_Move_Orig_Y;
      }

      if ( CUR.GS.dualVector.x == 0x4000 )
        CUR.func_dualproj = Project_x;
      else if ( CUR.GS.dualVector.y == 0x4000 )
        CUR.func_dualproj = Project_y;
      else
        CUR.func_dualproj = Dual_Project;

      /* Force recalculation of cached aspect ratio */
      CUR.tt_metrics.ratio = 0;

      return;
    }
#endif /* TT_CONFIG_OPTION_UNPATENTED_HINTING */

    if ( CUR.GS.freeVector.x == 0x4000 )
      CUR.F_dot_P = CUR.GS.projVector.x;
    else if ( CUR.GS.freeVector.y == 0x4000 )
      CUR.F_dot_P = CUR.GS.projVector.y;
    else
      CUR.F_dot_P = ( (FT_Long)CUR.GS.projVector.x * CUR.GS.freeVector.x +
                      (FT_Long)CUR.GS.projVector.y * CUR.GS.freeVector.y ) >>
                    14;

    if ( CUR.GS.projVector.x == 0x4000 )
      CUR.func_project = (TT_Project_Func)Project_x;
    else if ( CUR.GS.projVector.y == 0x4000 )
      CUR.func_project = (TT_Project_Func)Project_y;
    else
      CUR.func_project = (TT_Project_Func)Project;

    if ( CUR.GS.dualVector.x == 0x4000 )
      CUR.func_dualproj = (TT_Project_Func)Project_x;
    else if ( CUR.GS.dualVector.y == 0x4000 )
      CUR.func_dualproj = (TT_Project_Func)Project_y;
    else
      CUR.func_dualproj = (TT_Project_Func)Dual_Project;

    CUR.func_move      = (TT_Move_Func)Direct_Move;
    CUR.func_move_orig = (TT_Move_Func)Direct_Move_Orig;

    if ( CUR.F_dot_P == 0x4000L )
    {
      if ( CUR.GS.freeVector.x == 0x4000 )
      {
        CUR.func_move      = (TT_Move_Func)Direct_Move_X;
        CUR.func_move_orig = (TT_Move_Func)Direct_Move_Orig_X;
      }
      else if ( CUR.GS.freeVector.y == 0x4000 )
      {
        CUR.func_move      = (TT_Move_Func)Direct_Move_Y;
        CUR.func_move_orig = (TT_Move_Func)Direct_Move_Orig_Y;
      }
    }

    /* at small sizes, F_dot_P can become too small, resulting   */
    /* in overflows and `spikes' in a number of glyphs like `w'. */

    if ( FT_ABS( CUR.F_dot_P ) < 0x400L )
      CUR.F_dot_P = 0x4000L;

    /* Disable cached aspect ratio */
    CUR.tt_metrics.ratio = 0;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Normalize                                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Norms a vector.                                                    */
  /*                                                                       */
  /* <Input>                                                               */
  /*    Vx :: The horizontal input vector coordinate.                      */
  /*    Vy :: The vertical input vector coordinate.                        */
  /*                                                                       */
  /* <Output>                                                              */
  /*    R  :: The normed unit vector.                                      */
  /*                                                                       */
  /* <Return>                                                              */
  /*    Returns FAILURE if a vector parameter is zero.                     */
  /*                                                                       */
  /* <Note>                                                                */
  /*    In case Vx and Vy are both zero, Normalize() returns SUCCESS, and  */
  /*    R is undefined.                                                    */
  /*                                                                       */
  static FT_Bool
  Normalize( EXEC_OP_ FT_F26Dot6      Vx,
                      FT_F26Dot6      Vy,
                      FT_UnitVector*  R )
  {
    FT_F26Dot6  W;

    FT_UNUSED_EXEC;


    if ( FT_ABS( Vx ) < 0x4000L && FT_ABS( Vy ) < 0x4000L )
    {
      if ( Vx == 0 && Vy == 0 )
      {
        /* XXX: UNDOCUMENTED! It seems that it is possible to try   */
        /*      to normalize the vector (0,0).  Return immediately. */
        return SUCCESS;
      }

      Vx *= 0x4000;
      Vy *= 0x4000;
    }

    W = FT_Hypot( Vx, Vy );

    R->x = (FT_F2Dot14)TT_DivFix14( Vx, W );
    R->y = (FT_F2Dot14)TT_DivFix14( Vy, W );

    return SUCCESS;
  }


  /*************************************************************************/
  /*                                                                       */
  /* Here we start with the implementation of the various opcodes.         */
  /*                                                                       */
  /*************************************************************************/


  static FT_Bool
  Ins_SxVTL( EXEC_OP_ FT_UShort       aIdx1,
                      FT_UShort       aIdx2,
                      FT_Int          aOpc,
                      FT_UnitVector*  Vec )
  {
    FT_Long     A, B, C;
    FT_Vector*  p1;
    FT_Vector*  p2;


    if ( BOUNDS( aIdx1, CUR.zp2.n_points ) ||
         BOUNDS( aIdx2, CUR.zp1.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return FAILURE;
    }

    p1 = CUR.zp1.cur + aIdx2;
    p2 = CUR.zp2.cur + aIdx1;

    A = p1->x - p2->x;
    B = p1->y - p2->y;

    /* If p1 == p2, SPVTL and SFVTL behave the same as */
    /* SPVTCA[X] and SFVTCA[X], respectively.          */
    /*                                                 */
    /* Confirmed by Greg Hitchcock.                    */

    if ( A == 0 && B == 0 )
    {
      A    = 0x4000;
      aOpc = 0;
    }

    if ( ( aOpc & 1 ) != 0 )
    {
      C =  B;   /* counter clockwise rotation */
      B =  A;
      A = -C;
    }

    NORMalize( A, B, Vec );

    return SUCCESS;
  }


  /* When not using the big switch statements, the interpreter uses a */
  /* call table defined later below in this source.  Each opcode must */
  /* thus have a corresponding function, even trivial ones.           */
  /*                                                                  */
  /* They are all defined there.                                      */

#define DO_SVTCA                            \
  {                                         \
    FT_Short  A, B;                         \
                                            \
                                            \
    A = (FT_Short)( CUR.opcode & 1 ) << 14; \
    B = A ^ (FT_Short)0x4000;               \
                                            \
    CUR.GS.freeVector.x = A;                \
    CUR.GS.projVector.x = A;                \
    CUR.GS.dualVector.x = A;                \
                                            \
    CUR.GS.freeVector.y = B;                \
    CUR.GS.projVector.y = B;                \
    CUR.GS.dualVector.y = B;                \
                                            \
    COMPUTE_Funcs();                        \
  }


#define DO_SPVTCA                           \
  {                                         \
    FT_Short  A, B;                         \
                                            \
                                            \
    A = (FT_Short)( CUR.opcode & 1 ) << 14; \
    B = A ^ (FT_Short)0x4000;               \
                                            \
    CUR.GS.projVector.x = A;                \
    CUR.GS.dualVector.x = A;                \
                                            \
    CUR.GS.projVector.y = B;                \
    CUR.GS.dualVector.y = B;                \
                                            \
    GUESS_VECTOR( freeVector );             \
                                            \
    COMPUTE_Funcs();                        \
  }


#define DO_SFVTCA                           \
  {                                         \
    FT_Short  A, B;                         \
                                            \
                                            \
    A = (FT_Short)( CUR.opcode & 1 ) << 14; \
    B = A ^ (FT_Short)0x4000;               \
                                            \
    CUR.GS.freeVector.x = A;                \
    CUR.GS.freeVector.y = B;                \
                                            \
    GUESS_VECTOR( projVector );             \
                                            \
    COMPUTE_Funcs();                        \
  }


#define DO_SPVTL                                      \
    if ( INS_SxVTL( (FT_UShort)args[1],               \
                    (FT_UShort)args[0],               \
                    CUR.opcode,                       \
                    &CUR.GS.projVector ) == SUCCESS ) \
    {                                                 \
      CUR.GS.dualVector = CUR.GS.projVector;          \
      GUESS_VECTOR( freeVector );                     \
      COMPUTE_Funcs();                                \
    }


#define DO_SFVTL                                      \
    if ( INS_SxVTL( (FT_UShort)args[1],               \
                    (FT_UShort)args[0],               \
                    CUR.opcode,                       \
                    &CUR.GS.freeVector ) == SUCCESS ) \
    {                                                 \
      GUESS_VECTOR( projVector );                     \
      COMPUTE_Funcs();                                \
    }


#define DO_SFVTPV                          \
    GUESS_VECTOR( projVector );            \
    CUR.GS.freeVector = CUR.GS.projVector; \
    COMPUTE_Funcs();


#define DO_SPVFS                                \
  {                                             \
    FT_Short  S;                                \
    FT_Long   X, Y;                             \
                                                \
                                                \
    /* Only use low 16bits, then sign extend */ \
    S = (FT_Short)args[1];                      \
    Y = (FT_Long)S;                             \
    S = (FT_Short)args[0];                      \
    X = (FT_Long)S;                             \
                                                \
    NORMalize( X, Y, &CUR.GS.projVector );      \
                                                \
    CUR.GS.dualVector = CUR.GS.projVector;      \
    GUESS_VECTOR( freeVector );                 \
    COMPUTE_Funcs();                            \
  }


#define DO_SFVFS                                \
  {                                             \
    FT_Short  S;                                \
    FT_Long   X, Y;                             \
                                                \
                                                \
    /* Only use low 16bits, then sign extend */ \
    S = (FT_Short)args[1];                      \
    Y = (FT_Long)S;                             \
    S = (FT_Short)args[0];                      \
    X = S;                                      \
                                                \
    NORMalize( X, Y, &CUR.GS.freeVector );      \
    GUESS_VECTOR( projVector );                 \
    COMPUTE_Funcs();                            \
  }


#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
#define DO_GPV                                   \
    if ( CUR.face->unpatented_hinting )          \
    {                                            \
      args[0] = CUR.GS.both_x_axis ? 0x4000 : 0; \
      args[1] = CUR.GS.both_x_axis ? 0 : 0x4000; \
    }                                            \
    else                                         \
    {                                            \
      args[0] = CUR.GS.projVector.x;             \
      args[1] = CUR.GS.projVector.y;             \
    }
#else
#define DO_GPV                                   \
    args[0] = CUR.GS.projVector.x;               \
    args[1] = CUR.GS.projVector.y;
#endif


#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
#define DO_GFV                                   \
    if ( CUR.face->unpatented_hinting )          \
    {                                            \
      args[0] = CUR.GS.both_x_axis ? 0x4000 : 0; \
      args[1] = CUR.GS.both_x_axis ? 0 : 0x4000; \
    }                                            \
    else                                         \
    {                                            \
      args[0] = CUR.GS.freeVector.x;             \
      args[1] = CUR.GS.freeVector.y;             \
    }
#else
#define DO_GFV                                   \
    args[0] = CUR.GS.freeVector.x;               \
    args[1] = CUR.GS.freeVector.y;
#endif


#define DO_SRP0                      \
    CUR.GS.rp0 = (FT_UShort)args[0];


#define DO_SRP1                      \
    CUR.GS.rp1 = (FT_UShort)args[0];


#define DO_SRP2                      \
    CUR.GS.rp2 = (FT_UShort)args[0];


#define DO_RTHG                                         \
    CUR.GS.round_state = TT_Round_To_Half_Grid;         \
    CUR.func_round = (TT_Round_Func)Round_To_Half_Grid;


#define DO_RTG                                     \
    CUR.GS.round_state = TT_Round_To_Grid;         \
    CUR.func_round = (TT_Round_Func)Round_To_Grid;


#define DO_RTDG                                           \
    CUR.GS.round_state = TT_Round_To_Double_Grid;         \
    CUR.func_round = (TT_Round_Func)Round_To_Double_Grid;


#define DO_RUTG                                       \
    CUR.GS.round_state = TT_Round_Up_To_Grid;         \
    CUR.func_round = (TT_Round_Func)Round_Up_To_Grid;


#define DO_RDTG                                         \
    CUR.GS.round_state = TT_Round_Down_To_Grid;         \
    CUR.func_round = (TT_Round_Func)Round_Down_To_Grid;


#define DO_ROFF                                 \
    CUR.GS.round_state = TT_Round_Off;          \
    CUR.func_round = (TT_Round_Func)Round_None;


#define DO_SROUND                                \
    SET_SuperRound( 0x4000, args[0] );           \
    CUR.GS.round_state = TT_Round_Super;         \
    CUR.func_round = (TT_Round_Func)Round_Super;


#define DO_S45ROUND                                 \
    SET_SuperRound( 0x2D41, args[0] );              \
    CUR.GS.round_state = TT_Round_Super_45;         \
    CUR.func_round = (TT_Round_Func)Round_Super_45;


#define DO_SLOOP                            \
    if ( args[0] < 0 )                      \
      CUR.error = FT_THROW( Bad_Argument ); \
    else                                    \
      CUR.GS.loop = args[0];


#define DO_SMD                         \
    CUR.GS.minimum_distance = args[0];


#define DO_SCVTCI                                     \
    CUR.GS.control_value_cutin = (FT_F26Dot6)args[0];


#define DO_SSWCI                                     \
    CUR.GS.single_width_cutin = (FT_F26Dot6)args[0];


#define DO_SSW                                                     \
    CUR.GS.single_width_value = FT_MulFix( args[0],                \
                                           CUR.tt_metrics.scale );


#define DO_FLIPON            \
    CUR.GS.auto_flip = TRUE;


#define DO_FLIPOFF            \
    CUR.GS.auto_flip = FALSE;


#define DO_SDB                             \
    CUR.GS.delta_base = (FT_Short)args[0];


#define DO_SDS                              \
    CUR.GS.delta_shift = (FT_Short)args[0];


#define DO_MD  /* nothing */


#define DO_MPPEM              \
    args[0] = CURRENT_Ppem();


  /* Note: The pointSize should be irrelevant in a given font program; */
  /*       we thus decide to return only the ppem.                     */
#if 0

#define DO_MPS                       \
    args[0] = CUR.metrics.pointSize;

#else

#define DO_MPS                \
    args[0] = CURRENT_Ppem();

#endif /* 0 */


#define DO_DUP         \
    args[1] = args[0];


#define DO_CLEAR     \
    CUR.new_top = 0;


#define DO_SWAP        \
  {                    \
    FT_Long  L;        \
                       \
                       \
    L       = args[0]; \
    args[0] = args[1]; \
    args[1] = L;       \
  }


#define DO_DEPTH       \
    args[0] = CUR.top;


#define DO_CINDEX                                  \
  {                                                \
    FT_Long  L;                                    \
                                                   \
                                                   \
    L = args[0];                                   \
                                                   \
    if ( L <= 0 || L > CUR.args )                  \
    {                                              \
      if ( CUR.pedantic_hinting )                  \
        CUR.error = FT_THROW( Invalid_Reference ); \
      args[0] = 0;                                 \
    }                                              \
    else                                           \
      args[0] = CUR.stack[CUR.args - L];           \
  }


#define DO_JROT                                                    \
    if ( args[1] != 0 )                                            \
    {                                                              \
      if ( args[0] == 0 && CUR.args == 0 )                         \
        CUR.error = FT_THROW( Bad_Argument );                      \
      CUR.IP += args[0];                                           \
      if ( CUR.IP < 0                                           || \
           ( CUR.callTop > 0                                  &&   \
             CUR.IP > CUR.callStack[CUR.callTop - 1].Def->end ) )  \
        CUR.error = FT_THROW( Bad_Argument );                      \
      CUR.step_ins = FALSE;                                        \
    }


#define DO_JMPR                                                  \
    if ( args[0] == 0 && CUR.args == 0 )                         \
      CUR.error = FT_THROW( Bad_Argument );                      \
    CUR.IP += args[0];                                           \
    if ( CUR.IP < 0                                           || \
         ( CUR.callTop > 0                                  &&   \
           CUR.IP > CUR.callStack[CUR.callTop - 1].Def->end ) )  \
      CUR.error = FT_THROW( Bad_Argument );                      \
    CUR.step_ins = FALSE;


#define DO_JROF                                                    \
    if ( args[1] == 0 )                                            \
    {                                                              \
      if ( args[0] == 0 && CUR.args == 0 )                         \
        CUR.error = FT_THROW( Bad_Argument );                      \
      CUR.IP += args[0];                                           \
      if ( CUR.IP < 0                                           || \
           ( CUR.callTop > 0                                  &&   \
             CUR.IP > CUR.callStack[CUR.callTop - 1].Def->end ) )  \
        CUR.error = FT_THROW( Bad_Argument );                      \
      CUR.step_ins = FALSE;                                        \
    }


#define DO_LT                        \
    args[0] = ( args[0] < args[1] );


#define DO_LTEQ                       \
    args[0] = ( args[0] <= args[1] );


#define DO_GT                        \
    args[0] = ( args[0] > args[1] );


#define DO_GTEQ                       \
    args[0] = ( args[0] >= args[1] );


#define DO_EQ                         \
    args[0] = ( args[0] == args[1] );


#define DO_NEQ                        \
    args[0] = ( args[0] != args[1] );


#define DO_ODD                                                  \
    args[0] = ( ( CUR_Func_round( args[0], 0 ) & 127 ) == 64 );


#define DO_EVEN                                                \
    args[0] = ( ( CUR_Func_round( args[0], 0 ) & 127 ) == 0 );


#define DO_AND                        \
    args[0] = ( args[0] && args[1] );


#define DO_OR                         \
    args[0] = ( args[0] || args[1] );


#define DO_NOT          \
    args[0] = !args[0];


#define DO_ADD          \
    args[0] += args[1];


#define DO_SUB          \
    args[0] -= args[1];


#define DO_DIV                                               \
    if ( args[1] == 0 )                                      \
      CUR.error = FT_THROW( Divide_By_Zero );                \
    else                                                     \
      args[0] = FT_MulDiv_No_Round( args[0], 64L, args[1] );


#define DO_MUL                                    \
    args[0] = FT_MulDiv( args[0], args[1], 64L );


#define DO_ABS                   \
    args[0] = FT_ABS( args[0] );


#define DO_NEG          \
    args[0] = -args[0];


#define DO_FLOOR    \
    args[0] = FT_PIX_FLOOR( args[0] );


#define DO_CEILING                    \
    args[0] = FT_PIX_CEIL( args[0] );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

#define DO_RS                                             \
   {                                                      \
     FT_ULong  I = (FT_ULong)args[0];                     \
                                                          \
                                                          \
     if ( BOUNDSL( I, CUR.storeSize ) )                   \
     {                                                    \
       if ( CUR.pedantic_hinting )                        \
         ARRAY_BOUND_ERROR;                               \
       else                                               \
         args[0] = 0;                                     \
     }                                                    \
     else                                                 \
     {                                                    \
       /* subpixel hinting - avoid Typeman Dstroke and */ \
       /* IStroke and Vacuform rounds                  */ \
                                                          \
       if ( SUBPIXEL_HINTING                           && \
            CUR.ignore_x_mode                          && \
            ( ( I == 24                            &&     \
                ( CUR.face->sph_found_func_flags &        \
                  ( SPH_FDEF_SPACING_1 |                  \
                    SPH_FDEF_SPACING_2 )         ) ) ||   \
              ( I == 22                      &&           \
                ( CUR.sph_in_func_flags    &              \
                  SPH_FDEF_TYPEMAN_STROKES ) )       ||   \
              ( I == 8                             &&     \
                ( CUR.face->sph_found_func_flags &        \
                  SPH_FDEF_VACUFORM_ROUND_1      ) &&     \
                  CUR.iup_called                   ) ) )  \
         args[0] = 0;                                     \
       else                                               \
         args[0] = CUR.storage[I];                        \
     }                                                    \
   }

#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */

#define DO_RS                           \
   {                                    \
     FT_ULong  I = (FT_ULong)args[0];   \
                                        \
                                        \
     if ( BOUNDSL( I, CUR.storeSize ) ) \
     {                                  \
       if ( CUR.pedantic_hinting )      \
       {                                \
         ARRAY_BOUND_ERROR;             \
       }                                \
       else                             \
         args[0] = 0;                   \
     }                                  \
     else                               \
       args[0] = CUR.storage[I];        \
   }

#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */


#define DO_WS                           \
   {                                    \
     FT_ULong  I = (FT_ULong)args[0];   \
                                        \
                                        \
     if ( BOUNDSL( I, CUR.storeSize ) ) \
     {                                  \
       if ( CUR.pedantic_hinting )      \
       {                                \
         ARRAY_BOUND_ERROR;             \
       }                                \
     }                                  \
     else                               \
       CUR.storage[I] = args[1];        \
   }


#define DO_RCVT                          \
   {                                     \
     FT_ULong  I = (FT_ULong)args[0];    \
                                         \
                                         \
     if ( BOUNDSL( I, CUR.cvtSize ) )    \
     {                                   \
       if ( CUR.pedantic_hinting )       \
       {                                 \
         ARRAY_BOUND_ERROR;              \
       }                                 \
       else                              \
         args[0] = 0;                    \
     }                                   \
     else                                \
       args[0] = CUR_Func_read_cvt( I ); \
   }


#define DO_WCVTP                         \
   {                                     \
     FT_ULong  I = (FT_ULong)args[0];    \
                                         \
                                         \
     if ( BOUNDSL( I, CUR.cvtSize ) )    \
     {                                   \
       if ( CUR.pedantic_hinting )       \
       {                                 \
         ARRAY_BOUND_ERROR;              \
       }                                 \
     }                                   \
     else                                \
       CUR_Func_write_cvt( I, args[1] ); \
   }


#define DO_WCVTF                                                \
   {                                                            \
     FT_ULong  I = (FT_ULong)args[0];                           \
                                                                \
                                                                \
     if ( BOUNDSL( I, CUR.cvtSize ) )                           \
     {                                                          \
       if ( CUR.pedantic_hinting )                              \
       {                                                        \
         ARRAY_BOUND_ERROR;                                     \
       }                                                        \
     }                                                          \
     else                                                       \
       CUR.cvt[I] = FT_MulFix( args[1], CUR.tt_metrics.scale ); \
   }


#define DO_DEBUG                          \
    CUR.error = FT_THROW( Debug_OpCode );


#define DO_ROUND                                                   \
    args[0] = CUR_Func_round(                                      \
                args[0],                                           \
                CUR.tt_metrics.compensations[CUR.opcode - 0x68] );


#define DO_NROUND                                                            \
    args[0] = ROUND_None( args[0],                                           \
                          CUR.tt_metrics.compensations[CUR.opcode - 0x6C] );


#define DO_MAX               \
    if ( args[1] > args[0] ) \
      args[0] = args[1];


#define DO_MIN               \
    if ( args[1] < args[0] ) \
      args[0] = args[1];


#ifndef TT_CONFIG_OPTION_INTERPRETER_SWITCH


#undef  ARRAY_BOUND_ERROR
#define ARRAY_BOUND_ERROR                        \
    {                                            \
      CUR.error = FT_THROW( Invalid_Reference ); \
      return;                                    \
    }


  /*************************************************************************/
  /*                                                                       */
  /* SVTCA[a]:     Set (F and P) Vectors to Coordinate Axis                */
  /* Opcode range: 0x00-0x01                                               */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_SVTCA( INS_ARG )
  {
    DO_SVTCA
  }


  /*************************************************************************/
  /*                                                                       */
  /* SPVTCA[a]:    Set PVector to Coordinate Axis                          */
  /* Opcode range: 0x02-0x03                                               */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_SPVTCA( INS_ARG )
  {
    DO_SPVTCA
  }


  /*************************************************************************/
  /*                                                                       */
  /* SFVTCA[a]:    Set FVector to Coordinate Axis                          */
  /* Opcode range: 0x04-0x05                                               */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_SFVTCA( INS_ARG )
  {
    DO_SFVTCA
  }


  /*************************************************************************/
  /*                                                                       */
  /* SPVTL[a]:     Set PVector To Line                                     */
  /* Opcode range: 0x06-0x07                                               */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_SPVTL( INS_ARG )
  {
    DO_SPVTL
  }


  /*************************************************************************/
  /*                                                                       */
  /* SFVTL[a]:     Set FVector To Line                                     */
  /* Opcode range: 0x08-0x09                                               */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_SFVTL( INS_ARG )
  {
    DO_SFVTL
  }


  /*************************************************************************/
  /*                                                                       */
  /* SFVTPV[]:     Set FVector To PVector                                  */
  /* Opcode range: 0x0E                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_SFVTPV( INS_ARG )
  {
    DO_SFVTPV
  }


  /*************************************************************************/
  /*                                                                       */
  /* SPVFS[]:      Set PVector From Stack                                  */
  /* Opcode range: 0x0A                                                    */
  /* Stack:        f2.14 f2.14 -->                                         */
  /*                                                                       */
  static void
  Ins_SPVFS( INS_ARG )
  {
    DO_SPVFS
  }


  /*************************************************************************/
  /*                                                                       */
  /* SFVFS[]:      Set FVector From Stack                                  */
  /* Opcode range: 0x0B                                                    */
  /* Stack:        f2.14 f2.14 -->                                         */
  /*                                                                       */
  static void
  Ins_SFVFS( INS_ARG )
  {
    DO_SFVFS
  }


  /*************************************************************************/
  /*                                                                       */
  /* GPV[]:        Get Projection Vector                                   */
  /* Opcode range: 0x0C                                                    */
  /* Stack:        ef2.14 --> ef2.14                                       */
  /*                                                                       */
  static void
  Ins_GPV( INS_ARG )
  {
    DO_GPV
  }


  /*************************************************************************/
  /* GFV[]:        Get Freedom Vector                                      */
  /* Opcode range: 0x0D                                                    */
  /* Stack:        ef2.14 --> ef2.14                                       */
  /*                                                                       */
  static void
  Ins_GFV( INS_ARG )
  {
    DO_GFV
  }


  /*************************************************************************/
  /*                                                                       */
  /* SRP0[]:       Set Reference Point 0                                   */
  /* Opcode range: 0x10                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SRP0( INS_ARG )
  {
    DO_SRP0
  }


  /*************************************************************************/
  /*                                                                       */
  /* SRP1[]:       Set Reference Point 1                                   */
  /* Opcode range: 0x11                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SRP1( INS_ARG )
  {
    DO_SRP1
  }


  /*************************************************************************/
  /*                                                                       */
  /* SRP2[]:       Set Reference Point 2                                   */
  /* Opcode range: 0x12                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SRP2( INS_ARG )
  {
    DO_SRP2
  }


  /*************************************************************************/
  /*                                                                       */
  /* RTHG[]:       Round To Half Grid                                      */
  /* Opcode range: 0x19                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_RTHG( INS_ARG )
  {
    DO_RTHG
  }


  /*************************************************************************/
  /*                                                                       */
  /* RTG[]:        Round To Grid                                           */
  /* Opcode range: 0x18                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_RTG( INS_ARG )
  {
    DO_RTG
  }


  /*************************************************************************/
  /* RTDG[]:       Round To Double Grid                                    */
  /* Opcode range: 0x3D                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_RTDG( INS_ARG )
  {
    DO_RTDG
  }


  /*************************************************************************/
  /* RUTG[]:       Round Up To Grid                                        */
  /* Opcode range: 0x7C                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_RUTG( INS_ARG )
  {
    DO_RUTG
  }


  /*************************************************************************/
  /*                                                                       */
  /* RDTG[]:       Round Down To Grid                                      */
  /* Opcode range: 0x7D                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_RDTG( INS_ARG )
  {
    DO_RDTG
  }


  /*************************************************************************/
  /*                                                                       */
  /* ROFF[]:       Round OFF                                               */
  /* Opcode range: 0x7A                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_ROFF( INS_ARG )
  {
    DO_ROFF
  }


  /*************************************************************************/
  /*                                                                       */
  /* SROUND[]:     Super ROUND                                             */
  /* Opcode range: 0x76                                                    */
  /* Stack:        Eint8 -->                                               */
  /*                                                                       */
  static void
  Ins_SROUND( INS_ARG )
  {
    DO_SROUND
  }


  /*************************************************************************/
  /*                                                                       */
  /* S45ROUND[]:   Super ROUND 45 degrees                                  */
  /* Opcode range: 0x77                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_S45ROUND( INS_ARG )
  {
    DO_S45ROUND
  }


  /*************************************************************************/
  /*                                                                       */
  /* SLOOP[]:      Set LOOP variable                                       */
  /* Opcode range: 0x17                                                    */
  /* Stack:        int32? -->                                              */
  /*                                                                       */
  static void
  Ins_SLOOP( INS_ARG )
  {
    DO_SLOOP
  }


  /*************************************************************************/
  /*                                                                       */
  /* SMD[]:        Set Minimum Distance                                    */
  /* Opcode range: 0x1A                                                    */
  /* Stack:        f26.6 -->                                               */
  /*                                                                       */
  static void
  Ins_SMD( INS_ARG )
  {
    DO_SMD
  }


  /*************************************************************************/
  /*                                                                       */
  /* SCVTCI[]:     Set Control Value Table Cut In                          */
  /* Opcode range: 0x1D                                                    */
  /* Stack:        f26.6 -->                                               */
  /*                                                                       */
  static void
  Ins_SCVTCI( INS_ARG )
  {
    DO_SCVTCI
  }


  /*************************************************************************/
  /*                                                                       */
  /* SSWCI[]:      Set Single Width Cut In                                 */
  /* Opcode range: 0x1E                                                    */
  /* Stack:        f26.6 -->                                               */
  /*                                                                       */
  static void
  Ins_SSWCI( INS_ARG )
  {
    DO_SSWCI
  }


  /*************************************************************************/
  /*                                                                       */
  /* SSW[]:        Set Single Width                                        */
  /* Opcode range: 0x1F                                                    */
  /* Stack:        int32? -->                                              */
  /*                                                                       */
  static void
  Ins_SSW( INS_ARG )
  {
    DO_SSW
  }


  /*************************************************************************/
  /*                                                                       */
  /* FLIPON[]:     Set auto-FLIP to ON                                     */
  /* Opcode range: 0x4D                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_FLIPON( INS_ARG )
  {
    DO_FLIPON
  }


  /*************************************************************************/
  /*                                                                       */
  /* FLIPOFF[]:    Set auto-FLIP to OFF                                    */
  /* Opcode range: 0x4E                                                    */
  /* Stack: -->                                                            */
  /*                                                                       */
  static void
  Ins_FLIPOFF( INS_ARG )
  {
    DO_FLIPOFF
  }


  /*************************************************************************/
  /*                                                                       */
  /* SANGW[]:      Set ANGle Weight                                        */
  /* Opcode range: 0x7E                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SANGW( INS_ARG )
  {
    /* instruction not supported anymore */
  }


  /*************************************************************************/
  /*                                                                       */
  /* SDB[]:        Set Delta Base                                          */
  /* Opcode range: 0x5E                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SDB( INS_ARG )
  {
    DO_SDB
  }


  /*************************************************************************/
  /*                                                                       */
  /* SDS[]:        Set Delta Shift                                         */
  /* Opcode range: 0x5F                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SDS( INS_ARG )
  {
    DO_SDS
  }


  /*************************************************************************/
  /*                                                                       */
  /* MPPEM[]:      Measure Pixel Per EM                                    */
  /* Opcode range: 0x4B                                                    */
  /* Stack:        --> Euint16                                             */
  /*                                                                       */
  static void
  Ins_MPPEM( INS_ARG )
  {
    DO_MPPEM
  }


  /*************************************************************************/
  /*                                                                       */
  /* MPS[]:        Measure Point Size                                      */
  /* Opcode range: 0x4C                                                    */
  /* Stack:        --> Euint16                                             */
  /*                                                                       */
  static void
  Ins_MPS( INS_ARG )
  {
    DO_MPS
  }


  /*************************************************************************/
  /*                                                                       */
  /* DUP[]:        DUPlicate the top stack's element                       */
  /* Opcode range: 0x20                                                    */
  /* Stack:        StkElt --> StkElt StkElt                                */
  /*                                                                       */
  static void
  Ins_DUP( INS_ARG )
  {
    DO_DUP
  }


  /*************************************************************************/
  /*                                                                       */
  /* POP[]:        POP the stack's top element                             */
  /* Opcode range: 0x21                                                    */
  /* Stack:        StkElt -->                                              */
  /*                                                                       */
  static void
  Ins_POP( INS_ARG )
  {
    /* nothing to do */
  }


  /*************************************************************************/
  /*                                                                       */
  /* CLEAR[]:      CLEAR the entire stack                                  */
  /* Opcode range: 0x22                                                    */
  /* Stack:        StkElt... -->                                           */
  /*                                                                       */
  static void
  Ins_CLEAR( INS_ARG )
  {
    DO_CLEAR
  }


  /*************************************************************************/
  /*                                                                       */
  /* SWAP[]:       SWAP the stack's top two elements                       */
  /* Opcode range: 0x23                                                    */
  /* Stack:        2 * StkElt --> 2 * StkElt                               */
  /*                                                                       */
  static void
  Ins_SWAP( INS_ARG )
  {
    DO_SWAP
  }


  /*************************************************************************/
  /*                                                                       */
  /* DEPTH[]:      return the stack DEPTH                                  */
  /* Opcode range: 0x24                                                    */
  /* Stack:        --> uint32                                              */
  /*                                                                       */
  static void
  Ins_DEPTH( INS_ARG )
  {
    DO_DEPTH
  }


  /*************************************************************************/
  /*                                                                       */
  /* CINDEX[]:     Copy INDEXed element                                    */
  /* Opcode range: 0x25                                                    */
  /* Stack:        int32 --> StkElt                                        */
  /*                                                                       */
  static void
  Ins_CINDEX( INS_ARG )
  {
    DO_CINDEX
  }


  /*************************************************************************/
  /*                                                                       */
  /* EIF[]:        End IF                                                  */
  /* Opcode range: 0x59                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_EIF( INS_ARG )
  {
    /* nothing to do */
  }


  /*************************************************************************/
  /*                                                                       */
  /* JROT[]:       Jump Relative On True                                   */
  /* Opcode range: 0x78                                                    */
  /* Stack:        StkElt int32 -->                                        */
  /*                                                                       */
  static void
  Ins_JROT( INS_ARG )
  {
    DO_JROT
  }


  /*************************************************************************/
  /*                                                                       */
  /* JMPR[]:       JuMP Relative                                           */
  /* Opcode range: 0x1C                                                    */
  /* Stack:        int32 -->                                               */
  /*                                                                       */
  static void
  Ins_JMPR( INS_ARG )
  {
    DO_JMPR
  }


  /*************************************************************************/
  /*                                                                       */
  /* JROF[]:       Jump Relative On False                                  */
  /* Opcode range: 0x79                                                    */
  /* Stack:        StkElt int32 -->                                        */
  /*                                                                       */
  static void
  Ins_JROF( INS_ARG )
  {
    DO_JROF
  }


  /*************************************************************************/
  /*                                                                       */
  /* LT[]:         Less Than                                               */
  /* Opcode range: 0x50                                                    */
  /* Stack:        int32? int32? --> bool                                  */
  /*                                                                       */
  static void
  Ins_LT( INS_ARG )
  {
    DO_LT
  }


  /*************************************************************************/
  /*                                                                       */
  /* LTEQ[]:       Less Than or EQual                                      */
  /* Opcode range: 0x51                                                    */
  /* Stack:        int32? int32? --> bool                                  */
  /*                                                                       */
  static void
  Ins_LTEQ( INS_ARG )
  {
    DO_LTEQ
  }


  /*************************************************************************/
  /*                                                                       */
  /* GT[]:         Greater Than                                            */
  /* Opcode range: 0x52                                                    */
  /* Stack:        int32? int32? --> bool                                  */
  /*                                                                       */
  static void
  Ins_GT( INS_ARG )
  {
    DO_GT
  }


  /*************************************************************************/
  /*                                                                       */
  /* GTEQ[]:       Greater Than or EQual                                   */
  /* Opcode range: 0x53                                                    */
  /* Stack:        int32? int32? --> bool                                  */
  /*                                                                       */
  static void
  Ins_GTEQ( INS_ARG )
  {
    DO_GTEQ
  }


  /*************************************************************************/
  /*                                                                       */
  /* EQ[]:         EQual                                                   */
  /* Opcode range: 0x54                                                    */
  /* Stack:        StkElt StkElt --> bool                                  */
  /*                                                                       */
  static void
  Ins_EQ( INS_ARG )
  {
    DO_EQ
  }


  /*************************************************************************/
  /*                                                                       */
  /* NEQ[]:        Not EQual                                               */
  /* Opcode range: 0x55                                                    */
  /* Stack:        StkElt StkElt --> bool                                  */
  /*                                                                       */
  static void
  Ins_NEQ( INS_ARG )
  {
    DO_NEQ
  }


  /*************************************************************************/
  /*                                                                       */
  /* ODD[]:        Is ODD                                                  */
  /* Opcode range: 0x56                                                    */
  /* Stack:        f26.6 --> bool                                          */
  /*                                                                       */
  static void
  Ins_ODD( INS_ARG )
  {
    DO_ODD
  }


  /*************************************************************************/
  /*                                                                       */
  /* EVEN[]:       Is EVEN                                                 */
  /* Opcode range: 0x57                                                    */
  /* Stack:        f26.6 --> bool                                          */
  /*                                                                       */
  static void
  Ins_EVEN( INS_ARG )
  {
    DO_EVEN
  }


  /*************************************************************************/
  /*                                                                       */
  /* AND[]:        logical AND                                             */
  /* Opcode range: 0x5A                                                    */
  /* Stack:        uint32 uint32 --> uint32                                */
  /*                                                                       */
  static void
  Ins_AND( INS_ARG )
  {
    DO_AND
  }


  /*************************************************************************/
  /*                                                                       */
  /* OR[]:         logical OR                                              */
  /* Opcode range: 0x5B                                                    */
  /* Stack:        uint32 uint32 --> uint32                                */
  /*                                                                       */
  static void
  Ins_OR( INS_ARG )
  {
    DO_OR
  }


  /*************************************************************************/
  /*                                                                       */
  /* NOT[]:        logical NOT                                             */
  /* Opcode range: 0x5C                                                    */
  /* Stack:        StkElt --> uint32                                       */
  /*                                                                       */
  static void
  Ins_NOT( INS_ARG )
  {
    DO_NOT
  }


  /*************************************************************************/
  /*                                                                       */
  /* ADD[]:        ADD                                                     */
  /* Opcode range: 0x60                                                    */
  /* Stack:        f26.6 f26.6 --> f26.6                                   */
  /*                                                                       */
  static void
  Ins_ADD( INS_ARG )
  {
    DO_ADD
  }


  /*************************************************************************/
  /*                                                                       */
  /* SUB[]:        SUBtract                                                */
  /* Opcode range: 0x61                                                    */
  /* Stack:        f26.6 f26.6 --> f26.6                                   */
  /*                                                                       */
  static void
  Ins_SUB( INS_ARG )
  {
    DO_SUB
  }


  /*************************************************************************/
  /*                                                                       */
  /* DIV[]:        DIVide                                                  */
  /* Opcode range: 0x62                                                    */
  /* Stack:        f26.6 f26.6 --> f26.6                                   */
  /*                                                                       */
  static void
  Ins_DIV( INS_ARG )
  {
    DO_DIV
  }


  /*************************************************************************/
  /*                                                                       */
  /* MUL[]:        MULtiply                                                */
  /* Opcode range: 0x63                                                    */
  /* Stack:        f26.6 f26.6 --> f26.6                                   */
  /*                                                                       */
  static void
  Ins_MUL( INS_ARG )
  {
    DO_MUL
  }


  /*************************************************************************/
  /*                                                                       */
  /* ABS[]:        ABSolute value                                          */
  /* Opcode range: 0x64                                                    */
  /* Stack:        f26.6 --> f26.6                                         */
  /*                                                                       */
  static void
  Ins_ABS( INS_ARG )
  {
    DO_ABS
  }


  /*************************************************************************/
  /*                                                                       */
  /* NEG[]:        NEGate                                                  */
  /* Opcode range: 0x65                                                    */
  /* Stack: f26.6 --> f26.6                                                */
  /*                                                                       */
  static void
  Ins_NEG( INS_ARG )
  {
    DO_NEG
  }


  /*************************************************************************/
  /*                                                                       */
  /* FLOOR[]:      FLOOR                                                   */
  /* Opcode range: 0x66                                                    */
  /* Stack:        f26.6 --> f26.6                                         */
  /*                                                                       */
  static void
  Ins_FLOOR( INS_ARG )
  {
    DO_FLOOR
  }


  /*************************************************************************/
  /*                                                                       */
  /* CEILING[]:    CEILING                                                 */
  /* Opcode range: 0x67                                                    */
  /* Stack:        f26.6 --> f26.6                                         */
  /*                                                                       */
  static void
  Ins_CEILING( INS_ARG )
  {
    DO_CEILING
  }


  /*************************************************************************/
  /*                                                                       */
  /* RS[]:         Read Store                                              */
  /* Opcode range: 0x43                                                    */
  /* Stack:        uint32 --> uint32                                       */
  /*                                                                       */
  static void
  Ins_RS( INS_ARG )
  {
    DO_RS
  }


  /*************************************************************************/
  /*                                                                       */
  /* WS[]:         Write Store                                             */
  /* Opcode range: 0x42                                                    */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_WS( INS_ARG )
  {
    DO_WS
  }


  /*************************************************************************/
  /*                                                                       */
  /* WCVTP[]:      Write CVT in Pixel units                                */
  /* Opcode range: 0x44                                                    */
  /* Stack:        f26.6 uint32 -->                                        */
  /*                                                                       */
  static void
  Ins_WCVTP( INS_ARG )
  {
    DO_WCVTP
  }


  /*************************************************************************/
  /*                                                                       */
  /* WCVTF[]:      Write CVT in Funits                                     */
  /* Opcode range: 0x70                                                    */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_WCVTF( INS_ARG )
  {
    DO_WCVTF
  }


  /*************************************************************************/
  /*                                                                       */
  /* RCVT[]:       Read CVT                                                */
  /* Opcode range: 0x45                                                    */
  /* Stack:        uint32 --> f26.6                                        */
  /*                                                                       */
  static void
  Ins_RCVT( INS_ARG )
  {
    DO_RCVT
  }


  /*************************************************************************/
  /*                                                                       */
  /* AA[]:         Adjust Angle                                            */
  /* Opcode range: 0x7F                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_AA( INS_ARG )
  {
    /* intentionally no longer supported */
  }


  /*************************************************************************/
  /*                                                                       */
  /* DEBUG[]:      DEBUG.  Unsupported.                                    */
  /* Opcode range: 0x4F                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  /* Note: The original instruction pops a value from the stack.           */
  /*                                                                       */
  static void
  Ins_DEBUG( INS_ARG )
  {
    DO_DEBUG
  }


  /*************************************************************************/
  /*                                                                       */
  /* ROUND[ab]:    ROUND value                                             */
  /* Opcode range: 0x68-0x6B                                               */
  /* Stack:        f26.6 --> f26.6                                         */
  /*                                                                       */
  static void
  Ins_ROUND( INS_ARG )
  {
    DO_ROUND
  }


  /*************************************************************************/
  /*                                                                       */
  /* NROUND[ab]:   No ROUNDing of value                                    */
  /* Opcode range: 0x6C-0x6F                                               */
  /* Stack:        f26.6 --> f26.6                                         */
  /*                                                                       */
  static void
  Ins_NROUND( INS_ARG )
  {
    DO_NROUND
  }


  /*************************************************************************/
  /*                                                                       */
  /* MAX[]:        MAXimum                                                 */
  /* Opcode range: 0x68                                                    */
  /* Stack:        int32? int32? --> int32                                 */
  /*                                                                       */
  static void
  Ins_MAX( INS_ARG )
  {
    DO_MAX
  }


  /*************************************************************************/
  /*                                                                       */
  /* MIN[]:        MINimum                                                 */
  /* Opcode range: 0x69                                                    */
  /* Stack:        int32? int32? --> int32                                 */
  /*                                                                       */
  static void
  Ins_MIN( INS_ARG )
  {
    DO_MIN
  }


#endif  /* !TT_CONFIG_OPTION_INTERPRETER_SWITCH */


  /*************************************************************************/
  /*                                                                       */
  /* The following functions are called as is within the switch statement. */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* MINDEX[]:     Move INDEXed element                                    */
  /* Opcode range: 0x26                                                    */
  /* Stack:        int32? --> StkElt                                       */
  /*                                                                       */
  static void
  Ins_MINDEX( INS_ARG )
  {
    FT_Long  L, K;


    L = args[0];

    if ( L <= 0 || L > CUR.args )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
    }
    else
    {
      K = CUR.stack[CUR.args - L];

      FT_ARRAY_MOVE( &CUR.stack[CUR.args - L    ],
                     &CUR.stack[CUR.args - L + 1],
                     ( L - 1 ) );

      CUR.stack[CUR.args - 1] = K;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* ROLL[]:       ROLL top three elements                                 */
  /* Opcode range: 0x8A                                                    */
  /* Stack:        3 * StkElt --> 3 * StkElt                               */
  /*                                                                       */
  static void
  Ins_ROLL( INS_ARG )
  {
    FT_Long  A, B, C;

    FT_UNUSED_EXEC;


    A = args[2];
    B = args[1];
    C = args[0];

    args[2] = C;
    args[1] = A;
    args[0] = B;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MANAGING THE FLOW OF CONTROL                                          */
  /*                                                                       */
  /*   Instructions appear in the specification's order.                   */
  /*                                                                       */
  /*************************************************************************/


  static FT_Bool
  SkipCode( EXEC_OP )
  {
    CUR.IP += CUR.length;

    if ( CUR.IP < CUR.codeSize )
    {
      CUR.opcode = CUR.code[CUR.IP];

      CUR.length = opcode_length[CUR.opcode];
      if ( CUR.length < 0 )
      {
        if ( CUR.IP + 1 >= CUR.codeSize )
          goto Fail_Overflow;
        CUR.length = 2 - CUR.length * CUR.code[CUR.IP + 1];
      }

      if ( CUR.IP + CUR.length <= CUR.codeSize )
        return SUCCESS;
    }

  Fail_Overflow:
    CUR.error = FT_THROW( Code_Overflow );
    return FAILURE;
  }


  /*************************************************************************/
  /*                                                                       */
  /* IF[]:         IF test                                                 */
  /* Opcode range: 0x58                                                    */
  /* Stack:        StkElt -->                                              */
  /*                                                                       */
  static void
  Ins_IF( INS_ARG )
  {
    FT_Int   nIfs;
    FT_Bool  Out;


    if ( args[0] != 0 )
      return;

    nIfs = 1;
    Out = 0;

    do
    {
      if ( SKIP_Code() == FAILURE )
        return;

      switch ( CUR.opcode )
      {
      case 0x58:      /* IF */
        nIfs++;
        break;

      case 0x1B:      /* ELSE */
        Out = FT_BOOL( nIfs == 1 );
        break;

      case 0x59:      /* EIF */
        nIfs--;
        Out = FT_BOOL( nIfs == 0 );
        break;
      }
    } while ( Out == 0 );
  }


  /*************************************************************************/
  /*                                                                       */
  /* ELSE[]:       ELSE                                                    */
  /* Opcode range: 0x1B                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_ELSE( INS_ARG )
  {
    FT_Int  nIfs;

    FT_UNUSED_ARG;


    nIfs = 1;

    do
    {
      if ( SKIP_Code() == FAILURE )
        return;

      switch ( CUR.opcode )
      {
      case 0x58:    /* IF */
        nIfs++;
        break;

      case 0x59:    /* EIF */
        nIfs--;
        break;
      }
    } while ( nIfs != 0 );
  }


  /*************************************************************************/
  /*                                                                       */
  /* DEFINING AND USING FUNCTIONS AND INSTRUCTIONS                         */
  /*                                                                       */
  /*   Instructions appear in the specification's order.                   */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* FDEF[]:       Function DEFinition                                     */
  /* Opcode range: 0x2C                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_FDEF( INS_ARG )
  {
    FT_ULong       n;
    TT_DefRecord*  rec;
    TT_DefRecord*  limit;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    /* arguments to opcodes are skipped by `SKIP_Code' */
    FT_Byte    opcode_pattern[9][12] = {
                 /* #0 inline delta function 1 */
                 {
                   0x4B, /* PPEM    */
                   0x53, /* GTEQ    */
                   0x23, /* SWAP    */
                   0x4B, /* PPEM    */
                   0x51, /* LTEQ    */
                   0x5A, /* AND     */
                   0x58, /* IF      */
                   0x38, /*   SHPIX */
                   0x1B, /* ELSE    */
                   0x21, /*   POP   */
                   0x21, /*   POP   */
                   0x59  /* EIF     */
                 },
                 /* #1 inline delta function 2 */
                 {
                   0x4B, /* PPEM    */
                   0x54, /* EQ      */
                   0x58, /* IF      */
                   0x38, /*   SHPIX */
                   0x1B, /* ELSE    */
                   0x21, /*   POP   */
                   0x21, /*   POP   */
                   0x59  /* EIF     */
                 },
                 /* #2 diagonal stroke function */
                 {
                   0x20, /* DUP     */
                   0x20, /* DUP     */
                   0xB0, /* PUSHB_1 */
                         /*   1     */
                   0x60, /* ADD     */
                   0x46, /* GC_cur  */
                   0xB0, /* PUSHB_1 */
                         /*   64    */
                   0x23, /* SWAP    */
                   0x42  /* WS      */
                 },
                 /* #3 VacuFormRound function */
                 {
                   0x45, /* RCVT    */
                   0x23, /* SWAP    */
                   0x46, /* GC_cur  */
                   0x60, /* ADD     */
                   0x20, /* DUP     */
                   0xB0  /* PUSHB_1 */
                         /*   38    */
                 },
                 /* #4 TTFautohint bytecode (old) */
                 {
                   0x20, /* DUP     */
                   0x64, /* ABS     */
                   0xB0, /* PUSHB_1 */
                         /*   32    */
                   0x60, /* ADD     */
                   0x66, /* FLOOR   */
                   0x23, /* SWAP    */
                   0xB0  /* PUSHB_1 */
                 },
                 /* #5 spacing function 1 */
                 {
                   0x01, /* SVTCA_x */
                   0xB0, /* PUSHB_1 */
                         /*   24    */
                   0x43, /* RS      */
                   0x58  /* IF      */
                 },
                 /* #6 spacing function 2 */
                 {
                   0x01, /* SVTCA_x */
                   0x18, /* RTG     */
                   0xB0, /* PUSHB_1 */
                         /*   24    */
                   0x43, /* RS      */
                   0x58  /* IF      */
                 },
                 /* #7 TypeMan Talk DiagEndCtrl function */
                 {
                   0x01, /* SVTCA_x */
                   0x20, /* DUP     */
                   0xB0, /* PUSHB_1 */
                         /*   3     */
                   0x25, /* CINDEX  */
                 },
                 /* #8 TypeMan Talk Align */
                 {
                   0x06, /* SPVTL   */
                   0x7D, /* RDTG    */
                 },
               };
    FT_UShort  opcode_patterns   = 9;
    FT_UShort  opcode_pointer[9] = {  0, 0, 0, 0, 0, 0, 0, 0, 0 };
    FT_UShort  opcode_size[9]    = { 12, 8, 8, 6, 7, 4, 5, 4, 2 };
    FT_UShort  i;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */


    /* some font programs are broken enough to redefine functions! */
    /* We will then parse the current table.                       */

    rec   = CUR.FDefs;
    limit = rec + CUR.numFDefs;
    n     = args[0];

    for ( ; rec < limit; rec++ )
    {
      if ( rec->opc == n )
        break;
    }

    if ( rec == limit )
    {
      /* check that there is enough room for new functions */
      if ( CUR.numFDefs >= CUR.maxFDefs )
      {
        CUR.error = FT_THROW( Too_Many_Function_Defs );
        return;
      }
      CUR.numFDefs++;
    }

    /* Although FDEF takes unsigned 32-bit integer,  */
    /* func # must be within unsigned 16-bit integer */
    if ( n > 0xFFFFU )
    {
      CUR.error = FT_THROW( Too_Many_Function_Defs );
      return;
    }

    rec->range          = CUR.curRange;
    rec->opc            = (FT_UInt16)n;
    rec->start          = CUR.IP + 1;
    rec->active         = TRUE;
    rec->inline_delta   = FALSE;
    rec->sph_fdef_flags = 0x0000;

    if ( n > CUR.maxFunc )
      CUR.maxFunc = (FT_UInt16)n;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    /* We don't know for sure these are typeman functions, */
    /* however they are only active when RS 22 is called   */
    if ( n >= 64 && n <= 66 )
      rec->sph_fdef_flags |= SPH_FDEF_TYPEMAN_STROKES;
#endif

    /* Now skip the whole function definition. */
    /* We don't allow nested IDEFS & FDEFs.    */

    while ( SKIP_Code() == SUCCESS )
    {

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

      if ( SUBPIXEL_HINTING )
      {
        for ( i = 0; i < opcode_patterns; i++ )
        {
          if ( opcode_pointer[i] < opcode_size[i]                 &&
               CUR.opcode == opcode_pattern[i][opcode_pointer[i]] )
          {
            opcode_pointer[i] += 1;

            if ( opcode_pointer[i] == opcode_size[i] )
            {
              FT_TRACE7(( "sph: Function %d, opcode ptrn: %d, %s %s\n",
                          i, n,
                          CUR.face->root.family_name,
                          CUR.face->root.style_name ));

              switch ( i )
              {
              case 0:
                rec->sph_fdef_flags            |= SPH_FDEF_INLINE_DELTA_1;
                CUR.face->sph_found_func_flags |= SPH_FDEF_INLINE_DELTA_1;
                break;

              case 1:
                rec->sph_fdef_flags            |= SPH_FDEF_INLINE_DELTA_2;
                CUR.face->sph_found_func_flags |= SPH_FDEF_INLINE_DELTA_2;
                break;

              case 2:
                switch ( n )
                {
                  /* needs to be implemented still */
                case 58:
                  rec->sph_fdef_flags            |= SPH_FDEF_DIAGONAL_STROKE;
                  CUR.face->sph_found_func_flags |= SPH_FDEF_DIAGONAL_STROKE;
                }
                break;

              case 3:
                switch ( n )
                {
                case 0:
                  rec->sph_fdef_flags            |= SPH_FDEF_VACUFORM_ROUND_1;
                  CUR.face->sph_found_func_flags |= SPH_FDEF_VACUFORM_ROUND_1;
                }
                break;

              case 4:
                /* probably not necessary to detect anymore */
                rec->sph_fdef_flags            |= SPH_FDEF_TTFAUTOHINT_1;
                CUR.face->sph_found_func_flags |= SPH_FDEF_TTFAUTOHINT_1;
                break;

              case 5:
                switch ( n )
                {
                case 0:
                case 1:
                case 2:
                case 4:
                case 7:
                case 8:
                  rec->sph_fdef_flags            |= SPH_FDEF_SPACING_1;
                  CUR.face->sph_found_func_flags |= SPH_FDEF_SPACING_1;
                }
                break;

              case 6:
                switch ( n )
                {
                case 0:
                case 1:
                case 2:
                case 4:
                case 7:
                case 8:
                  rec->sph_fdef_flags            |= SPH_FDEF_SPACING_2;
                  CUR.face->sph_found_func_flags |= SPH_FDEF_SPACING_2;
                }
                break;

               case 7:
                 rec->sph_fdef_flags            |= SPH_FDEF_TYPEMAN_DIAGENDCTRL;
                 CUR.face->sph_found_func_flags |= SPH_FDEF_TYPEMAN_DIAGENDCTRL;
                 break;

               case 8:
#if 0
                 rec->sph_fdef_flags            |= SPH_FDEF_TYPEMAN_DIAGENDCTRL;
                 CUR.face->sph_found_func_flags |= SPH_FDEF_TYPEMAN_DIAGENDCTRL;
#endif
                 break;
              }
              opcode_pointer[i] = 0;
            }
          }

          else
            opcode_pointer[i] = 0;
        }

        /* Set sph_compatibility_mode only when deltas are detected */
        CUR.face->sph_compatibility_mode =
          ( ( CUR.face->sph_found_func_flags & SPH_FDEF_INLINE_DELTA_1 ) |
            ( CUR.face->sph_found_func_flags & SPH_FDEF_INLINE_DELTA_2 ) );
      }

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      switch ( CUR.opcode )
      {
      case 0x89:    /* IDEF */
      case 0x2C:    /* FDEF */
        CUR.error = FT_THROW( Nested_DEFS );
        return;

      case 0x2D:   /* ENDF */
        rec->end = CUR.IP;
        return;
      }
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* ENDF[]:       END Function definition                                 */
  /* Opcode range: 0x2D                                                    */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_ENDF( INS_ARG )
  {
    TT_CallRec*  pRec;

    FT_UNUSED_ARG;


#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    CUR.sph_in_func_flags = 0x0000;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    if ( CUR.callTop <= 0 )     /* We encountered an ENDF without a call */
    {
      CUR.error = FT_THROW( ENDF_In_Exec_Stream );
      return;
    }

    CUR.callTop--;

    pRec = &CUR.callStack[CUR.callTop];

    pRec->Cur_Count--;

    CUR.step_ins = FALSE;

    if ( pRec->Cur_Count > 0 )
    {
      CUR.callTop++;
      CUR.IP = pRec->Def->start;
    }
    else
      /* Loop through the current function */
      INS_Goto_CodeRange( pRec->Caller_Range,
                          pRec->Caller_IP );

    /* Exit the current call frame.                      */

    /* NOTE: If the last instruction of a program is a   */
    /*       CALL or LOOPCALL, the return address is     */
    /*       always out of the code range.  This is a    */
    /*       valid address, and it is why we do not test */
    /*       the result of Ins_Goto_CodeRange() here!    */
  }


  /*************************************************************************/
  /*                                                                       */
  /* CALL[]:       CALL function                                           */
  /* Opcode range: 0x2B                                                    */
  /* Stack:        uint32? -->                                             */
  /*                                                                       */
  static void
  Ins_CALL( INS_ARG )
  {
    FT_ULong       F;
    TT_CallRec*    pCrec;
    TT_DefRecord*  def;


    /* first of all, check the index */

    F = args[0];
    if ( BOUNDSL( F, CUR.maxFunc + 1 ) )
      goto Fail;

    /* Except for some old Apple fonts, all functions in a TrueType */
    /* font are defined in increasing order, starting from 0.  This */
    /* means that we normally have                                  */
    /*                                                              */
    /*    CUR.maxFunc+1 == CUR.numFDefs                             */
    /*    CUR.FDefs[n].opc == n for n in 0..CUR.maxFunc             */
    /*                                                              */
    /* If this isn't true, we need to look up the function table.   */

    def = CUR.FDefs + F;
    if ( CUR.maxFunc + 1 != CUR.numFDefs || def->opc != F )
    {
      /* look up the FDefs table */
      TT_DefRecord*  limit;


      def   = CUR.FDefs;
      limit = def + CUR.numFDefs;

      while ( def < limit && def->opc != F )
        def++;

      if ( def == limit )
        goto Fail;
    }

    /* check that the function is active */
    if ( !def->active )
      goto Fail;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                              &&
         CUR.ignore_x_mode                                             &&
         ( ( CUR.iup_called                                        &&
             ( CUR.sph_tweak_flags & SPH_TWEAK_NO_CALL_AFTER_IUP ) ) ||
           ( def->sph_fdef_flags & SPH_FDEF_VACUFORM_ROUND_1 )       ) )
      goto Fail;
    else
      CUR.sph_in_func_flags = def->sph_fdef_flags;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    /* check the call stack */
    if ( CUR.callTop >= CUR.callSize )
    {
      CUR.error = FT_THROW( Stack_Overflow );
      return;
    }

    pCrec = CUR.callStack + CUR.callTop;

    pCrec->Caller_Range = CUR.curRange;
    pCrec->Caller_IP    = CUR.IP + 1;
    pCrec->Cur_Count    = 1;
    pCrec->Def          = def;

    CUR.callTop++;

    INS_Goto_CodeRange( def->range,
                        def->start );

    CUR.step_ins = FALSE;

    return;

  Fail:
    CUR.error = FT_THROW( Invalid_Reference );
  }


  /*************************************************************************/
  /*                                                                       */
  /* LOOPCALL[]:   LOOP and CALL function                                  */
  /* Opcode range: 0x2A                                                    */
  /* Stack:        uint32? Eint16? -->                                     */
  /*                                                                       */
  static void
  Ins_LOOPCALL( INS_ARG )
  {
    FT_ULong       F;
    TT_CallRec*    pCrec;
    TT_DefRecord*  def;


    /* first of all, check the index */
    F = args[1];
    if ( BOUNDSL( F, CUR.maxFunc + 1 ) )
      goto Fail;

    /* Except for some old Apple fonts, all functions in a TrueType */
    /* font are defined in increasing order, starting from 0.  This */
    /* means that we normally have                                  */
    /*                                                              */
    /*    CUR.maxFunc+1 == CUR.numFDefs                             */
    /*    CUR.FDefs[n].opc == n for n in 0..CUR.maxFunc             */
    /*                                                              */
    /* If this isn't true, we need to look up the function table.   */

    def = CUR.FDefs + F;
    if ( CUR.maxFunc + 1 != CUR.numFDefs || def->opc != F )
    {
      /* look up the FDefs table */
      TT_DefRecord*  limit;


      def   = CUR.FDefs;
      limit = def + CUR.numFDefs;

      while ( def < limit && def->opc != F )
        def++;

      if ( def == limit )
        goto Fail;
    }

    /* check that the function is active */
    if ( !def->active )
      goto Fail;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                    &&
         CUR.ignore_x_mode                                   &&
         ( def->sph_fdef_flags & SPH_FDEF_VACUFORM_ROUND_1 ) )
      goto Fail;
    else
      CUR.sph_in_func_flags = def->sph_fdef_flags;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    /* check stack */
    if ( CUR.callTop >= CUR.callSize )
    {
      CUR.error = FT_THROW( Stack_Overflow );
      return;
    }

    if ( args[0] > 0 )
    {
      pCrec = CUR.callStack + CUR.callTop;

      pCrec->Caller_Range = CUR.curRange;
      pCrec->Caller_IP    = CUR.IP + 1;
      pCrec->Cur_Count    = (FT_Int)args[0];
      pCrec->Def          = def;

      CUR.callTop++;

      INS_Goto_CodeRange( def->range, def->start );

      CUR.step_ins = FALSE;
    }

    return;

  Fail:
    CUR.error = FT_THROW( Invalid_Reference );
  }


  /*************************************************************************/
  /*                                                                       */
  /* IDEF[]:       Instruction DEFinition                                  */
  /* Opcode range: 0x89                                                    */
  /* Stack:        Eint8 -->                                               */
  /*                                                                       */
  static void
  Ins_IDEF( INS_ARG )
  {
    TT_DefRecord*  def;
    TT_DefRecord*  limit;


    /*  First of all, look for the same function in our table */

    def   = CUR.IDefs;
    limit = def + CUR.numIDefs;

    for ( ; def < limit; def++ )
      if ( def->opc == (FT_ULong)args[0] )
        break;

    if ( def == limit )
    {
      /* check that there is enough room for a new instruction */
      if ( CUR.numIDefs >= CUR.maxIDefs )
      {
        CUR.error = FT_THROW( Too_Many_Instruction_Defs );
        return;
      }
      CUR.numIDefs++;
    }

    /* opcode must be unsigned 8-bit integer */
    if ( 0 > args[0] || args[0] > 0x00FF )
    {
      CUR.error = FT_THROW( Too_Many_Instruction_Defs );
      return;
    }

    def->opc    = (FT_Byte)args[0];
    def->start  = CUR.IP + 1;
    def->range  = CUR.curRange;
    def->active = TRUE;

    if ( (FT_ULong)args[0] > CUR.maxIns )
      CUR.maxIns = (FT_Byte)args[0];

    /* Now skip the whole function definition. */
    /* We don't allow nested IDEFs & FDEFs.    */

    while ( SKIP_Code() == SUCCESS )
    {
      switch ( CUR.opcode )
      {
      case 0x89:   /* IDEF */
      case 0x2C:   /* FDEF */
        CUR.error = FT_THROW( Nested_DEFS );
        return;
      case 0x2D:   /* ENDF */
        return;
      }
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* PUSHING DATA ONTO THE INTERPRETER STACK                               */
  /*                                                                       */
  /*   Instructions appear in the specification's order.                   */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* NPUSHB[]:     PUSH N Bytes                                            */
  /* Opcode range: 0x40                                                    */
  /* Stack:        --> uint32...                                           */
  /*                                                                       */
  static void
  Ins_NPUSHB( INS_ARG )
  {
    FT_UShort  L, K;


    L = (FT_UShort)CUR.code[CUR.IP + 1];

    if ( BOUNDS( L, CUR.stackSize + 1 - CUR.top ) )
    {
      CUR.error = FT_THROW( Stack_Overflow );
      return;
    }

    for ( K = 1; K <= L; K++ )
      args[K - 1] = CUR.code[CUR.IP + K + 1];

    CUR.new_top += L;
  }


  /*************************************************************************/
  /*                                                                       */
  /* NPUSHW[]:     PUSH N Words                                            */
  /* Opcode range: 0x41                                                    */
  /* Stack:        --> int32...                                            */
  /*                                                                       */
  static void
  Ins_NPUSHW( INS_ARG )
  {
    FT_UShort  L, K;


    L = (FT_UShort)CUR.code[CUR.IP + 1];

    if ( BOUNDS( L, CUR.stackSize + 1 - CUR.top ) )
    {
      CUR.error = FT_THROW( Stack_Overflow );
      return;
    }

    CUR.IP += 2;

    for ( K = 0; K < L; K++ )
      args[K] = GET_ShortIns();

    CUR.step_ins = FALSE;
    CUR.new_top += L;
  }


  /*************************************************************************/
  /*                                                                       */
  /* PUSHB[abc]:   PUSH Bytes                                              */
  /* Opcode range: 0xB0-0xB7                                               */
  /* Stack:        --> uint32...                                           */
  /*                                                                       */
  static void
  Ins_PUSHB( INS_ARG )
  {
    FT_UShort  L, K;


    L = (FT_UShort)( CUR.opcode - 0xB0 + 1 );

    if ( BOUNDS( L, CUR.stackSize + 1 - CUR.top ) )
    {
      CUR.error = FT_THROW( Stack_Overflow );
      return;
    }

    for ( K = 1; K <= L; K++ )
      args[K - 1] = CUR.code[CUR.IP + K];
  }


  /*************************************************************************/
  /*                                                                       */
  /* PUSHW[abc]:   PUSH Words                                              */
  /* Opcode range: 0xB8-0xBF                                               */
  /* Stack:        --> int32...                                            */
  /*                                                                       */
  static void
  Ins_PUSHW( INS_ARG )
  {
    FT_UShort  L, K;


    L = (FT_UShort)( CUR.opcode - 0xB8 + 1 );

    if ( BOUNDS( L, CUR.stackSize + 1 - CUR.top ) )
    {
      CUR.error = FT_THROW( Stack_Overflow );
      return;
    }

    CUR.IP++;

    for ( K = 0; K < L; K++ )
      args[K] = GET_ShortIns();

    CUR.step_ins = FALSE;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MANAGING THE GRAPHICS STATE                                           */
  /*                                                                       */
  /*  Instructions appear in the specs' order.                             */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* GC[a]:        Get Coordinate projected onto                           */
  /* Opcode range: 0x46-0x47                                               */
  /* Stack:        uint32 --> f26.6                                        */
  /*                                                                       */
  /* XXX: UNDOCUMENTED: Measures from the original glyph must be taken     */
  /*      along the dual projection vector!                                */
  /*                                                                       */
  static void
  Ins_GC( INS_ARG )
  {
    FT_ULong    L;
    FT_F26Dot6  R;


    L = (FT_ULong)args[0];

    if ( BOUNDSL( L, CUR.zp2.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      R = 0;
    }
    else
    {
      if ( CUR.opcode & 1 )
        R = CUR_fast_dualproj( &CUR.zp2.org[L] );
      else
        R = CUR_fast_project( &CUR.zp2.cur[L] );
    }

    args[0] = R;
  }


  /*************************************************************************/
  /*                                                                       */
  /* SCFS[]:       Set Coordinate From Stack                               */
  /* Opcode range: 0x48                                                    */
  /* Stack:        f26.6 uint32 -->                                        */
  /*                                                                       */
  /* Formula:                                                              */
  /*                                                                       */
  /*   OA := OA + ( value - OA.p )/( f.p ) * f                             */
  /*                                                                       */
  static void
  Ins_SCFS( INS_ARG )
  {
    FT_Long    K;
    FT_UShort  L;


    L = (FT_UShort)args[0];

    if ( BOUNDS( L, CUR.zp2.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    K = CUR_fast_project( &CUR.zp2.cur[L] );

    CUR_Func_move( &CUR.zp2, L, args[1] - K );

    /* UNDOCUMENTED!  The MS rasterizer does that with */
    /* twilight points (confirmed by Greg Hitchcock)   */
    if ( CUR.GS.gep2 == 0 )
      CUR.zp2.org[L] = CUR.zp2.cur[L];
  }


  /*************************************************************************/
  /*                                                                       */
  /* MD[a]:        Measure Distance                                        */
  /* Opcode range: 0x49-0x4A                                               */
  /* Stack:        uint32 uint32 --> f26.6                                 */
  /*                                                                       */
  /* XXX: UNDOCUMENTED: Measure taken in the original glyph must be along  */
  /*                    the dual projection vector.                        */
  /*                                                                       */
  /* XXX: UNDOCUMENTED: Flag attributes are inverted!                      */
  /*                      0 => measure distance in original outline        */
  /*                      1 => measure distance in grid-fitted outline     */
  /*                                                                       */
  /* XXX: UNDOCUMENTED: `zp0 - zp1', and not `zp2 - zp1!                   */
  /*                                                                       */
  static void
  Ins_MD( INS_ARG )
  {
    FT_UShort   K, L;
    FT_F26Dot6  D;


    K = (FT_UShort)args[1];
    L = (FT_UShort)args[0];

    if ( BOUNDS( L, CUR.zp0.n_points ) ||
         BOUNDS( K, CUR.zp1.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      D = 0;
    }
    else
    {
      if ( CUR.opcode & 1 )
        D = CUR_Func_project( CUR.zp0.cur + L, CUR.zp1.cur + K );
      else
      {
        /* XXX: UNDOCUMENTED: twilight zone special case */

        if ( CUR.GS.gep0 == 0 || CUR.GS.gep1 == 0 )
        {
          FT_Vector*  vec1 = CUR.zp0.org + L;
          FT_Vector*  vec2 = CUR.zp1.org + K;


          D = CUR_Func_dualproj( vec1, vec2 );
        }
        else
        {
          FT_Vector*  vec1 = CUR.zp0.orus + L;
          FT_Vector*  vec2 = CUR.zp1.orus + K;


          if ( CUR.metrics.x_scale == CUR.metrics.y_scale )
          {
            /* this should be faster */
            D = CUR_Func_dualproj( vec1, vec2 );
            D = FT_MulFix( D, CUR.metrics.x_scale );
          }
          else
          {
            FT_Vector  vec;


            vec.x = FT_MulFix( vec1->x - vec2->x, CUR.metrics.x_scale );
            vec.y = FT_MulFix( vec1->y - vec2->y, CUR.metrics.y_scale );

            D = CUR_fast_dualproj( &vec );
          }
        }
      }
    }

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    /* Disable Type 2 Vacuform Rounds - e.g. Arial Narrow */
    if ( SUBPIXEL_HINTING                       &&
         CUR.ignore_x_mode && FT_ABS( D ) == 64 )
      D += 1;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    args[0] = D;
  }


  /*************************************************************************/
  /*                                                                       */
  /* SDPVTL[a]:    Set Dual PVector to Line                                */
  /* Opcode range: 0x86-0x87                                               */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_SDPVTL( INS_ARG )
  {
    FT_Long    A, B, C;
    FT_UShort  p1, p2;            /* was FT_Int in pas type ERROR */
    FT_Int     aOpc = CUR.opcode;


    p1 = (FT_UShort)args[1];
    p2 = (FT_UShort)args[0];

    if ( BOUNDS( p2, CUR.zp1.n_points ) ||
         BOUNDS( p1, CUR.zp2.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    {
      FT_Vector* v1 = CUR.zp1.org + p2;
      FT_Vector* v2 = CUR.zp2.org + p1;


      A = v1->x - v2->x;
      B = v1->y - v2->y;

      /* If v1 == v2, SDPVTL behaves the same as */
      /* SVTCA[X], respectively.                 */
      /*                                         */
      /* Confirmed by Greg Hitchcock.            */

      if ( A == 0 && B == 0 )
      {
        A    = 0x4000;
        aOpc = 0;
      }
    }

    if ( ( aOpc & 1 ) != 0 )
    {
      C =  B;   /* counter clockwise rotation */
      B =  A;
      A = -C;
    }

    NORMalize( A, B, &CUR.GS.dualVector );

    {
      FT_Vector*  v1 = CUR.zp1.cur + p2;
      FT_Vector*  v2 = CUR.zp2.cur + p1;


      A = v1->x - v2->x;
      B = v1->y - v2->y;

      if ( A == 0 && B == 0 )
      {
        A    = 0x4000;
        aOpc = 0;
      }
    }

    if ( ( aOpc & 1 ) != 0 )
    {
      C =  B;   /* counter clockwise rotation */
      B =  A;
      A = -C;
    }

    NORMalize( A, B, &CUR.GS.projVector );

    GUESS_VECTOR( freeVector );

    COMPUTE_Funcs();
  }


  /*************************************************************************/
  /*                                                                       */
  /* SZP0[]:       Set Zone Pointer 0                                      */
  /* Opcode range: 0x13                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SZP0( INS_ARG )
  {
    switch ( (FT_Int)args[0] )
    {
    case 0:
      CUR.zp0 = CUR.twilight;
      break;

    case 1:
      CUR.zp0 = CUR.pts;
      break;

    default:
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    CUR.GS.gep0 = (FT_UShort)args[0];
  }


  /*************************************************************************/
  /*                                                                       */
  /* SZP1[]:       Set Zone Pointer 1                                      */
  /* Opcode range: 0x14                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SZP1( INS_ARG )
  {
    switch ( (FT_Int)args[0] )
    {
    case 0:
      CUR.zp1 = CUR.twilight;
      break;

    case 1:
      CUR.zp1 = CUR.pts;
      break;

    default:
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    CUR.GS.gep1 = (FT_UShort)args[0];
  }


  /*************************************************************************/
  /*                                                                       */
  /* SZP2[]:       Set Zone Pointer 2                                      */
  /* Opcode range: 0x15                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SZP2( INS_ARG )
  {
    switch ( (FT_Int)args[0] )
    {
    case 0:
      CUR.zp2 = CUR.twilight;
      break;

    case 1:
      CUR.zp2 = CUR.pts;
      break;

    default:
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    CUR.GS.gep2 = (FT_UShort)args[0];
  }


  /*************************************************************************/
  /*                                                                       */
  /* SZPS[]:       Set Zone PointerS                                       */
  /* Opcode range: 0x16                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SZPS( INS_ARG )
  {
    switch ( (FT_Int)args[0] )
    {
    case 0:
      CUR.zp0 = CUR.twilight;
      break;

    case 1:
      CUR.zp0 = CUR.pts;
      break;

    default:
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    CUR.zp1 = CUR.zp0;
    CUR.zp2 = CUR.zp0;

    CUR.GS.gep0 = (FT_UShort)args[0];
    CUR.GS.gep1 = (FT_UShort)args[0];
    CUR.GS.gep2 = (FT_UShort)args[0];
  }


  /*************************************************************************/
  /*                                                                       */
  /* INSTCTRL[]:   INSTruction ConTRoL                                     */
  /* Opcode range: 0x8e                                                    */
  /* Stack:        int32 int32 -->                                         */
  /*                                                                       */
  static void
  Ins_INSTCTRL( INS_ARG )
  {
    FT_Long  K, L;


    K = args[1];
    L = args[0];

    if ( K < 1 || K > 2 )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    if ( L != 0 )
        L = K;

    CUR.GS.instruct_control = FT_BOOL(
      ( (FT_Byte)CUR.GS.instruct_control & ~(FT_Byte)K ) | (FT_Byte)L );
  }


  /*************************************************************************/
  /*                                                                       */
  /* SCANCTRL[]:   SCAN ConTRoL                                            */
  /* Opcode range: 0x85                                                    */
  /* Stack:        uint32? -->                                             */
  /*                                                                       */
  static void
  Ins_SCANCTRL( INS_ARG )
  {
    FT_Int  A;


    /* Get Threshold */
    A = (FT_Int)( args[0] & 0xFF );

    if ( A == 0xFF )
    {
      CUR.GS.scan_control = TRUE;
      return;
    }
    else if ( A == 0 )
    {
      CUR.GS.scan_control = FALSE;
      return;
    }

    if ( ( args[0] & 0x100 ) != 0 && CUR.tt_metrics.ppem <= A )
      CUR.GS.scan_control = TRUE;

    if ( ( args[0] & 0x200 ) != 0 && CUR.tt_metrics.rotated )
      CUR.GS.scan_control = TRUE;

    if ( ( args[0] & 0x400 ) != 0 && CUR.tt_metrics.stretched )
      CUR.GS.scan_control = TRUE;

    if ( ( args[0] & 0x800 ) != 0 && CUR.tt_metrics.ppem > A )
      CUR.GS.scan_control = FALSE;

    if ( ( args[0] & 0x1000 ) != 0 && CUR.tt_metrics.rotated )
      CUR.GS.scan_control = FALSE;

    if ( ( args[0] & 0x2000 ) != 0 && CUR.tt_metrics.stretched )
      CUR.GS.scan_control = FALSE;
  }


  /*************************************************************************/
  /*                                                                       */
  /* SCANTYPE[]:   SCAN TYPE                                               */
  /* Opcode range: 0x8D                                                    */
  /* Stack:        uint32? -->                                             */
  /*                                                                       */
  static void
  Ins_SCANTYPE( INS_ARG )
  {
    if ( args[0] >= 0 )
      CUR.GS.scan_type = (FT_Int)args[0];
  }


  /*************************************************************************/
  /*                                                                       */
  /* MANAGING OUTLINES                                                     */
  /*                                                                       */
  /*   Instructions appear in the specification's order.                   */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* FLIPPT[]:     FLIP PoinT                                              */
  /* Opcode range: 0x80                                                    */
  /* Stack:        uint32... -->                                           */
  /*                                                                       */
  static void
  Ins_FLIPPT( INS_ARG )
  {
    FT_UShort  point;

    FT_UNUSED_ARG;


    if ( CUR.top < CUR.GS.loop )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Too_Few_Arguments );
      goto Fail;
    }

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;

      point = (FT_UShort)CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.pts.n_points ) )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Invalid_Reference );
          return;
        }
      }
      else
        CUR.pts.tags[point] ^= FT_CURVE_TAG_ON;

      CUR.GS.loop--;
    }

  Fail:
    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* FLIPRGON[]:   FLIP RanGe ON                                           */
  /* Opcode range: 0x81                                                    */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_FLIPRGON( INS_ARG )
  {
    FT_UShort  I, K, L;


    K = (FT_UShort)args[1];
    L = (FT_UShort)args[0];

    if ( BOUNDS( K, CUR.pts.n_points ) ||
         BOUNDS( L, CUR.pts.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    for ( I = L; I <= K; I++ )
      CUR.pts.tags[I] |= FT_CURVE_TAG_ON;
  }


  /*************************************************************************/
  /*                                                                       */
  /* FLIPRGOFF:    FLIP RanGe OFF                                          */
  /* Opcode range: 0x82                                                    */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_FLIPRGOFF( INS_ARG )
  {
    FT_UShort  I, K, L;


    K = (FT_UShort)args[1];
    L = (FT_UShort)args[0];

    if ( BOUNDS( K, CUR.pts.n_points ) ||
         BOUNDS( L, CUR.pts.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    for ( I = L; I <= K; I++ )
      CUR.pts.tags[I] &= ~FT_CURVE_TAG_ON;
  }


  static FT_Bool
  Compute_Point_Displacement( EXEC_OP_ FT_F26Dot6*   x,
                                       FT_F26Dot6*   y,
                                       TT_GlyphZone  zone,
                                       FT_UShort*    refp )
  {
    TT_GlyphZoneRec  zp;
    FT_UShort        p;
    FT_F26Dot6       d;


    if ( CUR.opcode & 1 )
    {
      zp = CUR.zp0;
      p  = CUR.GS.rp1;
    }
    else
    {
      zp = CUR.zp1;
      p  = CUR.GS.rp2;
    }

    if ( BOUNDS( p, zp.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      *refp = 0;
      return FAILURE;
    }

    *zone = zp;
    *refp = p;

    d = CUR_Func_project( zp.cur + p, zp.org + p );

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    if ( CUR.face->unpatented_hinting )
    {
      if ( CUR.GS.both_x_axis )
      {
        *x = d;
        *y = 0;
      }
      else
      {
        *x = 0;
        *y = d;
      }
    }
    else
#endif
    {
      *x = FT_MulDiv( d, (FT_Long)CUR.GS.freeVector.x, CUR.F_dot_P );
      *y = FT_MulDiv( d, (FT_Long)CUR.GS.freeVector.y, CUR.F_dot_P );
    }

    return SUCCESS;
  }


  static void
  Move_Zp2_Point( EXEC_OP_ FT_UShort   point,
                           FT_F26Dot6  dx,
                           FT_F26Dot6  dy,
                           FT_Bool     touch )
  {
#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    if ( CUR.face->unpatented_hinting )
    {
      if ( CUR.GS.both_x_axis )
      {
        CUR.zp2.cur[point].x += dx;
        if ( touch )
          CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_X;
      }
      else
      {
        CUR.zp2.cur[point].y += dy;
        if ( touch )
          CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_Y;
      }
      return;
    }
#endif

    if ( CUR.GS.freeVector.x != 0 )
    {
      CUR.zp2.cur[point].x += dx;
      if ( touch )
        CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_X;
    }

    if ( CUR.GS.freeVector.y != 0 )
    {
      CUR.zp2.cur[point].y += dy;
      if ( touch )
        CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_Y;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* SHP[a]:       SHift Point by the last point                           */
  /* Opcode range: 0x32-0x33                                               */
  /* Stack:        uint32... -->                                           */
  /*                                                                       */
  static void
  Ins_SHP( INS_ARG )
  {
    TT_GlyphZoneRec  zp;
    FT_UShort        refp;

    FT_F26Dot6       dx,
                     dy;
    FT_UShort        point;

    FT_UNUSED_ARG;


    if ( CUR.top < CUR.GS.loop )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    if ( COMPUTE_Point_Displacement( &dx, &dy, &zp, &refp ) )
      return;

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;
      point = (FT_UShort)CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.zp2.n_points ) )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Invalid_Reference );
          return;
        }
      }
      else
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      /* doesn't follow Cleartype spec but produces better result */
      if ( SUBPIXEL_HINTING  &&
           CUR.ignore_x_mode )
        MOVE_Zp2_Point( point, 0, dy, TRUE );
      else
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
        MOVE_Zp2_Point( point, dx, dy, TRUE );

      CUR.GS.loop--;
    }

  Fail:
    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* SHC[a]:       SHift Contour                                           */
  /* Opcode range: 0x34-35                                                 */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  /* UNDOCUMENTED: According to Greg Hitchcock, there is one (virtual)     */
  /*               contour in the twilight zone, namely contour number     */
  /*               zero which includes all points of it.                   */
  /*                                                                       */
  static void
  Ins_SHC( INS_ARG )
  {
    TT_GlyphZoneRec  zp;
    FT_UShort        refp;
    FT_F26Dot6       dx, dy;

    FT_Short         contour, bounds;
    FT_UShort        start, limit, i;


    contour = (FT_UShort)args[0];
    bounds  = ( CUR.GS.gep2 == 0 ) ? 1 : CUR.zp2.n_contours;

    if ( BOUNDS( contour, bounds ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    if ( COMPUTE_Point_Displacement( &dx, &dy, &zp, &refp ) )
      return;

    if ( contour == 0 )
      start = 0;
    else
      start = (FT_UShort)( CUR.zp2.contours[contour - 1] + 1 -
                           CUR.zp2.first_point );

    /* we use the number of points if in the twilight zone */
    if ( CUR.GS.gep2 == 0 )
      limit = CUR.zp2.n_points;
    else
      limit = (FT_UShort)( CUR.zp2.contours[contour] -
                           CUR.zp2.first_point + 1 );

    for ( i = start; i < limit; i++ )
    {
      if ( zp.cur != CUR.zp2.cur || refp != i )
        MOVE_Zp2_Point( i, dx, dy, TRUE );
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* SHZ[a]:       SHift Zone                                              */
  /* Opcode range: 0x36-37                                                 */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_SHZ( INS_ARG )
  {
    TT_GlyphZoneRec  zp;
    FT_UShort        refp;
    FT_F26Dot6       dx,
                     dy;

    FT_UShort        limit, i;


    if ( BOUNDS( args[0], 2 ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    if ( COMPUTE_Point_Displacement( &dx, &dy, &zp, &refp ) )
      return;

    /* XXX: UNDOCUMENTED! SHZ doesn't move the phantom points.     */
    /*      Twilight zone has no real contours, so use `n_points'. */
    /*      Normal zone's `n_points' includes phantoms, so must    */
    /*      use end of last contour.                               */
    if ( CUR.GS.gep2 == 0 )
      limit = (FT_UShort)CUR.zp2.n_points;
    else if ( CUR.GS.gep2 == 1 && CUR.zp2.n_contours > 0 )
      limit = (FT_UShort)( CUR.zp2.contours[CUR.zp2.n_contours - 1] + 1 );
    else
      limit = 0;

    /* XXX: UNDOCUMENTED! SHZ doesn't touch the points */
    for ( i = 0; i < limit; i++ )
    {
      if ( zp.cur != CUR.zp2.cur || refp != i )
        MOVE_Zp2_Point( i, dx, dy, FALSE );
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* SHPIX[]:      SHift points by a PIXel amount                          */
  /* Opcode range: 0x38                                                    */
  /* Stack:        f26.6 uint32... -->                                     */
  /*                                                                       */
  static void
  Ins_SHPIX( INS_ARG )
  {
    FT_F26Dot6  dx, dy;
    FT_UShort   point;
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    FT_Int      B1, B2;
#endif


    if ( CUR.top < CUR.GS.loop + 1 )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    if ( CUR.face->unpatented_hinting )
    {
      if ( CUR.GS.both_x_axis )
      {
        dx = (FT_UInt32)args[0];
        dy = 0;
      }
      else
      {
        dx = 0;
        dy = (FT_UInt32)args[0];
      }
    }
    else
#endif
    {
      dx = TT_MulFix14( (FT_UInt32)args[0], CUR.GS.freeVector.x );
      dy = TT_MulFix14( (FT_UInt32)args[0], CUR.GS.freeVector.y );
    }

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;

      point = (FT_UShort)CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.zp2.n_points ) )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Invalid_Reference );
          return;
        }
      }
      else
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      {
        /*  If not using ignore_x_mode rendering, allow ZP2 move.          */
        /*  If inline deltas aren't allowed, skip ZP2 move.                */
        /*  If using ignore_x_mode rendering, allow ZP2 point move if:     */
        /*   - freedom vector is y and sph_compatibility_mode is off       */
        /*   - the glyph is composite and the move is in the Y direction   */
        /*   - the glyph is specifically set to allow SHPIX moves          */
        /*   - the move is on a previously Y-touched point                 */

        if ( SUBPIXEL_HINTING  &&
             CUR.ignore_x_mode )
        {
          /* save point for later comparison */
          if ( CUR.GS.freeVector.y != 0 )
            B1 = CUR.zp2.cur[point].y;
          else
            B1 = CUR.zp2.cur[point].x;

          if ( !CUR.face->sph_compatibility_mode &&
               CUR.GS.freeVector.y != 0          )
          {
            MOVE_Zp2_Point( point, dx, dy, TRUE );

            /* save new point */
            if ( CUR.GS.freeVector.y != 0 )
            {
              B2 = CUR.zp2.cur[point].y;

              /* reverse any disallowed moves */
              if ( ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_NONPIXEL_Y_MOVES ) &&
                   ( B1 & 63 ) != 0                                          &&
                   ( B2 & 63 ) != 0                                          &&
                    B1 != B2                                                 )
                MOVE_Zp2_Point( point, -dx, -dy, TRUE );
            }
          }
          else if ( CUR.face->sph_compatibility_mode )
          {
            if ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_Y_MOVES )
            {
              dx = FT_PIX_ROUND( B1 + dx ) - B1;
              dy = FT_PIX_ROUND( B1 + dy ) - B1;
            }

            /* skip post-iup deltas */
            if ( CUR.iup_called                                          &&
                 ( ( CUR.sph_in_func_flags & SPH_FDEF_INLINE_DELTA_1 ) ||
                   ( CUR.sph_in_func_flags & SPH_FDEF_INLINE_DELTA_2 ) ) )
              goto Skip;

            if ( !( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_SKIP_DELTAP ) &&
                  ( ( CUR.is_composite && CUR.GS.freeVector.y != 0 ) ||
                    ( CUR.zp2.tags[point] & FT_CURVE_TAG_TOUCH_Y )   ||
                    ( CUR.sph_tweak_flags & SPH_TWEAK_DO_SHPIX )     )   )
              MOVE_Zp2_Point( point, 0, dy, TRUE );

            /* save new point */
            if ( CUR.GS.freeVector.y != 0 )
            {
              B2 = CUR.zp2.cur[point].y;

              /* reverse any disallowed moves */
              if ( ( B1 & 63 ) == 0 &&
                   ( B2 & 63 ) != 0 &&
                   B1 != B2         )
                MOVE_Zp2_Point( point, 0, -dy, TRUE );
            }
          }
          else if ( CUR.sph_in_func_flags & SPH_FDEF_TYPEMAN_DIAGENDCTRL )
            MOVE_Zp2_Point( point, dx, dy, TRUE );
        }
        else
          MOVE_Zp2_Point( point, dx, dy, TRUE );
      }

    Skip:

#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */

        MOVE_Zp2_Point( point, dx, dy, TRUE );

#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      CUR.GS.loop--;
    }

  Fail:
    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MSIRP[a]:     Move Stack Indirect Relative Position                   */
  /* Opcode range: 0x3A-0x3B                                               */
  /* Stack:        f26.6 uint32 -->                                        */
  /*                                                                       */
  static void
  Ins_MSIRP( INS_ARG )
  {
    FT_UShort   point;
    FT_F26Dot6  distance;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    FT_F26Dot6  control_value_cutin = 0; /* pacify compiler */


    if ( SUBPIXEL_HINTING )
    {
      control_value_cutin = CUR.GS.control_value_cutin;

      if ( CUR.ignore_x_mode                                 &&
           CUR.GS.freeVector.x != 0                          &&
           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
        control_value_cutin = 0;
    }

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    point = (FT_UShort)args[0];

    if ( BOUNDS( point,      CUR.zp1.n_points ) ||
         BOUNDS( CUR.GS.rp0, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    /* UNDOCUMENTED!  The MS rasterizer does that with */
    /* twilight points (confirmed by Greg Hitchcock)   */
    if ( CUR.GS.gep1 == 0 )
    {
      CUR.zp1.org[point] = CUR.zp0.org[CUR.GS.rp0];
      CUR_Func_move_orig( &CUR.zp1, point, args[1] );
      CUR.zp1.cur[point] = CUR.zp1.org[point];
    }

    distance = CUR_Func_project( CUR.zp1.cur + point,
                                 CUR.zp0.cur + CUR.GS.rp0 );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    /* subpixel hinting - make MSIRP respect CVT cut-in; */
    if ( SUBPIXEL_HINTING                                    &&
         CUR.ignore_x_mode                                   &&
         CUR.GS.freeVector.x != 0                            &&
         FT_ABS( distance - args[1] ) >= control_value_cutin )
      distance = args[1];
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    CUR_Func_move( &CUR.zp1, point, args[1] - distance );

    CUR.GS.rp1 = CUR.GS.rp0;
    CUR.GS.rp2 = point;

    if ( ( CUR.opcode & 1 ) != 0 )
      CUR.GS.rp0 = point;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MDAP[a]:      Move Direct Absolute Point                              */
  /* Opcode range: 0x2E-0x2F                                               */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_MDAP( INS_ARG )
  {
    FT_UShort   point;
    FT_F26Dot6  cur_dist;
    FT_F26Dot6  distance;


    point = (FT_UShort)args[0];

    if ( BOUNDS( point, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    if ( ( CUR.opcode & 1 ) != 0 )
    {
      cur_dist = CUR_fast_project( &CUR.zp0.cur[point] );
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      if ( SUBPIXEL_HINTING         &&
           CUR.ignore_x_mode        &&
           CUR.GS.freeVector.x != 0 )
        distance = ROUND_None(
                     cur_dist,
                     CUR.tt_metrics.compensations[0] ) - cur_dist;
      else
#endif
        distance = CUR_Func_round(
                     cur_dist,
                     CUR.tt_metrics.compensations[0] ) - cur_dist;
    }
    else
      distance = 0;

    CUR_Func_move( &CUR.zp0, point, distance );

    CUR.GS.rp0 = point;
    CUR.GS.rp1 = point;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MIAP[a]:      Move Indirect Absolute Point                            */
  /* Opcode range: 0x3E-0x3F                                               */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_MIAP( INS_ARG )
  {
    FT_ULong    cvtEntry;
    FT_UShort   point;
    FT_F26Dot6  distance;
    FT_F26Dot6  org_dist;
    FT_F26Dot6  control_value_cutin;


    control_value_cutin = CUR.GS.control_value_cutin;
    cvtEntry            = (FT_ULong)args[1];
    point               = (FT_UShort)args[0];

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                  &&
         CUR.ignore_x_mode                                 &&
         CUR.GS.freeVector.x != 0                          &&
         CUR.GS.freeVector.y == 0                          &&
         !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
      control_value_cutin = 0;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    if ( BOUNDS( point,     CUR.zp0.n_points ) ||
         BOUNDSL( cvtEntry, CUR.cvtSize )      )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    /* UNDOCUMENTED!                                                      */
    /*                                                                    */
    /* The behaviour of an MIAP instruction is quite different when used  */
    /* in the twilight zone.                                              */
    /*                                                                    */
    /* First, no control value cut-in test is performed as it would fail  */
    /* anyway.  Second, the original point, i.e. (org_x,org_y) of         */
    /* zp0.point, is set to the absolute, unrounded distance found in the */
    /* CVT.                                                               */
    /*                                                                    */
    /* This is used in the CVT programs of the Microsoft fonts Arial,     */
    /* Times, etc., in order to re-adjust some key font heights.  It      */
    /* allows the use of the IP instruction in the twilight zone, which   */
    /* otherwise would be invalid according to the specification.         */
    /*                                                                    */
    /* We implement it with a special sequence for the twilight zone.     */
    /* This is a bad hack, but it seems to work.                          */
    /*                                                                    */
    /* Confirmed by Greg Hitchcock.                                       */

    distance = CUR_Func_read_cvt( cvtEntry );

    if ( CUR.GS.gep0 == 0 )   /* If in twilight zone */
    {
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      /* Only adjust if not in sph_compatibility_mode or ignore_x_mode. */
      /* Determined via experimentation and may be incorrect...         */
      if ( !SUBPIXEL_HINTING                     ||
           ( !CUR.ignore_x_mode                ||
             !CUR.face->sph_compatibility_mode ) )
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
        CUR.zp0.org[point].x = TT_MulFix14( (FT_UInt32)distance,
                                            CUR.GS.freeVector.x );
      CUR.zp0.org[point].y = TT_MulFix14( (FT_UInt32)distance,
                                          CUR.GS.freeVector.y ),
      CUR.zp0.cur[point]   = CUR.zp0.org[point];
    }
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                              &&
         CUR.ignore_x_mode                             &&
         ( CUR.sph_tweak_flags & SPH_TWEAK_MIAP_HACK ) &&
         distance > 0                                  &&
         CUR.GS.freeVector.y != 0                      )
      distance = 0;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    org_dist = CUR_fast_project( &CUR.zp0.cur[point] );

    if ( ( CUR.opcode & 1 ) != 0 )   /* rounding and control cut-in flag */
    {
      if ( FT_ABS( distance - org_dist ) > control_value_cutin )
        distance = org_dist;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      if ( SUBPIXEL_HINTING         &&
           CUR.ignore_x_mode        &&
           CUR.GS.freeVector.x != 0 )
        distance = ROUND_None( distance,
                               CUR.tt_metrics.compensations[0] );
      else
#endif
        distance = CUR_Func_round( distance,
                                   CUR.tt_metrics.compensations[0] );
    }

    CUR_Func_move( &CUR.zp0, point, distance - org_dist );

  Fail:
    CUR.GS.rp0 = point;
    CUR.GS.rp1 = point;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MDRP[abcde]:  Move Direct Relative Point                              */
  /* Opcode range: 0xC0-0xDF                                               */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_MDRP( INS_ARG )
  {
    FT_UShort   point;
    FT_F26Dot6  org_dist, distance, minimum_distance;


    minimum_distance = CUR.GS.minimum_distance;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                  &&
         CUR.ignore_x_mode                                 &&
         CUR.GS.freeVector.x != 0                          &&
         !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
      minimum_distance = 0;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    point = (FT_UShort)args[0];

    if ( BOUNDS( point,      CUR.zp1.n_points ) ||
         BOUNDS( CUR.GS.rp0, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    /* XXX: Is there some undocumented feature while in the */
    /*      twilight zone?                                  */

    /* XXX: UNDOCUMENTED: twilight zone special case */

    if ( CUR.GS.gep0 == 0 || CUR.GS.gep1 == 0 )
    {
      FT_Vector*  vec1 = &CUR.zp1.org[point];
      FT_Vector*  vec2 = &CUR.zp0.org[CUR.GS.rp0];


      org_dist = CUR_Func_dualproj( vec1, vec2 );
    }
    else
    {
      FT_Vector*  vec1 = &CUR.zp1.orus[point];
      FT_Vector*  vec2 = &CUR.zp0.orus[CUR.GS.rp0];


      if ( CUR.metrics.x_scale == CUR.metrics.y_scale )
      {
        /* this should be faster */
        org_dist = CUR_Func_dualproj( vec1, vec2 );
        org_dist = FT_MulFix( org_dist, CUR.metrics.x_scale );
      }
      else
      {
        FT_Vector  vec;


        vec.x = FT_MulFix( vec1->x - vec2->x, CUR.metrics.x_scale );
        vec.y = FT_MulFix( vec1->y - vec2->y, CUR.metrics.y_scale );

        org_dist = CUR_fast_dualproj( &vec );
      }
    }

    /* single width cut-in test */

    if ( FT_ABS( org_dist - CUR.GS.single_width_value ) <
         CUR.GS.single_width_cutin )
    {
      if ( org_dist >= 0 )
        org_dist = CUR.GS.single_width_value;
      else
        org_dist = -CUR.GS.single_width_value;
    }

    /* round flag */

    if ( ( CUR.opcode & 4 ) != 0 )
    {
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      if ( SUBPIXEL_HINTING         &&
           CUR.ignore_x_mode        &&
           CUR.GS.freeVector.x != 0 )
        distance = ROUND_None(
                     org_dist,
                     CUR.tt_metrics.compensations[CUR.opcode & 3] );
      else
#endif
      distance = CUR_Func_round(
                   org_dist,
                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
    }
    else
      distance = ROUND_None(
                   org_dist,
                   CUR.tt_metrics.compensations[CUR.opcode & 3] );

    /* minimum distance flag */

    if ( ( CUR.opcode & 8 ) != 0 )
    {
      if ( org_dist >= 0 )
      {
        if ( distance < minimum_distance )
          distance = minimum_distance;
      }
      else
      {
        if ( distance > -minimum_distance )
          distance = -minimum_distance;
      }
    }

    /* now move the point */

    org_dist = CUR_Func_project( CUR.zp1.cur + point,
                                 CUR.zp0.cur + CUR.GS.rp0 );

    CUR_Func_move( &CUR.zp1, point, distance - org_dist );

  Fail:
    CUR.GS.rp1 = CUR.GS.rp0;
    CUR.GS.rp2 = point;

    if ( ( CUR.opcode & 16 ) != 0 )
      CUR.GS.rp0 = point;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MIRP[abcde]:  Move Indirect Relative Point                            */
  /* Opcode range: 0xE0-0xFF                                               */
  /* Stack:        int32? uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_MIRP( INS_ARG )
  {
    FT_UShort   point;
    FT_ULong    cvtEntry;

    FT_F26Dot6  cvt_dist,
                distance,
                cur_dist,
                org_dist,
                control_value_cutin,
                minimum_distance;
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    FT_Int      B1           = 0; /* pacify compiler */
    FT_Int      B2           = 0;
    FT_Bool     reverse_move = FALSE;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */


    minimum_distance    = CUR.GS.minimum_distance;
    control_value_cutin = CUR.GS.control_value_cutin;
    point               = (FT_UShort)args[0];
    cvtEntry            = (FT_ULong)( args[1] + 1 );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                  &&
         CUR.ignore_x_mode                                 &&
         CUR.GS.freeVector.x != 0                          &&
         !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
      control_value_cutin = minimum_distance = 0;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    /* XXX: UNDOCUMENTED! cvt[-1] = 0 always */

    if ( BOUNDS( point,      CUR.zp1.n_points ) ||
         BOUNDSL( cvtEntry,  CUR.cvtSize + 1 )  ||
         BOUNDS( CUR.GS.rp0, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    if ( !cvtEntry )
      cvt_dist = 0;
    else
      cvt_dist = CUR_Func_read_cvt( cvtEntry - 1 );

    /* single width test */

    if ( FT_ABS( cvt_dist - CUR.GS.single_width_value ) <
         CUR.GS.single_width_cutin )
    {
      if ( cvt_dist >= 0 )
        cvt_dist =  CUR.GS.single_width_value;
      else
        cvt_dist = -CUR.GS.single_width_value;
    }

    /* UNDOCUMENTED!  The MS rasterizer does that with */
    /* twilight points (confirmed by Greg Hitchcock)   */
    if ( CUR.GS.gep1 == 0 )
    {
      CUR.zp1.org[point].x = CUR.zp0.org[CUR.GS.rp0].x +
                             TT_MulFix14( (FT_UInt32)cvt_dist,
                                          CUR.GS.freeVector.x );
      CUR.zp1.org[point].y = CUR.zp0.org[CUR.GS.rp0].y +
                             TT_MulFix14( (FT_UInt32)cvt_dist,
                                          CUR.GS.freeVector.y );
      CUR.zp1.cur[point]   = CUR.zp1.org[point];
    }

    org_dist = CUR_Func_dualproj( &CUR.zp1.org[point],
                                  &CUR.zp0.org[CUR.GS.rp0] );
    cur_dist = CUR_Func_project ( &CUR.zp1.cur[point],
                                  &CUR.zp0.cur[CUR.GS.rp0] );

    /* auto-flip test */

    if ( CUR.GS.auto_flip )
    {
      if ( ( org_dist ^ cvt_dist ) < 0 )
        cvt_dist = -cvt_dist;
    }

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                         &&
         CUR.ignore_x_mode                                        &&
         CUR.GS.freeVector.y != 0                                 &&
         ( CUR.sph_tweak_flags & SPH_TWEAK_TIMES_NEW_ROMAN_HACK ) )
    {
      if ( cur_dist < -64 )
        cvt_dist -= 16;
      else if ( cur_dist > 64 && cur_dist < 84 )
        cvt_dist += 32;
    }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    /* control value cut-in and round */

    if ( ( CUR.opcode & 4 ) != 0 )
    {
      /* XXX: UNDOCUMENTED!  Only perform cut-in test when both points */
      /*      refer to the same zone.                                  */

      if ( CUR.GS.gep0 == CUR.GS.gep1 )
      {
        /* XXX: According to Greg Hitchcock, the following wording is */
        /*      the right one:                                        */
        /*                                                            */
        /*        When the absolute difference between the value in   */
        /*        the table [CVT] and the measurement directly from   */
        /*        the outline is _greater_ than the cut_in value, the */
        /*        outline measurement is used.                        */
        /*                                                            */
        /*      This is from `instgly.doc'.  The description in       */
        /*      `ttinst2.doc', version 1.66, is thus incorrect since  */
        /*      it implies `>=' instead of `>'.                       */

        if ( FT_ABS( cvt_dist - org_dist ) > control_value_cutin )
          cvt_dist = org_dist;
      }

      distance = CUR_Func_round(
                   cvt_dist,
                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
    }
    else
    {

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
      /* do cvt cut-in always in MIRP for sph */
      if ( SUBPIXEL_HINTING           &&
           CUR.ignore_x_mode          &&
           CUR.GS.gep0 == CUR.GS.gep1 )
      {
        if ( FT_ABS( cvt_dist - org_dist ) > control_value_cutin )
          cvt_dist = org_dist;
      }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

      distance = ROUND_None(
                   cvt_dist,
                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
    }

    /* minimum distance test */

    if ( ( CUR.opcode & 8 ) != 0 )
    {
      if ( org_dist >= 0 )
      {
        if ( distance < minimum_distance )
          distance = minimum_distance;
      }
      else
      {
        if ( distance > -minimum_distance )
          distance = -minimum_distance;
      }
    }

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING )
    {
      B1 = CUR.zp1.cur[point].y;

      /* Round moves if necessary */
      if ( CUR.ignore_x_mode                                          &&
           CUR.GS.freeVector.y != 0                                   &&
           ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_Y_MOVES ) )
        distance = FT_PIX_ROUND( B1 + distance - cur_dist ) - B1 + cur_dist;

      if ( CUR.ignore_x_mode                                      &&
           CUR.GS.freeVector.y != 0                               &&
           ( CUR.opcode & 16 ) == 0                               &&
           ( CUR.opcode & 8 ) == 0                                &&
           ( CUR.sph_tweak_flags & SPH_TWEAK_COURIER_NEW_2_HACK ) )
        distance += 64;
    }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    CUR_Func_move( &CUR.zp1, point, distance - cur_dist );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING )
    {
      B2 = CUR.zp1.cur[point].y;

      /* Reverse move if necessary */
      if ( CUR.ignore_x_mode )
      {
        if ( CUR.face->sph_compatibility_mode                          &&
             CUR.GS.freeVector.y != 0                                  &&
             ( B1 & 63 ) == 0                                          &&
             ( B2 & 63 ) != 0                                          )
          reverse_move = TRUE;

        if ( ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_NONPIXEL_Y_MOVES ) &&
             CUR.GS.freeVector.y != 0                                  &&
             ( B2 & 63 ) != 0                                          &&
             ( B1 & 63 ) != 0                                          )
          reverse_move = TRUE;
      }

      if ( reverse_move )
        CUR_Func_move( &CUR.zp1, point, -( distance - cur_dist ) );
    }

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

  Fail:
    CUR.GS.rp1 = CUR.GS.rp0;

    if ( ( CUR.opcode & 16 ) != 0 )
      CUR.GS.rp0 = point;

    CUR.GS.rp2 = point;
  }


  /*************************************************************************/
  /*                                                                       */
  /* ALIGNRP[]:    ALIGN Relative Point                                    */
  /* Opcode range: 0x3C                                                    */
  /* Stack:        uint32 uint32... -->                                    */
  /*                                                                       */
  static void
  Ins_ALIGNRP( INS_ARG )
  {
    FT_UShort   point;
    FT_F26Dot6  distance;

    FT_UNUSED_ARG;


#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING                                         &&
         CUR.ignore_x_mode                                        &&
         CUR.iup_called                                           &&
         ( CUR.sph_tweak_flags & SPH_TWEAK_NO_ALIGNRP_AFTER_IUP ) )
    {
      CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    if ( CUR.top < CUR.GS.loop ||
         BOUNDS( CUR.GS.rp0, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;

      point = (FT_UShort)CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.zp1.n_points ) )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Invalid_Reference );
          return;
        }
      }
      else
      {
        distance = CUR_Func_project( CUR.zp1.cur + point,
                                     CUR.zp0.cur + CUR.GS.rp0 );

        CUR_Func_move( &CUR.zp1, point, -distance );
      }

      CUR.GS.loop--;
    }

  Fail:
    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* ISECT[]:      moves point to InterSECTion                             */
  /* Opcode range: 0x0F                                                    */
  /* Stack:        5 * uint32 -->                                          */
  /*                                                                       */
  static void
  Ins_ISECT( INS_ARG )
  {
    FT_UShort   point,
                a0, a1,
                b0, b1;

    FT_F26Dot6  discriminant, dotproduct;

    FT_F26Dot6  dx,  dy,
                dax, day,
                dbx, dby;

    FT_F26Dot6  val;

    FT_Vector   R;


    point = (FT_UShort)args[0];

    a0 = (FT_UShort)args[1];
    a1 = (FT_UShort)args[2];
    b0 = (FT_UShort)args[3];
    b1 = (FT_UShort)args[4];

    if ( BOUNDS( b0, CUR.zp0.n_points )  ||
         BOUNDS( b1, CUR.zp0.n_points )  ||
         BOUNDS( a0, CUR.zp1.n_points )  ||
         BOUNDS( a1, CUR.zp1.n_points )  ||
         BOUNDS( point, CUR.zp2.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    /* Cramer's rule */

    dbx = CUR.zp0.cur[b1].x - CUR.zp0.cur[b0].x;
    dby = CUR.zp0.cur[b1].y - CUR.zp0.cur[b0].y;

    dax = CUR.zp1.cur[a1].x - CUR.zp1.cur[a0].x;
    day = CUR.zp1.cur[a1].y - CUR.zp1.cur[a0].y;

    dx = CUR.zp0.cur[b0].x - CUR.zp1.cur[a0].x;
    dy = CUR.zp0.cur[b0].y - CUR.zp1.cur[a0].y;

    CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_BOTH;

    discriminant = FT_MulDiv( dax, -dby, 0x40 ) +
                   FT_MulDiv( day, dbx, 0x40 );
    dotproduct   = FT_MulDiv( dax, dbx, 0x40 ) +
                   FT_MulDiv( day, dby, 0x40 );

    /* The discriminant above is actually a cross product of vectors     */
    /* da and db. Together with the dot product, they can be used as     */
    /* surrogates for sine and cosine of the angle between the vectors.  */
    /* Indeed,                                                           */
    /*       dotproduct   = |da||db|cos(angle)                           */
    /*       discriminant = |da||db|sin(angle)     .                     */
    /* We use these equations to reject grazing intersections by         */
    /* thresholding abs(tan(angle)) at 1/19, corresponding to 3 degrees. */
    if ( 19 * FT_ABS( discriminant ) > FT_ABS( dotproduct ) )
    {
      val = FT_MulDiv( dx, -dby, 0x40 ) + FT_MulDiv( dy, dbx, 0x40 );

      R.x = FT_MulDiv( val, dax, discriminant );
      R.y = FT_MulDiv( val, day, discriminant );

      CUR.zp2.cur[point].x = CUR.zp1.cur[a0].x + R.x;
      CUR.zp2.cur[point].y = CUR.zp1.cur[a0].y + R.y;
    }
    else
    {
      /* else, take the middle of the middles of A and B */

      CUR.zp2.cur[point].x = ( CUR.zp1.cur[a0].x +
                               CUR.zp1.cur[a1].x +
                               CUR.zp0.cur[b0].x +
                               CUR.zp0.cur[b1].x ) / 4;
      CUR.zp2.cur[point].y = ( CUR.zp1.cur[a0].y +
                               CUR.zp1.cur[a1].y +
                               CUR.zp0.cur[b0].y +
                               CUR.zp0.cur[b1].y ) / 4;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* ALIGNPTS[]:   ALIGN PoinTS                                            */
  /* Opcode range: 0x27                                                    */
  /* Stack:        uint32 uint32 -->                                       */
  /*                                                                       */
  static void
  Ins_ALIGNPTS( INS_ARG )
  {
    FT_UShort   p1, p2;
    FT_F26Dot6  distance;


    p1 = (FT_UShort)args[0];
    p2 = (FT_UShort)args[1];

    if ( BOUNDS( p1, CUR.zp1.n_points ) ||
         BOUNDS( p2, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    distance = CUR_Func_project( CUR.zp0.cur + p2,
                                 CUR.zp1.cur + p1 ) / 2;

    CUR_Func_move( &CUR.zp1, p1, distance );
    CUR_Func_move( &CUR.zp0, p2, -distance );
  }


  /*************************************************************************/
  /*                                                                       */
  /* IP[]:         Interpolate Point                                       */
  /* Opcode range: 0x39                                                    */
  /* Stack:        uint32... -->                                           */
  /*                                                                       */

  /* SOMETIMES, DUMBER CODE IS BETTER CODE */

  static void
  Ins_IP( INS_ARG )
  {
    FT_F26Dot6  old_range, cur_range;
    FT_Vector*  orus_base;
    FT_Vector*  cur_base;
    FT_Int      twilight;

    FT_UNUSED_ARG;


    if ( CUR.top < CUR.GS.loop )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    /*
     * We need to deal in a special way with the twilight zone.
     * Otherwise, by definition, the value of CUR.twilight.orus[n] is (0,0),
     * for every n.
     */
    twilight = CUR.GS.gep0 == 0 || CUR.GS.gep1 == 0 || CUR.GS.gep2 == 0;

    if ( BOUNDS( CUR.GS.rp1, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      goto Fail;
    }

    if ( twilight )
      orus_base = &CUR.zp0.org[CUR.GS.rp1];
    else
      orus_base = &CUR.zp0.orus[CUR.GS.rp1];

    cur_base = &CUR.zp0.cur[CUR.GS.rp1];

    /* XXX: There are some glyphs in some braindead but popular */
    /*      fonts out there (e.g. [aeu]grave in monotype.ttf)   */
    /*      calling IP[] with bad values of rp[12].             */
    /*      Do something sane when this odd thing happens.      */
    if ( BOUNDS( CUR.GS.rp1, CUR.zp0.n_points ) ||
         BOUNDS( CUR.GS.rp2, CUR.zp1.n_points ) )
    {
      old_range = 0;
      cur_range = 0;
    }
    else
    {
      if ( twilight )
        old_range = CUR_Func_dualproj( &CUR.zp1.org[CUR.GS.rp2],
                                       orus_base );
      else if ( CUR.metrics.x_scale == CUR.metrics.y_scale )
        old_range = CUR_Func_dualproj( &CUR.zp1.orus[CUR.GS.rp2],
                                       orus_base );
      else
      {
        FT_Vector  vec;


        vec.x = FT_MulFix( CUR.zp1.orus[CUR.GS.rp2].x - orus_base->x,
                           CUR.metrics.x_scale );
        vec.y = FT_MulFix( CUR.zp1.orus[CUR.GS.rp2].y - orus_base->y,
                           CUR.metrics.y_scale );

        old_range = CUR_fast_dualproj( &vec );
      }

      cur_range = CUR_Func_project ( &CUR.zp1.cur[CUR.GS.rp2], cur_base );
    }

    for ( ; CUR.GS.loop > 0; --CUR.GS.loop )
    {
      FT_UInt     point = (FT_UInt)CUR.stack[--CUR.args];
      FT_F26Dot6  org_dist, cur_dist, new_dist;


      /* check point bounds */
      if ( BOUNDS( point, CUR.zp2.n_points ) )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Invalid_Reference );
          return;
        }
        continue;
      }

      if ( twilight )
        org_dist = CUR_Func_dualproj( &CUR.zp2.org[point], orus_base );
      else if ( CUR.metrics.x_scale == CUR.metrics.y_scale )
        org_dist = CUR_Func_dualproj( &CUR.zp2.orus[point], orus_base );
      else
      {
        FT_Vector  vec;


        vec.x = FT_MulFix( CUR.zp2.orus[point].x - orus_base->x,
                           CUR.metrics.x_scale );
        vec.y = FT_MulFix( CUR.zp2.orus[point].y - orus_base->y,
                           CUR.metrics.y_scale );

        org_dist = CUR_fast_dualproj( &vec );
      }

      cur_dist = CUR_Func_project( &CUR.zp2.cur[point], cur_base );

      if ( org_dist )
      {
        if ( old_range )
          new_dist = FT_MulDiv( org_dist, cur_range, old_range );
        else
        {
          /* This is the same as what MS does for the invalid case:  */
          /*                                                         */
          /*   delta = (Original_Pt - Original_RP1) -                */
          /*           (Current_Pt - Current_RP1)         ;          */
          /*                                                         */
          /* In FreeType speak:                                      */
          /*                                                         */
          /*   delta = org_dist - cur_dist          .                */
          /*                                                         */
          /* We move `point' by `new_dist - cur_dist' after leaving  */
          /* this block, thus we have                                */
          /*                                                         */
          /*   new_dist - cur_dist = delta                   ,       */
          /*   new_dist - cur_dist = org_dist - cur_dist     ,       */
          /*              new_dist = org_dist                .       */

          new_dist = org_dist;
        }
      }
      else
        new_dist = 0;

      CUR_Func_move( &CUR.zp2, (FT_UShort)point, new_dist - cur_dist );
    }

  Fail:
    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* UTP[a]:       UnTouch Point                                           */
  /* Opcode range: 0x29                                                    */
  /* Stack:        uint32 -->                                              */
  /*                                                                       */
  static void
  Ins_UTP( INS_ARG )
  {
    FT_UShort  point;
    FT_Byte    mask;


    point = (FT_UShort)args[0];

    if ( BOUNDS( point, CUR.zp0.n_points ) )
    {
      if ( CUR.pedantic_hinting )
        CUR.error = FT_THROW( Invalid_Reference );
      return;
    }

    mask = 0xFF;

    if ( CUR.GS.freeVector.x != 0 )
      mask &= ~FT_CURVE_TAG_TOUCH_X;

    if ( CUR.GS.freeVector.y != 0 )
      mask &= ~FT_CURVE_TAG_TOUCH_Y;

    CUR.zp0.tags[point] &= mask;
  }


  /* Local variables for Ins_IUP: */
  typedef struct  IUP_WorkerRec_
  {
    FT_Vector*  orgs;   /* original and current coordinate */
    FT_Vector*  curs;   /* arrays                          */
    FT_Vector*  orus;
    FT_UInt     max_points;

  } IUP_WorkerRec, *IUP_Worker;


  static void
  _iup_worker_shift( IUP_Worker  worker,
                     FT_UInt     p1,
                     FT_UInt     p2,
                     FT_UInt     p )
  {
    FT_UInt     i;
    FT_F26Dot6  dx;


    dx = worker->curs[p].x - worker->orgs[p].x;
    if ( dx != 0 )
    {
      for ( i = p1; i < p; i++ )
        worker->curs[i].x += dx;

      for ( i = p + 1; i <= p2; i++ )
        worker->curs[i].x += dx;
    }
  }


  static void
  _iup_worker_interpolate( IUP_Worker  worker,
                           FT_UInt     p1,
                           FT_UInt     p2,
                           FT_UInt     ref1,
                           FT_UInt     ref2 )
  {
    FT_UInt     i;
    FT_F26Dot6  orus1, orus2, org1, org2, delta1, delta2;


    if ( p1 > p2 )
      return;

    if ( BOUNDS( ref1, worker->max_points ) ||
         BOUNDS( ref2, worker->max_points ) )
      return;

    orus1 = worker->orus[ref1].x;
    orus2 = worker->orus[ref2].x;

    if ( orus1 > orus2 )
    {
      FT_F26Dot6  tmp_o;
      FT_UInt     tmp_r;


      tmp_o = orus1;
      orus1 = orus2;
      orus2 = tmp_o;

      tmp_r = ref1;
      ref1  = ref2;
      ref2  = tmp_r;
    }

    org1   = worker->orgs[ref1].x;
    org2   = worker->orgs[ref2].x;
    delta1 = worker->curs[ref1].x - org1;
    delta2 = worker->curs[ref2].x - org2;

    if ( orus1 == orus2 )
    {
      /* simple shift of untouched points */
      for ( i = p1; i <= p2; i++ )
      {
        FT_F26Dot6  x = worker->orgs[i].x;


        if ( x <= org1 )
          x += delta1;
        else
          x += delta2;

        worker->curs[i].x = x;
      }
    }
    else
    {
      FT_Fixed  scale       = 0;
      FT_Bool   scale_valid = 0;


      /* interpolation */
      for ( i = p1; i <= p2; i++ )
      {
        FT_F26Dot6  x = worker->orgs[i].x;


        if ( x <= org1 )
          x += delta1;

        else if ( x >= org2 )
          x += delta2;

        else
        {
          if ( !scale_valid )
          {
            scale_valid = 1;
            scale       = FT_DivFix( org2 + delta2 - ( org1 + delta1 ),
                                     orus2 - orus1 );
          }

          x = ( org1 + delta1 ) +
              FT_MulFix( worker->orus[i].x - orus1, scale );
        }
        worker->curs[i].x = x;
      }
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* IUP[a]:       Interpolate Untouched Points                            */
  /* Opcode range: 0x30-0x31                                               */
  /* Stack:        -->                                                     */
  /*                                                                       */
  static void
  Ins_IUP( INS_ARG )
  {
    IUP_WorkerRec  V;
    FT_Byte        mask;

    FT_UInt   first_point;   /* first point of contour        */
    FT_UInt   end_point;     /* end point (last+1) of contour */

    FT_UInt   first_touched; /* first touched point in contour   */
    FT_UInt   cur_touched;   /* current touched point in contour */

    FT_UInt   point;         /* current point   */
    FT_Short  contour;       /* current contour */

    FT_UNUSED_ARG;


    /* ignore empty outlines */
    if ( CUR.pts.n_contours == 0 )
      return;

    if ( CUR.opcode & 1 )
    {
      mask   = FT_CURVE_TAG_TOUCH_X;
      V.orgs = CUR.pts.org;
      V.curs = CUR.pts.cur;
      V.orus = CUR.pts.orus;
    }
    else
    {
      mask   = FT_CURVE_TAG_TOUCH_Y;
      V.orgs = (FT_Vector*)( (FT_Pos*)CUR.pts.org + 1 );
      V.curs = (FT_Vector*)( (FT_Pos*)CUR.pts.cur + 1 );
      V.orus = (FT_Vector*)( (FT_Pos*)CUR.pts.orus + 1 );
    }
    V.max_points = CUR.pts.n_points;

    contour = 0;
    point   = 0;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    if ( SUBPIXEL_HINTING  &&
         CUR.ignore_x_mode )
    {
      CUR.iup_called = TRUE;
      if ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_IUP )
        return;
    }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    do
    {
      end_point   = CUR.pts.contours[contour] - CUR.pts.first_point;
      first_point = point;

      if ( BOUNDS ( end_point, CUR.pts.n_points ) )
        end_point = CUR.pts.n_points - 1;

      while ( point <= end_point && ( CUR.pts.tags[point] & mask ) == 0 )
        point++;

      if ( point <= end_point )
      {
        first_touched = point;
        cur_touched   = point;

        point++;

        while ( point <= end_point )
        {
          if ( ( CUR.pts.tags[point] & mask ) != 0 )
          {
            _iup_worker_interpolate( &V,
                                     cur_touched + 1,
                                     point - 1,
                                     cur_touched,
                                     point );
            cur_touched = point;
          }

          point++;
        }

        if ( cur_touched == first_touched )
          _iup_worker_shift( &V, first_point, end_point, cur_touched );
        else
        {
          _iup_worker_interpolate( &V,
                                   (FT_UShort)( cur_touched + 1 ),
                                   end_point,
                                   cur_touched,
                                   first_touched );

          if ( first_touched > 0 )
            _iup_worker_interpolate( &V,
                                     first_point,
                                     first_touched - 1,
                                     cur_touched,
                                     first_touched );
        }
      }
      contour++;
    } while ( contour < CUR.pts.n_contours );
  }


  /*************************************************************************/
  /*                                                                       */
  /* DELTAPn[]:    DELTA exceptions P1, P2, P3                             */
  /* Opcode range: 0x5D,0x71,0x72                                          */
  /* Stack:        uint32 (2 * uint32)... -->                              */
  /*                                                                       */
  static void
  Ins_DELTAP( INS_ARG )
  {
    FT_ULong   k, nump;
    FT_UShort  A;
    FT_ULong   C;
    FT_Long    B;
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    FT_UShort  B1, B2;


    if ( SUBPIXEL_HINTING                                        &&
         CUR.ignore_x_mode                                       &&
         CUR.iup_called                                          &&
         ( CUR.sph_tweak_flags & SPH_TWEAK_NO_DELTAP_AFTER_IUP ) )
      goto Fail;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */


#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    /* Delta hinting is covered by US Patent 5159668. */
    if ( CUR.face->unpatented_hinting )
    {
      FT_Long  n = args[0] * 2;


      if ( CUR.args < n )
      {
        if ( CUR.pedantic_hinting )
          CUR.error = FT_THROW( Too_Few_Arguments );
        n = CUR.args;
      }

      CUR.args -= n;
      CUR.new_top = CUR.args;
      return;
    }
#endif

    nump = (FT_ULong)args[0];   /* some points theoretically may occur more
                                   than once, thus UShort isn't enough */

    for ( k = 1; k <= nump; k++ )
    {
      if ( CUR.args < 2 )
      {
        if ( CUR.pedantic_hinting )
          CUR.error = FT_THROW( Too_Few_Arguments );
        CUR.args = 0;
        goto Fail;
      }

      CUR.args -= 2;

      A = (FT_UShort)CUR.stack[CUR.args + 1];
      B = CUR.stack[CUR.args];

      /* XXX: Because some popular fonts contain some invalid DeltaP */
      /*      instructions, we simply ignore them when the stacked   */
      /*      point reference is off limit, rather than returning an */
      /*      error.  As a delta instruction doesn't change a glyph  */
      /*      in great ways, this shouldn't be a problem.            */

      if ( !BOUNDS( A, CUR.zp0.n_points ) )
      {
        C = ( (FT_ULong)B & 0xF0 ) >> 4;

        switch ( CUR.opcode )
        {
        case 0x5D:
          break;

        case 0x71:
          C += 16;
          break;

        case 0x72:
          C += 32;
          break;
        }

        C += CUR.GS.delta_base;

        if ( CURRENT_Ppem() == (FT_Long)C )
        {
          B = ( (FT_ULong)B & 0xF ) - 8;
          if ( B >= 0 )
            B++;
          B = B * 64 / ( 1L << CUR.GS.delta_shift );

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

          if ( SUBPIXEL_HINTING )
          {
            /*
             *  Allow delta move if
             *
             *  - not using ignore_x_mode rendering
             *  - glyph is specifically set to allow it
             *  - glyph is composite and freedom vector is not subpixel
             *    vector
             */
            if ( !CUR.ignore_x_mode                                   ||
                 ( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_DO_DELTAP ) ||
                 ( CUR.is_composite && CUR.GS.freeVector.y != 0 )     )
              CUR_Func_move( &CUR.zp0, A, B );

            /* Otherwise apply subpixel hinting and */
            /* compatibility mode rules             */
            else if ( CUR.ignore_x_mode )
            {
              if ( CUR.GS.freeVector.y != 0 )
                B1 = CUR.zp0.cur[A].y;
              else
                B1 = CUR.zp0.cur[A].x;

#if 0
              /* Standard Subpixel Hinting: Allow y move.       */
              /* This messes up dejavu and may not be needed... */
              if ( !CUR.face->sph_compatibility_mode &&
                   CUR.GS.freeVector.y != 0          )
                CUR_Func_move( &CUR.zp0, A, B );
              else
#endif /* 0 */

              /* Compatibility Mode: Allow x or y move if point touched in */
              /* Y direction.                                              */
              if ( CUR.face->sph_compatibility_mode                      &&
                   !( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_SKIP_DELTAP ) )
              {
                /* save the y value of the point now; compare after move */
                B1 = CUR.zp0.cur[A].y;

                if ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_Y_MOVES )
                  B = FT_PIX_ROUND( B1 + B ) - B1;

                /* Allow delta move if using sph_compatibility_mode,   */
                /* IUP has not been called, and point is touched on Y. */
                if ( !CUR.iup_called                            &&
                     ( CUR.zp0.tags[A] & FT_CURVE_TAG_TOUCH_Y ) )
                  CUR_Func_move( &CUR.zp0, A, B );
              }

              B2 = CUR.zp0.cur[A].y;

              /* Reverse this move if it results in a disallowed move */
              if ( CUR.GS.freeVector.y != 0                           &&
                   ( ( CUR.face->sph_compatibility_mode           &&
                       ( B1 & 63 ) == 0                           &&
                       ( B2 & 63 ) != 0                           ) ||
                     ( ( CUR.sph_tweak_flags                    &
                         SPH_TWEAK_SKIP_NONPIXEL_Y_MOVES_DELTAP ) &&
                       ( B1 & 63 ) != 0                           &&
                       ( B2 & 63 ) != 0                           ) ) )
                CUR_Func_move( &CUR.zp0, A, -B );
            }
          }
          else
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

            CUR_Func_move( &CUR.zp0, A, B );
        }
      }
      else
        if ( CUR.pedantic_hinting )
          CUR.error = FT_THROW( Invalid_Reference );
    }

  Fail:
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* DELTACn[]:    DELTA exceptions C1, C2, C3                             */
  /* Opcode range: 0x73,0x74,0x75                                          */
  /* Stack:        uint32 (2 * uint32)... -->                              */
  /*                                                                       */
  static void
  Ins_DELTAC( INS_ARG )
  {
    FT_ULong  nump, k;
    FT_ULong  A, C;
    FT_Long   B;


#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
    /* Delta hinting is covered by US Patent 5159668. */
    if ( CUR.face->unpatented_hinting )
    {
      FT_Long  n = args[0] * 2;


      if ( CUR.args < n )
      {
        if ( CUR.pedantic_hinting )
          CUR.error = FT_THROW( Too_Few_Arguments );
        n = CUR.args;
      }

      CUR.args -= n;
      CUR.new_top = CUR.args;
      return;
    }
#endif

    nump = (FT_ULong)args[0];

    for ( k = 1; k <= nump; k++ )
    {
      if ( CUR.args < 2 )
      {
        if ( CUR.pedantic_hinting )
          CUR.error = FT_THROW( Too_Few_Arguments );
        CUR.args = 0;
        goto Fail;
      }

      CUR.args -= 2;

      A = (FT_ULong)CUR.stack[CUR.args + 1];
      B = CUR.stack[CUR.args];

      if ( BOUNDSL( A, CUR.cvtSize ) )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Invalid_Reference );
          return;
        }
      }
      else
      {
        C = ( (FT_ULong)B & 0xF0 ) >> 4;

        switch ( CUR.opcode )
        {
        case 0x73:
          break;

        case 0x74:
          C += 16;
          break;

        case 0x75:
          C += 32;
          break;
        }

        C += CUR.GS.delta_base;

        if ( CURRENT_Ppem() == (FT_Long)C )
        {
          B = ( (FT_ULong)B & 0xF ) - 8;
          if ( B >= 0 )
            B++;
          B = B * 64 / ( 1L << CUR.GS.delta_shift );

          CUR_Func_move_cvt( A, B );
        }
      }
    }

  Fail:
    CUR.new_top = CUR.args;
  }


  /*************************************************************************/
  /*                                                                       */
  /* MISC. INSTRUCTIONS                                                    */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* GETINFO[]:    GET INFOrmation                                         */
  /* Opcode range: 0x88                                                    */
  /* Stack:        uint32 --> uint32                                       */
  /*                                                                       */
  static void
  Ins_GETINFO( INS_ARG )
  {
    FT_Long  K;


    K = 0;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    /********************************/
    /* RASTERIZER VERSION           */
    /* Selector Bit:  0             */
    /* Return Bit(s): 0-7           */
    /*                              */
    if ( SUBPIXEL_HINTING     &&
         ( args[0] & 1 ) != 0 &&
         CUR.ignore_x_mode    )
    {
      K = CUR.rasterizer_version;
      FT_TRACE7(( "Setting rasterizer version %d\n",
                  CUR.rasterizer_version ));
    }
    else
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
      if ( ( args[0] & 1 ) != 0 )
        K = TT_INTERPRETER_VERSION_35;

    /********************************/
    /* GLYPH ROTATED                */
    /* Selector Bit:  1             */
    /* Return Bit(s): 8             */
    /*                              */
    if ( ( args[0] & 2 ) != 0 && CUR.tt_metrics.rotated )
      K |= 0x80;

    /********************************/
    /* GLYPH STRETCHED              */
    /* Selector Bit:  2             */
    /* Return Bit(s): 9             */
    /*                              */
    if ( ( args[0] & 4 ) != 0 && CUR.tt_metrics.stretched )
      K |= 1 << 8;

    /********************************/
    /* HINTING FOR GRAYSCALE        */
    /* Selector Bit:  5             */
    /* Return Bit(s): 12            */
    /*                              */
    if ( ( args[0] & 32 ) != 0 && CUR.grayscale )
      K |= 1 << 12;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

    if ( SUBPIXEL_HINTING                                    &&
         CUR.ignore_x_mode                                   &&
         CUR.rasterizer_version >= TT_INTERPRETER_VERSION_35 )
    {

      if ( CUR.rasterizer_version >= 37 )
      {
        /********************************/
        /* HINTING FOR SUBPIXEL         */
        /* Selector Bit:  6             */
        /* Return Bit(s): 13            */
        /*                              */
        if ( ( args[0] & 64 ) != 0 && CUR.subpixel )
          K |= 1 << 13;

        /********************************/
        /* COMPATIBLE WIDTHS ENABLED    */
        /* Selector Bit:  7             */
        /* Return Bit(s): 14            */
        /*                              */
        /* Functionality still needs to be added */
        if ( ( args[0] & 128 ) != 0 && CUR.compatible_widths )
          K |= 1 << 14;

        /********************************/
        /* SYMMETRICAL SMOOTHING        */
        /* Selector Bit:  8             */
        /* Return Bit(s): 15            */
        /*                              */
        /* Functionality still needs to be added */
        if ( ( args[0] & 256 ) != 0 && CUR.symmetrical_smoothing )
          K |= 1 << 15;

        /********************************/
        /* HINTING FOR BGR?             */
        /* Selector Bit:  9             */
        /* Return Bit(s): 16            */
        /*                              */
        /* Functionality still needs to be added */
        if ( ( args[0] & 512 ) != 0 && CUR.bgr )
          K |= 1 << 16;

        if ( CUR.rasterizer_version >= 38 )
        {
          /********************************/
          /* SUBPIXEL POSITIONED?         */
          /* Selector Bit:  10            */
          /* Return Bit(s): 17            */
          /*                              */
          /* Functionality still needs to be added */
          if ( ( args[0] & 1024 ) != 0 && CUR.subpixel_positioned )
            K |= 1 << 17;
        }
      }
    }

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    args[0] = K;
  }


  static void
  Ins_UNKNOWN( INS_ARG )
  {
    TT_DefRecord*  def   = CUR.IDefs;
    TT_DefRecord*  limit = def + CUR.numIDefs;

    FT_UNUSED_ARG;


    for ( ; def < limit; def++ )
    {
      if ( (FT_Byte)def->opc == CUR.opcode && def->active )
      {
        TT_CallRec*  call;


        if ( CUR.callTop >= CUR.callSize )
        {
          CUR.error = FT_THROW( Stack_Overflow );
          return;
        }

        call = CUR.callStack + CUR.callTop++;

        call->Caller_Range = CUR.curRange;
        call->Caller_IP    = CUR.IP + 1;
        call->Cur_Count    = 1;
        call->Def          = def;

        INS_Goto_CodeRange( def->range, def->start );

        CUR.step_ins = FALSE;
        return;
      }
    }

    CUR.error = FT_THROW( Invalid_Opcode );
  }


#ifndef TT_CONFIG_OPTION_INTERPRETER_SWITCH


  static
  TInstruction_Function  Instruct_Dispatch[256] =
  {
    /* Opcodes are gathered in groups of 16. */
    /* Please keep the spaces as they are.   */

    /*  SVTCA  y  */  Ins_SVTCA,
    /*  SVTCA  x  */  Ins_SVTCA,
    /*  SPvTCA y  */  Ins_SPVTCA,
    /*  SPvTCA x  */  Ins_SPVTCA,
    /*  SFvTCA y  */  Ins_SFVTCA,
    /*  SFvTCA x  */  Ins_SFVTCA,
    /*  SPvTL //  */  Ins_SPVTL,
    /*  SPvTL +   */  Ins_SPVTL,
    /*  SFvTL //  */  Ins_SFVTL,
    /*  SFvTL +   */  Ins_SFVTL,
    /*  SPvFS     */  Ins_SPVFS,
    /*  SFvFS     */  Ins_SFVFS,
    /*  GPV       */  Ins_GPV,
    /*  GFV       */  Ins_GFV,
    /*  SFvTPv    */  Ins_SFVTPV,
    /*  ISECT     */  Ins_ISECT,

    /*  SRP0      */  Ins_SRP0,
    /*  SRP1      */  Ins_SRP1,
    /*  SRP2      */  Ins_SRP2,
    /*  SZP0      */  Ins_SZP0,
    /*  SZP1      */  Ins_SZP1,
    /*  SZP2      */  Ins_SZP2,
    /*  SZPS      */  Ins_SZPS,
    /*  SLOOP     */  Ins_SLOOP,
    /*  RTG       */  Ins_RTG,
    /*  RTHG      */  Ins_RTHG,
    /*  SMD       */  Ins_SMD,
    /*  ELSE      */  Ins_ELSE,
    /*  JMPR      */  Ins_JMPR,
    /*  SCvTCi    */  Ins_SCVTCI,
    /*  SSwCi     */  Ins_SSWCI,
    /*  SSW       */  Ins_SSW,

    /*  DUP       */  Ins_DUP,
    /*  POP       */  Ins_POP,
    /*  CLEAR     */  Ins_CLEAR,
    /*  SWAP      */  Ins_SWAP,
    /*  DEPTH     */  Ins_DEPTH,
    /*  CINDEX    */  Ins_CINDEX,
    /*  MINDEX    */  Ins_MINDEX,
    /*  AlignPTS  */  Ins_ALIGNPTS,
    /*  INS_0x28  */  Ins_UNKNOWN,
    /*  UTP       */  Ins_UTP,
    /*  LOOPCALL  */  Ins_LOOPCALL,
    /*  CALL      */  Ins_CALL,
    /*  FDEF      */  Ins_FDEF,
    /*  ENDF      */  Ins_ENDF,
    /*  MDAP[0]   */  Ins_MDAP,
    /*  MDAP[1]   */  Ins_MDAP,

    /*  IUP[0]    */  Ins_IUP,
    /*  IUP[1]    */  Ins_IUP,
    /*  SHP[0]    */  Ins_SHP,
    /*  SHP[1]    */  Ins_SHP,
    /*  SHC[0]    */  Ins_SHC,
    /*  SHC[1]    */  Ins_SHC,
    /*  SHZ[0]    */  Ins_SHZ,
    /*  SHZ[1]    */  Ins_SHZ,
    /*  SHPIX     */  Ins_SHPIX,
    /*  IP        */  Ins_IP,
    /*  MSIRP[0]  */  Ins_MSIRP,
    /*  MSIRP[1]  */  Ins_MSIRP,
    /*  AlignRP   */  Ins_ALIGNRP,
    /*  RTDG      */  Ins_RTDG,
    /*  MIAP[0]   */  Ins_MIAP,
    /*  MIAP[1]   */  Ins_MIAP,

    /*  NPushB    */  Ins_NPUSHB,
    /*  NPushW    */  Ins_NPUSHW,
    /*  WS        */  Ins_WS,
    /*  RS        */  Ins_RS,
    /*  WCvtP     */  Ins_WCVTP,
    /*  RCvt      */  Ins_RCVT,
    /*  GC[0]     */  Ins_GC,
    /*  GC[1]     */  Ins_GC,
    /*  SCFS      */  Ins_SCFS,
    /*  MD[0]     */  Ins_MD,
    /*  MD[1]     */  Ins_MD,
    /*  MPPEM     */  Ins_MPPEM,
    /*  MPS       */  Ins_MPS,
    /*  FlipON    */  Ins_FLIPON,
    /*  FlipOFF   */  Ins_FLIPOFF,
    /*  DEBUG     */  Ins_DEBUG,

    /*  LT        */  Ins_LT,
    /*  LTEQ      */  Ins_LTEQ,
    /*  GT        */  Ins_GT,
    /*  GTEQ      */  Ins_GTEQ,
    /*  EQ        */  Ins_EQ,
    /*  NEQ       */  Ins_NEQ,
    /*  ODD       */  Ins_ODD,
    /*  EVEN      */  Ins_EVEN,
    /*  IF        */  Ins_IF,
    /*  EIF       */  Ins_EIF,
    /*  AND       */  Ins_AND,
    /*  OR        */  Ins_OR,
    /*  NOT       */  Ins_NOT,
    /*  DeltaP1   */  Ins_DELTAP,
    /*  SDB       */  Ins_SDB,
    /*  SDS       */  Ins_SDS,

    /*  ADD       */  Ins_ADD,
    /*  SUB       */  Ins_SUB,
    /*  DIV       */  Ins_DIV,
    /*  MUL       */  Ins_MUL,
    /*  ABS       */  Ins_ABS,
    /*  NEG       */  Ins_NEG,
    /*  FLOOR     */  Ins_FLOOR,
    /*  CEILING   */  Ins_CEILING,
    /*  ROUND[0]  */  Ins_ROUND,
    /*  ROUND[1]  */  Ins_ROUND,
    /*  ROUND[2]  */  Ins_ROUND,
    /*  ROUND[3]  */  Ins_ROUND,
    /*  NROUND[0] */  Ins_NROUND,
    /*  NROUND[1] */  Ins_NROUND,
    /*  NROUND[2] */  Ins_NROUND,
    /*  NROUND[3] */  Ins_NROUND,

    /*  WCvtF     */  Ins_WCVTF,
    /*  DeltaP2   */  Ins_DELTAP,
    /*  DeltaP3   */  Ins_DELTAP,
    /*  DeltaCn[0] */ Ins_DELTAC,
    /*  DeltaCn[1] */ Ins_DELTAC,
    /*  DeltaCn[2] */ Ins_DELTAC,
    /*  SROUND    */  Ins_SROUND,
    /*  S45Round  */  Ins_S45ROUND,
    /*  JROT      */  Ins_JROT,
    /*  JROF      */  Ins_JROF,
    /*  ROFF      */  Ins_ROFF,
    /*  INS_0x7B  */  Ins_UNKNOWN,
    /*  RUTG      */  Ins_RUTG,
    /*  RDTG      */  Ins_RDTG,
    /*  SANGW     */  Ins_SANGW,
    /*  AA        */  Ins_AA,

    /*  FlipPT    */  Ins_FLIPPT,
    /*  FlipRgON  */  Ins_FLIPRGON,
    /*  FlipRgOFF */  Ins_FLIPRGOFF,
    /*  INS_0x83  */  Ins_UNKNOWN,
    /*  INS_0x84  */  Ins_UNKNOWN,
    /*  ScanCTRL  */  Ins_SCANCTRL,
    /*  SDPVTL[0] */  Ins_SDPVTL,
    /*  SDPVTL[1] */  Ins_SDPVTL,
    /*  GetINFO   */  Ins_GETINFO,
    /*  IDEF      */  Ins_IDEF,
    /*  ROLL      */  Ins_ROLL,
    /*  MAX       */  Ins_MAX,
    /*  MIN       */  Ins_MIN,
    /*  ScanTYPE  */  Ins_SCANTYPE,
    /*  InstCTRL  */  Ins_INSTCTRL,
    /*  INS_0x8F  */  Ins_UNKNOWN,

    /*  INS_0x90  */   Ins_UNKNOWN,
    /*  INS_0x91  */   Ins_UNKNOWN,
    /*  INS_0x92  */   Ins_UNKNOWN,
    /*  INS_0x93  */   Ins_UNKNOWN,
    /*  INS_0x94  */   Ins_UNKNOWN,
    /*  INS_0x95  */   Ins_UNKNOWN,
    /*  INS_0x96  */   Ins_UNKNOWN,
    /*  INS_0x97  */   Ins_UNKNOWN,
    /*  INS_0x98  */   Ins_UNKNOWN,
    /*  INS_0x99  */   Ins_UNKNOWN,
    /*  INS_0x9A  */   Ins_UNKNOWN,
    /*  INS_0x9B  */   Ins_UNKNOWN,
    /*  INS_0x9C  */   Ins_UNKNOWN,
    /*  INS_0x9D  */   Ins_UNKNOWN,
    /*  INS_0x9E  */   Ins_UNKNOWN,
    /*  INS_0x9F  */   Ins_UNKNOWN,

    /*  INS_0xA0  */   Ins_UNKNOWN,
    /*  INS_0xA1  */   Ins_UNKNOWN,
    /*  INS_0xA2  */   Ins_UNKNOWN,
    /*  INS_0xA3  */   Ins_UNKNOWN,
    /*  INS_0xA4  */   Ins_UNKNOWN,
    /*  INS_0xA5  */   Ins_UNKNOWN,
    /*  INS_0xA6  */   Ins_UNKNOWN,
    /*  INS_0xA7  */   Ins_UNKNOWN,
    /*  INS_0xA8  */   Ins_UNKNOWN,
    /*  INS_0xA9  */   Ins_UNKNOWN,
    /*  INS_0xAA  */   Ins_UNKNOWN,
    /*  INS_0xAB  */   Ins_UNKNOWN,
    /*  INS_0xAC  */   Ins_UNKNOWN,
    /*  INS_0xAD  */   Ins_UNKNOWN,
    /*  INS_0xAE  */   Ins_UNKNOWN,
    /*  INS_0xAF  */   Ins_UNKNOWN,

    /*  PushB[0]  */  Ins_PUSHB,
    /*  PushB[1]  */  Ins_PUSHB,
    /*  PushB[2]  */  Ins_PUSHB,
    /*  PushB[3]  */  Ins_PUSHB,
    /*  PushB[4]  */  Ins_PUSHB,
    /*  PushB[5]  */  Ins_PUSHB,
    /*  PushB[6]  */  Ins_PUSHB,
    /*  PushB[7]  */  Ins_PUSHB,
    /*  PushW[0]  */  Ins_PUSHW,
    /*  PushW[1]  */  Ins_PUSHW,
    /*  PushW[2]  */  Ins_PUSHW,
    /*  PushW[3]  */  Ins_PUSHW,
    /*  PushW[4]  */  Ins_PUSHW,
    /*  PushW[5]  */  Ins_PUSHW,
    /*  PushW[6]  */  Ins_PUSHW,
    /*  PushW[7]  */  Ins_PUSHW,

    /*  MDRP[00]  */  Ins_MDRP,
    /*  MDRP[01]  */  Ins_MDRP,
    /*  MDRP[02]  */  Ins_MDRP,
    /*  MDRP[03]  */  Ins_MDRP,
    /*  MDRP[04]  */  Ins_MDRP,
    /*  MDRP[05]  */  Ins_MDRP,
    /*  MDRP[06]  */  Ins_MDRP,
    /*  MDRP[07]  */  Ins_MDRP,
    /*  MDRP[08]  */  Ins_MDRP,
    /*  MDRP[09]  */  Ins_MDRP,
    /*  MDRP[10]  */  Ins_MDRP,
    /*  MDRP[11]  */  Ins_MDRP,
    /*  MDRP[12]  */  Ins_MDRP,
    /*  MDRP[13]  */  Ins_MDRP,
    /*  MDRP[14]  */  Ins_MDRP,
    /*  MDRP[15]  */  Ins_MDRP,

    /*  MDRP[16]  */  Ins_MDRP,
    /*  MDRP[17]  */  Ins_MDRP,
    /*  MDRP[18]  */  Ins_MDRP,
    /*  MDRP[19]  */  Ins_MDRP,
    /*  MDRP[20]  */  Ins_MDRP,
    /*  MDRP[21]  */  Ins_MDRP,
    /*  MDRP[22]  */  Ins_MDRP,
    /*  MDRP[23]  */  Ins_MDRP,
    /*  MDRP[24]  */  Ins_MDRP,
    /*  MDRP[25]  */  Ins_MDRP,
    /*  MDRP[26]  */  Ins_MDRP,
    /*  MDRP[27]  */  Ins_MDRP,
    /*  MDRP[28]  */  Ins_MDRP,
    /*  MDRP[29]  */  Ins_MDRP,
    /*  MDRP[30]  */  Ins_MDRP,
    /*  MDRP[31]  */  Ins_MDRP,

    /*  MIRP[00]  */  Ins_MIRP,
    /*  MIRP[01]  */  Ins_MIRP,
    /*  MIRP[02]  */  Ins_MIRP,
    /*  MIRP[03]  */  Ins_MIRP,
    /*  MIRP[04]  */  Ins_MIRP,
    /*  MIRP[05]  */  Ins_MIRP,
    /*  MIRP[06]  */  Ins_MIRP,
    /*  MIRP[07]  */  Ins_MIRP,
    /*  MIRP[08]  */  Ins_MIRP,
    /*  MIRP[09]  */  Ins_MIRP,
    /*  MIRP[10]  */  Ins_MIRP,
    /*  MIRP[11]  */  Ins_MIRP,
    /*  MIRP[12]  */  Ins_MIRP,
    /*  MIRP[13]  */  Ins_MIRP,
    /*  MIRP[14]  */  Ins_MIRP,
    /*  MIRP[15]  */  Ins_MIRP,

    /*  MIRP[16]  */  Ins_MIRP,
    /*  MIRP[17]  */  Ins_MIRP,
    /*  MIRP[18]  */  Ins_MIRP,
    /*  MIRP[19]  */  Ins_MIRP,
    /*  MIRP[20]  */  Ins_MIRP,
    /*  MIRP[21]  */  Ins_MIRP,
    /*  MIRP[22]  */  Ins_MIRP,
    /*  MIRP[23]  */  Ins_MIRP,
    /*  MIRP[24]  */  Ins_MIRP,
    /*  MIRP[25]  */  Ins_MIRP,
    /*  MIRP[26]  */  Ins_MIRP,
    /*  MIRP[27]  */  Ins_MIRP,
    /*  MIRP[28]  */  Ins_MIRP,
    /*  MIRP[29]  */  Ins_MIRP,
    /*  MIRP[30]  */  Ins_MIRP,
    /*  MIRP[31]  */  Ins_MIRP
  };


#endif /* !TT_CONFIG_OPTION_INTERPRETER_SWITCH */


  /*************************************************************************/
  /*                                                                       */
  /* RUN                                                                   */
  /*                                                                       */
  /*  This function executes a run of opcodes.  It will exit in the        */
  /*  following cases:                                                     */
  /*                                                                       */
  /*  - Errors (in which case it returns FALSE).                           */
  /*                                                                       */
  /*  - Reaching the end of the main code range (returns TRUE).            */
  /*    Reaching the end of a code range within a function call is an      */
  /*    error.                                                             */
  /*                                                                       */
  /*  - After executing one single opcode, if the flag `Instruction_Trap'  */
  /*    is set to TRUE (returns TRUE).                                     */
  /*                                                                       */
  /*  On exit with TRUE, test IP < CodeSize to know whether it comes from  */
  /*  an instruction trap or a normal termination.                         */
  /*                                                                       */
  /*                                                                       */
  /*  Note: The documented DEBUG opcode pops a value from the stack.  This */
  /*        behaviour is unsupported; here a DEBUG opcode is always an     */
  /*        error.                                                         */
  /*                                                                       */
  /*                                                                       */
  /* THIS IS THE INTERPRETER'S MAIN LOOP.                                  */
  /*                                                                       */
  /*  Instructions appear in the specification's order.                    */
  /*                                                                       */
  /*************************************************************************/


  /* documentation is in ttinterp.h */

  FT_EXPORT_DEF( FT_Error )
  TT_RunIns( TT_ExecContext  exc )
  {
    FT_Long    ins_counter = 0;  /* executed instructions counter */
    FT_UShort  i;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    FT_Byte    opcode_pattern[1][2] = {
                  /* #8 TypeMan Talk Align */
                  {
                    0x06, /* SPVTL   */
                    0x7D, /* RDTG    */
                  },
                };
    FT_UShort  opcode_patterns   = 1;
    FT_UShort  opcode_pointer[1] = { 0 };
    FT_UShort  opcode_size[1]    = { 1 };
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */


#ifdef TT_CONFIG_OPTION_STATIC_RASTER
    cur = *exc;
#endif

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
    CUR.iup_called = FALSE;
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

    /* set CVT functions */
    CUR.tt_metrics.ratio = 0;
    if ( CUR.metrics.x_ppem != CUR.metrics.y_ppem )
    {
      /* non-square pixels, use the stretched routines */
      CUR.func_read_cvt  = Read_CVT_Stretched;
      CUR.func_write_cvt = Write_CVT_Stretched;
      CUR.func_move_cvt  = Move_CVT_Stretched;
    }
    else
    {
      /* square pixels, use normal routines */
      CUR.func_read_cvt  = Read_CVT;
      CUR.func_write_cvt = Write_CVT;
      CUR.func_move_cvt  = Move_CVT;
    }

    COMPUTE_Funcs();
    COMPUTE_Round( (FT_Byte)exc->GS.round_state );

    do
    {
      CUR.opcode = CUR.code[CUR.IP];

      FT_TRACE7(( "  " ));
      FT_TRACE7(( opcode_name[CUR.opcode] ));
      FT_TRACE7(( "\n" ));

      if ( ( CUR.length = opcode_length[CUR.opcode] ) < 0 )
      {
        if ( CUR.IP + 1 >= CUR.codeSize )
          goto LErrorCodeOverflow_;

        CUR.length = 2 - CUR.length * CUR.code[CUR.IP + 1];
      }

      if ( CUR.IP + CUR.length > CUR.codeSize )
        goto LErrorCodeOverflow_;

      /* First, let's check for empty stack and overflow */
      CUR.args = CUR.top - ( Pop_Push_Count[CUR.opcode] >> 4 );

      /* `args' is the top of the stack once arguments have been popped. */
      /* One can also interpret it as the index of the last argument.    */
      if ( CUR.args < 0 )
      {
        if ( CUR.pedantic_hinting )
        {
          CUR.error = FT_THROW( Too_Few_Arguments );
          goto LErrorLabel_;
        }

        /* push zeroes onto the stack */
        for ( i = 0; i < Pop_Push_Count[CUR.opcode] >> 4; i++ )
          CUR.stack[i] = 0;
        CUR.args = 0;
      }

      CUR.new_top = CUR.args + ( Pop_Push_Count[CUR.opcode] & 15 );

      /* `new_top' is the new top of the stack, after the instruction's */
      /* execution.  `top' will be set to `new_top' after the `switch'  */
      /* statement.                                                     */
      if ( CUR.new_top > CUR.stackSize )
      {
        CUR.error = FT_THROW( Stack_Overflow );
        goto LErrorLabel_;
      }

      CUR.step_ins = TRUE;
      CUR.error    = FT_Err_Ok;

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

      if ( SUBPIXEL_HINTING )
      {
        for ( i = 0; i < opcode_patterns; i++ )
        {
          if ( opcode_pointer[i] < opcode_size[i]                 &&
               CUR.opcode == opcode_pattern[i][opcode_pointer[i]] )
          {
            opcode_pointer[i] += 1;

            if ( opcode_pointer[i] == opcode_size[i] )
            {
              FT_TRACE7(( "sph: opcode ptrn: %d, %s %s\n",
                          i,
                          CUR.face->root.family_name,
                          CUR.face->root.style_name ));

              switch ( i )
              {
              case 0:
                break;
              }
              opcode_pointer[i] = 0;
            }
          }
          else
            opcode_pointer[i] = 0;
        }
      }

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

#ifdef TT_CONFIG_OPTION_INTERPRETER_SWITCH

      {
        FT_Long*  args   = CUR.stack + CUR.args;
        FT_Byte   opcode = CUR.opcode;


#undef  ARRAY_BOUND_ERROR
#define ARRAY_BOUND_ERROR  goto Set_Invalid_Ref


        switch ( opcode )
        {
        case 0x00:  /* SVTCA y  */
        case 0x01:  /* SVTCA x  */
        case 0x02:  /* SPvTCA y */
        case 0x03:  /* SPvTCA x */
        case 0x04:  /* SFvTCA y */
        case 0x05:  /* SFvTCA x */
          {
            FT_Short  AA, BB;


            AA = (FT_Short)( ( opcode & 1 ) << 14 );
            BB = (FT_Short)( AA ^ 0x4000 );

            if ( opcode < 4 )
            {
              CUR.GS.projVector.x = AA;
              CUR.GS.projVector.y = BB;

              CUR.GS.dualVector.x = AA;
              CUR.GS.dualVector.y = BB;
            }
            else
            {
              GUESS_VECTOR( projVector );
            }

            if ( ( opcode & 2 ) == 0 )
            {
              CUR.GS.freeVector.x = AA;
              CUR.GS.freeVector.y = BB;
            }
            else
            {
              GUESS_VECTOR( freeVector );
            }

            COMPUTE_Funcs();
          }
          break;

        case 0x06:  /* SPvTL // */
        case 0x07:  /* SPvTL +  */
          DO_SPVTL
          break;

        case 0x08:  /* SFvTL // */
        case 0x09:  /* SFvTL +  */
          DO_SFVTL
          break;

        case 0x0A:  /* SPvFS */
          DO_SPVFS
          break;

        case 0x0B:  /* SFvFS */
          DO_SFVFS
          break;

        case 0x0C:  /* GPV */
          DO_GPV
          break;

        case 0x0D:  /* GFV */
          DO_GFV
          break;

        case 0x0E:  /* SFvTPv */
          DO_SFVTPV
          break;

        case 0x0F:  /* ISECT  */
          Ins_ISECT( EXEC_ARG_ args );
          break;

        case 0x10:  /* SRP0 */
          DO_SRP0
          break;

        case 0x11:  /* SRP1 */
          DO_SRP1
          break;

        case 0x12:  /* SRP2 */
          DO_SRP2
          break;

        case 0x13:  /* SZP0 */
          Ins_SZP0( EXEC_ARG_ args );
          break;

        case 0x14:  /* SZP1 */
          Ins_SZP1( EXEC_ARG_ args );
          break;

        case 0x15:  /* SZP2 */
          Ins_SZP2( EXEC_ARG_ args );
          break;

        case 0x16:  /* SZPS */
          Ins_SZPS( EXEC_ARG_ args );
          break;

        case 0x17:  /* SLOOP */
          DO_SLOOP
          break;

        case 0x18:  /* RTG */
          DO_RTG
          break;

        case 0x19:  /* RTHG */
          DO_RTHG
          break;

        case 0x1A:  /* SMD */
          DO_SMD
          break;

        case 0x1B:  /* ELSE */
          Ins_ELSE( EXEC_ARG_ args );
          break;

        case 0x1C:  /* JMPR */
          DO_JMPR
          break;

        case 0x1D:  /* SCVTCI */
          DO_SCVTCI
          break;

        case 0x1E:  /* SSWCI */
          DO_SSWCI
          break;

        case 0x1F:  /* SSW */
          DO_SSW
          break;

        case 0x20:  /* DUP */
          DO_DUP
          break;

        case 0x21:  /* POP */
          /* nothing :-) */
          break;

        case 0x22:  /* CLEAR */
          DO_CLEAR
          break;

        case 0x23:  /* SWAP */
          DO_SWAP
          break;

        case 0x24:  /* DEPTH */
          DO_DEPTH
          break;

        case 0x25:  /* CINDEX */
          DO_CINDEX
          break;

        case 0x26:  /* MINDEX */
          Ins_MINDEX( EXEC_ARG_ args );
          break;

        case 0x27:  /* ALIGNPTS */
          Ins_ALIGNPTS( EXEC_ARG_ args );
          break;

        case 0x28:  /* ???? */
          Ins_UNKNOWN( EXEC_ARG_ args );
          break;

        case 0x29:  /* UTP */
          Ins_UTP( EXEC_ARG_ args );
          break;

        case 0x2A:  /* LOOPCALL */
          Ins_LOOPCALL( EXEC_ARG_ args );
          break;

        case 0x2B:  /* CALL */
          Ins_CALL( EXEC_ARG_ args );
          break;

        case 0x2C:  /* FDEF */
          Ins_FDEF( EXEC_ARG_ args );
          break;

        case 0x2D:  /* ENDF */
          Ins_ENDF( EXEC_ARG_ args );
          break;

        case 0x2E:  /* MDAP */
        case 0x2F:  /* MDAP */
          Ins_MDAP( EXEC_ARG_ args );
          break;

        case 0x30:  /* IUP */
        case 0x31:  /* IUP */
          Ins_IUP( EXEC_ARG_ args );
          break;

        case 0x32:  /* SHP */
        case 0x33:  /* SHP */
          Ins_SHP( EXEC_ARG_ args );
          break;

        case 0x34:  /* SHC */
        case 0x35:  /* SHC */
          Ins_SHC( EXEC_ARG_ args );
          break;

        case 0x36:  /* SHZ */
        case 0x37:  /* SHZ */
          Ins_SHZ( EXEC_ARG_ args );
          break;

        case 0x38:  /* SHPIX */
          Ins_SHPIX( EXEC_ARG_ args );
          break;

        case 0x39:  /* IP    */
          Ins_IP( EXEC_ARG_ args );
          break;

        case 0x3A:  /* MSIRP */
        case 0x3B:  /* MSIRP */
          Ins_MSIRP( EXEC_ARG_ args );
          break;

        case 0x3C:  /* AlignRP */
          Ins_ALIGNRP( EXEC_ARG_ args );
          break;

        case 0x3D:  /* RTDG */
          DO_RTDG
          break;

        case 0x3E:  /* MIAP */
        case 0x3F:  /* MIAP */
          Ins_MIAP( EXEC_ARG_ args );
          break;

        case 0x40:  /* NPUSHB */
          Ins_NPUSHB( EXEC_ARG_ args );
          break;

        case 0x41:  /* NPUSHW */
          Ins_NPUSHW( EXEC_ARG_ args );
          break;

        case 0x42:  /* WS */
          DO_WS
          break;

      Set_Invalid_Ref:
            CUR.error = FT_THROW( Invalid_Reference );
          break;

        case 0x43:  /* RS */
          DO_RS
          break;

        case 0x44:  /* WCVTP */
          DO_WCVTP
          break;

        case 0x45:  /* RCVT */
          DO_RCVT
          break;

        case 0x46:  /* GC */
        case 0x47:  /* GC */
          Ins_GC( EXEC_ARG_ args );
          break;

        case 0x48:  /* SCFS */
          Ins_SCFS( EXEC_ARG_ args );
          break;

        case 0x49:  /* MD */
        case 0x4A:  /* MD */
          Ins_MD( EXEC_ARG_ args );
          break;

        case 0x4B:  /* MPPEM */
          DO_MPPEM
          break;

        case 0x4C:  /* MPS */
          DO_MPS
          break;

        case 0x4D:  /* FLIPON */
          DO_FLIPON
          break;

        case 0x4E:  /* FLIPOFF */
          DO_FLIPOFF
          break;

        case 0x4F:  /* DEBUG */
          DO_DEBUG
          break;

        case 0x50:  /* LT */
          DO_LT
          break;

        case 0x51:  /* LTEQ */
          DO_LTEQ
          break;

        case 0x52:  /* GT */
          DO_GT
          break;

        case 0x53:  /* GTEQ */
          DO_GTEQ
          break;

        case 0x54:  /* EQ */
          DO_EQ
          break;

        case 0x55:  /* NEQ */
          DO_NEQ
          break;

        case 0x56:  /* ODD */
          DO_ODD
          break;

        case 0x57:  /* EVEN */
          DO_EVEN
          break;

        case 0x58:  /* IF */
          Ins_IF( EXEC_ARG_ args );
          break;

        case 0x59:  /* EIF */
          /* do nothing */
          break;

        case 0x5A:  /* AND */
          DO_AND
          break;

        case 0x5B:  /* OR */
          DO_OR
          break;

        case 0x5C:  /* NOT */
          DO_NOT
          break;

        case 0x5D:  /* DELTAP1 */
          Ins_DELTAP( EXEC_ARG_ args );
          break;

        case 0x5E:  /* SDB */
          DO_SDB
          break;

        case 0x5F:  /* SDS */
          DO_SDS
          break;

        case 0x60:  /* ADD */
          DO_ADD
          break;

        case 0x61:  /* SUB */
          DO_SUB
          break;

        case 0x62:  /* DIV */
          DO_DIV
          break;

        case 0x63:  /* MUL */
          DO_MUL
          break;

        case 0x64:  /* ABS */
          DO_ABS
          break;

        case 0x65:  /* NEG */
          DO_NEG
          break;

        case 0x66:  /* FLOOR */
          DO_FLOOR
          break;

        case 0x67:  /* CEILING */
          DO_CEILING
          break;

        case 0x68:  /* ROUND */
        case 0x69:  /* ROUND */
        case 0x6A:  /* ROUND */
        case 0x6B:  /* ROUND */
          DO_ROUND
          break;

        case 0x6C:  /* NROUND */
        case 0x6D:  /* NROUND */
        case 0x6E:  /* NRRUND */
        case 0x6F:  /* NROUND */
          DO_NROUND
          break;

        case 0x70:  /* WCVTF */
          DO_WCVTF
          break;

        case 0x71:  /* DELTAP2 */
        case 0x72:  /* DELTAP3 */
          Ins_DELTAP( EXEC_ARG_ args );
          break;

        case 0x73:  /* DELTAC0 */
        case 0x74:  /* DELTAC1 */
        case 0x75:  /* DELTAC2 */
          Ins_DELTAC( EXEC_ARG_ args );
          break;

        case 0x76:  /* SROUND */
          DO_SROUND
          break;

        case 0x77:  /* S45Round */
          DO_S45ROUND
          break;

        case 0x78:  /* JROT */
          DO_JROT
          break;

        case 0x79:  /* JROF */
          DO_JROF
          break;

        case 0x7A:  /* ROFF */
          DO_ROFF
          break;

        case 0x7B:  /* ???? */
          Ins_UNKNOWN( EXEC_ARG_ args );
          break;

        case 0x7C:  /* RUTG */
          DO_RUTG
          break;

        case 0x7D:  /* RDTG */
          DO_RDTG
          break;

        case 0x7E:  /* SANGW */
        case 0x7F:  /* AA    */
          /* nothing - obsolete */
          break;

        case 0x80:  /* FLIPPT */
          Ins_FLIPPT( EXEC_ARG_ args );
          break;

        case 0x81:  /* FLIPRGON */
          Ins_FLIPRGON( EXEC_ARG_ args );
          break;

        case 0x82:  /* FLIPRGOFF */
          Ins_FLIPRGOFF( EXEC_ARG_ args );
          break;

        case 0x83:  /* UNKNOWN */
        case 0x84:  /* UNKNOWN */
          Ins_UNKNOWN( EXEC_ARG_ args );
          break;

        case 0x85:  /* SCANCTRL */
          Ins_SCANCTRL( EXEC_ARG_ args );
          break;

        case 0x86:  /* SDPVTL */
        case 0x87:  /* SDPVTL */
          Ins_SDPVTL( EXEC_ARG_ args );
          break;

        case 0x88:  /* GETINFO */
          Ins_GETINFO( EXEC_ARG_ args );
          break;

        case 0x89:  /* IDEF */
          Ins_IDEF( EXEC_ARG_ args );
          break;

        case 0x8A:  /* ROLL */
          Ins_ROLL( EXEC_ARG_ args );
          break;

        case 0x8B:  /* MAX */
          DO_MAX
          break;

        case 0x8C:  /* MIN */
          DO_MIN
          break;

        case 0x8D:  /* SCANTYPE */
          Ins_SCANTYPE( EXEC_ARG_ args );
          break;

        case 0x8E:  /* INSTCTRL */
          Ins_INSTCTRL( EXEC_ARG_ args );
          break;

        case 0x8F:
          Ins_UNKNOWN( EXEC_ARG_ args );
          break;

        default:
          if ( opcode >= 0xE0 )
            Ins_MIRP( EXEC_ARG_ args );
          else if ( opcode >= 0xC0 )
            Ins_MDRP( EXEC_ARG_ args );
          else if ( opcode >= 0xB8 )
            Ins_PUSHW( EXEC_ARG_ args );
          else if ( opcode >= 0xB0 )
            Ins_PUSHB( EXEC_ARG_ args );
          else
            Ins_UNKNOWN( EXEC_ARG_ args );
        }

      }

#else

      Instruct_Dispatch[CUR.opcode]( EXEC_ARG_ &CUR.stack[CUR.args] );

#endif /* TT_CONFIG_OPTION_INTERPRETER_SWITCH */

      if ( CUR.error )
      {
        switch ( CUR.error )
        {
          /* looking for redefined instructions */
        case FT_ERR( Invalid_Opcode ):
          {
            TT_DefRecord*  def   = CUR.IDefs;
            TT_DefRecord*  limit = def + CUR.numIDefs;


            for ( ; def < limit; def++ )
            {
              if ( def->active && CUR.opcode == (FT_Byte)def->opc )
              {
                TT_CallRec*  callrec;


                if ( CUR.callTop >= CUR.callSize )
                {
                  CUR.error = FT_THROW( Invalid_Reference );
                  goto LErrorLabel_;
                }

                callrec = &CUR.callStack[CUR.callTop];

                callrec->Caller_Range = CUR.curRange;
                callrec->Caller_IP    = CUR.IP + 1;
                callrec->Cur_Count    = 1;
                callrec->Def          = def;

                if ( INS_Goto_CodeRange( def->range, def->start ) == FAILURE )
                  goto LErrorLabel_;

                goto LSuiteLabel_;
              }
            }
          }

          CUR.error = FT_THROW( Invalid_Opcode );
          goto LErrorLabel_;

#if 0
          break;   /* Unreachable code warning suppression.             */
                   /* Leave to remind in case a later change the editor */
                   /* to consider break;                                */
#endif

        default:
          goto LErrorLabel_;

#if 0
        break;
#endif
        }
      }

      CUR.top = CUR.new_top;

      if ( CUR.step_ins )
        CUR.IP += CUR.length;

      /* increment instruction counter and check if we didn't */
      /* run this program for too long (e.g. infinite loops). */
      if ( ++ins_counter > MAX_RUNNABLE_OPCODES )
        return FT_THROW( Execution_Too_Long );

    LSuiteLabel_:
      if ( CUR.IP >= CUR.codeSize )
      {
        if ( CUR.callTop > 0 )
        {
          CUR.error = FT_THROW( Code_Overflow );
          goto LErrorLabel_;
        }
        else
          goto LNo_Error_;
      }
    } while ( !CUR.instruction_trap );

  LNo_Error_:

#ifdef TT_CONFIG_OPTION_STATIC_RASTER
    *exc = cur;
#endif

    return FT_Err_Ok;

  LErrorCodeOverflow_:
    CUR.error = FT_THROW( Code_Overflow );

  LErrorLabel_:

#ifdef TT_CONFIG_OPTION_STATIC_RASTER
    *exc = cur;
#endif

    /* If any errors have occurred, function tables may be broken. */
    /* Force a re-execution of `prep' and `fpgm' tables if no      */
    /* bytecode debugger is run.                                   */
    if ( CUR.error && !CUR.instruction_trap )
    {
      FT_TRACE1(( "  The interpreter returned error 0x%x\n", CUR.error ));
      exc->size->cvt_ready      = FALSE;
    }

    return CUR.error;
  }


#endif /* TT_USE_BYTECODE_INTERPRETER */


/* END */
/***************************************************************************/
/*                                                                         */
/*  ttsubpix.c                                                             */
/*                                                                         */
/*    TrueType Subpixel Hinting.                                           */
/*                                                                         */
/*  Copyright 2010-2013 by                                                 */
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
#include FT_INTERNAL_SFNT_H
#include FT_TRUETYPE_TAGS_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_DRIVER_H

#include "ttsubpix.h"


#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING

  /*************************************************************************/
  /*                                                                       */
  /* These rules affect how the TT Interpreter does hinting, with the      */
  /* goal of doing subpixel hinting by (in general) ignoring x moves.      */
  /* Some of these rules are fixes that go above and beyond the            */
  /* stated techniques in the MS whitepaper on Cleartype, due to           */
  /* artifacts in many glyphs.  So, these rules make some glyphs render    */
  /* better than they do in the MS rasterizer.                             */
  /*                                                                       */
  /* "" string or 0 int/char indicates to apply to all glyphs.             */
  /* "-" used as dummy placeholders, but any non-matching string works.    */
  /*                                                                       */
  /* Some of this could arguably be implemented in fontconfig, however:    */
  /*                                                                       */
  /*  - Fontconfig can't set things on a glyph-by-glyph basis.             */
  /*  - The tweaks that happen here are very low-level, from an average    */
  /*    user's point of view and are best implemented in the hinter.       */
  /*                                                                       */
  /* The goal is to make the subpixel hinting techniques as generalized    */
  /* as possible across all fonts to prevent the need for extra rules such */
  /* as these.                                                             */
  /*                                                                       */
  /* The rule structure is designed so that entirely new rules can easily  */
  /* be added when a new compatibility feature is discovered.              */
  /*                                                                       */
  /* The rule structures could also use some enhancement to handle ranges. */
  /*                                                                       */
  /*     ****************** WORK IN PROGRESS *******************           */
  /*                                                                       */

  /* These are `classes' of fonts that can be grouped together and used in */
  /* rules below.  A blank entry "" is required at the end of these!       */
#define FAMILY_CLASS_RULES_SIZE  7

  static const SPH_Font_Class FAMILY_CLASS_Rules
                              [FAMILY_CLASS_RULES_SIZE] =
  {
    { "MS Legacy Fonts",
      { "Aharoni",
        "Andale Mono",
        "Andalus",
        "Angsana New",
        "AngsanaUPC",
        "Arabic Transparent",
        "Arial Black",
        "Arial Narrow",
        "Arial Unicode MS",
        "Arial",
        "Batang",
        "Browallia New",
        "BrowalliaUPC",
        "Comic Sans MS",
        "Cordia New",
        "CordiaUPC",
        "Courier New",
        "DFKai-SB",
        "David Transparent",
        "David",
        "DilleniaUPC",
        "Estrangelo Edessa",
        "EucrosiaUPC",
        "FangSong_GB2312",
        "Fixed Miriam Transparent",
        "FrankRuehl",
        "Franklin Gothic Medium",
        "FreesiaUPC",
        "Garamond",
        "Gautami",
        "Georgia",
        "Gulim",
        "Impact",
        "IrisUPC",
        "JasmineUPC",
        "KaiTi_GB2312",
        "KodchiangUPC",
        "Latha",
        "Levenim MT",
        "LilyUPC",
        "Lucida Console",
        "Lucida Sans Unicode",
        "MS Gothic",
        "MS Mincho",
        "MV Boli",
        "Mangal",
        "Marlett",
        "Microsoft Sans Serif",
        "Mingliu",
        "Miriam Fixed",
        "Miriam Transparent",
        "Miriam",
        "Narkisim",
        "Palatino Linotype",
        "Raavi",
        "Rod Transparent",
        "Rod",
        "Shruti",
        "SimHei",
        "Simplified Arabic Fixed",
        "Simplified Arabic",
        "Simsun",
        "Sylfaen",
        "Symbol",
        "Tahoma",
        "Times New Roman",
        "Traditional Arabic",
        "Trebuchet MS",
        "Tunga",
        "Verdana",
        "Webdings",
        "Wingdings",
        "",
      },
    },
    { "Core MS Legacy Fonts",
      { "Arial Black",
        "Arial Narrow",
        "Arial Unicode MS",
        "Arial",
        "Comic Sans MS",
        "Courier New",
        "Garamond",
        "Georgia",
        "Impact",
        "Lucida Console",
        "Lucida Sans Unicode",
        "Microsoft Sans Serif",
        "Palatino Linotype",
        "Tahoma",
        "Times New Roman",
        "Trebuchet MS",
        "Verdana",
        "",
      },
    },
    { "Apple Legacy Fonts",
      { "Geneva",
        "Times",
        "Monaco",
        "Century",
        "Chalkboard",
        "Lobster",
        "Century Gothic",
        "Optima",
        "Lucida Grande",
        "Gill Sans",
        "Baskerville",
        "Helvetica",
        "Helvetica Neue",
        "",
      },
    },
    { "Legacy Sans Fonts",
      { "Andale Mono",
        "Arial Unicode MS",
        "Arial",
        "Century Gothic",
        "Comic Sans MS",
        "Franklin Gothic Medium",
        "Geneva",
        "Lucida Console",
        "Lucida Grande",
        "Lucida Sans Unicode",
        "Lucida Sans Typewriter",
        "Microsoft Sans Serif",
        "Monaco",
        "Tahoma",
        "Trebuchet MS",
        "Verdana",
        "",
      },
    },

    { "Misc Legacy Fonts",
      { "Dark Courier", "", }, },
    { "Verdana Clones",
      { "DejaVu Sans",
        "Bitstream Vera Sans", "", }, },
    { "Verdana and Clones",
      { "DejaVu Sans",
        "Bitstream Vera Sans",
        "Verdana", "", }, },
  };


  /* Define this to force natural (i.e. not bitmap-compatible) widths.     */
  /* The default leans strongly towards natural widths except for a few    */
  /* legacy fonts where a selective combination produces nicer results.    */
/* #define FORCE_NATURAL_WIDTHS   */


  /* Define `classes' of styles that can be grouped together and used in   */
  /* rules below.  A blank entry "" is required at the end of these!       */
#define STYLE_CLASS_RULES_SIZE  5

  const SPH_Font_Class STYLE_CLASS_Rules
                       [STYLE_CLASS_RULES_SIZE] =
  {
    { "Regular Class",
      { "Regular",
        "Book",
        "Medium",
        "Roman",
        "Normal",
        "",
      },
    },
    { "Regular/Italic Class",
      { "Regular",
        "Book",
        "Medium",
        "Italic",
        "Oblique",
        "Roman",
        "Normal",
        "",
      },
    },
    { "Bold/BoldItalic Class",
      { "Bold",
        "Bold Italic",
        "Black",
        "",
      },
    },
    { "Bold/Italic/BoldItalic Class",
      { "Bold",
        "Bold Italic",
        "Black",
        "Italic",
        "Oblique",
        "",
      },
    },
    { "Regular/Bold Class",
      { "Regular",
        "Book",
        "Medium",
        "Normal",
        "Roman",
        "Bold",
        "Black",
        "",
      },
    },
  };


  /* Force special legacy fixes for fonts.                                 */
#define COMPATIBILITY_MODE_RULES_SIZE  1

  const SPH_TweakRule  COMPATIBILITY_MODE_Rules
                       [COMPATIBILITY_MODE_RULES_SIZE] =
  {
    { "-", 0, "", 0 },
  };


  /* Don't do subpixel (ignore_x_mode) hinting; do normal hinting.         */
#define PIXEL_HINTING_RULES_SIZE  2

  const SPH_TweakRule  PIXEL_HINTING_Rules
                       [PIXEL_HINTING_RULES_SIZE] =
  {
    /* these characters are almost always safe */
    { "Courier New", 12, "Italic", 'z' },
    { "Courier New", 11, "Italic", 'z' },
  };


  /* Subpixel hinting ignores SHPIX rules on X.  Force SHPIX for these.    */
#define DO_SHPIX_RULES_SIZE  1

  const SPH_TweakRule  DO_SHPIX_Rules
                       [DO_SHPIX_RULES_SIZE] =
  {
    { "-", 0, "", 0 },
  };


  /* Skip Y moves that start with a point that is not on a Y pixel         */
  /* boundary and don't move that point to a Y pixel boundary.             */
#define SKIP_NONPIXEL_Y_MOVES_RULES_SIZE  4

  const SPH_TweakRule  SKIP_NONPIXEL_Y_MOVES_Rules
                       [SKIP_NONPIXEL_Y_MOVES_RULES_SIZE] =
  {
    /* fix vwxyz thinness*/
    { "Consolas", 0, "", 0 },
    /* Fix thin middle stems */
    { "Core MS Legacy Fonts", 0, "Regular", 0 },
    /* Cyrillic small letter I */
    { "Legacy Sans Fonts", 0, "", 0 },
    /* Fix artifacts with some Regular & Bold */
    { "Verdana Clones", 0, "", 0 },
  };


#define SKIP_NONPIXEL_Y_MOVES_RULES_EXCEPTIONS_SIZE  1

  const SPH_TweakRule  SKIP_NONPIXEL_Y_MOVES_Rules_Exceptions
                       [SKIP_NONPIXEL_Y_MOVES_RULES_EXCEPTIONS_SIZE] =
  {
    /* Fixes < and > */
    { "Courier New", 0, "Regular", 0 },
  };


  /* Skip Y moves that start with a point that is not on a Y pixel         */
  /* boundary and don't move that point to a Y pixel boundary.             */
#define SKIP_NONPIXEL_Y_MOVES_DELTAP_RULES_SIZE  2

  const SPH_TweakRule  SKIP_NONPIXEL_Y_MOVES_DELTAP_Rules
                       [SKIP_NONPIXEL_Y_MOVES_DELTAP_RULES_SIZE] =
  {
    /* Maintain thickness of diagonal in 'N' */
    { "Times New Roman", 0, "Regular/Bold Class", 'N' },
    { "Georgia", 0, "Regular/Bold Class", 'N' },
  };


  /* Skip Y moves that move a point off a Y pixel boundary.                */
#define SKIP_OFFPIXEL_Y_MOVES_RULES_SIZE  1

  const SPH_TweakRule  SKIP_OFFPIXEL_Y_MOVES_Rules
                       [SKIP_OFFPIXEL_Y_MOVES_RULES_SIZE] =
  {
    { "-", 0, "", 0 },
  };


#define SKIP_OFFPIXEL_Y_MOVES_RULES_EXCEPTIONS_SIZE  1

  const SPH_TweakRule  SKIP_OFFPIXEL_Y_MOVES_Rules_Exceptions
                       [SKIP_OFFPIXEL_Y_MOVES_RULES_EXCEPTIONS_SIZE] =
  {
    { "-", 0, "", 0 },
  };


  /* Round moves that don't move a point to a Y pixel boundary.            */
#define ROUND_NONPIXEL_Y_MOVES_RULES_SIZE  2

  const SPH_TweakRule  ROUND_NONPIXEL_Y_MOVES_Rules
                       [ROUND_NONPIXEL_Y_MOVES_RULES_SIZE] =
  {
    /* Droid font instructions don't snap Y to pixels */
    { "Droid Sans", 0, "Regular/Italic Class", 0 },
    { "Droid Sans Mono", 0, "", 0 },
  };


#define ROUND_NONPIXEL_Y_MOVES_RULES_EXCEPTIONS_SIZE  1

  const SPH_TweakRule  ROUND_NONPIXEL_Y_MOVES_Rules_Exceptions
                       [ROUND_NONPIXEL_Y_MOVES_RULES_EXCEPTIONS_SIZE] =
  {
    { "-", 0, "", 0 },
  };


  /* Allow a Direct_Move along X freedom vector if matched.                */
#define ALLOW_X_DMOVE_RULES_SIZE  1

  const SPH_TweakRule  ALLOW_X_DMOVE_Rules
                       [ALLOW_X_DMOVE_RULES_SIZE] =
  {
    /* Fixes vanishing diagonal in 4 */
    { "Verdana", 0, "Regular", '4' },
  };


  /* Return MS rasterizer version 35 if matched.                           */
#define RASTERIZER_35_RULES_SIZE  8

  const SPH_TweakRule  RASTERIZER_35_Rules
                       [RASTERIZER_35_RULES_SIZE] =
  {
    /* This seems to be the only way to make these look good */
    { "Times New Roman", 0, "Regular", 'i' },
    { "Times New Roman", 0, "Regular", 'j' },
    { "Times New Roman", 0, "Regular", 'm' },
    { "Times New Roman", 0, "Regular", 'r' },
    { "Times New Roman", 0, "Regular", 'a' },
    { "Times New Roman", 0, "Regular", 'n' },
    { "Times New Roman", 0, "Regular", 'p' },
    { "Times", 0, "", 0 },
  };


  /* Don't round to the subpixel grid.  Round to pixel grid.               */
#define NORMAL_ROUND_RULES_SIZE  1

  const SPH_TweakRule  NORMAL_ROUND_Rules
                       [NORMAL_ROUND_RULES_SIZE] =
  {
    /* Fix serif thickness for certain ppems */
    /* Can probably be generalized somehow   */
    { "Courier New", 0, "", 0 },
  };


  /* Skip IUP instructions if matched.                                     */
#define SKIP_IUP_RULES_SIZE  1

  const SPH_TweakRule  SKIP_IUP_Rules
                       [SKIP_IUP_RULES_SIZE] =
  {
    { "Arial", 13, "Regular", 'a' },
  };


  /* Skip MIAP Twilight hack if matched.                                   */
#define MIAP_HACK_RULES_SIZE  1

  const SPH_TweakRule  MIAP_HACK_Rules
                       [MIAP_HACK_RULES_SIZE] =
  {
    { "Geneva", 12, "", 0 },
  };


  /* Skip DELTAP instructions if matched.                                  */
#define ALWAYS_SKIP_DELTAP_RULES_SIZE  23

  const SPH_TweakRule  ALWAYS_SKIP_DELTAP_Rules
                       [ALWAYS_SKIP_DELTAP_RULES_SIZE] =
  {
    { "Georgia", 0, "Regular", 'k' },
    /* fix various problems with e in different versions */
    { "Trebuchet MS", 14, "Regular", 'e' },
    { "Trebuchet MS", 13, "Regular", 'e' },
    { "Trebuchet MS", 15, "Regular", 'e' },
    { "Trebuchet MS", 0, "Italic", 'v' },
    { "Trebuchet MS", 0, "Italic", 'w' },
    { "Trebuchet MS", 0, "Regular", 'Y' },
    { "Arial", 11, "Regular", 's' },
    /* prevent problems with '3' and others */
    { "Verdana", 10, "Regular", 0 },
    { "Verdana", 9, "Regular", 0 },
    /* Cyrillic small letter short I */
    { "Legacy Sans Fonts", 0, "", 0x438 },
    { "Legacy Sans Fonts", 0, "", 0x439 },
    { "Arial", 10, "Regular", '6' },
    { "Arial", 0, "Bold/BoldItalic Class", 'a' },
    /* Make horizontal stems consistent with the rest */
    { "Arial", 24, "Bold", 'a' },
    { "Arial", 25, "Bold", 'a' },
    { "Arial", 24, "Bold", 's' },
    { "Arial", 25, "Bold", 's' },
    { "Arial", 34, "Bold", 's' },
    { "Arial", 35, "Bold", 's' },
    { "Arial", 36, "Bold", 's' },
    { "Arial", 25, "Regular", 's' },
    { "Arial", 26, "Regular", 's' },
  };


  /* Always do DELTAP instructions if matched.                             */
#define ALWAYS_DO_DELTAP_RULES_SIZE  1

  const SPH_TweakRule  ALWAYS_DO_DELTAP_Rules
                       [ALWAYS_DO_DELTAP_RULES_SIZE] =
  {
    { "-", 0, "", 0 },
  };


  /* Don't allow ALIGNRP after IUP.                                        */
#define NO_ALIGNRP_AFTER_IUP_RULES_SIZE  1

  static const SPH_TweakRule  NO_ALIGNRP_AFTER_IUP_Rules
                              [NO_ALIGNRP_AFTER_IUP_RULES_SIZE] =
  {
    /* Prevent creation of dents in outline */
    { "-", 0, "", 0 },
  };


  /* Don't allow DELTAP after IUP.                                         */
#define NO_DELTAP_AFTER_IUP_RULES_SIZE  1

  static const SPH_TweakRule  NO_DELTAP_AFTER_IUP_Rules
                              [NO_DELTAP_AFTER_IUP_RULES_SIZE] =
  {
    { "-", 0, "", 0 },
  };


  /* Don't allow CALL after IUP.                                           */
#define NO_CALL_AFTER_IUP_RULES_SIZE  1

  static const SPH_TweakRule  NO_CALL_AFTER_IUP_Rules
                              [NO_CALL_AFTER_IUP_RULES_SIZE] =
  {
    /* Prevent creation of dents in outline */
    { "-", 0, "", 0 },
  };


  /* De-embolden these glyphs slightly.                                    */
#define DEEMBOLDEN_RULES_SIZE  9

  static const SPH_TweakRule  DEEMBOLDEN_Rules
                              [DEEMBOLDEN_RULES_SIZE] =
  {
    { "Courier New", 0, "Bold", 'A' },
    { "Courier New", 0, "Bold", 'W' },
    { "Courier New", 0, "Bold", 'w' },
    { "Courier New", 0, "Bold", 'M' },
    { "Courier New", 0, "Bold", 'X' },
    { "Courier New", 0, "Bold", 'K' },
    { "Courier New", 0, "Bold", 'x' },
    { "Courier New", 0, "Bold", 'z' },
    { "Courier New", 0, "Bold", 'v' },
  };


  /* Embolden these glyphs slightly.                                       */
#define EMBOLDEN_RULES_SIZE  2

  static const SPH_TweakRule  EMBOLDEN_Rules
                              [EMBOLDEN_RULES_SIZE] =
  {
    { "Courier New", 0, "Regular", 0 },
    { "Courier New", 0, "Italic", 0 },
  };


  /* This is a CVT hack that makes thick horizontal stems on 2, 5, 7       */
  /* similar to Windows XP.                                                */
#define TIMES_NEW_ROMAN_HACK_RULES_SIZE  12

  static const SPH_TweakRule  TIMES_NEW_ROMAN_HACK_Rules
                              [TIMES_NEW_ROMAN_HACK_RULES_SIZE] =
  {
    { "Times New Roman", 16, "Italic", '2' },
    { "Times New Roman", 16, "Italic", '5' },
    { "Times New Roman", 16, "Italic", '7' },
    { "Times New Roman", 16, "Regular", '2' },
    { "Times New Roman", 16, "Regular", '5' },
    { "Times New Roman", 16, "Regular", '7' },
    { "Times New Roman", 17, "Italic", '2' },
    { "Times New Roman", 17, "Italic", '5' },
    { "Times New Roman", 17, "Italic", '7' },
    { "Times New Roman", 17, "Regular", '2' },
    { "Times New Roman", 17, "Regular", '5' },
    { "Times New Roman", 17, "Regular", '7' },
  };


  /* This fudges distance on 2 to get rid of the vanishing stem issue.     */
  /* A real solution to this is certainly welcome.                         */
#define COURIER_NEW_2_HACK_RULES_SIZE  15

  static const SPH_TweakRule  COURIER_NEW_2_HACK_Rules
                              [COURIER_NEW_2_HACK_RULES_SIZE] =
  {
    { "Courier New", 10, "Regular", '2' },
    { "Courier New", 11, "Regular", '2' },
    { "Courier New", 12, "Regular", '2' },
    { "Courier New", 13, "Regular", '2' },
    { "Courier New", 14, "Regular", '2' },
    { "Courier New", 15, "Regular", '2' },
    { "Courier New", 16, "Regular", '2' },
    { "Courier New", 17, "Regular", '2' },
    { "Courier New", 18, "Regular", '2' },
    { "Courier New", 19, "Regular", '2' },
    { "Courier New", 20, "Regular", '2' },
    { "Courier New", 21, "Regular", '2' },
    { "Courier New", 22, "Regular", '2' },
    { "Courier New", 23, "Regular", '2' },
    { "Courier New", 24, "Regular", '2' },
  };


#ifndef FORCE_NATURAL_WIDTHS

  /* Use compatible widths with these glyphs.  Compatible widths is always */
  /* on when doing B/W TrueType instructing, but is used selectively here, */
  /* typically on glyphs with 3 or more vertical stems.                    */
#define COMPATIBLE_WIDTHS_RULES_SIZE  38

  static const SPH_TweakRule  COMPATIBLE_WIDTHS_Rules
                              [COMPATIBLE_WIDTHS_RULES_SIZE] =
  {
    { "Arial Unicode MS", 12, "Regular Class", 'm' },
    { "Arial Unicode MS", 14, "Regular Class", 'm' },
    /* Cyrillic small letter sha */
    { "Arial", 10, "Regular Class", 0x448 },
    { "Arial", 11, "Regular Class", 'm' },
    { "Arial", 12, "Regular Class", 'm' },
    /* Cyrillic small letter sha */
    { "Arial", 12, "Regular Class", 0x448 },
    { "Arial", 13, "Regular Class", 0x448 },
    { "Arial", 14, "Regular Class", 'm' },
    /* Cyrillic small letter sha */
    { "Arial", 14, "Regular Class", 0x448 },
    { "Arial", 15, "Regular Class", 0x448 },
    { "Arial", 17, "Regular Class", 'm' },
    { "DejaVu Sans", 15, "Regular Class", 0 },
    { "Microsoft Sans Serif", 11, "Regular Class", 0 },
    { "Microsoft Sans Serif", 12, "Regular Class", 0 },
    { "Segoe UI", 11, "Regular Class", 0 },
    { "Monaco", 0, "Regular Class", 0 },
    { "Segoe UI", 12, "Regular Class", 'm' },
    { "Segoe UI", 14, "Regular Class", 'm' },
    { "Tahoma", 11, "Regular Class", 0 },
    { "Times New Roman", 16, "Regular Class", 'c' },
    { "Times New Roman", 16, "Regular Class", 'm' },
    { "Times New Roman", 16, "Regular Class", 'o' },
    { "Times New Roman", 16, "Regular Class", 'w' },
    { "Trebuchet MS", 11, "Regular Class", 0 },
    { "Trebuchet MS", 12, "Regular Class", 0 },
    { "Trebuchet MS", 14, "Regular Class", 0 },
    { "Trebuchet MS", 15, "Regular Class", 0 },
    { "Ubuntu", 12, "Regular Class", 'm' },
    /* Cyrillic small letter sha */
    { "Verdana", 10, "Regular Class", 0x448 },
    { "Verdana", 11, "Regular Class", 0x448 },
    { "Verdana and Clones", 12, "Regular Class", 'i' },
    { "Verdana and Clones", 12, "Regular Class", 'j' },
    { "Verdana and Clones", 12, "Regular Class", 'l' },
    { "Verdana and Clones", 12, "Regular Class", 'm' },
    { "Verdana and Clones", 13, "Regular Class", 'i' },
    { "Verdana and Clones", 13, "Regular Class", 'j' },
    { "Verdana and Clones", 13, "Regular Class", 'l' },
    { "Verdana and Clones", 14, "Regular Class", 'm' },
  };


  /* Scaling slightly in the x-direction prior to hinting results in       */
  /* more visually pleasing glyphs in certain cases.                       */
  /* This sometimes needs to be coordinated with compatible width rules.   */
  /* A value of 1000 corresponds to a scaled value of 1.0.                 */

#define X_SCALING_RULES_SIZE  50

  static const SPH_ScaleRule  X_SCALING_Rules[X_SCALING_RULES_SIZE] =
  {
    { "DejaVu Sans", 12, "Regular Class", 'm', 950 },
    { "Verdana and Clones", 12, "Regular Class", 'a', 1100 },
    { "Verdana and Clones", 13, "Regular Class", 'a', 1050 },
    { "Arial", 11, "Regular Class", 'm', 975 },
    { "Arial", 12, "Regular Class", 'm', 1050 },
    /* Cyrillic small letter el */
    { "Arial", 13, "Regular Class", 0x43B, 950 },
    { "Arial", 13, "Regular Class", 'o', 950 },
    { "Arial", 13, "Regular Class", 'e', 950 },
    { "Arial", 14, "Regular Class", 'm', 950 },
    /* Cyrillic small letter el */
    { "Arial", 15, "Regular Class", 0x43B, 925 },
    { "Bitstream Vera Sans", 10, "Regular/Italic Class", 0, 1100 },
    { "Bitstream Vera Sans", 12, "Regular/Italic Class", 0, 1050 },
    { "Bitstream Vera Sans", 16, "Regular Class", 0, 1050 },
    { "Bitstream Vera Sans", 9, "Regular/Italic Class", 0, 1050 },
    { "DejaVu Sans", 12, "Regular Class", 'l', 975 },
    { "DejaVu Sans", 12, "Regular Class", 'i', 975 },
    { "DejaVu Sans", 12, "Regular Class", 'j', 975 },
    { "DejaVu Sans", 13, "Regular Class", 'l', 950 },
    { "DejaVu Sans", 13, "Regular Class", 'i', 950 },
    { "DejaVu Sans", 13, "Regular Class", 'j', 950 },
    { "DejaVu Sans", 10, "Regular/Italic Class", 0, 1100 },
    { "DejaVu Sans", 12, "Regular/Italic Class", 0, 1050 },
    { "Georgia", 10, "", 0, 1050 },
    { "Georgia", 11, "", 0, 1100 },
    { "Georgia", 12, "", 0, 1025 },
    { "Georgia", 13, "", 0, 1050 },
    { "Georgia", 16, "", 0, 1050 },
    { "Georgia", 17, "", 0, 1030 },
    { "Liberation Sans", 12, "Regular Class", 'm', 1100 },
    { "Lucida Grande", 11, "Regular Class", 'm', 1100 },
    { "Microsoft Sans Serif", 11, "Regular Class", 'm', 950 },
    { "Microsoft Sans Serif", 12, "Regular Class", 'm', 1050 },
    { "Segoe UI", 12, "Regular Class", 'H', 1050 },
    { "Segoe UI", 12, "Regular Class", 'm', 1050 },
    { "Segoe UI", 14, "Regular Class", 'm', 1050 },
    { "Tahoma", 11, "Regular Class", 'i', 975 },
    { "Tahoma", 11, "Regular Class", 'l', 975 },
    { "Tahoma", 11, "Regular Class", 'j', 900 },
    { "Tahoma", 11, "Regular Class", 'm', 918 },
    { "Verdana", 10, "Regular/Italic Class", 0, 1100 },
    { "Verdana", 12, "Regular Class", 'm', 975 },
    { "Verdana", 12, "Regular/Italic Class", 0, 1050 },
    { "Verdana", 13, "Regular/Italic Class", 'i', 950 },
    { "Verdana", 13, "Regular/Italic Class", 'j', 950 },
    { "Verdana", 13, "Regular/Italic Class", 'l', 950 },
    { "Verdana", 16, "Regular Class", 0, 1050 },
    { "Verdana", 9, "Regular/Italic Class", 0, 1050 },
    { "Times New Roman", 16, "Regular Class", 'm', 918 },
    { "Trebuchet MS", 11, "Regular Class", 'm', 800 },
    { "Trebuchet MS", 12, "Regular Class", 'm', 800 },
  };

#else

#define COMPATIBLE_WIDTHS_RULES_SIZE  1

  static const SPH_TweakRule  COMPATIBLE_WIDTHS_Rules
                              [COMPATIBLE_WIDTHS_RULES_SIZE] =
  {
    { "-", 0, "", 0 },
  };


#define X_SCALING_RULES_SIZE  1

  static const SPH_ScaleRule  X_SCALING_Rules
                              [X_SCALING_RULES_SIZE] =
  {
    { "-", 0, "", 0, 1000 },
  };

#endif /* FORCE_NATURAL_WIDTHS */


  FT_LOCAL_DEF( FT_Bool )
  is_member_of_family_class( const FT_String*  detected_font_name,
                             const FT_String*  rule_font_name )
  {
    FT_UInt  i, j;


    /* Does font name match rule family? */
    if ( strcmp( detected_font_name, rule_font_name ) == 0 )
      return TRUE;

    /* Is font name a wildcard ""? */
    if ( strcmp( rule_font_name, "" ) == 0 )
      return TRUE;

    /* Is font name contained in a class list? */
    for ( i = 0; i < FAMILY_CLASS_RULES_SIZE; i++ )
    {
      if ( strcmp( FAMILY_CLASS_Rules[i].name, rule_font_name ) == 0 )
      {
        for ( j = 0; j < SPH_MAX_CLASS_MEMBERS; j++ )
        {
          if ( strcmp( FAMILY_CLASS_Rules[i].member[j], "" ) == 0 )
            continue;
          if ( strcmp( FAMILY_CLASS_Rules[i].member[j],
                       detected_font_name ) == 0 )
            return TRUE;
        }
      }
    }

    return FALSE;
  }


  FT_LOCAL_DEF( FT_Bool )
  is_member_of_style_class( const FT_String*  detected_font_style,
                            const FT_String*  rule_font_style )
  {
    FT_UInt  i, j;


    /* Does font style match rule style? */
    if ( strcmp( detected_font_style, rule_font_style ) == 0 )
      return TRUE;

    /* Is font style a wildcard ""? */
    if ( strcmp( rule_font_style, "" ) == 0 )
      return TRUE;

    /* Is font style contained in a class list? */
    for ( i = 0; i < STYLE_CLASS_RULES_SIZE; i++ )
    {
      if ( strcmp( STYLE_CLASS_Rules[i].name, rule_font_style ) == 0 )
      {
        for ( j = 0; j < SPH_MAX_CLASS_MEMBERS; j++ )
        {
          if ( strcmp( STYLE_CLASS_Rules[i].member[j], "" ) == 0 )
            continue;
          if ( strcmp( STYLE_CLASS_Rules[i].member[j],
                       detected_font_style ) == 0 )
            return TRUE;
        }
      }
    }

    return FALSE;
  }


  FT_LOCAL_DEF( FT_Bool )
  sph_test_tweak( TT_Face               face,
                  const FT_String*      family,
                  FT_UInt               ppem,
                  const FT_String*      style,
                  FT_UInt               glyph_index,
                  const SPH_TweakRule*  rule,
                  FT_UInt               num_rules )
  {
    FT_UInt  i;


    /* rule checks may be able to be optimized further */
    for ( i = 0; i < num_rules; i++ )
    {
      if ( family                                                   &&
           ( is_member_of_family_class ( family, rule[i].family ) ) )
        if ( rule[i].ppem == 0    ||
             rule[i].ppem == ppem )
          if ( style                                             &&
               is_member_of_style_class ( style, rule[i].style ) )
            if ( rule[i].glyph == 0                                ||
                 FT_Get_Char_Index( (FT_Face)face,
                                    rule[i].glyph ) == glyph_index )
        return TRUE;
    }

    return FALSE;
  }


  static FT_UInt
  scale_test_tweak( TT_Face               face,
                    const FT_String*      family,
                    FT_UInt               ppem,
                    const FT_String*      style,
                    FT_UInt               glyph_index,
                    const SPH_ScaleRule*  rule,
                    FT_UInt               num_rules )
  {
    FT_UInt  i;


    /* rule checks may be able to be optimized further */
    for ( i = 0; i < num_rules; i++ )
    {
      if ( family                                                   &&
           ( is_member_of_family_class ( family, rule[i].family ) ) )
        if ( rule[i].ppem == 0    ||
             rule[i].ppem == ppem )
          if ( style                                            &&
               is_member_of_style_class( style, rule[i].style ) )
            if ( rule[i].glyph == 0                                ||
                 FT_Get_Char_Index( (FT_Face)face,
                                    rule[i].glyph ) == glyph_index )
        return rule[i].scale;
    }

    return 1000;
  }


  FT_LOCAL_DEF( FT_UInt )
  sph_test_tweak_x_scaling( TT_Face           face,
                            const FT_String*  family,
                            FT_UInt           ppem,
                            const FT_String*  style,
                            FT_UInt           glyph_index )
  {
    return scale_test_tweak( face, family, ppem, style, glyph_index,
                             X_SCALING_Rules, X_SCALING_RULES_SIZE );
  }


#define TWEAK_RULES( x )                                       \
  if ( sph_test_tweak( face, family, ppem, style, glyph_index, \
                       x##_Rules, x##_RULES_SIZE ) )           \
    loader->exec->sph_tweak_flags |= SPH_TWEAK_##x;

#define TWEAK_RULES_EXCEPTIONS( x )                                        \
  if ( sph_test_tweak( face, family, ppem, style, glyph_index,             \
                       x##_Rules_Exceptions, x##_RULES_EXCEPTIONS_SIZE ) ) \
    loader->exec->sph_tweak_flags &= ~SPH_TWEAK_##x;


  FT_LOCAL_DEF( void )
  sph_set_tweaks( TT_Loader  loader,
                  FT_UInt    glyph_index )
  {
    TT_Face     face   = (TT_Face)loader->face;
    FT_String*  family = face->root.family_name;
    int         ppem   = loader->size->metrics.x_ppem;
    FT_String*  style  = face->root.style_name;


    /* don't apply rules if style isn't set */
    if ( !face->root.style_name )
      return;

#ifdef SPH_DEBUG_MORE_VERBOSE
    printf( "%s,%d,%s,%c=%d ",
            family, ppem, style, glyph_index, glyph_index );
#endif

    TWEAK_RULES( PIXEL_HINTING );

    if ( loader->exec->sph_tweak_flags & SPH_TWEAK_PIXEL_HINTING )
    {
      loader->exec->ignore_x_mode = FALSE;
      return;
    }

    TWEAK_RULES( ALLOW_X_DMOVE );
    TWEAK_RULES( ALWAYS_DO_DELTAP );
    TWEAK_RULES( ALWAYS_SKIP_DELTAP );
    TWEAK_RULES( DEEMBOLDEN );
    TWEAK_RULES( DO_SHPIX );
    TWEAK_RULES( EMBOLDEN );
    TWEAK_RULES( MIAP_HACK );
    TWEAK_RULES( NORMAL_ROUND );
    TWEAK_RULES( NO_ALIGNRP_AFTER_IUP );
    TWEAK_RULES( NO_CALL_AFTER_IUP );
    TWEAK_RULES( NO_DELTAP_AFTER_IUP );
    TWEAK_RULES( RASTERIZER_35 );
    TWEAK_RULES( SKIP_IUP );

    TWEAK_RULES( SKIP_OFFPIXEL_Y_MOVES );
    TWEAK_RULES_EXCEPTIONS( SKIP_OFFPIXEL_Y_MOVES );

    TWEAK_RULES( SKIP_NONPIXEL_Y_MOVES_DELTAP );

    TWEAK_RULES( SKIP_NONPIXEL_Y_MOVES );
    TWEAK_RULES_EXCEPTIONS( SKIP_NONPIXEL_Y_MOVES );

    TWEAK_RULES( ROUND_NONPIXEL_Y_MOVES );
    TWEAK_RULES_EXCEPTIONS( ROUND_NONPIXEL_Y_MOVES );

    if ( loader->exec->sph_tweak_flags & SPH_TWEAK_RASTERIZER_35 )
    {
      if ( loader->exec->rasterizer_version != TT_INTERPRETER_VERSION_35 )
      {
        loader->exec->rasterizer_version = TT_INTERPRETER_VERSION_35;
        loader->exec->size->cvt_ready    = FALSE;

        tt_size_ready_bytecode(
          loader->exec->size,
          FT_BOOL( loader->load_flags & FT_LOAD_PEDANTIC ) );
      }
      else
        loader->exec->rasterizer_version = TT_INTERPRETER_VERSION_35;
    }
    else
    {
      if ( loader->exec->rasterizer_version  !=
           SPH_OPTION_SET_RASTERIZER_VERSION )
      {
        loader->exec->rasterizer_version = SPH_OPTION_SET_RASTERIZER_VERSION;
        loader->exec->size->cvt_ready    = FALSE;

        tt_size_ready_bytecode(
          loader->exec->size,
          FT_BOOL( loader->load_flags & FT_LOAD_PEDANTIC ) );
      }
      else
        loader->exec->rasterizer_version = SPH_OPTION_SET_RASTERIZER_VERSION;
    }

    if ( IS_HINTED( loader->load_flags ) )
    {
      TWEAK_RULES( TIMES_NEW_ROMAN_HACK );
      TWEAK_RULES( COURIER_NEW_2_HACK );
    }

    if ( sph_test_tweak( face, family, ppem, style, glyph_index,
           COMPATIBILITY_MODE_Rules, COMPATIBILITY_MODE_RULES_SIZE ) )
      loader->exec->face->sph_compatibility_mode = TRUE;


    if ( IS_HINTED( loader->load_flags ) )
    {
      if ( sph_test_tweak( face, family, ppem, style, glyph_index,
             COMPATIBLE_WIDTHS_Rules, COMPATIBLE_WIDTHS_RULES_SIZE ) )
        loader->exec->compatible_widths |= TRUE;
    }
  }

#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */

  /* ANSI C doesn't like empty source files */
  typedef int  _tt_subpix_dummy;

#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */


/* END */
#endif

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
/***************************************************************************/
/*                                                                         */
/*  ttgxvar.c                                                              */
/*                                                                         */
/*    TrueType GX Font Variation loader                                    */
/*                                                                         */
/*  Copyright 2004-2013 by                                                 */
/*  David Turner, Robert Wilhelm, Werner Lemberg, and George Williams.     */
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
  /* Apple documents the `fvar', `gvar', `cvar', and `avar' tables at      */
  /*                                                                       */
  /*   http://developer.apple.com/fonts/TTRefMan/RM06/Chap6[fgca]var.html  */
  /*                                                                       */
  /* The documentation for `fvar' is inconsistent.  At one point it says   */
  /* that `countSizePairs' should be 3, at another point 2.  It should     */
  /* be 2.                                                                 */
  /*                                                                       */
  /* The documentation for `gvar' is not intelligible; `cvar' refers you   */
  /* to `gvar' and is thus also incomprehensible.                          */
  /*                                                                       */
  /* The documentation for `avar' appears correct, but Apple has no fonts  */
  /* with an `avar' table, so it is hard to test.                          */
  /*                                                                       */
  /* Many thanks to John Jenkins (at Apple) in figuring this out.          */
  /*                                                                       */
  /*                                                                       */
  /* Apple's `kern' table has some references to tuple indices, but as     */
  /* there is no indication where these indices are defined, nor how to    */
  /* interpolate the kerning values (different tuples have different       */
  /* classes) this issue is ignored.                                       */
  /*                                                                       */
  /*************************************************************************/


#include "ft2build.h"
#include FT_INTERNAL_DEBUG_H
#include FT_CONFIG_CONFIG_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_SFNT_H
#include FT_TRUETYPE_TAGS_H
#include FT_MULTIPLE_MASTERS_H

#include "ttpload.h"
#include "ttgxvar.h"

#include "tterrors.h"


#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT


#define FT_Stream_FTell( stream )  \
          (FT_ULong)( (stream)->cursor - (stream)->base )
#define FT_Stream_SeekSet( stream, off ) \
          ( (stream)->cursor = (stream)->base + (off) )


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttgxvar


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       Internal Routines                       *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* The macro ALL_POINTS is used in `ft_var_readpackedpoints'.  It        */
  /* indicates that there is a delta for every point without needing to    */
  /* enumerate all of them.                                                */
  /*                                                                       */

  /* ensure that value `0' has the same width as a pointer */
#define ALL_POINTS  (FT_UShort*)~(FT_PtrDist)0


#define GX_PT_POINTS_ARE_WORDS      0x80
#define GX_PT_POINT_RUN_COUNT_MASK  0x7F


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_var_readpackedpoints                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Read a set of points to which the following deltas will apply.     */
  /*    Points are packed with a run length encoding.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream    :: The data stream.                                      */
  /*                                                                       */
  /* <Output>                                                              */
  /*    point_cnt :: The number of points read.  A zero value means that   */
  /*                 all points in the glyph will be affected, without     */
  /*                 enumerating them individually.                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    An array of FT_UShort containing the affected points or the        */
  /*    special value ALL_POINTS.                                          */
  /*                                                                       */
  static FT_UShort*
  ft_var_readpackedpoints( FT_Stream  stream,
                           FT_UInt   *point_cnt )
  {
    FT_UShort *points = NULL;
    FT_Int     n;
    FT_Int     runcnt;
    FT_Int     i;
    FT_Int     j;
    FT_Int     first;
    FT_Memory  memory = stream->memory;
    FT_Error   error  = FT_Err_Ok;

    FT_UNUSED( error );


    *point_cnt = n = FT_GET_BYTE();
    if ( n == 0 )
      return ALL_POINTS;

    if ( n & GX_PT_POINTS_ARE_WORDS )
      n = FT_GET_BYTE() | ( ( n & GX_PT_POINT_RUN_COUNT_MASK ) << 8 );

    if ( FT_NEW_ARRAY( points, n ) )
      return NULL;

    i = 0;
    while ( i < n )
    {
      runcnt = FT_GET_BYTE();
      if ( runcnt & GX_PT_POINTS_ARE_WORDS )
      {
        runcnt = runcnt & GX_PT_POINT_RUN_COUNT_MASK;
        first  = points[i++] = FT_GET_USHORT();

        if ( runcnt < 1 || i + runcnt >= n )
          goto Exit;

        /* first point not included in runcount */
        for ( j = 0; j < runcnt; ++j )
          points[i++] = (FT_UShort)( first += FT_GET_USHORT() );
      }
      else
      {
        first = points[i++] = FT_GET_BYTE();

        if ( runcnt < 1 || i + runcnt >= n )
          goto Exit;

        for ( j = 0; j < runcnt; ++j )
          points[i++] = (FT_UShort)( first += FT_GET_BYTE() );
      }
    }

  Exit:
    return points;
  }


  enum
  {
    GX_DT_DELTAS_ARE_ZERO      = 0x80,
    GX_DT_DELTAS_ARE_WORDS     = 0x40,
    GX_DT_DELTA_RUN_COUNT_MASK = 0x3F
  };


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_var_readpackeddeltas                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Read a set of deltas.  These are packed slightly differently than  */
  /*    points.  In particular there is no overall count.                  */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream    :: The data stream.                                      */
  /*                                                                       */
  /*    delta_cnt :: The number of to be read.                             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    An array of FT_Short containing the deltas for the affected        */
  /*    points.  (This only gets the deltas for one dimension.  It will    */
  /*    generally be called twice, once for x, once for y.  When used in   */
  /*    cvt table, it will only be called once.)                           */
  /*                                                                       */
  static FT_Short*
  ft_var_readpackeddeltas( FT_Stream  stream,
                           FT_Offset  delta_cnt )
  {
    FT_Short  *deltas = NULL;
    FT_UInt    runcnt;
    FT_Offset  i;
    FT_UInt    j;
    FT_Memory  memory = stream->memory;
    FT_Error   error  = FT_Err_Ok;

    FT_UNUSED( error );


    if ( FT_NEW_ARRAY( deltas, delta_cnt ) )
      return NULL;

    i = 0;
    while ( i < delta_cnt )
    {
      runcnt = FT_GET_BYTE();
      if ( runcnt & GX_DT_DELTAS_ARE_ZERO )
      {
        /* runcnt zeroes get added */
        for ( j = 0;
              j <= ( runcnt & GX_DT_DELTA_RUN_COUNT_MASK ) && i < delta_cnt;
              ++j )
          deltas[i++] = 0;
      }
      else if ( runcnt & GX_DT_DELTAS_ARE_WORDS )
      {
        /* runcnt shorts from the stack */
        for ( j = 0;
              j <= ( runcnt & GX_DT_DELTA_RUN_COUNT_MASK ) && i < delta_cnt;
              ++j )
          deltas[i++] = FT_GET_SHORT();
      }
      else
      {
        /* runcnt signed bytes from the stack */
        for ( j = 0;
              j <= ( runcnt & GX_DT_DELTA_RUN_COUNT_MASK ) && i < delta_cnt;
              ++j )
          deltas[i++] = FT_GET_CHAR();
      }

      if ( j <= ( runcnt & GX_DT_DELTA_RUN_COUNT_MASK ) )
      {
        /* Bad format */
        FT_FREE( deltas );
        return NULL;
      }
    }

    return deltas;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_var_load_avar                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Parse the `avar' table if present.  It need not be, so we return   */
  /*    nothing.                                                           */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face :: The font face.                                             */
  /*                                                                       */
  static void
  ft_var_load_avar( TT_Face  face )
  {
    FT_Stream       stream = FT_FACE_STREAM(face);
    FT_Memory       memory = stream->memory;
    GX_Blend        blend  = face->blend;
    GX_AVarSegment  segment;
    FT_Error        error = FT_Err_Ok;
    FT_ULong        version;
    FT_Long         axisCount;
    FT_Int          i, j;
    FT_ULong        table_len;

    FT_UNUSED( error );


    blend->avar_checked = TRUE;
    if ( (error = face->goto_table( face, TTAG_avar, stream, &table_len )) != 0 )
      return;

    if ( FT_FRAME_ENTER( table_len ) )
      return;

    version   = FT_GET_LONG();
    axisCount = FT_GET_LONG();

    if ( version != 0x00010000L                       ||
         axisCount != (FT_Long)blend->mmvar->num_axis )
      goto Exit;

    if ( FT_NEW_ARRAY( blend->avar_segment, axisCount ) )
      goto Exit;

    segment = &blend->avar_segment[0];
    for ( i = 0; i < axisCount; ++i, ++segment )
    {
      segment->pairCount = FT_GET_USHORT();
      if ( FT_NEW_ARRAY( segment->correspondence, segment->pairCount ) )
      {
        /* Failure.  Free everything we have done so far.  We must do */
        /* it right now since loading the `avar' table is optional.   */

        for ( j = i - 1; j >= 0; --j )
          FT_FREE( blend->avar_segment[j].correspondence );

        FT_FREE( blend->avar_segment );
        blend->avar_segment = NULL;
        goto Exit;
      }

      for ( j = 0; j < segment->pairCount; ++j )
      {
        segment->correspondence[j].fromCoord =
          FT_GET_SHORT() << 2;    /* convert to Fixed */
        segment->correspondence[j].toCoord =
          FT_GET_SHORT()<<2;    /* convert to Fixed */
      }
    }

  Exit:
    FT_FRAME_EXIT();
  }


  typedef struct  GX_GVar_Head_
  {
    FT_Long    version;
    FT_UShort  axisCount;
    FT_UShort  globalCoordCount;
    FT_ULong   offsetToCoord;
    FT_UShort  glyphCount;
    FT_UShort  flags;
    FT_ULong   offsetToData;

  } GX_GVar_Head;


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_var_load_gvar                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Parses the `gvar' table if present.  If `fvar' is there, `gvar'    */
  /*    had better be there too.                                           */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face :: The font face.                                             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  static FT_Error
  ft_var_load_gvar( TT_Face  face )
  {
    FT_Stream     stream = FT_FACE_STREAM(face);
    FT_Memory     memory = stream->memory;
    GX_Blend      blend  = face->blend;
    FT_Error      error;
    FT_UInt       i, j;
    FT_ULong      table_len;
    FT_ULong      gvar_start;
    FT_ULong      offsetToData;
    GX_GVar_Head  gvar_head;

    static const FT_Frame_Field  gvar_fields[] =
    {

#undef  FT_STRUCTURE
#define FT_STRUCTURE  GX_GVar_Head

      FT_FRAME_START( 20 ),
        FT_FRAME_LONG  ( version ),
        FT_FRAME_USHORT( axisCount ),
        FT_FRAME_USHORT( globalCoordCount ),
        FT_FRAME_ULONG ( offsetToCoord ),
        FT_FRAME_USHORT( glyphCount ),
        FT_FRAME_USHORT( flags ),
        FT_FRAME_ULONG ( offsetToData ),
      FT_FRAME_END
    };

    if ( (error = face->goto_table( face, TTAG_gvar, stream, &table_len )) != 0 )
      goto Exit;

    gvar_start = FT_STREAM_POS( );
    if ( FT_STREAM_READ_FIELDS( gvar_fields, &gvar_head ) )
      goto Exit;

    blend->tuplecount  = gvar_head.globalCoordCount;
    blend->gv_glyphcnt = gvar_head.glyphCount;
    offsetToData       = gvar_start + gvar_head.offsetToData;

    if ( gvar_head.version   != (FT_Long)0x00010000L              ||
         gvar_head.axisCount != (FT_UShort)blend->mmvar->num_axis )
    {
      error = FT_THROW( Invalid_Table );
      goto Exit;
    }

    if ( FT_NEW_ARRAY( blend->glyphoffsets, blend->gv_glyphcnt + 1 ) )
      goto Exit;

    if ( gvar_head.flags & 1 )
    {
      /* long offsets (one more offset than glyphs, to mark size of last) */
      if ( FT_FRAME_ENTER( ( blend->gv_glyphcnt + 1 ) * 4L ) )
        goto Exit;

      for ( i = 0; i <= blend->gv_glyphcnt; ++i )
        blend->glyphoffsets[i] = offsetToData + FT_GET_LONG();

      FT_FRAME_EXIT();
    }
    else
    {
      /* short offsets (one more offset than glyphs, to mark size of last) */
      if ( FT_FRAME_ENTER( ( blend->gv_glyphcnt + 1 ) * 2L ) )
        goto Exit;

      for ( i = 0; i <= blend->gv_glyphcnt; ++i )
        blend->glyphoffsets[i] = offsetToData + FT_GET_USHORT() * 2;
                                              /* XXX: Undocumented: `*2'! */

      FT_FRAME_EXIT();
    }

    if ( blend->tuplecount != 0 )
    {
      if ( FT_NEW_ARRAY( blend->tuplecoords,
                         gvar_head.axisCount * blend->tuplecount ) )
        goto Exit;

      if ( FT_STREAM_SEEK( gvar_start + gvar_head.offsetToCoord )       ||
           FT_FRAME_ENTER( blend->tuplecount * gvar_head.axisCount * 2L )                   )
        goto Exit;

      for ( i = 0; i < blend->tuplecount; ++i )
        for ( j = 0 ; j < (FT_UInt)gvar_head.axisCount; ++j )
          blend->tuplecoords[i * gvar_head.axisCount + j] =
            FT_GET_SHORT() << 2;                /* convert to FT_Fixed */

      FT_FRAME_EXIT();
    }

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    ft_var_apply_tuple                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Figure out whether a given tuple (design) applies to the current   */
  /*    blend, and if so, what is the scaling factor.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    blend           :: The current blend of the font.                  */
  /*                                                                       */
  /*    tupleIndex      :: A flag saying whether this is an intermediate   */
  /*                       tuple or not.                                   */
  /*                                                                       */
  /*    tuple_coords    :: The coordinates of the tuple in normalized axis */
  /*                       units.                                          */
  /*                                                                       */
  /*    im_start_coords :: The initial coordinates where this tuple starts */
  /*                       to apply (for intermediate coordinates).        */
  /*                                                                       */
  /*    im_end_coords   :: The final coordinates after which this tuple no */
  /*                       longer applies (for intermediate coordinates).  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    An FT_Fixed value containing the scaling factor.                   */
  /*                                                                       */
  static FT_Fixed
  ft_var_apply_tuple( GX_Blend   blend,
                      FT_UShort  tupleIndex,
                      FT_Fixed*  tuple_coords,
                      FT_Fixed*  im_start_coords,
                      FT_Fixed*  im_end_coords )
  {
    FT_UInt   i;
    FT_Fixed  apply = 0x10000L;


    for ( i = 0; i < blend->num_axis; ++i )
    {
      if ( tuple_coords[i] == 0 )
        /* It's not clear why (for intermediate tuples) we don't need     */
        /* to check against start/end -- the documentation says we don't. */
        /* Similarly, it's unclear why we don't need to scale along the   */
        /* axis.                                                          */
        continue;

      else if ( blend->normalizedcoords[i] == 0                           ||
                ( blend->normalizedcoords[i] < 0 && tuple_coords[i] > 0 ) ||
                ( blend->normalizedcoords[i] > 0 && tuple_coords[i] < 0 ) )
      {
        apply = 0;
        break;
      }

      else if ( !( tupleIndex & GX_TI_INTERMEDIATE_TUPLE ) )
        /* not an intermediate tuple */
        apply = FT_MulFix( apply,
                           blend->normalizedcoords[i] > 0
                             ? blend->normalizedcoords[i]
                             : -blend->normalizedcoords[i] );

      else if ( blend->normalizedcoords[i] <= im_start_coords[i] ||
                blend->normalizedcoords[i] >= im_end_coords[i]   )
      {
        apply = 0;
        break;
      }

      else if ( blend->normalizedcoords[i] < tuple_coords[i] )
        apply = FT_MulDiv( apply,
                           blend->normalizedcoords[i] - im_start_coords[i],
                           tuple_coords[i] - im_start_coords[i] );

      else
        apply = FT_MulDiv( apply,
                           im_end_coords[i] - blend->normalizedcoords[i],
                           im_end_coords[i] - tuple_coords[i] );
    }

    return apply;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****               MULTIPLE MASTERS SERVICE FUNCTIONS              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  typedef struct  GX_FVar_Head_
  {
    FT_Long    version;
    FT_UShort  offsetToData;
    FT_UShort  countSizePairs;
    FT_UShort  axisCount;
    FT_UShort  axisSize;
    FT_UShort  instanceCount;
    FT_UShort  instanceSize;

  } GX_FVar_Head;


  typedef struct  fvar_axis_
  {
    FT_ULong   axisTag;
    FT_ULong   minValue;
    FT_ULong   defaultValue;
    FT_ULong   maxValue;
    FT_UShort  flags;
    FT_UShort  nameID;

  } GX_FVar_Axis;


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Get_MM_Var                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Check that the font's `fvar' table is valid, parse it, and return  */
  /*    those data.                                                        */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: The font face.                                           */
  /*              TT_Get_MM_Var initializes the blend structure.           */
  /*                                                                       */
  /* <Output>                                                              */
  /*    master :: The `fvar' data (must be freed by caller).               */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Get_MM_Var( TT_Face      face,
                 FT_MM_Var*  *master )
  {
    FT_Stream            stream = face->root.stream;
    FT_Memory            memory = face->root.memory;
    FT_ULong             table_len;
    FT_Error             error  = FT_Err_Ok;
    FT_ULong             fvar_start;
    FT_Int               i, j;
    FT_MM_Var*           mmvar = NULL;
    FT_Fixed*            next_coords;
    FT_String*           next_name;
    FT_Var_Axis*         a;
    FT_Var_Named_Style*  ns;
    GX_FVar_Head         fvar_head;

    static const FT_Frame_Field  fvar_fields[] =
    {

#undef  FT_STRUCTURE
#define FT_STRUCTURE  GX_FVar_Head

      FT_FRAME_START( 16 ),
        FT_FRAME_LONG  ( version ),
        FT_FRAME_USHORT( offsetToData ),
        FT_FRAME_USHORT( countSizePairs ),
        FT_FRAME_USHORT( axisCount ),
        FT_FRAME_USHORT( axisSize ),
        FT_FRAME_USHORT( instanceCount ),
        FT_FRAME_USHORT( instanceSize ),
      FT_FRAME_END
    };

    static const FT_Frame_Field  fvaraxis_fields[] =
    {

#undef  FT_STRUCTURE
#define FT_STRUCTURE  GX_FVar_Axis

      FT_FRAME_START( 20 ),
        FT_FRAME_ULONG ( axisTag ),
        FT_FRAME_ULONG ( minValue ),
        FT_FRAME_ULONG ( defaultValue ),
        FT_FRAME_ULONG ( maxValue ),
        FT_FRAME_USHORT( flags ),
        FT_FRAME_USHORT( nameID ),
      FT_FRAME_END
    };


    if ( face->blend == NULL )
    {
      /* both `fvar' and `gvar' must be present */
      if ( (error = face->goto_table( face, TTAG_gvar,
                                      stream, &table_len )) != 0 )
        goto Exit;

      if ( (error = face->goto_table( face, TTAG_fvar,
                                      stream, &table_len )) != 0 )
        goto Exit;

      fvar_start = FT_STREAM_POS( );

      if ( FT_STREAM_READ_FIELDS( fvar_fields, &fvar_head ) )
        goto Exit;

      if ( fvar_head.version != (FT_Long)0x00010000L                      ||
           fvar_head.countSizePairs != 2                                  ||
           fvar_head.axisSize != 20                                       ||
           /* axisCount limit implied by 16-bit instanceSize */
           fvar_head.axisCount > 0x3FFE                                   ||
           fvar_head.instanceSize != 4 + 4 * fvar_head.axisCount          ||
           /* instanceCount limit implied by limited range of name IDs */
           fvar_head.instanceCount > 0x7EFF                               ||
           fvar_head.offsetToData + fvar_head.axisCount * 20U +
             fvar_head.instanceCount * fvar_head.instanceSize > table_len )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }

      if ( FT_NEW( face->blend ) )
        goto Exit;

      /* cannot overflow 32-bit arithmetic because of limits above */
      face->blend->mmvar_len =
        sizeof ( FT_MM_Var ) +
        fvar_head.axisCount * sizeof ( FT_Var_Axis ) +
        fvar_head.instanceCount * sizeof ( FT_Var_Named_Style ) +
        fvar_head.instanceCount * fvar_head.axisCount * sizeof ( FT_Fixed ) +
        5 * fvar_head.axisCount;

      if ( FT_ALLOC( mmvar, face->blend->mmvar_len ) )
        goto Exit;
      face->blend->mmvar = mmvar;

      mmvar->num_axis =
        fvar_head.axisCount;
      mmvar->num_designs =
        ~0U;                   /* meaningless in this context; each glyph */
                               /* may have a different number of designs  */
                               /* (or tuples, as called by Apple)         */
      mmvar->num_namedstyles =
        fvar_head.instanceCount;
      mmvar->axis =
        (FT_Var_Axis*)&(mmvar[1]);
      mmvar->namedstyle =
        (FT_Var_Named_Style*)&(mmvar->axis[fvar_head.axisCount]);

      next_coords =
        (FT_Fixed*)&(mmvar->namedstyle[fvar_head.instanceCount]);
      for ( i = 0; i < fvar_head.instanceCount; ++i )
      {
        mmvar->namedstyle[i].coords  = next_coords;
        next_coords                 += fvar_head.axisCount;
      }

      next_name = (FT_String*)next_coords;
      for ( i = 0; i < fvar_head.axisCount; ++i )
      {
        mmvar->axis[i].name  = next_name;
        next_name           += 5;
      }

      if ( FT_STREAM_SEEK( fvar_start + fvar_head.offsetToData ) )
        goto Exit;

      a = mmvar->axis;
      for ( i = 0; i < fvar_head.axisCount; ++i )
      {
        GX_FVar_Axis  axis_rec;


        if ( FT_STREAM_READ_FIELDS( fvaraxis_fields, &axis_rec ) )
          goto Exit;
        a->tag     = axis_rec.axisTag;
        a->minimum = axis_rec.minValue;     /* A Fixed */
        a->def     = axis_rec.defaultValue; /* A Fixed */
        a->maximum = axis_rec.maxValue;     /* A Fixed */
        a->strid   = axis_rec.nameID;

        a->name[0] = (FT_String)(   a->tag >> 24 );
        a->name[1] = (FT_String)( ( a->tag >> 16 ) & 0xFF );
        a->name[2] = (FT_String)( ( a->tag >>  8 ) & 0xFF );
        a->name[3] = (FT_String)( ( a->tag       ) & 0xFF );
        a->name[4] = 0;

        ++a;
      }

      ns = mmvar->namedstyle;
      for ( i = 0; i < fvar_head.instanceCount; ++i, ++ns )
      {
        if ( FT_FRAME_ENTER( 4L + 4L * fvar_head.axisCount ) )
          goto Exit;

        ns->strid       =    FT_GET_USHORT();
        (void) /* flags = */ FT_GET_USHORT();

        for ( j = 0; j < fvar_head.axisCount; ++j )
          ns->coords[j] = FT_GET_ULONG();     /* A Fixed */

        FT_FRAME_EXIT();
      }
    }

    if ( master != NULL )
    {
      FT_UInt  n;


      if ( FT_ALLOC( mmvar, face->blend->mmvar_len ) )
        goto Exit;
      FT_MEM_COPY( mmvar, face->blend->mmvar, face->blend->mmvar_len );

      mmvar->axis =
        (FT_Var_Axis*)&(mmvar[1]);
      mmvar->namedstyle =
        (FT_Var_Named_Style*)&(mmvar->axis[mmvar->num_axis]);
      next_coords =
        (FT_Fixed*)&(mmvar->namedstyle[mmvar->num_namedstyles]);

      for ( n = 0; n < mmvar->num_namedstyles; ++n )
      {
        mmvar->namedstyle[n].coords  = next_coords;
        next_coords                 += mmvar->num_axis;
      }

      a = mmvar->axis;
      next_name = (FT_String*)next_coords;
      for ( n = 0; n < mmvar->num_axis; ++n )
      {
        a->name = next_name;

        /* standard PostScript names for some standard apple tags */
        if ( a->tag == TTAG_wght )
          a->name = (char *)"Weight";
        else if ( a->tag == TTAG_wdth )
          a->name = (char *)"Width";
        else if ( a->tag == TTAG_opsz )
          a->name = (char *)"OpticalSize";
        else if ( a->tag == TTAG_slnt )
          a->name = (char *)"Slant";

        next_name += 5;
        ++a;
      }

      *master = mmvar;
    }

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Set_MM_Blend                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Set the blend (normalized) coordinates for this instance of the    */
  /*    font.  Check that the `gvar' table is reasonable and does some     */
  /*    initial preparation.                                               */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face       :: The font.                                            */
  /*                  Initialize the blend structure with `gvar' data.     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    num_coords :: Must be the axis count of the font.                  */
  /*                                                                       */
  /*    coords     :: An array of num_coords, each between [-1,1].         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Set_MM_Blend( TT_Face    face,
                   FT_UInt    num_coords,
                   FT_Fixed*  coords )
  {
    FT_Error    error = FT_Err_Ok;
    GX_Blend    blend;
    FT_MM_Var*  mmvar;
    FT_UInt     i;
    FT_Memory   memory = face->root.memory;

    enum
    {
      mcvt_retain,
      mcvt_modify,
      mcvt_load

    } manageCvt;


    face->doblend = FALSE;

    if ( face->blend == NULL )
    {
      if ( (error = TT_Get_MM_Var( face, NULL)) != 0 )
        goto Exit;
    }

    blend = face->blend;
    mmvar = blend->mmvar;

    if ( num_coords != mmvar->num_axis )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    for ( i = 0; i < num_coords; ++i )
      if ( coords[i] < -0x00010000L || coords[i] > 0x00010000L )
      {
        error = FT_THROW( Invalid_Argument );
        goto Exit;
      }

    if ( blend->glyphoffsets == NULL )
      if ( (error = ft_var_load_gvar( face )) != 0 )
        goto Exit;

    if ( blend->normalizedcoords == NULL )
    {
      if ( FT_NEW_ARRAY( blend->normalizedcoords, num_coords ) )
        goto Exit;

      manageCvt = mcvt_modify;

      /* If we have not set the blend coordinates before this, then the  */
      /* cvt table will still be what we read from the `cvt ' table and  */
      /* we don't need to reload it.  We may need to change it though... */
    }
    else
    {
      manageCvt = mcvt_retain;
      for ( i = 0; i < num_coords; ++i )
      {
        if ( blend->normalizedcoords[i] != coords[i] )
        {
          manageCvt = mcvt_load;
          break;
        }
      }

      /* If we don't change the blend coords then we don't need to do  */
      /* anything to the cvt table.  It will be correct.  Otherwise we */
      /* no longer have the original cvt (it was modified when we set  */
      /* the blend last time), so we must reload and then modify it.   */
    }

    blend->num_axis = num_coords;
    FT_MEM_COPY( blend->normalizedcoords,
                 coords,
                 num_coords * sizeof ( FT_Fixed ) );

    face->doblend = TRUE;

    if ( face->cvt != NULL )
    {
      switch ( manageCvt )
      {
      case mcvt_load:
        /* The cvt table has been loaded already; every time we change the */
        /* blend we may need to reload and remodify the cvt table.         */
        FT_FREE( face->cvt );
        face->cvt = NULL;

        tt_face_load_cvt( face, face->root.stream );
        break;

      case mcvt_modify:
        /* The original cvt table is in memory.  All we need to do is */
        /* apply the `cvar' table (if any).                           */
        tt_face_vary_cvt( face, face->root.stream );
        break;

      case mcvt_retain:
        /* The cvt table is correct for this set of coordinates. */
        break;
      }
    }

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Set_Var_Design                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Set the coordinates for the instance, measured in the user         */
  /*    coordinate system.  Parse the `avar' table (if present) to convert */
  /*    from user to normalized coordinates.                               */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face       :: The font face.                                       */
  /*                  Initialize the blend struct with `gvar' data.        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    num_coords :: This must be the axis count of the font.             */
  /*                                                                       */
  /*    coords     :: A coordinate array with `num_coords' elements.       */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Set_Var_Design( TT_Face    face,
                     FT_UInt    num_coords,
                     FT_Fixed*  coords )
  {
    FT_Error        error      = FT_Err_Ok;
    FT_Fixed*       normalized = NULL;
    GX_Blend        blend;
    FT_MM_Var*      mmvar;
    FT_UInt         i, j;
    FT_Var_Axis*    a;
    GX_AVarSegment  av;
    FT_Memory       memory = face->root.memory;


    if ( face->blend == NULL )
    {
      if ( (error = TT_Get_MM_Var( face, NULL )) != 0 )
        goto Exit;
    }

    blend = face->blend;
    mmvar = blend->mmvar;

    if ( num_coords != mmvar->num_axis )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    /* Axis normalization is a two stage process.  First we normalize */
    /* based on the [min,def,max] values for the axis to be [-1,0,1]. */
    /* Then, if there's an `avar' table, we renormalize this range.   */

    if ( FT_NEW_ARRAY( normalized, mmvar->num_axis ) )
      goto Exit;

    a = mmvar->axis;
    for ( i = 0; i < mmvar->num_axis; ++i, ++a )
    {
      if ( coords[i] > a->maximum || coords[i] < a->minimum )
      {
        error = FT_THROW( Invalid_Argument );
        goto Exit;
      }

      if ( coords[i] < a->def )
        normalized[i] = -FT_DivFix( coords[i] - a->def, a->minimum - a->def );
      else if ( a->maximum == a->def )
        normalized[i] = 0;
      else
        normalized[i] = FT_DivFix( coords[i] - a->def, a->maximum - a->def );
    }

    if ( !blend->avar_checked )
      ft_var_load_avar( face );

    if ( blend->avar_segment != NULL )
    {
      av = blend->avar_segment;
      for ( i = 0; i < mmvar->num_axis; ++i, ++av )
      {
        for ( j = 1; j < (FT_UInt)av->pairCount; ++j )
          if ( normalized[i] < av->correspondence[j].fromCoord )
          {
            normalized[i] =
              FT_MulDiv( normalized[i] - av->correspondence[j - 1].fromCoord,
                         av->correspondence[j].toCoord -
                           av->correspondence[j - 1].toCoord,
                         av->correspondence[j].fromCoord -
                           av->correspondence[j - 1].fromCoord ) +
              av->correspondence[j - 1].toCoord;
            break;
          }
      }
    }

    error = TT_Set_MM_Blend( face, num_coords, normalized );

  Exit:
    FT_FREE( normalized );
    return error;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GX VAR PARSING ROUTINES                   *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_vary_cvt                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Modify the loaded cvt table according to the `cvar' table and the  */
  /*    font's blend.                                                      */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  /*    Most errors are ignored.  It is perfectly valid not to have a      */
  /*    `cvar' table even if there is a `gvar' and `fvar' table.           */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_vary_cvt( TT_Face    face,
                    FT_Stream  stream )
  {
    FT_Error    error;
    FT_Memory   memory = stream->memory;
    FT_ULong    table_start;
    FT_ULong    table_len;
    FT_UInt     tupleCount;
    FT_ULong    offsetToData;
    FT_ULong    here;
    FT_UInt     i, j;
    FT_Fixed*   tuple_coords    = NULL;
    FT_Fixed*   im_start_coords = NULL;
    FT_Fixed*   im_end_coords   = NULL;
    GX_Blend    blend           = face->blend;
    FT_UInt     point_count;
    FT_UShort*  localpoints;
    FT_Short*   deltas;


    FT_TRACE2(( "CVAR " ));

    if ( blend == NULL )
    {
      FT_TRACE2(( "tt_face_vary_cvt: no blend specified\n" ));

      error = FT_Err_Ok;
      goto Exit;
    }

    if ( face->cvt == NULL )
    {
      FT_TRACE2(( "tt_face_vary_cvt: no `cvt ' table\n" ));

      error = FT_Err_Ok;
      goto Exit;
    }

    error = face->goto_table( face, TTAG_cvar, stream, &table_len );
    if ( error )
    {
      FT_TRACE2(( "is missing\n" ));

      error = FT_Err_Ok;
      goto Exit;
    }

    if ( FT_FRAME_ENTER( table_len ) )
    {
      error = FT_Err_Ok;
      goto Exit;
    }

    table_start = FT_Stream_FTell( stream );
    if ( FT_GET_LONG() != 0x00010000L )
    {
      FT_TRACE2(( "bad table version\n" ));

      error = FT_Err_Ok;
      goto FExit;
    }

    if ( FT_NEW_ARRAY( tuple_coords, blend->num_axis )    ||
         FT_NEW_ARRAY( im_start_coords, blend->num_axis ) ||
         FT_NEW_ARRAY( im_end_coords, blend->num_axis )   )
      goto FExit;

    tupleCount   = FT_GET_USHORT();
    offsetToData = table_start + FT_GET_USHORT();

    /* The documentation implies there are flags packed into the        */
    /* tuplecount, but John Jenkins says that shared points don't apply */
    /* to `cvar', and no other flags are defined.                       */

    for ( i = 0; i < ( tupleCount & 0xFFF ); ++i )
    {
      FT_UInt   tupleDataSize;
      FT_UInt   tupleIndex;
      FT_Fixed  apply;


      tupleDataSize = FT_GET_USHORT();
      tupleIndex    = FT_GET_USHORT();

      /* There is no provision here for a global tuple coordinate section, */
      /* so John says.  There are no tuple indices, just embedded tuples.  */

      if ( tupleIndex & GX_TI_EMBEDDED_TUPLE_COORD )
      {
        for ( j = 0; j < blend->num_axis; ++j )
          tuple_coords[j] = FT_GET_SHORT() << 2; /* convert from        */
                                                 /* short frac to fixed */
      }
      else
      {
        /* skip this tuple; it makes no sense */

        if ( tupleIndex & GX_TI_INTERMEDIATE_TUPLE )
          for ( j = 0; j < 2 * blend->num_axis; ++j )
            (void)FT_GET_SHORT();

        offsetToData += tupleDataSize;
        continue;
      }

      if ( tupleIndex & GX_TI_INTERMEDIATE_TUPLE )
      {
        for ( j = 0; j < blend->num_axis; ++j )
          im_start_coords[j] = FT_GET_SHORT() << 2;
        for ( j = 0; j < blend->num_axis; ++j )
          im_end_coords[j] = FT_GET_SHORT() << 2;
      }

      apply = ft_var_apply_tuple( blend,
                                  (FT_UShort)tupleIndex,
                                  tuple_coords,
                                  im_start_coords,
                                  im_end_coords );
      if ( /* tuple isn't active for our blend */
           apply == 0                                    ||
           /* global points not allowed,           */
           /* if they aren't local, makes no sense */
           !( tupleIndex & GX_TI_PRIVATE_POINT_NUMBERS ) )
      {
        offsetToData += tupleDataSize;
        continue;
      }

      here = FT_Stream_FTell( stream );

      FT_Stream_SeekSet( stream, offsetToData );

      localpoints = ft_var_readpackedpoints( stream, &point_count );
      deltas      = ft_var_readpackeddeltas( stream,
                                             point_count == 0 ? face->cvt_size
                                                              : point_count );
      if ( localpoints == NULL || deltas == NULL )
        /* failure, ignore it */;

      else if ( localpoints == ALL_POINTS )
      {
        /* this means that there are deltas for every entry in cvt */
        for ( j = 0; j < face->cvt_size; ++j )
          face->cvt[j] = (FT_Short)( face->cvt[j] +
                                     FT_MulFix( deltas[j], apply ) );
      }

      else
      {
        for ( j = 0; j < point_count; ++j )
        {
          int  pindex = localpoints[j];

          face->cvt[pindex] = (FT_Short)( face->cvt[pindex] +
                                          FT_MulFix( deltas[j], apply ) );
        }
      }

      if ( localpoints != ALL_POINTS )
        FT_FREE( localpoints );
      FT_FREE( deltas );

      offsetToData += tupleDataSize;

      FT_Stream_SeekSet( stream, here );
    }

  FExit:
    FT_FRAME_EXIT();

  Exit:
    FT_FREE( tuple_coords );
    FT_FREE( im_start_coords );
    FT_FREE( im_end_coords );

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    TT_Vary_Get_Glyph_Deltas                                           */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the appropriate deltas for the current glyph.                 */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face        :: A handle to the target face object.                 */
  /*                                                                       */
  /*    glyph_index :: The index of the glyph being modified.              */
  /*                                                                       */
  /*    n_points    :: The number of the points in the glyph, including    */
  /*                   phantom points.                                     */
  /*                                                                       */
  /* <Output>                                                              */
  /*    deltas      :: The array of points to change.                      */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  TT_Vary_Get_Glyph_Deltas( TT_Face      face,
                            FT_UInt      glyph_index,
                            FT_Vector*  *deltas,
                            FT_UInt      n_points )
  {
    FT_Stream   stream = face->root.stream;
    FT_Memory   memory = stream->memory;
    GX_Blend    blend  = face->blend;
    FT_Vector*  delta_xy = NULL;

    FT_Error    error;
    FT_ULong    glyph_start;
    FT_UInt     tupleCount;
    FT_ULong    offsetToData;
    FT_ULong    here;
    FT_UInt     i, j;
    FT_Fixed*   tuple_coords    = NULL;
    FT_Fixed*   im_start_coords = NULL;
    FT_Fixed*   im_end_coords   = NULL;
    FT_UInt     point_count, spoint_count = 0;
    FT_UShort*  sharedpoints = NULL;
    FT_UShort*  localpoints  = NULL;
    FT_UShort*  points;
    FT_Short    *deltas_x, *deltas_y;


    if ( !face->doblend || blend == NULL )
      return FT_THROW( Invalid_Argument );

    /* to be freed by the caller */
    if ( FT_NEW_ARRAY( delta_xy, n_points ) )
      goto Exit;
    *deltas = delta_xy;

    if ( glyph_index >= blend->gv_glyphcnt      ||
         blend->glyphoffsets[glyph_index] ==
           blend->glyphoffsets[glyph_index + 1] )
      return FT_Err_Ok;               /* no variation data for this glyph */

    if ( FT_STREAM_SEEK( blend->glyphoffsets[glyph_index] )   ||
         FT_FRAME_ENTER( blend->glyphoffsets[glyph_index + 1] -
                           blend->glyphoffsets[glyph_index] ) )
      goto Fail1;

    glyph_start = FT_Stream_FTell( stream );

    /* each set of glyph variation data is formatted similarly to `cvar' */
    /* (except we get shared points and global tuples)                   */

    if ( FT_NEW_ARRAY( tuple_coords, blend->num_axis )    ||
         FT_NEW_ARRAY( im_start_coords, blend->num_axis ) ||
         FT_NEW_ARRAY( im_end_coords, blend->num_axis )   )
      goto Fail2;

    tupleCount   = FT_GET_USHORT();
    offsetToData = glyph_start + FT_GET_USHORT();

    if ( tupleCount & GX_TC_TUPLES_SHARE_POINT_NUMBERS )
    {
      here = FT_Stream_FTell( stream );

      FT_Stream_SeekSet( stream, offsetToData );

      sharedpoints = ft_var_readpackedpoints( stream, &spoint_count );
      offsetToData = FT_Stream_FTell( stream );

      FT_Stream_SeekSet( stream, here );
    }

    for ( i = 0; i < ( tupleCount & GX_TC_TUPLE_COUNT_MASK ); ++i )
    {
      FT_UInt   tupleDataSize;
      FT_UInt   tupleIndex;
      FT_Fixed  apply;


      tupleDataSize = FT_GET_USHORT();
      tupleIndex    = FT_GET_USHORT();

      if ( tupleIndex & GX_TI_EMBEDDED_TUPLE_COORD )
      {
        for ( j = 0; j < blend->num_axis; ++j )
          tuple_coords[j] = FT_GET_SHORT() << 2;  /* convert from        */
                                                  /* short frac to fixed */
      }
      else if ( ( tupleIndex & GX_TI_TUPLE_INDEX_MASK ) >= blend->tuplecount )
      {
        error = FT_THROW( Invalid_Table );
        goto Fail3;
      }
      else
      {
        FT_MEM_COPY(
          tuple_coords,
          &blend->tuplecoords[(tupleIndex & 0xFFF) * blend->num_axis],
          blend->num_axis * sizeof ( FT_Fixed ) );
      }

      if ( tupleIndex & GX_TI_INTERMEDIATE_TUPLE )
      {
        for ( j = 0; j < blend->num_axis; ++j )
          im_start_coords[j] = FT_GET_SHORT() << 2;
        for ( j = 0; j < blend->num_axis; ++j )
          im_end_coords[j] = FT_GET_SHORT() << 2;
      }

      apply = ft_var_apply_tuple( blend,
                                  (FT_UShort)tupleIndex,
                                  tuple_coords,
                                  im_start_coords,
                                  im_end_coords );

      if ( apply == 0 )              /* tuple isn't active for our blend */
      {
        offsetToData += tupleDataSize;
        continue;
      }

      here = FT_Stream_FTell( stream );

      if ( tupleIndex & GX_TI_PRIVATE_POINT_NUMBERS )
      {
        FT_Stream_SeekSet( stream, offsetToData );

        localpoints = ft_var_readpackedpoints( stream, &point_count );
        points      = localpoints;
      }
      else
      {
        points      = sharedpoints;
        point_count = spoint_count;
      }

      deltas_x = ft_var_readpackeddeltas( stream,
                                          point_count == 0 ? n_points
                                                           : point_count );
      deltas_y = ft_var_readpackeddeltas( stream,
                                          point_count == 0 ? n_points
                                                           : point_count );

      if ( points == NULL || deltas_y == NULL || deltas_x == NULL )
        ; /* failure, ignore it */

      else if ( points == ALL_POINTS )
      {
        /* this means that there are deltas for every point in the glyph */
        for ( j = 0; j < n_points; ++j )
        {
          delta_xy[j].x += FT_MulFix( deltas_x[j], apply );
          delta_xy[j].y += FT_MulFix( deltas_y[j], apply );
        }
      }

      else
      {
        for ( j = 0; j < point_count; ++j )
        {
          if ( localpoints[j] >= n_points )
            continue;

          delta_xy[localpoints[j]].x += FT_MulFix( deltas_x[j], apply );
          delta_xy[localpoints[j]].y += FT_MulFix( deltas_y[j], apply );
        }
      }

      if ( localpoints != ALL_POINTS )
        FT_FREE( localpoints );
      FT_FREE( deltas_x );
      FT_FREE( deltas_y );

      offsetToData += tupleDataSize;

      FT_Stream_SeekSet( stream, here );
    }

  Fail3:
    FT_FREE( tuple_coords );
    FT_FREE( im_start_coords );
    FT_FREE( im_end_coords );

  Fail2:
    FT_FRAME_EXIT();

  Fail1:
    if ( error )
    {
      FT_FREE( delta_xy );
      *deltas = NULL;
    }

  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_done_blend                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Frees the blend internal data structure.                           */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_done_blend( FT_Memory  memory,
                 GX_Blend   blend )
  {
    if ( blend != NULL )
    {
      FT_UInt  i;


      FT_FREE( blend->normalizedcoords );
      FT_FREE( blend->mmvar );

      if ( blend->avar_segment != NULL )
      {
        for ( i = 0; i < blend->num_axis; ++i )
          FT_FREE( blend->avar_segment[i].correspondence );
        FT_FREE( blend->avar_segment );
      }

      FT_FREE( blend->tuplecoords );
      FT_FREE( blend->glyphoffsets );
      FT_FREE( blend );
    }
  }

#endif /* TT_CONFIG_OPTION_GX_VAR_SUPPORT */


/* END */
#endif


/* END */
