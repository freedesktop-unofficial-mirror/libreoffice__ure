/*************************************************************************
 *
 *  $RCSfile: ustring.c,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: th $ $Date: 2001-05-22 10:21:55 $
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

#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif

#ifndef _RTL_STRIMP_H_
#include "strimp.h"
#endif
#ifndef _RTL_USTRING_H_
#include <rtl/ustring.h>
#endif

/* ======================================================================= */

/* static data to be referenced by all empty strings
 * the refCount is predefined to 1 and must never become 0 !
 */
static rtl_uString aImplEmpty_rtl_uString =
{
    1,      /* sal_Int32    refCount;   */
    0,      /* sal_Int32    length;     */
    { 0 }   /* sal_Unicode  buffer[1];  */
};

/* ======================================================================= */

/*************************************************************************
 *  rtl_ustr_ascii_shortenedCompare_WithLength
 */
sal_Int32 SAL_CALL rtl_ustr_ascii_shortenedCompare_WithLength( const sal_Unicode * first,
                                                            sal_Int32 firstLen,
                                                            const sal_Char * second,
                                                            sal_Int32 shortenedLength )
{
    const sal_Unicode * firstEnd = first + firstLen;
    sal_Int32 nResult = 0;
    while( shortenedLength--
        && first < firstEnd
        && *second // necessary if 0 is allowed in Unicode
        && (0 == (nResult = (sal_Int32)*first++ - (sal_Int32)*second++ ) ) )
    {
        /* Check ASCII range */
        OSL_ENSURE( (*(second-1) & 0x80) == 0, "Found ASCII char > 127");
    }
    if( !nResult && (shortenedLength != -1) )
    {
        if( *second )
        {
            OSL_ENSURE( first == firstEnd, "first == firstEnd failed" );
            // first is a substring of the second string => less (negative value)
            nResult = -1;
        }
        else
            // greater or equal
            nResult = firstEnd - first;
    }

    return nResult;
}

/*************************************************************************
 *  rtl_ustr_asciil_reverseCompare_WithLength
 */
sal_Int32 SAL_CALL rtl_ustr_asciil_reverseCompare_WithLength( const sal_Unicode * first, sal_Int32 firstLen,
                                                            const sal_Char * second, sal_Int32 secondLen )
{
    const sal_Unicode * firstRun = first + firstLen;
    const sal_Char * secondRun = second + secondLen;
    sal_Int32 nResult = 0;
    while( first < firstRun && second < secondRun
        && (0 == (nResult = (sal_Int32)*--firstRun - (sal_Int32)*--secondRun ) ) )
        ;
    if( nResult )
        return nResult;
    return firstLen - secondLen;
}

/* ======================================================================= */

#define IMPL_RTL_STRCODE            sal_Unicode
#define IMPL_RTL_USTRCODE( c )      (c)
#define IMPL_RTL_STRNAME( n )       rtl_ustr_ ## n

#define IMPL_RTL_STRINGNAME( n )    rtl_uString_ ## n
#define IMPL_RTL_STRINGDATA         rtl_uString
#define IMPL_RTL_EMPTYSTRING        aImplEmpty_rtl_uString

/* ======================================================================= */

/* Include String/UString template code */

#include "strtmpl.c"

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
    while( ((nRet = ((sal_Int32)(*pStr1))-
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

void SAL_CALL rtl_string2UString( rtl_uString** ppThis,
                                  const sal_Char* pStr,
                                  sal_Int32 nLen,
                                  rtl_TextEncoding eTextEncoding,
                                  sal_uInt32 nCvtFlags )
{
    OSL_ENSURE( (eTextEncoding != 9) &&
                (eTextEncoding != RTL_TEXTENCODING_DONTKNOW) &&
                (eTextEncoding != RTL_TEXTENCODING_UCS2) &&
                (eTextEncoding != RTL_TEXTENCODING_UCS4),
                "rtl_string2UString() - Wrong TextEncoding" );

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
            pBuffer = (*ppThis)->buffer;
            do
            {
                /* Check ASCII range */
                OSL_ENSURE( ((unsigned char)*pStr) <= 127,
                            "rtl_string2UString() - Found char > 127 and RTL_TEXTENCODING_ASCII_US is specified" );

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
                    pBuffer = (*ppThis)->buffer;
                    do
                    {
                        /* Check ASCII range */
                        OSL_ENSURE( ((unsigned char)*pStr) <= 127,
                                    "rtl_string2UString() - UTF8 test encoding is wrong" );

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
                nDestChars = rtl_convertTextToUnicode( hConverter, 0,
                                                       pStr, nLen,
                                                       pTemp->buffer, nNewLen,
                                                       nCvtFlags,
                                                       &nInfo, &nSrcBytes );
            }

            /* Set the buffer to the correct size or is there to
               much overhead, reallocate to the correct size */
            if ( nNewLen > nDestChars+8 )
            {
                rtl_uString* pTemp2 = IMPL_RTL_STRINGNAME( ImplAlloc )( nDestChars );
                rtl_str_ImplCopy( pTemp2->buffer, pTemp->buffer, nDestChars );
                rtl_freeMemory( pTemp );
                pTemp = pTemp2;
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
