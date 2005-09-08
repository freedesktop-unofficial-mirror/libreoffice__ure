/*************************************************************************
#
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rtl_String_Utils.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 15:57:02 $
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
#*************************************************************************/

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifndef _RTL_STRING_UTILS_HXX_
#define _RTL_STRING_UTILS_HXX_

#ifdef __cplusplus

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifndef _SAL_TYPES_H_
    #include <sal/types.h>
#endif

#ifndef _RTL_USTRING_H_
    #include <rtl/ustring.h>
#endif

#ifndef _RTL_STRING_HXX_
    #include <rtl/string.hxx>
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
sal_Char* cpystr( sal_Char* dst, const sal_Char* src );
sal_Char* cpynstr( sal_Char* dst, const sal_Char* src, sal_uInt32 cnt );

sal_Bool cmpstr( const sal_Char* str1, const sal_Char* str2, sal_uInt32 len );
sal_Bool cmpustr( const sal_Unicode* str1, const sal_Unicode* str2, sal_uInt32 len );
sal_Bool cmpustr( const sal_Unicode* str1, const sal_Unicode* str2 );

sal_Char* createName( sal_Char* dst, const sal_Char* src, sal_uInt32 cnt );
void makeComment(char *com, const char *str1, const char *str2, sal_Int32 sgn);


sal_uInt32 AStringLen( const sal_Char *pAStr );

sal_uInt32 UStringLen( const sal_Unicode *pUStr );

//------------------------------------------------------------------------

sal_Bool AStringToFloatCompare ( const sal_Char  *pStr,
                                 const float      nX,
                                 const float      nEPS
                                );

sal_Bool AStringToDoubleCompare ( const sal_Char  *pStr,
                                  const double     nX,
                                  const double     nEPS
                                );

//------------------------------------------------------------------------

sal_Bool AStringIsValid( const sal_Char  *pAStr );

sal_Bool AStringNIsValid( const sal_Char   *pAStr,
                          const sal_uInt32  nStrLen
                        );

//------------------------------------------------------------------------

sal_Int32 AStringToUStringCompare( const sal_Unicode *pUStr,
                                   const sal_Char    *pAStr
                                 );

sal_Int32 AStringToUStringNCompare( const sal_Unicode  *pUStr,
                                    const sal_Char     *pAStr,
                                    const sal_uInt32    nAStrCount
                                   );

sal_Int32 AStringToRTLUStringCompare( const rtl_uString  *pRTLUStr,
                                      const sal_Char     *pAStr
                                    );

sal_Int32 AStringToRTLUStringNCompare( const rtl_uString  *pRTLUStr,
                                       const sal_Char     *pAStr,
                                       const sal_uInt32    nAStrCount
                                     );

//------------------------------------------------------------------------

sal_Bool AStringToUStringCopy( sal_Unicode     *pDest,
                               const sal_Char  *pSrc
                             );

sal_Bool AStringToUStringNCopy( sal_Unicode       *pDest,
                                const sal_Char    *pSrc,
                                const sal_uInt32   nSrcLen
                              );

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* _RTL_STRING_UTILS_HXX */







