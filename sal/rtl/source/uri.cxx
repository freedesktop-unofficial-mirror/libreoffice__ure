/*************************************************************************
 *
 *  $RCSfile: uri.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: sb $ $Date: 2002-10-08 07:49:12 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "rtl/uri.h"

#include "osl/diagnose.h"
#include "rtl/textenc.h"
#include "rtl/uri.h"
#include "rtl/ustrbuf.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#include <cstddef>

namespace {

std::size_t const nCharClassSize = 128;

sal_Unicode const cEscapePrefix = 0x25; // '%'

inline bool isDigit(sal_uInt32 nUtf32)
{
    return nUtf32 >= 0x30 && nUtf32 <= 0x39; // '0'--'9'
}

inline bool isAlpha(sal_uInt32 nUtf32)
{
    return nUtf32 >= 0x41 && nUtf32 <= 0x5A || nUtf32 >= 0x61 && nUtf32 <= 0x7A;
        // 'A'--'Z', 'a'--'z'
}

inline bool isHighSurrogate(sal_uInt32 nUtf16)
{
    return nUtf16 >= 0xD800 && nUtf16 <= 0xDBFF;
}

inline bool isLowSurrogate(sal_uInt32 nUtf16)
{
    return nUtf16 >= 0xDC00 && nUtf16 <= 0xDFFF;
}

inline int getHexWeight(sal_uInt32 nUtf32)
{
    return nUtf32 >= 0x30 && nUtf32 <= 0x39 ? // '0'--'9'
               static_cast< int >(nUtf32 - 0x30) :
           nUtf32 >= 0x41 && nUtf32 <= 0x46 ? // 'A'--'F'
               static_cast< int >(nUtf32 - 0x41 + 10) :
           nUtf32 >= 0x61 && nUtf32 <= 0x66 ? // 'a'--'f'
               static_cast< int >(nUtf32 - 0x61 + 10) :
               -1; // not a hex digit
}

inline bool isValid(sal_Bool const * pCharClass, sal_uInt32 nUtf32)
{
    return nUtf32 < nCharClassSize && pCharClass[nUtf32];
}

inline void writeUnicode(rtl_uString ** pBuffer, sal_Int32 * pCapacity,
                         sal_Unicode cChar)
{
    rtl_uStringbuffer_insert(pBuffer, pCapacity, (*pBuffer)->length, &cChar, 1);
}

enum EscapeType
{
    EscapeNo,
    EscapeChar,
    EscapeOctet
};

/* Read any of the following:

   - sequence of escape sequences representing character from eCharset,
     translated to single UCS4 character; or

   - pair of UTF-16 surrogates, translated to single UCS4 character; or

   _ single UTF-16 character, extended to UCS4 character.
 */
sal_uInt32 readUcs4(sal_Unicode const ** pBegin, sal_Unicode const * pEnd,
                    bool bEncoded, rtl_TextEncoding eCharset,
                    EscapeType * pType)
{
    sal_uInt32 nChar = *(*pBegin)++;
    int nWeight1;
    int nWeight2;
    if (nChar == cEscapePrefix && bEncoded && pEnd - *pBegin >= 2
        && (nWeight1 = getHexWeight((*pBegin)[0])) >= 0
        && (nWeight2 = getHexWeight((*pBegin)[1])) >= 0)
    {
        *pBegin += 2;
        nChar = static_cast< sal_uInt32 >(nWeight1 << 4 | nWeight2);
        switch (eCharset)
        {
        case RTL_TEXTENCODING_ASCII_US:
            *pType = nChar <= 0x7F ? EscapeChar : EscapeOctet;
            return nChar;

        case RTL_TEXTENCODING_ISO_8859_1:
            *pType = EscapeChar;
            return nChar;

        default:
            OSL_ENSURE(false, "unsupported eCharset"); // FIXME
        case RTL_TEXTENCODING_UTF8:
            if (nChar <= 0x7F)
                *pType = EscapeChar;
            else
            {
                if (nChar >= 0xC0 && nChar <= 0xFC)
                {
                    sal_uInt32 nEncoded;
                    int nShift;
                    sal_uInt32 nMin;
                    if (nChar <= 0xDF)
                    {
                        nEncoded = (nChar & 0x1F) << 6;
                        nShift = 0;
                        nMin = 0x80;
                    }
                    else if (nChar <= 0xEF)
                    {
                        nEncoded = (nChar & 0x0F) << 12;
                        nShift = 6;
                        nMin = 0x800;
                    }
                    else if (nChar <= 0xF7)
                    {
                        nEncoded = (nChar & 0x07) << 18;
                        nShift = 12;
                        nMin = 0x10000;
                    }
                    else if (nChar <= 0xFB)
                    {
                        nEncoded = (nChar & 0x03) << 24;
                        nShift = 18;
                        nMin = 0x200000;
                    }
                    else
                    {
                        nEncoded = 0;
                        nShift = 24;
                        nMin = 0x4000000;
                    }
                    sal_Unicode const * p = *pBegin;
                    bool bUTF8 = true;
                    for (; nShift >= 0; nShift -= 6)
                    {
                        if (pEnd - p < 3 || p[0] != cEscapePrefix
                            || (nWeight1 = getHexWeight(p[1])) < 8
                            || nWeight1 > 11
                            || (nWeight2 = getHexWeight(p[2])) < 0)
                        {
                            bUTF8 = sal_False;
                            break;
                        }
                        p += 3;
                        nEncoded |= ((nWeight1 & 3) << 4 | nWeight2) << nShift;
                    }
                    if (bUTF8 && nEncoded >= nMin && !isHighSurrogate(nEncoded)
                        && !isLowSurrogate(nEncoded) && nEncoded <= 0x10FFFF)
                    {
                        *pBegin = p;
                        *pType = EscapeChar;
                        return nEncoded;
                    }
                }
                *pType = EscapeOctet;
            }
            return nChar;
        }
    }
    else
    {
        *pType = EscapeNo;
        return isHighSurrogate(nChar) && *pBegin < pEnd
               && isLowSurrogate(**pBegin) ?
                   ((nChar & 0x3FF) << 10 | *(*pBegin)++ & 0x3FF) + 0x10000 :
                   nChar;
    }
}

void writeSurrogates(rtl_uString ** pBuffer, sal_Int32 * pCapacity,
                     sal_uInt32 nUtf32)
{
    OSL_ENSURE(nUtf32 > 0xFFFF && nUtf32 <= 0x10FFFF, "bad UTF-32 char");
    nUtf32 -= 0x10000;
    writeUnicode(pBuffer, pCapacity,
                 static_cast< sal_Unicode >(nUtf32 >> 10 | 0xD800));
    writeUnicode(pBuffer, pCapacity,
                 static_cast< sal_Unicode >(nUtf32 & 0x3FF | 0xDC00));
}

void writeEscapeOctet(rtl_uString ** pBuffer, sal_Int32 * pCapacity,
                      sal_uInt32 nOctet)
{
    OSL_ENSURE(nOctet <= 0xFF, "bad octet");

    static sal_Unicode const aHex[16]
        = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
            0x41, 0x42, 0x43, 0x44, 0x45, 0x46 }; /* '0'--'9', 'A'--'F' */

    writeUnicode(pBuffer, pCapacity, cEscapePrefix);
    writeUnicode(pBuffer, pCapacity, aHex[nOctet >> 4]);
    writeUnicode(pBuffer, pCapacity, aHex[nOctet & 15]);
}

void writeEscapeChar(rtl_uString ** pBuffer, sal_Int32 * pCapacity,
                     sal_uInt32 nUtf32, rtl_TextEncoding eCharset)
{
    switch (eCharset)
    {
    case RTL_TEXTENCODING_ASCII_US:
    case RTL_TEXTENCODING_ISO_8859_1:
        OSL_ENSURE(nUtf32 <= (eCharset == RTL_TEXTENCODING_ASCII_US ? 0x7FU :
                                                                     0xFFU),
                   "bad ASCII or ISO 8859-1 char");
        writeEscapeOctet(pBuffer, pCapacity, nUtf32);
        break;

    default:
        OSL_ENSURE(false, "unsupported eCharset"); // FIXME
    case RTL_TEXTENCODING_UTF8:
        OSL_ENSURE(nUtf32 <= 0x7FFFFFFF, "bad UCS-4 char");
        if (nUtf32 < 0x80)
            writeEscapeOctet(pBuffer, pCapacity, nUtf32);
        else if (nUtf32 < 0x800)
        {
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 6 | 0xC0);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 & 0x3F | 0x80);
        }
        else if (nUtf32 < 0x10000)
        {
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 12 | 0xE0);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 6 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 & 0x3F | 0x80);
        }
        else if (nUtf32 < 0x200000)
        {
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 18 | 0xF0);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 12 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 6 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 & 0x3F | 0x80);
        }
        else if (nUtf32 < 0x4000000)
        {
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 24 | 0xF8);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 18 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 12 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 6 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 & 0x3F | 0x80);
        }
        else
        {
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 30 | 0xFC);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 24 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 18 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 12 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 >> 6 & 0x3F | 0x80);
            writeEscapeOctet(pBuffer, pCapacity, nUtf32 & 0x3F | 0x80);
        }
        break;
    }
}

struct Component
{
    sal_Unicode const * pBegin;
    sal_Unicode const * pEnd;

    inline Component(): pBegin(0) {}

    inline bool isPresent() const { return pBegin != 0; }

    inline sal_Int32 getLength() const;
};

inline sal_Int32 Component::getLength() const
{
    OSL_ENSURE(isPresent(), "taking length of non-present component");
    return static_cast< sal_Int32 >(pEnd - pBegin);
}

struct Components
{
    Component aScheme;
    Component aAuthority;
    Component aPath;
    Component aQuery;
    Component aFragment;
};

void parseUriRef(rtl_uString const * pUriRef, Components * pComponents)
{
    // This algorithm is liberal and accepts various forms of illegal input.

    sal_Unicode const * pBegin = pUriRef->buffer;
    sal_Unicode const * pEnd = pBegin + pUriRef->length;
    sal_Unicode const * pPos = pBegin;

    if (pPos != pEnd && isAlpha(*pPos))
        for (sal_Unicode const * p = pPos + 1; p != pEnd; ++p)
            if (*p == ':')
            {
                pComponents->aScheme.pBegin = pBegin;
                pComponents->aScheme.pEnd = ++p;
                pPos = p;
                break;
            }
            else if (!isAlpha(*p) && !isDigit(*p) && *p != '+' && *p != '-'
                     && *p != '.')
                break;

    if (pEnd - pPos >= 2 && pPos[0] == '/' && pPos[1] == '/')
    {
        pComponents->aAuthority.pBegin = pPos;
        pPos += 2;
        while (pPos != pEnd && *pPos != '/' && *pPos != '?' && *pPos != '#')
            ++pPos;
        pComponents->aAuthority.pEnd = pPos;
    }

    pComponents->aPath.pBegin = pPos;
    while (pPos != pEnd && *pPos != '?' && * pPos != '#')
        ++pPos;
    pComponents->aPath.pEnd = pPos;

    if (pPos != pEnd && *pPos == '?')
    {
        pComponents->aQuery.pBegin = pPos++;
        while (pPos != pEnd && * pPos != '#')
            ++pPos;
        pComponents->aQuery.pEnd = pPos;
    }

    if (pPos != pEnd)
    {
        OSL_ASSERT(*pPos == '#');
        pComponents->aFragment.pBegin = pPos;
        pComponents->aFragment.pEnd = pEnd;
    }
}

rtl::OUString joinPaths(Component const & rBasePath, Component const & rRelPath)
{
    OSL_ASSERT(rBasePath.isPresent() && *rBasePath.pBegin == '/');
    OSL_ASSERT(rRelPath.isPresent());

    // The invariant of aBuffer is that it always starts and ends with a slash
    // (until probably right at the end of the algorithm, when the last segment
    // of rRelPath is added, which does not necessarily end in a slash):
    rtl::OUStringBuffer aBuffer(rBasePath.getLength() + rRelPath.getLength());
        // XXX  numeric overflow

    // Segments "." and ".." within rBasePath are not conisdered special,
    // RFC 2396 seems a bit unclear about this point:
    sal_Unicode const * p = rBasePath.pEnd;
    while (p[-1] != '/')
        --p;
    aBuffer.append(rBasePath.pBegin, p - rBasePath.pBegin);

    p = rRelPath.pBegin;
    if (p != rRelPath.pEnd)
        for (sal_Int32 nFixed = 1;;)
        {
            sal_Unicode const * q = p;
            sal_Unicode const * r;
            for (;;)
            {
                if (q == rRelPath.pEnd)
                {
                    r = q;
                    break;
                }
                if (*q == '/')
                {
                    r = q + 1;
                    break;
                }
                ++q;
            }
            if (q - p == 2 && p[0] == '.' && p[1] == '.')
            {
                // Erroneous excess segments ".." within rRelPath are left
                // intact, as the examples in RFC 2396, section C.2, suggest:
                sal_Int32 i = aBuffer.getLength() - 1;
                if (i < nFixed)
                {
                    aBuffer.append(p, r - p);
                    nFixed += 3;
                }
                else
                {
                    while (aBuffer.charAt(i - 1) != '/')
                        --i;
                    aBuffer.setLength(i);
                }
            }
            else if (q - p != 1 || *p != '.')
                aBuffer.append(p, r - p);
            if (q == rRelPath.pEnd)
                break;
            p = q + 1;
        }

    return aBuffer.makeStringAndClear();
}

}

sal_Bool const * rtl_getUriCharClass(rtl_UriCharClass eCharClass)
    SAL_THROW_EXTERN_C()
{
    static sal_Bool const aCharClass[][nCharClassSize]
    = {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* None */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* !"#$%&'()*+,-./*/
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0123456789:;<=>?*/
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*@ABCDEFGHIJKLMNO*/
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*PQRSTUVWXYZ[\]^_*/
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*`abcdefghijklmno*/
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* Uric */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, /*0123456789:;<=>?*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* UricNoSlash */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, /*0123456789:;<=>?*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* RelSegment */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, /*0123456789:;<=>?*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* RegName */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, /*0123456789:;<=>?*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* Userinfo */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, /*0123456789:;<=>?*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* Pchar */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, /*0123456789:;<=>?*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       },
       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* UnoParamValue */
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, /* !"#$%&'()*+,-./*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*0123456789:;<=>?*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*@ABCDEFGHIJKLMNO*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*PQRSTUVWXYZ[\]^_*/
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*`abcdefghijklmno*/
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  /*pqrstuvwxyz{|}~ */
       }};
    OSL_ENSURE(eCharClass < sizeof aCharClass / sizeof aCharClass[0],
               "bad eCharClass");
    return aCharClass[eCharClass];
}

void rtl_uriEncode(rtl_uString * pText, sal_Bool const * pCharClass,
                   rtl_UriEncodeMechanism eMechanism, rtl_TextEncoding eCharset,
                   rtl_uString ** pResult)
    SAL_THROW_EXTERN_C()
{
    OSL_ENSURE(!pCharClass[0x25], "bad pCharClass");
        // make sure the percent sign is encoded...

    sal_Unicode const * p = pText->buffer;
    sal_Unicode const * pEnd = p + pText->length;
    sal_Int32 nCapacity = 0;
    rtl_uString_new(pResult);
    while (p < pEnd)
    {
        EscapeType eType;
        sal_uInt32 nUtf32 = readUcs4(&p, pEnd,
                                     eMechanism != rtl_UriEncodeIgnoreEscapes,
                                     eCharset, &eType);
        switch (eType)
        {
        case EscapeNo:
            if (isValid(pCharClass, nUtf32)) // implies nUtf32 <= 0x7F
                writeUnicode(pResult, &nCapacity,
                             static_cast< sal_Unicode >(nUtf32));
            else
                writeEscapeChar(pResult, &nCapacity, nUtf32, eCharset);
            break;

        case EscapeChar:
            if (eMechanism == rtl_UriEncodeCheckEscapes
                && isValid(pCharClass, nUtf32)) // implies nUtf32 <= 0x7F
                writeUnicode(pResult, &nCapacity,
                             static_cast< sal_Unicode >(nUtf32));
            else
                writeEscapeChar(pResult, &nCapacity, nUtf32, eCharset);
            break;

        case EscapeOctet:
            writeEscapeOctet(pResult, &nCapacity, nUtf32);
            break;
        }
    }
}

void rtl_uriDecode(rtl_uString * pText, rtl_UriDecodeMechanism eMechanism,
                   rtl_TextEncoding eCharset, rtl_uString ** pResult)
    SAL_THROW_EXTERN_C()
{
    switch (eMechanism)
    {
    case rtl_UriDecodeNone:
        rtl_uString_assign(pResult, pText);
        break;

    case rtl_UriDecodeToIuri:
        eCharset = RTL_TEXTENCODING_UTF8;
    default: // rtl_UriDecodeWithCharset
        {
            sal_Unicode const * p = pText->buffer;
            sal_Unicode const * pEnd = p + pText->length;
            sal_Int32 nCapacity = 0;
            rtl_uString_new(pResult);
            while (p < pEnd)
            {
                EscapeType eType;
                sal_uInt32 nUtf32 = readUcs4(&p, pEnd, true, eCharset, &eType);
                switch (eType)
                {
                case EscapeChar:
                    if (nUtf32 <= 0x7F && eMechanism == rtl_UriDecodeToIuri)
                    {
                        writeEscapeOctet(pResult, &nCapacity, nUtf32);
                        break;
                    }
                case EscapeNo:
                    if (nUtf32 <= 0xFFFF)
                        writeUnicode(pResult, &nCapacity,
                                     static_cast< sal_Unicode >(nUtf32));
                    else if (nUtf32 <= 0x10FFFF)
                        writeSurrogates(pResult, &nCapacity, nUtf32);
                    else
                        writeEscapeChar(pResult, &nCapacity, nUtf32, eCharset);
                    break;

                case EscapeOctet:
                    writeEscapeOctet(pResult, &nCapacity, nUtf32);
                    break;
                }
            }
        }
        break;
    }
}

sal_Bool rtl_uriConvertRelToAbs(rtl_uString * pBaseUriRef,
                                rtl_uString * pRelUriRef,
                                rtl_uString ** pResult,
                                rtl_uString ** pException)
    SAL_THROW_EXTERN_C()
{
    // If pRelUriRef starts with a scheme component it is an absolute URI
    // reference, and we are done (i.e., this algorithm does not support
    // backwards-compatible relative URIs starting with a scheme component, see
    // RFC 2396, section 5.2, step 3):
    Components aRelComponents;
    parseUriRef(pRelUriRef, &aRelComponents);
    if (aRelComponents.aScheme.isPresent())
    {
        rtl_uString_assign(pResult, pRelUriRef);
        return true;
    }

    // Parse pBaseUriRef; if the scheme component is not present or not valid,
    // or the path component is not empty and starts with anything but a slash,
    // an exception is raised:
    Components aBaseComponents;
    parseUriRef(pBaseUriRef, &aBaseComponents);
    if (!aBaseComponents.aScheme.isPresent())
    {
        rtl::OUString aMessage(pBaseUriRef);
        aMessage += rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            " does not start with a scheme component"));
        rtl_uString_assign(pException,
                           const_cast< rtl::OUString & >(aMessage).pData);
        return false;
    }
    if (aBaseComponents.aPath.pBegin != aBaseComponents.aPath.pEnd
        && *aBaseComponents.aPath.pBegin != '/')
    {
        rtl::OUString aMessage(pBaseUriRef);
        aMessage += rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "path component does not start with slash"));
        rtl_uString_assign(pException, aMessage.pData);
        return false;
    }

    // Use the algorithm from RFC 2396, section 5.2, to turn the relative URI
    // into an absolute one (if the relative URI is a reference to the "current
    // document," the "current document" is here taken to be the base URI):
    rtl::OUStringBuffer aBuffer;
    aBuffer.append(aBaseComponents.aScheme.pBegin,
                   aBaseComponents.aScheme.getLength());
    if (aRelComponents.aAuthority.isPresent())
    {
        aBuffer.append(aRelComponents.aAuthority.pBegin,
                       aRelComponents.aAuthority.getLength());
        aBuffer.append(aRelComponents.aPath.pBegin,
                       aRelComponents.aPath.getLength());
        if (aRelComponents.aQuery.isPresent())
            aBuffer.append(aRelComponents.aQuery.pBegin,
                           aRelComponents.aQuery.getLength());
    }
    else
    {
        if (aBaseComponents.aAuthority.isPresent())
            aBuffer.append(aBaseComponents.aAuthority.pBegin,
                           aBaseComponents.aAuthority.getLength());
        if (aRelComponents.aPath.pBegin == aRelComponents.aPath.pEnd
            && !aRelComponents.aQuery.isPresent())
        {
            aBuffer.append(aBaseComponents.aPath.pBegin,
                           aBaseComponents.aPath.getLength());
            if (aBaseComponents.aQuery.isPresent())
                aBuffer.append(aBaseComponents.aQuery.pBegin,
                               aBaseComponents.aQuery.getLength());
        }
        else
        {
            if (*aRelComponents.aPath.pBegin == '/')
                aBuffer.append(aRelComponents.aPath.pBegin,
                               aRelComponents.aPath.getLength());
            else
                aBuffer.append(joinPaths(aBaseComponents.aPath,
                                         aRelComponents.aPath));
            if (aRelComponents.aQuery.isPresent())
                aBuffer.append(aRelComponents.aQuery.pBegin,
                               aRelComponents.aQuery.getLength());
        }
    }
    if (aRelComponents.aFragment.isPresent())
        aBuffer.append(aRelComponents.aFragment.pBegin,
                       aRelComponents.aFragment.getLength());
    rtl_uString_assign(pResult, aBuffer.makeStringAndClear().pData);
    return true;
}
