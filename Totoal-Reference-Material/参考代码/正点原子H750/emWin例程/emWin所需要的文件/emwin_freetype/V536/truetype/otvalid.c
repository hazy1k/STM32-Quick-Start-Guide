/***************************************************************************/
/*                                                                         */
/*  otvalid.c                                                              */
/*                                                                         */
/*    FreeType validator for OpenType tables (body only).                  */
/*                                                                         */
/*  Copyright 2004, 2007 by                                                */
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
/*  otvbase.c                                                              */
/*                                                                         */
/*    OpenType BASE table validation (body).                               */
/*                                                                         */
/*  Copyright 2004, 2007 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvalid.h"
#include "otvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvbase


  static void
  otv_BaseCoord_validate( FT_Bytes       table,
                          OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   BaseCoordFormat;


    OTV_NAME_ENTER( "BaseCoord" );

    OTV_LIMIT_CHECK( 4 );
    BaseCoordFormat = FT_NEXT_USHORT( p );
    p += 2;     /* skip Coordinate */

    OTV_TRACE(( " (format %d)\n", BaseCoordFormat ));

    switch ( BaseCoordFormat )
    {
    case 1:     /* BaseCoordFormat1 */
      break;

    case 2:     /* BaseCoordFormat2 */
      OTV_LIMIT_CHECK( 4 );   /* ReferenceGlyph, BaseCoordPoint */
      break;

    case 3:     /* BaseCoordFormat3 */
      OTV_LIMIT_CHECK( 2 );
      /* DeviceTable */
      otv_Device_validate( table + FT_NEXT_USHORT( p ), valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  static void
  otv_BaseTagList_validate( FT_Bytes       table,
                            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   BaseTagCount;


    OTV_NAME_ENTER( "BaseTagList" );

    OTV_LIMIT_CHECK( 2 );

    BaseTagCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (BaseTagCount = %d)\n", BaseTagCount ));

    OTV_LIMIT_CHECK( BaseTagCount * 4 );          /* BaselineTag */

    OTV_EXIT;
  }


  static void
  otv_BaseValues_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   BaseCoordCount;


    OTV_NAME_ENTER( "BaseValues" );

    OTV_LIMIT_CHECK( 4 );

    p             += 2;                     /* skip DefaultIndex */
    BaseCoordCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (BaseCoordCount = %d)\n", BaseCoordCount ));

    OTV_LIMIT_CHECK( BaseCoordCount * 2 );

    /* BaseCoord */
    for ( ; BaseCoordCount > 0; BaseCoordCount-- )
      otv_BaseCoord_validate( table + FT_NEXT_USHORT( p ), valid );

    OTV_EXIT;
  }


  static void
  otv_MinMax_validate( FT_Bytes       table,
                       OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   table_size;
    FT_UInt   FeatMinMaxCount;

    OTV_OPTIONAL_TABLE( MinCoord );
    OTV_OPTIONAL_TABLE( MaxCoord );


    OTV_NAME_ENTER( "MinMax" );

    OTV_LIMIT_CHECK( 6 );

    OTV_OPTIONAL_OFFSET( MinCoord );
    OTV_OPTIONAL_OFFSET( MaxCoord );
    FeatMinMaxCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (FeatMinMaxCount = %d)\n", FeatMinMaxCount ));

    table_size = FeatMinMaxCount * 8 + 6;

    OTV_SIZE_CHECK( MinCoord );
    if ( MinCoord )
      otv_BaseCoord_validate( table + MinCoord, valid );

    OTV_SIZE_CHECK( MaxCoord );
    if ( MaxCoord )
      otv_BaseCoord_validate( table + MaxCoord, valid );

    OTV_LIMIT_CHECK( FeatMinMaxCount * 8 );

    /* FeatMinMaxRecord */
    for ( ; FeatMinMaxCount > 0; FeatMinMaxCount-- )
    {
      p += 4;                           /* skip FeatureTableTag */

      OTV_OPTIONAL_OFFSET( MinCoord );
      OTV_OPTIONAL_OFFSET( MaxCoord );

      OTV_SIZE_CHECK( MinCoord );
      if ( MinCoord )
        otv_BaseCoord_validate( table + MinCoord, valid );

      OTV_SIZE_CHECK( MaxCoord );
      if ( MaxCoord )
        otv_BaseCoord_validate( table + MaxCoord, valid );
    }

    OTV_EXIT;
  }


  static void
  otv_BaseScript_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   table_size;
    FT_UInt   BaseLangSysCount;

    OTV_OPTIONAL_TABLE( BaseValues    );
    OTV_OPTIONAL_TABLE( DefaultMinMax );


    OTV_NAME_ENTER( "BaseScript" );

    OTV_LIMIT_CHECK( 6 );
    OTV_OPTIONAL_OFFSET( BaseValues    );
    OTV_OPTIONAL_OFFSET( DefaultMinMax );
    BaseLangSysCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (BaseLangSysCount = %d)\n", BaseLangSysCount ));

    table_size = BaseLangSysCount * 6 + 6;

    OTV_SIZE_CHECK( BaseValues );
    if ( BaseValues )
      otv_BaseValues_validate( table + BaseValues, valid );

    OTV_SIZE_CHECK( DefaultMinMax );
    if ( DefaultMinMax )
      otv_MinMax_validate( table + DefaultMinMax, valid );

    OTV_LIMIT_CHECK( BaseLangSysCount * 6 );

    /* BaseLangSysRecord */
    for ( ; BaseLangSysCount > 0; BaseLangSysCount-- )
    {
      p += 4;       /* skip BaseLangSysTag */

      otv_MinMax_validate( table + FT_NEXT_USHORT( p ), valid );
    }

    OTV_EXIT;
  }


  static void
  otv_BaseScriptList_validate( FT_Bytes       table,
                               OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   BaseScriptCount;


    OTV_NAME_ENTER( "BaseScriptList" );

    OTV_LIMIT_CHECK( 2 );
    BaseScriptCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (BaseScriptCount = %d)\n", BaseScriptCount ));

    OTV_LIMIT_CHECK( BaseScriptCount * 6 );

    /* BaseScriptRecord */
    for ( ; BaseScriptCount > 0; BaseScriptCount-- )
    {
      p += 4;       /* skip BaseScriptTag */

      /* BaseScript */
      otv_BaseScript_validate( table + FT_NEXT_USHORT( p ), valid );
    }

    OTV_EXIT;
  }


  static void
  otv_Axis_validate( FT_Bytes       table,
                     OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   table_size;

    OTV_OPTIONAL_TABLE( BaseTagList );


    OTV_NAME_ENTER( "Axis" );

    OTV_LIMIT_CHECK( 4 );
    OTV_OPTIONAL_OFFSET( BaseTagList );

    table_size = 4;

    OTV_SIZE_CHECK( BaseTagList );
    if ( BaseTagList )
      otv_BaseTagList_validate( table + BaseTagList, valid );

    /* BaseScriptList */
    otv_BaseScriptList_validate( table + FT_NEXT_USHORT( p ), valid );

    OTV_EXIT;
  }


  FT_LOCAL_DEF( void )
  otv_BASE_validate( FT_Bytes      table,
                     FT_Validator  ftvalid )
  {
    OTV_ValidatorRec  validrec;
    OTV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_UInt           table_size;

    OTV_OPTIONAL_TABLE( HorizAxis );
    OTV_OPTIONAL_TABLE( VertAxis  );


    valid->root = ftvalid;

    FT_TRACE3(( "validating BASE table\n" ));
    OTV_INIT;

    OTV_LIMIT_CHECK( 6 );

    if ( FT_NEXT_ULONG( p ) != 0x10000UL )      /* Version */
      FT_INVALID_FORMAT;

    table_size = 6;

    OTV_OPTIONAL_OFFSET( HorizAxis );
    OTV_SIZE_CHECK( HorizAxis );
    if ( HorizAxis )
      otv_Axis_validate( table + HorizAxis, valid );

    OTV_OPTIONAL_OFFSET( VertAxis );
    OTV_SIZE_CHECK( VertAxis );
    if ( VertAxis )
      otv_Axis_validate( table + VertAxis, valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvcommn.c                                                             */
/*                                                                         */
/*    OpenType common tables validation (body).                            */
/*                                                                         */
/*  Copyright 2004, 2005, 2006, 2007 by                                    */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvcommon


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       COVERAGE TABLE                          *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  otv_Coverage_validate( FT_Bytes       table,
                         OTV_Validator  valid,
                         FT_Int         expected_count )
  {
    FT_Bytes  p = table;
    FT_UInt   CoverageFormat;
    FT_UInt   total = 0;


    OTV_NAME_ENTER( "Coverage" );

    OTV_LIMIT_CHECK( 4 );
    CoverageFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", CoverageFormat ));

    switch ( CoverageFormat )
    {
    case 1:     /* CoverageFormat1 */
      {
        FT_UInt  GlyphCount;
        FT_UInt  i;


        GlyphCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (GlyphCount = %d)\n", GlyphCount ));

        OTV_LIMIT_CHECK( GlyphCount * 2 );        /* GlyphArray */

        for ( i = 0; i < GlyphCount; ++i )
        {
          FT_UInt  gid;


          gid = FT_NEXT_USHORT( p );
          if ( gid >= valid->glyph_count )
            FT_INVALID_GLYPH_ID;
        }

        total = GlyphCount;
      }
      break;

    case 2:     /* CoverageFormat2 */
      {
        FT_UInt  n, RangeCount;
        FT_UInt  Start, End, StartCoverageIndex, last = 0;


        RangeCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (RangeCount = %d)\n", RangeCount ));

        OTV_LIMIT_CHECK( RangeCount * 6 );

        /* RangeRecord */
        for ( n = 0; n < RangeCount; n++ )
        {
          Start              = FT_NEXT_USHORT( p );
          End                = FT_NEXT_USHORT( p );
          StartCoverageIndex = FT_NEXT_USHORT( p );

          if ( Start > End || StartCoverageIndex != total )
            FT_INVALID_DATA;

          if ( End >= valid->glyph_count )
            FT_INVALID_GLYPH_ID;

          if ( n > 0 && Start <= last )
            FT_INVALID_DATA;

          total += End - Start + 1;
          last   = End;
        }
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    /* Generally, a coverage table offset has an associated count field.  */
    /* The number of glyphs in the table should match this field.  If     */
    /* there is no associated count, a value of -1 tells us not to check. */
    if ( expected_count != -1 && (FT_UInt)expected_count != total )
      FT_INVALID_DATA;

    OTV_EXIT;
  }


  FT_LOCAL_DEF( FT_UInt )
  otv_Coverage_get_first( FT_Bytes  table )
  {
    FT_Bytes  p = table;


    p += 4;     /* skip CoverageFormat and Glyph/RangeCount */

    return FT_NEXT_USHORT( p );
  }


  FT_LOCAL_DEF( FT_UInt )
  otv_Coverage_get_last( FT_Bytes  table )
  {
    FT_Bytes  p = table;
    FT_UInt   CoverageFormat = FT_NEXT_USHORT( p );
    FT_UInt   count          = FT_NEXT_USHORT( p );     /* Glyph/RangeCount */
    FT_UInt   result = 0;


    switch ( CoverageFormat )
    {
    case 1:
      p += ( count - 1 ) * 2;
      result = FT_NEXT_USHORT( p );
      break;

    case 2:
      p += ( count - 1 ) * 6 + 2;
      result = FT_NEXT_USHORT( p );
      break;

    default:
      ;
    }

    return result;
  }


  FT_LOCAL_DEF( FT_UInt )
  otv_Coverage_get_count( FT_Bytes  table )
  {
    FT_Bytes  p              = table;
    FT_UInt   CoverageFormat = FT_NEXT_USHORT( p );
    FT_UInt   count          = FT_NEXT_USHORT( p );     /* Glyph/RangeCount */
    FT_UInt   result         = 0;


    switch ( CoverageFormat )
    {
    case 1:
      return count;

    case 2:
      {
        FT_UInt  Start, End;


        for ( ; count > 0; count-- )
        {
          Start = FT_NEXT_USHORT( p );
          End   = FT_NEXT_USHORT( p );
          p    += 2;                    /* skip StartCoverageIndex */

          result += End - Start + 1;
        }
      }
      break;

    default:
      ;
    }

    return result;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                   CLASS DEFINITION TABLE                      *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  otv_ClassDef_validate( FT_Bytes       table,
                         OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   ClassFormat;


    OTV_NAME_ENTER( "ClassDef" );

    OTV_LIMIT_CHECK( 4 );
    ClassFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", ClassFormat ));

    switch ( ClassFormat )
    {
    case 1:     /* ClassDefFormat1 */
      {
        FT_UInt  StartGlyph;
        FT_UInt  GlyphCount;


        OTV_LIMIT_CHECK( 4 );

        StartGlyph = FT_NEXT_USHORT( p );
        GlyphCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (GlyphCount = %d)\n", GlyphCount ));

        OTV_LIMIT_CHECK( GlyphCount * 2 );    /* ClassValueArray */

        if ( StartGlyph + GlyphCount - 1 >= valid->glyph_count )
          FT_INVALID_GLYPH_ID;
      }
      break;

    case 2:     /* ClassDefFormat2 */
      {
        FT_UInt  n, ClassRangeCount;
        FT_UInt  Start, End, last = 0;


        ClassRangeCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (ClassRangeCount = %d)\n", ClassRangeCount ));

        OTV_LIMIT_CHECK( ClassRangeCount * 6 );

        /* ClassRangeRecord */
        for ( n = 0; n < ClassRangeCount; n++ )
        {
          Start = FT_NEXT_USHORT( p );
          End   = FT_NEXT_USHORT( p );
          p    += 2;                        /* skip Class */

          if ( Start > End || ( n > 0 && Start <= last ) )
            FT_INVALID_DATA;

          if ( End >= valid->glyph_count )
            FT_INVALID_GLYPH_ID;

          last = End;
        }
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    /* no need to check glyph indices used as input to class definition   */
    /* tables since even invalid glyph indices return a meaningful result */

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      DEVICE TABLE                             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  otv_Device_validate( FT_Bytes       table,
                       OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   StartSize, EndSize, DeltaFormat, count;


    OTV_NAME_ENTER( "Device" );

    OTV_LIMIT_CHECK( 8 );
    StartSize   = FT_NEXT_USHORT( p );
    EndSize     = FT_NEXT_USHORT( p );
    DeltaFormat = FT_NEXT_USHORT( p );

    if ( DeltaFormat < 1 || DeltaFormat > 3 )
      FT_INVALID_FORMAT;

    if ( EndSize < StartSize )
      FT_INVALID_DATA;

    count = EndSize - StartSize + 1;
    OTV_LIMIT_CHECK( ( 1 << DeltaFormat ) * count / 8 );  /* DeltaValue */

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         LOOKUPS                               *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* uses valid->type_count */
  /* uses valid->type_funcs */

  FT_LOCAL_DEF( void )
  otv_Lookup_validate( FT_Bytes       table,
                       OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_UInt            LookupType, SubTableCount;
    OTV_Validate_Func  validate;


    OTV_NAME_ENTER( "Lookup" );

    OTV_LIMIT_CHECK( 6 );
    LookupType    = FT_NEXT_USHORT( p );
    p            += 2;                      /* skip LookupFlag */
    SubTableCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (type %d)\n", LookupType ));

    if ( LookupType == 0 || LookupType > valid->type_count )
      FT_INVALID_DATA;

    validate = valid->type_funcs[LookupType - 1];

    OTV_TRACE(( " (SubTableCount = %d)\n", SubTableCount ));

    OTV_LIMIT_CHECK( SubTableCount * 2 );

    /* SubTable */
    for ( ; SubTableCount > 0; SubTableCount-- )
      validate( table + FT_NEXT_USHORT( p ), valid );

    OTV_EXIT;
  }


  /* uses valid->lookup_count */

  FT_LOCAL_DEF( void )
  otv_LookupList_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   LookupCount;


    OTV_NAME_ENTER( "LookupList" );

    OTV_LIMIT_CHECK( 2 );
    LookupCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (LookupCount = %d)\n", LookupCount ));

    OTV_LIMIT_CHECK( LookupCount * 2 );

    valid->lookup_count = LookupCount;

    /* Lookup */
    for ( ; LookupCount > 0; LookupCount-- )
      otv_Lookup_validate( table + FT_NEXT_USHORT( p ), valid );

    OTV_EXIT;
  }


  static FT_UInt
  otv_LookupList_get_count( FT_Bytes  table )
  {
    return FT_NEXT_USHORT( table );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                        FEATURES                               *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* uses valid->lookup_count */

  FT_LOCAL_DEF( void )
  otv_Feature_validate( FT_Bytes       table,
                        OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   LookupCount;


    OTV_NAME_ENTER( "Feature" );

    OTV_LIMIT_CHECK( 4 );
    p           += 2;                   /* skip FeatureParams (unused) */
    LookupCount  = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (LookupCount = %d)\n", LookupCount ));

    OTV_LIMIT_CHECK( LookupCount * 2 );

    /* LookupListIndex */
    for ( ; LookupCount > 0; LookupCount-- )
      if ( FT_NEXT_USHORT( p ) >= valid->lookup_count )
        FT_INVALID_DATA;

    OTV_EXIT;
  }


  static FT_UInt
  otv_Feature_get_count( FT_Bytes  table )
  {
    return FT_NEXT_USHORT( table );
  }


  /* sets valid->lookup_count */

  FT_LOCAL_DEF( void )
  otv_FeatureList_validate( FT_Bytes       table,
                            FT_Bytes       lookups,
                            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   FeatureCount;


    OTV_NAME_ENTER( "FeatureList" );

    OTV_LIMIT_CHECK( 2 );
    FeatureCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (FeatureCount = %d)\n", FeatureCount ));

    OTV_LIMIT_CHECK( FeatureCount * 2 );

    valid->lookup_count = otv_LookupList_get_count( lookups );

    /* FeatureRecord */
    for ( ; FeatureCount > 0; FeatureCount-- )
    {
      p += 4;       /* skip FeatureTag */

      /* Feature */
      otv_Feature_validate( table + FT_NEXT_USHORT( p ), valid );
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       LANGUAGE SYSTEM                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /* uses valid->extra1 (number of features) */

  FT_LOCAL_DEF( void )
  otv_LangSys_validate( FT_Bytes       table,
                        OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   ReqFeatureIndex;
    FT_UInt   FeatureCount;


    OTV_NAME_ENTER( "LangSys" );

    OTV_LIMIT_CHECK( 6 );
    p              += 2;                    /* skip LookupOrder (unused) */
    ReqFeatureIndex = FT_NEXT_USHORT( p );
    FeatureCount    = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (ReqFeatureIndex = %d)\n", ReqFeatureIndex ));
    OTV_TRACE(( " (FeatureCount = %d)\n",    FeatureCount    ));

    if ( ReqFeatureIndex != 0xFFFFU && ReqFeatureIndex >= valid->extra1 )
      FT_INVALID_DATA;

    OTV_LIMIT_CHECK( FeatureCount * 2 );

    /* FeatureIndex */
    for ( ; FeatureCount > 0; FeatureCount-- )
      if ( FT_NEXT_USHORT( p ) >= valid->extra1 )
        FT_INVALID_DATA;

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                           SCRIPTS                             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  otv_Script_validate( FT_Bytes       table,
                       OTV_Validator  valid )
  {
    FT_UInt   DefaultLangSys, LangSysCount;
    FT_Bytes  p = table;


    OTV_NAME_ENTER( "Script" );

    OTV_LIMIT_CHECK( 4 );
    DefaultLangSys = FT_NEXT_USHORT( p );
    LangSysCount   = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (LangSysCount = %d)\n", LangSysCount ));

    if ( DefaultLangSys != 0 )
      otv_LangSys_validate( table + DefaultLangSys, valid );

    OTV_LIMIT_CHECK( LangSysCount * 6 );

    /* LangSysRecord */
    for ( ; LangSysCount > 0; LangSysCount-- )
    {
      p += 4;       /* skip LangSysTag */

      /* LangSys */
      otv_LangSys_validate( table + FT_NEXT_USHORT( p ), valid );
    }

    OTV_EXIT;
  }


  /* sets valid->extra1 (number of features) */

  FT_LOCAL_DEF( void )
  otv_ScriptList_validate( FT_Bytes       table,
                           FT_Bytes       features,
                           OTV_Validator  valid )
  {
    FT_UInt   ScriptCount;
    FT_Bytes  p = table;


    OTV_NAME_ENTER( "ScriptList" );

    OTV_LIMIT_CHECK( 2 );
    ScriptCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (ScriptCount = %d)\n", ScriptCount ));

    OTV_LIMIT_CHECK( ScriptCount * 6 );

    valid->extra1 = otv_Feature_get_count( features );

    /* ScriptRecord */
    for ( ; ScriptCount > 0; ScriptCount-- )
    {
      p += 4;       /* skip ScriptTag */

      otv_Script_validate( table + FT_NEXT_USHORT( p ), valid ); /* Script */
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*
     u:   uint16
     ux:  unit16 [x]

     s:   struct
     sx:  struct [x]
     sxy: struct [x], using external y count

     x:   uint16 x

     C:   Coverage

     O:   Offset
     On:  Offset (NULL)
     Ox:  Offset [x]
     Onx: Offset (NULL) [x]
  */

  FT_LOCAL_DEF( void )
  otv_x_Ox( FT_Bytes       table,
            OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_UInt            Count;
    OTV_Validate_Func  func;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 2 );
    Count = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count = %d)\n", Count ));

    OTV_LIMIT_CHECK( Count * 2 );

    valid->nesting_level++;
    func = valid->func[valid->nesting_level];

    for ( ; Count > 0; Count-- )
      func( table + FT_NEXT_USHORT( p ), valid );

    valid->nesting_level--;

    OTV_EXIT;
  }


  FT_LOCAL_DEF( void )
  otv_u_C_x_Ox( FT_Bytes       table,
                OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_UInt            Count, Coverage;
    OTV_Validate_Func  func;


    OTV_ENTER;

    p += 2;     /* skip Format */

    OTV_LIMIT_CHECK( 4 );
    Coverage = FT_NEXT_USHORT( p );
    Count    = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count = %d)\n", Count ));

    otv_Coverage_validate( table + Coverage, valid, Count );

    OTV_LIMIT_CHECK( Count * 2 );

    valid->nesting_level++;
    func = valid->func[valid->nesting_level];

    for ( ; Count > 0; Count-- )
      func( table + FT_NEXT_USHORT( p ), valid );

    valid->nesting_level--;

    OTV_EXIT;
  }


  /* uses valid->extra1 (if > 0: array value limit) */

  FT_LOCAL_DEF( void )
  otv_x_ux( FT_Bytes       table,
            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   Count;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 2 );
    Count = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count = %d)\n", Count ));

    OTV_LIMIT_CHECK( Count * 2 );

    if ( valid->extra1 )
    {
      for ( ; Count > 0; Count-- )
        if ( FT_NEXT_USHORT( p ) >= valid->extra1 )
          FT_INVALID_DATA;
    }

    OTV_EXIT;
  }


  /* `ux' in the function's name is not really correct since only x-1 */
  /* elements are tested                                              */

  /* uses valid->extra1 (array value limit) */

  FT_LOCAL_DEF( void )
  otv_x_y_ux_sy( FT_Bytes       table,
                 OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   Count1, Count2;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 4 );
    Count1 = FT_NEXT_USHORT( p );
    Count2 = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count1 = %d)\n", Count1 ));
    OTV_TRACE(( " (Count2 = %d)\n", Count2 ));

    if ( Count1 == 0 )
      FT_INVALID_DATA;

    OTV_LIMIT_CHECK( ( Count1 - 1 ) * 2 + Count2 * 4 );
    p += ( Count1 - 1 ) * 2;

    for ( ; Count2 > 0; Count2-- )
    {
      if ( FT_NEXT_USHORT( p ) >= Count1 )
        FT_INVALID_DATA;

      if ( FT_NEXT_USHORT( p ) >= valid->extra1 )
        FT_INVALID_DATA;
    }

    OTV_EXIT;
  }


  /* `uy' in the function's name is not really correct since only y-1 */
  /* elements are tested                                              */

  /* uses valid->extra1 (array value limit) */

  FT_LOCAL_DEF( void )
  otv_x_ux_y_uy_z_uz_p_sp( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   BacktrackCount, InputCount, LookaheadCount;
    FT_UInt   Count;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 2 );
    BacktrackCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (BacktrackCount = %d)\n", BacktrackCount ));

    OTV_LIMIT_CHECK( BacktrackCount * 2 + 2 );
    p += BacktrackCount * 2;

    InputCount = FT_NEXT_USHORT( p );
    if ( InputCount == 0 )
      FT_INVALID_DATA;

    OTV_TRACE(( " (InputCount = %d)\n", InputCount ));

    OTV_LIMIT_CHECK( InputCount * 2 );
    p += ( InputCount - 1 ) * 2;

    LookaheadCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (LookaheadCount = %d)\n", LookaheadCount ));

    OTV_LIMIT_CHECK( LookaheadCount * 2 + 2 );
    p += LookaheadCount * 2;

    Count = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count = %d)\n", Count ));

    OTV_LIMIT_CHECK( Count * 4 );

    for ( ; Count > 0; Count-- )
    {
      if ( FT_NEXT_USHORT( p ) >= InputCount )
        FT_INVALID_DATA;

      if ( FT_NEXT_USHORT( p ) >= valid->extra1 )
        FT_INVALID_DATA;
    }

    OTV_EXIT;
  }


  /* sets valid->extra1 (valid->lookup_count) */

  FT_LOCAL_DEF( void )
  otv_u_O_O_x_Onx( FT_Bytes       table,
                   OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_UInt            Coverage, ClassDef, ClassSetCount;
    OTV_Validate_Func  func;


    OTV_ENTER;

    p += 2;     /* skip Format */

    OTV_LIMIT_CHECK( 6 );
    Coverage      = FT_NEXT_USHORT( p );
    ClassDef      = FT_NEXT_USHORT( p );
    ClassSetCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (ClassSetCount = %d)\n", ClassSetCount ));

    otv_Coverage_validate( table + Coverage, valid, -1 );
    otv_ClassDef_validate( table + ClassDef, valid );

    OTV_LIMIT_CHECK( ClassSetCount * 2 );

    valid->nesting_level++;
    func          = valid->func[valid->nesting_level];
    valid->extra1 = valid->lookup_count;

    for ( ; ClassSetCount > 0; ClassSetCount-- )
    {
      FT_UInt  offset = FT_NEXT_USHORT( p );


      if ( offset )
        func( table + offset, valid );
    }

    valid->nesting_level--;

    OTV_EXIT;
  }


  /* uses valid->lookup_count */

  FT_LOCAL_DEF( void )
  otv_u_x_y_Ox_sy( FT_Bytes       table,
                   OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   GlyphCount, Count, count1;


    OTV_ENTER;

    p += 2;     /* skip Format */

    OTV_LIMIT_CHECK( 4 );
    GlyphCount = FT_NEXT_USHORT( p );
    Count      = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (GlyphCount = %d)\n", GlyphCount ));
    OTV_TRACE(( " (Count = %d)\n",      Count      ));

    OTV_LIMIT_CHECK( GlyphCount * 2 + Count * 4 );

    for ( count1 = GlyphCount; count1 > 0; count1-- )
      otv_Coverage_validate( table + FT_NEXT_USHORT( p ), valid, -1 );

    for ( ; Count > 0; Count-- )
    {
      if ( FT_NEXT_USHORT( p ) >= GlyphCount )
        FT_INVALID_DATA;

      if ( FT_NEXT_USHORT( p ) >= valid->lookup_count )
        FT_INVALID_DATA;
    }

    OTV_EXIT;
  }


  /* sets valid->extra1 (valid->lookup_count)    */

  FT_LOCAL_DEF( void )
  otv_u_O_O_O_O_x_Onx( FT_Bytes       table,
                       OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_UInt            Coverage;
    FT_UInt            BacktrackClassDef, InputClassDef, LookaheadClassDef;
    FT_UInt            ChainClassSetCount;
    OTV_Validate_Func  func;


    OTV_ENTER;

    p += 2;     /* skip Format */

    OTV_LIMIT_CHECK( 10 );
    Coverage           = FT_NEXT_USHORT( p );
    BacktrackClassDef  = FT_NEXT_USHORT( p );
    InputClassDef      = FT_NEXT_USHORT( p );
    LookaheadClassDef  = FT_NEXT_USHORT( p );
    ChainClassSetCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (ChainClassSetCount = %d)\n", ChainClassSetCount ));

    otv_Coverage_validate( table + Coverage, valid, -1 );

    otv_ClassDef_validate( table + BacktrackClassDef,  valid );
    otv_ClassDef_validate( table + InputClassDef, valid );
    otv_ClassDef_validate( table + LookaheadClassDef, valid );

    OTV_LIMIT_CHECK( ChainClassSetCount * 2 );

    valid->nesting_level++;
    func          = valid->func[valid->nesting_level];
    valid->extra1 = valid->lookup_count;

    for ( ; ChainClassSetCount > 0; ChainClassSetCount-- )
    {
      FT_UInt  offset = FT_NEXT_USHORT( p );


      if ( offset )
        func( table + offset, valid );
    }

    valid->nesting_level--;

    OTV_EXIT;
  }


  /* uses valid->lookup_count */

  FT_LOCAL_DEF( void )
  otv_u_x_Ox_y_Oy_z_Oz_p_sp( FT_Bytes       table,
                             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   BacktrackGlyphCount, InputGlyphCount, LookaheadGlyphCount;
    FT_UInt   count1, count2;


    OTV_ENTER;

    p += 2;     /* skip Format */

    OTV_LIMIT_CHECK( 2 );
    BacktrackGlyphCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (BacktrackGlyphCount = %d)\n", BacktrackGlyphCount ));

    OTV_LIMIT_CHECK( BacktrackGlyphCount * 2 + 2 );

    for ( ; BacktrackGlyphCount > 0; BacktrackGlyphCount-- )
      otv_Coverage_validate( table + FT_NEXT_USHORT( p ), valid, -1 );

    InputGlyphCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (InputGlyphCount = %d)\n", InputGlyphCount ));

    OTV_LIMIT_CHECK( InputGlyphCount * 2 + 2 );

    for ( count1 = InputGlyphCount; count1 > 0; count1-- )
      otv_Coverage_validate( table + FT_NEXT_USHORT( p ), valid, -1 );

    LookaheadGlyphCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (LookaheadGlyphCount = %d)\n", LookaheadGlyphCount ));

    OTV_LIMIT_CHECK( LookaheadGlyphCount * 2 + 2 );

    for ( ; LookaheadGlyphCount > 0; LookaheadGlyphCount-- )
      otv_Coverage_validate( table + FT_NEXT_USHORT( p ), valid, -1 );

    count2 = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count = %d)\n", count2 ));

    OTV_LIMIT_CHECK( count2 * 4 );

    for ( ; count2 > 0; count2-- )
    {
      if ( FT_NEXT_USHORT( p ) >= InputGlyphCount )
        FT_INVALID_DATA;

      if ( FT_NEXT_USHORT( p ) >= valid->lookup_count )
        FT_INVALID_DATA;
    }

    OTV_EXIT;
  }


  FT_LOCAL_DEF( FT_UInt )
  otv_GSUBGPOS_get_Lookup_count( FT_Bytes  table )
  {
    FT_Bytes  p = table + 8;


    return otv_LookupList_get_count( table + FT_NEXT_USHORT( p ) );
  }


  FT_LOCAL_DEF( FT_UInt )
  otv_GSUBGPOS_have_MarkAttachmentType_flag( FT_Bytes  table )
  {
    FT_Bytes  p, lookup;
    FT_UInt   count;


    if ( !table )
      return 0;

    /* LookupList */
    p      = table + 8;
    table += FT_NEXT_USHORT( p );

    /* LookupCount */
    p     = table;
    count = FT_NEXT_USHORT( p );

    for ( ; count > 0; count-- )
    {
      FT_Bytes  oldp;


      /* Lookup */
      lookup = table + FT_NEXT_USHORT( p );

      oldp = p;

      /* LookupFlag */
      p = lookup + 2;
      if ( FT_NEXT_USHORT( p ) & 0xFF00U )
        return 1;

      p = oldp;
    }

    return 0;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvgdef.c                                                              */
/*                                                                         */
/*    OpenType GDEF table validation (body).                               */
/*                                                                         */
/*  Copyright 2004, 2005, 2007 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvalid.h"
#include "otvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvgdef


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define AttachListFunc    otv_O_x_Ox
#define LigCaretListFunc  otv_O_x_Ox

  /* sets valid->extra1 (0)           */

  static void
  otv_O_x_Ox( FT_Bytes       table,
              OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_Bytes           Coverage;
    FT_UInt            GlyphCount;
    OTV_Validate_Func  func;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 4 );
    Coverage   = table + FT_NEXT_USHORT( p );
    GlyphCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (GlyphCount = %d)\n", GlyphCount ));

    otv_Coverage_validate( Coverage, valid, GlyphCount );
    if ( GlyphCount != otv_Coverage_get_count( Coverage ) )
      FT_INVALID_DATA;

    OTV_LIMIT_CHECK( GlyphCount * 2 );

    valid->nesting_level++;
    func          = valid->func[valid->nesting_level];
    valid->extra1 = 0;

    for ( ; GlyphCount > 0; GlyphCount-- )
      func( table + FT_NEXT_USHORT( p ), valid );

    valid->nesting_level--;

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       LIGATURE CARETS                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define CaretValueFunc  otv_CaretValue_validate

  static void
  otv_CaretValue_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   CaretValueFormat;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 4 );

    CaretValueFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format = %d)\n", CaretValueFormat ));

    switch ( CaretValueFormat )
    {
    case 1:     /* CaretValueFormat1 */
      /* skip Coordinate, no test */
      break;

    case 2:     /* CaretValueFormat2 */
      /* skip CaretValuePoint, no test */
      break;

    case 3:     /* CaretValueFormat3 */
      p += 2;   /* skip Coordinate */

      OTV_LIMIT_CHECK( 2 );

      /* DeviceTable */
      otv_Device_validate( table + FT_NEXT_USHORT( p ), valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         GDEF TABLE                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->glyph_count */

  FT_LOCAL_DEF( void )
  otv_GDEF_validate( FT_Bytes      table,
                     FT_Bytes      gsub,
                     FT_Bytes      gpos,
                     FT_UInt       glyph_count,
                     FT_Validator  ftvalid )
  {
    OTV_ValidatorRec  validrec;
    OTV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_UInt           table_size;
    FT_Bool           need_MarkAttachClassDef;

    OTV_OPTIONAL_TABLE( GlyphClassDef );
    OTV_OPTIONAL_TABLE( AttachListOffset );
    OTV_OPTIONAL_TABLE( LigCaretListOffset );
    OTV_OPTIONAL_TABLE( MarkAttachClassDef );


    valid->root = ftvalid;

    FT_TRACE3(( "validating GDEF table\n" ));
    OTV_INIT;

    OTV_LIMIT_CHECK( 12 );

    if ( FT_NEXT_ULONG( p ) != 0x10000UL )          /* Version */
      FT_INVALID_FORMAT;

    /* MarkAttachClassDef has been added to the OpenType */
    /* specification without increasing GDEF's version,  */
    /* so we use this ugly hack to find out whether the  */
    /* table is needed actually.                         */

    need_MarkAttachClassDef = FT_BOOL(
      otv_GSUBGPOS_have_MarkAttachmentType_flag( gsub ) ||
      otv_GSUBGPOS_have_MarkAttachmentType_flag( gpos ) );

    if ( need_MarkAttachClassDef )
      table_size = 12;              /* OpenType >= 1.2 */
    else
      table_size = 10;              /* OpenType < 1.2  */

    valid->glyph_count = glyph_count;

    OTV_OPTIONAL_OFFSET( GlyphClassDef );
    OTV_SIZE_CHECK( GlyphClassDef );
    if ( GlyphClassDef )
      otv_ClassDef_validate( table + GlyphClassDef, valid );

    OTV_OPTIONAL_OFFSET( AttachListOffset );
    OTV_SIZE_CHECK( AttachListOffset );
    if ( AttachListOffset )
    {
      OTV_NEST2( AttachList, AttachPoint );
      OTV_RUN( table + AttachListOffset, valid );
    }

    OTV_OPTIONAL_OFFSET( LigCaretListOffset );
    OTV_SIZE_CHECK( LigCaretListOffset );
    if ( LigCaretListOffset )
    {
      OTV_NEST3( LigCaretList, LigGlyph, CaretValue );
      OTV_RUN( table + LigCaretListOffset, valid );
    }

    if ( need_MarkAttachClassDef )
    {
      OTV_OPTIONAL_OFFSET( MarkAttachClassDef );
      OTV_SIZE_CHECK( MarkAttachClassDef );
      if ( MarkAttachClassDef )
        otv_ClassDef_validate( table + MarkAttachClassDef, valid );
    }

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvgpos.c                                                              */
/*                                                                         */
/*    OpenType GPOS table validation (body).                               */
/*                                                                         */
/*  Copyright 2002, 2004, 2005, 2006, 2007, 2008 by                        */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvalid.h"
#include "otvcommn.h"
#include "otvgpos.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvgpos


  static void
  otv_Anchor_validate( FT_Bytes       table,
                       OTV_Validator  valid );

  static void
  otv_MarkArray_validate( FT_Bytes       table,
                          OTV_Validator  valid );


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define BaseArrayFunc       otv_x_sxy
#define LigatureAttachFunc  otv_x_sxy
#define Mark2ArrayFunc      otv_x_sxy

  /* uses valid->extra1 (counter)                             */
  /* uses valid->extra2 (boolean to handle NULL anchor field) */

  static void
  otv_x_sxy( FT_Bytes       table,
             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   Count, count1, table_size;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 2 );

    Count = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (Count = %d)\n", Count ));

    OTV_LIMIT_CHECK( Count * valid->extra1 * 2 );

    table_size = Count * valid->extra1 * 2 + 2;

    for ( ; Count > 0; Count-- )
      for ( count1 = valid->extra1; count1 > 0; count1-- )
      {
        OTV_OPTIONAL_TABLE( anchor_offset );


        OTV_OPTIONAL_OFFSET( anchor_offset );

        if ( valid->extra2 )
        {
          OTV_SIZE_CHECK( anchor_offset );
          if ( anchor_offset )
            otv_Anchor_validate( table + anchor_offset, valid );
        }
        else
          otv_Anchor_validate( table + anchor_offset, valid );
      }

    OTV_EXIT;
  }


#define MarkBasePosFormat1Func  otv_u_O_O_u_O_O
#define MarkLigPosFormat1Func   otv_u_O_O_u_O_O
#define MarkMarkPosFormat1Func  otv_u_O_O_u_O_O

  /* sets valid->extra1 (class count) */

  static void
  otv_u_O_O_u_O_O( FT_Bytes       table,
                   OTV_Validator  valid )
  {
    FT_Bytes           p = table;
    FT_UInt            Coverage1, Coverage2, ClassCount;
    FT_UInt            Array1, Array2;
    OTV_Validate_Func  func;


    OTV_ENTER;

    p += 2;     /* skip PosFormat */

    OTV_LIMIT_CHECK( 10 );
    Coverage1  = FT_NEXT_USHORT( p );
    Coverage2  = FT_NEXT_USHORT( p );
    ClassCount = FT_NEXT_USHORT( p );
    Array1     = FT_NEXT_USHORT( p );
    Array2     = FT_NEXT_USHORT( p );

    otv_Coverage_validate( table + Coverage1, valid, -1 );
    otv_Coverage_validate( table + Coverage2, valid, -1 );

    otv_MarkArray_validate( table + Array1, valid );

    valid->nesting_level++;
    func          = valid->func[valid->nesting_level];
    valid->extra1 = ClassCount;

    func( table + Array2, valid );

    valid->nesting_level--;

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                        VALUE RECORDS                          *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static FT_UInt
  otv_value_length( FT_UInt  format )
  {
    FT_UInt  count;


    count = ( ( format & 0xAA ) >> 1 ) + ( format & 0x55 );
    count = ( ( count  & 0xCC ) >> 2 ) + ( count  & 0x33 );
    count = ( ( count  & 0xF0 ) >> 4 ) + ( count  & 0x0F );

    return count * 2;
  }


  /* uses valid->extra3 (pointer to base table) */

  static void
  otv_ValueRecord_validate( FT_Bytes       table,
                            FT_UInt        format,
                            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   count;

#ifdef FT_DEBUG_LEVEL_TRACE
    FT_Int    loop;
    FT_ULong  res = 0;


    OTV_NAME_ENTER( "ValueRecord" );

    /* display `format' in dual representation */
    for ( loop = 7; loop >= 0; loop-- )
    {
      res <<= 4;
      res  += ( format >> loop ) & 1;
    }

    OTV_TRACE(( " (format 0b%08lx)\n", res ));
#endif

    if ( format >= 0x100 )
      FT_INVALID_FORMAT;

    for ( count = 4; count > 0; count-- )
    {
      if ( format & 1 )
      {
        /* XPlacement, YPlacement, XAdvance, YAdvance */
        OTV_LIMIT_CHECK( 2 );
        p += 2;
      }

      format >>= 1;
    }

    for ( count = 4; count > 0; count-- )
    {
      if ( format & 1 )
      {
        FT_PtrDist  table_size;

        OTV_OPTIONAL_TABLE( device );


        /* XPlaDevice, YPlaDevice, XAdvDevice, YAdvDevice */
        OTV_LIMIT_CHECK( 2 );
        OTV_OPTIONAL_OFFSET( device );

        /* XXX: this value is usually too small, especially if the current */
        /* ValueRecord is part of an array -- getting the correct table    */
        /* size is probably not worth the trouble                          */

        table_size = p - valid->extra3;

        OTV_SIZE_CHECK( device );
        if ( device )
          otv_Device_validate( valid->extra3 + device, valid );
      }
      format >>= 1;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                           ANCHORS                             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_Anchor_validate( FT_Bytes       table,
                       OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   AnchorFormat;


    OTV_NAME_ENTER( "Anchor");

    OTV_LIMIT_CHECK( 6 );
    AnchorFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", AnchorFormat ));

    p += 4;     /* skip XCoordinate and YCoordinate */

    switch ( AnchorFormat )
    {
    case 1:
      break;

    case 2:
      OTV_LIMIT_CHECK( 2 );  /* AnchorPoint */
      break;

    case 3:
      {
        FT_UInt   table_size;

        OTV_OPTIONAL_TABLE( XDeviceTable );
        OTV_OPTIONAL_TABLE( YDeviceTable );


        OTV_LIMIT_CHECK( 4 );
        OTV_OPTIONAL_OFFSET( XDeviceTable );
        OTV_OPTIONAL_OFFSET( YDeviceTable );

        table_size = 6 + 4;

        OTV_SIZE_CHECK( XDeviceTable );
        if ( XDeviceTable )
          otv_Device_validate( table + XDeviceTable, valid );

        OTV_SIZE_CHECK( YDeviceTable );
        if ( YDeviceTable )
          otv_Device_validate( table + YDeviceTable, valid );
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         MARK ARRAYS                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_MarkArray_validate( FT_Bytes       table,
                          OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   MarkCount;


    OTV_NAME_ENTER( "MarkArray" );

    OTV_LIMIT_CHECK( 2 );
    MarkCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (MarkCount = %d)\n", MarkCount ));

    OTV_LIMIT_CHECK( MarkCount * 4 );

    /* MarkRecord */
    for ( ; MarkCount > 0; MarkCount-- )
    {
      p += 2;   /* skip Class */
      /* MarkAnchor */
      otv_Anchor_validate( table + FT_NEXT_USHORT( p ), valid );
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 1                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra3 (pointer to base table) */

  static void
  otv_SinglePos_validate( FT_Bytes       table,
                          OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "SinglePos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    valid->extra3 = table;

    switch ( PosFormat )
    {
    case 1:     /* SinglePosFormat1 */
      {
        FT_UInt  Coverage, ValueFormat;


        OTV_LIMIT_CHECK( 4 );
        Coverage    = FT_NEXT_USHORT( p );
        ValueFormat = FT_NEXT_USHORT( p );

        otv_Coverage_validate( table + Coverage, valid, -1 );
        otv_ValueRecord_validate( p, ValueFormat, valid ); /* Value */
      }
      break;

    case 2:     /* SinglePosFormat2 */
      {
        FT_UInt  Coverage, ValueFormat, ValueCount, len_value;


        OTV_LIMIT_CHECK( 6 );
        Coverage    = FT_NEXT_USHORT( p );
        ValueFormat = FT_NEXT_USHORT( p );
        ValueCount  = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (ValueCount = %d)\n", ValueCount ));

        len_value = otv_value_length( ValueFormat );

        otv_Coverage_validate( table + Coverage, valid, ValueCount );

        OTV_LIMIT_CHECK( ValueCount * len_value );

        /* Value */
        for ( ; ValueCount > 0; ValueCount-- )
        {
          otv_ValueRecord_validate( p, ValueFormat, valid );
          p += len_value;
        }
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 2                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_PairSet_validate( FT_Bytes       table,
                        FT_UInt        format1,
                        FT_UInt        format2,
                        OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   value_len1, value_len2, PairValueCount;


    OTV_NAME_ENTER( "PairSet" );

    OTV_LIMIT_CHECK( 2 );
    PairValueCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (PairValueCount = %d)\n", PairValueCount ));

    value_len1 = otv_value_length( format1 );
    value_len2 = otv_value_length( format2 );

    OTV_LIMIT_CHECK( PairValueCount * ( value_len1 + value_len2 + 2 ) );

    /* PairValueRecord */
    for ( ; PairValueCount > 0; PairValueCount-- )
    {
      p += 2;       /* skip SecondGlyph */

      if ( format1 )
        otv_ValueRecord_validate( p, format1, valid ); /* Value1 */
      p += value_len1;

      if ( format2 )
        otv_ValueRecord_validate( p, format2, valid ); /* Value2 */
      p += value_len2;
    }

    OTV_EXIT;
  }


  /* sets valid->extra3 (pointer to base table) */

  static void
  otv_PairPos_validate( FT_Bytes       table,
                        OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "PairPos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    valid->extra3 = table;

    switch ( PosFormat )
    {
    case 1:     /* PairPosFormat1 */
      {
        FT_UInt  Coverage, ValueFormat1, ValueFormat2, PairSetCount;


        OTV_LIMIT_CHECK( 8 );
        Coverage     = FT_NEXT_USHORT( p );
        ValueFormat1 = FT_NEXT_USHORT( p );
        ValueFormat2 = FT_NEXT_USHORT( p );
        PairSetCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (PairSetCount = %d)\n", PairSetCount ));

        otv_Coverage_validate( table + Coverage, valid, -1 );

        OTV_LIMIT_CHECK( PairSetCount * 2 );

        /* PairSetOffset */
        for ( ; PairSetCount > 0; PairSetCount-- )
          otv_PairSet_validate( table + FT_NEXT_USHORT( p ),
                                ValueFormat1, ValueFormat2, valid );
      }
      break;

    case 2:     /* PairPosFormat2 */
      {
        FT_UInt  Coverage, ValueFormat1, ValueFormat2, ClassDef1, ClassDef2;
        FT_UInt  ClassCount1, ClassCount2, len_value1, len_value2, count;


        OTV_LIMIT_CHECK( 14 );
        Coverage     = FT_NEXT_USHORT( p );
        ValueFormat1 = FT_NEXT_USHORT( p );
        ValueFormat2 = FT_NEXT_USHORT( p );
        ClassDef1    = FT_NEXT_USHORT( p );
        ClassDef2    = FT_NEXT_USHORT( p );
        ClassCount1  = FT_NEXT_USHORT( p );
        ClassCount2  = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (ClassCount1 = %d)\n", ClassCount1 ));
        OTV_TRACE(( " (ClassCount2 = %d)\n", ClassCount2 ));

        len_value1 = otv_value_length( ValueFormat1 );
        len_value2 = otv_value_length( ValueFormat2 );

        otv_Coverage_validate( table + Coverage, valid, -1 );
        otv_ClassDef_validate( table + ClassDef1, valid );
        otv_ClassDef_validate( table + ClassDef2, valid );

        OTV_LIMIT_CHECK( ClassCount1 * ClassCount2 *
                     ( len_value1 + len_value2 ) );

        /* Class1Record */
        for ( ; ClassCount1 > 0; ClassCount1-- )
        {
          /* Class2Record */
          for ( count = ClassCount2; count > 0; count-- )
          {
            if ( ValueFormat1 )
              /* Value1 */
              otv_ValueRecord_validate( p, ValueFormat1, valid );
            p += len_value1;

            if ( ValueFormat2 )
              /* Value2 */
              otv_ValueRecord_validate( p, ValueFormat2, valid );
            p += len_value2;
          }
        }
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 3                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_CursivePos_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "CursivePos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:     /* CursivePosFormat1 */
      {
        FT_UInt   table_size;
        FT_UInt   Coverage, EntryExitCount;

        OTV_OPTIONAL_TABLE( EntryAnchor );
        OTV_OPTIONAL_TABLE( ExitAnchor  );


        OTV_LIMIT_CHECK( 4 );
        Coverage       = FT_NEXT_USHORT( p );
        EntryExitCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (EntryExitCount = %d)\n", EntryExitCount ));

        otv_Coverage_validate( table + Coverage, valid, EntryExitCount );

        OTV_LIMIT_CHECK( EntryExitCount * 4 );

        table_size = EntryExitCount * 4 + 4;

        /* EntryExitRecord */
        for ( ; EntryExitCount > 0; EntryExitCount-- )
        {
          OTV_OPTIONAL_OFFSET( EntryAnchor );
          OTV_OPTIONAL_OFFSET( ExitAnchor  );

          OTV_SIZE_CHECK( EntryAnchor );
          if ( EntryAnchor )
            otv_Anchor_validate( table + EntryAnchor, valid );

          OTV_SIZE_CHECK( ExitAnchor );
          if ( ExitAnchor )
            otv_Anchor_validate( table + ExitAnchor, valid );
        }
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 4                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* UNDOCUMENTED (in OpenType 1.5):              */
  /* BaseRecord tables can contain NULL pointers. */

  /* sets valid->extra2 (1) */

  static void
  otv_MarkBasePos_validate( FT_Bytes       table,
                            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "MarkBasePos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:
      valid->extra2 = 1;
      OTV_NEST2( MarkBasePosFormat1, BaseArray );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 5                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra2 (1) */

  static void
  otv_MarkLigPos_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "MarkLigPos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:
      valid->extra2 = 1;
      OTV_NEST3( MarkLigPosFormat1, LigatureArray, LigatureAttach );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 6                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra2 (0) */

  static void
  otv_MarkMarkPos_validate( FT_Bytes       table,
                            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "MarkMarkPos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:
      valid->extra2 = 0;
      OTV_NEST2( MarkMarkPosFormat1, Mark2Array );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 7                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra1 (lookup count) */

  static void
  otv_ContextPos_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "ContextPos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      valid->extra1 = valid->lookup_count;
      OTV_NEST3( ContextPosFormat1, PosRuleSet, PosRule );
      OTV_RUN( table, valid );
      break;

    case 2:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      OTV_NEST3( ContextPosFormat2, PosClassSet, PosClassRule );
      OTV_RUN( table, valid );
      break;

    case 3:
      OTV_NEST1( ContextPosFormat3 );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 8                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra1 (lookup count) */

  static void
  otv_ChainContextPos_validate( FT_Bytes       table,
                                OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "ChainContextPos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      valid->extra1 = valid->lookup_count;
      OTV_NEST3( ChainContextPosFormat1,
                 ChainPosRuleSet, ChainPosRule );
      OTV_RUN( table, valid );
      break;

    case 2:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      OTV_NEST3( ChainContextPosFormat2,
                 ChainPosClassSet, ChainPosClassRule );
      OTV_RUN( table, valid );
      break;

    case 3:
      OTV_NEST1( ChainContextPosFormat3 );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                     GPOS LOOKUP TYPE 9                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* uses valid->type_funcs */

  static void
  otv_ExtensionPos_validate( FT_Bytes       table,
                             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   PosFormat;


    OTV_NAME_ENTER( "ExtensionPos" );

    OTV_LIMIT_CHECK( 2 );
    PosFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", PosFormat ));

    switch ( PosFormat )
    {
    case 1:     /* ExtensionPosFormat1 */
      {
        FT_UInt            ExtensionLookupType;
        FT_ULong           ExtensionOffset;
        OTV_Validate_Func  validate;


        OTV_LIMIT_CHECK( 6 );
        ExtensionLookupType = FT_NEXT_USHORT( p );
        ExtensionOffset     = FT_NEXT_ULONG( p );

        if ( ExtensionLookupType == 0 || ExtensionLookupType >= 9 )
          FT_INVALID_DATA;

        validate = valid->type_funcs[ExtensionLookupType - 1];
        validate( table + ExtensionOffset, valid );
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  static const OTV_Validate_Func  otv_gpos_validate_funcs[9] =
  {
    otv_SinglePos_validate,
    otv_PairPos_validate,
    otv_CursivePos_validate,
    otv_MarkBasePos_validate,
    otv_MarkLigPos_validate,
    otv_MarkMarkPos_validate,
    otv_ContextPos_validate,
    otv_ChainContextPos_validate,
    otv_ExtensionPos_validate
  };


  /* sets valid->type_count */
  /* sets valid->type_funcs */

  FT_LOCAL_DEF( void )
  otv_GPOS_subtable_validate( FT_Bytes       table,
                              OTV_Validator  valid )
  {
    valid->type_count = 9;
    valid->type_funcs = (OTV_Validate_Func*)otv_gpos_validate_funcs;

    otv_Lookup_validate( table, valid );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          GPOS TABLE                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->glyph_count */

  FT_LOCAL_DEF( void )
  otv_GPOS_validate( FT_Bytes      table,
                     FT_UInt       glyph_count,
                     FT_Validator  ftvalid )
  {
    OTV_ValidatorRec  validrec;
    OTV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_UInt           ScriptList, FeatureList, LookupList;


    valid->root = ftvalid;

    FT_TRACE3(( "validating GPOS table\n" ));
    OTV_INIT;

    OTV_LIMIT_CHECK( 10 );

    if ( FT_NEXT_ULONG( p ) != 0x10000UL )      /* Version */
      FT_INVALID_FORMAT;

    ScriptList  = FT_NEXT_USHORT( p );
    FeatureList = FT_NEXT_USHORT( p );
    LookupList  = FT_NEXT_USHORT( p );

    valid->type_count  = 9;
    valid->type_funcs  = (OTV_Validate_Func*)otv_gpos_validate_funcs;
    valid->glyph_count = glyph_count;

    otv_LookupList_validate( table + LookupList,
                             valid );
    otv_FeatureList_validate( table + FeatureList, table + LookupList,
                              valid );
    otv_ScriptList_validate( table + ScriptList, table + FeatureList,
                             valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvgsub.c                                                              */
/*                                                                         */
/*    OpenType GSUB table validation (body).                               */
/*                                                                         */
/*  Copyright 2004, 2005, 2007 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvalid.h"
#include "otvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvgsub


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                  GSUB LOOKUP TYPE 1                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* uses valid->glyph_count */

  static void
  otv_SingleSubst_validate( FT_Bytes       table,
                            OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "SingleSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:     /* SingleSubstFormat1 */
      {
        FT_Bytes  Coverage;
        FT_Int    DeltaGlyphID;
        FT_Long   idx;


        OTV_LIMIT_CHECK( 4 );
        Coverage     = table + FT_NEXT_USHORT( p );
        DeltaGlyphID = FT_NEXT_SHORT( p );

        otv_Coverage_validate( Coverage, valid, -1 );

        idx = otv_Coverage_get_first( Coverage ) + DeltaGlyphID;
        if ( idx < 0 )
          FT_INVALID_DATA;

        idx = otv_Coverage_get_last( Coverage ) + DeltaGlyphID;
        if ( (FT_UInt)idx >= valid->glyph_count )
          FT_INVALID_DATA;
      }
      break;

    case 2:     /* SingleSubstFormat2 */
      {
        FT_UInt  Coverage, GlyphCount;


        OTV_LIMIT_CHECK( 4 );
        Coverage   = FT_NEXT_USHORT( p );
        GlyphCount = FT_NEXT_USHORT( p );

        OTV_TRACE(( " (GlyphCount = %d)\n", GlyphCount ));

        otv_Coverage_validate( table + Coverage, valid, GlyphCount );

        OTV_LIMIT_CHECK( GlyphCount * 2 );

        /* Substitute */
        for ( ; GlyphCount > 0; GlyphCount-- )
          if ( FT_NEXT_USHORT( p ) >= valid->glyph_count )
            FT_INVALID_GLYPH_ID;
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                  GSUB LOOKUP TYPE 2                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra1 (glyph count) */

  static void
  otv_MultipleSubst_validate( FT_Bytes       table,
                              OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "MultipleSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:
      valid->extra1 = valid->glyph_count;
      OTV_NEST2( MultipleSubstFormat1, Sequence );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    GSUB LOOKUP TYPE 3                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra1 (glyph count) */

  static void
  otv_AlternateSubst_validate( FT_Bytes       table,
                               OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "AlternateSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:
      valid->extra1 = valid->glyph_count;
      OTV_NEST2( AlternateSubstFormat1, AlternateSet );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    GSUB LOOKUP TYPE 4                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define LigatureFunc  otv_Ligature_validate

  /* uses valid->glyph_count */

  static void
  otv_Ligature_validate( FT_Bytes       table,
                         OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   LigatureGlyph, CompCount;


    OTV_ENTER;

    OTV_LIMIT_CHECK( 4 );
    LigatureGlyph = FT_NEXT_USHORT( p );
    if ( LigatureGlyph >= valid->glyph_count )
      FT_INVALID_DATA;

    CompCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (CompCount = %d)\n", CompCount ));

    if ( CompCount == 0 )
      FT_INVALID_DATA;

    CompCount--;

    OTV_LIMIT_CHECK( CompCount * 2 );     /* Component */

    /* no need to check the Component glyph indices */

    OTV_EXIT;
  }


  static void
  otv_LigatureSubst_validate( FT_Bytes       table,
                              OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "LigatureSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:
      OTV_NEST3( LigatureSubstFormat1, LigatureSet, Ligature );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                  GSUB LOOKUP TYPE 5                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra1 (lookup count) */

  static void
  otv_ContextSubst_validate( FT_Bytes       table,
                             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "ContextSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      valid->extra1 = valid->lookup_count;
      OTV_NEST3( ContextSubstFormat1, SubRuleSet, SubRule );
      OTV_RUN( table, valid );
      break;

    case 2:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      OTV_NEST3( ContextSubstFormat2, SubClassSet, SubClassRule );
      OTV_RUN( table, valid );
      break;

    case 3:
      OTV_NEST1( ContextSubstFormat3 );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    GSUB LOOKUP TYPE 6                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->extra1 (lookup count)            */

  static void
  otv_ChainContextSubst_validate( FT_Bytes       table,
                                  OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "ChainContextSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      valid->extra1 = valid->lookup_count;
      OTV_NEST3( ChainContextSubstFormat1,
                 ChainSubRuleSet, ChainSubRule );
      OTV_RUN( table, valid );
      break;

    case 2:
      /* no need to check glyph indices/classes used as input for these */
      /* context rules since even invalid glyph indices/classes return  */
      /* meaningful results                                             */

      OTV_NEST3( ChainContextSubstFormat2,
                 ChainSubClassSet, ChainSubClassRule );
      OTV_RUN( table, valid );
      break;

    case 3:
      OTV_NEST1( ChainContextSubstFormat3 );
      OTV_RUN( table, valid );
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    GSUB LOOKUP TYPE 7                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* uses valid->type_funcs */

  static void
  otv_ExtensionSubst_validate( FT_Bytes       table,
                               OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   SubstFormat;


    OTV_NAME_ENTER( "ExtensionSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:     /* ExtensionSubstFormat1 */
      {
        FT_UInt            ExtensionLookupType;
        FT_ULong           ExtensionOffset;
        OTV_Validate_Func  validate;


        OTV_LIMIT_CHECK( 6 );
        ExtensionLookupType = FT_NEXT_USHORT( p );
        ExtensionOffset     = FT_NEXT_ULONG( p );

        if ( ExtensionLookupType == 0 ||
             ExtensionLookupType == 7 ||
             ExtensionLookupType > 8  )
          FT_INVALID_DATA;

        validate = valid->type_funcs[ExtensionLookupType - 1];
        validate( table + ExtensionOffset, valid );
      }
      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    GSUB LOOKUP TYPE 8                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* uses valid->glyph_count */

  static void
  otv_ReverseChainSingleSubst_validate( FT_Bytes       table,
                                        OTV_Validator  valid )
  {
    FT_Bytes  p = table, Coverage;
    FT_UInt   SubstFormat;
    FT_UInt   BacktrackGlyphCount, LookaheadGlyphCount, GlyphCount;


    OTV_NAME_ENTER( "ReverseChainSingleSubst" );

    OTV_LIMIT_CHECK( 2 );
    SubstFormat = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (format %d)\n", SubstFormat ));

    switch ( SubstFormat )
    {
    case 1:     /* ReverseChainSingleSubstFormat1 */
      OTV_LIMIT_CHECK( 4 );
      Coverage            = table + FT_NEXT_USHORT( p );
      BacktrackGlyphCount = FT_NEXT_USHORT( p );

      OTV_TRACE(( " (BacktrackGlyphCount = %d)\n", BacktrackGlyphCount ));

      otv_Coverage_validate( Coverage, valid, -1 );

      OTV_LIMIT_CHECK( BacktrackGlyphCount * 2 + 2 );

      for ( ; BacktrackGlyphCount > 0; BacktrackGlyphCount-- )
        otv_Coverage_validate( table + FT_NEXT_USHORT( p ), valid, -1 );

      LookaheadGlyphCount = FT_NEXT_USHORT( p );

      OTV_TRACE(( " (LookaheadGlyphCount = %d)\n", LookaheadGlyphCount ));

      OTV_LIMIT_CHECK( LookaheadGlyphCount * 2 + 2 );

      for ( ; LookaheadGlyphCount > 0; LookaheadGlyphCount-- )
        otv_Coverage_validate( table + FT_NEXT_USHORT( p ), valid, -1 );

      GlyphCount = FT_NEXT_USHORT( p );

      OTV_TRACE(( " (GlyphCount = %d)\n", GlyphCount ));

      if ( GlyphCount != otv_Coverage_get_count( Coverage ) )
        FT_INVALID_DATA;

      OTV_LIMIT_CHECK( GlyphCount * 2 );

      /* Substitute */
      for ( ; GlyphCount > 0; GlyphCount-- )
        if ( FT_NEXT_USHORT( p ) >= valid->glyph_count )
          FT_INVALID_DATA;

      break;

    default:
      FT_INVALID_FORMAT;
    }

    OTV_EXIT;
  }


  static const OTV_Validate_Func  otv_gsub_validate_funcs[8] =
  {
    otv_SingleSubst_validate,
    otv_MultipleSubst_validate,
    otv_AlternateSubst_validate,
    otv_LigatureSubst_validate,
    otv_ContextSubst_validate,
    otv_ChainContextSubst_validate,
    otv_ExtensionSubst_validate,
    otv_ReverseChainSingleSubst_validate
  };


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          GSUB TABLE                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->type_count  */
  /* sets valid->type_funcs  */
  /* sets valid->glyph_count */

  FT_LOCAL_DEF( void )
  otv_GSUB_validate( FT_Bytes      table,
                     FT_UInt       glyph_count,
                     FT_Validator  ftvalid )
  {
    OTV_ValidatorRec  validrec;
    OTV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_UInt           ScriptList, FeatureList, LookupList;


    valid->root = ftvalid;

    FT_TRACE3(( "validating GSUB table\n" ));
    OTV_INIT;

    OTV_LIMIT_CHECK( 10 );

    if ( FT_NEXT_ULONG( p ) != 0x10000UL )      /* Version */
      FT_INVALID_FORMAT;

    ScriptList  = FT_NEXT_USHORT( p );
    FeatureList = FT_NEXT_USHORT( p );
    LookupList  = FT_NEXT_USHORT( p );

    valid->type_count  = 8;
    valid->type_funcs  = (OTV_Validate_Func*)otv_gsub_validate_funcs;
    valid->glyph_count = glyph_count;

    otv_LookupList_validate( table + LookupList,
                             valid );
    otv_FeatureList_validate( table + FeatureList, table + LookupList,
                              valid );
    otv_ScriptList_validate( table + ScriptList, table + FeatureList,
                             valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvjstf.c                                                              */
/*                                                                         */
/*    OpenType JSTF table validation (body).                               */
/*                                                                         */
/*  Copyright 2004, 2007 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvalid.h"
#include "otvcommn.h"
#include "otvgpos.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvjstf


#define JstfPriorityFunc  otv_JstfPriority_validate
#define JstfLookupFunc    otv_GPOS_subtable_validate

  /* uses valid->extra1 (GSUB lookup count) */
  /* uses valid->extra2 (GPOS lookup count) */
  /* sets valid->extra1 (counter)           */

  static void
  otv_JstfPriority_validate( FT_Bytes       table,
                             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   table_size;
    FT_UInt   gsub_lookup_count, gpos_lookup_count;

    OTV_OPTIONAL_TABLE( ShrinkageEnableGSUB  );
    OTV_OPTIONAL_TABLE( ShrinkageDisableGSUB );
    OTV_OPTIONAL_TABLE( ShrinkageEnableGPOS  );
    OTV_OPTIONAL_TABLE( ShrinkageDisableGPOS );
    OTV_OPTIONAL_TABLE( ExtensionEnableGSUB  );
    OTV_OPTIONAL_TABLE( ExtensionDisableGSUB );
    OTV_OPTIONAL_TABLE( ExtensionEnableGPOS  );
    OTV_OPTIONAL_TABLE( ExtensionDisableGPOS );
    OTV_OPTIONAL_TABLE( ShrinkageJstfMax );
    OTV_OPTIONAL_TABLE( ExtensionJstfMax );


    OTV_ENTER;
    OTV_TRACE(( "JstfPriority table\n" ));

    OTV_LIMIT_CHECK( 20 );

    gsub_lookup_count = valid->extra1;
    gpos_lookup_count = valid->extra2;

    table_size = 20;

    valid->extra1 = gsub_lookup_count;

    OTV_OPTIONAL_OFFSET( ShrinkageEnableGSUB );
    OTV_SIZE_CHECK( ShrinkageEnableGSUB );
    if ( ShrinkageEnableGSUB )
      otv_x_ux( table + ShrinkageEnableGSUB, valid );

    OTV_OPTIONAL_OFFSET( ShrinkageDisableGSUB );
    OTV_SIZE_CHECK( ShrinkageDisableGSUB );
    if ( ShrinkageDisableGSUB )
      otv_x_ux( table + ShrinkageDisableGSUB, valid );

    valid->extra1 = gpos_lookup_count;

    OTV_OPTIONAL_OFFSET( ShrinkageEnableGPOS );
    OTV_SIZE_CHECK( ShrinkageEnableGPOS );
    if ( ShrinkageEnableGPOS )
      otv_x_ux( table + ShrinkageEnableGPOS, valid );

    OTV_OPTIONAL_OFFSET( ShrinkageDisableGPOS );
    OTV_SIZE_CHECK( ShrinkageDisableGPOS );
    if ( ShrinkageDisableGPOS )
      otv_x_ux( table + ShrinkageDisableGPOS, valid );

    OTV_OPTIONAL_OFFSET( ShrinkageJstfMax );
    OTV_SIZE_CHECK( ShrinkageJstfMax );
    if ( ShrinkageJstfMax )
    {
      /* XXX: check lookup types? */
      OTV_NEST2( JstfMax, JstfLookup );
      OTV_RUN( table + ShrinkageJstfMax, valid );
    }

    valid->extra1 = gsub_lookup_count;

    OTV_OPTIONAL_OFFSET( ExtensionEnableGSUB );
    OTV_SIZE_CHECK( ExtensionEnableGSUB );
    if ( ExtensionEnableGSUB )
      otv_x_ux( table + ExtensionEnableGSUB, valid );

    OTV_OPTIONAL_OFFSET( ExtensionDisableGSUB );
    OTV_SIZE_CHECK( ExtensionDisableGSUB );
    if ( ExtensionDisableGSUB )
      otv_x_ux( table + ExtensionDisableGSUB, valid );

    valid->extra1 = gpos_lookup_count;

    OTV_OPTIONAL_OFFSET( ExtensionEnableGPOS );
    OTV_SIZE_CHECK( ExtensionEnableGPOS );
    if ( ExtensionEnableGPOS )
      otv_x_ux( table + ExtensionEnableGPOS, valid );

    OTV_OPTIONAL_OFFSET( ExtensionDisableGPOS );
    OTV_SIZE_CHECK( ExtensionDisableGPOS );
    if ( ExtensionDisableGPOS )
      otv_x_ux( table + ExtensionDisableGPOS, valid );

    OTV_OPTIONAL_OFFSET( ExtensionJstfMax );
    OTV_SIZE_CHECK( ExtensionJstfMax );
    if ( ExtensionJstfMax )
    {
      /* XXX: check lookup types? */
      OTV_NEST2( JstfMax, JstfLookup );
      OTV_RUN( table + ExtensionJstfMax, valid );
    }

    valid->extra1 = gsub_lookup_count;
    valid->extra2 = gpos_lookup_count;

    OTV_EXIT;
  }


  /* sets valid->extra (glyph count)               */
  /* sets valid->func1 (otv_JstfPriority_validate) */

  static void
  otv_JstfScript_validate( FT_Bytes       table,
                           OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   table_size;
    FT_UInt   JstfLangSysCount;

    OTV_OPTIONAL_TABLE( ExtGlyph );
    OTV_OPTIONAL_TABLE( DefJstfLangSys );


    OTV_NAME_ENTER( "JstfScript" );

    OTV_LIMIT_CHECK( 6 );
    OTV_OPTIONAL_OFFSET( ExtGlyph );
    OTV_OPTIONAL_OFFSET( DefJstfLangSys );
    JstfLangSysCount = FT_NEXT_USHORT( p );

    OTV_TRACE(( " (JstfLangSysCount = %d)\n", JstfLangSysCount ));

    table_size = JstfLangSysCount * 6 + 6;

    OTV_SIZE_CHECK( ExtGlyph );
    if ( ExtGlyph )
    {
      valid->extra1 = valid->glyph_count;
      OTV_NEST1( ExtenderGlyph );
      OTV_RUN( table + ExtGlyph, valid );
    }

    OTV_SIZE_CHECK( DefJstfLangSys );
    if ( DefJstfLangSys )
    {
      OTV_NEST2( JstfLangSys, JstfPriority );
      OTV_RUN( table + DefJstfLangSys, valid );
    }

    OTV_LIMIT_CHECK( 6 * JstfLangSysCount );

    /* JstfLangSysRecord */
    OTV_NEST2( JstfLangSys, JstfPriority );
    for ( ; JstfLangSysCount > 0; JstfLangSysCount-- )
    {
      p += 4;       /* skip JstfLangSysTag */

      OTV_RUN( table + FT_NEXT_USHORT( p ), valid );
    }

    OTV_EXIT;
  }


  /* sets valid->extra1 (GSUB lookup count) */
  /* sets valid->extra2 (GPOS lookup count) */
  /* sets valid->glyph_count                */

  FT_LOCAL_DEF( void )
  otv_JSTF_validate( FT_Bytes      table,
                     FT_Bytes      gsub,
                     FT_Bytes      gpos,
                     FT_UInt       glyph_count,
                     FT_Validator  ftvalid )
  {
    OTV_ValidatorRec  validrec;
    OTV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_UInt           JstfScriptCount;


    valid->root = ftvalid;

    FT_TRACE3(( "validating JSTF table\n" ));
    OTV_INIT;

    OTV_LIMIT_CHECK( 6 );

    if ( FT_NEXT_ULONG( p ) != 0x10000UL )      /* Version */
      FT_INVALID_FORMAT;

    JstfScriptCount = FT_NEXT_USHORT( p );

    FT_TRACE3(( " (JstfScriptCount = %d)\n", JstfScriptCount ));

    OTV_LIMIT_CHECK( JstfScriptCount * 6 );

    if ( gsub )
      valid->extra1 = otv_GSUBGPOS_get_Lookup_count( gsub );
    else
      valid->extra1 = 0;

    if ( gpos )
      valid->extra2 = otv_GSUBGPOS_get_Lookup_count( gpos );
    else
      valid->extra2 = 0;

    valid->glyph_count = glyph_count;

    /* JstfScriptRecord */
    for ( ; JstfScriptCount > 0; JstfScriptCount-- )
    {
      p += 4;       /* skip JstfScriptTag */

      /* JstfScript */
      otv_JstfScript_validate( table + FT_NEXT_USHORT( p ), valid );
    }

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvmath.c                                                              */
/*                                                                         */
/*    OpenType MATH table validation (body).                               */
/*                                                                         */
/*  Copyright 2007, 2008 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  Written by George Williams.                                            */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "otvalid.h"
#include "otvcommn.h"
#include "otvgpos.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvmath



  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                  MATH TYPOGRAPHIC CONSTANTS                   *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_MathConstants_validate( FT_Bytes       table,
                              OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   i;
    FT_UInt   table_size;

    OTV_OPTIONAL_TABLE( DeviceTableOffset );


    OTV_NAME_ENTER( "MathConstants" );

    /* 56 constants, 51 have device tables */
    OTV_LIMIT_CHECK( 2 * ( 56 + 51 ) );
    table_size = 2 * ( 56 + 51 );

    p += 4 * 2;                 /* First 4 constants have no device tables */
    for ( i = 0; i < 51; ++i )
    {
      p += 2;                                            /* skip the value */
      OTV_OPTIONAL_OFFSET( DeviceTableOffset );
      OTV_SIZE_CHECK( DeviceTableOffset );
      if ( DeviceTableOffset )
        otv_Device_validate( table + DeviceTableOffset, valid );
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                   MATH ITALICS CORRECTION                     *****/
  /*****                 MATH TOP ACCENT ATTACHMENT                    *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_MathItalicsCorrectionInfo_validate( FT_Bytes       table,
                                          OTV_Validator  valid,
                                          FT_Int         isItalic )
  {
    FT_Bytes  p = table;
    FT_UInt   i, cnt, table_size ;

    OTV_OPTIONAL_TABLE( Coverage );
    OTV_OPTIONAL_TABLE( DeviceTableOffset );

    FT_UNUSED( isItalic );  /* only used if tracing is active */


    OTV_NAME_ENTER( isItalic ? "MathItalicsCorrectionInfo"
                             : "MathTopAccentAttachment" );

    OTV_LIMIT_CHECK( 4 );

    OTV_OPTIONAL_OFFSET( Coverage );
    cnt = FT_NEXT_USHORT( p );

    OTV_LIMIT_CHECK( 4 * cnt );
    table_size = 4 + 4 * cnt;

    OTV_SIZE_CHECK( Coverage );
    otv_Coverage_validate( table + Coverage, valid, cnt );

    for ( i = 0; i < cnt; ++i )
    {
      p += 2;                                            /* Skip the value */
      OTV_OPTIONAL_OFFSET( DeviceTableOffset );
      OTV_SIZE_CHECK( DeviceTableOffset );
      if ( DeviceTableOffset )
        otv_Device_validate( table + DeviceTableOffset, valid );
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                           MATH KERNING                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_MathKern_validate( FT_Bytes       table,
                         OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   i, cnt, table_size;

    OTV_OPTIONAL_TABLE( DeviceTableOffset );


    /* OTV_NAME_ENTER( "MathKern" );*/

    OTV_LIMIT_CHECK( 2 );

    cnt = FT_NEXT_USHORT( p );

    OTV_LIMIT_CHECK( 4 * cnt + 2 );
    table_size = 4 + 4 * cnt;

    /* Heights */
    for ( i = 0; i < cnt; ++i )
    {
      p += 2;                                            /* Skip the value */
      OTV_OPTIONAL_OFFSET( DeviceTableOffset );
      OTV_SIZE_CHECK( DeviceTableOffset );
      if ( DeviceTableOffset )
        otv_Device_validate( table + DeviceTableOffset, valid );
    }

    /* One more Kerning value */
    for ( i = 0; i < cnt + 1; ++i )
    {
      p += 2;                                            /* Skip the value */
      OTV_OPTIONAL_OFFSET( DeviceTableOffset );
      OTV_SIZE_CHECK( DeviceTableOffset );
      if ( DeviceTableOffset )
        otv_Device_validate( table + DeviceTableOffset, valid );
    }

    OTV_EXIT;
  }


  static void
  otv_MathKernInfo_validate( FT_Bytes       table,
                             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   i, j, cnt, table_size;

    OTV_OPTIONAL_TABLE( Coverage );
    OTV_OPTIONAL_TABLE( MKRecordOffset );


    OTV_NAME_ENTER( "MathKernInfo" );

    OTV_LIMIT_CHECK( 4 );

    OTV_OPTIONAL_OFFSET( Coverage );
    cnt = FT_NEXT_USHORT( p );

    OTV_LIMIT_CHECK( 8 * cnt );
    table_size = 4 + 8 * cnt;

    OTV_SIZE_CHECK( Coverage );
    otv_Coverage_validate( table + Coverage, valid, cnt );

    for ( i = 0; i < cnt; ++i )
    {
      for ( j = 0; j < 4; ++j )
      {
        OTV_OPTIONAL_OFFSET( MKRecordOffset );
        OTV_SIZE_CHECK( MKRecordOffset );
        if ( MKRecordOffset )
          otv_MathKern_validate( table + MKRecordOffset, valid );
      }
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         MATH GLYPH INFO                       *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_MathGlyphInfo_validate( FT_Bytes       table,
                              OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   MathItalicsCorrectionInfo, MathTopAccentAttachment;
    FT_UInt   ExtendedShapeCoverage, MathKernInfo;


    OTV_NAME_ENTER( "MathGlyphInfo" );

    OTV_LIMIT_CHECK( 8 );

    MathItalicsCorrectionInfo = FT_NEXT_USHORT( p );
    MathTopAccentAttachment   = FT_NEXT_USHORT( p );
    ExtendedShapeCoverage     = FT_NEXT_USHORT( p );
    MathKernInfo              = FT_NEXT_USHORT( p );

    if ( MathItalicsCorrectionInfo )
      otv_MathItalicsCorrectionInfo_validate(
        table + MathItalicsCorrectionInfo, valid, TRUE );

    /* Italic correction and Top Accent Attachment have the same format */
    if ( MathTopAccentAttachment )
      otv_MathItalicsCorrectionInfo_validate(
        table + MathTopAccentAttachment, valid, FALSE );

    if ( ExtendedShapeCoverage )
    {
      OTV_NAME_ENTER( "ExtendedShapeCoverage" );
      otv_Coverage_validate( table + ExtendedShapeCoverage, valid, -1 );
      OTV_EXIT;
    }

    if ( MathKernInfo )
      otv_MathKernInfo_validate( table + MathKernInfo, valid );

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    MATH GLYPH CONSTRUCTION                    *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  otv_GlyphAssembly_validate( FT_Bytes       table,
                              OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   pcnt, table_size;
    FT_UInt   i;

    OTV_OPTIONAL_TABLE( DeviceTableOffset );


    /* OTV_NAME_ENTER( "GlyphAssembly" ); */

    OTV_LIMIT_CHECK( 6 );

    p += 2;                           /* Skip the Italics Correction value */
    OTV_OPTIONAL_OFFSET( DeviceTableOffset );
    pcnt = FT_NEXT_USHORT( p );

    OTV_LIMIT_CHECK( 8 * pcnt );
    table_size = 6 + 8 * pcnt;

    OTV_SIZE_CHECK( DeviceTableOffset );
    if ( DeviceTableOffset )
      otv_Device_validate( table + DeviceTableOffset, valid );

    for ( i = 0; i < pcnt; ++i )
    {
      FT_UInt  gid;


      gid = FT_NEXT_USHORT( p );
      if ( gid >= valid->glyph_count )
        FT_INVALID_GLYPH_ID;
      p += 2*4;             /* skip the Start, End, Full, and Flags fields */
    }

    /* OTV_EXIT; */
  }


  static void
  otv_MathGlyphConstruction_validate( FT_Bytes       table,
                                      OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   vcnt, table_size;
    FT_UInt   i;

    OTV_OPTIONAL_TABLE( GlyphAssembly );


    /* OTV_NAME_ENTER( "MathGlyphConstruction" ); */

    OTV_LIMIT_CHECK( 4 );

    OTV_OPTIONAL_OFFSET( GlyphAssembly );
    vcnt = FT_NEXT_USHORT( p );

    OTV_LIMIT_CHECK( 4 * vcnt );
    table_size = 4 + 4 * vcnt;

    for ( i = 0; i < vcnt; ++i )
    {
      FT_UInt  gid;


      gid = FT_NEXT_USHORT( p );
      if ( gid >= valid->glyph_count )
        FT_INVALID_GLYPH_ID;
      p += 2;                          /* skip the size */
    }

    OTV_SIZE_CHECK( GlyphAssembly );
    if ( GlyphAssembly )
      otv_GlyphAssembly_validate( table+GlyphAssembly, valid );

    /* OTV_EXIT; */
  }


  static void
  otv_MathVariants_validate( FT_Bytes       table,
                             OTV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_UInt   vcnt, hcnt, i, table_size;

    OTV_OPTIONAL_TABLE( VCoverage );
    OTV_OPTIONAL_TABLE( HCoverage );
    OTV_OPTIONAL_TABLE( Offset );


    OTV_NAME_ENTER( "MathVariants" );

    OTV_LIMIT_CHECK( 10 );

    p += 2;                       /* Skip the MinConnectorOverlap constant */
    OTV_OPTIONAL_OFFSET( VCoverage );
    OTV_OPTIONAL_OFFSET( HCoverage );
    vcnt = FT_NEXT_USHORT( p );
    hcnt = FT_NEXT_USHORT( p );

    OTV_LIMIT_CHECK( 2 * vcnt + 2 * hcnt );
    table_size = 10 + 2 * vcnt + 2 * hcnt;

    OTV_SIZE_CHECK( VCoverage );
    if ( VCoverage )
      otv_Coverage_validate( table + VCoverage, valid, vcnt );

    OTV_SIZE_CHECK( HCoverage );
    if ( HCoverage )
      otv_Coverage_validate( table + HCoverage, valid, hcnt );

    for ( i = 0; i < vcnt; ++i )
    {
      OTV_OPTIONAL_OFFSET( Offset );
      OTV_SIZE_CHECK( Offset );
      otv_MathGlyphConstruction_validate( table + Offset, valid );
    }

    for ( i = 0; i < hcnt; ++i )
    {
      OTV_OPTIONAL_OFFSET( Offset );
      OTV_SIZE_CHECK( Offset );
      otv_MathGlyphConstruction_validate( table + Offset, valid );
    }

    OTV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          MATH TABLE                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* sets valid->glyph_count */

  FT_LOCAL_DEF( void )
  otv_MATH_validate( FT_Bytes      table,
                     FT_UInt       glyph_count,
                     FT_Validator  ftvalid )
  {
    OTV_ValidatorRec  validrec;
    OTV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_UInt           MathConstants, MathGlyphInfo, MathVariants;


    valid->root = ftvalid;

    FT_TRACE3(( "validating MATH table\n" ));
    OTV_INIT;

    OTV_LIMIT_CHECK( 10 );

    if ( FT_NEXT_ULONG( p ) != 0x10000UL )      /* Version */
      FT_INVALID_FORMAT;

    MathConstants = FT_NEXT_USHORT( p );
    MathGlyphInfo = FT_NEXT_USHORT( p );
    MathVariants  = FT_NEXT_USHORT( p );

    valid->glyph_count = glyph_count;

    otv_MathConstants_validate( table + MathConstants,
                                valid );
    otv_MathGlyphInfo_validate( table + MathGlyphInfo,
                                valid );
    otv_MathVariants_validate ( table + MathVariants,
                                valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  otvmod.c                                                               */
/*                                                                         */
/*    FreeType's OpenType validation module implementation (body).         */
/*                                                                         */
/*  Copyright 2004-2008, 2013 by                                           */
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
#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_TAGS_H
#include FT_OPENTYPE_VALIDATE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_SERVICE_OPENTYPE_VALIDATE_H

#include "otvmod.h"
#include "otvalid.h"
#include "otvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_otvmodule


  static FT_Error
  otv_load_table( FT_Face             face,
                  FT_Tag              tag,
                  FT_Byte* volatile*  table,
                  FT_ULong*           table_len )
  {
    FT_Error   error;
    FT_Memory  memory = FT_FACE_MEMORY( face );


    error = FT_Load_Sfnt_Table( face, tag, 0, NULL, table_len );
    if ( FT_ERR_EQ( error, Table_Missing ) )
      return FT_Err_Ok;
    if ( error )
      goto Exit;

    if ( FT_ALLOC( *table, *table_len ) )
      goto Exit;

    error = FT_Load_Sfnt_Table( face, tag, 0, *table, table_len );

  Exit:
    return error;
  }


  static FT_Error
  otv_validate( FT_Face volatile   face,
                FT_UInt            ot_flags,
                FT_Bytes          *ot_base,
                FT_Bytes          *ot_gdef,
                FT_Bytes          *ot_gpos,
                FT_Bytes          *ot_gsub,
                FT_Bytes          *ot_jstf )
  {
    FT_Error                  error = FT_Err_Ok;
    FT_Byte* volatile         base;
    FT_Byte* volatile         gdef;
    FT_Byte* volatile         gpos;
    FT_Byte* volatile         gsub;
    FT_Byte* volatile         jstf;
    FT_Byte* volatile         math;
    FT_ULong                  len_base, len_gdef, len_gpos, len_gsub, len_jstf;
    FT_ULong                  len_math;
    FT_UInt                   num_glyphs = (FT_UInt)face->num_glyphs;
    FT_ValidatorRec volatile  valid;


    base     = gdef     = gpos     = gsub     = jstf     = math     = NULL;
    len_base = len_gdef = len_gpos = len_gsub = len_jstf = len_math = 0;

    /*
     * XXX: OpenType tables cannot handle 32-bit glyph index,
     *      although broken TrueType can have 32-bit glyph index.
     */
    if ( face->num_glyphs > 0xFFFFL )
    {
      FT_TRACE1(( "otv_validate: Invalid glyphs index (0x0000FFFF - 0x%08x) ",
                  face->num_glyphs ));
      FT_TRACE1(( "are not handled by OpenType tables\n" ));
      num_glyphs = 0xFFFF;
    }

    /* load tables */

    if ( ot_flags & FT_VALIDATE_BASE )
    {
      error = otv_load_table( face, TTAG_BASE, &base, &len_base );
      if ( error )
        goto Exit;
    }

    if ( ot_flags & FT_VALIDATE_GDEF )
    {
      error = otv_load_table( face, TTAG_GDEF, &gdef, &len_gdef );
      if ( error )
        goto Exit;
    }

    if ( ot_flags & FT_VALIDATE_GPOS )
    {
      error = otv_load_table( face, TTAG_GPOS, &gpos, &len_gpos );
      if ( error )
        goto Exit;
    }

    if ( ot_flags & FT_VALIDATE_GSUB )
    {
      error = otv_load_table( face, TTAG_GSUB, &gsub, &len_gsub );
      if ( error )
        goto Exit;
    }

    if ( ot_flags & FT_VALIDATE_JSTF )
    {
      error = otv_load_table( face, TTAG_JSTF, &jstf, &len_jstf );
      if ( error )
        goto Exit;
    }

    if ( ot_flags & FT_VALIDATE_MATH )
    {
      error = otv_load_table( face, TTAG_MATH, &math, &len_math );
      if ( error )
        goto Exit;
    }

    /* validate tables */

    if ( base )
    {
      ft_validator_init( &valid, base, base + len_base, FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        otv_BASE_validate( base, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    if ( gpos )
    {
      ft_validator_init( &valid, gpos, gpos + len_gpos, FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        otv_GPOS_validate( gpos, num_glyphs, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    if ( gsub )
    {
      ft_validator_init( &valid, gsub, gsub + len_gsub, FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        otv_GSUB_validate( gsub, num_glyphs, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    if ( gdef )
    {
      ft_validator_init( &valid, gdef, gdef + len_gdef, FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        otv_GDEF_validate( gdef, gsub, gpos, num_glyphs, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    if ( jstf )
    {
      ft_validator_init( &valid, jstf, jstf + len_jstf, FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        otv_JSTF_validate( jstf, gsub, gpos, num_glyphs, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    if ( math )
    {
      ft_validator_init( &valid, math, math + len_math, FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        otv_MATH_validate( math, num_glyphs, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    *ot_base = (FT_Bytes)base;
    *ot_gdef = (FT_Bytes)gdef;
    *ot_gpos = (FT_Bytes)gpos;
    *ot_gsub = (FT_Bytes)gsub;
    *ot_jstf = (FT_Bytes)jstf;

  Exit:
    if ( error )
    {
      FT_Memory  memory = FT_FACE_MEMORY( face );


      FT_FREE( base );
      FT_FREE( gdef );
      FT_FREE( gpos );
      FT_FREE( gsub );
      FT_FREE( jstf );
    }

    {
      FT_Memory  memory = FT_FACE_MEMORY( face );


      FT_FREE( math );                 /* Can't return this as API is frozen */
    }

    return error;
  }


  static
  const FT_Service_OTvalidateRec  otvalid_interface =
  {
    otv_validate
  };


  static
  const FT_ServiceDescRec  otvalid_services[] =
  {
    { FT_SERVICE_ID_OPENTYPE_VALIDATE, &otvalid_interface },
    { NULL, NULL }
  };


  static FT_Pointer
  otvalid_get_service( FT_Module    module,
                       const char*  service_id )
  {
    FT_UNUSED( module );

    return ft_service_list_lookup( otvalid_services, service_id );
  }


  FT_CALLBACK_TABLE_DEF
  const FT_Module_Class  otv_module_class =
  {
    0,
    sizeof ( FT_ModuleRec ),
    "otvalid",
    0x10000L,
    0x20000L,

    0,              /* module-specific interface */

    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  otvalid_get_service
  };


/* END */

/* END */
