/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: macro.hxx,v $
 * $Revision: 1.11 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _RTL_MACRO_HXX
#define _RTL_MACRO_HXX

#include <rtl/bootstrap.h>
#include <rtl/ustring.hxx>

#if defined WIN32
#define THIS_OS "Windows"
#elif defined OS2
#define THIS_OS "OS/2"
#elif defined SOLARIS
#define THIS_OS "Solaris"
#elif defined LINUX
#define THIS_OS "Linux"
#elif defined MACOSX
#define THIS_OS "MacOSX"
#elif defined NETBSD
#define THIS_OS "NetBSD"
#elif defined FREEBSD
#define THIS_OS "FreeBSD"
#elif defined IRIX
#define THIS_OS "Irix"
#endif

#if ! defined THIS_OS
#error "unknown OS -- insert your OS identifier above"
this is inserted for the case that the preprocessor ignores error
#endif

#if defined INTEL
#define THIS_ARCH "x86"
#elif defined POWERPC
#define THIS_ARCH "PowerPC"
#elif defined S390
#define THIS_ARCH "S390"
#elif defined SPARC
#define THIS_ARCH "SPARC"
#elif defined IRIX
#define THIS_ARCH "MIPS"
#elif defined X86_64
#define THIS_ARCH "X86_64"
#elif defined MIPS
#define THIS_ARCH "MIPS"
#elif defined ARM
#define THIS_ARCH "ARM"
#elif defined IA64
#define THIS_ARCH "IA64"
#endif

#if ! defined THIS_ARCH
#error "unknown ARCH -- insert your ARCH identifier above"
this is inserted for the case that the preprocessor ignores error
#endif

#endif





