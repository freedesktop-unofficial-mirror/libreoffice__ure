/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: threadident.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 00:19:56 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_cppu.hxx"
#include <stdio.h>

#include <list>

#include <osl/mutex.hxx>
#include <osl/thread.h>
#include <osl/diagnose.h>

#include <rtl/process.h>
#include <rtl/byteseq.hxx>

#include <uno/threadpool.h>

#include "current.hxx"


using namespace ::std;
using namespace ::osl;
using namespace ::rtl;
using namespace ::cppu;


static inline void createLocalId( sal_Sequence **ppThreadId )
{
    rtl_byte_sequence_constructNoDefault( ppThreadId , 4 + 16 );
    *((sal_Int32*) ((*ppThreadId)->elements)) = osl_getThreadIdentifier(0);

    rtl_getGlobalProcessId( (sal_uInt8 * ) &( (*ppThreadId)->elements[4]) );
}


extern "C" void SAL_CALL
uno_getIdOfCurrentThread( sal_Sequence **ppThreadId )
    SAL_THROW_EXTERN_C()
{
    IdContainer * p = getIdContainer();
    if( ! p->bInit )
    {
        // first time, that the thread enters the bridge
        createLocalId( ppThreadId );

        // TODO
        // note : this is a leak !
        p->pLocalThreadId = *ppThreadId;
        p->pCurrentId = *ppThreadId;
        p->nRefCountOfCurrentId = 1;
        rtl_byte_sequence_acquire( p->pLocalThreadId );
        rtl_byte_sequence_acquire( p->pCurrentId );
        p->bInit = sal_True;
    }
    else
    {
        p->nRefCountOfCurrentId ++;
        if( *ppThreadId )
        {
            rtl_byte_sequence_release( *ppThreadId );
        }
        *ppThreadId = p->pCurrentId;
        rtl_byte_sequence_acquire( *ppThreadId );
    }
}


extern "C"   void SAL_CALL uno_releaseIdFromCurrentThread()
    SAL_THROW_EXTERN_C()
{
    IdContainer *p = getIdContainer();
    OSL_ASSERT( p );
    OSL_ASSERT( p->nRefCountOfCurrentId );

    p->nRefCountOfCurrentId --;
    if( ! p->nRefCountOfCurrentId && (p->pLocalThreadId != p->pCurrentId) )
    {
        rtl_byte_sequence_assign( &(p->pCurrentId) , p->pLocalThreadId );
    }
}

extern "C"  sal_Bool SAL_CALL uno_bindIdToCurrentThread( sal_Sequence *pThreadId )
    SAL_THROW_EXTERN_C()
{
    IdContainer *p = getIdContainer();
    if( ! p->bInit )
    {
        p->pLocalThreadId = 0;
        createLocalId( &(p->pLocalThreadId) );
        p->nRefCountOfCurrentId = 1;
        p->pCurrentId = pThreadId;
        rtl_byte_sequence_acquire( p->pCurrentId );
        p->bInit = sal_True;
    }
    else
    {
        OSL_ASSERT( 0 == p->nRefCountOfCurrentId );
        if( 0 == p->nRefCountOfCurrentId )
        {
            rtl_byte_sequence_assign(&( p->pCurrentId ), pThreadId );
            p->nRefCountOfCurrentId ++;
        }
        else
        {
            return sal_False;
        }

    }
    return sal_True;
}
