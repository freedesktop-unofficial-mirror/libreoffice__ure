/*************************************************************************
 *
 *  $RCSfile: nlsupport.c,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obr $ $Date: 2001-05-14 09:48:55 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include <osl/nlsupport.h>

#if defined(LINUX) || defined(SOLARIS)
#include <pthread.h>
#include <locale.h>
#include <langinfo.h>
#endif

/*****************************************************************************/
/* typedefs
/*****************************************************************************/


typedef struct {
    const char              *key;
    const rtl_TextEncoding   value;
} _pair;


/*****************************************************************************/
/* compare function for binary search   
/*****************************************************************************/

static int
_pair_compare (const char *key, const _pair *pair)
{
    int result = rtl_str_compareIgnoreAsciiCase( key, pair->key );
    return result;
}

/*****************************************************************************/
/* binary search on encoding tables
/*****************************************************************************/

static const _pair*
_pair_search (const char *key, const _pair *base, unsigned int member )
{
    unsigned int lower = 0;
    unsigned int upper = member;
    unsigned int current;
    int comparison;

    /* check for validity of input */
    if ( (key == NULL) || (base == NULL) || (member == 0) )
        return NULL;

    /* binary search */
    while ( lower < upper )
    {
        current = (lower + upper) / 2;
        comparison = _pair_compare( key, base + current );
        if (comparison < 0)
            upper = current;
        else
        if (comparison > 0)
            lower = current + 1;
        else
            return base + current;
    }

    return NULL;
}



#if defined(LINUX) || defined(SOLARIS)

/*
 * This implementation of osl_getTextEncodingFromLocale maps 
 * from nl_langinfo(CODESET) to rtl_textencoding defines. 
 * nl_langinfo() is supported only on Linux and Solaris. 
 *
 * This routine is SLOW because of the setlocale call, so
 * grab the result and cache it.
 *
 * XXX this code has the usual mt problems aligned with setlocale() XXX
 */

#ifdef LINUX
#if !defined(CODESET)
#define CODESET _NL_CTYPE_CODESET_NAME
#endif
#endif


#if defined(SOLARIS)

const _pair _nl_language_list[] = {
    { "5601",           RTL_TEXTENCODING_EUC_KR         }, /* ko_KR.EUC */
    { "646",            RTL_TEXTENCODING_ISO_8859_1     }, /* fake: ASCII_US */
    { "ANSI-1251",      RTL_TEXTENCODING_MS_1251        }, /* ru_RU.ANSI1251 */
    { "BIG5",           RTL_TEXTENCODING_BIG5           },
    { "CNS11643",       RTL_TEXTENCODING_EUC_TW         }, /* zh_TW.EUC */
    { "EUCJP",          RTL_TEXTENCODING_EUC_JP         },
    { "GB2312",         RTL_TEXTENCODING_EUC_CN         }, /* zh_CN.EUC */
    { "GBK",            RTL_TEXTENCODING_GBK            }, /* zh_CN.GBK */
    { "ISO8859-1",      RTL_TEXTENCODING_ISO_8859_1     },
    { "ISO8859-13",     RTL_TEXTENCODING_DONTKNOW       }, /* lt_LT lv_LV */
    { "ISO8859-14",     RTL_TEXTENCODING_ISO_8859_14    },
    { "ISO8859-15",     RTL_TEXTENCODING_ISO_8859_15    },
    { "ISO8859-2",      RTL_TEXTENCODING_ISO_8859_2     },
    { "ISO8859-3",      RTL_TEXTENCODING_ISO_8859_3     },
    { "ISO8859-4",      RTL_TEXTENCODING_ISO_8859_4     },
    { "ISO8859-5",      RTL_TEXTENCODING_ISO_8859_5     },
    { "ISO8859-6",      RTL_TEXTENCODING_ISO_8859_6     },
    { "ISO8859-7",      RTL_TEXTENCODING_ISO_8859_7     },
    { "ISO8859-8",      RTL_TEXTENCODING_ISO_8859_8     },
    { "ISO8859-9",      RTL_TEXTENCODING_ISO_8859_9     },
    { "KOI8-R",         RTL_TEXTENCODING_KOI8_R         },
    { "PCK",            RTL_TEXTENCODING_MS_932         },
    { "SUN_EU_GREEK",   RTL_TEXTENCODING_ISO_8859_7     }, /* 8859-7 + Euro */
    { "TIS620.2533",    RTL_TEXTENCODING_MS_874         }, /* th_TH.TIS620 */
    { "UTF-8",          RTL_TEXTENCODING_UTF8           }
};

/* XXX MS-874 is an extension to tis620, so this is not
 * really equivalent */

#elif defined(LINUX)

const _pair _nl_language_list[] = {
    { "ANSI_X3.110-1983",           RTL_TEXTENCODING_DONTKNOW   },  /* ISO-IR-99 NAPLPS */
    { "ANSI_X3.4-1968",             RTL_TEXTENCODING_ISO_8859_1 },  /* fake: ASCII_US */
    { "ASMO_449",                   RTL_TEXTENCODING_DONTKNOW },    /* ISO_9036 ARABIC7 */
    { "BALTIC",                     RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-179 */
    { "BS_4730",                    RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-4 ISO646-GB */
    { "BS_VIEWDATA",                RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-47 */
    { "CP1250",                     RTL_TEXTENCODING_MS_1250 },     /* MS-EE */
    { "CP1251",                     RTL_TEXTENCODING_MS_1251 },     /* MS-CYRL */
    { "CP1252",                     RTL_TEXTENCODING_MS_1252 },     /* MS-ANSI */
    { "CP1253",                     RTL_TEXTENCODING_MS_1253 },     /* MS-GREEK */
    { "CP1254",                     RTL_TEXTENCODING_MS_1254 },     /* MS-TURK */
    { "CP1255",                     RTL_TEXTENCODING_MS_1255 },     /* MS-HEBR */
    { "CP1256",                     RTL_TEXTENCODING_MS_1256 },     /* MS-ARAB */
    { "CP1257",                     RTL_TEXTENCODING_MS_1257 },     /* WINBALTRIM */
    { "CSA_Z243.4-1985-1",          RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-121 */
    { "CSA_Z243.4-1985-2",          RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-122 CSA7-2 */
    { "CSA_Z243.4-1985-GR",         RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-123 */
    { "CSN_369103",                 RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-139 */
    { "CWI",                        RTL_TEXTENCODING_DONTKNOW },    /* CWI-2 CP-HU */
    { "DEC-MCS",                    RTL_TEXTENCODING_DONTKNOW },    /* DEC */
    { "DIN_66003",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-21 */
    { "DS_2089",                    RTL_TEXTENCODING_DONTKNOW },    /* DS2089 ISO646-DK */
    { "EBCDIC-AT-DE",               RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-AT-DE-A",             RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-CA-FR",               RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-DK-NO",               RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-DK-NO-A",             RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-ES",                  RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-ES-A",                RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-ES-S",                RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-FI-SE",               RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-FI-SE-A",             RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-FR",                  RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-IS-FRISS",            RTL_TEXTENCODING_DONTKNOW },    /*  FRISS */
    { "EBCDIC-IT",                  RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-PT",                  RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-UK",                  RTL_TEXTENCODING_DONTKNOW },
    { "EBCDIC-US",                  RTL_TEXTENCODING_DONTKNOW },
    { "ECMA-CYRILLIC",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-111 */
    { "ES",                         RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-17 */
    { "ES2",                        RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-85 */
    { "GB_1988-80",                 RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-57 */
    { "GOST_19768-74",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-153 */
    { "GREEK-CCITT",                RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-150 */
    { "GREEK7",                     RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-88 */
    { "GREEK7-OLD",                 RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-18 */
    { "HP-ROMAN8",                  RTL_TEXTENCODING_DONTKNOW },    /* ROMAN8 R8 */
    { "IBM037",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-[US|CA|WT] */
    { "IBM038",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-INT CP038 */
    { "IBM1004",                    RTL_TEXTENCODING_DONTKNOW },    /* CP1004 OS2LATIN1 */
    { "IBM1026",                    RTL_TEXTENCODING_DONTKNOW },    /* CP1026 1026 */
    { "IBM1047",                    RTL_TEXTENCODING_DONTKNOW },    /* CP1047 1047 */
    { "IBM256",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-INT1 */
    { "IBM273",                     RTL_TEXTENCODING_DONTKNOW },    /* CP273 */
    { "IBM274",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-BE CP274 */
    { "IBM275",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-BR CP275 */
    { "IBM277",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-CP-[DK|NO] */
    { "IBM278",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-CP-[FISE]*/
    { "IBM280",                     RTL_TEXTENCODING_DONTKNOW },    /* CP280 EBCDIC-CP-IT*/
    { "IBM281",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-JP-E CP281 */
    { "IBM284",                     RTL_TEXTENCODING_DONTKNOW },    /* CP284 EBCDIC-CP-ES */
    { "IBM285",                     RTL_TEXTENCODING_DONTKNOW },    /* CP285 EBCDIC-CP-GB */
    { "IBM290",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-JP-KANA */
    { "IBM297",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-CP-FR */
    { "IBM420",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-CP-AR1 */
    { "IBM423",                     RTL_TEXTENCODING_DONTKNOW },    /* CP423 EBCDIC-CP-GR */
    { "IBM424",                     RTL_TEXTENCODING_DONTKNOW },    /* CP424 EBCDIC-CP-HE */
    { "IBM437",                     RTL_TEXTENCODING_IBM_437 },     /* CP437 437 */
    { "IBM500",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-CP-[BE|CH] */
    { "IBM850",                     RTL_TEXTENCODING_IBM_850 },     /* CP850 850 */
    { "IBM851",                     RTL_TEXTENCODING_DONTKNOW },    /* CP851 851 */
    { "IBM852",                     RTL_TEXTENCODING_IBM_852 },     /* CP852 852 */
    { "IBM855",                     RTL_TEXTENCODING_IBM_855 },     /* CP855 855 */
    { "IBM857",                     RTL_TEXTENCODING_IBM_857 },     /* CP857 857 */
    { "IBM860",                     RTL_TEXTENCODING_IBM_860 },     /* CP860 860 */
    { "IBM861",                     RTL_TEXTENCODING_IBM_861 },     /* CP861 861 CP-IS */
    { "IBM862",                     RTL_TEXTENCODING_IBM_862 },     /* CP862 862 */
    { "IBM863",                     RTL_TEXTENCODING_IBM_863 },     /* CP863 863 */
    { "IBM864",                     RTL_TEXTENCODING_IBM_864 },     /* CP864 */
    { "IBM865",                     RTL_TEXTENCODING_IBM_865 },     /* CP865 865 */
    { "IBM866",                     RTL_TEXTENCODING_IBM_866 },     /* CP866 866 */
    { "IBM868",                     RTL_TEXTENCODING_DONTKNOW },    /* CP868 CP-AR */
    { "IBM869",                     RTL_TEXTENCODING_IBM_869 },     /* CP869 869 CP-GR */
    { "IBM870",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-[ROECE|YU] */
    { "IBM871",                     RTL_TEXTENCODING_DONTKNOW },    /* CP871 EBCDIC-CP-IS */
    { "IBM875",                     RTL_TEXTENCODING_DONTKNOW },    /* CP875 EBCDIC-GREEK */
    { "IBM880",                     RTL_TEXTENCODING_DONTKNOW },    /* EBCDIC-CYRILLIC */
    { "IBM891",                     RTL_TEXTENCODING_DONTKNOW },    /* CP891 */
    { "IBM903",                     RTL_TEXTENCODING_DONTKNOW },    /* CP903 */
    { "IBM904",                     RTL_TEXTENCODING_DONTKNOW },    /* CP904 904 */
    { "IBM905",                     RTL_TEXTENCODING_DONTKNOW },    /* CP905 EBCDIC-CP-TR */
    { "IBM918",                     RTL_TEXTENCODING_DONTKNOW },    /* CP918 EBCDIC-AR2 */
    { "IEC_P27-1",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-143 */
    { "INIS",                       RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-49 */
    { "INIS-8",                     RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-50 */
    { "INIS-CYRILLIC",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-51 */
    { "INVARIANT",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-170 */
    { "ISO-8859-1",                 RTL_TEXTENCODING_ISO_8859_1 },  /* ISO-IR-100 CP819 */
    { "ISO-8859-10",                RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-157 LATIN6 */
    { "ISO-8859-13",                RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-179 LATIN7 */
    { "ISO-8859-14",                RTL_TEXTENCODING_ISO_8859_14 }, /* LATIN8 L8 */
    { "ISO-8859-15",                RTL_TEXTENCODING_ISO_8859_15 },
    { "ISO-8859-2",                 RTL_TEXTENCODING_ISO_8859_2 },  /* LATIN2 L2 */
    { "ISO-8859-3",                 RTL_TEXTENCODING_ISO_8859_3 },  /* LATIN3 L3 */
    { "ISO-8859-4",                 RTL_TEXTENCODING_ISO_8859_4 },  /* LATIN4 L4 */
    { "ISO-8859-5",                 RTL_TEXTENCODING_ISO_8859_5 },  /* CYRILLIC */
    { "ISO-8859-6",                 RTL_TEXTENCODING_ISO_8859_6 },  /* ECMA-114 ARABIC */
    { "ISO-8859-7",                 RTL_TEXTENCODING_ISO_8859_7 },  /* ECMA-118 GREEK8 */
    { "ISO-8859-8",                 RTL_TEXTENCODING_ISO_8859_8 },  /* ISO_8859-8 HEBREW */
    { "ISO-8859-9",                 RTL_TEXTENCODING_ISO_8859_9 },  /* ISO_8859-9 LATIN5 */
    { "ISO-IR-90",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO_6937-2:1983 */
    { "ISO_10367-BOX",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-155 */
    { "ISO_2033-1983",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-98 E13B */
    { "ISO_5427",                   RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-37 KOI-7 */
    { "ISO_5427-EXT",               RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-54  */
    { "ISO_5428",                   RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-55 */
    { "ISO_646.BASIC",              RTL_TEXTENCODING_ASCII_US },    /* REF */
    { "ISO_646.IRV",                RTL_TEXTENCODING_ASCII_US },    /* ISO-IR-2 IRV */
    { "ISO_646.IRV:1983",           RTL_TEXTENCODING_ISO_8859_1 },  /* fake: ASCII_US, used for "C" locale*/
    { "ISO_6937",                   RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-156 ISO6937*/
    { "ISO_6937-2-25",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-152 */
    { "ISO_6937-2-ADD",             RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-142 */
    { "ISO_8859-SUPP",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-154 */
    { "IT",                         RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-15  */
    { "JIS_C6220-1969-JP",          RTL_TEXTENCODING_DONTKNOW },    /* KATAKANA X0201-7 */
    { "JIS_C6220-1969-RO",          RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-14 */
    { "JIS_C6229-1984-A",           RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-91 */
    { "JIS_C6229-1984-B",           RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-92 */
    { "JIS_C6229-1984-B-ADD",       RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-93 */
    { "JIS_C6229-1984-HAND",        RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-94 */
    { "JIS_C6229-1984-HAND-ADD",    RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-95 */
    { "JIS_C6229-1984-KANA",        RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-96 */
    { "JIS_X0201",                  RTL_TEXTENCODING_DONTKNOW },    /* X0201 */
    { "JUS_I.B1.002",               RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-141 */
    { "JUS_I.B1.003-MAC",           RTL_TEXTENCODING_DONTKNOW },    /* MACEDONIAN */
    { "JUS_I.B1.003-SERB",          RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-146 SERBIAN */
    { "KOI-8",                      RTL_TEXTENCODING_DONTKNOW },
    { "KOI8-R",                     RTL_TEXTENCODING_KOI8_R },
    { "KOI8-U",                     RTL_TEXTENCODING_DONTKNOW },
    { "KSC5636",                    RTL_TEXTENCODING_DONTKNOW },    /* ISO646-KR */
    { "LATIN-GREEK",                RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-19 */
    { "LATIN-GREEK-1",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-27 */
    { "MAC-IS",                     RTL_TEXTENCODING_APPLE_ROMAN },
    { "MAC-UK",                     RTL_TEXTENCODING_APPLE_ROMAN },
    { "MACINTOSH",                  RTL_TEXTENCODING_APPLE_ROMAN }, /* MAC */
    { "MSZ_7795.3",                 RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-86 */
    { "NATS-DANO",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-9-1 */
    { "NATS-DANO-ADD",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-9-2 */
    { "NATS-SEFI",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-8-1 */
    { "NATS-SEFI-ADD",              RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-8-2 */
    { "NC_NC00-10",                 RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-151 */
    { "NEXTSTEP",                   RTL_TEXTENCODING_DONTKNOW },    /* NEXT */
    { "NF_Z_62-010",                RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-69 */
    { "NF_Z_62-010_(1973)",         RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-25 */
    { "NS_4551-1",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-60 */
    { "NS_4551-2",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-61 */
    { "PT",                         RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-16 */
    { "PT2",                        RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-84 */
    { "SAMI",                       RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-158 */
    { "SEN_850200_B",               RTL_TEXTENCODING_DONTKNOW },    /* ISO646-[FI|SE] */
    { "SEN_850200_C",               RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-11 */
    { "T.101-G2",                   RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-128 */
    { "T.61-7BIT",                  RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-102 */
    { "T.61-8BIT",                  RTL_TEXTENCODING_DONTKNOW },    /* T.61 ISO-IR-103 */
    { "UTF-8",                      RTL_TEXTENCODING_UTF8 },        /* ISO-10646/UTF-8 */
    { "VIDEOTEX-SUPPL",             RTL_TEXTENCODING_DONTKNOW },    /* ISO-IR-70 */
    { "WIN-SAMI-2",                 RTL_TEXTENCODING_DONTKNOW }     /* WS2 */
};

#endif /* ifdef LINUX */

static pthread_mutex_t aLocalMutex = PTHREAD_MUTEX_INITIALIZER;

rtl_TextEncoding osl_getTextEncodingFromLocale( rtl_Locale * pLocale )
{
    const _pair *language=0;
    
    char  locale_buf[64] = "";
    char  codeset_buf[64];

    char *ctype_locale = 0;
    char *codeset      = 0;


    /* remember the charset as indicated by the LC_CTYPE locale */
    ctype_locale = setlocale( LC_CTYPE, NULL );

    /* check if a valid locale is specified */
    if( pLocale && pLocale->Language && (pLocale->Language->length == 2) )
    {
        /* convert language code to ascii */
        {
            rtl_String *pLanguage = NULL;
            
            rtl_uString2String( &pLanguage, 
                pLocale->Language->buffer, pLocale->Language->length,
                RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
                
            strcpy( locale_buf, pLanguage->buffer );
            rtl_string_release( pLanguage );
        }
        
        /* convert country code to ascii */
        if( pLocale->Country && (pLocale->Country->length == 2) )
        {
            rtl_String *pCountry = NULL;
            
            rtl_uString2String( &pCountry, 
                pLocale->Country->buffer, pLocale->Country->length,
                RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
             
            strcat( locale_buf, "_" );   
            strcat( locale_buf, pCountry->buffer );
            rtl_string_release( pCountry );
        }
        
        /* convert variant to ascii */
        if( pLocale->Variant && pLocale->Variant->length &&
            ( pLocale->Variant->length < sizeof(locale_buf) - 7 ) )
        {
            rtl_String *pVariant = NULL;
            
            rtl_uString2String( &pVariant,
                pLocale->Variant->buffer, pLocale->Variant->length,
                RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
             
            strcat( locale_buf, "." );   
            strcat( locale_buf, pVariant->buffer );
            rtl_string_release( pVariant );
        }
    }
        
    /* basic thread safeness */
    pthread_mutex_lock(&aLocalMutex);
    
    /* set the desired LC_CTYPE locale */
    if( NULL == setlocale( LC_CTYPE, locale_buf ) )
    {
        pthread_mutex_unlock(&aLocalMutex);
        return RTL_TEXTENCODING_DONTKNOW;
    }
        
    /* get the charset as indicated by the LC_CTYPE locale */
    codeset = nl_langinfo( CODESET );

    if ( codeset != NULL )
    {
        /* get codeset into mt save memory */
        rtl_copyMemory( codeset_buf, codeset, sizeof(codeset_buf) );
        codeset = codeset_buf;
    }

    /* restore the original value of locale */
    if ( ctype_locale != NULL )
        setlocale( LC_CTYPE, ctype_locale );

    pthread_mutex_unlock(&aLocalMutex);
    
    /* search the codeset in our language list */
    if ( codeset != NULL )
    {
        const unsigned int members = sizeof(_nl_language_list) / sizeof(_pair);
        language = _pair_search (codeset, _nl_language_list, members);
    }

    /* a matching item in our list provides a mapping from codeset to
     * rtl-codeset */
    if ( language != NULL )
        return language->value;
   
    return RTL_TEXTENCODING_DONTKNOW;
}


#elif defined(MACOSX) /* ifdef LINUX || SOLARIS */

/*
 * FIXME: the MacOS X implemetation is missing
 */
 
rtl_TextEncoding osl_getTextEncodingFromLocale( rtl_Locale * pLocale )
{
    return RTL_TEXTENCODING_DONTKNOW;
}

#else /* ifdef LINUX || SOLARIS || MACOSX */

/*
 * This implementation of osl_getTextEncodingFromLocale maps 
 * from the ISO language codes.
 */
 
const _pair _locale_extension_list[] = {
    { "big5",         RTL_TEXTENCODING_BIG5        },
    { "euc",          RTL_TEXTENCODING_EUC_JP      },
    { "iso8859-1",    RTL_TEXTENCODING_ISO_8859_1  },
    { "iso8859-15",   RTL_TEXTENCODING_ISO_8859_15 },
    { "iso8859-2",    RTL_TEXTENCODING_ISO_8859_2  },
    { "iso8859-3",    RTL_TEXTENCODING_ISO_8859_3  },
    { "iso8859-4",    RTL_TEXTENCODING_ISO_8859_4  },
    { "iso8859-5",    RTL_TEXTENCODING_ISO_8859_5  },
    { "iso8859-6",    RTL_TEXTENCODING_ISO_8859_6  },
    { "iso8859-7",    RTL_TEXTENCODING_ISO_8859_7  },
    { "iso8859-8",    RTL_TEXTENCODING_ISO_8859_8  },
    { "iso8859-9",    RTL_TEXTENCODING_ISO_8859_9  },
    { "koi8-r",       RTL_TEXTENCODING_KOI8_R      },
    { "pck",          RTL_TEXTENCODING_MS_932      },
#if (0)
    { "sun_eu_greek", RTL_TEXTENCODING_DONTKNOW    },
#endif
    { "utf-7",        RTL_TEXTENCODING_UTF7        },
    { "utf-8",        RTL_TEXTENCODING_UTF8        },
    { "utf-16",       RTL_TEXTENCODING_UNICODE     },
    { NULL,           RTL_TEXTENCODING_DONTKNOW    }
};

const _pair _iso_language_list[] = {
    { "af",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "ar",  RTL_TEXTENCODING_ISO_8859_6 }, 
    { "az",  RTL_TEXTENCODING_ISO_8859_9 }, 
    { "be",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "bg",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "ca",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "cs",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "da",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "de",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "el",  RTL_TEXTENCODING_ISO_8859_7 }, 
    { "en",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "es",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "et",  RTL_TEXTENCODING_ISO_8859_4 }, 
    { "eu",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "fa",  RTL_TEXTENCODING_ISO_8859_6 }, 
    { "fi",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "fo",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "fr",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "hi",  RTL_TEXTENCODING_DONTKNOW }, 
    { "hi",  RTL_TEXTENCODING_DONTKNOW }, 
    { "hr",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "hu",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "hy",  RTL_TEXTENCODING_DONTKNOW }, 
    { "id",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "is",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "it",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "iw",  RTL_TEXTENCODING_ISO_8859_8 }, 
    { "ja",  RTL_TEXTENCODING_EUC_JP }, 
    { "ka",  RTL_TEXTENCODING_DONTKNOW }, 
    { "kk",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "ko",  RTL_TEXTENCODING_EUC_KR }, 
    { "lt",  RTL_TEXTENCODING_ISO_8859_4 }, 
    { "lv",  RTL_TEXTENCODING_ISO_8859_4 }, 
    { "mk",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "mr",  RTL_TEXTENCODING_DONTKNOW }, 
    { "ms",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "nl",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "no",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "pl",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "pt",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "ro",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "ru",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "sa",  RTL_TEXTENCODING_DONTKNOW }, 
    { "sk",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "sl",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "sq",  RTL_TEXTENCODING_ISO_8859_2 }, 
    { "sv",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "sw",  RTL_TEXTENCODING_ISO_8859_1 }, 
    { "ta",  RTL_TEXTENCODING_DONTKNOW }, 
    { "th",  RTL_TEXTENCODING_DONTKNOW }, 
    { "tr",  RTL_TEXTENCODING_ISO_8859_9 }, 
    { "tt",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "uk",  RTL_TEXTENCODING_ISO_8859_5 }, 
    { "ur",  RTL_TEXTENCODING_ISO_8859_6 }, 
    { "uz",  RTL_TEXTENCODING_ISO_8859_9 }, 
    { "vi",  RTL_TEXTENCODING_DONTKNOW }, 
    { "zh",  RTL_TEXTENCODING_BIG5 }, 
    { NULL,  RTL_TEXTENCODING_DONTKNOW    }
};

rtl_TextEncoding osl_getTextEncodingFromLocale( rtl_Locale * pLocale )
{
    const _pair *language = 0;
    char locale_buf[64] = "";
    char *cp;

    /* check if a valid locale is specified */
    if( pLocale && pLocale->Language && (pLocale->Language->length == 2) )
    {
        /* convert language code to ascii */
        {
            rtl_String *pLanguage = NULL;
            
            rtl_uString2String( &pLanguage, 
                pLocale->Language->buffer, pLocale->Language->length,
                RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
                
            strcpy( locale_buf, pLanguage->buffer );
            rtl_string_release( pLanguage );
        }
        
        /* convert country code to ascii */
        if( pLocale->Country && (pLocale->Country->length == 2) )
        {
            rtl_String *pCountry = NULL;
            
            rtl_uString2String( &pCountry, 
                pLocale->Country->buffer, pLocale->Country->length,
                RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
             
            strcat( locale_buf, "_" );   
            strcat( locale_buf, pCountry->buffer );
            rtl_string_release( pCountry );
        }
        
        /* convert variant to ascii */
        if( pLocale->Variant && pLocale->Variant->length &&
            ( pLocale->Variant->length < sizeof(locale_buf) - 7 ) )
        {
            rtl_String *pVariant = NULL;
            
            rtl_uString2String( &pVariant,
                pLocale->Variant->buffer, pLocale->Variant->length,
                RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
             
            strcat( locale_buf, "." );   
            strcat( locale_buf, pVariant->buffer );
            rtl_string_release( pVariant );
        }
    }
    
    /* simulate behavior off setlocale */
    else
    {
        char * locale = getenv( "LC_ALL" );
    
        if( NULL == locale )
            locale = getenv( "LC_CTYPE" );
        
        if( NULL == locale )
            locale = getenv( "LANG" );

        if( locale )
        {
            strncpy( locale_buf, locale, sizeof(locale_buf) - 1 );
            locale_buf[sizeof(locale_buf) - 1 ] = '\0';
        }
    }
    
    /*
     *  check if there is a charset qualifier at the end of the given locale string
     *  e.g. de.ISO8859-15 or de.ISO8859-15@euro which strongly indicates what
     *  charset to use
     */
    cp = strrchr( locale_buf, "." );
    
    if( NULL != cp )
    {
        const unsigned int members = sizeof( _locale_extension_list ) / sizeof( _pair );
        language = _pair_search( cp + 1, _locale_extension_list, members);
    }
    
    /* use iso language code to determine the charset */
    if( NULL == language )
    {
        const unsigned int members = sizeof( _iso_language_list ) / sizeof( _pair );
        
        /* iso lang codes have 2 charaters */
        locale_buf[2] = '\0';
        
        language = _pair_search( locale_buf, _iso_language_list, members);
    }
        
    /* a matching item in our list provides a mapping from codeset to
     * rtl-codeset */
    if ( language != NULL )
        return language->value;
   
    return RTL_TEXTENCODING_DONTKNOW;
}

#endif /* ifdef LINUX || SOLARIS */


