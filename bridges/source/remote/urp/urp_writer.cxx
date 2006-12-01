/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: urp_writer.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: rt $ $Date: 2006-12-01 14:49:32 $
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
#include "precompiled_bridges.hxx"
#include <stdio.h>
#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif

#include <osl/mutex.hxx>
#include <osl/conditn.h>

#include <typelib/typedescription.h>

#include <bridges/remote/connection.h>
#include <bridges/remote/remote.hxx>

#include <com/sun/star/uno/Sequence.hxx>

#include <bridges/remote/counter.hxx>

#include "urp_writer.hxx"
#include "urp_bridgeimpl.hxx"
#include "urp_marshal.hxx"
#include "urp_dispatch.hxx"

#if OSL_DEBUG_LEVEL > 1
static MyCounter thisCounter( "DEBUG : WriterThread" );
#endif

using namespace ::osl;

namespace bridges_urp {

OWriterThread::OWriterThread( remote_Connection *pConnection, urp_BridgeImpl *pBridgeImpl,
                              uno_Environment *pEnvRemote) :
    m_bAbort( sal_False ),
    m_bInBlockingWait( sal_False ),
    m_bEnterBlockingWait( sal_False ),
    m_pConnection( pConnection ),
    m_pBridgeImpl( pBridgeImpl ),
    m_pEnvRemote( pEnvRemote )

{
    m_oslCondition = osl_createCondition();
    osl_resetCondition( m_oslCondition );
    m_pConnection->acquire( m_pConnection );

#if OSL_DEBUG_LEVEL > 1
    thisCounter.acquire();
#endif
}

OWriterThread::~OWriterThread()
{
    osl_destroyCondition( m_oslCondition );
    m_pConnection->release( m_pConnection );
#if OSL_DEBUG_LEVEL > 1
    thisCounter.release();
#endif
}


// touch is called with locked m_marshalingMutex
void OWriterThread::touch( sal_Bool bImmediately )
{
      if( bImmediately || m_pBridgeImpl->m_blockMarshaler.getPos() > m_pBridgeImpl->m_properties.nFlushBlockSize )
      {
        write();
      }
    else
    {
        // wake the writer thread up
        if( m_bInBlockingWait )
        {
            m_bInBlockingWait = sal_False;
            osl_setCondition( m_oslCondition );
        }
        else
        {
            // ensure, that the writing thread does not enter blocking mode
              m_bEnterBlockingWait = sal_False;
        }
    }
}


void OWriterThread::abort()
{
    MutexGuard guard( m_pBridgeImpl->m_marshalingMutex );

    m_bAbort = sal_True;
    m_bEnterBlockingWait = sal_False;
    if( m_bInBlockingWait )
    {
        m_bInBlockingWait = sal_False;
        osl_setCondition( m_oslCondition );
    }
}


// must be called with locked marshaling mutex
void OWriterThread::write()
{
     if( ! m_pBridgeImpl->m_blockMarshaler.empty() && ! m_bAbort )
     {
        m_pBridgeImpl->m_blockMarshaler.finish( m_pBridgeImpl->m_nMarshaledMessages);
        m_pBridgeImpl->m_nMarshaledMessages = 0;

        sal_Int32 nLength = m_pBridgeImpl->m_blockMarshaler.getSize();
        sal_Int8 *pBuf = m_pBridgeImpl->m_blockMarshaler.getBuffer();

        if( nLength != m_pConnection->write( m_pConnection, pBuf, nLength ))
        {
            m_pBridgeImpl->m_blockMarshaler.restart();
            return;
        }
        m_pConnection->flush( m_pConnection );
        m_pBridgeImpl->m_blockMarshaler.restart();
    }
}

void OWriterThread::sendEmptyMessage()
{
    // must be called with locked marshaling mutex
    sal_Int32 a[2] = {0,0};
    if( m_pConnection )
    {
        m_pConnection->write( m_pConnection , (sal_Int8*) a , sizeof( sal_Int32) *2 );
    }
}

void OWriterThread::insertReleaseRemoteCall(
    rtl_uString *pOid,typelib_TypeDescriptionReference *pTypeRef)
{
    {
        ::osl::MutexGuard guard( m_releaseCallMutex );

        struct RemoteReleaseCall call;
        call.sOid = pOid;
        call.typeInterface = pTypeRef;
        m_lstReleaseCalls.push_back( call );
    }
    {
        MutexGuard guard( m_pBridgeImpl->m_marshalingMutex );
        if( m_bInBlockingWait )
        {
            m_bInBlockingWait = sal_False;
            osl_setCondition( m_oslCondition );
        }
        else
        {
            // ensure, that the writing thread does not enter blocking mode
              m_bEnterBlockingWait = sal_False;
        }
    }
}

/* The release calls for doubled interfaces
 *
 *
 ***/
void OWriterThread::executeReleaseRemoteCalls()
{
    ::std::list< struct RemoteReleaseCall > lstReleaseCalls;
    {
        ::osl::MutexGuard guard( m_releaseCallMutex );
        lstReleaseCalls.swap( m_lstReleaseCalls );
    }

    for( ::std::list< struct RemoteReleaseCall >::iterator ii = lstReleaseCalls.begin();
         ii != lstReleaseCalls.end();
         ++ ii )
    {
        struct RemoteReleaseCall &call = (*ii) ;

        typelib_TypeDescription *pInterfaceTypeDesc = 0;
        typelib_TypeDescription *pReleaseMethod = 0;

        call.typeInterface.getDescription( &pInterfaceTypeDesc );
        if( ! pInterfaceTypeDesc->bComplete )
        {
            typelib_typedescription_complete( &pInterfaceTypeDesc );
        }

        uno_Any any;
        uno_Any *pAny = &any;

        typelib_typedescriptionreference_getDescription(
            &pReleaseMethod ,
            ((typelib_InterfaceTypeDescription*)pInterfaceTypeDesc)->ppAllMembers[REMOTE_RELEASE_METHOD_INDEX] );

        urp_sendRequest_internal(
            m_pEnvRemote , pReleaseMethod, call.sOid.pData,
            (typelib_InterfaceTypeDescription*) pInterfaceTypeDesc, 0, 0,
            &pAny );

        typelib_typedescription_release( pReleaseMethod );
        typelib_typedescription_release( pInterfaceTypeDesc );
    }
}


void OWriterThread::run()
{
    while( ! m_bAbort )
    {
        sal_Bool bWait;
        {
            MutexGuard guard( m_pBridgeImpl->m_marshalingMutex );
            bWait = m_bEnterBlockingWait;
            if( bWait )
            {
                osl_resetCondition( m_oslCondition );
                m_bInBlockingWait = sal_True;
            }
            m_bEnterBlockingWait = sal_True;
        }

        // wait for some notification
        if( bWait )
            osl_waitCondition( m_oslCondition , 0 );
        // (m_bInBlockingWait = sal_False was set by the activating thread)

          if( m_bAbort )
              break;

        // Wait for the timeout
        TimeValue value = { 0 , 1000 * m_pBridgeImpl->m_properties.nOnewayTimeoutMUSEC };
        osl_resetCondition( m_oslCondition );
        osl_waitCondition( m_oslCondition , &value );

        // check if there are some release calls to be sent ....
        executeReleaseRemoteCalls();

        {
            // write to the socket
            MutexGuard guard( m_pBridgeImpl->m_marshalingMutex );
            if( ! m_pBridgeImpl->m_blockMarshaler.empty() )
            {
                write();
            }
        }
    }
}


}

