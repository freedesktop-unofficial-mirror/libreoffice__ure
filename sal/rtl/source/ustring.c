/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ustring.c,v $
 *
 *  $Revision: 1.26 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-03 14:06:19 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma warning(disable:4738) // storing 32-bit float result in memory, possible loss of performance
#endif

#ifndef _RTL_MEMORY_H_
#include <rtl/memory.h>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_INTERLOCK_H_
#include <osl/interlck.h>
#endif
#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#include <osl/mutex.h>
#include <osl/doublecheckedlocking.h>

#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif

#include <string.h>
#include <sal/alloca.h>

#include "hash.h"
#include "strimp.h"

#ifndef _RTL_USTRING_H_
#include <rtl/ustring.h>
#endif

#include "rtl/math.h"
#include "rtl/tencinfo.h"

/* ======================================================================= */

/* static data to be referenced by all empty strings
 * the refCount is predefined to 1 and must never become 0 !
 */
static rtl_uString const aImplEmpty_rtl_uString =
{
    SAL_STRING_STATIC_FLAG|1, /* sal_Int32    refCount;   */
    0,                        /* sal_Int32    length;     */
    { 0 }                     /* sal_Unicode  buffer[1];  */
};

/* ======================================================================= */

#define IMPL_RTL_STRCODE            sal_Unicode
#define IMPL_RTL_USTRCODE( c )      (c)
#define IMPL_RTL_STRNAME( n )       rtl_ustr_ ## n

#define IMPL_RTL_STRINGNAME( n )    rtl_uString_ ## n
#define IMPL_RTL_STRINGDATA         rtl_uString
#define IMPL_RTL_EMPTYSTRING        aImplEmpty_rtl_uString
#define IMPL_RTL_INTERN
static void internRelease (rtl_uString *pThis);

/* ======================================================================= */

/* Include String/UString template code */

#include "strtmpl.c"

sal_Int32 SAL_CALL rtl_ustr_valueOfFloat(sal_Unicode * pStr, float f)
{
    rtl_uString * pResult = NULL;
    sal_Int32 nLen;
    rtl_math_doubleToUString(
        &pResult, 0, 0, f, rtl_math_StringFormat_G,
        RTL_USTR_MAX_VALUEOFFLOAT - RTL_CONSTASCII_LENGTH("-x.E-xxx"), '.', 0,
        0, sal_True);
    nLen = pResult->length;
    OSL_ASSERT(nLen < RTL_USTR_MAX_VALUEOFFLOAT);
    rtl_copyMemory(pStr, pResult->buffer, (nLen + 1) * sizeof(sal_Unicode));
    rtl_uString_release(pResult);
    return nLen;
}

sal_Int32 SAL_CALL rtl_ustr_valueOfDouble(sal_Unicode * pStr, double d)
{
    rtl_uString * pResult = NULL;
    sal_Int32 nLen;
    rtl_math_doubleToUString(
        &pResult, 0, 0, d, rtl_math_StringFormat_G,
        RTL_USTR_MAX_VALUEOFDOUBLE - RTL_CONSTASCII_LENGTH("-x.E-xxx"), '.', 0,
        0, sal_True);
    nLen = pResult->length;
    OSL_ASSERT(nLen < RTL_USTR_MAX_VALUEOFDOUBLE);
    rtl_copyMemory(pStr, pResult->buffer, (nLen + 1) * sizeof(sal_Unicode));
    rtl_uString_release(pResult);
    return nLen;
}

float SAL_CALL rtl_ustr_toFloat(sal_Unicode const * pStr)
{
    return (float) rtl_math_uStringToDouble(pStr,
                                            pStr + rtl_ustr_getLength(pStr),
                                            '.', 0, 0, 0);
}

double SAL_CALL rtl_ustr_toDouble(sal_Unicode const * pStr)
{
    return rtl_math_uStringToDouble(pStr, pStr + rtl_ustr_getLength(pStr), '.',
                                    0, 0, 0);
}

/* ======================================================================= */

sal_Int32 SAL_CALL rtl_ustr_ascii_compare( const sal_Unicode* pStr1,
                                           const sal_Char* pStr2 )
{
    sal_Int32 nRet;
    while ( ((nRet = ((sal_Int32)(*pStr1))-
                     ((sal_Int32)((unsigned char)(*pStr2)))) == 0) &&
            *pStr2 )
    {
        pStr1++;
        pStr2++;
    }

    return nRet;
}

/* ----------------------------------------------------------------------- */

sal_Int32 SAL_CALL rtl_ustr_ascii_compare_WithLength( const sal_Unicode* pStr1,
                                                      sal_Int32 nStr1Len,
                                                      const sal_Char* pStr2 )
{
    sal_Int32 nRet = 0;
    while( ((nRet = (nStr1Len ? (sal_Int32)(*pStr1) : 0)-
                    ((sal_Int32)((unsigned char)(*pStr2)))) == 0) &&
           nStr1Len && *pStr2 )
    {
        pStr1++;
        pStr2++;
        nStr1Len--;
    }

    return nRet;
}

/* ----------------------------------------------------------------------- */

sal_Int32 SAL_CALL rtl_ustr_ascii_shortenedCompare_WithLength( const sal_Unicode* pStr1,
                                                               sal_Int32 nStr1Len,
                                                               const sal_Char* pStr2,
                                                               sal_Int32 nShortenedLength )
{
    const sal_Unicode*  pStr1End = pStr1 + nStr1Len;
    sal_Int32           nRet;
    while ( (nShortenedLength > 0) &&
            (pStr1 < pStr1End) && *pStr2 )
    {
        /* Check ASCII range */
        OSL_ENSURE( (*pStr2 & 0x80) == 0, "Found ASCII char > 127");

        nRet = ((sal_Int32)*pStr1)-
               ((sal_Int32)(unsigned char)*pStr2);
        if ( nRet != 0 )
            return nRet;

        nShortenedLength--;
        pStr1++;
        pStr2++;
    }

    if ( nShortenedLength <= 0 )
        return 0;

    if ( *pStr2 )
    {
        OSL_ENSURE( pStr1 == pStr1End, "pStr1 == pStr1End failed" );
        // first is a substring of the second string => less (negative value)
        nRet = -1;
    }
    else
    {
        // greater or equal
        nRet = pStr1End - pStr1;
    }

    return nRet;
}

/* ----------------------------------------------------------------------- */

sal_Int32 SAL_CALL rtl_ustr_asciil_reverseCompare_WithLength( const sal_Unicode* pStr1,
                                                              sal_Int32 nStr1Len,
                                                              const sal_Char* pStr2,
                                                              sal_Int32 nStr2Len )
{
    const sal_Unicode*  pStr1Run = pStr1+nStr1Len;
    const sal_Char*     pStr2Run = pStr2+nStr2Len;
    sal_Int32           nRet;
    while ( (pStr1 < pStr1Run) && (pStr2 < pStr2Run) )
    {
        pStr1Run--;
        pStr2Run--;
        nRet = ((sal_Int32)*pStr1Run)-((sal_Int32)*pStr2Run);
        if ( nRet )
            return nRet;
    }

    return nStr1Len - nStr2Len;
}

/* ----------------------------------------------------------------------- */

sal_Bool SAL_CALL rtl_ustr_asciil_reverseEquals_WithLength( const sal_Unicode* pStr1,
                                                              const sal_Char* pStr2,
                                                              sal_Int32 nStrLen )
{
    const sal_Unicode*  pStr1Run = pStr1+nStrLen;
    const sal_Char*     pStr2Run = pStr2+nStrLen;
    while ( pStr1 < pStr1Run )
    {
        pStr1Run--;
        pStr2Run--;
        if( *pStr1Run != (sal_Unicode)*pStr2Run )
            return sal_False;
    }

    return sal_True;
}

/* ----------------------------------------------------------------------- */

sal_Int32 SAL_CALL rtl_ustr_ascii_compareIgnoreAsciiCase( const sal_Unicode* pStr1,
                                                          const sal_Char* pStr2 )
{
    sal_Int32   nRet;
    sal_Int32   c1;
    sal_Int32   c2;
    do
    {
        /* If character between 'A' and 'Z', than convert it to lowercase */
        c1 = (sal_Int32)*pStr1;
        c2 = (sal_Int32)((unsigned char)*pStr2);
        if ( (c1 >= 65) && (c1 <= 90) )
            c1 += 32;
        if ( (c2 >= 65) && (c2 <= 90) )
            c2 += 32;
        nRet = c1-c2;
        if ( nRet != 0 )
            return nRet;

        pStr1++;
        pStr2++;
    }
    while ( c2 );

    return 0;
}

/* ----------------------------------------------------------------------- */

sal_Int32 SAL_CALL rtl_ustr_ascii_compareIgnoreAsciiCase_WithLength( const sal_Unicode* pStr1,
                                                                     sal_Int32 nStr1Len,
                                                                     const sal_Char* pStr2 )
{
    sal_Int32   nRet;
    sal_Int32   c1;
    sal_Int32   c2;
    do
    {
        if ( !nStr1Len )
            return *pStr2 == '\0' ? 0 : -1;

        /* If character between 'A' and 'Z', than convert it to lowercase */
        c1 = (sal_Int32)*pStr1;
        c2 = (sal_Int32)((unsigned char)*pStr2);
        if ( (c1 >= 65) && (c1 <= 90) )
            c1 += 32;
        if ( (c2 >= 65) && (c2 <= 90) )
            c2 += 32;
        nRet = c1-c2;
        if ( nRet != 0 )
            return nRet;

        pStr1++;
        pStr2++;
        nStr1Len--;
    }
    while( c2 );

    return 0;
}

sal_Int32 rtl_ustr_ascii_compareIgnoreAsciiCase_WithLengths(
    sal_Unicode const * first, sal_Int32 firstLen,
    char const * second, sal_Int32 secondLen)
{
    sal_Int32 i;
    sal_Int32 len = firstLen < secondLen ? firstLen : secondLen;
    for (i = 0; i < len; ++i) {
        sal_Int32 c1 = *first++;
        sal_Int32 c2 = (unsigned char) *second++;
        sal_Int32 d;
        if (c1 >= 65 && c1 <= 90) {
            c1 += 32;
        }
        if (c2 >= 65 && c2 <= 90) {
            c2 += 32;
        }
        d = c1 - c2;
        if (d != 0) {
            return d;
        }
    }
    return firstLen - secondLen;
}

/* ----------------------------------------------------------------------- */

sal_Int32 SAL_CALL rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( const sal_Unicode* pStr1,
                                                                              sal_Int32 nStr1Len,
                                                                              const sal_Char* pStr2,
                                                                              sal_Int32 nShortenedLength )
{
    const sal_Unicode*  pStr1End = pStr1 + nStr1Len;
    sal_Int32           nRet;
    sal_Int32           c1;
    sal_Int32           c2;
    while ( (nShortenedLength > 0) &&
            (pStr1 < pStr1End) && *pStr2 )
    {
        /* Check ASCII range */
        OSL_ENSURE( (*pStr2 & 0x80) == 0, "Found ASCII char > 127");

        /* If character between 'A' and 'Z', than convert it to lowercase */
        c1 = (sal_Int32)*pStr1;
        c2 = (sal_Int32)((unsigned char)*pStr2);
        if ( (c1 >= 65) && (c1 <= 90) )
            c1 += 32;
        if ( (c2 >= 65) && (c2 <= 90) )
            c2 += 32;
        nRet = c1-c2;
        if ( nRet != 0 )
            return nRet;

        nShortenedLength--;
        pStr1++;
        pStr2++;
    }

    if ( nShortenedLength <= 0 )
        return 0;

    if ( *pStr2 )
    {
        OSL_ENSURE( pStr1 == pStr1End, "pStr1 == pStr1End failed" );
        // first is a substring of the second string => less (negative value)
        nRet = -1;
    }
    else
    {
        // greater or equal
        nRet = pStr1End - pStr1;
    }

    return nRet;
}

/* ----------------------------------------------------------------------- */

void SAL_CALL rtl_uString_newFromAscii( rtl_uString** ppThis,
                                        const sal_Char* pCharStr )
{
    sal_Int32 nLen;

    if ( pCharStr )
    {
        const sal_Char* pTempStr = pCharStr;
        while( *pTempStr )
            pTempStr++;
        nLen = pTempStr-pCharStr;
    }
    else
        nLen = 0;

    if ( !nLen )
    {
        IMPL_RTL_STRINGNAME( new )( ppThis );
        return;
    }

    if ( *ppThis )
        IMPL_RTL_STRINGNAME( release )( *ppThis );

    *ppThis = IMPL_RTL_STRINGNAME( ImplAlloc )( nLen );
    OSL_ASSERT(*ppThis != NULL);
    if ( (*ppThis) )
    {
        IMPL_RTL_STRCODE* pBuffer = (*ppThis)->buffer;
        do
        {
            /* Check ASCII range */
            OSL_ENSURE( ((unsigned char)*pCharStr) <= 127,
                        "rtl_uString_newFromAscii() - Found ASCII char > 127" );

            *pBuffer = *pCharStr;
            pBuffer++;
            pCharStr++;
        }
        while ( *pCharStr );
    }
}

/* ======================================================================= */

static int rtl_ImplGetFastUTF8UnicodeLen( const sal_Char* pStr, sal_Int32 nLen )
{
    int             n;
    sal_uChar       c;
    const sal_Char* pEndStr;

    n = 0;
    pEndStr  = pStr+nLen;
    while ( pStr < pEndStr )
    {
        c = (sal_uChar)*pStr;

        if ( !(c & 0x80) )
            pStr++;
        else if ( (c & 0xE0) == 0xC0 )
            pStr += 2;
        else if ( (c & 0xF0) == 0xE0 )
            pStr += 3;
        else if ( (c & 0xF8) == 0xF0 )
            pStr += 4;
        else if ( (c & 0xFC) == 0xF8 )
            pStr += 5;
        else if ( (c & 0xFE) == 0xFC )
            pStr += 6;
        else
            pStr++;

        n++;
    }

    return n;
}

/* ----------------------------------------------------------------------- */

static void rtl_string2UString_status( rtl_uString** ppThis,
                                       const sal_Char* pStr,
                                       sal_Int32 nLen,
                                       rtl_TextEncoding eTextEncoding,
                                       sal_uInt32 nCvtFlags,
                                       sal_uInt32 *pInfo )
{
    OSL_ENSURE(rtl_isOctetTextEncoding(eTextEncoding),
               "rtl_string2UString_status() - Wrong TextEncoding" );

    if ( !nLen )
        rtl_uString_new( ppThis );
    else
    {
        if ( *ppThis )
            IMPL_RTL_STRINGNAME( release )( *ppThis );

        /* Optimization for US-ASCII */
        if ( eTextEncoding == RTL_TEXTENCODING_ASCII_US )
        {
            IMPL_RTL_STRCODE* pBuffer;
            *ppThis = IMPL_RTL_STRINGNAME( ImplAlloc )( nLen );
            if (*ppThis == NULL) {
                return;
            }
            pBuffer = (*ppThis)->buffer;
            do
            {
                /* Check ASCII range */
                OSL_ENSURE( ((unsigned char)*pStr) <= 127,
                            "rtl_string2UString_status() - Found char > 127 and RTL_TEXTENCODING_ASCII_US is specified" );

                *pBuffer = *pStr;
                pBuffer++;
                pStr++;
                nLen--;
            }
            while ( nLen );
        }
        else
        {
            rtl_uString*                pTemp;
            rtl_TextToUnicodeConverter  hConverter;
            sal_uInt32                  nInfo;
            sal_Size                    nSrcBytes;
            sal_Size                    nDestChars;
            sal_Size                    nNewLen;

            /* Optimization for UTF-8 - we try to calculate the exact length */
            /* For all other encoding we try the maximum - and reallocate
               the buffer if needed */
            if ( eTextEncoding == RTL_TEXTENCODING_UTF8 )
            {
                nNewLen = rtl_ImplGetFastUTF8UnicodeLen( pStr, nLen );
                /* Includes the string only ASCII, then we could copy
                   the buffer faster */
                if ( nNewLen == (sal_Size)nLen )
                {
                    IMPL_RTL_STRCODE* pBuffer;
                    *ppThis = IMPL_RTL_STRINGNAME( ImplAlloc )( nLen );
                    if (*ppThis == NULL)
                    {
                        return;
                    }
                    pBuffer = (*ppThis)->buffer;
                    do
                    {
                        /* Check ASCII range */
                        OSL_ENSURE( ((unsigned char)*pStr) <= 127,
                                    "rtl_string2UString_status() - UTF8 test encoding is wrong" );

                        *pBuffer = *pStr;
                        pBuffer++;
                        pStr++;
                        nLen--;
                    }
                    while ( nLen );
                    return;
                }
            }
            else
                nNewLen = nLen;

            nCvtFlags |= RTL_TEXTTOUNICODE_FLAGS_FLUSH;
            hConverter = rtl_createTextToUnicodeConverter( eTextEncoding );

            pTemp = IMPL_RTL_STRINGNAME( ImplAlloc )( nNewLen );
            if (pTemp == NULL) {
                return;
            }
            nDestChars = rtl_convertTextToUnicode( hConverter, 0,
                                                   pStr, nLen,
                                                   pTemp->buffer, nNewLen,
                                                   nCvtFlags,
                                                   &nInfo, &nSrcBytes );

            /* Buffer not big enough, try again with enough space */
            /* Shouldn't be the case, but if we get textencoding which
               could results in more unicode characters we have this
               code here. Could be the case for apple encodings */
            while ( nInfo & RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL )
            {
                rtl_freeMemory( pTemp );
                nNewLen += 8;
                pTemp = IMPL_RTL_STRINGNAME( ImplAlloc )( nNewLen );
                if (pTemp == NULL) {
                    return;
                }
                nDestChars = rtl_convertTextToUnicode( hConverter, 0,
                                                       pStr, nLen,
                                                       pTemp->buffer, nNewLen,
                                                       nCvtFlags,
                                                       &nInfo, &nSrcBytes );
            }

            if (pInfo)
                *pInfo = nInfo;

            /* Set the buffer to the correct size or if there is too
               much overhead, reallocate to the correct size */
            if ( nNewLen > nDestChars+8 )
            {
                rtl_uString* pTemp2 = IMPL_RTL_STRINGNAME( ImplAlloc )( nDestChars );
                if (pTemp2 != NULL)
                {
                    rtl_str_ImplCopy(pTemp2->buffer, pTemp->buffer, nDestChars);
                    rtl_freeMemory(pTemp);
                    pTemp = pTemp2;
                }
                else
                {
                    rtl_freeMemory(pTemp);
                    return;
                }
            }
            else
            {
                pTemp->length = nDestChars;
                pTemp->buffer[nDestChars] = 0;
            }

            rtl_destroyTextToUnicodeConverter( hConverter );
            *ppThis = pTemp;

            /* Results the conversion in an empty buffer -
               create an empty string */
            if ( pTemp && !nDestChars )
                rtl_uString_new( ppThis );
        }
    }
}

void SAL_CALL rtl_string2UString( rtl_uString** ppThis,
                                  const sal_Char* pStr,
                                  sal_Int32 nLen,
                                  rtl_TextEncoding eTextEncoding,
                                  sal_uInt32 nCvtFlags )
{
    rtl_string2UString_status( ppThis, pStr, nLen, eTextEncoding,
                               nCvtFlags, NULL );
}

/* ----------------------------------------------------------------------- */

typedef enum {
    CANNOT_RETURN,
    CAN_RETURN = 1
} StrLifecycle;

static oslMutex
getInternMutex()
{
    static oslMutex pPoolGuard = NULL;
    if( !pPoolGuard )
    {
        oslMutex pGlobalGuard;
        pGlobalGuard = *osl_getGlobalMutex();
        osl_acquireMutex( pGlobalGuard );
        if( !pPoolGuard )
        {
            oslMutex p = osl_createMutex();
            OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
            pPoolGuard = p;
        }
        osl_releaseMutex( pGlobalGuard );
    }
    else
    {
        OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
    }

    return pPoolGuard;
}

static StringHashTable *pInternPool = NULL;

/* returns true if we found a dup in the pool */
static void rtl_ustring_intern_internal( rtl_uString ** newStr,
                                         rtl_uString  * str,
                                         StrLifecycle   can_return )
{
    oslMutex pPoolMutex;

    pPoolMutex = getInternMutex();

    osl_acquireMutex( pPoolMutex );

    if (!pInternPool)
        pInternPool = rtl_str_hash_new (1024);
    *newStr = rtl_str_hash_intern (pInternPool, str, can_return);

    osl_releaseMutex( pPoolMutex );

    if( can_return && *newStr != str )
    { /* we dupped, then found a match */
        rtl_freeMemory( str );
    }
}

void SAL_CALL rtl_uString_intern( rtl_uString ** newStr,
                                  rtl_uString  * str)
{
    if (SAL_STRING_IS_INTERN(str))
    {
        IMPL_RTL_AQUIRE( str );
        *newStr = str;
    }
    else
    {
        if (*newStr)
        {
            rtl_uString_release (*newStr);
            *newStr = NULL;
        }
        rtl_ustring_intern_internal( newStr, str, CANNOT_RETURN );
    }
}

void SAL_CALL rtl_uString_internConvert( rtl_uString   ** newStr,
                                         const sal_Char * str,
                                         sal_Int32        len,
                                         rtl_TextEncoding eTextEncoding,
                                         sal_uInt32       convertFlags,
                                         sal_uInt32     * pInfo )
{
    rtl_uString *scratch;

    if (*newStr)
    {
        rtl_uString_release (*newStr);
        *newStr = NULL;
    }

    if ( len < 256 )
    { // try various optimisations
        if ( len < 0 )
            len = strlen( str );
        if ( eTextEncoding == RTL_TEXTENCODING_ASCII_US )
        {
            int i;
            rtl_uString *pScratch;
            pScratch = alloca( sizeof( rtl_uString )
                               + len * sizeof (IMPL_RTL_STRCODE ) );
            for (i = 0; i < len; i++)
            {
                /* Check ASCII range */
                OSL_ENSURE( ((unsigned char)str[i]) <= 127,
                            "rtl_ustring_internConvert() - Found char > 127 and RTL_TEXTENCODING_ASCII_US is specified" );
                pScratch->buffer[i] = str[i];
            }
            pScratch->length = len;
            rtl_ustring_intern_internal( newStr, pScratch, CANNOT_RETURN );
            return;
        }
        /* FIXME: we want a nice UTF-8 / alloca shortcut here */
    }

    scratch = NULL;
    rtl_string2UString_status( &scratch, str, len, eTextEncoding, convertFlags,
                               pInfo );
    if (!scratch) {
        return;
    }
    rtl_ustring_intern_internal( newStr, scratch, CAN_RETURN );
}

static void
internRelease (rtl_uString *pThis)
{
    oslMutex pPoolMutex;

    rtl_uString *pFree = NULL;
    if ( SAL_STRING_REFCOUNT(
             osl_decrementInterlockedCount( &(pThis->refCount) ) ) == 0)
    {
        pPoolMutex = getInternMutex();
        osl_acquireMutex( pPoolMutex );

        rtl_str_hash_remove (pInternPool, pThis);

        /* May have been separately acquired */
        if ( SAL_STRING_REFCOUNT(
                 osl_incrementInterlockedCount( &(pThis->refCount) ) ) == 1 )
        {
            /* we got the last ref */
            pFree = pThis;
        }
        else /* very unusual */
        {
            internRelease (pThis);
        }

        osl_releaseMutex( pPoolMutex );
    }
    if (pFree)
        rtl_freeMemory (pFree);
}
