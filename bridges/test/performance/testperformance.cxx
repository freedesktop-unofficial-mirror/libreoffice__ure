/*#*************************************************************************
#
#   $RCSfile: testperformance.cxx,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: jbu $ $Date: 2001-01-03 14:06:22 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#************************************************************************* */

#include <stdio.h>
#include <math.h>

#include <osl/interlck.h>
#include <osl/mutex.hxx>
#include <osl/semaphor.h>

#include <rtl/string.hxx>
#include <rtl/byteseq.hxx>

#include <com/sun/star/uno/Sequence.hxx>

#ifdef SAL_W32
#include <windows.h>
#else
#include <sys/times.h>
#endif
#ifndef ULONG_MAX
#define ULONG_MAX 0xffffffff
#endif

using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star::uno;

static inline sal_uInt32 getSystemTicks()
{
#ifdef SAL_W32
    return (sal_uInt32)GetTickCount();
#else // only UNX supported for now
    static sal_uInt32	nImplTicksPerSecond = 0;
    static double		dImplTicksPerSecond;
    static double		dImplTicksULONGMAX;
    
    struct tms			aTms;
    sal_uInt32 nTicks = (sal_uInt32)times( &aTms );

    if ( !nImplTicksPerSecond )
    {
        nImplTicksPerSecond = CLK_TCK;
        dImplTicksPerSecond = nImplTicksPerSecond;
        dImplTicksULONGMAX	= (double)(sal_uInt32)ULONG_MAX;
    }
    
    double fTicks = nTicks;
    fTicks *= 1000;
    fTicks /= dImplTicksPerSecond;
    fTicks = fmod (fTicks, dImplTicksULONGMAX);

    return (sal_uInt32)fTicks;
#endif
}

class MyTimer
{
public:
    MyTimer(  const OString &descString ) :
        nStart( getSystemTicks() ),
        m_descString( descString )
        {
        }
    ~MyTimer( )
        {
            printf( "%f s : %s\n", (getSystemTicks() -nStart) / 1000., m_descString.getStr() );
        }
private:
    sal_uInt32 nStart;
    OString m_descString;
};

void main()
{
    // interlocked count
    {
        MyTimer timer( "performance - 1000*10000 interlocked count" );
        oslInterlockedCount count;
        for( int i = 0 ; i < 1000*10000 ; i ++  )
        {
            osl_incrementInterlockedCount( &count );
        }
    }
    {
        OString myDummyString( "blubber" );
        MyTimer timer( "performance - 1000*10000 acquiring/releasing a refcounted string(without destruction)" );
        for( int i = 0 ; i < 1000*10000 ; i ++  )
        {
            OString myNextDummyString = myDummyString ;
        }
    }

    printf( "--------------------\n" );
    {
        Mutex mutex;
        MyTimer timer( "performance - 1000*10000 acquiring/releasing an osl::Mutex" );
        for( int i = 0 ; i < 1000*10000 ; i ++  )
        {
            MutexGuard guard( mutex );
        }
    }

    {
        oslSemaphore sema = osl_createSemaphore(1);
        MyTimer timer( "performance - 1000*10000 acquiring/releasing an osl::Semaphore" );
        for( int i = 0 ; i < 1000*10000 ; i ++  )
        {
            osl_acquireSemaphore( sema );
            osl_releaseSemaphore( sema );
        }
    }

    printf( "--------------------\n" );
    {
        MyTimer timer( "performance - 1000*10000 rtl::ByteSequence(500)" );
        for( int i = 0 ; i < 1000*1000 ; i ++  )
        {
            ByteSequence seq(500);
        }
    }

    {
        MyTimer timer( "performance - 1000*1000 rtl::ByteSequence(500,BYTESEQ_NODEFAULT)" );
        for( int i = 0 ; i < 1000*1000 ; i ++  )
        {
            ByteSequence seq(500, BYTESEQ_NODEFAULT);
        }
    }
    {
        MyTimer timer( "performance - 1000*1000 com::sun::star::uno::Sequence< sal_Int8 > (500)" );
        for( int i = 0 ; i < 1000*1000 ; i ++  )
        {
            Sequence< sal_Int8> seq(500);
        }
    }
    {
        MyTimer timer( "performance - 1000*1000 rtl_freeMemory( rtl_allocateMemory( 512 ) )" );
        for( int i = 0 ; i < 1000*1000 ; i ++  )
        {
            rtl_freeMemory( rtl_allocateMemory( 512 ) );
        }
    }

    printf( "--------------------\n" );
    {
        MyTimer timer( "performance - 1000*1000 byte  string construction/destruction" );
        for( int i = 0 ; i < 1000*1000 ; i ++  )
        {
            OString textEnc( "this is a test string" );
        }
    }

    {
        MyTimer timer( "performance - 1000*1000 unicode string construction/destruction" );
        for( int i = 0 ; i < 1000*1000 ; i ++  )
        {
            OUString textEnc( RTL_CONSTASCII_USTRINGPARAM( "this is a test string" ) );
        }
    }
        
}
