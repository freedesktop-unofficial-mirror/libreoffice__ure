/*************************************************************************
 *
 *  $RCSfile: interlck.c,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hjs $ $Date: 2003-08-18 15:18:36 $
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


#include "system.h"

#include <osl/interlck.h>
#include <osl/diagnose.h>

#if  ( defined ( SOLARIS ) || defined ( NETBSD ) ) && defined ( SPARC )
#error please use asm/interlck_sparc.s
#elif defined ( SOLARIS) && defined ( X86 )
#error please use asm/interlck_x86.s
#elif defined ( GCC ) && defined ( X86 )

/*****************************************************************************/
/* osl_incrementInterlockedCount */
/*****************************************************************************/
oslInterlockedCount SAL_CALL osl_incrementInterlockedCount(oslInterlockedCount* pCount)
{
    oslInterlockedCount nCount;

    __asm__ __volatile__ (
        "movl $1, %0\n\t"
        "lock\n\t" 
        "xadd %0, %2\n\t"
        "incl %0"
    :	"=&r" (nCount), "=m" (*pCount)
    :	"m" (*pCount)
    :	"memory");

    return nCount;
}

oslInterlockedCount SAL_CALL osl_decrementInterlockedCount(oslInterlockedCount* pCount)
{
    oslInterlockedCount nCount;

    __asm__ __volatile__ (
        "movl $-1, %0\n\t"
        "lock\n\t"
        "xadd %0, %2\n\t"
        "decl %0"
    :	"=&r" (nCount), "=m" (*pCount)
    :	"m" (*pCount)
    :	"memory");

    return nCount;
}

#elif defined ( GCC ) && defined ( POWERPC )

/*****************************************************************************/
/* osl_incrementInterlockedCount */
/*****************************************************************************/
oslInterlockedCount SAL_CALL osl_incrementInterlockedCount(oslInterlockedCount* pCount)
{
    /* "addi" doesn't work with r0 as second parameter */
    register oslInterlockedCount nCount __asm__ ("r4");

    __asm__ __volatile__ (
        "1: lwarx   %0,0,%2\n\t"
        "   addi    %0,%0,1\n\t"
        "   stwcx.  %0,0,%2\n\t"
        "   bne-    1b\n\t"
        "   isync"
        : "=&r" (nCount), "=m" (*pCount)
        : "r" (pCount)
        : "memory");

    return nCount;
}

oslInterlockedCount SAL_CALL osl_decrementInterlockedCount(oslInterlockedCount* pCount)
{
    /* "subi" doesn't work with r0 as second parameter */
    register oslInterlockedCount nCount __asm__ ("r4");

    __asm__ __volatile__ (
        "1: lwarx   %0,0,%2\n\t"
        "   subi    %0,%0,1\n\t"
        "   stwcx.  %0,0,%2\n\t"
        "   bne-    1b\n\t"
        "   isync"
        : "=&r" (nCount), "=m" (*pCount)
        : "r" (pCount)
        : "memory");

    return nCount;
}

#else
/* use only if nothing else works, expensive due to single mutex for all reference counts */

static pthread_mutex_t InterLock = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************/
/* osl_incrementInterlockedCount */
/*****************************************************************************/
oslInterlockedCount SAL_CALL osl_incrementInterlockedCount(oslInterlockedCount* pCount)
{
    oslInterlockedCount Count;

    pthread_mutex_lock(&InterLock);
    Count = ++(*pCount);
    pthread_mutex_unlock(&InterLock);

    return (Count);
}

/*****************************************************************************/
/* osl_decrementInterlockedCount */
/*****************************************************************************/
oslInterlockedCount SAL_CALL osl_decrementInterlockedCount(oslInterlockedCount* pCount)
{
    oslInterlockedCount Count;

    pthread_mutex_lock(&InterLock);
    Count = --(*pCount);
    pthread_mutex_unlock(&InterLock);

    return (Count);
}

#endif /* default */
