/*************************************************************************
 *
 *  $RCSfile: unichars.c,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: sb $ $Date: 2001-10-17 14:35:30 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef INCLUDED_RTL_TEXTENC_UNICHARS_H
#include "unichars.h"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include "osl/diagnose.h"
#endif
#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

sal_Bool ImplIsNoncharacter(sal_uInt32 nUtf32)
{
    /* All code points that are noncharacters, as of Unicode 3.1.1. */
    return nUtf32 >= 0xFDD0 && nUtf32 <= 0xFDEF
           || (nUtf32 & 0xFFFF) >= 0xFFFE
           || nUtf32 > 0x10FFFF;
}

sal_Bool ImplIsControlOrFormat(sal_uInt32 nUtf32)
{
    /* All code points of <http://www.unicode.org/Public/UNIDATA/
       UnicodeData.txt>, Version 3.1.1, that have a General Category of Cc
       (Other, Control) or Cf (Other, Format).
     */
    return nUtf32 <= 0x001F
           || nUtf32 >= 0x007F && nUtf32 <= 0x009F
           || nUtf32 == 0x070F /* SYRIAC ABBREVIATION MARK */
           || nUtf32 == 0x180B /* MONGOLIAN FREE VARIATION SELECTOR ONE */
           || nUtf32 == 0x180C /* MONGOLIAN FREE VARIATION SELECTOR TWO */
           || nUtf32 == 0x180D /* MONGOLIAN FREE VARIATION SELECTOR THREE */
           || nUtf32 == 0x180E /* MONGOLIAN VOWEL SEPARATOR */
           || nUtf32 == 0x200C /* ZERO WIDTH NON-JOINER */
           || nUtf32 == 0x200D /* ZERO WIDTH JOINER */
           || nUtf32 == 0x200E /* LEFT-TO-RIGHT MARK */
           || nUtf32 == 0x200F /* RIGHT-TO-LEFT MARK */
           || nUtf32 == 0x202A /* LEFT-TO-RIGHT EMBEDDING */
           || nUtf32 == 0x202B /* RIGHT-TO-LEFT EMBEDDING */
           || nUtf32 == 0x202C /* POP DIRECTIONAL FORMATTING */
           || nUtf32 == 0x202D /* LEFT-TO-RIGHT OVERRIDE */
           || nUtf32 == 0x202E /* RIGHT-TO-LEFT OVERRIDE */
           || nUtf32 == 0x206A /* INHIBIT SYMMETRIC SWAPPING */
           || nUtf32 == 0x206B /* ACTIVATE SYMMETRIC SWAPPING */
           || nUtf32 == 0x206C /* INHIBIT ARABIC FORM SHAPING */
           || nUtf32 == 0x206D /* ACTIVATE ARABIC FORM SHAPING */
           || nUtf32 == 0x206E /* NATIONAL DIGIT SHAPES */
           || nUtf32 == 0x206F /* NOMINAL DIGIT SHAPES */
           || nUtf32 == 0xFEFF /* ZERO WIDTH NO-BREAK SPACE */
           || nUtf32 == 0xFFF9 /* INTERLINEAR ANNOTATION ANCHOR */
           || nUtf32 == 0xFFFA /* INTERLINEAR ANNOTATION SEPARATOR */
           || nUtf32 == 0xFFFB /* INTERLINEAR ANNOTATION TERMINATOR */
           || nUtf32 == 0x1D173 /* MUSICAL SYMBOL BEGIN BEAM */
           || nUtf32 == 0x1D174 /* MUSICAL SYMBOL END BEAM */
           || nUtf32 == 0x1D175 /* MUSICAL SYMBOL BEGIN TIE */
           || nUtf32 == 0x1D176 /* MUSICAL SYMBOL END TIE */
           || nUtf32 == 0x1D177 /* MUSICAL SYMBOL BEGIN SLUR */
           || nUtf32 == 0x1D178 /* MUSICAL SYMBOL END SLUR */
           || nUtf32 == 0x1D179 /* MUSICAL SYMBOL BEGIN PHRASE */
           || nUtf32 == 0x1D17A /* MUSICAL SYMBOL END PHRASE */
           || nUtf32 == 0xE0001 /* LANGUAGE TAG */
           || nUtf32 >= 0xE0020 && nUtf32 <= 0xE007F;
}

sal_Bool ImplIsHighSurrogate(sal_uInt32 nUtf32)
{
    /* All code points that are high-surrogates, as of Unicode 3.1.1. */
    return nUtf32 >= 0xD800 && nUtf32 <= 0xDBFF;
}

sal_Bool ImplIsLowSurrogate(sal_uInt32 nUtf32)
{
    /* All code points that are low-surrogates, as of Unicode 3.1.1. */
    return nUtf32 >= 0xDC00 && nUtf32 <= 0xDFFF;
}

sal_Bool ImplIsPrivateUse(sal_uInt32 nUtf32)
{
    /* All code points of <http://www.unicode.org/Public/UNIDATA/
       UnicodeData.txt>, Version 3.1.1, that have a General Category of Co
       (Other, Private Use).
     */
    return nUtf32 >= 0xE000 && nUtf32 <= 0xF8FF
           || nUtf32 >= 0xF0000 && nUtf32 <= 0xFFFFD
           || nUtf32 >= 0x100000 && nUtf32 <= 0x10FFFD;
}

sal_Bool ImplIsZeroWidth(sal_uInt32 nUtf32)
{
    /* All code points of <http://www.unicode.org/Public/UNIDATA/
       UnicodeData.txt>, Version 3.1.1, that have "ZERO WIDTH" in their
       Character name.
     */
    return nUtf32 == 0x200B /* ZERO WIDTH SPACE */
           || nUtf32 == 0x200C /* ZERO WIDTH NON-JOINER */
           || nUtf32 == 0x200D /* ZERO WIDTH JOINER */
           || nUtf32 == 0xFEFF; /* ZEOR WIDTH NO-BREAK SPACE */
}

sal_uInt32 ImplGetHighSurrogate(sal_uInt32 nUtf32)
{
    OSL_ENSURE(nUtf32 >= 0x10000, "specification violation");
    return ((nUtf32 - 0x10000) >> 10) | 0xD800;
}

sal_uInt32 ImplGetLowSurrogate(sal_uInt32 nUtf32)
{
    OSL_ENSURE(nUtf32 >= 0x10000, "specification violation");
    return ((nUtf32 - 0x10000) & 0x3FF) | 0xDC00;
}

sal_uInt32 ImplCombineSurrogates(sal_uInt32 nHigh, sal_uInt32 nLow)
{
    OSL_ENSURE(ImplIsHighSurrogate(nHigh) && ImplIsLowSurrogate(nLow),
               "specification violation");
    return (((nHigh & 0x3FF) << 10) | (nLow & 0x3FF)) + 0x10000;
}
