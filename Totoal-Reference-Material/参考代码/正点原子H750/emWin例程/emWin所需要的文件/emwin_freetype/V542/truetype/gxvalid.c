/***************************************************************************/
/*                                                                         */
/*  gxvalid.c                                                              */
/*                                                                         */
/*    FreeType validator for TrueTypeGX/AAT tables (body only).            */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
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
/*  gxvfeat.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT feat table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005, 2008, 2012 by                                    */
/*  suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"
#include "gxvfeat.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvfeat


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  typedef struct  GXV_feat_DataRec_
  {
    FT_UInt    reserved_size;
    FT_UShort  feature;
    FT_UShort  setting;

  } GXV_feat_DataRec, *GXV_feat_Data;


#define GXV_FEAT_DATA( field )  GXV_TABLE_DATA( feat, field )


  typedef enum  GXV_FeatureFlagsMask_
  {
    GXV_FEAT_MASK_EXCLUSIVE_SETTINGS = 0x8000U,
    GXV_FEAT_MASK_DYNAMIC_DEFAULT    = 0x4000,
    GXV_FEAT_MASK_UNUSED             = 0x3F00,
    GXV_FEAT_MASK_DEFAULT_SETTING    = 0x00FF

  } GXV_FeatureFlagsMask;


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_feat_registry_validate( FT_UShort      feature,
                              FT_UShort      nSettings,
                              FT_Bool        exclusive,
                              GXV_Validator  valid )
  {
    GXV_NAME_ENTER( "feature in registry" );

    GXV_TRACE(( " (feature = %u)\n", feature ));

    if ( feature >= gxv_feat_registry_length )
    {
      GXV_TRACE(( "feature number %d is out of range %d\n",
                  feature, gxv_feat_registry_length ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
      goto Exit;
    }

    if ( gxv_feat_registry[feature].existence == 0 )
    {
      GXV_TRACE(( "feature number %d is in defined range but doesn't exist\n",
                  feature ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
      goto Exit;
    }

    if ( gxv_feat_registry[feature].apple_reserved )
    {
      /* Don't use here. Apple is reserved. */
      GXV_TRACE(( "feature number %d is reserved by Apple\n", feature ));
      if ( valid->root->level >= FT_VALIDATE_TIGHT )
        FT_INVALID_DATA;
    }

    if ( nSettings != gxv_feat_registry[feature].nSettings )
    {
      GXV_TRACE(( "feature %d: nSettings %d != defined nSettings %d\n",
                  feature, nSettings,
                  gxv_feat_registry[feature].nSettings ));
      if ( valid->root->level >= FT_VALIDATE_TIGHT )
        FT_INVALID_DATA;
    }

    if ( exclusive != gxv_feat_registry[feature].exclusive )
    {
      GXV_TRACE(( "exclusive flag %d differs from predefined value\n",
                  exclusive ));
      if ( valid->root->level >= FT_VALIDATE_TIGHT )
        FT_INVALID_DATA;
    }

  Exit:
    GXV_EXIT;
  }


  static void
  gxv_feat_name_index_validate( FT_Bytes       table,
                                FT_Bytes       limit,
                                GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    FT_Short  nameIndex;


    GXV_NAME_ENTER( "nameIndex" );

    GXV_LIMIT_CHECK( 2 );
    nameIndex = FT_NEXT_SHORT ( p );
    GXV_TRACE(( " (nameIndex = %d)\n", nameIndex ));

    gxv_sfntName_validate( (FT_UShort)nameIndex,
                           255,
                           32768U,
                           valid );

    GXV_EXIT;
  }


  static void
  gxv_feat_setting_validate( FT_Bytes       table,
                             FT_Bytes       limit,
                             FT_Bool        exclusive,
                             GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  setting;


    GXV_NAME_ENTER( "setting" );

    GXV_LIMIT_CHECK( 2 );

    setting = FT_NEXT_USHORT( p );

    /* If we have exclusive setting, the setting should be odd. */
    if ( exclusive && ( setting & 1 ) == 0 )
      FT_INVALID_DATA;

    gxv_feat_name_index_validate( p, limit, valid );

    GXV_FEAT_DATA( setting ) = setting;

    GXV_EXIT;
  }


  static void
  gxv_feat_name_validate( FT_Bytes       table,
                          FT_Bytes       limit,
                          GXV_Validator  valid )
  {
    FT_Bytes   p             = table;
    FT_UInt    reserved_size = GXV_FEAT_DATA( reserved_size );

    FT_UShort  feature;
    FT_UShort  nSettings;
    FT_ULong   settingTable;
    FT_UShort  featureFlags;

    FT_Bool    exclusive;
    FT_Int     last_setting;
    FT_UInt    i;


    GXV_NAME_ENTER( "name" );

    /* feature + nSettings + settingTable + featureFlags */
    GXV_LIMIT_CHECK( 2 + 2 + 4 + 2 );

    feature = FT_NEXT_USHORT( p );
    GXV_FEAT_DATA( feature ) = feature;

    nSettings    = FT_NEXT_USHORT( p );
    settingTable = FT_NEXT_ULONG ( p );
    featureFlags = FT_NEXT_USHORT( p );

    if ( settingTable < reserved_size )
      FT_INVALID_OFFSET;

    if ( ( featureFlags & GXV_FEAT_MASK_UNUSED ) == 0 )
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );

    exclusive = FT_BOOL( featureFlags & GXV_FEAT_MASK_EXCLUSIVE_SETTINGS );
    if ( exclusive )
    {
      FT_Byte  dynamic_default;


      if ( featureFlags & GXV_FEAT_MASK_DYNAMIC_DEFAULT )
        dynamic_default = (FT_Byte)( featureFlags &
                                     GXV_FEAT_MASK_DEFAULT_SETTING );
      else
        dynamic_default = 0;

      /* If exclusive, check whether default setting is in the range. */
      if ( !( dynamic_default < nSettings ) )
        FT_INVALID_FORMAT;
    }

    gxv_feat_registry_validate( feature, nSettings, exclusive, valid );

    gxv_feat_name_index_validate( p, limit, valid );

    p = valid->root->base + settingTable;
    for ( last_setting = -1, i = 0; i < nSettings; i++ )
    {
      gxv_feat_setting_validate( p, limit, exclusive, valid );

      if ( (FT_Int)GXV_FEAT_DATA( setting ) <= last_setting )
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_FORMAT );

      last_setting = (FT_Int)GXV_FEAT_DATA( setting );
      /* setting + nameIndex */
      p += ( 2 + 2 );
    }

    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         feat TABLE                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_feat_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;

    GXV_feat_DataRec  featrec;
    GXV_feat_Data     feat = &featrec;

    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;

    FT_UInt           featureNameCount;

    FT_UInt           i;
    FT_Int            last_feature;


    valid->root       = ftvalid;
    valid->table_data = feat;
    valid->face       = face;

    FT_TRACE3(( "validating `feat' table\n" ));
    GXV_INIT;

    feat->reserved_size = 0;

    /* version + featureNameCount + none_0 + none_1  */
    GXV_LIMIT_CHECK( 4 + 2 + 2 + 4 );
    feat->reserved_size += 4 + 2 + 2 + 4;

    if ( FT_NEXT_ULONG( p ) != 0x00010000UL ) /* Version */
      FT_INVALID_FORMAT;

    featureNameCount = FT_NEXT_USHORT( p );
    GXV_TRACE(( " (featureNameCount = %d)\n", featureNameCount ));

    if ( !( IS_PARANOID_VALIDATION ) )
      p += 6; /* skip (none) and (none) */
    else
    {
      if ( FT_NEXT_USHORT( p ) != 0 )
        FT_INVALID_DATA;

      if ( FT_NEXT_ULONG( p )  != 0 )
        FT_INVALID_DATA;
    }

    feat->reserved_size += featureNameCount * ( 2 + 2 + 4 + 2 + 2 );

    for ( last_feature = -1, i = 0; i < featureNameCount; i++ )
    {
      gxv_feat_name_validate( p, limit, valid );

      if ( (FT_Int)GXV_FEAT_DATA( feature ) <= last_feature )
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_FORMAT );

      last_feature = GXV_FEAT_DATA( feature );
      p += 2 + 2 + 4 + 2 + 2;
    }

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvcommn.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT common tables validation (body).                      */
/*                                                                         */
/*  Copyright 2004, 2005, 2009, 2010, 2013                                 */
/*  by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvcommon


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       16bit offset sorter                     *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static int
  gxv_compare_ushort_offset( FT_UShort*  a,
                             FT_UShort*  b )
  {
    if ( *a < *b )
      return -1;
    else if ( *a > *b )
      return 1;
    else
      return 0;
  }


  FT_LOCAL_DEF( void )
  gxv_set_length_by_ushort_offset( FT_UShort*     offset,
                                   FT_UShort**    length,
                                   FT_UShort*     buff,
                                   FT_UInt        nmemb,
                                   FT_UShort      limit,
                                   GXV_Validator  valid )
  {
    FT_UInt  i;


    for ( i = 0; i < nmemb; i++ )
      *(length[i]) = 0;

    for ( i = 0; i < nmemb; i++ )
      buff[i] = offset[i];
    buff[nmemb] = limit;

    ft_qsort( buff, ( nmemb + 1 ), sizeof ( FT_UShort ),
              ( int(*)(const void*, const void*) )gxv_compare_ushort_offset );

    if ( buff[nmemb] > limit )
      FT_INVALID_OFFSET;

    for ( i = 0; i < nmemb; i++ )
    {
      FT_UInt  j;


      for ( j = 0; j < nmemb; j++ )
        if ( buff[j] == offset[i] )
          break;

      if ( j == nmemb )
        FT_INVALID_OFFSET;

      *(length[i]) = (FT_UShort)( buff[j + 1] - buff[j] );

      if ( 0 != offset[i] && 0 == *(length[i]) )
        FT_INVALID_OFFSET;
    }
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       32bit offset sorter                     *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static int
  gxv_compare_ulong_offset( FT_ULong*  a,
                            FT_ULong*  b )
  {
    if ( *a < *b )
      return -1;
    else if ( *a > *b )
      return 1;
    else
      return 0;
  }


  FT_LOCAL_DEF( void )
  gxv_set_length_by_ulong_offset( FT_ULong*      offset,
                                  FT_ULong**     length,
                                  FT_ULong*      buff,
                                  FT_UInt        nmemb,
                                  FT_ULong       limit,
                                  GXV_Validator  valid)
  {
    FT_UInt  i;


    for ( i = 0; i < nmemb; i++ )
      *(length[i]) = 0;

    for ( i = 0; i < nmemb; i++ )
      buff[i] = offset[i];
    buff[nmemb] = limit;

    ft_qsort( buff, ( nmemb + 1 ), sizeof ( FT_ULong ),
              ( int(*)(const void*, const void*) )gxv_compare_ulong_offset );

    if ( buff[nmemb] > limit )
      FT_INVALID_OFFSET;

    for ( i = 0; i < nmemb; i++ )
    {
      FT_UInt  j;


      for ( j = 0; j < nmemb; j++ )
        if ( buff[j] == offset[i] )
          break;

      if ( j == nmemb )
        FT_INVALID_OFFSET;

      *(length[i]) = buff[j + 1] - buff[j];

      if ( 0 != offset[i] && 0 == *(length[i]) )
        FT_INVALID_OFFSET;
    }
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****               scan value array and get min & max              *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  FT_LOCAL_DEF( void )
  gxv_array_getlimits_byte( FT_Bytes       table,
                            FT_Bytes       limit,
                            FT_Byte*       min,
                            FT_Byte*       max,
                            GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    *min = 0xFF;
    *max = 0x00;

    while ( p < limit )
    {
      FT_Byte  val;


      GXV_LIMIT_CHECK( 1 );
      val = FT_NEXT_BYTE( p );

      *min = (FT_Byte)FT_MIN( *min, val );
      *max = (FT_Byte)FT_MAX( *max, val );
    }

    valid->subtable_length = p - table;
  }


  FT_LOCAL_DEF( void )
  gxv_array_getlimits_ushort( FT_Bytes       table,
                              FT_Bytes       limit,
                              FT_UShort*     min,
                              FT_UShort*     max,
                              GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    *min = 0xFFFFU;
    *max = 0x0000;

    while ( p < limit )
    {
      FT_UShort  val;


      GXV_LIMIT_CHECK( 2 );
      val = FT_NEXT_USHORT( p );

      *min = (FT_Byte)FT_MIN( *min, val );
      *max = (FT_Byte)FT_MAX( *max, val );
    }

    valid->subtable_length = p - table;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                       BINSEARCHHEADER                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  typedef struct  GXV_BinSrchHeader_
  {
    FT_UShort  unitSize;
    FT_UShort  nUnits;
    FT_UShort  searchRange;
    FT_UShort  entrySelector;
    FT_UShort  rangeShift;

  } GXV_BinSrchHeader;


  static void
  gxv_BinSrchHeader_check_consistency( GXV_BinSrchHeader*  binSrchHeader,
                                       GXV_Validator       valid )
  {
    FT_UShort  searchRange;
    FT_UShort  entrySelector;
    FT_UShort  rangeShift;


    if ( binSrchHeader->unitSize == 0 )
      FT_INVALID_DATA;

    if ( binSrchHeader->nUnits == 0 )
    {
      if ( binSrchHeader->searchRange   == 0 &&
           binSrchHeader->entrySelector == 0 &&
           binSrchHeader->rangeShift    == 0 )
        return;
      else
        FT_INVALID_DATA;
    }

    for ( searchRange = 1, entrySelector = 1;
          ( searchRange * 2 ) <= binSrchHeader->nUnits &&
            searchRange < 0x8000U;
          searchRange *= 2, entrySelector++ )
      ;

    entrySelector--;
    searchRange = (FT_UShort)( searchRange * binSrchHeader->unitSize );
    rangeShift  = (FT_UShort)( binSrchHeader->nUnits * binSrchHeader->unitSize
                               - searchRange );

    if ( searchRange   != binSrchHeader->searchRange   ||
         entrySelector != binSrchHeader->entrySelector ||
         rangeShift    != binSrchHeader->rangeShift    )
    {
      GXV_TRACE(( "Inconsistency found in BinSrchHeader\n" ));
      GXV_TRACE(( "originally: unitSize=%d, nUnits=%d, "
                  "searchRange=%d, entrySelector=%d, "
                  "rangeShift=%d\n",
                  binSrchHeader->unitSize, binSrchHeader->nUnits,
                  binSrchHeader->searchRange, binSrchHeader->entrySelector,
                  binSrchHeader->rangeShift ));
      GXV_TRACE(( "calculated: unitSize=%d, nUnits=%d, "
                  "searchRange=%d, entrySelector=%d, "
                  "rangeShift=%d\n",
                  binSrchHeader->unitSize, binSrchHeader->nUnits,
                  searchRange, entrySelector, rangeShift ));

      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }
  }


  /*
   * parser & validator of BinSrchHeader
   * which is used in LookupTable format 2, 4, 6.
   *
   * Essential parameters (unitSize, nUnits) are returned by
   * given pointer, others (searchRange, entrySelector, rangeShift)
   * can be calculated by essential parameters, so they are just
   * validated and discarded.
   *
   * However, wrong values in searchRange, entrySelector, rangeShift
   * won't cause fatal errors, because these parameters might be
   * only used in old m68k font driver in MacOS.
   *   -- suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>
   */

  FT_LOCAL_DEF( void )
  gxv_BinSrchHeader_validate( FT_Bytes       table,
                              FT_Bytes       limit,
                              FT_UShort*     unitSize_p,
                              FT_UShort*     nUnits_p,
                              GXV_Validator  valid )
  {
    FT_Bytes           p = table;
    GXV_BinSrchHeader  binSrchHeader;


    GXV_NAME_ENTER( "BinSrchHeader validate" );

    if ( *unitSize_p == 0 )
    {
      GXV_LIMIT_CHECK( 2 );
      binSrchHeader.unitSize =  FT_NEXT_USHORT( p );
    }
    else
      binSrchHeader.unitSize = *unitSize_p;

    if ( *nUnits_p == 0 )
    {
      GXV_LIMIT_CHECK( 2 );
      binSrchHeader.nUnits = FT_NEXT_USHORT( p );
    }
    else
      binSrchHeader.nUnits = *nUnits_p;

    GXV_LIMIT_CHECK( 2 + 2 + 2 );
    binSrchHeader.searchRange   = FT_NEXT_USHORT( p );
    binSrchHeader.entrySelector = FT_NEXT_USHORT( p );
    binSrchHeader.rangeShift    = FT_NEXT_USHORT( p );
    GXV_TRACE(( "nUnits %d\n", binSrchHeader.nUnits ));

    gxv_BinSrchHeader_check_consistency( &binSrchHeader, valid );

    if ( *unitSize_p == 0 )
      *unitSize_p = binSrchHeader.unitSize;

    if ( *nUnits_p == 0 )
      *nUnits_p = binSrchHeader.nUnits;

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         LOOKUP TABLE                          *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define GXV_LOOKUP_VALUE_LOAD( P, SIGNSPEC )                   \
          ( P += 2, gxv_lookup_value_load( P - 2, SIGNSPEC ) )

  static GXV_LookupValueDesc
  gxv_lookup_value_load( FT_Bytes  p,
                         int       signspec )
  {
    GXV_LookupValueDesc  v;


    if ( signspec == GXV_LOOKUPVALUE_UNSIGNED )
      v.u = FT_NEXT_USHORT( p );
    else
      v.s = FT_NEXT_SHORT( p );

    return v;
  }


#define GXV_UNITSIZE_VALIDATE( FORMAT, UNITSIZE, NUNITS, CORRECTSIZE ) \
          FT_BEGIN_STMNT                                               \
            if ( UNITSIZE != CORRECTSIZE )                             \
            {                                                          \
              FT_ERROR(( "unitSize=%d differs from"                    \
                         " expected unitSize=%d"                       \
                         " in LookupTable %s\n",                       \
                          UNITSIZE, CORRECTSIZE, FORMAT ));            \
              if ( UNITSIZE != 0 && NUNITS != 0 )                      \
              {                                                        \
                FT_ERROR(( " cannot validate anymore\n" ));            \
                FT_INVALID_FORMAT;                                     \
              }                                                        \
              else                                                     \
                FT_ERROR(( " forcibly continues\n" ));                 \
            }                                                          \
          FT_END_STMNT


  /* ================= Simple Array Format 0 Lookup Table ================ */
  static void
  gxv_LookupTable_fmt0_validate( FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  i;

    GXV_LookupValueDesc  value;


    GXV_NAME_ENTER( "LookupTable format 0" );

    GXV_LIMIT_CHECK( 2 * valid->face->num_glyphs );

    for ( i = 0; i < valid->face->num_glyphs; i++ )
    {
      GXV_LIMIT_CHECK( 2 );
      if ( p + 2 >= limit )     /* some fonts have too-short fmt0 array */
      {
        GXV_TRACE(( "too short, glyphs %d - %d are missing\n",
                    i, valid->face->num_glyphs ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
        break;
      }

      value = GXV_LOOKUP_VALUE_LOAD( p, valid->lookupval_sign );
      valid->lookupval_func( i, &value, valid );
    }

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  /* ================= Segment Single Format 2 Loolup Table ============== */
  /*
   * Apple spec says:
   *
   *   To guarantee that a binary search terminates, you must include one or
   *   more special `end of search table' values at the end of the data to
   *   be searched.  The number of termination values that need to be
   *   included is table-specific.  The value that indicates binary search
   *   termination is 0xFFFF.
   *
   * The problem is that nUnits does not include this end-marker.  It's
   * quite difficult to discriminate whether the following 0xFFFF comes from
   * the end-marker or some next data.
   *
   *   -- suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>
   */
  static void
  gxv_LookupTable_fmt2_skip_endmarkers( FT_Bytes       table,
                                        FT_UShort      unitSize,
                                        GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    while ( ( p + 4 ) < valid->root->limit )
    {
      if ( p[0] != 0xFF || p[1] != 0xFF || /* lastGlyph */
           p[2] != 0xFF || p[3] != 0xFF )  /* firstGlyph */
        break;
      p += unitSize;
    }

    valid->subtable_length = p - table;
  }


  static void
  gxv_LookupTable_fmt2_validate( FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid )
  {
    FT_Bytes             p = table;
    FT_UShort            gid;

    FT_UShort            unitSize;
    FT_UShort            nUnits;
    FT_UShort            unit;
    FT_UShort            lastGlyph;
    FT_UShort            firstGlyph;
    GXV_LookupValueDesc  value;


    GXV_NAME_ENTER( "LookupTable format 2" );

    unitSize = nUnits = 0;
    gxv_BinSrchHeader_validate( p, limit, &unitSize, &nUnits, valid );
    p += valid->subtable_length;

    GXV_UNITSIZE_VALIDATE( "format2", unitSize, nUnits, 6 );

    for ( unit = 0, gid = 0; unit < nUnits; unit++ )
    {
      GXV_LIMIT_CHECK( 2 + 2 + 2 );
      lastGlyph  = FT_NEXT_USHORT( p );
      firstGlyph = FT_NEXT_USHORT( p );
      value      = GXV_LOOKUP_VALUE_LOAD( p, valid->lookupval_sign );

      gxv_glyphid_validate( firstGlyph, valid );
      gxv_glyphid_validate( lastGlyph, valid );

      if ( lastGlyph < gid )
      {
        GXV_TRACE(( "reverse ordered segment specification:"
                    " lastGlyph[%d]=%d < lastGlyph[%d]=%d\n",
                    unit, lastGlyph, unit - 1 , gid ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
      }

      if ( lastGlyph < firstGlyph )
      {
        GXV_TRACE(( "reverse ordered range specification at unit %d:",
                    " lastGlyph %d < firstGlyph %d ",
                    unit, lastGlyph, firstGlyph ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );

        if ( valid->root->level == FT_VALIDATE_TIGHT )
          continue;     /* ftxvalidator silently skips such an entry */

        FT_TRACE4(( "continuing with exchanged values\n" ));
        gid        = firstGlyph;
        firstGlyph = lastGlyph;
        lastGlyph  = gid;
      }

      for ( gid = firstGlyph; gid <= lastGlyph; gid++ )
        valid->lookupval_func( gid, &value, valid );
    }

    gxv_LookupTable_fmt2_skip_endmarkers( p, unitSize, valid );
    p += valid->subtable_length;

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  /* ================= Segment Array Format 4 Lookup Table =============== */
  static void
  gxv_LookupTable_fmt4_validate( FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid )
  {
    FT_Bytes             p = table;
    FT_UShort            unit;
    FT_UShort            gid;

    FT_UShort            unitSize;
    FT_UShort            nUnits;
    FT_UShort            lastGlyph;
    FT_UShort            firstGlyph;
    GXV_LookupValueDesc  base_value;
    GXV_LookupValueDesc  value;


    GXV_NAME_ENTER( "LookupTable format 4" );

    unitSize = nUnits = 0;
    gxv_BinSrchHeader_validate( p, limit, &unitSize, &nUnits, valid );
    p += valid->subtable_length;

    GXV_UNITSIZE_VALIDATE( "format4", unitSize, nUnits, 6 );

    for ( unit = 0, gid = 0; unit < nUnits; unit++ )
    {
      GXV_LIMIT_CHECK( 2 + 2 );
      lastGlyph  = FT_NEXT_USHORT( p );
      firstGlyph = FT_NEXT_USHORT( p );

      gxv_glyphid_validate( firstGlyph, valid );
      gxv_glyphid_validate( lastGlyph, valid );

      if ( lastGlyph < gid )
      {
        GXV_TRACE(( "reverse ordered segment specification:"
                    " lastGlyph[%d]=%d < lastGlyph[%d]=%d\n",
                    unit, lastGlyph, unit - 1 , gid ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
      }

      if ( lastGlyph < firstGlyph )
      {
        GXV_TRACE(( "reverse ordered range specification at unit %d:",
                    " lastGlyph %d < firstGlyph %d ",
                    unit, lastGlyph, firstGlyph ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );

        if ( valid->root->level == FT_VALIDATE_TIGHT )
          continue; /* ftxvalidator silently skips such an entry */

        FT_TRACE4(( "continuing with exchanged values\n" ));
        gid        = firstGlyph;
        firstGlyph = lastGlyph;
        lastGlyph  = gid;
      }

      GXV_LIMIT_CHECK( 2 );
      base_value = GXV_LOOKUP_VALUE_LOAD( p, GXV_LOOKUPVALUE_UNSIGNED );

      for ( gid = firstGlyph; gid <= lastGlyph; gid++ )
      {
        value = valid->lookupfmt4_trans( (FT_UShort)( gid - firstGlyph ),
                                         &base_value,
                                         limit,
                                         valid );

        valid->lookupval_func( gid, &value, valid );
      }
    }

    gxv_LookupTable_fmt2_skip_endmarkers( p, unitSize, valid );
    p += valid->subtable_length;

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  /* ================= Segment Table Format 6 Lookup Table =============== */
  static void
  gxv_LookupTable_fmt6_skip_endmarkers( FT_Bytes       table,
                                        FT_UShort      unitSize,
                                        GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    while ( p < valid->root->limit )
    {
      if ( p[0] != 0xFF || p[1] != 0xFF )
        break;
      p += unitSize;
    }

    valid->subtable_length = p - table;
  }


  static void
  gxv_LookupTable_fmt6_validate( FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid )
  {
    FT_Bytes             p = table;
    FT_UShort            unit;
    FT_UShort            prev_glyph;

    FT_UShort            unitSize;
    FT_UShort            nUnits;
    FT_UShort            glyph;
    GXV_LookupValueDesc  value;


    GXV_NAME_ENTER( "LookupTable format 6" );

    unitSize = nUnits = 0;
    gxv_BinSrchHeader_validate( p, limit, &unitSize, &nUnits, valid );
    p += valid->subtable_length;

    GXV_UNITSIZE_VALIDATE( "format6", unitSize, nUnits, 4 );

    for ( unit = 0, prev_glyph = 0; unit < nUnits; unit++ )
    {
      GXV_LIMIT_CHECK( 2 + 2 );
      glyph = FT_NEXT_USHORT( p );
      value = GXV_LOOKUP_VALUE_LOAD( p, valid->lookupval_sign );

      if ( gxv_glyphid_validate( glyph, valid ) )
        GXV_TRACE(( " endmarker found within defined range"
                    " (entry %d < nUnits=%d)\n",
                    unit, nUnits ));

      if ( prev_glyph > glyph )
      {
        GXV_TRACE(( "current gid 0x%04x < previous gid 0x%04x\n",
                    glyph, prev_glyph ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
      }
      prev_glyph = glyph;

      valid->lookupval_func( glyph, &value, valid );
    }

    gxv_LookupTable_fmt6_skip_endmarkers( p, unitSize, valid );
    p += valid->subtable_length;

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  /* ================= Trimmed Array Format 8 Lookup Table =============== */
  static void
  gxv_LookupTable_fmt8_validate( FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid )
  {
    FT_Bytes              p = table;
    FT_UShort             i;

    GXV_LookupValueDesc   value;
    FT_UShort             firstGlyph;
    FT_UShort             glyphCount;


    GXV_NAME_ENTER( "LookupTable format 8" );

    /* firstGlyph + glyphCount */
    GXV_LIMIT_CHECK( 2 + 2 );
    firstGlyph = FT_NEXT_USHORT( p );
    glyphCount = FT_NEXT_USHORT( p );

    gxv_glyphid_validate( firstGlyph, valid );
    gxv_glyphid_validate( (FT_UShort)( firstGlyph + glyphCount ), valid );

    /* valueArray */
    for ( i = 0; i < glyphCount; i++ )
    {
      GXV_LIMIT_CHECK( 2 );
      value = GXV_LOOKUP_VALUE_LOAD( p, valid->lookupval_sign );
      valid->lookupval_func( (FT_UShort)( firstGlyph + i ), &value, valid );
    }

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_LookupTable_validate( FT_Bytes       table,
                            FT_Bytes       limit,
                            GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  format;

    GXV_Validate_Func  fmt_funcs_table[] =
    {
      gxv_LookupTable_fmt0_validate, /* 0 */
      NULL,                          /* 1 */
      gxv_LookupTable_fmt2_validate, /* 2 */
      NULL,                          /* 3 */
      gxv_LookupTable_fmt4_validate, /* 4 */
      NULL,                          /* 5 */
      gxv_LookupTable_fmt6_validate, /* 6 */
      NULL,                          /* 7 */
      gxv_LookupTable_fmt8_validate, /* 8 */
    };

    GXV_Validate_Func  func;


    GXV_NAME_ENTER( "LookupTable" );

    /* lookuptbl_head may be used in fmt4 transit function. */
    valid->lookuptbl_head = table;

    /* format */
    GXV_LIMIT_CHECK( 2 );
    format = FT_NEXT_USHORT( p );
    GXV_TRACE(( " (format %d)\n", format ));

    if ( format > 8 )
      FT_INVALID_FORMAT;

    func = fmt_funcs_table[format];
    if ( func == NULL )
      FT_INVALID_FORMAT;

    func( p, limit, valid );
    p += valid->subtable_length;

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          Glyph ID                             *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( FT_Int )
  gxv_glyphid_validate( FT_UShort      gid,
                        GXV_Validator  valid )
  {
    FT_Face  face;


    if ( gid == 0xFFFFU )
    {
      GXV_EXIT;
      return 1;
    }

    face = valid->face;
    if ( face->num_glyphs < gid )
    {
      GXV_TRACE(( " gxv_glyphid_check() gid overflow: num_glyphs %d < %d\n",
                  face->num_glyphs, gid ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
    }

    return 0;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                        CONTROL POINT                          *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_ctlPoint_validate( FT_UShort      gid,
                         FT_Short       ctl_point,
                         GXV_Validator  valid )
  {
    FT_Face       face;
    FT_Error      error;

    FT_GlyphSlot  glyph;
    FT_Outline    outline;
    short         n_points;


    face = valid->face;

    error = FT_Load_Glyph( face,
                           gid,
                           FT_LOAD_NO_BITMAP | FT_LOAD_IGNORE_TRANSFORM );
    if ( error )
      FT_INVALID_GLYPH_ID;

    glyph    = face->glyph;
    outline  = glyph->outline;
    n_points = outline.n_points;


    if ( !( ctl_point < n_points ) )
      FT_INVALID_DATA;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          SFNT NAME                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_sfntName_validate( FT_UShort      name_index,
                         FT_UShort      min_index,
                         FT_UShort      max_index,
                         GXV_Validator  valid )
  {
    FT_SfntName  name;
    FT_UInt      i;
    FT_UInt      nnames;


    GXV_NAME_ENTER( "sfntName" );

    if ( name_index < min_index || max_index < name_index )
      FT_INVALID_FORMAT;

    nnames = FT_Get_Sfnt_Name_Count( valid->face );
    for ( i = 0; i < nnames; i++ )
    {
      if ( FT_Get_Sfnt_Name( valid->face, i, &name ) != FT_Err_Ok )
        continue ;

      if ( name.name_id == name_index )
        goto Out;
    }

    GXV_TRACE(( "  nameIndex = %d (UNTITLED)\n", name_index ));
    FT_INVALID_DATA;
    goto Exit;  /* make compiler happy */

  Out:
    FT_TRACE1(( "  nameIndex = %d (", name_index ));
    GXV_TRACE_HEXDUMP_SFNTNAME( name );
    FT_TRACE1(( ")\n" ));

  Exit:
    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          STATE TABLE                          *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* -------------------------- Class Table --------------------------- */

  /*
   * highestClass specifies how many classes are defined in this
   * Class Subtable.  Apple spec does not mention whether undefined
   * holes in the class (e.g.: 0-3 are predefined, 4 is unused, 5 is used)
   * are permitted.  At present, holes in a defined class are not checked.
   *   -- suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>
   */

  static void
  gxv_ClassTable_validate( FT_Bytes       table,
                           FT_UShort*     length_p,
                           FT_UShort      stateSize,
                           FT_Byte*       maxClassID_p,
                           GXV_Validator  valid )
  {
    FT_Bytes   p     = table;
    FT_Bytes   limit = table + *length_p;
    FT_UShort  firstGlyph;
    FT_UShort  nGlyphs;


    GXV_NAME_ENTER( "ClassTable" );

    *maxClassID_p = 3;  /* Classes 0, 2, and 3 are predefined */

    GXV_LIMIT_CHECK( 2 + 2 );
    firstGlyph = FT_NEXT_USHORT( p );
    nGlyphs    = FT_NEXT_USHORT( p );

    GXV_TRACE(( " (firstGlyph = %d, nGlyphs = %d)\n", firstGlyph, nGlyphs ));

    if ( !nGlyphs )
      goto Out;

    gxv_glyphid_validate( (FT_UShort)( firstGlyph + nGlyphs ), valid );

    {
      FT_Byte    nGlyphInClass[256];
      FT_Byte    classID;
      FT_UShort  i;


      ft_memset( nGlyphInClass, 0, 256 );


      for ( i = 0; i < nGlyphs; i++ )
      {
        GXV_LIMIT_CHECK( 1 );
        classID = FT_NEXT_BYTE( p );
        switch ( classID )
        {
          /* following classes should not appear in class array */
        case 0:             /* end of text */
        case 2:             /* out of bounds */
        case 3:             /* end of line */
          FT_INVALID_DATA;
          break;

        case 1:             /* out of bounds */
        default:            /* user-defined: 4 - ( stateSize - 1 ) */
          if ( classID >= stateSize )
            FT_INVALID_DATA;   /* assign glyph to undefined state */

          nGlyphInClass[classID]++;
          break;
        }
      }
      *length_p = (FT_UShort)( p - table );

      /* scan max ClassID in use */
      for ( i = 0; i < stateSize; i++ )
        if ( ( 3 < i ) && ( nGlyphInClass[i] > 0 ) )
          *maxClassID_p = (FT_Byte)i;  /* XXX: Check Range? */
    }

  Out:
    GXV_TRACE(( "Declared stateSize=0x%02x, Used maxClassID=0x%02x\n",
                stateSize, *maxClassID_p ));
    GXV_EXIT;
  }


  /* --------------------------- State Array ----------------------------- */

  static void
  gxv_StateArray_validate( FT_Bytes       table,
                           FT_UShort*     length_p,
                           FT_Byte        maxClassID,
                           FT_UShort      stateSize,
                           FT_Byte*       maxState_p,
                           FT_Byte*       maxEntry_p,
                           GXV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_Bytes  limit = table + *length_p;
    FT_Byte   clazz;
    FT_Byte   entry;

    FT_UNUSED( stateSize ); /* for the non-debugging case */


    GXV_NAME_ENTER( "StateArray" );

    GXV_TRACE(( "parse %d bytes by stateSize=%d maxClassID=%d\n",
                (int)(*length_p), stateSize, (int)(maxClassID) ));

    /*
     * 2 states are predefined and must be described in StateArray:
     * state 0 (start of text), 1 (start of line)
     */
    GXV_LIMIT_CHECK( ( 1 + maxClassID ) * 2 );

    *maxState_p = 0;
    *maxEntry_p = 0;

    /* read if enough to read another state */
    while ( p + ( 1 + maxClassID ) <= limit )
    {
      (*maxState_p)++;
      for ( clazz = 0; clazz <= maxClassID; clazz++ )
      {
        entry = FT_NEXT_BYTE( p );
        *maxEntry_p = (FT_Byte)FT_MAX( *maxEntry_p, entry );
      }
    }
    GXV_TRACE(( "parsed: maxState=%d, maxEntry=%d\n",
                *maxState_p, *maxEntry_p ));

    *length_p = (FT_UShort)( p - table );

    GXV_EXIT;
  }


  /* --------------------------- Entry Table ----------------------------- */

  static void
  gxv_EntryTable_validate( FT_Bytes       table,
                           FT_UShort*     length_p,
                           FT_Byte        maxEntry,
                           FT_UShort      stateArray,
                           FT_UShort      stateArray_length,
                           FT_Byte        maxClassID,
                           FT_Bytes       statetable_table,
                           FT_Bytes       statetable_limit,
                           GXV_Validator  valid )
  {
    FT_Bytes  p     = table;
    FT_Bytes  limit = table + *length_p;
    FT_Byte   entry;
    FT_Byte   state;
    FT_Int    entrySize = 2 + 2 + GXV_GLYPHOFFSET_SIZE( statetable );

    GXV_XStateTable_GlyphOffsetDesc  glyphOffset;


    GXV_NAME_ENTER( "EntryTable" );

    GXV_TRACE(( "maxEntry=%d entrySize=%d\n", maxEntry, entrySize ));

    if ( ( maxEntry + 1 ) * entrySize > *length_p )
    {
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_TOO_SHORT );

      /* ftxvalidator and FontValidator both warn and continue */
      maxEntry = (FT_Byte)( *length_p / entrySize - 1 );
      GXV_TRACE(( "too large maxEntry, shrinking to %d fit EntryTable length\n",
                  maxEntry ));
    }

    for ( entry = 0; entry <= maxEntry; entry++ )
    {
      FT_UShort  newState;
      FT_UShort  flags;


      GXV_LIMIT_CHECK( 2 + 2 );
      newState = FT_NEXT_USHORT( p );
      flags    = FT_NEXT_USHORT( p );


      if ( newState < stateArray                     ||
           stateArray + stateArray_length < newState )
      {
        GXV_TRACE(( " newState offset 0x%04x is out of stateArray\n",
                    newState ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
        continue;
      }

      if ( 0 != ( ( newState - stateArray ) % ( 1 + maxClassID ) ) )
      {
        GXV_TRACE(( " newState offset 0x%04x is not aligned to %d-classes\n",
                    newState,  1 + maxClassID ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
        continue;
      }

      state = (FT_Byte)( ( newState - stateArray ) / ( 1 + maxClassID ) );

      switch ( GXV_GLYPHOFFSET_FMT( statetable ) )
      {
      case GXV_GLYPHOFFSET_NONE:
        glyphOffset.uc = 0;  /* make compiler happy */
        break;

      case GXV_GLYPHOFFSET_UCHAR:
        glyphOffset.uc = FT_NEXT_BYTE( p );
        break;

      case GXV_GLYPHOFFSET_CHAR:
        glyphOffset.c = FT_NEXT_CHAR( p );
        break;

      case GXV_GLYPHOFFSET_USHORT:
        glyphOffset.u = FT_NEXT_USHORT( p );
        break;

      case GXV_GLYPHOFFSET_SHORT:
        glyphOffset.s = FT_NEXT_SHORT( p );
        break;

      case GXV_GLYPHOFFSET_ULONG:
        glyphOffset.ul = FT_NEXT_ULONG( p );
        break;

      case GXV_GLYPHOFFSET_LONG:
        glyphOffset.l = FT_NEXT_LONG( p );
        break;

      default:
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_FORMAT );
        goto Exit;
      }

      if ( NULL != valid->statetable.entry_validate_func )
        valid->statetable.entry_validate_func( state,
                                               flags,
                                               &glyphOffset,
                                               statetable_table,
                                               statetable_limit,
                                               valid );
    }

  Exit:
    *length_p = (FT_UShort)( p - table );

    GXV_EXIT;
  }


  /* =========================== State Table ============================= */

  FT_LOCAL_DEF( void )
  gxv_StateTable_subtable_setup( FT_UShort      table_size,
                                 FT_UShort      classTable,
                                 FT_UShort      stateArray,
                                 FT_UShort      entryTable,
                                 FT_UShort*     classTable_length_p,
                                 FT_UShort*     stateArray_length_p,
                                 FT_UShort*     entryTable_length_p,
                                 GXV_Validator  valid )
  {
    FT_UShort   o[3];
    FT_UShort*  l[3];
    FT_UShort   buff[4];


    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;

    gxv_set_length_by_ushort_offset( o, l, buff, 3, table_size, valid );
  }


  FT_LOCAL_DEF( void )
  gxv_StateTable_validate( FT_Bytes       table,
                           FT_Bytes       limit,
                           GXV_Validator  valid )
  {
    FT_UShort   stateSize;
    FT_UShort   classTable;     /* offset to Class(Sub)Table */
    FT_UShort   stateArray;     /* offset to StateArray */
    FT_UShort   entryTable;     /* offset to EntryTable */

    FT_UShort   classTable_length;
    FT_UShort   stateArray_length;
    FT_UShort   entryTable_length;
    FT_Byte     maxClassID;
    FT_Byte     maxState;
    FT_Byte     maxEntry;

    GXV_StateTable_Subtable_Setup_Func  setup_func;

    FT_Bytes    p = table;


    GXV_NAME_ENTER( "StateTable" );

    GXV_TRACE(( "StateTable header\n" ));

    GXV_LIMIT_CHECK( 2 + 2 + 2 + 2 );
    stateSize  = FT_NEXT_USHORT( p );
    classTable = FT_NEXT_USHORT( p );
    stateArray = FT_NEXT_USHORT( p );
    entryTable = FT_NEXT_USHORT( p );

    GXV_TRACE(( "stateSize=0x%04x\n", stateSize ));
    GXV_TRACE(( "offset to classTable=0x%04x\n", classTable ));
    GXV_TRACE(( "offset to stateArray=0x%04x\n", stateArray ));
    GXV_TRACE(( "offset to entryTable=0x%04x\n", entryTable ));

    if ( stateSize > 0xFF )
      FT_INVALID_DATA;

    if ( valid->statetable.optdata_load_func != NULL )
      valid->statetable.optdata_load_func( p, limit, valid );

    if ( valid->statetable.subtable_setup_func != NULL)
      setup_func = valid->statetable.subtable_setup_func;
    else
      setup_func = gxv_StateTable_subtable_setup;

    setup_func( (FT_UShort)( limit - table ),
                classTable,
                stateArray,
                entryTable,
                &classTable_length,
                &stateArray_length,
                &entryTable_length,
                valid );

    GXV_TRACE(( "StateTable Subtables\n" ));

    if ( classTable != 0 )
      gxv_ClassTable_validate( table + classTable,
                               &classTable_length,
                               stateSize,
                               &maxClassID,
                               valid );
    else
      maxClassID = (FT_Byte)( stateSize - 1 );

    if ( stateArray != 0 )
      gxv_StateArray_validate( table + stateArray,
                               &stateArray_length,
                               maxClassID,
                               stateSize,
                               &maxState,
                               &maxEntry,
                               valid );
    else
    {
#if 0
      maxState = 1;     /* 0:start of text, 1:start of line are predefined */
#endif
      maxEntry = 0;
    }

    if ( maxEntry > 0 && entryTable == 0 )
      FT_INVALID_OFFSET;

    if ( entryTable != 0 )
      gxv_EntryTable_validate( table + entryTable,
                               &entryTable_length,
                               maxEntry,
                               stateArray,
                               stateArray_length,
                               maxClassID,
                               table,
                               limit,
                               valid );

    GXV_EXIT;
  }


  /* ================= eXtended State Table (for morx) =================== */

  FT_LOCAL_DEF( void )
  gxv_XStateTable_subtable_setup( FT_ULong       table_size,
                                  FT_ULong       classTable,
                                  FT_ULong       stateArray,
                                  FT_ULong       entryTable,
                                  FT_ULong*      classTable_length_p,
                                  FT_ULong*      stateArray_length_p,
                                  FT_ULong*      entryTable_length_p,
                                  GXV_Validator  valid )
  {
    FT_ULong   o[3];
    FT_ULong*  l[3];
    FT_ULong   buff[4];


    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;

    gxv_set_length_by_ulong_offset( o, l, buff, 3, table_size, valid );
  }


  static void
  gxv_XClassTable_lookupval_validate( FT_UShort            glyph,
                                      GXV_LookupValueCPtr  value_p,
                                      GXV_Validator        valid )
  {
    FT_UNUSED( glyph );

    if ( value_p->u >= valid->xstatetable.nClasses )
      FT_INVALID_DATA;
    if ( value_p->u > valid->xstatetable.maxClassID )
      valid->xstatetable.maxClassID = value_p->u;
  }


  /*
    +===============+ --------+
    | lookup header |         |
    +===============+         |
    | BinSrchHeader |         |
    +===============+         |
    | lastGlyph[0]  |         |
    +---------------+         |
    | firstGlyph[0] |         |    head of lookup table
    +---------------+         |             +
    | offset[0]     |    ->   |          offset            [byte]
    +===============+         |             +
    | lastGlyph[1]  |         | (glyphID - firstGlyph) * 2 [byte]
    +---------------+         |
    | firstGlyph[1] |         |
    +---------------+         |
    | offset[1]     |         |
    +===============+         |
                              |
     ....                     |
                              |
    16bit value array         |
    +===============+         |
    |     value     | <-------+
     ....
  */
  static GXV_LookupValueDesc
  gxv_XClassTable_lookupfmt4_transit( FT_UShort            relative_gindex,
                                      GXV_LookupValueCPtr  base_value_p,
                                      FT_Bytes             lookuptbl_limit,
                                      GXV_Validator        valid )
  {
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;

    /* XXX: check range? */
    offset = (FT_UShort)( base_value_p->u +
                          relative_gindex * sizeof ( FT_UShort ) );

    p     = valid->lookuptbl_head + offset;
    limit = lookuptbl_limit;

    GXV_LIMIT_CHECK ( 2 );
    value.u = FT_NEXT_USHORT( p );

    return value;
  }


  static void
  gxv_XStateArray_validate( FT_Bytes       table,
                            FT_ULong*      length_p,
                            FT_UShort      maxClassID,
                            FT_ULong       stateSize,
                            FT_UShort*     maxState_p,
                            FT_UShort*     maxEntry_p,
                            GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_Bytes   limit = table + *length_p;
    FT_UShort  clazz;
    FT_UShort  entry;

    FT_UNUSED( stateSize ); /* for the non-debugging case */


    GXV_NAME_ENTER( "XStateArray" );

    GXV_TRACE(( "parse % 3d bytes by stateSize=% 3d maxClassID=% 3d\n",
                (int)(*length_p), stateSize, (int)(maxClassID) ));

    /*
     * 2 states are predefined and must be described:
     * state 0 (start of text), 1 (start of line)
     */
    GXV_LIMIT_CHECK( ( 1 + maxClassID ) * 2 * 2 );

    *maxState_p = 0;
    *maxEntry_p = 0;

    /* read if enough to read another state */
    while ( p + ( ( 1 + maxClassID ) * 2 ) <= limit )
    {
      (*maxState_p)++;
      for ( clazz = 0; clazz <= maxClassID; clazz++ )
      {
        entry = FT_NEXT_USHORT( p );
        *maxEntry_p = (FT_UShort)FT_MAX( *maxEntry_p, entry );
      }
    }
    GXV_TRACE(( "parsed: maxState=%d, maxEntry=%d\n",
                *maxState_p, *maxEntry_p ));

    *length_p = p - table;

    GXV_EXIT;
  }


  static void
  gxv_XEntryTable_validate( FT_Bytes       table,
                            FT_ULong*      length_p,
                            FT_UShort      maxEntry,
                            FT_ULong       stateArray_length,
                            FT_UShort      maxClassID,
                            FT_Bytes       xstatetable_table,
                            FT_Bytes       xstatetable_limit,
                            GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_Bytes   limit = table + *length_p;
    FT_UShort  entry;
    FT_UShort  state;
    FT_Int     entrySize = 2 + 2 + GXV_GLYPHOFFSET_SIZE( xstatetable );


    GXV_NAME_ENTER( "XEntryTable" );
    GXV_TRACE(( "maxEntry=%d entrySize=%d\n", maxEntry, entrySize ));

    if ( ( p + ( maxEntry + 1 ) * entrySize ) > limit )
      FT_INVALID_TOO_SHORT;

    for (entry = 0; entry <= maxEntry ; entry++ )
    {
      FT_UShort                        newState_idx;
      FT_UShort                        flags;
      GXV_XStateTable_GlyphOffsetDesc  glyphOffset;


      GXV_LIMIT_CHECK( 2 + 2 );
      newState_idx = FT_NEXT_USHORT( p );
      flags        = FT_NEXT_USHORT( p );

      if ( stateArray_length < (FT_ULong)( newState_idx * 2 ) )
      {
        GXV_TRACE(( "  newState index 0x%04x points out of stateArray\n",
                    newState_idx ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
      }

      state = (FT_UShort)( newState_idx / ( 1 + maxClassID ) );
      if ( 0 != ( newState_idx % ( 1 + maxClassID ) ) )
      {
        FT_TRACE4(( "-> new state = %d (supposed)\n"
                    "but newState index 0x%04x is not aligned to %d-classes\n",
                    state, newState_idx,  1 + maxClassID ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
      }

      switch ( GXV_GLYPHOFFSET_FMT( xstatetable ) )
      {
      case GXV_GLYPHOFFSET_NONE:
        glyphOffset.uc = 0; /* make compiler happy */
        break;

      case GXV_GLYPHOFFSET_UCHAR:
        glyphOffset.uc = FT_NEXT_BYTE( p );
        break;

      case GXV_GLYPHOFFSET_CHAR:
        glyphOffset.c = FT_NEXT_CHAR( p );
        break;

      case GXV_GLYPHOFFSET_USHORT:
        glyphOffset.u = FT_NEXT_USHORT( p );
        break;

      case GXV_GLYPHOFFSET_SHORT:
        glyphOffset.s = FT_NEXT_SHORT( p );
        break;

      case GXV_GLYPHOFFSET_ULONG:
        glyphOffset.ul = FT_NEXT_ULONG( p );
        break;

      case GXV_GLYPHOFFSET_LONG:
        glyphOffset.l = FT_NEXT_LONG( p );
        break;

      default:
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_FORMAT );
        goto Exit;
      }

      if ( NULL != valid->xstatetable.entry_validate_func )
        valid->xstatetable.entry_validate_func( state,
                                                flags,
                                                &glyphOffset,
                                                xstatetable_table,
                                                xstatetable_limit,
                                                valid );
    }

  Exit:
    *length_p = p - table;

    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_XStateTable_validate( FT_Bytes       table,
                            FT_Bytes       limit,
                            GXV_Validator  valid )
  {
    /* StateHeader members */
    FT_ULong   classTable;      /* offset to Class(Sub)Table */
    FT_ULong   stateArray;      /* offset to StateArray */
    FT_ULong   entryTable;      /* offset to EntryTable */

    FT_ULong   classTable_length;
    FT_ULong   stateArray_length;
    FT_ULong   entryTable_length;
    FT_UShort  maxState;
    FT_UShort  maxEntry;

    GXV_XStateTable_Subtable_Setup_Func  setup_func;

    FT_Bytes   p = table;


    GXV_NAME_ENTER( "XStateTable" );

    GXV_TRACE(( "XStateTable header\n" ));

    GXV_LIMIT_CHECK( 4 + 4 + 4 + 4 );
    valid->xstatetable.nClasses = FT_NEXT_ULONG( p );
    classTable = FT_NEXT_ULONG( p );
    stateArray = FT_NEXT_ULONG( p );
    entryTable = FT_NEXT_ULONG( p );

    GXV_TRACE(( "nClasses =0x%08x\n", valid->xstatetable.nClasses ));
    GXV_TRACE(( "offset to classTable=0x%08x\n", classTable ));
    GXV_TRACE(( "offset to stateArray=0x%08x\n", stateArray ));
    GXV_TRACE(( "offset to entryTable=0x%08x\n", entryTable ));

    if ( valid->xstatetable.nClasses > 0xFFFFU )
      FT_INVALID_DATA;

    GXV_TRACE(( "StateTable Subtables\n" ));

    if ( valid->xstatetable.optdata_load_func != NULL )
      valid->xstatetable.optdata_load_func( p, limit, valid );

    if ( valid->xstatetable.subtable_setup_func != NULL )
      setup_func = valid->xstatetable.subtable_setup_func;
    else
      setup_func = gxv_XStateTable_subtable_setup;

    setup_func( limit - table,
                classTable,
                stateArray,
                entryTable,
                &classTable_length,
                &stateArray_length,
                &entryTable_length,
                valid );

    if ( classTable != 0 )
    {
      valid->xstatetable.maxClassID = 0;
      valid->lookupval_sign         = GXV_LOOKUPVALUE_UNSIGNED;
      valid->lookupval_func         = gxv_XClassTable_lookupval_validate;
      valid->lookupfmt4_trans       = gxv_XClassTable_lookupfmt4_transit;
      gxv_LookupTable_validate( table + classTable,
                                table + classTable + classTable_length,
                                valid );
#if 0
      if ( valid->subtable_length < classTable_length )
        classTable_length = valid->subtable_length;
#endif
    }
    else
    {
      /* XXX: check range? */
      valid->xstatetable.maxClassID =
        (FT_UShort)( valid->xstatetable.nClasses - 1 );
    }

    if ( stateArray != 0 )
      gxv_XStateArray_validate( table + stateArray,
                                &stateArray_length,
                                valid->xstatetable.maxClassID,
                                valid->xstatetable.nClasses,
                                &maxState,
                                &maxEntry,
                                valid );
    else
    {
#if 0
      maxState = 1; /* 0:start of text, 1:start of line are predefined */
#endif
      maxEntry = 0;
    }

    if ( maxEntry > 0 && entryTable == 0 )
      FT_INVALID_OFFSET;

    if ( entryTable != 0 )
      gxv_XEntryTable_validate( table + entryTable,
                                &entryTable_length,
                                maxEntry,
                                stateArray_length,
                                valid->xstatetable.maxClassID,
                                table,
                                limit,
                                valid );

    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                        Table overlapping                      *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static int
  gxv_compare_ranges( FT_Bytes  table1_start,
                      FT_ULong  table1_length,
                      FT_Bytes  table2_start,
                      FT_ULong  table2_length )
  {
    if ( table1_start == table2_start )
    {
      if ( ( table1_length == 0 || table2_length == 0 ) )
        goto Out;
    }
    else if ( table1_start < table2_start )
    {
      if ( ( table1_start + table1_length ) <= table2_start )
        goto Out;
    }
    else if ( table1_start > table2_start )
    {
      if ( ( table1_start >= table2_start + table2_length ) )
        goto Out;
    }
    return 1;

  Out:
    return 0;
  }


  FT_LOCAL_DEF( void )
  gxv_odtect_add_range( FT_Bytes          start,
                        FT_ULong          length,
                        const FT_String*  name,
                        GXV_odtect_Range  odtect )
  {
    odtect->range[odtect->nRanges].start  = start;
    odtect->range[odtect->nRanges].length = length;
    odtect->range[odtect->nRanges].name   = (FT_String*)name;
    odtect->nRanges++;
  }


  FT_LOCAL_DEF( void )
  gxv_odtect_validate( GXV_odtect_Range  odtect,
                       GXV_Validator     valid )
  {
    FT_UInt  i, j;


    GXV_NAME_ENTER( "check overlap among multi ranges" );

    for ( i = 0; i < odtect->nRanges; i++ )
      for ( j = 0; j < i; j++ )
        if ( 0 != gxv_compare_ranges( odtect->range[i].start,
                                      odtect->range[i].length,
                                      odtect->range[j].start,
                                      odtect->range[j].length ) )
        {
#ifdef FT_DEBUG_LEVEL_TRACE
          if ( odtect->range[i].name || odtect->range[j].name )
            GXV_TRACE(( "found overlap between range %d and range %d\n",
                        i, j ));
          else
            GXV_TRACE(( "found overlap between `%s' and `%s\'\n",
                        odtect->range[i].name,
                        odtect->range[j].name ));
#endif
          FT_INVALID_OFFSET;
        }

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvbsln.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT bsln table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvbsln


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define GXV_BSLN_VALUE_COUNT  32
#define GXV_BSLN_VALUE_EMPTY  0xFFFFU


  typedef struct  GXV_bsln_DataRec_
  {
    FT_Bytes   ctlPoints_p;
    FT_UShort  defaultBaseline;

  } GXV_bsln_DataRec, *GXV_bsln_Data;


#define GXV_BSLN_DATA( field )  GXV_TABLE_DATA( bsln, field )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_bsln_LookupValue_validate( FT_UShort            glyph,
                                 GXV_LookupValueCPtr  value_p,
                                 GXV_Validator        valid )
  {
    FT_UShort   v = value_p->u;
    FT_UShort*  ctlPoints;

    FT_UNUSED( glyph );


    GXV_NAME_ENTER( "lookup value" );

    if ( v >= GXV_BSLN_VALUE_COUNT )
      FT_INVALID_DATA;

    ctlPoints = (FT_UShort*)GXV_BSLN_DATA( ctlPoints_p );
    if ( ctlPoints && ctlPoints[v] == GXV_BSLN_VALUE_EMPTY )
      FT_INVALID_DATA;

    GXV_EXIT;
  }


  /*
    +===============+ --------+
    | lookup header |         |
    +===============+         |
    | BinSrchHeader |         |
    +===============+         |
    | lastGlyph[0]  |         |
    +---------------+         |
    | firstGlyph[0] |         |    head of lookup table
    +---------------+         |             +
    | offset[0]     |    ->   |          offset            [byte]
    +===============+         |             +
    | lastGlyph[1]  |         | (glyphID - firstGlyph) * 2 [byte]
    +---------------+         |
    | firstGlyph[1] |         |
    +---------------+         |
    | offset[1]     |         |
    +===============+         |
                              |
    ...                       |
                              |
    16bit value array         |
    +===============+         |
    |     value     | <-------+
    ...
  */

  static GXV_LookupValueDesc
  gxv_bsln_LookupFmt4_transit( FT_UShort            relative_gindex,
                               GXV_LookupValueCPtr  base_value_p,
                               FT_Bytes             lookuptbl_limit,
                               GXV_Validator        valid )
  {
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;

    /* XXX: check range ? */
    offset = (FT_UShort)( base_value_p->u +
                          ( relative_gindex * sizeof ( FT_UShort ) ) );

    p     = valid->lookuptbl_head + offset;
    limit = lookuptbl_limit;
    GXV_LIMIT_CHECK( 2 );

    value.u = FT_NEXT_USHORT( p );

    return value;
  }


  static void
  gxv_bsln_parts_fmt0_validate( FT_Bytes       tables,
                                FT_Bytes       limit,
                                GXV_Validator  valid )
  {
    FT_Bytes  p = tables;


    GXV_NAME_ENTER( "parts format 0" );

    /* deltas */
    GXV_LIMIT_CHECK( 2 * GXV_BSLN_VALUE_COUNT );

    valid->table_data = NULL;      /* No ctlPoints here. */

    GXV_EXIT;
  }


  static void
  gxv_bsln_parts_fmt1_validate( FT_Bytes       tables,
                                FT_Bytes       limit,
                                GXV_Validator  valid )
  {
    FT_Bytes  p = tables;


    GXV_NAME_ENTER( "parts format 1" );

    /* deltas */
    gxv_bsln_parts_fmt0_validate( p, limit, valid );

    /* mappingData */
    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_bsln_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_bsln_LookupFmt4_transit;
    gxv_LookupTable_validate( p + 2 * GXV_BSLN_VALUE_COUNT,
                              limit,
                              valid );

    GXV_EXIT;
  }


  static void
  gxv_bsln_parts_fmt2_validate( FT_Bytes       tables,
                                FT_Bytes       limit,
                                GXV_Validator  valid )
  {
    FT_Bytes   p = tables;

    FT_UShort  stdGlyph;
    FT_UShort  ctlPoint;
    FT_Int     i;

    FT_UShort  defaultBaseline = GXV_BSLN_DATA( defaultBaseline );


    GXV_NAME_ENTER( "parts format 2" );

    GXV_LIMIT_CHECK( 2 + ( 2 * GXV_BSLN_VALUE_COUNT ) );

    /* stdGlyph */
    stdGlyph = FT_NEXT_USHORT( p );
    GXV_TRACE(( " (stdGlyph = %u)\n", stdGlyph ));

    gxv_glyphid_validate( stdGlyph, valid );

    /* Record the position of ctlPoints */
    GXV_BSLN_DATA( ctlPoints_p ) = p;

    /* ctlPoints */
    for ( i = 0; i < GXV_BSLN_VALUE_COUNT; i++ )
    {
      ctlPoint = FT_NEXT_USHORT( p );
      if ( ctlPoint == GXV_BSLN_VALUE_EMPTY )
      {
        if ( i == defaultBaseline )
          FT_INVALID_DATA;
      }
      else
        gxv_ctlPoint_validate( stdGlyph, (FT_Short)ctlPoint, valid );
    }

    GXV_EXIT;
  }


  static void
  gxv_bsln_parts_fmt3_validate( FT_Bytes       tables,
                                FT_Bytes       limit,
                                GXV_Validator  valid)
  {
    FT_Bytes  p = tables;


    GXV_NAME_ENTER( "parts format 3" );

    /* stdGlyph + ctlPoints */
    gxv_bsln_parts_fmt2_validate( p, limit, valid );

    /* mappingData */
    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_bsln_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_bsln_LookupFmt4_transit;
    gxv_LookupTable_validate( p + ( 2 + 2 * GXV_BSLN_VALUE_COUNT ),
                              limit,
                              valid );

    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         bsln TABLE                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_bsln_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;

    GXV_bsln_DataRec  bslnrec;
    GXV_bsln_Data     bsln = &bslnrec;

    FT_Bytes  p     = table;
    FT_Bytes  limit = 0;

    FT_ULong   version;
    FT_UShort  format;
    FT_UShort  defaultBaseline;

    GXV_Validate_Func  fmt_funcs_table [] =
    {
      gxv_bsln_parts_fmt0_validate,
      gxv_bsln_parts_fmt1_validate,
      gxv_bsln_parts_fmt2_validate,
      gxv_bsln_parts_fmt3_validate,
    };


    valid->root       = ftvalid;
    valid->table_data = bsln;
    valid->face       = face;

    FT_TRACE3(( "validating `bsln' table\n" ));
    GXV_INIT;


    GXV_LIMIT_CHECK( 4 + 2 + 2 );
    version         = FT_NEXT_ULONG( p );
    format          = FT_NEXT_USHORT( p );
    defaultBaseline = FT_NEXT_USHORT( p );

    /* only version 1.0 is defined (1996) */
    if ( version != 0x00010000UL )
      FT_INVALID_FORMAT;

    /* only format 1, 2, 3 are defined (1996) */
    GXV_TRACE(( " (format = %d)\n", format ));
    if ( format > 3 )
      FT_INVALID_FORMAT;

    if ( defaultBaseline > 31 )
      FT_INVALID_FORMAT;

    bsln->defaultBaseline = defaultBaseline;

    fmt_funcs_table[format]( p, limit, valid );

    FT_TRACE4(( "\n" ));
  }


/* arch-tag: ebe81143-fdaa-4c68-a4d1-b57227daa3bc
   (do not change this comment) */


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvtrak.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT trak table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvtrak


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

    /*
     * referred track table format specification:
     * http://developer.apple.com/fonts/TTRefMan/RM06/Chap6trak.html
     * last update was 1996.
     * ----------------------------------------------
     * [MINIMUM HEADER]: GXV_TRAK_SIZE_MIN
     * version          (fixed:  32bit) = 0x00010000
     * format           (uint16: 16bit) = 0 is only defined (1996)
     * horizOffset      (uint16: 16bit)
     * vertOffset       (uint16: 16bit)
     * reserved         (uint16: 16bit) = 0
     * ----------------------------------------------
     * [VARIABLE BODY]:
     * horizData
     *   header         ( 2 + 2 + 4
     *   trackTable       + nTracks * ( 4 + 2 + 2 )
     *   sizeTable        + nSizes * 4 )
     * ----------------------------------------------
     * vertData
     *   header         ( 2 + 2 + 4
     *   trackTable       + nTracks * ( 4 + 2 + 2 )
     *   sizeTable        + nSizes * 4 )
     * ----------------------------------------------
     */
  typedef struct  GXV_trak_DataRec_
  {
    FT_UShort  trackValueOffset_min;
    FT_UShort  trackValueOffset_max;

  } GXV_trak_DataRec, *GXV_trak_Data;


#define GXV_TRAK_DATA( FIELD )  GXV_TABLE_DATA( trak, FIELD )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_trak_trackTable_validate( FT_Bytes       table,
                                FT_Bytes       limit,
                                FT_UShort      nTracks,
                                GXV_Validator  valid )
  {
    FT_Bytes   p = table;

    FT_Fixed   track, t;
    FT_UShort  nameIndex;
    FT_UShort  offset;
    FT_UShort  i, j;


    GXV_NAME_ENTER( "trackTable" );

    GXV_TRAK_DATA( trackValueOffset_min ) = 0xFFFFU;
    GXV_TRAK_DATA( trackValueOffset_max ) = 0x0000;

    GXV_LIMIT_CHECK( nTracks * ( 4 + 2 + 2 ) );

    for ( i = 0; i < nTracks; i ++ )
    {
      p = table + i * ( 4 + 2 + 2 );
      track     = FT_NEXT_LONG( p );
      nameIndex = FT_NEXT_USHORT( p );
      offset    = FT_NEXT_USHORT( p );

      if ( offset < GXV_TRAK_DATA( trackValueOffset_min ) )
        GXV_TRAK_DATA( trackValueOffset_min ) = offset;
      if ( offset > GXV_TRAK_DATA( trackValueOffset_max ) )
        GXV_TRAK_DATA( trackValueOffset_max ) = offset;

      gxv_sfntName_validate( nameIndex, 256, 32767, valid );

      for ( j = i; j < nTracks; j ++ )
      {
         p = table + j * ( 4 + 2 + 2 );
         t = FT_NEXT_LONG( p );
         if ( t == track )
           GXV_TRACE(( "duplicated entries found for track value 0x%x\n",
                        track ));
      }
    }

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  static void
  gxv_trak_trackData_validate( FT_Bytes       table,
                               FT_Bytes       limit,
                               GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  nTracks;
    FT_UShort  nSizes;
    FT_ULong   sizeTableOffset;

    GXV_ODTECT( 4, odtect );


    GXV_ODTECT_INIT( odtect );
    GXV_NAME_ENTER( "trackData" );

    /* read the header of trackData */
    GXV_LIMIT_CHECK( 2 + 2 + 4 );
    nTracks         = FT_NEXT_USHORT( p );
    nSizes          = FT_NEXT_USHORT( p );
    sizeTableOffset = FT_NEXT_ULONG( p );

    gxv_odtect_add_range( table, p - table, "trackData header", odtect );

    /* validate trackTable */
    gxv_trak_trackTable_validate( p, limit, nTracks, valid );
    gxv_odtect_add_range( p, valid->subtable_length,
                          "trackTable", odtect );

    /* sizeTable is array of FT_Fixed, don't check contents */
    p = valid->root->base + sizeTableOffset;
    GXV_LIMIT_CHECK( nSizes * 4 );
    gxv_odtect_add_range( p, nSizes * 4, "sizeTable", odtect );

    /* validate trackValueOffet */
    p = valid->root->base + GXV_TRAK_DATA( trackValueOffset_min );
    if ( limit - p < nTracks * nSizes * 2 )
      GXV_TRACE(( "too short trackValue array\n" ));

    p = valid->root->base + GXV_TRAK_DATA( trackValueOffset_max );
    GXV_LIMIT_CHECK( nSizes * 2 );

    gxv_odtect_add_range( valid->root->base
                            + GXV_TRAK_DATA( trackValueOffset_min ),
                          GXV_TRAK_DATA( trackValueOffset_max )
                            - GXV_TRAK_DATA( trackValueOffset_min )
                            + nSizes * 2,
                          "trackValue array", odtect );

    gxv_odtect_validate( odtect, valid );

    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          trak TABLE                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_trak_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    FT_Bytes          p = table;
    FT_Bytes          limit = 0;

    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;
    GXV_trak_DataRec  trakrec;
    GXV_trak_Data     trak = &trakrec;

    FT_ULong   version;
    FT_UShort  format;
    FT_UShort  horizOffset;
    FT_UShort  vertOffset;
    FT_UShort  reserved;


    GXV_ODTECT( 3, odtect );

    GXV_ODTECT_INIT( odtect );
    valid->root       = ftvalid;
    valid->table_data = trak;
    valid->face       = face;

    limit      = valid->root->limit;

    FT_TRACE3(( "validating `trak' table\n" ));
    GXV_INIT;

    GXV_LIMIT_CHECK( 4 + 2 + 2 + 2 + 2 );
    version     = FT_NEXT_ULONG( p );
    format      = FT_NEXT_USHORT( p );
    horizOffset = FT_NEXT_USHORT( p );
    vertOffset  = FT_NEXT_USHORT( p );
    reserved    = FT_NEXT_USHORT( p );

    GXV_TRACE(( " (version = 0x%08x)\n", version ));
    GXV_TRACE(( " (format = 0x%04x)\n", format ));
    GXV_TRACE(( " (horizOffset = 0x%04x)\n", horizOffset ));
    GXV_TRACE(( " (vertOffset = 0x%04x)\n", vertOffset ));
    GXV_TRACE(( " (reserved = 0x%04x)\n", reserved ));

    /* Version 1.0 (always:1996) */
    if ( version != 0x00010000UL )
      FT_INVALID_FORMAT;

    /* format 0 (always:1996) */
    if ( format != 0x0000 )
      FT_INVALID_FORMAT;

    GXV_32BIT_ALIGNMENT_VALIDATE( horizOffset );
    GXV_32BIT_ALIGNMENT_VALIDATE( vertOffset );

    /* Reserved Fixed Value (always) */
    if ( reserved != 0x0000 )
      FT_INVALID_DATA;

    /* validate trackData */
    if ( 0 < horizOffset )
    {
      gxv_trak_trackData_validate( table + horizOffset, limit, valid );
      gxv_odtect_add_range( table + horizOffset, valid->subtable_length,
                            "horizJustData", odtect );
    }

    if ( 0 < vertOffset )
    {
      gxv_trak_trackData_validate( table + vertOffset, limit, valid );
      gxv_odtect_add_range( table + vertOffset, valid->subtable_length,
                            "vertJustData", odtect );
    }

    gxv_odtect_validate( odtect, valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvjust.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT just table validation (body).                         */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"

#include FT_SFNT_NAMES_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvjust

  /*
   * referred `just' table format specification:
   * http://developer.apple.com/fonts/TTRefMan/RM06/Chap6just.html
   * last updated 2000.
   * ----------------------------------------------
   * [JUST HEADER]: GXV_JUST_HEADER_SIZE
   * version     (fixed:  32bit) = 0x00010000
   * format      (uint16: 16bit) = 0 is only defined (2000)
   * horizOffset (uint16: 16bit)
   * vertOffset  (uint16: 16bit)
   * ----------------------------------------------
   */

  typedef struct  GXV_just_DataRec_
  {
    FT_UShort  wdc_offset_max;
    FT_UShort  wdc_offset_min;
    FT_UShort  pc_offset_max;
    FT_UShort  pc_offset_min;

  } GXV_just_DataRec, *GXV_just_Data;


#define  GXV_JUST_DATA( a )  GXV_TABLE_DATA( just, a )


  /* GX just table does not define their subset of GID */
  static void
  gxv_just_check_max_gid( FT_UShort         gid,
                          const FT_String*  msg_tag,
                          GXV_Validator     valid )
  {
    if ( gid < valid->face->num_glyphs )
      return;

    GXV_TRACE(( "just table includes too large %s"
                " GID=%d > %d (in maxp)\n",
                msg_tag, gid, valid->face->num_glyphs ));
    GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
  }


  static void
  gxv_just_wdp_entry_validate( FT_Bytes       table,
                               FT_Bytes       limit,
                               GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_ULong   justClass;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_Fixed   beforeGrowLimit;
    FT_Fixed   beforeShrinkGrowLimit;
    FT_Fixed   afterGrowLimit;
    FT_Fixed   afterShrinkGrowLimit;
    FT_UShort  growFlags;
    FT_UShort  shrinkFlags;
#endif


    GXV_LIMIT_CHECK( 4 + 4 + 4 + 4 + 4 + 2 + 2 );
    justClass             = FT_NEXT_ULONG( p );
#ifndef GXV_LOAD_UNUSED_VARS
    p += 4 + 4 + 4 + 4 + 2 + 2;
#else
    beforeGrowLimit       = FT_NEXT_ULONG( p );
    beforeShrinkGrowLimit = FT_NEXT_ULONG( p );
    afterGrowLimit        = FT_NEXT_ULONG( p );
    afterShrinkGrowLimit  = FT_NEXT_ULONG( p );
    growFlags             = FT_NEXT_USHORT( p );
    shrinkFlags           = FT_NEXT_USHORT( p );
#endif

    /* According to Apple spec, only 7bits in justClass is used */
    if ( ( justClass & 0xFFFFFF80 ) != 0 )
    {
      GXV_TRACE(( "just table includes non-zero value"
                  " in unused justClass higher bits"
                  " of WidthDeltaPair" ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }

    valid->subtable_length = p - table;
  }


  static void
  gxv_just_wdc_entry_validate( FT_Bytes       table,
                               FT_Bytes       limit,
                               GXV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_ULong  count, i;


    GXV_LIMIT_CHECK( 4 );
    count = FT_NEXT_ULONG( p );
    for ( i = 0; i < count; i++ )
    {
      GXV_TRACE(( "validating wdc pair %d/%d\n", i + 1, count ));
      gxv_just_wdp_entry_validate( p, limit, valid );
      p += valid->subtable_length;
    }

    valid->subtable_length = p - table;
  }


  static void
  gxv_just_widthDeltaClusters_validate( FT_Bytes       table,
                                        FT_Bytes       limit,
                                        GXV_Validator  valid )
  {
    FT_Bytes  p         = table ;
    FT_Bytes  wdc_end   = table + GXV_JUST_DATA( wdc_offset_max );
    FT_UInt   i;


    GXV_NAME_ENTER( "just justDeltaClusters" );

    if ( limit <= wdc_end )
      FT_INVALID_OFFSET;

    for ( i = 0; p <= wdc_end; i++ )
    {
      gxv_just_wdc_entry_validate( p, limit, valid );
      p += valid->subtable_length;
    }

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  static void
  gxv_just_actSubrecord_type0_validate( FT_Bytes       table,
                                        FT_Bytes       limit,
                                        GXV_Validator  valid )
  {
    FT_Bytes   p = table;

    FT_Fixed   lowerLimit;
    FT_Fixed   upperLimit;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  order;
#endif
    FT_UShort  decomposedCount;

    FT_UInt    i;


    GXV_LIMIT_CHECK( 4 + 4 + 2 + 2 );
    lowerLimit      = FT_NEXT_ULONG( p );
    upperLimit      = FT_NEXT_ULONG( p );
#ifdef GXV_LOAD_UNUSED_VARS
    order           = FT_NEXT_USHORT( p );
#else
    p += 2;
#endif
    decomposedCount = FT_NEXT_USHORT( p );

    if ( lowerLimit >= upperLimit )
    {
      GXV_TRACE(( "just table includes invalid range spec:"
                  " lowerLimit(%d) > upperLimit(%d)\n"     ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }

    for ( i = 0; i < decomposedCount; i++ )
    {
      FT_UShort glyphs;


      GXV_LIMIT_CHECK( 2 );
      glyphs = FT_NEXT_USHORT( p );
      gxv_just_check_max_gid( glyphs, "type0:glyphs", valid );
    }

    valid->subtable_length = p - table;
  }


  static void
  gxv_just_actSubrecord_type1_validate( FT_Bytes       table,
                                        FT_Bytes       limit,
                                        GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  addGlyph;


    GXV_LIMIT_CHECK( 2 );
    addGlyph = FT_NEXT_USHORT( p );

    gxv_just_check_max_gid( addGlyph, "type1:addGlyph", valid );

    valid->subtable_length = p - table;
  }


  static void
  gxv_just_actSubrecord_type2_validate( FT_Bytes       table,
                                        FT_Bytes       limit,
                                        GXV_Validator  valid )
  {
    FT_Bytes   p = table;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_Fixed   substThreshhold; /* Apple misspelled "Threshhold" */
#endif
    FT_UShort  addGlyph;
    FT_UShort  substGlyph;


    GXV_LIMIT_CHECK( 4 + 2 + 2 );
#ifdef GXV_LOAD_UNUSED_VARS
    substThreshhold = FT_NEXT_ULONG( p );
#else
    p += 4;
#endif
    addGlyph        = FT_NEXT_USHORT( p );
    substGlyph      = FT_NEXT_USHORT( p );

    if ( addGlyph != 0xFFFF )
      gxv_just_check_max_gid( addGlyph, "type2:addGlyph", valid );

    gxv_just_check_max_gid( substGlyph, "type2:substGlyph", valid );

    valid->subtable_length = p - table;
  }


  static void
  gxv_just_actSubrecord_type4_validate( FT_Bytes       table,
                                        FT_Bytes       limit,
                                        GXV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_ULong  variantsAxis;
    FT_Fixed  minimumLimit;
    FT_Fixed  noStretchValue;
    FT_Fixed  maximumLimit;


    GXV_LIMIT_CHECK( 4 + 4 + 4 + 4 );
    variantsAxis   = FT_NEXT_ULONG( p );
    minimumLimit   = FT_NEXT_ULONG( p );
    noStretchValue = FT_NEXT_ULONG( p );
    maximumLimit   = FT_NEXT_ULONG( p );

    valid->subtable_length = p - table;

    if ( variantsAxis != 0x64756374 ) /* 'duct' */
      GXV_TRACE(( "variantsAxis 0x%08x is non default value",
                   variantsAxis ));

    if ( minimumLimit > noStretchValue )
      GXV_TRACE(( "type4:minimumLimit 0x%08x > noStretchValue 0x%08x\n",
                  minimumLimit, noStretchValue ));
    else if ( noStretchValue > maximumLimit )
      GXV_TRACE(( "type4:noStretchValue 0x%08x > maximumLimit 0x%08x\n",
                  noStretchValue, maximumLimit ));
    else if ( !IS_PARANOID_VALIDATION )
      return;

    FT_INVALID_DATA;
  }


  static void
  gxv_just_actSubrecord_type5_validate( FT_Bytes       table,
                                        FT_Bytes       limit,
                                        GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  flags;
    FT_UShort  glyph;


    GXV_LIMIT_CHECK( 2 + 2 );
    flags = FT_NEXT_USHORT( p );
    glyph = FT_NEXT_USHORT( p );

    if ( flags )
      GXV_TRACE(( "type5: nonzero value 0x%04x in unused flags\n",
                   flags ));
    gxv_just_check_max_gid( glyph, "type5:glyph", valid );

    valid->subtable_length = p - table;
  }


  /* parse single actSubrecord */
  static void
  gxv_just_actSubrecord_validate( FT_Bytes       table,
                                  FT_Bytes       limit,
                                  GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  actionClass;
    FT_UShort  actionType;
    FT_ULong   actionLength;


    GXV_NAME_ENTER( "just actSubrecord" );

    GXV_LIMIT_CHECK( 2 + 2 + 4 );
    actionClass  = FT_NEXT_USHORT( p );
    actionType   = FT_NEXT_USHORT( p );
    actionLength = FT_NEXT_ULONG( p );

    /* actionClass is related with justClass using 7bit only */
    if ( ( actionClass & 0xFF80 ) != 0 )
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );

    if ( actionType == 0 )
      gxv_just_actSubrecord_type0_validate( p, limit, valid );
    else if ( actionType == 1 )
      gxv_just_actSubrecord_type1_validate( p, limit, valid );
    else if ( actionType == 2 )
      gxv_just_actSubrecord_type2_validate( p, limit, valid );
    else if ( actionType == 3 )
      ;                         /* Stretch glyph action: no actionData */
    else if ( actionType == 4 )
      gxv_just_actSubrecord_type4_validate( p, limit, valid );
    else if ( actionType == 5 )
      gxv_just_actSubrecord_type5_validate( p, limit, valid );
    else
      FT_INVALID_DATA;

    valid->subtable_length = actionLength;

    GXV_EXIT;
  }


  static void
  gxv_just_pcActionRecord_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_ULong  actionCount;
    FT_ULong  i;


    GXV_LIMIT_CHECK( 4 );
    actionCount = FT_NEXT_ULONG( p );
    GXV_TRACE(( "actionCount = %d\n", actionCount ));

    for ( i = 0; i < actionCount; i++ )
    {
      gxv_just_actSubrecord_validate( p, limit, valid );
      p += valid->subtable_length;
    }

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  static void
  gxv_just_pcTable_LookupValue_entry_validate( FT_UShort            glyph,
                                               GXV_LookupValueCPtr  value_p,
                                               GXV_Validator        valid )
  {
    FT_UNUSED( glyph );

    if ( value_p->u > GXV_JUST_DATA( pc_offset_max ) )
      GXV_JUST_DATA( pc_offset_max ) = value_p->u;
    if ( value_p->u < GXV_JUST_DATA( pc_offset_max ) )
      GXV_JUST_DATA( pc_offset_min ) = value_p->u;
  }


  static void
  gxv_just_pcLookupTable_validate( FT_Bytes       table,
                                   FT_Bytes       limit,
                                   GXV_Validator  valid )
  {
    FT_Bytes p = table;


    GXV_NAME_ENTER( "just pcLookupTable" );
    GXV_JUST_DATA( pc_offset_max ) = 0x0000;
    GXV_JUST_DATA( pc_offset_min ) = 0xFFFFU;

    valid->lookupval_sign = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func = gxv_just_pcTable_LookupValue_entry_validate;

    gxv_LookupTable_validate( p, limit, valid );

    /* subtable_length is set by gxv_LookupTable_validate() */

    GXV_EXIT;
  }


  static void
  gxv_just_postcompTable_validate( FT_Bytes       table,
                                   FT_Bytes       limit,
                                   GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    GXV_NAME_ENTER( "just postcompTable" );

    gxv_just_pcLookupTable_validate( p, limit, valid );
    p += valid->subtable_length;

    gxv_just_pcActionRecord_validate( p, limit, valid );
    p += valid->subtable_length;

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  static void
  gxv_just_classTable_entry_validate(
    FT_Byte                         state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    /* TODO: validate markClass & currentClass */
    FT_UShort  setMark;
    FT_UShort  dontAdvance;
    FT_UShort  markClass;
    FT_UShort  currentClass;
#endif

    FT_UNUSED( state );
    FT_UNUSED( glyphOffset_p );
    FT_UNUSED( table );
    FT_UNUSED( limit );
    FT_UNUSED( valid );

#ifndef GXV_LOAD_UNUSED_VARS
    FT_UNUSED( flags );
#else
    setMark      = (FT_UShort)( ( flags >> 15 ) & 1    );
    dontAdvance  = (FT_UShort)( ( flags >> 14 ) & 1    );
    markClass    = (FT_UShort)( ( flags >> 7  ) & 0x7F );
    currentClass = (FT_UShort)(   flags         & 0x7F );
#endif
  }


  static void
  gxv_just_justClassTable_validate ( FT_Bytes       table,
                                     FT_Bytes       limit,
                                     GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  length;
    FT_UShort  coverage;
    FT_ULong   subFeatureFlags;


    GXV_NAME_ENTER( "just justClassTable" );

    GXV_LIMIT_CHECK( 2 + 2 + 4 );
    length          = FT_NEXT_USHORT( p );
    coverage        = FT_NEXT_USHORT( p );
    subFeatureFlags = FT_NEXT_ULONG( p );

    GXV_TRACE(( "  justClassTable: coverage = 0x%04x (%s) ", coverage ));
    if ( ( coverage & 0x4000 ) == 0  )
      GXV_TRACE(( "ascending\n" ));
    else
      GXV_TRACE(( "descending\n" ));

    if ( subFeatureFlags )
      GXV_TRACE(( "  justClassTable: nonzero value (0x%08x)"
                  " in unused subFeatureFlags\n", subFeatureFlags ));

    valid->statetable.optdata               = NULL;
    valid->statetable.optdata_load_func     = NULL;
    valid->statetable.subtable_setup_func   = NULL;
    valid->statetable.entry_glyphoffset_fmt = GXV_GLYPHOFFSET_NONE;
    valid->statetable.entry_validate_func   =
      gxv_just_classTable_entry_validate;

    gxv_StateTable_validate( p, table + length, valid );

    /* subtable_length is set by gxv_LookupTable_validate() */

    GXV_EXIT;
  }


  static void
  gxv_just_wdcTable_LookupValue_validate( FT_UShort            glyph,
                                          GXV_LookupValueCPtr  value_p,
                                          GXV_Validator        valid )
  {
    FT_UNUSED( glyph );

    if ( value_p->u > GXV_JUST_DATA( wdc_offset_max ) )
      GXV_JUST_DATA( wdc_offset_max ) = value_p->u;
    if ( value_p->u < GXV_JUST_DATA( wdc_offset_min ) )
      GXV_JUST_DATA( wdc_offset_min ) = value_p->u;
  }


  static void
  gxv_just_justData_lookuptable_validate( FT_Bytes       table,
                                          FT_Bytes       limit,
                                          GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    GXV_JUST_DATA( wdc_offset_max ) = 0x0000;
    GXV_JUST_DATA( wdc_offset_min ) = 0xFFFFU;

    valid->lookupval_sign = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func = gxv_just_wdcTable_LookupValue_validate;

    gxv_LookupTable_validate( p, limit, valid );

    /* subtable_length is set by gxv_LookupTable_validate() */

    GXV_EXIT;
  }


  /*
   * gxv_just_justData_validate() parses and validates horizData, vertData.
   */
  static void
  gxv_just_justData_validate( FT_Bytes       table,
                              FT_Bytes       limit,
                              GXV_Validator  valid )
  {
    /*
     * following 3 offsets are measured from the start of `just'
     * (which table points to), not justData
     */
    FT_UShort  justClassTableOffset;
    FT_UShort  wdcTableOffset;
    FT_UShort  pcTableOffset;
    FT_Bytes   p = table;

    GXV_ODTECT( 4, odtect );


    GXV_NAME_ENTER( "just justData" );

    GXV_ODTECT_INIT( odtect );
    GXV_LIMIT_CHECK( 2 + 2 + 2 );
    justClassTableOffset = FT_NEXT_USHORT( p );
    wdcTableOffset       = FT_NEXT_USHORT( p );
    pcTableOffset        = FT_NEXT_USHORT( p );

    GXV_TRACE(( " (justClassTableOffset = 0x%04x)\n", justClassTableOffset ));
    GXV_TRACE(( " (wdcTableOffset = 0x%04x)\n", wdcTableOffset ));
    GXV_TRACE(( " (pcTableOffset = 0x%04x)\n", pcTableOffset ));

    gxv_just_justData_lookuptable_validate( p, limit, valid );
    gxv_odtect_add_range( p, valid->subtable_length,
                          "just_LookupTable", odtect );

    if ( wdcTableOffset )
    {
      gxv_just_widthDeltaClusters_validate(
        valid->root->base + wdcTableOffset, limit, valid );
      gxv_odtect_add_range( valid->root->base + wdcTableOffset,
                            valid->subtable_length, "just_wdcTable", odtect );
    }

    if ( pcTableOffset )
    {
      gxv_just_postcompTable_validate( valid->root->base + pcTableOffset,
                                       limit, valid );
      gxv_odtect_add_range( valid->root->base + pcTableOffset,
                            valid->subtable_length, "just_pcTable", odtect );
    }

    if ( justClassTableOffset )
    {
      gxv_just_justClassTable_validate(
        valid->root->base + justClassTableOffset, limit, valid );
      gxv_odtect_add_range( valid->root->base + justClassTableOffset,
                            valid->subtable_length, "just_justClassTable",
                            odtect );
    }

    gxv_odtect_validate( odtect, valid );

    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_just_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    FT_Bytes           p     = table;
    FT_Bytes           limit = 0;

    GXV_ValidatorRec   validrec;
    GXV_Validator      valid = &validrec;
    GXV_just_DataRec   justrec;
    GXV_just_Data      just = &justrec;

    FT_ULong           version;
    FT_UShort          format;
    FT_UShort          horizOffset;
    FT_UShort          vertOffset;

    GXV_ODTECT( 3, odtect );


    GXV_ODTECT_INIT( odtect );

    valid->root       = ftvalid;
    valid->table_data = just;
    valid->face       = face;

    FT_TRACE3(( "validating `just' table\n" ));
    GXV_INIT;

    limit      = valid->root->limit;

    GXV_LIMIT_CHECK( 4 + 2 + 2 + 2 );
    version     = FT_NEXT_ULONG( p );
    format      = FT_NEXT_USHORT( p );
    horizOffset = FT_NEXT_USHORT( p );
    vertOffset  = FT_NEXT_USHORT( p );
    gxv_odtect_add_range( table, p - table, "just header", odtect );


    /* Version 1.0 (always:2000) */
    GXV_TRACE(( " (version = 0x%08x)\n", version ));
    if ( version != 0x00010000UL )
      FT_INVALID_FORMAT;

    /* format 0 (always:2000) */
    GXV_TRACE(( " (format = 0x%04x)\n", format ));
    if ( format != 0x0000 )
        FT_INVALID_FORMAT;

    GXV_TRACE(( " (horizOffset = %d)\n", horizOffset  ));
    GXV_TRACE(( " (vertOffset = %d)\n", vertOffset  ));


    /* validate justData */
    if ( 0 < horizOffset )
    {
      gxv_just_justData_validate( table + horizOffset, limit, valid );
      gxv_odtect_add_range( table + horizOffset, valid->subtable_length,
                            "horizJustData", odtect );
    }

    if ( 0 < vertOffset )
    {
      gxv_just_justData_validate( table + vertOffset, limit, valid );
      gxv_odtect_add_range( table + vertOffset, valid->subtable_length,
                            "vertJustData", odtect );
    }

    gxv_odtect_validate( odtect, valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmort.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT mort table validation (body).                         */
/*                                                                         */
/*  Copyright 2005, 2013 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmort.h"
#include "gxvfeat.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmort


  static void
  gxv_mort_feature_validate( GXV_mort_feature  f,
                             GXV_Validator     valid )
  {
    if ( f->featureType >= gxv_feat_registry_length )
    {
      GXV_TRACE(( "featureType %d is out of registered range, "
                  "setting %d is unchecked\n",
                  f->featureType, f->featureSetting ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }
    else if ( !gxv_feat_registry[f->featureType].existence )
    {
      GXV_TRACE(( "featureType %d is within registered area "
                  "but undefined, setting %d is unchecked\n",
                  f->featureType, f->featureSetting ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }
    else
    {
      FT_Byte  nSettings_max;


      /* nSettings in gxvfeat.c is halved for exclusive on/off settings */
      nSettings_max = gxv_feat_registry[f->featureType].nSettings;
      if ( gxv_feat_registry[f->featureType].exclusive )
        nSettings_max = (FT_Byte)( 2 * nSettings_max );

      GXV_TRACE(( "featureType %d is registered", f->featureType ));
      GXV_TRACE(( "setting %d", f->featureSetting ));

      if ( f->featureSetting > nSettings_max )
      {
        GXV_TRACE(( "out of defined range %d", nSettings_max ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
      }
      GXV_TRACE(( "\n" ));
    }

    /* TODO: enableFlags must be unique value in specified chain?  */
  }


  /*
   * nFeatureFlags is typed to FT_ULong to accept that in
   * mort (typed FT_UShort) and morx (typed FT_ULong).
   */
  FT_LOCAL_DEF( void )
  gxv_mort_featurearray_validate( FT_Bytes       table,
                                  FT_Bytes       limit,
                                  FT_ULong       nFeatureFlags,
                                  GXV_Validator  valid )
  {
    FT_Bytes  p = table;
    FT_ULong  i;

    GXV_mort_featureRec  f = GXV_MORT_FEATURE_OFF;


    GXV_NAME_ENTER( "mort feature list" );
    for ( i = 0; i < nFeatureFlags; i++ )
    {
      GXV_LIMIT_CHECK( 2 + 2 + 4 + 4 );
      f.featureType    = FT_NEXT_USHORT( p );
      f.featureSetting = FT_NEXT_USHORT( p );
      f.enableFlags    = FT_NEXT_ULONG( p );
      f.disableFlags   = FT_NEXT_ULONG( p );

      gxv_mort_feature_validate( &f, valid );
    }

    if ( !IS_GXV_MORT_FEATURE_OFF( f ) )
      FT_INVALID_DATA;

    valid->subtable_length = p - table;
    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_mort_coverage_validate( FT_UShort      coverage,
                              GXV_Validator  valid )
  {
    FT_UNUSED( valid );

#ifdef FT_DEBUG_LEVEL_TRACE
    if ( coverage & 0x8000U )
      GXV_TRACE(( " this subtable is for vertical text only\n" ));
    else
      GXV_TRACE(( " this subtable is for horizontal text only\n" ));

    if ( coverage & 0x4000 )
      GXV_TRACE(( " this subtable is applied to glyph array "
                  "in descending order\n" ));
    else
      GXV_TRACE(( " this subtable is applied to glyph array "
                  "in ascending order\n" ));

    if ( coverage & 0x2000 )
      GXV_TRACE(( " this subtable is forcibly applied to "
                  "vertical/horizontal text\n" ));

    if ( coverage & 0x1FF8 )
      GXV_TRACE(( " coverage has non-zero bits in reserved area\n" ));
#endif
  }


  static void
  gxv_mort_subtables_validate( FT_Bytes       table,
                               FT_Bytes       limit,
                               FT_UShort      nSubtables,
                               GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_Validate_Func fmt_funcs_table[] =
    {
      gxv_mort_subtable_type0_validate, /* 0 */
      gxv_mort_subtable_type1_validate, /* 1 */
      gxv_mort_subtable_type2_validate, /* 2 */
      NULL,                             /* 3 */
      gxv_mort_subtable_type4_validate, /* 4 */
      gxv_mort_subtable_type5_validate, /* 5 */

    };

    FT_UShort  i;


    GXV_NAME_ENTER( "subtables in a chain" );

    for ( i = 0; i < nSubtables; i++ )
    {
      GXV_Validate_Func  func;

      FT_UShort  length;
      FT_UShort  coverage;
#ifdef GXV_LOAD_UNUSED_VARS
      FT_ULong   subFeatureFlags;
#endif
      FT_UInt    type;
      FT_UInt    rest;


      GXV_LIMIT_CHECK( 2 + 2 + 4 );
      length          = FT_NEXT_USHORT( p );
      coverage        = FT_NEXT_USHORT( p );
#ifdef GXV_LOAD_UNUSED_VARS
      subFeatureFlags = FT_NEXT_ULONG( p );
#else
      p += 4;
#endif

      GXV_TRACE(( "validating chain subtable %d/%d (%d bytes)\n",
                  i + 1, nSubtables, length ));
      type = coverage & 0x0007;
      rest = length - ( 2 + 2 + 4 );

      GXV_LIMIT_CHECK( rest );
      gxv_mort_coverage_validate( coverage, valid );

      if ( type > 5 )
        FT_INVALID_FORMAT;

      func = fmt_funcs_table[type];
      if ( func == NULL )
        GXV_TRACE(( "morx type %d is reserved\n", type ));

      func( p, p + rest, valid );

      p += rest;
      /* TODO: validate subFeatureFlags */
    }

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  static void
  gxv_mort_chain_validate( FT_Bytes       table,
                           FT_Bytes       limit,
                           GXV_Validator  valid )
  {
    FT_Bytes   p = table;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_ULong   defaultFlags;
#endif
    FT_ULong   chainLength;
    FT_UShort  nFeatureFlags;
    FT_UShort  nSubtables;


    GXV_NAME_ENTER( "mort chain header" );

    GXV_LIMIT_CHECK( 4 + 4 + 2 + 2 );
#ifdef GXV_LOAD_UNUSED_VARS
    defaultFlags  = FT_NEXT_ULONG( p );
#else
    p += 4;
#endif
    chainLength   = FT_NEXT_ULONG( p );
    nFeatureFlags = FT_NEXT_USHORT( p );
    nSubtables    = FT_NEXT_USHORT( p );

    gxv_mort_featurearray_validate( p, table + chainLength,
                                    nFeatureFlags, valid );
    p += valid->subtable_length;
    gxv_mort_subtables_validate( p, table + chainLength, nSubtables, valid );
    valid->subtable_length = chainLength;

    /* TODO: validate defaultFlags */
    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_mort_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;
    FT_ULong          version;
    FT_ULong          nChains;
    FT_ULong          i;


    valid->root = ftvalid;
    valid->face = face;
    limit       = valid->root->limit;

    FT_TRACE3(( "validating `mort' table\n" ));
    GXV_INIT;

    GXV_LIMIT_CHECK( 4 + 4 );
    version = FT_NEXT_ULONG( p );
    nChains = FT_NEXT_ULONG( p );

    if (version != 0x00010000UL)
      FT_INVALID_FORMAT;

    for ( i = 0; i < nChains; i++ )
    {
      GXV_TRACE(( "validating chain %d/%d\n", i + 1, nChains ));
      GXV_32BIT_ALIGNMENT_VALIDATE( p - table );
      gxv_mort_chain_validate( p, limit, valid );
      p += valid->subtable_length;
    }

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmort0.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT mort table validation                                 */
/*    body for type0 (Indic Script Rearrangement) subtable.                */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmort.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmort


  static const char* GXV_Mort_IndicScript_Msg[] =
  {
    "no change",
    "Ax => xA",
    "xD => Dx",
    "AxD => DxA",
    "ABx => xAB",
    "ABx => xBA",
    "xCD => CDx",
    "xCD => DCx",
    "AxCD => CDxA",
    "AxCD => DCxA",
    "ABxD => DxAB",
    "ABxD => DxBA",
    "ABxCD => CDxAB",
    "ABxCD => CDxBA",
    "ABxCD => DCxAB",
    "ABxCD => DCxBA",

  };


  static void
  gxv_mort_subtable_type0_entry_validate(
    FT_Byte                         state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
    FT_UShort  markFirst;
    FT_UShort  dontAdvance;
    FT_UShort  markLast;
    FT_UShort  reserved;
    FT_UShort  verb = 0;

    FT_UNUSED( state );
    FT_UNUSED( table );
    FT_UNUSED( limit );

    FT_UNUSED( GXV_Mort_IndicScript_Msg[verb] ); /* for the non-debugging */
    FT_UNUSED( glyphOffset_p );                  /* case                  */


    markFirst   = (FT_UShort)( ( flags >> 15 ) & 1 );
    dontAdvance = (FT_UShort)( ( flags >> 14 ) & 1 );
    markLast    = (FT_UShort)( ( flags >> 13 ) & 1 );

    reserved = (FT_UShort)( flags & 0x1FF0 );
    verb     = (FT_UShort)( flags & 0x000F );

    GXV_TRACE(( "  IndicScript MorphRule for glyphOffset 0x%04x",
                glyphOffset_p->u ));
    GXV_TRACE(( " markFirst=%01d", markFirst ));
    GXV_TRACE(( " dontAdvance=%01d", dontAdvance ));
    GXV_TRACE(( " markLast=%01d", markLast ));
    GXV_TRACE(( " %02d", verb ));
    GXV_TRACE(( " %s\n", GXV_Mort_IndicScript_Msg[verb] ));

    if ( markFirst > 0 && markLast > 0 )
    {
      GXV_TRACE(( "  [odd] a glyph is marked as the first and last"
                  "  in Indic rearrangement\n" ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }

    if ( markFirst > 0 && dontAdvance > 0 )
    {
      GXV_TRACE(( "  [odd] the first glyph is marked as dontAdvance"
                  " in Indic rearrangement\n" ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }

    if ( 0 < reserved )
    {
      GXV_TRACE(( " non-zero bits found in reserved range\n" ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }
    else
      GXV_TRACE(( "\n" ));
  }


  FT_LOCAL_DEF( void )
  gxv_mort_subtable_type0_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    GXV_NAME_ENTER(
      "mort chain subtable type0 (Indic-Script Rearrangement)" );

    GXV_LIMIT_CHECK( GXV_STATETABLE_HEADER_SIZE );

    valid->statetable.optdata               = NULL;
    valid->statetable.optdata_load_func     = NULL;
    valid->statetable.subtable_setup_func   = NULL;
    valid->statetable.entry_glyphoffset_fmt = GXV_GLYPHOFFSET_NONE;
    valid->statetable.entry_validate_func =
      gxv_mort_subtable_type0_entry_validate;

    gxv_StateTable_validate( p, limit, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmort1.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT mort table validation                                 */
/*    body for type1 (Contextual Substitution) subtable.                   */
/*                                                                         */
/*  Copyright 2005, 2007 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmort.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmort


  typedef struct  GXV_mort_subtable_type1_StateOptRec_
  {
    FT_UShort  substitutionTable;
    FT_UShort  substitutionTable_length;

  }  GXV_mort_subtable_type1_StateOptRec,
    *GXV_mort_subtable_type1_StateOptRecData;

#define GXV_MORT_SUBTABLE_TYPE1_HEADER_SIZE \
          ( GXV_STATETABLE_HEADER_SIZE + 2 )


  static void
  gxv_mort_subtable_type1_substitutionTable_load( FT_Bytes       table,
                                                  FT_Bytes       limit,
                                                  GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_mort_subtable_type1_StateOptRecData  optdata =
      (GXV_mort_subtable_type1_StateOptRecData)valid->statetable.optdata;


    GXV_LIMIT_CHECK( 2 );
    optdata->substitutionTable = FT_NEXT_USHORT( p );
  }


  static void
  gxv_mort_subtable_type1_subtable_setup( FT_UShort      table_size,
                                          FT_UShort      classTable,
                                          FT_UShort      stateArray,
                                          FT_UShort      entryTable,
                                          FT_UShort*     classTable_length_p,
                                          FT_UShort*     stateArray_length_p,
                                          FT_UShort*     entryTable_length_p,
                                          GXV_Validator  valid )
  {
    FT_UShort  o[4];
    FT_UShort  *l[4];
    FT_UShort  buff[5];

    GXV_mort_subtable_type1_StateOptRecData  optdata =
      (GXV_mort_subtable_type1_StateOptRecData)valid->statetable.optdata;


    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->substitutionTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &( optdata->substitutionTable_length );

    gxv_set_length_by_ushort_offset( o, l, buff, 4, table_size, valid );
  }


  static void
  gxv_mort_subtable_type1_offset_to_subst_validate(
    FT_Short          wordOffset,
    const FT_String*  tag,
    FT_Byte           state,
    GXV_Validator     valid )
  {
    FT_UShort  substTable;
    FT_UShort  substTable_limit;

    FT_UNUSED( tag );
    FT_UNUSED( state );


    substTable =
      ((GXV_mort_subtable_type1_StateOptRec *)
       (valid->statetable.optdata))->substitutionTable;
    substTable_limit =
      (FT_UShort)( substTable +
                   ((GXV_mort_subtable_type1_StateOptRec *)
                    (valid->statetable.optdata))->substitutionTable_length );

    valid->min_gid = (FT_UShort)( ( substTable       - wordOffset * 2 ) / 2 );
    valid->max_gid = (FT_UShort)( ( substTable_limit - wordOffset * 2 ) / 2 );
    valid->max_gid = (FT_UShort)( FT_MAX( valid->max_gid,
                                          valid->face->num_glyphs ) );

    /* XXX: check range? */

    /* TODO: min_gid & max_gid comparison with ClassTable contents */
  }


  static void
  gxv_mort_subtable_type1_entry_validate(
    FT_Byte                         state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  setMark;
    FT_UShort  dontAdvance;
#endif
    FT_UShort  reserved;
    FT_Short   markOffset;
    FT_Short   currentOffset;

    FT_UNUSED( table );
    FT_UNUSED( limit );


#ifdef GXV_LOAD_UNUSED_VARS
    setMark       = (FT_UShort)(   flags >> 15            );
    dontAdvance   = (FT_UShort)( ( flags >> 14 ) & 1      );
#endif
    reserved      = (FT_Short)(    flags         & 0x3FFF );

    markOffset    = (FT_Short)( glyphOffset_p->ul >> 16 );
    currentOffset = (FT_Short)( glyphOffset_p->ul       );

    if ( 0 < reserved )
    {
      GXV_TRACE(( " non-zero bits found in reserved range\n" ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }

    gxv_mort_subtable_type1_offset_to_subst_validate( markOffset,
                                                      "markOffset",
                                                      state,
                                                      valid );

    gxv_mort_subtable_type1_offset_to_subst_validate( currentOffset,
                                                      "currentOffset",
                                                      state,
                                                      valid );
  }


  static void
  gxv_mort_subtable_type1_substTable_validate( FT_Bytes       table,
                                               FT_Bytes       limit,
                                               GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  num_gids = (FT_UShort)(
                 ((GXV_mort_subtable_type1_StateOptRec *)
                  (valid->statetable.optdata))->substitutionTable_length / 2 );
    FT_UShort  i;


    GXV_NAME_ENTER( "validating contents of substitutionTable" );
    for ( i = 0; i < num_gids ; i ++ )
    {
      FT_UShort  dst_gid;


      GXV_LIMIT_CHECK( 2 );
      dst_gid = FT_NEXT_USHORT( p );

      if ( dst_gid >= 0xFFFFU )
        continue;

      if ( dst_gid < valid->min_gid || valid->max_gid < dst_gid )
      {
        GXV_TRACE(( "substTable include a strange gid[%d]=%d >"
                    " out of define range (%d..%d)\n",
                    i, dst_gid, valid->min_gid, valid->max_gid ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
      }
    }

    GXV_EXIT;
  }


  /*
   * subtable for Contextual glyph substitution is a modified StateTable.
   * In addition to classTable, stateArray, and entryTable, the field
   * `substitutionTable' is added.
   */
  FT_LOCAL_DEF( void )
  gxv_mort_subtable_type1_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_mort_subtable_type1_StateOptRec  st_rec;


    GXV_NAME_ENTER( "mort chain subtable type1 (Contextual Glyph Subst)" );

    GXV_LIMIT_CHECK( GXV_MORT_SUBTABLE_TYPE1_HEADER_SIZE );

    valid->statetable.optdata =
      &st_rec;
    valid->statetable.optdata_load_func =
      gxv_mort_subtable_type1_substitutionTable_load;
    valid->statetable.subtable_setup_func =
      gxv_mort_subtable_type1_subtable_setup;
    valid->statetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_ULONG;
    valid->statetable.entry_validate_func =

      gxv_mort_subtable_type1_entry_validate;
    gxv_StateTable_validate( p, limit, valid );

    gxv_mort_subtable_type1_substTable_validate(
      table + st_rec.substitutionTable,
      table + st_rec.substitutionTable + st_rec.substitutionTable_length,
      valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmort2.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT mort table validation                                 */
/*    body for type2 (Ligature Substitution) subtable.                     */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmort.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmort


  typedef struct  GXV_mort_subtable_type2_StateOptRec_
  {
    FT_UShort  ligActionTable;
    FT_UShort  componentTable;
    FT_UShort  ligatureTable;
    FT_UShort  ligActionTable_length;
    FT_UShort  componentTable_length;
    FT_UShort  ligatureTable_length;

  }  GXV_mort_subtable_type2_StateOptRec,
    *GXV_mort_subtable_type2_StateOptRecData;

#define GXV_MORT_SUBTABLE_TYPE2_HEADER_SIZE \
          ( GXV_STATETABLE_HEADER_SIZE + 2 + 2 + 2 )


  static void
  gxv_mort_subtable_type2_opttable_load( FT_Bytes       table,
                                         FT_Bytes       limit,
                                         GXV_Validator  valid )
  {
    FT_Bytes p = table;
    GXV_mort_subtable_type2_StateOptRecData  optdata =
      (GXV_mort_subtable_type2_StateOptRecData)valid->statetable.optdata;


    GXV_LIMIT_CHECK( 2 + 2 + 2 );
    optdata->ligActionTable = FT_NEXT_USHORT( p );
    optdata->componentTable = FT_NEXT_USHORT( p );
    optdata->ligatureTable  = FT_NEXT_USHORT( p );

    GXV_TRACE(( "offset to ligActionTable=0x%04x\n",
                optdata->ligActionTable ));
    GXV_TRACE(( "offset to componentTable=0x%04x\n",
                optdata->componentTable ));
    GXV_TRACE(( "offset to ligatureTable=0x%04x\n",
                optdata->ligatureTable ));
  }


  static void
  gxv_mort_subtable_type2_subtable_setup( FT_UShort      table_size,
                                          FT_UShort      classTable,
                                          FT_UShort      stateArray,
                                          FT_UShort      entryTable,
                                          FT_UShort      *classTable_length_p,
                                          FT_UShort      *stateArray_length_p,
                                          FT_UShort      *entryTable_length_p,
                                          GXV_Validator  valid )
  {
    FT_UShort  o[6];
    FT_UShort  *l[6];
    FT_UShort  buff[7];

    GXV_mort_subtable_type2_StateOptRecData  optdata =
      (GXV_mort_subtable_type2_StateOptRecData)valid->statetable.optdata;


    GXV_NAME_ENTER( "subtable boundaries setup" );

    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->ligActionTable;
    o[4] = optdata->componentTable;
    o[5] = optdata->ligatureTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->ligActionTable_length);
    l[4] = &(optdata->componentTable_length);
    l[5] = &(optdata->ligatureTable_length);

    gxv_set_length_by_ushort_offset( o, l, buff, 6, table_size, valid );

    GXV_TRACE(( "classTable: offset=0x%04x length=0x%04x\n",
                classTable, *classTable_length_p ));
    GXV_TRACE(( "stateArray: offset=0x%04x length=0x%04x\n",
                stateArray, *stateArray_length_p ));
    GXV_TRACE(( "entryTable: offset=0x%04x length=0x%04x\n",
                entryTable, *entryTable_length_p ));
    GXV_TRACE(( "ligActionTable: offset=0x%04x length=0x%04x\n",
                optdata->ligActionTable,
                optdata->ligActionTable_length ));
    GXV_TRACE(( "componentTable: offset=0x%04x length=0x%04x\n",
                optdata->componentTable,
                optdata->componentTable_length ));
    GXV_TRACE(( "ligatureTable:  offset=0x%04x length=0x%04x\n",
                optdata->ligatureTable,
                optdata->ligatureTable_length ));

    GXV_EXIT;
  }


  static void
  gxv_mort_subtable_type2_ligActionOffset_validate(
    FT_Bytes       table,
    FT_UShort      ligActionOffset,
    GXV_Validator  valid )
  {
    /* access ligActionTable */
    GXV_mort_subtable_type2_StateOptRecData  optdata =
      (GXV_mort_subtable_type2_StateOptRecData)valid->statetable.optdata;

    FT_Bytes lat_base  = table + optdata->ligActionTable;
    FT_Bytes p         = table + ligActionOffset;
    FT_Bytes lat_limit = lat_base + optdata->ligActionTable;


    GXV_32BIT_ALIGNMENT_VALIDATE( ligActionOffset );
    if ( p < lat_base )
    {
      GXV_TRACE(( "too short offset 0x%04x: p < lat_base (%d byte rewind)\n",
                  ligActionOffset, lat_base - p ));

      /* FontValidator, ftxvalidator, ftxdumperfuser warn but continue */
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
    }
    else if ( lat_limit < p )
    {
      GXV_TRACE(( "too large offset 0x%04x: lat_limit < p (%d byte overrun)\n",
                  ligActionOffset, p - lat_limit ));

      /* FontValidator, ftxvalidator, ftxdumperfuser warn but continue */
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
    }
    else
    {
      /* validate entry in ligActionTable */
      FT_ULong   lig_action;
#ifdef GXV_LOAD_UNUSED_VARS
      FT_UShort  last;
      FT_UShort  store;
#endif
      FT_ULong   offset;


      lig_action = FT_NEXT_ULONG( p );
#ifdef GXV_LOAD_UNUSED_VARS
      last   = (FT_UShort)( ( lig_action >> 31 ) & 1 );
      store  = (FT_UShort)( ( lig_action >> 30 ) & 1 );
#endif

      /* Apple spec defines this offset as a word offset */
      offset = lig_action & 0x3FFFFFFFUL;
      if ( offset * 2 < optdata->ligatureTable )
      {
        GXV_TRACE(( "too short offset 0x%08x:"
                    " 2 x offset < ligatureTable (%d byte rewind)\n",
                     offset, optdata->ligatureTable - offset * 2 ));

        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
      } else if ( offset * 2 >
                  (FT_ULong)(optdata->ligatureTable + optdata->ligatureTable_length) )
      {
        GXV_TRACE(( "too long offset 0x%08x:"
                    " 2 x offset > ligatureTable + ligatureTable_length"
                    " (%d byte overrun)\n",
                     offset,
                     optdata->ligatureTable + optdata->ligatureTable_length
                     - offset * 2 ));

        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
      }
    }
  }


  static void
  gxv_mort_subtable_type2_entry_validate(
    FT_Byte                         state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort setComponent;
    FT_UShort dontAdvance;
#endif
    FT_UShort offset;

    FT_UNUSED( state );
    FT_UNUSED( glyphOffset_p );
    FT_UNUSED( limit );


#ifdef GXV_LOAD_UNUSED_VARS
    setComponent = (FT_UShort)( ( flags >> 15 ) & 1 );
    dontAdvance  = (FT_UShort)( ( flags >> 14 ) & 1 );
#endif

    offset = (FT_UShort)( flags & 0x3FFFU );

    if ( 0 < offset )
      gxv_mort_subtable_type2_ligActionOffset_validate( table, offset,
                                                        valid );
  }


  static void
  gxv_mort_subtable_type2_ligatureTable_validate( FT_Bytes       table,
                                                  GXV_Validator  valid )
  {
    GXV_mort_subtable_type2_StateOptRecData  optdata =
      (GXV_mort_subtable_type2_StateOptRecData)valid->statetable.optdata;

    FT_Bytes p     = table + optdata->ligatureTable;
    FT_Bytes limit = table + optdata->ligatureTable
                           + optdata->ligatureTable_length;


    GXV_NAME_ENTER( "mort chain subtable type2 - substitutionTable" );
    if ( 0 != optdata->ligatureTable )
    {
      /* Apple does not give specification of ligatureTable format */
      while ( p < limit )
      {
        FT_UShort  lig_gid;


        GXV_LIMIT_CHECK( 2 );
        lig_gid = FT_NEXT_USHORT( p );

        if ( valid->face->num_glyphs < lig_gid )
          GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
      }
    }
    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_mort_subtable_type2_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_mort_subtable_type2_StateOptRec  lig_rec;


    GXV_NAME_ENTER( "mort chain subtable type2 (Ligature Substitution)" );

    GXV_LIMIT_CHECK( GXV_MORT_SUBTABLE_TYPE2_HEADER_SIZE );

    valid->statetable.optdata =
      &lig_rec;
    valid->statetable.optdata_load_func =
      gxv_mort_subtable_type2_opttable_load;
    valid->statetable.subtable_setup_func =
      gxv_mort_subtable_type2_subtable_setup;
    valid->statetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_NONE;
    valid->statetable.entry_validate_func =
      gxv_mort_subtable_type2_entry_validate;

    gxv_StateTable_validate( p, limit, valid );

    p += valid->subtable_length;
    gxv_mort_subtable_type2_ligatureTable_validate( table, valid );

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmort4.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT mort table validation                                 */
/*    body for type4 (Non-Contextual Glyph Substitution) subtable.         */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmort.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmort


  static void
  gxv_mort_subtable_type4_lookupval_validate( FT_UShort            glyph,
                                              GXV_LookupValueCPtr  value_p,
                                              GXV_Validator        valid )
  {
    FT_UNUSED( glyph );

    gxv_glyphid_validate( value_p->u, valid );
  }

  /*
    +===============+ --------+
    | lookup header |         |
    +===============+         |
    | BinSrchHeader |         |
    +===============+         |
    | lastGlyph[0]  |         |
    +---------------+         |
    | firstGlyph[0] |         |    head of lookup table
    +---------------+         |             +
    | offset[0]     |    ->   |          offset            [byte]
    +===============+         |             +
    | lastGlyph[1]  |         | (glyphID - firstGlyph) * 2 [byte]
    +---------------+         |
    | firstGlyph[1] |         |
    +---------------+         |
    | offset[1]     |         |
    +===============+         |
                              |
     ....                     |
                              |
    16bit value array         |
    +===============+         |
    |     value     | <-------+
     ....
  */

  static GXV_LookupValueDesc
  gxv_mort_subtable_type4_lookupfmt4_transit(
    FT_UShort            relative_gindex,
    GXV_LookupValueCPtr  base_value_p,
    FT_Bytes             lookuptbl_limit,
    GXV_Validator        valid )
  {
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;

    /* XXX: check range? */
    offset = (FT_UShort)( base_value_p->u +
                          relative_gindex * sizeof ( FT_UShort ) );

    p     = valid->lookuptbl_head + offset;
    limit = lookuptbl_limit;

    GXV_LIMIT_CHECK( 2 );
    value.u = FT_NEXT_USHORT( p );

    return value;
  }


  FT_LOCAL_DEF( void )
  gxv_mort_subtable_type4_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    GXV_NAME_ENTER( "mort chain subtable type4 "
                    "(Non-Contextual Glyph Substitution)" );

    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_mort_subtable_type4_lookupval_validate;
    valid->lookupfmt4_trans = gxv_mort_subtable_type4_lookupfmt4_transit;

    gxv_LookupTable_validate( p, limit, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmort5.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT mort table validation                                 */
/*    body for type5 (Contextual Glyph Insertion) subtable.                */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmort.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmort


  /*
   * mort subtable type5 (Contextual Glyph Insertion)
   * has the format of StateTable with insertion-glyph-list,
   * but without name.  The offset is given by glyphOffset in
   * entryTable.  There is no table location declaration
   * like xxxTable.
   */

  typedef struct  GXV_mort_subtable_type5_StateOptRec_
  {
    FT_UShort   classTable;
    FT_UShort   stateArray;
    FT_UShort   entryTable;

#define GXV_MORT_SUBTABLE_TYPE5_HEADER_SIZE  GXV_STATETABLE_HEADER_SIZE

    FT_UShort*  classTable_length_p;
    FT_UShort*  stateArray_length_p;
    FT_UShort*  entryTable_length_p;

  }  GXV_mort_subtable_type5_StateOptRec,
    *GXV_mort_subtable_type5_StateOptRecData;


  FT_LOCAL_DEF( void )
  gxv_mort_subtable_type5_subtable_setup( FT_UShort      table_size,
                                          FT_UShort      classTable,
                                          FT_UShort      stateArray,
                                          FT_UShort      entryTable,
                                          FT_UShort*     classTable_length_p,
                                          FT_UShort*     stateArray_length_p,
                                          FT_UShort*     entryTable_length_p,
                                          GXV_Validator  valid )
  {
    GXV_mort_subtable_type5_StateOptRecData  optdata =
      (GXV_mort_subtable_type5_StateOptRecData)valid->statetable.optdata;


    gxv_StateTable_subtable_setup( table_size,
                                   classTable,
                                   stateArray,
                                   entryTable,
                                   classTable_length_p,
                                   stateArray_length_p,
                                   entryTable_length_p,
                                   valid );

    optdata->classTable = classTable;
    optdata->stateArray = stateArray;
    optdata->entryTable = entryTable;

    optdata->classTable_length_p = classTable_length_p;
    optdata->stateArray_length_p = stateArray_length_p;
    optdata->entryTable_length_p = entryTable_length_p;
  }


  static void
  gxv_mort_subtable_type5_InsertList_validate( FT_UShort      offset,
                                               FT_UShort      count,
                                               FT_Bytes       table,
                                               FT_Bytes       limit,
                                               GXV_Validator  valid )
  {
    /*
     * We don't know the range of insertion-glyph-list.
     * Set range by whole of state table.
     */
    FT_Bytes  p = table + offset;

    GXV_mort_subtable_type5_StateOptRecData  optdata =
      (GXV_mort_subtable_type5_StateOptRecData)valid->statetable.optdata;

    if ( optdata->classTable < offset                                   &&
         offset < optdata->classTable + *(optdata->classTable_length_p) )
      GXV_TRACE(( " offset runs into ClassTable" ));
    if ( optdata->stateArray < offset                                   &&
         offset < optdata->stateArray + *(optdata->stateArray_length_p) )
      GXV_TRACE(( " offset runs into StateArray" ));
    if ( optdata->entryTable < offset                                   &&
         offset < optdata->entryTable + *(optdata->entryTable_length_p) )
      GXV_TRACE(( " offset runs into EntryTable" ));

#ifndef GXV_LOAD_TRACE_VARS
    GXV_LIMIT_CHECK( count * 2 );
#else
    while ( p < table + offset + ( count * 2 ) )
    {
      FT_UShort insert_glyphID;


      GXV_LIMIT_CHECK( 2 );
      insert_glyphID = FT_NEXT_USHORT( p );
      GXV_TRACE(( " 0x%04x", insert_glyphID ));
    }
    GXV_TRACE(( "\n" ));
#endif
  }


  static void
  gxv_mort_subtable_type5_entry_validate(
    FT_Byte                         state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_Bool    setMark;
    FT_Bool    dontAdvance;
    FT_Bool    currentIsKashidaLike;
    FT_Bool    markedIsKashidaLike;
    FT_Bool    currentInsertBefore;
    FT_Bool    markedInsertBefore;
#endif
    FT_Byte    currentInsertCount;
    FT_Byte    markedInsertCount;
    FT_UShort  currentInsertList;
    FT_UShort  markedInsertList;

    FT_UNUSED( state );


#ifdef GXV_LOAD_UNUSED_VARS
    setMark              = FT_BOOL( ( flags >> 15 ) & 1 );
    dontAdvance          = FT_BOOL( ( flags >> 14 ) & 1 );
    currentIsKashidaLike = FT_BOOL( ( flags >> 13 ) & 1 );
    markedIsKashidaLike  = FT_BOOL( ( flags >> 12 ) & 1 );
    currentInsertBefore  = FT_BOOL( ( flags >> 11 ) & 1 );
    markedInsertBefore   = FT_BOOL( ( flags >> 10 ) & 1 );
#endif

    currentInsertCount   = (FT_Byte)( ( flags >> 5 ) & 0x1F   );
    markedInsertCount    = (FT_Byte)(   flags        & 0x001F );

    currentInsertList    = (FT_UShort)( glyphOffset->ul >> 16 );
    markedInsertList     = (FT_UShort)( glyphOffset->ul       );

    if ( 0 != currentInsertList && 0 != currentInsertCount )
    {
      gxv_mort_subtable_type5_InsertList_validate( currentInsertList,
                                                   currentInsertCount,
                                                   table,
                                                   limit,
                                                   valid );
    }

    if ( 0 != markedInsertList && 0 != markedInsertCount )
    {
      gxv_mort_subtable_type5_InsertList_validate( markedInsertList,
                                                   markedInsertCount,
                                                   table,
                                                   limit,
                                                   valid );
    }
  }


  FT_LOCAL_DEF( void )
  gxv_mort_subtable_type5_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_mort_subtable_type5_StateOptRec      et_rec;
    GXV_mort_subtable_type5_StateOptRecData  et = &et_rec;


    GXV_NAME_ENTER( "mort chain subtable type5 (Glyph Insertion)" );

    GXV_LIMIT_CHECK( GXV_MORT_SUBTABLE_TYPE5_HEADER_SIZE );

    valid->statetable.optdata =
      et;
    valid->statetable.optdata_load_func =
      NULL;
    valid->statetable.subtable_setup_func =
      gxv_mort_subtable_type5_subtable_setup;
    valid->statetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_ULONG;
    valid->statetable.entry_validate_func =
      gxv_mort_subtable_type5_entry_validate;

    gxv_StateTable_validate( p, limit, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmorx.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation (body).                         */
/*                                                                         */
/*  Copyright 2005, 2008, 2013 by                                          */
/*  suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmorx.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


  static void
  gxv_morx_subtables_validate( FT_Bytes       table,
                               FT_Bytes       limit,
                               FT_UShort      nSubtables,
                               GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_Validate_Func fmt_funcs_table[] =
    {
      gxv_morx_subtable_type0_validate, /* 0 */
      gxv_morx_subtable_type1_validate, /* 1 */
      gxv_morx_subtable_type2_validate, /* 2 */
      NULL,                             /* 3 */
      gxv_morx_subtable_type4_validate, /* 4 */
      gxv_morx_subtable_type5_validate, /* 5 */

    };

    FT_UShort  i;


    GXV_NAME_ENTER( "subtables in a chain" );

    for ( i = 0; i < nSubtables; i++ )
    {
      GXV_Validate_Func  func;

      FT_ULong  length;
      FT_ULong  coverage;
#ifdef GXV_LOAD_UNUSED_VARS
      FT_ULong  subFeatureFlags;
#endif
      FT_ULong  type;
      FT_ULong  rest;


      GXV_LIMIT_CHECK( 4 + 4 + 4 );
      length          = FT_NEXT_ULONG( p );
      coverage        = FT_NEXT_ULONG( p );
#ifdef GXV_LOAD_UNUSED_VARS
      subFeatureFlags = FT_NEXT_ULONG( p );
#else
      p += 4;
#endif

      GXV_TRACE(( "validating chain subtable %d/%d (%d bytes)\n",
                  i + 1, nSubtables, length ));

      type = coverage & 0x0007;
      rest = length - ( 4 + 4 + 4 );
      GXV_LIMIT_CHECK( rest );

      /* morx coverage consists of mort_coverage & 16bit padding */
      gxv_mort_coverage_validate( (FT_UShort)( ( coverage >> 16 ) | coverage ),
                                  valid );
      if ( type > 5 )
        FT_INVALID_FORMAT;

      func = fmt_funcs_table[type];
      if ( func == NULL )
        GXV_TRACE(( "morx type %d is reserved\n", type ));

      func( p, p + rest, valid );

      /* TODO: subFeatureFlags should be unique in a table? */
      p += rest;
    }

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  static void
  gxv_morx_chain_validate( FT_Bytes       table,
                           FT_Bytes       limit,
                           GXV_Validator  valid )
  {
    FT_Bytes  p = table;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_ULong  defaultFlags;
#endif
    FT_ULong  chainLength;
    FT_ULong  nFeatureFlags;
    FT_ULong  nSubtables;


    GXV_NAME_ENTER( "morx chain header" );

    GXV_LIMIT_CHECK( 4 + 4 + 4 + 4 );
#ifdef GXV_LOAD_UNUSED_VARS
    defaultFlags  = FT_NEXT_ULONG( p );
#else
    p += 4;
#endif
    chainLength   = FT_NEXT_ULONG( p );
    nFeatureFlags = FT_NEXT_ULONG( p );
    nSubtables    = FT_NEXT_ULONG( p );

    /* feature-array of morx is same with that of mort */
    gxv_mort_featurearray_validate( p, limit, nFeatureFlags, valid );
    p += valid->subtable_length;

    if ( nSubtables >= 0x10000L )
      FT_INVALID_DATA;

    gxv_morx_subtables_validate( p, table + chainLength,
                                 (FT_UShort)nSubtables, valid );

    valid->subtable_length = chainLength;

    /* TODO: defaultFlags should be compared with the flags in tables */

    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_morx_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;
    FT_ULong          version;
    FT_ULong          nChains;
    FT_ULong          i;


    valid->root = ftvalid;
    valid->face = face;

    FT_TRACE3(( "validating `morx' table\n" ));
    GXV_INIT;

    GXV_LIMIT_CHECK( 4 + 4 );
    version = FT_NEXT_ULONG( p );
    nChains = FT_NEXT_ULONG( p );

    if ( version != 0x00020000UL )
      FT_INVALID_FORMAT;

    for ( i = 0; i < nChains; i++ )
    {
      GXV_TRACE(( "validating chain %d/%d\n", i + 1, nChains ));
      GXV_32BIT_ALIGNMENT_VALIDATE( p - table );
      gxv_morx_chain_validate( p, limit, valid );
      p += valid->subtable_length;
    }

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmorx0.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for type0 (Indic Script Rearrangement) subtable.                */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmorx.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


  static void
  gxv_morx_subtable_type0_entry_validate(
    FT_UShort                        state,
    FT_UShort                        flags,
    GXV_XStateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                         table,
    FT_Bytes                         limit,
    GXV_Validator                    valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  markFirst;
    FT_UShort  dontAdvance;
    FT_UShort  markLast;
#endif
    FT_UShort  reserved;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  verb;
#endif

    FT_UNUSED( state );
    FT_UNUSED( glyphOffset_p );
    FT_UNUSED( table );
    FT_UNUSED( limit );


#ifdef GXV_LOAD_UNUSED_VARS
    markFirst   = (FT_UShort)( ( flags >> 15 ) & 1 );
    dontAdvance = (FT_UShort)( ( flags >> 14 ) & 1 );
    markLast    = (FT_UShort)( ( flags >> 13 ) & 1 );
#endif

    reserved = (FT_UShort)( flags & 0x1FF0 );
#ifdef GXV_LOAD_UNUSED_VARS
    verb     = (FT_UShort)( flags & 0x000F );
#endif

    if ( 0 < reserved )
    {
      GXV_TRACE(( " non-zero bits found in reserved range\n" ));
      FT_INVALID_DATA;
    }
  }


  FT_LOCAL_DEF( void )
  gxv_morx_subtable_type0_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;


    GXV_NAME_ENTER(
      "morx chain subtable type0 (Indic-Script Rearrangement)" );

    GXV_LIMIT_CHECK( GXV_STATETABLE_HEADER_SIZE );

    valid->xstatetable.optdata               = NULL;
    valid->xstatetable.optdata_load_func     = NULL;
    valid->xstatetable.subtable_setup_func   = NULL;
    valid->xstatetable.entry_glyphoffset_fmt = GXV_GLYPHOFFSET_NONE;
    valid->xstatetable.entry_validate_func =
      gxv_morx_subtable_type0_entry_validate;

    gxv_XStateTable_validate( p, limit, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmorx1.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for type1 (Contextual Substitution) subtable.                   */
/*                                                                         */
/*  Copyright 2005, 2007 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmorx.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


  typedef struct  GXV_morx_subtable_type1_StateOptRec_
  {
    FT_ULong   substitutionTable;
    FT_ULong   substitutionTable_length;
    FT_UShort  substitutionTable_num_lookupTables;

  }  GXV_morx_subtable_type1_StateOptRec,
    *GXV_morx_subtable_type1_StateOptRecData;


#define GXV_MORX_SUBTABLE_TYPE1_HEADER_SIZE \
          ( GXV_STATETABLE_HEADER_SIZE + 2 )


  static void
  gxv_morx_subtable_type1_substitutionTable_load( FT_Bytes       table,
                                                  FT_Bytes       limit,
                                                  GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_morx_subtable_type1_StateOptRecData  optdata =
      (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;


    GXV_LIMIT_CHECK( 2 );
    optdata->substitutionTable = FT_NEXT_USHORT( p );
  }


  static void
  gxv_morx_subtable_type1_subtable_setup( FT_ULong       table_size,
                                          FT_ULong       classTable,
                                          FT_ULong       stateArray,
                                          FT_ULong       entryTable,
                                          FT_ULong*      classTable_length_p,
                                          FT_ULong*      stateArray_length_p,
                                          FT_ULong*      entryTable_length_p,
                                          GXV_Validator  valid )
  {
    FT_ULong  o[4];
    FT_ULong  *l[4];
    FT_ULong  buff[5];

    GXV_morx_subtable_type1_StateOptRecData  optdata =
      (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;


    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->substitutionTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->substitutionTable_length);

    gxv_set_length_by_ulong_offset( o, l, buff, 4, table_size, valid );
  }


  static void
  gxv_morx_subtable_type1_entry_validate(
    FT_UShort                       state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_TRACE_VARS
    FT_UShort  setMark;
    FT_UShort  dontAdvance;
#endif
    FT_UShort  reserved;
    FT_Short   markIndex;
    FT_Short   currentIndex;

    GXV_morx_subtable_type1_StateOptRecData  optdata =
      (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;

    FT_UNUSED( state );
    FT_UNUSED( table );
    FT_UNUSED( limit );


#ifdef GXV_LOAD_TRACE_VARS
    setMark      = (FT_UShort)( ( flags >> 15 ) & 1 );
    dontAdvance  = (FT_UShort)( ( flags >> 14 ) & 1 );
#endif

    reserved = (FT_UShort)( flags & 0x3FFF );

    markIndex    = (FT_Short)( glyphOffset_p->ul >> 16 );
    currentIndex = (FT_Short)( glyphOffset_p->ul       );

    GXV_TRACE(( " setMark=%01d dontAdvance=%01d\n",
                setMark, dontAdvance ));

    if ( 0 < reserved )
    {
      GXV_TRACE(( " non-zero bits found in reserved range\n" ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_DATA );
    }

    GXV_TRACE(( "markIndex = %d, currentIndex = %d\n",
                markIndex, currentIndex ));

    if ( optdata->substitutionTable_num_lookupTables < markIndex + 1 )
      optdata->substitutionTable_num_lookupTables =
        (FT_Short)( markIndex + 1 );

    if ( optdata->substitutionTable_num_lookupTables < currentIndex + 1 )
      optdata->substitutionTable_num_lookupTables =
        (FT_Short)( currentIndex + 1 );
  }


  static void
  gxv_morx_subtable_type1_LookupValue_validate( FT_UShort            glyph,
                                                GXV_LookupValueCPtr  value_p,
                                                GXV_Validator        valid )
  {
    FT_UNUSED( glyph ); /* for the non-debugging case */

    GXV_TRACE(( "morx subtable type1 subst.: %d -> %d\n", glyph, value_p->u ));

    if ( value_p->u > valid->face->num_glyphs )
      FT_INVALID_GLYPH_ID;
  }


  static GXV_LookupValueDesc
  gxv_morx_subtable_type1_LookupFmt4_transit(
    FT_UShort            relative_gindex,
    GXV_LookupValueCPtr  base_value_p,
    FT_Bytes             lookuptbl_limit,
    GXV_Validator        valid )
  {
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;

    /* XXX: check range? */
    offset = (FT_UShort)( base_value_p->u +
                          relative_gindex * sizeof ( FT_UShort ) );

    p     = valid->lookuptbl_head + offset;
    limit = lookuptbl_limit;

    GXV_LIMIT_CHECK ( 2 );
    value.u = FT_NEXT_USHORT( p );

    return value;
  }


  /*
   * TODO: length should be limit?
   **/
  static void
  gxv_morx_subtable_type1_substitutionTable_validate( FT_Bytes       table,
                                                      FT_Bytes       limit,
                                                      GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  i;

    GXV_morx_subtable_type1_StateOptRecData  optdata =
      (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;


    /* TODO: calculate offset/length for each lookupTables */
    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_morx_subtable_type1_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_morx_subtable_type1_LookupFmt4_transit;

    for ( i = 0; i < optdata->substitutionTable_num_lookupTables; i++ )
    {
      FT_ULong  offset;


      GXV_LIMIT_CHECK( 4 );
      offset = FT_NEXT_ULONG( p );

      gxv_LookupTable_validate( table + offset, limit, valid );
    }

    /* TODO: overlapping of lookupTables in substitutionTable */
  }


  /*
   * subtable for Contextual glyph substitution is a modified StateTable.
   * In addition to classTable, stateArray, entryTable, the field
   * `substitutionTable' is added.
   */
  FT_LOCAL_DEF( void )
  gxv_morx_subtable_type1_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_morx_subtable_type1_StateOptRec  st_rec;


    GXV_NAME_ENTER( "morx chain subtable type1 (Contextual Glyph Subst)" );

    GXV_LIMIT_CHECK( GXV_MORX_SUBTABLE_TYPE1_HEADER_SIZE );

    st_rec.substitutionTable_num_lookupTables = 0;

    valid->xstatetable.optdata =
      &st_rec;
    valid->xstatetable.optdata_load_func =
      gxv_morx_subtable_type1_substitutionTable_load;
    valid->xstatetable.subtable_setup_func =
      gxv_morx_subtable_type1_subtable_setup;
    valid->xstatetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_ULONG;
    valid->xstatetable.entry_validate_func =
      gxv_morx_subtable_type1_entry_validate;

    gxv_XStateTable_validate( p, limit, valid );

    gxv_morx_subtable_type1_substitutionTable_validate(
      table + st_rec.substitutionTable,
      table + st_rec.substitutionTable + st_rec.substitutionTable_length,
      valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmorx2.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for type2 (Ligature Substitution) subtable.                     */
/*                                                                         */
/*  Copyright 2005, 2013 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmorx.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


  typedef struct  GXV_morx_subtable_type2_StateOptRec_
  {
    FT_ULong  ligActionTable;
    FT_ULong  componentTable;
    FT_ULong  ligatureTable;
    FT_ULong  ligActionTable_length;
    FT_ULong  componentTable_length;
    FT_ULong  ligatureTable_length;

  }  GXV_morx_subtable_type2_StateOptRec,
    *GXV_morx_subtable_type2_StateOptRecData;


#define GXV_MORX_SUBTABLE_TYPE2_HEADER_SIZE \
          ( GXV_XSTATETABLE_HEADER_SIZE + 4 + 4 + 4 )


  static void
  gxv_morx_subtable_type2_opttable_load( FT_Bytes       table,
                                         FT_Bytes       limit,
                                         GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_morx_subtable_type2_StateOptRecData  optdata =
      (GXV_morx_subtable_type2_StateOptRecData)valid->xstatetable.optdata;


    GXV_LIMIT_CHECK( 4 + 4 + 4 );
    optdata->ligActionTable = FT_NEXT_ULONG( p );
    optdata->componentTable = FT_NEXT_ULONG( p );
    optdata->ligatureTable  = FT_NEXT_ULONG( p );

    GXV_TRACE(( "offset to ligActionTable=0x%08x\n",
                optdata->ligActionTable ));
    GXV_TRACE(( "offset to componentTable=0x%08x\n",
                optdata->componentTable ));
    GXV_TRACE(( "offset to ligatureTable=0x%08x\n",
                optdata->ligatureTable ));
  }


  static void
  gxv_morx_subtable_type2_subtable_setup( FT_ULong       table_size,
                                          FT_ULong       classTable,
                                          FT_ULong       stateArray,
                                          FT_ULong       entryTable,
                                          FT_ULong*      classTable_length_p,
                                          FT_ULong*      stateArray_length_p,
                                          FT_ULong*      entryTable_length_p,
                                          GXV_Validator  valid )
  {
    FT_ULong   o[6];
    FT_ULong*  l[6];
    FT_ULong   buff[7];

    GXV_morx_subtable_type2_StateOptRecData  optdata =
      (GXV_morx_subtable_type2_StateOptRecData)valid->xstatetable.optdata;


    GXV_NAME_ENTER( "subtable boundaries setup" );

    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->ligActionTable;
    o[4] = optdata->componentTable;
    o[5] = optdata->ligatureTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->ligActionTable_length);
    l[4] = &(optdata->componentTable_length);
    l[5] = &(optdata->ligatureTable_length);

    gxv_set_length_by_ulong_offset( o, l, buff, 6, table_size, valid );

    GXV_TRACE(( "classTable: offset=0x%08x length=0x%08x\n",
                classTable, *classTable_length_p ));
    GXV_TRACE(( "stateArray: offset=0x%08x length=0x%08x\n",
                stateArray, *stateArray_length_p ));
    GXV_TRACE(( "entryTable: offset=0x%08x length=0x%08x\n",
                entryTable, *entryTable_length_p ));
    GXV_TRACE(( "ligActionTable: offset=0x%08x length=0x%08x\n",
                optdata->ligActionTable,
                optdata->ligActionTable_length ));
    GXV_TRACE(( "componentTable: offset=0x%08x length=0x%08x\n",
                optdata->componentTable,
                optdata->componentTable_length ));
    GXV_TRACE(( "ligatureTable:  offset=0x%08x length=0x%08x\n",
                optdata->ligatureTable,
                optdata->ligatureTable_length ));

    GXV_EXIT;
  }


#define GXV_MORX_LIGACTION_ENTRY_SIZE  4


  static void
  gxv_morx_subtable_type2_ligActionIndex_validate(
    FT_Bytes       table,
    FT_UShort      ligActionIndex,
    GXV_Validator  valid )
  {
    /* access ligActionTable */
    GXV_morx_subtable_type2_StateOptRecData optdata =
      (GXV_morx_subtable_type2_StateOptRecData)valid->xstatetable.optdata;

    FT_Bytes lat_base  = table + optdata->ligActionTable;
    FT_Bytes p         = lat_base +
                         ligActionIndex * GXV_MORX_LIGACTION_ENTRY_SIZE;
    FT_Bytes lat_limit = lat_base + optdata->ligActionTable;


    if ( p < lat_base )
    {
      GXV_TRACE(( "p < lat_base (%d byte rewind)\n", lat_base - p ));
      FT_INVALID_OFFSET;
    }
    else if ( lat_limit < p )
    {
      GXV_TRACE(( "lat_limit < p (%d byte overrun)\n", p - lat_limit ));
      FT_INVALID_OFFSET;
    }

    {
      /* validate entry in ligActionTable */
      FT_ULong   lig_action;
#ifdef GXV_LOAD_UNUSED_VARS
      FT_UShort  last;
      FT_UShort  store;
#endif
      FT_ULong   offset;
      FT_Long    gid_limit;


      lig_action = FT_NEXT_ULONG( p );
#ifdef GXV_LOAD_UNUSED_VARS
      last       = (FT_UShort)( ( lig_action >> 31 ) & 1 );
      store      = (FT_UShort)( ( lig_action >> 30 ) & 1 );
#endif

      offset = lig_action & 0x3FFFFFFFUL;

      /* this offset is 30-bit signed value to add to GID */
      /* it is different from the location offset in mort */
      if ( ( offset & 0x3FFF0000UL ) == 0x3FFF0000UL )
      { /* negative offset */
        gid_limit = valid->face->num_glyphs - ( offset & 0x0000FFFFUL );
        if ( gid_limit > 0 )
          return;

        GXV_TRACE(( "ligature action table includes"
                    " too negative offset moving all GID"
                    " below defined range: 0x%04x\n",
                    offset & 0xFFFFU ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
      }
      else if ( ( offset & 0x3FFF0000UL ) == 0x0000000UL )
      { /* positive offset */
        if ( (FT_Long)offset < valid->face->num_glyphs )
          return;

        GXV_TRACE(( "ligature action table includes"
                    " too large offset moving all GID"
                    " over defined range: 0x%04x\n",
                    offset & 0xFFFFU ));
        GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
      }

      GXV_TRACE(( "ligature action table includes"
                  " invalid offset to add to 16-bit GID:"
                  " 0x%08x\n", offset ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_OFFSET );
    }
  }


  static void
  gxv_morx_subtable_type2_entry_validate(
    FT_UShort                       state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  setComponent;
    FT_UShort  dontAdvance;
    FT_UShort  performAction;
#endif
    FT_UShort  reserved;
    FT_UShort  ligActionIndex;

    FT_UNUSED( state );
    FT_UNUSED( limit );


#ifdef GXV_LOAD_UNUSED_VARS
    setComponent   = (FT_UShort)( ( flags >> 15 ) & 1 );
    dontAdvance    = (FT_UShort)( ( flags >> 14 ) & 1 );
    performAction  = (FT_UShort)( ( flags >> 13 ) & 1 );
#endif

    reserved       = (FT_UShort)( flags & 0x1FFF );
    ligActionIndex = glyphOffset_p->u;

    if ( reserved > 0 )
      GXV_TRACE(( "  reserved 14bit is non-zero\n" ));

    if ( 0 < ligActionIndex )
      gxv_morx_subtable_type2_ligActionIndex_validate(
        table, ligActionIndex, valid );
  }


  static void
  gxv_morx_subtable_type2_ligatureTable_validate( FT_Bytes       table,
                                                  GXV_Validator  valid )
  {
    GXV_morx_subtable_type2_StateOptRecData  optdata =
      (GXV_morx_subtable_type2_StateOptRecData)valid->xstatetable.optdata;

    FT_Bytes p     = table + optdata->ligatureTable;
    FT_Bytes limit = table + optdata->ligatureTable
                           + optdata->ligatureTable_length;


    GXV_NAME_ENTER( "morx chain subtable type2 - substitutionTable" );

    if ( 0 != optdata->ligatureTable )
    {
      /* Apple does not give specification of ligatureTable format */
      while ( p < limit )
      {
        FT_UShort  lig_gid;


        GXV_LIMIT_CHECK( 2 );
        lig_gid = FT_NEXT_USHORT( p );
        if ( lig_gid < valid->face->num_glyphs )
          GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
      }
    }

    GXV_EXIT;
  }


  FT_LOCAL_DEF( void )
  gxv_morx_subtable_type2_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_morx_subtable_type2_StateOptRec  lig_rec;


    GXV_NAME_ENTER( "morx chain subtable type2 (Ligature Substitution)" );

    GXV_LIMIT_CHECK( GXV_MORX_SUBTABLE_TYPE2_HEADER_SIZE );

    valid->xstatetable.optdata =
      &lig_rec;
    valid->xstatetable.optdata_load_func =
      gxv_morx_subtable_type2_opttable_load;
    valid->xstatetable.subtable_setup_func =
      gxv_morx_subtable_type2_subtable_setup;
    valid->xstatetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_USHORT;
    valid->xstatetable.entry_validate_func =
      gxv_morx_subtable_type2_entry_validate;

    gxv_XStateTable_validate( p, limit, valid );

#if 0
    p += valid->subtable_length;
#endif
    gxv_morx_subtable_type2_ligatureTable_validate( table, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmorx4.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for "morx" type4 (Non-Contextual Glyph Substitution) subtable.  */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmorx.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


  FT_LOCAL_DEF( void )
  gxv_morx_subtable_type4_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    GXV_NAME_ENTER( "morx chain subtable type4 "
                    "(Non-Contextual Glyph Substitution)" );

    gxv_mort_subtable_type4_validate( table, limit, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmorx5.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for type5 (Contextual Glyph Insertion) subtable.                */
/*                                                                         */
/*  Copyright 2005, 2007 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvmorx.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


  /*
   * `morx' subtable type5 (Contextual Glyph Insertion)
   * has format of a StateTable with insertion-glyph-list
   * without name.  However, the 32bit offset from the head
   * of subtable to the i-g-l is given after `entryTable',
   * without variable name specification (the existence of
   * this offset to the table is different from mort type5).
   */


  typedef struct  GXV_morx_subtable_type5_StateOptRec_
  {
    FT_ULong  insertionGlyphList;
    FT_ULong  insertionGlyphList_length;

  }  GXV_morx_subtable_type5_StateOptRec,
    *GXV_morx_subtable_type5_StateOptRecData;


#define GXV_MORX_SUBTABLE_TYPE5_HEADER_SIZE \
          ( GXV_STATETABLE_HEADER_SIZE + 4 )


  static void
  gxv_morx_subtable_type5_insertionGlyphList_load( FT_Bytes       table,
                                                   FT_Bytes       limit,
                                                   GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_morx_subtable_type5_StateOptRecData  optdata =
      (GXV_morx_subtable_type5_StateOptRecData)valid->xstatetable.optdata;


    GXV_LIMIT_CHECK( 4 );
    optdata->insertionGlyphList = FT_NEXT_ULONG( p );
  }


  static void
  gxv_morx_subtable_type5_subtable_setup( FT_ULong       table_size,
                                          FT_ULong       classTable,
                                          FT_ULong       stateArray,
                                          FT_ULong       entryTable,
                                          FT_ULong*      classTable_length_p,
                                          FT_ULong*      stateArray_length_p,
                                          FT_ULong*      entryTable_length_p,
                                          GXV_Validator  valid )
  {
    FT_ULong   o[4];
    FT_ULong*  l[4];
    FT_ULong   buff[5];

    GXV_morx_subtable_type5_StateOptRecData  optdata =
      (GXV_morx_subtable_type5_StateOptRecData)valid->xstatetable.optdata;


    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->insertionGlyphList;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->insertionGlyphList_length);

    gxv_set_length_by_ulong_offset( o, l, buff, 4, table_size, valid );
  }


  static void
  gxv_morx_subtable_type5_InsertList_validate( FT_UShort      table_index,
                                               FT_UShort      count,
                                               FT_Bytes       table,
                                               FT_Bytes       limit,
                                               GXV_Validator  valid )
  {
    FT_Bytes p = table + table_index * 2;


#ifndef GXV_LOAD_TRACE_VARS
    GXV_LIMIT_CHECK( count * 2 );
#else
    while ( p < table + count * 2 + table_index * 2 )
    {
      FT_UShort  insert_glyphID;


      GXV_LIMIT_CHECK( 2 );
      insert_glyphID = FT_NEXT_USHORT( p );
      GXV_TRACE(( " 0x%04x", insert_glyphID ));
    }

    GXV_TRACE(( "\n" ));
#endif
  }


  static void
  gxv_morx_subtable_type5_entry_validate(
    FT_UShort                       state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_Bool    setMark;
    FT_Bool    dontAdvance;
    FT_Bool    currentIsKashidaLike;
    FT_Bool    markedIsKashidaLike;
    FT_Bool    currentInsertBefore;
    FT_Bool    markedInsertBefore;
#endif
    FT_Byte    currentInsertCount;
    FT_Byte    markedInsertCount;
    FT_Byte    currentInsertList;
    FT_UShort  markedInsertList;

    FT_UNUSED( state );


#ifdef GXV_LOAD_UNUSED_VARS
    setMark              = FT_BOOL( ( flags >> 15 ) & 1 );
    dontAdvance          = FT_BOOL( ( flags >> 14 ) & 1 );
    currentIsKashidaLike = FT_BOOL( ( flags >> 13 ) & 1 );
    markedIsKashidaLike  = FT_BOOL( ( flags >> 12 ) & 1 );
    currentInsertBefore  = FT_BOOL( ( flags >> 11 ) & 1 );
    markedInsertBefore   = FT_BOOL( ( flags >> 10 ) & 1 );
#endif

    currentInsertCount = (FT_Byte)( ( flags >> 5 ) & 0x1F   );
    markedInsertCount  = (FT_Byte)(   flags        & 0x001F );

    currentInsertList = (FT_Byte)  ( glyphOffset_p->ul >> 16 );
    markedInsertList  = (FT_UShort)( glyphOffset_p->ul       );

    if ( currentInsertList && 0 != currentInsertCount )
      gxv_morx_subtable_type5_InsertList_validate( currentInsertList,
                                                   currentInsertCount,
                                                   table, limit,
                                                   valid );

    if ( markedInsertList && 0 != markedInsertCount )
      gxv_morx_subtable_type5_InsertList_validate( markedInsertList,
                                                   markedInsertCount,
                                                   table, limit,
                                                   valid );
  }


  FT_LOCAL_DEF( void )
  gxv_morx_subtable_type5_validate( FT_Bytes       table,
                                    FT_Bytes       limit,
                                    GXV_Validator  valid )
  {
    FT_Bytes  p = table;

    GXV_morx_subtable_type5_StateOptRec      et_rec;
    GXV_morx_subtable_type5_StateOptRecData  et = &et_rec;


    GXV_NAME_ENTER( "morx chain subtable type5 (Glyph Insertion)" );

    GXV_LIMIT_CHECK( GXV_MORX_SUBTABLE_TYPE5_HEADER_SIZE );

    valid->xstatetable.optdata =
      et;
    valid->xstatetable.optdata_load_func =
      gxv_morx_subtable_type5_insertionGlyphList_load;
    valid->xstatetable.subtable_setup_func =
      gxv_morx_subtable_type5_subtable_setup;
    valid->xstatetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_ULONG;
    valid->xstatetable.entry_validate_func =
      gxv_morx_subtable_type5_entry_validate;

    gxv_XStateTable_validate( p, limit, valid );

    GXV_EXIT;
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvkern.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT kern table validation (body).                         */
/*                                                                         */
/*  Copyright 2004-2007, 2013                                              */
/*  by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"

#include FT_SFNT_NAMES_H
#include FT_SERVICE_GX_VALIDATE_H


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvkern


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  typedef enum  GXV_kern_Version_
  {
    KERN_VERSION_CLASSIC = 0x0000,
    KERN_VERSION_NEW     = 0x0001

  } GXV_kern_Version;


  typedef enum GXV_kern_Dialect_
  {
    KERN_DIALECT_UNKNOWN = 0,
    KERN_DIALECT_MS      = FT_VALIDATE_MS,
    KERN_DIALECT_APPLE   = FT_VALIDATE_APPLE,
    KERN_DIALECT_ANY     = FT_VALIDATE_CKERN

  } GXV_kern_Dialect;


  typedef struct  GXV_kern_DataRec_
  {
    GXV_kern_Version  version;
    void             *subtable_data;
    GXV_kern_Dialect  dialect_request;

  } GXV_kern_DataRec, *GXV_kern_Data;


#define GXV_KERN_DATA( field )  GXV_TABLE_DATA( kern, field )

#define KERN_IS_CLASSIC( valid )                               \
          ( KERN_VERSION_CLASSIC == GXV_KERN_DATA( version ) )
#define KERN_IS_NEW( valid )                                   \
          ( KERN_VERSION_NEW     == GXV_KERN_DATA( version ) )

#define KERN_DIALECT( valid )              \
          GXV_KERN_DATA( dialect_request )
#define KERN_ALLOWS_MS( valid )                       \
          ( KERN_DIALECT( valid ) & KERN_DIALECT_MS )
#define KERN_ALLOWS_APPLE( valid )                       \
          ( KERN_DIALECT( valid ) & KERN_DIALECT_APPLE )

#define GXV_KERN_HEADER_SIZE           ( KERN_IS_NEW( valid ) ? 8 : 4 )
#define GXV_KERN_SUBTABLE_HEADER_SIZE  ( KERN_IS_NEW( valid ) ? 8 : 6 )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      SUBTABLE VALIDATORS                      *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/


  /* ============================= format 0 ============================== */

  static void
  gxv_kern_subtable_fmt0_pairs_validate( FT_Bytes       table,
                                         FT_Bytes       limit,
                                         FT_UShort      nPairs,
                                         GXV_Validator  valid )
  {
    FT_Bytes   p = table;
    FT_UShort  i;

    FT_UShort  last_gid_left  = 0;
    FT_UShort  last_gid_right = 0;

    FT_UNUSED( limit );


    GXV_NAME_ENTER( "kern format 0 pairs" );

    for ( i = 0; i < nPairs; i++ )
    {
      FT_UShort  gid_left;
      FT_UShort  gid_right;
#ifdef GXV_LOAD_UNUSED_VARS
      FT_Short   kernValue;
#endif


      /* left */
      gid_left  = FT_NEXT_USHORT( p );
      gxv_glyphid_validate( gid_left, valid );

      /* right */
      gid_right = FT_NEXT_USHORT( p );
      gxv_glyphid_validate( gid_right, valid );

      /* Pairs of left and right GIDs must be unique and sorted. */
      GXV_TRACE(( "left gid = %u, right gid = %u\n", gid_left, gid_right ));
      if ( gid_left == last_gid_left )
      {
        if ( last_gid_right < gid_right )
          last_gid_right = gid_right;
        else
          FT_INVALID_DATA;
      }
      else if ( last_gid_left < gid_left )
      {
        last_gid_left  = gid_left;
        last_gid_right = gid_right;
      }
      else
        FT_INVALID_DATA;

      /* skip the kern value */
#ifdef GXV_LOAD_UNUSED_VARS
      kernValue = FT_NEXT_SHORT( p );
#else
      p += 2;
#endif
    }

    GXV_EXIT;
  }

  static void
  gxv_kern_subtable_fmt0_validate( FT_Bytes       table,
                                   FT_Bytes       limit,
                                   GXV_Validator  valid )
  {
    FT_Bytes   p = table + GXV_KERN_SUBTABLE_HEADER_SIZE;

    FT_UShort  nPairs;
    FT_UShort  unitSize;


    GXV_NAME_ENTER( "kern subtable format 0" );

    unitSize = 2 + 2 + 2;
    nPairs   = 0;

    /* nPairs, searchRange, entrySelector, rangeShift */
    GXV_LIMIT_CHECK( 2 + 2 + 2 + 2 );
    gxv_BinSrchHeader_validate( p, limit, &unitSize, &nPairs, valid );
    p += 2 + 2 + 2 + 2;

    gxv_kern_subtable_fmt0_pairs_validate( p, limit, nPairs, valid );

    GXV_EXIT;
  }


  /* ============================= format 1 ============================== */


  typedef struct  GXV_kern_fmt1_StateOptRec_
  {
    FT_UShort  valueTable;
    FT_UShort  valueTable_length;

  } GXV_kern_fmt1_StateOptRec, *GXV_kern_fmt1_StateOptRecData;


  static void
  gxv_kern_subtable_fmt1_valueTable_load( FT_Bytes       table,
                                          FT_Bytes       limit,
                                          GXV_Validator  valid )
  {
    FT_Bytes                       p = table;
    GXV_kern_fmt1_StateOptRecData  optdata =
      (GXV_kern_fmt1_StateOptRecData)valid->statetable.optdata;


    GXV_LIMIT_CHECK( 2 );
    optdata->valueTable = FT_NEXT_USHORT( p );
  }


  /*
   * passed tables_size covers whole StateTable, including kern fmt1 header
   */
  static void
  gxv_kern_subtable_fmt1_subtable_setup( FT_UShort      table_size,
                                         FT_UShort      classTable,
                                         FT_UShort      stateArray,
                                         FT_UShort      entryTable,
                                         FT_UShort*     classTable_length_p,
                                         FT_UShort*     stateArray_length_p,
                                         FT_UShort*     entryTable_length_p,
                                         GXV_Validator  valid )
  {
    FT_UShort  o[4];
    FT_UShort  *l[4];
    FT_UShort  buff[5];

    GXV_kern_fmt1_StateOptRecData  optdata =
      (GXV_kern_fmt1_StateOptRecData)valid->statetable.optdata;


    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->valueTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->valueTable_length);

    gxv_set_length_by_ushort_offset( o, l, buff, 4, table_size, valid );
  }


  /*
   * passed table & limit are of whole StateTable, not including subtables
   */
  static void
  gxv_kern_subtable_fmt1_entry_validate(
    FT_Byte                         state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetCPtr  glyphOffset_p,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid )
  {
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  push;
    FT_UShort  dontAdvance;
#endif
    FT_UShort  valueOffset;
#ifdef GXV_LOAD_UNUSED_VARS
    FT_UShort  kernAction;
    FT_UShort  kernValue;
#endif

    FT_UNUSED( state );
    FT_UNUSED( glyphOffset_p );


#ifdef GXV_LOAD_UNUSED_VARS
    push        = (FT_UShort)( ( flags >> 15 ) & 1      );
    dontAdvance = (FT_UShort)( ( flags >> 14 ) & 1      );
#endif
    valueOffset = (FT_UShort)(   flags         & 0x3FFF );

    {
      GXV_kern_fmt1_StateOptRecData  vt_rec =
        (GXV_kern_fmt1_StateOptRecData)valid->statetable.optdata;
      FT_Bytes  p;


      if ( valueOffset < vt_rec->valueTable )
        FT_INVALID_OFFSET;

      p     = table + valueOffset;
      limit = table + vt_rec->valueTable + vt_rec->valueTable_length;

      GXV_LIMIT_CHECK( 2 + 2 );
#ifdef GXV_LOAD_UNUSED_VARS
      kernAction = FT_NEXT_USHORT( p );
      kernValue  = FT_NEXT_USHORT( p );
#endif
    }
  }


  static void
  gxv_kern_subtable_fmt1_validate( FT_Bytes       table,
                                   FT_Bytes       limit,
                                   GXV_Validator  valid )
  {
    FT_Bytes                   p = table;
    GXV_kern_fmt1_StateOptRec  vt_rec;


    GXV_NAME_ENTER( "kern subtable format 1" );

    valid->statetable.optdata =
      &vt_rec;
    valid->statetable.optdata_load_func =
      gxv_kern_subtable_fmt1_valueTable_load;
    valid->statetable.subtable_setup_func =
      gxv_kern_subtable_fmt1_subtable_setup;
    valid->statetable.entry_glyphoffset_fmt =
      GXV_GLYPHOFFSET_NONE;
    valid->statetable.entry_validate_func =
      gxv_kern_subtable_fmt1_entry_validate;

    gxv_StateTable_validate( p, limit, valid );

    GXV_EXIT;
  }


  /* ================ Data for Class-Based Subtables 2, 3 ================ */

  typedef enum  GXV_kern_ClassSpec_
  {
    GXV_KERN_CLS_L = 0,
    GXV_KERN_CLS_R

  } GXV_kern_ClassSpec;


  /* ============================= format 2 ============================== */

  /* ---------------------- format 2 specific data ----------------------- */

  typedef struct  GXV_kern_subtable_fmt2_DataRec_
  {
    FT_UShort         rowWidth;
    FT_UShort         array;
    FT_UShort         offset_min[2];
    FT_UShort         offset_max[2];
    const FT_String*  class_tag[2];
    GXV_odtect_Range  odtect;

  } GXV_kern_subtable_fmt2_DataRec, *GXV_kern_subtable_fmt2_Data;


#define GXV_KERN_FMT2_DATA( field )                         \
        ( ( (GXV_kern_subtable_fmt2_DataRec *)              \
              ( GXV_KERN_DATA( subtable_data ) ) )->field )


  /* -------------------------- utility functions ----------------------- */

  static void
  gxv_kern_subtable_fmt2_clstbl_validate( FT_Bytes            table,
                                          FT_Bytes            limit,
                                          GXV_kern_ClassSpec  spec,
                                          GXV_Validator       valid )
  {
    const FT_String*  tag    = GXV_KERN_FMT2_DATA( class_tag[spec] );
    GXV_odtect_Range  odtect = GXV_KERN_FMT2_DATA( odtect );

    FT_Bytes   p = table;
    FT_UShort  firstGlyph;
    FT_UShort  nGlyphs;


    GXV_NAME_ENTER( "kern format 2 classTable" );

    GXV_LIMIT_CHECK( 2 + 2 );
    firstGlyph = FT_NEXT_USHORT( p );
    nGlyphs    = FT_NEXT_USHORT( p );
    GXV_TRACE(( " %s firstGlyph=%d, nGlyphs=%d\n",
                tag, firstGlyph, nGlyphs ));

    gxv_glyphid_validate( firstGlyph, valid );
    gxv_glyphid_validate( (FT_UShort)( firstGlyph + nGlyphs - 1 ), valid );

    gxv_array_getlimits_ushort( p, p + ( 2 * nGlyphs ),
                                &( GXV_KERN_FMT2_DATA( offset_min[spec] ) ),
                                &( GXV_KERN_FMT2_DATA( offset_max[spec] ) ),
                                valid );

    gxv_odtect_add_range( table, 2 * nGlyphs, tag, odtect );

    GXV_EXIT;
  }


  static void
  gxv_kern_subtable_fmt2_validate( FT_Bytes       table,
                                   FT_Bytes       limit,
                                   GXV_Validator  valid )
  {
    GXV_ODTECT( 3, odtect );
    GXV_kern_subtable_fmt2_DataRec  fmt2_rec =
      { 0, 0, { 0, 0 }, { 0, 0 }, { "leftClass", "rightClass" }, NULL };

    FT_Bytes   p = table + GXV_KERN_SUBTABLE_HEADER_SIZE;
    FT_UShort  leftOffsetTable;
    FT_UShort  rightOffsetTable;


    GXV_NAME_ENTER( "kern subtable format 2" );

    GXV_ODTECT_INIT( odtect );
    fmt2_rec.odtect = odtect;
    GXV_KERN_DATA( subtable_data ) = &fmt2_rec;

    GXV_LIMIT_CHECK( 2 + 2 + 2 + 2 );
    GXV_KERN_FMT2_DATA( rowWidth ) = FT_NEXT_USHORT( p );
    leftOffsetTable                = FT_NEXT_USHORT( p );
    rightOffsetTable               = FT_NEXT_USHORT( p );
    GXV_KERN_FMT2_DATA( array )    = FT_NEXT_USHORT( p );

    GXV_TRACE(( "rowWidth = %d\n", GXV_KERN_FMT2_DATA( rowWidth ) ));


    GXV_LIMIT_CHECK( leftOffsetTable );
    GXV_LIMIT_CHECK( rightOffsetTable );
    GXV_LIMIT_CHECK( GXV_KERN_FMT2_DATA( array ) );

    gxv_kern_subtable_fmt2_clstbl_validate( table + leftOffsetTable, limit,
                                            GXV_KERN_CLS_L, valid );

    gxv_kern_subtable_fmt2_clstbl_validate( table + rightOffsetTable, limit,
                                            GXV_KERN_CLS_R, valid );

    if ( GXV_KERN_FMT2_DATA( offset_min[GXV_KERN_CLS_L] ) +
           GXV_KERN_FMT2_DATA( offset_min[GXV_KERN_CLS_R] )
         < GXV_KERN_FMT2_DATA( array )                      )
      FT_INVALID_OFFSET;

    gxv_odtect_add_range( table + GXV_KERN_FMT2_DATA( array ),
                          GXV_KERN_FMT2_DATA( offset_max[GXV_KERN_CLS_L] )
                            + GXV_KERN_FMT2_DATA( offset_max[GXV_KERN_CLS_R] )
                            - GXV_KERN_FMT2_DATA( array ),
                          "array", odtect );

    gxv_odtect_validate( odtect, valid );

    GXV_EXIT;
  }


  /* ============================= format 3 ============================== */

  static void
  gxv_kern_subtable_fmt3_validate( FT_Bytes       table,
                                   FT_Bytes       limit,
                                   GXV_Validator  valid )
  {
    FT_Bytes   p = table + GXV_KERN_SUBTABLE_HEADER_SIZE;
    FT_UShort  glyphCount;
    FT_Byte    kernValueCount;
    FT_Byte    leftClassCount;
    FT_Byte    rightClassCount;
    FT_Byte    flags;


    GXV_NAME_ENTER( "kern subtable format 3" );

    GXV_LIMIT_CHECK( 2 + 1 + 1 + 1 + 1 );
    glyphCount      = FT_NEXT_USHORT( p );
    kernValueCount  = FT_NEXT_BYTE( p );
    leftClassCount  = FT_NEXT_BYTE( p );
    rightClassCount = FT_NEXT_BYTE( p );
    flags           = FT_NEXT_BYTE( p );

    if ( valid->face->num_glyphs != glyphCount )
    {
      GXV_TRACE(( "maxGID=%d, but glyphCount=%d\n",
                  valid->face->num_glyphs, glyphCount ));
      GXV_SET_ERR_IF_PARANOID( FT_INVALID_GLYPH_ID );
    }

    if ( flags != 0 )
      GXV_TRACE(( "kern subtable fmt3 has nonzero value"
                  " (%d) in unused flag\n", flags ));
    /*
     * just skip kernValue[kernValueCount]
     */
    GXV_LIMIT_CHECK( 2 * kernValueCount );
    p += 2 * kernValueCount;

    /*
     * check leftClass[gid] < leftClassCount
     */
    {
      FT_Byte  min, max;


      GXV_LIMIT_CHECK( glyphCount );
      gxv_array_getlimits_byte( p, p + glyphCount, &min, &max, valid );
      p += valid->subtable_length;

      if ( leftClassCount < max )
        FT_INVALID_DATA;
    }

    /*
     * check rightClass[gid] < rightClassCount
     */
    {
      FT_Byte  min, max;


      GXV_LIMIT_CHECK( glyphCount );
      gxv_array_getlimits_byte( p, p + glyphCount, &min, &max, valid );
      p += valid->subtable_length;

      if ( rightClassCount < max )
        FT_INVALID_DATA;
    }

    /*
     * check kernIndex[i, j] < kernValueCount
     */
    {
      FT_UShort  i, j;


      for ( i = 0; i < leftClassCount; i++ )
      {
        for ( j = 0; j < rightClassCount; j++ )
        {
          GXV_LIMIT_CHECK( 1 );
          if ( kernValueCount < FT_NEXT_BYTE( p ) )
            FT_INVALID_OFFSET;
        }
      }
    }

    valid->subtable_length = p - table;

    GXV_EXIT;
  }


  static FT_Bool
  gxv_kern_coverage_new_apple_validate( FT_UShort      coverage,
                                        FT_UShort*     format,
                                        GXV_Validator  valid )
  {
    /* new Apple-dialect */
#ifdef GXV_LOAD_TRACE_VARS
    FT_Bool  kernVertical;
    FT_Bool  kernCrossStream;
    FT_Bool  kernVariation;
#endif

    FT_UNUSED( valid );


    /* reserved bits = 0 */
    if ( coverage & 0x1FFC )
      return FALSE;

#ifdef GXV_LOAD_TRACE_VARS
    kernVertical    = FT_BOOL( ( coverage >> 15 ) & 1 );
    kernCrossStream = FT_BOOL( ( coverage >> 14 ) & 1 );
    kernVariation   = FT_BOOL( ( coverage >> 13 ) & 1 );
#endif

    *format = (FT_UShort)( coverage & 0x0003 );

    GXV_TRACE(( "new Apple-dialect: "
                "horizontal=%d, cross-stream=%d, variation=%d, format=%d\n",
                 !kernVertical, kernCrossStream, kernVariation, *format ));

    GXV_TRACE(( "kerning values in Apple format subtable are ignored\n" ));

    return TRUE;
  }


  static FT_Bool
  gxv_kern_coverage_classic_apple_validate( FT_UShort      coverage,
                                            FT_UShort*     format,
                                            GXV_Validator  valid )
  {
    /* classic Apple-dialect */
#ifdef GXV_LOAD_TRACE_VARS
    FT_Bool  horizontal;
    FT_Bool  cross_stream;
#endif


    /* check expected flags, but don't check if MS-dialect is impossible */
    if ( !( coverage & 0xFD00 ) && KERN_ALLOWS_MS( valid ) )
      return FALSE;

    /* reserved bits = 0 */
    if ( coverage & 0x02FC )
      return FALSE;

#ifdef GXV_LOAD_TRACE_VARS
    horizontal   = FT_BOOL( ( coverage >> 15 ) & 1 );
    cross_stream = FT_BOOL( ( coverage >> 13 ) & 1 );
#endif

    *format = (FT_UShort)( coverage & 0x0003 );

    GXV_TRACE(( "classic Apple-dialect: "
                "horizontal=%d, cross-stream=%d, format=%d\n",
                 horizontal, cross_stream, *format ));

    /* format 1 requires GX State Machine, too new for classic */
    if ( *format == 1 )
      return FALSE;

    GXV_TRACE(( "kerning values in Apple format subtable are ignored\n" ));

    return TRUE;
  }


  static FT_Bool
  gxv_kern_coverage_classic_microsoft_validate( FT_UShort      coverage,
                                                FT_UShort*     format,
                                                GXV_Validator  valid )
  {
    /* classic Microsoft-dialect */
#ifdef GXV_LOAD_TRACE_VARS
    FT_Bool  horizontal;
    FT_Bool  minimum;
    FT_Bool  cross_stream;
    FT_Bool  override;
#endif

    FT_UNUSED( valid );


    /* reserved bits = 0 */
    if ( coverage & 0xFDF0 )
      return FALSE;

#ifdef GXV_LOAD_TRACE_VARS
    horizontal   = FT_BOOL(   coverage        & 1 );
    minimum      = FT_BOOL( ( coverage >> 1 ) & 1 );
    cross_stream = FT_BOOL( ( coverage >> 2 ) & 1 );
    override     = FT_BOOL( ( coverage >> 3 ) & 1 );
#endif

    *format = (FT_UShort)( ( coverage >> 8 ) & 0x0003 );

    GXV_TRACE(( "classic Microsoft-dialect: "
                "horizontal=%d, minimum=%d, cross-stream=%d, "
                "override=%d, format=%d\n",
                horizontal, minimum, cross_stream, override, *format ));

    if ( *format == 2 )
      GXV_TRACE((
        "kerning values in Microsoft format 2 subtable are ignored\n" ));

    return TRUE;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                            MAIN                               *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static GXV_kern_Dialect
  gxv_kern_coverage_validate( FT_UShort      coverage,
                              FT_UShort*     format,
                              GXV_Validator  valid )
  {
    GXV_kern_Dialect  result = KERN_DIALECT_UNKNOWN;


    GXV_NAME_ENTER( "validating coverage" );

    GXV_TRACE(( "interprete coverage 0x%04x by Apple style\n", coverage ));

    if ( KERN_IS_NEW( valid ) )
    {
      if ( gxv_kern_coverage_new_apple_validate( coverage,
                                                 format,
                                                 valid ) )
      {
        result = KERN_DIALECT_APPLE;
        goto Exit;
      }
    }

    if ( KERN_IS_CLASSIC( valid ) && KERN_ALLOWS_APPLE( valid ) )
    {
      if ( gxv_kern_coverage_classic_apple_validate( coverage,
                                                     format,
                                                     valid ) )
      {
        result = KERN_DIALECT_APPLE;
        goto Exit;
      }
    }

    if ( KERN_IS_CLASSIC( valid ) && KERN_ALLOWS_MS( valid ) )
    {
      if ( gxv_kern_coverage_classic_microsoft_validate( coverage,
                                                         format,
                                                         valid ) )
      {
        result = KERN_DIALECT_MS;
        goto Exit;
      }
    }

    GXV_TRACE(( "cannot interprete coverage, broken kern subtable\n" ));

  Exit:
    GXV_EXIT;
    return result;
  }


  static void
  gxv_kern_subtable_validate( FT_Bytes       table,
                              FT_Bytes       limit,
                              GXV_Validator  valid )
  {
    FT_Bytes   p = table;
#ifdef GXV_LOAD_TRACE_VARS
    FT_UShort  version = 0;    /* MS only: subtable version, unused */
#endif
    FT_ULong   length;         /* MS: 16bit, Apple: 32bit*/
    FT_UShort  coverage;
#ifdef GXV_LOAD_TRACE_VARS
    FT_UShort  tupleIndex = 0; /* Apple only */
#endif
    FT_UShort  u16[2];
    FT_UShort  format = 255;   /* subtable format */


    GXV_NAME_ENTER( "kern subtable" );

    GXV_LIMIT_CHECK( 2 + 2 + 2 );
    u16[0]   = FT_NEXT_USHORT( p ); /* Apple: length_hi MS: version */
    u16[1]   = FT_NEXT_USHORT( p ); /* Apple: length_lo MS: length */
    coverage = FT_NEXT_USHORT( p );

    switch ( gxv_kern_coverage_validate( coverage, &format, valid ) )
    {
    case KERN_DIALECT_MS:
#ifdef GXV_LOAD_TRACE_VARS
      version    = u16[0];
#endif
      length     = u16[1];
#ifdef GXV_LOAD_TRACE_VARS
      tupleIndex = 0;
#endif
      GXV_TRACE(( "Subtable version = %d\n", version ));
      GXV_TRACE(( "Subtable length = %d\n", length ));
      break;

    case KERN_DIALECT_APPLE:
#ifdef GXV_LOAD_TRACE_VARS
      version    = 0;
#endif
      length     = ( u16[0] << 16 ) + u16[1];
#ifdef GXV_LOAD_TRACE_VARS
      tupleIndex = 0;
#endif
      GXV_TRACE(( "Subtable length = %d\n", length ));

      if ( KERN_IS_NEW( valid ) )
      {
        GXV_LIMIT_CHECK( 2 );
#ifdef GXV_LOAD_TRACE_VARS
        tupleIndex = FT_NEXT_USHORT( p );
#else
        p += 2;
#endif
        GXV_TRACE(( "Subtable tupleIndex = %d\n", tupleIndex ));
      }
      break;

    default:
      length = u16[1];
      GXV_TRACE(( "cannot detect subtable dialect, "
                  "just skip %d byte\n", length ));
      goto Exit;
    }

    /* formats 1, 2, 3 require the position of the start of this subtable */
    if ( format == 0 )
      gxv_kern_subtable_fmt0_validate( table, table + length, valid );
    else if ( format == 1 )
      gxv_kern_subtable_fmt1_validate( table, table + length, valid );
    else if ( format == 2 )
      gxv_kern_subtable_fmt2_validate( table, table + length, valid );
    else if ( format == 3 )
      gxv_kern_subtable_fmt3_validate( table, table + length, valid );
    else
      FT_INVALID_DATA;

  Exit:
    valid->subtable_length = length;
    GXV_EXIT;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         kern TABLE                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_kern_validate_generic( FT_Bytes          table,
                             FT_Face           face,
                             FT_Bool           classic_only,
                             GXV_kern_Dialect  dialect_request,
                             FT_Validator      ftvalid )
  {
    GXV_ValidatorRec   validrec;
    GXV_Validator      valid = &validrec;

    GXV_kern_DataRec   kernrec;
    GXV_kern_Data      kern = &kernrec;

    FT_Bytes           p     = table;
    FT_Bytes           limit = 0;

    FT_ULong           nTables = 0;
    FT_UInt            i;


    valid->root       = ftvalid;
    valid->table_data = kern;
    valid->face       = face;

    FT_TRACE3(( "validating `kern' table\n" ));
    GXV_INIT;
    KERN_DIALECT( valid ) = dialect_request;

    GXV_LIMIT_CHECK( 2 );
    GXV_KERN_DATA( version ) = (GXV_kern_Version)FT_NEXT_USHORT( p );
    GXV_TRACE(( "version 0x%04x (higher 16bit)\n",
                GXV_KERN_DATA( version ) ));

    if ( 0x0001 < GXV_KERN_DATA( version ) )
      FT_INVALID_FORMAT;
    else if ( KERN_IS_CLASSIC( valid ) )
    {
      GXV_LIMIT_CHECK( 2 );
      nTables = FT_NEXT_USHORT( p );
    }
    else if ( KERN_IS_NEW( valid ) )
    {
      if ( classic_only )
        FT_INVALID_FORMAT;

      if ( 0x0000 != FT_NEXT_USHORT( p ) )
        FT_INVALID_FORMAT;

      GXV_LIMIT_CHECK( 4 );
      nTables = FT_NEXT_ULONG( p );
    }

    for ( i = 0; i < nTables; i++ )
    {
      GXV_TRACE(( "validating subtable %d/%d\n", i, nTables ));
      /* p should be 32bit-aligned? */
      gxv_kern_subtable_validate( p, 0, valid );
      p += valid->subtable_length;
    }

    FT_TRACE4(( "\n" ));
  }


  FT_LOCAL_DEF( void )
  gxv_kern_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    gxv_kern_validate_generic( table, face, 0, KERN_DIALECT_ANY, ftvalid );
  }


  FT_LOCAL_DEF( void )
  gxv_kern_validate_classic( FT_Bytes      table,
                             FT_Face       face,
                             FT_Int        dialect_flags,
                             FT_Validator  ftvalid )
  {
    GXV_kern_Dialect  dialect_request;


    dialect_request = (GXV_kern_Dialect)dialect_flags;
    gxv_kern_validate_generic( table, face, 1, dialect_request, ftvalid );
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvopbd.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT opbd table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvopbd


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  typedef struct  GXV_opbd_DataRec_
  {
    FT_UShort  format;
    FT_UShort  valueOffset_min;

  } GXV_opbd_DataRec, *GXV_opbd_Data;


#define GXV_OPBD_DATA( FIELD )  GXV_TABLE_DATA( opbd, FIELD )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_opbd_LookupValue_validate( FT_UShort            glyph,
                                 GXV_LookupValueCPtr  value_p,
                                 GXV_Validator        valid )
  {
    /* offset in LookupTable is measured from the head of opbd table */
    FT_Bytes   p     = valid->root->base + value_p->u;
    FT_Bytes   limit = valid->root->limit;
    FT_Short   delta_value;
    int        i;


    if ( value_p->u < GXV_OPBD_DATA( valueOffset_min ) )
      GXV_OPBD_DATA( valueOffset_min ) = value_p->u;

    for ( i = 0; i < 4; i++ )
    {
      GXV_LIMIT_CHECK( 2 );
      delta_value = FT_NEXT_SHORT( p );

      if ( GXV_OPBD_DATA( format ) )    /* format 1, value is ctrl pt. */
      {
        if ( delta_value == -1 )
          continue;

        gxv_ctlPoint_validate( glyph, delta_value, valid );
      }
      else                              /* format 0, value is distance */
        continue;
    }
  }


  /*
    opbd ---------------------+
                              |
    +===============+         |
    | lookup header |         |
    +===============+         |
    | BinSrchHeader |         |
    +===============+         |
    | lastGlyph[0]  |         |
    +---------------+         |
    | firstGlyph[0] |         |  head of opbd sfnt table
    +---------------+         |             +
    | offset[0]     |    ->   |          offset            [byte]
    +===============+         |             +
    | lastGlyph[1]  |         | (glyphID - firstGlyph) * 4 * sizeof(FT_Short) [byte]
    +---------------+         |
    | firstGlyph[1] |         |
    +---------------+         |
    | offset[1]     |         |
    +===============+         |
                              |
     ....                     |
                              |
    48bit value array         |
    +===============+         |
    |     value     | <-------+
    |               |
    |               |
    |               |
    +---------------+
    .... */

  static GXV_LookupValueDesc
  gxv_opbd_LookupFmt4_transit( FT_UShort            relative_gindex,
                               GXV_LookupValueCPtr  base_value_p,
                               FT_Bytes             lookuptbl_limit,
                               GXV_Validator        valid )
  {
    GXV_LookupValueDesc  value;

    FT_UNUSED( lookuptbl_limit );
    FT_UNUSED( valid );

    /* XXX: check range? */
    value.u = (FT_UShort)( base_value_p->u +
                           relative_gindex * 4 * sizeof ( FT_Short ) );

    return value;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         opbd TABLE                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_opbd_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;
    GXV_opbd_DataRec  opbdrec;
    GXV_opbd_Data     opbd  = &opbdrec;
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;

    FT_ULong  version;


    valid->root       = ftvalid;
    valid->table_data = opbd;
    valid->face       = face;

    FT_TRACE3(( "validating `opbd' table\n" ));
    GXV_INIT;
    GXV_OPBD_DATA( valueOffset_min ) = 0xFFFFU;


    GXV_LIMIT_CHECK( 4 + 2 );
    version                 = FT_NEXT_ULONG( p );
    GXV_OPBD_DATA( format ) = FT_NEXT_USHORT( p );


    /* only 0x00010000 is defined (1996) */
    GXV_TRACE(( "(version=0x%08x)\n", version ));
    if ( 0x00010000UL != version )
      FT_INVALID_FORMAT;

    /* only values 0 and 1 are defined (1996) */
    GXV_TRACE(( "(format=0x%04x)\n", GXV_OPBD_DATA( format ) ));
    if ( 0x0001 < GXV_OPBD_DATA( format ) )
      FT_INVALID_FORMAT;

    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_opbd_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_opbd_LookupFmt4_transit;

    gxv_LookupTable_validate( p, limit, valid );
    p += valid->subtable_length;

    if ( p > table + GXV_OPBD_DATA( valueOffset_min ) )
    {
      GXV_TRACE((
        "found overlap between LookupTable and opbd_value array\n" ));
      FT_INVALID_OFFSET;
    }

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvprop.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT prop table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvprop


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

#define GXV_PROP_HEADER_SIZE  ( 4 + 2 + 2 )
#define GXV_PROP_SIZE_MIN     GXV_PROP_HEADER_SIZE

  typedef struct  GXV_prop_DataRec_
  {
    FT_Fixed  version;

  } GXV_prop_DataRec, *GXV_prop_Data;

#define GXV_PROP_DATA( field )  GXV_TABLE_DATA( prop, field )

#define GXV_PROP_FLOATER                      0x8000U
#define GXV_PROP_USE_COMPLEMENTARY_BRACKET    0x1000U
#define GXV_PROP_COMPLEMENTARY_BRACKET_OFFSET 0x0F00U
#define GXV_PROP_ATTACHING_TO_RIGHT           0x0080U
#define GXV_PROP_RESERVED                     0x0060U
#define GXV_PROP_DIRECTIONALITY_CLASS         0x001FU


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_prop_zero_advance_validate( FT_UShort      gid,
                                  GXV_Validator  valid )
  {
    FT_Face       face;
    FT_Error      error;
    FT_GlyphSlot  glyph;


    GXV_NAME_ENTER( "zero advance" );

    face = valid->face;

    error = FT_Load_Glyph( face,
                           gid,
                           FT_LOAD_IGNORE_TRANSFORM );
    if ( error )
      FT_INVALID_GLYPH_ID;

    glyph = face->glyph;

    if ( glyph->advance.x != (FT_Pos)0 ||
         glyph->advance.y != (FT_Pos)0 )
    {
      GXV_TRACE(( "  found non-zero advance in zero-advance glyph\n" ));
      FT_INVALID_DATA;
    }

    GXV_EXIT;
  }


  /* Pass 0 as GLYPH to check the default property */
  static void
  gxv_prop_property_validate( FT_UShort      property,
                              FT_UShort      glyph,
                              GXV_Validator  valid )
  {
    if ( glyph != 0 && ( property & GXV_PROP_FLOATER ) )
      gxv_prop_zero_advance_validate( glyph, valid );

    if ( property & GXV_PROP_USE_COMPLEMENTARY_BRACKET )
    {
      FT_UShort  offset;
      char       complement;


      offset = (FT_UShort)( property & GXV_PROP_COMPLEMENTARY_BRACKET_OFFSET );
      if ( offset == 0 )
      {
        GXV_TRACE(( "  found zero offset to property\n" ));
        FT_INVALID_OFFSET;
      }

      complement = (char)( offset >> 8 );
      if ( complement & 0x08 )
      {
        /* Top bit is set: negative */

        /* Calculate the absolute offset */
        complement = (char)( ( complement & 0x07 ) + 1 );

        /* The gid for complement must be greater than 0 */
        if ( glyph <= complement )
        {
          GXV_TRACE(( "  found non-positive glyph complement\n" ));
          FT_INVALID_DATA;
        }
      }
      else
      {
        /* The gid for complement must be the face. */
        gxv_glyphid_validate( (FT_UShort)( glyph + complement ), valid );
      }
    }
    else
    {
      if ( property & GXV_PROP_COMPLEMENTARY_BRACKET_OFFSET )
        GXV_TRACE(( "glyph %d cannot have complementary bracketing\n",
                    glyph ));
    }

    /* this is introduced in version 2.0 */
    if ( property & GXV_PROP_ATTACHING_TO_RIGHT )
    {
      if ( GXV_PROP_DATA( version ) == 0x00010000UL )
      {
        GXV_TRACE(( "  found older version (1.0) in new version table\n" ));
        FT_INVALID_DATA;
      }
    }

    if ( property & GXV_PROP_RESERVED )
    {
      GXV_TRACE(( "  found non-zero bits in reserved bits\n" ));
      FT_INVALID_DATA;
    }

    if ( ( property & GXV_PROP_DIRECTIONALITY_CLASS ) > 11 )
    {
      /* TODO: Too restricted. Use the validation level. */
      if ( GXV_PROP_DATA( version ) == 0x00010000UL ||
           GXV_PROP_DATA( version ) == 0x00020000UL )
      {
        GXV_TRACE(( "  found too old version in directionality class\n" ));
        FT_INVALID_DATA;
      }
    }
  }


  static void
  gxv_prop_LookupValue_validate( FT_UShort            glyph,
                                 GXV_LookupValueCPtr  value_p,
                                 GXV_Validator        valid )
  {
    gxv_prop_property_validate( value_p->u, glyph, valid );
  }


  /*
    +===============+ --------+
    | lookup header |         |
    +===============+         |
    | BinSrchHeader |         |
    +===============+         |
    | lastGlyph[0]  |         |
    +---------------+         |
    | firstGlyph[0] |         |    head of lookup table
    +---------------+         |             +
    | offset[0]     |    ->   |          offset            [byte]
    +===============+         |             +
    | lastGlyph[1]  |         | (glyphID - firstGlyph) * 2 [byte]
    +---------------+         |
    | firstGlyph[1] |         |
    +---------------+         |
    | offset[1]     |         |
    +===============+         |
                              |
     ...                      |
                              |
    16bit value array         |
    +===============+         |
    |     value     | <-------+
    ...
  */

  static GXV_LookupValueDesc
  gxv_prop_LookupFmt4_transit( FT_UShort            relative_gindex,
                               GXV_LookupValueCPtr  base_value_p,
                               FT_Bytes             lookuptbl_limit,
                               GXV_Validator        valid )
  {
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;

    /* XXX: check range? */
    offset = (FT_UShort)( base_value_p->u +
                          relative_gindex * sizeof ( FT_UShort ) );
    p      = valid->lookuptbl_head + offset;
    limit  = lookuptbl_limit;

    GXV_LIMIT_CHECK ( 2 );
    value.u = FT_NEXT_USHORT( p );

    return value;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                         prop TABLE                            *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_prop_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;

    GXV_prop_DataRec  proprec;
    GXV_prop_Data     prop = &proprec;

    FT_Fixed          version;
    FT_UShort         format;
    FT_UShort         defaultProp;


    valid->root       = ftvalid;
    valid->table_data = prop;
    valid->face       = face;

    FT_TRACE3(( "validating `prop' table\n" ));
    GXV_INIT;

    GXV_LIMIT_CHECK( 4 + 2 + 2 );
    version     = FT_NEXT_ULONG( p );
    format      = FT_NEXT_USHORT( p );
    defaultProp = FT_NEXT_USHORT( p );

    GXV_TRACE(( "  version 0x%08x\n", version ));
    GXV_TRACE(( "  format  0x%04x\n", format ));
    GXV_TRACE(( "  defaultProp  0x%04x\n", defaultProp ));

    /* only versions 1.0, 2.0, 3.0 are defined (1996) */
    if ( version != 0x00010000UL &&
         version != 0x00020000UL &&
         version != 0x00030000UL )
    {
      GXV_TRACE(( "  found unknown version\n" ));
      FT_INVALID_FORMAT;
    }


    /* only formats 0x0000, 0x0001 are defined (1996) */
    if ( format > 1 )
    {
      GXV_TRACE(( "  found unknown format\n" ));
      FT_INVALID_FORMAT;
    }

    gxv_prop_property_validate( defaultProp, 0, valid );

    if ( format == 0 )
    {
      FT_TRACE3(( "(format 0, no per-glyph properties, "
                  "remaining %d bytes are skipped)", limit - p ));
      goto Exit;
    }

    /* format == 1 */
    GXV_PROP_DATA( version ) = version;

    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_prop_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_prop_LookupFmt4_transit;

    gxv_LookupTable_validate( p, limit, valid );

  Exit:
    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvlcar.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT lcar table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "gxvalid.h"
#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvlcar


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      Data and Types                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  typedef struct  GXV_lcar_DataRec_
  {
    FT_UShort  format;

  } GXV_lcar_DataRec, *GXV_lcar_Data;


#define GXV_LCAR_DATA( FIELD )  GXV_TABLE_DATA( lcar, FIELD )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                      UTILITY FUNCTIONS                        *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  static void
  gxv_lcar_partial_validate( FT_UShort      partial,
                             FT_UShort      glyph,
                             GXV_Validator  valid )
  {
    GXV_NAME_ENTER( "partial" );

    if ( GXV_LCAR_DATA( format ) != 1 )
      goto Exit;

    gxv_ctlPoint_validate( glyph, partial, valid );

  Exit:
    GXV_EXIT;
  }


  static void
  gxv_lcar_LookupValue_validate( FT_UShort            glyph,
                                 GXV_LookupValueCPtr  value_p,
                                 GXV_Validator        valid )
  {
    FT_Bytes   p     = valid->root->base + value_p->u;
    FT_Bytes   limit = valid->root->limit;
    FT_UShort  count;
    FT_Short   partial;
    FT_UShort  i;


    GXV_NAME_ENTER( "element in lookupTable" );

    GXV_LIMIT_CHECK( 2 );
    count = FT_NEXT_USHORT( p );

    GXV_LIMIT_CHECK( 2 * count );
    for ( i = 0; i < count; i++ )
    {
      partial = FT_NEXT_SHORT( p );
      gxv_lcar_partial_validate( partial, glyph, valid );
    }

    GXV_EXIT;
  }


  /*
    +------ lcar --------------------+
    |                                |
    |      +===============+         |
    |      | looup header  |         |
    |      +===============+         |
    |      | BinSrchHeader |         |
    |      +===============+         |
    |      | lastGlyph[0]  |         |
    |      +---------------+         |
    |      | firstGlyph[0] |         |  head of lcar sfnt table
    |      +---------------+         |             +
    |      | offset[0]     |    ->   |          offset            [byte]
    |      +===============+         |             +
    |      | lastGlyph[1]  |         | (glyphID - firstGlyph) * 2 [byte]
    |      +---------------+         |
    |      | firstGlyph[1] |         |
    |      +---------------+         |
    |      | offset[1]     |         |
    |      +===============+         |
    |                                |
    |       ....                     |
    |                                |
    |      16bit value array         |
    |      +===============+         |
    +------|     value     | <-------+
    |       ....
    |
    |
    |
    |
    |
    +---->  lcar values...handled by lcar callback function
  */

  static GXV_LookupValueDesc
  gxv_lcar_LookupFmt4_transit( FT_UShort            relative_gindex,
                               GXV_LookupValueCPtr  base_value_p,
                               FT_Bytes             lookuptbl_limit,
                               GXV_Validator        valid )
  {
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;

    FT_UNUSED( lookuptbl_limit );

    /* XXX: check range? */
    offset = (FT_UShort)( base_value_p->u +
                          relative_gindex * sizeof ( FT_UShort ) );
    p      = valid->root->base + offset;
    limit  = valid->root->limit;

    GXV_LIMIT_CHECK ( 2 );
    value.u = FT_NEXT_USHORT( p );

    return value;
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                          lcar TABLE                           *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  FT_LOCAL_DEF( void )
  gxv_lcar_validate( FT_Bytes      table,
                     FT_Face       face,
                     FT_Validator  ftvalid )
  {
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;

    GXV_lcar_DataRec  lcarrec;
    GXV_lcar_Data     lcar = &lcarrec;

    FT_Fixed          version;


    valid->root       = ftvalid;
    valid->table_data = lcar;
    valid->face       = face;

    FT_TRACE3(( "validating `lcar' table\n" ));
    GXV_INIT;

    GXV_LIMIT_CHECK( 4 + 2 );
    version = FT_NEXT_ULONG( p );
    GXV_LCAR_DATA( format ) = FT_NEXT_USHORT( p );

    if ( version != 0x00010000UL)
      FT_INVALID_FORMAT;

    if ( GXV_LCAR_DATA( format ) > 1 )
      FT_INVALID_FORMAT;

    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_lcar_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_lcar_LookupFmt4_transit;
    gxv_LookupTable_validate( p, limit, valid );

    FT_TRACE4(( "\n" ));
  }


/* END */
/***************************************************************************/
/*                                                                         */
/*  gxvmod.c                                                               */
/*                                                                         */
/*    FreeType's TrueTypeGX/AAT validation module implementation (body).   */
/*                                                                         */
/*  Copyright 2004-2006, 2013                                              */
/*  by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/


#include "ft2build.h"
#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_TAGS_H
#include FT_GX_VALIDATE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_SERVICE_GX_VALIDATE_H

#include "gxvmod.h"
#include "gxvalid.h"
#include "gxvcommn.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmodule


  static FT_Error
  gxv_load_table( FT_Face             face,
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


#define GXV_TABLE_DECL( _sfnt )                     \
          FT_Byte* volatile  _sfnt          = NULL; \
          FT_ULong            len_ ## _sfnt = 0

#define GXV_TABLE_LOAD( _sfnt )                                     \
          if ( ( FT_VALIDATE_ ## _sfnt ## _INDEX < table_count ) && \
               ( gx_flags & FT_VALIDATE_ ## _sfnt )            )    \
          {                                                         \
            error = gxv_load_table( face, TTAG_ ## _sfnt,           \
                                    &_sfnt, &len_ ## _sfnt );       \
            if ( error )                                            \
              goto Exit;                                            \
          }

#define GXV_TABLE_VALIDATE( _sfnt )                                  \
          if ( _sfnt )                                               \
          {                                                          \
            ft_validator_init( &valid, _sfnt, _sfnt + len_ ## _sfnt, \
                               FT_VALIDATE_DEFAULT );                \
            if ( ft_setjmp( valid.jump_buffer ) == 0 )               \
              gxv_ ## _sfnt ## _validate( _sfnt, face, &valid );     \
            error = valid.error;                                     \
            if ( error )                                             \
              goto Exit;                                             \
          }

#define GXV_TABLE_SET( _sfnt )                                        \
          if ( FT_VALIDATE_ ## _sfnt ## _INDEX < table_count )        \
            tables[FT_VALIDATE_ ## _sfnt ## _INDEX] = (FT_Bytes)_sfnt


  static FT_Error
  gxv_validate( FT_Face   face,
                FT_UInt   gx_flags,
                FT_Bytes  tables[FT_VALIDATE_GX_LENGTH],
                FT_UInt   table_count )
  {
    FT_Memory volatile        memory = FT_FACE_MEMORY( face );

    FT_Error                  error = FT_Err_Ok;
    FT_ValidatorRec volatile  valid;

    FT_UInt  i;


    GXV_TABLE_DECL( feat );
    GXV_TABLE_DECL( bsln );
    GXV_TABLE_DECL( trak );
    GXV_TABLE_DECL( just );
    GXV_TABLE_DECL( mort );
    GXV_TABLE_DECL( morx );
    GXV_TABLE_DECL( kern );
    GXV_TABLE_DECL( opbd );
    GXV_TABLE_DECL( prop );
    GXV_TABLE_DECL( lcar );

    for ( i = 0; i < table_count; i++ )
      tables[i] = 0;

    /* load tables */
    GXV_TABLE_LOAD( feat );
    GXV_TABLE_LOAD( bsln );
    GXV_TABLE_LOAD( trak );
    GXV_TABLE_LOAD( just );
    GXV_TABLE_LOAD( mort );
    GXV_TABLE_LOAD( morx );
    GXV_TABLE_LOAD( kern );
    GXV_TABLE_LOAD( opbd );
    GXV_TABLE_LOAD( prop );
    GXV_TABLE_LOAD( lcar );

    /* validate tables */
    GXV_TABLE_VALIDATE( feat );
    GXV_TABLE_VALIDATE( bsln );
    GXV_TABLE_VALIDATE( trak );
    GXV_TABLE_VALIDATE( just );
    GXV_TABLE_VALIDATE( mort );
    GXV_TABLE_VALIDATE( morx );
    GXV_TABLE_VALIDATE( kern );
    GXV_TABLE_VALIDATE( opbd );
    GXV_TABLE_VALIDATE( prop );
    GXV_TABLE_VALIDATE( lcar );

    /* Set results */
    GXV_TABLE_SET( feat );
    GXV_TABLE_SET( mort );
    GXV_TABLE_SET( morx );
    GXV_TABLE_SET( bsln );
    GXV_TABLE_SET( just );
    GXV_TABLE_SET( kern );
    GXV_TABLE_SET( opbd );
    GXV_TABLE_SET( trak );
    GXV_TABLE_SET( prop );
    GXV_TABLE_SET( lcar );

  Exit:
    if ( error )
    {
      FT_FREE( feat );
      FT_FREE( bsln );
      FT_FREE( trak );
      FT_FREE( just );
      FT_FREE( mort );
      FT_FREE( morx );
      FT_FREE( kern );
      FT_FREE( opbd );
      FT_FREE( prop );
      FT_FREE( lcar );
    }

    return error;
  }


  static FT_Error
  classic_kern_validate( FT_Face    face,
                         FT_UInt    ckern_flags,
                         FT_Bytes*  ckern_table )
  {
    FT_Memory volatile        memory = FT_FACE_MEMORY( face );

    FT_Byte* volatile         ckern     = NULL;
    FT_ULong                  len_ckern = 0;

    /* without volatile on `error' GCC 4.1.1. emits:                         */
    /*  warning: variable 'error' might be clobbered by 'longjmp' or 'vfork' */
    /* this warning seems spurious but ---                                   */
    FT_Error volatile         error;
    FT_ValidatorRec volatile  valid;


    *ckern_table = NULL;

    error = gxv_load_table( face, TTAG_kern, &ckern, &len_ckern );
    if ( error )
      goto Exit;

    if ( ckern )
    {
      ft_validator_init( &valid, ckern, ckern + len_ckern,
                         FT_VALIDATE_DEFAULT );
      if ( ft_setjmp( valid.jump_buffer ) == 0 )
        gxv_kern_validate_classic( ckern, face,
                                   ckern_flags & FT_VALIDATE_CKERN, &valid );
      error = valid.error;
      if ( error )
        goto Exit;
    }

    *ckern_table = ckern;

  Exit:
    if ( error )
      FT_FREE( ckern );

    return error;
  }


  static
  const FT_Service_GXvalidateRec  gxvalid_interface =
  {
    gxv_validate
  };


  static
  const FT_Service_CKERNvalidateRec  ckernvalid_interface =
  {
    classic_kern_validate
  };


  static
  const FT_ServiceDescRec  gxvalid_services[] =
  {
    { FT_SERVICE_ID_GX_VALIDATE,          &gxvalid_interface },
    { FT_SERVICE_ID_CLASSICKERN_VALIDATE, &ckernvalid_interface },
    { NULL, NULL }
  };


  static FT_Pointer
  gxvalid_get_service( FT_Module    module,
                       const char*  service_id )
  {
    FT_UNUSED( module );

    return ft_service_list_lookup( gxvalid_services, service_id );
  }


  FT_CALLBACK_TABLE_DEF
  const FT_Module_Class  gxv_module_class =
  {
    0,
    sizeof ( FT_ModuleRec ),
    "gxvalid",
    0x10000L,
    0x20000L,

    0,              /* module-specific interface */

    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  gxvalid_get_service
  };


/* END */


/* END */
