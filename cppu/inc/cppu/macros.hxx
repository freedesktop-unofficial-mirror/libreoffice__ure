/*************************************************************************
 *
 *  $RCSfile: macros.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: dbo $ $Date: 2001-10-12 16:20:12 $
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
#ifndef _CPPU_MACROS_HXX_
#define _CPPU_MACROS_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _UNO_LBNAMES_H_
#include <uno/lbnames.h>
#endif

// patching the GNU3 incomatible alignment change
#define CPPU_GNU3_ALIGN( x )

// Microsoft Visual C++ 4.x, 5.0, 6.0
#if defined(_MSC_VER)
#	if ( _MSC_VER < 1000 )
#  		error "msc version must be between 4.2 and 6.x"
#   elif (_MSC_VER < 1100)  // MSVC 4.x
#  		pragma warning( disable: 4290 )
#		define	CPPU_CLBN_TMP	UNO_LB_MSCI
#		define	CPPU_CLBN_NS_TMP UNO_LB_MSCI_NAMESPACE
#		define  CPPU_DLL_POSTFIX "MSC"
#	elif(_MSC_VER < 1200)  // MSVC 5.0
#		define	CPPU_CLBN_TMP	UNO_LB_MSCI
#		define	CPPU_CLBN_NS_TMP UNO_LB_MSCI_NAMESPACE
#		define  CPPU_DLL_POSTFIX "MSC"
#	elif(_MSC_VER < 1300)  // MSVC 6.0
#		define	CPPU_CLBN_TMP	UNO_LB_MSCI
#		define	CPPU_CLBN_NS_TMP UNO_LB_MSCI_NAMESPACE
#		define  CPPU_DLL_POSTFIX "MSC"
#   elif(_MSC_VER < 1400)  // MSVC 7.0
#       define  CPPU_CLBN_TMP   UNO_LB_MSCI
#       define  CPPU_CLBN_NS_TMP UNO_LB_MSCI_NAMESPACE
#       define  CPPU_DLL_POSTFIX "MSC"
#	else
#     error "msc version must be between 4.2 and 7.x"
#	endif

// AIX xlC 3.1 , 3.0.1 ==0x301
// Visual Age C++ 3.x
#elif ( defined (__xlC__) && __xlC__ < 0x400 ) || \
    ( defined (  __IBMCPP__ ) && (  __IBMCPP__ < 400 ) )
#	error "visual age on aix not supported"

// Borland C++ ( 5.x )
#elif defined (BC50) 
#	error "borland compiler not supported"

#elif defined(__SUNPRO_CC)
#	if ( __SUNPRO_CC < 0x500 )
#		error "sunpro cc version must be 5.x"
#	elif( __SUNPRO_CC < 0x600 )
#		define	CPPU_CLBN_TMP	UNO_LB_SUNPRO5
#		define	CPPU_CLBN_NS_TMP UNO_LB_SUNPRO5_NAMESPACE
#		define  CPPU_DLL_POSTFIX "C50"
#  	else
#   	error "sunpro cc version must be 5.x"
#	endif

// g++ 2.7.x
#elif defined __GNUC__
// cygnus have a lot of version, let's assume the best.
// no specific definitions known except this one
#	if ( __GNUC__ == 2 && __GNUC_MINOR__ == 7 )
#   	error "gcc 2.7 compiler not supported"
#	elif ( __GNUC__ == 2 && __GNUC_MINOR__ == 91 )
#		define	CPPU_CLBN_TMP	UNO_LB_GCC2
#		define	CPPU_CLBN_NS_TMP UNO_LB_GCC2_NAMESPACE
#		define  CPPU_DLL_POSTFIX "GCC"
#	elif ( __GNUC__ == 2 && __GNUC_MINOR__ == 95 )
#		define	CPPU_CLBN_TMP	UNO_LB_GCC2
#		define	CPPU_CLBN_NS_TMP UNO_LB_GCC2_NAMESPACE
#		define  CPPU_DLL_POSTFIX "GCC"
#	elif ( __GNUC__ == 3 && __GNUC_MINOR__ == 0 )
#		define	CPPU_CLBN_TMP	UNO_LB_GCC3
#		define	CPPU_CLBN_NS_TMP UNO_LB_GCC3_NAMESPACE
#		define  CPPU_DLL_POSTFIX "GCC"
#undef CPPU_GNU3_ALIGN
#define CPPU_GNU3_ALIGN( x ) __attribute__ ((aligned (x)))
#  else
#		error "unknown gcc version"
#	endif

#elif defined (__WATCOM_CPLUSPLUS__)
#	error "watcom compiler not supported"

// Symantec 7.5
#elif defined (__SC__)
#	error "symantec compiler not supported"

// HP-UX und aCC
#elif defined(HPUX) && !defined(__GNUC__)
#	error "HP-UX compiler not supported"

// MAC Metrowerks
#elif defined (__MWERKS__)
#	error "Metroworks compiler not supported"
#else
#	error "unknown compiler"
#endif

/**
    The compiler and systems defines to identify compatibilities
    use to mark an unknown system, processor or compiler
*/
#define	CPPU_CURRENT_LANGUAGE_BINDING_NAME	CPPU_CLBN_TMP
#define	CPPU_CURRENT_NAMESPACE CPPU_CLBN_NS_TMP

#ifdef SAL_DLLPREFIX
#define CPPU_LIBRARY(name) SAL_DLLPREFIX name CPPU_DLL_POSTFIX SAL_DLLEXTENSION
#else
#define CPPU_LIBRARY(name) name CPPU_DLL_POSTFIX SAL_DLLEXTENSION
#endif

#endif	// _CPPU_MACROS_HXX_



