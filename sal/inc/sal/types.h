/*************************************************************************
 *
 *  $RCSfile: types.h,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: hr $ $Date: 2002-02-07 14:00:56 $
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

#ifndef _SAL_TYPES_H_
#define _SAL_TYPES_H_

#ifndef _SAL_CONFIG_H_
#include <sal/config.h>
#endif

#ifndef INCLUDED_STDLIB_H
#include <stdlib.h>
#define INCLUDED_STDLIB_H
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/
/* Data types
*/

/* Boolean */
typedef unsigned char sal_Bool;
#   define sal_False ((unsigned char)0)                
#   define sal_True  ((unsigned char)1) 

typedef signed char         sal_Int8;
typedef unsigned char       sal_uInt8;
typedef signed short        sal_Int16;
typedef unsigned short      sal_uInt16;
typedef signed long         sal_Int32;
typedef unsigned long       sal_uInt32;

#   if (_MSC_VER >= 1000)
typedef __int64             sal_Int64;
typedef unsigned __int64    sal_uInt64;
#   elif defined(__SUNPRO_CC) || defined(__SUNPRO_C) || defined (__GNUC__) || defined (__MWERKS__) || defined(__hpux) || defined (sgi)
typedef long long           sal_Int64;
typedef unsigned long long  sal_uInt64;
#endif

typedef char                sal_Char;
typedef signed char         sal_sChar;
typedef unsigned char       sal_uChar;
#if defined(SAL_W32)
typedef wchar_t             sal_Unicode;
#else
#define SAL_UNICODE_NOTEQUAL_WCHAR_T
typedef sal_uInt16          sal_Unicode;
#endif

typedef void*               sal_Handle;
typedef unsigned long       sal_Size;

#if defined(SAL_W32) || defined(SAL_OS2) || defined(SAL_UNX) || defined(SAL_MAC)
#define SAL_MAX_ENUM 0x7fffffff
#elif defined(SAL_W16)
#define SAL_MAX_ENUM 0x7fff
#endif

#if defined(_MSC_VER) || (defined(WNT) && defined(__GNUC__))
#   define SAL_DLLEXPORT    __declspec(dllexport)
#   define SAL_CALL         __cdecl
#   define SAL_CALL_ELLIPSE __cdecl
#elif defined SAL_OS2
#   define SAL_DLLEXPORT
#   define SAL_CALL
#   define SAL_CALL_ELLIPSE
#elif defined SAL_UNX
#   define SAL_DLLEXPORT
#   define SAL_CALL
#   define SAL_CALL_ELLIPSE
#elif defined SAL_MAC
#   define SAL_DLLEXPORT
#   define SAL_CALL
#   define SAL_CALL_ELLIPSE
#else
#   error("unknown platform")
#endif

/* Use this for pure virtual classes, e.g. class SAL_NO_VTABLE Foo { ...
   This hinders the compiler from setting a generic vtable stating that
   a pure virtual function was called and thus slightly reduces code size.
*/
#ifdef _MSC_VER
#define SAL_NO_VTABLE __declspec(novtable)
#else
#define SAL_NO_VTABLE
#endif

#ifdef SAL_W32
#pragma pack(push, 8)
#elif defined(SAL_OS2)
#pragma pack(8)
#endif

/** This is the binary specification of a SAL sequence.
*/
typedef struct _sal_Sequence
{
    /** reference count of sequence
    */
    sal_Int32			nRefCount;
    /** element count
    */
    sal_Int32			nElements;
    /** elements array
    */
    char				elements[1];
} sal_Sequence;
/** calculated header size excluding elements array
*/
#define SAL_SEQUENCE_HEADER_SIZE ((sal_Size)&((sal_Sequence *)0)->elements)

#ifdef SAL_W32
#pragma pack(pop)
#elif defined(SAL_OS2)
#pragma pack()
#endif


/* Wrap C++ const_cast, reinterpret_cast and static_cast expressions in
   macros to keep code portable to old compilers (since most compilers still
   lack RTTI support, dynamic_cast is not included here).
*/
#ifdef __cplusplus
#if defined SAL_W32 || defined SOLARIS || defined LINUX || defined MACOSX || defined FREEBSD || defined NETBSD
#define SAL_CONST_CAST(type, expr) (const_cast< type >(expr))
#define SAL_REINTERPRET_CAST(type, expr) (reinterpret_cast< type >(expr))
#define SAL_STATIC_CAST(type, expr) (static_cast< type >(expr))
#else /* SAL_W32, SOLARIS, LINUX */
#define SAL_CONST_CAST(type, expr) ((type) (expr))
#define SAL_REINTERPRET_CAST(type, expr) ((type) (expr))
#define SAL_STATIC_CAST(type, expr) ((type) (expr))
#endif /* SAL_W32, SOLARIS, LINUX */
#endif /* __cplusplus */

/* Definition of function throw clause macros.  These have been introduced
   to reduce code size by balancing out compiler bugs.
   
   These macros are ONLY for function declarations,
   use common C++ throw statement for throwing exceptions, e.g.
   throw RuntimeException();
   
   SAL_THROW()			should be used for all C++ functions, e.g. SAL_THROW( () )
   SAL_THROW_EXTERN_C() should be used for all C functions
*/
#ifdef __cplusplus
#if defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__sgi)
#define SAL_THROW( exc )
#else /* MSVC, all other */
#define SAL_THROW( exc ) throw exc
#endif /* __GNUC__, __SUNPRO_CC */
#define SAL_THROW_EXTERN_C() throw ()
#else /* ! __cplusplus */
/* SAL_THROW() must not be used in C headers, only SAL_THROW_EXTERN_C() is defined */
#define SAL_THROW_EXTERN_C()
#endif


#ifdef __cplusplus
/** definition of a no acquire enum for ctors
*/
enum __sal_NoAcquire
{
    /** definition of a no acquire enum for ctors
    */
    SAL_NO_ACQUIRE
};
#endif /* __cplusplus */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*_SAL_TYPES_H_ */

